[Setup]
AppName=HakuNeko
AppVerName=HakuNeko
AppVersion=1.1.1
VersionInfoVersion=1.1.1
AppPublisher=Ronny Wegener
AppPublisherURL=http://hakuneko.sourceforge.net
SetupIconFile=res\app.ico
UninstallDisplayIcon={app}\hakuneko.exe
DisableWelcomePage=yes
DefaultDirName={pf}\hakuneko
;DefaultGroupName=hakuneko
DisableProgramGroupPage=yes
DisableReadyPage=yes
; refresh windows explorer to update associated icons
;ChangesAssociations=yes
OutputDir=.
OutputBaseFilename=hakuneko_1.1.1_i386_msw-setup
;WizardImageFile=compiler:wizmodernimage.bmp
;WizardSmallImageFile=compiler:wizmodernsmallimage.bmp

[Tasks]
Name: shortcuts; Description: "All"; GroupDescription: "Create Shortcuts:";
Name: shortcuts\desktop; Description: "Desktop"; GroupDescription: "Create Shortcuts:";
Name: shortcuts\startmenu; Description: "Startmenu Programs"; GroupDescription: "Create Shortcuts:"; Flags: unchecked
Name: shortcuts\quicklaunch; Description: "Quick Launch"; GroupDescription: "Create Shortcuts:"; Flags: unchecked

[Files]
Source: dist\msw\bin\hakuneko.exe; DestDir: {app}
;Source: dist\msw\bin\gui.conf; DestDir: {userappdata}\hakuneko
;Source: dist\msw\bin\connectors\*.*; DestDir: {commonappdata}\hakuneko

[UninstallDelete]
Name: {app}; Type: filesandordirs
Name: {userappdata}\hakuneko; Type: filesandordirs
Name: {commonappdata}\hakuneko; Type: filesandordirs

[Icons]
Name: "{commondesktop}\hakuneko"; Filename: "{app}\hakuneko.exe"; Tasks: shortcuts\desktop
Name: "{commonstartmenu}\hakuneko"; Filename: "{app}\hakuneko.exe"; Tasks: shortcuts\startmenu
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\hakuneko"; Filename: "{app}\hakuneko.exe"; Tasks: shortcuts\quicklaunch
