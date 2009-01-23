/* $Id$ */

/*
	Skyscraper 1.3 Alpha - Simulation Frontend
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

#ifndef _SKYSCRAPER_H
#define _SKYSCRAPER_H

#include "wx/timer.h"
#include "sbs.h"

int main (int argc, char* argv[]);

class Skyscraper : public wxApp
{
public:
	virtual bool OnInit(void);
	virtual int OnExit(void);
	void Run();

	//file loader functions
	int LoadBuilding(const char * filename);
	int LoadDataFile(const char * filename);

	//File I/O
	csString BuildingFile;
	csArray<csString> BuildingData;

	//frame rate handler class
        class Pump : public wxTimer
	{
	public:
		SBS* s;
		wxApp* app;
		Pump() { };
		virtual void Notify()
		{	
			s->PushFrame();
			#ifndef CS_PLATFORM_WIN32
			        while (app->Pending())
		                app->Dispatch();
			#endif
		}
	};

        //timer object
        Pump* p;
};

DECLARE_APP(Skyscraper)

#endif

