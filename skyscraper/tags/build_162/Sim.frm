VERSION 5.00
Begin VB.Form Sim 
   BackColor       =   &H80000007&
   Caption         =   "SkyScraper"
   ClientHeight    =   7920
   ClientLeft      =   2025
   ClientTop       =   1860
   ClientWidth     =   10440
   Icon            =   "Sim.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   7920
   ScaleWidth      =   10440
   StartUpPosition =   2  'CenterScreen
   Begin VB.Timer IntroMusic 
      Enabled         =   0   'False
      Interval        =   10
      Left            =   2880
      Top             =   360
   End
   Begin VB.Timer Timer31 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1200
      Top             =   5400
   End
   Begin VB.Timer Timer32 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1800
      Top             =   5400
   End
   Begin VB.Timer Timer33 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   2400
      Top             =   5400
   End
   Begin VB.Timer Timer34 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3000
      Top             =   5400
   End
   Begin VB.Timer Timer35 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3600
      Top             =   5400
   End
   Begin VB.Timer Timer36 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4200
      Top             =   5400
   End
   Begin VB.Timer Timer37 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4800
      Top             =   5400
   End
   Begin VB.Timer Timer38 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   5400
      Top             =   5400
   End
   Begin VB.Timer Timer39 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6000
      Top             =   5400
   End
   Begin VB.Timer Timer40 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6600
      Top             =   5400
   End
   Begin VB.Timer Timer21 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1200
      Top             =   4920
   End
   Begin VB.Timer Timer22 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1800
      Top             =   4920
   End
   Begin VB.Timer Timer23 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   2400
      Top             =   4920
   End
   Begin VB.Timer Timer24 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3000
      Top             =   4920
   End
   Begin VB.Timer Timer25 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3600
      Top             =   4920
   End
   Begin VB.Timer Timer26 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4200
      Top             =   4920
   End
   Begin VB.Timer Timer27 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4800
      Top             =   4920
   End
   Begin VB.Timer Timer28 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   5400
      Top             =   4920
   End
   Begin VB.Timer Timer29 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6000
      Top             =   4920
   End
   Begin VB.Timer Timer30 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6600
      Top             =   4920
   End
   Begin VB.Timer Timer11 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1200
      Top             =   4440
   End
   Begin VB.Timer Timer12 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1800
      Top             =   4440
   End
   Begin VB.Timer Timer13 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   2400
      Top             =   4440
   End
   Begin VB.Timer Timer14 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3000
      Top             =   4440
   End
   Begin VB.Timer Timer15 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3600
      Top             =   4440
   End
   Begin VB.Timer Timer16 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4200
      Top             =   4440
   End
   Begin VB.Timer Timer17 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4800
      Top             =   4440
   End
   Begin VB.Timer Timer18 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   5400
      Top             =   4440
   End
   Begin VB.Timer Timer19 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6000
      Top             =   4440
   End
   Begin VB.Timer Timer20 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6600
      Top             =   4440
   End
   Begin VB.Timer Elevator40Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6600
      Top             =   3240
   End
   Begin VB.Timer Elevator39Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6000
      Top             =   3240
   End
   Begin VB.Timer Elevator38Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   5400
      Top             =   3240
   End
   Begin VB.Timer Elevator37Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4800
      Top             =   3240
   End
   Begin VB.Timer Elevator36Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4200
      Top             =   3240
   End
   Begin VB.Timer Elevator35Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3600
      Top             =   3240
   End
   Begin VB.Timer Elevator34Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3000
      Top             =   3240
   End
   Begin VB.Timer Elevator33Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   2400
      Top             =   3240
   End
   Begin VB.Timer Elevator32Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1800
      Top             =   3240
   End
   Begin VB.Timer Elevator31Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1200
      Top             =   3240
   End
   Begin VB.Timer Elevator30Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6600
      Top             =   2760
   End
   Begin VB.Timer Elevator29Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6000
      Top             =   2760
   End
   Begin VB.Timer Elevator28Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   5400
      Top             =   2760
   End
   Begin VB.Timer Elevator27Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4800
      Top             =   2760
   End
   Begin VB.Timer Elevator26Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4200
      Top             =   2760
   End
   Begin VB.Timer Elevator25Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3600
      Top             =   2760
   End
   Begin VB.Timer Elevator24Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3000
      Top             =   2760
   End
   Begin VB.Timer Elevator23Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   2400
      Top             =   2760
   End
   Begin VB.Timer Elevator22Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1800
      Top             =   2760
   End
   Begin VB.Timer Elevator21Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1200
      Top             =   2760
   End
   Begin VB.Timer Elevator20Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6600
      Top             =   2280
   End
   Begin VB.Timer Elevator19Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6000
      Top             =   2280
   End
   Begin VB.Timer Elevator18Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   5400
      Top             =   2280
   End
   Begin VB.Timer Elevator17Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4800
      Top             =   2280
   End
   Begin VB.Timer Elevator16Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4200
      Top             =   2280
   End
   Begin VB.Timer Elevator15Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3600
      Top             =   2280
   End
   Begin VB.Timer Elevator14Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3000
      Top             =   2280
   End
   Begin VB.Timer Elevator13Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   2400
      Top             =   2280
   End
   Begin VB.Timer Elevator12Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1800
      Top             =   2280
   End
   Begin VB.Timer Elevator11Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1200
      Top             =   2280
   End
   Begin VB.Timer Timer10 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6600
      Top             =   3960
   End
   Begin VB.Timer Timer9 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   6000
      Top             =   3960
   End
   Begin VB.Timer Timer8 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   5400
      Top             =   3960
   End
   Begin VB.Timer Timer7 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4800
      Top             =   3960
   End
   Begin VB.Timer Timer6 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4200
      Top             =   3960
   End
   Begin VB.Timer Timer5 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3600
      Top             =   3960
   End
   Begin VB.Timer Timer4 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   3000
      Top             =   3960
   End
   Begin VB.Timer Timer3 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   2400
      Top             =   3960
   End
   Begin VB.Timer Timer2 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1800
      Top             =   3960
   End
   Begin VB.Timer Elevator10Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6600
      Top             =   1800
   End
   Begin VB.Timer Elevator9Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   6000
      Top             =   1800
   End
   Begin VB.Timer Elevator8Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   5400
      Top             =   1800
   End
   Begin VB.Timer Elevator7Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4800
      Top             =   1800
   End
   Begin VB.Timer Elevator6Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4200
      Top             =   1800
   End
   Begin VB.Timer Elevator5Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3600
      Top             =   1800
   End
   Begin VB.Timer Elevator4Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3000
      Top             =   1800
   End
   Begin VB.Timer Elevator3Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   2400
      Top             =   1800
   End
   Begin VB.Timer Elevator2Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1800
      Top             =   1800
   End
   Begin VB.Timer StairsTimer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   2160
      Top             =   360
   End
   Begin VB.Timer Elevator1Timer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1200
      Top             =   1800
   End
   Begin VB.Timer MainTimer 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   960
      Top             =   360
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   1200
      Top             =   3960
   End
   Begin VB.Label Label2 
      BackColor       =   &H80000007&
      ForeColor       =   &H80000009&
      Height          =   1215
      Left            =   120
      TabIndex        =   1
      Top             =   2040
      Width           =   6495
   End
   Begin VB.Label Label1 
      BackColor       =   &H80000007&
      ForeColor       =   &H80000009&
      Height          =   1815
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   6495
   End
End
Attribute VB_Name = "Sim"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'Skycraper 0.97 Beta - Simulation Window
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

Option Explicit

Private isRunning As Boolean
Public scr As TVScreen2DText
Public Screen As TVScreen2DImmediate
Public RollOnTexture As Integer
Public Explosion As TVMesh
Public Fin As Integer

Public bMain As Boolean, bMulti As Boolean, bOptions As Boolean, bSing As Boolean

Public mx As Long, my As Long, b1 As Integer
Sub Main()
    DrawMenu
    Inp.GetAbsMouseState mx, my, b1
    
    'Single Player
    If IsOnButton((Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2), (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 50) = True Then
    
      If b1 = 128 Then
        Effect.FadeIn 1000
        Sing
      End If
    End If
    
    'Multiplayer
    If IsOnButton((Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 50, (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 100) = True Then
      If b1 = 128 Then
        Effect.FadeIn 1000
        'Multi
      End If
    End If
    
    'Options
    If IsOnButton((Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 100, (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 150) = True Then
      If b1 = 128 Then
        Effect.FadeIn 1000
        'Options
      End If
    End If
            
    Dim blahmx As String, blahmy As String
    blahmx = mx
    blahmy = my
    'scr.DrawText "X = " & blahmx, 10, 10, RGBA(1, 0, 0, 1), "menu"
    'scr.DrawText "Y = " & blahmy, 10, 25, RGBA(1, 0, 0, 1), "menu"
End Sub


Public Sub DrawMenu()
  Dim i As Long
  Dim b(5) As Boolean

  'b(0) = Screen.DRAW_RollOver(GetTex("sp2"), GetTex("sp1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2), (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 50)
  'b(1) = Screen.DRAW_RollOver(GetTex("mp2"), GetTex("mp1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 50, (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 100)
  'b(2) = Screen.DRAW_RollOver(GetTex("op2"), GetTex("op1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 100, (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 150)
  'b(2) = Screen.DRAW_RollOver(GetTex("qt2"), GetTex("qt1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 150, (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 200)
  b(0) = Screen.DRAW_RollOver(GetTex("sp2"), GetTex("sp1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2), (Me.ScaleWidth / 2) + 175, (Me.ScaleHeight / 2) + 50)
  b(1) = Screen.DRAW_RollOver(GetTex("mp2"), GetTex("mp1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 50, (Me.ScaleWidth / 2) + 175, (Me.ScaleHeight / 2) + 100)
  b(2) = Screen.DRAW_RollOver(GetTex("op2"), GetTex("op1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 100, (Me.ScaleWidth / 2) + 175, (Me.ScaleHeight / 2) + 150)
  b(2) = Screen.DRAW_RollOver(GetTex("qt2"), GetTex("qt1"), (Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 150, (Me.ScaleWidth / 2) + 175, (Me.ScaleHeight / 2) + 200)
  For i = 0 To 3
    If b(i) = True Then RollOnTexture = i
  Next i
End Sub
Public Function IsOnButton(X, Y, x2, Y2) As Boolean
  IsOnButton = False
  If mx > X Then
    If mx < x2 Then
      If my > Y Then
        If my < Y2 Then
          IsOnButton = True
        End If
      End If
    End If
  End If
End Function
Sub Multi()
bOptions = False
bMain = False
bMulti = True
bSing = False
    
    
    
    scr.TextureFont_DrawText "Multiplayer Here", Me.ScaleWidth / 2, Me.ScaleHeight / 2, RGBA(1, 0, 0, 1), "Multi"
    scr.TextureFont_DrawText "Back", Me.ScaleWidth / 2, Me.ScaleHeight / 2 + 50, RGBA(1, 0, 0, 1), "Back"
    Inp.GetAbsMouseState mx, my, b1
    
    If IsOnButton(Me.ScaleWidth / 2, Me.ScaleHeight / 2 + 50, Me.ScaleWidth / 2 + 125, Me.ScaleHeight / 2 + 100) = True Then
      If b1 = 128 Then
      Effect.FadeIn 1000
       bOptions = False
       bMain = True
       bMulti = False
       bSing = False
      End If
    End If
    
    Dim blahmx As String, blahmy As String
    blahmx = mx
    blahmy = my
    'scr.DrawText "X = " & blahmx, 10, 10, RGBA(1, 0, 0, 1), "menu"
    'scr.DrawText "Y = " & blahmy, 10, 25, RGBA(1, 0, 0, 1), "menu"
End Sub
Sub Options()
bOptions = True
bMain = False
bMulti = False
bSing = False
    
    
    
    scr.TextureFont_DrawText "Options Here", Me.ScaleWidth / 2, Me.ScaleHeight / 2, RGBA(1, 0, 0, 1), "Options"
    scr.TextureFont_DrawText "Back", Me.ScaleWidth / 2, Me.ScaleHeight / 2 + 50, RGBA(1, 0, 0, 1), "Back"
    Inp.GetAbsMouseState mx, my, b1
    
    If IsOnButton(Me.ScaleWidth / 2, Me.ScaleHeight / 2 + 50, Me.ScaleWidth / 2 + 125, Me.ScaleHeight / 2 + 100) = True Then
      If b1 = 128 Then
      Effect.FadeIn 1000
       bOptions = False
       bMain = True
       bMulti = False
       bSing = False
      End If
    End If
    
    Dim blahmx As String, blahmy As String
    blahmx = mx
    blahmy = my
    'scr.DrawText "X = " & blahmx, 10, 10, RGBA(1, 0, 0, 1), "menu"
    'scr.DrawText "Y = " & blahmy, 10, 25, RGBA(1, 0, 0, 1), "menu"
End Sub
Sub Sing()
bOptions = False
bMain = False
bMulti = False
bSing = True
    
'Start with no other menus
Fin = 1
      'Effect.FadeIn 1000
      isRunning = False
        Set scr = Nothing
        Set Screen = Nothing
        Sim.Refresh
        Start
        Exit Sub
'End of new start
   
    scr.TextureFont_DrawText "Single Player Here", Me.ScaleWidth / 2, Me.ScaleHeight / 2, RGBA(1, 0, 0, 1), "Single"
    scr.TextureFont_DrawText "Back", Me.ScaleWidth / 2, Me.ScaleHeight / 2 + 50, RGBA(1, 0, 0, 1), "Back"
    Inp.GetAbsMouseState mx, my, b1
    
    If IsOnButton(Me.ScaleWidth / 2, Me.ScaleHeight / 2 + 50, Me.ScaleWidth / 2 + 125, Me.ScaleHeight / 2 + 100) = True Then
      If b1 = 128 Then
      Effect.FadeIn 1000
       bOptions = False
       bMain = True
       bMulti = False
       bSing = False
      End If
    End If
    
    Dim blahmx As String, blahmy As String
    blahmx = mx
    blahmy = my
    'scr.DrawText "X = " & blahmx, 10, 10, RGBA(1, 0, 0, 1), "menu"
    'scr.DrawText "Y = " & blahmy, 10, 25, RGBA(1, 0, 0, 1), "menu"
End Sub










Private Sub Elevator10Timer_Timer()
Call ElevatorLoop(10)

End Sub

Private Sub Elevator11Timer_Timer()
Call ElevatorLoop(11)

End Sub

Private Sub Elevator12Timer_Timer()
Call ElevatorLoop(12)

End Sub

Private Sub Elevator13Timer_Timer()
Call ElevatorLoop(13)

End Sub

Private Sub Elevator14Timer_Timer()
Call ElevatorLoop(14)

End Sub

Private Sub Elevator15Timer_Timer()
Call ElevatorLoop(15)

End Sub

Private Sub Elevator16Timer_Timer()
Call ElevatorLoop(16)

End Sub

Private Sub Elevator17Timer_Timer()
Call ElevatorLoop(17)

End Sub

Private Sub Elevator18Timer_Timer()
Call ElevatorLoop(18)

End Sub

Private Sub Elevator19Timer_Timer()
Call ElevatorLoop(19)

End Sub

Private Sub Elevator1Timer_Timer()
Call ElevatorLoop(1)
End Sub

Private Sub Elevator20Timer_Timer()
Call ElevatorLoop(20)

End Sub

Private Sub Elevator21Timer_Timer()
Call ElevatorLoop(21)

End Sub

Private Sub Elevator22Timer_Timer()
Call ElevatorLoop(22)

End Sub

Private Sub Elevator23Timer_Timer()
Call ElevatorLoop(23)

End Sub

Private Sub Elevator24Timer_Timer()
Call ElevatorLoop(24)

End Sub

Private Sub Elevator25Timer_Timer()
Call ElevatorLoop(25)

End Sub

Private Sub Elevator26Timer_Timer()
Call ElevatorLoop(26)

End Sub

Private Sub Elevator27Timer_Timer()
Call ElevatorLoop(27)

End Sub

Private Sub Elevator28Timer_Timer()
Call ElevatorLoop(28)

End Sub

Private Sub Elevator29Timer_Timer()
Call ElevatorLoop(29)

End Sub

Private Sub Elevator2Timer_Timer()
Call ElevatorLoop(2)

End Sub

Private Sub Elevator30Timer_Timer()
Call ElevatorLoop(30)

End Sub

Private Sub Elevator31Timer_Timer()
Call ElevatorLoop(31)

End Sub

Private Sub Elevator32Timer_Timer()
Call ElevatorLoop(32)

End Sub

Private Sub Elevator33Timer_Timer()
Call ElevatorLoop(33)

End Sub

Private Sub Elevator34Timer_Timer()
Call ElevatorLoop(34)

End Sub

Private Sub Elevator35Timer_Timer()
Call ElevatorLoop(35)

End Sub

Private Sub Elevator36Timer_Timer()
Call ElevatorLoop(36)

End Sub

Private Sub Elevator37Timer_Timer()
Call ElevatorLoop(37)

End Sub

Private Sub Elevator38Timer_Timer()
Call ElevatorLoop(38)

End Sub

Private Sub Elevator39Timer_Timer()
Call ElevatorLoop(39)

End Sub

Private Sub Elevator3Timer_Timer()
Call ElevatorLoop(3)

End Sub

Private Sub Elevator40Timer_Timer()
Call ElevatorLoop(40)

End Sub

Private Sub Elevator4Timer_Timer()
Call ElevatorLoop(4)

End Sub

Private Sub Elevator5Timer_Timer()
Call ElevatorLoop(5)

End Sub

Private Sub Elevator6Timer_Timer()
Call ElevatorLoop(6)

End Sub

Private Sub Elevator7Timer_Timer()
Call ElevatorLoop(7)

End Sub

Private Sub Elevator8Timer_Timer()
Call ElevatorLoop(8)

End Sub

Private Sub Elevator9Timer_Timer()
Call ElevatorLoop(9)

End Sub

Private Sub Form_GotFocus()
Focused = True
End Sub






Public Sub Form_Load()
On Error GoTo ErrorHandler
Sim.ScaleWidth = 671
Sim.ScaleMode = 3
Sim.ScaleHeight = 504

  Set TV = New TVEngine
  Set Scene = New TVScene
  Set scr = New TVScreen2DText
  Set TextureFactory = New TVTextureFactory
  Set Effect = New TVGraphicEffect
  Set SoundEngine = New TV3DMedia.TVSoundEngine
  Set Screen = New TVScreen2DImmediate
  Set MainMusic = New TV3DMedia.TVSoundMP3
  Dim Fin As Integer
  isRunning = True
  Show
  DoEvents
  'TV.EnableHardwareTL True
  If TV.ShowDriverDialog = False Then End
  DoEvents
  TV.Initialize Me.hWnd
  
  Set Inp = New TVInputEngine
  'TV.SetSearchDirectory "data"
  Call SoundEngine.Init(Sim.hWnd)

  TextureFactory.LoadTexture App.Path + "\data\mp1.bmp", "mp1", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\mp2.bmp", "mp2", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\op1.bmp", "op1", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\op2.bmp", "op2", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\qt1.bmp", "qt1", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\qt2.bmp", "qt2", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\sp1.bmp", "sp1", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\sp2.bmp", "sp2", , , TV_COLORKEY_BLACK
  TextureFactory.LoadTexture App.Path + "\data\background.jpg", "Image"
  TextureFactory.LoadTexture App.Path + "\data\skyscraper.jpg", "Title", , , TV_COLORKEY_BLACK

  Effect.FadeIn 1500
  Scene.LoadCursor App.Path + "\data\pointer.bmp", TV_COLORKEY_BLACK, 24, 24
  
   'Scene.LoadShaders App.Path + "\data\common.shader"
   
  Scene.SetCamera 25, 0, -50, 25, 0, 0
  
  bMain = True
  
  'font stuff
  scr.TextureFont_Create "Options", "Arial", 10, True, False, False
    scr.TextureFont_Create "Back", "Arial", 20, True, False, False
    scr.TextureFont_Create "Single", "Arial", 10, True, False, False
     scr.TextureFont_Create "Multi", "Arial", 10, True, False, False
     scr.TextureFont_Create "Menu", "Arial", 10, True, False, False

  MainMusic.Load App.Path + "\data\intro.mp3"
  IntroMusic.Enabled = True
  
  Do Until Effect.FadeFinished = True And isRunning = False
    
  Dim TexInfo As TV_TEXTURE
  TexInfo = TextureFactory.GetTextureInfo(GetTex("Title"))
  
    TV.Clear
    Screen.DRAW_Texture GetTex("Image"), 0, 0, Me.ScaleWidth, Me.ScaleHeight
    Screen.DRAW_Texture GetTex("Title"), (Me.ScaleWidth / 2) - (TexInfo.Width / 3), (Me.ScaleHeight / 2) - (TexInfo.Height / 3) - 150, (Me.ScaleWidth / 2) + (TexInfo.Width / 3), (Me.ScaleHeight / 2) + (TexInfo.Height / 3) - 150
    'scr.CreateUserFont "menu", "Arial", 10, True, False, False
    
    
    Scene.RenderAllMeshes
    
    If bMain = True Then
     Main
    End If
    
    If bSing = True Then
     'Sing
     Exit Sub
    End If
    
    If bMulti = True Then
     Multi
    End If
    
    If bOptions = True Then
     Options
    End If
    
    If bMain = True Then
     If IsOnButton((Me.ScaleWidth / 2) - 30, (Me.ScaleHeight / 2) + 150, (Me.ScaleWidth / 2) + 105, (Me.ScaleHeight / 2) + 200) = True Then
      If b1 = 128 Then
       Fin = 1
       Effect.FadeOut 1000
       isRunning = False
      End If
     End If
    End If
    
    TV.RenderToScreen
    
    If Inp.IsKeyPressed(TV_KEY_ESCAPE) = True Then
      Fin = 1
      
      isRunning = False
        Set scr = Nothing
        Set Screen = Nothing
           Set Effect = Nothing
        MainMusic.Stop_
        End
        Exit Sub
    End If
    If Inp.IsKeyPressed(TV_KEY_RETURN) = True Then
      Fin = 1
      
      isRunning = False
        Set scr = Nothing
        Set Screen = Nothing
        Sim.Refresh
        'start simulator
        Start
        Exit Sub
    End If
  DoEvents
  Loop
    Set Effect = Nothing
  MainMusic.Stop_
  End
  
ErrorHandler:
   MsgBox "An error occurred loading and initializing the TrueVision3D graphics engine." + vbCrLf + "Skyscraper requires TrueVision3D 6.2 to be installed." + vbCrLf + "It can be obtained at http://www.truevision3d.com" + vbCrLf + "Make sure it is installed properly and that all the libraries exist." + vbCrLf + vbCrLf + "Error source: " + Err.Source + vbCrLf + "Description: " + Err.Description, vbCritical
   End

End Sub

Private Sub Form_LostFocus()
Focused = False
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
isRunning = False
End Sub

Private Sub Form_Resize()
TV.ResizeDevice
End Sub

Private Sub Form_Unload(Cancel As Integer)

    Call CleanUp
    End
End Sub

Private Sub IntroMusic_Timer()

MainMusic.Play

End Sub

Private Sub MainTimer_Timer()
Call MainLoop
End Sub

Sub StairsTimer_Timer()
Call StairsLoop
End Sub

Private Sub Timer1_Timer()
OpenElevator(1) = -1
Timer1.Enabled = False
End Sub

Private Sub Timer10_Timer()
OpenElevator(10) = -1
Timer10.Enabled = False

End Sub

Private Sub Timer11_Timer()
OpenElevator(11) = -1
Timer11.Enabled = False

End Sub

Private Sub Timer12_Timer()
OpenElevator(12) = -1
Timer12.Enabled = False

End Sub

Private Sub Timer13_Timer()
OpenElevator(13) = -1
Timer13.Enabled = False

End Sub

Private Sub Timer14_Timer()
OpenElevator(14) = -1
Timer14.Enabled = False

End Sub

Private Sub Timer15_Timer()
OpenElevator(15) = -1
Timer15.Enabled = False

End Sub

Private Sub Timer16_Timer()
OpenElevator(16) = -1
Timer16.Enabled = False

End Sub

Private Sub Timer17_Timer()
OpenElevator(17) = -1
Timer17.Enabled = False

End Sub

Private Sub Timer18_Timer()
OpenElevator(18) = -1
Timer18.Enabled = False

End Sub

Private Sub Timer19_Timer()
OpenElevator(19) = -1
Timer19.Enabled = False

End Sub

Private Sub Timer2_Timer()
OpenElevator(2) = -1
Timer2.Enabled = False

End Sub

Private Sub Timer20_Timer()
OpenElevator(20) = -1
Timer20.Enabled = False

End Sub

Private Sub Timer21_Timer()
OpenElevator(21) = -1
Timer21.Enabled = False

End Sub

Private Sub Timer22_Timer()
OpenElevator(22) = -1
Timer22.Enabled = False

End Sub

Private Sub Timer23_Timer()
OpenElevator(23) = -1
Timer23.Enabled = False

End Sub

Private Sub Timer24_Timer()
OpenElevator(24) = -1
Timer24.Enabled = False

End Sub

Private Sub Timer25_Timer()
OpenElevator(25) = -1
Timer25.Enabled = False

End Sub

Private Sub Timer26_Timer()
OpenElevator(26) = -1
Timer26.Enabled = False

End Sub

Private Sub Timer27_Timer()
OpenElevator(27) = -1
Timer27.Enabled = False

End Sub

Private Sub Timer28_Timer()
OpenElevator(28) = -1
Timer28.Enabled = False

End Sub

Private Sub Timer29_Timer()
OpenElevator(29) = -1
Timer29.Enabled = False

End Sub

Private Sub Timer3_Timer()
OpenElevator(3) = -1
Timer3.Enabled = False

End Sub

Private Sub Timer30_Timer()
OpenElevator(30) = -1
Timer30.Enabled = False

End Sub

Private Sub Timer31_Timer()
OpenElevator(31) = -1
Timer31.Enabled = False

End Sub

Private Sub Timer32_Timer()
OpenElevator(32) = -1
Timer32.Enabled = False

End Sub

Private Sub Timer33_Timer()
OpenElevator(33) = -1
Timer33.Enabled = False

End Sub

Private Sub Timer34_Timer()
OpenElevator(34) = -1
Timer34.Enabled = False

End Sub

Private Sub Timer35_Timer()
OpenElevator(35) = -1
Timer35.Enabled = False

End Sub

Private Sub Timer36_Timer()
OpenElevator(36) = -1
Timer36.Enabled = False

End Sub

Private Sub Timer37_Timer()
OpenElevator(37) = -1
Timer37.Enabled = False

End Sub

Private Sub Timer38_Timer()
OpenElevator(38) = -1
Timer38.Enabled = False

End Sub

Private Sub Timer39_Timer()
OpenElevator(39) = -1
Timer39.Enabled = False

End Sub

Private Sub Timer4_Timer()
OpenElevator(4) = -1
Timer4.Enabled = False

End Sub

Private Sub Timer40_Timer()
OpenElevator(40) = -1
Timer40.Enabled = False

End Sub

Private Sub Timer5_Timer()
OpenElevator(5) = -1
Timer5.Enabled = False

End Sub

Private Sub Timer6_Timer()
OpenElevator(6) = -1
Timer6.Enabled = False

End Sub

Private Sub Timer7_Timer()
OpenElevator(7) = -1
Timer7.Enabled = False

End Sub

Private Sub Timer8_Timer()
OpenElevator(8) = -1
Timer8.Enabled = False

End Sub

Private Sub Timer9_Timer()
OpenElevator(9) = -1
Timer9.Enabled = False

End Sub
