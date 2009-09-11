/* $Id$ */

/*
	Scalable Building Simulator - Floor Indicator Subsystem Class
	The Skyscraper Project - Version 1.5 Alpha
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
#include "floorindicator.h"
#include "sbs.h"
#include "elevator.h"

extern SBS *sbs; //external pointer to the SBS engine

FloorIndicator::FloorIndicator(int elevator, const char *texture_prefix, const char *direction, float CenterX, float CenterZ, float width, float height, float altitude)
{
	//creates a new floor indicator at the specified position
	Elevator = elevator;
	Prefix = texture_prefix;

	csString buffer;
	buffer = elevator;
	buffer.Insert(0, "FloorIndicator ");
	buffer.Trim();
	FloorIndicatorMesh = CS::Geometry::GeneralMeshBuilder::CreateFactoryAndMesh(sbs->engine, sbs->area, buffer, buffer + " factory");
	FloorIndicatorMesh->SetZBufMode(CS_ZBUF_USE);
	FloorIndicatorMesh->SetRenderPriority(sbs->engine->GetObjectRenderPriority());
	FloorIndicator_movable = FloorIndicatorMesh->GetMovable();

	csString texture = "Button" + sbs->GetFloor(sbs->GetElevator(elevator)->OriginFloor)->ID;
	csString tmpdirection = direction;
	tmpdirection.Downcase();

	if (tmpdirection == "front")
		sbs->AddGenWall(FloorIndicatorMesh, texture, CenterX - (width / 2), CenterZ, CenterX + (width / 2), CenterZ, height, altitude, 1, 1);
	if (tmpdirection == "back")
		sbs->AddGenWall(FloorIndicatorMesh, texture, CenterX + (width / 2), CenterZ, CenterX - (width / 2), CenterZ, height, altitude, 1, 1);
	if (tmpdirection == "left")
		sbs->AddGenWall(FloorIndicatorMesh, texture, CenterX, CenterZ + (width / 2), CenterX, CenterZ - (width / 2), height, altitude, 1, 1);
	if (tmpdirection == "right")
		sbs->AddGenWall(FloorIndicatorMesh, texture, CenterX, CenterZ - (width / 2), CenterX, CenterZ + (width / 2), height, altitude, 1, 1);
}

FloorIndicator::~FloorIndicator()
{

}

void FloorIndicator::Enabled(bool value)
{
	//turns indicator on/off
	sbs->EnableMesh(FloorIndicatorMesh, value);
	IsEnabled = value;
}

void FloorIndicator::SetPosition(csVector3 position)
{
	//set position of indicator
	FloorIndicator_movable->SetPosition(position);
	FloorIndicator_movable->UpdateMove();
}

void FloorIndicator::MovePosition(csVector3 position)
{
	//move indicator by a relative amount
	FloorIndicator_movable->MovePosition(position);
	FloorIndicator_movable->UpdateMove();
}

void FloorIndicator::Update(const char *value)
{
	//update display with a new texture value, such as a floor number

	csString texture;
	texture = value;
	texture.Insert(0, Prefix);

	sbs->ChangeTexture(FloorIndicatorMesh, texture.GetData());
}

csVector3 FloorIndicator::GetPosition()
{
	//return current position of the indicator
	return FloorIndicator_movable->GetPosition();
}
