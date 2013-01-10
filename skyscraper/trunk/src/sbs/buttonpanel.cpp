/* $Id$ */

/*
	Scalable Building Simulator - Elevator Button Panel Class
	The Skyscraper Project - Version 1.8 Alpha
	Copyright (C)2004-2013 Ryan Thoryk
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
#include "buttonpanel.h"
#include "elevator.h"
#include "camera.h"
#include "action.h"
#include "unix.h"

extern SBS *sbs; //external pointer to the SBS engine

ButtonPanel::ButtonPanel(int _elevator, int index, const char *texture, int rows, int columns, const char *direction, float CenterX, float CenterZ, float buttonwidth, float buttonheight, float spacingX, float spacingY, float voffset, float tw, float th)
{
	//Create an elevator button panel
	//index is for specifying multiple panels within the same elevator

	//set up SBS object
	object = new Object();
	object->SetValues(this, sbs->GetElevator(_elevator)->object, "ButtonPanel", "", false);

	IsEnabled = true;
	elevator = _elevator;
	Index = index;
	Direction = direction;
	Origin.x = sbs->GetElevator(elevator)->Origin.x + CenterX;
	Origin.z = sbs->GetElevator(elevator)->Origin.z + CenterZ;
	ButtonWidth = buttonwidth;
	ButtonHeight = buttonheight;
	Rows = rows;
	Columns = columns;
	SpacingX = ButtonWidth * spacingX;
	SpacingY = ButtonHeight * spacingY;

	//total spacing plus total button widths
	Width = ((Columns + 1) * SpacingX) + (Columns * ButtonWidth);
	Height = ((Rows + 1) * SpacingY) + (Rows * ButtonHeight);

	//get vertical
	Origin.y = voffset - (Height / 2);

	//create mesh
	std::string buffer, buffer2, buffer3;
	buffer2 = Ogre::StringConverter::toString(elevator);
	buffer3 = Ogre::StringConverter::toString(index);
	buffer = "Button Panel " + buffer2 + ":" + buffer3;
	TrimString(buffer);
	object->SetName(buffer.c_str());
	ButtonPanelMesh = new MeshObject(object, buffer.c_str(), false, 0, sbs->GetConfigFloat("Skyscraper.SBS.MaxSmallRenderDistance", 100));

	//move
	SetToElevatorAltitude();

	//create panel back
	sbs->ResetTextureMapping(true);
	if (Direction == "front")
	{
		sbs->DrawWalls(true, false, false, false, false, false);
		AddWall("Panel", texture, 0, -(Width / 2), 0, Width / 2, 0, Height, Height, 0, 0, tw, th);
	}
	if (Direction == "back")
	{
		sbs->DrawWalls(false, true, false, false, false, false);
		AddWall("Panel", texture, 0, -(Width / 2), 0, Width / 2, 0, Height, Height, 0, 0, tw, th);
	}
	if (Direction == "left")
	{
		sbs->DrawWalls(true, false, false, false, false, false);
		AddWall("Panel", texture, 0, 0, -(Width / 2), 0, Width / 2, Height, Height, 0, 0, tw, th);
	}
	if (Direction == "right")
	{
		sbs->DrawWalls(false, true, false, false, false, false);
		AddWall("Panel", texture, 0, 0, -(Width / 2), 0, Width / 2, Height, Height, 0, 0, tw, th);
	}
	sbs->ResetWalls();
	sbs->ResetTextureMapping();
}

ButtonPanel::~ButtonPanel()
{
	//delete controls
	for (int i = 0; i < (int)controls.size(); i++)
	{
		if (controls[i])
		{
			controls[i]->object->parent_deleting = true;
			delete controls[i];
		}
		controls[i] = 0;
	}
	//delete panel
	if (ButtonPanelMesh)
		delete ButtonPanelMesh;
	ButtonPanelMesh = 0;

	//unregister with parent floor object
	if (sbs->FastDelete == false && object->parent_deleting == false)
		sbs->GetElevator(elevator)->RemovePanel(this);

	delete object;
}

void ButtonPanel::AddButton(const char *sound, const char *texture, const char *texture_lit, int row, int column, const char *type, float width, float height, float hoffset, float voffset)
{
	//create a standard button at specified row/column position
	//width and height are the button size percentage that the button should be (divided by 100); default is 1 for each, aka 100%.
	//hoffset and voffset are the optional horizontal and vertical offsets to place the button (in order to place it outside of the default grid position)

	//type is one of these:
	//floor number
	//open = Open Doors
	//close = Close Doors
	//cancel = Call Cancel
	//stop = Emergency Stop
	//alarm = Alarm

	std::vector<std::string> textures, names;
	std::string newtype = type;
	SetCase(newtype, false);

	//switch 'stop' to 'estop' for compatibility
	if (newtype == "stop")
		newtype = "estop";

	int positions = 1;
	textures.push_back(texture);
	textures.push_back(texture_lit);
	if (IsNumeric(newtype.c_str()) == true)
	{
		names.push_back("off");
		names.push_back(newtype);
	}
	else
		names.push_back(newtype);
	AddControl(sound, row, column, width, height, hoffset, voffset, names, textures);

}

void ButtonPanel::AddControl(const char *sound, int row, int column, float bwidth, float bheight, float hoffset, float voffset, std::vector<std::string> &action_names, std::vector<std::string> &textures)
{
	//create an elevator control (button, switch, knob)

	float xpos = 0, ypos = 0, zpos = 0;

	//set to default if value is 0
	if (bwidth == 0)
		bwidth = 1;

	if (bheight == 0)
		bheight = 1;

	//vertical position is the top of the panel, minus the total spacing above it,
	//minus the total button spaces above (and including) it, minus half of the extra height
	ypos = (Origin.y + Height) - (SpacingY * row) - (ButtonHeight * row) - ((bheight - 1) / 2);
	ypos += voffset * ButtonHeight;

	if (Direction == "front")
	{
		//horizontal position is the left-most edge of the panel (origin aka center minus
		//half the width), plus total spacing to the left of it, plus total button spaces
		//to the left of it, plus half of the extra width
		xpos = (Origin.x - (Width / 2)) + (SpacingX * column) + (ButtonWidth * (column - 1)) + ((bwidth - 1) / 2);
		zpos = Origin.z - 0.01;
		xpos += hoffset * ButtonWidth;
	}
	if (Direction == "back")
	{
		//back
		xpos = (Origin.x + (Width / 2)) - (SpacingX * column) - (ButtonWidth * (column - 1)) - ((bwidth - 1) / 2);
		zpos = Origin.z + 0.01;
		xpos -= hoffset * ButtonWidth;
	}
	if (Direction == "left")
	{
		xpos = Origin.x - 0.01;
		zpos = (Origin.z + (Width / 2))  - (SpacingX * column) - (ButtonWidth * (column - 1)) - ((bwidth - 1) / 2);
		zpos -= hoffset * ButtonWidth;
	}
	if (Direction == "right")
	{
		//right
		xpos = Origin.x + 0.01;
		zpos = (Origin.z - (Width / 2)) + (SpacingX * column) + (ButtonWidth * (column - 1)) + ((bwidth - 1) / 2);
		zpos += hoffset * ButtonWidth;
	}

	//create control object
	controls.resize(controls.size() + 1);
	int control_index = (int)controls.size() - 1;
	std::string buffer, buffer2, buffer3, buffer4;
	buffer2 = Ogre::StringConverter::toString(elevator);
	buffer3 = Ogre::StringConverter::toString(Index);
	buffer4 = Ogre::StringConverter::toString(control_index);
	buffer = "Button Panel " + buffer2 + ":" + buffer3 + " Control " + buffer4;
	TrimString(buffer);

	//register actions
	std::vector<std::string> actions;
	for (int i = 0; i < action_names.size(); i++)
	{
		std::string newname = sbs->GetElevator(elevator)->object->GetName();
		newname += ":" + action_names[i];
		std::vector<Object*> parents;
		parents.push_back(sbs->GetElevator(elevator)->object);
		sbs->AddAction(newname, parents, action_names[i]);
		actions.push_back(newname);
	}

	controls[control_index] = new Control(this->object, buffer.c_str(), sound, actions, textures, Direction.c_str(), ButtonWidth * bwidth, ButtonHeight * bheight, ypos);
	//move control
	controls[control_index]->SetPosition(Ogre::Vector3(xpos, sbs->GetElevator(elevator)->GetPosition().y, zpos));
}

void ButtonPanel::DeleteButton(int row, int column)
{

}

void ButtonPanel::Press(int index)
{
	//Press selected button

	if (index == -1)
		return;

	//exit if index is invalid
	if (index < 0 || index > (int)controls.size() - 1)
		return;

	//press button
	controls[index]->Press();
}

void ButtonPanel::Move(const Ogre::Vector3 &position)
{
	//relative movement
	ButtonPanelMesh->Move(position, true, true, true);

	//move controls
	for (int i = 0; i < (int)controls.size(); i++)
	{
		controls[i]->Move(position);
	}
}

void ButtonPanel::SetToElevatorAltitude()
{
	Ogre::Vector3 pos = ButtonPanelMesh->GetPosition();
	Ogre::Vector3 pos_new = Ogre::Vector3(pos.x, sbs->GetElevator(elevator)->GetPosition().y, pos.z);
	ButtonPanelMesh->Move(pos_new, false, false, false);

	//move controls
	for (int i = 0; i < (int)controls.size(); i++)
	{
		controls[i]->SetPositionY(pos_new.y);
	}
}

void ButtonPanel::Enabled(bool value)
{
	//enable or disable button panel

	if (IsEnabled == value)
		return;

	ButtonPanelMesh->Enable(value);

	for (int i = 0; i < (int)controls.size(); i++)
	{
		controls[i]->Enabled(value);
	}
	IsEnabled = value;
}

int ButtonPanel::AddWall(const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height1, float height2, float voffset1, float voffset2, float tw, float th)
{
	//Adds a wall with the specified dimensions

	return sbs->AddWallMain(object, ButtonPanelMesh, name, texture, thickness, Origin.x + x1, Origin.z + z1, Origin.x + x2, Origin.z + z2, height1, height2, Origin.y + voffset1, Origin.y + voffset2, tw, th, true);
}

Control* ButtonPanel::GetControl(int index)
{
	//return pointer to control object
	return controls[index];
}

void ButtonPanel::ChangeLight(int floor, bool value)
{
	//change light status for all buttons that call the specified floor

	std::string floornum = Ogre::StringConverter::toString(floor);
	for (int i = 0; i < (int)controls.size(); i++)
	{
		controls[i]->ChangeLight(floor, value);
	}
}

int ButtonPanel::GetFloorButtonIndex(int floor)
{
	//return the index number of a floor button

	std::string floornum = Ogre::StringConverter::toString(floor);

	if (controls.size() > 0)
	{
		for (int i = 0; i < (int)controls.size(); i++)
		{
			if (controls[i]->FindActionPosition(floornum.c_str()))
				return i;
		}
	}
	return -1;
}

void ButtonPanel::RemoveControl(Control *control)
{
	//remove a control from the array (does not delete the object)
	for (int i = 0; i < (int)controls.size(); i++)
	{
		if (controls[i] == control)
		{
			controls.erase(controls.begin() + i);
			return;
		}
	}
}
