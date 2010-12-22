/* $Id$ */

/*
	Scalable Building Simulator - Camera Object Class
	The Skyscraper Project - Version 1.8 Alpha
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

#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreBulletDynamicsRigidBody.h>
#include <OgreBulletCollisionsRay.h>
#include <Shapes/OgreBulletCollisionsCapsuleShape.h>
#include <Shapes/OgreBulletCollisionsCylinderShape.h>
#include "globals.h"
#include "sbs.h"
#include "callbutton.h"
#include "buttonpanel.h"
#include "camera.h"
#include "unix.h"

extern SBS *sbs; //external pointer to the SBS engine

Camera::Camera(Ogre::Camera *camera)
{
	//set up SBS object
	object = new Object();
	object->SetValues(this, sbs->object, "Camera", "Camera", true);

	//init variables
	CurrentFloor = 0;
	StartFloor = 0;
	StartPositionX = 0;
	StartPositionZ = 0;
	StartDirection = Ogre::Vector3(0, 0, 0);
	StartRotation = Ogre::Vector3(0, 0, 0);
	FloorTemp = 0;
	velocity = Ogre::Vector3(0, 0, 0);
	desired_velocity = Ogre::Vector3(0, 0, 0);
	angle_velocity = Ogre::Vector3(0, 0, 0);
	desired_angle_velocity = Ogre::Vector3(0, 0, 0);
	cfg_jumpspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.JumpSpeed", 14.0);
	cfg_walk_accelerate = sbs->GetConfigFloat("Skyscraper.SBS.Camera.WalkAccelerate", 0.040);
	cfg_walk_maxspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.WalkMaxSpeed", 0.1);
	cfg_walk_maxspeed_mult = sbs->GetConfigFloat("Skyscraper.SBS.Camera.WalkMaxSpeed_Mult", 10.0);
	cfg_walk_maxspeed_multreal = sbs->GetConfigFloat("Skyscraper.SBS.Camera.WalkMaxSpeed_MultReal", 1.0);
	cfg_walk_brake = sbs->GetConfigFloat("Skyscraper.SBS.Camera.WalkBrake", 0.040);
	cfg_rotate_accelerate = sbs->GetConfigFloat("Skyscraper.SBS.Camera.RotateAccelerate", 0.005);
	cfg_rotate_maxspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.RotateMaxSpeed", 0.015);
	cfg_rotate_brake = sbs->GetConfigFloat("Skyscraper.SBS.Camera.RotateBrake", 0.015);
	cfg_look_accelerate = sbs->GetConfigFloat("Skyscraper.SBS.Camera.LookAccelerate", 0.028);
	cfg_body_height = sbs->GetConfigFloat("Skyscraper.SBS.Camera.BodyHeight", 2.7);
	cfg_body_width = sbs->GetConfigFloat("Skyscraper.SBS.Camera.BodyWidth", 1.64);
	cfg_body_depth = sbs->GetConfigFloat("Skyscraper.SBS.Camera.BodyDepth", 1.64);
	cfg_legs_height = sbs->GetConfigFloat("Skyscraper.SBS.Camera.LegsHeight", 2.3);
	cfg_legs_width = sbs->GetConfigFloat("Skyscraper.SBS.Camera.LegsWidth", 1.312);
	cfg_legs_depth = sbs->GetConfigFloat("Skyscraper.SBS.Camera.LegsDepth", 1.312);
	cfg_lookspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.LookSpeed", 150.0);
	cfg_turnspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.TurnSpeed", 100.0);
	cfg_spinspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.SpinSpeed", 150.0);
	cfg_floatspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.FloatSpeed", 140.0);
	cfg_stepspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.StepSpeed", 140.0);
	cfg_strafespeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.StrafeSpeed", 140.0);
	cfg_speed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.Speed", 1.0);
	cfg_speedfast = sbs->GetConfigFloat("Skyscraper.SBS.Camera.FastSpeed", 2.0);
	cfg_speedslow = sbs->GetConfigFloat("Skyscraper.SBS.Camera.SlowSpeed", 0.5);
	cfg_zoomspeed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.ZoomSpeed", 0.2);
	speed = 1;
	Collisions = 0;
	GravityStatus = sbs->GetConfigBool("Skyscraper.SBS.Camera.GravityStatus", true);
	SetGravity(sbs->GetConfigFloat("Skyscraper.SBS.Camera.Gravity", 32.1719)); // 9.806 m/s/s
	lastfloor = 0;
	lastfloorset = false;
	MouseDown = false;
	ReportCollisions = sbs->GetConfigBool("Skyscraper.SBS.Camera.ReportCollisions", false);
	Freelook = sbs->GetConfigBool("Skyscraper.SBS.Camera.Freelook", false);
	Freelook_speed = sbs->GetConfigFloat("Skyscraper.SBS.Camera.FreelookSpeed", 200.0);
	FOV = sbs->GetConfigFloat("Skyscraper.SBS.Camera.FOV", 71.263794);
	ResetOnGround = sbs->GetConfigBool("Skyscraper.SBS.Camera.ResetOnGround", false);
	object_number = 0;
	object_line = 0;
	HitPosition = 0;
	previous_location = Ogre::Vector3::ZERO;
	RotationStopped = false;
	MovementStopped = false;

	//set up camera and scene nodes
	MainCamera = camera;
	MainCamera->setNearClipDistance(0.1f);
	MainCamera->setFarClipDistance(0.0f);
	MainCamera->setPosition(Ogre::Vector3(0, sbs->ToRemote((cfg_body_height + cfg_legs_height) / 2), 0));
	CameraNode = sbs->mSceneManager->getRootSceneNode()->createChildSceneNode("Camera");
	CameraNode->attachObject(MainCamera);

	//set up physics parameters
	Ogre::Vector3 bounds = Ogre::Vector3(sbs->ToRemote(cfg_body_width / 2), sbs->ToRemote((cfg_body_height + cfg_legs_height) / 2), sbs->ToRemote(cfg_body_depth / 2));
	//mShape = new OgreBulletCollisions::CapsuleCollisionShape(sbs->ToRemote(cfg_body_width / 2), sbs->ToRemote(cfg_body_height + cfg_legs_height), Ogre::Vector3::UNIT_Y);
	mShape = new OgreBulletCollisions::CylinderCollisionShape(bounds, Ogre::Vector3::UNIT_Y);
	mBody = new OgreBulletDynamics::RigidBody("CameraCollider", sbs->mWorld);
	mBody->setShape(CameraNode, mShape, 0.1, 0.5, 1);
	mBody->setSleepingThresholds(0, 0); //prevent object from deactivating
	mBody->setAngularFactor(0, 0, 0); //prevent other objects from affecting this object's rotation
	EnableCollisions(sbs->GetConfigBool("Skyscraper.SBS.Camera.EnableCollisions", true));
}

Camera::~Camera()
{
	//Destructor
	//MainCamera = 0;
	delete object;
}

void Camera::SetPosition(const Ogre::Vector3 &vector)
{
	//sets the camera to an absolute position in 3D space
	CameraNode->setPosition(sbs->ToRemote(vector));
	mBody->updateTransform(true);
}

void Camera::SetDirection(const Ogre::Vector3 &vector)
{
	//sets the camera's direction to an absolute position
	//CameraNode->lookAt(vector);
}

void Camera::SetRotation(Ogre::Vector3 vector)
{
	//sets the camera's rotation in degrees

	//keep rotation within 360 degree boundaries
	if (vector.x > 360)
		vector.x -= 360;
	if (vector.x < 0)
		vector.x += 360;
	if (vector.y > 360)
		vector.y -= 360;
	if (vector.y < 0)
		vector.y += 360;
	if (vector.z > 360)
		vector.z -= 360;
	if (vector.z < 0)
		vector.z += 360;

	Ogre::Quaternion x(Ogre::Degree(vector.x), Ogre::Vector3::UNIT_X);
	Ogre::Quaternion y(Ogre::Degree(vector.y), Ogre::Vector3::NEGATIVE_UNIT_Y);
	Ogre::Quaternion z(Ogre::Degree(vector.z), Ogre::Vector3::UNIT_Z);
	Ogre::Quaternion camrot = x * z;
	Ogre::Quaternion bodyrot = y;
	rotation = vector;
	MainCamera->setOrientation(camrot);
	mBody->setOrientation(bodyrot);
	mBody->updateTransform(true);
}

Ogre::Vector3 Camera::GetPosition()
{
	//returns the camera's current position
	if (CameraNode)
		return sbs->ToLocal(CameraNode->getPosition());
	else
		return Ogre::Vector3(0, 0, 0);
}

void Camera::GetDirection(Ogre::Vector3 &front, Ogre::Vector3 &top)
{
	//returns the camera's current direction in front and top vectors
	front = CameraNode->getOrientation().zAxis();
	front.x = -front.x; //convert to left-hand coordinate system
	front.y = -front.y; //convert to left-hand coordinate system
	front.normalise();
	top = CameraNode->getOrientation().yAxis();
	top.z = -top.z; //convert to left-hand coordinate system
	top.normalise();
}

Ogre::Vector3 Camera::GetRotation()
{
	//returns the camera's current rotation

	//return mBody->getWorldOrientation() * MainCamera->getOrientation();
	return rotation;
}

void Camera::UpdateCameraFloor()
{
	int newlastfloor;

	if (lastfloorset == true)
		newlastfloor = sbs->GetFloorNumber(GetPosition().y, lastfloor, true);
	else
		newlastfloor = sbs->GetFloorNumber(GetPosition().y);

	//if camera moved to a different floor, update floor indicators
	if ((lastfloor != newlastfloor) && sbs->GetFloor(newlastfloor))
		sbs->GetFloor(newlastfloor)->UpdateFloorIndicators();

	lastfloor = newlastfloor;
	lastfloorset = true;
	CurrentFloor = lastfloor;
	if (sbs->GetFloor(CurrentFloor))
		CurrentFloorID = sbs->GetFloor(CurrentFloor)->ID;
}

bool Camera::Move(Ogre::Vector3 vector, float speed)
{
	//moves the camera in a relative amount specified by a vector

	if (MovementStopped == true && vector == Ogre::Vector3::ZERO)
		return false;

	MovementStopped = false;

	if (vector == Ogre::Vector3::ZERO)
		MovementStopped = true;

	//multiply vector with object's orientation, and flip X axis
	vector = mBody->getWorldOrientation() * vector * Ogre::Vector3(-1, 1, 1);

	//vector.y += sbs->ToLocal(mBody->getLinearVelocity().y);

	//mBody->setLinearVelocity(sbs->ToRemote(vector));
	if (vector.y == 0)
	{
		if (GetGravityStatus() == true)
			mBody->setLinearVelocity(Ogre::Vector3(sbs->ToRemote(vector.x), mBody->getLinearVelocity().y, sbs->ToRemote(-vector.z)));
		else
			mBody->setLinearVelocity(Ogre::Vector3(sbs->ToRemote(vector.x), 0, sbs->ToRemote(-vector.z)));
	}
	else
	{
		mBody->setLinearVelocity(Ogre::Vector3(sbs->ToRemote(vector.x), 0, sbs->ToRemote(-vector.z)));
		mBody->applyForce(Ogre::Vector3(0, sbs->ToRemote(vector.y * 50), 0), Ogre::Vector3::ZERO);
	}

	//bounce when hitting an object
	/*Ogre::Vector3 diff = sbs->ToLocal(mBody->getWorldPosition()) - previous_location;
	if ((vector.z > 1 && diff.z < 0.01) || (vector.x > 1 && diff.x < 0.01) && vector.y == 0)
		velocity.y = 10;*/
	//printf("vel: %g - pos change: %g\n", vector.z, sbs->ToLocal(-mBody->getWorldPosition().z) - previous_location.z);
	previous_location = sbs->ToLocal(mBody->getWorldPosition());

	return true;
}

void Camera::Rotate(const Ogre::Vector3 &vector, float speed)
{
	//rotates the camera in a relative amount in world space

	//mBody->setAngularVelocity(sbs->ToRemote(Ogre::Vector3(0, -vector.y, 0)));
	//Ogre::Vector3 rot = GetRotation() + (Ogre::Vector3(vector.x, 0, vector.z) * speed);
	Ogre::Vector3 rot = GetRotation() + (vector.x * speed);

	SetRotation(rot);
}

void Camera::RotateLocal(const Ogre::Vector3 &vector, float speed)
{
	//rotates the camera in a relative amount in local camera space

	if (RotationStopped == true && vector == Ogre::Vector3::ZERO)
		return;

	RotationStopped = false;

	if (vector == Ogre::Vector3::ZERO)
		RotationStopped = true;

	//rotate collider body on Y axis (left/right)
	/*float ydeg = Ogre::Math::RadiansToDegrees(vector.y) * speed;
	rotation.y += ydeg;

	if (rotation.y > 360)
		rotation.y -= 360;
	if (rotation.y < 0)
		rotation.y += 360;

	//Ogre::Quaternion rot(Ogre::Radian(vector.y) * speed, Ogre::Vector3::NEGATIVE_UNIT_Y);
	Ogre::Quaternion rot(Ogre::Degree(rotation.y), Ogre::Vector3::NEGATIVE_UNIT_Y);
	mBody->setOrientation(rot);
	mBody->updateTransform(true);*/

	mBody->setAngularVelocity(0, -vector.y, 0);

	//rotate camera on X and Z axes
	MainCamera->pitch(Ogre::Radian(vector.x) * speed);
	MainCamera->roll(Ogre::Radian(vector.z) * speed);
}

void Camera::SetStartDirection(const Ogre::Vector3 &vector)
{
	StartDirection = vector;
}

Ogre::Vector3 Camera::GetStartDirection()
{
	return StartDirection;
}

void Camera::SetStartRotation(const Ogre::Vector3 &vector)
{
	StartRotation = vector;
}

Ogre::Vector3 Camera::GetStartRotation()
{
	return StartRotation;
}

void Camera::SetToStartPosition()
{
	if (sbs->GetFloor(StartFloor))
		SetPosition(Ogre::Vector3(StartPositionX, sbs->GetFloor(StartFloor)->GetBase() + GetHeight(), StartPositionZ));
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
		Elevator *elevator = sbs->GetElevator(i);
		if (elevator->IsInElevator(GetPosition()) == true)
		{
			test = true;
			if (sbs->InElevator == false || sbs->ElevatorNumber != i)
			{
				elevator->EnableObjects(true);
				elevator->UpdateFloorIndicators();
			}
			sbs->InElevator = true;
			sbs->ElevatorNumber = i;
			sbs->ElevatorSync = true;
			elevator = 0;
			return;
		}
		else if (sbs->InElevator == true && sbs->ElevatorNumber == i)
			elevator->EnableObjects(false); //turn off objects if user is not in the checked elevator
		else if (elevator->CameraOffset > elevator->Height && elevator->CameraOffset < elevator->Height * 2)
		{
			//if camera is within vertical elevator range, turn on syncing to allow things like elevator surfing
			test = true;
			sbs->ElevatorNumber = i;
			sbs->ElevatorSync = true;
		}
		elevator = 0;
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
	//check to see if user (camera) is in a shaft

	for (int i = 1; i <= sbs->Shafts(); i++)
	{
		Shaft *shaft = sbs->GetShaft(i);

		if (!shaft)
			continue; //skip if shaft doesn't exist

		if (shaft->IsInShaft(GetPosition()) == true)
		{
			if (shaft->InsideShaft == false && sbs->InElevator == false)
			{
				//user is in the shaft
				shaft->InsideShaft = true;
				sbs->InShaft = true;

				//turn on entire shaft
				shaft->EnableWholeShaft(true, true);
			}
			else if (shaft->InsideShaft == true && sbs->InElevator == true)
			{
				//user has moved from the shaft to an elevator
				shaft->InsideShaft = false;
				sbs->InShaft = false;

				//turn off entire shaft if ShowFullShafts is false
				if (sbs->ShowFullShafts == false)
					shaft->EnableWholeShaft(false, true);
				else
					shaft->EnableWholeShaft(true, true);
			}
			else if (shaft->InsideShaft == false && sbs->InElevator == true && sbs->ShowFullShafts == false)
			{
				//if user is in an elevator, show a range of the shaft at a time (while it's moving)
				shaft->EnableRange(CurrentFloor, sbs->ShaftDisplayRange, true, false);
				sbs->GetElevator(sbs->ElevatorNumber)->ShaftDoorsEnabledRange(0, CurrentFloor, sbs->ShaftDisplayRange);
			}

			if (shaft->InsideShaft == false && sbs->InElevator == true && sbs->GetElevator(sbs->ElevatorNumber)->IsMoving == true)
			{
				//if specified, show floors or outside if user is in a moving elevator
				if (shaft->ShowFloors == true)
					sbs->EnableFloorRange(CurrentFloor, sbs->FloorDisplayRange, true, true, i);

				if (shaft->ShowOutside == true)
				{
					int loc = -1;
					for (int i = 0; i < (int)shaft->ShowOutsideList.size(); i++)
					{
						if (shaft->ShowOutsideList[i] == CurrentFloor)
							loc = i;
					}

					if (loc != -1)
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
		}
		else if (shaft->InsideShaft == true)
		{
			//user has moved out of the shaft
			shaft->InsideShaft = false;
			sbs->InShaft = false;

			//turn off shaft
			shaft->EnableWholeShaft(false, true);
		}
		else if (shaft->InsideShaft == false)
		{
			//show specified shaft range if outside the shaft
			shaft->EnableRange(CurrentFloor, sbs->ShaftOutsideDisplayRange, true, true);
		}
		shaft = 0;
	}
}

void Camera::CheckStairwell()
{
	//check to see if user (camera) is in a stairwell

	for (int i = 1; i <= sbs->StairsNum(); i++)
	{
		Stairs *stairs = sbs->GetStairs(i);
		if (stairs->IsInStairwell(GetPosition()) == true)
		{
			if (stairs->InsideStairwell == false)
			{
				stairs->InsideStairwell = true;

				//turn on entire stairwell if ShowFullStairs is true
				if (sbs->ShowFullStairs == true)
					stairs->EnableWholeStairwell(true);
			}
			if (stairs->InsideStairwell == true)
			{
				//show specified stairwell range while in the stairwell
				stairs->EnableRange(CurrentFloor, sbs->StairsDisplayRange);

				//if user walked to a different floor, enable new floor and disable previous
				if (CurrentFloor != FloorTemp)
				{
					if (sbs->GetFloor(CurrentFloor)->IsInGroup(FloorTemp) == false)
					{
						//only disable other floor if it's not in a group list
						sbs->GetFloor(FloorTemp)->Enabled(false);
						sbs->GetFloor(FloorTemp)->EnableGroup(false);
					}
					sbs->GetFloor(CurrentFloor)->Enabled(true);
					sbs->GetFloor(CurrentFloor)->EnableGroup(true);
				}
			}
		}
		else if (stairs->InsideStairwell == true)
		{
			stairs->InsideStairwell = false;

			//turn off stairwell if ShowFullStairs is true
			if (sbs->ShowFullStairs == true)
				stairs->EnableWholeStairwell(false);
		}
		else if (stairs->InsideStairwell == false)
		{
			//show specified stairwell range if outside the stairwell
			stairs->EnableRange(CurrentFloor, sbs->StairsOutsideDisplayRange);
		}
		stairs = 0;
	}
	FloorTemp = CurrentFloor;
}

void Camera::ClickedObject(bool shift, bool ctrl, bool alt)
{
	//get mesh object that the user clicked on, and perform related work

	//cast a ray from the camera in the direction of the clicked position
	float width = MainCamera->getViewport()->getActualWidth();
	float height = MainCamera->getViewport()->getActualHeight();
	float x = sbs->mouse_x / width;
	float y = sbs->mouse_y / height;
	Ogre::Ray ray = MainCamera->getCameraToViewportRay(x, y);
	
	//get a collision callback from Bullet
	OgreBulletCollisions::CollisionClosestRayResultCallback callback (ray, sbs->mWorld, sbs->ToRemote(1000));

	//check for collision
	sbs->mWorld->launchRay(callback);

	//exit if no collision
	if (callback.doesCollide() == false)
		return;

	//get collided collision object
	OgreBulletCollisions::Object* object = callback.getCollidedObject();

	//get name of collision object's parent scenenode (which is the same name as the mesh object)
	meshname = object->getRootNode()->getName();

	//get hit/intersection position
	HitPosition = sbs->ToLocal(callback.getCollisionPoint());

	//get wall name
	WallObject* wall = 0;
	MeshObject* meshobject = sbs->FindMeshObject(meshname);
	if (!meshobject)
		return;

	int num = meshobject->FindWall(HitPosition);
	if (num > -1)
		wall = meshobject->Walls[num];

	if (wall)
		polyname = wall->GetName();
	else
		polyname = "";

	//get and strip object number
	std::string number;
	object_number = 0;
	if (wall)
	{
		object_number = wall->GetNumber();
		int index = polyname.find(")");
		if (index > -1)
			polyname.erase(polyname.begin(), polyname.begin() + index + 1);
	}
	if (meshname.find("(") == 0)
	{
		int index = meshname.find(")");
		if (object_number == 0)
			object_number = atoi(meshname.substr(1, index - 1).c_str());
		meshname.erase(meshname.begin(), meshname.begin() + index + 1);
	}
	number = Ogre::StringConverter::toString(object_number);

	//store parameters of object
	Object *obj = sbs->GetObject(object_number);
	if (obj)
	{
		object_line = obj->linenum;
		object_cmd = obj->command;
		object_cmd_processed = obj->command_processed;
	}

	//show result
	if (wall)
		sbs->Report("Clicked on object " + number + ": Mesh: " + meshname + ", Polygon: " + polyname);
	else
		sbs->Report("Clicked on object " + number + ": " + meshname);

	//delete wall if ctrl is pressed
	if (wall && ctrl == true && object_number > 0)
	{
		if (obj)
		{
			if (std::string(obj->GetType()) == "Wall")
				sbs->DeleteObject(obj);
			else
				sbs->Report("Cannot delete object " + number);
			return;
		}
	}

	//check call buttons
	if (meshname.find("Call Button") != -1)
	{
		//user clicked on a call button
		int index = meshname.find(":");
		int index2 = meshname.find(":", index + 1);
		int floor = atoi(meshname.substr(12, index - 12).c_str());
		int number = atoi(meshname.substr(index + 1, index2 - index - 1).c_str());
		CallButton *buttonref = sbs->GetFloor(floor)->CallButtonArray[number];
		std::string direction = meshname.substr(index2 + 1);
		TrimString(direction);

		if (shift == false)
		{
			//press button
			if (direction == "Up")
				buttonref->Call(true);
			else
				buttonref->Call(false);
		}
		else
		{
			//if shift is held, change button status instead
			if (direction == "Up")
				buttonref->UpLight(!buttonref->UpStatus);
			else
				buttonref->DownLight(!buttonref->DownStatus);
		}
	}

	//check elevator buttons
	if (meshname.find("Button Panel") != -1 && meshname.find("Control") != -1)
	{
		//user clicked on an elevator button
		int index = meshname.find(":");
		int index2 = meshname.find("Control");
		int elevator = atoi(meshname.substr(13, index - 13).c_str());
		int panel_number = atoi(meshname.substr(index + 1, meshname.find(" ", index) - index - 1).c_str());
		int control_number = atoi(meshname.substr(index2 + 8).c_str());

		//press button
		sbs->GetElevator(elevator)->GetPanel(panel_number)->Press(control_number);
	}

	//check shaft doors
	if (meshname.find("Shaft Door") != -1 && shift == true)
	{
		//user clicked on a shaft door
		int elevator = atoi(meshname.substr(9, meshname.find(":") - 9).c_str());
		int index = meshname.find("Shaft Door");
		int index2 = meshname.find(":", index);
		int number = atoi(meshname.substr(index + 10, index2 - (index + 10)).c_str());
		int floor = atoi(meshname.substr(index2 + 1, meshname.length() - index2 - 2).c_str());
		if (sbs->GetElevator(elevator)->AreShaftDoorsOpen(number, floor) == false)
			sbs->GetElevator(elevator)->OpenDoorsEmergency(number, 3, floor);
		else
			sbs->GetElevator(elevator)->CloseDoorsEmergency(number, 3, floor);
	}

	//check elevator doors
	if (meshname.find("ElevatorDoor") != -1 && shift == true)
	{
		//user clicked on an elevator door
		int elevator = atoi(meshname.substr(13, meshname.find(":") - 13).c_str());
		int number = atoi(meshname.substr(meshname.find(":") + 1, meshname.length() - meshname.find(":") - 1).c_str());
		if (sbs->GetElevator(elevator)->AreDoorsOpen(number) == false)
			sbs->GetElevator(elevator)->OpenDoorsEmergency(number, 2);
		else
			sbs->GetElevator(elevator)->CloseDoorsEmergency(number, 2);
	}

	//check doors
	if (meshname.find(":Door") != -1)
	{
		//user clicked on a door
		if (meshname.substr(0, 5) == "Floor")
		{
			int marker = meshname.find(":");
			int floornumber = atoi(meshname.substr(5, marker - 5).c_str());
			int doornumber = atoi(meshname.substr(marker + 5).c_str());
			Floor *floor = sbs->GetFloor(floornumber);
			if (floor->IsDoorOpen(doornumber) == false)
			{
				if (floor->IsDoorMoving(doornumber) == false)
					floor->OpenDoor(doornumber);
				else
					floor->CloseDoor(doornumber);
			}
			else
			{
				if (floor->IsDoorMoving(doornumber) == false)
					floor->CloseDoor(doornumber);
				else
					floor->OpenDoor(doornumber);
			}
		}
		if (meshname.substr(0, 9) == "Stairwell")
		{
			int marker = meshname.find(":");
			int stairsnumber = atoi(meshname.substr(9, marker - 9).c_str());
			int doornumber = atoi(meshname.substr(marker + 5).c_str());
			Stairs *stairs = sbs->GetStairs(stairsnumber);
			if (stairs->IsDoorOpen(doornumber) == false)
			{
				if (stairs->IsDoorMoving(doornumber) == false)
					stairs->OpenDoor(doornumber);
				else
					stairs->CloseDoor(doornumber);
			}
			else
			{
				if (stairs->IsDoorMoving(doornumber) == false)
					stairs->CloseDoor(doornumber);
				else
					stairs->OpenDoor(doornumber);
			}
		}
		if (meshname.substr(0, 8) == "Elevator" && meshname.find("Shaft Door") == -1 && meshname.find("ElevatorDoor") == -1)
		{
			int marker = meshname.find(":");
			int elevnumber = atoi(meshname.substr(8, marker - 8).c_str());
			int doornumber = atoi(meshname.substr(marker + 5).c_str());
			Elevator *elevator = sbs->GetElevator(elevnumber);
			if (elevator->IsDoorOpen(doornumber) == false)
			{
				if (elevator->IsDoorMoving(doornumber) == false)
					elevator->OpenDoor(doornumber);
				else
					elevator->CloseDoor(doornumber);
			}
			else
			{
				if (elevator->IsDoorMoving(doornumber) == false)
					elevator->CloseDoor(doornumber);
				else
					elevator->OpenDoor(doornumber);
			}
		}
	}
}

const char* Camera::GetClickedMeshName()
{
	//return name of last clicked mesh

	return meshname.c_str();
}

const char* Camera::GetClickedPolyName()
{
	//return name of last clicked polygon

	return polyname.c_str();
}

int Camera::GetClickedObjectNumber()
{
	//return number of last clicked object
	return object_number;
}

int Camera::GetClickedObjectLine()
{
	//return line number of last clicked object
	return object_line;
}

const char *Camera::GetClickedObjectCommand()
{
	//return command line of last clicked object
	return object_cmd.c_str();
}

const char *Camera::GetClickedObjectCommandP()
{
	//return processed command line of last clicked object
	return object_cmd_processed.c_str();
}

void Camera::CreateColliders()
{
	// Define the player bounding box.

	/*collider_actor.SetCollideSystem(sbs->collision_sys);
	collider_actor.SetEngine(sbs->engine);
	Ogre::Vector3 legs (cfg_legs_width, cfg_legs_height, cfg_legs_depth);
	Ogre::Vector3 body (cfg_body_width, cfg_body_height, cfg_body_depth);
	Ogre::Vector3 shift (0, -(cfg_legs_height + cfg_body_height), 0);
	collider_actor.InitializeColliders (MainCamera, sbs->ToRemote(legs), sbs->ToRemote(body), sbs->ToRemote(shift));
	collider_actor.SetCamera(MainCamera, true);
	EnableGravity(GravityStatus);*/
}

void Camera::Loop()
{
	//calculate acceleration
	InterpolateMovement();

	//general movement
	float delta = sbs->GetElapsedTime() / 1000.0f;
	//float delta = sbs->GetElapsedTime() / 1000.0f;
	Move(velocity * speed, delta);
	RotateLocal(angle_velocity * speed, delta);

	//get list of hit meshes and put them into the 'hitlist' array
	/*if (EnableCollisions == true)
	{
		if (collider_actor.CheckHitMeshes())
		{
			//get mesh list iterator from the actor object
			csSet<csPtrKey<iMeshWrapper> >::GlobalIterator iterator = collider_actor.GetHitMeshes().GetIterator();

			//get names of each hit mesh and check if any where shaft doors or elevator doors
			while (iterator.HasNext())
			{
				//get mesh name
				std::string mesh = iterator.Next()->QueryObject()->GetName();
				int index = mesh.find(")");
				mesh.DeleteAt(0, index + 1);
				LastHitMesh = mesh;

				//report name of mesh
				if (ReportCollisions == true)
					sbs->Report(mesh);

				//check shaft doors
				if (mesh.find("Shaft Door") != -1)
				{
					//user hit a shaft door
					int elevator = atoi(mesh.substr(9, mesh.find(":") - 9));
					int index = mesh.find("Shaft Door");
					int index2 = mesh.find(":", index);
					int number = atoi(mesh.substr(index + 10, index2 - (index + 10)));
					//int floor = atoi(mesh.substr(index2 + 1, mesh.length() - index2 - 2));
					Elevator *tmpelevator = sbs->GetElevator(elevator);
					int whichdoors = tmpelevator->GetDoor(number)->GetWhichDoors();
					if (tmpelevator->GetDoor(number)->OpenDoor == -1 && whichdoors == 1)
						sbs->GetElevator(elevator)->OpenDoors(number, 1);
					tmpelevator = 0;
				}

				//check elevator doors
				if (mesh.find("ElevatorDoor") != -1)
				{
					//user hit an elevator door
					int elevator = atoi(mesh.substr(13, mesh.find(":") - 13));
					int number = atoi(mesh.substr(mesh.find(":") + 1, mesh.length() - mesh.find(":") - 1));
					Elevator *tmpelevator = sbs->GetElevator(elevator);
					int whichdoors = tmpelevator->GetDoor(number)->GetWhichDoors();
					if (tmpelevator->GetDoor(number)->OpenDoor == -1 && whichdoors == 1)
						sbs->GetElevator(elevator)->OpenDoors(number, 1);
					tmpelevator = 0;
				}
			}
		}
		else
			collider_actor.EnableHitMeshes(true); //tell collider to report on hit meshes if currently off
	}*/

	//sync sound listener object to camera position
	sbs->SetListenerPosition(GetPosition());

	//set direction of listener to camera's direction
	Ogre::Vector3 front, top;
	GetDirection(front, top);
	sbs->SetListenerDirection(front, top);
}

void Camera::Strafe(float speed)
{
	speed *= cfg_walk_maxspeed_multreal;
	desired_velocity.x = -cfg_strafespeed * speed * cfg_walk_maxspeed * cfg_walk_maxspeed_multreal;
}

void Camera::Step(float speed)
{
	speed *= cfg_walk_maxspeed_multreal;
	desired_velocity.z = cfg_stepspeed * speed * cfg_walk_maxspeed * cfg_walk_maxspeed_multreal;
}

void Camera::Float(float speed)
{
	speed *= cfg_walk_maxspeed_multreal;
	desired_velocity.y = cfg_floatspeed * speed * cfg_walk_maxspeed * cfg_walk_maxspeed_multreal;
}

void Camera::Jump()
{
	velocity.y = cfg_jumpspeed;
	desired_velocity.y = 0.0f;
}

void Camera::Look(float speed)
{
	//look up/down by rotating camera on X axis
	desired_angle_velocity.x = cfg_lookspeed * speed * cfg_rotate_maxspeed;
}

void Camera::Turn(float speed)
{
	//turn camera by rotating on Y axis
	desired_angle_velocity.y = cfg_turnspeed * speed * cfg_rotate_maxspeed * cfg_walk_maxspeed_multreal;
}

void Camera::Spin(float speed)
{
	//spin camera by rotating on Z axis
	desired_angle_velocity.z = cfg_spinspeed * speed * cfg_rotate_maxspeed;
}

void Camera::InterpolateMovement()
{
	//calculate acceleration
	float elapsed = sbs->GetElapsedTime() / 1000.0f;
	elapsed *= 1700.0f;

	for (size_t i = 0; i < 3; i++)
	{
		if (velocity[i] < desired_velocity[i])
		{
			velocity[i] += cfg_walk_accelerate * elapsed;
			if (velocity[i] > desired_velocity[i])
				velocity[i] = desired_velocity[i];
		}
		else
		{
			velocity[i] -= cfg_walk_accelerate * elapsed;
			if (velocity[i] < desired_velocity[i])
				velocity[i] = desired_velocity[i];
		}
	}

	for (size_t i = 0; i < 3; i++)
	{
		if (angle_velocity[i] < desired_angle_velocity[i])
		{
			angle_velocity[i] += cfg_rotate_accelerate * elapsed;
			if (angle_velocity[i] > desired_angle_velocity[i])
				angle_velocity[i] = desired_angle_velocity[i];
		}
		else
		{
			angle_velocity[i] -= cfg_rotate_accelerate * elapsed;
			if (angle_velocity[i] < desired_angle_velocity[i])
				angle_velocity[i] = desired_angle_velocity[i];
		}
	}
}

void Camera::SetGravity(float gravity)
{
	Gravity = gravity;
	sbs->mWorld->setGravity(Ogre::Vector3(0, sbs->ToRemote(-Gravity), 0));
}

float Camera::GetGravity()
{
	return Gravity;
}

void Camera::EnableGravity(bool value)
{
	if (value == true)
		sbs->mWorld->setGravity(Ogre::Vector3(0, sbs->ToRemote(-Gravity), 0));
	else
	{
		//stop all velocity
		sbs->mWorld->setGravity(Ogre::Vector3::ZERO);
		mBody->setLinearVelocity(Ogre::Vector3::ZERO);
		velocity.y = 0;
		desired_velocity.y = 0;
	}
	GravityStatus = value;
}

bool Camera::GetGravityStatus()
{
	return GravityStatus;
}

void Camera::SetFOVAngle(float angle)
{
	//set camera FOV angle
	//if (angle > 0 && angle < 179.63)
		//CameraNode->SetFOVAngle(angle, sbs->g2d->GetWidth());
}

float Camera::GetFOVAngle()
{
	//return MainCamera->GetFOVAngle();
	return 0;
}

void Camera::SetToDefaultFOV()
{
	//set to default FOV angle value
	SetFOVAngle(FOV);
}

float Camera::GetHeight()
{
	//return camera's height
	return cfg_body_height + cfg_legs_height;
}

void Camera::SetViewMode(int mode)
{
	//set view mode of camera:
	//0 - solid polygons (normal)
	//1 - wireframe mode
	//2 - point mode

	if (mode == 0)
		MainCamera->setPolygonMode(Ogre::PM_SOLID);
	if (mode == 1)
		MainCamera->setPolygonMode(Ogre::PM_WIREFRAME);
	if (mode == 2)
		MainCamera->setPolygonMode(Ogre::PM_POINTS);
}

void Camera::EnableCollisions(bool value)
{
	if (value == Collisions)
		return;

	Collisions = value;
	mBody->enableCollisions(value);
}

bool Camera::CollisionsEnabled()
{
	return Collisions;
}

bool Camera::IsOnGround()
{
	//return true or false depending on if the camera is on the ground - uses Bullet raycasting
	//this function currently doesn't work properly

	//cast a ray from the camera position downwards
	Ogre::Ray ray (CameraNode->getPosition(), Ogre::Vector3::NEGATIVE_UNIT_Y);
	
	//get a collision callback from Bullet
	OgreBulletCollisions::CollisionClosestRayResultCallback callback (ray, sbs->mWorld, sbs->ToRemote((cfg_body_height + cfg_legs_height) / 2));

	//check for collision
	sbs->mWorld->launchRay(callback);

	return callback.doesCollide();
}
