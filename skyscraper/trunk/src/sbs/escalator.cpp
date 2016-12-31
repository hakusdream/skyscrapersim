/* $Id$ */

/*
	Scalable Building Simulator - Escalator Object
	The Skyscraper Project - Version 1.11 Alpha
	Copyright (C)2004-2016 Ryan Thoryk
	http://www.skyscrapersim.com
	http://sourceforge.net/projects/skyscraper
	Contact - ryan@skyscrapersim.com

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
#include "floor.h"
#include "sound.h"
#include "texture.h"
#include "profiler.h"
#include "dynamicmesh.h"
#include "step.h"
#include "escalator.h"

namespace SBS {

Escalator::Escalator(Object *parent, const std::string &name, int run, float speed, const std::string &sound_file, const std::string &texture, const std::string &direction, float CenterX, float CenterZ, float width, float risersize, float treadsize, int num_steps, float voffset, float tw, float th) : Object(parent)
{
	//create a new escalator object
	//run is either 1 for forward motion, -1 for reverse motion, 0 for stop
	//direction is where the step base is - front, back, left, or right.

	//set up SBS object
	SetValues("Escalator", name, false);

	is_enabled = true;
	Run = run;
	Speed = speed;
	sbs->IncrementEscalatorCount();
	start = 0;
	end = 0;

	StepContainer = new DynamicMesh(this, GetSceneNode(), name + " Step Container", 0, true);

	//move object
	Move(CenterX, voffset, CenterZ);

	//create step meshes
	for (int i = 0; i < num_steps; i++)
	{
		Step *mesh = new Step(this, "Step " + ToString(i + 1), StepContainer);
		Steps.push_back(mesh);
	}

	//create sound object
	sound = new Sound(this, name, true);
	sound->Load(sound_file);

	//create steps
	CreateSteps(texture, direction, width, risersize, treadsize, tw, th);
}

Escalator::~Escalator()
{
	if (sound)
	{
		sound->parent_deleting = true;
		delete sound;
	}
	sound = 0;

	//remove step meshes
	for (size_t i = 0; i < Steps.size(); i++)
	{
		if (Steps[i])
		{
			Steps[i]->parent_deleting = true;
			delete Steps[i];
		}
		Steps[i] = 0;
	}

	if (StepContainer)
		delete StepContainer;
	StepContainer = 0;

	//unregister from parent
	if (sbs->FastDelete == false)
	{
		sbs->DecrementEscalatorCount();

		//unregister from parent
		if (parent_deleting == false)
		{
			std::string type = GetParent()->GetType();

			if (type == "Floor")
				static_cast<Floor*>(GetParent())->RemoveEscalator(this);
		}
	}
}

void Escalator::Enabled(bool value)
{
	//enable or disable escalator

	if (is_enabled == value)
		return;

	EnableLoop(value);

	for (size_t i = 0; i < Steps.size(); i++)
		Steps[i]->Enabled(value);

	if (value == false && sound->IsPlaying() == true)
		sound->Stop();

	is_enabled = value;
}

void Escalator::Report(const std::string &message)
{
	//general reporting function
	sbs->Report("Escalator " + GetName() + ": " + message);
}

bool Escalator::ReportError(const std::string &message)
{
	//general reporting function
	return sbs->ReportError("Escalator " + GetName() + ": " + message);
}

void Escalator::Loop()
{
	//run loop

	SBS_PROFILE("Escalator::Loop");

	if (!IsEnabled() || Run == 0)
	{
		if (sound->IsPlaying() == true)
			sound->Stop();
		return;
	}

	if (sound->IsPlaying() == false)
	{
		sound->SetLoopState(true);
		sound->Play();
	}

	MoveSteps();
}

void Escalator::CreateSteps(const std::string &texture, const std::string &direction, float width, float risersize, float treadsize, float tw, float th)
{
	//create steps
	std::string Name = GetName();
	TrimString(Name);
	Direction = direction;
	this->treadsize = treadsize;
	this->risersize = risersize;
	SetCase(Direction, false);
	int num_steps = (int)Steps.size();

	sbs->GetTextureManager()->ResetTextureMapping(true);
	if (Direction == "right" || Direction == "back")
		sbs->SetWallOrientation("right");
	if (Direction == "left" || Direction == "front")
		sbs->SetWallOrientation("left");

	for (int i = 1; i <= num_steps; i++)
	{
		float pos = 0;
		std::string base = Name + ":" + ToString(i);

		//create wall object
		Wall *wall = Steps[i - 1]->CreateWallObject(base);

		float thickness = treadsize;

		if (Direction == "right")
		{
			pos = ((treadsize * num_steps + 1) / 2) - (treadsize * i);
			sbs->DrawWalls(true, true, true, true, false, true);
			sbs->AddWallMain(wall, base + "-riser", texture, thickness, treadsize, -(width / 2), treadsize, width / 2, risersize, risersize, 0, 0, tw, th, true);

			sbs->DrawWalls(false, true, false, false, false, false);
			sbs->AddFloorMain(wall, base + "-tread", texture, 0, 0, -(width / 2), treadsize, width / 2, risersize, risersize, false, false, tw, th, true);

			if (i < 3)
				Steps[i - 1]->Move(Ogre::Vector3(pos, -risersize, 0));
			else if (i < num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(pos, risersize * (i - 4), 0));
			else if (i == num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(pos, risersize * (i - 5), 0));

			if (i == 1)
				start = Steps[i - 1]->GetPosition();
			if (i == num_steps)
				end = Steps[i - 1]->GetPosition();
			Steps[i - 1]->start = Steps[i - 1]->GetPosition();
		}
		if (Direction == "left")
		{
			pos = -((treadsize * num_steps + 1) / 2) + (treadsize * i);
			sbs->DrawWalls(true, true, true, true, false, true);
			sbs->AddWallMain(wall, base + "-riser", texture, thickness, -treadsize, width / 2, -treadsize, -(width / 2), risersize, risersize, 0, 0, tw, th, true);

			sbs->DrawWalls(false, true, false, false, false, false);
			sbs->AddFloorMain(wall, base + "-tread", texture, 0, -treadsize, -(width / 2), 0, width / 2, risersize, risersize, false, false, tw, th, true);

			if (i < 3)
				Steps[i - 1]->Move(Ogre::Vector3(pos, -risersize, 0));
			else if (i < num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(pos, risersize * (i - 4), 0));
			else if (i == num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(pos, risersize * (i - 5), 0));

			if (i == 1)
				start = Steps[i - 1]->GetPosition();
			if (i == num_steps)
				end = Steps[i - 1]->GetPosition();
			Steps[i - 1]->start = Steps[i - 1]->GetPosition();
		}
		if (Direction == "back")
		{
			pos = ((treadsize * num_steps + 1) / 2) - (treadsize * i);
			sbs->DrawWalls(true, true, true, true, false, true);
			sbs->AddWallMain(wall, base + "-riser", texture, thickness, width / 2, treadsize, -(width / 2), treadsize, risersize, risersize, 0, 0, tw, th, true);

			sbs->DrawWalls(false, true, false, false, false, false);
			sbs->AddFloorMain(wall, base + "-tread", texture, 0, -(width / 2), 0, width / 2, treadsize, risersize, risersize, false, false, tw, th, true);

			if (i < 3)
				Steps[i - 1]->Move(Ogre::Vector3(0, -risersize, pos));
			else if (i < num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(0, risersize * (i - 4), pos));
			else if (i == num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(0, risersize * (i - 5), pos));

			if (i == 1)
				start = Steps[i - 1]->GetPosition();
			if (i == num_steps)
				end = Steps[i - 1]->GetPosition();
			Steps[i - 1]->start = Steps[i - 1]->GetPosition();
		}
		if (Direction == "front")
		{
			pos = -((treadsize * num_steps + 1) / 2) + (treadsize * i);
			sbs->DrawWalls(true, true, true, true, false, true);
			sbs->AddWallMain(wall, base + "-riser", texture, thickness, -(width / 2), -treadsize, width / 2, -treadsize, risersize, risersize, 0, 0, tw, th, true);

			sbs->DrawWalls(false, true, false, false, false, false);
			sbs->AddFloorMain(wall, base + "-tread", texture, 0, -(width / 2), -treadsize, width / 2, 0, risersize, risersize, false, false, tw, th, true);

			if (i < 3)
				Steps[i - 1]->Move(Ogre::Vector3(0, -risersize, pos));
			else if (i < num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(0, risersize * (i - 4), pos));
			else if (i == num_steps)
				Steps[i - 1]->Move(Ogre::Vector3(0, risersize * (i - 5), pos));

			if (i == 1)
				start = Steps[i - 1]->GetPosition();
			if (i == num_steps)
				end = Steps[i - 1]->GetPosition();
			Steps[i - 1]->start = Steps[i - 1]->GetPosition();
		}
	}
	sbs->ResetWalls(true);
	sbs->GetTextureManager()->ResetTextureMapping();
}

void Escalator::MoveSteps()
{
	for (size_t i = 0; i < Steps.size(); i++)
	{
		if (Run == 1)
		{
			if (Direction == "right")
			{
				float pos = Steps[i]->GetPosition().x;
				if (pos < end.x - treadsize)
					Steps[i]->SetPosition(start);
				else if (pos >= start.x - (treadsize * 2) || pos <= end.x + treadsize)
					Steps[i]->Move(Ogre::Vector3(-Run, 0, 0), Speed * sbs->delta);
				else if (pos > end.x + treadsize)
					Steps[i]->Move(Ogre::Vector3(-Run, Run * (risersize / treadsize), 0), Speed * sbs->delta);
			}
			if (Direction == "left")
			{
				float pos = Steps[i]->GetPosition().x;
				if (pos > end.x + treadsize)
					Steps[i]->SetPosition(start);
				else if (pos <= start.x + (treadsize * 2) || pos >= end.x - treadsize)
					Steps[i]->Move(Ogre::Vector3(Run, 0, 0), Speed * sbs->delta);
				else if (pos < end.x - treadsize)
					Steps[i]->Move(Ogre::Vector3(Run, Run * (risersize / treadsize), 0), Speed * sbs->delta);
			}
			if (Direction == "back")
			{
				float pos = Steps[i]->GetPosition().z;
				if (pos < end.z - treadsize)
					Steps[i]->SetPosition(start);
				else if (pos >= start.z - (treadsize * 2) || pos <= end.z + treadsize)
					Steps[i]->Move(Ogre::Vector3(0, 0, -Run), Speed * sbs->delta);
				else if (pos > end.z + treadsize)
					Steps[i]->Move(Ogre::Vector3(0, Run * (risersize / treadsize), -Run), Speed * sbs->delta);
			}
			if (Direction == "front")
			{
				float pos = Steps[i]->GetPosition().z;
				if (pos > end.z + treadsize)
					Steps[i]->SetPosition(start);
				else if (pos <= start.z + (treadsize * 2) || pos >= end.z - treadsize)
					Steps[i]->Move(Ogre::Vector3(0, 0, Run), Speed * sbs->delta);
				else if (pos < end.z - treadsize)
					Steps[i]->Move(Ogre::Vector3(0, Run * (risersize / treadsize), Run), Speed * sbs->delta);
			}
		}
		else if (Run == -1)
		{
			if (Direction == "right")
			{
				float pos = Steps[i]->GetPosition().x;
				if (pos > start.x)
					Steps[i]->SetPosition(Ogre::Vector3(end.x - treadsize, end.y, end.z));
				else if (pos <= end.x + treadsize || pos >= start.x - (treadsize * 2))
					Steps[i]->Move(Ogre::Vector3(-Run, 0, 0), Speed * sbs->delta);
				else if (pos < start.x - treadsize)
					Steps[i]->Move(Ogre::Vector3(-Run, Run * (risersize / treadsize), 0), Speed * sbs->delta);
			}
			if (Direction == "left")
			{
				float pos = Steps[i]->GetPosition().x;
				if (pos < start.x)
					Steps[i]->SetPosition(Ogre::Vector3(end.x + treadsize, end.y, end.z));
				else if (pos >= end.x - treadsize || pos <= start.x + (treadsize * 2))
					Steps[i]->Move(Ogre::Vector3(Run, 0, 0), Speed * sbs->delta);
				else if (pos > start.x + treadsize)
					Steps[i]->Move(Ogre::Vector3(Run, Run * (risersize / treadsize), 0), Speed * sbs->delta);
			}
			if (Direction == "back")
			{
				float pos = Steps[i]->GetPosition().z;
				if (pos > start.z)
					Steps[i]->SetPosition(Ogre::Vector3(end.x, end.y, end.z - treadsize));
				else if (pos <= end.z + treadsize || pos >= start.z - (treadsize * 2))
					Steps[i]->Move(Ogre::Vector3(0, 0, -Run), Speed * sbs->delta);
				else if (pos < start.z - treadsize)
					Steps[i]->Move(Ogre::Vector3(0, Run * (risersize / treadsize), -Run), Speed * sbs->delta);
			}
			if (Direction == "front")
			{
				float pos = Steps[i]->GetPosition().z;
				if (pos < start.z)
					Steps[i]->SetPosition(Ogre::Vector3(end.x, end.y, end.z + treadsize));
				else if (pos >= end.z - treadsize || pos <= start.z + (treadsize * 2))
					Steps[i]->Move(Ogre::Vector3(0, 0, Run), Speed * sbs->delta);
				else if (pos > start.z + treadsize)
					Steps[i]->Move(Ogre::Vector3(0, Run * (risersize / treadsize), Run), Speed * sbs->delta);
			}
		}
	}
}

void Escalator::OnClick(Ogre::Vector3 &position, bool shift, bool ctrl, bool alt, bool right)
{
	//cycle run stages if shift-click is performed

	if (shift == true)
	{
		if (Run == 1)
		{
			Run = 0;
			for (size_t i = 0; i < Steps.size(); i++)
			{
				Steps[i]->vector = 0;
				Steps[i]->speed = 0;
			}
		}
		else if (Run == 0)
			Run = -1;
		else if (Run == -1)
			Run = 1;
	}
}

void Escalator::ResetState()
{
	//reset escalator state

	Run = 0;
	for (size_t i = 0; i < Steps.size(); i++)
	{
		Steps[i]->SetPosition(Steps[i]->start);
	}
}

}
