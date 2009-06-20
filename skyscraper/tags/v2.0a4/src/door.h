/* $Id$ */

/*
	Scalable Building Simulator - Door Class
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

#ifndef _SBS_DOOR_H
#define _SBS_DOOR_H

class SBSIMPEXP Door
{
public:

	csString Name; //door name
	int Direction;
	bool OpenState;
	csVector3 origin; //door origin
	bool IsMoving; //is door moving?
	bool OpenDoor; //open or close door?
	bool Clockwise; //if door should rotate clockwise

	Door(const char *name, const char *texture, float thickness, int direction, float CenterX, float CenterZ, float width, float height, float altitude, float tw, float th);
	~Door();
	void Open();
	void Close();
	bool IsOpen();
	void Enabled(bool value);
	void MoveDoor();

private:
	csRef<iMeshWrapper> DoorMesh; //door mesh
	csRef<iThingFactoryState> DoorMesh_state;
	csRef<iMovable> DoorMesh_movable;

	float rotation;

	char intbuffer[65];
	char buffer[20];
};

#endif
