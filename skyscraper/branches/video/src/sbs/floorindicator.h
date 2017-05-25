/* $Id$ */

/*
	Scalable Building Simulator - Floor Indicator Object
	The Skyscraper Project - Version 1.11 Alpha
	Copyright (C)2004-2017 Ryan Thoryk
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

#ifndef _SBS_FLOORINDICATOR_H
#define _SBS_FLOORINDICATOR_H

namespace SBS {

class SBSIMPEXP FloorIndicator : public Object
{
public:

	int elev; //elevator this indicator is assigned to
	int car; //elevator car this indicator is assigned to
	std::string Prefix; //texture name prefix

	//functions
	FloorIndicator(Object *parent, int elevator, int car, const std::string &texture_prefix, const std::string &direction, Real CenterX, Real CenterZ, Real width, Real height, Real altitude);
	~FloorIndicator();
	void Enabled(bool value);
	void Update();
	bool IsEnabled() { return is_enabled; }

private:
	MeshObject* FloorIndicatorMesh; //indicator mesh object
	bool is_enabled;
};

}

#endif
