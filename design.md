# The Design of Skyscraper and the Scalable Building Simulator

### by Ryan Thoryk



This document is a basic birds-eye view of the internals of both the frontend and the backend (the Scalable Building Simulator engine) of Skyscraper.

This is intended for people who are interested in helping out with C++ development to quickly get familiar with the system's design and how it works.

***

Skyscraper is comprised of two parts: the frontend, and the Scalable Building Simulator (backend).  The frontend is the controller portion of the program and is responsible for setting up and managing the CrystalSpace environment, for starting and shutting down the SBS simulator, making calls to SBS, handling keyboard/mouse input, and rendering.  The reason this is split is so that there can be multiple frontends sharing the same simulation library, such as the regular Skyscraper app, a graphical building designer, etc all sharing the same SBS.dll.  Skyscraper.exe is the frontend, and does those things mentioned above, and also has a script interpreter which makes the SBS calls (the script language is known as ISL2, or Interactive Scripting Language 2).


## A Typical Run

When the program is run, wxWidgets will pass control onto the Skyscraper class for startup (the `Skyscraper::OnInit()` function). Skyscraper will create the main window, initialize CrystalSpace, set up things such as sound and the buttons, and loop inside the main intro screen.  When a building is selected, `Start()` will be called which creates an SBS object, initializes it, runs the script interpreter for the selected file (which makes lots of SBS calls), tells SBS to create the sky, runs SBS's `Start()` function (the startup initializer), and creates the debug/control panel.  Some flags are then set which tell Skyscraper that the simulator is ready to run.  When the main screen idles, wxWidgets calls that window's `OnIdle()` function which calls Skyscraper's `PushFrame()` function (this is how the program loops).  PushFrame is the program's top-level loop function, and does some CrystalSpace work and calls the program's CrystalSpace event handler.  The event handler eventually does a frame dispatch which calls `SetupFrame()`, which is Skyscraper's top-level run loop function.  This calls things such as the SBS main loop, the input handler (for keyboard/mouse input), the SBS camera loop, and CrystalSpace rendering.  So in a normal run, SBS's main loop will be called first in there, which is `SBS::MainLoop()`.  In this loop, other runloops are called, such as `Elevator::MonitorLoop()` for each elevator (the elevator loop; this handles all elevator movement, elevator door movement, shaft door movement, etc), the camera's `CheckShaft` function (which is basically a shaft loop that handles the displaying of shafts), and the camera's `CheckStairs` function (which is basically the stairs loop and handles the displaying of stairs).   The mainloop also handles door callbacks, which operate the door movement (it calls `Door::MoveDoor()` which is the Door main loop and does the raw door movement).  After the SBS main loop is run, Skyscraper's input handler is run which gets keyboard and mouse input from wxWidgets, and calls the related SBS function for these.  This function also calls functions such as `Camera::ClickObject` which checks to see if a user clicked on an object (and performs the related functions for that, such as a button click).  After that, the SBS camera loop (`Camera::Loop()`) is run, which handles all camera movement, collision detection, and keeping the sound listener object locked to the position and rotation of the camera.  Finally the `Render()` function is run, which tells CrystalSpace to render the 3D graphics onto the screen.  The program will loop in this way, until the main simulator window is closed.  When it's closed, wxWidgets will call that window's `OnClose()` function (`MainWindow::OnClose()`) which turns off the debug panel timer and calls the main Skyscraper app's OnExit function.  The `Skyscraper::OnExit()` function stops the frame printer (automatic renderer), cleans up the sound object, deletes all windows, deletes the SBS object (which causes the simulator to clean up all it's objects), and calls CrystalSpace's `DestroyApplication()` process which cleans up the rest of the objects.


## Some Function Descriptions
### The Frontend:
`Skyscraper::OnInit()` - this is called by wxWidgets on program startup, and creates the main window and calls the CrystalSpace initialization function.
`Skyscraper::OnExit()` - this is called by wxWidgets when a user closes the main window, and is responsible for cleaning up objects and shutting down the app.
`MainScreen` – this is a wxWidgets window class, and represents the main simulator window.
`Skyscraper::Render()` - this is the CrystalSpace rendering function
`Skyscraper::SetupFrame()` - this is considered the top-level runloop function, and calls everything else from this, such as the main menu stuff, window resize functions, the SBS main loop, the input handler, the SBS camera loop, and the rendering function.  This function also has speed limiter code in order to keep the simulator running at the same speed despite changes in frame rate (the frame rate is basically how many times a second the entire app performs one loop cycle).
`Skyscraper::HandleEvent()` - this is right above `SetupFrame()`, and is a CrystalSpace event dispatcher; this is responsible for running both the SBS `CalculateFrameRate()` function and `SetupFrame()` within a frame event context.
`SkyscraperEventHandler()` - this is the top-level event handler, and really just calls HandleEvent.  This is outside of the Skyscraper class.
`Skyscraper::Initialize()` - this is the CrystalSpace initialization code.
`Skyscraper::GetInput()` - this is the keyboard and mouse input handler.  It uses wxWidgets for events, and calls SBS to process those events.
`Skyscraper::PushFrame()` - this is the CrystalSpace top-level loop code, and is responsible for advancing CrystalSpace's virtual clock, and for calling the event handler.  This function is called from MainScreen's `OnIdle()` function.
`Skyscraper::SelectBuilding()` - this is the wxWidgets code that pops up a file selection dialog for selecting a building.
`Skyscraper::Start()` - this starts the simulator.  This creates a new SBS object, calls the SBS initialization function, calls the script interpreter (which translates the script commands into SBS calls),  tells SBS to create the sky, runs the CrystalSpace engine object preparation, runs the SBS `Start()` function, and creates the debug control panel.

The file fileio.cpp has the raw building loader code and the script interpreter.  These are part of the Skyscraper class.
