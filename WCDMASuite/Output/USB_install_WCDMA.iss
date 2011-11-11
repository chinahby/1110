[Setup]
AppName=WCDMA
AppVerName=WCDMA
DefaultDirName={pf}\WCDMA
DefaultGroupName=WCDMA

DirExistsWarning = yes

VersionInfoCompany=
VersionInfoVersion=1.0.0.
OutputBaseFilename=Setup_WCDMA
OutputDir=../Installation

LanguageDetectionMethod=uilanguage

[Languages]
Name: "Ru"; MessagesFile: "Russian.isl"
Name: "En"; MessagesFile: "english.isl"

[Files]
Source: "SunnyCat.exe"; DestDir: "{app}";DestName:"WCDMA.exe"
Source: "SetupProduct_WCDMA.exe"; DestDir: "{app}";DestName:"SetupProduct.exe"
Source: "RmTemp.exe"; DestDir: "{app}"
Source: "*.dll"; DestDir: "{app}"

;copy db
Source: "SC700_EN.DB"; DestDir: "{app}";  DestName:"M500EUSB.DB";
;Source: "SC700_EN.DB"; DestDir: "{app}";  DestName:"M500EUSB.DB";Flags:uninsneveruninstall onlyifdoesntexist ;Check: CheckEn

;copy driver
Source: "Driver_WCDMA\*.*"; DestDir: "{app}\driver"; Flags: recursesubdirs

;Copy music
Source: "music\*"; DestDir: "{app}\music"

;Copy configure
Source: "D-dur-Ru.dat"; DestDir: "{app}";  DestName:"D-dur.dat";Flags:uninsneveruninstall onlyifdoesntexist ;Check: CheckRu
Source: "D-dur-En.dat"; DestDir: "{app}";  DestName:"D-dur.dat";Flags:uninsneveruninstall onlyifdoesntexist ;Check: CheckEn

;Copy skin files
Source: "picture\Russian\BMP\*.*";DestDir: "{app}\skin\Russian\BMP" ;Check: CheckRu
Source: "picture\English\BMP\*.*";DestDir: "{app}\skin\English\BMP" ;Check: CheckEn
Source: "Network\Russian\*.*";DestDir: "{app}\skin" ;Check: CheckRu
Source: "Network\English\*.*";DestDir: "{app}\skin" ;Check: CheckEn
;Source: "skin\neostyle\*.*";DestDir: "{app}\skin\neostyle"
;Source: "skin\PhoneSkins\*.*";DestDir: "{app}\skin\PhoneSkins"
Source: "skin\*.*";DestDir: "{app}\skin" ;Flags: recursesubdirs



;Copy resource
Source: "resource\help_en.pdf"; DestDir: "{app}\Resource" ;DestName:"help_en.pdf";Check: CheckEn
;Source: "resource\help.txt"; DestDir: "{app}\Resource" ;DestName:"help.txt";Check: CheckCh
Source: "resource\resource_ru_USB.txt"; DestDir: "{app}\Resource"; Check: CheckRu
Source: "resource\resource_en_USB.txt"; DestDir: "{app}\Resource"; Check: CheckEn

;Copy Mach5
;Source: "*.reg"; DestDir: "{app}"
;Source: "Mach5\*.*"; DestDir: "{app}\Mach5";Flags: recursesubdirs

[Icons]
;Program menu
Name: "{group}\WCDMA"; Filename: "{app}\WCDMA.exe"
Name: "{group}\§µ§Õ§Ñ§Ý§Ú§ä§îWCDMA"; Filename: "{uninstallexe}" ;Check: CheckRu
Name: "{group}\Uninstall WCDMA"; Filename: "{uninstallexe}" ;Check: CheckEn

;Desktop
Name: "{userdesktop}\WCDMA"; Filename: "{app}\WCDMA.exe"


[Run]
;Install driver
FileName: "{app}\SetupProduct.exe"; Parameters: "-i -u -Ru" ;Check: CheckRu
FileName: "{app}\SetupProduct.exe"; Parameters: "-i -u -En" ;Check: CheckEn
;Filename: "{app}\EVDO Modem.exe"; Description: "{cm:LaunchProgram,EVDOModem}"; Flags: nowait postinstall skipifsilent



[UninstallRun]
FileName: "{app}\SetupProduct.exe"; Parameters: "-u -u -Ru";Check: CheckRu
FileName: "{app}\SetupProduct.exe"; Parameters: "-u -u -En";Check: CheckEn
FileName: "{app}\RmTemp.exe"; Flags: runhidden

[Registry]
;Remove registry enry
Root: HKLM; Subkey: "Software\WCDMA Modem\WCDMA"; Flags: dontcreatekey uninsdeletekey


[Code]


var
UsagePage: TInputOptionWizardPage;

function CheckRu(): Boolean;
begin
if (ActiveLanguage = 'Ru') then
   result:=true
else
   result:=false;
end;

function CheckEn(): Boolean;
begin
if (ActiveLanguage = 'En') then
   result:=true
else
   result:=false;
end;



var
RegStr:String;
FileName: String;
ResultCode: Integer;

procedure CurStepChanged(CurStep: TSetupStep);
begin
     RegStr:= 'Software\WCDMA Modem\WCDMA'

  if CurStep = ssDone then
  begin
    RegWriteStringValue(HKEY_LOCAL_MACHINE, RegStr, 'Uninstall', ExpandConstant('{uninstallexe}'));
  end;

end;

function InitializeSetup(): Boolean;
var
FileName: String;
ResultCode: Integer;

begin
    ResultCode := 0;
    if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'Software\WCDMA Modem\WCDMA', 'Uninstall', FileName) then
    begin
        if FileExists(FileName) then
          Exec(FileName, '', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
    end;

    Result := ResultCode = 0;
end;

