; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Skyscraper"
!define PRODUCT_VERSION "2.0 Alpha 6"
!define PRODUCT_PUBLISHER "Ryan Thoryk"
!define PRODUCT_WEB_SITE "http://www.skyscrapersim.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Skyscraper.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; Location where Skyscraper files are stored
;example: !define LOCAL_FILES "${LOCAL_FILES}"
!define LOCAL_FILES "..\..\"
!define WINDOWS_DIR "C:\WINDOWS"
!define SYSTEM_DIR "C:\WINDOWS\System32"

SetCompressor lzma
VIAddVersionKey ProductName "Skyscraper"
VIAddVersionKey FileDescription "Skyscraper"
VIAddVersionKey CompanyName "TLI Networks"
VIAddVersionKey LegalCopyright "�2003-2010 Ryan Thoryk"
VIAddVersionKey FileVersion "1.6.0.0"
VIAddVersionKey ProductVersion "1.6.0.0"
VIProductVersion 1.6.0.0

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${LOCAL_FILES}\skyscraper.ico"
!define MUI_UNICON "${LOCAL_FILES}\skyscraper.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${LOCAL_FILES}\tools\installer\install.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${LOCAL_FILES}\tools\installer\install.bmp"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "${LOCAL_FILES}\gpl.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Skyscraper"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
;!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!define MUI_FINISHPAGE_RUN "$INSTDIR\Skyscraper.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "skyscraper20alpha6.exe"
InstallDir "$PROGRAMFILES\Skyscraper"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "Application" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "${LOCAL_FILES}\Skyscraper.exe"
  File "${LOCAL_FILES}\SBS.dll"
  File "${LOCAL_FILES}\Skyscraper.pdb"
  File "${LOCAL_FILES}\SBS.pdb"
  File "${LOCAL_FILES}\skyscraper.ini"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Skyscraper.lnk" "$INSTDIR\Skyscraper.exe"
  CreateShortCut "$DESKTOP\Skyscraper.lnk" "$INSTDIR\Skyscraper.exe"
  File "${LOCAL_FILES}\readme.txt"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Release Notes.lnk" "write.exe" '"$INSTDIR\readme.txt"'
  File "${LOCAL_FILES}\designguide.html"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Building Design Guide.lnk" "$INSTDIR\designguide.html"
  File "${LOCAL_FILES}\changelog.txt"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Changelog.lnk" "write.exe" '"$INSTDIR\changelog.txt"'
  File "${LOCAL_FILES}\design.rtf"
  SetOutPath "$INSTDIR\buildings"
  File "${LOCAL_FILES}\buildings\*.bld"
  SetOutPath "$INSTDIR\guide"
  File "${LOCAL_FILES}\guide\*.*"
  SetOutPath "$INSTDIR\data"
  File "${LOCAL_FILES}\data\*.*"
  SetOutPath "$INSTDIR\data\triton_signs"
  File "${LOCAL_FILES}\data\triton_signs\*.*"
  SetOutPath "$INSTDIR\data\fonts"
  File "${LOCAL_FILES}\data\fonts\*.*"
  SetOutPath "$INSTDIR\data\MT"
  File "${LOCAL_FILES}\data\MT\*.*"
  SetOutPath "$INSTDIR\data\scripts"
  File "${LOCAL_FILES}\data\scripts\*.*"
SectionEnd

Section /o "Source Code" SEC02
  SetOutPath "$INSTDIR"
  File "${LOCAL_FILES}\skyscraper.ico"
  File "${LOCAL_FILES}\gpl.txt"
  File "${LOCAL_FILES}\*.patch"
  SetOutPath "$INSTDIR\src"
  File "${LOCAL_FILES}\src\*.*"
  SetOutPath "$INSTDIR\codeblocks"
  File "${LOCAL_FILES}\codeblocks\*.*"
  SetOutPath "$INSTDIR\codeblocks\wxsmith"
  File "${LOCAL_FILES}\codeblocks\wxsmith\*.*"
  SetOutPath "$INSTDIR\msvc"
  File "${LOCAL_FILES}\msvc\*.*"
  SetOutPath "$INSTDIR\tools"
  File /r "${LOCAL_FILES}\tools\*.*"
SectionEnd

Section "Visual C++ 2005 runtime" SEC03
  SetOutPath "$INSTDIR"
  File "${LOCAL_FILES}\vcredist_x86.exe"
  Call CheckVCRedist
  Delete "${LOCAL_FILES}\vcredist_x86.exe"
SectionEnd

Section "Crystal Space libraries" SEC04
  SetOutPath "$INSTDIR"
  File "${LOCAL_FILES}\bugplug.dll"
  File "${LOCAL_FILES}\cg.dll"
  File "${LOCAL_FILES}\cgGL.dll"
  File "${LOCAL_FILES}\crystalspace-1.4-vc8.dll"
  File "${LOCAL_FILES}\crystalspace_directx-1.4-vc8.dll"
  File "${LOCAL_FILES}\crystalspace_opengl-1.4-vc8.dll"
  File "${LOCAL_FILES}\crystalspace_windows-1.4-vc8.dll"
  File "${LOCAL_FILES}\csbmpimg.dll"
  File "${LOCAL_FILES}\csconout.dll"
  File "${LOCAL_FILES}\csddsimg.dll"
  File "${LOCAL_FILES}\csfont.dll"
  File "${LOCAL_FILES}\csgifimg.dll"
  File "${LOCAL_FILES}\csjngimg.dll"
  File "${LOCAL_FILES}\csjpgimg.dll"
  File "${LOCAL_FILES}\csopcode.dll"
  File "${LOCAL_FILES}\csparser.dll"
  File "${LOCAL_FILES}\cspngimg.dll"
  File "${LOCAL_FILES}\cssynldr.dll"
  File "${LOCAL_FILES}\cstgaimg.dll"
  File "${LOCAL_FILES}\ddraw2d.dll"
  File "${LOCAL_FILES}\engine.dll"
  File "${LOCAL_FILES}\fancycon.dll"
  File "${LOCAL_FILES}\fontplex.dll"
  File "${LOCAL_FILES}\freefnt2.dll"
  File "${LOCAL_FILES}\frustvis.dll"
  File "${LOCAL_FILES}\genmesh.dll"
  File "${LOCAL_FILES}\gl3d.dll"
  File "${LOCAL_FILES}\glshader_cg.dll"
  File "${LOCAL_FILES}\glshader_fixed.dll"
  File "${LOCAL_FILES}\glwin32.dll"
  File "${LOCAL_FILES}\imgplex.dll"
  File "${LOCAL_FILES}\libfreetype2-cs.dll"
  File "${LOCAL_FILES}\libjpeg-cs.dll"
  File "${LOCAL_FILES}\libmng-cs.dll"
  File "${LOCAL_FILES}\libogg-cs.dll"
  File "${LOCAL_FILES}\libpng-cs.dll"
  File "${LOCAL_FILES}\libvorbis-cs.dll"
  File "${LOCAL_FILES}\libvorbisfile-cs.dll"
  File "${LOCAL_FILES}\libz-cs.dll"
  File "${LOCAL_FILES}\rendstep_std.dll"
  File "${LOCAL_FILES}\reporter.dll"
  File "${LOCAL_FILES}\shadermgr.dll"
  File "${LOCAL_FILES}\shaderweaver.dll"
  File "${LOCAL_FILES}\sndmanager.dll"
  File "${LOCAL_FILES}\sndsysloader.dll"
  File "${LOCAL_FILES}\sndsysogg.dll"
  File "${LOCAL_FILES}\sndsysopenal.dll"
  File "${LOCAL_FILES}\sndsyssoft.dll"
  File "${LOCAL_FILES}\sndsyswav.dll"
  File "${LOCAL_FILES}\sndsyswin.dll"
  File "${LOCAL_FILES}\stdrep.dll"
  File "${LOCAL_FILES}\thing.dll"
  File "${LOCAL_FILES}\vfs.cfg"
  File "${LOCAL_FILES}\vfs.dll"
  File "${LOCAL_FILES}\wxgl.dll"
  File "${LOCAL_FILES}\xmlshader.dll"
  SetOutPath "$INSTDIR\data\config-plugins"
  File /r "${LOCAL_FILES}\data\config-plugins\*.*"
  SetOutPath "$INSTDIR\data\shader"
  File /r "${LOCAL_FILES}\data\shader\*.*"
SectionEnd

Section "wxWidgets libraries" SEC05
  SetOutPath "$INSTDIR"
  File "${LOCAL_FILES}\wxbase28_vc_custom.dll"
  File "${LOCAL_FILES}\wxmsw28_core_vc_custom.dll"
  File "${LOCAL_FILES}\wxmsw28_gl_vc_custom.dll"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Skyscraper.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Skyscraper.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "${PRODUCT_STARTMENU_REGVAL}" "$ICONS_GROUP"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Application"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Source code"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "Visual C++ 2005 runtime"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "Crystal Space libraries"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC05} "wxWidgets libraries"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

;-------------------------------
; Test if Visual Studio Redistributables 2005+ SP1 installed
Function CheckVCRedist
   Push $R0
   ClearErrors
   ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{770657D0-A123-3C07-8E44-1C83EC895118}" "Version"

   ; if VS 2005+ redist SP1 not installed, install it
   IfErrors 0 VSRedistInstalled
   ExecWait '"$INSTDIR\vcredist_x86.exe" /Q'
   ;StrCpy $R0 "-1"

VSRedistInstalled:
   Exch $R0
FunctionEnd

Section Uninstall
  ReadRegStr $ICONS_GROUP ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "${PRODUCT_STARTMENU_REGVAL}"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"

  RMdir /r "$INSTDIR"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  Delete "$DESKTOP\Skyscraper.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Skyscraper.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Release Notes.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Building Design Guide.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Changelog.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd