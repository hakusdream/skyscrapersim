/* $Id$ */

/*
	Scalable Building Simulator - Sound System
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

#ifndef _SBS_SOUNDSYSTEM_H
#define _SBS_SOUNDSYSTEM_H

#include <fmod.hpp>

namespace SBS {

class SBSIMPEXP SoundSystem : public Object
{
public:

	struct SBSIMPEXP SoundData
	{
		SoundData();
		~SoundData();
		void RemoveChannel(FMOD::Channel* channel);
		FMOD::Sound* sound; //sound data object
		std::string filename; //filename of sound file
		std::vector<FMOD::Channel*> channels; //associated channels
	};

	SoundSystem(FMOD::System *fmodsystem);
	~SoundSystem();
	void SetListenerPosition(const Ogre::Vector3 &position);
	void SetListenerDirection(const Ogre::Vector3 &front, const Ogre::Vector3 &top);
	void Loop();
	FMOD::System *GetFmodSystem() { return soundsys; } //temporary for transition
	void Cleanup();
	unsigned int GetLength(SoundData *sound);
	SoundData* Load(const std::string &filename);
	bool IsLoaded(std::string filename);
	void Report(const std::string &message);
	bool ReportError(const std::string &message);
	FMOD::Channel* Prepare(SoundData *sound);
	SoundData* GetSoundData(std::string filename);

private:

	//FMOD system
	FMOD::System *soundsys;

	//listener sound objects
	FMOD_VECTOR listener_position;
	FMOD_VECTOR listener_velocity;
	FMOD_VECTOR listener_forward;
	FMOD_VECTOR listener_up;

	//sound array
	std::vector<SoundData*> sounds;
};

}

#endif
