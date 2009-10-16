/* $Id$ */

/*
	Scalable Building Simulator - Core
	The Skyscraper Project - Version 1.5 Alpha
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

	//initialize other variables
	BuildingName = "";
	BuildingDesigner = "";
	BuildingLocation = "";
	BuildingDescription = "";
	BuildingVersion = "";
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
	FrameRate = 30;
	FrameLimiter = false;
	AutoShafts = true;
	AutoStairs = true;
	ElevatorSync = false;
	ElevatorNumber = 1;
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
	delta = 0.01f;
	wall1a = false;
	wall1b = false;
	wall2a = false;
	wall2b = false;
	AutoX = true;
	AutoY = true;
	ReverseAxisValue = false;
	TextureOverride = false;
	ProcessElevators = true;
	FlipTexture = false;
	mainnegflip = 0;
	mainposflip = 0;
	sidenegflip = 0;
	sideposflip = 0;
	topflip = 0;
	bottomflip = 0;
	widthscale.SetSize(6);
	heightscale.SetSize(6);
	remaining_delta = 0;
	start_time = 0;
	InShaft = false;
	DisableSound = false;
	MapIndex.SetSize(3);
	MapUV.SetSize(3);
	OldMapIndex.SetSize(3);
	OldMapUV.SetSize(3);
	for (int i = 0; i <= 2; i++)
	{
		MapIndex[i] = 0;
		OldMapIndex[i] = 0;
		MapUV[i] = 0;
		OldMapUV[i] = 0;
	}
	ResetTextureMapping(true); //set default texture map values
}

SBS::~SBS()
{
	//engine destructor

	//delete camera object
	if (camera)
		delete camera;
	camera = 0;

	//delete callbacks
	doorcallbacks.DeleteAll();
	buttoncallbacks.DeleteAll();

	//delete floors
	for (int i = 0; i < FloorArray.GetSize(); i++)
	{
		if (FloorArray[i].object)
			delete FloorArray[i].object;
		FloorArray[i].object = 0;
	}
	FloorArray.DeleteAll();

	//delete elevators
	for (int i = 0; i < ElevatorArray.GetSize(); i++)
	{
		if (ElevatorArray[i].object)
			delete ElevatorArray[i].object;
		ElevatorArray[i].object = 0;
	}
	ElevatorArray.DeleteAll();

	//delete shafts
	for (int i = 0; i < ShaftArray.GetSize(); i++)
	{
		if (ShaftArray[i].object)
			delete ShaftArray[i].object;
		ShaftArray[i].object = 0;
	}
	ShaftArray.DeleteAll();

	//delete stairs
	for (int i = 0; i < StairsArray.GetSize(); i++)
	{
		if (StairsArray[i].object)
			delete StairsArray[i].object;
		StairsArray[i].object = 0;
	}
	StairsArray.DeleteAll();

	//clear self reference
	sbs = 0;
}

bool SBS::Start()
{
	//Post-init startup code goes here, before the runloop

	//initialize mesh colliders
	csColliderHelper::InitializeCollisionWrappers(collision_sys, engine);

	//initialize camera/actor
	camera->CreateColliders();

	//move camera to start location
	camera->SetToStartPosition();
	camera->SetToStartDirection();
	camera->SetToStartRotation();

	//set sound listener object to initial position
	if (DisableSound == false)
	{
		if (sndrenderer->GetListener())
			SetListenerLocation(camera->GetPosition());
		else
		{
			ReportError("Sound listener object not available. Sound support disabled");
			DisableSound = true;
		}
	}

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
			if (ShaftArray[i].object->ShowFullShaft == false)
			{
				ShaftArray[i].object->EnableWholeShaft(false, true);
				//enable extents
				ShaftArray[i].object->Enabled(ShaftArray[i].object->startfloor, true, true);
				ShaftArray[i].object->Enabled(ShaftArray[i].object->endfloor, true, true);
			}
			else
				ShaftArray[i].object->EnableWholeShaft(true, true, true);
		}
	}

	//turn off stairwells
	for (int i = 0; i < StairsNum(); i++)
	{
		if (StairsArray[i].object)
			StairsArray[i].object->EnableWholeStairwell(false);
	}

	//init elevators
	for (int i = 0; i < Elevators(); i++)
	{
		if (ElevatorArray[i].object)
		{
			//turn on shaft doors
			ElevatorArray[i].object->ShaftDoorsEnabled(0, camera->StartFloor, true);
			ElevatorArray[i].object->ShaftDoorsEnabled(0, GetShaft(ElevatorArray[i].object->AssignedShaft)->startfloor, true);
			ElevatorArray[i].object->ShaftDoorsEnabled(0, GetShaft(ElevatorArray[i].object->AssignedShaft)->endfloor, true);
			//turn off directional indicators
			ElevatorArray[i].object->EnableDirectionalIndicators(false);
			//turn on indicators for current floor
			ElevatorArray[i].object->EnableDirectionalIndicator(camera->StartFloor, true);
			//disable objects
			ElevatorArray[i].object->EnableObjects(false);
		}
	}

	//turn on start floor
	GetFloor(camera->StartFloor)->Enabled(true);
	GetFloor(camera->StartFloor)->EnableGroup(true);

	return true;
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
	csPrintf("\n Scalable Building Simulator 0.5 Alpha\n");
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

	//calculate start and running time
	if (start_time == 0)
		start_time = vc->GetCurrentTicks() / 1000.0;
	running_time = (vc->GetCurrentTicks() / 1000.0) - start_time;

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
			ProcessDoors();

			//process call button callbacks
			ProcessCallButtons();

			//process misc operations on current floor
			GetFloor(camera->CurrentFloor)->Loop();

			//process auto areas
			CheckAutoAreas();

			//check if the user is outside (no code yet)

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

bool SBS::Initialize(iSCF* scf, iObjectRegistry* objreg, iView* view, iSector* sector, const char* rootdirectory, const char* directory_char)
{
	//initialize CS references
#ifdef _WIN32
	iSCF::SCF = scf;
#endif
	object_reg = objreg;
	engine = csQueryRegistry<iEngine> (object_reg);
	if (!engine)
		return ReportError("No engine plugin found");
	g3d = csQueryRegistry<iGraphics3D> (object_reg);
	if (!g3d)
		return ReportError("No 3D renderer plugin found");
	g2d = csQueryRegistry<iGraphics2D> (object_reg);
	if (!g2d)
		return ReportError("No 2D plugin found");
	loader = csQueryRegistry<iLoader> (object_reg);
	if (!loader)
		return ReportError("No loader plugin found");
	vc = csQueryRegistry<iVirtualClock> (object_reg);
	if (!vc)
		return ReportError("No virtual clock plugin found");
	vfs = csQueryRegistry<iVFS> (object_reg);
	if (!vfs)
		return ReportError("No VFS plugin found");
	collision_sys = csQueryRegistry<iCollideSystem> (object_reg);
	if (!collision_sys)
		return ReportError("No collision plugin found");
	rep = csQueryRegistry<iReporter> (object_reg);
	if (!rep)
		return ReportError("No reporter plugin found");
	sndrenderer = csQueryRegistry<iSndSysRenderer> (object_reg);
	if (!sndrenderer)
		return ReportError("No sound renderer plugin found");
	sndloader = csQueryRegistry<iSndSysLoader> (object_reg);
	if (!sndloader)
		return ReportError("No sound loader plugin found");
	confman = csQueryRegistry<iConfigManager> (object_reg);
	if (!confman)
		return ReportError("No configuration manager plugin found");
	this->view = view;
	if (!this->view)
		return ReportError("No iView object available");

	//disable sound if renderer or loader are not available
	if (!sndrenderer || !sndloader)
		DisableSound = true;

	area = sector;
	if (!area)
		return ReportError("No iSector object available");

	root_dir = rootdirectory;
	dir_char = directory_char;

	//load SBS configuration file
	//confman->AddDomain("/root/data/config/sbs.cfg", vfs, confman->ConfigPriorityApplication);

	//load default values from config file
	HorizScale = confman->GetFloat("Skyscraper.SBS.HorizScale", 1); //Set default horizontal scaling value
	SkyName = confman->GetStr("Skyscraper.SBS.SkyName", "noon");
	AutoShafts = confman->GetBool("Skyscraper.SBS.AutoShafts", true);
	AutoStairs = confman->GetBool("Skyscraper.SBS.AutoStairs", true);
	ShowFullShafts = confman->GetBool("Skyscraper.SBS.ShowFullShafts", false);
	ShowFullStairs = confman->GetBool("Skyscraper.SBS.ShowFullStairs", false);
	ShaftDisplayRange = confman->GetInt("Skyscraper.SBS.ShaftDisplayRange", 3);
	StairsDisplayRange = confman->GetInt("Skyscraper.SBS.StairsDisplayRange", 5);
	ShaftOutsideDisplayRange = confman->GetInt("Skyscraper.SBS.ShaftOutsideDisplayRange", 3);
	StairsOutsideDisplayRange = confman->GetInt("Skyscraper.SBS.StairsOutsideDisplayRange", 3);
	FloorDisplayRange = confman->GetInt("Skyscraper.SBS.FloorDisplayRange", 3);
	ProcessElevators = confman->GetBool("Skyscraper.SBS.ProcessElevators", true);
	DisableSound = confman->GetBool("Skyscraper.SBS.DisableSound", false);

	//mount sign texture packs
	Mount("signs-sans.zip", "/root/signs/sans");
	Mount("signs-sans_bold.zip", "/root/signs/sans_bold");
	Mount("signs-sans_cond.zip", "/root/signs/sans_cond");
	Mount("signs-sans_cond_bold.zip", "/root/signs/sans_cond_bold");

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
	// Load a texture
	if (!loader->LoadTexture(name, filename, CS_TEXTURE_3D, 0, true, true, false))
		return ReportError("Error loading texture");

	TextureInfo info;
	info.name = name;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	textureinfo.Push(info);
	return true;
}

bool SBS::UnloadTexture(const char *name)
{
	//unloads a texture

	csRef<iTextureWrapper> wrapper = engine->GetTextureList()->FindByName(name);
	if (!wrapper)
		return false;
	if (!engine->GetTextureList()->Remove(wrapper))
		return false;
	return true;
}

bool SBS::LoadTextureCropped(const char *filename, const char *name, int x, int y, int width, int height, float widthmult, float heightmult)
{
	//loads only a portion of the specified texture

	iTextureManager *tm = g3d->GetTextureManager();

	//load image
	csRef<iImage> image = loader->LoadImage(filename, tm->GetTextureFormat());

	if (!image)
		return ReportError("LoadTextureCropped: Error loading image");

	//set default values if specified
	if (x == -1)
		x = 0;
	if (y == -1)
		y = 0;
	if (width < 1)
		width = image->GetWidth();
	if (height < 1)
		height = image->GetHeight();

	if (x > image->GetWidth() || y > image->GetHeight())
		return ReportError("LoadTextureCropped: invalid coordinates");
	if (x + width > image->GetWidth() || y + height > image->GetHeight())
		return ReportError("LoadTextureCropped: invalid size");

	//crop image
	csRef<iImage> cropped_image = csImageManipulate::Crop(image, x, y, width, height);
	if (!cropped_image)
		return ReportError("LoadTextureCropped: Error cropping image");

	//register texture
	csRef<iTextureHandle> handle = tm->RegisterTexture(cropped_image, CS_TEXTURE_3D);
	if (!handle)
		return ReportError("LoadTextureCropped: Error registering texture");

	//create texture wrapper
	csRef<iTextureWrapper> wrapper = engine->GetTextureList()->NewTexture(handle);
	wrapper->QueryObject()->SetName(name);

	//create material
	csRef<iMaterial> material (engine->CreateBaseMaterial(wrapper));
	csRef<iMaterialWrapper> matwrapper = engine->GetMaterialList()->NewMaterial(material, name);

	wrapper->SetImageFile(cropped_image);

	//add texture multipliers for new texture
	TextureInfo info;
	info.name = name;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	textureinfo.Push(info);

	return true;
}

bool SBS::AddTextToTexture(const char *origname, const char *name, const char *font_filename, float font_size, const char *text, int x1, int y1, int x2, int y2, const char *h_align, const char *v_align, int ColorR, int ColorG, int ColorB)
{
	//adds text to the named texture, in the given box coordinates and alignment

	//h_align is either "left", "right" or "center" - default is center
	//v_align is either "top", "bottom", or "center" - default is center

	//if either x1 or y1 are -1, the value of 0 is used.
	//If either x2 or y2 are -1, the width or height of the texture is used.

	//load font
	csRef<iFont> font = g2d->GetFontServer()->LoadFont(font_filename, font_size);
	if (!font)
	{
		ReportError("AddTextToTexture: Invalid font");
		return false;
	}

	//get original texture
	csRef<iTextureWrapper> wrapper = engine->GetTextureList()->FindByName(origname);
	if (!wrapper)
	{
		ReportError("AddTextToTexture: Invalid original texture");
		return false;
	}

	//get texture tiling info
	float widthmult, heightmult;
	GetTextureTiling(origname, widthmult, heightmult);

	//get height and width of texture
	int width, height;
	wrapper->GetTextureHandle()->GetOriginalDimensions(width, height);

	//create new empty texture
	csRef<iTextureHandle> th = g3d->GetTextureManager()->CreateTexture(width, height, csimg2D, "rgb8", CS_TEXTURE_3D);
	if (!th)
	{
		ReportError("AddTextToTexture: Error creating texture");
		th = 0;
		return false;
	}

	//get new texture dimensions, if it was resized
	th->GetOriginalDimensions(width, height);

	//create a texture wrapper for the new texture
	csRef<iTextureWrapper> tex = engine->GetTextureList()->NewTexture(th);
	if (!tex)
	{
		ReportError("AddTextToTexture: Error creating texture wrapper");
		th = 0;
		return false;
	}
	
	//set texture name
	tex->QueryObject()->SetName(name);

	//create material
	csRef<iMaterial> material (engine->CreateBaseMaterial(tex));
	csRef<iMaterialWrapper> matwrapper = engine->GetMaterialList()->NewMaterial(material, name);
	
	//add texture multipliers for new texture
	TextureInfo info;
	info.name = name;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	textureinfo.Push(info);

	//set default values if specified
	if (x1 == -1)
		x1 = 0;
	if (y1 == -1)
		y1 = 0;
	if (x2 == -1)
		x2 = width;
	if (y2 == -1)
		y2 = height;

	iTextureHandle *handle = tex->GetTextureHandle();
	if (!handle)
	{
		ReportError("AddTextToTexture: No texture handle available");
		return false;
	}

	//set graphics rendering to the texture image
	g3d->SetRenderTarget(handle);
	if (!g3d->ValidateRenderTargets())
		return false;
	if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS)) return false;

	//draw original image onto backbuffer
	g3d->DrawPixmap(wrapper->GetTextureHandle(), 0, 0, width, height, 0, 0, width, height);

	//get texture size info
	int x, y, w, h;
	font->GetDimensions(text, w, h);

	//horizontal alignment
	if (h_align == "left")
		x = x1;
	else if (h_align == "right")
		x = x2 - w;
	else //center
		x = x1 + ((x2 - x1) >> 1) - (w >> 1);

	//vertical alignment
	if (v_align == "top")
		y = y1;
	else if (v_align == "bottom")
		y = y2 - h;
	else //center
		y = y1 + ((y2 - y1) >> 1) - (h >> 1);

	//write text
	g2d->Write(font, x, y, g2d->FindRGB(ColorR, ColorG, ColorB), -1, text);

	//finish with buffer
	g3d->FinishDraw();

	return true;
}

bool SBS::AddTextureOverlay(const char *orig_texture, const char *overlay_texture, const char *name, int x, int y, int width, int height, float widthmult, float heightmult)
{
	//draws the specified texture on top of another texture
	//orig_texture is the original texture to use; overlay_texture is the texture to draw on top of it

	//get original texture
	csRef<iImage> image1 = engine->GetTextureList()->FindByName(orig_texture)->GetImageFile();
	if (!image1)
	{
		ReportError("AddTextureOverlay: Invalid original texture");
		return false;
	}

	//get overlay texture
	csRef<iImage> image2 = engine->GetTextureList()->FindByName(overlay_texture)->GetImageFile();
	if (!image2)
	{
		ReportError("AddTextureOverlay: Invalid overlay texture");
		return false;
	}

	//set default values if specified
	if (x == -1)
		x = 0;
	if (y == -1)
		y = 0;
	if (width < 1)
		width = image2->GetWidth();
	if (height < 1)
		height = image2->GetHeight();

	if (x > image1->GetWidth() || y > image1->GetHeight())
		return ReportError("AddTextureOverlay: invalid coordinates");
	if (x + width > image1->GetWidth() || y + height > image1->GetHeight())
		return ReportError("AddTextureOverlay: invalid size");

	//copy overlay image onto source image
	csRef<csImageMemory> imagemem;
	imagemem.AttachNew(new csImageMemory(image1));

	imagemem->CopyScale(image2, x, y, width, height);

	//register new texture
	csRef<iTextureHandle> handle = g3d->GetTextureManager()->RegisterTexture(imagemem, CS_TEXTURE_3D);
	if (!handle)
		return ReportError("AddTextureOverlay: Error registering texture");

	//create texture wrapper
	csRef<iTextureWrapper> wrapper = engine->GetTextureList()->NewTexture(handle);
	wrapper->QueryObject()->SetName(name);

	//create material
	csRef<iMaterial> material (engine->CreateBaseMaterial(wrapper));
	csRef<iMaterialWrapper> matwrapper = engine->GetMaterialList()->NewMaterial(material, name);

	wrapper->SetImageFile(imagemem);

	//add texture multipliers for new texture
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

	//convert to clockwise coordinates if on x-axis, or counterclockwise if on z-axis
	if ((x1 > x2 && axis == 1) || (z1 < z2 && axis == 2))
	{
		//reverse coordinates
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
		if (axis == 1)
			tmpindex = dest->AddQuad(v5, v1, v4, v8); //left wall
		else
			tmpindex = dest->AddQuad(v2, v6, v7, v3); //left wall
		NewName = name;
		NewName.Append(":left");
		dest->SetPolygonName(csPolygonRange(tmpindex, tmpindex), NewName);
	}
	if (tmpindex > index && index == -1)
		index = tmpindex;

	if (DrawSideP == true)
	{
		if (axis == 1)
			tmpindex = dest->AddQuad(v2, v6, v7, v3); //right wall
		else
			tmpindex = dest->AddQuad(v5, v1, v4, v8); //right wall
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

	//set texture
	if (TextureOverride == false && FlipTexture == false)
		SetTexture(dest, index, texture, true, tw, th);
	else
	{
		ProcessTextureFlip(tw, th);
		int endindex = index + GetDrawWallsCount();
		if (TextureOverride == true)
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
		v2.Set(x1, altitude1, z2); //top left
		v3.Set(x2, altitude2, z2); //top right
		v4.Set(x2, altitude2, z1); //bottom right
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
		if (TextureOverride == true)
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
	bool result;
	iMaterialWrapper* tm = GetTextureMaterial(texture, result);
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
	Buildings->SetRenderPriority(sbs->engine->GetObjectRenderPriority());

	External = engine->CreateSectorWallsMesh (area, "External");
	External_state = scfQueryInterface<iThingFactoryState> (External->GetMeshObject()->GetFactory());
	External->SetZBufMode(CS_ZBUF_USE);
	External->SetRenderPriority(engine->GetObjectRenderPriority());

	Landscape = engine->CreateSectorWallsMesh (area, "Landscape");
	Landscape_state = scfQueryInterface<iThingFactoryState> (Landscape->GetMeshObject()->GetFactory());
	Landscape->SetZBufMode(CS_ZBUF_USE);
	Landscape->SetRenderPriority(sbs->engine->GetObjectRenderPriority());
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

	csString polyname = dest->GetPolygonName(firstidx);
	csString texname = texture;
	bool result;
	csRef<iMaterialWrapper> material = GetTextureMaterial(texture, result, polyname.GetData());
	if (!result)
		texname = "Default";
	dest->SetPolygonMaterial (csPolygonRange(firstidx, firstidx + 1), material);

	float tw3 = tw2, th3 = th2;
	float mw, mh;
	if (GetTextureTiling(texname.GetData(), mw, mh))
	{
		//multiply the tiling parameters (tw and th) by
		//the stored multipliers for that texture
		tw3 = tw2 * mw;
		th3 = th2 * mh;
	}

	//UV texture mapping
	for (int i = firstidx; i <= firstidx + 1; i++)
	{
		dest->SetPolygonTextureMapping (csPolygonRange(i, i),
			dest->GetPolygonVertex(i, MapIndex[0]),
			csVector2(MapUV[0].x * tw3, MapUV[0].y * th3),
			dest->GetPolygonVertex(i, MapIndex[1]),
			csVector2(MapUV[1].x * tw3, MapUV[1].y * th3),
			dest->GetPolygonVertex(i, MapIndex[2]),
			csVector2(MapUV[2].x * tw3, MapUV[2].y * th3));
	}

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

	csString polyname = dest->GetPolygonName(firstidx);
	csString texname = texture;
	bool result;
	csRef<iMaterialWrapper> material = GetTextureMaterial(texture, result, polyname.GetData());
	if (!result)
		texname = "Default";
	dest->SetPolygonMaterial (csPolygonRange(firstidx, firstidx + 1), material);

	float tw3 = tw2, th3 = th2;
	float mw, mh;
	if (GetTextureTiling(texname.GetData(), mw, mh))
	{
		//multiply the tiling parameters (tw and th) by
		//the stored multipliers for that texture
		tw3 = tw2 * mw;
		th3 = th2 * mh;
	}

	//UV texture mapping
	for (int i = firstidx; i <= firstidx + 1; i++)
	{
		dest->SetPolygonTextureMapping (csPolygonRange(i, i),
			dest->GetPolygonVertex(i, MapIndex[0]),
			csVector2(MapUV[0].x * tw3, MapUV[0].y * th3),
			dest->GetPolygonVertex(i, MapIndex[1]),
			csVector2(MapUV[1].x * tw3, MapUV[1].y * th3),
			dest->GetPolygonVertex(i, MapIndex[2]),
			csVector2(MapUV[2].x * tw3, MapUV[2].y * th3));
	}

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
	EnableMesh(Buildings, value);
	IsBuildingsEnabled = value;
}

void SBS::EnableLandscape(bool value)
{
	//turns landscape on/off
	EnableMesh(Landscape, value);
	IsLandscapeEnabled = value;
}

void SBS::EnableExternal(bool value)
{
	//turns external on/off
	EnableMesh(External, value);
	IsExternalEnabled = value;
}

void SBS::EnableSkybox(bool value)
{
	//turns skybox on/off
	EnableMesh(SkyBox, value);
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
	SkyBox->SetZBufMode(CS_ZBUF_NONE);
	SkyBox->SetRenderPriority(sbs->engine->GetSkyRenderPriority());

	//create a skybox that extends 30 miles (30 * 5280 ft) in each direction
	int firstidx = SkyBox_state->AddInsideBox(csVector3(-158400, -158400, -158400), csVector3(158400, 158400, 158400));
	bool result;
	csRef<iMaterialWrapper> material = GetTextureMaterial("SkyBack", result);
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx, firstidx), material);
	material = GetTextureMaterial("SkyRight", result);
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 1, firstidx + 1), material);
	material = GetTextureMaterial("SkyFront", result);
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 2, firstidx + 2), material);
	material = GetTextureMaterial("SkyLeft", result);
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 3, firstidx + 3), material);
	material = GetTextureMaterial("SkyBottom", result);
	SkyBox_state->SetPolygonMaterial (csPolygonRange(firstidx + 4, firstidx + 4), material);
	material = GetTextureMaterial("SkyTop", result);
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
		float lastfloor_altitude = GetFloor(lastfloor)->Altitude;
		float upperfloor_altitude;
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

iMaterialWrapper* SBS::ChangeTexture(iMeshWrapper *mesh, const char *texture, bool matcheck)
{
	//changes a texture - works with genmeshes only
	//if matcheck is true, exit if old and new textures are the same

	//exit if mesh pointer's invalid
	if (!mesh)
		return 0;

	//get new material
	csRef<iMaterialWrapper> newmat = engine->GetMaterialList()->FindByName(texture);

	//exit if old and new materials are the same
	if (matcheck == true)
	{
		if (mesh->GetMeshObject()->GetMaterialWrapper() == newmat)
			return 0;
	}

	//set material if valid
	if (newmat)
	{
		mesh->GetMeshObject()->SetMaterialWrapper(newmat);
		return newmat;
	}
	else //otherwise report error
		ReportError("ChangeTexture: Invalid texture '" + csString(texture) + "'");

	return 0;
}

void SBS::SetTexture(csRef<iThingFactoryState> mesh, int index, const char *texture, bool has_thickness, float tw, float th)
{
	//sets texture for a range of polygons

	csString texname = texture;
	bool result;
	csRef<iMaterialWrapper> material = GetTextureMaterial(texture, result, mesh->GetPolygonName(index));
	if (!result)
		texname = "Default";

	if (tw == 0)
		tw = 1;
	if (th == 0)
		th = 1;

	float tw2 = tw, th2 = th;

	float mw, mh;
	if (GetTextureTiling(texname.GetData(), mw, mh))
	{
		//multiply the tiling parameters (tw and th) by
		//the stored multipliers for that texture
		tw2 = tw * mw;
		th2 = th * mh;
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
			//set UV texture mapping
			mesh->SetPolygonTextureMapping (csPolygonRange(i, i),
				mesh->GetPolygonVertex(i, MapIndex[0]),
				csVector2 (MapUV[0].x * tw2, MapUV[0].y * th2),
				mesh->GetPolygonVertex(i, MapIndex[1]),
				csVector2 (MapUV[1].x * tw2, MapUV[1].y * th2),
				mesh->GetPolygonVertex(i, MapIndex[2]),
				csVector2 (MapUV[2].x * tw2, MapUV[2].y * th2));
		}
	}
	else
	{
			mesh->SetPolygonMaterial(csPolygonRange(index, index), material);
			//set UV texture mapping
			mesh->SetPolygonTextureMapping (csPolygonRange(index, index),
				mesh->GetPolygonVertex(index, MapIndex[0]),
				csVector2 (MapUV[0].x * tw2, MapUV[0].y * th2),
				mesh->GetPolygonVertex(index, MapIndex[1]),
				csVector2 (MapUV[1].x * tw2, MapUV[1].y * th2),
				mesh->GetPolygonVertex(index, MapIndex[2]),
				csVector2 (MapUV[2].x * tw2, MapUV[2].y * th2));
	}
}

bool SBS::NewElevator(int number)
{
	//create a new elevator object
	for (size_t i = 0; i < ElevatorArray.GetSize(); i++)
		if (ElevatorArray[i].number == number)
			return false;
	ElevatorArray.SetSize(ElevatorArray.GetSize() + 1);
	ElevatorArray[ElevatorArray.GetSize() - 1].number = number;
	ElevatorArray[ElevatorArray.GetSize() - 1].object = new Elevator(number);
	return true;
}

bool SBS::NewFloor(int number)
{
	//create a new floor object
	for (size_t i = 0; i < FloorArray.GetSize(); i++)
		if (FloorArray[i].number == number)
			return false;
	FloorArray.SetSize(FloorArray.GetSize() + 1);
	FloorArray[FloorArray.GetSize() - 1].number = number;
	FloorArray[FloorArray.GetSize() - 1].object = new Floor(number);

	if (number < 0)
		Basements++;
	else
		Floors++;
	return true;
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

Floor* SBS::GetFloor(int number)
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
		else if (number < 0)
		{
			if (FloorArray[-(number + 1)].number == number)
			{
				if (FloorArray[-(number + 1)].object)
					return FloorArray[-(number + 1)].object;
				else
					return 0;
			}
		}
	}

	for (size_t i = 0; i < FloorArray.GetSize(); i++)
		if (FloorArray[i].number == number)
			return FloorArray[i].object;
	return 0;
}

Elevator* SBS::GetElevator(int number)
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

Shaft* SBS::GetShaft(int number)
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

Stairs* SBS::GetStairs(int number)
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

bool SBS::SetWallOrientation(const char *direction)
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
	else if (temp == "center")
		wall_orientation = 1;
	else if (temp == "right")
		wall_orientation = 2;
	else
		return false;
	return true;
}

int SBS::GetWallOrientation()
{
	return wall_orientation;
}

bool SBS::SetFloorOrientation(const char *direction)
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
	else if (temp == "center")
		floor_orientation = 1;
	else if (temp == "top")
		floor_orientation = 2;
	else
		return false;
	return true;
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

void SBS::SetTextureMapping(int vertindex1, csVector2 uv1, int vertindex2, csVector2 uv2, int vertindex3, csVector2 uv3)
{
	//Manually sets UV texture mapping.  Use ResetTextureMapping to return to default values
	//backup old values
	for (int i = 0; i <= 2; i++)
	{
		OldMapIndex[i] = MapIndex[i];
		OldMapUV[i] = MapUV[i];
	}

	//set new values
	MapIndex[0] = vertindex1;
	MapIndex[1] = vertindex2;
	MapIndex[2] = vertindex3;
	MapUV[0] = uv1;
	MapUV[1] = uv2;
	MapUV[2] = uv3;
}

void SBS::ResetTextureMapping(bool todefaults)
{
	//Resets UV texture mapping to defaults or previous values
	if (todefaults == true)
		SetTextureMapping(0, csVector2(0, 0), 1, csVector2(1, 0), 2, csVector2(1, 1));
	else
		SetTextureMapping(OldMapIndex[0], OldMapUV[0], OldMapIndex[1], OldMapUV[1], OldMapIndex[2], OldMapUV[2]);
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

csVector3 SBS::GetPoint(csRef<iThingFactoryState> mesh, const char *polyname, const csVector3 &start, const csVector3 &end)
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

void SBS::Cut(csRef<iThingFactoryState> state, const csVector3 &start, const csVector3 &end, bool cutwalls, bool cutfloors, const csVector3 &mesh_origin, const csVector3 &object_origin, int checkwallnumber, const char *checkstring)
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
	return feet / 3.2808399f;
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

void SBS::SetListenerLocation(const csVector3 &location)
{
	//set position of sound listener object
	if (DisableSound == false)
		sndrenderer->GetListener()->SetPosition(location);
}

void SBS::SetListenerDirection(const csVector3 &front, const csVector3 &top)
{
	//set direction of sound listener object
	if (DisableSound == false)
		sndrenderer->GetListener()->SetDirection(front, top);
}

void SBS::SetListenerDistanceFactor(float factor)
{
	if (DisableSound == false)
		sndrenderer->GetListener()->SetDistanceFactor(factor);
}

float SBS::GetListenerDistanceFactor()
{
	if (DisableSound == false)
		return sndrenderer->GetListener()->GetDistanceFactor();
	else
		return 0;
}

void SBS::SetListenerRollOffFactor(float factor)
{
	if (DisableSound == false)
		sndrenderer->GetListener()->SetRollOffFactor(factor);
}

float SBS::GetListenerRollOffFactor()
{
	if (DisableSound == false)
		return sndrenderer->GetListener()->GetRollOffFactor();
	else
		return 0;
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

	int index = doorcallbacks.Find(door);

	if (index == csArrayItemNotFound)
	{
		//if door isn't already in the array, add it
		doorcallbacks.Push(door);
	}
	else
	{
		//otherwise change door's direction
		if (doorcallbacks[index])
			doorcallbacks[index]->OpenDoor = !doorcallbacks[index]->OpenDoor;
	}
	return true;
}

bool SBS::UnregisterDoorCallback(Door *door)
{
	int index = doorcallbacks.Find(door);

	if (index != csArrayItemNotFound && doorcallbacks[index])
	{
		//unregister existing door callback
		if (doorcallbacks[index]->IsMoving == false)
		{
			doorcallbacks.Delete(door);
			return true;
		}
		else
		{
			Report("Door in use; cannot unregister callback");
			return false;
		}
	}
	else
		return false;
}

bool SBS::RegisterCallButtonCallback(CallButton *button)
{
	//register a door object for callbacks (used for door movement)

	int index = buttoncallbacks.Find(button);

	if (index == csArrayItemNotFound)
	{
		//if call button isn't already in the array, add it
		buttoncallbacks.Push(button);
	}
	else
		return false;
	return true;
}

bool SBS::UnregisterCallButtonCallback(CallButton *button)
{
	int index = buttoncallbacks.Find(button);

	if (index != csArrayItemNotFound && buttoncallbacks[index])
	{
		//unregister existing call button callback
		buttoncallbacks.Delete(button);
	}
	else
		return false;
	return true;
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
		int info;
		for (int i = 0; i <= 5; i++)
		{
			info = 0;
			if (i == 0)
				info = mainnegflip;
			if (i == 1)
				info = mainposflip;
			if (i == 2)
				info = sidenegflip;
			if (i == 3)
				info = sideposflip;
			if (i == 4)
				info = topflip;
			if (i == 5)
				info = bottomflip;

			if (info == 1 || info == 3)
				widthscale[i] = -tw;
			if (info == 2 || info == 3)
				heightscale[i] = -th;
		}
	}
}

iMaterialWrapper* SBS::GetTextureMaterial(const char *texture, bool &result, const char *polygon_name)
{
	//perform a lookup on a texture, and return as a material
	//returns false in &result if texture load failed, and if default material was used instead
	iMaterialWrapper *material = engine->GetMaterialList()->FindByName(texture);

	if (!material)
	{
		//if material's not found, display a warning and use a default material
		csString message;
		if (polygon_name)
			message = "Texture '" + csString(texture) + "' not found for polygon '" + csString(polygon_name) + "'; using default material";
		else
			message = "Texture '" + csString(texture) + "' not found; using default material";
		ReportError(message);
		//set to default material
		material = engine->GetMaterialList()->FindByName("Default");
		result = false;
	}
	else
		result = true;
	return material;
}

bool SBS::GetTextureTiling(const char *texture, float &tw, float &th)
{
	//get per-texture tiling values from the textureinfo array
	for (int i = 0; i < textureinfo.GetSize(); i++)
	{
		if (textureinfo[i].name == texture)
		{
			tw = textureinfo[i].widthmult;
			th = textureinfo[i].heightmult;
			return true;
		}
	}
	return false;
}

void SBS::EnableMesh(csRef<iMeshWrapper> mesh, bool value)
{
	//enables or disables a mesh
	if (value == true)
	{
		mesh->GetFlags().Reset(CS_ENTITY_INVISIBLEMESH);
		mesh->GetFlags().Reset(CS_ENTITY_NOSHADOWS);
		mesh->GetFlags().Reset(CS_ENTITY_NOHITBEAM);
	}
	else
	{
		mesh->GetFlags().Set(CS_ENTITY_INVISIBLEMESH);
		mesh->GetFlags().Set(CS_ENTITY_NOSHADOWS);
		mesh->GetFlags().Set(CS_ENTITY_NOHITBEAM);
	}
}

iMeshWrapper* SBS::AddGenWall(csRef<iMeshWrapper> mesh, const char *texture, float x1, float z1, float x2, float z2, float height, float altitude, float tw, float th)
{
	//add a simple wall in a general mesh (currently only used for objects that change textures)

	//get texture
	csString texname = texture;
	bool result;
	csRef<iMaterialWrapper> material = GetTextureMaterial(texture, result, mesh->QueryObject()->GetName());
	if (!result)
		texname = "Default";

	if (tw == 0)
		tw = 1;
	if (th == 0)
		th = 1;

	float tw2 = tw, th2 = th;

	float mw, mh;
	if (GetTextureTiling(texname.GetData(), mw, mh))
	{
		//multiply the tiling parameters (tw and th) by
		//the stored multipliers for that texture
		tw2 = tw * mw;
		th2 = th * mh;
	}

	//create texture mapping table
	csVector2 table[] = {csVector2(tw2, th2), csVector2(0, th2), csVector2(tw2, 0), csVector2(0, 0)};

	//create a quad, map the texture, and append to the mesh
	CS::Geometry::TesselatedQuad wall (csVector3(x2, altitude, z2), csVector3(x1, altitude, z1), csVector3(x2, altitude + height, z2));
	CS::Geometry::TableTextureMapper mapper(table);
	wall.SetMapper(&mapper);
	wall.Append(mesh->GetFactory());

	//set lighting factor
	mesh->GetMeshObject()->SetColor(csColor(1, 1, 1));

	//set texture
	mesh->GetMeshObject()->SetMaterialWrapper(material);

	return mesh;
}

void SBS::ProcessCallButtons()
{
	//process all registered call buttons

	//the up and down sections need to be processed separately due to the removal of callbacks
	//during the run of each

	for (int i = 0; i < buttoncallbacks.GetSize(); i++)
	{
		//process up calls
		if (buttoncallbacks[i])
			buttoncallbacks[i]->Loop(true);
	}

	for (int i = 0; i < buttoncallbacks.GetSize(); i++)
	{
		//process down calls
		if (buttoncallbacks[i])
			buttoncallbacks[i]->Loop(false);
	}
}

void SBS::ProcessDoors()
{
	//process all registered doors
	for (int i = 0; i < doorcallbacks.GetSize(); i++)
	{
		if (doorcallbacks[i])
		{
			if (doorcallbacks[i]->IsMoving == true)
				doorcallbacks[i]->MoveDoor();
			else
				UnregisterDoorCallback(doorcallbacks[i]);
		}
	}
}

int SBS::GetDoorCallbackCount()
{
	//return the number of registered door callbacks
	return doorcallbacks.GetSize();
}

int SBS::GetCallButtonCallbackCount()
{
	//return the number of registered call button callbacks
	return buttoncallbacks.GetSize();

}

bool SBS::Mount(const char *filename, const char *path)
{
	//mounts a zip file into the virtual filesystem

	csString file = filename;
	csString Path = path;

	Report("Mounting " + file + " as path " + Path);

	return vfs->Mount(path, root_dir + "data" + dir_char + file);
}

void SBS::FreeTextureImages()
{
	//unload images in all texture wrappers

	for (int i = 0; i < engine->GetTextureList()->GetCount(); i++)
		engine->GetTextureList()->Get(i)->SetImageFile(0);
}

void SBS::AddFloorAutoArea(csVector3 start, csVector3 end)
{
	//adds an auto area that enables/disables floors

	AutoArea newarea;
	newarea.box = csBox3(start, end);
	newarea.inside = false;
	newarea.camerafloor = 0;
	FloorAutoArea.Push(newarea);
}

void SBS::CheckAutoAreas()
{
	//check all automatic areas
	
	csVector3 position = camera->GetPosition();
	int floor = camera->CurrentFloor;

	for (int i = 0; i < FloorAutoArea.GetSize(); i++)
	{
		//reset inside value if floor changed
		if (FloorAutoArea[i].camerafloor != floor)
			FloorAutoArea[i].inside = false;

		if (FloorAutoArea[i].box.In(position) == true && FloorAutoArea[i].inside == false)
		{
			//user moved into box; enable floors
			FloorAutoArea[i].inside = true;
			FloorAutoArea[i].camerafloor = floor;
			if (floor > -Basements)
			{
				GetFloor(floor - 1)->Enabled(true);
				GetFloor(floor - 1)->EnableGroup(true);
			}
			GetFloor(floor)->Enabled(true);
			GetFloor(floor)->EnableGroup(true);
			if (floor < Floors - 1)
			{
				GetFloor(floor + 1)->Enabled(true);
				GetFloor(floor + 1)->EnableGroup(true);
			}
		}
		if (FloorAutoArea[i].box.In(position) == false && FloorAutoArea[i].inside == true)
		{
			//user moved out of box; disable floors except current
			FloorAutoArea[i].inside = false;
			FloorAutoArea[i].camerafloor = 0;
			if (floor > -Basements)
			{
				GetFloor(floor - 1)->Enabled(false);
				GetFloor(floor - 1)->EnableGroup(false);
			}
			if (floor < Floors - 1)
			{
				GetFloor(floor + 1)->Enabled(false);
				GetFloor(floor + 1)->EnableGroup(false);
			}
			GetFloor(floor)->Enabled(true);
			GetFloor(floor)->EnableGroup(true);
		}
	}
}
