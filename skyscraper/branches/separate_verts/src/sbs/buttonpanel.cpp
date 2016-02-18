/* $Id$ */

/*
	Scalable Building Simulator - Elevator Button Panel Object
	The Skyscraper Project - Version 1.10 Alpha
	Copyright (C)2004-2016 Ryan Thoryk
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
#include "mesh.h"
#include "texture.h"
#include "buttonpanel.h"
#include "elevator.h"
#include "camera.h"
#include "action.h"
#include "control.h"

namespace SBS {

ButtonPanel::ButtonPanel(Elevator *elevator, int index, const std::string &texture, int rows, int columns, const std::string &direction, float CenterX, float CenterZ, float buttonwidth, float buttonheight, float spacingX, float spacingY, float voffset, float tw, float th) : Object(elevator)
{
	//Create an elevator button panel
	//index is for specifying multiple panels within the same elevator

	//set up SBS object
	SetValues("ButtonPanel", "", false);

	IsEnabled = true;
	this->elevator = elevator->Number;
	Index = index;
	Direction = direction;
	ButtonWidth = buttonwidth;
	ButtonHeight = buttonheight;
	Rows = rows;
	Columns = columns;
	SpacingX = ButtonWidth * spacingX;
	SpacingY = ButtonHeight * spacingY;
	off_action = 0;

	//total spacing plus total button widths
	Width = ((Columns + 1) * SpacingX) + (Columns * ButtonWidth);
	Height = ((Rows + 1) * SpacingY) + (Rows * ButtonHeight);

	//create mesh
	std::string name = "Button Panel " + ToString(this->elevator) + ":" + ToString(index);
	TrimString(name);
	SetName(name);
	ButtonPanelMesh = new MeshObject(this, name, 0, "", sbs->GetConfigFloat("Skyscraper.SBS.MaxSmallRenderDistance", 100));

	//create panel back
	sbs->GetTextureManager()->ResetTextureMapping(true);
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
	sbs->GetTextureManager()->ResetTextureMapping();

	//set position of object
	Move(CenterX, voffset - (Height / 2), CenterZ);
}

ButtonPanel::~ButtonPanel()
{
	//delete controls
	for (int i = 0; i < (int)controls.size(); i++)
	{
		if (controls[i])
		{
			controls[i]->parent_deleting = true;
			delete controls[i];
		}
		controls[i] = 0;
	}
	//delete panel
	if (ButtonPanelMesh)
	{
		ButtonPanelMesh->parent_deleting = true;
		delete ButtonPanelMesh;
	}
	ButtonPanelMesh = 0;

	if (sbs->FastDelete == false)
	{
		//unregister with parent floor object
		if (parent_deleting == false)
			sbs->GetElevator(elevator)->RemovePanel(this);

		//remove associated actions
		for (int i = 0; i < (int)action_list.size(); i++)
			sbs->RemoveAction(action_list[i]);
	}
}

Control* ButtonPanel::AddButton(const std::string &sound, const std::string &texture, const std::string &texture_lit, int row, int column, const std::string &type, float width, float height, float hoffset, float voffset)
{
	//create a standard button at specified row/column position
	//width and height are the button size percentage that the button should be (divided by 100); default is 1 for each, aka 100%.
	//hoffset and voffset are the optional horizontal and vertical offsets to place the button (in order to place it outside of the default grid position)

	//type is an elevator action, defined in action.cpp

	std::vector<std::string> textures, names;
	std::string newtype = type;
	SetCase(newtype, false);

	//switch 'stop' to 'estop' for compatibility
	if (newtype == "stop")
		newtype = "estop";

	textures.push_back(texture);
	textures.push_back(texture_lit);
	if (IsNumeric(newtype) == true)
	{
		names.push_back("off");
		names.push_back(newtype);
	}
	else
		names.push_back(newtype);

	return AddControl(sound, row, column, width, height, hoffset, voffset, names, textures);
}

Control* ButtonPanel::AddControl(const std::string &sound, int row, int column, float bwidth, float bheight, float hoffset, float voffset, std::vector<std::string> &action_names, std::vector<std::string> &textures)
{
	//create an elevator control (button, switch, knob)

	Ogre::Vector3 position = Ogre::Vector3::ZERO;

	//set to default if value is 0
	if (bwidth == 0)
		bwidth = 1;

	if (bheight == 0)
		bheight = 1;

	//vertical position is the top of the panel, minus the total spacing above it,
	//minus the total button spaces above (and including) it, minus half of the extra height multiplier
	position.y = Height - (SpacingY * row) - (ButtonHeight * row) - ((ButtonHeight * (bheight - 1)) / 2);
	position.y += voffset * ButtonHeight;

	if (Direction == "front")
	{
		//horizontal position is the left-most edge of the panel (origin aka center minus
		//half the width), plus total spacing to the left of it, plus total button spaces
		//to the left of it, plus half of the extra width multiplier
		position.x = (-Width / 2) + (SpacingX * column) + (ButtonWidth * (column - 1)) - ((ButtonWidth * (bwidth - 1)) / 2);
		position.z = -0.01f;
		position.x += hoffset * ButtonWidth;
	}
	if (Direction == "back")
	{
		//back
		position.x = (Width / 2) - (SpacingX * column) - (ButtonWidth * (column - 1)) + ((ButtonWidth * (bwidth - 1)) / 2);
		position.z = 0.01f;
		position.x -= hoffset * ButtonWidth;
	}
	if (Direction == "left")
	{
		position.x = -0.01f;
		position.z = (Width / 2) - (SpacingX * column) - (ButtonWidth * (column - 1)) + ((ButtonWidth * (bwidth - 1)) / 2);
		position.z -= hoffset * ButtonWidth;
	}
	if (Direction == "right")
	{
		//right
		position.x = 0.01f;
		position.z = (-Width / 2) + (SpacingX * column) + (ButtonWidth * (column - 1)) - ((ButtonWidth * (bwidth - 1)) / 2);
		position.z += hoffset * ButtonWidth;
	}

	//create control object
	controls.resize(controls.size() + 1);
	int control_index = (int)controls.size() - 1;
	std::string name = "Button Panel " + ToString(elevator) + ":" + ToString(Index) + " Control " + ToString(control_index);
	TrimString(name);

	//register actions
	std::vector<Action*> actions;
	std::vector<std::string> actionsnull; //not used

	for (int i = 0; i < (int)action_names.size(); i++)
	{
		std::string newname = sbs->GetElevator(elevator)->GetName();
		newname += ":" + action_names[i];
		std::vector<Object*> parents;
		parents.push_back(sbs->GetElevator(elevator));
		if ((off_action == 0 && action_names[i] == "off") || action_names[i] != "off")
		{
			Action* action = sbs->AddAction(newname, parents, action_names[i]);
			actions.push_back(action);
			action_list.push_back(action);
			if (action_names[i] == "off")
				off_action = action;
		}
		else
			actions.push_back(off_action);
	}

	Control *control = controls[control_index] = new Control(this, name, false, sound, actionsnull, actions, textures, Direction, ButtonWidth * bwidth, ButtonHeight * bheight, false);

	//move control
	controls[control_index]->Move(position);

	return control;
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

bool ButtonPanel::AddWall(const std::string &name, const std::string &texture, float thickness, float x1, float z1, float x2, float z2, float height1, float height2, float voffset1, float voffset2, float tw, float th)
{
	//Adds a wall with the specified dimensions

	return sbs->AddWallMain(this, ButtonPanelMesh, name, texture, thickness, x1, z1, x2, z2, height1, height2, voffset1, voffset2, tw, th, true);
}

void ButtonPanel::ChangeLight(int floor, bool value)
{
	//change light status for all buttons that call the specified floor

	for (int i = 0; i < (int)controls.size(); i++)
	{
		controls[i]->ChangeFloorLight(floor, value);
	}
}

void ButtonPanel::ChangeAllLights(bool value)
{
	//turns on or off all lights (for floor buttons)

	for (int i = 0; i < (int)controls.size(); i++)
	{
		controls[i]->ChangeLight(value);
	}
}

Control* ButtonPanel::GetFloorButton(int floor)
{
	//return the first control found for the specified floor

	std::string floornum = ToString(floor);

	if (controls.empty() == false)
	{
		for (int i = 0; i < (int)controls.size(); i++)
		{
			if (controls[i]->FindActionPosition(floornum) > 0)
				return controls[i];
		}
	}
	return 0;
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

}
