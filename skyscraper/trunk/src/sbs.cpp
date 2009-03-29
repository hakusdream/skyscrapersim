/* $Id$ */

/*
	Scalable Building Simulator - Core
	The Skyscraper Project - Version 1.3 Alpha
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

#ifdef _WIN32
	#define CS_IMPLEMENT_PLATFORM_APPLICATION
	#define CS_NO_MALLOC_OVERRIDE
#endif

#include <wx/wx.h>
#include <wx/variant.h>
#include "globals.h"
#include "sbs.h"
#include "unix.h"

#ifdef _WIN32
	CS_IMPLEMENT_FOREIGN_DLL
#endif

SBS *sbs; //self reference

SBS::SBS()
{
	sbs = this;

	//Print SBS banner
	PrintBanner();

	//Pause for 2 seconds
	csSleep(2000);

	//Set default horizontal scaling value
	HorizScale = 1;

	//Set default starting elevator
	ElevatorNumber = 1;

	//Set default frame rate
	FrameRate = 30;

	//initialize other variables
	BuildingName = "";
	BuildingDesigner = "";
	BuildingLocation = "";
	BuildingDescription = "";
	BuildingVersion = "";
	SkyName = "noon";
	IsRunning = false;
	Floors = 0;
	Basements = 0;
	RenderOnly = false;
	InputOnly = false;
	IsFalling = false;
	InStairwell = false;
	InElevator = false;
	IsBuildingsEnabled = false;
	IsExternalEnabled = false;
	IsLandscapeEnabled = false;
	IsSkyboxEnabled = false;
	fps_frame_count = 0;
	fps_tottime = 0;
	FPS = 0;
	FrameLimiter = false;
	AutoShafts = true;
	AutoStairs = true;
	ElevatorSync = false;
	mouse_x = 0;
	mouse_y = 0;
	wall_orientation = 1;
	floor_orientation = 2;
	DrawMainN = true;
	DrawMainP = true;
	DrawSideN = false;
	DrawSideP = false;
	DrawTop = false;
	DrawBottom = false;
	DrawMainNOld = true;
	DrawMainPOld = true;
	DrawSideNOld = false;
	DrawSidePOld = false;
	DrawTopOld = false;
	DrawBottomOld = false;
	RevX = false;
	RevY = false;
	RevZ = false;
	RevXold = false;
	RevYold = false;
	RevZold = false;
	remaining_delta = 0;
	delta = 0.01f;
	ShowFullShafts = false;
	ShowFullStairs = false;
	ShaftDisplayRange = 3;
	StairsDisplayRange = 5;
	ShaftOutsideDisplayRange = 3;
	StairsOutsideDisplayRange = 3;
	FloorDisplayRange = 3;
	wall1a = false;
	wall1b = false;
	wall2a = false;
	wall2b = false;
	AutoX = true;
	AutoY = true;
	ReverseAxisValue = false;
	TextureOverride = false;
	ProcessElevators = true;
	callbackdoor = 0;
	FlipTexture = false;
	mainnegflip = 0;
	mainposflip = 0;
	sidenegflip = 0;
	sideposflip = 0;
	topflip = 0;
	bottomflip = 0;
	widthscale.SetSize(6);
	heightscale.SetSize(6);
}

SBS::~SBS()
{
	//engine destructor

	//delete camera object
	if (camera)
		delete camera;
	camera = 0;

	//delete floors
	FloorArray.DeleteAll();

	//delete elevators
	ElevatorArray.DeleteAll();

	//delete shafts
	ShaftArray.DeleteAll();

	//delete stairs
	StairsArray.DeleteAll();

	//clear self reference
	sbs = 0;
}

void SBS::Start()
{
	//Post-init startup code goes here, before the runloop

	//initialize mesh colliders
	csColliderHelper::InitializeCollisionWrappers (collision_sys, engine);

	//initialize camera/actor
	camera->CreateColliders();

	//move camera to start location
	camera->SetToStartPosition();
	camera->SetToStartDirection();
	camera->SetToStartRotation();

	//set sound listener object to initial position
	SetListenerLocation(camera->GetPosition());

	//turn on main objects
	EnableBuildings(true);
	EnableLandscape(true);
	EnableExternal(true);
	EnableSkybox(true);

	//turn off floors
	for (int i = 0; i < TotalFloors(); i++)
		FloorArray[i].object->Enabled(false);

	//turn off shafts
	for (int i = 0; i < Shafts(); i++)
	{
		if (ShaftArray[i].object)
		{
			ShaftArray[i].object->EnableWholeShaft(false, true);
			//enable extents
			ShaftArray[i].object->Enabled(ShaftArray[i].object->startfloor, true, true);
			ShaftArray[i].object->Enabled(ShaftArray[i].object->endfloor, true, true);
		}
	}

	//turn off stairwells
	for (int i = 0; i < StairsNum(); i++)
	{
		if (StairsArray[i].object)
			StairsArray[i].object->EnableWholeStairwell(false);
	}

	//turn on shaft elevator doors
	for (int i = 0; i < Elevators(); i++)
	{
		if (ElevatorArray[i].object)
		{
			ElevatorArray[i].object->ShaftDoorsEnabled(camera->StartFloor, true);
			ElevatorArray[i].object->ShaftDoorsEnabled(GetShaft(ElevatorArray[i].object->AssignedShaft)->startfloor, true);
			ElevatorArray[i].object->ShaftDoorsEnabled(GetShaft(ElevatorArray[i].object->AssignedShaft)->endfloor, true);
		}
	}

	//turn on start floor
	GetFloor(camera->StartFloor)->Enabled(true);
	GetFloor(camera->StartFloor)->EnableGroup(true);

}

float SBS::AutoSize(float n1, float n2, bool iswidth, float offset)
{
	//Texture autosizing formulas

	if (offset == 0)
		offset = 1;

	if (iswidth == true)
	{
		if (AutoX == true)
			return fabs(n1 - n2) * offset;
		else
			return offset;
	}
	else
	{
		if (AutoY == true)
			return fabs(n1 - n2) * offset;
		else
			return offset;
	}
}

void SBS::PrintBanner()
{
	csPrintf("\n Scalable Building Simulator 0.3 Alpha\n");
	csPrintf(" Copyright (C)2004-2009 Ryan Thoryk\n");
	csPrintf(" This software comes with ABSOLUTELY NO WARRANTY. This is free\n");
	csPrintf(" software, and you are welcome to redistribute it under certain\n");
	csPrintf(" conditions. For details, see the file gpl.txt\n\n");
}

void SBS::MainLoop()
{
	//Main simulator loop

	//This makes sure all timer steps are the same size, in order to prevent the physics from changing
	//depending on frame rate
	float elapsed = remaining_delta + (vc->GetElapsedTicks() / 1000.0);
	//limit the elapsed value to prevent major slowdowns during debugging
	if (elapsed > 0.5)
		elapsed = 0.5;
	while (elapsed >= delta)
	{
		if (RenderOnly == false && InputOnly == false)
		{
			//Determine floor that the camera is on
			camera->UpdateCameraFloor();

			//run elevator handlers
			if (ProcessElevators == true)
			{
				for (int i = 1; i <= Elevators(); i++)
					GetElevator(i)->MonitorLoop();

				//check if the user is in an elevator
				camera->CheckElevator();
			}

			//check if the user is in a shaft
			if (AutoShafts == true)
				camera->CheckShaft();

			//check if the user is in a stairwell
			if (AutoStairs == true)
				camera->CheckStairwell();

			//open/close doors by using door callback
			if (callbackdoor)
			{
				if (callbackdoor->IsMoving == true)
					callbackdoor->MoveDoor();
				else
					UnregisterDoorCallback(callbackdoor);
			}

			//check if the user is outside

		}

		elapsed -= delta;
	}
	remaining_delta = elapsed;
}

void SBS::CalculateFrameRate()
{
	// First get elapsed time from the virtual clock.
	elapsed_time = vc->GetElapsedTicks ();

	//calculate frame rate
	fps_tottime += elapsed_time;
	fps_frame_count++;
	if (fps_tottime > 500)
	{
		FPS = (float (fps_frame_count) * 1000.0) / float (fps_tottime);
		fps_frame_count = 0;
		fps_tottime = 0;
	}
}

void SBS::Initialize(iSCF* scf, iEngine* engineref, iLoader* loaderref, iVirtualClock* vcref, iView* viewref, iVFS* vfsref,
					 iCollideSystem* collideref, iReporter* reporterref, iSndSysRenderer* sndrenderref, iSndSysLoader* sndloaderref,
					 iMaterialWrapper* matref, iSector* sectorref, const char* rootdirectory, const char* directory_char)
{
	//initialize CS references
#ifdef _WIN32
	iSCF::SCF = scf;
#endif
	engine = engineref;
	loader = loaderref;
	vc = vcref;
	view = viewref;
	vfs = vfsref;
	collision_sys = collideref;
	rep = reporterref;
	sndrenderer = sndrenderref;
	sndloader = sndloaderref;
	material = matref;
	area = sectorref;
	root_dir = rootdirectory;
	dir_char = directory_char;

	//mount sign texture packs
	vfs->Mount("/root/signs/sans", root_dir + "data" + dir_char + "signs-sans.zip");
	vfs->Mount("/root/signs/sans_bold", root_dir + "data" + dir_char + "signs-sans_bold.zip");
	vfs->Mount("/root/signs/sans_cond", root_dir + "data" + dir_char + "signs-sans_cond.zip");
	vfs->Mount("/root/signs/sans_cond_bold", root_dir + "data" + dir_char + "signs-sans_cond_bold.zip");

	//load default textures
	csPrintf("Loading default textures...");
	LoadTexture("/root/data/brick1.jpg", "Default", 1, 1);
	LoadTexture("/root/data/gray2-sm.jpg", "ConnectionWall", 1, 1);
	LoadTexture("/root/data/metal1-sm.jpg", "Connection", 1, 1);
	csPrintf("Done\n");

	//create camera object
	camera = new Camera();
}

bool SBS::LoadTexture(const char *filename, const char *name, float widthmult, float heightmult)
{
	// Load the texture from the standard library.  This is located in
	// CS/data/standard.zip and mounted as /lib/std using the Virtual
	// File System (VFS) plugin.
	if (!loader->LoadTexture (name, filename))
	{
		ReportError("Error loading texture");
		return false;
	}
	TextureInfo info;
	info.name = name;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	textureinfo.Push(info);
	return true;
}

void SBS::AddLight(const char *name, float x, float y, float z, float radius, float r, float g, float b)
{
	csRef<iLightList> ll = area->GetLights();
	csRef<iLight> light = engine->CreateLight(name, csVector3(x, y, z), radius, csColor(r, g, b));
	ll->Add(light);
}

int SBS::AddWallMain(csRef<iThingFactoryState> dest, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float altitude1, float altitude2, float tw, float th)
{
	//determine axis of wall
	int axis = 0;
	if (fabs(x1 - x2) > fabs(z1 - z2))
		//x axis
		axis = 1;
	else
		//z axis
		axis = 2;

	//convert to clockwise coordinates (x-axis wall test)
	if (x1 > x2 && axis == 1)
	{
		//wall's along the x axis and coordinates are counterclockwise; reverse coordinates (to make clockwise)
		float temp = x1;
		x1 = x2;
		x2 = temp;
		temp = z1;
		z1 = z2;
		z2 = temp;
		temp = altitude1;
		altitude1 = altitude2;
		altitude2 = temp;
		temp = height_in1;
		height_in1 = height_in2;
		height_in2 = temp;
	}

	//make sure coordinates are counterclockwise if wall is on z axis
	if (axis == 2)
	{
		//only perform if coordinates are clockwise; otherwise ignore
		if (z1 < z2)
		{
			//wall's along the z axis; reverse coordinates (to make counterclockwise)
			float temp = x1;
			x1 = x2;
			x2 = temp;
			temp = z1;
			z1 = z2;
			z2 = temp;
			temp = altitude1;
			altitude1 = altitude2;
			altitude2 = temp;
			temp = height_in1;
			height_in1 = height_in2;
			height_in2 = temp;
		}
	}

	//Adds a wall with the specified dimensions
	csVector3 v1 (x1, altitude1 + height_in1, z1); //left top
	csVector3 v2 (x2, altitude2 + height_in2, z2); //right top
	csVector3 v3 (x2, altitude2, z2); //right base
	csVector3 v4 (x1, altitude1, z1); //left base

	csVector3 v5 = v1;
	csVector3 v6 = v2;
	csVector3 v7 = v3;
	csVector3 v8 = v4;

	//expand to specified thickness
	if (axis == 1)
	{
		//x axis
		if (wall_orientation == 0)
		{
			//left
			v5.z += thickness;
			v6.z += thickness;
			v7.z += thickness;
			v8.z += thickness;
		}
		if (wall_orientation == 1)
		{
			//center
			v1.z -= thickness / 2;
			v2.z -= thickness / 2;
			v3.z -= thickness / 2;
			v4.z -= thickness / 2;
			v5.z += thickness / 2;
			v6.z += thickness / 2;
			v7.z += thickness / 2;
			v8.z += thickness / 2;
		}
		if (wall_orientation == 2)
		{
			//right
			v1.z -= thickness;
			v2.z -= thickness;
			v3.z -= thickness;
			v4.z -= thickness;
		}
	}
	else
	{
		//z axis
		if (wall_orientation == 0)
		{
			//left
			v5.x += thickness;
            v6.x += thickness;
            v7.x += thickness;
            v8.x += thickness;
		}
		if (wall_orientation == 1)
		{
			//center
			v1.x -= thickness / 2;
            v2.x -= thickness / 2;
            v3.x -= thickness / 2;
            v4.x -= thickness / 2;
            v5.x += thickness / 2;
            v6.x += thickness / 2;
            v7.x += thickness / 2;
            v8.x += thickness / 2;
		}
		if (wall_orientation == 2)
		{
			//right
			v1.x -= thickness;
            v2.x -= thickness;
            v3.x -= thickness;
            v4.x -= thickness;
		}
	}

	//create polygons and set names
	int index = -1;
	int tmpindex = -1;
	csString NewName;

	if (DrawMainN == true)
	{
		tmpindex = dest->AddQuad(v1, v2, v3, v4); //front wall
		NewName = name;
		if (GetDrawWallsCount() > 1)
			NewName.Append(":front");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawMainP == true)
	{
		tmpindex = dest->AddQuad(v6, v5, v8, v7); //back wall
		NewName = name;
		NewName.Append(":back");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawSideN == true)
	{
		tmpindex = dest->AddQuad(v5, v1, v4, v8); //left wall
		NewName = name;
		NewName.Append(":left");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawSideP == true)
	{
		tmpindex = dest->AddQuad(v2, v6, v7, v3); //right wall
		NewName = name;
		NewName.Append(":right");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawTop == true)
	{
		tmpindex = dest->AddQuad(v5, v6, v2, v1); //top wall
		NewName = name;
		NewName.Append(":top");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawBottom == true)
	{
		tmpindex = dest->AddQuad(v4, v3, v7, v8); //bottom wall
		NewName = name;
		NewName.Append(":bottom");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	//reverse vector portions if specified
	if (RevX == true)
	{
		float tmpx1, tmpx2;
		tmpx1 = v1.x;
		tmpx2 = v2.x;
		v1.x = tmpx2;
		v2.x = tmpx1;
		v3.x = tmpx1;
		v4.x = tmpx2;
	}
	if (RevY == true)
	{
		v1.y = altitude1;
		v2.y = altitude2;
		v3.y = altitude2 + height_in2;
		v4.y = altitude1 + height_in1;
	}
	if (RevZ == true)
	{
		float tmpz1, tmpz2;
		tmpz1 = v1.z;
		tmpz2 = v2.z;
		v1.z = tmpz2;
		v2.z = tmpz1;
		v3.z = tmpz1;
		v4.z = tmpz2;
	}

	//set texture
	if (TextureOverride == false && FlipTexture == false)
		SetTexture(dest, index, texture, true, tw, th);
	else
	{
		ProcessTextureFlip(tw, th);
		int endindex = index + GetDrawWallsCount();
		if (FlipTexture == false)
		{
			for (int i = index; i < endindex; i++)
			{
				if (i - index == 0)
					SetTexture(dest, i, mainnegtex.GetData(), false, widthscale[0], heightscale[0]);
				if (i - index == 1)
					SetTexture(dest, i, mainpostex.GetData(), false, widthscale[1], heightscale[1]);
				if (i - index == 2)
					SetTexture(dest, i, sidenegtex.GetData(), false, widthscale[2], heightscale[2]);
				if (i - index == 3)
					SetTexture(dest, i, sidepostex.GetData(), false, widthscale[3], heightscale[3]);
				if (i - index == 4)
					SetTexture(dest, i, toptex.GetData(), false, widthscale[4], heightscale[4]);
				if (i - index == 5)
					SetTexture(dest, i, bottomtex.GetData(), false, widthscale[5], heightscale[5]);
			}
		}
		else
		{
			for (int i = index; i < endindex; i++)
				SetTexture(dest, i, texture, false, widthscale[i - index], heightscale[i - index]);
		}
	}

	return index;
}

int SBS::AddFloorMain(csRef<iThingFactoryState> dest, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float altitude1, float altitude2, float tw, float th)
{
	//Adds a floor with the specified dimensions and vertical offset

	//convert to clockwise coordinates
	float temp;
	if (x1 > x2 && fabs(x1 - x2) > fabs(z1 - z2))
	{
		//reverse coordinates if the difference between x coordinates is greater
		temp = x1;
		x1 = x2;
		x2 = temp;
		temp = z1;
		z1 = z2;
		z2 = temp;
		temp = altitude1;
		altitude1 = altitude2;
		altitude2 = temp;
	}
	if (z1 > z2 && fabs(z1 - z2) > fabs(x1 - x2))
	{
		//reverse coordinates if the difference between z coordinates is greater
		temp = x1;
		x1 = x2;
		x2 = temp;
		temp = z1;
		z1 = z2;
		z2 = temp;
		temp = altitude1;
		altitude1 = altitude2;
		altitude2 = temp;
	}

	csVector3 v1, v2, v3, v4;

	if (ReverseAxisValue == false)
	{
		v1.Set(x1, altitude1, z1); //bottom left
		v2.Set(x2, altitude1, z1); //bottom right
		v3.Set(x2, altitude2, z2); //top right
		v4.Set(x1, altitude2, z2); //top left
	}
	else
	{
		v1.Set(x1, altitude1, z1); //bottom left
		v2.Set(x1, altitude1, z2); //bottom right
		v3.Set(x2, altitude2, z2); //top right
		v4.Set(x2, altitude2, z1); //top left
	}

	csVector3 v5 = v1;
	csVector3 v6 = v2;
	csVector3 v7 = v3;
	csVector3 v8 = v4;

	//expand to specified thickness
	if (floor_orientation == 0)
	{
		//bottom
		v5.y += thickness;
		v6.y += thickness;
		v7.y += thickness;
		v8.y += thickness;
	}
	if (floor_orientation == 1)
	{
		//center
		v1.y -= thickness / 2;
		v2.y -= thickness / 2;
		v3.y -= thickness / 2;
		v4.y -= thickness / 2;
		v5.y += thickness / 2;
		v6.y += thickness / 2;
		v7.y += thickness / 2;
		v8.y += thickness / 2;
	}
	if (floor_orientation == 2)
	{
		//top
		v1.y -= thickness;
		v2.y -= thickness;
		v3.y -= thickness;
		v4.y -= thickness;
	}

	//create polygons and set names
	int index = -1;
	int tmpindex = -1;
	csString NewName;

	if (DrawMainN == true)
	{
		tmpindex = dest->AddQuad(v1, v2, v3, v4); //bottom wall
		NewName = name;
		if (GetDrawWallsCount() > 1)
			NewName.Append(":front");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawMainP == true)
	{
		tmpindex = dest->AddQuad(v8, v7, v6, v5); //top wall
		NewName = name;
		NewName.Append(":back");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawSideN == true)
	{
		tmpindex = dest->AddQuad(v8, v5, v1, v4); //left wall
		NewName = name;
		NewName.Append(":left");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawSideP == true)
	{
		tmpindex = dest->AddQuad(v6, v7, v3, v2); //right wall
		NewName = name;
		NewName.Append(":right");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawTop == true)
	{
		tmpindex = dest->AddQuad(v5, v6, v2, v1); //front wall
		NewName = name;
		NewName.Append(":top");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawBottom == true)
	{
		tmpindex = dest->AddQuad(v7, v8, v4, v3); //back wall
		NewName = name;
		NewName.Append(":bottom");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	//set texture
	if (TextureOverride == false && FlipTexture == false)
		SetTexture(dest, index, texture, true, tw, th);
	else
	{
		ProcessTextureFlip(tw, th);
		int endindex = index + GetDrawWallsCount();
		if (FlipTexture == false)
		{
			for (int i = index; i < endindex; i++)
			{
				if (i - index == 0)
					SetTexture(dest, i, mainnegtex.GetData(), false, widthscale[0], heightscale[0]);
				if (i - index == 1)
					SetTexture(dest, i, mainpostex.GetData(), false, widthscale[1], heightscale[1]);
				if (i - index == 2)
					SetTexture(dest, i, sidenegtex.GetData(), false, widthscale[2], heightscale[2]);
				if (i - index == 3)
					SetTexture(dest, i, sidepostex.GetData(), false, widthscale[3], heightscale[3]);
				if (i - index == 4)
					SetTexture(dest, i, toptex.GetData(), false, widthscale[4], heightscale[4]);
				if (i - index == 5)
					SetTexture(dest, i, bottomtex.GetData(), false, widthscale[5], heightscale[5]);
			}
		}
		else
		{
			for (int i = index; i < endindex; i++)
				SetTexture(dest, i, texture, false, widthscale[i - index], heightscale[i - index]);
		}
	}

	return index;
}

void SBS::DeleteWall(csRef<iThingFactoryState> dest, int index)
{
	//delete wall polygons (front and back) from specified mesh
	dest->RemovePolygon(index);
	dest->RemovePolygon(index + 1);
}


void SBS::DeleteFloor(csRef<iThingFactoryState> dest, int index)
{
	//delete floor polygons (front and back) from specified mesh
	dest->RemovePolygon(index);
	dest->RemovePolygon(index + 1);
}

void SBS::Report (const char* msg, ...)
{
	va_list arg;
	va_start (arg, msg);
	if (rep)
		rep->ReportV (CS_REPORTER_SEVERITY_NOTIFY, "sbs", msg, arg);
	else
	{
		csPrintfV (msg, arg);
		csPrintf ("\n");
		fflush (stdout);
	}
	va_end (arg);
}

bool SBS::ReportError (const char* msg, ...)
{
	va_list arg;
	va_start (arg, msg);
	if (rep)
		rep->ReportV (CS_REPORTER_SEVERITY_ERROR, "sbs", msg, arg);
	else
	{
		csPrintfV (msg, arg);
		csPrintf ("\n");
		fflush (stdout);
	}
	va_end (arg);
	return false;
}

int SBS::CreateWallBox(csRef<iThingFactoryState> dest, const char *name, const char *texture, float x1, float x2, float z1, float z2, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom)
{
	//create 4 walls

	int firstidx = 0;
	int tmpidx = 0;
	int range = 0;
	int range2 = 0;

	if (inside == true)
	{
		//generate a box visible from the inside
		csBox3 box (csVector3(x1 * HorizScale, voffset, z1 * HorizScale), csVector3(x2 * HorizScale, voffset + height_in, z2 * HorizScale));
		firstidx = dest->AddQuad( //front
			box.GetCorner(CS_BOX_CORNER_xyz),
			box.GetCorner(CS_BOX_CORNER_Xyz),
			box.GetCorner(CS_BOX_CORNER_XYz),
			box.GetCorner(CS_BOX_CORNER_xYz));
		range++;
		dest->AddQuad( //right
			box.GetCorner(CS_BOX_CORNER_Xyz),
			box.GetCorner(CS_BOX_CORNER_XyZ),
			box.GetCorner(CS_BOX_CORNER_XYZ),
			box.GetCorner(CS_BOX_CORNER_XYz));
		range++;
		dest->AddQuad( //back
			box.GetCorner(CS_BOX_CORNER_XyZ),
			box.GetCorner(CS_BOX_CORNER_xyZ),
			box.GetCorner(CS_BOX_CORNER_xYZ),
			box.GetCorner(CS_BOX_CORNER_XYZ));
		range++;
		dest->AddQuad( //left
			box.GetCorner(CS_BOX_CORNER_xyZ),
			box.GetCorner(CS_BOX_CORNER_xyz),
			box.GetCorner(CS_BOX_CORNER_xYz),
			box.GetCorner(CS_BOX_CORNER_xYZ));
		range++;
		if (bottom == true)
		{
			dest->AddQuad( //bottom
				box.GetCorner(CS_BOX_CORNER_xyZ),
				box.GetCorner(CS_BOX_CORNER_XyZ),
				box.GetCorner(CS_BOX_CORNER_Xyz),
				box.GetCorner(CS_BOX_CORNER_xyz));
			range++;
		}
		if (top == true)
		{
			dest->AddQuad( //top
				box.GetCorner(CS_BOX_CORNER_xYz),
				box.GetCorner(CS_BOX_CORNER_XYz),
				box.GetCorner(CS_BOX_CORNER_XYZ),
				box.GetCorner(CS_BOX_CORNER_xYZ));
			range++;
		}
	}

	if (outside == true)
	{
		csBox3 box (csVector3(x1 * HorizScale, voffset, z1 * HorizScale), csVector3(x2 * HorizScale, voffset + height_in, z2 * HorizScale));
		tmpidx = dest->AddQuad( //front
			box.GetCorner(CS_BOX_CORNER_xYz),
			box.GetCorner(CS_BOX_CORNER_XYz),
			box.GetCorner(CS_BOX_CORNER_Xyz),
			box.GetCorner(CS_BOX_CORNER_xyz));
		range2++;
		if (inside == false)
			firstidx = tmpidx;
		dest->AddQuad( //right
			box.GetCorner(CS_BOX_CORNER_XYz),
			box.GetCorner(CS_BOX_CORNER_XYZ),
			box.GetCorner(CS_BOX_CORNER_XyZ),
			box.GetCorner(CS_BOX_CORNER_Xyz));
		range2++;
		dest->AddQuad( //back
			box.GetCorner(CS_BOX_CORNER_XYZ),
			box.GetCorner(CS_BOX_CORNER_xYZ),
			box.GetCorner(CS_BOX_CORNER_xyZ),
			box.GetCorner(CS_BOX_CORNER_XyZ));
		range2++;
		dest->AddQuad( //left
			box.GetCorner(CS_BOX_CORNER_xYZ),
			box.GetCorner(CS_BOX_CORNER_xYz),
			box.GetCorner(CS_BOX_CORNER_xyz),
			box.GetCorner(CS_BOX_CORNER_xyZ));
		range2++;
		if (bottom == true)
		{
			dest->AddQuad( //bottom
				box.GetCorner(CS_BOX_CORNER_xyz),
				box.GetCorner(CS_BOX_CORNER_Xyz),
				box.GetCorner(CS_BOX_CORNER_XyZ),
				box.GetCorner(CS_BOX_CORNER_xyZ));
			range2++;
		}
		if (top == true)
		{
			dest->AddQuad( //top
				box.GetCorner(CS_BOX_CORNER_xYZ),
				box.GetCorner(CS_BOX_CORNER_XYZ),
				box.GetCorner(CS_BOX_CORNER_XYz),
				box.GetCorner(CS_BOX_CORNER_xYz));
			range2++;
		}
	}

	//texture mapping
	iMaterialWrapper* tm = engine->GetMaterialList()->FindByName(texture);
	dest->SetPolygonMaterial(csPolygonRange(firstidx, firstidx + range + range2), tm);
	dest->SetPolygonTextureMapping(csPolygonRange(firstidx, firstidx + range + range2), 3);

	//polygon names
	csString NewName;
	if (inside == true)
	{
		NewName = name;
		NewName.Append(":inside");
		dest->SetPolygonName(csPolygonRange(firstidx, firstidx + range), NewName);
	}
	if (outside == true)
	{
		NewName = name;
		NewName.Append(":outside");
		dest->SetPolygonName(csPolygonRange(tmpidx, tmpidx + range2), NewName);
	}

	return firstidx;
}

int SBS::CreateWallBox2(csRef<iThingFactoryState> dest, const char *name, const char *texture, float CenterX, float CenterZ, float WidthX, float LengthZ, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom)
{
	//create 4 walls from a central point

	float x1;
	float x2;
	float z1;
	float z2;

	x1 = CenterX - (WidthX / 2);
	x2 = CenterX + (WidthX / 2);
	z1 = CenterZ - (LengthZ / 2);
	z2 = CenterZ + (LengthZ / 2);

	return CreateWallBox(dest, name, texture, x1, x2, z1, z2, height_in, voffset, tw, th, inside, outside, top, bottom);
}

void SBS::InitMeshes()
{
	//create object meshes
	Buildings = engine->CreateSectorWallsMesh (area, "Buildings");
	Buildings_state = scfQueryInterface<iThingFactoryState> (Buildings->GetMeshObject()->GetFactory());
	Buildings->SetZBufMode(CS_ZBUF_USE);

	External = engine->CreateSectorWallsMesh (area, "External");
	External_state = scfQueryInterface<iThingFactoryState> (External->GetMeshObject()->GetFactory());
	External->SetZBufMode(CS_ZBUF_USE);
	//External->SetRenderPriority(engine->GetAlphaRenderPriority());
	//External->GetMeshObject()->SetMixMode(CS_FX_ALPHA);

	Landscape = engine->CreateSectorWallsMesh (area, "Landscape");
	Landscape_state = scfQueryInterface<iThingFactoryState> (Landscape->GetMeshObject()->GetFactory());
	Landscape->SetZBufMode(CS_ZBUF_USE);
}

int SBS::AddCustomWall(csRef<iThingFactoryState> dest, const char *name, const char *texture, csPoly3D &varray, float tw, float th)
{
	//Adds a wall from a specified array of 3D vectors
	float tw2 = tw;
	float th2;
	float tempw1;
	float tempw2;
	int num;
	int i;
	csPoly3D varray1;
	csPoly3D varray2;

	//get number of stored vertices
	num = varray.GetVertexCount();

	//Set horizontal scaling
	for (i = 0; i < num; i++)
		varray1.AddVertex(varray[i].x * HorizScale, varray[i].y, varray[i].z * HorizScale);

	//create a second array with reversed vertices
	for (i = num - 1; i >= 0; i--)
		varray2.AddVertex(varray1[i]);

	csVector2 x, y, z;

	//get extents for texture autosizing
	x = GetExtents(varray1, 1);
	y = GetExtents(varray1, 2);
	z = GetExtents(varray1, 3);

	//Call texture autosizing formulas
	if (z.x == z.y)
		tw2 = AutoSize(x.x, x.y, true, tw);
	if (x.x == x.y)
		tw2 = AutoSize(z.x, z.y, true, tw);
	if ((z.x != z.y) && (x.x != x.y))
	{
		//calculate diagonals
		tempw1 = fabs(x.y - x.x);
		tempw2 = fabs(z.y - z.x);
		tw2 = AutoSize(0, sqrt(pow(tempw1, 2) + pow(tempw2, 2)), true, tw);
	}
	th2 = AutoSize(0, fabs(y.y - y.x), false, th);

	//create 2 polygons (front and back) from the vertex array
	int firstidx = dest->AddPolygon(varray1.GetVertices(), num);
	dest->AddPolygon(varray2.GetVertices(), num);

	material = engine->GetMaterialList ()->FindByName (texture);
	dest->SetPolygonMaterial (csPolygonRange(firstidx, firstidx + 1), material);

	csString texname = texture;
	float tw3 = tw2, th3 = th2;

	if (material == 0)
	{
		//if material's not found, display a warning and use a default material
		csString Texture = texture;
		csString polyname = dest->GetPolygonName(firstidx);
		csString message = "Texture '" + Texture + "' not found for polygon '" + polyname + "'; using default material";
		ReportError(message);
		//set to default material
		material = engine->GetMaterialList()->FindByName("Default");
		texname = "Default";
	}

	//get per-texture tiling values from the textureinfo array
	for (int i = 0; i < textureinfo.GetSize(); i++)
	{
		if (textureinfo[i].name == texname)
		{
			//multiply the tiling parameters (tw and th) by
			//the stored multipliers for that texture
			tw3 = tw2 * textureinfo[i].widthmult;
			th3 = th2 * textureinfo[i].heightmult;
		}
	}

	//reverse extents if specified
	float tmpv;
	if (RevX == true)
	{
		tmpv = x.x;
		x.x = x.y;
		x.y = tmpv;
	}
	if (RevY == true)
	{
		tmpv = y.x;
		y.x = y.y;
		y.y = tmpv;
	}
	if (RevZ == true)
	{
		tmpv = z.x;
		z.x = z.y;
		z.y = tmpv;
	}

	//texture mapping is set from 3 manual vectors (origin, width extent,
	//height extent) in a square layout
	csVector3 v1 (x.x, y.y, z.x); //top left
	csVector3 v2 (x.y, y.y, z.y); //top right
	csVector3 v3 (x.y, y.x, z.y); //bottom right

	dest->SetPolygonTextureMapping (csPolygonRange(firstidx, firstidx),
		v1,
		csVector2 (0, 0),
		v2,
		csVector2 (tw3, 0),
		v3,
		csVector2 (tw3, th3));
	dest->SetPolygonTextureMapping (csPolygonRange(firstidx + 1, firstidx + 1),
		v1,
		csVector2 (tw3, 0),
		v2,
		csVector2 (0, 0),
		v3,
		csVector2 (0, th3));

	//set polygon names
	csString NewName;
	NewName = name;
	NewName.Append(":0");
	dest->SetPolygonName(csPolygonRange(firstidx, firstidx), NewName);
	NewName = name;
	NewName.Append(":1");
	dest->SetPolygonName(csPolygonRange(firstidx + 1, firstidx + 1), NewName);

	return firstidx;
}

int SBS::AddCustomFloor(csRef<iThingFactoryState> dest, const char *name, const char *texture, csPoly3D &varray, float tw, float th)
{
	//Adds a wall from a specified array of 3D vectors
	float tw2 = tw;
	float th2;
	float tempw1;
	float tempw2;
	int num;
	int i;
	csPoly3D varray1;

	//get number of stored vertices
	num = varray.GetVertexCount();

	//Set horizontal scaling
	for (i = 0; i < num; i++)
		varray1.AddVertex(varray[i].x * HorizScale, varray[i].y, varray[i].z * HorizScale);

	//create a second array with reversed vertices
	for (i = num - 1; i >= 0; i--)
		varray1.AddVertex(varray1[i]);

	csVector2 x, y, z;

	//get extents for texture autosizing
	x = GetExtents(varray, 1);
	y = GetExtents(varray, 2);
	z = GetExtents(varray, 3);

	//Call texture autosizing formulas
	if (z.x == z.y)
		tw2 = AutoSize(x.x, x.y, true, tw);
	if (x.x == x.y)
		tw2 = AutoSize(z.x, z.y, true, tw);
	if ((z.x != z.y) && (x.x != x.y))
	{
		//calculate diagonals
		tempw1 = fabs(x.y - x.x);
		tempw2 = fabs(z.y - z.x);
		tw2 = AutoSize(0, sqrt(pow(tempw1, 2) + pow(tempw2, 2)), true, tw);
	}
	th2 = AutoSize(0, fabs(y.y - y.x), false, th);

	//create 2 polygons (front and back) from the vertex array
	int firstidx = dest->AddPolygon(varray.GetVertices(), num);
	dest->AddPolygon(varray1.GetVertices(), num);

	material = engine->GetMaterialList ()->FindByName (texture);
	dest->SetPolygonMaterial (csPolygonRange(firstidx, firstidx + 1), material);

	//reverse extents if specified
	float tmpv;
	if (RevX == true)
	{
		tmpv = x.x;
		x.x = x.y;
		x.y = tmpv;
	}
	if (RevY == true)
	{
		tmpv = y.x;
		y.x = y.y;
		y.y = tmpv;
	}
	if (RevZ == true)
	{
		tmpv = z.x;
		z.x = z.y;
		z.y = tmpv;
	}

	//texture mapping is set from 3 manual vectors (origin, width extent,
	//height extent) in a square layout
	csVector3 v1 (x.x, y.y, z.x); //top left
	csVector3 v2 (x.y, y.y, z.y); //top right
	csVector3 v3 (x.y, y.x, z.y); //bottom right

	dest->SetPolygonTextureMapping (csPolygonRange(firstidx, firstidx),
		v1,
		csVector2 (0, 0),
		v2,
		csVector2 (tw2, 0),
		v3,
		csVector2 (tw2, th2));
	dest->SetPolygonTextureMapping (csPolygonRange(firstidx + 1, firstidx + 1),
		v1,
		csVector2 (tw2, 0),
		v2,
		csVector2 (0, 0),
		v3,
		csVector2 (0, th2));

	//set polygon names
	csString NewName;
	NewName = name;
	NewName.Append(":0");
	dest->SetPolygonName(csPolygonRange(firstidx, firstidx), NewName);
	NewName = name;
	NewName.Append(":1");
	dest->SetPolygonName(csPolygonRange(firstidx + 1, firstidx + 1), NewName);

	return firstidx;
}

int SBS::AddTriangleWall(csRef<iThingFactoryState> dest, const char *name, const char *texture, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float tw, float th)
{
	//Adds a triangular wall with the specified dimensions
	csPoly3D varray;

	//set up temporary vertex array
	varray.AddVertex(x1, y1, z1);
	varray.AddVertex(x2, y2, z2);
	varray.AddVertex(x3, y3, z3);

	//pass data on to AddCustomWall function
	int firstidx = AddCustomWall(dest, name, texture, varray, tw, th);

	return firstidx;
}

void SBS::EnableBuildings(bool value)
{
	//turns buildings on/off
	if (value == true)
	{
		Buildings->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		Buildings->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		Buildings->GetFlags().Reset (CS_ENTITY_NOHITBEAM);
	}
	else
	{
		Buildings->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		Buildings->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		Buildings->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
	IsBuildingsEnabled = value;
}

void SBS::EnableLandscape(bool value)
{
	//turns landscape on/off
	if (value == true)
	{
		Landscape->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		Landscape->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		Landscape->GetFlags().Reset (CS_ENTITY_NOHITBEAM);
	}
	else
	{
		Landscape->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		Landscape->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		Landscape->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
	IsLandscapeEnabled = value;
}

void SBS::EnableExternal(bool value)
{
	//turns external on/off
	if (value == true)
	{
		External->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		External->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		External->GetFlags().Reset (CS_ENTITY_NOHITBEAM);
	}
	else
	{
		External->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		External->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		External->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
	IsExternalEnabled = value;
}

void SBS::EnableSkybox(bool value)
{
	//turns skybox on/off
	if (value == true)
	{
		SkyBox->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		SkyBox->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		SkyBox->GetFlags().Reset (CS_ENTITY_NOHITBEAM);
	}
	else
	{
		SkyBox->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		SkyBox->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		SkyBox->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
	IsSkyboxEnabled = value;
}

csVector2 SBS::GetExtents(csPoly3D &varray, int coord)
{
	//returns the smallest and largest values from a specified coordinate type
	//(x, y, or z) from a vertex array (polygon).
	//first parameter must be a vertex array object
	//second must be either 1 (for x), 2 (for y) or 3 (for z)

	float esmall;
	float ebig;
	float tempnum;
	int i;
	int num = varray.GetVertexCount();

	//return 0,0 if coord value is out of range
	if (coord < 1 || coord > 3)
		return csVector2(0, 0);

	for (i = 0; i < num; i++)
	{
		if (coord == 1)
			tempnum = varray[i].x;
		if (coord == 2)
			tempnum = varray[i].y;
		if (coord == 3)
			tempnum = varray[i].z;

		if (i == 0)
		{
			esmall = tempnum;
			ebig = tempnum;
		}
		else
		{
			if (tempnum < esmall)
				esmall = tempnum;
			if (tempnum > ebig)
				ebig = tempnum;
		}
	}

	return csVector2(esmall, ebig);
}

int SBS::CreateSky(const char *filenamebase)
{
	csString file = filenamebase;
	vfs->Mount("/root/sky", root_dir + "data" + dir_char + "sky-" + file + ".zip");

	//load textures
	LoadTexture("/root/sky/up.jpg", "SkyTop", 1, 1);
	LoadTexture("/root/sky/down.jpg", "SkyBottom", 1, 1);
	LoadTexture("/root/sky/left.jpg", "SkyLeft", 1, 1);
	LoadTexture("/root/sky/right.jpg", "SkyRight", 1, 1);
	LoadTexture("/root/sky/front.jpg", "SkyFront", 1, 1);
	LoadTexture("/root/sky/back.jpg", "SkyBack", 1, 1);

	SkyBox = (engine->CreateSectorWallsMesh (area, "SkyBox"));
	SkyBox_state = scfQueryInterface<iThingFactoryState> (SkyBox->GetMeshObject()->GetFactory());
	SkyBox->SetZBufMode(CS_ZBUF_USE);

	//create a skybox that extends 30 miles (30 * 5280 ft) in each direction
	int firstidx = SkyBox_state->AddInsideBox(csVector3(-158400, -158400, -158400), csVector3(158400, 158400, 158400));
	material = engine->GetMaterialList ()->FindByName ("SkyBack");
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx, firstidx), material);
	material = engine->GetMaterialList ()->FindByName ("SkyRight");
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 1, firstidx + 1), material);
	material = engine->GetMaterialList ()->FindByName ("SkyFront");
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 2, firstidx + 2), material);
	material = engine->GetMaterialList ()->FindByName ("SkyLeft");
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 3, firstidx + 3), material);
	material = engine->GetMaterialList ()->FindByName ("SkyBottom");
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 4, firstidx + 4), material);
	material = engine->GetMaterialList ()->FindByName ("SkyTop");
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 5, firstidx + 5), material);

	SkyBox_state->SetPolygonTextureMapping (csPolygonRange(firstidx, firstidx + 5),
		csVector2 (0, 1),
		csVector2 (1, 1),
		csVector2 (1, 0));

	return firstidx;
}

int SBS::GetFloorNumber(float altitude, int lastfloor, bool checklastfloor)
{
	//Returns floor number located at a specified altitude

	//check to see if altitude is below bottom floor
	if (altitude < GetFloor(-Basements)->Altitude)
		return -Basements;

	//if checklastfloor is specified, compare altitude with lastfloor
	if (checklastfloor == true)
	{
		int lastfloor_altitude = GetFloor(lastfloor)->Altitude;
		int upperfloor_altitude;
		if (lastfloor < Floors - 1)
			upperfloor_altitude = GetFloor(lastfloor + 1)->Altitude;
		else
			upperfloor_altitude = GetFloor(lastfloor)->Altitude + GetFloor(lastfloor)->FullHeight();

		if (upperfloor_altitude > altitude && lastfloor_altitude <= altitude)
			return lastfloor;
		else
		{
			//if altitude is below lastfloor, search downwards; otherwise search upwards
			if (altitude < lastfloor_altitude)
			{
				for (int i = lastfloor - 1; i >= -Basements; i--)
				{
					if (GetFloor(i + 1)->Altitude > altitude && GetFloor(i)->Altitude <= altitude)
						return i;
				}
			}
			else if (altitude >= upperfloor_altitude)
			{
				for (int i = lastfloor + 1; i < Floors; i++)
				{
					if (GetFloor(i - 1)->Altitude <= altitude && GetFloor(i)->Altitude > altitude)
						return i - 1;
				}
			}
		}
	}

	//otherwise do a slow linear search through floors
	for (int i = -Basements + 1; i < Floors; i++)
	{
		//check to see if altitude is within a floor (between the current floor's base and
		//the lower floor's base)
		if ((GetFloor(i)->Altitude > altitude) && (GetFloor(i - 1)->Altitude <= altitude))
			return i - 1;
		//check to see if altitude is above top floor's altitude
		if ((i == Floors - 1) && (altitude > GetFloor(i)->Altitude))
			return i;
	}
	return 0;
}

float SBS::GetDistance(float x1, float x2, float z1, float z2)
{
	//returns the distance between 2 2D vectors

	if (z1 == z2)
		return fabs(x1 - x2);
	if (x1 == x2)
		return fabs(z1 - z2);
	if ((x1 != x2) && (z2 != x2))
		return sqrt(pow(fabs(x1 - x2), 2) + pow(fabs(z1 - z2), 2)); //calculate diagonals
	return 0;
}

void SBS::DumpVertices(csRef<iThingFactoryState> mesh)
{
	//dumps a list of vertices from a mesh object to the console/logfile

	Report("--- Vertex Dump ---\n");
	for (int i = 0; i < mesh->GetVertexCount(); i++)
		Report(csString(_itoa(i, intbuffer, 10)) + ": " + csString(_gcvt(mesh->GetVertices()[i].x, 6, buffer)) + ", " + csString(_gcvt(mesh->GetVertices()[i].y, 6, buffer)) + ", " + csString(_gcvt(mesh->GetVertices()[i].z, 6, buffer)));
}

void SBS::ListAltitudes()
{
	//dumps the floor altitude list

	Report("--- Floor Altitudes ---\n");
	for (int i = -Basements; i < Floors; i++)
		Report(csString(_itoa(i, intbuffer, 10)) + "(" + GetFloor(i)->ID + ")\t----\t" + csString(_gcvt(GetFloor(i)->FullHeight(), 6, buffer)) + "\t----\t" + csString(_gcvt(GetFloor(i)->Altitude, 6, buffer)));
}

void SBS::CreateShaft(int number, int type, float CenterX, float CenterZ, int _startfloor, int _endfloor)
{
	//create a shaft object

	for (size_t i = 0; i < ShaftArray.GetSize(); i++)
		if (ShaftArray[i].number == number)
			return;
	ShaftArray.SetSize(ShaftArray.GetSize() + 1);
	ShaftArray[ShaftArray.GetSize() - 1].number = number;
	ShaftArray[ShaftArray.GetSize() - 1].object = new Shaft(number, type, CenterX, CenterZ, _startfloor, _endfloor);
}

void SBS::CreateStairwell(int number, float CenterX, float CenterZ, int _startfloor, int _endfloor)
{
	//create a stairwell object

	for (size_t i = 0; i < StairsArray.GetSize(); i++)
		if (StairsArray[i].number == number)
			return;
	StairsArray.SetSize(StairsArray.GetSize() + 1);
	StairsArray[StairsArray.GetSize() - 1].number = number;
	StairsArray[StairsArray.GetSize() - 1].object = new Stairs(number, CenterX, CenterZ, _startfloor, _endfloor);
}

iMaterialWrapper *SBS::ChangeTexture(iMeshWrapper *mesh, csRef<iMaterialWrapper> oldmat, const char *texture)
{
	//changes a texture

	csRef<iThingState> thingstate = scfQueryInterface<iThingState> (mesh->GetMeshObject());

	//get new material
	csRef<iMaterialWrapper> newmat = engine->GetMaterialList()->FindByName(texture);

	//switch material
	thingstate->ClearReplacedMaterials();
	thingstate->ReplaceMaterial(oldmat, newmat);

	return newmat;
}

void SBS::SetTexture(csRef<iThingFactoryState> mesh, int index, const char *texture, bool has_thickness, float tw, float th)
{
	//sets texture for a range of polygons

	material = engine->GetMaterialList()->FindByName(texture);
	csString texname = texture;

	if (tw == 0)
		tw = 1;
	if (th == 0)
		th = 1;

	float tw2 = tw, th2 = th;

	if (material == 0)
	{
		//if material's not found, display a warning and use a default material
		csString Texture = texture;
		csString polyname = mesh->GetPolygonName(index);
		csString message = "Texture '" + Texture + "' not found for polygon '" + polyname + "'; using default material";
		ReportError(message);
		//set to default material
		material = engine->GetMaterialList()->FindByName("Default");
		texname = "Default";
	}

	//get per-texture tiling values from the textureinfo array
	for (int i = 0; i < textureinfo.GetSize(); i++)
	{
		if (textureinfo[i].name == texname)
		{
			//multiply the tiling parameters (tw and th) by
			//the stored multipliers for that texture
			tw2 = tw * textureinfo[i].widthmult;
			th2 = th * textureinfo[i].heightmult;
			break;
		}
	}

	int endindex;
	if (has_thickness == true)
		endindex = index + GetDrawWallsCount() - 1;
	else
		endindex = index;

	if (TextureOverride == false)
	{
		for (int i = index; i <= endindex; i++)
		{
			mesh->SetPolygonMaterial(csPolygonRange(i, i), material);
			//texture mapping is set from first 3 coordinates
			mesh->SetPolygonTextureMapping (csPolygonRange(i, i),
				mesh->GetPolygonVertex(i, 0),
				csVector2 (0, 0),
				mesh->GetPolygonVertex(i, 1),
				csVector2 (tw2, 0),
				mesh->GetPolygonVertex(i, 2),
				csVector2 (tw2, th2));
		}
	}
	else
	{
			mesh->SetPolygonMaterial(csPolygonRange(index, index), material);
			//texture mapping is set from first 3 coordinates
			mesh->SetPolygonTextureMapping (csPolygonRange(index, index),
				mesh->GetPolygonVertex(index, 0),
				csVector2 (0, 0),
				mesh->GetPolygonVertex(index, 1),
				csVector2 (tw2, 0),
				mesh->GetPolygonVertex(index, 2),
				csVector2 (tw2, th2));
	}
}

void SBS::NewElevator(int number)
{
	//create a new elevator object
	for (size_t i = 0; i < ElevatorArray.GetSize(); i++)
		if (ElevatorArray[i].number == number)
			return;
	ElevatorArray.SetSize(ElevatorArray.GetSize() + 1);
	ElevatorArray[ElevatorArray.GetSize() - 1].number = number;
	ElevatorArray[ElevatorArray.GetSize() - 1].object = new Elevator(number);
}

void SBS::NewFloor(int number)
{
	//create a new floor object
	for (size_t i = 0; i < FloorArray.GetSize(); i++)
		if (FloorArray[i].number == number)
			return;
	FloorArray.SetSize(FloorArray.GetSize() + 1);
	FloorArray[FloorArray.GetSize() - 1].number = number;
	FloorArray[FloorArray.GetSize() - 1].object = new Floor(number);

	if (number < 0)
		Basements++;
	else
		Floors++;
}

int SBS::Elevators()
{
	//return the number of elevators
	return ElevatorArray.GetSize();
}

int SBS::TotalFloors()
{
	//return the number of floors
	return FloorArray.GetSize();
}

int SBS::Shafts()
{
	//return the number of shafts
	return ShaftArray.GetSize();
}

int SBS::StairsNum()
{
	//return the number of stairs
	return StairsArray.GetSize();
}

Floor *SBS::GetFloor(int number)
{
	//return pointer to floor object

	if (number < -Basements || number > Floors - 1)
		return 0;

	if (FloorArray.GetSize() > 0)
	{
		//quick prediction
		if (FloorArray[Basements + number].number == number)
		{
			if (FloorArray[Basements + number].object)
				return FloorArray[Basements + number].object;
			else
				return 0;
		}
	}

	for (size_t i = 0; i < FloorArray.GetSize(); i++)
		if (FloorArray[i].number == number)
			return FloorArray[i].object;
	return 0;
}

Elevator *SBS::GetElevator(int number)
{
	//return pointer to elevator object

	if (number < 1 || number > Elevators())
		return 0;

	if (ElevatorArray.GetSize() > number - 1)
	{
		//quick prediction
		if (ElevatorArray[number - 1].number == number)
		{
			if (ElevatorArray[number - 1].object)
				return ElevatorArray[number - 1].object;
			else
				return 0;
		}
	}

	for (size_t i = 0; i < ElevatorArray.GetSize(); i++)
		if (ElevatorArray[i].number == number)
			return ElevatorArray[i].object;
	return 0;
}

Shaft *SBS::GetShaft(int number)
{
	//return pointer to shaft object

	if (number < 1 || number > Shafts())
		return 0;

	if (ShaftArray.GetSize() > number - 1)
	{
		//quick prediction
		if (ShaftArray[number - 1].number == number)
		{
			if (ShaftArray[number - 1].object)
				return ShaftArray[number - 1].object;
			else
				return 0;
		}
	}

	for (size_t i = 0; i < ShaftArray.GetSize(); i++)
		if (ShaftArray[i].number == number)
			return ShaftArray[i].object;
	return 0;
}

Stairs *SBS::GetStairs(int number)
{
	//return pointer to stairs object

	if (number < 1 || number > StairsNum())
		return 0;

	if (StairsArray.GetSize() > number - 1)
	{
		//quick prediction
		if (StairsArray[number - 1].number == number)
		{
			if (StairsArray[number - 1].object)
				return StairsArray[number - 1].object;
			else
				return 0;
		}
	}

	for (size_t i = 0; i < StairsArray.GetSize(); i++)
		if (StairsArray[i].number == number)
			return StairsArray[i].object;
	return 0;
}

void SBS::SetWallOrientation(const char *direction)
{
	//changes internal wall orientation parameter.
	//direction can either be "left" (negative), "center" (0), or "right" (positive).
	//default on startup is 1, or center.
	//the parameter is used to determine the location of the wall's
	//x1/x2 or z1/z2 coordinates in relation to the thickness extents

	csString temp = direction;
	temp.Downcase();

	if (temp == "left")
		wall_orientation = 0;
	if (temp == "center")
		wall_orientation = 1;
	if (temp == "right")
		wall_orientation = 2;
}

int SBS::GetWallOrientation()
{
	return wall_orientation;
}

void SBS::SetFloorOrientation(const char *direction)
{
	//changes internal floor orientation parameter.
	//direction can either be "bottom" (negative), "center" (0), or "top" (positive).
	//default on startup is 2, or top.
	//the parameter is used to determine the location of the floor's
	//x1/x2 or z1/z2 coordinates in relation to the thickness extents

	csString temp = direction;
	temp.Downcase();

	if (temp == "bottom")
		floor_orientation = 0;
	if (temp == "center")
		floor_orientation = 1;
	if (temp == "top")
		floor_orientation = 2;
}

int SBS::GetFloorOrientation()
{
	return floor_orientation;
}

void SBS::DrawWalls(bool MainN, bool MainP, bool SideN, bool SideP, bool Top, bool Bottom)
{
	//sets which walls should be drawn

	//first backup old parameters
	DrawMainNOld = DrawMainN;
	DrawMainPOld = DrawMainP;
	DrawSideNOld = DrawSideN;
	DrawSidePOld = DrawSideP;
	DrawTopOld = DrawTop;
	DrawBottomOld = DrawBottom;

	//now set new parameters
	DrawMainN = MainN;
	DrawMainP = MainP;
	DrawSideN = SideN;
	DrawSideP = SideP;
	DrawTop = Top;
	DrawBottom = Bottom;

}

void SBS::ResetWalls(bool ToDefaults)
{
	//if ToDefaults is true, this resets the DrawWalls data to the defaults.
	//if ToDefaults is false, this reverts the DrawWalls data to the previous settings.

	if (ToDefaults == true)
		DrawWalls(true, true, false, false, false, false);
	else
		DrawWalls(DrawMainNOld, DrawMainPOld, DrawSideNOld, DrawSidePOld, DrawTopOld, DrawBottomOld);
}

void SBS::ReverseExtents(bool X, bool Y, bool Z)
{
	//Reverses texture mapping per axis

	//first backup old parameters
	RevXold = RevX;
	RevYold = RevY;
	RevZold = RevZ;

	//now set new parameters
	RevX = X;
	RevY = Y;
	RevZ = Z;
}

void SBS::ResetExtents(bool ToDefaults)
{
	//if ToDefaults is true, this resets the extents data to the defaults.
	//if ToDefaults is false, this reverts the extents data to the previous settings.

	if (ToDefaults == true)
		ReverseExtents(false, false, false);
	else
		ReverseExtents(RevXold, RevYold, RevZold);
}

int SBS::GetDrawWallsCount()
{
	//gets the number of wall polygons enabled

	int sides = 0;

	if (DrawMainN == true)
		sides++;
	if (DrawMainP == true)
		sides++;
	if (DrawSideN == true)
		sides++;
	if (DrawSideP == true)
		sides++;
	if (DrawTop == true)
		sides++;
	if (DrawBottom == true)
		sides++;

	return sides;
}

csVector3 SBS::GetPoint(csRef<iThingFactoryState> mesh, const char *polyname, csVector3 start, csVector3 end)
{
	//do a line intersection with a specified mesh, and return
	//the intersection point
	int polyindex = mesh->FindPolygonByName(polyname);

	//do a plane intersection with a line
	csVector3 isect;
	float dist;
	csPlane3 plane = mesh->GetPolygonObjectPlane(polyindex);
	csIntersect3::SegmentPlane(start, end, plane, isect, dist);

	return isect;
}

void SBS::Cut(csRef<iThingFactoryState> state, csVector3 start, csVector3 end, bool cutwalls, bool cutfloors, csVector3 mesh_origin, csVector3 object_origin, int checkwallnumber, const char *checkstring)
{
	//cuts a rectangular hole in the polygons within the specified range
	//mesh_origin is a modifier for meshes with relative polygon coordinates (used only for calculating door positions) - in this you specify the mesh's global position
	//object_origin is for the object's (such as a shaft) central position, used for calculating wall offsets

	if (cutwalls == false && cutfloors == false)
		return;

	csPoly3D temppoly, temppoly2, temppoly3, temppoly4, temppoly5, worker;
	int addpolys;
	int tmpindex;
	int tmpindex_tmp;
	int polycount;
	bool polycheck;
	if (checkwallnumber == 1)
	{
		wall1a = false;
		wall1b = false;
	}
	if (checkwallnumber == 2)
	{
		wall2a = false;
		wall2b = false;
	}

	//step through each polygon
	polycount = state->GetPolygonCount();
	for (int i = 0; i < polycount; i++)
	{
		temppoly.MakeEmpty();
		temppoly2.MakeEmpty();
		temppoly3.MakeEmpty();
		temppoly4.MakeEmpty();
		temppoly5.MakeEmpty();
		worker.MakeEmpty();
		addpolys = 0;
		tmpindex = -1;
		tmpindex_tmp = -1;
		csVector2 extentsx, extentsy, extentsz;
		polycheck = false;

		//copy source polygon vertices
		csString name = state->GetPolygonName(i);
		for (int j = 0; j < state->GetPolygonVertexCount(i); j++)
			temppoly.AddVertex(state->GetPolygonVertex(i, j));

		//make sure the polygon is not outside the cut area
		if (temppoly.ClassifyX(start.x) != CS_POL_FRONT &&
			temppoly.ClassifyX(end.x) != CS_POL_BACK &&
			temppoly.ClassifyY(start.y) != CS_POL_FRONT &&
			temppoly.ClassifyY(end.y) != CS_POL_BACK &&
			temppoly.ClassifyZ(start.z) != CS_POL_FRONT &&
			temppoly.ClassifyZ(end.z) != CS_POL_BACK)
		{
			//Report("Cutting polygon " + name);

			extentsx = GetExtents(temppoly, 1);
			extentsy = GetExtents(temppoly, 2);
			extentsz = GetExtents(temppoly, 3);

			//is polygon a wall?
			if (extentsy.x != extentsy.y)
			{
				if (cutwalls == true)
				{
					//wall
					if (fabs(extentsx.x - extentsx.y) > fabs(extentsz.x - extentsz.y))
					{
						//wall is facing forward/backward

						//get left side
						worker = temppoly;
						worker.SplitWithPlaneX(temppoly, temppoly2, start.x);
						worker.MakeEmpty();

						//get right side
						if (temppoly2.GetVertexCount() > 0)
							worker = temppoly2;
						else
							worker = temppoly;
						worker.SplitWithPlaneX(temppoly3, temppoly2, end.x);
						worker.MakeEmpty();

						//get lower
						if (temppoly3.GetVertexCount() > 0)
							worker = temppoly3;
						else if (temppoly2.GetVertexCount() > 0)
							worker = temppoly2;
						else if (temppoly.GetVertexCount() > 0)
							worker = temppoly;
						worker.SplitWithPlaneY(temppoly3, temppoly4, start.y);
						worker.MakeEmpty();

						//get upper
						if (temppoly4.GetVertexCount() > 0)
							worker = temppoly4;
						else if (temppoly3.GetVertexCount() > 0)
							worker = temppoly3;
						else if (temppoly2.GetVertexCount() > 0)
							worker = temppoly2;
						else if (temppoly.GetVertexCount() > 0)
							worker = temppoly;
						worker.SplitWithPlaneY(temppoly5, temppoly4, end.y);
						worker.MakeEmpty();
					}
					else
					{
						//wall is facing left/right

						//get left side
						worker = temppoly;
						worker.SplitWithPlaneZ(temppoly, temppoly2, start.z);
						worker.MakeEmpty();

						//get right side
						if (temppoly2.GetVertexCount() > 0)
							worker = temppoly2;
						else
							worker = temppoly;
						worker.SplitWithPlaneZ(temppoly3, temppoly2, end.z);
						worker.MakeEmpty();

						//get lower
						if (temppoly3.GetVertexCount() > 0)
							worker = temppoly3;
						else if (temppoly2.GetVertexCount() > 0)
							worker = temppoly2;
						else if (temppoly.GetVertexCount() > 0)
							worker = temppoly;
						worker.SplitWithPlaneY(temppoly3, temppoly4, start.y);
						worker.MakeEmpty();

						//get upper
						if (temppoly4.GetVertexCount() > 0)
							worker = temppoly4;
						else if (temppoly3.GetVertexCount() > 0)
							worker = temppoly3;
						else if (temppoly2.GetVertexCount() > 0)
							worker = temppoly2;
						else if (temppoly.GetVertexCount() > 0)
							worker = temppoly;
						worker.SplitWithPlaneY(temppoly5, temppoly4, end.y);
						worker.MakeEmpty();
					}
					polycheck = true;
					//store extents of temppoly5 for door sides if needed
					if (checkwallnumber > 0 && checkwallnumber < 3)
					{
						if (name.Find(checkstring) >= 0)
						{
							float extent;
							if (checkwallnumber == 2 && (wall2a == false || wall2b == false))
							{
								//level walls
								if (wall2a == true)
									wall2b = true;
								wall2a = true;
								extent = GetExtents(temppoly5, 1).x + mesh_origin.x;
								if (wall2b == false || (wall2b == true && fabs(extent - object_origin.x) > fabs(wall_extents_x.x - object_origin.x)))
									wall_extents_x.x = extent;
								extent = GetExtents(temppoly5, 3).x + mesh_origin.z;
								if (wall2b == false || (wall2b == true && fabs(extent - object_origin.z) > fabs(wall_extents_z.x - object_origin.z)))
									wall_extents_z.x = extent;
								wall_extents_y = GetExtents(temppoly5, 2) + mesh_origin.y;
							}
							else if (wall1a == false || wall1b == false)
							{
								//shaft walls
								if (wall1a == true)
									wall1b = true;
								wall1a = true;
								extent = GetExtents(temppoly5, 1).y + mesh_origin.x;
								if (wall1b == false || (wall1b == true && fabs(extent - object_origin.x) < fabs(wall_extents_x.y - object_origin.x)))
									wall_extents_x.y = extent;
								extent = GetExtents(temppoly5, 3).y + mesh_origin.z;
								if (wall1b == false || (wall1b == true && fabs(extent - object_origin.z) < fabs(wall_extents_z.y - object_origin.z)))
									wall_extents_z.y = extent;
							}
						}
					}
				}
			}
			else if (cutfloors == true)
			{
				//floor

				//get left side
				worker = temppoly;
				worker.SplitWithPlaneX(temppoly, temppoly2, start.x);
				worker.MakeEmpty();

				//get right side
				if (temppoly2.GetVertexCount() > 0)
					worker = temppoly2;
				else
					worker = temppoly;
				worker.SplitWithPlaneX(temppoly3, temppoly2, end.x);
				worker.MakeEmpty();

				//get lower
				if (temppoly3.GetVertexCount() > 0)
					worker = temppoly3;
				else if (temppoly2.GetVertexCount() > 0)
					worker = temppoly2;
				else if (temppoly.GetVertexCount() > 0)
					worker = temppoly;
				worker.SplitWithPlaneZ(temppoly3, temppoly4, start.z);
				worker.MakeEmpty();

				//get upper
				if (temppoly4.GetVertexCount() > 0)
					worker = temppoly4;
				else if (temppoly3.GetVertexCount() > 0)
					worker = temppoly3;
				else if (temppoly2.GetVertexCount() > 0)
					worker = temppoly2;
				else if (temppoly.GetVertexCount() > 0)
					worker = temppoly;
				worker.SplitWithPlaneZ(temppoly5, temppoly4, end.z);
				worker.MakeEmpty();

				polycheck = true;
			}

			if (polycheck == true)
			{
				//get texture data from original polygon
				iMaterialWrapper *oldmat = state->GetPolygonMaterial(i);
				csVector3 oldvector;
				csMatrix3 mapping;
				state->GetPolygonTextureMapping(i, mapping, oldvector);

				//delete original polygon
				state->RemovePolygon(i);
				i--;
				polycount--;

				//create splitted polygons
				if (temppoly.GetVertexCount() > 2)
				{
					addpolys++;
					tmpindex_tmp = state->AddPolygon(temppoly.GetVertices(), temppoly.GetVertexCount());
					state->SetPolygonName(csPolygonRange(tmpindex_tmp, tmpindex_tmp), name);
					if (tmpindex == -1)
						tmpindex = tmpindex_tmp;
				}
				temppoly.MakeEmpty();
				if (temppoly2.GetVertexCount() > 2)
				{
					addpolys++;
					tmpindex_tmp = state->AddPolygon(temppoly2.GetVertices(), temppoly2.GetVertexCount());
					state->SetPolygonName(csPolygonRange(tmpindex_tmp, tmpindex_tmp), name);
					if (tmpindex == -1)
						tmpindex = tmpindex_tmp;
				}
				temppoly2.MakeEmpty();
				if (temppoly3.GetVertexCount() > 2)
				{
					addpolys++;
					tmpindex_tmp = state->AddPolygon(temppoly3.GetVertices(), temppoly3.GetVertexCount());
					state->SetPolygonName(csPolygonRange(tmpindex_tmp, tmpindex_tmp), name);
					if (tmpindex == -1)
						tmpindex = tmpindex_tmp;
				}
				temppoly3.MakeEmpty();
				if (temppoly4.GetVertexCount() > 2)
				{
					addpolys++;
					tmpindex_tmp = state->AddPolygon(temppoly4.GetVertices(), temppoly4.GetVertexCount());
					state->SetPolygonName(csPolygonRange(tmpindex_tmp, tmpindex_tmp), name);
					if (tmpindex == -1)
						tmpindex = tmpindex_tmp;
				}
				temppoly4.MakeEmpty();

				//apply material to new polygon set
				if (addpolys > 0)
				{
					state->SetPolygonMaterial(csPolygonRange(tmpindex, tmpindex + addpolys - 1), oldmat);
					state->SetPolygonTextureMapping(csPolygonRange(tmpindex, tmpindex + addpolys - 1), mapping, oldvector);
				}
			}
		}
	}
}

float SBS::MetersToFeet(float meters)
{
	//converts meters to feet
	return meters * 3.2808399f;
}

float SBS::FeetToMeters(float feet)
{
	//converts feet to meters
	return feet * 3.2808399f;
}

int SBS::AddDoorwayWalls(csRef<iThingFactoryState> mesh, const char *texture, float tw, float th)
{
	//add joining doorway polygons if needed
	int index = 0;
	if (wall1a == true && wall2a == true)
	{
		wall1a = false;
		wall1b = false;
		wall2a = false;
		wall2b = false;
		DrawWalls(true, true, false, false, false, false);
		if (fabs(wall_extents_x.x - wall_extents_x.y) > fabs(wall_extents_z.x - wall_extents_z.y))
		{
			//doorway is facing forward/backward
			index = AddWallMain(mesh, "DoorwayLeft", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.x, wall_extents_z.y, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th);
			AddWallMain(mesh, "DoorwayRight", texture, 0, wall_extents_x.y, wall_extents_z.x, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th);
			AddFloorMain(mesh, "DoorwayTop", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y, wall_extents_y.y, tw, th);
		}
		else
		{
			//doorway is facing left/right
			AddWallMain(mesh, "DoorwayLeft", texture, 0, wall_extents_x.x, wall_extents_z.y, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th);
			AddWallMain(mesh, "DoorwayRight", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.y, wall_extents_z.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th);
			AddFloorMain(mesh, "DoorwayTop", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y, wall_extents_y.y, tw, th);
		}
		ResetWalls();
	}
	return index;
}

void SBS::SetAutoSize(bool x, bool y)
{
	//enable or disable texture autosizing
	AutoX = x;
	AutoY = y;
}

csVector2 SBS::GetAutoSize()
{
	return csVector2(AutoX, AutoY);
}

void SBS::ReverseAxis(bool value)
{
	//reverse wall/floor altitude axis
	ReverseAxisValue = value;
}

bool SBS::GetReverseAxis()
{
	return ReverseAxisValue;
}

void SBS::SetListenerLocation(csVector3 location)
{
	//set position of sound listener object
	sndrenderer->GetListener()->SetPosition(location);
}

void SBS::SetListenerDirection(csVector3 front, csVector3 top)
{
	//set direction of sound listener object
	sndrenderer->GetListener()->SetDirection(front, top);
}

void SBS::SetListenerDistanceFactor(float factor)
{
	sndrenderer->GetListener()->SetDistanceFactor(factor);
}

float SBS::GetListenerDistanceFactor()
{
	return sndrenderer->GetListener()->GetDistanceFactor();
}

void SBS::SetListenerRollOffFactor(float factor)
{
	sndrenderer->GetListener()->SetRollOffFactor(factor);
}

float SBS::GetListenerRollOffFactor()
{
	return sndrenderer->GetListener()->GetRollOffFactor();
}

void SBS::SetTextureOverride(const char *mainneg, const char *mainpos, const char *sideneg, const char *sidepos, const char *top, const char *bottom)
{
	//set override textures and enable override
	mainnegtex = mainneg;
	mainnegtex.Trim();
	mainpostex = mainpos;
	mainpostex.Trim();
	sidenegtex = sideneg;
	sidenegtex.Trim();
	sidepostex = sidepos;
	sidepostex.Trim();
	toptex = top;
	toptex.Trim();
	bottomtex = bottom;
	bottomtex.Trim();
	TextureOverride = true;
}

void SBS::SetTextureFlip(int mainneg, int mainpos, int sideneg, int sidepos, int top, int bottom)
{
	//flip a texture on a specified side either horizontally or vertically (or both)
	//parameters are:
	//0 = no flipping
	//1 = flip horizontally
	//2 = flip vertically
	//3 = flip both

	mainnegflip = mainneg;
	mainposflip = mainpos;
	sidenegflip = sideneg;
	sideposflip = sidepos;
	topflip = top;
	bottomflip = bottom;
	FlipTexture = true;
}

int SBS::AddWall(const char *meshname, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float altitude1, float altitude2, float tw, float th)
{
	//meshname can either be:
	//external, landscape, or buildings

	//Adds a wall with the specified dimensions
	float tw2 = tw;
	float th2;
	float tempw1;
	float tempw2;
	csString mesh = meshname;
	mesh.Trim();

	//Set horizontal scaling
	x1 = x1 * HorizScale;
	x2 = x2 * HorizScale;
	z1 = z1 * HorizScale;
	z2 = z2 * HorizScale;

	//Call texture autosizing formulas
	if (z1 == z2)
		tw2 = AutoSize(x1, x2, true, tw);
	if (x1 == x2)
		tw2 = AutoSize(z1, z2, true, tw);
	if ((z1 != z2) && (x1 != x2))
	{
		//calculate diagonals
		if (x1 > x2)
			tempw1 = x1 - x2;
		else
			tempw1 = x2 - x1;
		if (z1 > z2)
			tempw2 = z1 - z2;
		else
			tempw2 = z2 - z1;
		tw2 = AutoSize(0, sqrt(pow(tempw1, 2) + pow(tempw2, 2)), true, tw);
	}
	th2 = AutoSize(0, height_in1, false, th);

	csRef<iThingFactoryState> tmpstate;
	if (mesh.CompareNoCase("external") == true)
		tmpstate = External_state;
	if (mesh.CompareNoCase("buildings") == true)
		tmpstate = Buildings_state;
	if (mesh.CompareNoCase("landscape") == true)
		tmpstate = Landscape_state;

	return AddWallMain(tmpstate, name, texture, thickness, x1, z1, x2, z2, height_in1, height_in2, altitude1, altitude2, tw2, th2);
}

int SBS::AddFloor(const char *meshname, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float altitude1, float altitude2, float tw, float th)
{
	//meshname can either be:
	//external, landscape, or buildings

	//Adds a floor with the specified dimensions and vertical offset
	float tw2;
	float th2;
	csString mesh = meshname;
	mesh.Trim();

	//Set horizontal scaling
	x1 = x1 * HorizScale;
	x2 = x2 * HorizScale;
	z1 = z1 * HorizScale;
	z2 = z2 * HorizScale;

	//Call texture autosizing formulas
	tw2 = AutoSize(x1, x2, true, tw);
	th2 = AutoSize(z1, z2, false, th);

	csRef<iThingFactoryState> tmpstate;
	if (mesh.CompareNoCase("external") == true)
		tmpstate = External_state;
	if (mesh.CompareNoCase("buildings") == true)
		tmpstate = Buildings_state;
	if (mesh.CompareNoCase("landscape") == true)
		tmpstate = Landscape_state;

	return AddFloorMain(tmpstate, name, texture, thickness, x1, z1, x2, z2, altitude1, altitude2, tw2, th2);
}

int SBS::AddGround(const char *name, const char *texture, float x1, float z1, float x2, float z2, float altitude, int tile_x, int tile_z)
{
	//Adds ground based on a tiled-floor layout, with the specified dimensions and vertical offset
	//this does not support thickness

	csVector3 v1, v2, v3, v4;

	float minx, minz, maxx, maxz;

	//get min and max values
	if (x1 < x2)
	{
		minx = x1;
		maxx = x2;
	}
	else
	{
		minx = x2;
		maxx = x1;
	}
	if (z1 < z2)
	{
		minz = z1;
		maxz = z2;
	}
	else
	{
		minz = z2;
		maxz = z1;
	}

	int index = -1;
	int tmpindex = -1;

	//create polygon tiles
	for (float i = minx; i < maxx; i += tile_x)
	{
		int sizex, sizez;

		if (i + tile_x > maxx)
			sizex = maxx - i;
		else
			sizex = tile_x;

		for (float j = minz; j < maxz; j += tile_z)
		{
			if (j + tile_z > maxz)
				sizez = maxz - i;
			else
				sizez = tile_z;

			v1.Set(i + sizex, altitude, j + sizez); //bottom right
			v2.Set(i, altitude, j + sizez); //bottom left
			v3.Set(i, altitude, j); //top left
			v4.Set(i + sizex, altitude, j); //top right

			tmpindex = Landscape_state->AddQuad(v4, v3, v2, v1);
			Landscape_state->SetPolygonName(csPolygonRange(tmpindex, tmpindex), name);
			if (tmpindex > index && index == -1)
				index = tmpindex;

			//set texture
			SetTexture(Landscape_state, tmpindex, texture, false, 1, 1);
		}
	}

	return index;
}

void SBS::EnableFloorRange(int floor, int range, bool value, bool enablegroups, int shaftnumber)
{
	//turn on/off a range of floors
	//if range is 3, show shaft on current floor (floor), and 1 floor below and above (3 total floors)
	//if range is 1, show only the current floor (floor)

	//range must be greater than 0
	if (range < 1)
		range = 1;

	//range must be an odd number; if it's even, then add 1
	if (IsEven(range) == true)
		range++;

	int additionalfloors;
	if (range > 1)
		additionalfloors = (range - 1) / 2;
	else
		additionalfloors = 0;

	//disable floors 1 floor outside of range
	if (value == true)
	{
		if (floor - additionalfloors - 1 >= -Basements && floor - additionalfloors - 1 < Floors)
			GetFloor(floor - additionalfloors - 1)->Enabled(false);
		if (floor + additionalfloors + 1 >= -Basements && floor + additionalfloors + 1 < Floors)
			GetFloor(floor + additionalfloors + 1)->Enabled(false);
	}

	//enable floors within range
	for (int i = floor - additionalfloors; i <= floor + additionalfloors; i++)
	{
		if (i >= -Basements && i < Floors)
		{
			if (shaftnumber > 0)
			{
				//if a shaft is specified, only show the floor if it is in the related shaft's ShowFloorsList array
				if (GetShaft(shaftnumber)->ShowFloors == true)
				{
					if (GetShaft(shaftnumber)->ShowFloorsList.Find(i) != -1 && value == true)
					{
						GetFloor(i)->Enabled(true);
						if (enablegroups == true)
							GetFloor(i)->EnableGroup(true);
					}
					else
					{
						GetFloor(i)->Enabled(false);
						if (enablegroups == true)
							GetFloor(i)->EnableGroup(false);
					}
				}
			}
			else
			{
				GetFloor(i)->Enabled(value);
				if (enablegroups == true)
					GetFloor(i)->EnableGroup(value);
			}
		}
	}
}

bool SBS::RegisterDoorCallback(Door *door)
{
	//register a door object for callbacks (used for door movement)
	if (!callbackdoor)
	{
		callbackdoor = door;
		return true;
	}
	else if (callbackdoor == door && callbackdoor->IsMoving == true)
		callbackdoor->OpenDoor = !callbackdoor->OpenDoor;
	else
	{
		Report("Door in use; cannot register callback");
		return false;
	}
}

bool SBS::UnregisterDoorCallback(Door *door)
{
	//unregister existing door callback
	if (callbackdoor->IsMoving == false && door == callbackdoor)
	{
		callbackdoor = 0;
		return true;
	}
	else
	{
		Report("Door in use; cannot unregister callback");
		return false;
	}
}

void SBS::ProcessTextureFlip(float tw, float th)
{
	//process texture flip info
	for (int i = 0; i <= 5; i++)
	{
		widthscale[i] = tw;
		heightscale[i] = th;
	}
	
	//texture flipping
	if (FlipTexture == true)
	{
		int *info;
		for (int i = 0; i <= 5; i++)
		{
			if (i == 0)
				info = &mainnegflip;
			if (i == 1)
				info = &mainposflip;
			if (i == 2)
				info = &sidenegflip;
			if (i == 3)
				info = &sideposflip;
			if (i == 4)
				info = &topflip;
			if (i == 5)
				info = &bottomflip;

			if (*info == 1 || *info == 3)
				widthscale[i] = -tw;
			if (*info == 2 || *info == 3)
				heightscale[i] = -th;
		}
	}
}
