/* $Id$ */

/*
	Scalable Building Simulator - Shaft Object
	The Skyscraper Project - Version 1.10 Alpha
	Copyright (C)2004-2015 Ryan Thoryk
	http://www.skyscrapersim.com
	http://sourceforge.net/projects/skyscraper
	Contact - ryan@tliquest.net

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "globals.h"
#include "sbs.h"
#include "shaft.h"
#include "camera.h"
#include "unix.h"

namespace SBS {

Shaft::Shaft(int number, float CenterX, float CenterZ, int startfloor, int endfloor)
{
	//creates a shaft in the location specified by CenterX and CenterZ
	//and that spans the floor range specified by startfloor and endfloor

	//set up SBS object
	SetValues(sbs, "Shaft", "", false);

	ShaftNumber = number;
	this->startfloor = startfloor;
	this->endfloor = endfloor;

	//make sure start and ending floors are within a valid range
	if (startfloor < -sbs->Basements)
		return;
	if (endfloor > sbs->Floors - 1)
		return;

	InsideShaft = false;
	IsEnabled = true;
	top = sbs->GetFloor(endfloor)->Altitude + sbs->GetFloor(endfloor)->FullHeight();
	bottom = sbs->GetFloor(startfloor)->Altitude;
	cutstart = 0;
	cutend = 0;
	ShowFloors = 0;
	ShowOutside = false;
	ShowInterfloors = false;
	ShowFullShaft = false;
	EnableCheck = false;
	lastcheckresult = false;
	checkfirstrun = true;
	lastposition = 0;
	InElevator = false;
	ShowFloorsFull_Enabled = false;

	std::string name = "Shaft " + ToString(number);

	SetName(name);
	SetPosition(CenterX, sbs->GetFloor(startfloor)->Altitude, CenterZ);

	ShaftArray.resize(endfloor - startfloor + 1);
	EnableArray.resize(endfloor - startfloor + 1);
	DoorArray.resize(endfloor - startfloor + 1);
	lights.resize(endfloor - startfloor + 1);
	ModelArray.resize(endfloor - startfloor + 1);
	ControlArray.resize(endfloor - startfloor + 1);
	//TriggerArray.resize(endfloor - startfloor + 1);

	for (int i = startfloor; i <= endfloor; i++)
	{
		//Create shaft meshes
		ShaftArray[i - startfloor] = new MeshObject(this, name + ":" + ToString(i));
		ShaftArray[i - startfloor]->SetPositionY(sbs->GetFloor(i)->Altitude);
		EnableArray[i - startfloor] = true;
	}
}

Shaft::~Shaft()
{
	//destructor

	//delete controls
	for (int i = 0; i < (int)ControlArray.size(); i++)
	{
		for (int j = 0; j < (int)ControlArray[i].size(); j++)
		{
			if (ControlArray[i][j])
			{
				ControlArray[i][j]->parent_deleting = true;
				delete ControlArray[i][j];
			}
			ControlArray[i][j] = 0;
		}
	}

	//delete triggers
	/*for (int i = 0; i < (int)TriggerArray.size(); i++)
	{
		for (int j = 0; j < (int)TriggerArray[i].size(); j++)
		{
			if (TriggerArray[i][j])
			{
				TriggerArray[i][j]->parent_deleting = true;
				delete TriggerArray[i][j];
			}
			TriggerArray[i][j] = 0;
		}
	}*/

	//delete models
	for (int i = 0; i < (int)ModelArray.size(); i++)
	{
		for (int j = 0; j < (int)ModelArray[i].size(); j++)
		{
			if (ModelArray[i][j])
			{
				ModelArray[i][j]->parent_deleting = true;
				delete ModelArray[i][j];
			}
			ModelArray[i][j] = 0;
		}
	}

	//delete lights
	for (int i = 0; i < (int)lights.size(); i++)
	{
		for (int j = 0; j < (int)lights[i].size(); j++)
		{
			if (lights[i][j])
			{
				lights[i][j]->parent_deleting = true;
				delete lights[i][j];
			}
			lights[i][j] = 0;
		}
	}

	//delete doors
	for (int i = 0; i < (int)DoorArray.size(); i++)
	{
		for (int j = 0; j < (int)DoorArray[i].size(); j++)
		{
			if (DoorArray[i][j])
			{
				DoorArray[i][j]->parent_deleting = true;
				delete DoorArray[i][j];
			}
			DoorArray[i][j] = 0;
		}
	}

	//delete mesh array objects
	for (int i = 0; i < (int)ShaftArray.size(); i++)
	{
		if (ShaftArray[i])
		{
			ShaftArray[i]->parent_deleting = true;
			delete ShaftArray[i];
		}
		ShaftArray[i] = 0;
	}

	//unregister from parent
	if (sbs->FastDelete == false && parent_deleting == false)
		sbs->RemoveShaft(this);
}

WallObject* Shaft::AddWall(int floor, const std::string &name, const std::string &texture, float thickness, float x1, float z1, float x2, float z2, float height1, float height2, float voffset1, float voffset2, float tw, float th)
{
	//exit with an error if floor is invalid
	if (IsValidFloor(floor) == false)
	{
		ReportError("AddWall: Floor " + ToString(floor) + " out of range");
		return 0;
	}

	WallObject *wall = GetMeshObject(floor)->CreateWallObject(name);
	AddWall(wall, floor, name, texture, thickness, x1, z1, x2, z2, height1, height2, voffset1, voffset2, tw, th);
	return wall;
}

bool Shaft::AddWall(WallObject *wall, int floor, const std::string &name, const std::string &texture, float thickness, float x1, float z1, float x2, float z2, float height1, float height2, float voffset1, float voffset2, float tw, float th)
{
	//exit with an error if floor is invalid
	if (IsValidFloor(floor) == false)
		return ReportError("AddWall: Floor " + ToString(floor) + " out of range");

	return sbs->AddWallMain(wall, name, texture, thickness, x1, z1, x2, z2, height1, height2, voffset1, voffset2, tw, th, true);
}

WallObject* Shaft::AddFloor(int floor, const std::string &name, const std::string &texture, float thickness, float x1, float z1, float x2, float z2, float voffset1, float voffset2, bool reverse_axis, bool texture_direction, float tw, float th, bool legacy_behavior)
{
	//exit with an error if floor is invalid
	if (IsValidFloor(floor) == false)
	{
		ReportError("AddFloor: Floor " + ToString(floor) + " out of range");
		return 0;
	}

	WallObject *wall = GetMeshObject(floor)->CreateWallObject(name);
	AddFloor(wall, floor, name, texture, thickness, x1, z1, x2, z2, voffset1, voffset2, reverse_axis, texture_direction, tw, th, legacy_behavior);
	return wall;
}

bool Shaft::AddFloor(WallObject *wall, int floor, const std::string &name, const std::string &texture, float thickness, float x1, float z1, float x2, float z2, float voffset1, float voffset2, bool reverse_axis, bool texture_direction, float tw, float th, bool legacy_behavior)
{
	//exit with an error if floor is invalid
	if (IsValidFloor(floor) == false)
		return ReportError("AddFloor: Floor " + ToString(floor) + " out of range");

	//get shaft extents
	float altitude = sbs->GetFloor(floor)->Altitude;

	//recalculate shaft extents if needed
	if (altitude + voffset1 < bottom)
		bottom = altitude + voffset1;
	if (altitude + voffset2 < bottom)
		bottom = altitude + voffset2;
	if (altitude + voffset1 > top)
		top = altitude + voffset1;
	if (altitude + voffset2 > top)
		top = altitude + voffset2;

	return sbs->AddFloorMain(wall, name, texture, thickness, x1, z1, x2, z2, voffset1, voffset2, reverse_axis, texture_direction, tw, th, true, legacy_behavior);
}

void Shaft::Enabled(int floor, bool value, bool EnableShaftDoors)
{
	SBS_PROFILE("Shaft::Enabled");
	if (IsEnabledFloor(floor) != value && floor >= startfloor && floor <= endfloor && EnableCheck == false)
	{
		//turns shaft on/off for a specific floor

		GetMeshObject(floor)->Enable(value);
		EnableArray[floor - startfloor] = value;

		//doors
		for (size_t i = 0; i < DoorArray[floor - startfloor].size(); i++)
		{
			if (DoorArray[floor - startfloor][i])
				DoorArray[floor - startfloor][i]->Enabled(value);
		}

		//controls
		for (size_t i = 0; i < ControlArray[floor - startfloor].size(); i++)
		{
			if (ControlArray[floor - startfloor][i])
				ControlArray[floor - startfloor][i]->Enabled(value);
		}

		//triggers
		/*for (size_t i = 0; i < TriggerArray[floor - startfloor].size(); i++)
		{
			if (TriggerArray[floor - startfloor][i])
				TriggerArray[floor - startfloor][i]->Enabled(value);
		}*/

		//models
		for (size_t i = 0; i < ModelArray[floor - startfloor].size(); i++)
		{
			if (ModelArray[floor - startfloor][i])
				ModelArray[floor - startfloor][i]->Enable(value);
		}

		if (EnableShaftDoors == true)
		{
			for (size_t i = 0; i < elevators.size(); i++)
			{
				Elevator *elevator = sbs->GetElevator(elevators[i]);
				if (elevator)
				{
					for(size_t j = 0; j < elevator->ServicedFloors.size(); j++)
					{
						if (elevator->ServicedFloors[j] == floor)
							elevator->ShaftDoorsEnabled(0, elevator->ServicedFloors[j], value);
					}
				}
			}
		}
	}
}

bool Shaft::IsShaft(Ogre::MeshPtr test)
{
	for (size_t i = 0; i < ShaftArray.size(); i++)
	{
		if (test == ShaftArray[i]->MeshWrapper)
			return true;
	}
	return false;
}

void Shaft::EnableWholeShaft(bool value, bool EnableShaftDoors, bool force)
{
	//turn on/off entire shaft

	if (force == true)
		IsEnabled = !value;

	if (IsEnabled == !value && EnableCheck == false)
	{
		for (int i = startfloor; i <= endfloor; i++)
		{
			if (force == true)
				EnableArray[i - startfloor] = !value;
			Enabled(i, value, EnableShaftDoors);
		}
	}
	IsEnabled = value;
	if (ShowFullShaft == true)
		EnableCheck = true;
}

bool Shaft::IsInShaft(const Ogre::Vector3 &position)
{
	//SBS_PROFILE("Shaft::IsInShaft");

	//if last position is the same as new, return previous result
	if ((position.x >= (lastposition.x - 0.01)) &&
		(position.y >= (lastposition.y - 0.01)) &&
		(position.z >= (lastposition.z - 0.01)) &&
		(position.x <= (lastposition.x + 0.01)) &&
		(position.y <= (lastposition.y + 0.01)) &&
		(position.z <= (lastposition.z + 0.01)) &&
		checkfirstrun == false)
		return lastcheckresult;

	checkfirstrun = false;

	if (position.y > bottom && position.y < top && ShaftArray.size() > 0)
	{
		//first determine if camera has X and Z values within the first shaft floor's bounding box
		if (ShaftArray[0]->InBoundingBox(position, false) == true)
		{
			//do a hit beam test from the position to the bottom of the shaft, to see if it hits a shaft floor
			bool result = (ShaftArray[0]->HitBeam(position, Ogre::Vector3::NEGATIVE_UNIT_Y, position.y - (bottom - 1)) >= 0);

			//cache values
			lastcheckresult = result;
			lastposition = position;

			return result;
		}
	}

	//cache values
	lastcheckresult = false;
	lastposition = position;

	return false;
}

void Shaft::CutFloors(bool relative, const Ogre::Vector2 &start, const Ogre::Vector2 &end, float startvoffset, float endvoffset)
{
	//Cut through floor/ceiling polygons on all associated levels, within the voffsets

	Report("cutting...");

	float voffset1, voffset2;
	cutstart = start;
	cutend = end;

	if (!sbs->GetFloor(startfloor) || !sbs->GetFloor(endfloor))
		return;

	for (int i = startfloor; i <= endfloor; i++)
	{
		Floor *floorptr = sbs->GetFloor(i);
		if (!floorptr)
			continue;

		voffset1 = 0;
		voffset2 = floorptr->FullHeight() + 1;

		if (i == startfloor)
			voffset1 = startvoffset;
		else if (i == endfloor)
			voffset2 = endvoffset;

		if (relative == true)
			floorptr->Cut(Ogre::Vector3(GetPosition().x + start.x, voffset1, GetPosition().z + start.y), Ogre::Vector3(GetPosition().x + end.x, voffset2, GetPosition().z + end.y), false, true, false);
		else
			floorptr->Cut(Ogre::Vector3(start.x, voffset1, start.y), Ogre::Vector3(end.x, voffset2, end.y), false, true, false);
		floorptr = 0;
	}

	//cut external
	voffset1 = sbs->GetFloor(startfloor)->Altitude + startvoffset;
	voffset2 = sbs->GetFloor(endfloor)->Altitude + endvoffset;

	for (int i = 0; i < (int)sbs->External->Walls.size(); i++)
	{
		if (relative == true)
			sbs->Cut(sbs->External->Walls[i], Ogre::Vector3(GetPosition().x + start.x, voffset1, GetPosition().z + start.y), Ogre::Vector3(GetPosition().x + end.x, voffset2, GetPosition().z + end.y), false, true);
		else
			sbs->Cut(sbs->External->Walls[i], Ogre::Vector3(start.x, voffset1, start.y), Ogre::Vector3(end.x, voffset2, end.y), false, true);
	}
}

bool Shaft::Cut(bool relative, int floor, const Ogre::Vector3 &start, const Ogre::Vector3 &end, bool cutwalls, bool cutfloors, int checkwallnumber)
{
	//Cut through a wall segment
	//the Y values in start and end are both relative to the floor's altitude

	//exit with an error if floor is invalid
	if (IsValidFloor(floor) == false)
	{
		if (sbs->Verbose)
			ReportError("Cut: Floor " + ToString(floor) + " out of range");
		else
			sbs->LastError = "Cut: Floor " + ToString(floor) + " out of range";
		return false;
	}

	if (!sbs->GetFloor(floor))
		return false;

	for (int i = 0; i < (int)GetMeshObject(floor)->Walls.size(); i++)
	{
		bool reset = true;
		if (i > 0)
			reset = false;

		if (relative == true)
			sbs->Cut(GetMeshObject(floor)->Walls[i], Ogre::Vector3(start.x, start.y, start.z), Ogre::Vector3(end.x, end.y, end.z), cutwalls, cutfloors, checkwallnumber, reset);
		else
			sbs->Cut(GetMeshObject(floor)->Walls[i], Ogre::Vector3(start.x - GetPosition().x, start.y, start.z - GetPosition().z), Ogre::Vector3(end.x - GetPosition().x, end.y, end.z - GetPosition().z), cutwalls, cutfloors, checkwallnumber, reset);
	}
	return true;
}

void Shaft::EnableRange(int floor, int range, bool value, bool EnableShaftDoors)
{
	//turn on/off a range of floors
	//if range is 3, show shaft on current floor (floor), and 1 floor below and above (3 total floors)
	//if range is 1, show only the current floor (floor)

	//exit if ShowFullShaft is true
	if (ShowFullShaft == true)
		return;

	if (!sbs->GetFloor(floor))
		return;

	SBS_PROFILE("Shaft::EnableRange");

	//range must be greater than 0
	if (range < 1)
		range = 1;

	//range must be an odd number; if it's even, then add 1
	if (IsEven(range) == true)
		range++;

	int additionalfloors;
	if (range > 1)
		additionalfloors = (range - 1) / 2;
	else
		additionalfloors = 0;

	//disable floors 1 floor outside of range
	if (value == true)
	{
		if (floor - additionalfloors - 1 >= startfloor && floor - additionalfloors - 1 <= endfloor)
		{
			if (sbs->GetFloor(floor)->IsInGroup(floor - additionalfloors - 1) == false) //only disable if not in group
				Enabled(floor - additionalfloors - 1, false, EnableShaftDoors);
		}
		if (floor + additionalfloors + 1 >= startfloor && floor + additionalfloors + 1 <= endfloor)
		{
			if (sbs->GetFloor(floor)->IsInGroup(floor + additionalfloors + 1) == false) //only disable if not in group
				Enabled(floor + additionalfloors + 1, false, EnableShaftDoors);
		}
	}

	//enable floors within range
	for (int i = floor - additionalfloors; i <= floor + additionalfloors; i++)
	{
		if (i >= startfloor && i <= endfloor)
			Enabled(i, value, EnableShaftDoors);
	}
}

bool Shaft::IsEnabledFloor(int floor)
{
	if (floor >= startfloor && floor <= endfloor)
		return EnableArray[floor - startfloor];
	else
		return false;
}

void Shaft::AddShowFloor(int floor)
{
	//adds a floor number to the ShowFloors list

	if (IsShowFloor(floor))
		return;

	ShowFloorsList.push_back(floor);
	std::sort(ShowFloorsList.begin(), ShowFloorsList.end());
}

void Shaft::RemoveShowFloor(int floor)
{
	//removes a floor number from the ShowFloors list

	for (int i = 0; i < (int)ShowFloorsList.size(); i++)
	{
		if (ShowFloorsList[i] == floor)
		{
			ShowFloorsList.erase(ShowFloorsList.begin() + i);
			return;
		}
	}
}

bool Shaft::IsShowFloor(int floor)
{
	//return true if a floor is in the ShowFloors list

	for (int i = 0; i < (int)ShowFloorsList.size(); i++)
	{
		if (ShowFloorsList[i] == floor)
			return true;
	}
	return false;
}

void Shaft::AddShowOutside(int floor)
{
	//adds a floor number to the ShowOutside list

	if (IsShowOutside(floor))
		return;

	ShowOutsideList.push_back(floor);
	std::sort(ShowOutsideList.begin(), ShowOutsideList.end());
}

void Shaft::RemoveShowOutside(int floor)
{
	//removes a floor number from the ShowOutside list

	for (int i = 0; i < (int)ShowOutsideList.size(); i++)
	{
		if (ShowOutsideList[i] == floor)
		{
			ShowOutsideList.erase(ShowOutsideList.begin() + i);
			return;
		}
	}
}

bool Shaft::IsShowOutside(int floor)
{
	//return true if a floor is in the ShowOutside list

	for (int i = 0; i < (int)ShowOutsideList.size(); i++)
	{
		if (ShowOutsideList[i] == floor)
			return true;
	}
	return false;
}

void Shaft::AddShowInterfloor(int floor)
{
	//adds a floor number to the ShowInterfloors list

	if (IsShowInterfloor(floor))
		return;

	ShowInterfloorsList.push_back(floor);
	std::sort(ShowInterfloorsList.begin(), ShowInterfloorsList.end());
}

void Shaft::RemoveShowInterfloor(int floor)
{
	//removes a floor number from the ShowInterfloors list

	for (int i = 0; i < (int)ShowInterfloorsList.size(); i++)
	{
		if (ShowInterfloorsList[i] == floor)
		{
			ShowInterfloorsList.erase(ShowInterfloorsList.begin() + i);
			return;
		}
	}
}

bool Shaft::IsShowInterfloor(int floor)
{
	//return true if a floor is in the ShowInterfloors list

	for (int i = 0; i < (int)ShowInterfloorsList.size(); i++)
	{
		if (ShowInterfloorsList[i] == floor)
			return true;
	}
	return false;
}

bool Shaft::IsValidFloor(int floor)
{
	//return true if the shaft services the specified floor

	if (floor < startfloor || floor > endfloor)
		return false;

	if (!ShaftArray[floor - startfloor])
		return false;

	return true;
}

void Shaft::AddElevator(int number)
{
	//add specified elevator to list

	for (int i = 0; i < (int)elevators.size(); i++)
	{
		if (elevators[i] == number)
			return;
	}

	elevators.push_back(number);
	std::sort(elevators.begin(), elevators.end());
}

void Shaft::RemoveElevator(int number)
{
	//remove specified elevator from list
	for (int i = 0; i < (int)elevators.size(); i++)
	{
		if (elevators[i] == number)
		{
			elevators.erase(elevators.begin() + i);
			return;
		}
	}
}

void Shaft::RemoveLight(Light *light)
{
	//remove a light reference (does not delete the object itself)
	for (int i = 0; i < (int)lights.size(); i++)
	{
		for (int j = 0; j < (int)lights[i].size(); j++)
		{
			if (lights[i][j] == light)
			{
				lights[i].erase(lights[i].begin() + j);
				return;
			}
		}
	}
}

void Shaft::RemoveModel(Model *model)
{
	//remove a model reference (does not delete the object itself)
	for (int i = 0; i < (int)ModelArray.size(); i++)
	{
		for (int j = 0; j < (int)ModelArray[i].size(); j++)
		{
			if (ModelArray[i][j] == model)
			{
				ModelArray[i].erase(ModelArray[i].begin() + j);
				return;
			}
		}
	}
}

void Shaft::RemoveControl(Control *control)
{
	//remove a control reference (does not delete the object itself)
	for (int i = 0; i < (int)ControlArray.size(); i++)
	{
		for (int j = 0; j < (int)ControlArray[i].size(); j++)
		{
			if (ControlArray[i][j] == control)
			{
				ControlArray[i].erase(ControlArray[i].begin() + j);
				return;
			}
		}
	}
}

void Shaft::RemoveTrigger(Trigger *trigger)
{
	//remove a trigger reference (does not delete the object itself)
	/*for (int i = 0; i < (int)TriggerArray.size(); i++)
	{
		for (int j = 0; j < (int)TriggerArray[i].size(); j++)
		{
			if (TriggerArray[i][j] == trigger)
			{
				TriggerArray[i].erase(TriggerArray[i].begin() + j);
				return;
			}
		}
	}*/
}

MeshObject* Shaft::GetMeshObject(int floor)
{
	//returns the mesh object for the specified floor

	if (!IsValidFloor(floor))
		return 0;

	return ShaftArray[floor - startfloor];
}

void Shaft::Report(const std::string &message)
{
	//general reporting function
	sbs->Report("Shaft " + ToString(ShaftNumber) + ": " + message);
}

bool Shaft::ReportError(const std::string &message)
{
	//general reporting function
	return sbs->ReportError("Shaft " + ToString(ShaftNumber) + ": " + message);
}

Light* Shaft::AddLight(int floor, const std::string &name, int type, Ogre::Vector3 position, Ogre::Vector3 direction, float color_r, float color_g, float color_b, float spec_color_r, float spec_color_g, float spec_color_b, float spot_inner_angle, float spot_outer_angle, float spot_falloff, float att_range, float att_constant, float att_linear, float att_quadratic)
{
	//add a global light

	//exit if floor is invalid
	if (!IsValidFloor(floor))
		return 0;

	Light* light = new Light(GetMeshObject(floor), name, type, position, direction, color_r, color_g, color_b, spec_color_r, spec_color_g, spec_color_b, spot_inner_angle, spot_outer_angle, spot_falloff, att_range, att_constant, att_linear, att_quadratic);
	lights[floor - startfloor].push_back(light);
	return light;
}

Model* Shaft::AddModel(int floor, const std::string &name, const std::string &filename, bool center, Ogre::Vector3 position, Ogre::Vector3 rotation, float max_render_distance, float scale_multiplier, bool enable_physics, float restitution, float friction, float mass)
{
	//add a model

	//exit if floor is invalid
	if (!IsValidFloor(floor))
		return 0;

	Model* model = new Model(GetMeshObject(floor), name, filename, center, position, rotation, max_render_distance, scale_multiplier, enable_physics, restitution, friction, mass);
	if (model->load_error == true)
	{
		delete model;
		return 0;
	}
	ModelArray[floor - startfloor].push_back(model);
	return model;
}

void Shaft::AddModel(int floor, Model *model)
{
	//add a model reference

	if (!model)
		return;

	//exit if floor is invalid
	if (!IsValidFloor(floor))
		return;

	for (int i = 0; i < (int)ModelArray[floor - startfloor].size(); i++)
	{
		if (ModelArray[floor - startfloor][i] == model)
			return;
	}

	ModelArray[floor - startfloor].push_back(model);
}

Control* Shaft::AddControl(int floor, const std::string &name, const std::string &sound, const std::string &direction, float CenterX, float CenterZ, float width, float height, float voffset, std::vector<std::string> &action_names, std::vector<std::string> &textures)
{
	//add a control

	//exit if floor is invalid
	if (!IsValidFloor(floor))
		return 0;

	std::vector<Action*> actionnull; //not used
	Control* control = new Control(GetMeshObject(floor), name, false, sound, action_names, actionnull, textures, direction, width, height, true);
	control->Move(CenterX, voffset, CenterZ);
	ControlArray[floor - startfloor].push_back(control);
	return control;
}

Trigger* Shaft::AddTrigger(int floor, const std::string &name, const std::string &sound_file, Ogre::Vector3 &area_min, Ogre::Vector3 &area_max, std::vector<std::string> &action_names)
{
	//triggers disabled for now

	//add a trigger

	//exit if floor is invalid
	/*if (!IsValidFloor(floor))
		return 0;

	Trigger* trigger = new Trigger(GetMeshObject(floor), name, false, sound_file, area_min, area_max, action_names);
	TriggerArray[floor - startfloor].push_back(trigger);
	return trigger;*/
	return 0;
}

void Shaft::ReplaceTexture(const std::string &oldtexture, const std::string &newtexture)
{
	for (int i = startfloor; i <= endfloor; i++)
        	GetMeshObject(i)->ReplaceTexture(oldtexture, newtexture);
}

void Shaft::OnInit()
{
	//startup initialization of shafts

	if (ShowFullShaft == false)
		EnableWholeShaft(false, true);
	else
		EnableWholeShaft(true, true, true);
}

Door* Shaft::AddDoor(int floor, const std::string &open_sound, const std::string &close_sound, bool open_state, const std::string &texture, float thickness, int direction, float speed, float CenterX, float CenterZ, float width, float height, float voffset, float tw, float th)
{
	//add a door

	//exit with an error if floor is invalid
	if (IsValidFloor(floor) == false)
	{
		ReportError("AddDoor: Floor " + ToString(floor) + " out of range");
		return 0;
	}

	Floor *floorptr = sbs->GetFloor(floor);
	if (!floorptr)
		return 0;

	float x1, z1, x2, z2;
	//set up coordinates
	if (direction < 5)
	{
		x1 = CenterX;
		x2 = CenterX;
		z1 = CenterZ - (width / 2);
		z2 = CenterZ + (width / 2);
	}
	else
	{
		x1 = CenterX - (width / 2);
		x2 = CenterX + (width / 2);
		z1 = CenterZ;
		z2 = CenterZ;
	}

	//cut area
	sbs->ResetDoorwayWalls();
	if (direction < 5)
	{
		Cut(1, floor, Ogre::Vector3(x1 - 0.5f, voffset, z1), Ogre::Vector3(x2 + 0.5f, voffset + height, z2), true, false, 1);
		floorptr->Cut(Ogre::Vector3(GetPosition().x + x1 - 0.5f, floorptr->GetBase(true) + voffset, GetPosition().z + z1), Ogre::Vector3(GetPosition().x + x2 + 0.5f, floorptr->GetBase(true) + voffset + height, GetPosition().z + z2), true, false, true, 2);
	}
	else
	{
		Cut(1, floor, Ogre::Vector3(x1, voffset, z1 - 0.5f), Ogre::Vector3(x2, voffset + height, z2 + 0.5f), true, false, 1);
		floorptr->Cut(Ogre::Vector3(GetPosition().x + x1, floorptr->GetBase(true) + voffset, GetPosition().z + z1 - 0.5f), Ogre::Vector3(GetPosition().x + x2, floorptr->GetBase(true) + voffset + height, GetPosition().z + z2 + 0.5f), true, false, true, 2);
	}

	//create doorway walls
	WallObject *wall = GetMeshObject(floor)->CreateWallObject("Connection Walls");
	sbs->AddDoorwayWalls(wall, "ConnectionWall", 0, 0);

	int index = floor - startfloor;
	std::string num = ToString((int)DoorArray[index].size());
	std::string name = "Shaft " + ToString(ShaftNumber) + ":Door " + ToString(floor) + ":" + num;

	Door* door = new Door(GetMeshObject(floor), name, open_sound, close_sound, open_state, texture, thickness, direction, speed, CenterX, CenterZ, width, height, floorptr->GetBase(true) + voffset, tw, th);
	DoorArray[index].push_back(door);

	floorptr = 0;
	return door;
}

void Shaft::RemoveDoor(Door *door)
{
	//remove a door reference (this does not delete the object)
	for (int i = 0; i < (int)DoorArray.size(); i++)
	{
		for (int j = 0; j < (int)DoorArray[i].size(); j++)
		{
			if (DoorArray[i][j] == door)
			{
				DoorArray[i].erase(DoorArray[i].begin() + j);
				return;
			}
		}
	}
}

void Shaft::Check(Ogre::Vector3 position, int current_floor)
{
	Elevator *elevator = sbs->GetElevator(sbs->ElevatorNumber);

	if (!elevator)
		return;

	if (IsInShaft(position) == true)
	{
		if (InsideShaft == false && sbs->InElevator == false)
		{
			//user is in the shaft
			InsideShaft = true;
			sbs->InShaft = true;
			InElevator = false;

			//turn on entire shaft
			EnableWholeShaft(true, true);
		}
		else if (InsideShaft == true && sbs->InElevator == true)
		{
			//user has moved from the shaft to an elevator
			InsideShaft = false;
			sbs->InShaft = false;
			InElevator = true;

			EnableWholeShaft(ShowFullShaft, true);
		}
		else if (InsideShaft == false && sbs->InElevator == true && ShowFullShaft == false)
		{
			//if user is in an elevator, show a range of the shaft at a time (while it's moving)
			EnableRange(current_floor, sbs->ShaftDisplayRange, true, false);
			elevator->ShaftDoorsEnabledRange(0, current_floor, sbs->ShaftDisplayRange);
		}

		//turn on related floors if ShowFloors is true
		//display a selected range of floors in the floor list if the user is in a moving elevator
		if (InsideShaft == false && sbs->InElevator == true && elevator->IsMoving == true && elevator->Leveling == false)
		{
			if (ShowFloors == 1)
				sbs->EnableFloorRange(current_floor, sbs->FloorDisplayRange, true, true, ShaftNumber);

			if (ShowOutside == true)
			{
				if (IsShowOutside(current_floor) == true)
				{
					sbs->EnableSkybox(true);
					sbs->EnableBuildings(true);
					sbs->EnableLandscape(true);
					sbs->EnableExternal(true);
				}
				else
				{
					sbs->EnableSkybox(false);
					sbs->EnableBuildings(false);
					sbs->EnableLandscape(false);
					sbs->EnableExternal(false);
				}
			}
		}

		//display the full range of floors in the floor list
		if (ShowFloors == 2 && ShowFloorsFull_Enabled == false)
		{
			ShowFloorsFull_Enabled = true;
			for (int i = 0; i < (int)ShowFloorsList.size(); i++)
			{
				Floor *floor = sbs->GetFloor(ShowFloorsList[i]);
				if (floor->IsEnabled == false)
				{
					floor->Enabled(true);
					//floor->EnableGroup(true);
				}
			}
		}

		//display interfloors
		if (ShowInterfloors == true)
		{
			for (int i = 0; i < (int)ShowInterfloorsList.size(); i++)
			{
				Floor *floor = sbs->GetFloor(ShowInterfloorsList[i]);
				if (floor->IsInterfloorEnabled == false)
					floor->EnableInterfloor(true);
			}
		}

	}
	else if (InsideShaft == true || InElevator == true)
	{
		//user has moved out of the shaft
		InsideShaft = false;
		sbs->InShaft = false;
		InElevator = false;

		//turn off shaft
		EnableWholeShaft(false, true, true);

		//disable floors listed in ShowFloors list, when "full" mode is enabled
		if (ShowFloors == 2 && ShowFloorsFull_Enabled == true)
		{
			ShowFloorsFull_Enabled = false;
			for (int i = 0; i < (int)ShowFloorsList.size(); i++)
			{
				Floor *floor = sbs->GetFloor(ShowFloorsList[i]);
				if (floor->IsEnabled == true && sbs->camera->CurrentFloor != floor->Number)
				{
					//don't disable floors that were originally enabled as part of the camera floor's group
					if ((floor->EnabledGroup == true && floor->EnabledGroup_Floor == sbs->camera->CurrentFloor) == false)
					{
						//only disable floor if not part of the camera floor's group
						if (floor->IsInGroup(sbs->camera->CurrentFloor) == false)
						{
							floor->Enabled(false);
							//floor->EnableGroup(false);
						}
					}
				}
			}
		}

		//disable interfloors
		if (ShowInterfloors == true)
		{
			for (int i = 0; i < (int)ShowInterfloorsList.size(); i++)
			{
				Floor *floor = sbs->GetFloor(ShowInterfloorsList[i]);
				if (floor->IsInterfloorEnabled == true && floor->IsEnabled == false)
					floor->EnableInterfloor(false);
			}
		}
	}
	else if (InsideShaft == false)
	{
		//show specified shaft range if outside the shaft
		EnableRange(current_floor, sbs->ShaftOutsideDisplayRange, true, true);
	}
}

void Shaft::Loop()
{
	//shaft runloop

	for (int i = 0; i < (int)ModelArray.size(); i++)
	{
		if (EnableArray[i] == true)
		{
			for (int j = 0; j < (int)ModelArray[i].size(); j++)
			{
				if (ModelArray[i][j])
					ModelArray[i][j]->Loop();
			}
		}
	}
}

}
