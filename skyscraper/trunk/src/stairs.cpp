/* $Id$ */

/*
	Scalable Building Simulator - Stairs Subsystem Class
	The Skyscraper Project - Version 1.1 Alpha
	Copyright (C)2005-2006 Ryan Thoryk
	http://www.tliquest.net/skyscraper
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

#include <crystalspace.h>
#include "stairs.h"
#include "sbs.h"
#include "camera.h"

extern SBS *sbs; //external pointer to the SBS engine

Stairs::Stairs(int number, int CenterX, int CenterZ, int _startfloor, int _endfloor)
{
	StairsNum = number;
	startfloor = _startfloor;
	endfloor = _endfloor;
	origin.x = CenterX;
	origin.z = CenterZ;
	origin.y = sbs->GetFloor(startfloor)->Altitude;

	csString buffer, buffer2, buffer3;

	StairArray.SetSize(endfloor - startfloor + 1);
	StairArray_state.SetSize(endfloor - startfloor + 1);

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
		StairArray[i - startfloor]->GetMovable()->SetPosition(origin);
		StairArray[i - startfloor]->GetMovable()->UpdateMove();
	}
}

Stairs::~Stairs()
{

}

int Stairs::AddStairs(int floor, const char *name, const char *texture, const char *direction, double CenterX, double CenterZ, double width, double risersize, double treadsize, int num_stairs, double voffset, double tw, double th)
{
	return 0;
}

int Stairs::AddWall(int floor, const char *name, const char *texture, double x1, double z1, double x2, double z2, double height1, double height2, double voffset1, double voffset2, double tw, double th, bool revX, bool revY, bool revZ)
{
	return sbs->AddWallMain(StairArray_state[floor - startfloor], name, texture, x1, z1, x2, z2, height1, height2, sbs->GetFloor(floor)->Altitude + voffset1, sbs->GetFloor(floor)->Altitude + voffset2, tw, th, revX, revY, revZ);
}

int Stairs::AddFloor(int floor, const char *name, const char *texture, double x1, double z1, double x2, double z2, double voffset1, double voffset2, double tw, double th)
{
	return sbs->AddFloorMain(StairArray_state[floor - startfloor], name, texture, x1, z1, x2, z2, sbs->GetFloor(floor)->Altitude + voffset1, sbs->GetFloor(floor)->Altitude + voffset2, tw, th);
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
	
	double bottom = sbs->GetFloor(startfloor)->Altitude;
	double top = sbs->GetFloor(endfloor)->Altitude + sbs->GetFloor(endfloor)->FullHeight();

	if (position.y > bottom && position.y < top)
	{
		csHitBeamResult result = StairArray[startfloor]->HitBeam(position, csVector3(position.x, position.y - (top - bottom), position.z));
		return result.hit;
	}
	return false;
}
