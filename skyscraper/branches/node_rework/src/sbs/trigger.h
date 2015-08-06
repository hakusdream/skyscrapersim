/* $Id$ */

/*
	Scalable Building Simulator - Proximity Trigger Class
	The Skyscraper Project - Version 1.10 Alpha
	Copyright (C)2004-2015 Ryan Thoryk
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

#ifndef _SBS_TRIGGER_H
#define _SBS_TRIGGER_H

#include "globals.h"
#include "action.h"

class SBSIMPEXP Trigger : public Object
{
public:
	std::string Name;
	bool IsEnabled;

	//functions
	Trigger(Object *parent, const char *name, bool permanent, const char *sound_file, Ogre::Vector3 &area_min, Ogre::Vector3 &area_max, const std::vector<std::string> &action_names);
	~Trigger();
	void Enabled(bool value);
	bool SetSelectPosition(int position);
	bool ChangeSelectPosition(int position);
	bool NextSelectPosition(bool check_state = true);
	bool PreviousSelectPosition(bool check_state = true);
	int GetSelectPosition();
	const char* GetPositionAction(int position);
	const char* GetSelectPositionAction();
	int GetNextSelectPosition();
	int GetPreviousSelectPosition();
	int GetPositions();
	void PlaySound();
	int FindActionPosition(const char *name);
	bool DoAction();
	bool Check();
	bool IsInside();

private:
	Ogre::Vector3 area_min;
	Ogre::Vector3 area_max;
	int current_position; //current trigger position
	bool is_inside;
	std::vector<std::string> Actions; //trigger actions

	Sound *sound; //sound object

};

#endif
