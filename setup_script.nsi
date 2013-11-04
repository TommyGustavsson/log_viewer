;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "LogViewer"
  OutFile "Setup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\LogViewer"
  
  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Main Section" SectionMain

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
	File ..\LogViewer-build-desktop-release\release\LogViewer.exe

		CreateDirectory "$SMPROGRAMS\LogViewer"
		CreateShortCut "$SMPROGRAMS\LogViewer\LogViewer.lnk" "$INSTDIR\LogViewer.exe" \
			"" "" 0 SW_SHOWNORMAL \
			"" "LogViewer"
		CreateShortCut "$SMPROGRAMS\LogViewer\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

	; Tell the compiler to write an uninstaller and to look for a "Uninstall" section
	WriteUninstaller $INSTDIR\Uninstall.exe
	
	File .\redist\libgcc_s_dw2-1.dll
	File .\redist\mingwm10.dll
	File .\redist\QtCore4.dll
	File .\redist\QtGui4.dll
	File .\redist\QtNetwork4.dll
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\LogViewer.exe"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

SectionEnd
