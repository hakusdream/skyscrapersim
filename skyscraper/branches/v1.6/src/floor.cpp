/* $Id$ */

/*
	Scalable Building Simulator - Floor Class
	The Skyscraper Project - Version 1.6 Alpha
	Copyright (C)2004-2010 Ryan Thoryk
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
#include "floor.h"
#include "camera.h"
#include "unix.h"

extern SBS *sbs; //external pointer to the SBS engine

Floor::Floor(int number)
{
	//set up SBS object
	object = new Object();
	object->SetValues(this, sbs->object, "Floor", false);

	csString buffer;

	//Set floor's object number
	Number = number;

	//Create primary level mesh
	buffer = Number;
	buffer.Insert(0, "Level ");
	buffer.Trim();
	Level = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	Level_state = scfQueryInterface<iThingFactoryState> (Level->GetMeshObject()->GetFactory());
	Level->SetZBufMode(CS_ZBUF_USE);
	Level->SetRenderPriority(sbs->engine->GetObjectRenderPriority());

	//Create interfloor mesh
	buffer = Number;
	buffer.Insert(0, "Interfloor ");
	buffer.Trim();
	Interfloor = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	Interfloor_state = scfQueryInterface<iThingFactoryState> (Interfloor->GetMeshObject()->GetFactory());
	Interfloor->SetZBufMode(CS_ZBUF_USE);
	Interfloor->SetRenderPriority(sbs->engine->GetObjectRenderPriority());

	//Create columnframe mesh
	buffer = Number;
	buffer.Insert(0, "ColumnFrame ");
	buffer.Trim();
	ColumnFrame = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	ColumnFrame_state = scfQueryInterface<iThingFactoryState> (ColumnFrame->GetMeshObject()->GetFactory());
	ColumnFrame->SetZBufMode(CS_ZBUF_USE);
	ColumnFrame->SetRenderPriority(sbs->engine->GetObjectRenderPriority());

	//set enabled flag
	IsEnabled = true;

	//init other variables
	Name = "";
	ID = "";
	FloorType = "";
	Description = "";
	IndicatorTexture = "";
	Altitude = 0;
	Height = 0;
	InterfloorHeight = 0;
}

Floor::~Floor()
{
	//Destructor

	//delete call buttons
	for (int i = 0; i < CallButtonArray.GetSize(); i++)
	{
		if (CallButtonArray[i])
			delete CallButtonArray[i];
		CallButtonArray[i] = 0;
	}
	CallButtonArray.DeleteAll();

	//delete doors
	for (int i = 0; i < DoorArray.GetSize(); i++)
	{
		if (DoorArray[i])
			delete DoorArray[i];
		DoorArray[i] = 0;
	}
	DoorArray.DeleteAll();

	//delete floor indicators
	for (int i = 0; i < FloorIndicatorArray.GetSize(); i++)
	{
		if (FloorIndicatorArray[i])
			delete FloorIndicatorArray[i];
	}
	FloorIndicatorArray.DeleteAll();

	//delete directional indicators
	for (int i = 0; i < DirIndicatorArray.GetSize(); i++)
	{
		if (DirIndicatorArray[i])
			delete DirIndicatorArray[i];
	}
	DirIndicatorArray.DeleteAll();

	//delete sounds
	for (int i = 0; i < sounds.GetSize(); i++)
	{
		if (sounds[i])
			delete sounds[i];
		sounds[i] = 0;
	}
	sounds.DeleteAll();

	//delete wall objects
	for (int i = 0; i < level_walls.GetSize(); i++)
	{
		if (level_walls[i])
			delete level_walls[i];
		level_walls[i] = 0;
	}
	for (int i = 0; i < interfloor_walls.GetSize(); i++)
	{
		if (interfloor_walls[i])
			delete interfloor_walls[i];
		interfloor_walls[i] = 0;
	}
	for (int i = 0; i < columnframe_walls.GetSize(); i++)
	{
		if (columnframe_walls[i])
			delete columnframe_walls[i];
		columnframe_walls[i] = 0;
	}

	ColumnFrame_state = 0;
	ColumnFrame = 0;
	Interfloor_state = 0;
	Interfloor = 0;
	Level_state = 0;
	Level = 0;
	delete object;
}

void Floor::SetCameraFloor()
{
	//Moves camera to specified floor (sets altitude to the floor's base plus DefaultAltitude)

	csVector3 camlocation = sbs->camera->GetPosition();
	sbs->camera->SetPosition(csVector3(camlocation.x, GetBase() + sbs->camera->cfg_body_height + sbs->camera->cfg_legs_height, camlocation.z));
}

WallObject* Floor::AddFloor(const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float voffset1, float voffset2, float tw, float th, bool isexternal)
{
	//Adds a floor with the specified dimensions and vertical offset
	float tw2;
	float th2;

	//Set horizontal scaling
	x1 = x1 * sbs->HorizScale;
	x2 = x2 * sbs->HorizScale;
	z1 = z1 * sbs->HorizScale;
	z2 = z2 * sbs->HorizScale;

	//get texture force value
	bool force_enable, force_mode;
	sbs->GetTextureForce(texture, force_enable, force_mode);

	//Call texture autosizing formulas
	tw2 = sbs->AutoSize(x1, x2, true, tw, force_enable, force_mode);
	th2 = sbs->AutoSize(z1, z2, false, th, force_enable, force_mode);

	WallObject *wall;
	if (isexternal == false)
	{
		wall = sbs->CreateWallObject(level_walls, Level, this->object, name);
		sbs->AddFloorMain(wall, name, texture, thickness, x1, z1, x2, z2, GetBase() + voffset1, GetBase() + voffset2, tw2, th2);
	}
	else
	{
		wall = sbs->CreateWallObject(sbs->External_walls, sbs->External, this->object, name);
		sbs->AddFloorMain(wall, name, texture, thickness, x1, z1, x2, z2, Altitude + voffset1, Altitude + voffset2, tw2, th2);
	}
	return wall;
}

WallObject* Floor::AddInterfloorFloor(const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float voffset1, float voffset2, float tw, float th)
{
	//Adds an interfloor floor with the specified dimensions and vertical offset
	float tw2;
	float th2;

	//Set horizontal scaling
	x1 = x1 * sbs->HorizScale;
	x2 = x2 * sbs->HorizScale;
	z1 = z1 * sbs->HorizScale;
	z2 = z2 * sbs->HorizScale;

	//get texture force value
	bool force_enable, force_mode;
	sbs->GetTextureForce(texture, force_enable, force_mode);

	//Texture autosizing formulas
	tw2 = sbs->AutoSize(x1, x2, true, tw, force_enable, force_mode);
	th2 = sbs->AutoSize(z1, z2, false, th, force_enable, force_mode);

	WallObject *wall = sbs->CreateWallObject(interfloor_walls, Interfloor, this->object, name);
	sbs->AddFloorMain(wall, name, texture, thickness, x1, z1, x2, z2, Altitude + voffset1, Altitude + voffset2, tw2, th2);
	return wall;
}

WallObject* Floor::AddWall(const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float voffset1, float voffset2, float tw, float th, bool isexternal)
{
	//Adds a wall with the specified dimensions

	//Set horizontal scaling
	x1 = x1 * sbs->HorizScale;
	x2 = x2 * sbs->HorizScale;
	z1 = z1 * sbs->HorizScale;
	z2 = z2 * sbs->HorizScale;

	//calculate autosizing
	float tmpheight;
	if (height_in1 > height_in2)
		tmpheight = height_in1;
	else
		tmpheight = height_in2;
	csVector2 sizing = sbs->CalculateSizing(texture, csVector2(x1, x2), csVector2(0, tmpheight), csVector2(z1, z2), tw, th);

	WallObject *wall;
	if (isexternal == false)
	{
		wall = sbs->CreateWallObject(level_walls, Level, this->object, name);
		sbs->AddWallMain(wall, name, texture, thickness, x1, z1, x2, z2, height_in1, height_in2, GetBase() + voffset1, GetBase() + voffset2, sizing.x, sizing.y);
		return wall;
	}
	else
	{
		wall = sbs->CreateWallObject(sbs->External_walls, sbs->External, this->object, name);
		sbs->AddWallMain(wall, name, texture, thickness, x1, z1, x2, z2, height_in1, height_in2, Altitude + voffset1, Altitude + voffset2, sizing.x, sizing.y);
		return wall;
	}
}

WallObject* Floor::AddInterfloorWall(const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float voffset1, float voffset2, float tw, float th)
{
	//Adds an interfloor wall with the specified dimensions

	//Set horizontal scaling
	x1 = x1 * sbs->HorizScale;
	x2 = x2 * sbs->HorizScale;
	z1 = z1 * sbs->HorizScale;
	z2 = z2 * sbs->HorizScale;

	//calculate autosizing
	float tmpheight;
	if (height_in1 > height_in2)
		tmpheight = height_in1;
	else
		tmpheight = height_in2;
	csVector2 sizing = sbs->CalculateSizing(texture, csVector2(x1, x2), csVector2(0, tmpheight), csVector2(z1, z2), tw, th);

	WallObject *wall = sbs->CreateWallObject(interfloor_walls, Interfloor, this->object, name);
	sbs->AddWallMain(wall, name, texture, thickness, x1, z1, x2, z2, height_in1, height_in2, Altitude + voffset1, Altitude + voffset2, sizing.x, sizing.y);
	return wall;
}

void Floor::Enabled(bool value)
{
	//turns floor on/off

	if (IsEnabled == value)
		return;

	sbs->EnableMesh(Level, value);
	sbs->EnableMesh(Interfloor, value);
	IsEnabled = value;

	EnableColumnFrame(value);

	//call buttons
	for (size_t i = 0; i < CallButtonArray.GetSize(); i++)
	{
		if (CallButtonArray[i])
			CallButtonArray[i]->Enabled(value);
	}

	//doors
	for (size_t i = 0; i < DoorArray.GetSize(); i++)
	{
		if (DoorArray[i])
			DoorArray[i]->Enabled(value);
	}

	//turn on/off directional indicators
	for (int i = 0; i < DirIndicatorArray.GetSize(); i++)
	{
		if (DirIndicatorArray[i])
			DirIndicatorArray[i]->Enabled(value);
	}
	UpdateDirectionalIndicators();

	//floor indicators
	for (int i = 0; i < FloorIndicatorArray.GetSize(); i++)
	{
		if (FloorIndicatorArray[i])
			FloorIndicatorArray[i]->Enabled(value);
	}
	//update floor indicator values
	UpdateFloorIndicators();

	//sounds
	for (int i = 0; i < sounds.GetSize(); i++)
	{
		if (sounds[i])
		{
			if (value == false)
				sounds[i]->Stop();
			else
				sounds[i]->Play();
		}
	}
}

float Floor::FullHeight()
{
	//calculate full height of a floor
	return InterfloorHeight + Height;
}

Object* Floor::AddCallButtons(csArray<int> &elevators, const char *BackTexture, const char *UpButtonTexture, const char *UpButtonTexture_Lit, const char *DownButtonTexture, const char *DownButtonTexture_Lit, float CenterX, float CenterZ, float voffset, const char *direction, float BackWidth, float BackHeight, bool ShowBack, float tw, float th)
{
	//adds call buttons

	//check if any of the elevators serve the current floor
	bool check = false;
	for (int i = 0; i < elevators.GetSize(); i++)
	{
		if (sbs->GetElevator(elevators[i]))
		{
			if (sbs->GetElevator(elevators[i])->IsServicedFloor(Number) == true)
			{
				check = true;
				break;
			}
		}
	}

	//exit if none of the elevators serve the floor
	if (check == false)
		return 0;

	//create call button
	CallButtonArray.SetSize(CallButtonArray.GetSize() + 1);
	int Current = CallButtonArray.GetSize() - 1;
	CallButtonArray[Current] = new CallButton(elevators, Number, Current, BackTexture, UpButtonTexture, UpButtonTexture_Lit, DownButtonTexture, DownButtonTexture_Lit, CenterX, CenterZ, voffset, direction, BackWidth, BackHeight, ShowBack, tw, th);
	return CallButtonArray[Current]->object;
}

void Floor::Cut(const csVector3 &start, const csVector3 &end, bool cutwalls, bool cutfloors, bool fast, int checkwallnumber, const char *checkstring)
{
	//caller to SBS cut function
	//Y values are relative to the floor's altitude
	//if fast is specified, skips the interfloor scan

	sbs->Cut(Level, level_walls, csVector3(start.x, Altitude + start.y, start.z), csVector3(end.x, Altitude + end.y, end.z), cutwalls, cutfloors, csVector3(0, 0, 0), csVector3(0, 0, 0), checkwallnumber, checkstring);
	if (fast == false)
		sbs->Cut(Interfloor, interfloor_walls, csVector3(start.x, Altitude + start.y, start.z), csVector3(end.x, Altitude + end.y, end.z), cutwalls, cutfloors, csVector3(0, 0, 0), csVector3(0, 0, 0), checkwallnumber, checkstring);
}

void Floor::CutAll(const csVector3 &start, const csVector3 &end, bool cutwalls, bool cutfloors)
{
	//cuts all objects related to this floor (floor, interfloor, shafts, stairs and external)
	//Y values are relative to the floor's altitude

	//cut current level
	Cut(start, end, cutwalls, cutfloors, false);

	//cut shafts
	for (int i = 1; i <= sbs->Shafts(); i++)
	{
		if (cutwalls == true)
			sbs->GetShaft(i)->CutWall(false, Number, start, end);
		if (cutfloors == true)
			sbs->GetShaft(i)->CutFloors(false, csVector2(start.x, start.z), csVector2(end.x, end.z), Altitude, Altitude + FullHeight());
	}

	//cut stairs
	for (int i = 1; i <= sbs->StairsNum(); i++)
	{
		if (cutwalls == true)
			sbs->GetStairs(i)->CutWall(false, Number, start, end);
		if (cutfloors == true)
			sbs->GetStairs(i)->CutFloors(false, csVector2(start.x, start.z), csVector2(end.x, end.z), Altitude, Altitude + FullHeight());
	}

	//cut external
	sbs->Cut(sbs->External, sbs->External_walls, csVector3(start.x, Altitude + start.y, start.z), csVector3(end.x, Altitude + end.y, end.z), cutwalls, cutfloors, csVector3(0, 0, 0), csVector3(0, 0, 0));
}

void Floor::AddGroupFloor(int number)
{
	//adds a floor number to the group list.
	//Groups are used to enable multiple floors at the same time when
	//a user arrives at a floor

	if (Group.Find(number) == csArrayItemNotFound)
		Group.InsertSorted(number);
}

void Floor::RemoveGroupFloor(int number)
{
	//removes a floor number from the group list

	if (Group.Find(number) != csArrayItemNotFound)
		Group.Delete(number);
}

void Floor::EnableGroup(bool value)
{
	//enable floors grouped with this floor

	if (Group.GetSize() > 0)
	{
		for (size_t i = 0; i < Group.GetSize(); i++)
		{
			sbs->GetFloor(Group[i])->Enabled(value);
		}
	}
}

Object* Floor::AddDoor(const char *texture, float thickness, int direction, float CenterX, float CenterZ, float width, float height, float voffset, float tw, float th)
{
	//interface to the SBS AddDoor function

	if (direction > 8 | direction < 1)
	{
		ReportError("Door direction out of range");
		return 0;
	}

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
	if (direction < 5)
		CutAll(csVector3(x1 - 1, GetBase(true) + voffset, z1), csVector3(x2 + 1, GetBase(true) + voffset + height, z2), true, false);
	else
		CutAll(csVector3(x1, GetBase(true) + voffset, z1 - 1), csVector3(x2, GetBase(true) + voffset + height, z2 + 1), true, false);

	DoorArray.SetSize(DoorArray.GetSize() + 1);
	csString floornum = _itoa(Number, intbuffer, 10);
	csString num = _itoa(DoorArray.GetSize() - 1, intbuffer, 10);
	DoorArray[DoorArray.GetSize() - 1] = new Door(this->object, "Floor " + floornum + ":Door " + num, texture, thickness, direction, CenterX, CenterZ, width, height, voffset + GetBase(), tw, th);
	return DoorArray[DoorArray.GetSize() - 1]->object;
}

float Floor::CalculateAltitude()
{
	//calculate the floor's altitude in relation to floor below (or above it, if it's a basement level)
	//and return the altitude value

	if (Altitude == 0)
	{
		if (Number > 0)
			Altitude = sbs->GetFloor(Number - 1)->Altitude + sbs->GetFloor(Number - 1)->FullHeight();
		if (Number == -1)
			Altitude = -FullHeight();
		if (Number < -1)
			Altitude = sbs->GetFloor(Number + 1)->Altitude - FullHeight();
	}
	return Altitude;
}

void Floor::EnableColumnFrame(bool value)
{
	//enable/disable columnframe mesh
	sbs->EnableMesh(ColumnFrame, value);
	IsColumnFrameEnabled = value;
}

WallObject* Floor::ColumnWallBox(const char *name, const char *texture, float x1, float x2, float z1, float z2, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom)
{
	//create columnframe wall box
	float tw2 = tw;
	float th2;

	//Set horizontal scaling
	x1 = x1 * sbs->HorizScale;
	x2 = x2 * sbs->HorizScale;
	z1 = z1 * sbs->HorizScale;
	z2 = z2 * sbs->HorizScale;

	//get texture force value
	bool force_enable, force_mode;
	sbs->GetTextureForce(texture, force_enable, force_mode);

	//Texture autosizing formulas
	if (z1 == z2)
		tw2 = sbs->AutoSize(x1, x2, true, tw, force_enable, force_mode);
	if (x1 == x2)
		tw2 = sbs->AutoSize(z1, z2, true, tw, force_enable, force_mode);
	th2 = sbs->AutoSize(0, height_in, false, th, force_enable, force_mode);

	WallObject *wall = sbs->CreateWallObject(columnframe_walls, ColumnFrame, this->object, name);
	sbs->CreateWallBox(wall, name, texture, x1, x2, z1, z2, height_in, Altitude + voffset, tw, th, inside, outside, top, bottom);
	return wall;
}

WallObject* Floor::ColumnWallBox2(const char *name, const char *texture, float CenterX, float CenterZ, float WidthX, float LengthZ, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom)
{
	//create columnframe wall box from a central location
	float x1;
	float x2;
	float z1;
	float z2;

	x1 = CenterX - (WidthX / 2);
	x2 = CenterX + (WidthX / 2);
	z1 = CenterZ - (LengthZ / 2);
	z2 = CenterZ + (LengthZ / 2);

	return ColumnWallBox(name, texture, x1, x2, z1, z2, height_in, voffset, tw, th, inside, outside, top, bottom);
}

Object* Floor::AddFloorIndicator(int elevator, bool relative, const char *texture_prefix, const char *direction, float CenterX, float CenterZ, float width, float height, float voffset)
{
	//Creates a floor indicator at the specified location

	int size = FloorIndicatorArray.GetSize();
	FloorIndicatorArray.SetSize(size + 1);

	if (relative == false)
	{
		FloorIndicatorArray[size] = new FloorIndicator(elevator, texture_prefix, direction, CenterX, CenterZ, width, height, GetBase() + voffset);
		return FloorIndicatorArray[size]->object;
	}
	else
	{
		Elevator* elev = sbs->GetElevator(elevator);
		if (elev)
		{
			FloorIndicatorArray[size] = new FloorIndicator(elevator, texture_prefix, direction, elev->Origin.x + CenterX, elev->Origin.z + CenterZ, width, height, GetBase() + voffset);
			return FloorIndicatorArray[size]->object;
		}
		else
			return 0;
	}
}

void Floor::UpdateFloorIndicators(int elevator)
{
	//changes the number texture on the floor indicators to the specified elevator's current floor

	csString value;
	for (int i = 0; i < FloorIndicatorArray.GetSize(); i++)
	{
		if (FloorIndicatorArray[i])
		{
			if (FloorIndicatorArray[i]->Elevator == elevator)
			{
				Elevator *elev = sbs->GetElevator(elevator);
				if (elev->UseFloorSkipText == true && elev->IsServicedFloor(elev->GetFloor()) == false)
					value = elev->GetFloorSkipText();
				else
					value = sbs->GetFloor(elev->GetFloor())->ID;
				value.Trim();
				FloorIndicatorArray[i]->Update(value);
			}
		}
	}
}

void Floor::UpdateFloorIndicators()
{
	//changes the number texture on the floor indicators

	csString value;
	for (int i = 0; i < FloorIndicatorArray.GetSize(); i++)
	{
		if (FloorIndicatorArray[i])
		{
			Elevator *elevator = sbs->GetElevator(FloorIndicatorArray[i]->Elevator);
			if (elevator->UseFloorSkipText == true && elevator->IsServicedFloor(elevator->GetFloor()) == false)
				value = elevator->GetFloorSkipText();
			else
				value = sbs->GetFloor(elevator->GetFloor())->ID;
			value.Trim();
			FloorIndicatorArray[i]->Update(value);
		}
	}
}

void Floor::Loop()
{
	//floor object main loop; runs if camera is currently on this floor

}

csArray<int> Floor::GetCallButtons(int elevator)
{
	//get numbers of call buttons that service the specified elevator
	
	csArray<int> buttons;
	for (int i = 0; i < CallButtonArray.GetSize(); i++)
	{
		//put button number onto the array if it serves the elevator
		if (CallButtonArray[i]->ServicesElevator(elevator) == true)
			buttons.Push(i);
	}
	return buttons;
}

void Floor::AddFillerWalls(const char *texture, float thickness, float CenterX, float CenterZ, float width, float height, float voffset, bool direction, float tw, float th)
{
	//convenience function for adding filler walls around doors
	//direction is either "false" for a door that faces left/right, or "true" for one that faces front/back

	float x1 = 0, x2 = 0, z1 = 0, z2 = 0, depth1 = 0, depth2 = 0;

	if (sbs->GetWallOrientation() == 0)
	{
		depth1 = 0;
		depth2 = thickness;
	}
	if (sbs->GetWallOrientation() == 1)
	{
		depth1 = thickness / 2;
		depth2 = thickness / 2;
	}
	if (sbs->GetWallOrientation() == 2)
	{
		depth1 = thickness;
		depth2 = 0;
	}

	if (direction == false)
	{
		//door faces left/right
		x1 = CenterX - depth1;
		x2 = CenterX + depth2;
		z1 = CenterZ - (width / 2);
		z2 = CenterZ + (width / 2);
	}
	else
	{
		//door faces front/back
		x1 = CenterX - (width / 2);
		x2 = CenterX + (width / 2);
		z1 = CenterZ - depth1;
		z2 = CenterZ + depth2;
	}

	//perform a cut in the area
	CutAll(csVector3(x1, GetBase(true) + voffset, z1), csVector3(x2, GetBase(true) + voffset + height, z2), true, false);

	//create walls
	sbs->DrawWalls(false, true, false, false, false, false);
	if (direction == false)
		AddWall("FillerWallLeft", texture, 0, x1, z1, x2, z1, height, height, voffset, voffset, tw, th, false);
	else
		AddWall("FillerWallLeft", texture, 0, x1, z1, x1, z2, height, height, voffset, voffset, tw, th, false);
	sbs->ResetWalls();
	sbs->DrawWalls(true, false, false, false, false, false);
	if (direction == false)
	{
		AddWall("FillerWallRight", texture, 0, x1, z2, x2, z2, height, height, voffset, voffset, tw, th, false);
	}
	else
	{
		AddWall("FillerWallRight", texture, 0, x2, z1, x2, z2, height, height, voffset, voffset, tw, th, false);
	}
	AddFloor("FillerWallTop", texture, 0, x1, z1, x2, z2, height + voffset, height + voffset, tw, th, false);
	sbs->ResetWalls();
}

Object* Floor::AddSound(const char *name, const char *filename, csVector3 position, int volume, int speed, float min_distance, float max_distance, float dir_radiation, csVector3 direction)
{
	//create a looping sound object
	sounds.SetSize(sounds.GetSize() + 1);
	Sound *sound = sounds[sounds.GetSize() - 1];
	sound = new Sound(this->object, name);

	//set parameters and play sound
	sound->SetPosition(csVector3(position.x, GetBase() + position.y, position.z));
	sound->SetDirection(direction);
	sound->SetVolume(volume);
	sound->SetSpeed(speed);
	sound->SetMinimumDistance(min_distance);
	sound->SetMaximumDistance(max_distance);
	sound->SetDirection(direction);
	sound->SetDirectionalRadiation(dir_radiation);
	sound->Load(filename);
	sound->Loop(true);
	sound->Play();

	return sound->object;
}

void Floor::Report(const char *message)
{
	//general reporting function
	sbs->Report("Floor " + csString(_itoa(Number, intbuffer, 10)) + ": " + message);
}

void Floor::ReportError(const char *message)
{
	//general reporting function
	sbs->ReportError("Floor " + csString(_itoa(Number, intbuffer, 10)) + ": " + message);
}

float Floor::GetBase(bool relative)
{
	//returns the base of the floor
	//if Interfloor is on the bottom of the level (by default), the base is GetBase()
	//otherwise the base is just altitude
	if (relative == false)
	{
		if (sbs->InterfloorOnTop == false)
			return Altitude + InterfloorHeight;
		else
			return Altitude;
	}
	else
	{
		if (sbs->InterfloorOnTop == false)
			return InterfloorHeight;
		else
			return 0;
	}
}

Object* Floor::AddDirectionalIndicator(int elevator, bool relative, bool active_direction, bool single, bool vertical, const char *BackTexture, const char *uptexture, const char *uptexture_lit, const char *downtexture, const char *downtexture_lit, float CenterX, float CenterZ, float voffset, const char *direction, float BackWidth, float BackHeight, bool ShowBack, float tw, float th)
{
	//create a directional indicator on the specified floor, associated with a given elevator

	if (sbs->Verbose)
		Report("adding directional indicator");

	Elevator *elev = sbs->GetElevator(elevator);
	if (!elev)
		return 0;

	float x, z;
	if (relative == true)
	{
		x = elev->Origin.x + CenterX;
		z = elev->Origin.z + CenterZ;
	}
	else
	{
		x = CenterX;
		z = CenterZ;
	}

	if (active_direction == false)
	{
		//if active_direction is false, only create indicator if the elevator serves the floor
		if (elev->IsServicedFloor(Number) == false)
			return 0;
	}

	int index = DirIndicatorArray.GetSize();
	DirIndicatorArray.SetSize(index + 1);
	DirIndicatorArray[index] = new DirectionalIndicator(elevator, Number, active_direction, single, vertical, BackTexture, uptexture, uptexture_lit, downtexture, downtexture_lit, x, z, voffset, direction, BackWidth, BackHeight, ShowBack, tw, th);
	return DirIndicatorArray[index]->object;
}

void Floor::SetDirectionalIndicators(int elevator, bool UpLight, bool DownLight)
{
	//set light status of all standard (non active-direction) directional indicators associated with the given elevator

	for (int i = 0; i < DirIndicatorArray.GetSize(); i++)
	{
		if (DirIndicatorArray[i]->elevator_num == elevator && DirIndicatorArray[i]->ActiveDirection == false)
		{
			DirIndicatorArray[i]->DownLight(DownLight);
			DirIndicatorArray[i]->UpLight(UpLight);
		}
	}
}

void Floor::UpdateDirectionalIndicators(int elevator)
{
	//updates the active-direction indicators associated with the given elevator

	for (int i = 0; i < DirIndicatorArray.GetSize(); i++)
	{
		if (DirIndicatorArray[i])
		{
			if (DirIndicatorArray[i]->elevator_num == elevator && DirIndicatorArray[i]->ActiveDirection == true)
			{
				Elevator *elev = sbs->GetElevator(elevator);
				if (elev->ActiveDirection == 1)
				{
					DirIndicatorArray[i]->UpLight(true);
					DirIndicatorArray[i]->DownLight(false);
				}
				if (elev->ActiveDirection == 0)
				{
					DirIndicatorArray[i]->UpLight(false);
					DirIndicatorArray[i]->DownLight(false);
				}
				if (elev->ActiveDirection == -1)
				{
					DirIndicatorArray[i]->UpLight(false);
					DirIndicatorArray[i]->DownLight(true);
				}
			}
		}
	}
}

void Floor::UpdateDirectionalIndicators()
{
	//updates all active-direction indicators

	csString value;
	for (int i = 0; i < DirIndicatorArray.GetSize(); i++)
	{
		if (DirIndicatorArray[i])
		{
			if (DirIndicatorArray[i]->ActiveDirection == true)
			{
				Elevator *elev = sbs->GetElevator(DirIndicatorArray[i]->elevator_num);
				if (elev->ActiveDirection == 1)
				{
					DirIndicatorArray[i]->UpLight(true);
					DirIndicatorArray[i]->DownLight(false);
				}
				if (elev->ActiveDirection == 0)
				{
					DirIndicatorArray[i]->UpLight(false);
					DirIndicatorArray[i]->DownLight(false);
				}
				if (elev->ActiveDirection == -1)
				{
					DirIndicatorArray[i]->UpLight(false);
					DirIndicatorArray[i]->DownLight(true);
				}
			}
		}
	}
}
