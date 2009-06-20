/* $Id$ */

/*
	Scalable Building Simulator - Call Button Subsystem Class
	The Skyscraper Project - Version 1.4 Alpha
	Copyright (C)2005-2009 Ryan Thoryk
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
#include "callbutton.h"
#include "sbs.h"
#include "camera.h"
#include "elevator.h"

extern SBS *sbs; //external pointer to the SBS engine

CallButton::CallButton(csArray<int> &elevators, int floornum, int number, const char *BackTexture, const char *UpButtonTexture, const char *DownButtonTexture, float CenterX, float CenterZ, float voffset, const char *direction, float BackWidth, float BackHeight, bool ShowBack, float tw, float th)
{
	//create a set of call buttons

	IsEnabled = true;
	Elevators.SetSize(elevators.GetSize());
	for (size_t i = 0; i < elevators.GetSize(); i++)
		Elevators[i] = elevators[i];

	//create object mesh
	csString buffer, buffer2, buffer3;
	buffer2 = floornum;
	buffer3 = number;
	buffer = "Call Panel " + buffer2 + ":" + buffer3;
	buffer.Trim();
	CallButtonBackMesh = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	buffer = "Call Button " + buffer2 + ":" + buffer3;
	buffer.Trim();
	CallButtonMesh = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	CallButton_back_state = scfQueryInterface<iThingFactoryState> (CallButtonBackMesh->GetMeshObject()->GetFactory());
	CallButton_state = scfQueryInterface<iThingFactoryState> (CallButtonMesh->GetMeshObject()->GetFactory());
	CallButtonBackMesh->SetZBufMode(CS_ZBUF_USE);
	CallButtonBackMesh->SetRenderPriority(sbs->engine->GetAlphaRenderPriority());
	CallButtonBackMesh->GetMeshObject()->SetMixMode(CS_FX_ALPHA);
	CallButtonMesh->SetZBufMode(CS_ZBUF_USE);
	CallButtonMesh->SetRenderPriority(sbs->engine->GetAlphaRenderPriority());
	CallButtonMesh->GetMeshObject()->SetMixMode(CS_FX_ALPHA);

	//set variables
	floor = floornum;
	Number = number;
	Direction = direction;
	Direction.Downcase();
	Direction.Trim();

	sbs->ReverseExtents(false, false, false);

	//create panel
	if (ShowBack == true)
	{
		if (Direction == "front" || Direction == "back")
		{
			if (Direction == "front")
				sbs->DrawWalls(true, false, false, false, false, false);
			else
				sbs->DrawWalls(false, true, false, false, false, false);
			sbs->AddWallMain(CallButton_back_state, "Panel", BackTexture, 0, CenterX - (BackWidth / 2), CenterZ, CenterX + (BackWidth / 2), CenterZ, BackHeight, BackHeight, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset, tw, th);
			sbs->ResetWalls();
		}
		if (Direction == "left" || Direction == "right")
		{
			if (Direction == "left")
				sbs->DrawWalls(true, false, false, false, false, false);
			else
				sbs->DrawWalls(false, true, false, false, false, false);
			sbs->AddWallMain(CallButton_back_state, "Panel", BackTexture, 0, CenterX, CenterZ + (BackWidth / 2), CenterX, CenterZ - (BackWidth / 2), BackHeight, BackHeight, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset, tw, th);
			sbs->ResetWalls();
		}
	}

	//create buttons
	int bottomfloor = sbs->GetElevator(Elevators[0])->GetBottomFloor();
	int topfloor = sbs->GetElevator(Elevators[0])->GetTopFloor();

	if (Direction == "front" || Direction == "back")
	{
		float offset;
		if (Direction == "front")
		{
			sbs->DrawWalls(true, false, false, false, false, false);
			offset = -0.01f;
		}
		else
		{
			sbs->DrawWalls(false, true, false, false, false, false);
			offset = 0.01f;
		}
		if (floornum > bottomfloor && floornum < topfloor)
		{
			sbs->AddWallMain(CallButton_state, "Up", UpButtonTexture, 0, CenterX - (BackWidth / 4), CenterZ + offset, CenterX + (BackWidth / 4), CenterZ + offset, (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + ((BackHeight / 7) * 4), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + ((BackHeight / 7) * 4), 1, 1);
			sbs->AddWallMain(CallButton_state, "Down", DownButtonTexture, 0, CenterX - (BackWidth / 4), CenterZ + offset, CenterX + (BackWidth / 4), CenterZ + offset, (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (BackHeight / 7), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (BackHeight / 7), 1, 1);
		}
		else
		{
			if (floornum < topfloor)
				sbs->AddWallMain(CallButton_state, "Up", UpButtonTexture, 0, CenterX - (BackWidth / 4), CenterZ + offset, CenterX + (BackWidth / 4), CenterZ + offset, (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), 1, 1);
			if (floornum > bottomfloor)
				sbs->AddWallMain(CallButton_state, "Down", DownButtonTexture, 0, CenterX - (BackWidth / 4), CenterZ + offset, CenterX + (BackWidth / 4), CenterZ + offset, (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), 1, 1);
		}

		sbs->ResetWalls();
	}
	else
	{
		float offset;
		if (Direction == "left")
		{
			sbs->DrawWalls(true, false, false, false, false, false);
			offset = -0.01f;
		}
		else
		{
			//right
			sbs->DrawWalls(false, true, false, false, false, false);
			offset = 0.01f;
		}
		if (floornum > bottomfloor && floornum < topfloor)
		{
			sbs->AddWallMain(CallButton_state, "Up", UpButtonTexture, 0, CenterX + offset, CenterZ - (BackWidth / 4), CenterX + offset, CenterZ + (BackWidth / 4), (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + ((BackHeight / 7) * 4), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + ((BackHeight / 7) * 4), 1, 1);
			sbs->AddWallMain(CallButton_state, "Down", DownButtonTexture, 0, CenterX + offset, CenterZ - (BackWidth / 4), CenterX + offset, CenterZ + (BackWidth / 4), (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (BackHeight / 7), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (BackHeight / 7), 1, 1);
		}
		else
		{
			if (floornum < topfloor)
				sbs->AddWallMain(CallButton_state, "Up", UpButtonTexture, 0, CenterX + offset, CenterZ - (BackWidth / 4), CenterX + offset, CenterZ + (BackWidth / 4), (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), 1, 1);
			if (floornum > bottomfloor)
				sbs->AddWallMain(CallButton_state, "Down", DownButtonTexture, 0, CenterX + offset, CenterZ - (BackWidth / 4), CenterX + offset, CenterZ + (BackWidth / 4), (BackHeight / 7) * 2, (BackHeight / 7) * 2, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight + voffset + (((BackHeight / 7) * 3) - (BackHeight / 14)), 1, 1);
		}

		sbs->ResetWalls();
	}
	sbs->ResetExtents();
}

CallButton::~CallButton()
{

}

void CallButton::Enabled(bool value)
{
	//turns panel on/off
	sbs->EnableMesh(CallButtonBackMesh, value);
	sbs->EnableMesh(CallButtonMesh, value);
	IsEnabled = value;
}

void CallButton::Call(int direction)
{
	//calls the closest elevator in the elevator array list to the current floor,
	//and also depending on the direction it's travelling

	int closest = abs(sbs->GetElevator(Elevators[0])->GetFloor() - floor);
	int closest_elev = 0;

	for (size_t i = 0; i < Elevators.GetSize(); i++)
	{
		int current = sbs->GetElevator(Elevators[i])->GetFloor();

		//if elevator is closer than the previously checked one
		if (abs(current - floor) < closest)
		{
			//and if it's above the current floor and should be called down, or below the
			//current floor and called up, or on the same floor
			if ((current > floor && direction == -1) || (current < floor && direction == 1) || current == floor)
			{
				//and if it's either going the same direction as the call or not moving at all
				if ((sbs->GetElevator(Elevators[i])->Direction == direction || sbs->GetElevator(Elevators[i])->Direction == 0) && sbs->GetElevator(Elevators[i])->IsMoving == false)
				{
					//and if it's not in any service mode
					if (sbs->GetElevator(Elevators[i])->InServiceMode() == false)
					{
						closest = current;
						closest_elev = i;
					}
				}
			}
		}
	}

	//if selected elevator is in a service mode, exit
	if (sbs->GetElevator(Elevators[closest_elev])->InServiceMode() == true)
		return;

	if (sbs->GetElevator(Elevators[closest_elev])->GetFloor() == floor)
	{
		//turn on directional indicator
		if (direction == -1)
			sbs->GetElevator(Elevators[closest_elev])->SetDirectionalIndicator(floor, false, true);
		else
			sbs->GetElevator(Elevators[closest_elev])->SetDirectionalIndicator(floor, true, false);
		//play chime sound
		sbs->GetElevator(Elevators[closest_elev])->Chime(0, floor);
		//open elevator if it's on the same floor
		sbs->GetElevator(Elevators[closest_elev])->OpenDoors();
	}
	else
		//otherwise add a route entry to this floor
		sbs->GetElevator(Elevators[closest_elev])->AddRoute(floor, direction);
}

void CallButton::Press(int index)
{
	//Press selected button

	if (index == -1)
		return;

	csString name = CallButton_state->GetPolygonName(index);
	if (name.Slice(0, 2) == "Up")
		Call(1);
	if (name.Slice(0, 4) == "Down")
		Call(-1);
}
