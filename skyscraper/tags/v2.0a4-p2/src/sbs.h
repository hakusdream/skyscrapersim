/* $Id$ */

/*
	Scalable Building Simulator - Core
	The Skyscraper Project - Version 1.4 Alpha
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

#ifndef _SBS_H
#define _SBS_H

#include "floor.h"
#include "elevator.h"
#include "shaft.h"
#include "camera.h"
#include "stairs.h"
#include "sound.h"

//global functions

struct SBSIMPEXP FloorMap
{
	int number; //floor number
	Floor *object; //floor object reference
};

struct SBSIMPEXP ElevatorMap
{
	int number; //elevator number
	Elevator *object; //elevator object reference
};

struct SBSIMPEXP ShaftMap
{
	int number; //shaft number
	Shaft *object; //shaft object reference
};

struct SBSIMPEXP StairsMap
{
	int number; //stairs number
	Stairs *object; //stairs object reference
};

//SBS class
class SBSIMPEXP SBS
{
public:

	//CS engine objects
	csRef<iEngine> engine;
	csRef<iGraphics3D> g3d;
	csRef<iGraphics2D> g2d;
	csRef<iLoader> loader;
	csRef<iVirtualClock> vc;
	csRef<iView> view;
	csRef<iVFS> vfs;
	csRef<iCollideSystem> collision_sys;
	csRef<iReporter> rep;
	csRef<iSndSysRenderer> sndrenderer;
	csRef<iSndSysLoader> sndloader;
	csRef<iMaterialWrapper> material;
	csRef<iSector> area;

	csTicks elapsed_time;
	float delta;

	//Building information
	csString BuildingName;
	csString BuildingDesigner;
	csString BuildingLocation;
	csString BuildingDescription;
	csString BuildingVersion;

	//Internal data
	bool IsRunning; //is sim engine running?
	int Floors; //number of above-ground floors including 0
	int Basements; //number of basement floors
	Camera *camera; //camera object
	bool RenderOnly; //skip sim processing and only render graphics
	bool InputOnly; //skip sim processing and only run input and rendering code
	bool IsFalling; //make user fall
	bool InStairwell; //true if user is in a stairwell
	bool InElevator; //true if user is in an elevator
	int ElevatorNumber; //number of currently selected elevator
	bool ElevatorSync; //true if user should move with elevator
	bool FrameLimiter; //frame limiter toggle
	int FrameRate; //max frame rate
	float running_time; //time (in seconds) the simulator has been running
	float start_time; //time (in seconds) that the simulator mainloop started
	float HorizScale; //horizontal X/Z scaling multiplier (in feet). Normally is 1
	bool IsBuildingsEnabled; //contains status of buildings object
	bool IsExternalEnabled; //contains status of external object
	bool IsLandscapeEnabled; //contains status of landscape object
	bool IsSkyboxEnabled; //contains status of skybox object
	float FPS; //current frame rate
	bool AutoShafts; //true if shafts should turn on and off automatically
	bool AutoStairs; //true if stairwells should turn on and off automatically
	bool ProcessElevators; //true if elevator system should be enabled
	bool ShowFullShafts; //true if entire shaft should be displayed while user is in an elevator
	bool ShowFullStairs; //true if entire stairwell should be displayed while user is in it
	int ShaftDisplayRange; //number of shaft floors to display while in elevator; has no effect if ShowFullShafts is true
	int StairsDisplayRange; //number of stairwell floors to display while in stairwell; has no effect if ShowFullStairs is true
	int ShaftOutsideDisplayRange; //number of shaft floors to display while outside of shaft
	int StairsOutsideDisplayRange; //number of stairwell floors to display while outside of stairwell
	int FloorDisplayRange; //number of floors to display while in elevator, if shaft's ShowFloors is true
	bool TextureOverride; //if enabled, overrides textures with ones set with SetTextureOverride()
	bool FlipTexture; //if enabled, flips textures according to parameters set in SetTextureFlip()
	csString SkyName; //base filename of sky texture pack
	csString root_dir; //app directory
	csString dir_char;

	//mouse coordinates
	int mouse_x, mouse_y;

	//public functions
	SBS();
	~SBS();
	void Report (const char* msg, ...);
	bool ReportError (const char* msg, ...);
	bool LoadTexture(const char *filename, const char *name, float widthmult, float heightmult);
	float AutoSize(float n1, float n2, bool iswidth, float offset);
	void Initialize(iSCF* scf, iGraphics3D* g3dref, iGraphics2D* g2dref, iEngine* engineref, iLoader* loaderref, iVirtualClock* vcref, iView* viewref, iVFS* vfsref,
					iCollideSystem* collideref, iReporter* reporterref, iSndSysRenderer* sndrenderref, iSndSysLoader* sndloaderref,
					iMaterialWrapper* matref, iSector* sectorref, const char* rootdirectory, const char* directory_char);
	bool Start();
	int CreateSky(const char *filenamebase);
	void AddLight(const char *name, float x, float y, float z, float radius, float r, float g, float b);
	int AddWallMain(csRef<iThingFactoryState> dest, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float altitude1, float altitude2, float tw, float th);
	int AddFloorMain(csRef<iThingFactoryState> dest, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float altitude1, float altitude2, float tw, float th);
	void DeleteWall(csRef<iThingFactoryState> dest, int index);
	void DeleteFloor(csRef<iThingFactoryState> dest, int index);
	void CalculateFrameRate();
	void MainLoop();
	int CreateWallBox(csRef<iThingFactoryState> dest, const char *name, const char *texture, float x1, float x2, float z1, float z2, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom);
	int CreateWallBox2(csRef<iThingFactoryState> dest, const char *name, const char *texture, float CenterX, float CenterZ, float WidthX, float LengthZ, float height_in, float voffset, float tw, float th, bool inside, bool outside, bool top, bool bottom);
	int AddTriangleWall(csRef<iThingFactoryState> dest, const char *name, const char *texture, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float tw, float th);
	int AddCustomWall(csRef<iThingFactoryState> dest, const char *name, const char *texture, csPoly3D &varray, float tw, float th);
	int AddCustomFloor(csRef<iThingFactoryState> dest, const char *name, const char *texture, csPoly3D &varray, float tw, float th);
	csVector2 GetExtents(csPoly3D &varray, int coord);
	void InitMeshes();
	void EnableBuildings(bool value);
	void EnableLandscape(bool value);
	void EnableExternal(bool value);
	void EnableSkybox(bool value);
	int GetFloorNumber(float altitude, int lastfloor = 0, bool checklastfloor = false);
	float GetDistance(float x1, float x2, float z1, float z2);
	void DumpVertices(csRef<iThingFactoryState> mesh);
	void ListAltitudes();
	void CreateShaft(int number, int type, float CenterX, float CenterZ, int _startfloor, int _endfloor);
	void CreateStairwell(int number, float CenterX, float CenterZ, int _startfloor, int _endfloor);
	void SetTexture(csRef<iThingFactoryState> mesh, int index, const char *texture, bool has_thickness, float tw, float th);
	iMaterialWrapper* ChangeTexture(iMeshWrapper *mesh, const char *texture, bool matcheck = true);
	iMaterialWrapper* GetTextureMaterial(const char *texture, bool &result, const char *polygon_name = 0);
	bool NewElevator(int number);
	bool NewFloor(int number);
	int Elevators();
	int TotalFloors(); //all floors including basements
	int Shafts();
	int StairsNum();
	Floor* GetFloor(int number);
	Elevator* GetElevator(int number);
	Shaft* GetShaft(int number);
	Stairs* GetStairs(int number);
	bool SetWallOrientation(const char *direction);
	int GetWallOrientation();
	bool SetFloorOrientation(const char *direction);
	int GetFloorOrientation();
	void DrawWalls(bool MainN, bool MainP, bool SideN, bool SideP, bool Top, bool Bottom);
	void ResetWalls(bool ToDefaults = false);
	void ReverseExtents(bool X, bool Y, bool Z);
	void ResetExtents(bool ToDefaults = false);
	void ReverseAxis(bool value);
	bool GetReverseAxis();
	void SetAutoSize(bool x, bool y);
	csVector2 GetAutoSize();
	int GetDrawWallsCount();
	csVector3 GetPoint(csRef<iThingFactoryState> mesh, const char *polyname, const csVector3 &start, const csVector3 &end);
	void Cut(csRef<iThingFactoryState> state, const csVector3 &start, const csVector3 &end, bool cutwalls, bool cutfloors, const csVector3 &mesh_origin, const csVector3 &object_origin, int checkwallnumber = 0, const char *checkstring = "");
	float MetersToFeet(float meters); //converts meters to feet
	float FeetToMeters(float feet); //converts feet to meters
	int AddDoorwayWalls(csRef<iThingFactoryState> mesh, const char *texture, float tw, float th);
	void SetListenerLocation(const csVector3 &location);
	void SetListenerDirection(const csVector3 &front, const csVector3 &top);
	void SetListenerDistanceFactor(float factor);
	float GetListenerDistanceFactor();
	void SetListenerRollOffFactor(float factor);
	float GetListenerRollOffFactor();
	void SetTextureOverride(const char *mainneg, const char *mainpos, const char *sideneg, const char *sidepos, const char *top, const char *bottom);
	void SetTextureFlip(int mainneg, int mainpos, int sideneg, int sidepos, int top, int bottom);
	int AddWall(const char *meshname, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float height_in1, float height_in2, float altitude1, float altitude2, float tw, float th);
	int AddFloor(const char *meshname, const char *name, const char *texture, float thickness, float x1, float z1, float x2, float z2, float altitude1, float altitude2, float tw, float th);
	int AddGround(const char *name, const char *texture, float x1, float z1, float x2, float z2, float altitude, int tile_x, int tile_z);
	void EnableFloorRange(int floor, int range, bool value, bool enablegroups, int shaftnumber = 0);
	bool RegisterDoorCallback(Door *door);
	bool UnregisterDoorCallback(Door *door);
	void ProcessTextureFlip(float tw, float th);
	bool GetTextureTiling(const char *texture, float &tw, float &th);
	bool AddTextToTexture(const char *texturename, const char *fontname, const char *text, uint x1, uint y1, uint x2, uint y2, uint h_align, uint v_align);
	void EnableMesh(csRef<iMeshWrapper> mesh, bool value);
	iMeshWrapper* AddGenWall(csRef<iMeshWrapper> mesh, const char *texture, float x1, float z1, float x2, float z2, float height, float altitude, float tw, float th);

	//Meshes
	csRef<iMeshWrapper> Buildings; //building mesh
		csRef<iThingFactoryState> Buildings_state;

	csRef<iMeshWrapper> External; //external mesh
		csRef<iThingFactoryState> External_state;

	csRef<iMeshWrapper> Landscape; //landscape mesh
		csRef<iThingFactoryState> Landscape_state;

	csRef<iMeshWrapper> SkyBox; //skybox mesh
		csRef<iThingFactoryState> SkyBox_state;

private:

	//fps
	int fps_frame_count;
	int fps_tottime;
	float remaining_delta;

	//conversion buffers
	char intbuffer[65];
	char buffer[20];

	//orientations
	int wall_orientation;
	int floor_orientation;

	//wall/floor sides
	bool DrawMainN; //or top, if floor
	bool DrawMainP; //or bottom, if floor
	bool DrawSideN;
	bool DrawSideP;
	bool DrawTop; //or back, if floor
	bool DrawBottom; //or front, if floor
	bool ReverseAxisValue;

	//old wall/floor sides
	bool DrawMainNOld; //or top, if floor
	bool DrawMainPOld; //or bottom, if floor
	bool DrawSideNOld;
	bool DrawSidePOld;
	bool DrawTopOld; //or back, if floor
	bool DrawBottomOld; //or front, if floor

	//texture mapping
	bool RevX, RevY, RevZ;
	bool RevXold, RevYold, RevZold;
	bool AutoX, AutoY; //autosizing

	//object arrays
	csArray<FloorMap> FloorArray; //floor object array
	csArray<ElevatorMap> ElevatorArray; //elevator object array
	csArray<ShaftMap> ShaftArray; //shaft object array
	csArray<StairsMap> StairsArray; //stairs object array

	//private functions
	void PrintBanner();

	//doorway data
	bool wall1a, wall1b, wall2a, wall2b;
	csVector2 wall_extents_x, wall_extents_z, wall_extents_y;

	//texture information structure
	struct TextureInfo
	{
		csString name;
		float widthmult;
		float heightmult;
	};

	csArray<TextureInfo> textureinfo;

	//override textures
	csString mainnegtex, mainpostex, sidenegtex, sidepostex, toptex, bottomtex;

	//texture flipping
	int mainnegflip, mainposflip, sidenegflip, sideposflip, topflip, bottomflip;
	csArray<int> widthscale;
	csArray<int> heightscale;

	//door object for callback
	Door *callbackdoor;
};

#endif
