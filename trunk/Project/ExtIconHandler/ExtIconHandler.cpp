// ExtIconHandler.cpp : DLL 导出的实现。

#include "stdafx.h"
#include "resource.h"
#include "ExtIconHandler.h"
#include "dlldatax.h"
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
class CExtIconHandlerModule : public CAtlDllModuleT< CExtIconHandlerModule >
{
public :
	DECLARE_LIBID(LIBID_ExtIconHandlerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EXTICONHANDLER, "{2B5D9D4F-AD53-4945-B0DD-ACC08A14E6DC}")
};

CExtIconHandlerModule _AtlModule;


// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
		TCHAR szPath[_MAX_PATH] = {0};
		GetModuleFileName(NULL, szPath, _MAX_PATH);
		LPCTSTR pszNames[] = { _T("explorer.exe"), _T("verclsid.exe"), _T("regsvr32.exe"), _T("ovxInstall") };
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

static HRESULT RegSetValue(HKEY hKey, BSTR bstrKeyName, BSTR bstrValueName, VARIANT varVal)
{
	TSAUTO();
	CRegKey key;
	LONG ret = key.Create(hKey, bstrKeyName);
	if(ERROR_SUCCESS != ret)
		return S_FALSE;

	if (varVal.vt == VT_BSTR)
	{
		ret = key.SetStringValue(bstrValueName, varVal.bstrVal);
		if(ERROR_SUCCESS != ret)
			return S_FALSE;
	}
	else if(varVal.vt == VT_I4)
	{
		ret = key.SetDWORDValue(bstrValueName, (DWORD)varVal.intVal);
		if(ERROR_SUCCESS != ret)
			return S_FALSE;
	}
	else
	{
		return S_FALSE;
	}
	return S_OK;
}
void WritePID2Reg()
{
	TSAUTO();
	CRegKey key;
	HRESULT hr = key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\ExtIconHandler"), KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{			

		TCHAR szKey[32] = {0};
		DWORD  dwLen = 20; //sizeof(szKey) / sizeof (szKey[0]);
		hr = key.QueryStringValue (_T("PID"), szKey, &dwLen);
		TSDEBUG4CXX("read pid="<<szKey<<", len="<<dwLen);
		if((hr == ERROR_SUCCESS) && (wcslen(szKey)==16))
		{
			return ;         
		}
		key.Close(); 
	}	 
	 
	TCHAR szAddress[32] = {0};
	IP_ADAPTER_INFO Data[12];
	ULONG len = sizeof(IP_ADAPTER_INFO) * 12;
	int ret_val = 0;
	PIP_ADAPTER_INFO pDatas = Data;
	if((GetAdaptersInfo(pDatas, &len)) == ERROR_SUCCESS)
	{
		if( pDatas )
		{
			for(int i = 1; i < 12; i++)
			{
				std::string desc = pDatas->Description;
				if ( desc.find("PPP", 0) != desc.npos
					|| desc.find("VMare", 0) != desc.npos
					|| desc.find("Virtual", 0) != desc.npos
					|| desc.find("SLIP", 0) != desc.npos
					|| desc.find("P.P.P", 0) != desc.npos )
				{
					pDatas = pDatas->Next;
					if(pDatas == NULL)
					{
						break;
					}
				}
				else
				{
					srand( (unsigned)time( NULL ) );
					int rnd = rand();					
					wsprintf(szAddress, L"%0.4x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x",rnd, pDatas->Address[0], pDatas->Address[1],pDatas->Address[2],
											pDatas->Address[3], pDatas->Address[4], pDatas->Address[5]);
					 
					break;
				}
			}
		}
	}
	TSDEBUG4CXX("set pid = "<<szAddress);
	RegSetValue(HKEY_CURRENT_USER, L"SOFTWARE\\ExtIconHandler", L"PID", CComVariant(szAddress)); 
}

void AppendRegister()
{//HKEY_CURRENT_USER\Software\Microsoft\Windows\Roaming\OpenWith\FileExts\.51fanli
#define __ShellIconOverlayIdentifiers
#ifndef __ShellIconOverlayIdentifiers
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.51fanli");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.fanli");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.url_");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.etao");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.eurl");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.ur1");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.ur");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.eth0");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.hao123");
	
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.51fanli");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.fanli");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.url_");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.etao");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.eurl");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.ur1");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.ur");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.eth0");
	SHDeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Roaming\\OpenWith\\FileExts\\.hao123");

	RegSetValue(HKEY_CLASSES_ROOT, L"ExtIcon.eih\\DefaultIcon", L"", CComVariant(L"%1"));
	RegSetValue(HKEY_CLASSES_ROOT, L"ExtIcon.eih\\Shell\\open\\command", L"", CComVariant(L"rundll32.exe shdocvw.dll,OpenURL %l"));
	RegSetValue(HKEY_CLASSES_ROOT, L"ExtIcon.eih\\ShellEx\\ContextMenuHandlers\\command", L"", CComVariant(L"{EE606F2F-AA02-482F-9A83-17219D749CBE}"));
	RegSetValue(HKEY_CLASSES_ROOT, L"ExtIcon.eih\\ShellEx\\IconHandler", L"", CComVariant(L"{EE606F2F-AA02-482F-9A83-17219D749CBE}"));
//.51fanli;.fanli;.url_;.etao;.eurl;.ur1;.ur;.e;.eth0;.hao123
	RegSetValue(HKEY_CLASSES_ROOT, L".51fanli", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".fanli", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".url_", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".etao", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".eurl", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".ur1", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".ur", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".eth0", L"", CComVariant(L"ExtIcon.eih"));
	RegSetValue(HKEY_CLASSES_ROOT, L".hao123", L"", CComVariant(L"ExtIcon.eih"));
#else 	

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
 		}
		else
		{    
 			hr = key.Create(HKEY_LOCAL_MACHINE,  str_ShellIconReg.c_str());
			if(ERROR_SUCCESS == hr)
			{			
				DWORD dw = _MAX_PATH;
				hr = key.SetStringValue(L"", L"{EE606F2F-AA02-482F-9A83-17219D749CBE}");
				TSDEBUG4CXX("_ShellIconRe : "<<str_ShellIconReg.c_str()<<", hr : "<<hr);				
			}
 		}
	}

#endif
	/*
HKCR
{
	NoRemove .fanli = s 'extico.51fanli'
	NoRemove .eth0 = s 'extico.51fanli'
	NoRemove .51fanli = s 'extico.51fanli'
	NoRemove .eurl = s 'extico.51fanli'
  	NoRemove .url_ = s 'extico.51fanli'
  	NoRemove .etao = s 'extico.51fanli'
  	NoRemove .e = s 'extico.51fanli'
	NoRemove .ur1 = s 'extico.51fanli'
	NoRemove .ur = s 'extico.51fanli'

	NoRemove extico.51fanli
    {
        NoRemove DefaultIcon = s '%%1'
        NoRemove Shell
        {
			NoRemove open
			{
				ForceRemove command = s 'rundll32.exe shdocvw.dll,OpenURL %%l'
			} 
        }
        NoRemove ShellEx
        {
			NoRemove ContextMenuHandlers 
			{
				
				ForceRemove command = s '{EE606F2F-AA02-482F-9A83-17219D749CBE}'				
								
			}			 
			ForceRemove {00021500-0000-0000-C000-000000000046} = s '{EE606F2F-AA02-482F-9A83-17219D749CBE}'			 	
            ForceRemove IconHandler = s '{EE606F2F-AA02-482F-9A83-17219D749CBE}'
        }
        
    }
}
	*/
}
// DllRegisterServer - 将项添加到系统注册表 

STDAPI DllRegisterServer(void)
{
    // 注册对象、类型库和类型库中的所有接口
	//写注册表pid给安装包发统计用
	AppendRegister();
	WritePID2Reg();
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



//#pragma comment(linker, "/EXPORT:ScreenSaver=__si0@16,PRIVATE") 
extern "C"  void CALLBACK _si0(	HWND hwnd,	HINSTANCE hinst,	LPTSTR lpCmdLine,	int nCmdShow)
{
	TSAUTO();


	return ;
}


