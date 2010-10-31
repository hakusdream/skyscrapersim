/* $Id$ */

/*
	This code was originally part of Crystal Space
    Copyright (C) 1998-2005 by Jorrit Tyberghein
	Modifications Copyright (C)2010 Ryan Thoryk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <math.h>
#include "globals.h"
#include "sbs.h"
#include "unix.h"

extern SBS *sbs; //external pointer to the SBS engine

#undef EPSILON
#define EPSILON 0.001f
#undef SMALL_EPSILON
#define SMALL_EPSILON 0.000001f

bool MeshObject::ComputeTextureMap(Ogre::Matrix3 &t_matrix, Ogre::Vector3 &t_vector, std::vector<Ogre::Vector3> &vertices, const Ogre::Vector3 &p1, const Ogre::Vector2 &uv1, const Ogre::Vector3 &p2, const Ogre::Vector2 &uv2, const Ogre::Vector3 &p3, const Ogre::Vector2 &uv3)
{
	//this is modified code from the Crystal Space thingmesh system, from the "plugins/mesh/thing/object/polygon.cpp" file.
	//given an array of vertices, this returns the texture transformation matrix and vector

	//original description:
	// Some explanation. We have three points for
	// which we know the uv coordinates. This gives:
	//     P1 -> UV1
	//     P2 -> UV2
	//     P3 -> UV3
	// P1, P2, and P3 are on the same plane so we can write:
	//     P = P1 + lambda * (P2-P1) + mu * (P3-P1)
	// For the same lambda and mu we can write:
	//     UV = UV1 + lambda * (UV2-UV1) + mu * (UV3-UV1)
	// What we want is Po, Pu, and Pv (also on the same
	// plane) so that the following uv coordinates apply:
	//     Po -> 0,0
	//     Pu -> 1,0
	//     Pv -> 0,1
	// The UV equation can be written as follows:
	//     U = U1 + lambda * (U2-U1) + mu * (U3-U1)
	//     V = V1 + lambda * (V2-V1) + mu * (V3-V1)
	// This is a matrix equation (2x2 matrix):
	//     UV = UV1 + M * PL
	// We have UV in this case and we need PL so we
	// need to invert this equation:
	//     (1/M) * (UV - UV1) = PL

	//set up 2D matrix
	float m11, m12, m21, m22;
	m11 = uv2.x - uv1.x;
	m12 = uv3.x - uv1.x;
	m21 = uv2.y - uv1.y;
	m22 = uv3.y - uv1.y;
	
	//compute determinant of matrix
	float det = m11 * m22 - m12 * m21;

	if (abs(det) < SMALL_EPSILON)
	{
		sbs->ReportError("Warning: bad UV coordinates");
		return false;
	}
	else
	{
		//invert matrix
		float inv_det = 1 / (m11 * m22 - m12 * m21);
		float tmp1, tmp2, tmp3, tmp4;
		tmp1 = m11;
		tmp2 = m12;
		tmp3 = m21;
		tmp4 = m22;

		m11 = tmp4 * inv_det;
		m12 = -tmp2 * inv_det;
		m21 = -tmp3 * inv_det;
		m22 = tmp1 * inv_det;
	}

	Ogre::Vector2 pl;
	Ogre::Vector3 po, pu, pv;

	// For (0,0) and Po
	Ogre::Vector2 v = Ogre::Vector2(0, 0) - uv1;
	pl = Ogre::Vector2(m11 * v.x + m12 * v.y, m21 * v.x + m22 * v.y);
	po = p1 + pl.x * (p2 - p1) + pl.y * (p3 - p1);

	// For (1,0) and Pu
	v = Ogre::Vector2(1, 0) - uv1;
	pl = Ogre::Vector2(m11 * v.x + m12 * v.y, m21 * v.x + m22 * v.y);
	pu = p1 + pl.x * (p2 - p1) + pl.y * (p3 - p1);

	// For (0,1) and Pv
	v = Ogre::Vector2(0, 1) - uv1;
	pl = Ogre::Vector2(m11 * v.x + m12 * v.y, m21 * v.x + m22 * v.y);
	pv = p1 + pl.x * (p2 - p1) + pl.y * (p3 - p1);

	//compute norms of vectors
	Ogre::Vector3 len1 = pu - po;
	Ogre::Vector3 len2 = pv - po;
	float len1f = sqrtf(len1.x * len1.x + len1.y * len1.y + len1.z * len1.z);
	float len2f = sqrtf(len2.x * len2.x + len2.y * len2.y + len2.z * len2.z);

	ComputeTextureSpace(t_matrix, t_vector, po, pu, len1f, pv, len2f);
	return true;
}

bool MeshObject::ComputeTextureSpace(Ogre::Matrix3 &m, Ogre::Vector3 &v, const Ogre::Vector3 &v_orig, const Ogre::Vector3 &v1, float len1, const Ogre::Vector3 &v2, float len2)
{
	//originally from Crystal Space's libs/csgeom/textrans.cpp file
	
	float d = v_orig.squaredDistance(v1);
	//get inverse square of d
	float invl1 = 1 / sqrtf(d);

	d = v_orig.squaredDistance(v2);
	//get inverse square of d
	float invl2 = (d) ? 1 / sqrtf(d) : 0;

	Ogre::Vector3 v_u = (v1 - v_orig) * len1 * invl1;
	Ogre::Vector3 v_v = (v2 - v_orig) * len2 * invl2;
	Ogre::Vector3 v_w = v_u % v_v;

	m.GetColumn(1).x = v_u.x;
	m.GetColumn(1).y = v_v.x;
	m.GetColumn(1).z = v_w.x;
	m.GetColumn(2).x = v_u.y;
	m.GetColumn(2).y = v_v.y;
	m.GetColumn(2).z = v_w.y;
	m.GetColumn(3).x = v_u.z;
	m.GetColumn(3).y = v_v.z;
	m.GetColumn(3).z = v_w.z;

	v = v_orig;

	float det = m.Determinant();
	if (abs(det) < SMALL_EPSILON)
	{
		m = m.IDENTITY;
		return false;
	}
	else
		m = m.Inverse();

	return true;
}

int SBS::Classify(int axis, std::vector<Ogre::Vector3> &vertices, float value)
{
	//from Crystal Space libs/csgeom/poly3d.cpp
	//axis is 0 for X, 1 for Y, 2 for Z

	//return codes:
	//0 - polygon is on same plane
	//1 - polygon is in front of plane
	//2 - polygon is in back of plane
	//3 - polygon intersects with plane

	int i;
	int front = 0, back = 0;

	for (i = 0; i < vertices.size(); i++)
	{
		float loc = 0;
		if (axis == 0)
			loc = vertices[i].x - value;
		if (axis == 1)
			loc = vertices[i].y - value;
		if (axis == 2)
			loc = vertices[i].z - value;

		if (loc < -EPSILON)
			front++;
		else if (loc > EPSILON)
			back++;
	}

	if (back == 0 && front == 0)
		return 0; //polygon is on same plane
	if (back == 0)
		return 1; //polygon is in front of plane
	if (front == 0)
		return 2; //polygon is in back of plane
	return 3; //polygon intersects with plane
}

void SBS::SplitWithPlane(int axis, std::vector<Ogre::Vector3> &orig, std::vector<Ogre::Vector3> &poly1, std::vector<Ogre::Vector3> &poly2, float value)
{
	//from Crystal Space libs/csgeom/poly3d.cpp
	//axis is 0 for X, 1 for Y, 2 for Z
	//splits the "orig" polygon on the desired plane into two resulting polygons

	poly1.clear();
	poly2.clear();

	Ogre::Vector3 ptB;
	float sideA, sideB;
	Ogre::Vector3 ptA = orig[orig.size() - 1];

	if (axis == 0)
		sideA = ptA.x - value;
	if (axis == 1)
		sideA = ptA.y - value;
	if (axis == 2)
		sideA = ptA.z - value;

	if (abs(sideA) < SMALL_EPSILON)
		sideA = 0;

	int i;
	for (i = -1; ++i < (int)orig.size();)
	{
		ptB = orig[i];
		if (axis == 0)
			sideB = ptB.x - value;
		if (axis == 1)
			sideB = ptB.y - value;
		if (axis == 2)
			sideB = ptB.z - value;
		
		if (abs(sideB) < SMALL_EPSILON)
			sideB = 0;

		if (sideB > 0)
		{
			if (sideA < 0)
			{
				// Compute the intersection point of the line
				// from point A to point B with the partition
				// plane. This is a simple ray-plane intersection.

				Ogre::Vector3 v = ptB;
				v -= ptA;

				float sect = -(ptA.x - x) / v.x;
				v *= sect;
				v += ptA;
				poly1.push_back(v);
				poly2.push_back(v);
			}

			poly2.push_back(ptB);
		}
		else if (sideB < 0)
		{
			if (sideA > 0)
			{
				// Compute the intersection point of the line
				// from point A to point B with the partition
				// plane. This is a simple ray-plane intersection.
				
				Ogre::Vector3 v = ptB;
				v -= ptA;

				float sect = -(ptA.x - x) / v.x;
				v *= sect;
				v += ptA;
				poly1.push_back(v);
				poly2.push_back(v);
			}

			poly1.push_back(ptB);
		}
		else
		{
			poly1.push_back(ptB);
			poly2.push_back(ptB);
		}

		ptA = ptB;
		sideA = sideB;
	}
}

Ogre::Vector3 SBS::ComputeNormal(std::vector<Ogre::Vector3> &vertices)
{
	//from Crystal Space libs/csgeom/poly3d.cpp
	//calculate polygon normal

	float ayz = 0;
	float azx = 0;
	float axy = 0;
	size_t i, i1;
	float x1, y1, z1, x, y, z;

	i1 = num - 1;
	x1 = vertices[i1].x;
	y1 = vertices[i1].y;
	z1 = vertices[i1].z;
	for (i = 0; i < vertices.size(); i++)
	{
		x = vertices[i].x;
		y = vertices[i].y;
		z = vertices[i].z;
		ayz += (z1 + z) * (y - y1);
		azx += (x1 + x) * (z - z1);
		axy += (y1 + y) * (x - x1);
		x1 = x;
		y1 = y;
		z1 = z;
	}

	float sqd = ayz * ayz + azx * azx + axy * axy;
	float invd;
	if (sqd < SMALL_EPSILON)
		invd = 1.0f / SMALL_EPSILON;
	else
		invd = 1.0f / sqrtf(sqd);
	return Ogre::Vector3(ayz * invd, azx * invd, axy * invd);
}

bool SBS::InPolygon(std::vector<Ogre::Vector3> &poly, const Ogre::Vector3 &v)
{
	//from Crystal Space libs/csgeom/poly3d.cpp
	//determine if the point 'v' is inside the given polygon

	int i, i1;
	i1 = poly.size() - 1;
	for (i = 0; i < poly.size(); i++)
	{
		if (WhichSide3D(v, poly[i1], poly[i]) < 0)
			return false;
		i1 = i;
	}

	return true;
}

int SBS::WhichSide3D(const Ogre::Vector3 &p, const Ogre::Vector3 &v1, const Ogre::Vector3 &v2)
{
	//from Crystal Space include/csgeom/math3d.h

	/**
	   * Tests which side of a plane the given 3D point is on.
	   * \return -1 if point p is left of plane '0-v1-v2',
	   *         1 if point p is right of plane '0-v1-v2',
	   *      or 0 if point p lies on plane '0-v1-v2'.
	   * Plane '0-v1-v2' is the plane passing through points <0,0,0>, v1, and v2.
	   *<p>
	   * Warning: the result of this function when 'p' is exactly on the plane
	   * 0-v1-v2 is undefined. It should return 0 but it will not often do that
	   * due to numerical inaccuracies. So you should probably test for this
	   * case separately.
	   */

	float s = p.x * (v1.y * v2.z - v1.z * v2.y) + p.y * (v1.z * v2.x - v1.x * v2.z) + p.z * (v1.x * v2.y - v1.y * v2.x);
	if (s < 0)
		return 1;
	else if (s > 0)
		return -1;
	else return 0;
}
