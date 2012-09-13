/* $Id$ */

/*
	Scalable Building Simulator - Core
	The Skyscraper Project - Version 1.8 Alpha
	Copyright (C)2004-2011 Ryan Thoryk
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

#include <wx/wx.h>
#include <wx/variant.h>
#include <OgreRoot.h>
#include <OgreFileSystem.h>
#include <OgreMaterialManager.h>
#include <OgreFontManager.h>
#include <OgreFont.h>
#include <OgreHardwarePixelBuffer.h>
#include <fmod.hpp>
#include <OgreBulletDynamicsRigidBody.h>
#include "globals.h"
#include "sbs.h"
#include "unix.h"
#include "revsbs.h"
#include "model.h"

SBS *sbs; //self reference

SBS::SBS()
{
	sbs = this;
	version = "0.8.0." + std::string(SVN_REVSTR);
	version_state = "Alpha";

	//set up SBS object
	object = new Object();
	object->SetValues(this, 0, "SBS", "SBS", true);

	//Print SBS banner
	PrintBanner();

	//Pause for 2 seconds
	wxSleep(2);

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
	widthscale.resize(6);
	heightscale.resize(6);
	remaining_delta = 0;
	start_time = 0;
	running_time = 0;
	InShaft = false;
	DisableSound = false;
	MapIndex.resize(3);
	MapUV.resize(3);
	OldMapIndex.resize(3);
	OldMapUV.resize(3);
	MapVerts1.resize(3);
	MapVerts2.resize(3);
	MapVerts3.resize(3);
	OldMapVerts1.resize(3);
	OldMapVerts2.resize(3);
	OldMapVerts3.resize(3);
	MapMethod = 0;
	OldMapMethod = 0;
	RevX = false;
	RevY = false;
	RevZ = false;
	OldRevX = false;
	OldRevY = false;
	OldRevZ = false;
	PlanarFlat = false;
	OldPlanarFlat = false;
	for (int i = 0; i <= 2; i++)
	{
		MapIndex[i] = 0;
		OldMapIndex[i] = 0;
		OldMapUV[i] = 0;
		MapUV[i] = 0;
	}
	RecreateColliders = false;
	soundcount = 0;
	UnitScale = 1;
	Verbose = false;
	InterfloorOnTop = false;
	DefaultMapper = 0;
	ObjectCount = 0;
	FastDelete = false;
	WallCount = 0;
	PolygonCount = 0;
	Shaders = false;
	SkyBox = 0;
	Landscape = 0;
	External = 0;
	Buildings = 0;
	current_time = 0;
	current_virtual_time = 0;
	elapsed_time = 0;
	average_time = 0;
	listener_position.x = 0;
	listener_position.y = 0;
	listener_position.z = 0;
	listener_velocity.x = 0;
	listener_velocity.y = 0;
	listener_velocity.z = 0;
	listener_forward.x = 0;
	listener_forward.y = 0;
	listener_forward.z = 0;
	listener_up.x = 0;
	listener_up.y = 0;
	listener_up.z = 0;
	timer = new Ogre::Timer();
	AmbientR = 1;
	AmbientG = 1;
	AmbientB = 1;
	OldAmbientR = 1;
	OldAmbientG = 1;
	OldAmbientB = 1;
	TexelOverride = false;
	enable_profiling = false;
	enable_advanced_profiling = false;
}

SBS::~SBS()
{
	//engine destructor

	Report("Deleting SBS objects...");

	FastDelete = true;

	//delete models
	for (int i = 0; i < (int)ModelArray.size(); i++)
	{
		if (ModelArray[i])
			delete ModelArray[i];
		ModelArray[i] = 0;
	}

	//delete lights
	for (int i = 0; i < (int)lights.size(); i++)
	{
		if (lights[i])
			delete lights[i];
		lights[i] = 0;
	}

	//delete camera object
	if (camera)
		delete camera;
	camera = 0;

	//delete callbacks
	doorcallbacks.clear();
	buttoncallbacks.clear();

	//delete floors
	for (int i = 0; i < (int)FloorArray.size(); i++)
	{
		if (FloorArray[i].object)
			delete FloorArray[i].object;
		FloorArray[i].object = 0;
	}
	FloorArray.clear();

	//delete elevators
	for (int i = 0; i < (int)ElevatorArray.size(); i++)
	{
		if (ElevatorArray[i].object)
			delete ElevatorArray[i].object;
		ElevatorArray[i].object = 0;
	}
	ElevatorArray.clear();

	//delete shafts
	for (int i = 0; i < (int)ShaftArray.size(); i++)
	{
		if (ShaftArray[i].object)
			delete ShaftArray[i].object;
		ShaftArray[i].object = 0;
	}
	ShaftArray.clear();

	//delete stairs
	for (int i = 0; i < (int)StairsArray.size(); i++)
	{
		if (StairsArray[i].object)
			delete StairsArray[i].object;
		StairsArray[i].object = 0;
	}
	StairsArray.clear();

	//delete sounds
	for (int i = 0; i < (int)sounds.size(); i++)
	{
		if (sounds[i])
			delete sounds[i];
		sounds[i] = 0;
	}
	sounds.clear();

	//delete wall objects
	if (SkyBox)
		delete SkyBox;
	SkyBox = 0;

	if (Landscape)
		delete Landscape;
	Landscape = 0;

	if (External)
		delete External;
	External = 0;

	if (Buildings)
		delete Buildings;
	Buildings = 0;

	//remove referenced sounds
	//sndmanager->RemoveSounds();

	//remove all engine objects
	//Report("Deleting CS engine objects...");
	//engine->clear();

	//delete physics objects
	if (mWorld)
	{
		//delete mWorld->getDebugDrawer();
		//mWorld->setDebugDrawer(0);
		delete mWorld;
	}
	mWorld = 0;

	//clear scene
	//mSceneManager->clearScene();

	ObjectArray.clear();
	verify_results.clear();

	if (timer)
		delete timer;
	timer = 0;

	//remove all meshes
	Ogre::MeshManager::getSingleton().removeAll();

	//remove all materials
	Ogre::MaterialManager::getSingleton().removeAll();
	Ogre::MaterialManager::getSingleton().initialise();  //restore default materials

	//remove all fonts
	Ogre::FontManager::getSingleton().removeAll();

	//remove all textures
	Ogre::TextureManager::getSingleton().removeAll();

	//clear self reference
	sbs = 0;

	Report("Exiting");
}

bool SBS::Start()
{
	//Post-init startup code goes here, before the runloop

	timer->reset();

	//free text texture memory
	for (int i = 0; i < textureboxes.size(); i++)
		free(textureboxes[i].buffer);
	textureboxes.clear();

	//move camera to start location
	camera->SetToStartPosition();
	camera->SetToStartDirection();
	camera->SetToStartRotation();

	//set sound listener object to initial position
	if (DisableSound == false)
	{
		/*if (sndrenderer->GetListener())
			SetListenerLocation(camera->GetPosition());
		else
		{
			ReportError("Sound listener object not available. Sound support disabled");
			DisableSound = true;
		}*/
	}

	//turn on main objects
	EnableBuildings(true);
	EnableLandscape(true);
	EnableExternal(true);
	EnableSkybox(true);
	
	//area->SetLightCulling(true);

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
		bool enable_elevators = sbs->GetConfigBool("Skyscraper.SBS.Elevator.IsEnabled", true);
		if (ElevatorArray[i].object)
		{
			//turn on shaft doors
			ElevatorArray[i].object->ShaftDoorsEnabled(0, camera->StartFloor, true);
			ElevatorArray[i].object->ShaftDoorsEnabled(0, GetShaft(ElevatorArray[i].object->AssignedShaft)->startfloor, true);
			ElevatorArray[i].object->ShaftDoorsEnabled(0, GetShaft(ElevatorArray[i].object->AssignedShaft)->endfloor, true);
			//disable objects
			ElevatorArray[i].object->EnableObjects(false);
			if (enable_elevators == false)
				ElevatorArray[i].object->Enabled(false);
		}
	}

	//turn on start floor
	if (GetFloor(camera->StartFloor))
	{
		GetFloor(camera->StartFloor)->Enabled(true);
		GetFloor(camera->StartFloor)->EnableGroup(true);
	}

	return true;
}

float SBS::AutoSize(float n1, float n2, bool iswidth, float offset, bool enable_force, bool force_mode)
{
	//Texture autosizing formulas

	if (offset == 0)
		offset = 1;

	if (iswidth == true)
	{
		if ((AutoX == true && enable_force == false) || (enable_force == true && force_mode == true))
			return fabs(n1 - n2) * offset;
		else
			return offset;
	}
	else
	{
		if ((AutoY == true && enable_force == false) || (enable_force == true && force_mode == true))
			return fabs(n1 - n2) * offset;
		else
			return offset;
	}
}

void SBS::PrintBanner()
{
	Report("\n Scalable Building Simulator " + version + " " + version_state);
	Report(" Copyright (C)2004-2011 Ryan Thoryk");
	Report(" This software comes with ABSOLUTELY NO WARRANTY. This is free");
	Report(" software, and you are welcome to redistribute it under certain");
	Report(" conditions. For details, see the file gpl.txt\n");
}

void SBS::MainLoop()
{
	//Main simulator loop
	SBS_PROFILE("SBS::MainLoop");

	//This makes sure all timer steps are the same size, in order to prevent the physics from changing
	//depending on frame rate
	float elapsed = remaining_delta + (GetElapsedTime() / 1000.0);

	//calculate start and running time
	if (start_time == 0)
		start_time = GetRunTime() / 1000.0;
	running_time = (GetRunTime() / 1000.0) - start_time;

	//update physics
	float step = float(GetElapsedTime()) / 1000.0;
	if (enable_advanced_profiling == false)
		SBSProfileManager::Start_Profile("Collisions/Physics");
	else
		SBSProfileManager::Start_Profile("Bullet");
	int steps = mWorld->stepSimulation(step, 1);
	SBSProfileManager::Stop_Profile();

	//only move character if Bullet processed a step (within it's 60fps timestep)
	if (steps >= 1)
		camera->MoveCharacter();

	//sync camera to physics
	camera->Sync();

	//update sound
	if (enable_advanced_profiling == false)
		SBSProfileManager::Start_Profile("Sound");
	else
		SBSProfileManager::Start_Profile("FMOD");
	soundsys->update();
	SBSProfileManager::Stop_Profile();

	//limit the elapsed value to prevent major slowdowns during debugging
	if (elapsed > 0.5)
		elapsed = 0.5;
	SBSProfileManager::Start_Profile("Simulator Loop");
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
	SBSProfileManager::Stop_Profile();
}

void SBS::CalculateFrameRate()
{
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

bool SBS::Initialize(Ogre::RenderWindow* mRenderWindow, Ogre::SceneManager* mSceneManager, Ogre::Camera *camera, const char* rootdirectory, const char* directory_char, FMOD::System *fmodsystem)
{
	mRoot = Ogre::Root::getSingletonPtr();
	this->mSceneManager = mSceneManager;

	root_dir = rootdirectory;
	dir_char = directory_char;

	//load config file
	configfile.load("skyscraper.ini");

	//load default values from config file
	SkyName = GetConfigString("Skyscraper.SBS.SkyName", "noon");
	AutoShafts = GetConfigBool("Skyscraper.SBS.AutoShafts", true);
	AutoStairs = GetConfigBool("Skyscraper.SBS.AutoStairs", true);
	ShowFullShafts = GetConfigBool("Skyscraper.SBS.ShowFullShafts", false);
	ShowFullStairs = GetConfigBool("Skyscraper.SBS.ShowFullStairs", false);
	ShaftDisplayRange = GetConfigInt("Skyscraper.SBS.ShaftDisplayRange", 3);
	StairsDisplayRange = GetConfigInt("Skyscraper.SBS.StairsDisplayRange", 5);
	ShaftOutsideDisplayRange = GetConfigInt("Skyscraper.SBS.ShaftOutsideDisplayRange", 3);
	StairsOutsideDisplayRange = GetConfigInt("Skyscraper.SBS.StairsOutsideDisplayRange", 3);
	FloorDisplayRange = GetConfigInt("Skyscraper.SBS.FloorDisplayRange", 3);
	ProcessElevators = GetConfigBool("Skyscraper.SBS.ProcessElevators", true);
	DisableSound = GetConfigBool("Skyscraper.SBS.DisableSound", false);
	UnitScale = GetConfigFloat("Skyscraper.SBS.UnitScale", 5);
	if (UnitScale <= 0)
		UnitScale = 1;
	Verbose = GetConfigBool("Skyscraper.SBS.Verbose", false);
	DefaultMapper = GetConfigInt("Skyscraper.SBS.TextureMapper", 0);
	ResetTextureMapping(true); //set default texture map values

	//disable sound if system is not available
	if (!fmodsystem)
		DisableSound = true;
	soundsys = fmodsystem;

	//set up sound options (mainly to set sound distance factor to feet instead of meters)
	if (DisableSound == false)
		soundsys->set3DSettings(1.0, 3.28, 1.0);

	//set up physics
	mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneManager, Ogre::AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000), Ogre::Vector3(10000, 10000, 10000)), Ogre::Vector3(0, 0, 0), true);
	mWorld->setAllowedCcdPenetration(0);

	/*debugDrawer = new OgreBulletCollisions::DebugDrawer();
	debugDrawer->setDrawWireframe(true);
	mWorld->setDebugDrawer(debugDrawer);
	Ogre::SceneNode *node = mSceneManager->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
	node->attachObject(static_cast<Ogre::SimpleRenderable*> (debugDrawer));
	*/

	//mount sign texture packs
	Mount("signs-sans.zip", "signs/sans");
	Mount("signs-sans_bold.zip", "signs/sans_bold");
	Mount("signs-sans_cond.zip", "signs/sans_cond");
	Mount("signs-sans_cond_bold.zip", "signs/sans_cond_bold");

	//load default textures
	Report("Loading default textures...");
	SetLighting();
	LoadTexture("data/brick1.jpg", "Default", 1, 1);
	LoadTexture("data/gray2-sm.jpg", "ConnectionWall", 1, 1);
	LoadTexture("data/metal1-sm.jpg", "Connection", 1, 1);
	ResetLighting();
	Report("Done");

	//create camera object
	this->camera = new Camera(camera);
	
	return true;
}

bool SBS::LoadTexture(const char *filename, const char *name, float widthmult, float heightmult, bool enable_force, bool force_mode, bool disable_depth_buffer, int mipmaps, bool use_alpha_color, Ogre::ColourValue alpha_color)
{
	//set verbosity level
	Ogre::TextureManager::getSingleton().setVerbose(Verbose);

	//first verify the filename
	std::string filename2 = VerifyFile(filename);

	//determine if the file is a GIF image, to force keycolor alpha
	std::string extension = filename2.substr(filename2.size() - 3);
	SetCase(extension, false);
	if (extension == "gif")
		use_alpha_color = true;

	// Load the texture
	std::string path = GetMountPath(filename2.c_str(), filename2);
	Ogre::TexturePtr mTex;
	std::string texturename;
	bool has_alpha = false;

	try
	{
		if (use_alpha_color == false)
		{
			mTex = Ogre::TextureManager::getSingleton().load(filename2, path, Ogre::TEX_TYPE_2D, mipmaps);

			if (mTex.isNull())
				return ReportError("Error loading texture" + filename2);
			texturename = mTex->getName();
			has_alpha = mTex->hasAlpha();
		}
		else
		{
			//load based on chroma key for alpha

			texturename = "kc_" + filename2;
			loadChromaKeyedTexture(filename2, path, texturename, Ogre::ColourValue::White);
			has_alpha = true;
		}
	}
	catch (Ogre::Exception &e)
	{
		return ReportError("Error loading texture " + filename2 + "\n" + e.getDescription());
	}

	//create a new material
	std::string matname = name;
	TrimString(matname);
	Ogre::MaterialPtr mMat = Ogre::MaterialManager::getSingleton().create(matname, "General");
	mMat->setLightingEnabled(true);
	mMat->setAmbient(AmbientR, AmbientG, AmbientB);

	//bind texture to material
	mMat->getTechnique(0)->getPass(0)->createTextureUnitState(texturename);

	//show only clockwise side of material
	mMat->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
	
	if (disable_depth_buffer == true)
	{
		mMat->setDepthCheckEnabled(false);
		mMat->setDepthWriteEnabled(false);
	}

	//enable alpha blending for related textures
	if (has_alpha == true)
	{
		mMat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		//enable hard alpha for alpha mask values 128 and above
		mMat->getTechnique(0)->getPass(0)->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
	}

	Report("Loaded texture " + std::string(filename));

	TextureInfo info;
	info.name = matname;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	info.enable_force = enable_force;
	info.force_mode = force_mode;
	textureinfo.push_back(info);
	return true;
}

bool SBS::UnloadTexture(const char *name)
{
	//unloads a texture

	Ogre::TexturePtr wrapper = Ogre::TextureManager::getSingleton().getByName(name);
	if (!wrapper.isNull())
		return false;
	Ogre::TextureManager::getSingleton().remove(name);
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(name);
	if (!wrapper.isNull())
		return false;
	Ogre::MaterialManager::getSingleton().remove(name);

	return true;
}

bool SBS::LoadTextureCropped(const char *filename, const char *name, int x, int y, int width, int height, float widthmult, float heightmult, bool enable_force, bool force_mode)
{
	//loads only a portion of the specified texture

	Ogre::ColourValue alpha_color = Ogre::ColourValue::Black;
	int mipmaps = -1;
	bool use_alpha_color = false;

	//set verbosity level
	Ogre::TextureManager::getSingleton().setVerbose(Verbose);

	//first verify the filename
	std::string filename2 = VerifyFile(filename);

	//determine if the file is a GIF image, to force keycolor alpha
	std::string extension = filename2.substr(filename2.size() - 3);
	SetCase(extension, false);
	if (extension == "gif")
		use_alpha_color = true;

	// Load the texture
	std::string path = GetMountPath(filename2.c_str(), filename2);
	Ogre::TexturePtr mTex;
	std::string texturename;
	bool has_alpha = false;

	try
	{
		if (use_alpha_color == false)
		{
			mTex = Ogre::TextureManager::getSingleton().load(filename2, path, Ogre::TEX_TYPE_2D, mipmaps);

			if (mTex.isNull())
				return ReportError("Error loading texture" + filename2);
			texturename = mTex->getName();
			has_alpha = mTex->hasAlpha();
		}
		else
		{
			//load based on chroma key for alpha

			texturename = "kc_" + filename2;
			loadChromaKeyedTexture(filename2, path, texturename, Ogre::ColourValue::White);
			has_alpha = true;
		}
	}
	catch (Ogre::Exception &e)
	{
		return ReportError("Error loading texture " + filename2 + "\n" + e.getDescription());
	}

	std::string Name = name;
	std::string Filename = filename;

	//set default values if specified
	if (x == -1)
		x = 0;
	if (y == -1)
		y = 0;
	if (width < 1)
		width = mTex->getWidth();
	if (height < 1)
		height = mTex->getHeight();

	if (x > mTex->getWidth() || y > mTex->getHeight())
		return ReportError("LoadTextureCropped: invalid coordinates for '" + Name + "'");
	if (x + width > mTex->getWidth() || y + height > mTex->getHeight())
		return ReportError("LoadTextureCropped: invalid size for '" + Name + "'");

	//create new empty texture
	Ogre::TexturePtr new_texture = Ogre::TextureManager::getSingleton().createManual(Name, "General", Ogre::TEX_TYPE_2D, width, height, Ogre::MIP_UNLIMITED, Ogre::PF_R8G8B8A8, Ogre::TU_STATIC|Ogre::TU_AUTOMIPMAP);

	//copy source and overlay images onto new image
	Ogre::Box source (x, y, x + width, y + height);
	Ogre::Box dest (0, 0, width, height);
	new_texture->getBuffer()->blit(mTex->getBuffer(), source, dest);

	//create a new material
	Ogre::MaterialPtr mMat = Ogre::MaterialManager::getSingleton().create(Name, "General");
	mMat->setLightingEnabled(true);
	mMat->setAmbient(AmbientR, AmbientG, AmbientB);

	//bind texture to material
	mMat->getTechnique(0)->getPass(0)->createTextureUnitState(Name);

	//show only clockwise side of material
	mMat->setCullingMode(Ogre::CULL_ANTICLOCKWISE);

	//enable alpha blending for related textures
	mMat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	//enable hard alpha for alpha mask values 128 and above
	mMat->getTechnique(0)->getPass(0)->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);

	//add texture multipliers for new texture
	TextureInfo info;
	info.name = name;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	info.enable_force = enable_force;
	info.force_mode = force_mode;
	textureinfo.push_back(info);

	return true;
}

bool SBS::AddTextToTexture(const char *origname, const char *name, const char *font_filename, float font_size, const char *text, int x1, int y1, int x2, int y2, const char *h_align, const char *v_align, int ColorR, int ColorG, int ColorB, bool enable_force, bool force_mode)
{
	//adds text to the named texture, in the given box coordinates and alignment

	//h_align is either "left", "right" or "center" - default is center
	//v_align is either "top", "bottom", or "center" - default is center

	//if either x1 or y1 are -1, the value of 0 is used.
	//If either x2 or y2 are -1, the width or height of the texture is used.

	std::string hAlign = h_align;
	std::string vAlign = v_align;
	std::string Name = name;
	std::string Origname = origname;
	std::string Text = text;
	TrimString(Text);
	TrimString(Name);
	TrimString(Origname);

	std::string font_filename2 = VerifyFile(font_filename);

	//load font
	Ogre::FontPtr font;
	Ogre::String fontname = font_filename2 + Ogre::StringConverter::toString(font_size);
	font = Ogre::FontManager::getSingleton().getByName(fontname, "General");

	//load if font is not already loaded
	if (font.isNull())
	{
		try
		{
			font = Ogre::FontManager::getSingleton().create(fontname, "General");
			//font->setType(Ogre::FontType::FT_TRUETYPE);
			font->setSource(font_filename2);
			font->setTrueTypeSize(font_size);
			font->setTrueTypeResolution(96);
			font->setAntialiasColour(true);
			font->addCodePointRange(Ogre::Font::CodePointRange(48, 122));
			font->load();
		}
		catch (Ogre::Exception &e)
		{
			sbs->ReportError("Error loading font " + fontname + "\n" + e.getDescription());
			return false;
		}
	}

	//get original texture
	Ogre::MaterialPtr ptr = Ogre::MaterialManager::getSingleton().getByName(Origname);
	if (ptr.isNull())
	{
		ReportError("AddTextToTexture: Invalid original material '" + Origname + "'");
		return false;
	}
	std::string texname = ptr->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
	Ogre::TexturePtr background = Ogre::TextureManager::getSingleton().getByName(texname);
	if (background.isNull())
	{
		ReportError("AddTextToTexture: Invalid original texture '" + texname + "'");
		return false;
	}

	//get texture tiling info
	float widthmult, heightmult;
	GetTextureTiling(origname, widthmult, heightmult);

	//get height and width of texture
	int width, height;
	width = background->getWidth();
	height = background->getHeight();

	//create new empty texture
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual(Name, "General", Ogre::TEX_TYPE_2D, width, height, Ogre::MIP_UNLIMITED, Ogre::PF_R8G8B8A8, Ogre::TU_STATIC|Ogre::TU_AUTOMIPMAP);

	//get new texture dimensions, if it was resized
	width = texture->getWidth();
	height = texture->getHeight();

	//add texture multipliers for new texture
	TextureInfo info;
	info.name = name;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	info.enable_force = enable_force;
	info.force_mode = force_mode;
	textureinfo.push_back(info);

	//set default values if specified
	if (x1 == -1)
		x1 = 0;
	if (y1 == -1)
		y1 = 0;
	if (x2 == -1)
		x2 = width - 1;
	if (y2 == -1)
		y2 = height - 1;

	//draw original image onto new texture
	texture->getBuffer()->blit(background->getBuffer());

	TrimString(hAlign);
	TrimString(vAlign);
	char align = 'c';
	if (hAlign == "left")
		align = 'l';
	if (hAlign == "right")
		align = 'r';
	char valign = 'c';
	if (vAlign == "top")
		valign = 't';
	if (vAlign == "bottom")
		valign = 'b';

	//write text
	bool result = WriteToTexture(Text, texture, Ogre::Box(x1, y1, x2, y2), font, Ogre::ColourValue(ColorR, ColorG, ColorB, 1.0), align, valign);
	if (result == false)
		return false;

	//create a new material
	Ogre::MaterialPtr mMat = Ogre::MaterialManager::getSingleton().create(Name, "General");
	mMat->setLightingEnabled(true);
	mMat->setAmbient(AmbientR, AmbientG, AmbientB);

	//bind texture to material
	mMat->getTechnique(0)->getPass(0)->createTextureUnitState(Name);
	
	//show only clockwise side of material
	mMat->setCullingMode(Ogre::CULL_ANTICLOCKWISE);

	//enable alpha blending for related textures
	mMat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

	//enable hard alpha for alpha mask values 128 and above
	mMat->getTechnique(0)->getPass(0)->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);

	Report("AddTextToTexture: created texture " + Name);
	CacheFilename(Name, Name);
	return true;
}

bool SBS::AddTextureOverlay(const char *orig_texture, const char *overlay_texture, const char *name, int x, int y, int width, int height, float widthmult, float heightmult, bool enable_force, bool force_mode)
{
	//draws the specified texture on top of another texture
	//orig_texture is the original texture to use; overlay_texture is the texture to draw on top of it

	std::string Name = name;
	std::string Origname = orig_texture;
	std::string Overlay = overlay_texture;

	//get original texture
	Ogre::MaterialPtr ptr = Ogre::MaterialManager::getSingleton().getByName(Origname);
	if (ptr.isNull())
	{
		ReportError("AddTextureOverlay: Invalid original material '" + Origname + "'");
		return false;
	}
	std::string texname = ptr->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
	Ogre::TexturePtr image1 = Ogre::TextureManager::getSingleton().getByName(texname);
	if (image1.isNull())
	{
		ReportError("AddTextureOverlay: Invalid original texture '" + texname + "'");
		return false;
	}

	//get overlay texture
	ptr = Ogre::MaterialManager::getSingleton().getByName(Overlay);
	if (ptr.isNull())
	{
		ReportError("AddTextureOverlay: Invalid overlay material '" + Overlay + "'");
		return false;
	}
	texname = ptr->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
	Ogre::TexturePtr image2 = Ogre::TextureManager::getSingleton().getByName(texname);
	if (image2.isNull())
	{
		ReportError("AddTextureOverlay: Invalid overlay texture '" + texname + "'");
		return false;
	}

	//set default values if specified
	if (x == -1)
		x = 0;
	if (y == -1)
		y = 0;
	if (width < 1)
		width = image2->getWidth();
	if (height < 1)
		height = image2->getHeight();

	if (x > image1->getWidth() || y > image1->getHeight())
		return ReportError("AddTextureOverlay: invalid coordinates for '" + Name + "'");
	if (x + width > image1->getWidth() || y + height > image1->getHeight())
		return ReportError("AddTextureOverlay: invalid size for '" + Name + "'");

	//create new empty texture
	Ogre::TexturePtr new_texture = Ogre::TextureManager::getSingleton().createManual(Name, "General", Ogre::TEX_TYPE_2D, image1->getWidth(), image1->getHeight(), Ogre::MIP_UNLIMITED, Ogre::PF_R8G8B8A8, Ogre::TU_STATIC|Ogre::TU_AUTOMIPMAP);

	//copy source and overlay images onto new image
	Ogre::Box source (x, y, x + width, y + height);
	Ogre::Box source_full (0, 0, image1->getWidth(), image1->getHeight());
	Ogre::Box overlay (0, 0, image2->getWidth(), image2->getHeight());
	new_texture->getBuffer()->blit(image1->getBuffer(), source_full, source_full);
	new_texture->getBuffer()->blit(image2->getBuffer(), overlay, source);

	//create a new material
	Ogre::MaterialPtr mMat = Ogre::MaterialManager::getSingleton().create(Name, "General");
	mMat->setLightingEnabled(true);
	mMat->setAmbient(AmbientR, AmbientG, AmbientB);

	//bind texture to material
	mMat->getTechnique(0)->getPass(0)->createTextureUnitState(Name);

	//show only clockwise side of material
	mMat->setCullingMode(Ogre::CULL_ANTICLOCKWISE);

	//enable alpha blending for related textures
	mMat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

	//enable hard alpha for alpha mask values 128 and above
	mMat->getTechnique(0)->getPass(0)->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);

	//add texture multipliers for new texture
	TextureInfo info;
	info.name = name;
	info.widthmult = widthmult;
	info.heightmult = heightmult;
	info.enable_force = enable_force;
	info.force_mode = force_mode;
	textureinfo.push_back(info);

	return true;
}

int SBS::AddWallMain(Object *parent, MeshObject* mesh, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float altitude1, float altitude2, float tw, float th, bool autosize)
{
	WallObject *object = new WallObject(mesh, parent, true);
	int result = AddWallMain(object, name, texture, thickness, x1, z1, x2, z2, height_in1, height_in2, altitude1, altitude2, tw, th, autosize);
	delete object;
	return result;
}

int SBS::AddWallMain(WallObject* wallobject, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float altitude1, float altitude2, float tw, float th, bool autosize)
{
	//determine axis of wall
	int axis = 0;
	if (fabs(x1 - x2) > (fabs(z1 - z2) + 0.00001))
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
	Ogre::Vector3 v1 (x1, altitude1 + height_in1, z1); //left top
	Ogre::Vector3 v2 (x2, altitude2 + height_in2, z2); //right top
	Ogre::Vector3 v3 (x2, altitude2, z2); //right base
	Ogre::Vector3 v4 (x1, altitude1, z1); //left base

	Ogre::Vector3 v5 = v1;
	Ogre::Vector3 v6 = v2;
	Ogre::Vector3 v7 = v3;
	Ogre::Vector3 v8 = v4;

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
	std::string NewName, texture2 = texture;
	float tw2 = tw, th2 = th;

	if (FlipTexture == true)
		ProcessTextureFlip(tw, th);

	if (DrawMainN == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[0];
			th2 = heightscale[0];
		}
		if (TextureOverride == true)
			texture2 = mainnegtex;

		NewName = name;
		if (GetDrawWallsCount() > 1)
			NewName.append(":front");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v1, v2, v3, v4, tw2, th2, autosize); //front wall
	}

	if (DrawMainP == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[1];
			th2 = heightscale[1];
		}
		if (TextureOverride == true)
			texture2 = mainpostex;

		NewName = name;
		NewName.append(":back");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v6, v5, v8, v7, tw2, th2, autosize); //back wall
	}

	if (DrawSideN == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[2];
			th2 = heightscale[2];
		}
		if (TextureOverride == true)
			texture2 = sidenegtex;

		NewName = name;
		NewName.append(":left");
		if (axis == 1)
			wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v5, v1, v4, v8, tw2, th2, autosize); //left wall
		else
			wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v2, v6, v7, v3, tw2, th2, autosize); //left wall
	}

	if (DrawSideP == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[3];
			th2 = heightscale[3];
		}
		if (TextureOverride == true)
			texture2 = sidepostex;

		NewName = name;
		NewName.append(":right");
		if (axis == 1)
			wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v2, v6, v7, v3, tw2, th2, autosize); //right wall
		else
			wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v5, v1, v4, v8, tw2, th2, autosize); //right wall
	}

	if (DrawTop == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[4];
			th2 = heightscale[4];
		}
		if (TextureOverride == true)
			texture2 = toptex;

		NewName = name;
		NewName.append(":top");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v5, v6, v2, v1, tw2, th2, autosize); //top wall
	}

	if (DrawBottom == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[5];
			th2 = heightscale[5];
		}
		if (TextureOverride == true)
			texture2 = bottomtex;

		NewName = name;
		NewName.append(":bottom");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v4, v3, v7, v8, tw2, th2, autosize); //bottom wall
	}

	//recreate colliders if specified
	if (RecreateColliders == true)
	{
		//DeleteColliders(wallobject);
		//CreateColliders(wallobject);
	}

	return 0;
}

int SBS::AddFloorMain(Object *parent, MeshObject* mesh, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float altitude1, float altitude2, float tw, float th, bool autosize)
{
	WallObject *object = new WallObject(mesh, parent, true);
	int result = AddFloorMain(object, name, texture, thickness, x1, z1, x2, z2, altitude1, altitude2, tw, th, autosize);
	delete object;
	return result;
}

int SBS::AddFloorMain(WallObject* wallobject, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float altitude1, float altitude2, float tw, float th, bool autosize)
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

	Ogre::Vector3 v1, v2, v3, v4;

	if (ReverseAxisValue == false)
	{
		v1 = Ogre::Vector3(x1, altitude1, z1); //bottom left
		v2 = Ogre::Vector3(x2, altitude1, z1); //bottom right
		v3 = Ogre::Vector3(x2, altitude2, z2); //top right
		v4 = Ogre::Vector3(x1, altitude2, z2); //top left
	}
	else
	{
		v1 = Ogre::Vector3(x1, altitude1, z1); //bottom left
		v2 = Ogre::Vector3(x1, altitude1, z2); //top left
		v3 = Ogre::Vector3(x2, altitude2, z2); //top right
		v4 = Ogre::Vector3(x2, altitude2, z1); //bottom right
	}

	Ogre::Vector3 v5 = v1;
	Ogre::Vector3 v6 = v2;
	Ogre::Vector3 v7 = v3;
	Ogre::Vector3 v8 = v4;

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
	std::string NewName, texture2 = texture;
	float tw2 = tw, th2 = th;

	if (FlipTexture == true)
		ProcessTextureFlip(tw, th);

	if (DrawMainN == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[0];
			th2 = heightscale[0];
		}
		if (TextureOverride == true)
			texture2 = mainnegtex;

		NewName = name;
		if (GetDrawWallsCount() > 1)
			NewName.append(":front");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v1, v2, v3, v4, tw2, th2, autosize); //bottom wall
	}

	if (DrawMainP == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[1];
			th2 = heightscale[1];
		}
		if (TextureOverride == true)
			texture2 = mainpostex;

		NewName = name;
		NewName.append(":back");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v8, v7, v6, v5, tw2, th2, autosize); //top wall
	}

	if (DrawSideN == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[2];
			th2 = heightscale[2];
		}
		if (TextureOverride == true)
			texture2 = sidenegtex;

		NewName = name;
		NewName.append(":left");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v8, v5, v1, v4, tw2, th2, autosize); //left wall
	}

	if (DrawSideP == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[3];
			th2 = heightscale[3];
		}
		if (TextureOverride == true)
			texture2 = sidepostex;

		NewName = name;
		NewName.append(":right");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v6, v7, v3, v2, tw2, th2, autosize); //right wall
	}

	if (DrawTop == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[4];
			th2 = heightscale[4];
		}
		if (TextureOverride == true)
			texture2 = toptex;

		NewName = name;
		NewName.append(":top");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v5, v6, v2, v1, tw2, th2, autosize); //front wall
	}

	if (DrawBottom == true)
	{
		if (FlipTexture == true)
		{
			tw2 = widthscale[5];
			th2 = heightscale[5];
		}
		if (TextureOverride == true)
			texture2 = bottomtex;

		NewName = name;
		NewName.append(":bottom");
		wallobject->AddQuad(NewName.c_str(), texture2.c_str(), v7, v8, v4, v3, tw2, th2, autosize); //back wall
	}

	//recreate colliders if specified
	if (RecreateColliders == true)
	{
		//DeleteColliders(wallobject);
		//CreateColliders(wallobject);
	}

	return 0;
}

void SBS::Report(std::string message)
{
	ReplaceAll(message, "%", "%%"); //allow percent signs

	printf(message.c_str());
	printf("\n");
	fflush (stdout);

	LastNotification = message;
}

bool SBS::ReportError(std::string message)
{
	ReplaceAll(message, "%", "%%"); //allow percent signs

	printf(message.c_str());
	printf("\n");
	fflush (stdout);

	LastError = message;
	return false;
}

int SBS::CreateWallBox(WallObject* wallobject, const char *name, const char *texture, float x1, float x2, float z1, float z2, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom, bool autosize)
{
	//create 4 walls

	std::string NewName;
	int firstidx = 0;

	//swap values if the first is greater than the second
	if (x1 > x2)
	{
		float tmpx = x1;
		x1 = x2;
		x2 = tmpx;
	}
	if (z1 > z2)
	{
		float tmpz = z1;
		z1 = z2;
		z2 = tmpz;
	}

	if (inside == true)
	{
		//generate a box visible from the inside

		NewName = name;
		NewName.append(":inside");

		wallobject->AddQuad( //front
			NewName.c_str(),
			texture,
			Ogre::Vector3(x1, voffset, z1),
			Ogre::Vector3(x2, voffset, z1),
			Ogre::Vector3(x2, voffset + height_in, z1),
			Ogre::Vector3(x1, voffset + height_in, z1), tw, th, autosize);
		wallobject->AddQuad( //right
			NewName.c_str(),
			texture,
			Ogre::Vector3(x2, voffset, z1),
			Ogre::Vector3(x2, voffset, z2),
			Ogre::Vector3(x2, voffset + height_in, z2),
			Ogre::Vector3(x2, voffset + height_in, z1), tw, th, autosize);
		wallobject->AddQuad( //back
			NewName.c_str(),
			texture,
			Ogre::Vector3(x2, voffset, z2),
			Ogre::Vector3(x1, voffset, z2),
			Ogre::Vector3(x1, voffset + height_in, z2),
			Ogre::Vector3(x2, voffset + height_in, z2), tw, th, autosize);
		wallobject->AddQuad( //left
			NewName.c_str(),
			texture,
			Ogre::Vector3(x1, voffset, z2),
			Ogre::Vector3(x1, voffset, z1),
			Ogre::Vector3(x1, voffset + height_in, z1),
			Ogre::Vector3(x1, voffset + height_in, z2), tw, th, autosize);
		if (bottom == true)
		{
			wallobject->AddQuad( //bottom
				NewName.c_str(),
				texture,
				Ogre::Vector3(x1, voffset, z2),
				Ogre::Vector3(x2, voffset, z2),
				Ogre::Vector3(x2, voffset, z1),
				Ogre::Vector3(x1, voffset, z1), tw, th, autosize);
		}
		if (top == true)
		{
			wallobject->AddQuad( //top
				NewName.c_str(),
				texture,
				Ogre::Vector3(x1, voffset + height_in, z1),
				Ogre::Vector3(x2, voffset + height_in, z1),
				Ogre::Vector3(x2, voffset + height_in, z1),
				Ogre::Vector3(x1, voffset + height_in, z2), tw, th, autosize);
		}
	}

	if (outside == true)
	{
		NewName = name;
		NewName.append(":outside");

		wallobject->AddQuad( //front
			NewName.c_str(),
			texture,
			Ogre::Vector3(x1, voffset + height_in, z1),
			Ogre::Vector3(x2, voffset + height_in, z1),
			Ogre::Vector3(x2, voffset, z1),
			Ogre::Vector3(x1, voffset, z1), tw, th, autosize);
		wallobject->AddQuad( //right
			NewName.c_str(),
			texture,
			Ogre::Vector3(x2, voffset + height_in, z1),
			Ogre::Vector3(x2, voffset + height_in, z2),
			Ogre::Vector3(x2, voffset, z2),
			Ogre::Vector3(x2, voffset, z1), tw, th, autosize);
		wallobject->AddQuad( //back
			NewName.c_str(),
			texture,
			Ogre::Vector3(x2, voffset + height_in, z2),
			Ogre::Vector3(x1, voffset + height_in, z2),
			Ogre::Vector3(x1, voffset, z2),
			Ogre::Vector3(x2, voffset, z2), tw, th, autosize);
		wallobject->AddQuad( //left
			NewName.c_str(),
			texture,
			Ogre::Vector3(x1, voffset + height_in, z2),
			Ogre::Vector3(x1, voffset + height_in, z1),
			Ogre::Vector3(x1, voffset, z1),
			Ogre::Vector3(x1, voffset, z2), tw, th, autosize);
		if (bottom == true)
		{
			wallobject->AddQuad( //bottom
				NewName.c_str(),
				texture,
				Ogre::Vector3(x1, voffset, z1),
				Ogre::Vector3(x2, voffset, z1),
				Ogre::Vector3(x2, voffset, z2),
				Ogre::Vector3(x1, voffset, z2), tw, th, autosize);
		}
		if (top == true)
		{
			wallobject->AddQuad( //top
				NewName.c_str(),
				texture,
				Ogre::Vector3(x1, voffset + height_in, z2),
				Ogre::Vector3(x2, voffset + height_in, z2),
				Ogre::Vector3(x2, voffset + height_in, z1),
				Ogre::Vector3(x1, voffset + height_in, z1), tw, th, autosize);
		}
	}
	return firstidx;
}

int SBS::CreateWallBox2(WallObject* wallobject, const char *name, const char *texture, float CenterX, float CenterZ, float WidthX, float LengthZ, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom, bool autosize)
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

	return CreateWallBox(wallobject, name, texture, x1, x2, z1, z2, height_in, voffset, tw, th, inside, outside, top, bottom, autosize);
}

void SBS::InitMeshes()
{
	//create object meshes
	Buildings = new MeshObject(this->object, "Buildings");
	External = new MeshObject(this->object, "External");
	Landscape = new MeshObject(this->object, "Landscape");
}

int SBS::AddCustomWall(WallObject* wallobject, const char *name, const char *texture, std::vector<Ogre::Vector3> &varray, float tw, float th)
{
	//Adds a wall from a specified array of 3D vectors

	int num;
	std::vector<Ogre::Vector3> varray1 = varray;
	std::vector<Ogre::Vector3> varray2;

	//get number of stored vertices
	num = (int)varray.size();

	//create a second array with reversed vertices
	varray2.reserve(num);
	for (int i = num - 1; i >= 0; i--)
		varray2.push_back(varray1[i]);

	//create 2 polygons (front and back) from the vertex array

	//get polygon native direction
	Ogre::Vector3 direction = GetPolygonDirection(varray1);

	//if the polygon is facing right, down or to the back, reverse faces
	//to keep the vertices clockwise
	std::vector<Ogre::Vector3> tmppoly;
	if (direction.x == 1 || direction.y == -1 || direction.z == 1)
	{
		tmppoly = varray1;
		varray1 = varray2;
		varray2 = tmppoly;
	}

	//get texture sizing info
	float tw2 = tw, th2 = th;
	float mw, mh;
	if (GetTextureTiling(texture, mw, mh))
	{
		//multiply the tiling parameters (tw and th) by
		//the stored multipliers for that texture
		tw2 = tw * mw;
		th2 = th * mh;
	}

	//add the polygons
	if (DrawMainN == true)
	{
		std::string NewName;
		NewName = name;
		NewName.append(":0");
		wallobject->AddPolygon(NewName.c_str(), texture, varray1, tw2, th2, true);
	}
	if (DrawMainP == true)
	{
		std::string NewName;
		NewName = name;
		NewName.append(":1");
		wallobject->AddPolygon(NewName.c_str(), texture, varray2, tw2, th2, true);
	}

	//recreate colliders if specified
	if (RecreateColliders == true)
	{
		//DeleteColliders(wallobject);
		//CreateColliders(wallobject);
	}

	return 0;
}

int SBS::AddTriangleWall(WallObject* wallobject, const char *name, const char *texture, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float tw, float th)
{
	//Adds a triangular wall with the specified dimensions
	std::vector<Ogre::Vector3> varray;

	//set up temporary vertex array
	varray.reserve(3);
	varray.push_back(Ogre::Vector3(x1, y1, z1));
	varray.push_back(Ogre::Vector3(x2, y2, z2));
	varray.push_back(Ogre::Vector3(x3, y3, z3));

	//pass data on to AddCustomWall function
	return AddCustomWall(wallobject, name, texture, varray, tw, th);
}

void SBS::EnableBuildings(bool value)
{
	//turns buildings on/off
	Buildings->Enable(value);
	IsBuildingsEnabled = value;
}

void SBS::EnableLandscape(bool value)
{
	//turns landscape on/off
	Landscape->Enable(value);
	IsLandscapeEnabled = value;
}

void SBS::EnableExternal(bool value)
{
	//turns external on/off
	External->Enable(value);
	IsExternalEnabled = value;
}

void SBS::EnableSkybox(bool value)
{
	//turns skybox on/off
	IsSkyboxEnabled = value;
}

void SBS::CreateSky(const char *filenamebase)
{
	//create skybox

	std::string file = filenamebase;
	Mount(std::string("data/sky-" + file + ".zip").c_str(), "sky");

	//load textures
	SetLighting();
	LoadTexture("sky/up.jpg", "SkyTop", 1, 1, false, false, false, 0);
	LoadTexture("sky/down.jpg", "SkyBottom", 1, 1, false, false, false, 0);
	LoadTexture("sky/left.jpg", "SkyLeft", 1, 1, false, false, false, 0);
	LoadTexture("sky/right.jpg", "SkyRight", 1, 1, false, false, false, 0);
	LoadTexture("sky/front.jpg", "SkyFront", 1, 1, false, false, false, 0);
	LoadTexture("sky/back.jpg", "SkyBack", 1, 1, false, false, false, 0);
	ResetLighting();

	SkyBox = new MeshObject(this->object, "SkyBox");
	SkyBox->no_collider = true;

	//create a skybox that extends by default 30 miles (30 * 5280 ft) in each direction
	float skysize = GetConfigInt("Skyscraper.SBS.HorizonDistance", 30) * 5280;
	sbs->ResetTextureMapping(true);
	WallObject *wall = new WallObject(SkyBox, SkyBox->object, true);

	wall->AddQuad( //front
		"SkyFront",
		"SkyFront",
		Ogre::Vector3(-skysize, -skysize, -skysize),
		Ogre::Vector3(skysize, -skysize, -skysize),
		Ogre::Vector3(skysize, skysize, -skysize),
		Ogre::Vector3(-skysize, skysize, -skysize), 1, 1, false);
	wall->AddQuad( //right
		"SkyRight",
		"SkyRight",
		Ogre::Vector3(skysize, -skysize, -skysize),
		Ogre::Vector3(skysize, -skysize, skysize),
		Ogre::Vector3(skysize, skysize, skysize),
		Ogre::Vector3(skysize, skysize, -skysize), 1, 1, false);
	wall->AddQuad( //back
		"SkyBack",
		"SkyBack",
		Ogre::Vector3(skysize, -skysize, skysize),
		Ogre::Vector3(-skysize, -skysize, skysize),
		Ogre::Vector3(-skysize, skysize, skysize),
		Ogre::Vector3(skysize, skysize, skysize), 1, 1, false);
	wall->AddQuad( //left
		"SkyLeft",
		"SkyLeft",
		Ogre::Vector3(-skysize, -skysize, skysize),
		Ogre::Vector3(-skysize, -skysize, -skysize),
		Ogre::Vector3(-skysize, skysize, -skysize),
		Ogre::Vector3(-skysize, skysize, skysize), 1, 1, false);
	wall->AddQuad( //bottom
		"SkyBottom",
		"SkyBottom",
		Ogre::Vector3(-skysize, -skysize, skysize),
		Ogre::Vector3(skysize, -skysize, skysize),
		Ogre::Vector3(skysize, -skysize, -skysize),
		Ogre::Vector3(-skysize, -skysize, -skysize), 1, 1, false);
	wall->AddQuad( //top
		"SkyTop",
		"SkyTop",
		Ogre::Vector3(-skysize, skysize, -skysize),
		Ogre::Vector3(skysize, skysize, -skysize),
		Ogre::Vector3(skysize, skysize, skysize),
		Ogre::Vector3(-skysize, skysize, skysize), 1, 1, false);

	sbs->ResetTextureMapping();
	delete wall;
}

int SBS::GetFloorNumber(float altitude, int lastfloor, bool checklastfloor)
{
	//Returns floor number located at a specified altitude

	if (TotalFloors() == 0)
		return 0;

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
					if (i == Floors - 1 && GetFloor(i)->Altitude <= altitude)
						return i; //return top floor if on top
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

void SBS::ListAltitudes()
{
	//dumps the floor altitude list

	Report("--- Floor Altitudes ---\n");
	for (int i = -Basements; i < Floors; i++)
		Report(std::string(_itoa(i, intbuffer, 10)) + "(" + GetFloor(i)->ID + ")\t----\t" + std::string(_gcvt(GetFloor(i)->FullHeight(), 6, buffer)) + "\t----\t" + std::string(_gcvt(GetFloor(i)->Altitude, 6, buffer)));
}

Object* SBS::CreateShaft(int number, int type, float CenterX, float CenterZ, int _startfloor, int _endfloor)
{
	//create a shaft object

	for (size_t i = 0; i < ShaftArray.size(); i++)
	{
		if (ShaftArray[i].number == number)
		{
			std::string num = Ogre::StringConverter::toString(number);
			ReportError("Shaft " + num + " already exists");
			return 0;
		}
	}

	//verify floor range
	if (_startfloor > _endfloor)
	{
		ReportError("CreateShaft: starting floor is greater than ending floor");
		return 0;
	}

	if (IsValidFloor(_startfloor) == false)
	{
		std::string num = Ogre::StringConverter::toString(_startfloor);
		ReportError("CreateShaft: Invalid starting floor " + num);
		return 0;
	}
	if (IsValidFloor(_endfloor) == false)
	{
		std::string num = Ogre::StringConverter::toString(_endfloor);
		ReportError("CreateShaft: Invalid ending floor " + num);
		return 0;
	}

	ShaftArray.resize(ShaftArray.size() + 1);
	ShaftArray[ShaftArray.size() - 1].number = number;
	ShaftArray[ShaftArray.size() - 1].object = new Shaft(number, type, CenterX, CenterZ, _startfloor, _endfloor);
	return ShaftArray[ShaftArray.size() - 1].object->object;
}

Object* SBS::CreateStairwell(int number, float CenterX, float CenterZ, int _startfloor, int _endfloor)
{
	//create a stairwell object

	for (size_t i = 0; i < StairsArray.size(); i++)
	{
		if (StairsArray[i].number == number)
		{
			std::string num = Ogre::StringConverter::toString(number);
			ReportError("Stairwell " + num + " already exists");
			return 0;
		}
	}

	//verify floor range
	if (_startfloor > _endfloor)
	{
		ReportError("CreateStairwell: starting floor is greater than ending floor");
		return 0;
	}
	if (IsValidFloor(_startfloor) == false)
	{
		std::string num = Ogre::StringConverter::toString(_startfloor);
		ReportError("CreateStairwell: Invalid starting floor " + num);
		return 0;
	}
	if (IsValidFloor(_endfloor) == false)
	{
		std::string num = Ogre::StringConverter::toString(_endfloor);
		ReportError("CreateStairwell: Invalid ending floor " + num);
		return 0;
	}

	StairsArray.resize(StairsArray.size() + 1);
	StairsArray[StairsArray.size() - 1].number = number;
	StairsArray[StairsArray.size() - 1].object = new Stairs(number, CenterX, CenterZ, _startfloor, _endfloor);
	return StairsArray[StairsArray.size() - 1].object->object;
}

bool SBS::NewElevator(int number)
{
	//create a new elevator object
	for (size_t i = 0; i < ElevatorArray.size(); i++)
		if (ElevatorArray[i].number == number)
			return false;
	ElevatorArray.resize(ElevatorArray.size() + 1);
	ElevatorArray[ElevatorArray.size() - 1].number = number;
	ElevatorArray[ElevatorArray.size() - 1].object = new Elevator(number);
	return true;
}

bool SBS::NewFloor(int number)
{
	//create a new floor object
	for (size_t i = 0; i < FloorArray.size(); i++)
		if (FloorArray[i].number == number)
			return false;
	FloorArray.resize(FloorArray.size() + 1);
	FloorArray[FloorArray.size() - 1].number = number;
	FloorArray[FloorArray.size() - 1].object = new Floor(number);

	if (number < 0)
		Basements++;
	else
		Floors++;
	return true;
}

int SBS::Elevators()
{
	//return the number of elevators
	return (int)ElevatorArray.size();
}

int SBS::TotalFloors()
{
	//return the number of floors
	return (int)FloorArray.size();
}

int SBS::Shafts()
{
	//return the number of shafts
	return (int)ShaftArray.size();
}

int SBS::StairsNum()
{
	//return the number of stairs
	return (int)StairsArray.size();
}

Floor* SBS::GetFloor(int number)
{
	//return pointer to floor object

	if (Basements + number < 0)
		return 0;

	if (FloorArray.size() > 0)
	{
		//quick prediction
		if (Basements + number < (int)FloorArray.size())
		{
			FloorMap floor = FloorArray[Basements + number];
			if (floor.number == number)
			{
				if (floor.object)
					return floor.object;
				else
					return 0;
			}
			else if (number < 0)
			{
				if (-(number + 1) < (int)FloorArray.size())
				{
					floor = FloorArray[-(number + 1)];
					if (floor.number == number)
					{
						if (floor.object)
							return floor.object;
						else
							return 0;
					}
				}
			}
		}
	}

	for (size_t i = 0; i < (int)FloorArray.size(); i++)
		if (FloorArray[i].number == number)
			return FloorArray[i].object;
	return 0;
}

Elevator* SBS::GetElevator(int number)
{
	//return pointer to elevator object

	if (number < 1 || number > Elevators())
		return 0;

	if ((int)ElevatorArray.size() > number - 1)
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

	for (size_t i = 0; i < ElevatorArray.size(); i++)
		if (ElevatorArray[i].number == number)
			return ElevatorArray[i].object;
	return 0;
}

Shaft* SBS::GetShaft(int number)
{
	//return pointer to shaft object

	if (number < 1 || number > Shafts())
		return 0;

	if ((int)ShaftArray.size() > number - 1)
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

	for (size_t i = 0; i < ShaftArray.size(); i++)
		if (ShaftArray[i].number == number)
			return ShaftArray[i].object;
	return 0;
}

Stairs* SBS::GetStairs(int number)
{
	//return pointer to stairs object

	if (number < 1 || number > StairsNum())
		return 0;

	if ((int)StairsArray.size() > number - 1)
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

	for (size_t i = 0; i < StairsArray.size(); i++)
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

	std::string temp = direction;
	SetCase(temp, false);

	if (temp == "left")
		wall_orientation = 0;
	else if (temp == "center")
		wall_orientation = 1;
	else if (temp == "right")
		wall_orientation = 2;
	else
	{
		ReportError("SetWallOrientation: Invalid wall orientation");
		return false;
	}
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

	std::string temp = direction;
	SetCase(temp, false);

	if (temp == "bottom")
		floor_orientation = 0;
	else if (temp == "center")
		floor_orientation = 1;
	else if (temp == "top")
		floor_orientation = 2;
	else
	{
		ReportError("SetFloorOrientation: Invalid floor orientation");
		return false;
	}
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

void SBS::SetPlanarMapping(bool flat, bool X, bool Y, bool Z)
{
	//sets planar texture mapping parameters
	//X, Y and Z reverse planar texture mapping per axis
	//Flat determines if depth should be ignored when mapping

	//first backup old parameters
	BackupMapping();

	//now set new parameters
	RevX = X;
	RevY = Y;
	RevZ = Z;
	MapUV[0] = Ogre::Vector2(0, 0);
	MapUV[1] = Ogre::Vector2(1, 0);
	MapUV[2] = Ogre::Vector2(1, 1);
	PlanarFlat = flat;
	MapMethod = 0;
}

void SBS::SetTextureMapping(int vertindex1, Ogre::Vector2 uv1, int vertindex2, Ogre::Vector2 uv2, int vertindex3, Ogre::Vector2 uv3)
{
	//Manually sets UV texture mapping.  Use ResetTextureMapping to return to default values

	BackupMapping();

	//set new values
	MapIndex[0] = vertindex1;
	MapIndex[1] = vertindex2;
	MapIndex[2] = vertindex3;
	MapUV[0] = uv1;
	MapUV[1] = uv2;
	MapUV[2] = uv3;
	MapMethod = 1;
}

void SBS::SetTextureMapping2(std::string x1, std::string y1, std::string z1, Ogre::Vector2 uv1, std::string x2, std::string y2, std::string z2, Ogre::Vector2 uv2, std::string x3, std::string y3, std::string z3, Ogre::Vector2 uv3)
{
	//Manually sets UV texture mapping (advanced version)
	//Use ResetTextureMapping to return to default values

	BackupMapping();

	MapVerts1[0] = x1;
	MapVerts1[1] = y1;
	MapVerts1[2] = z1;
	MapVerts2[0] = x2;
	MapVerts2[1] = y2;
	MapVerts2[2] = z2;
	MapVerts3[0] = x3;
	MapVerts3[1] = y3;
	MapVerts3[2] = z3;
	MapUV[0] = uv1;
	MapUV[1] = uv2;
	MapUV[2] = uv3;
	MapMethod = 2;
}

void SBS::BackupMapping()
{
	//backup texture mapping parameters
	if (MapMethod == 0)
	{
		OldRevX = RevX;
		OldRevY = RevY;
		OldRevZ = RevZ;
		OldPlanarFlat = PlanarFlat;
	}
	else
	{
		for (int i = 0; i <= 2; i++)
		{
			if (MapMethod == 1)
				OldMapIndex[i] = MapIndex[i];
			if (MapMethod == 2)
			{
				OldMapVerts1[i] = MapVerts1[i];
				OldMapVerts1[i] = MapVerts1[i];
				OldMapVerts1[i] = MapVerts1[i];
			}
		}
	}
	for (int i = 0; i <= 2; i++)
		OldMapUV[i] = MapUV[i];
	OldMapMethod = MapMethod;
}

void SBS::GetTextureMapping(std::vector<Ogre::Vector3> &vertices, Ogre::Vector3 &v1, Ogre::Vector3 &v2, Ogre::Vector3 &v3)
{
	//returns texture mapping coordinates for the specified polygon index, in the v1, v2, and v3 vectors
	//this performs one of 3 methods - planar mapping, index mapping and manual vertex mapping

	if (MapMethod == 0)
	{
		//planar method

		Ogre::Vector2 x, y, z;
		std::vector<Ogre::Vector3> varray1, varray2;
		bool rev_x = false, rev_z = false;

		//copy vertices into polygon object
		varray1.reserve(vertices.size());
		for (int i = 0; i < (int)vertices.size(); i++)
			varray1.push_back(vertices[i]);

		//determine the largest projection dimension (the dimension that the polygon is generally on;
		//with a floor Y would be biggest)
		Ogre::Plane plane = Ogre::Plane(varray1[0], varray1[1], varray1[2]);
		Ogre::Vector3 normal = -plane.normal; //flip normal for coordinate system conversion
		int projDimension = 0; //x; faces left/right

		if (fabsf (normal.y) > fabsf (normal.x) && fabsf (normal.y) > fabsf (normal.z))
			projDimension = 1; //y biggest; faces up/down
		else if (fabsf (normal.z) > fabsf (normal.x))
			projDimension = 2; //z biggest; faces front/back

		size_t selX = (1 << projDimension) & 0x3;
		size_t selY = (1 << selX) & 0x3;

		varray2.reserve(varray1.size());
		for (int i = 0; i < (int)varray1.size(); i++)
		{
			Ogre::Vector3 tmpvertex = varray1[i];
			varray2.push_back(Ogre::Vector3(tmpvertex[selX], tmpvertex[selY], 0));
		}

		if (RevX == true || (normal.x < 0.001 && normal.z < 0.001 && fabs(normal.x) > 0.999 && fabs(normal.z) > 0.999) || normal.z < -0.999)
			rev_x = true;

		if (RevZ == true || (normal.x > 0.001 && normal.z > 0.001 && fabs(normal.x) > 0.999 && fabs(normal.z) > 0.999) || normal.x > 0.999)
			rev_z = true;

		//get extents of both dimensions, since the polygon is projected in 2D as X and Y coordinates
		Ogre::Vector2 a, b;
		a = GetExtents(varray2, 1);
		b = GetExtents(varray2, 2);

		//set the result 2D coordinates
		if (projDimension == 0)
		{
			if (rev_z == false)
			{
				v1.z = b.x; //left
				v2.z = b.y; //right
				v3.z = b.y; //right
			}
			else
			{
				v1.z = b.y; //right
				v2.z = b.x; //left
				v3.z = b.x; //left
			}
			if (RevY == false)
			{
				v1.y = a.y; //top
				v2.y = a.y; //top
				v3.y = a.x; //bottom
			}
			else
			{
				v1.y = a.x; //bottom
				v2.y = a.x; //bottom
				v3.y = a.y; //top
			}
		}
		if (projDimension == 1)
		{
			if (rev_x == false)
			{
				v1.x = b.x; //left
				v2.x = b.y; //right
				v3.x = b.y; //right
			}
			else
			{
				v1.x = b.y; //right
				v2.x = b.x; //left
				v3.x = b.x; //left
			}
			if (rev_z == false)
			{
				v1.z = a.y; //top
				v2.z = a.y; //top
				v3.z = a.x; //bottom
			}
			else
			{
				v1.z = a.x; //bottom
				v2.z = a.x; //bottom
				v3.z = a.y; //top
			}
		}
		if (projDimension == 2)
		{
			if (rev_x == false)
			{
				v1.x = a.x; //left
				v2.x = a.y; //right
				v3.x = a.y; //right
			}
			else
			{
				v1.x = a.y; //right
				v2.x = a.x; //left
				v3.x = a.x; //left
			}
			if (RevY == false)
			{
				v1.y = b.y; //top
				v2.y = b.y; //top
				v3.y = b.x; //bottom
			}
			else
			{
				v1.y = b.x; //bottom
				v2.y = b.x; //bottom
				v3.y = b.y; //top
			}
		}

		//use the plane equation to get the coordinate values of the dropped dimension
		if (projDimension == 0)
		{
			v1.x = -((normal.y * v1.y) + (normal.z * v1.z) + plane.d) / normal.x; //get X
			v2.x = -((normal.y * v2.y) + (normal.z * v2.z) + plane.d) / normal.x; //get X
			v3.x = -((normal.y * v3.y) + (normal.z * v3.z) + plane.d) / normal.x; //get X

			if (PlanarFlat == true)
				v3.x = v2.x;
		}
		if (projDimension == 1)
		{
			v1.y = -((normal.x * v1.x) + (normal.z * v1.z) + plane.d) / normal.y; //get Y
			v2.y = -((normal.x * v2.x) + (normal.z * v2.z) + plane.d) / normal.y; //get Y
			v3.y = -((normal.x * v3.x) + (normal.z * v3.z) + plane.d) / normal.y; //get Y

			if (PlanarFlat == true)
				v3.y = v2.y;
		}
		if (projDimension == 2)
		{
			v1.z = -((normal.x * v1.x) + (normal.y * v1.y) + plane.d) / normal.z; //get Z
			v2.z = -((normal.x * v2.x) + (normal.y * v2.y) + plane.d) / normal.z; //get Z
			v3.z = -((normal.x * v3.x) + (normal.y * v3.y) + plane.d) / normal.z; //get Z

			if (PlanarFlat == true)
				v3.z = v2.z;
		}
	}
	if (MapMethod == 1)
	{
		//index method
		v1 = vertices[MapIndex[0]];
		v2 = vertices[MapIndex[1]];
		v3 = vertices[MapIndex[2]];
	}
	if (MapMethod == 2)
	{
		//advanced manual vertex method

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				std::string string;
				if (j == 0)
					string = MapVerts1[i];
				if (j == 1)
					string = MapVerts2[i];
				if (j == 2)
					string = MapVerts3[i];

				SetCase(string, false);

				//find X component
				int location = string.find("x");
				if (location >= 0)
				{
					std::string number = string.substr(location + 1);
					if (atoi(number.c_str()) < (int)vertices.size())
						ReplaceAll(string, std::string("x" + number).c_str(), _gcvt(vertices[atoi(number.c_str())].x, 12, buffer));
					else
						ReplaceAll(string, std::string("x" + number).c_str(), "0"); //number value out of bounds
				}

				//find Y component
				location = string.find("y");
				if (location >= 0)
				{
					std::string number = string.substr(location + 1);
					if (atoi(number.c_str()) < (int)vertices.size())
						ReplaceAll(string, std::string("y" + number).c_str(), _gcvt(vertices[atoi(number.c_str())].y, 12, buffer));
					else
						ReplaceAll(string, std::string("y" + number).c_str(), "0"); //number value out of bounds
				}

				//find Z component
				location = string.find("z");
				if (location >= 0)
				{
					std::string number = string.substr(location + 1);
					if (atoi(number.c_str()) < (int)vertices.size())
						ReplaceAll(string, std::string("z" + number).c_str(), _gcvt(vertices[atoi(number.c_str())].z, 12, buffer));
					else
						ReplaceAll(string, std::string("z" + number).c_str(), "0"); //number value out of bounds
				}

				//store values
				if (i == 0)
				{
					if (j == 0)
						v1.x = atof(string.c_str());
					if (j == 1)
						v2.x = atof(string.c_str());
					if (j == 2)
						v3.x = atof(string.c_str());
				}
				if (i == 1)
				{
					if (j == 0)
						v1.y = atof(string.c_str());
					if (j == 1)
						v2.y = atof(string.c_str());
					if (j == 2)
						v3.y = atof(string.c_str());
				}
				if (i == 2)
				{
					if (j == 0)
						v1.z = atof(string.c_str());
					if (j == 1)
						v2.z = atof(string.c_str());
					if (j == 2)
						v3.z = atof(string.c_str());
				}
			}
		}
	}
}

void SBS::ResetTextureMapping(bool todefaults)
{
	//Resets UV texture mapping to defaults or previous values
	if (todefaults == true)
	{
		if (DefaultMapper == 0)
			SetPlanarMapping(false, false, false, false);
		if (DefaultMapper == 1)
			SetTextureMapping(0, Ogre::Vector2(0, 0), 1, Ogre::Vector2(1, 0), 2, Ogre::Vector2(1, 1));
		if (DefaultMapper == 2)
			SetTextureMapping2("x0", "y0", "z0", Ogre::Vector2(0, 0), "x1", "y1", "z1", Ogre::Vector2(1, 0), "x2", "y2", "z2", Ogre::Vector2(1, 1));
	}
	else
	{
		if (OldMapMethod == 0)
			SetPlanarMapping(OldPlanarFlat, OldRevX, OldRevY, OldRevZ);
		if (OldMapMethod == 1)
			SetTextureMapping(OldMapIndex[0], OldMapUV[0], OldMapIndex[1], OldMapUV[1], OldMapIndex[2], OldMapUV[2]);
		if (OldMapMethod == 2)
			SetTextureMapping2(OldMapVerts1[0], OldMapVerts1[1], OldMapVerts1[2], OldMapUV[0], OldMapVerts2[0], OldMapVerts2[1], OldMapVerts2[2], OldMapUV[1], OldMapVerts3[0], OldMapVerts3[1], OldMapVerts3[2], OldMapUV[2]);
	}
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

int SBS::AddDoorwayWalls(WallObject *wallobject, const char *texture, float tw, float th)
{
	//add joining doorway polygons if needed

	int index = 0;
	if (wall1a == true && wall2a == true)
	{
		DrawWalls(true, true, false, false, false, false);
		if (fabs(wall_extents_x.x - wall_extents_x.y) > fabs(wall_extents_z.x - wall_extents_z.y))
		{
			//doorway is facing forward/backward
			index = AddWallMain(wallobject, "DoorwayLeft", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.x, wall_extents_z.y, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th, true);
			AddWallMain(wallobject, "DoorwayRight", texture, 0, wall_extents_x.y, wall_extents_z.x, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th, true);
			AddFloorMain(wallobject, "DoorwayTop", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y, wall_extents_y.y, tw, th, true);
		}
		else
		{
			//doorway is facing left/right
			AddWallMain(wallobject, "DoorwayLeft", texture, 0, wall_extents_x.x, wall_extents_z.y, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th, true);
			AddWallMain(wallobject, "DoorwayRight", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.y, wall_extents_z.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.y - wall_extents_y.x, wall_extents_y.x, wall_extents_y.x, tw, th, true);
			AddFloorMain(wallobject, "DoorwayTop", texture, 0, wall_extents_x.x, wall_extents_z.x, wall_extents_x.y, wall_extents_z.y, wall_extents_y.y, wall_extents_y.y, tw, th, true);
		}
		ResetWalls();
		ResetDoorwayWalls();
	}
	return index;
}

void SBS::ResetDoorwayWalls()
{
	wall1a = false;
	wall1b = false;
	wall2a = false;
	wall2b = false;
	wall_extents_x = 0;
	wall_extents_y = 0;
	wall_extents_z = 0;
}

void SBS::SetAutoSize(bool x, bool y)
{
	//enable or disable texture autosizing
	AutoX = x;
	AutoY = y;
}

void SBS::GetAutoSize(bool &x, bool &y)
{
	//return autosizing values
	x = AutoX;
	y = AutoY;
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

void SBS::SetListenerPosition(const Ogre::Vector3 &position)
{
	//set position of sound listener object

	//calculate sound velocity
	if (GetElapsedTime() > 0)
	{
		listener_velocity.x = (position.x - listener_position.x) * (1000 / GetElapsedTime());
		listener_velocity.y = (position.y - listener_position.y) * (1000 / GetElapsedTime());
		listener_velocity.z = (position.z - listener_position.z) * (1000 / GetElapsedTime());
	}

	listener_position.x = position.x;
	listener_position.y = position.y;
	listener_position.z = position.z;

	if (DisableSound == false)
		soundsys->set3DListenerAttributes(0, &listener_position, &listener_velocity, &listener_forward, &listener_up);
}

void SBS::SetListenerDirection(const Ogre::Vector3 &front, const Ogre::Vector3 &top)
{
	//set direction of sound listener object
	listener_forward.x = front.x;
	listener_forward.y = front.y;
	listener_forward.z = front.z;
	listener_up.x = top.x;
	listener_up.y = top.y;
	listener_up.z = top.z;

	if (DisableSound == false)
		soundsys->set3DListenerAttributes(0, &listener_position, &listener_velocity, &listener_forward, &listener_up);
}

void SBS::SetTextureOverride(const char *mainneg, const char *mainpos, const char *sideneg, const char *sidepos, const char *top, const char *bottom)
{
	//set override textures and enable override
	mainnegtex = mainneg;
	TrimString(mainnegtex);
	mainpostex = mainpos;
	TrimString(mainpostex);
	sidenegtex = sideneg;
	TrimString(sidenegtex);
	sidepostex = sidepos;
	TrimString(sidepostex);
	toptex = top;
	TrimString(toptex);
	bottomtex = bottom;
	TrimString(bottomtex);
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

WallObject* SBS::AddWall(const char *meshname, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float altitude1, float altitude2, float tw, float th)
{
	//meshname can either be:
	//external, landscape, or buildings

	//Adds a wall with the specified dimensions
	std::string mesh = meshname;
	TrimString(mesh);
	SetCase(mesh, false);

	WallObject *wall;
	if (mesh == "external")
		wall = External->CreateWallObject(this->object, name);
	if (mesh == "buildings")
		wall = Buildings->CreateWallObject(this->object, name);
	if (mesh == "landscape")
		wall = Landscape->CreateWallObject(this->object, name);

	AddWallMain(wall, name, texture, thickness, x1, z1, x2, z2, height_in1, height_in2, altitude1, altitude2, tw, th, true);
	return wall;
}

WallObject* SBS::AddFloor(const char *meshname, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float altitude1, float altitude2, float tw, float th)
{
	//meshname can either be:
	//external, landscape, or buildings

	//Adds a floor with the specified dimensions and vertical offset
	std::string mesh = meshname;
	TrimString(mesh);
	SetCase(mesh, false);

	WallObject *wall;
	if (mesh == "external")
		wall = External->CreateWallObject(this->object, name);
	if (mesh == "buildings")
		wall = Buildings->CreateWallObject(this->object, name);
	if (mesh == "landscape")
		wall = Landscape->CreateWallObject(this->object, name);

	AddFloorMain(wall, name, texture, thickness, x1, z1, x2, z2, altitude1, altitude2, tw, th, true);
	return wall;
}

WallObject* SBS::AddGround(const char *name, const char *texture, float x1, float z1, float x2, float z2, float altitude, int tile_x, int tile_z)
{
	//Adds ground based on a tiled-floor layout, with the specified dimensions and vertical offset
	//this does not support thickness

	Ogre::Vector3 v1, v2, v3, v4;

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

	WallObject *wall = Landscape->CreateWallObject(this->object, name);

	Report("Creating ground...");

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

			AddFloorMain(wall, name, texture, 0, i, j, i + sizex, j + sizez, altitude, altitude, 1, 1, false);
		}
	}
	Report("Finished ground");
	return wall;
}

void SBS::EnableFloorRange(int floor, int range, bool value, bool enablegroups, int shaftnumber)
{
	//turn on/off a range of floors
	//if range is 3, show shaft on current floor (floor), and 1 floor below and above (3 total floors)
	//if range is 1, show only the current floor (floor)

	SBS_PROFILE("SBS::EnableFloorRange");

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

	//disable floors 1 floor outside of range, unless part of group
	if (value == true)
	{
		int floorval = floor - additionalfloors - 1;
		if (IsValidFloor(floorval) && GetFloor(floor)->IsInGroup(floorval) == false)
				GetFloor(floorval)->Enabled(false);

		floorval = floor + additionalfloors + 1;
		if (IsValidFloor(floorval) && GetFloor(floor)->IsInGroup(floorval) == false)
			GetFloor(floorval)->Enabled(false);
	}

	//enable floors within range
	for (int i = floor - additionalfloors; i <= floor + additionalfloors; i++)
	{
		if (IsValidFloor(i))
		{
			if (shaftnumber > 0)
			{
				//if a shaft is specified, only show the floor if it is in the related shaft's ShowFloorsList array
				if (GetShaft(shaftnumber)->ShowFloors == true)
				{
					int index = -1;
					for (int j = 0; j < (int)GetShaft(shaftnumber)->ShowFloorsList.size(); j++)
					{
						if (GetShaft(shaftnumber)->ShowFloorsList[j] == i)
							index = j;
					}
					if (index != -1 && value == true)
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

	int index = -1;
	for (int i = 0; i < (int)doorcallbacks.size(); i++)
	{
		if (doorcallbacks[i] == door)
			index = i;
	}

	if (index == -1)
	{
		//if door isn't already in the array, add it
		doorcallbacks.push_back(door);
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
	int index = -1;
	for (int i = 0; i < (int)doorcallbacks.size(); i++)
	{
		if (doorcallbacks[i] == door)
			index = i;
	}

	if (index != -1 && doorcallbacks[index])
	{
		//unregister existing door callback
		if (doorcallbacks[index]->IsMoving == false)
		{
			for (int i = 0; i < (int)doorcallbacks.size(); i++)
			{
				if (doorcallbacks[i] == door)
					doorcallbacks.erase(doorcallbacks.begin() + i);
			}
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

	int index = -1;
	for (int i = 0; i < (int)buttoncallbacks.size(); i++)
	{
		if (buttoncallbacks[i] == button)
			index = i;
	}

	if (index == -1)
	{
		//if call button isn't already in the array, add it
		buttoncallbacks.push_back(button);
	}
	else
		return false;
	return true;
}

bool SBS::UnregisterCallButtonCallback(CallButton *button)
{
	int index = -1;
	for (int i = 0; i < (int)buttoncallbacks.size(); i++)
	{
		if (buttoncallbacks[i] == button)
			index = i;
	}

	if (index != -1 && buttoncallbacks[index])
	{
		//unregister existing call button callback
		for (int i = 0; i < (int)buttoncallbacks.size(); i++)
		{
			if (buttoncallbacks[i] == button)
				buttoncallbacks.erase(buttoncallbacks.begin() + i);
		}
	}
	else
		return false;
	return true;
}

void SBS::ProcessTextureFlip(float tw, float th)
{
	//process texture flip info

	if (tw == 0)
		tw = 1;
	if (th == 0)
		th = 1;

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

std::string SBS::GetTextureMaterial(const char *name, bool &result, const char *polygon_name)
{
	//perform a lookup on a texture, and return material name if it exists, or "Default" if not
	//returns false in &result if texture load failed, and if default material was used instead
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(name);

	std::string final_material = name;

	if (!material.get())
	{
		//if material's not found, display a warning and use a default material
		std::string message;
		if (polygon_name)
			message = "Texture '" + std::string(name) + "' not found for polygon '" + std::string(polygon_name) + "'; using default material";
		else
			message = "Texture '" + std::string(name) + "' not found; using default material";
		ReportError(message);

		//set to default material
		final_material = "Default";
		result = false;
	}
	else
		result = true;
	return final_material;
}

bool SBS::GetTextureTiling(const char *texture, float &tw, float &th)
{
	//get per-texture tiling values from the textureinfo array
	for (int i = 0; i < (int)textureinfo.size(); i++)
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

bool SBS::GetTextureForce(const char *texture, bool &enable_force, bool &force_mode)
{
	//get per-texture tiling values from the textureinfo array
	for (int i = 0; i < (int)textureinfo.size(); i++)
	{
		if (textureinfo[i].name == texture)
		{
			enable_force = textureinfo[i].enable_force;
			force_mode = textureinfo[i].force_mode;
			return true;
		}
	}
	return false;
}

void SBS::ProcessCallButtons()
{
	//process all registered call buttons

	//the up and down sections need to be processed separately due to the removal of callbacks
	//during the run of each

	for (int i = 0; i < (int)buttoncallbacks.size(); i++)
	{
		//process up calls
		if (buttoncallbacks[i])
			buttoncallbacks[i]->Loop(true);
	}

	for (int i = 0; i < (int)buttoncallbacks.size(); i++)
	{
		//process down calls
		if (buttoncallbacks[i])
			buttoncallbacks[i]->Loop(false);
	}
}

void SBS::ProcessDoors()
{
	//process all registered doors
	for (int i = 0; i < (int)doorcallbacks.size(); i++)
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
	return (int)doorcallbacks.size();
}

int SBS::GetCallButtonCallbackCount()
{
	//return the number of registered call button callbacks
	return (int)buttoncallbacks.size();
}

bool SBS::Mount(const char *filename, const char *path)
{
	//mounts a zip file into the virtual filesystem

	std::string file = VerifyFile(filename);
	std::string Path = path;

	Report("Mounting " + file + " as path " + Path);
	try
	{
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(file, "Zip", Path, true);
	}
	catch (Ogre::Exception &e)
	{
		return ReportError("Error mounting file " + file + "\n" + e.getDescription());
	}
	return true;
}

void SBS::FreeTextureImages()
{
	//unload images in all texture wrappers

	//for (int i = 0; i < engine->GetTextureList()->GetCount(); i++)
		//engine->GetTextureList()->Get(i)->SetImageFile(0);
}

void SBS::AddFloorAutoArea(Ogre::Vector3 start, Ogre::Vector3 end)
{
	//adds an auto area that enables/disables floors

	AutoArea newarea;
	newarea.start = start;
	newarea.end = end;
	newarea.inside = false;
	newarea.camerafloor = 0;
	FloorAutoArea.push_back(newarea);
}

void SBS::CheckAutoAreas()
{
	//check all automatic areas
	
	Ogre::Vector3 position = camera->GetPosition();
	int floor = camera->CurrentFloor;

	for (int i = 0; i < (int)FloorAutoArea.size(); i++)
	{
		//reset inside value if floor changed
		if (FloorAutoArea[i].camerafloor != floor)
			FloorAutoArea[i].inside = false;

		if (InBox(FloorAutoArea[i].start, FloorAutoArea[i].end, position) == true && FloorAutoArea[i].inside == false)
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
		if (InBox(FloorAutoArea[i].start, FloorAutoArea[i].end, position) == false && FloorAutoArea[i].inside == true)
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

int SBS::GetMeshCount()
{
	//return total number of mesh objects
	return meshes.size();
}

int SBS::GetTextureCount()
{
	//return total number of textures
	//return engine->GetTextureList()->GetCount();
	return 0;
}

int SBS::GetMaterialCount()
{
	//return total number of materials
	//return engine->GetMaterialList()->GetCount();
	return 0;
}

int SBS::GetMeshFactoryCount()
{
	//return total number of mesh factories
	//return engine->GetMeshFactories()->GetCount();
	return 0;
}

Object* SBS::AddSound(const char *name, const char *filename, Ogre::Vector3 position, int volume, int speed, float min_distance, float max_distance, float dir_radiation, Ogre::Vector3 direction)
{
	//create a looping sound object
	sounds.resize(sounds.size() + 1);
	Sound *sound = sounds[sounds.size() - 1];
	sound = new Sound(this->object, name, false);

	//set parameters and play sound
	sound->SetPosition(position);
	sound->SetDirection(direction);
	sound->SetVolume(volume);
	sound->SetSpeed(speed);
	sound->SetDistances(min_distance, max_distance);
	sound->SetDirection(direction);
	sound->SetDirectionalRadiation(dir_radiation);
	sound->Load(filename);
	sound->Loop(true);
	sound->Play();

	return sound->object;
}

int SBS::GetSoundCount()
{
	//return total number of allocated sounds
	return soundcount;
}

void SBS::IncrementSoundCount()
{
	soundcount++;
}

void SBS::DecrementSoundCount()
{
	soundcount--;
}

float SBS::ToLocal(float remote_value)
{
	//convert remote (Crystal Space) vertex positions to local (SBS) positions

	//note - OGRE uses a right-hand coordinate system, while SBS uses left-hand.
	//this means that all Z values that use this function must be inverted.

	return remote_value * UnitScale;
}

Ogre::Vector2 SBS::ToLocal(const Ogre::Vector2& remote_value)
{
	//convert remote (Crystal Space) vertex positions to local (SBS) positions

	//note - OGRE uses a right-hand coordinate system, while SBS uses left-hand.
	//this means that all Z values that use this function must be inverted.

	return remote_value * UnitScale;
}

Ogre::Vector3 SBS::ToLocal(const Ogre::Vector3& remote_value, bool rescale)
{
	//convert remote (Crystal Space) vertex positions to local (SBS) positions
	//also convert Z value to OGRE's right-hand coordinate system

	Ogre::Vector3 newvalue;
	newvalue.x = remote_value.x;
	newvalue.y = remote_value.y;
	newvalue.z = -remote_value.z; //flip z value for OGRE's right-hand coordinate system
	if (rescale == true)
		return newvalue * UnitScale;
	else
		return newvalue;
}

float SBS::ToRemote(float local_value)
{
	//convert local (SBS) vertex positions to remote (Crystal Space) positions

	//note - OGRE uses a right-hand coordinate system, while SBS uses left-hand.
	//this means that all Z values that use this function must be inverted.

	return local_value / UnitScale;
}

Ogre::Vector2 SBS::ToRemote(const Ogre::Vector2& local_value)
{
	//convert local (SBS) vertex positions to remote (Crystal Space) positions

	//note - OGRE uses a right-hand coordinate system, while SBS uses left-hand.
	//this means that all Z values that use this function must be inverted.

	return local_value / UnitScale;
}

Ogre::Vector3 SBS::ToRemote(const Ogre::Vector3& local_value, bool rescale)
{
	//convert local (SBS) vertex positions to remote (Crystal Space) positions
	
	Ogre::Vector3 newvalue;
	newvalue.x = local_value.x;
	newvalue.y = local_value.y;
	newvalue.z = -local_value.z; //flip z value for OGRE's right-hand coordinate system
	if (rescale == true)
		return (newvalue / UnitScale);
	else
		return newvalue;
}

int SBS::GetObjectCount()
{
	//return number of registered SBS objects
	return ObjectCount;
}

Object* SBS::GetObject(int number)
{
	//return object pointer from global array
	if (number >= 0 && number < (int)ObjectArray.size())
		return ObjectArray[number];
	else
		return 0;
}

int SBS::RegisterObject(Object *object)
{
	//add object to global array
	ObjectCount++;
	ObjectArray.push_back(object);
	return (int)ObjectArray.size() - 1;
}

bool SBS::UnregisterObject(int number)
{
	//remove object
	//note - this doesn't delete the objects
	ObjectCount--;

	if (number < (int)ObjectArray.size())
	{
		if (ObjectArray[number])
		{
			if (ObjectArray[number]->GetNumber() == number)
			{
				ObjectArray[number] = 0;
				return true;
			}
		}
	}
	return false;
}

Ogre::Vector2 SBS::CalculateSizing(const char *texture, Ogre::Vector2 x, Ogre::Vector2 y, Ogre::Vector2 z, float tw, float th)
{
	//calculate autosizing based on polygon extents

	//Call texture autosizing formulas
	float tw2 = tw, th2 = th;

	bool force_enable = false, force_mode = false;
	bool result = GetTextureForce(texture, force_enable, force_mode);

	bool is_wall = true;
	if (fabs(y.y - y.x) < fabs(x.y - x.x) && fabs(y.y - y.x) < fabs(z.y - z.x))
	is_wall = false;

	if (is_wall)
	{
		if (z.x == z.y)
			tw2 = AutoSize(x.x, x.y, true, tw, force_enable, force_mode);
		if (x.x == x.y)
			tw2 = AutoSize(z.x, z.y, true, tw, force_enable, force_mode);
		if ((z.x != z.y) && (x.x != x.y))
		{
			//calculate diagonals
			float tempw1, tempw2;
			if (x.x > x.y)
				tempw1 = x.x - x.y;
			else
				tempw1 = x.y - x.x;
			if (z.x > z.y)
				tempw2 = z.x - z.y;
			else
				tempw2 = z.y - z.x;
			tw2 = AutoSize(0, sqrt(pow(tempw1, 2) + pow(tempw2, 2)), true, tw, force_enable, force_mode);
		}
		th2 = AutoSize(y.x, y.y, false, th, force_enable, force_mode);
	}
	else
	{
		tw2 = sbs->AutoSize(x.x, x.y, true, tw, force_enable, force_mode);
		th2 = sbs->AutoSize(z.x, z.y, false, th, force_enable, force_mode);
	}

	//return results
	return Ogre::Vector2(tw2, th2);
}

/*WallObject* SBS::GetWallObject(std::vector<WallObject*> &wallarray, int polygon_index)
{
	//returns the wall object that contains the specified polygon index
	for (int i = 0; i < wallarray.size(); i++)
	{
		for (int j = 0; j < wallarray[i]->handles.size(); j++)
		{
			if (wallarray[i]->handles[j] == polygon_index)
				return wallarray[i];
		}
	}
	return 0;
}*/

/*std::string SBS::TruncateNumber(double value, int decimals)
{
	//truncates the numeric value to the specified number of decimal places (does not round)

	return Ogre::StringConverter::toString(value, decimals);
}*/

std::string SBS::TruncateNumber(float value, int decimals)
{
	//truncates the numeric value to the specified number of decimal places (does not round)
	
	return Ogre::StringConverter::toString(value, decimals);
}

std::string SBS::TruncateNumber(const char *value, int decimals)
{
	//truncates the numeric value to the specified number of decimal places (does not round)
	std::string number = value;
	
	if (decimals < 1)
		return number;
	number.erase(number.find(".") + decimals + 1);
	if (number.at(number.length() - 1) == '.')
		number = number.substr(0, number.length() - 1); //strip of extra decimal point if even
	return number;
}

bool SBS::IsValidFloor(int floor)
{
	//determine if a floor is valid

	if (GetFloor(floor))
		return true;
	return false;
}

std::string SBS::DumpState()
{
	//dump basic simulator state to a character array

	std::string output = "SBS version: " + version + "\n";
	output.append("Building Name: " + BuildingName + "\n");
	output.append("Building Filename: " + BuildingFilename + "\n");
	output.append("Building Version: " + BuildingVersion + "\n");
	output.append("InStairwell: ");
	output.append(wxString::FromAscii(BoolToString(InStairwell)).ToAscii());
	output.append("\n");
	output.append("InElevator: ");
	output.append(wxString::FromAscii(BoolToString(InElevator)).ToAscii());
	output.append("\n");
	output.append("InShaft: ");
	output.append(wxString::FromAscii(BoolToString(InShaft)).ToAscii());
	output.append("\n");
	output.append("CameraFloor: ");
	if (camera)
		output.append(wxVariant((int)camera->CurrentFloor).GetString().ToAscii());
	output.append("\n");
	output.append("ElevatorNumber: ");
	output.append(wxVariant((int)ElevatorNumber).GetString().ToAscii());
	output.append("\n");
	output.append("ElevatorSync: ");
	output.append(wxString::FromAscii(BoolToString(ElevatorSync)).ToAscii());
	output.append("\n");
	output.append("Running Time: ");
	output.append(TruncateNumber(running_time, 2));
	output.append("\n");
	output.append("BuildingsEnabled: ");
	output.append(wxString::FromAscii(BoolToString(IsBuildingsEnabled)).ToAscii());
	output.append("\n");
	output.append("ExternalEnabled: ");
	output.append(wxString::FromAscii(BoolToString(IsExternalEnabled)).ToAscii());
	output.append("\n");
	output.append("LandscapeEnabled: ");
	output.append(wxString::FromAscii(BoolToString(IsLandscapeEnabled)).ToAscii());
	output.append("\n");
	output.append("SkyboxEnabled: ");
	output.append(wxString::FromAscii(BoolToString(IsSkyboxEnabled)).ToAscii());
	output.append("\n");
	output.append("Verbose: ");
	output.append(wxString::FromAscii(BoolToString(Verbose)).ToAscii());
	output.append("\n");
	output.append("InterfloorOnTop: ");
	output.append(wxString::FromAscii(BoolToString(InterfloorOnTop)).ToAscii());
	output.append("\n");
	output.append("Object Count: ");
	output.append(wxVariant((int)ObjectCount).GetString().ToAscii());
	output.append("\n");
	if (camera)
	{
		output.append("Camera Floor: ");
		output.append(wxVariant((int)camera->CurrentFloor).GetString().ToAscii());
		output.append("\n");
		output.append("Camera Position: " + TruncateNumber(camera->GetPosition().x, 2) + ", " + TruncateNumber(camera->GetPosition().y, 2) + ", " + TruncateNumber(camera->GetPosition().z, 2) + "\n");
	}

	return output;
}

bool SBS::DeleteObject(Object *object)
{
	//object deletion routine
	//this should be called to delete a simulator object during runtime

	std::string number = Ogre::StringConverter::toString(object->GetNumber());
	bool deleted = false;

	if (!object)
	{
		sbs->Report("Invalid object " + number);
		return false;
	}

	if (!object->GetRawObject())
	{
		sbs->Report("Invalid raw object " + number);
		return false;
	}

	//don't delete permanent objects
	if (object->IsPermanent() == true)
	{
		sbs->Report("Cannot delete permanent object " + number);
		return false;
	}

	std::string type = object->GetType();

	//perform standard delete based on object type
	if (type == "Floor")
	{
		delete (Floor*)object->GetRawObject();
		deleted = true;
	}
	if (type == "Elevator")
	{
		delete (Elevator*)object->GetRawObject();
		deleted = true;
	}
	if (type == "ButtonPanel")
	{
		delete (ButtonPanel*)object->GetRawObject();
		deleted = true;
	}
	if (type == "CallButton")
	{
		delete (CallButton*)object->GetRawObject();
		deleted = true;
	}
	if (type == "DirectionalIndicator")
	{
		delete (DirectionalIndicator*)object->GetRawObject();
		deleted = true;
	}
	if (type == "Door")
	{
		delete (Door*)object->GetRawObject();
		deleted = true;
	}
	if (type == "ElevatorDoor")
	{
		delete (ElevatorDoor*)object->GetRawObject();
		deleted = true;
	}
	if (type == "FloorIndicator")
	{
		delete (FloorIndicator*)object->GetRawObject();
		deleted = true;
	}
	if (type == "Shaft")
	{
		delete (Shaft*)object->GetRawObject();
		deleted = true;
	}
	if (type == "Sound")
	{
		delete (Sound*)object->GetRawObject();
		deleted = true;
	}
	if (type == "Stairs")
	{
		delete (Stairs*)object->GetRawObject();
		deleted = true;
	}
	if (type == "Wall")
	{
		WallObject *obj = (WallObject*)object->GetRawObject();
		obj->DeletePolygons();
		delete obj;
		deleted = true;
	}

	if (deleted == true)
	{
		//engine->Prepare();
		sbs->Report("Deleted object " + number);
		return true;
	}
	return false;
}

bool SBS::DeleteObject(int object)
{
	//delete object by numeric ID
	return DeleteObject(GetObject(object));
}

void SBS::RemoveFloor(Floor *floor)
{
	//remove a floor (does not delete the object)
	for (int i = 0; i < (int)FloorArray.size(); i++)
	{
		if (FloorArray[i].object == floor)
		{
			FloorArray.erase(FloorArray.begin() + i);
			return;
		}
	}
}

void SBS::RemoveElevator(Elevator *elevator)
{
	//remove an elevator (does not delete the object)
	for (int i = 0; i < (int)ElevatorArray.size(); i++)
	{
		if (ElevatorArray[i].object == elevator)
		{
			ElevatorArray.erase(ElevatorArray.begin() + i);
			return;
		}
	}
}

void SBS::RemoveShaft(Shaft *shaft)
{
	//remove a shaft (does not delete the object)
	for (int i = 0; i < (int)ShaftArray.size(); i++)
	{
		if (ShaftArray[i].object == shaft)
		{
			ShaftArray.erase(ShaftArray.begin() + i);
			return;
		}
	}
}

void SBS::RemoveStairs(Stairs *stairs)
{
	//remove a stairs object (does not delete the object)
	for (int i = 0; i < (int)StairsArray.size(); i++)
	{
		if (StairsArray[i].object == stairs)
		{
			StairsArray.erase(StairsArray.begin() + i);
			return;
		}
	}
}

void SBS::RemoveSound(Sound *sound)
{
	//remove a sound from the array
	//this does not delete the object

	for (int i = 0; i < (int)sounds.size(); i++)
	{
		if (sounds[i] == sound)
		{
			sounds.erase(sounds.begin() + i);
			return;
		}
	}
}

std::string SBS::VerifyFile(const char *filename)
{
	//verify a filename
	//if it exists, return the same filename
	//otherwise search the related folder and find a matching filename with a different
	//case (fixes case-sensitivity issues mainly on Linux)
	//returns the original string if not found

	std::string file = filename;
	TrimString(file);

	//check for a cached result
	for (int i = 0; i < verify_results.size(); i++)
	{
		if (verify_results[i].filename == file)
			return verify_results[i].result;
	}

	Ogre::FileSystemArchive filesystem(".","FileSystem");
	if (filesystem.exists(file))
	{
		CacheFilename(file, file);
		return file;
	}

	/*std::string directory;
	int loc1 = file.find_last_of("/");
	int loc2 = file.find_last_of("\\");
	int loc = loc1;
	if (loc2 > 0)
		loc = loc2;

	directory = file.substr(0, loc + 1);*/

	Ogre::StringVectorPtr listing = filesystem.list();
	for (int i = 0; i < listing->size(); i++)
	{
		std::string check = listing->at(i);
		std::string checkoriginal = SetCaseCopy(check, false);
		std::string checkfile = SetCaseCopy(file, false);
		if (checkoriginal == checkfile)
		{
			CacheFilename(file, check);
			return check;
		}
	}

	CacheFilename(file, file);
	return file;
}

bool SBS::FileExists(const char *filename)
{
	//check to see if the specified file exists

	std::string file = filename;
	TrimString(file);

	Ogre::FileSystemArchive filesystem(".","FileSystem");
	if (filesystem.exists(file))
		return true;

	std::string verify = VerifyFile(file.c_str());
	if (verify != file)
		return true;
	return false;
}

int SBS::GetWallCount()
{
	//return total number of registered walls
	return WallCount;
}

int SBS::GetPolygonCount()
{
	//return total number of registered walls
	return PolygonCount;
}

void SBS::AddLightHandle(Light* handle)
{
	all_lights.push_back(handle);
}

void SBS::DeleteLightHandle(Light* handle)
{
	for (int i = 0; i < (int)all_lights.size(); i++)
	{
		if (all_lights[i] == handle)
			all_lights.erase(all_lights.begin() + i);
	}
}

void SBS::AddModelHandle(Model* handle)
{
	all_models.push_back(handle);
}

void SBS::DeleteModelHandle(Model* handle)
{
	for (int i = 0; i < (int)all_models.size(); i++)
	{
		if (all_models[i] == handle)
			all_models.erase(all_models.begin() + i);
	}
}

void SBS::Prepare()
{
	//prepare objects for run
	
	Report("Preparing objects...");
	Report("Creating colliders...");
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->CreateCollider();
	}
	Report("Processing geometry...");
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->Prepare();
	}
	Report("Finished prepare");
}

Object* SBS::AddLight(const char *name, int type, Ogre::Vector3 position, Ogre::Vector3 direction, float color_r, float color_g, float color_b, float spec_color_r, float spec_color_g, float spec_color_b, float spot_inner_angle, float spot_outer_angle, float spot_falloff, float att_range, float att_constant, float att_linear, float att_quadratic)
{
	//add a global light

	Light* light = new Light(name, type, position, direction, color_r, color_g, color_b, spec_color_r, spec_color_g, spec_color_b, spot_inner_angle, spot_outer_angle, spot_falloff, att_range, att_constant, att_linear, att_quadratic);
	lights.push_back(light);
	return light->object;
}

void SBS::AddMeshHandle(MeshObject* handle)
{
	meshes.push_back(handle);
}

void SBS::DeleteMeshHandle(MeshObject* handle)
{
	for (int i = 0; i < (int)meshes.size(); i++)
	{
		if (meshes[i] == handle)
			meshes.erase(meshes.begin() + i);
	}
}

MeshObject* SBS::FindMeshObject(Ogre::MeshPtr meshwrapper)
{
	//find a mesh object by searching for matching wrapper
	for (int i = 0; i < (int)meshes.size(); i++)
	{
		if (meshes[i]->MeshWrapper == meshwrapper)
			return meshes[i];
	}
	return 0;
}

MeshObject* SBS::FindMeshObject(std::string name)
{
	//find a mesh object by searching for matching wrapper
	for (int i = 0; i < (int)meshes.size(); i++)
	{
		if (meshes[i]->name == name)
			return meshes[i];
	}
	return 0;
}

Object* SBS::AddModel(const char *name, const char *filename, Ogre::Vector3 position, Ogre::Vector3 rotation, float max_render_distance, float scale_multiplier, bool enable_physics, float restitution, float friction, float mass)
{
	//add a model
	Model* model = new Model(name, filename, position, rotation, max_render_distance, scale_multiplier, enable_physics, restitution, friction, mass);
	if (model->load_error == true)
	{
		delete model;
		return 0;
	}
	ModelArray.push_back(model);
	return model->object;
}

int SBS::GetConfigInt(std::string key, int default_value)
{
	std::string result = configfile.getSetting(key, Ogre::StringUtil::BLANK, Ogre::StringConverter::toString(default_value));
	return Ogre::StringConverter::parseInt(result);
}

std::string SBS::GetConfigString(std::string key, std::string default_value)
{
	return configfile.getSetting(key, Ogre::StringUtil::BLANK, default_value);
}

bool SBS::GetConfigBool(std::string key, bool default_value)
{
	std::string result = configfile.getSetting(key, Ogre::StringUtil::BLANK, Ogre::StringConverter::toString(default_value));
	return Ogre::StringConverter::parseBool(result);
}

float SBS::GetConfigFloat(std::string key, float default_value)
{
	std::string result = configfile.getSetting(key, Ogre::StringUtil::BLANK, Ogre::StringConverter::toString(default_value));
	return Ogre::StringConverter::parseReal(result);
}

bool SBS::InBox(Ogre::Vector3 &start, Ogre::Vector3 &end, Ogre::Vector3 &test)
{
	//determine if a point (test) is inside the box defines by start and end vertices

	if (test.x > start.x && test.y > start.y && test.z > start.z && test.x < end.x && test.y < end.y && test.z < end.z)
		return true;
	return false;
}

void SBS::AdvanceClock()
{
	//advance the clock

	unsigned long last = current_time;

	//get current time
	current_time = GetCurrentTime();
	if (last == 0)
		last = current_time;

	if (current_time < last)
		elapsed_time = current_time + ((unsigned long)-1 - last) + 1;
	else
		elapsed_time = current_time - last;
	current_virtual_time += elapsed_time;
	frame_times.push_back(current_time);
	CalculateElapsedTime();
}

unsigned long SBS::GetCurrentTime()
{
	//get current time
	return timer->getMilliseconds();
}

unsigned long SBS::GetRunTime()
{
	//returns simulator run time
	return current_virtual_time;
}

unsigned long SBS::GetElapsedTime()
{
	//returns the actual elapsed time between frames
	return elapsed_time;
}

unsigned long SBS::GetElapsedTimeAverage()
{
	//returns the average elapsed time between frames
	return average_time;
}

void SBS::CalculateElapsedTime()
{
	//calculates the average frame processing time for a specified number of frames

	if (frame_times.size() <= 1)
		return;

	//maximum number of milliseconds to hold timing info
	unsigned int smoothing_limit = 1000;
	
	//find oldest time to keep
	std::deque<unsigned long>::iterator it = frame_times.begin(), end = frame_times.end() - 2;

	while (it != end)
	{
		if (frame_times.back() - *it > smoothing_limit)
			++it;
		else
			break;
	}

	//remove old times
	frame_times.erase(frame_times.begin(), it);

	//calculate average time
	average_time = (frame_times.back() - frame_times.front()) / (frame_times.size() - 1);
}

std::string SBS::GetMountPath(const char *filename, std::string &newfilename)
{
	//get mountpoint (resource group) path of given file
	//if not found, return "General"
	
	std::string file = filename;
	Ogre::StringVector list = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
	newfilename = filename;

	for (int i = 0; i < list.size(); i++)
	{
		std::string check = file + "/";
		if (StartsWith(check, list[i].c_str()) == true)
		{
			newfilename = file.substr(list[i].size() + 1);
			return list[i];
		}
		check = file + "\\";
		if (StartsWith(check, list[i].c_str()) == true)
		{
			newfilename = file.substr(list[i].size() + 1);
			return list[i];
		}
	}
	return "General";
}

void SBS::EnableVSync(bool value)
{
	//enable or disable vertical sync
	mRoot->getRenderSystem()->setWaitForVerticalBlank(value);
}

void SBS::ShowColliders(bool value)
{
	if (mWorld)
		mWorld->setShowDebugShapes(value);
	camera->ShowDebugShape(value);
}

void SBS::CacheFilename(std::string filename, std::string result)
{
	//caches filename information for VerifyFile function
	VerifyResult verify;
	verify.filename = filename;
	verify.result = result;
	verify_results.push_back(verify);
}

void SBS::SetLighting(float red, float green, float blue)
{
	OldAmbientR = AmbientR;
	OldAmbientG = AmbientG;
	OldAmbientB = AmbientB;
	AmbientR = red;
	AmbientG = green;
	AmbientB = blue;
}

void SBS::ResetLighting()
{
	AmbientR = OldAmbientR;
	AmbientG = OldAmbientG;
	AmbientB = OldAmbientB;
}
