/* $Id$ */

/*
	Scalable Building Simulator - Wall Object
	The Skyscraper Project - Version 1.10 Alpha
	Copyright (C)2004-2016 Ryan Thoryk
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

#ifndef _SBS_WALL_H
#define _SBS_WALL_H

#include "polygon.h"

namespace SBS {

class SBSIMPEXP WallObject : public Object
{
public:

	//functions
	WallObject(MeshObject* wrapper, Object *proxy = 0, bool temporary = false);
	~WallObject();
	Polygon* AddQuad(const std::string &name, const std::string &texture, const Ogre::Vector3 &v1, const Ogre::Vector3 &v2, const Ogre::Vector3 &v3, const Ogre::Vector3 &v4, float tw, float th, bool autosize);
	Polygon* AddPolygon(const std::string &name, const std::string &texture, std::vector<Ogre::Vector3> &vertices, float tw, float th, bool autosize);
	Polygon* AddPolygon(const std::string &name, const std::string &material, std::vector<std::vector<Ogre::Vector3> > &vertices, Ogre::Matrix3 &tex_matrix, Ogre::Vector3 &tex_vector);
	int CreatePolygon(std::vector<Triangle> &triangles, std::vector<Extents> &index_extents, Ogre::Matrix3 &tex_matrix, Ogre::Vector3 &tex_vector, const std::string &material, const std::string &name, Ogre::Plane &plane);
	void DeletePolygons(bool recreate_collider = true);
	void DeletePolygon(int index, bool recreate_colliders);
	int GetPolygonCount();
	Polygon* GetPolygon(int index);
	int FindPolygon(const std::string &name);
	void GetGeometry(int index, std::vector<std::vector<Ogre::Vector3> > &vertices, bool firstonly = false, bool convert = true, bool rescale = true, bool relative = true, bool reverse = false);
	bool IsPointOnWall(const Ogre::Vector3 &point, bool convert = true);
	bool IntersectsWall(const Ogre::Vector3 &start, const Ogre::Vector3 &end, Ogre::Vector3 &isect, bool convert = true);
	void Move(const Ogre::Vector3 &position, float speed = 1.0f);
	MeshObject* GetMesh();
	void SetParentArray(std::vector<WallObject*> &array);
	Ogre::Vector3 GetPoint(const Ogre::Vector3 &start, const Ogre::Vector3 &end);
	Ogre::Vector3 GetWallExtents(float altitude, bool get_max);

private:
	//mesh wrapper
	MeshObject* meshwrapper;

	//polygon array
	std::vector<Polygon> polygons;

	//pointer to parent array
	std::vector<WallObject*> *parent_array;
};

}

#endif
