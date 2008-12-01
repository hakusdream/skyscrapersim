/* $Id$ */

/*
	Scalable Building Simulator - Core
	The Skyscraper Project - Version 1.1 Alpha
	Copyright (C)2005-2008 Ryan Thoryk
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
#include "globals.h"
#include "sbs.h"
#include "unix.h"

SBS *sbs; //self reference

iObjectRegistry* object_reg;

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
	Gravity = 9.806f; // 9.806 m/s/s
	IsRunning = false;
	Floors = 0;
	Basements = 0;
	RenderOnly = false;
	InputOnly = false;
	IsFalling = false;
	InStairwell = false;
	InElevator = false;
	EnableCollisions = true;
	IsBuildingsEnabled = false;
	IsColumnFrameEnabled = false;
	IsExternalEnabled = false;
	IsLandscapeEnabled = false;
	IsSkyboxEnabled = false;
	fps_frame_count = 0;
	fps_tottime = 0;
	FPS = 0;
	AutoShafts = true;
	AutoStairs = true;
	ElevatorSync = false;
	mouse_x = 0;
	mouse_y = 0;
	MouseDown = false;
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
	canvas_width = 0;
	canvas_height = 0;
	remaining_delta = 0;
	delta = 0.01f;
	ShowFullShafts = false;
	ShowFullStairs = false;
	ShaftDisplayRange = 3;
	StairsDisplayRange = 3;
	ShaftOutsideDisplayRange = 3;
	StairsOutsideDisplayRange = 3;
	wall1a = false;
	wall1b = false;
	wall2a = false;
	wall2b = false;
	AutoX = true;
	AutoY = true;
}

SBS::~SBS()
{
	//engine destructor

	//stop and delete timer
	p->Stop();
	p->s = 0;
	delete p;
	p = 0;

	//delete camera object
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

	//delete wx canvas
	delete canvas;
	canvas = 0;
}

void SBS::Start()
{
	//set running value
	IsRunning = true;

	//create skybox
	CreateSky();

	//Post-init startup code goes here, before the runloop
	engine->Prepare();

	//initialize mesh colliders
	csColliderHelper::InitializeCollisionWrappers (collision_sys, engine);

	//move camera to start location
	camera->SetToStartPosition();
	camera->SetToStartDirection();
	camera->SetToStartRotation();

	//turn on main objects
	EnableBuildings(true);
	EnableLandscape(true);
	EnableExternal(true);
	EnableColumnFrame(true);
	EnableSkybox(true);

	//turn off floors
	for (int i = 0; i < TotalFloors(); i++)
		FloorArray[i].object->Enabled(false);

	//turn off shafts
	for (int i = 0; i < Shafts(); i++)
	{
		if (ShaftArray[i].object)
			ShaftArray[i].object->EnableWholeShaft(false, true);
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
			ElevatorArray[i].object->ShaftDoorsEnabled(camera->StartFloor, true);
	}

	//turn on start floor
	GetFloor(camera->StartFloor)->Enabled(true);
	GetFloor(camera->StartFloor)->EnableGroup(true);

}

void SBS::Run()
{
	//start runloop

	Report("Running simulation...");

	//start simulation with a timer-based runloop
	p = new Pump();
	p->s = sbs;
	if (FrameLimiter == true)
		p->Start(1000 / FrameRate);
	else
		p->Start(1);
}

void SBS::Wait(long milliseconds)
{

}

float SBS::AutoSize(float n1, float n2, bool iswidth, bool external, float offset)
{
	//Texture autosizing formulas

	float size1 = offset;
	float size2 = offset;

	if (offset == 0)
		offset = 1;

	if (external == false)
	{
		if (AutoX == true)
			size1 = 0.269 * offset;
		if (AutoY == true)
			size2 = 0.25 * offset;
	}
	else
	{
		if (AutoX == true)
			size1 = 0.072 * offset;
		if (AutoY == true)
			size2 = offset;
	}

	if (iswidth == true && AutoX == true)
		return fabs(n1 - n2) * size1;
	else
	{
		if (external == false && AutoY == true)
			return fabs(n1 - n2) * size2;
		else
			return size2;
	}
	return offset;
}

void SBS::PrintBanner()
{
	csPrintf("\n Scalable Building Simulator 0.1 Alpha\n");
	csPrintf(" Copyright (C)2004-2008 Ryan Thoryk\n");
	csPrintf(" This software comes with ABSOLUTELY NO WARRANTY. This is free\n");
	csPrintf(" software, and you are welcome to redistribute it under certain\n");
	csPrintf(" conditions. For details, see the file gpl.txt\n");
}

void SBS::GetInput()
{
	// First get elapsed time from the virtual clock.
	elapsed_time = vc->GetElapsedTicks ();
	current_time = vc->GetCurrentTicks ();

	// Now rotate the camera according to keyboard state
	float speed = (elapsed_time / 1000.0) * (0.06 * 20);

	//get mouse pointer coordinates
	mouse_x = mouse->GetLastX();
	mouse_y = mouse->GetLastY();

	//check if the user clicked on an object, and process it
	if (mouse->GetLastButton(0) == true && MouseDown == false)
	{
		MouseDown = true;
		camera->ClickedObject();
	}

	//reset mouse state
	if (mouse->GetLastButton(0) == false)
		MouseDown = false;

	//if (wxGetKeyState(WXK_ESCAPE))
		//p->Stop();

	if (wxGetKeyState(WXK_F2))
		Report(wxVariant(FPS).GetString().ToAscii());

	if (wxGetKeyState(WXK_CONTROL))
		speed *= 4;

	if (wxGetKeyState(WXK_SHIFT))
	{
		// If the user is holding down shift, the arrow keys will cause
		// the camera to strafe up, down, left or right from it's
		// current position.
		if (wxGetKeyState(WXK_RIGHT))
			camera->Move (CS_VEC_RIGHT, speed * 8);
		if (wxGetKeyState(WXK_LEFT))
			camera->Move (CS_VEC_LEFT, speed * 8);
		if (wxGetKeyState(WXK_UP))
			camera->Move (CS_VEC_UP, speed * 8);
		if (wxGetKeyState(WXK_DOWN))
			camera->Move (CS_VEC_DOWN, speed * 8);
	}
	else if (wxGetKeyState(WXK_ALT))
	{
		//rotate on the Z axis if the Alt key is pressed with the left/right arrows
		if (wxGetKeyState(WXK_RIGHT))
			camera->Rotate(CS_VEC_FORWARD, speed);
		if (wxGetKeyState(WXK_LEFT))
			camera->Rotate(CS_VEC_BACKWARD, speed);
	}
	else
	{
		// left and right cause the camera to rotate on the global Y
		// axis; page up and page down cause the camera to rotate on the
		// _camera's_ X axis (more on this in a second) and up and down
		// arrows cause the camera to go forwards and backwards.
		if (wxGetKeyState(WXK_RIGHT))
			camera->Rotate(CS_VEC_UP, speed);
		if (wxGetKeyState(WXK_LEFT))
			camera->Rotate(CS_VEC_DOWN, speed);
		if (wxGetKeyState(WXK_PRIOR)) //page up
			camera->Rotate(CS_VEC_RIGHT, speed);
		if (wxGetKeyState(WXK_NEXT)) //page down
			camera->Rotate(CS_VEC_LEFT, speed);
		if (wxGetKeyState(WXK_UP))
		{
			float KeepAltitude;
			KeepAltitude = camera->GetPosition().y;
			camera->Move (CS_VEC_FORWARD, speed * 8);
			if (camera->GetPosition().y != KeepAltitude)
				camera->SetPosition(csVector3(camera->GetPosition().x, KeepAltitude, camera->GetPosition().z));
		}
		if (wxGetKeyState(WXK_DOWN))
		{
			float KeepAltitude;
			KeepAltitude = camera->GetPosition().y;
			camera->Move (CS_VEC_BACKWARD, speed * 8);
			if (camera->GetPosition().y != KeepAltitude)
				camera->SetPosition(csVector3(camera->GetPosition().x, KeepAltitude, camera->GetPosition().z));
		}

		if (wxGetKeyState(WXK_SPACE))
		{
			//reset view
			camera->SetToStartDirection();
			camera->SetToStartRotation();
		}

		//values from old version
		if (wxGetKeyState(WXK_HOME))
			camera->Move (CS_VEC_UP, speed * 8);
		if (wxGetKeyState(WXK_END))
			camera->Move (CS_VEC_DOWN, speed * 8);
	}
}

void SBS::Render()
{
	// Tell 3D driver we're going to display 3D things.
	if (!g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS | CSDRAW_CLEARZBUFFER | CSDRAW_CLEARSCREEN ))
		return;

	// Tell the camera to render into the frame buffer.
	view->Draw ();
}

void SBS::SetupFrame()
{
	//Main simulator loop

	//resize canvas if needed
	if (canvas->GetSize().GetWidth() != canvas_width || canvas->GetSize().GetHeight() != canvas_height)
	{
		//update canvas size values
		canvas_width = canvas->GetSize().GetWidth();
		canvas_height = canvas->GetSize().GetHeight();

		//resize viewport
		view->SetAutoResize(false);
		float oldfov = view->GetCamera()->GetFOVAngle();
		wxwin->GetWindow()->SetSize(canvas->GetSize());
		view->GetCamera()->SetFOVAngle(oldfov, canvas_width);
		view->GetCamera()->SetPerspectiveCenter(canvas_width / 2, canvas_height / 2);
		view->SetRectangle(0, 0, canvas_width, canvas_height);
		g3d->SetDimensions(canvas_width, canvas_height);
		view->ClearView();
	}

	//This makes sure all timer steps are the same size, in order to prevent the physics from changing
	//depending on frame rate
	float elapsed = remaining_delta + (vc->GetElapsedTicks() / 1000.0);
	while (elapsed >= delta)
	{
		if (RenderOnly == false && InputOnly == false)
		{
			//Process gravity
			if (EnableCollisions == true)
				camera->Gravity();

			//Determine floor that the camera is on
			camera->UpdateCameraFloor();

			//run elevator handlers
			for (int i = 1; i <= Elevators(); i++)
				GetElevator(i)->MonitorLoop();

			//check if the user is in an elevator
			camera->CheckElevator();

			//check if the user is in a shaft
			if (AutoShafts == true)
				camera->CheckShaft();

			//check if the user is in a stairwell
			if (AutoStairs == true)
				camera->CheckStairwell();

			//check if the user is outside

		}

		elapsed -= delta;
	}
	remaining_delta = elapsed;

	if (RenderOnly == false)
		GetInput();

	Render();
}

void SBS::FinishFrame()
{
	g3d->FinishDraw();
	g3d->Print(0);
}


bool SBS::HandleEvent(iEvent& Event)
{
	//Event handler
	if (Event.Name == Process)
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

		SetupFrame ();
		return true;
	}
	else if (Event.Name == FinalProcess)
	{
		FinishFrame ();
		return true;
	}
	return false;
}

static bool SBSEventHandler(iEvent& Event)
{
	if (sbs)
		return sbs->HandleEvent (Event);
	else
		return false;
}

bool SBS::Initialize(int argc, const char* const argv[], wxPanel* RenderObject)
{
	object_reg = csInitializer::CreateEnvironment (argc, argv);
	if (!object_reg) return false;

	if (!csInitializer::RequestPlugins (object_reg,
		CS_REQUEST_VFS,
		CS_REQUEST_PLUGIN("crystalspace.graphics2d.wxgl", iGraphics2D),
		CS_REQUEST_OPENGL3D,
		CS_REQUEST_ENGINE,
		CS_REQUEST_FONTSERVER,
		CS_REQUEST_IMAGELOADER,
		CS_REQUEST_LEVELLOADER,
		CS_REQUEST_CONSOLEOUT,
		CS_REQUEST_REPORTER,
		CS_REQUEST_REPORTERLISTENER,
		CS_REQUEST_PLUGIN("crystalspace.collisiondetection.opcode", iCollideSystem),
		CS_REQUEST_END))
		return ReportError ("Couldn't init app!");

	FocusGained = csevFocusGained (object_reg);
	FocusLost = csevFocusLost (object_reg);
	Process = csevProcess (object_reg);
	FinalProcess = csevFinalProcess (object_reg);
	KeyboardDown = csevKeyboardDown (object_reg);

	if (!csInitializer::SetupEventHandler (object_reg, SBSEventHandler))
		return ReportError ("Couldn't initialize event handler!");

	// Check for commandline help.
	if (csCommandLineHelper::CheckHelp (object_reg))
	{
		csCommandLineHelper::Help (object_reg);
		return false;
	}

	csRef<iPluginManager> plugin_mgr = csQueryRegistry<iPluginManager> (object_reg);

	vc = csQueryRegistry<iVirtualClock> (object_reg);
	if (!vc) return ReportError("Failed to locate Virtual Clock");
	kbd = csQueryRegistry<iKeyboardDriver> (object_reg);
	if (!kbd) return ReportError ("Failed to locate Keyboard Driver");
	engine = csQueryRegistry<iEngine> (object_reg);
	if (!engine) return ReportError ("Failed to locate 3D engine");
	loader = csQueryRegistry<iLoader> (object_reg);
	if (!loader) return ReportError ("Failed to locate Loader");
	g3d = csQueryRegistry<iGraphics3D> (object_reg);
	if (!g3d) return ReportError ("Failed to locate 3D renderer");
	imageio = csQueryRegistry<iImageIO> (object_reg);
	if (!imageio) return ReportError ("Failed to locate image loader");
	vfs = csQueryRegistry<iVFS> (object_reg);
	if (!vfs) return ReportError ("Failed to locate VFS");
	console = csQueryRegistry<iConsoleOutput> (object_reg);
	if (!console) return ReportError ("Failed to locate console output driver");
	mouse = csQueryRegistry<iMouseDriver> (object_reg);
	if (!mouse) return ReportError("Failed to locate mouse driver");
	collision_sys = csQueryRegistry<iCollideSystem> (object_reg);
	if (!collision_sys) return ReportError("Failed to locate collision detection driver");
	plug = csLoadPluginAlways (plugin_mgr, "crystalspace.utilities.bugplug");
	if (!plug) return ReportError ("Failed to locate BugPlug!");
	if (plug) plug->IncRef ();
	rep = csQueryRegistry<iReporter> (object_reg);

	stdrep = csQueryRegistry<iStandardReporterListener> (object_reg);
	if (!stdrep) return ReportError ("Failed to locate stdrep plugin!");
	stdrep->SetDebugFile ("/tmp/sbs_report.txt");
	stdrep->SetMessageDestination (CS_REPORTER_SEVERITY_BUG, true, false, false, false, true, false);
	stdrep->SetMessageDestination (CS_REPORTER_SEVERITY_ERROR, true, false, false, false, true, false);
	stdrep->SetMessageDestination (CS_REPORTER_SEVERITY_WARNING, true, false, false, false, true, false);
	stdrep->SetMessageDestination (CS_REPORTER_SEVERITY_NOTIFY, true, false, false, false, true, false);
	stdrep->SetMessageDestination (CS_REPORTER_SEVERITY_DEBUG, true, false, false, false, true, false);

	//mount app's directory in VFS
	#ifndef CS_PLATFORM_WIN32
		vfs->Mount("/root/", csInstallationPathsHelper::GetAppDir(argv[0]) + "/");
	#else
		vfs->Mount("/root/", csInstallationPathsHelper::GetAppDir(argv[0]) + "\\");
	#endif

	g2d = g3d->GetDriver2D();
	g2d->AllowResize(true); //allow canvas resizing
	wxwin = scfQueryInterface<iWxWindow> (g2d);
	if(!wxwin)
	{
		ReportError("Canvas is no iWxWindow plugin!");
		return false;
	}
	wxwin->SetParent(RenderObject);
	canvas = RenderObject;
	canvas_width = canvas->GetSize().GetWidth();
	canvas_height = canvas->GetSize().GetHeight();

	//font = g2d->GetFontServer()->LoadFont(CSFONT_LARGE);

	// Open the main system. This will open all the previously loaded plug-ins.
	if (!csInitializer::OpenApplication (object_reg))
		return ReportError ("Error opening system!");

	equeue = csQueryRegistry<iEventQueue> (object_reg);

	// First disable the lighting cache. Our app is simple enough
	// not to need this.
	engine->SetLightingCacheMode (0);
	engine->SetAmbientLight(csColor(0.5, 0.5, 0.5));

	//create 3D environments
	area = engine->CreateSector("area");

	//load default textures
	csPrintf("Loading default textures...");
	LoadTexture("/root/data/top.jpg", "SkyTop");
	LoadTexture("/root/data/bottom.jpg", "SkyBottom");
	LoadTexture("/root/data/left.jpg", "SkyLeft");
	LoadTexture("/root/data/right.jpg", "SkyRight");
	LoadTexture("/root/data/front.jpg", "SkyFront");
	LoadTexture("/root/data/back.jpg", "SkyBack");
	LoadTexture("/root/data/brick1.jpg", "Default");
	csPrintf("Done\n");

	//set up viewport
	view = csPtr<iView>(new csView (engine, g3d));
	view->SetRectangle(0, 0, g2d->GetWidth(), g2d->GetHeight());

	//create camera object
	camera = new Camera();

	//clear user variables
	UserVariable.SetSize(256);

	return true;
}

bool SBS::LoadTexture(const char *filename, const char *name)
{
	// Load the texture from the standard library.  This is located in
	// CS/data/standard.zip and mounted as /lib/std using the Virtual
	// File System (VFS) plugin.
	if (!loader->LoadTexture (name, filename))
	{
		ReportError("Error loading texture");
		return false;
	}
	return true;
}

void SBS::AddLight(const char *name, float x, float y, float z, float radius, float r, float g, float b)
{
	ll = area->GetLights();
	light = engine->CreateLight(name, csVector3(x, y, z), radius, csColor(r, g, b));
	ll->Add(light);
}

void Cleanup()
{
	//cleanup
	csPrintf ("Cleaning up...\n");
	sbs = 0;
	csInitializer::DestroyApplication (object_reg);
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
	SetTexture(dest, index, texture, tw, th);

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

	csVector3 v1 (x1, altitude1, z1); //bottom left
	csVector3 v2 (x2, altitude1, z1); //bottom right
	csVector3 v3 (x2, altitude2, z2); //top right
	csVector3 v4 (x1, altitude2, z2); //top left

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
	SetTexture(dest, index, texture, tw, th);

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

int SBS::CreateWallBox(csRef<iThingFactoryState> dest, const char *name, const char *texture, float x1, float x2, float z1, float z2, float height_in, float voffset, float tw, float th)
{
	//create 4 walls

	iMaterialWrapper* tm;

	int firstidx = dest->AddInsideBox(csVector3(x1 * HorizScale, voffset, z1 * HorizScale), csVector3(x2 * HorizScale, voffset + height_in, z2 * HorizScale));
	tm = engine->GetMaterialList ()->FindByName (texture);
	dest->SetPolygonMaterial (CS_POLYRANGE_LAST, tm);
	dest->SetPolygonTextureMapping (CS_POLYRANGE_LAST, 3); //see todo below

	dest->AddOutsideBox(csVector3(x1 * HorizScale, voffset, z1 * HorizScale), csVector3(x2 * HorizScale, voffset + height_in, z2 * HorizScale));
	tm = engine->GetMaterialList ()->FindByName (texture);
	dest->SetPolygonMaterial (CS_POLYRANGE_LAST, tm);
	dest->SetPolygonTextureMapping (CS_POLYRANGE_LAST, 3); //see todo below

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

int SBS::CreateWallBox2(csRef<iThingFactoryState> dest, const char *name, const char *texture, float CenterX, float CenterZ, float WidthX, float LengthZ, float height_in, float voffset, float tw, float th)
{
	//create 4 walls from a central point

	iMaterialWrapper* tm;
	float x1;
	float x2;
	float z1;
	float z2;

	x1 = CenterX - (WidthX / 2);
	x2 = CenterX + (WidthX / 2);
	z1 = CenterZ - (LengthZ / 2);
	z2 = CenterZ + (LengthZ / 2);

	int firstidx = dest->AddInsideBox(csVector3(x1 * HorizScale, voffset, z1 * HorizScale), csVector3(x2 * HorizScale, voffset + height_in, z2 * HorizScale));
	tm = engine->GetMaterialList ()->FindByName (texture);
	dest->SetPolygonMaterial (CS_POLYRANGE_LAST, tm);
	dest->SetPolygonTextureMapping (CS_POLYRANGE_LAST, 3); //see todo below

	dest->AddOutsideBox(csVector3(x1 * HorizScale, voffset, z1 * HorizScale), csVector3(x2 * HorizScale, voffset + height_in, z2 * HorizScale));
	tm = engine->GetMaterialList ()->FindByName (texture);
	dest->SetPolygonMaterial (CS_POLYRANGE_LAST, tm);
	dest->SetPolygonTextureMapping (CS_POLYRANGE_LAST, 3); //see todo below

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

void SBS::InitMeshes()
{
	//create object meshes
	Buildings = engine->CreateSectorWallsMesh (area, "Buildings");
	Buildings_state = scfQueryInterface<iThingFactoryState> (Buildings->GetMeshObject()->GetFactory());
	Buildings->SetZBufMode(CS_ZBUF_USE);
	Buildings->SetRenderPriority(engine->GetAlphaRenderPriority());
	Buildings->GetMeshObject()->SetMixMode(CS_FX_ALPHA);

	External = engine->CreateSectorWallsMesh (area, "External");
	External_state = scfQueryInterface<iThingFactoryState> (External->GetMeshObject()->GetFactory());
	External->SetZBufMode(CS_ZBUF_USE);
	External->SetRenderPriority(engine->GetAlphaRenderPriority());
	External->GetMeshObject()->SetMixMode(CS_FX_ALPHA);

	Landscape = engine->CreateSectorWallsMesh (area, "Landscape");
	Landscape_state = scfQueryInterface<iThingFactoryState> (Landscape->GetMeshObject()->GetFactory());
	Landscape->SetZBufMode(CS_ZBUF_USE);
	Landscape->SetRenderPriority(engine->GetAlphaRenderPriority());
	Landscape->GetMeshObject()->SetMixMode(CS_FX_ALPHA);

	ColumnFrame = engine->CreateSectorWallsMesh (area, "ColumnFrame");
	ColumnFrame_state = scfQueryInterface<iThingFactoryState> (ColumnFrame->GetMeshObject()->GetFactory());
	ColumnFrame->SetZBufMode(CS_ZBUF_USE);
	ColumnFrame->SetRenderPriority(engine->GetAlphaRenderPriority());
	ColumnFrame->GetMeshObject()->SetMixMode(CS_FX_ALPHA);
}

int SBS::AddCustomWall(csRef<iThingFactoryState> dest, const char *name, const char *texture, csPoly3D &varray, float tw, float th, bool IsExternal)
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
		tw2 = AutoSize(x.x, x.y, true, IsExternal, tw);
	if (x.x == x.y)
		tw2 = AutoSize(z.x, z.y, true, IsExternal, tw);
	if ((z.x != z.y) && (x.x != x.y))
	{
		//calculate diagonals
		tempw1 = fabs(x.y - x.x);
		tempw2 = fabs(z.y - z.x);
		tw2 = AutoSize(0, sqrt(pow(tempw1, 2) + pow(tempw2, 2)), true, IsExternal, tw);
	}
	th2 = AutoSize(0, fabs(y.y - y.x), false, IsExternal, th);

	//create 2 polygons (front and back) from the vertex array
	int firstidx = dest->AddPolygon(varray1.GetVertices(), num);
	dest->AddPolygon(varray2.GetVertices(), num);

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

int SBS::AddCustomFloor(csRef<iThingFactoryState> dest, const char *name, const char *texture, csPoly3D &varray, float tw, float th, bool IsExternal)
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
		tw2 = AutoSize(x.x, x.y, true, IsExternal, tw);
	if (x.x == x.y)
		tw2 = AutoSize(z.x, z.y, true, IsExternal, tw);
	if ((z.x != z.y) && (x.x != x.y))
	{
		//calculate diagonals
		tempw1 = fabs(x.y - x.x);
		tempw2 = fabs(z.y - z.x);
		tw2 = AutoSize(0, sqrt(pow(tempw1, 2) + pow(tempw2, 2)), true, IsExternal, tw);
	}
	th2 = AutoSize(0, fabs(y.y - y.x), false, IsExternal, th);

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

int SBS::AddTriangleWall(csRef<iThingFactoryState> dest, const char *name, const char *texture, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float tw, float th, bool IsExternal)
{
	//Adds a triangular wall with the specified dimensions
	csPoly3D varray;

	//set up temporary vertex array
	varray.AddVertex(x1, y1, z1);
	varray.AddVertex(x2, y2, z2);
	varray.AddVertex(x3, y3, z3);

	//pass data on to AddCustomWall function
	int firstidx = AddCustomWall(dest, name, texture, varray, tw, th, IsExternal);

	return firstidx;
}

csString SBS::Calc(const char *expression)
{
	//performs a calculation operation on a string
	//for example, the string "1 + 1" would output to "2"
	//supports multiple and nested operations (within parenthesis)

	int temp1;
	csString tmpcalc = expression;
	char buffer[20];
	csString one;
	csString two;
	int start, end;

	//first remove all whitespace from the string
	tmpcalc.ReplaceAll(" ", "");

	if (tmpcalc.Find("=", 0) == -1 && tmpcalc.Find("!", 0) == -1 && tmpcalc.Find("<", 0) == -1 && tmpcalc.Find(">", 0) == -1)
	{
		//find parenthesis
		do
		{
			start = tmpcalc.Find("(", 0);
			if (start >= 0)
			{
				//find matching parenthesis
				int match = 1;
				int end = -1;
				for (int i = start + 1; i < tmpcalc.Length(); i++)
				{
					if (tmpcalc.GetAt(i) == '(')
						match++;
					if (tmpcalc.GetAt(i) == ')')
						match--;
					if (match == 0)
					{
						end = i;
						break;
					}
				}
				if (end != -1)
				{
					//call function recursively
					csString newdata;
					newdata = Calc(tmpcalc.Slice(start + 1, end - start - 1));
					//construct new string
					one = tmpcalc.Slice(0, start);
					if (end < tmpcalc.Length() - 1)
						two = tmpcalc.Slice(end + 1);
					else
						two = "";
					tmpcalc = one + newdata + two;
				}
				else
				{
					ReportError("Syntax error in math operation: '" + tmpcalc + "' (might be nested)");
					return "false";
				}
			}
			else
				break;
		} while (1 == 1);

		//find number of operators and recurse if multiple found
		int operators;
		do
		{
			operators = 0;
			end = 0;
			for (int i = 1; i < tmpcalc.Length(); i++)
			{
				if (tmpcalc.GetAt(i) == '+' || tmpcalc.GetAt(i) == '/' || tmpcalc.GetAt(i) == '*')
				{
					operators++;
					if (operators == 2)
						end = i;
				}
				if (tmpcalc.GetAt(i) == '-' && tmpcalc.GetAt(i - 1) != '-' && tmpcalc.GetAt(i - 1) != '+' && tmpcalc.GetAt(i - 1) != '/' && tmpcalc.GetAt(i - 1) != '*')
				{
					operators++;
					if (operators == 2)
						end = i;
				}
			}
			if (end >= tmpcalc.Length() - 1 && operators > 0)
			{
				ReportError("Syntax error in math operation: '" + tmpcalc + "' (might be nested)");
				return "false";
			}
			if (operators > 1)
			{
				csString newdata;
				newdata = Calc(tmpcalc.Slice(0, end));
				//construct new string
				two = tmpcalc.Slice(end);
				tmpcalc = newdata + two;
			}
			else
				break;
		} while (1 == 1);
		
		//return value if none found
		if (operators == 0)
			return tmpcalc.GetData();
	
		//otherwise perform math
		temp1 = tmpcalc.Find("+", 1);
		if (temp1 > 0)
		{
			one = tmpcalc.Slice(0, temp1);
			two = tmpcalc.Slice(temp1 + 1);
			if (IsNumeric(one.GetData()) == true && IsNumeric(two.GetData()) == true)
			{
				tmpcalc = _gcvt(atof(one.GetData()) + atof(two.GetData()), 12, buffer);
				if (tmpcalc.GetAt(tmpcalc.Length() - 1) == '.')
					tmpcalc = tmpcalc.Slice(0, tmpcalc.Length() - 1); //strip of extra decimal point if even
				return tmpcalc.GetData();
			}
		}
		temp1 = tmpcalc.Find("-", 1);
		if (temp1 > 0)
		{
			one = tmpcalc.Slice(0, temp1);
			two = tmpcalc.Slice(temp1 + 1);
			if (IsNumeric(one.GetData()) == true && IsNumeric(two.GetData()) == true)
			{
				tmpcalc = _gcvt(atof(one.GetData()) - atof(two.GetData()), 12, buffer);
				if (tmpcalc.GetAt(tmpcalc.Length() - 1) == '.')
					tmpcalc = tmpcalc.Slice(0, tmpcalc.Length() - 1); //strip of extra decimal point if even
				return tmpcalc.GetData();
			}
		}
		temp1 = tmpcalc.Find("/", 1);
		if (temp1 > 0)
		{
			one = tmpcalc.Slice(0, temp1);
			two = tmpcalc.Slice(temp1 + 1);
			if (IsNumeric(one.GetData()) == true && IsNumeric(two.GetData()) == true)
			{
				tmpcalc = _gcvt(atof(one.GetData()) / atof(two.GetData()), 12, buffer);
				if (tmpcalc.GetAt(tmpcalc.Length() - 1) == '.')
					tmpcalc = tmpcalc.Slice(0, tmpcalc.Length() - 1); //strip of extra decimal point if even
				return tmpcalc.GetData();
			}
		}
		temp1 = tmpcalc.Find("*", 1);
		if (temp1 > 0)
		{
			one = tmpcalc.Slice(0, temp1);
			two = tmpcalc.Slice(temp1 + 1);
			if (IsNumeric(one.GetData()) == true && IsNumeric(two.GetData()) == true)
			{
				tmpcalc = _gcvt(atof(one.GetData()) * atof(two.GetData()), 12, buffer);
				if (tmpcalc.GetAt(tmpcalc.Length() - 1) == '.')
					tmpcalc = tmpcalc.Slice(0, tmpcalc.Length() - 1); //strip of extra decimal point if even
				return tmpcalc.GetData();
			}
		}
		return tmpcalc.GetData();
	}

	//boolean operators
	temp1 = tmpcalc.Find("=", 1);
	if (temp1 > 0)
	{
		if (atof(tmpcalc.Slice(0, temp1)) == atof(tmpcalc.Slice(temp1 + 1)))
			return "true";
		else
			return "false";
	}
	temp1 = tmpcalc.Find("!", 1);
	if (temp1 > 0)
	{
		if (atof(tmpcalc.Slice(0, temp1)) != atof(tmpcalc.Slice(temp1 + 1)))
			return "true";
		else
			return "false";
	}
	temp1 = tmpcalc.Find("<", 1);
	if (temp1 > 0)
	{
		if (atof(tmpcalc.Slice(0, temp1)) < atof(tmpcalc.Slice(temp1 + 1)))
			return "true";
		else
			return "false";
	}
	temp1 = tmpcalc.Find(">", 1);
	if (temp1 > 0)
	{
		if (atof(tmpcalc.Slice(0, temp1)) > atof(tmpcalc.Slice(temp1 + 1)))
			return "true";
		else
			return "false";
	}

	return tmpcalc.GetData();
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

void SBS::EnableColumnFrame(bool value)
{
	//turns column frame on/off
	if (value == true)
	{
		ColumnFrame->GetFlags().Reset (CS_ENTITY_INVISIBLEMESH);
		ColumnFrame->GetFlags().Reset (CS_ENTITY_NOSHADOWS);
		ColumnFrame->GetFlags().Reset (CS_ENTITY_NOHITBEAM);
	}
	else
	{
		ColumnFrame->GetFlags().Set (CS_ENTITY_INVISIBLEMESH);
		ColumnFrame->GetFlags().Set (CS_ENTITY_NOSHADOWS);
		ColumnFrame->GetFlags().Set (CS_ENTITY_NOHITBEAM);
	}
	IsColumnFrameEnabled = value;
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

int SBS::CreateSky()
{
	SkyBox = (engine->CreateSectorWallsMesh (area, "SkyBox"));
	SkyBox_state = scfQueryInterface<iThingFactoryState> (SkyBox->GetMeshObject()->GetFactory());
	SkyBox->SetZBufMode(CS_ZBUF_USE);

	int firstidx = SkyBox_state->AddInsideBox(csVector3(-2000, -1000, -2000), csVector3(2000, 3000, 2000));
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

int SBS::GetFloorNumber(float altitude)
{
	//Returns floor number located at a specified altitude

	//check to see if altitude is below bottom floor
	if (altitude < GetFloor(-Basements)->Altitude)
		return -Basements;

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

iMaterialWrapper *SBS::ChangeTexture(iMeshObject *mesh, csRef<iMaterialWrapper> oldmat, const char *texture)
{
	//changes a texture

	csRef<iThingState> thingstate = scfQueryInterface<iThingState> (mesh);

	//get new material
	csRef<iMaterialWrapper> newmat = engine->GetMaterialList()->FindByName(texture);

	//switch material
	thingstate->ClearReplacedMaterials();
	thingstate->ReplaceMaterial(oldmat, newmat);

	return newmat;
}

void SBS::SetTexture(csRef<iThingFactoryState> mesh, int index, const char *texture, float tw, float th)
{
	//sets a polygon's texture

	material = engine->GetMaterialList()->FindByName(texture);

	if (material == 0)
	{
		//if material's not found, display a warning and use a default material
		csString Texture = texture;
		csString polyname = mesh->GetPolygonName(index);
		csString message = "Texture '" + Texture + "' not found for polygon '" + polyname + "'; using default material";
		ReportError(message);
		//set to default material
		material = engine->GetMaterialList()->FindByName("Default");
	}

	for (int i = index; i < index + GetDrawWallsCount(); i++)
	{
		mesh->SetPolygonMaterial(csPolygonRange(i, i), material);
		//texture mapping is set from first 3 coordinates
		mesh->SetPolygonTextureMapping (csPolygonRange(i, i),
			mesh->GetPolygonVertex(i, 0),
			csVector2 (0, 0),
			mesh->GetPolygonVertex(i, 1),
			csVector2 (tw, 0),
			mesh->GetPolygonVertex(i, 2),
			csVector2 (tw, th));
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

	for (size_t i = 0; i < FloorArray.GetSize(); i++)
		if (FloorArray[i].number == number)
			return FloorArray[i].object;
	return 0;
}

Elevator *SBS::GetElevator(int number)
{
	//return pointer to elevator object

	for (size_t i = 0; i < ElevatorArray.GetSize(); i++)
		if (ElevatorArray[i].number == number)
			return ElevatorArray[i].object;
	return 0;
}

Shaft *SBS::GetShaft(int number)
{
	//return pointer to shaft object

	for (size_t i = 0; i < ShaftArray.GetSize(); i++)
		if (ShaftArray[i].number == number)
			return ShaftArray[i].object;
	return 0;
}

Stairs *SBS::GetStairs(int number)
{
	//return pointer to stairs object

	for (size_t i = 0; i < StairsArray.GetSize(); i++)
		if (StairsArray[i].number == number)
			return StairsArray[i].object;
	return 0;
}

Door *SBS::GetDoor(int number)
{
	//return pointer to door object

	for (size_t i = 0; i < DoorArray.GetSize(); i++)
		if (DoorArray[i].number == number)
			return DoorArray[i].object;
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

void SBS::PushFrame()
{
	if (!equeue)
		return ;

	if (vc)
		vc->Advance();

	equeue->Process();
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

int SBS::CreateDoor(csRef<iThingFactoryState> cutmesh, csVector3 cutmesh_origin, const char *texture, float thickness, int direction, float CenterX, float CenterZ, float width, float height, float altitude, float tw, float th)
{
	//create a door object

	DoorArray.SetSize(DoorArray.GetSize() + 1);
	DoorArray[DoorArray.GetSize() - 1].number = DoorArray.GetSize();
	DoorArray[DoorArray.GetSize() - 1].object = new Door(cutmesh, cutmesh_origin, DoorArray.GetSize(), texture, thickness, direction, CenterX, CenterZ, width, height, altitude, tw, th);
	return DoorArray.GetSize();
}

int SBS::Doors()
{
	//return the number of doors
	return DoorArray.GetSize();
}

float SBS::MetersToFeet(float meters)
{
	//converts meters to feet

	return meters * 3.2808399f;
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

void SBS::Stop()
{
	//stop timer
	p->Stop();
}

void SBS::SetAutoSize(bool x, bool y)
{
	//enable or disable texture autosizing
	AutoX = x;
	AutoY = y;
}
