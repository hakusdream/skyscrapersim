/* $Id$ */

/*
	Scalable Building Simulator - Camera Object Class
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
#include "callbutton.h"
#include "buttonpanel.h"
#include "camera.h"
#include "sbs.h"
#include "unix.h"

extern SBS *sbs; //external pointer to the SBS engine

Camera::Camera()
{
	MainCamera = sbs->view->GetCamera();
	MainCamera->SetSector(sbs->area);

	//init variables
	DefaultAltitude = 0;
	CurrentFloor = 0;
	OriginalPosition = 0;
	StartFloor = 0;
	StartPositionX = 0;
	StartPositionZ = 0;
	StartDirection = csVector3(0, 0, 0);
	StartRotation = csVector3(0, 0, 0);
	FallRate = 0;
	
	// Create the avatar.
	avatar = sbs->engine->CreateSectorWallsMesh(sbs->area, "body");
	avatar_state = scfQueryInterface<iThingFactoryState> (avatar->GetMeshObject()->GetFactory());
	avatar->SetZBufMode(CS_ZBUF_USE);
	sbs->CreateWallBox2(avatar_state, "body", "Brick", 0, 0, 2, 1, 6, 0, 0, 0);

	// Create a body and attach the mesh.
	avatarbody = sbs->dynSys->CreateBody ();
	avatarbody->SetProperties (1, csVector3 (0), csMatrix3 ());
	avatarbody->AttachMesh(avatar);

	// Create and attach a box collider.
	avatarbody->AttachColliderSphere (1.5, csVector3 (0), 10, 1, 0.8f);
}

Camera::~Camera()
{
	//Destructor
}

void Camera::SetPosition(csVector3 vector)
{
	//sets the camera to an absolute position in 3D space
	avatarbody->SetPosition(vector);
	UpdateCamera();
}

void Camera::SetDirection(csVector3 vector)
{
	//sets the camera's direction to an absolute position
	MainCamera->GetTransform().LookAt(vector, csVector3 (0, 1, 0));
}

void Camera::SetRotation(csVector3 vector)
{
	//sets the camera's rotation to an absolute position

	// We now assign a new rotation transformation to the camera.  You
	// can think of the rotation this way: starting from the zero
	// position, you first rotate "rotY" radians on your Y axis to get
	// the first rotation.  From there you rotate "rotX" radians on the
	// your X axis to get the final rotation.  We multiply the
	// individual rotations on each axis together to get a single
	// rotation matrix.  The rotations are applied in right to left
	// order .
	csMatrix3 rot = csXRotMatrix3 (vector.x) * csYRotMatrix3 (vector.y) * csZRotMatrix3 (vector.z);
	csOrthoTransform ot (rot, MainCamera->GetTransform().GetOrigin ());
	MainCamera->SetTransform (ot);
}

csVector3 Camera::GetPosition()
{
	//returns the camera's current position
	return csVector3(MainCamera->GetTransform().GetOrigin().x,
				MainCamera->GetTransform().GetOrigin().y,
				MainCamera->GetTransform().GetOrigin().z);
}

csVector3 Camera::GetDirection()
{
	//returns the camera's current direction
	return csVector3(0, 0, 0);
}

csVector3 Camera::GetRotation()
{
	//returns the camera's current rotation
	return csVector3(0, 0, 0);
}

void Camera::UpdateCameraFloor()
{
	CurrentFloor = sbs->GetFloorNumber(MainCamera->GetTransform().GetOrigin().y);
}

bool Camera::Move(csVector3 vector, float speed)
{
	//collision detection
	/*if (sbs->EnableCollisions == true)
	{
		csTraceBeamResult result;
		if (vector != CS_VEC_DOWN)
			result = csColliderHelper::TraceBeam(sbs->collision_sys, sbs->area, GetPosition(), GetPosition() + (vector * speed), false);
		else
			result = csColliderHelper::TraceBeam(sbs->collision_sys, sbs->area, GetPosition(), GetPosition() + (vector * speed) - csVector3(0, DefaultAltitude, 0), false);

		if (result.closest_mesh)
			return false;
	}*/

	//moves the camera in a relative amount specified by a vector
	//MainCamera->Move(vector * speed, sbs->EnableCollisions);
	avatarbody->SetLinearVelocity(MainCamera->GetTransform().GetT2O() * (vector * speed));
	UpdateCamera();
	return true;
}

void Camera::Rotate(csVector3 vector, float speed)
{
	//rotates the camera in a relative amount

	MainCamera->GetTransform().RotateThis(vector, speed);
}

void Camera::SetStartDirection(csVector3 vector)
{
	StartDirection = vector;
}

csVector3 Camera::GetStartDirection()
{
	return StartDirection;
}

void Camera::SetStartRotation(csVector3 vector)
{
	StartRotation = vector;
	SetRotation(vector);
}

csVector3 Camera::GetStartRotation()
{
	return StartRotation;
}

void Camera::SetToStartPosition()
{
	SetPosition(csVector3(StartPositionX, sbs->GetFloor(StartFloor)->Altitude + sbs->GetFloor(StartFloor)->InterfloorHeight + DefaultAltitude, StartPositionZ));
}

void Camera::SetToStartDirection()
{
	SetDirection(StartDirection);
}

void Camera::SetToStartRotation()
{
	SetRotation(StartRotation);
}

void Camera::CheckElevator()
{
	//check to see if user (camera) is in an elevator

	//first checks to see if camera is within an elevator's height range, and then
	//checks for a collision with the elevator's floor below

	bool test = false;
	for (int i = 1; i <= sbs->Elevators(); i++)
	{
		if (sbs->GetElevator(i)->IsInElevator(GetPosition()) == true)
		{
			test = true;
			sbs->InElevator = true;
			sbs->ElevatorNumber = i;
			sbs->ElevatorSync = true;
			return;
		}
	}
	//user is not in an elevator if all elevators returned false
	if (test == false)
	{
		sbs->InElevator = false;
		sbs->ElevatorSync = false;
	}
}

void Camera::CheckShaft()
{
	//check to see if user (camera) is in the shaft

	if (sbs->AutoShafts == false)
		return;

	for (int i = 1; i < sbs->Shafts(); i++)
	{
		if (sbs->GetShaft(i)->IsInShaft(GetPosition()) == true)
		{
			if (sbs->GetShaft(i)->InsideShaft == false && sbs->InElevator == false)
			{
				sbs->GetShaft(i)->InsideShaft = true;

				//turn on entire shaft
				sbs->GetShaft(i)->EnableWholeShaft(true);
			}
			else if (sbs->GetShaft(i)->InsideShaft == true && sbs->InElevator == true)
			{
				sbs->GetShaft(i)->InsideShaft = false;

				//turn off shaft except for camera floor
				sbs->GetShaft(i)->EnableWholeShaft(false);
				sbs->GetShaft(i)->Enabled(sbs->camera->CurrentFloor, true);
			}
		}
		else if (sbs->GetShaft(i)->InsideShaft == true)
		{
			sbs->GetShaft(i)->InsideShaft = false;

			//turn off shaft except for camera floor
			sbs->GetShaft(i)->EnableWholeShaft(false);
			sbs->GetShaft(i)->Enabled(sbs->camera->CurrentFloor, true);
		}
	}
}

void Camera::ClickedObject()
{
	//some code and comments from the CrystalSpace manual
	//this returns the mesh that the user clicks on

	// Setup a 2D vector with our mouse position.  We invert the y
	// (based on vertical screen dimension) because CS assumes y=0
	// is down for 3D calculations.
	csVector2 v2d (sbs->mouse_x, MainCamera->GetShiftY() * 2 - sbs->mouse_y);

	// We calculate the inverse perspective of this 2D point at
	// z=100.  This results in a 3D position in camera space at
	// z=100 that directly corresponds to the 2D position we
	// clicked on.  We use z=100 to ensure that we will at least
	// hit all objects that are before that distance.
	csVector3 v3d = MainCamera->InvPerspective(v2d, 100);

	// We are going to cast a beam in the current sector of the
	// camera from our camera position in the direction of the
	// 'v3d' point.  First we transform the v3d camera space
	// location to world space.
	csVector3 startbeam = MainCamera->GetTransform().GetOrigin();
	csVector3 endbeam = MainCamera->GetTransform().This2Other(v3d);

	// Now do the actual intersection.
	csSectorHitBeamResult result = sbs->area->HitBeam(startbeam, endbeam, true);
	if (!result.mesh)
		return;

	//get mesh name
	meshname = result.mesh->QueryObject()->GetName();

	//get polygon name
	csRef<iThingFactoryState> state = scfQueryInterface<iThingFactoryState> (result.mesh->GetMeshObject()->GetFactory());
	polyname = state->GetPolygonName(result.polygon_idx);

	//show result
	sbs->Report("Clicked on object - Mesh: " + meshname + ", Polygon: " + polyname);

	//check call buttons
	if (meshname.Find("Call Button") != -1)
	{
		//user clicked on a call button
		int floor = atoi(meshname.Slice(12, meshname.Find(":") - 12));
		int number = atoi(meshname.Slice(meshname.Find(":") + 1));
		//press button
		sbs->GetFloor(floor)->CallButtonArray[number]->Press(result.polygon_idx);
	}

	//check elevator buttons
	if (meshname.Find("Button Panel") != -1)
	{
		//user clicked on an elevator button
		int elevator = atoi(meshname.Slice(13));
		//press button
		sbs->GetElevator(elevator)->Panel->Press(result.polygon_idx);
	}

	//check doors
	if (meshname.Find("Door") != -1)
	{
		//user clicked on a door
		int doornumber = atoi(meshname.Slice(5));

		//open or close door
		if (sbs->GetDoor(doornumber)->IsOpen == false)
			sbs->GetDoor(doornumber)->OpenDoor();
		else
			sbs->GetDoor(doornumber)->CloseDoor();
	}
}

const char *Camera::GetClickedMeshName()
{
	//return name of last clicked mesh

	return meshname.GetData();
}

const char *Camera::GetClickedPolyName()
{
	//return name of last clicked polygon

	return polyname.GetData();
}

void Camera::UpdateCamera()
{
	//set the camera position to the avatar body position
	MainCamera->GetTransform().SetOrigin(avatar->GetMovable()->GetTransform().GetOrigin());
}

void Camera::Stop()
{
	//stops camera/avatar movement
	avatarbody->SetLinearVelocity(csVector3 (0, 0, 0));
	avatarbody->SetAngularVelocity (csVector3 (0, 0, 0));
}
