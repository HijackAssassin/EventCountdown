[Setup]
AppName=EventCountdowns
AppVersion=1.0
DefaultDirName={autopf}\EventCountdowns
DefaultGroupName=EventCountdowns
OutputDir=C:\Users\PatyT\Downloads\Event Countdowns\Installer
OutputBaseFilename=EventCountdownsSetup
Compression=lzma
SolidCompression=yes

[Files]
; Copy all contents (including subfolders) into Program Files\EventCountdowns\
Source: "C:\Users\PatyT\Downloads\Event Countdowns\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\EventCountdowns"; Filename: "{app}\EventCountdowns.exe"; WorkingDir: "{app}"
Name: "{commondesktop}\EventCountdowns"; Filename: "{app}\EventCountdowns.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"

[Run]
Filename: "{app}\EventCountdowns.exe"; Description: "Launch EventCountdowns now"; Flags: nowait postinstall skipifsilent
