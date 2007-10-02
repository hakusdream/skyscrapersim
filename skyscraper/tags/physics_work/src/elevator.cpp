/* $Id$ */

/*
	Scalable Building Simulator - Elevator Subsystem Class
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
#include "elevator.h"
#include "buttonpanel.h"
#include "sbs.h"
#include "camera.h"
#include "shaft.h"
#include "unix.h"

extern SBS *sbs; //external pointer to the SBS engine

Elevator::Elevator(int number)
{
	csString buffer;

	//set elevator number
	Number = number;

	//init variables
	Name = "";
	QueuePositionDirection = 0;
	LastQueueFloor[0] = 0;
	LastQueueFloor[1] = 0;
	PauseQueueSearch = true;
	ElevatorSpeed = 0;
	MoveElevator = false;
	MoveElevatorFloor = 0;
	GotoFloor = 0;
	OpenDoor = 0;
	Acceleration = 0;
	Deceleration = 0;
	OpenSpeed = 0;
	ElevatorStart = 0;
	ElevatorFloor = 0;
	DoorsOpen = false;
	Direction = 0;
	DistanceToTravel = 0;
	Destination = 0;
	ElevatorRate = 0;
	StoppingDistance = 0;
	CalculateStoppingDistance = false;
	Brakes = false;
	ElevatorDoorSpeed = 0;
	ElevWait = false;
	EmergencyStop = false;
	WhichDoors = 0;
	ShaftDoorFloor = 0;
	AssignedShaft = 0;
	IsEnabled = true;
	Height = 0;
	Panel = 0;
	DoorAcceleration = 0;
	TempDeceleration = 0;
	ErrorOffset = 0;

	//create object meshes
	buffer = Number;
	buffer.Insert(0, "Elevator ");
	buffer.Trim();
	ElevatorMesh = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	Elevator_state = scfQueryInterface<iThingFactoryState> (ElevatorMesh->GetMeshObject()->GetFactory());
	Elevator_movable = ElevatorMesh->GetMovable();
	ElevatorMesh->SetZBufMode(CS_ZBUF_USE);

	buffer = Number;
	buffer.Insert(0, "FloorIndicator ");
	buffer.Trim();
	FloorIndicator = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	FloorIndicator_state = scfQueryInterface<iThingFactoryState> (FloorIndicator->GetMeshObject()->GetFactory());
	FloorIndicator_movable = FloorIndicator->GetMovable();
	FloorIndicator->SetZBufMode(CS_ZBUF_USE);

	buffer = Number;
	buffer.Insert(0, "ElevatorDoorL ");
	buffer.Trim();
	ElevatorDoorL = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	ElevatorDoorL_state = scfQueryInterface<iThingFactoryState> (ElevatorDoorL->GetMeshObject()->GetFactory());
	ElevatorDoorL_movable = ElevatorDoorL->GetMovable();
	ElevatorDoorL->SetZBufMode(CS_ZBUF_USE);

	buffer = Number;
	buffer.Insert(0, "ElevatorDoorR ");
	buffer.Trim();
	ElevatorDoorR = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	ElevatorDoorR_state = scfQueryInterface<iThingFactoryState> (ElevatorDoorR->GetMeshObject()->GetFactory());
	ElevatorDoorR_movable = ElevatorDoorR->GetMovable();
	ElevatorDoorR->SetZBufMode(CS_ZBUF_USE);

	buffer = Number;
	buffer.Insert(0, "Plaque ");
	buffer.Trim();
	Plaque = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
	Plaque_state = scfQueryInterface<iThingFactoryState> (Plaque->GetMeshObject()->GetFactory());
	Plaque_movable = Plaque->GetMovable();
	Plaque->SetZBufMode(CS_ZBUF_USE);
}

Elevator::~Elevator()
{
	//Destructor
	if (Panel)
		delete Panel;
	Panel = 0;
}

void Elevator::CreateElevator(float x, float z, int floor)
{
	//Creates elevator at specified location and floor
	//x and z are the center coordinates

	//set data
	Origin = csVector3(x, sbs->GetFloor(floor)->Altitude + sbs->GetFloor(floor)->InterfloorHeight, z);
	OriginFloor = floor;

	//add elevator to associated shaft's list
	sbs->GetShaft(AssignedShaft)->elevators.InsertSorted(Number);

	//move objects to positions
	Elevator_movable->SetPosition(Origin);
	Elevator_movable->UpdateMove();
	FloorIndicator_movable->SetPosition(Origin);
	FloorIndicator_movable->UpdateMove();
	Plaque_movable->SetPosition(Origin);
	Plaque_movable->UpdateMove();
	ElevatorDoorL_movable->SetPosition(Origin);
	ElevatorDoorL_movable->UpdateMove();
	ElevatorDoorR_movable->SetPosition(Origin);
	ElevatorDoorR_movable->UpdateMove();

	//resize shaft door arrays
	ShaftDoorL.SetSize(ServicedFloors.GetSize());
	ShaftDoorR.SetSize(ServicedFloors.GetSize());
	ShaftDoorL_state.SetSize(ServicedFloors.GetSize());
	ShaftDoorR_state.SetSize(ServicedFloors.GetSize());
	ShaftDoorsOpen.SetSize(ServicedFloors.GetSize());

	sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": created at " + csString(_gcvt(x, 12, buffer)) + ", " + csString(_gcvt(z, 12, buffer)) + ", " + csString(_itoa(floor, buffer, 12)));
}

void Elevator::AddRoute(int floor, int direction)
{
	//Add call route to elevator routing table, in sorted order

	if (direction == 1)
	{
		if (UpQueue.GetSize() == 0 && QueuePositionDirection == 0)
			PauseQueueSearch = false;
		UpQueue.InsertSorted(floor);
		LastQueueFloor[0] = floor;
		LastQueueFloor[1] = 1;
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": adding route to floor " + csString(_itoa(floor, intbuffer, 10)) + " direction up");
	}
	else
	{
		if (DownQueue.GetSize() == 0 && QueuePositionDirection == 0)
			PauseQueueSearch = false;
		DownQueue.InsertSorted(floor);
		LastQueueFloor[0] = floor;
		LastQueueFloor[1] = -1;
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": adding route to floor " + csString(_itoa(floor, intbuffer, 10)) + " direction down");
	}

}

void Elevator::DeleteRoute(int floor, int direction)
{
	//Delete call route from elevator routing table

	if (direction == 1)
	{
		UpQueue.Delete(floor);
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": deleting route to floor " + csString(_itoa(floor, intbuffer, 10)) + " direction up");
	}
	else
	{
		DownQueue.Delete(floor);
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": deleting route to floor " + csString(_itoa(floor, intbuffer, 10)) + " direction down");
	}
}

void Elevator::CancelLastRoute()
{
	//cancels the last added route

	if (LastQueueFloor[1] == 1)
	{
		DeleteRoute(LastQueueFloor[0], 1);
		LastQueueFloor[0] = 0;
		LastQueueFloor[1] = 0;
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": cancelling last route");
	}
	else if (LastQueueFloor[1] == -1)
	{
		DeleteRoute(LastQueueFloor[0], -1);
		LastQueueFloor[0] = 0;
		LastQueueFloor[1] = 0;
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": cancelling last route");
	}
}

void Elevator::Alarm()
{
	//elevator alarm code

	sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": alarm on");
}

void Elevator::StopElevator()
{
	//Tells elevator to stop moving, no matter where it is

	EmergencyStop = true;
	sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": emergency stop");
}

void Elevator::OpenHatch()
{
	//Opens the elevator's upper escape hatch, allowing access to the shaft

	sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": opening hatch");
}

void Elevator::ProcessCallQueue()
{
	//Processes the elevator's call queue, and sends elevators to called floors

	//set queue search direction if queues aren't empty
	if (QueuePositionDirection == 0)
	{
		if (UpQueue.GetSize() != 0)
			QueuePositionDirection = 1;
		if (DownQueue.GetSize() != 0)
			QueuePositionDirection = -1;
	}

	//pause queue search if both queues are empty
	if (UpQueue.GetSize() == 0 && DownQueue.GetSize() == 0)
	{
		QueuePositionDirection = 0;
		PauseQueueSearch = true;
		return;
	}

	//set search direction to 0 if any related queue is empty
	if (QueuePositionDirection == 1 && UpQueue.GetSize() == 0)
		QueuePositionDirection = 0;
	if (QueuePositionDirection == -1 && DownQueue.GetSize() == 0)
		QueuePositionDirection = 0;

	//if elevator is moving, keep queue paused
	if (MoveElevator == true)
		PauseQueueSearch = true;

	//if elevator is stopped, pause queue
	if (QueuePositionDirection != 0 && MoveElevator == false)
		PauseQueueSearch = false;

	if (PauseQueueSearch == true)
		return;

	//Search through queue lists and find next valid floor call
	if (QueuePositionDirection == 1)
	{
		//search through up queue
		for (size_t i = 0; i < UpQueue.GetSize(); i++)
		{
			if (UpQueue[i] > GetFloor() || (UpQueue[i] < GetFloor() && UpQueue.GetSize() == 1))
			{
				PauseQueueSearch = true;
				CloseDoors();
				GotoFloor = UpQueue[i];
				MoveElevator = true;
				DeleteRoute(UpQueue[i], 1);
				return;
			}
		}
	}
	else if (QueuePositionDirection == -1)
	{
		//search through down queue
		for (size_t i = 0; i < DownQueue.GetSize(); i++)
		{
			if (DownQueue[i] < GetFloor() || (DownQueue[i] > GetFloor() && DownQueue.GetSize() == 1))
			{
				PauseQueueSearch = true;
				CloseDoors();
				GotoFloor = DownQueue[i];
				MoveElevator = true;
				DeleteRoute(DownQueue[i], -1);
				return;
			}
		}
	}
}

int Elevator::GetFloor()
{
	//Determine floor that the elevator is on

	return sbs->GetFloorNumber(GetPosition().y);
}

void Elevator::MonitorLoop()
{
	//Monitors elevator and starts actions if needed

	//make sure height value is set
	if (Height == 0)
	{
		for (int i = 0; i < Elevator_state->GetVertexCount(); i++)
		{
			if (Elevator_state->GetVertex(i).y > Height)
				Height = Elevator_state->GetVertex(i).y;
		}
	}

	//call queue processor
	ProcessCallQueue();

	if (OpenDoor == 1)
		MoveDoors(true, false);
	if (OpenDoor == -1)
		MoveDoors(false, false);
	if (OpenDoor == 2)
		MoveDoors(true, true);
	if (OpenDoor == -2)
		MoveDoors(false, true);

	if (MoveElevator == true)
		MoveElevatorToFloor();

}

void Elevator::OpenDoorsEmergency(int whichdoors, int floor)
{
	//Simulates manually prying doors open.
	//Slowly opens the elevator doors no matter where elevator is.
	//If lined up with shaft doors, then opens the shaft doors also

	if (OpenDoor != 0)
	{
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": doors in use");
		return;
	}

	//check if elevator doors are already open
	if (DoorsOpen == true && whichdoors != 3)
		return;

	//check if shaft doors are already open
	if (ShaftDoorsOpen[ServicedFloors.Find(floor)] == true && whichdoors == 3)
		return;

	if (whichdoors != 3)
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": manually opening doors");
	else
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": manually opening shaft doors on floor " + csString(_itoa(floor, intbuffer, 10)));

	OpenDoor = 2;
	WhichDoors = whichdoors;
	ShaftDoorFloor = floor;
	MoveDoors(true, true);
}

void Elevator::CloseDoorsEmergency(int whichdoors, int floor)
{
	//Simulates manually closing doors.
	//Slowly closes the elevator doors no matter where elevator is.
	//If lined up with shaft doors, then closes the shaft doors also

	if (OpenDoor != 0)
	{
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": doors in use");
		return;
	}

	//check if elevator doors are already closed
	if (DoorsOpen == false && whichdoors != 3)
		return;

	//check if shaft doors are already closed
	if (ShaftDoorsOpen[ServicedFloors.Find(floor)] == false && whichdoors == 3)
		return;

	if (whichdoors != 3)
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": manually closing doors");
	else
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": manually closing shaft doors on floor " + csString(_itoa(floor, intbuffer, 10)));

	OpenDoor = -2;
	WhichDoors = whichdoors;
	ShaftDoorFloor = floor;
	MoveDoors(false, true);
}

void Elevator::OpenDoors(int whichdoors, int floor)
{
	//Opens elevator doors

	if (OpenDoor != 0)
	{
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": doors in use");
		return;
	}

	//check if elevator doors are already open
	if (DoorsOpen == true && whichdoors != 3)
		return;

	//check if shaft doors are already open
	if (ShaftDoorsOpen[ServicedFloors.Find(floor)] == true && whichdoors == 3)
		return;

	if (whichdoors != 3)
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": opening doors");
	else
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": opening shaft doors on floor " + csString(_itoa(floor, intbuffer, 10)));

	OpenDoor = 1;
	WhichDoors = whichdoors;
	ShaftDoorFloor = floor;
	MoveDoors(true, false);
}

void Elevator::CloseDoors(int whichdoors, int floor)
{
	//Closes elevator doors

	if (OpenDoor != 0)
	{
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": doors in use");
		return;
	}

	//check if elevator doors are already closed
	if (DoorsOpen == false && whichdoors != 3)
		return;

	//check if shaft doors are already closed
	if (ShaftDoorsOpen[ServicedFloors.Find(floor)] == false && whichdoors == 3)
		return;

	if (whichdoors != 3)
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": closing doors");
	else
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": closing shaft doors on floor " + csString(_itoa(floor, intbuffer, 10)));

	OpenDoor = -1;
	WhichDoors = whichdoors;
	ShaftDoorFloor = floor;
	MoveDoors(false, false);
}

void Elevator::MoveDoors(bool open, bool emergency)
{
	//opens or closes elevator doors
	//currently only supports doors on either the left/right or front/back
	//diagonal doors will be done later, by possibly using relative plane movement

	//WhichDoors is the doors to move:
	//1 = both shaft and elevator doors
	//2 = only elevator doors
	//3 = only shaft doors

	//ShaftDoorFloor is the floor the shaft doors are on - only has effect if whichdoors is 3

	static bool IsRunning = false;
	static float OpenChange;
	static float marker1;
	static float marker2;
	static int index;
	static float stopping_distance;
	static float temp_change;
	static bool accelerating;
	static float door_error;

	//todo: turn off autoclose timer

	if (IsRunning == false)
	{
		//initialization code

		IsRunning = true;

		if (emergency == false)
		{
			OpenChange = OpenSpeed / 50;
			marker1 = DoorWidth / 8;
			marker2 = (DoorWidth / 2) - (DoorWidth / 8);
		}
		else
		{
			OpenChange = 0.003f;
			marker1 = DoorWidth / 16;
			marker2 = (DoorWidth / 2) - (DoorWidth / 16);
		}

		if (emergency == false)
		{
			//play elevator opening sound
			//"data/elevatoropen.wav"
			//"data/elevatorclose.wav"
		}

		ElevatorDoorSpeed = 0;
		if (WhichDoors == 3)
			index = ServicedFloors.Find(ShaftDoorFloor);
		else
			index = ServicedFloors.Find(GetFloor());
	}

	//get reference movable object
	csRef<iMovable> tmpMovable;
	if (WhichDoors < 3)
		tmpMovable = ElevatorDoorL_movable;
	else
		tmpMovable = ShaftDoorL[index]->GetMovable();

	bool elevdoors = false, shaftdoors = false;
	if (WhichDoors == 1)
	{
		elevdoors = true;
		shaftdoors = true;
	}
	if (WhichDoors == 2)
		elevdoors = true;
	if (WhichDoors == 3)
		shaftdoors = true;

	//Speed up doors
	if (DoorDirection == false)
	{
		if ((DoorOrigin.z - tmpMovable->GetPosition().z <= marker1 && open == true) || (DoorOrigin.z - tmpMovable->GetPosition().z > marker2 && open == false))
		{
			accelerating = true;
			if (open == true)
				ElevatorDoorSpeed += OpenChange;
			else
				ElevatorDoorSpeed -= OpenChange;

			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->MovePosition(csVector3(0, 0, -ElevatorDoorSpeed * FPSModifierStatic));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->MovePosition(csVector3(0, 0, ElevatorDoorSpeed * FPSModifierStatic));
				ElevatorDoorR_movable->UpdateMove();
			}
			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->MovePosition(csVector3(0, 0, -ElevatorDoorSpeed * FPSModifierStatic));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->MovePosition(csVector3(0, 0, ElevatorDoorSpeed * FPSModifierStatic));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}

			//get stopping distance
			stopping_distance = DoorOrigin.z - tmpMovable->GetPosition().z;

			return;
		}
	}
	else
	{
		if ((DoorOrigin.x - tmpMovable->GetPosition().x <= marker1 && open == true) || (DoorOrigin.x - tmpMovable->GetPosition().x > marker2 && open == false))
		{
			accelerating = true;
			if (open == true)
				ElevatorDoorSpeed += OpenChange;
			else
				ElevatorDoorSpeed -= OpenChange;

			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->MovePosition(csVector3(-ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->MovePosition(csVector3(ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->MovePosition(csVector3(-ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->MovePosition(csVector3(ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}

			//get stopping distance
			stopping_distance = DoorOrigin.x - tmpMovable->GetPosition().x;

			return;
		}
	}

	//Normal door movement
	if (DoorDirection == false)
	{
		if ((DoorOrigin.z - tmpMovable->GetPosition().z <= marker2 && open == true) || (DoorOrigin.z - tmpMovable->GetPosition().z > marker1 && open == false))
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->MovePosition(csVector3(0, 0, -ElevatorDoorSpeed * FPSModifierStatic));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->MovePosition(csVector3(0, 0, ElevatorDoorSpeed * FPSModifierStatic));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->MovePosition(csVector3(0, 0, -ElevatorDoorSpeed * FPSModifierStatic));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->MovePosition(csVector3(0, 0, ElevatorDoorSpeed * FPSModifierStatic));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}

			return;
		}
	}
	else
	{
		if ((DoorOrigin.x - tmpMovable->GetPosition().x <= marker2 && open == true) || (DoorOrigin.x - tmpMovable->GetPosition().x > marker1 && open == false))
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->MovePosition(csVector3(-ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->MovePosition(csVector3(ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->MovePosition(csVector3(-ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->MovePosition(csVector3(ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}

			return;
		}
	}

	if (accelerating == true)
	{
		accelerating = false;
		if (DoorDirection == false)
			temp_change = OpenChange * (stopping_distance / (tmpMovable->GetPosition().z - (DoorOrigin.z - (DoorWidth / 2))));
		else
			temp_change = OpenChange * (stopping_distance / (tmpMovable->GetPosition().x - (DoorOrigin.x - (DoorWidth / 2))));
	}

	//slow down doors
	if ((ElevatorDoorSpeed > 0 && open == true) || (ElevatorDoorSpeed < 0 && open == false))
	{
		if (open == true)
			ElevatorDoorSpeed -= temp_change;
		else
			ElevatorDoorSpeed += temp_change;
		if (DoorDirection == false)
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->MovePosition(csVector3(0, 0, -ElevatorDoorSpeed * FPSModifierStatic));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->MovePosition(csVector3(0, 0, ElevatorDoorSpeed * FPSModifierStatic));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->MovePosition(csVector3(0, 0, -ElevatorDoorSpeed * FPSModifierStatic));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->MovePosition(csVector3(0, 0, ElevatorDoorSpeed * FPSModifierStatic));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}
		}
		else
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->MovePosition(csVector3(-ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->MovePosition(csVector3(ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->MovePosition(csVector3(-ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->MovePosition(csVector3(ElevatorDoorSpeed * FPSModifierStatic, 0, 0));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}
		}
		return;
	}

	//get error value
	if (open == true && DoorDirection == false)
		door_error = tmpMovable->GetPosition().z - (DoorOrigin.z - (DoorWidth / 2));
	if (open == true && DoorDirection == true)
		door_error = tmpMovable->GetPosition().x - (DoorOrigin.x - (DoorWidth / 2));
	if (open == false && DoorDirection == false)
		door_error = DoorOrigin.z - tmpMovable->GetPosition().z;
	if (open == false && DoorDirection == true)
		door_error = DoorOrigin.x - tmpMovable->GetPosition().x;

	//place doors in positions (fixes any overrun errors)
	if (open == true)
	{
		if (DoorDirection == false)
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->SetPosition(csVector3(Origin.x, GetPosition().y, Origin.z - (DoorWidth / 2)));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->SetPosition(csVector3(Origin.x, GetPosition().y, Origin.z + (DoorWidth / 2)));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->SetPosition(csVector3(Origin.x, 0, Origin.z - (DoorWidth / 2)));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->SetPosition(csVector3(Origin.x, 0, Origin.z + (DoorWidth / 2)));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}
		}
		else
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->SetPosition(csVector3(Origin.x - (DoorWidth / 2), GetPosition().y, Origin.z));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->SetPosition(csVector3(Origin.x + (DoorWidth / 2), GetPosition().y, Origin.z));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->SetPosition(csVector3(Origin.x - (DoorWidth / 2), 0, Origin.z));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->SetPosition(csVector3(Origin.x + (DoorWidth / 2), 0, Origin.z));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}
		}
	}
	else
	{
		if (DoorDirection == false)
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->SetPosition(csVector3(Origin.x, GetPosition().y, Origin.z));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->SetPosition(csVector3(Origin.x, GetPosition().y, Origin.z));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->SetPosition(csVector3(Origin.x, 0, Origin.z));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->SetPosition(csVector3(Origin.x, 0, Origin.z));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}
		}
		else
		{
			if (elevdoors == true)
			{
				//move elevator doors
				ElevatorDoorL_movable->SetPosition(csVector3(Origin.x, GetPosition().y, Origin.z));
				ElevatorDoorL_movable->UpdateMove();
				ElevatorDoorR_movable->SetPosition(csVector3(Origin.x, GetPosition().y, Origin.z));
				ElevatorDoorR_movable->UpdateMove();
			}

			if (shaftdoors == true)
			{
				//move shaft doors
				ShaftDoorL[index]->GetMovable()->SetPosition(csVector3(Origin.x, 0, Origin.z));
				ShaftDoorL[index]->GetMovable()->UpdateMove();
				ShaftDoorR[index]->GetMovable()->SetPosition(csVector3(Origin.x, 0, Origin.z));
				ShaftDoorR[index]->GetMovable()->UpdateMove();
			}
		}
	}

	//reset values
	ElevatorDoorSpeed = 0;
	OpenDoor = 0;
	WhichDoors = 0;

	//turn on autoclose timer

	//the doors are open or closed now
	if (WhichDoors != 3)
	{
		if (open == true)
			DoorsOpen = true;
		else
			DoorsOpen = false;
	}
	else
	{
		if (open == true)
			ShaftDoorsOpen[ServicedFloors.Find(ShaftDoorFloor)] = true;
		else
			ShaftDoorsOpen[ServicedFloors.Find(ShaftDoorFloor)] = false;
	}

	IsRunning = false;

}

void Elevator::MoveElevatorToFloor()
{
	//Main processing routine; sends elevator to floor specified in GotoFloor
	static bool IsRunning = false;
	static int oldfloor;

	//exit if doors are moving
	if (OpenDoor != 0)
		return;

	if (IsRunning == false)
	{
		IsRunning = true;
		csString dir_string;

		//get elevator's current altitude
		ElevatorStart = GetPosition().y;

		//get elevator's current floor
		ElevatorFloor = GetFloor();
		oldfloor = ElevatorFloor;

		//If elevator is already on specified floor, open doors and exit
		if (ElevatorFloor == GotoFloor)
		{
			sbs->Report("Elevator already on specified floor");
			MoveElevator = false;
			IsRunning = false;
			OpenDoors();
			return;
		}

		//if destination floor is not in the ServicedFloors array, reset and exit
		if (ServicedFloors.Find(GotoFloor) == csArrayItemNotFound)
		{
			sbs->Report("Destination floor not in ServicedFloors list");
			MoveElevator = false;
			IsRunning = false;
			return;
		}

		//close doors if open
		if (DoorsOpen == true)
			CloseDoors();

		//Determine direction
		if (GotoFloor < ElevatorFloor)
		{
			Direction = -1;
			dir_string = "down";
		}
		if (GotoFloor > ElevatorFloor)
		{
			Direction = 1;
			dir_string = "up";
		}

		//Determine distance to destination floor
		Destination = sbs->GetFloor(GotoFloor)->Altitude + sbs->GetFloor(GotoFloor)->InterfloorHeight;
		DistanceToTravel = fabs(fabs(Destination) - fabs(ElevatorStart));
		CalculateStoppingDistance = true;

		//If user is riding this elevator, then turn off objects
		if (sbs->ElevatorSync == true && sbs->ElevatorNumber == Number)
		{
			//turn off floor
			sbs->GetFloor(sbs->camera->CurrentFloor)->Enabled(false);
			sbs->GetFloor(sbs->camera->CurrentFloor)->EnableGroup(false);

			//Turn off sky, buildings, and landscape
			sbs->EnableSkybox(false);
			sbs->EnableBuildings(false);
			sbs->EnableLandscape(false);
			sbs->EnableExternal(false); //temporary - remove when window objects are made
			sbs->EnableColumnFrame(false);
		}

		//Play starting sound
		//"\data\elevstart.wav"

		//Get first rate increment value
		ElevatorRate = Direction * (ElevatorSpeed * (Acceleration / sbs->FrameRate));

		//notify about movement
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": moving " + dir_string + " to floor " + csString(_itoa(GotoFloor, intbuffer, 10)));
	}

	if (EmergencyStop == true && Brakes == false)
	{
		CalculateStoppingDistance = false;
		TempDeceleration = Deceleration;
		if (Direction == 1)
			Direction = -1;
		else
			Direction = 1;
		Brakes = true;
	}

	//Movement sound
	//"\data\elevmove.wav"

	//move elevator objects and camera
	Elevator_movable->MovePosition(csVector3(0, (ElevatorRate * FPSModifierStatic) / sbs->FrameRate, 0));
	Elevator_movable->UpdateMove();
	if (sbs->ElevatorSync == true && sbs->ElevatorNumber == Number)
		sbs->camera->SetPosition(csVector3(sbs->camera->GetPosition().x, GetPosition().y + sbs->camera->DefaultAltitude, sbs->camera->GetPosition().z));
	ElevatorDoorL_movable->MovePosition(csVector3(0, (ElevatorRate * FPSModifierStatic) / sbs->FrameRate, 0));
	ElevatorDoorL_movable->UpdateMove();
	ElevatorDoorR_movable->MovePosition(csVector3(0, (ElevatorRate * FPSModifierStatic) / sbs->FrameRate, 0));
	ElevatorDoorR_movable->UpdateMove();
	FloorIndicator_movable->MovePosition(csVector3(0, (ElevatorRate * FPSModifierStatic) / sbs->FrameRate, 0));
	FloorIndicator_movable->UpdateMove();
	Plaque_movable->MovePosition(csVector3(0, (ElevatorRate * FPSModifierStatic) / sbs->FrameRate, 0));
	Plaque_movable->UpdateMove();
	if (Panel)
		Panel->Move(csVector3(0, (ElevatorRate * FPSModifierStatic) / sbs->FrameRate, 0));

	//show partial shaft areas (3 floors at a time)
	if (sbs->AutoShafts == true && sbs->InElevator == true && sbs->ElevatorNumber == Number)
	{
		int i = GetFloor();
		sbs->GetShaft(AssignedShaft)->Enabled(i, true);
		if (i > sbs->GetShaft(AssignedShaft)->startfloor)
			sbs->GetShaft(AssignedShaft)->Enabled(i - 1, true);
		if (i < sbs->GetShaft(AssignedShaft)->endfloor)
			sbs->GetShaft(AssignedShaft)->Enabled(i + 1, true);
		if (i > sbs->GetShaft(AssignedShaft)->startfloor + 1)
			sbs->GetShaft(AssignedShaft)->Enabled(i - 2, false);
		if (i < sbs->GetShaft(AssignedShaft)->endfloor - 1)
			sbs->GetShaft(AssignedShaft)->Enabled(i + 2, false);
	}

	//move sounds

	//motion calculation
	if (Brakes == false)
	{
		//regular motion
		if (Direction == 1)
			ElevatorRate += ElevatorSpeed * (Acceleration / sbs->FrameRate);
		if (Direction == -1)
			ElevatorRate -= ElevatorSpeed * (Acceleration / sbs->FrameRate);
	}
	else
	{
		//slow down
		if (Direction == 1)
			ElevatorRate += ElevatorSpeed * (TempDeceleration / sbs->FrameRate);
		if (Direction == -1)
			ElevatorRate -= ElevatorSpeed * (TempDeceleration / sbs->FrameRate);
	}

	//change speeds
	if ((Direction == 1) && (ElevatorRate > ElevatorSpeed))
	{
		ElevatorRate = ElevatorSpeed;
		CalculateStoppingDistance = false;
	}
	if ((Direction == -1) && (ElevatorRate < -ElevatorSpeed))
	{
		ElevatorRate = -ElevatorSpeed;
		CalculateStoppingDistance = false;
	}
	if ((Direction == 1) && (Brakes == true) && (ElevatorRate > 0))
		ElevatorRate = 0;
	if ((Direction == -1) && (Brakes == true) && (ElevatorRate < 0))
		ElevatorRate = 0;

	//get distance needed to stop elevator
	if (CalculateStoppingDistance == true)
	{
		if (Direction == 1)
			StoppingDistance = (GetPosition().y - ElevatorStart) * (Acceleration / Deceleration);
		else if (Direction == -1)
			StoppingDistance = (ElevatorStart - GetPosition().y) * (Acceleration / Deceleration);
	}

	//Deceleration routines with floor overrun correction (there's still problems, but it works pretty good)
	//since this function cycles at a slower/less granular rate (cycles according to frames per sec), an error factor is present where the elevator overruns the dest floor,
	//even though the algorithms are all correct. Since the elevator moves by "jumping" to a new altitude every frame - and usually jumps right over the altitude value where it is supposed to
	//start the deceleration process, causing the elevator to decelerate too late, and end up passing/overrunning the dest floor's altitude.  This code corrects this problem
	//by determining if the next "jump" will overrun the deceleration marker (which is Dest's Altitude minus Stopping Distance), and temporarily altering the deceleration rate according to how far off the mark it is
	//and then starting the deceleration process immediately.

	//up movement
	if ((Brakes == false) && (Direction == 1))
	{
		//determine if next jump altitude is over deceleration marker
		if (((GetPosition().y + ((ElevatorRate * FPSModifierStatic) / sbs->FrameRate)) > (Destination - StoppingDistance)) && (GetPosition().y != (Destination - StoppingDistance)))
		{
			CalculateStoppingDistance = false;
			//recalculate deceleration value based on distance from marker, and store result in tempdeceleration
			TempDeceleration = Deceleration * (StoppingDistance / (Destination - GetPosition().y));
			//start deceleration
			Direction = -1;
			Brakes = true;
			ElevatorRate -= ElevatorSpeed * (TempDeceleration / sbs->FrameRate);
			//stop sounds
			//play elevator stopping sound
			//"\data\elevstop.wav"
		}

		//normal routine - this will rarely happen (only if the elevator happens to reach the exact deceleration marker)
		if (GetPosition().y == (Destination - StoppingDistance))
		{
			TempDeceleration = Deceleration;
			CalculateStoppingDistance = false;
			//slow down elevator
			Direction = -1;
			Brakes = true;
			ElevatorRate -= ElevatorSpeed * (TempDeceleration / sbs->FrameRate);
			//stop sounds
			//play stopping sound
			//"\data\elevstop.wav"
		}
	}

	//down movement
	if (Brakes == false && Direction == -1)
	{
	//determine if next jump altitude is below deceleration marker
		if (((GetPosition().y - ((ElevatorRate * FPSModifierStatic) / sbs->FrameRate)) < (Destination + StoppingDistance)) && (GetPosition().y != (Destination + StoppingDistance)))
		{
			CalculateStoppingDistance = false;
			//recalculate deceleration value based on distance from marker, and store result in tempdeceleration
			TempDeceleration = Deceleration * (StoppingDistance / (GetPosition().y - Destination));
			//start deceleration
			Direction = 1;
			Brakes = true;
			ElevatorRate += ElevatorSpeed * (TempDeceleration / sbs->FrameRate);
			//stop sounds
			//play stopping sound
			//"\data\elevstop.wav"
		}

		//normal routine - this will rarely happen (only if the elevator happens to reach the exact deceleration marker)
		if (GetPosition().y == (Destination + StoppingDistance))
		{
			TempDeceleration = Deceleration;
			CalculateStoppingDistance = false;
			//slow down elevator
			Direction = 1;
			Brakes = true;
			ElevatorRate += ElevatorSpeed * (TempDeceleration / sbs->FrameRate);
			//stop sounds
			//play stopping sound
			//"\data\elevstop.wav"
		}
	}

	//update floor indicators
	if (GetFloor() != oldfloor)
		UpdateFloorIndicators();

	oldfloor = GetFloor();

	//exit if elevator's running
	if (fabs(ElevatorRate) != 0)
		return;

	if (EmergencyStop == false)
	{
		//store error offset value
		if (Direction == -1)
			ErrorOffset = GetPosition().y - Destination;
		else if (Direction == 1)
			ErrorOffset = Destination - GetPosition().y;
		else
			ErrorOffset = 0;

		//set elevator and objects to floor altitude (corrects offset errors)
		//move elevator objects and camera
		Elevator_movable->SetPosition(csVector3(GetPosition().x, Destination, GetPosition().z));
		Elevator_movable->UpdateMove();
		if (sbs->ElevatorSync == true && sbs->ElevatorNumber == Number)
			sbs->camera->SetPosition(csVector3(sbs->camera->GetPosition().x, GetPosition().y + sbs->camera->DefaultAltitude, sbs->camera->GetPosition().z));
		ElevatorDoorL_movable->SetPosition(csVector3(ElevatorDoorL_movable->GetPosition().x, Destination, ElevatorDoorL_movable->GetPosition().z));
		ElevatorDoorL_movable->UpdateMove();
		ElevatorDoorR_movable->SetPosition(csVector3(ElevatorDoorR_movable->GetPosition().x, Destination, ElevatorDoorR_movable->GetPosition().z));
		ElevatorDoorR_movable->UpdateMove();
		FloorIndicator_movable->SetPosition(csVector3(FloorIndicator_movable->GetPosition().x, Destination, FloorIndicator_movable->GetPosition().z));
		FloorIndicator_movable->UpdateMove();
		Plaque_movable->SetPosition(csVector3(Plaque_movable->GetPosition().x, Destination, Plaque_movable->GetPosition().z));
		Plaque_movable->UpdateMove();
		if (Panel)
			Panel->SetToElevatorAltitude();

		//move sounds
	}

	//reset values if at destination floor
	ElevatorRate = 0;
	Direction = 0;
	Brakes = false;
	Destination = 0;
	DistanceToTravel = 0;
	ElevatorStart = 0;
	IsRunning = false;
	MoveElevator = false;

	if (EmergencyStop == false)
	{
		//update elevator's floor number
		GetFloor();

		//Turn on objects if user is in elevator
		if (sbs->ElevatorSync == true && sbs->ElevatorNumber == Number)
		{
			//turn on floor
			sbs->GetFloor(GotoFloor)->Enabled(true);
			sbs->GetFloor(GotoFloor)->EnableGroup(true);

			//Turn on sky, buildings, and landscape
			sbs->EnableSkybox(true);
			sbs->EnableBuildings(true);
			sbs->EnableLandscape(true);
			sbs->EnableExternal(true); //temporary - remove when window objects are made
			sbs->EnableColumnFrame(true);
		}

		//open doors
		OpenDoors();

	}
	EmergencyStop = false;
}

int Elevator::AddWall(const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height1, float height2, float voffset1, float voffset2, float tw, float th)
{
	return sbs->AddWallMain(Elevator_state, name, texture, thickness, x1, z1, x2, z2, height1, height2, voffset1, voffset2, tw, th);
}

int Elevator::AddFloor(const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float voffset1, float voffset2, float tw, float th)
{
	return sbs->AddFloorMain(Elevator_state, name, texture, thickness, x1, z1, x2, z2, voffset1, voffset2, tw, th);
}

int Elevator::AddFloorIndicator(const char *direction, float CenterX, float CenterZ, float width, float height, float voffset)
{
	//Creates a floor indicator at the specified location
	int index = -1;
	csString texture = "Button" + sbs->GetFloor(OriginFloor)->ID;
	csString tmpdirection = direction;
	tmpdirection.Downcase();

	sbs->DrawWalls(true, false, false, false, false, false);
	sbs->ReverseExtents(false, false, false);
	if (tmpdirection == "front")
		index = sbs->AddWallMain(FloorIndicator_state, "Floor Indicator", texture.GetData(), 0, CenterX - (width / 2), CenterZ, CenterX + (width / 2), CenterZ, height, height, voffset, voffset, 1, 1);
	if (tmpdirection == "back")
		index = sbs->AddWallMain(FloorIndicator_state, "Floor Indicator", texture.GetData(), 0, CenterX + (width / 2), CenterZ, CenterX - (width / 2), CenterZ, height, height, voffset, voffset, 1, 1);
	if (tmpdirection == "left")
		index = sbs->AddWallMain(FloorIndicator_state, "Floor Indicator", texture.GetData(), 0, CenterX, CenterZ + (width / 2), CenterX, CenterZ - (width / 2), height, height, voffset, voffset, 1, 1);
	if (tmpdirection == "right")
		index = sbs->AddWallMain(FloorIndicator_state, "Floor Indicator", texture.GetData(), 0, CenterX, CenterZ - (width / 2), CenterX, CenterZ + (width / 2), height, height, voffset, voffset, 1, 1);
	sbs->ResetWalls();
	sbs->ResetExtents();

	if (index != -1 && !orig_indicator)
		orig_indicator = FloorIndicator_state->GetPolygonMaterial(index);

	return index;
}

int Elevator::AddDoors(const char *texture, float thickness, float CenterX, float CenterZ, float width, float height, bool direction, float tw, float th)
{
	//adds elevator doors specified at a relative central position (off of elevator origin)
	//if direction is false, doors are on the left/right side; otherwise front/back
	float x1, x2, x3, x4;
	float z1, z2, z3, z4;
	float spacing = 0.025f; //spacing between doors

	//set door parameters
	DoorDirection = direction;
	DoorWidth = width;
	DoorHeight = height;
	DoorOrigin = csVector3(Origin.x + CenterX, Origin.y, Origin.z + CenterZ);

	//set up coordinates
	if (direction == false)
	{
		x1 = CenterX;
		x2 = CenterX;
		x3 = CenterX;
		x4 = CenterX;
		z1 = CenterZ - (width / 2);
		z2 = CenterZ - spacing;
		z3 = CenterZ + spacing;
		z4 = CenterZ + (width / 2);
	}
	else
	{
		x1 = CenterX - (width / 2);
		x2 = CenterX - spacing;
		x3 = CenterX + spacing;
		x4 = CenterX + (width / 2);
		z1 = CenterZ;
		z2 = CenterZ;
		z3 = CenterZ;
		z4 = CenterZ;
	}

	//create doors
	sbs->DrawWalls(true, true, true, true, true, true);
	sbs->ReverseExtents(false, false, false);
	int firstidx = sbs->AddWallMain(ElevatorDoorL_state, "Door", texture, thickness, x1, z1, x2, z2, height, height, Origin.y, Origin.y, tw, th);
	sbs->AddWallMain(ElevatorDoorR_state, "Door", texture, thickness, x3, z3, x4, z4, height, height, Origin.y, Origin.y, tw, th);
	sbs->ResetWalls();
	sbs->ResetExtents();
	return firstidx;
}

int Elevator::AddShaftDoors(const char *texture, float thickness, float CenterX, float CenterZ, float tw, float th)
{
	//adds shaft's elevator doors specified at a relative central position (off of elevator origin)
	//uses some parameters (width, height, direction) from AddDoors function
	float x1, x2, x3, x4;
	float z1, z2, z3, z4;
	float base, base2;

	//set door parameters
	ShaftDoorOrigin = csVector3(Origin.x + CenterX, Origin.y, Origin.z + CenterZ);

	//set up coordinates
	if (DoorDirection == false)
	{
		x1 = CenterX;
		x2 = CenterX;
		x3 = CenterX;
		x4 = CenterX;
		z1 = CenterZ - (DoorWidth / 2);
		z2 = CenterZ;
		z3 = CenterZ;
		z4 = CenterZ + (DoorWidth / 2);
	}
	else
	{
		x1 = CenterX - (DoorWidth / 2);
		x2 = CenterX;
		x3 = CenterX;
		x4 = CenterX + (DoorWidth / 2);
		z1 = CenterZ;
		z2 = CenterZ;
		z3 = CenterZ;
		z4 = CenterZ;
	}

	csString buffer, buffer2, buffer3, buffer4;

	sbs->DrawWalls(true, true, true, true, true, true);
	sbs->ReverseExtents(false, false, false);

	//create doors
	for (size_t i = 0; i < ServicedFloors.GetSize(); i++)
	{
		base = sbs->GetFloor(ServicedFloors[i])->InterfloorHeight;
		base2 = sbs->GetFloor(ServicedFloors[i])->Altitude + base;

		//cut shaft and floor walls
		if (DoorDirection == false)
		{
			sbs->GetShaft(AssignedShaft)->CutWall(ServicedFloors[i], csVector3(x1 - 2, base, z1), csVector3(x1 + 2, base + DoorHeight, z4));
			sbs->GetFloor(ServicedFloors[i])->Cut(csVector3(Origin.x + x1 - 2, base, Origin.z + z1), csVector3(Origin.x + x1 + 2, base + DoorHeight, Origin.z + z4), true, false, true);
		}
		else
		{
			sbs->GetShaft(AssignedShaft)->CutWall(ServicedFloors[i], csVector3(x1, base, z1 - 2), csVector3(x4, base + DoorHeight, z1 + 2));
			sbs->GetFloor(ServicedFloors[i])->Cut(csVector3(Origin.x + x1, base, Origin.z + z1 - 2), csVector3(Origin.x + x4, base + DoorHeight, Origin.z + z1 + 2), true, false, true);
		}
		
		//create meshes
		buffer3 = Number;
		buffer4 = i;
		buffer = "Elevator " + buffer3 + ": Shaft Door " + buffer4 + "L";
		buffer2 = "Elevator " + buffer3 + ": Shaft Door " + buffer4 + "R";
		buffer.Trim();
		buffer2.Trim();
		csRef<iMeshWrapper> tmpmesh;
		csRef<iThingFactoryState> tmpstate;

		//door L
		tmpmesh = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer.GetData());
		ShaftDoorL[i] = tmpmesh;
		tmpstate = scfQueryInterface<iThingFactoryState> (ShaftDoorL[i]->GetMeshObject()->GetFactory());
		ShaftDoorL_state[i] = tmpstate;
		ShaftDoorL[i]->SetZBufMode(CS_ZBUF_USE);

		//door R
		tmpmesh = sbs->engine->CreateSectorWallsMesh (sbs->area, buffer2.GetData());
		ShaftDoorR[i] = tmpmesh;
		tmpstate = scfQueryInterface<iThingFactoryState> (ShaftDoorR[i]->GetMeshObject()->GetFactory());
		ShaftDoorR_state[i] = tmpstate;
		ShaftDoorR[i]->SetZBufMode(CS_ZBUF_USE);

		//reposition meshes
		ShaftDoorL[i]->GetMovable()->SetPosition(Origin);
		ShaftDoorL[i]->GetMovable()->UpdateMove();
		ShaftDoorR[i]->GetMovable()->SetPosition(Origin);
		ShaftDoorR[i]->GetMovable()->UpdateMove();

		//create doors
		sbs->AddWallMain(ShaftDoorL_state[i], "Door", texture, thickness, x1, z1, x2, z2, DoorHeight, DoorHeight, base2, base2, tw, th);
		sbs->AddWallMain(ShaftDoorR_state[i], "Door", texture, thickness, x3, z3, x4, z4, DoorHeight, DoorHeight, base2, base2, tw, th);

		//make doors invisible on start
		ShaftDoorL[i]->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ShaftDoorL[i]->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ShaftDoorL[i]->GetFlags().Set (CS_ENTITY_NOHITBEAM);

		ShaftDoorR[i]->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ShaftDoorR[i]->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ShaftDoorR[i]->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
	sbs->ResetWalls();
	sbs->ResetExtents();

	return 0;
}

int Elevator::AddPlaque(const char *texture, float x1, float z1, float x2, float z2, float height, float voffset, float tw, float th)
{
	sbs->DrawWalls(true, false, false, false, false, false);
	sbs->ReverseExtents(false, false, false);
	return sbs->AddWallMain(Plaque_state, "Plaque", texture, 0, x1, z1, x2, z2, height, height, voffset, voffset, tw, th);
	sbs->ResetWalls();
	sbs->ResetExtents();
}

const csVector3 Elevator::GetPosition()
{
	//returns the elevator's position
	return Elevator_movable->GetPosition();
}

void Elevator::DumpQueues()
{
	//dump both (up and down) elevator queues

	sbs->Report("--- Elevator " + csString(_itoa(Number, intbuffer, 10)) + " Queues ---\n");
	sbs->Report("Up:");
	for (size_t i = 0; i < UpQueue.GetSize(); i++)
		sbs->Report(csString(_itoa(i, intbuffer, 10)) + " - " + csString(_itoa(UpQueue[i], intbuffer, 10)));
	sbs->Report("Down:");
	for (size_t i = 0; i < DownQueue.GetSize(); i++)
		sbs->Report(csString(_itoa(i, intbuffer, 10)) + " - " + csString(_itoa(DownQueue[i], intbuffer, 10)));
}

void Elevator::Enabled(bool value)
{
	//shows/hides elevator
	if (value == true)
	{
		ElevatorMesh->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		ElevatorMesh->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		ElevatorMesh->GetFlags().Reset (CS_ENTITY_NOHITBEAM);

		FloorIndicator->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		FloorIndicator->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		FloorIndicator->GetFlags().Reset (CS_ENTITY_NOHITBEAM);

		ElevatorDoorL->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		ElevatorDoorL->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		ElevatorDoorL->GetFlags().Reset (CS_ENTITY_NOHITBEAM);

		ElevatorDoorR->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		ElevatorDoorR->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		ElevatorDoorR->GetFlags().Reset (CS_ENTITY_NOHITBEAM);

		Plaque->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		Plaque->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		Plaque->GetFlags().Reset (CS_ENTITY_NOHITBEAM);

		IsEnabled = true;
	}
	else
	{
		ElevatorMesh->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ElevatorMesh->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ElevatorMesh->GetFlags().Set (CS_ENTITY_NOHITBEAM);

		FloorIndicator->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		FloorIndicator->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		FloorIndicator->GetFlags().Set (CS_ENTITY_NOHITBEAM);

		ElevatorDoorL->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ElevatorDoorL->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ElevatorDoorL->GetFlags().Set (CS_ENTITY_NOHITBEAM);

		ElevatorDoorR->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ElevatorDoorR->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ElevatorDoorR->GetFlags().Set (CS_ENTITY_NOHITBEAM);

		Plaque->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		Plaque->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		Plaque->GetFlags().Set (CS_ENTITY_NOHITBEAM);

		IsEnabled = false;
	}
}

void Elevator::ShaftDoorsEnabled(int floor, bool value)
{
	//turns shaft elevator doors on/off

	int index = ServicedFloors.Find(floor);
	if (index == csArrayItemNotFound)
		return;

	//exit if the specified floor has no shaft doors
	if (ShaftDoorL[index] == 0)
		return;

	if (value == true)
	{
		ShaftDoorL[index]->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		ShaftDoorL[index]->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		ShaftDoorL[index]->GetFlags().Reset (CS_ENTITY_NOHITBEAM);

		ShaftDoorR[index]->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		ShaftDoorR[index]->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		ShaftDoorR[index]->GetFlags().Reset (CS_ENTITY_NOHITBEAM);
	}
	else
	{
		ShaftDoorL[index]->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ShaftDoorL[index]->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ShaftDoorL[index]->GetFlags().Set (CS_ENTITY_NOHITBEAM);

		ShaftDoorR[index]->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ShaftDoorR[index]->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ShaftDoorR[index]->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
}

bool Elevator::IsElevator(csRef<iMeshWrapper> test)
{
	if (test == ElevatorMesh)
		return true;

	return false;
}

csHitBeamResult Elevator::HitBeam(const csVector3 &start, const csVector3 &end)
{
	//passes info onto HitBeam function
	return ElevatorMesh->HitBeam(start, end);
}

bool Elevator::IsInElevator(const csVector3 &position)
{
	if (position.y > GetPosition().y && position.y < GetPosition().y + Height)
	{
		csHitBeamResult result = ElevatorMesh->HitBeam(position, csVector3(position.x, position.y - Height, position.z));
		return result.hit;
	}
	return false;
}

float Elevator::GetElevatorStart()
{
	//returns the internal elevator starting position
	return ElevatorStart;
}

bool Elevator::AreDoorsOpen()
{
	//returns the internal door state
	return DoorsOpen;
}

float Elevator::GetDestination()
{
	//returns the internal destination value
	return Destination;
}

float Elevator::GetStoppingDistance()
{
	//returns the internal stopping distance value
	return StoppingDistance;
}

bool Elevator::GetBrakeStatus()
{
	//returns the internal brake status value
	return Brakes;
}

float Elevator::GetCurrentDoorSpeed()
{
	//returns the internal door speed value
	return ElevatorDoorSpeed;
}

bool Elevator::GetEmergencyStopStatus()
{
	//returns the internal emergency stop status
	return EmergencyStop;
}

void Elevator::DumpServicedFloors()
{
	//dump serviced floors list

	sbs->Report("--- Elevator " + csString(_itoa(Number, intbuffer, 10)) + "'s Serviced Floors ---\n");
	for (size_t i = 0; i < ServicedFloors.GetSize(); i++)
		sbs->Report(csString(_itoa(i, intbuffer, 10)) + " - " + csString(_itoa(ServicedFloors[i], intbuffer, 10)));
}

void Elevator::AddServicedFloor(int number)
{
	if (ServicedFloors.Find(number) == csArrayItemNotFound)
		ServicedFloors.InsertSorted(number);
}

void Elevator::RemoveServicedFloor(int number)
{
	if (ServicedFloors.Find(number) != csArrayItemNotFound)
		ServicedFloors.Delete(number);
}

void Elevator::CreateButtonPanel(const char *texture, int columns, int rows, const char *direction, float CenterX, float CenterZ, float width, float height, float voffset, float spacingX, float spacingY, float tw, float th)
{
	//create a new button panel object and store the pointer
	if (!Panel)
		Panel = new ButtonPanel(Number, texture, rows, columns, direction, CenterX, CenterZ, width, height, voffset, spacingX, spacingY, tw, th);
	else
		sbs->Report("Elevator " + csString(_itoa(Number, intbuffer, 10)) + ": Button panel already exists");
}

void Elevator::UpdateFloorIndicators()
{
	//changes the number texture on the floor indicators to the elevator's current floor

	csString texture = "Button" + sbs->GetFloor(GetFloor())->ID;

	for (int i = 0; i < FloorIndicator_state->GetPolygonCount(); i++)
		sbs->ChangeTexture(FloorIndicator->GetMeshObject(), orig_indicator, texture.GetData());
}
