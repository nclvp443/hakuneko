[Setup]
AppName=HakuNeko
AppVerName=HakuNeko
AppVersion=1.0.2
VersionInfoVersion=1.0.2
AppPublisher=Ronny Wegener
AppPublisherURL=http://hakuneko.googlecode.com
SetupIconFile=rc\app.ico
UninstallDisplayIcon={app}\HakuNeko.exe
DisableWelcomePage=yes
DefaultDirName={pf}\HakuNeko
;DefaultGroupName=HakuNeko
DisableProgramGroupPage=yes
DisableReadyPage=yes
; refresh windows explorer to update associated icons
;ChangesAssociations=yes
OutputDir=.
OutputBaseFilename=dist\winnt\HakuNeko_Setup
;WizardImageFile=compiler:wizmodernimage.bmp
;WizardSmallImageFile=compiler:wizmodernsmallimage.bmp

[Tasks]
Name: shortcuts; Description: "All"; GroupDescription: "Create Shortcuts:";
Name: shortcuts\desktop; Description: "Desktop"; GroupDescription: "Create Shortcuts:";
Name: shortcuts\startmenu; Description: "Startmenu Programs"; GroupDescription: "Create Shortcuts:"; Flags: unchecked
Name: shortcuts\quicklaunch; Description: "Quick Launch"; GroupDescription: "Create Shortcuts:"; Flags: unchecked

[Files]
Source: dist\winnt\hakuneko\HakuNeko.exe; DestDir: {app}
;Source: dist\winnt\hakuneko\gui.conf; DestDir: {userappdata}\.hakuneko
;Source: dist\winnt\hakuneko\connectors\*.*; DestDir: {commonappdata}\HakuNeko

[UninstallDelete]
;Name: {app}; Type: filesandordirs
Name: {userappdata}\.hakuneko; Type: filesandordirs
Name: {commonappdata}\connectors\*.*; Type: filesandordirs

[Icons]
Name: "{commondesktop}\HakuNeko"; Filename: "{app}\HakuNeko.exe"; Tasks: shortcuts\desktop
Name: "{commonstartmenu}\HakuNeko"; Filename: "{app}\HakuNeko.exe"; Tasks: shortcuts\startmenu
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\HakuNeko"; Filename: "{app}\HakuNeko.exe"; Tasks: shortcuts\quicklaunch