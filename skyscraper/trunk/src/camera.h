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

#ifndef _SBS_CAMERA_H
#define _SBS_CAMERA_H

#include <iengine/camera.h>
#include <iengine/movable.h>

class SBSIMPEXP Camera
{
public:
	Object *object; //SBS object
	int CurrentFloor; //floor camera's on
	csString CurrentFloorID; //indicator ID of camera's current floor
	int StartFloor; //Starting floor
	float StartPositionX; //starting position on X axis
	float StartPositionZ; //starting position on Z axis
	float cfg_jumpspeed; //Initial speed of jumping.
	float cfg_walk_accelerate; //walk acceleration
	float cfg_walk_maxspeed; //walk maximum speed
	float cfg_walk_maxspeed_mult; //multiplier for max speed
	float cfg_walk_maxspeed_multreal; //is multiplier used?
	float cfg_walk_brake; //walk brake deceleration
	float cfg_rotate_accelerate; //rotate acceleration
	float cfg_rotate_maxspeed; //rotate maximum speed
	float cfg_rotate_brake; //rotate brake deceleration
	float cfg_look_accelerate; //look acceleration
	float cfg_body_height; //body height
	float cfg_body_width; //body width
	float cfg_body_depth; //body depth
	float cfg_legs_height; //legs height
	float cfg_legs_width; //legs width
	float cfg_legs_depth; //legs depth
	float cfg_lookspeed; //base look speed
	float cfg_turnspeed; //base turn speed
	float cfg_spinspeed; //base spin speed
	float cfg_floatspeed; //base float speed
	float cfg_stepspeed; //base step speed
	float cfg_strafespeed; //base strafe speed
	float cfg_speed; //camera speed multiplier
	float cfg_speedfast; //camera fast speed multiplier
	float cfg_speedslow; //camera slow speed multiplier
	float cfg_zoomspeed; //camera zoom speed
	csVector3 desired_velocity;
	csVector3 velocity;
	csVector3 desired_angle_velocity;
	csVector3 angle_velocity;
	float speed; //movement speed base
	bool EnableCollisions; //enable collision detection
	bool MouseDown; //mouse status
	bool ReportCollisions; //if true, print collisions on console
	csString LastHitMesh; //name of last hit mesh
	bool Freelook; //freelook (mouselook) is enabled/disabled
	float Freelook_speed; //freelook speed
	csVector3 HitPosition; //last hit position

	//functions
	Camera();
	~Camera();
	void SetPosition(const csVector3 &vector);
	void SetDirection(const csVector3 &vector);
	void SetRotation(csVector3 vector);
	csVector3 GetPosition();
	void GetDirection(csVector3 &front, csVector3 &top);
	csVector3 GetRotation();
	void UpdateCameraFloor();
	bool Move(const csVector3 &vector, float speed);
	void Rotate(const csVector3 &vector, float speed);
	void SetStartDirection(const csVector3 &vector);
	csVector3 GetStartDirection();
	void SetStartRotation(const csVector3 &vector);
	csVector3 GetStartRotation();
	void SetToStartPosition();
	void SetToStartDirection();
	void SetToStartRotation();
	void CheckElevator();
	void CheckShaft();
	void CheckStairwell();
	void ClickedObject(bool shift, bool ctrl, bool alt);
	const char* GetClickedMeshName();
	const char* GetClickedPolyName();
	int GetClickedObjectNumber();
	int GetClickedObjectLine();
	const char* GetClickedObjectCommand();
	const char* GetClickedObjectCommandP();
	void CreateColliders();
	void Loop();
	void Strafe(float speed);
	void Step(float speed);
	void Float(float speed);
	void Jump();
	void Look(float speed);
	void Turn(float speed);
	void Spin(float speed);
	void InterpolateMovement();
	void SetGravity(float gravity);
	float GetGravity();
	void EnableGravity(bool value);
	bool GetGravityStatus();
	void SetFOVAngle(float angle);
	float GetFOVAngle();
	void SetToDefaultFOV();
	float GetHeight();

private:
	csRef<iCamera> MainCamera; //main first-person view camera
	csVector3 StartDirection; //direction camera faces on start
	csVector3 StartRotation; //camera's startup rotation
	csString meshname; //last clicked mesh name
	csString polyname; //last clicked polygon name
	int object_number; //last clicked object number
	int object_line; //script line number of last clicked object
	csString object_cmd; //original script command of last clicked object
	csString object_cmd_processed; //processed script command of last clicked object
	char intbuffer[65];
	char buffer[20];
	int FloorTemp; //previous floor check value
	float Gravity;
	bool GravityStatus;
	int lastfloor;
	bool lastfloorset;
	float FOV; //default FOV angle
	bool ResetOnGround;

	//collision
	csColliderActor collider_actor;
};

#endif
