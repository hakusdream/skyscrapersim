Attribute VB_Name = "SimCore"
'Skycraper 0.97 Beta - Simulator core
'Copyright (C) 2004 Ryan Thoryk
'http://www.tliquest.net/skyscraper
'http://sourceforge.net/projects/skyscraper
'Contact - ryan@tliquest.net

'This program is free software; you can redistribute it and/or
'modify it under the terms of the GNU General Public License
'as published by the Free Software Foundation; either version 2
'of the License, or (at your option) any later version.

'This program is distributed in the hope that it will be useful,
'but WITHOUT ANY WARRANTY; without even the implied warranty of
'MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
'GNU General Public License for more details.

'You should have received a copy of the GNU General Public License
'along with this program; if not, write to the Free Software
'Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
'
Option Explicit

Sub CheckCollisions()
Dim jxx As Single
 'Main collision code
LineTest = lineend
 If lineend.X > linestart.X Then LineTest.X = lineend.X + 2
 If lineend.X < linestart.X Then LineTest.X = lineend.X - 2
 If lineend.z > linestart.z Then LineTest.z = lineend.z + 2
 If lineend.z < linestart.z Then LineTest.z = lineend.z - 2
    
'Turn on collisions
        Room(CameraFloor).SetCollisionEnable True
        CrawlSpace(CameraFloor).SetCollisionEnable True
        PipeShaft(1).SetCollisionEnable True
        PipeShaft(2).SetCollisionEnable True
        PipeShaft(3).SetCollisionEnable True
        PipeShaft(4).SetCollisionEnable True
        Buildings.SetCollisionEnable True
        Landscape.SetCollisionEnable True
        External.SetCollisionEnable True
        Shafts1(CameraFloor).SetCollisionEnable True
        Shafts2(CameraFloor).SetCollisionEnable True
        Shafts3(CameraFloor).SetCollisionEnable True
        Shafts4(CameraFloor).SetCollisionEnable True
        ShaftsFloor(CameraFloor).SetCollisionEnable True
        For jxx = 1 To 40
        Elevator(jxx).SetCollisionEnable True
        ElevatorInsDoorL(jxx).SetCollisionEnable True
        ElevatorInsDoorR(jxx).SetCollisionEnable True
        ElevatorDoorL(jxx).SetCollisionEnable True
        ElevatorDoorR(jxx).SetCollisionEnable True
        Next jxx
        If StairDataTable(CameraFloor) = True Then Stairs(CameraFloor).SetCollisionEnable True
        
 'Elevator Collision
 For jxx = 1 To 40
    If Elevator(jxx).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If ElevatorInsDoorL(jxx).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If ElevatorInsDoorR(jxx).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
 Next jxx
    
 'Collision code for all other objects
    For jxx = 1 To 40
    If ElevatorDoorL(jxx).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If ElevatorDoorR(jxx).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    Next jxx
    
    If External.Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If Room(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If CrawlSpace(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If StairDataTable(CameraFloor) = True Then If Stairs(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If Buildings.Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If Landscape.Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If Shafts1(CameraFloor).IsMeshEnabled = True Then If Shafts1(CameraFloor).IsMeshEnabled = True Then If Shafts1(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If Shafts2(CameraFloor).IsMeshEnabled = True Then If Shafts2(CameraFloor).IsMeshEnabled = True Then If Shafts2(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If Shafts3(CameraFloor).IsMeshEnabled = True Then If Shafts3(CameraFloor).IsMeshEnabled = True Then If Shafts3(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If Shafts4(CameraFloor).IsMeshEnabled = True Then If Shafts4(CameraFloor).IsMeshEnabled = True Then If Shafts4(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If ShaftsFloor(CameraFloor).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    
    For jxx = 1 To 4
    If StairDataTable(CameraFloor) = True Then If StairDoor(jxx).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    If PipeShaft(jxx).Collision(linestart, LineTest, TV_TESTTYPE_ACCURATETESTING) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    Next jxx

'Object Collision
On Error GoTo MethodFix
For jxx = 1 To 150
j50 = jxx + (150 * (CameraFloor - 1))
If Objects(j50).IsMeshEnabled = True Then
    'Objects(j50).SetCollisionEnable True
    If Objects(j50).Collision(linestart, LineTest, TV_TESTTYPE_BOUNDINGBOX) = True Then Camera.SetPosition linestart.X, Camera.GetPosition.Y, linestart.z: GoTo CollisionEnd
    'Objects(j50).SetCollisionEnable False
End If
MethodFix:
Next jxx

CollisionEnd:

'Turn off collisions
        Room(CameraFloor).SetCollisionEnable False
        CrawlSpace(CameraFloor).SetCollisionEnable False
        PipeShaft(1).SetCollisionEnable False
        PipeShaft(2).SetCollisionEnable False
        PipeShaft(3).SetCollisionEnable False
        PipeShaft(4).SetCollisionEnable False
        External.SetCollisionEnable False
        Buildings.SetCollisionEnable False
        Landscape.SetCollisionEnable False
        Shafts1(CameraFloor).SetCollisionEnable False
        Shafts2(CameraFloor).SetCollisionEnable False
        Shafts3(CameraFloor).SetCollisionEnable False
        Shafts4(CameraFloor).SetCollisionEnable False
        ShaftsFloor(CameraFloor).SetCollisionEnable False
        For jxx = 1 To 40
        Elevator(jxx).SetCollisionEnable False
        ElevatorInsDoorL(jxx).SetCollisionEnable False
        ElevatorInsDoorR(jxx).SetCollisionEnable False
        ElevatorDoorL(jxx).SetCollisionEnable False
        ElevatorDoorR(jxx).SetCollisionEnable False
        Next jxx
        If StairDataTable(CameraFloor) = True Then Stairs(CameraFloor).SetCollisionEnable False
        
End Sub

Sub Fall()

'This detects if the user is above a hole or something (ready to fall)
Room(CameraFloor).SetCollisionEnable True
CrawlSpace(CameraFloor).SetCollisionEnable True
Shafts1(CameraFloor).SetCollisionEnable True
Shafts2(CameraFloor).SetCollisionEnable True
Shafts3(CameraFloor).SetCollisionEnable True
Shafts4(CameraFloor).SetCollisionEnable True
Buildings.SetCollisionEnable True
Landscape.SetCollisionEnable True
        
If Room(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And _
    CrawlSpace(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And _
    Shafts1(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And _
    Shafts2(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And _
    Shafts3(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And _
    Shafts4(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And _
    Landscape.Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And _
    Buildings.Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 12, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And InElevator = False And InStairwell = False Then IsFalling = True

Room(CameraFloor).SetCollisionEnable False
CrawlSpace(CameraFloor).SetCollisionEnable False
Shafts1(CameraFloor).SetCollisionEnable False
Shafts2(CameraFloor).SetCollisionEnable False
Shafts3(CameraFloor).SetCollisionEnable False
Shafts4(CameraFloor).SetCollisionEnable False
Buildings.SetCollisionEnable False
Landscape.SetCollisionEnable False

'*********************************
If IsFalling = False Then Exit Sub

'The gravity originally acted weird
Dim TimeRate As Single
Dim OldPos As Single
Dim NewPos As Single
Dim HeightChange As Single
Gravity = 5

If FallRate = 0 Then
lngOldTick = GetTickCount()
CameraOriginalPos = Camera.GetPosition.Y
End If

'MsgBox ((GetTickCount() / 1000) - (lngOldTick / 1000))

'Basically this is Fallrate=Gravity*SecondsPassed
TimeRate = ((GetTickCount() / 1000) - (lngOldTick / 1000))
FallRate = (Gravity * TimeRate) ^ 1.8
If FallRate = 0 Then FallRate = 0.1
If TimeRate = 0 Then TimeRate = 0.1
'MsgBox ("Gravity:" + Str$(Gravity) + " Time Passed:" + Str$((GetTickCount() / 1000) - (lngOldTick / 1000)))

OldPos = Camera.GetPosition.Y
Camera.SetPosition Camera.GetPosition.X, CameraOriginalPos - FallRate, Camera.GetPosition.z
NewPos = Camera.GetPosition.Y
HeightChange = OldPos - NewPos
'If HeightChange > 546 Then MsgBox (FallRate) 'terminal velocity

Room(CameraFloor).SetCollisionEnable True
CrawlSpace(CameraFloor).SetCollisionEnable True
Shafts1(CameraFloor).SetCollisionEnable True
Shafts2(CameraFloor).SetCollisionEnable True
Shafts3(CameraFloor).SetCollisionEnable True
Shafts4(CameraFloor).SetCollisionEnable True
Buildings.SetCollisionEnable True
Landscape.SetCollisionEnable True

'If Room(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - (FallRate / TimeRate), Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Then
If Room(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Or _
    CrawlSpace(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Or _
    Shafts1(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Or _
    Shafts2(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Or _
    Shafts3(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Or _
    Shafts4(CameraFloor).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Or _
    Buildings.Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Or _
    Landscape.Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - HeightChange - 10, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Then

        FallRate = 0
        IsFalling = False
        If CameraFloor > BottomFloor Then Camera.SetPosition Camera.GetPosition.X, GetFloorAltitude(CameraFloor) + 10, Camera.GetPosition.z
        If CameraFloor = 0 Then Camera.SetPosition Camera.GetPosition.X, 10, Camera.GetPosition.z
End If

Room(CameraFloor).SetCollisionEnable False
CrawlSpace(CameraFloor).SetCollisionEnable False
Shafts1(CameraFloor).SetCollisionEnable False
Shafts2(CameraFloor).SetCollisionEnable False
Shafts3(CameraFloor).SetCollisionEnable False
Shafts4(CameraFloor).SetCollisionEnable False
Buildings.SetCollisionEnable False
Landscape.SetCollisionEnable False

End Sub

Sub CloseDoor()
ClosingDoor = ClosingDoor - 1
On Error Resume Next
Objects(DoorNumber).RotateY -(ClosingDoor / 110)
If DoorRotated = 0 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X - (ClosingDoor / 48), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z - (ClosingDoor / 38)
If DoorRotated = 1 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X - (ClosingDoor / 38), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z + (ClosingDoor / 48)
If DoorRotated = 2 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X + (ClosingDoor / 48), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z + (ClosingDoor / 38)
If DoorRotated = 3 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X + (ClosingDoor / 38), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z - (ClosingDoor / 48)

If ClosingDoor = 0 And DoorRotated = 0 Then Objects(DoorNumber).SetMeshName ("DoorA " + Str$(DoorNumber))
If ClosingDoor = 0 And DoorRotated = 1 Then Objects(DoorNumber).SetMeshName ("DoorB " + Str$(DoorNumber))
If ClosingDoor = 0 And DoorRotated = 2 Then Objects(DoorNumber).SetMeshName ("DoorC " + Str$(DoorNumber))
If ClosingDoor = 0 And DoorRotated = 3 Then Objects(DoorNumber).SetMeshName ("DoorD " + Str$(DoorNumber))

End Sub

Sub Intro()
LookUp = LookUp + 1
If LookUp < 168 Then Camera.ChaseCamera IntroMesh, Vector3(0, 5, 70), Vector(0, Camera.GetPosition.Y + 800 - (LookUp * 9.5), -90), 3, True, 75, False, 1
If LookUp >= 168 Then Camera.ChaseCamera IntroMesh, Vector3(0, 5, 70), Vector(0, 10, -90), 3, True, 75, False, 1
End Sub


Sub CloseStairDoor()
ClosingDoor = ClosingDoor - 1
CallingStairDoors = True
On Error Resume Next
StairDoor(DoorNumber).RotateY -(ClosingDoor / 110)
StairDoor(DoorNumber).SetPosition StairDoor(DoorNumber).GetPosition.X - (ClosingDoor / 38), StairDoor(DoorNumber).GetPosition.Y, StairDoor(DoorNumber).GetPosition.z + (ClosingDoor / 48)

If ClosingDoor = 0 Then
StairDoor(DoorNumber).SetMeshName ("DoorB " + Str$(DoorNumber))
CallingStairDoors = False
End If

End Sub

Sub OpenStairDoor()
OpeningDoor = OpeningDoor + 1
CallingStairDoors = True

'DoorRotated values:
'0 is horizontal, opens downward
'1 is vertical, opens to the left
'2 is horizontal, opens upward
'3 is vertical, opens to the right
On Error Resume Next

If Room(CameraFloor).IsMeshEnabled = False Then
Room(CameraFloor).Enable True
CrawlSpace(CameraFloor).Enable True
      For i51 = 1 To 40
      ElevatorDoorL(i51).Enable True
      ElevatorDoorL(i51).Enable True
      Next i51
      ShaftsFloor(CameraFloor).Enable True
      For i51 = 1 To 40
      CallButtonsUp(i51).Enable True
      CallButtonsDown(i51).Enable True
      Next i51
      If StairDataTable(CameraFloor) = False Then CreateStairs (CameraFloor)
      Atmos.SkyBox_Enable True
      Buildings.Enable True
      Landscape.Enable True
      Call InitRealtime(CameraFloor)
      InitObjectsForFloor (CameraFloor)
End If

StairDoor(DoorNumber).RotateY (OpeningDoor / 110)
StairDoor(DoorNumber).SetPosition StairDoor(DoorNumber).GetPosition.X + (OpeningDoor / 38), StairDoor(DoorNumber).GetPosition.Y, StairDoor(DoorNumber).GetPosition.z - (OpeningDoor / 48)

If OpeningDoor = 17 Then
    CallingStairDoors = False
    OpeningDoor = 0
    StairDoor(DoorNumber).SetMeshName ("DoorSBO " + Str$(DoorNumber))
End If
End Sub


Sub MainLoop()
On Error Resume Next

Dim FloorHeight As Single
Dim FloorAltitude As Single
'Determine floor that the camera is on, if the camera is not in the stairwell
If InStairwell = False Then CameraFloor = GetCameraFloor
FloorHeight = GetFloorHeight(CameraFloor)
FloorAltitude = GetFloorAltitude(CameraFloor)
    
'Calls frame limiter function, which sets the max frame rate
'note - the frame rate determines elevator speed, walking speed, etc
'In order to raise it, elevator timers and walking speed must both be changed
If DebugPanel.Check10.Value = 1 Then SlowToFPS (20)

If RenderOnly = True Then GoTo Render
If DebugPanel.Check11.Value = 0 Then GoTo InputOnly

If OpeningDoor > 0 And CallingStairDoors = False Then Call OpenDoor
If ClosingDoor > 0 And CallingStairDoors = False Then Call CloseDoor
If OpeningDoor > 0 And CallingStairDoors = True Then Call OpenStairDoor
If ClosingDoor > 0 And CallingStairDoors = True Then Call CloseStairDoor

'Turn on pipe shafts if camera is in the crawlspace area
If Camera.GetPosition.Y > FloorAltitude + 25 And Camera.GetPosition.Y < FloorAltitude + FloorHeight Then
PipeShaft(1).Enable True
PipeShaft(2).Enable True
PipeShaft(3).Enable True
PipeShaft(4).Enable True
Else
PipeShaft(1).Enable False
PipeShaft(2).Enable False
PipeShaft(3).Enable False
PipeShaft(4).Enable False
End If

If GotoFloor(ElevatorNumber) = 0 Then ElevatorSync(ElevatorNumber) = False

'This code turns off elevator objects and elevator sync if you leave the elevator
Elevator(ElevatorNumber).SetCollisionEnable True
If Elevator(ElevatorNumber).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 25, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = False And ElevatorSync(ElevatorNumber) = True Then
InElevator = False
ElevatorSync(ElevatorNumber) = False
ButtonsEnabled = False
For j50 = -11 To 144
Buttons(i54).ResetMesh
Next j50
End If
Elevator(ElevatorNumber).SetCollisionEnable False

'This code checks to see if the camera is in an elevator or not (to draw the elevator buttons, set current elevator, etc)
'It draws a testing line below the camera, to see if the line hits the floor of an elevator.
    
For i50 = 1 To 40
    
    'This code fixes a bug where the camera's height changes in the elevator if the user moves
    If ElevatorSync(i50) = True And GotoFloor(i50) <> 0 And OpenElevator(i50) = 0 Then Camera.SetPosition Camera.GetPosition.X, Elevator(i50).GetPosition.Y + 10, Camera.GetPosition.z
    
    'detects if the person is in an elevator
    Elevator(i50).SetCollisionEnable True
    If Elevator(i50).Collision(Camera.GetPosition, Vector(Camera.GetPosition.X, Camera.GetPosition.Y - 25, Camera.GetPosition.z), TV_TESTTYPE_ACCURATETESTING) = True Then
        If InElevator = False Then DrawElevatorButtons (i50)
        ElevatorNumber = i50
        InElevator = True
                
        'displays 3 floors of the inside shaft while the elevator's moving
        If ElevatorSync(i50) = True And OpenElevator(i50) = 0 And GotoFloor(i50) <> 0 Then
            If i50 = 2 Or i50 = 4 Or i50 = 6 Or i50 = 8 Or i50 = 10 Or i50 = 12 Or i50 = 14 Or i50 = 16 Or i50 = 18 Or i50 = 20 Then
                If ElevatorFloor(i50) > 0 Then Shafts2(ElevatorFloor(i50) - 1).Enable True
                If ElevatorFloor(i50) < TopFloor Then Shafts2(ElevatorFloor(i50) + 1).Enable True
                Shafts2(ElevatorFloor(i50)).Enable True
                If ElevatorFloor(i50) > 2 Then Shafts2(ElevatorFloor(i50) - 2).Enable False
                If ElevatorFloor(i50) < (TopFloor - 1) Then Shafts2(ElevatorFloor(i50) + 2).Enable False
            End If
            If i50 = 1 Or i50 = 3 Or i50 = 5 Or i50 = 7 Or i50 = 9 Or i50 = 11 Or i50 = 13 Or i50 = 15 Or i50 = 17 Or i50 = 19 Then
                If ElevatorFloor(i50) > 0 Then Shafts1(ElevatorFloor(i50) - 1).Enable True
                If ElevatorFloor(i50) < TopFloor Then Shafts1(ElevatorFloor(i50) + 1).Enable True
                Shafts1(ElevatorFloor(i50)).Enable True
                If ElevatorFloor(i50) > 2 Then Shafts1(ElevatorFloor(i50) - 2).Enable False
                If ElevatorFloor(i50) < (TopFloor - 1) Then Shafts1(ElevatorFloor(i50) + 2).Enable False
            End If
            If i50 = 22 Or i50 = 24 Or i50 = 26 Or i50 = 28 Or i50 = 30 Or i50 = 32 Or i50 = 34 Or i50 = 36 Or i50 = 38 Or i50 = 40 Then
                If ElevatorFloor(i50) > 0 Then Shafts4(ElevatorFloor(i50) - 1).Enable True
                If ElevatorFloor(i50) < TopFloor Then Shafts4(ElevatorFloor(i50) + 1).Enable True
                Shafts4(ElevatorFloor(i50)).Enable True
                If ElevatorFloor(i50) > 2 Then Shafts4(ElevatorFloor(i50) - 2).Enable False
                If ElevatorFloor(i50) < (TopFloor - 1) Then Shafts4(ElevatorFloor(i50) + 2).Enable False
            End If
            If i50 = 21 Or i50 = 23 Or i50 = 25 Or i50 = 27 Or i50 = 29 Or i50 = 31 Or i50 = 33 Or i50 = 35 Or i50 = 37 Or i50 = 39 Then
                If ElevatorFloor(i50) > 0 Then Shafts3(ElevatorFloor(i50) - 1).Enable True
                If ElevatorFloor(i50) < TopFloor Then Shafts3(ElevatorFloor(i50) + 1).Enable True
                Shafts3(ElevatorFloor(i50)).Enable True
                If ElevatorFloor(i50) > 2 Then Shafts3(ElevatorFloor(i50) - 2).Enable False
                If ElevatorFloor(i50) < (TopFloor - 1) Then Shafts3(ElevatorFloor(i50) + 2).Enable False
            End If
        Else
            If ElevatorFloor(i50) > 0 Then Shafts1(ElevatorFloor(i50) - 1).Enable False: Shafts2(ElevatorFloor(i50) - 1).Enable False: Shafts3(ElevatorFloor(i50) - 1).Enable False: Shafts4(ElevatorFloor(i50) - 1).Enable False
            If ElevatorFloor(i50) < TopFloor Then Shafts1(ElevatorFloor(i50) + 1).Enable False: Shafts2(ElevatorFloor(i50) + 1).Enable False: Shafts3(ElevatorFloor(i50) + 1).Enable False: Shafts4(ElevatorFloor(i50) + 1).Enable False
            Shafts1(ElevatorFloor(i50)).Enable False
            Shafts2(ElevatorFloor(i50)).Enable False
            Shafts3(ElevatorFloor(i50)).Enable False
            Shafts4(ElevatorFloor(i50)).Enable False
            If ElevatorFloor(i50) > 2 Then Shafts1(ElevatorFloor(i50) - 2).Enable False: Shafts2(ElevatorFloor(i50) - 2).Enable False: Shafts3(ElevatorFloor(i50) - 2).Enable False: Shafts4(ElevatorFloor(i50) - 2).Enable False
            If ElevatorFloor(i50) < (TopFloor - 1) Then Shafts1(ElevatorFloor(i50) + 2).Enable False: Shafts2(ElevatorFloor(i50) + 2).Enable False: Shafts3(ElevatorFloor(i50) + 2).Enable False: Shafts4(ElevatorFloor(i50) + 2).Enable False
        End If
        
        If Plaque(i50).IsMeshEnabled = False Then
            Plaque(i50).Enable True
            FloorIndicator(i50).Enable True
            Plaque(i50).SetPosition Plaque(i50).GetPosition.X, Elevator(i50).GetPosition.Y + 13, Plaque(i50).GetPosition.z
            FloorIndicator(i50).SetPosition FloorIndicator(i50).GetPosition.X, Elevator(i50).GetPosition.Y + 16, FloorIndicator(i50).GetPosition.z
        End If
    Else
    
        If Plaque(i50).IsMeshEnabled = True And ElevatorSync(ElevatorNumber) = False Then
            InElevator = False
            For j50 = -11 To 144
            Buttons(i54).ResetMesh
            Next j50
            ButtonsEnabled = False
            Plaque(i50).Enable False
            FloorIndicator(i50).Enable False
        End If
    End If
Next i50
   
'This code changes the elevator floor indicator picture
For i50 = 1 To 40
FloorIndicatorPic(i50) = Str$(ElevatorFloor(i50))
FloorIndicatorPic(i50) = "Button" + GetFloorName(ElevatorFloor(i50))
If FloorIndicatorPic(i50) <> FloorIndicatorPicOld(i50) Then
    
    'TextureFactory.DeleteTexture GetTex("FloorIndicator" + Str$(i50))
    'TextureFactory.LoadTexture App.Path + "\data\floorindicators\" + FloorIndicatorPic(i50), "FloorIndicator" + Str$(i50)
    FloorIndicator(i50).SetTexture GetTex(FloorIndicatorPic(i50))
    
End If
FloorIndicatorPicOld(i50) = FloorIndicatorPic(i50)

'Update the floor indicator
FloorIndicatorText(i50) = GetFloorName(ElevatorFloor(i50))

Next i50

'Stair doors used to speed up stairway
i50 = CameraFloor
If CameraFloor > BottomFloor Then FakeStairDoor(i50 - 1, 1).Enable True: FakeStairDoor(i50, 1).Enable False
If CameraFloor > BottomFloor Then FakeStairDoor(i50 - 1, 2).Enable True: FakeStairDoor(i50, 2).Enable False
If CameraFloor > BottomFloor Then FakeStairDoor(i50 - 1, 3).Enable True: FakeStairDoor(i50, 3).Enable False
If CameraFloor > BottomFloor Then FakeStairDoor(i50 - 1, 4).Enable True: FakeStairDoor(i50, 4).Enable False
If CameraFloor < TopFloor Then FakeStairDoor(i50 + 1, 1).Enable True: FakeStairDoor(i50, 1).Enable False
If CameraFloor < TopFloor Then FakeStairDoor(i50 + 1, 2).Enable True: FakeStairDoor(i50, 2).Enable False
If CameraFloor < TopFloor Then FakeStairDoor(i50 + 1, 3).Enable True: FakeStairDoor(i50, 3).Enable False
If CameraFloor < TopFloor Then FakeStairDoor(i50 + 1, 4).Enable True: FakeStairDoor(i50, 4).Enable False
If CameraFloor > BottomFloor + 1 Then FakeStairDoor(i50 - 2, 1).Enable False
If CameraFloor > BottomFloor + 1 Then FakeStairDoor(i50 - 2, 2).Enable False
If CameraFloor > BottomFloor + 1 Then FakeStairDoor(i50 - 2, 3).Enable False
If CameraFloor > BottomFloor + 1 Then FakeStairDoor(i50 - 2, 4).Enable False
If CameraFloor < (TopFloor - 1) Then FakeStairDoor(i50 + 2, 1).Enable False
If CameraFloor < (TopFloor - 1) Then FakeStairDoor(i50 + 2, 2).Enable False
If CameraFloor < (TopFloor - 1) Then FakeStairDoor(i50 + 2, 3).Enable False
If CameraFloor < (TopFloor - 1) Then FakeStairDoor(i50 + 2, 4).Enable False

DebugPanel.Label1.Caption = FloorIndicatorText(1)
DebugPanel.Label2.Caption = FloorIndicatorText(2)
DebugPanel.Label3.Caption = FloorIndicatorText(3)
DebugPanel.Label4.Caption = FloorIndicatorText(4)
DebugPanel.Label5.Caption = FloorIndicatorText(5)
DebugPanel.Label6.Caption = FloorIndicatorText(6)
DebugPanel.Label7.Caption = FloorIndicatorText(7)
DebugPanel.Label8.Caption = FloorIndicatorText(8)
DebugPanel.Label9.Caption = FloorIndicatorText(9)
DebugPanel.Label10.Caption = FloorIndicatorText(10)
DebugPanel.Label11.Caption = FloorIndicatorText(11)
DebugPanel.Label12.Caption = FloorIndicatorText(12)
DebugPanel.Label13.Caption = FloorIndicatorText(13)
DebugPanel.Label14.Caption = FloorIndicatorText(14)
DebugPanel.Label15.Caption = FloorIndicatorText(15)
DebugPanel.Label16.Caption = FloorIndicatorText(16)
DebugPanel.Label17.Caption = FloorIndicatorText(17)
DebugPanel.Label18.Caption = FloorIndicatorText(18)
DebugPanel.Label19.Caption = FloorIndicatorText(19)
DebugPanel.Label20.Caption = FloorIndicatorText(20)
DebugPanel.Label21.Caption = FloorIndicatorText(21)
DebugPanel.Label22.Caption = FloorIndicatorText(22)
DebugPanel.Label23.Caption = FloorIndicatorText(23)
DebugPanel.Label24.Caption = FloorIndicatorText(24)
DebugPanel.Label25.Caption = FloorIndicatorText(25)
DebugPanel.Label26.Caption = FloorIndicatorText(26)
DebugPanel.Label27.Caption = FloorIndicatorText(27)
DebugPanel.Label28.Caption = FloorIndicatorText(28)
DebugPanel.Label29.Caption = FloorIndicatorText(29)
DebugPanel.Label30.Caption = FloorIndicatorText(30)
DebugPanel.Label31.Caption = FloorIndicatorText(31)
DebugPanel.Label32.Caption = FloorIndicatorText(32)
DebugPanel.Label33.Caption = FloorIndicatorText(33)
DebugPanel.Label34.Caption = FloorIndicatorText(34)
DebugPanel.Label35.Caption = FloorIndicatorText(35)
DebugPanel.Label36.Caption = FloorIndicatorText(36)
DebugPanel.Label37.Caption = FloorIndicatorText(37)
DebugPanel.Label38.Caption = FloorIndicatorText(38)
DebugPanel.Label39.Caption = FloorIndicatorText(39)
DebugPanel.Label40.Caption = FloorIndicatorText(40)
   

Dim A As Single
''update lights
'a = a + TV.TimeElapsed * 0.001
'      LightD.Position = Vector(0, 10, Sin(a) * 50 + 50)
'      Light.UpdateLight 1, LightD
      
      
'this determines if the person is inside the stairwell shaft or not, and sets a variable accordingly.
If Camera.GetPosition.X < -12.5 And Camera.GetPosition.X > -32.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 Then
InStairwell = True
Else
If Camera.GetPosition.X < 52.5 And Camera.GetPosition.X > 32.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 Then
InStairwell = True
Else
If Camera.GetPosition.X < -90.5 And Camera.GetPosition.X > -110.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 Then
InStairwell = True
Else
If Camera.GetPosition.X < 130.5 And Camera.GetPosition.X > 110.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 Then
InStairwell = True
Else
InStairwell = False
End If
End If
End If
End If

If CameraFloor = 137 Then
For i50 = BottomFloor To TopFloor
Shafts2(i50).Enable True
Next i50
Else
'For i50 = 1 To TopFloor
'Shafts(i50).Enable False
'Next i50
End If

'floors 135 and 136 are combined. this simply makes sure that they are off when not in use :)

If CameraFloor <> 136 And CameraFloor <> 135 Then
Room(136).Enable False
'ElevatorDoor1L(136).Enable False
'ElevatorDoor1R(136).Enable False
'ElevatorDoor1L(135).Enable False
'ElevatorDoor1R(135).Enable False
'ElevatorDoor2L(136).Enable False
'ElevatorDoor2R(136).Enable False
'ElevatorDoor2L(135).Enable False
'ElevatorDoor2R(135).Enable False
'ElevatorDoorL(1).Enable False
'ElevatorDoorL(2).Enable False
'ElevatorDoorL(3).Enable False
'ElevatorDoorL(4).Enable False
'ElevatorDoorR(1).Enable False
'ElevatorDoorR(2).Enable False
'ElevatorDoorR(3).Enable False
'ElevatorDoorR(4).Enable False
Room(135).Enable False
ShaftsFloor(135).Enable False
ShaftsFloor(136).Enable False
Else
If CameraFloor = 136 Or CameraFloor = 135 Then
    If GotoFloor(ElevatorNumber) = 0 And InStairwell = False Then
    Room(136).Enable True
    'ElevatorDoor1L(136).Enable True
    'ElevatorDoor1R(136).Enable True
    'ElevatorDoor1L(135).Enable True
    'ElevatorDoor1R(135).Enable True
    'ElevatorDoor2L(136).Enable True
    'ElevatorDoor2R(136).Enable True
    'ElevatorDoor2L(135).Enable True
    'ElevatorDoor2R(135).Enable True
    'ElevatorDoorL(1).Enable True
    'ElevatorDoorL(2).Enable True
    'ElevatorDoorL(3).Enable True
    'ElevatorDoorL(4).Enable True
    'ElevatorDoorR(1).Enable True
    'ElevatorDoorR(2).Enable True
    'ElevatorDoorR(3).Enable True
    'ElevatorDoorR(4).Enable True
    Room(135).Enable True
    ShaftsFloor(135).Enable True
    ShaftsFloor(136).Enable True
    End If
End If
End If

'This section turns on and off the external mesh (outside windows, not inside windows), based on the camera's current location
If CameraFloor >= 0 And CameraFloor <= 39 Then
    If Camera.GetPosition.X < -160 Or Camera.GetPosition.X > 160 Or Camera.GetPosition.z < -150 Or Camera.GetPosition.z > 150 Then
    If External.IsMeshEnabled = False Then
        External.Enable True
        'Buildings.Enable True
        Room(CameraFloor).Enable False
        CrawlSpace(CameraFloor).Enable False
        ShaftsFloor(CameraFloor).Enable False
        DestroyObjects (CameraFloor)
    End If
    Else
    If External.IsMeshEnabled = True Then
        External.Enable False
        'Buildings.Enable False
        Room(CameraFloor).Enable True
        CrawlSpace(CameraFloor).Enable True
        ShaftsFloor(CameraFloor).Enable True
        Call InitRealtime(CameraFloor)
        InitObjectsForFloor (CameraFloor)
    End If
    End If
End If
If CameraFloor >= 40 And CameraFloor <= 79 Then
    If Camera.GetPosition.X < -135 Or Camera.GetPosition.X > 135 Or Camera.GetPosition.z < -150 Or Camera.GetPosition.z > 150 Then
    If External.IsMeshEnabled = False Then
        External.Enable True
        'Buildings.Enable True
        Room(CameraFloor).Enable False
        CrawlSpace(CameraFloor).Enable False
        ShaftsFloor(CameraFloor).Enable False
        DestroyObjects (CameraFloor)
    End If
    Else
    If External.IsMeshEnabled = True Then
        External.Enable False
        'Buildings.Enable False
        Room(CameraFloor).Enable True
        CrawlSpace(CameraFloor).Enable True
        ShaftsFloor(CameraFloor).Enable True
        Call InitRealtime(CameraFloor)
        InitObjectsForFloor (CameraFloor)
    End If
   End If
End If
If CameraFloor >= 80 And CameraFloor <= 117 Then
    If Camera.GetPosition.X < -110 Or Camera.GetPosition.X > 110 Or Camera.GetPosition.z < -150 Or Camera.GetPosition.z > 150 Then
    If External.IsMeshEnabled = False Then
        External.Enable True
        'Buildings.Enable True
        Room(CameraFloor).Enable False
        CrawlSpace(CameraFloor).Enable False
        ShaftsFloor(CameraFloor).Enable False
        DestroyObjects (CameraFloor)
    End If
    Else
    If External.IsMeshEnabled = True Then
        External.Enable False
        'Buildings.Enable False
        Room(CameraFloor).Enable True
        CrawlSpace(CameraFloor).Enable True
        ShaftsFloor(CameraFloor).Enable True
        Call InitRealtime(CameraFloor)
        InitObjectsForFloor (CameraFloor)
    End If
    End If
End If
If CameraFloor >= 118 And CameraFloor <= 134 Then
    If Camera.GetPosition.X < -85 Or Camera.GetPosition.X > 85 Or Camera.GetPosition.z < -150 Or Camera.GetPosition.z > 150 Then
    If External.IsMeshEnabled = False Then
        External.Enable True
        'Buildings.Enable True
        Room(CameraFloor).Enable False
        CrawlSpace(CameraFloor).Enable False
        ShaftsFloor(CameraFloor).Enable False
        DestroyObjects (CameraFloor)
    End If
    Else
    If External.IsMeshEnabled = True Then
        External.Enable False
        'Buildings.Enable False
        Room(CameraFloor).Enable True
        CrawlSpace(CameraFloor).Enable True
        ShaftsFloor(CameraFloor).Enable True
        Call InitRealtime(CameraFloor)
        InitObjectsForFloor (CameraFloor)
    End If
    End If
End If
If CameraFloor >= 135 And CameraFloor <= 138 Then
    If Camera.GetPosition.X < -60 Or Camera.GetPosition.X > 60 Or Camera.GetPosition.z < -150 Or Camera.GetPosition.z > 150 Then
    If External.IsMeshEnabled = False Then
        External.Enable True
        'Buildings.Enable True
        Room(CameraFloor).Enable False
        CrawlSpace(CameraFloor).Enable False
        ShaftsFloor(CameraFloor).Enable False
        DestroyObjects (CameraFloor)
    End If
    Else
    If External.IsMeshEnabled = True Then
        External.Enable False
        'Buildings.Enable False
        Room(CameraFloor).Enable True
        CrawlSpace(CameraFloor).Enable True
        ShaftsFloor(CameraFloor).Enable True
        Call InitRealtime(CameraFloor)
        InitObjectsForFloor (CameraFloor)
    End If
    End If
End If

'Calls the Fall sub, and if IsFalling is true then the user falls until they hit something
If EnableCollisions = True Then Call Fall

''This section turns on and off the Shafts mesh (inside the elevator and pipe shafts) when the camera is located inside them.

If CameraFloor = 137 Then GoTo EndShafts
If GotoFloor(ElevatorNumber) <> 0 Then GoTo EndShafts

'if user is in service room, turn on shaft
If CameraFloor >= 118 And CameraFloor <= 129 Then
If Camera.GetPosition.X < 50 And Camera.GetPosition.X > 32.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -20 And CameraFloor <> 1 And CameraFloor <> 132 And CameraFloor < 134 Then
For i50 = BottomFloor To TopFloor
Shafts2(i50).Enable True
Next i50
GoTo EndShafts
End If
'if user is outside service room, turn off shaft
If Camera.GetPosition.X < 50 And Camera.GetPosition.X > 32.5 And Camera.GetPosition.z < -46.25 And CameraFloor <> 1 And CameraFloor <> 132 And CameraFloor < 134 Then
For i50 = BottomFloor To TopFloor
Shafts2(i50).Enable False
Next i50
GoTo EndShafts
End If
End If

'Automatic Shaft Enable/Disable
   
If DebugPanel.Check12.Value = 0 Then GoTo EndShafts

    Dim SectionNum As Single
    If InStairwell = True Then GoTo EndShafts
    
    If CameraFloor >= BottomFloor And CameraFloor <= 39 Then SectionNum = 1
    If CameraFloor >= 40 And CameraFloor <= 79 Then SectionNum = 2
    If CameraFloor >= 80 And CameraFloor <= 117 Then SectionNum = 3
    If CameraFloor >= 118 And CameraFloor <= 130 Then SectionNum = 4
    If CameraFloor >= 131 And CameraFloor <= 136 Then SectionNum = 5
    If CameraFloor >= 137 Then SectionNum = 6
    
'Pipe Shafts
    If Camera.GetPosition.X > 12.5 And Camera.GetPosition.X < 32.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 And InElevator = False Then
        PipeShaft(1).Enable True
        If CameraFloor > -9 Then CrawlSpace(CameraFloor - 2).Enable False
        If CameraFloor < (TopFloor - 1) Then CrawlSpace(CameraFloor + 2).Enable False
        CrawlSpace(CameraFloor).Enable True
        If CameraFloor > BottomFloor Then CrawlSpace(CameraFloor - 1).Enable True
        If CameraFloor < TopFloor Then CrawlSpace(CameraFloor + 1).Enable True
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Shafts3(i50).Enable False
        Shafts4(i50).Enable False
        If i50 > BottomFloor And i50 < TopFloor And i50 <> CameraFloor And i50 <> CameraFloor - 1 And i50 <> CameraFloor + 1 Then CrawlSpaceShaft1(i50).Enable True
        CrawlSpaceShaft1(CameraFloor).Enable False
        If i50 > BottomFloor Then CrawlSpaceShaft1(CameraFloor - 1).Enable False
        If i50 < TopFloor Then CrawlSpaceShaft1(CameraFloor + 1).Enable False
        Next i50
        GoTo EndShafts
    End If
    If Camera.GetPosition.X > -52.5 And Camera.GetPosition.X < -32.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 And InElevator = False And SectionNum <= 3 Then
        PipeShaft(2).Enable True
        If CameraFloor > -9 Then CrawlSpace(CameraFloor - 2).Enable False
        If CameraFloor < (TopFloor - 1) Then CrawlSpace(CameraFloor + 2).Enable False
        CrawlSpace(CameraFloor).Enable True
        If CameraFloor > BottomFloor Then CrawlSpace(CameraFloor - 1).Enable True
        If CameraFloor < TopFloor Then CrawlSpace(CameraFloor + 1).Enable True
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Shafts3(i50).Enable False
        Shafts4(i50).Enable False
        If i50 > BottomFloor And i50 < TopFloor And i50 <> CameraFloor And i50 <> CameraFloor - 1 And i50 <> CameraFloor + 1 Then CrawlSpaceShaft2(i50).Enable True
        CrawlSpaceShaft2(CameraFloor).Enable False
        If i50 > BottomFloor Then CrawlSpaceShaft2(CameraFloor - 1).Enable False
        If i50 < TopFloor Then CrawlSpaceShaft2(CameraFloor + 1).Enable False
        Next i50
        GoTo EndShafts
    End If
    If Camera.GetPosition.X > 90.5 And Camera.GetPosition.X < 110.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 And InElevator = False And (SectionNum <= 2 Or CameraFloor = 80) Then
        PipeShaft(3).Enable True
        If CameraFloor > -9 Then CrawlSpace(CameraFloor - 2).Enable False
        If CameraFloor < (TopFloor - 1) Then CrawlSpace(CameraFloor + 2).Enable False
        CrawlSpace(CameraFloor).Enable True
        If CameraFloor > BottomFloor Then CrawlSpace(CameraFloor - 1).Enable True
        If CameraFloor < TopFloor Then CrawlSpace(CameraFloor + 1).Enable True
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Shafts3(i50).Enable False
        Shafts4(i50).Enable False
        If i50 > BottomFloor And i50 < TopFloor And i50 <> CameraFloor And i50 <> CameraFloor - 1 And i50 <> CameraFloor + 1 Then CrawlSpaceShaft3(i50).Enable True
        CrawlSpaceShaft3(CameraFloor).Enable False
        If i50 > BottomFloor Then CrawlSpaceShaft3(CameraFloor - 1).Enable False
        If i50 < TopFloor Then CrawlSpaceShaft3(CameraFloor + 1).Enable False
        Next i50
        GoTo EndShafts
    End If
    If Camera.GetPosition.X > -130.5 And Camera.GetPosition.X < -110.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30 And InElevator = False And (SectionNum = 1 Or CameraFloor = 40) Then
        PipeShaft(4).Enable True
        If CameraFloor > -9 Then CrawlSpace(CameraFloor - 2).Enable False
        If CameraFloor < (TopFloor - 1) Then CrawlSpace(CameraFloor + 2).Enable False
        CrawlSpace(CameraFloor).Enable True
        If CameraFloor > BottomFloor Then CrawlSpace(CameraFloor - 1).Enable True
        If CameraFloor < TopFloor Then CrawlSpace(CameraFloor + 1).Enable True
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Shafts3(i50).Enable False
        Shafts4(i50).Enable False
        If i50 > BottomFloor And i50 < TopFloor And i50 <> CameraFloor And i50 <> CameraFloor - 1 And i50 <> CameraFloor + 1 Then CrawlSpaceShaft4(i50).Enable True
        CrawlSpaceShaft4(CameraFloor).Enable False
        If i50 > BottomFloor Then CrawlSpaceShaft4(CameraFloor - 1).Enable False
        If i50 < TopFloor Then CrawlSpaceShaft4(CameraFloor + 1).Enable False
        Next i50
        GoTo EndShafts
    End If
    
    If PipeShaft(1).IsMeshEnabled = True Or PipeShaft(2).IsMeshEnabled = True Or PipeShaft(3).IsMeshEnabled = True Or PipeShaft(4).IsMeshEnabled = True Then
        'PipeShaft(1).Enable False
        'PipeShaft(2).Enable False
        'PipeShaft(3).Enable False
        'PipeShaft(4).Enable False
        If CameraFloor > -9 Then CrawlSpace(CameraFloor - 2).Enable False
        If CameraFloor < (TopFloor - 1) Then CrawlSpace(CameraFloor + 2).Enable False
        'If CameraFloor > BottomFloor Then CrawlSpace(CameraFloor - 1).Enable False
        If CameraFloor < TopFloor Then CrawlSpace(CameraFloor + 1).Enable False
        For i50 = BottomFloor To TopFloor
        CrawlSpaceShaft1(i50).Enable False
        CrawlSpaceShaft2(i50).Enable False
        CrawlSpaceShaft3(i50).Enable False
        CrawlSpaceShaft4(i50).Enable False
        Next i50
    End If
         
'Main Shafts
If SectionNum = 1 Then
    
    'right shaft (the one with the stairs)
    If Camera.GetPosition.X > -52.5 And Camera.GetPosition.X < -12.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 0 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts1(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    'left shaft (the one with the pipe shaft)
    If Camera.GetPosition.X > 12.5 And Camera.GetPosition.X < 52.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 0 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts2(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Camera.GetPosition.X > -130.5 And Camera.GetPosition.X < -90.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts3(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Camera.GetPosition.X > 90.5 And Camera.GetPosition.X < 130.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts4(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Shafts1(CameraFloor).IsMeshEnabled = True Or Shafts2(CameraFloor).IsMeshEnabled = True Or Shafts3(CameraFloor).IsMeshEnabled = True Or Shafts4(CameraFloor).IsMeshEnabled = True Then
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Shafts3(i50).Enable False
        Shafts4(i50).Enable False
        Next i50
    End If
End If

If SectionNum = 2 Then
    If Camera.GetPosition.X > -52.5 And Camera.GetPosition.X < -12.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 0 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts1(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Camera.GetPosition.X > 12.5 And Camera.GetPosition.X < 52.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 0 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts2(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Camera.GetPosition.X > -110.5 And Camera.GetPosition.X < -90.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts3(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Camera.GetPosition.X > 90.5 And Camera.GetPosition.X < 110.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts4(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Shafts1(CameraFloor).IsMeshEnabled = True Or Shafts2(CameraFloor).IsMeshEnabled = True Or Shafts3(CameraFloor).IsMeshEnabled = True Or Shafts4(CameraFloor).IsMeshEnabled = True Then
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Shafts3(i50).Enable False
        Shafts4(i50).Enable False
        Next i50
    End If
End If

If SectionNum = 3 Then
    If Camera.GetPosition.X > -52.5 And Camera.GetPosition.X < -12.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts1(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    'left shaft (the one with the pipe shaft)
    If Camera.GetPosition.X > 12.5 And Camera.GetPosition.X < 52.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts2(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Shafts1(CameraFloor).IsMeshEnabled = True Or Shafts2(CameraFloor).IsMeshEnabled = True Then
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Next i50
    End If
End If

If SectionNum = 4 Then
    If Camera.GetPosition.X > -32.5 And Camera.GetPosition.X < -12.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts1(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    'left shaft (the one with the pipe shaft)
    If Camera.GetPosition.X > 12.5 And Camera.GetPosition.X < 32.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 46.25 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts2(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Shafts1(CameraFloor).IsMeshEnabled = True Or Shafts2(CameraFloor).IsMeshEnabled = True Then
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Next i50
    End If
End If

If SectionNum = 5 Then
    If Camera.GetPosition.X > -32.5 And Camera.GetPosition.X < -12.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 0 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts1(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Camera.GetPosition.X > 12.5 And Camera.GetPosition.X < 32.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < 0 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts2(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Shafts1(CameraFloor).IsMeshEnabled = True Or Shafts2(CameraFloor).IsMeshEnabled = True Then
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Shafts2(i50).Enable False
        Next i50
    End If
End If

If SectionNum = 6 Then
    If Camera.GetPosition.X > -32.5 And Camera.GetPosition.X < -12.5 And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -15.42 And InElevator = False Then
    For i50 = BottomFloor To TopFloor
    Shafts1(i50).Enable True
    Next i50
    GoTo EndShafts
    End If
    
    If Shafts1(CameraFloor).IsMeshEnabled = True Then
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).Enable False
        Next i50
    End If
End If

EndShafts:

    linestart = Camera.GetPosition

 Inp.GetAbsMouseState tmpMouseX, tmpMouseY, tmpMouseB1

'Click
 If tmpMouseB1 <> 0 Then
        'If FloorIndicator1.Collision=True
        
        For i50 = 1 To 4
        StairDoor(i50).SetCollisionEnable True
        Next i50
        
        Room(CameraFloor).SetCollisionEnable False
        CrawlSpace(CameraFloor).SetCollisionEnable False
        External.SetCollisionEnable False
        Buildings.SetCollisionEnable False
        Landscape.SetCollisionEnable False
        For i50 = BottomFloor To TopFloor
        Shafts1(i50).SetCollisionEnable False
        Shafts2(i50).SetCollisionEnable False
        Shafts3(i50).SetCollisionEnable False
        Shafts4(i50).SetCollisionEnable False
        Next i50
        ShaftsFloor(CameraFloor).SetCollisionEnable False
        For i50 = 1 To 40
        Elevator(i50).SetCollisionEnable False
        ElevatorInsDoorL(i50).SetCollisionEnable False
        ElevatorInsDoorR(i50).SetCollisionEnable False
        Next i50
        For i50 = 1 To 40
        ElevatorDoorL(i50).SetCollisionEnable False
        ElevatorDoorR(i50).SetCollisionEnable False
        Next i50
        Stairs(CameraFloor).SetCollisionEnable False
        If CameraFloor > BottomFloor Then Stairs(CameraFloor - 1).SetCollisionEnable False
        If CameraFloor < TopFloor Then Stairs(CameraFloor + 1).SetCollisionEnable False
        If CameraFloor = 135 Then Room(136).SetCollisionEnable False
        If CameraFloor = 136 Then Room(135).SetCollisionEnable False
        
        Set CollisionResult = Scene.MousePicking(tmpMouseX, tmpMouseY, TV_COLLIDE_MESH, TV_TESTTYPE_ACCURATETESTING)
         
        
        If CollisionResult.IsCollision Then
        Call CheckElevatorButtons
            For i50 = 1 To 40
            Dim CallElevatorTemp As Boolean
            Dim Direction As Integer
                
            If CollisionResult.GetCollisionMesh.GetMeshName = CallButtonsUp(i50).GetMeshName Then CallButtonsUp(i50).SetColor RGBA(1, 1, 0, 1): CallElevatorTemp = True: Direction = 1
            If CollisionResult.GetCollisionMesh.GetMeshName = CallButtonsDown(i50).GetMeshName Then CallButtonsDown(i50).SetColor RGBA(1, 1, 0, 1): CallElevatorTemp = True: Direction = -1
            
            If CallElevatorTemp = True Then
                
                CallElevatorTemp = False
                
                'Elevator Sections:
                'Section Num - Elev Nums
                '1 - 1
                '2 - 2,3,4
                '3 - 5,6,7,8,9,10
                '4 - 11
                '5 - 12
                '6 - 13
                '7 - 14
                '8 - 15,17,19
                '9 - 16,18,20
                '10 - 21,23,25,27,29
                '11 - 22,24,26,28,30
                '12 - 31,33,35,37,39
                '13 - 32,34,36,38,40
                
                j50 = i50
                Dim Section As Integer
                
                'Get section number
                If j50 = 1 Then Section = 1
                If j50 >= 2 And j50 <= 4 Then Section = 2
                If j50 >= 5 And j50 <= 10 Then Section = 3
                If j50 = 11 Then Section = 4
                If j50 = 12 Then Section = 5
                If j50 = 13 Then Section = 6
                If j50 = 14 Then Section = 7
                If j50 = 15 Or j50 = 17 Or j50 = 19 Then Section = 8
                If j50 = 16 Or j50 = 18 Or j50 = 20 Then Section = 9
                If j50 = 21 Or j50 = 23 Or j50 = 25 Or j50 = 27 Or j50 = 29 Then Section = 10
                If j50 = 22 Or j50 = 24 Or j50 = 26 Or j50 = 28 Or j50 = 30 Then Section = 11
                If j50 = 31 Or j50 = 33 Or j50 = 35 Or j50 = 37 Or j50 = 39 Then Section = 12
                If j50 = 32 Or j50 = 34 Or j50 = 36 Or j50 = 38 Or j50 = 40 Then Section = 13
                
                Call CallElevator(CameraFloor, Section, Direction)
            
            End If
EndCall:
            Next i50
            'CollisionResult.GetCollisionMesh.Enable False
        
        SelectedObject = CollisionResult.GetCollisionMesh.GetMeshName
        
        If OpeningDoor = 0 And ClosingDoor = 0 Then
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 7) = "DoorSB " Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 7, 20))
            DoorRotated = 0
            Call OpenStairDoor
            End If
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorA " Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 6, 20))
            DoorRotated = 0
            Call OpenDoor
            End If
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorB " Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 6, 20))
            DoorRotated = 1
            Call OpenDoor
            End If
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorC " Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 6, 20))
            DoorRotated = 2
            Call OpenDoor
            End If
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorD " Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 6, 20))
            DoorRotated = 3
            Call OpenDoor
            End If
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 7) = "DoorSBO" Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 8, 20))
            DoorRotated = 0
            ClosingDoor = 18
            Call CloseStairDoor
            End If
        
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorAO" Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 7, 20))
            DoorRotated = 0
            ClosingDoor = 18
            Call CloseDoor
            End If
        
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorBO" Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 7, 20))
            DoorRotated = 1
            ClosingDoor = 18
            Call CloseDoor
            End If
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorCO" Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 7, 20))
            DoorRotated = 2
            ClosingDoor = 18
            Call CloseDoor
            End If
            
            If Left(CollisionResult.GetCollisionMesh.GetMeshName, 6) = "DoorDO" Then
            DoorNumber = Val(Mid$(CollisionResult.GetCollisionMesh.GetMeshName, 7, 20))
            DoorRotated = 3
            ClosingDoor = 18
            Call CloseDoor
            End If
        
        End If
            
            End If
    End If



    'Synchronize the listener with the camera
    'Listener.SetPosition Camera.GetPosition.X / 100, Camera.GetPosition.Y / 100, Camera.GetPosition.z / 100
    Listener.SetPosition Camera.GetPosition.X / SoundDivisor, Camera.GetPosition.Y / SoundDivisor, Camera.GetPosition.z / SoundDivisor
    'ElevatorMusic.GetPosition ListenerDirection.X, ListenerDirection.Y, ListenerDirection.z
    
    'Set the listener orientation to the camera's rotational matrix
    Dim matCamera As D3DMATRIX
    matCamera = Camera.GetRotationMatrix
    Listener.SetOrientation matCamera.m31, matCamera.m32, matCamera.m33, matCamera.m21, matCamera.m22, matCamera.m23
        
      
'*** First movement system
InputOnly:
      
      'If Inp.IsKeyPressed(TV_KEY_UP) = True And Focused = True Then
      If Inp.IsKeyPressed(TV_KEY_UP) = True Then
      KeepAltitude = Camera.GetPosition.Y
      If Inp.IsKeyPressed(TV_KEY_Z) = False Then Camera.MoveRelative 0.7, 0, 0
      If Inp.IsKeyPressed(TV_KEY_Z) = True Then Camera.MoveRelative 1.4, 0, 0
      If Camera.GetPosition.Y <> KeepAltitude Then Camera.SetPosition Camera.GetPosition.X, KeepAltitude, Camera.GetPosition.z
      End If
      
      'If Inp.IsKeyPressed(TV_KEY_DOWN) = True And Focused = True Then
      If Inp.IsKeyPressed(TV_KEY_DOWN) = True Then
      KeepAltitude = Camera.GetPosition.Y
      If Inp.IsKeyPressed(TV_KEY_Z) = False Then Camera.MoveRelative -0.7, 0, 0
      If Inp.IsKeyPressed(TV_KEY_Z) = True Then Camera.MoveRelative -1.4, 0, 0
      If Camera.GetPosition.Y <> KeepAltitude Then Camera.SetPosition Camera.GetPosition.X, KeepAltitude, Camera.GetPosition.z
      End If
      
      'If Inp.IsKeyPressed(TV_KEY_RIGHT) = True And Focused = True Then Camera.RotateY 0.07
      'If Inp.IsKeyPressed(TV_KEY_LEFT) = True And Focused = True Then Camera.RotateY -0.07
      If Inp.IsKeyPressed(TV_KEY_RIGHT) = True And Inp.IsKeyPressed(TV_KEY_Z) = False Then Camera.RotateY 0.07
      If Inp.IsKeyPressed(TV_KEY_LEFT) = True And Inp.IsKeyPressed(TV_KEY_Z) = False Then Camera.RotateY -0.07
      If Inp.IsKeyPressed(TV_KEY_RIGHT) = True And Inp.IsKeyPressed(TV_KEY_Z) = True Then Camera.RotateY 0.14
      If Inp.IsKeyPressed(TV_KEY_LEFT) = True And Inp.IsKeyPressed(TV_KEY_Z) = True Then Camera.RotateY -0.14
      'If Inp.IsKeyPressed(TV_KEY_PAGEUP) = True And Focused = True Then Camera.RotateX -0.006
      'If Inp.IsKeyPressed(TV_KEY_PAGEDOWN) = True And Focused = True Then Camera.RotateX 0.006
      If Inp.IsKeyPressed(TV_KEY_PAGEUP) = True And Inp.IsKeyPressed(TV_KEY_Z) = False Then Camera.RotateX -0.006
      If Inp.IsKeyPressed(TV_KEY_PAGEUP) = True And Inp.IsKeyPressed(TV_KEY_Z) = True Then Camera.RotateX -0.012
      If Inp.IsKeyPressed(TV_KEY_PAGEDOWN) = True And Inp.IsKeyPressed(TV_KEY_Z) = False Then Camera.RotateX 0.006
      If Inp.IsKeyPressed(TV_KEY_PAGEDOWN) = True And Inp.IsKeyPressed(TV_KEY_Z) = True Then Camera.RotateX 0.012
      
      'If Inp.IsKeyPressed(TV_KEY_HOME) = True And Focused = True Then Camera.MoveRelative 0, 1, 0
      'If Inp.IsKeyPressed(TV_KEY_END) = True And Focused = True Then Camera.MoveRelative 0, -1, 0
      If Inp.IsKeyPressed(TV_KEY_HOME) = True And Inp.IsKeyPressed(TV_KEY_Z) = False Then Camera.MoveRelative 0, 1, 0
      If Inp.IsKeyPressed(TV_KEY_HOME) = True And Inp.IsKeyPressed(TV_KEY_Z) = True Then Camera.MoveRelative 0, 2, 0
      If Inp.IsKeyPressed(TV_KEY_END) = True And Inp.IsKeyPressed(TV_KEY_Z) = False And EnableCollisions = False Then Camera.MoveRelative 0, -1, 0
      If Inp.IsKeyPressed(TV_KEY_END) = True And Inp.IsKeyPressed(TV_KEY_Z) = True And EnableCollisions = False Then Camera.MoveRelative 0, -2, 0
      'If Inp.IsKeyPressed(TV_KEY_1) = True And Focused = True Then ElevatorDirection = 1
      'If Inp.IsKeyPressed(TV_KEY_2) = True And Focused = True Then ElevatorDirection = -1
      'If Inp.IsKeyPressed(TV_KEY_3) = True And Focused = True Then OpenElevator(ElevatorNumber) = 1
      'If Inp.IsKeyPressed(TV_KEY_4) = True And Focused = True Then OpenElevator(ElevatorNumber) = -1
      'If Inp.IsKeyPressed(TV_KEY_5) = True And Focused = True Then Call ElevatorMusic.Play
      'If Inp.IsKeyPressed(TV_KEY_6) = True And Focused = True Then Call ElevatorMusic.Stop_
      If Inp.IsKeyPressed(TV_KEY_SPACE) = True Then Camera.SetRotation 0, 0, 0
      'If Inp.IsKeyPressed(TV_KEY_6) = True Then MsgBox (Str$(Camera.GetLookAt.X) + Str$(Camera.GetLookAt.Y) + Str$(Camera.GetLookAt.z))
      If Inp.IsKeyPressed(TV_KEY_7) = True Then IsFalling = True
      
      If Inp.IsKeyPressed(TV_KEY_F1) = True And Focused = True Then TV.ScreenShot (App.Path + "\shot.bmp")

      
DebugPanel.Text1.Text = "Elevator Number= " + Str$(ElevatorNumber) + vbCrLf + "Elevator Floor=" + Str$(ElevatorFloor(ElevatorNumber)) + vbCrLf + "Camera Floor=" + Str$(CameraFloor) + vbCrLf + "Current Location= " + Str$(Int(Camera.GetPosition.X)) + "," + Str$(Int(Camera.GetPosition.Y)) + "," + Str$(Int(Camera.GetPosition.z)) + vbCrLf + "GotoFloor=" + Str$(GotoFloor(ElevatorNumber)) + vbCrLf + "DistancetoDest=" + Str$(Abs(GotoFloor(ElevatorNumber) - CurrentFloor(ElevatorNumber))) + vbCrLf + "Rate=" + Str$(ElevatorEnable(ElevatorNumber) / 5) + vbCrLf + "Selected Object=" + SelectedObject
             
      If InStairwell = False Then CameraFloor = GetCameraFloor
      FloorHeight = GetFloorHeight(CameraFloor)
      FloorAltitude = GetFloorAltitude(CameraFloor)

      lineend = Camera.GetPosition
          
If EnableCollisions = True Then Call CheckCollisions
  
Render:
    If IntroOn = True Then Call Intro
    
    'On Error Resume Next
    TV.Clear
    Atmos.Atmosphere_Render
    Scene.RenderAllMeshes
    TV.RenderToScreen
    DoEvents

End Sub

Sub OpenDoor()
OpeningDoor = OpeningDoor + 1

'DoorRotated values:
'0 is horizontal, opens downward
'1 is vertical, opens to the left
'2 is horizontal, opens upward
'3 is vertical, opens to the right
On Error Resume Next

Objects(DoorNumber).RotateY (OpeningDoor / 110)
If DoorRotated = 0 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X + (OpeningDoor / 48), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z + (OpeningDoor / 38)
If DoorRotated = 1 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X + (OpeningDoor / 38), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z - (OpeningDoor / 48)
If DoorRotated = 2 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X - (OpeningDoor / 48), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z - (OpeningDoor / 38)
If DoorRotated = 3 Then Objects(DoorNumber).SetPosition Objects(DoorNumber).GetPosition.X - (OpeningDoor / 38), Objects(DoorNumber).GetPosition.Y, Objects(DoorNumber).GetPosition.z + (OpeningDoor / 48)

If OpeningDoor = 17 Then
    OpeningDoor = 0
    If DoorRotated = 0 Then Objects(DoorNumber).SetMeshName ("DoorAO " + Str$(DoorNumber))
    If DoorRotated = 1 Then Objects(DoorNumber).SetMeshName ("DoorBO " + Str$(DoorNumber))
    If DoorRotated = 2 Then Objects(DoorNumber).SetMeshName ("DoorCO " + Str$(DoorNumber))
    If DoorRotated = 3 Then Objects(DoorNumber).SetMeshName ("DoorDO " + Str$(DoorNumber))
End If
End Sub

Sub SlowToFPS(ByVal FrameRate As Long)
Dim lngTicksPerFrame As Long
Static lngOldTickCount As Long
lngTicksPerFrame = 1000 / FrameRate
While GetTickCount() < lngOldTickCount
Sleep 5
Wend
lngOldTickCount = GetTickCount() + lngTicksPerFrame
End Sub

Sub StairsLoop()
If DebugPanel.Check11.Value = 0 Then Exit Sub

    Dim RiserHeight As Single
    RiserHeight = 2
    Dim ShaftNum As Integer
    Dim ShaftLeft As Single
    Dim ShaftRight As Single
    
    Dim FloorHeight As Single
    Dim FloorAltitude As Single
    FloorHeight = GetFloorHeight(CameraFloor)
    FloorAltitude = GetFloorAltitude(CameraFloor)


For ShaftNum = 1 To 4
    If ShaftNum = 1 Then ShaftLeft = 12.5: ShaftRight = 32.5
    If ShaftNum = 2 Then ShaftLeft = -52.5: ShaftRight = -32.5
    If ShaftNum = 3 Then ShaftLeft = 90.5: ShaftRight = 110.5
    If ShaftNum = 4 Then ShaftLeft = -130.5: ShaftRight = -110.5
    
'Stairs Movement
    
    'Go up a floor
    If Camera.GetPosition.X <= -ShaftLeft And Camera.GetPosition.X > -(ShaftLeft + 6) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30.85 And Camera.GetPosition.Y = FloorAltitude + 10 + (RiserHeight * 15) Then
        Room(CameraFloor).Enable False
        CrawlSpace(CameraFloor).Enable False
        For i51 = 1 To 40
            ElevatorDoorL(i51).Enable False
            ElevatorDoorR(i51).Enable False
        Next i51
        For i51 = 1 To 40
            CallButtonsUp(i51).Enable False
            CallButtonsDown(i51).Enable False
        Next i51
        ShaftsFloor(CameraFloor).Enable False
        Atmos.SkyBox_Enable False
        Buildings.Enable False
        Landscape.Enable False
        DestroyObjects (CameraFloor)
      
        'If CameraFloor < TopFloor And StairDataTable(CameraFloor + 1) = True Then DeleteStairs (CameraFloor + 1)
        If CameraFloor > BottomFloor And StairDataTable(CameraFloor - 1) = True Then DeleteStairs (CameraFloor - 1)
        
        If GetFloorHeight(CameraFloor) = 32 Or (GetFloorHeight(CameraFloor) > 32 And Camera.GetPosition.Y = (GetFloorAltitude(CameraFloor) * (GetFloorHeight(CameraFloor) / 32)) + 10 + (RiserHeight * 15)) Then
            If CameraFloor < TopFloor Then
                Call DeleteStairDoors
                CameraFloor = CameraFloor + 1
                Call CreateStairDoors(CameraFloor)
            End If
        Else
            FloorAltitude = FloorAltitude + 32
        End If
        
        If StairDataTable(CameraFloor) = False Then CreateStairs (CameraFloor)
      
        If CameraFloor < TopFloor And StairDataTable(CameraFloor + 1) = False Then CreateStairs (CameraFloor + 1)
        If CameraFloor > BottomFloor And StairDataTable(CameraFloor - 1) = False Then CreateStairs (CameraFloor - 1)
        Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10, Camera.GetPosition.z
    End If
    
    'Landing
    If Camera.GetPosition.X <= -ShaftLeft And Camera.GetPosition.X > -(ShaftLeft + 6) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -30.85 And Camera.GetPosition.Y = FloorAltitude + 10 + (RiserHeight * 1) Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10, Camera.GetPosition.z
       
    'Go down a floor
    If Camera.GetPosition.X <= -(ShaftLeft + 6) And Camera.GetPosition.X > -(ShaftLeft + 7.5) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 And Camera.GetPosition.Y = FloorAltitude + 10 Then
        Room(CameraFloor).Enable False
        CrawlSpace(CameraFloor).Enable False
        For i51 = 1 To 40
            ElevatorDoorL(i51).Enable False
            ElevatorDoorL(i51).Enable False
        Next i51
        ShaftsFloor(CameraFloor).Enable False
        For i51 = 1 To 40
            CallButtonsUp(i51).Enable False
            CallButtonsDown(i51).Enable False
        Next i51
        Atmos.SkyBox_Enable False
        Buildings.Enable False
        Landscape.Enable False
        DestroyObjects (CameraFloor)
        
        If Camera.GetPosition.Y = GetFloorAltitude(CameraFloor) + 10 Then
            If CameraFloor < TopFloor And StairDataTable(CameraFloor + 1) = True Then DeleteStairs (CameraFloor + 1)
            If CameraFloor > BottomFloor Then
                Call DeleteStairDoors
                CameraFloor = CameraFloor - 1
                Call CreateStairDoors(CameraFloor)
            End If
        Else
            FloorAltitude = FloorAltitude - 32
        End If
        
        If StairDataTable(CameraFloor) = False Then CreateStairs (CameraFloor)
        If CameraFloor < TopFloor Then If StairDataTable(CameraFloor + 1) = False Then CreateStairs (CameraFloor + 1)
        If CameraFloor > BottomFloor Then If StairDataTable(CameraFloor - 1) = False Then CreateStairs (CameraFloor - 1)
        If GetFloorHeight(CameraFloor) > 32 Then FloorAltitude = GetFloorAltitude(CameraFloor + 1) - 32
        Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 15), Camera.GetPosition.z
    End If
    
    'Go up/down steps
    If Camera.GetPosition.X <= -(ShaftLeft + 6) And Camera.GetPosition.X > -(ShaftLeft + 7.5) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 And Camera.GetPosition.Y = FloorAltitude + 10 + (RiserHeight * 14) Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 15), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 7.5) And Camera.GetPosition.X > -(ShaftLeft + 9) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 14), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 9) And Camera.GetPosition.X > -(ShaftLeft + 10.5) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 13), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 10.5) And Camera.GetPosition.X > -(ShaftLeft + 12) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 12), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 12) And Camera.GetPosition.X > -(ShaftLeft + 13.5) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 11), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 13.5) And Camera.GetPosition.X > -(ShaftLeft + 15) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 10), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 15) And Camera.GetPosition.X > -(ShaftLeft + 16) And Camera.GetPosition.z > -46.25 And Camera.GetPosition.z < -46.25 + 7.71 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 9), Camera.GetPosition.z
    
    If Camera.GetPosition.X <= -(ShaftLeft + 6) And Camera.GetPosition.X > -(ShaftLeft + 7.5) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 1), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 7.5) And Camera.GetPosition.X > -(ShaftLeft + 9) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 2), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 9) And Camera.GetPosition.X > -(ShaftLeft + 10.5) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 3), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 10.5) And Camera.GetPosition.X > -(ShaftLeft + 12) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 4), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 12) And Camera.GetPosition.X > -(ShaftLeft + 13.5) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 5), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 13.5) And Camera.GetPosition.X > -(ShaftLeft + 15) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 6), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 15) And Camera.GetPosition.X > -(ShaftLeft + 16) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 7), Camera.GetPosition.z
    If Camera.GetPosition.X <= -(ShaftLeft + 16) And Camera.GetPosition.X > -(ShaftLeft + 20) And Camera.GetPosition.z > -46.25 + 7.71 And Camera.GetPosition.z < -30.85 Then Camera.SetPosition Camera.GetPosition.X, FloorAltitude + 10 + (RiserHeight * 8), Camera.GetPosition.z

Next ShaftNum

End Sub

Sub DrawCrawlSpaces(FloorID As Integer, ShaftNum As Integer)
Dim FloorHeight As Single
Dim FloorAltitude As Single
FloorHeight = GetFloorHeight(FloorID)
FloorAltitude = GetFloorAltitude(FloorID)

Dim ShaftLeft As Single
Dim ShaftRight As Single
If ShaftNum = 1 Then ShaftLeft = 12.5: ShaftRight = 32.5
If ShaftNum = 2 Then ShaftLeft = 52.5: ShaftRight = 32.5
If ShaftNum = 3 Then ShaftLeft = 90.5: ShaftRight = 110.5
If ShaftNum = 4 Then ShaftLeft = 130.5: ShaftRight = 110.5

'temporary crawlspace walls

    'left walls
    If ShaftNum = 1 Then CrawlSpaceShaft1(FloorID).AddWall GetTex("BrickTexture"), ShaftRight, -46.25, ShaftRight, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 3 Then CrawlSpaceShaft3(FloorID).AddWall GetTex("BrickTexture"), ShaftRight, -46.25, ShaftRight, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 2 Then CrawlSpaceShaft2(FloorID).AddWall GetTex("BrickTexture"), -ShaftRight, -46.25, -ShaftRight, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 4 Then CrawlSpaceShaft4(FloorID).AddWall GetTex("BrickTexture"), -ShaftRight, -46.25, -ShaftRight, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
            
    'right walls
    If ShaftNum = 1 Then CrawlSpaceShaft1(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft, -46.25, ShaftLeft, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 3 Then CrawlSpaceShaft3(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft, -46.25, ShaftLeft, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 2 Then CrawlSpaceShaft2(FloorID).AddWall GetTex("BrickTexture"), -ShaftLeft, -46.25, -ShaftLeft, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 4 Then CrawlSpaceShaft4(FloorID).AddWall GetTex("BrickTexture"), -ShaftLeft, -46.25, -ShaftLeft, -60, FloorHeight, FloorAltitude, 20 * 0.086, 2
    
    'Floor
    If ShaftNum = 1 Then CrawlSpaceShaft1(FloorID).AddFloor GetTex("BrickTexture"), ShaftRight, -46.25, ShaftLeft, -60, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 3 Then CrawlSpaceShaft3(FloorID).AddFloor GetTex("BrickTexture"), ShaftRight, -46.25, ShaftLeft, -60, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 2 Then CrawlSpaceShaft2(FloorID).AddFloor GetTex("BrickTexture"), -ShaftRight, -46.25, -ShaftLeft, -60, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 4 Then CrawlSpaceShaft4(FloorID).AddFloor GetTex("BrickTexture"), -ShaftRight, -46.25, -ShaftLeft, -60, FloorAltitude, 20 * 0.086, 2
    
    'Ceiling
    If ShaftNum = 1 Then CrawlSpaceShaft1(FloorID).AddFloor GetTex("BrickTexture"), ShaftRight, -46.25, ShaftLeft, -60, FloorAltitude + FloorHeight - 0.1, 20 * 0.086, 2
    If ShaftNum = 3 Then CrawlSpaceShaft3(FloorID).AddFloor GetTex("BrickTexture"), ShaftRight, -46.25, ShaftLeft, -60, FloorAltitude + FloorHeight - 0.1, 20 * 0.086, 2
    If ShaftNum = 2 Then CrawlSpaceShaft2(FloorID).AddFloor GetTex("BrickTexture"), -ShaftRight, -46.25, -ShaftLeft, -60, FloorAltitude + FloorHeight - 0.1, 20 * 0.086, 2
    If ShaftNum = 4 Then CrawlSpaceShaft4(FloorID).AddFloor GetTex("BrickTexture"), -ShaftRight, -46.25, -ShaftLeft, -60, FloorAltitude + FloorHeight - 0.1, 20 * 0.086, 2
    
End Sub

Sub DrawElevatorWalls(FloorID As Integer, SectionNum As Integer, ShaftNum As Integer, JoinShafts As Boolean, e1 As Boolean, e2 As Boolean, e3 As Boolean, e4 As Boolean, e5 As Boolean, e6 As Boolean, e7 As Boolean, e8 As Boolean, e9 As Boolean, e10 As Boolean)

'Shaftnum is the number of the shaft, for layout purposes
'1=center in (for floors 118-138)
'2=center out (for floors 80-117)
'3=outer in (for floors 40-79)
'4=outer out (for floors 2-39)

Dim WallOffset As Single

'SectionNum determines the length of the shafts
'1 = originally lobby (5 elevators each w/lobby tex)
'2 = originally 2-39 (5 elevators each w/room tex)
'3 = originally 40-79 (4 elevators each)
'4 = originally 80-117 (3 elevators each)
'5 = originally 118-134 (2 elevators each)
'6 = originally 135-136 (1 elevator each)
'7 = originally 137-138 (1 elevator each w/brick texture)

'WallOffset is the distance between the walls and the elevator itself

Dim FloorHeight As Single
Dim FloorAltitude As Single
FloorHeight = GetFloorHeight(FloorID)
FloorAltitude = GetFloorAltitude(FloorID)

Dim TextureName As String
Dim ShaftEnd As Single
Dim ShaftLeft As Single
Dim ShaftRight As Single

If SectionNum = 1 Then TextureName = "Wall2"
If SectionNum > 1 And SectionNum < 7 Then TextureName = "Wall1"
If SectionNum = 7 Then TextureName = "BrickTexture"
If FloorID = 0 Then TextureName = "Wall2"

If ShaftNum = 1 Then ShaftLeft = 12.5: ShaftRight = 32.5
If ShaftNum = 2 Then ShaftLeft = 52.5: ShaftRight = 32.5
If ShaftNum = 3 Then ShaftLeft = 90.5: ShaftRight = 110.5
If ShaftNum = 4 Then ShaftLeft = 130.5: ShaftRight = 110.5

If ShaftNum = 1 Or ShaftNum = 3 Then WallOffset = 0.05
If ShaftNum = 2 Or ShaftNum = 4 Then WallOffset = -0.05

        'wall left of stairs
        If ShaftNum = 1 Or ShaftNum = 3 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -46.25, -ShaftLeft, -40.3, 19.5, FloorAltitude, (6 * 0.086), (19.5 * 0.08)
        Else
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -46.25, ShaftLeft, -40.3, 19.5, FloorAltitude, (6 * 0.086), (19.5 * 0.08)
        End If
        'wall between stairs (if available) and 1st elevator
        If e1 = True Then
            If ShaftNum = 1 Or ShaftNum = 3 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -32.5, -ShaftLeft, -30, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Or ShaftNum = 4 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -46.25, -ShaftLeft, -30, 19.5, FloorAltitude, (16.25 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -32.5, -(ShaftLeft + WallOffset), -30.5, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -46.25, -(ShaftLeft + WallOffset), -30.5, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -32.5, -(ShaftLeft + WallOffset), -30.5, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -46.25, -(ShaftLeft + WallOffset), -30.5, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
        Else
            If ShaftNum = 1 Or ShaftNum = 3 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -32.5, -ShaftLeft, -16, 19.5, FloorAltitude, (16.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Or ShaftNum = 4 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -46.25, -ShaftLeft, -16, 19.5, FloorAltitude, (16.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -32.5, -(ShaftLeft + WallOffset), -16, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -46.25, -(ShaftLeft + WallOffset), -16, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -32.5, -(ShaftLeft + WallOffset), -16, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -46.25, -(ShaftLeft + WallOffset), -16, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
        End If
        'wall between stairs (if available), and 2nd elevator
        If e2 = True Then
            If ShaftNum = 1 Or ShaftNum = 3 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -46.25, ShaftLeft, -30, 19.5, FloorAltitude, (16.25 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Or ShaftNum = 4 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -32.5, ShaftLeft, -30, 19.5, FloorAltitude, (2.5 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -46.25, ShaftLeft + WallOffset, -30, 19.5, FloorAltitude, (16.25 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -32.5, ShaftLeft + WallOffset, -30, 19.5, FloorAltitude, (16.25 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -46.25, ShaftLeft + WallOffset, -30, 19.5, FloorAltitude, (16.25 * 0.086), (19.5 * 0.08)
            If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -32.5, ShaftLeft + WallOffset, -30, 19.5, FloorAltitude, (16.25 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 7 Then
            If ShaftNum = 1 Or ShaftNum = 3 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -46.25, ShaftLeft, -16, 19.5, FloorAltitude, (30 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Or ShaftNum = 4 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -32.5, ShaftLeft, -16, 19.5, FloorAltitude, (30 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -46.25, ShaftLeft + WallOffset, -16, 19.5, FloorAltitude, (30 * 0.086), (19.5 * 0.08)
            If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -32.5, ShaftLeft + WallOffset, -16, 19.5, FloorAltitude, (30 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -46.25, ShaftLeft + WallOffset, -16, 19.5, FloorAltitude, (30 * 0.086), (19.5 * 0.08)
            If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -32.5, ShaftLeft + WallOffset, -16, 19.5, FloorAltitude, (30 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 1st and 3rd elevator
        If e3 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -16, -ShaftLeft, -15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 6 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -16, -ShaftLeft, -1, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -1, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -1, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 3rd and 5th elevator
        If e5 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -1, -ShaftLeft, 0, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -1, -(ShaftLeft + WallOffset), 0, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -1, -(ShaftLeft + WallOffset), 0, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 5 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -1, -ShaftLeft, 14, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -1, -(ShaftLeft + WallOffset), 14, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -1, -(ShaftLeft + WallOffset), 14, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 5th and 7th elevator
        If e7 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, 14, -ShaftLeft, 15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 14, -(ShaftLeft + WallOffset), 15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 14, -(ShaftLeft + WallOffset), 15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 4 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, 14, -ShaftLeft, 29, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 14, -(ShaftLeft + WallOffset), 29, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 14, -(ShaftLeft + WallOffset), 29, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 7th and 9th elevator
        If e9 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, 29, -ShaftLeft, 30, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 29, -(ShaftLeft + WallOffset), 30, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 29, -(ShaftLeft + WallOffset), 30, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 3 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, 29, -ShaftLeft, 44, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 29, -(ShaftLeft + WallOffset), 44, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 29, -(ShaftLeft + WallOffset), 44, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 2nd and 4th elevator
        If e4 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -16, ShaftLeft, -15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -16, ShaftLeft + WallOffset, -15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -16, ShaftLeft + WallOffset, -15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 6 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -16, ShaftLeft, -1, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -16, ShaftLeft + WallOffset, -1, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -16, ShaftLeft + WallOffset, -1, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 4th and 6th elevator
        If e6 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -1, ShaftLeft, 0, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -1, ShaftLeft + WallOffset, 0, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -1, ShaftLeft + WallOffset, 0, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 5 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -1, ShaftLeft, 14, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -1, ShaftLeft + WallOffset, 14, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -1, ShaftLeft + WallOffset, 14, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 6th and 8th elevator
        If e8 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, 14, ShaftLeft, 15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 14, ShaftLeft + WallOffset, 15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 14, ShaftLeft + WallOffset, 15, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 4 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, 14, ShaftLeft, 29, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 14, ShaftLeft + WallOffset, 29, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 14, ShaftLeft + WallOffset, 29, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        'wall between 8th and 10th elevator
        If e10 = True Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, 29, ShaftLeft, 30, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 29, ShaftLeft + WallOffset, 30, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 29, ShaftLeft + WallOffset, 30, 19.5, FloorAltitude, (1 * 0.086), (19.5 * 0.08)
        Else
            If SectionNum < 3 Then
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, 29, ShaftLeft, 44, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 29, ShaftLeft + WallOffset, 44, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 29, ShaftLeft + WallOffset, 44, 19.5, FloorAltitude, (15 * 0.086), (19.5 * 0.08)
            End If
        End If
        
        If SectionNum <= 2 Then ShaftEnd = 46.25
        If SectionNum = 3 Then ShaftEnd = 30.83
        If SectionNum = 4 Then ShaftEnd = 15.41
        If SectionNum = 5 Then ShaftEnd = 0
        If SectionNum >= 6 Then ShaftEnd = -15.42
        
        'this section places any shaft floors/ceilings that need to be made
        If FloorID = 0 Then
            If ShaftNum = 1 Or ShaftNum = 2 Then
            Shafts1(FloorID).AddFloor GetTex("BrickTexture"), -ShaftLeft, -30, -ShaftRight, 0, 0.05, (20 * 0.086), (30 * 0.08)
            Shafts2(FloorID).AddFloor GetTex("BrickTexture"), ShaftLeft, -30, ShaftRight, 0, 0.05, (20 * 0.086), (30 * 0.08)
            End If
            If ShaftNum = 3 Or ShaftNum = 4 Then
            Shafts3(FloorID).AddFloor GetTex("BrickTexture"), -ShaftLeft, -30, -ShaftRight, 46.25, 0.05, (20 * 0.086), (76.25 * 0.08)
            Shafts4(FloorID).AddFloor GetTex("BrickTexture"), ShaftLeft, -30, ShaftRight, 46.25, 0.05, (20 * 0.086), (76.25 * 0.08)
            End If
        End If
        If FloorID = 40 Then
            If ShaftNum = 4 Then
            Shafts3(FloorID).AddFloor GetTex("BrickTexture"), -ShaftLeft, -46.25, -ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            Shafts4(FloorID).AddFloor GetTex("BrickTexture"), ShaftLeft, -46.25, ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            End If
        End If
        If FloorID = 80 Then
            If ShaftNum = 3 Then
            Shafts3(FloorID).AddFloor GetTex("BrickTexture"), -ShaftLeft, -46.25, -ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            Shafts4(FloorID).AddFloor GetTex("BrickTexture"), ShaftLeft, -46.25, ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            End If
        End If
        If FloorID = 118 Then
            If ShaftNum = 2 Then
            Shafts1(FloorID).AddFloor GetTex("BrickTexture"), -ShaftLeft, -46.25, -ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            Shafts2(FloorID).AddFloor GetTex("BrickTexture"), ShaftLeft, -46.25, ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            End If
        End If
        If FloorID = 130 Then
            If ShaftNum = 1 Then
            Shafts1(FloorID).AddFloor GetTex("BrickTexture"), -ShaftLeft, 0, -ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            Shafts2(FloorID).AddFloor GetTex("BrickTexture"), ShaftLeft, 0, ShaftRight, 46.25, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
            End If
        End If
        If FloorID = 138 Then Shafts1(FloorID).AddFloor GetTex("BrickTexture"), -ShaftLeft, -30, -ShaftRight, -15.42, FloorAltitude + 24.95, (20 * 0.086), (16 * 0.08)
        
        'this section places the wall that extends to the end of the shaft, according to where the last elevator is
        If SectionNum = 7 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -16, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((16 + ShaftEnd) * 0.086), (19.5 * 0.08)
        Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((16 + ShaftEnd) * 0.086), (19.5 * 0.08)
        End If
        If SectionNum = 6 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -16, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((16 + ShaftEnd) * 0.086), (19.5 * 0.08)
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -16, ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((16 + ShaftEnd) * 0.086), (19.5 * 0.08)
        Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((16 + ShaftEnd) * 0.086), (19.5 * 0.08)
        Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -16, ShaftLeft + WallOffset, ShaftEnd, 19.5, FloorAltitude, ((16 + ShaftEnd) * 0.086), (19.5 * 0.08)
        End If
        If SectionNum = 5 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -1, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((1 + ShaftEnd) * 0.086), (19.5 * 0.08)
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -1, ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((1 + ShaftEnd) * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then
            Shafts1(FloorID).AddWall GetTex("BrickTexture"), -ShaftLeft, -1, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((1 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft, -1, ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((1 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
            If ShaftNum = 3 Or ShaftNum = 4 Then
            Shafts3(FloorID).AddWall GetTex("BrickTexture"), -ShaftLeft, -1, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((1 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft, -1, ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((1 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
        End If
        If SectionNum = 4 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, 14, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((-14 + ShaftEnd) * 0.086), (19.5 * 0.08)
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, 14, ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((-14 + ShaftEnd) * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then
            Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 14, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((-14 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 14, ShaftLeft + WallOffset, ShaftEnd, 19.5, FloorAltitude, ((-14 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
            If ShaftNum = 3 Or ShaftNum = 4 Then
            Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 14, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((-14 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 14, ShaftLeft + WallOffset, ShaftEnd, 19.5, FloorAltitude, ((-14 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
        End If
        If SectionNum = 3 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, 29, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((-29 + ShaftEnd) * 0.086), (19.5 * 0.08)
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, 29, ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((-29 + ShaftEnd) * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then
            Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 29, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((-29 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 29, ShaftLeft + WallOffset, ShaftEnd, 19.5, FloorAltitude, ((-29 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
            If ShaftNum = 3 Or ShaftNum = 4 Then
            Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 29, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((-29 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 29, ShaftLeft + WallOffset, ShaftEnd, 19.5, FloorAltitude, ((-29 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
        End If
        If SectionNum <= 2 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, 44, -ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((-44 + ShaftEnd) * 0.086), (19.5 * 0.08)
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, 44, ShaftLeft, ShaftEnd, 19.5, FloorAltitude, ((-44 + ShaftEnd) * 0.086), (19.5 * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then
            Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 44, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((-44 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 44, ShaftLeft + WallOffset, ShaftEnd, 19.5, FloorAltitude, ((-44 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
            If ShaftNum = 3 Or ShaftNum = 4 Then
            Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), 44, -(ShaftLeft + WallOffset), ShaftEnd, 19.5, FloorAltitude, ((-44 + ShaftEnd) * 0.086), (19.5 * 0.08)
            Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, 44, ShaftLeft + WallOffset, ShaftEnd, 19.5, FloorAltitude, ((-44 + ShaftEnd) * 0.086), (19.5 * 0.08)
            End If
        End If
        
        'walls above
            ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, -46.25, -ShaftLeft, ShaftEnd, (FloorHeight - 19.5), 19.5 + FloorAltitude, ((46.25 + ShaftEnd) * 0.086), ((FloorHeight - 19.5) * 0.08)
            If ShaftNum = 1 Or ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -30.5, -(ShaftLeft + WallOffset), ShaftEnd, (FloorHeight - 19.5), 19.5 + FloorAltitude, ((30 + ShaftEnd) * 0.086), ((FloorHeight - 19.5) * 0.08)
            If ShaftNum = 3 Or ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -30.5, -(ShaftLeft + WallOffset), ShaftEnd, (FloorHeight - 19.5), 19.5 + FloorAltitude, ((30 + ShaftEnd) * 0.086), ((FloorHeight - 19.5) * 0.08)
            If SectionNum <> 7 Then
                ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, -46.25, ShaftLeft, ShaftEnd, (FloorHeight - 19.5), 19.5 + FloorAltitude, ((46.25 + ShaftEnd) * 0.086), ((FloorHeight - 19.5) * 0.08)
                If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -30.5, ShaftLeft + WallOffset, ShaftEnd, (FloorHeight - 19.5), 19.5 + FloorAltitude, ((46.25 + ShaftEnd) * 0.086), ((FloorHeight - 19.5) * 0.08)
                If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -30.5, ShaftLeft + WallOffset, ShaftEnd, (FloorHeight - 19.5), 19.5 + FloorAltitude, ((46.25 + ShaftEnd) * 0.086), ((FloorHeight - 19.5) * 0.08)
            End If
    
    'other parts - document this section better
    
    'Hallway walls
    If FloorID = 0 Then
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftRight, -46.25, ShaftLeft, -46.25, FloorHeight, FloorAltitude, 20 * 0.086, 2
        If SectionNum <> 7 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftRight, -46.25, -ShaftLeft, -46.25, FloorHeight, FloorAltitude, 20 * 0.086, 2
    Else
        ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftRight, -46.25, ShaftLeft, -46.25, 25, FloorAltitude, 20 * 0.086, 2
        If SectionNum <> 7 Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftRight, -46.25, -ShaftLeft, -46.25, 25, FloorAltitude, 20 * 0.086, 2
    End If
    
    'Wall in front (south) of stairwell
    If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -ShaftRight, -30 - WallOffset, -ShaftLeft, -30 - WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -ShaftRight, -30 - WallOffset, -ShaftLeft, -30 - WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftRight, -30 + WallOffset, ShaftLeft, -30 + WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftRight, -30 + WallOffset, ShaftLeft, -30 + WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
    
    'Back (north) pipeshaft walls
    If ShaftNum = 1 Then PipeShaft(1).AddWall GetTex("BrickTexture"), ShaftRight, -46.25 + WallOffset, ShaftLeft, -46.25 + WallOffset, 25.1, FloorAltitude - 0.1, 20 * 0.086, 2
    If ShaftNum = 3 Then PipeShaft(3).AddWall GetTex("BrickTexture"), ShaftRight, -46.25 + WallOffset, ShaftLeft, -46.25 + WallOffset, 25.1, FloorAltitude - 0.1, 20 * 0.086, 2
    If ShaftNum = 2 Then PipeShaft(2).AddWall GetTex("BrickTexture"), -ShaftRight, -46.25 - WallOffset, -ShaftLeft, -46.25 - WallOffset, 25.1, FloorAltitude - 0.1, 20 * 0.086, 2
    If ShaftNum = 4 Then PipeShaft(4).AddWall GetTex("BrickTexture"), -ShaftRight, -46.25 - WallOffset, -ShaftLeft, -46.25 - WallOffset, 25.1, FloorAltitude - 0.1, 20 * 0.086, 2
    
    'front (south) pipeshaft walls
    If ShaftNum = 1 Then PipeShaft(1).AddWall GetTex("BrickTexture"), ShaftRight, -30 - WallOffset, ShaftLeft, -30 - WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 3 Then PipeShaft(3).AddWall GetTex("BrickTexture"), ShaftRight, -30 - WallOffset, ShaftLeft, -30 - WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 2 Then PipeShaft(2).AddWall GetTex("BrickTexture"), -ShaftRight, -30 + WallOffset, -ShaftLeft, -30 + WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 4 Then PipeShaft(4).AddWall GetTex("BrickTexture"), -ShaftRight, -30 + WallOffset, -ShaftLeft, -30 + WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
            
    'left (west) pipeshaft walls
    If ShaftNum = 1 Then PipeShaft(1).AddWall GetTex("BrickTexture"), ShaftRight - WallOffset, -46.25, ShaftRight - WallOffset, -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 3 Then PipeShaft(3).AddWall GetTex("BrickTexture"), ShaftRight - WallOffset, -46.25, ShaftRight - WallOffset, -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 2 Then PipeShaft(2).AddWall GetTex("BrickTexture"), -(ShaftRight - WallOffset), -46.25, -(ShaftRight - WallOffset), -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 4 Then PipeShaft(4).AddWall GetTex("BrickTexture"), -(ShaftRight - WallOffset), -46.25, -(ShaftRight - WallOffset), -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
            
    'right (east) pipeshaft walls
    If ShaftNum = 1 Then PipeShaft(1).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -46.25, ShaftLeft + WallOffset, -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 3 Then PipeShaft(3).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, -46.25, ShaftLeft + WallOffset, -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 2 Then PipeShaft(2).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -46.25, -(ShaftLeft + WallOffset), -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
    If ShaftNum = 4 Then PipeShaft(4).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), -46.25, -(ShaftLeft + WallOffset), -30, FloorHeight, FloorAltitude, 20 * 0.086, 2
        
    'pipeshaft tops
    If ShaftNum = 1 And FloorID = 136 Then CrawlSpaceShaft1(137).AddWall GetTex("BrickTexture"), ShaftLeft, -46.25, ShaftRight, -30, FloorAltitude + FloorHeight - 0.05, 20 * 0.086, 2
    If ShaftNum = 3 And FloorID = 79 Then PipeShaft(3).AddFloor GetTex("BrickTexture"), ShaftLeft, -46.25, ShaftRight, -30, FloorAltitude + (FloorHeight * 2) - 0.05, 20 * 0.086, 2
    If ShaftNum = 2 And FloorID = 117 Then PipeShaft(2).AddFloor GetTex("BrickTexture"), -ShaftLeft, -46.25, -ShaftRight, -30, FloorAltitude + (FloorHeight * 2) - 0.05, 20 * 0.086, 2
    If ShaftNum = 4 And FloorID = 39 Then PipeShaft(4).AddFloor GetTex("BrickTexture"), -ShaftLeft, -46.25, -ShaftRight, -30, FloorAltitude + (FloorHeight * 2) - 0.05, 20 * 0.086, 2
    
    'Crawlspace Elevator Walls
    If FloorID <> 0 And FloorID <> 1 Then
        If JoinShafts = False Then CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), -(ShaftRight + WallOffset), -46.25 - WallOffset, -(ShaftRight + WallOffset), ShaftEnd + WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (46.25 + ShaftEnd) * 0.086, 1
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft - WallOffset), -46.25 - WallOffset, -(ShaftLeft - WallOffset), ShaftEnd + WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (46.25 + ShaftEnd) * 0.086, 1
        If JoinShafts = False Then CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), ShaftRight + WallOffset, -46.25 - WallOffset, ShaftRight + WallOffset, ShaftEnd + WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (46.25 + ShaftEnd) * 0.086, 1
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft - WallOffset, -46.25 - WallOffset, ShaftLeft - WallOffset, ShaftEnd + WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (46.25 + ShaftEnd) * 0.086, 1
    If ShaftNum = 1 Or ShaftNum = 3 Then
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft - WallOffset, ShaftEnd + WallOffset, ShaftRight + WallOffset, ShaftEnd + WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (ShaftRight - ShaftLeft) * 0.086, 1
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft - WallOffset), ShaftEnd + WallOffset, -(ShaftRight + WallOffset), ShaftEnd + WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (ShaftRight - ShaftLeft) * 0.086, 1
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft - WallOffset), -46.25 - WallOffset, -(ShaftRight + WallOffset), -46.25 - WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (ShaftRight - ShaftLeft) * 0.086, 1
    End If
    
    If ShaftNum = 2 Or ShaftNum = 4 Then
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft + WallOffset, ShaftEnd - WallOffset, ShaftRight - WallOffset, ShaftEnd - WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (ShaftRight - ShaftLeft) * 0.086, 1
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), -(ShaftLeft + WallOffset), ShaftEnd - WallOffset, -(ShaftRight - WallOffset), ShaftEnd - WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (ShaftRight - ShaftLeft) * 0.086, 1
        CrawlSpace(FloorID).AddWall GetTex("BrickTexture"), (ShaftLeft + WallOffset), -46.25 + WallOffset, (ShaftRight - WallOffset), -46.25 + WallOffset, FloorHeight - 25, (FloorID * FloorHeight) + FloorHeight + 25, (ShaftRight - ShaftLeft) * 0.086, 1
    End If
    End If
    
    If FloorID <> 0 And FloorID <> 1 Then Call DrawCrawlSpaces(FloorID, ShaftNum)
    
            'Main shaft area
            If SectionNum <> 7 Then
                
                ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftLeft, ShaftEnd, ShaftRight, ShaftEnd, FloorHeight, FloorAltitude, 20 * 0.086, 2
                
                If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft, ShaftEnd - 0.05, ShaftRight, ShaftEnd - 0.05, FloorHeight, FloorAltitude, 20 * 0.086, 2
                If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), ShaftLeft, ShaftEnd - 0.05, ShaftRight, ShaftEnd - 0.05, FloorHeight, FloorAltitude, 20 * 0.086, 2
                
                If JoinShafts = False Then
                ShaftsFloor(FloorID).AddWall GetTex(TextureName), ShaftRight, ShaftEnd, ShaftRight, -46.25, FloorHeight, FloorAltitude, (46.25 + ShaftEnd) * 0.086, 2
                If ShaftNum = 1 Or ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("BrickTexture"), (ShaftRight - WallOffset), ShaftEnd, (ShaftRight - WallOffset), -30.85, FloorHeight, FloorAltitude, (46.25 + ShaftEnd) * 0.086, 2
                If ShaftNum = 3 Or ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("BrickTexture"), (ShaftRight - WallOffset), ShaftEnd, (ShaftRight - WallOffset), -30.85, FloorHeight, FloorAltitude, (46.25 + ShaftEnd) * 0.086, 2
                End If
                
                ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, ShaftEnd, -ShaftRight, ShaftEnd, FloorHeight, FloorAltitude, 20 * 0.086, 2
                If JoinShafts = False Then ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftRight, ShaftEnd, -ShaftRight, -46.25, FloorHeight, FloorAltitude, (46.25 + ShaftEnd) * 0.086, 2
                
                If ShaftNum = 1 Or ShaftNum = 2 Then
                Shafts1(FloorID).AddWall GetTex("BrickTexture"), -ShaftLeft, ShaftEnd - 0.05, -ShaftRight, ShaftEnd - 0.05, FloorHeight, FloorAltitude, 20 * 0.086, 2
                    If JoinShafts = False Then Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftRight - WallOffset), ShaftEnd, -(ShaftRight - WallOffset), -30.5, FloorHeight, FloorAltitude, (30 + ShaftEnd) * 0.086, 2
                End If
                If ShaftNum = 3 Or ShaftNum = 4 Then
                Shafts3(FloorID).AddWall GetTex("BrickTexture"), -ShaftLeft, ShaftEnd - 0.05, -ShaftRight, ShaftEnd - 0.05, FloorHeight, FloorAltitude, 20 * 0.086, 2
                    If JoinShafts = False Then Shafts3(FloorID).AddWall GetTex("BrickTexture"), -(ShaftRight - WallOffset), ShaftEnd, -(ShaftRight - WallOffset), -30.5, FloorHeight, FloorAltitude, (30 + ShaftEnd) * 0.086, 2
                End If
            End If
            If SectionNum = 7 Then
                ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftLeft, ShaftEnd, -ShaftRight, ShaftEnd, FloorHeight, FloorAltitude, 20 * 0.086, 2
                ShaftsFloor(FloorID).AddWall GetTex(TextureName), -ShaftRight, ShaftEnd, -ShaftRight, -46.25, FloorHeight, FloorAltitude, (46.25 + ShaftEnd) * 0.086, 2
                Shafts1(FloorID).AddWall GetTex("BrickTexture"), -ShaftLeft, ShaftEnd - WallOffset, -ShaftRight, ShaftEnd - WallOffset, FloorHeight, FloorAltitude, 20 * 0.086, 2
                Shafts1(FloorID).AddWall GetTex("BrickTexture"), -(ShaftRight - WallOffset), ShaftEnd, -(ShaftRight - WallOffset), -30.5, FloorHeight, FloorAltitude, (30 + ShaftEnd) * 0.086, 2
            End If
    
'Elevator Doors (inside shafts)
    
    If e1 = True Then
    If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16, -(ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -16, -ShaftLeft, -19, 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -30, -ShaftLeft, -27, 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e2 = True Then
    If ShaftNum = 1 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16, (ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16, (ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16, (ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16, (ShaftLeft + WallOffset), -30, 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -16, ShaftLeft, -19, 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -30, ShaftLeft, -27, 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e3 = True Then
    If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 1), -(ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 1), -(ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 1), -(ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 1), -(ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -16 + (15 * 1), -ShaftLeft, -19 + (15 * 1), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -30 + (15 * 1), -ShaftLeft, -27 + (15 * 1), 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e4 = True Then
    If ShaftNum = 1 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 1), (ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 1), (ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 1), (ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 1), (ShaftLeft + WallOffset), -30 + (15 * 1), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -16 + (15 * 1), ShaftLeft, -19 + (15 * 1), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -30 + (15 * 1), ShaftLeft, -27 + (15 * 1), 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e5 = True Then
    If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 2), -(ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 2), -(ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 2), -(ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 2), -(ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -16 + (15 * 2), -ShaftLeft, -19 + (15 * 2), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -30 + (15 * 2), -ShaftLeft, -27 + (15 * 2), 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e6 = True Then
    If ShaftNum = 1 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 2), (ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 2), (ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 2), (ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 2), (ShaftLeft + WallOffset), -30 + (15 * 2), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -16 + (15 * 2), ShaftLeft, -19 + (15 * 2), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -30 + (15 * 2), ShaftLeft, -27 + (15 * 2), 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e7 = True Then
    If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 3), -(ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 3), -(ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 3), -(ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 3), -(ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -16 + (15 * 3), -ShaftLeft, -19 + (15 * 3), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -30 + (15 * 3), -ShaftLeft, -27 + (15 * 3), 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e8 = True Then
    If ShaftNum = 1 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 3), (ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 3), (ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 3), (ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 3), (ShaftLeft + WallOffset), -30 + (15 * 3), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -16 + (15 * 3), ShaftLeft, -19 + (15 * 3), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -30 + (15 * 3), ShaftLeft, -27 + (15 * 3), 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e9 = True Then
    If ShaftNum = 1 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 4), -(ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts1(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 4), -(ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 4), -(ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts3(FloorID).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -16 + (15 * 4), -(ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -16 + (15 * 4), -ShaftLeft, -19 + (15 * 4), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), -ShaftLeft, -30 + (15 * 4), -ShaftLeft, -27 + (15 * 4), 19.6, FloorAltitude, 0.5, 1
    End If
    
    If e10 = True Then
    If ShaftNum = 1 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 4), (ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 2 Then Shafts2(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 4), (ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 3 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 4), (ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    If ShaftNum = 4 Then Shafts4(FloorID).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -16 + (15 * 4), (ShaftLeft + WallOffset), -30 + (15 * 4), 19.6, FloorAltitude, 1, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -16 + (15 * 4), ShaftLeft, -19 + (15 * 4), 19.6, FloorAltitude, 0.5, 1
    Room(FloorID).AddWall GetTex("ElevExtPanels"), ShaftLeft, -30 + (15 * 4), ShaftLeft, -27 + (15 * 4), 19.6, FloorAltitude, 0.5, 1
    End If
    
End Sub

Sub InitRealtime(FloorID As Integer)
'Initialize realtime objects

'Destroy meshes
For i54 = 1 To 40
CallButtonsUp(i54).Enable False
CallButtonsDown(i54).Enable False
'ElevatorDoorL(i54).Enable False
'ElevatorDoorR(i54).Enable False
Scene.DestroyMesh CallButtonsUp(i54)
Scene.DestroyMesh CallButtonsDown(i54)
Scene.DestroyMesh ElevatorDoorL(i54)
Scene.DestroyMesh ElevatorDoorR(i54)
Set CallButtonsUp(i54) = Nothing
Set CallButtonsDown(i54) = Nothing
Set ElevatorDoorL(i54) = Nothing
Set ElevatorDoorR(i54) = Nothing
'Sleep 10
Next i54

Call DeleteStairDoors

'Create blank meshes
For i54 = 1 To 40
Set ElevatorDoorL(i54) = Scene.CreateMeshBuilder("ElevatorDoorL" + Str$(i54))
Set ElevatorDoorR(i54) = Scene.CreateMeshBuilder("ElevatorDoorR" + Str$(i54))
Set CallButtonsUp(i54) = Scene.CreateMeshBuilder("CallButtonsUp" + Str$(i54))
Set CallButtonsDown(i54) = Scene.CreateMeshBuilder("CallButtonsDown" + Str$(i54))
Next i54

'Generate objects for floors


If FloorID = 0 Then
Call ProcessRealtime(FloorID, 5, 1, True, True, True, True, True, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 5, 2, True, True, True, True, True, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 1, 3, True, True, True, True, True, True, True, True, True, True, True)
Call ProcessRealtime(FloorID, 1, 4, True, True, True, True, True, True, True, True, True, True, True)
End If

If FloorID = 2 Or FloorID = 39 Then
Call ProcessRealtime(FloorID, 5, 1, True, True, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 5, 2, True, True, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 3, True, False, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 4, True, True, True, True, True, True, True, True, True, True, True)
End If
If FloorID > 2 And FloorID < 39 Then
Call ProcessRealtime(FloorID, 5, 1, True, False, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 5, 2, True, True, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 3, True, False, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 4, True, True, True, True, True, True, True, True, True, True, True)
End If

If FloorID = 40 Or FloorID = 79 Then
Call ProcessRealtime(FloorID, 5, 1, True, True, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 3, False, True, True, True, True, True, True, True, True, True, True)
End If
If FloorID > 40 And FloorID < 79 Then
Call ProcessRealtime(FloorID, 5, 1, True, False, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 3, False, True, True, True, True, True, True, True, True, True, True)
End If
If FloorID >= 40 And FloorID <= 51 Then
Call ProcessRealtime(FloorID, 5, 2, True, True, False, False, False, False, False, False, False, False, False)
End If
If FloorID <= 79 And FloorID > 51 Then
Call ProcessRealtime(FloorID, 5, 2, True, False, True, False, False, False, False, False, False, False, False)
End If
       
If FloorID >= 82 And FloorID <= 98 Then
Call ProcessRealtime(FloorID, 2, 1, True, False, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 2, True, False, True, False, False, True, True, True, True, True, True)
End If
If FloorID >= 102 And FloorID < 114 Then
Call ProcessRealtime(FloorID, 2, 1, True, True, False, False, False, True, True, True, True, True, True)
Call ProcessRealtime(FloorID, 2, 2, True, False, False, False, False, False, False, False, False, False, False)
End If
If FloorID = 81 Or FloorID = 99 Then
Call ProcessRealtime(FloorID, 2, 1, True, False, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 2, True, False, True, False, False, True, True, True, True, True, True)
End If
If FloorID = 100 Then
Call ProcessRealtime(FloorID, 2, 1, True, False, False, False, False, True, True, True, True, True, True)
Call ProcessRealtime(FloorID, 2, 2, True, False, True, False, False, False, False, False, False, False, False)
End If
If FloorID = 114 Then
Call ProcessRealtime(FloorID, 2, 1, True, True, False, False, False, True, True, True, True, True, True)
Call ProcessRealtime(FloorID, 2, 2, True, False, False, False, False, False, False, False, False, False, False)
End If
If FloorID = 101 Then
Call ProcessRealtime(FloorID, 2, 1, True, True, False, False, False, True, True, True, True, True, True)
Call ProcessRealtime(FloorID, 2, 2, True, False, True, False, False, False, False, False, False, False, False)
End If
 
If FloorID >= 118 And FloorID <= 129 Then
Call ProcessRealtime(FloorID, 2, 1, False, True, False, False, False, True, True, True, True, True, True)
End If

If FloorID = 80 Then
Call ProcessRealtime(FloorID, 2, 1, True, True, True, True, True, True, True, True, True, True, True)
Call ProcessRealtime(FloorID, 2, 2, True, True, True, True, True, True, True, True, True, True, True)
End If

If FloorID = 115 Then
Call ProcessRealtime(FloorID, 2, 1, True, True, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 2, True, True, True, False, False, False, False, False, False, False, False)
End If

If FloorID = 116 Then
Call ProcessRealtime(FloorID, 2, 1, True, True, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 2, True, True, True, False, False, False, False, False, False, False, False)
End If

If FloorID = 117 Then
Call ProcessRealtime(FloorID, 2, 1, True, True, False, False, False, False, False, False, False, False, False)
Call ProcessRealtime(FloorID, 2, 2, True, True, True, False, False, False, False, False, False, False, False)
End If

If FloorID = 130 Then Call ProcessRealtime(FloorID, 5, 1, False, True, False, False, False, False, False, False, False, False, False)
If FloorID = 131 Then Call ProcessRealtime(FloorID, 5, 1, False, True, False, False, False, False, False, False, False, False, False)
If FloorID = 132 Then Call ProcessRealtime(FloorID, 5, 1, False, True, True, True, True, False, False, False, False, False, False)
If FloorID = 133 Then Call ProcessRealtime(FloorID, 5, 1, False, True, False, False, False, False, False, False, False, False, False)
If FloorID = 134 Then Call ProcessRealtime(FloorID, 5, 1, False, True, True, True, True, False, False, False, False, False, False)
If FloorID = 135 Then Call ProcessRealtime(FloorID, 5, 1, False, True, True, True, True, False, False, False, False, False, False)
If FloorID = 136 Then Call ProcessRealtime(FloorID, 5, 1, False, True, True, True, True, False, False, False, False, False, False)
If FloorID = 137 Then Call ProcessRealtime(FloorID, 7, 1, False, True, False, False, False, False, False, False, False, False, False)
If FloorID = 138 Then Call ProcessRealtime(FloorID, 7, 1, False, True, False, False, False, False, False, False, False, False, False)

End Sub

Sub ProcessRealtime(FloorID As Integer, SectionNum As Integer, ShaftNum As Integer, JoinShafts As Boolean, e1 As Boolean, e2 As Boolean, e3 As Boolean, e4 As Boolean, e5 As Boolean, e6 As Boolean, e7 As Boolean, e8 As Boolean, e9 As Boolean, e10 As Boolean)
'This subroutine is similar to the DrawElevatorWalls routines, and it is designed to create the external elevator doors and other objects in realtime.

Dim FloorHeight As Single
Dim FloorAltitude As Single
FloorHeight = GetFloorHeight(FloorID)
FloorAltitude = GetFloorAltitude(FloorID)

Dim ShaftEnd As Single
Dim ShaftLeft As Single
Dim ShaftRight As Single
Dim WallOffset As Single
Dim WallOffset2 As Single

If ShaftNum = 1 Then ShaftLeft = 12.5: ShaftRight = 32.5
If ShaftNum = 2 Then ShaftLeft = 52.5: ShaftRight = 32.5
If ShaftNum = 3 Then ShaftLeft = 90.5: ShaftRight = 110.5
If ShaftNum = 4 Then ShaftLeft = 130.5: ShaftRight = 110.5

If SectionNum <= 2 Then ShaftEnd = 46.25
If SectionNum = 3 Then ShaftEnd = 30.83
If SectionNum = 4 Then ShaftEnd = 15.41
If SectionNum = 5 Then ShaftEnd = 0
If SectionNum >= 6 Then ShaftEnd = -15.42

If ShaftNum = 1 Or ShaftNum = 3 Then WallOffset = 0.05: WallOffset2 = 0.01
If ShaftNum = 2 Or ShaftNum = 4 Then WallOffset = -0.05: WallOffset2 = 0.01

'Call Button Panels
    'original height offset - 8.5
    If e1 = True And ShaftNum = 1 Then CallButtonsDown(1).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -18, -(ShaftLeft - WallOffset2), -17, 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(1).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -18, -(ShaftLeft - WallOffset2), -17, 1.5, 9.5 + FloorAltitude, 1, 1
    If e1 = True And ShaftNum = 1 And FloorID = 1 Then CallButtonsDown(1).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -18, -(ShaftLeft - WallOffset2), -17, 1.5, 8 + (0 * FloorHeight) + FloorHeight, 1, 1: CallButtonsUp(1).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -18, -(ShaftLeft - WallOffset2), -17, 1.5, 9.5 + (0 * FloorHeight) + FloorHeight, 1, 1
    If e3 = True And ShaftNum = 1 Then CallButtonsDown(3).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 1), -(ShaftLeft - WallOffset2), -15 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(3).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 1), -(ShaftLeft - WallOffset2), -15 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e3 = True And ShaftNum = 1 And FloorID = 1 Then CallButtonsDown(3).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 1), -(ShaftLeft - WallOffset2), -15 + (15 * 1), 1.5, 8 + (0 * FloorHeight) + FloorHeight, 1, 1: CallButtonsUp(3).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 1), -(ShaftLeft - WallOffset2), -15 + (15 * 1), 1.5, 9.5 + (0 * FloorHeight) + FloorHeight, 1, 1
    If e4 = True And ShaftNum = 1 Then CallButtonsDown(4).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 1), (ShaftLeft - WallOffset2), -30 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(4).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 1), (ShaftLeft - WallOffset2), -30 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e4 = True And ShaftNum = 1 And FloorID = 1 Then CallButtonsDown(4).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 1), (ShaftLeft - WallOffset2), -30 + (15 * 1), 1.5, 8 + (0 * FloorHeight) + FloorHeight, 1, 1: CallButtonsUp(4).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 1), (ShaftLeft - WallOffset2), -30 + (15 * 1), 1.5, 9.5 + (0 * FloorHeight) + FloorHeight, 1, 1
    If e6 = True And ShaftNum = 1 Then CallButtonsDown(6).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -16 + (15 * 2), (ShaftLeft - WallOffset2), -15 + (15 * 2), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(6).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -16 + (15 * 2), (ShaftLeft - WallOffset2), -15 + (15 * 2), 1.5, 9.5 + FloorAltitude, 1, 1
    If e7 = True And ShaftNum = 1 Then CallButtonsDown(7).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -31 + (15 * 3), -(ShaftLeft - WallOffset2), -30 + (15 * 3), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(7).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -31 + (15 * 3), -(ShaftLeft - WallOffset2), -30 + (15 * 3), 1.5, 9.5 + FloorAltitude, 1, 1
    
    If e2 = True And ShaftNum = 2 Then CallButtonsDown(12).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -18, (ShaftLeft + WallOffset2), -17, 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(12).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -18, (ShaftLeft + WallOffset2), -17, 1.5, 9.5 + FloorAltitude, 1, 1
    If e1 = True And ShaftNum = 2 Then CallButtonsDown(11).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -29, -(ShaftLeft + WallOffset2), -28, 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(11).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -29, -(ShaftLeft + WallOffset2), -28, 1.5, 9.5 + FloorAltitude, 1, 1
    If e4 = True And ShaftNum = 2 Then CallButtonsDown(14).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -18 + (15 * 1), (ShaftLeft + WallOffset2), -17 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(14).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -18 + (15 * 1), (ShaftLeft + WallOffset2), -17 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e3 = True And ShaftNum = 2 Then CallButtonsDown(13).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -29 + (15 * 1), -(ShaftLeft + WallOffset2), -28 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(13).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -29 + (15 * 1), -(ShaftLeft + WallOffset2), -28 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e8 = True And ShaftNum = 2 Then CallButtonsDown(18).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -16 + (15 * 3), (ShaftLeft + WallOffset2), -15 + (15 * 3), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(18).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -16 + (15 * 3), (ShaftLeft + WallOffset2), -15 + (15 * 3), 1.5, 9.5 + FloorAltitude, 1, 1
    If e7 = True And ShaftNum = 2 Then CallButtonsDown(17).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -31 + (15 * 3), -(ShaftLeft + WallOffset2), -30 + (15 * 3), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(17).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -31 + (15 * 3), -(ShaftLeft + WallOffset2), -30 + (15 * 3), 1.5, 9.5 + FloorAltitude, 1, 1
    
    
    If e3 = True And ShaftNum = 3 Then CallButtonsDown(23).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 1), -(ShaftLeft - WallOffset2), -15 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(23).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 1), -(ShaftLeft - WallOffset2), -15 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e4 = True And ShaftNum = 3 Then CallButtonsDown(24).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 1), (ShaftLeft - WallOffset2), -30 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(24).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 1), (ShaftLeft - WallOffset2), -30 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e7 = True And ShaftNum = 3 Then CallButtonsDown(27).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 3), -(ShaftLeft - WallOffset2), -15 + (15 * 3), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(27).AddWall GetTex("CallButtonsTex"), -(ShaftLeft - WallOffset2), -16 + (15 * 3), -(ShaftLeft - WallOffset2), -15 + (15 * 3), 1.5, 9.5 + FloorAltitude, 1, 1
    If e8 = True And ShaftNum = 3 Then CallButtonsDown(28).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 3), (ShaftLeft - WallOffset2), -30 + (15 * 3), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(28).AddWall GetTex("CallButtonsTex"), (ShaftLeft - WallOffset2), -31 + (15 * 3), (ShaftLeft - WallOffset2), -30 + (15 * 3), 1.5, 9.5 + FloorAltitude, 1, 1
    
    If e4 = True And ShaftNum = 4 Then CallButtonsDown(34).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -16 + (15 * 1), (ShaftLeft + WallOffset2), -15 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(34).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -16 + (15 * 1), (ShaftLeft + WallOffset2), -15 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e3 = True And ShaftNum = 4 Then CallButtonsDown(33).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -31 + (15 * 1), -(ShaftLeft + WallOffset2), -30 + (15 * 1), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(33).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -31 + (15 * 1), -(ShaftLeft + WallOffset2), -30 + (15 * 1), 1.5, 9.5 + FloorAltitude, 1, 1
    If e8 = True And ShaftNum = 4 Then CallButtonsDown(38).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -16 + (15 * 3), (ShaftLeft + WallOffset2), -15 + (15 * 3), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(38).AddWall GetTex("CallButtonsTex"), (ShaftLeft + WallOffset2), -16 + (15 * 3), (ShaftLeft + WallOffset2), -15 + (15 * 3), 1.5, 9.5 + FloorAltitude, 1, 1
    If e7 = True And ShaftNum = 4 Then CallButtonsDown(37).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -31 + (15 * 3), -(ShaftLeft + WallOffset2), -30 + (15 * 3), 1.5, 8 + FloorAltitude, 1, 1: CallButtonsUp(37).AddWall GetTex("CallButtonsTex"), -(ShaftLeft + WallOffset2), -31 + (15 * 3), -(ShaftLeft + WallOffset2), -30 + (15 * 3), 1.5, 9.5 + FloorAltitude, 1, 1
    
'Shaft Elevator Doors
        If e1 = True And ShaftNum = 1 Then ElevatorDoorL(1).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05, -(ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e1 = True And ShaftNum = 1 Then ElevatorDoorR(1).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05, -(ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        If e1 = True And ShaftNum = 2 Then ElevatorDoorL(11).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05, -(ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e1 = True And ShaftNum = 2 Then ElevatorDoorR(11).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05, -(ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        If e1 = True And ShaftNum = 3 Then ElevatorDoorL(21).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05, -(ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e1 = True And ShaftNum = 3 Then ElevatorDoorR(21).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05, -(ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        If e1 = True And ShaftNum = 4 Then ElevatorDoorL(31).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05, -(ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e1 = True And ShaftNum = 4 Then ElevatorDoorR(31).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05, -(ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        
        If e2 = True And ShaftNum = 1 Then ElevatorDoorL(2).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05, (ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e2 = True And ShaftNum = 1 Then ElevatorDoorR(2).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05, (ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        If e2 = True And ShaftNum = 2 Then ElevatorDoorL(12).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05, (ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e2 = True And ShaftNum = 2 Then ElevatorDoorR(12).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05, (ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        If e2 = True And ShaftNum = 3 Then ElevatorDoorL(22).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05, (ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e2 = True And ShaftNum = 3 Then ElevatorDoorR(22).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05, (ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        If e2 = True And ShaftNum = 4 Then ElevatorDoorL(32).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05, (ShaftLeft + WallOffset), -22.95, 19.6, FloorAltitude, 1, 1
        If e2 = True And ShaftNum = 4 Then ElevatorDoorR(32).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05, (ShaftLeft + WallOffset), -27.05, 19.6, FloorAltitude, 1, 1
        
        If e3 = True And ShaftNum = 1 Then ElevatorDoorL(3).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 1), -(ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e3 = True And ShaftNum = 1 Then ElevatorDoorR(3).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 1), -(ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e3 = True And ShaftNum = 2 Then ElevatorDoorL(13).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 1), -(ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e3 = True And ShaftNum = 2 Then ElevatorDoorR(13).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 1), -(ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e3 = True And ShaftNum = 3 Then ElevatorDoorL(23).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 1), -(ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e3 = True And ShaftNum = 3 Then ElevatorDoorR(23).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 1), -(ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e3 = True And ShaftNum = 4 Then ElevatorDoorL(33).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 1), -(ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e3 = True And ShaftNum = 4 Then ElevatorDoorR(33).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 1), -(ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        
        If e4 = True And ShaftNum = 1 Then ElevatorDoorL(4).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 1), (ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e4 = True And ShaftNum = 1 Then ElevatorDoorR(4).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 1), (ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e4 = True And ShaftNum = 2 Then ElevatorDoorL(14).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 1), (ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e4 = True And ShaftNum = 2 Then ElevatorDoorR(14).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 1), (ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e4 = True And ShaftNum = 3 Then ElevatorDoorL(24).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 1), (ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e4 = True And ShaftNum = 3 Then ElevatorDoorR(24).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 1), (ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e4 = True And ShaftNum = 4 Then ElevatorDoorL(34).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 1), (ShaftLeft + WallOffset), -22.95 + (15 * 1), 19.6, FloorAltitude, 1, 1
        If e4 = True And ShaftNum = 4 Then ElevatorDoorR(34).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 1), (ShaftLeft + WallOffset), -27.05 + (15 * 1), 19.6, FloorAltitude, 1, 1
        
        If e5 = True And ShaftNum = 1 Then ElevatorDoorL(5).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 2), -(ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e5 = True And ShaftNum = 1 Then ElevatorDoorR(5).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 2), -(ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e5 = True And ShaftNum = 2 Then ElevatorDoorL(15).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 2), -(ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e5 = True And ShaftNum = 2 Then ElevatorDoorR(15).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 2), -(ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e5 = True And ShaftNum = 3 Then ElevatorDoorL(25).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 2), -(ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e5 = True And ShaftNum = 3 Then ElevatorDoorR(25).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 2), -(ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e5 = True And ShaftNum = 4 Then ElevatorDoorL(35).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 2), -(ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e5 = True And ShaftNum = 4 Then ElevatorDoorR(35).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 2), -(ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        
        If e6 = True And ShaftNum = 1 Then ElevatorDoorL(6).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 2), (ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e6 = True And ShaftNum = 1 Then ElevatorDoorR(6).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 2), (ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e6 = True And ShaftNum = 2 Then ElevatorDoorL(16).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 2), (ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e6 = True And ShaftNum = 2 Then ElevatorDoorR(16).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 2), (ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e6 = True And ShaftNum = 3 Then ElevatorDoorL(26).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 2), (ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e6 = True And ShaftNum = 3 Then ElevatorDoorR(26).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 2), (ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e6 = True And ShaftNum = 4 Then ElevatorDoorL(36).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 2), (ShaftLeft + WallOffset), -22.95 + (15 * 2), 19.6, FloorAltitude, 1, 1
        If e6 = True And ShaftNum = 4 Then ElevatorDoorR(36).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 2), (ShaftLeft + WallOffset), -27.05 + (15 * 2), 19.6, FloorAltitude, 1, 1
        
        If e7 = True And ShaftNum = 1 Then ElevatorDoorL(7).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 3), -(ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e7 = True And ShaftNum = 1 Then ElevatorDoorR(7).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 3), -(ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e7 = True And ShaftNum = 2 Then ElevatorDoorL(17).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 3), -(ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e7 = True And ShaftNum = 2 Then ElevatorDoorR(17).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 3), -(ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e7 = True And ShaftNum = 3 Then ElevatorDoorL(27).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 3), -(ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e7 = True And ShaftNum = 3 Then ElevatorDoorR(27).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 3), -(ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e7 = True And ShaftNum = 4 Then ElevatorDoorL(37).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 3), -(ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e7 = True And ShaftNum = 4 Then ElevatorDoorR(37).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 3), -(ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        
        If e8 = True And ShaftNum = 1 Then ElevatorDoorL(8).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 3), (ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e8 = True And ShaftNum = 1 Then ElevatorDoorR(8).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 3), (ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e8 = True And ShaftNum = 2 Then ElevatorDoorL(18).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 3), (ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e8 = True And ShaftNum = 2 Then ElevatorDoorR(18).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 3), (ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e8 = True And ShaftNum = 3 Then ElevatorDoorL(28).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 3), (ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e8 = True And ShaftNum = 3 Then ElevatorDoorR(28).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 3), (ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e8 = True And ShaftNum = 4 Then ElevatorDoorL(38).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 3), (ShaftLeft + WallOffset), -22.95 + (15 * 3), 19.6, FloorAltitude, 1, 1
        If e8 = True And ShaftNum = 4 Then ElevatorDoorR(38).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 3), (ShaftLeft + WallOffset), -27.05 + (15 * 3), 19.6, FloorAltitude, 1, 1
        
        If e9 = True And ShaftNum = 1 Then ElevatorDoorL(9).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 4), -(ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e9 = True And ShaftNum = 1 Then ElevatorDoorR(9).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 4), -(ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e9 = True And ShaftNum = 2 Then ElevatorDoorL(19).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 4), -(ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e9 = True And ShaftNum = 2 Then ElevatorDoorR(19).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 4), -(ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e9 = True And ShaftNum = 3 Then ElevatorDoorL(29).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 4), -(ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e9 = True And ShaftNum = 3 Then ElevatorDoorR(29).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 4), -(ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e9 = True And ShaftNum = 4 Then ElevatorDoorL(39).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -19.05 + (15 * 4), -(ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e9 = True And ShaftNum = 4 Then ElevatorDoorR(39).AddWall GetTex("ElevDoors"), -(ShaftLeft + WallOffset), -23.05 + (15 * 4), -(ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
        
        If e10 = True And ShaftNum = 1 Then ElevatorDoorL(10).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 4), (ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e10 = True And ShaftNum = 1 Then ElevatorDoorR(10).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 4), (ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e10 = True And ShaftNum = 2 Then ElevatorDoorL(20).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 4), (ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e10 = True And ShaftNum = 2 Then ElevatorDoorR(20).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 4), (ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e10 = True And ShaftNum = 3 Then ElevatorDoorL(30).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 4), (ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e10 = True And ShaftNum = 3 Then ElevatorDoorR(30).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 4), (ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e10 = True And ShaftNum = 4 Then ElevatorDoorL(40).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -19.05 + (15 * 4), (ShaftLeft + WallOffset), -22.95 + (15 * 4), 19.6, FloorAltitude, 1, 1
        If e10 = True And ShaftNum = 4 Then ElevatorDoorR(40).AddWall GetTex("ElevDoors"), (ShaftLeft + WallOffset), -23.05 + (15 * 4), (ShaftLeft + WallOffset), -27.05 + (15 * 4), 19.6, FloorAltitude, 1, 1
            
Call CreateStairDoors(FloorID)
If StairDataTable(FloorID) = False Then CreateStairs (FloorID)
If FloorID < TopFloor And StairDataTable(FloorID + 1) = False Then CreateStairs (CameraFloor + 1)
If FloorID > BottomFloor And StairDataTable(FloorID - 1) = False Then CreateStairs (CameraFloor - 1)
                
End Sub

Sub Init_Objects(Floor As Integer, ObjectIndex As Integer)
'currently, 150 objects per floor (150*TopFloor)
i53 = ObjectIndex + (150 * (Floor - 1))
Set Objects(i53) = Scene.CreateMeshBuilder("Objects " + Str$(i53))
'MsgBox (Str$(i53))
'objectindex + (150 * (currentfloor - 1 ))
    
End Sub

Sub CreateStairDoors(FloorID As Integer)
Call DeleteStairDoors

'Stairway Doors
Dim jxx As Integer
For jxx = 1 To 4
Set StairDoor(jxx) = Scene.CreateMeshBuilder
Next jxx

For jxx = 1 To 4
Dim Endfloor As Integer
If jxx = 1 Then Endfloor = 138
If jxx = 2 Then Endfloor = 117
If jxx = 3 Then Endfloor = 79
If jxx = 4 Then Endfloor = 39
    
    Dim ShaftLeft As Single
    Dim ShaftRight As Single
    
    If jxx = 1 Then ShaftLeft = 12.5: ShaftRight = 32.5
    If jxx = 2 Then ShaftLeft = -52.5: ShaftRight = -32.5
    If jxx = 3 Then ShaftLeft = 90.5: ShaftRight = 110.5
    If jxx = 4 Then ShaftLeft = -130.5: ShaftRight = -110.5
    
    If FloorID < 2 Then StairDoor(jxx).AddWall GetTex("StairsDoor2"), -3.9, 0, 3.9, 0, 19.5, 0, 1, 1
    If FloorID >= 2 And FloorID <= Endfloor Then StairDoor(jxx).AddWall GetTex("StairsDoor"), -3.9, 0, 3.9, 0, 19.5, 0, 1, 1
    
    If FloorID <= Endfloor Then
    StairDoor(jxx).SetMeshName ("DoorSB " + Str$(jxx))
    StairDoor(jxx).SetRotation 0, 1.56, 0
    StairDoor(jxx).SetPosition -(ShaftLeft + 0.3), GetFloorAltitude(FloorID), -36.4
    End If
    
Next jxx

End Sub

Sub DeleteStairDoors()
Dim jxx As Integer
For jxx = 1 To 4
Scene.DestroyMesh StairDoor(jxx)
Set StairDoor(jxx) = Nothing
Next jxx
End Sub

Sub CreateStairs(FloorID As Integer)
Dim FloorHeight As Single
Dim FloorAltitude As Single
FloorHeight = GetFloorHeight(FloorID)
'FloorAltitude = GetFloorAltitude(FloorID)

    Set Stairs(FloorID) = Scene.CreateMeshBuilder("Stairs " + Str$(FloorID))
    Set FakeStairDoor(FloorID, 1) = Scene.CreateMeshBuilder("FakeStairDoor " + Str$(FloorID) + " 1")
    Set FakeStairDoor(FloorID, 2) = Scene.CreateMeshBuilder("FakeStairDoor " + Str$(FloorID) + " 2")
    Set FakeStairDoor(FloorID, 3) = Scene.CreateMeshBuilder("FakeStairDoor " + Str$(FloorID) + " 3")
    Set FakeStairDoor(FloorID, 4) = Scene.CreateMeshBuilder("FakeStairDoor " + Str$(FloorID) + " 4")
  
    StairDataTable(FloorID) = True
    
    Dim RiserHeight As Single
    RiserHeight = 2
    Dim ShaftNum As Integer
    Dim ShaftLeft As Single
    Dim ShaftRight As Single

Dim DoubleFloor As Integer

'Will loop for floors greater than 1 normal floor height
For DoubleFloor = 1 To (FloorHeight / 32)
FloorAltitude = GetFloorAltitude(FloorID) + ((DoubleFloor - 1) * 32)

For ShaftNum = 1 To 4
    If ShaftNum = 1 Then ShaftLeft = 12.5: ShaftRight = 32.5
    If ShaftNum = 2 Then ShaftLeft = -52.5: ShaftRight = -32.5
    If ShaftNum = 3 Then ShaftLeft = 90.5: ShaftRight = 110.5
    If ShaftNum = 4 Then ShaftLeft = -130.5: ShaftRight = -110.5
    
    'Stairway End Block Walls
    If FloorID = BottomFloor Then Stairs(BottomFloor).AddWall GetTex("Concrete"), -(ShaftLeft + 6), -46.25, -(ShaftLeft + 6), -46.25 + 7.71, (FloorHeight - 6), FloorAltitude, (7.71 * 0.086), (FloorHeight * 0.086)
    If ShaftNum = 1 And FloorID = TopFloor Then Stairs(TopFloor).AddWall GetTex("Concrete"), -12.5 - 6, -46.25 + 7.71, -12.5 - 6, -30.5, FloorHeight, FloorAltitude, (7.71 * 0.086), (FloorHeight * 0.086)
    
Dim Endfloor As Integer
If ShaftNum = 1 Then Endfloor = 138
If ShaftNum = 2 Then Endfloor = 117
If ShaftNum = 3 Then Endfloor = 79
If ShaftNum = 4 Then Endfloor = 39

    'Fake Stair Doors
    If FloorID <= Endfloor And DoubleFloor = 1 Then
        FakeStairDoor(FloorID, ShaftNum).AddWall GetTex("StairsDoor"), -3.9, 0, 3.9, 0, 19.5, 0, 1, 1
        FakeStairDoor(FloorID, ShaftNum).SetRotation 0, 1.56, 0
        FakeStairDoor(FloorID, ShaftNum).SetPosition -(ShaftLeft + 0.3), FloorAltitude, -36.4
    End If
    
    'Stairs
    DoEvents
    
    'Stairwell Walls
    Stairs(FloorID).AddWall GetTex("Concrete"), -(ShaftLeft + 0.5), -46.25 + 0.1, -(ShaftLeft + 0.5), -40.3, FloorHeight, FloorAltitude, ((46.25 - 40.3) * 0.086), (FloorHeight * 0.086)
    Stairs(FloorID).AddWall GetTex("Concrete"), -(ShaftLeft + 0.5), -32.5, -(ShaftLeft + 0.5), -30, FloorHeight, FloorAltitude, (2.5 * 0.086), (FloorHeight * 0.086)
    Stairs(FloorID).AddWall GetTex("Concrete"), -(ShaftRight - 0.1), -46.25 + 0.1, -(ShaftRight - 0.1), -30 - 1, FloorHeight, FloorAltitude, (16.25 * 0.086), (FloorHeight * 0.086)
    Stairs(FloorID).AddWall GetTex("Concrete"), -ShaftLeft, -46.25 + 0.1, -ShaftRight, -46.25 + 0.1, FloorHeight, FloorAltitude, (20 * 0.086), (FloorHeight * 0.086)
    Stairs(FloorID).AddWall GetTex("Concrete"), -ShaftLeft, -30 - 1, -ShaftRight, -30 - 1, FloorHeight, FloorAltitude, (20 * 0.086), (FloorHeight * 0.086)
    Stairs(FloorID).AddWall GetTex("Concrete"), -(ShaftLeft + 6), -46.25 + 7.71, -(ShaftLeft + 16), -46.25 + 7.71, FloorHeight, FloorAltitude, (10 * 0.086), (FloorHeight * 0.086)
    Stairs(FloorID).AddWall GetTex("Concrete"), -(ShaftLeft + 0.5), -40.3, -(ShaftLeft + 0.5), -30, (FloorHeight - 19.5), 19.5 + FloorAltitude, (10.3 * 0.086), ((FloorHeight - 19.5) * 0.08)
        
    'Stairs
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 6), -46.25 + 7.71, -(ShaftLeft + 6), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 0)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 7.5), -46.25 + 7.71, -(ShaftLeft + 7.5), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 1)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 9), -46.25 + 7.71, -(ShaftLeft + 9), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 2)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 10.5), -46.25 + 7.71, -(ShaftLeft + 10.5), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 3)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 12), -46.25 + 7.71, -(ShaftLeft + 12), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 4)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 13.5), -46.25 + 7.71, -(ShaftLeft + 13.5), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 5)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 15), -46.25 + 7.71, -(ShaftLeft + 15), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 6)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 16), -46.25 + 7.71, -(ShaftLeft + 16), -30.85, RiserHeight, FloorAltitude + (RiserHeight * 7)
    
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 6), -46.25 + 7.71, -(ShaftLeft + 7.5), -30.85, FloorAltitude + (RiserHeight * 1)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 7.5), -46.25 + 7.71, -(ShaftLeft + 9), -30.85, FloorAltitude + (RiserHeight * 2)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 9), -46.25 + 7.71, -(ShaftLeft + 10.5), -30.85, FloorAltitude + (RiserHeight * 3)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 10.5), -46.25 + 7.71, -(ShaftLeft + 12), -30.85, FloorAltitude + (RiserHeight * 4)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 12), -46.25 + 7.71, -(ShaftLeft + 13.5), -30.85, FloorAltitude + (RiserHeight * 5)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 13.5), -46.25 + 7.71, -(ShaftLeft + 15), -30.85, FloorAltitude + (RiserHeight * 6)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 15), -46.25 + 7.71, -(ShaftLeft + 16), -30.85, FloorAltitude + (RiserHeight * 7)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 16), -46.25, -(ShaftLeft + 20), -30.85, FloorAltitude + (RiserHeight * 8)
    
    Stairs(FloorID).AddFloor GetTex("stairs"), -ShaftLeft, -46.25, -(ShaftLeft + 6), -30.85, FloorAltitude + FloorHeight
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 6), -46.25 + 7.71, -(ShaftLeft + 7.5), -46.25, FloorAltitude + (RiserHeight * 15)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 7.5), -46.25 + 7.71, -(ShaftLeft + 9), -46.25, FloorAltitude + (RiserHeight * 14)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 9), -46.25 + 7.71, -(ShaftLeft + 10.5), -46.25, FloorAltitude + (RiserHeight * 13)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 10.5), -46.25 + 7.71, -(ShaftLeft + 12), -46.25, FloorAltitude + (RiserHeight * 12)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 12), -46.25 + 7.71, -(ShaftLeft + 13.5), -46.25, FloorAltitude + (RiserHeight * 11)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 13.5), -46.25 + 7.71, -(ShaftLeft + 15), -46.25, FloorAltitude + (RiserHeight * 10)
    Stairs(FloorID).AddFloor GetTex("stairs"), -(ShaftLeft + 15), -46.25 + 7.71, -(ShaftLeft + 16), -46.25, FloorAltitude + (RiserHeight * 9)
    
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 6), -46.25, -(ShaftLeft + 6), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 15)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 7.5), -46.25, -(ShaftLeft + 7.5), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 14)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 9), -46.25, -(ShaftLeft + 9), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 13)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 10.5), -46.25, -(ShaftLeft + 10.5), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 12)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 12), -46.25, -(ShaftLeft + 12), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 11)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 13.5), -46.25, -(ShaftLeft + 13.5), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 10)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 15), -46.25, -(ShaftLeft + 15), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 9)
    Stairs(FloorID).AddWall GetTex("stairs"), -(ShaftLeft + 16), -46.25, -(ShaftLeft + 16), -46.25 + 7.71, RiserHeight, FloorAltitude + (RiserHeight * 8)
    
    Dim TextureName As String
    
    If DoubleFloor = 1 Then
    'Floor Signs
    Stairs(FloorID).AddWall GetTex("FloorSign"), -(ShaftLeft + 0.52), -42.5, -(ShaftLeft + 0.52), -44.5, 0.5, FloorAltitude + 11 - 0.4, 1, 1
    Stairs(FloorID).AddWall GetTex("Button" + GetFloorName(FloorID)), -(ShaftLeft + 0.51), -42.5, -(ShaftLeft + 0.51), -44.5, 1.5, 9.5, 1, 1
    If FloorID = 0 Then TextureName = "FloorSignLobby"
    If FloorID = 1 Then TextureName = "FloorSignMez"
    If FloorID >= 2 And FloorID <= 79 Then TextureName = "FloorSignOffices"
    If FloorID = 80 Then TextureName = "FloorSignSkylobby"
    If FloorID >= 81 And FloorID <= 99 Then TextureName = "FloorSignHotel"
    If FloorID >= 100 And FloorID <= 114 Then TextureName = "FloorSignResidential"
    If FloorID >= 115 And FloorID <= 117 Then TextureName = "FloorSignMaint"
    If FloorID >= 118 And FloorID <= 129 Then TextureName = "FloorSignResidential"
    If FloorID = 130 Or FloorID = 131 Then TextureName = "FloorSignMaint"
    If FloorID = 132 Then TextureName = "FloorSignObservatory"
    If FloorID = 133 Then TextureName = "FloorSignMaint"
    If FloorID = 134 Then TextureName = "FloorSignPool"
    If FloorID = 135 Then TextureName = "FloorSignBallroom"
    If FloorID = 136 Then TextureName = "FloorSignBalcony"
    If FloorID = 137 Then TextureName = "FloorSignMechanical"
    If FloorID = 138 Then TextureName = "FloorSignRoof"
    Stairs(FloorID).AddWall GetTex(TextureName), -(ShaftLeft + 0.52), -42.5, -(ShaftLeft + 0.52), -44.5, 0.5, 9 + 0.3, 1, 1
    End If
    
Next ShaftNum
Next DoubleFloor
    
End Sub

Sub DeleteStairs(FloorID As Integer)
'MsgBox ("DeleteStairs" & FloorID)

Scene.DestroyMesh Stairs(FloorID)
Scene.DestroyMesh FakeStairDoor(FloorID, 1)
Scene.DestroyMesh FakeStairDoor(FloorID, 2)
Scene.DestroyMesh FakeStairDoor(FloorID, 3)
Scene.DestroyMesh FakeStairDoor(FloorID, 4)
Set Stairs(FloorID) = Nothing
Set FakeStairDoor(FloorID, 1) = Nothing
Set FakeStairDoor(FloorID, 2) = Nothing
Set FakeStairDoor(FloorID, 3) = Nothing
Set FakeStairDoor(FloorID, 4) = Nothing
StairDataTable(FloorID) = False
    
End Sub

Sub DestroyObjects(Floor As Integer)
'currently, 150 objects per floor (150*TopFloor)
For i = (1 + (150 * (Floor - 1))) To (150 + (150 * (Floor - 1)))
'The destroymesh function is broken
On Error Resume Next
Objects(i).Enable False
Scene.DestroyMesh Objects(i)
Set Objects(i) = Nothing
Next i

'objectindex + (150 * (currentfloor - 1 ))

End Sub

