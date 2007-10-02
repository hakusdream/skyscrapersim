/* $Id$ */

/*
	Scalable Building Simulator - Stairs Subsystem Class
	The Skyscraper Project - Version 1.1 Alpha
	Copyright (C)2005-2007 Ryan Thoryk
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
#include "stairs.h"
#include "sbs.h"
#include "camera.h"

extern SBS *sbs; //external pointer to the SBS engine

Stairs::Stairs(int number, float CenterX, float CenterZ, int _startfloor, int _endfloor)
{
	StairsNum = number;
	startfloor = _startfloor;
	endfloor = _endfloor;
	origin.x = CenterX;
	origin.z = CenterZ;
	origin.y = sbs->GetFloor(startfloor)->Altitude + sbs->GetFloor(startfloor)->InterfloorHeight;

	csString buffer, buffer2, buffer3;

	StairArray.SetSize(endfloor - startfloor + 1);
	StairArray_state.SetSize(endfloor - startfloor + 1);
	StairArray_collider.SetSize(endfloor - startfloor + 1);

	for (int i = startfloor; i <= endfloor; i++)
	{
		//Create stairwell meshes
		buffer2 = number;
		buffer3 = i;
		buffer = "Stairwell " + buffer2 + ":" + buffer3;
		buffer.Trim();
		csRef<iMeshWrapper> tmpmesh;
		csRef<iThingFactoryState> tmpstate;
		tmpmesh = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
		StairArray[i - startfloor] = tmpmesh;
		tmpstate = scfQueryInterface<iThingFactoryState> (StairArray[i - startfloor]->GetMeshObject()->GetFactory());
		StairArray_state[i - startfloor] = tmpstate;
		StairArray[i - startfloor]->SetZBufMode(CS_ZBUF_USE);
		StairArray[i - startfloor]->GetMovable()->SetPosition(csVector3(origin.x, sbs->GetFloor(i)->Altitude + sbs->GetFloor(i)->InterfloorHeight, origin.z));
		StairArray[i - startfloor]->GetMovable()->UpdateMove();
	}
}

Stairs::~Stairs()
{

}

int Stairs::AddStairs(int floor, const char *name, const char *texture, const char *direction, float CenterX, float CenterZ, float width, float risersize, float treadsize, int num_stairs, float voffset, float tw, float th)
{
	//num_stairs is subtracted by 1 since it includes the floor platform above, but not below
	//direction is where the stairs base is - front, back, left, or right.

	csString buffer, buffer2, buffer3;
	csString Direction = direction;
	Direction.Downcase();
	int index = -1;
	int tmpindex = 0;
	buffer3 = name;
	buffer3.Trim();

	sbs->ReverseExtents(false, false, false);
	if (Direction == "right" || Direction == "back")
		sbs->SetWallOrientation("right");
	if (Direction == "left" || Direction == "front")
		sbs->SetWallOrientation("left");

	for (int i = 1; i <= num_stairs; i++)
	{
		float pos;
		buffer2 = i;
		float thickness;
		if (i < num_stairs - 1)
			thickness = treadsize * 2;
		if (i == num_stairs - 1)
			thickness = treadsize;
		if (i == num_stairs)
			thickness = 0;

		if (Direction == "right")
		{
			pos = CenterX + ((treadsize * (num_stairs - 1)) / 2) - (treadsize * i);
			buffer = buffer3 + " " + buffer2 + "-riser";
			if (i != num_stairs)
				sbs->DrawWalls(true, true, true, true, false, true);
			else
				sbs->DrawWalls(true, true, false, false, false, false);
			tmpindex = AddWall(floor, buffer.GetData(), texture, thickness, pos + treadsize, -(width / 2), pos + treadsize, width / 2, risersize, risersize, voffset + (risersize * (i - 1)), voffset + (risersize * (i - 1)), tw, th);
			buffer = buffer3 + " " + buffer2 + "-tread";
			if (i != num_stairs)
			{
				sbs->DrawWalls(true, false, false, false, false, false);
				AddFloor(floor, buffer.GetData(), texture, 0, pos, -(width / 2), pos + treadsize, width / 2, voffset + (risersize * i), voffset + (risersize * i), tw, th);
			}
		}
		if (Direction == "left")
		{
			pos = CenterX - ((treadsize * (num_stairs - 1)) / 2) + (treadsize * i);
			buffer = buffer3 + " " + buffer2 + "-riser";
			if (i != num_stairs)
				sbs->DrawWalls(true, true, true, true, false, true);
			else
				sbs->DrawWalls(true, true, false, false, false, false);
			tmpindex = AddWall(floor, buffer.GetData(), texture, thickness, pos - treadsize, width / 2, pos - treadsize, -(width / 2), risersize, risersize, voffset + (risersize * (i - 1)), voffset + (risersize * (i - 1)), tw, th);
			buffer = buffer3 + " " + buffer2 + "-tread";
			if (i != num_stairs)
			{
				sbs->DrawWalls(true, false, false, false, false, false);
				AddFloor(floor, buffer.GetData(), texture, 0, pos, width / 2, pos - treadsize, -(width / 2), voffset + (risersize * i), voffset + (risersize * i), tw, th);
			}
		}
		if (Direction == "back")
		{
			pos = CenterZ + ((treadsize * (num_stairs - 1)) / 2) - (treadsize * i);
			buffer = buffer3 + " " + buffer2 + "-riser";
			if (i != num_stairs)
				sbs->DrawWalls(true, true, true, true, false, true);
			else
				sbs->DrawWalls(true, true, false, false, false, false);
			tmpindex = AddWall(floor, buffer.GetData(), texture, thickness, width / 2, pos + treadsize, -(width / 2), pos + treadsize, risersize, risersize, voffset + (risersize * (i - 1)), voffset + (risersize * (i - 1)), tw, th);
			buffer = buffer3 + " " + buffer2 + "-tread";
			if (i != num_stairs)
			{
				sbs->DrawWalls(true, false, false, false, false, false);
				AddFloor(floor, buffer.GetData(), texture, 0, width / 2, pos, -(width / 2), pos + treadsize, voffset + (risersize * i), voffset + (risersize * i), tw, th);
			}
		}
		if (Direction == "front")
		{
			pos = CenterZ - ((treadsize * (num_stairs - 1)) / 2) + (treadsize * i);
			buffer = buffer3 + " " + buffer2 + "-riser";
			if (i != num_stairs)
				sbs->DrawWalls(true, true, true, true, false, true);
			else
				sbs->DrawWalls(true, true, false, false, false, false);
			tmpindex = AddWall(floor, buffer.GetData(), texture, thickness, -(width / 2), pos - treadsize, width / 2, pos - treadsize, risersize, risersize, voffset + (risersize * (i - 1)), voffset + (risersize * (i - 1)), tw, th);
			buffer = buffer3 + " " + buffer2 + "-tread";
			if (i != num_stairs)
			{
				sbs->DrawWalls(true, false, false, false, false, false);
				AddFloor(floor, buffer.GetData(), texture, 0, -(width / 2), pos, width / 2, pos - treadsize, voffset + (risersize * i), voffset + (risersize * i), tw, th);
			}
		}

		if (index == -1)
			index = tmpindex;
	}
	sbs->ResetWalls(true);
	sbs->ResetExtents();

	return index;
}

int Stairs::AddWall(int floor, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height1, float height2, float voffset1, float voffset2, float tw, float th)
{
	return sbs->AddWallMain(StairArray_state[floor - startfloor], name, texture, thickness, x1, z1, x2, z2, height1, height2, voffset1, voffset2, tw, th);
}

int Stairs::AddFloor(int floor, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float voffset1, float voffset2, float tw, float th)
{
	return sbs->AddFloorMain(StairArray_state[floor - startfloor], name, texture, thickness, x1, z1, x2, z2, voffset1, voffset2, tw, th);
}

void Stairs::Enabled(int floor, bool value)
{
	//turns stairwell on/off for a specific floor
	if (value == true)
	{
		StairArray[floor - startfloor]->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		StairArray[floor - startfloor]->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		StairArray[floor - startfloor]->GetFlags().Reset (CS_ENTITY_NOHITBEAM);
	}
	else
	{
		StairArray[floor - startfloor]->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		StairArray[floor - startfloor]->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		StairArray[floor - startfloor]->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
}

void Stairs::EnableWholeStairwell(bool value)
{
	//turn on/off entire stairwell
	for (int i = startfloor; i <= endfloor; i++)
		Enabled(i, value);
}

bool Stairs::IsInStairwell(const csVector3 &position)
{
	//determine if user is in the stairwell

	float bottom = sbs->GetFloor(startfloor)->Altitude + sbs->GetFloor(startfloor)->InterfloorHeight;
	float top = sbs->GetFloor(endfloor)->Altitude + sbs->GetFloor(endfloor)->FullHeight();

	if (position.y > bottom && position.y < top)
	{
		csHitBeamResult result = StairArray[startfloor]->HitBeam(position, csVector3(position.x, position.y - (top - bottom), position.z));
		return result.hit;
	}
	return false;
}

int Stairs::AddDoor(int floor, const char *texture, float thickness, int direction, float CenterX, float CenterZ, float width, float height, float voffset, float tw, float th)
{
	//interface to the SBS AddDoor function
	
	return sbs->CreateDoor(StairArray_state[floor - startfloor], texture, thickness, direction, CenterX, CenterZ, width, height, voffset + sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight, tw, th);
}

void Stairs::InitColliders()
{
	for (int i = 0; i < StairArray.GetSize(); i++)
		StairArray_collider[i] = sbs->CreateMeshCollider(StairArray[i]);
}
