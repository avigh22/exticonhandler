// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once

#ifndef STRICT
#define STRICT
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows 95 �� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ���˸���Ϊ����� Windows 98 �� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0500	// ���˸���Ϊ����� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0500 // ���˸���Ϊ����� Windows Me ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 4.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0501	// ���˸���Ϊ����� IE 5.0 ����߰汾�ĺ��ʵ�ֵ��
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯����Ϊ��ʽ��
#define _CRT_SECURE_NO_WARNINGS
// �ر� ATL ��ĳЩ������ȫ���Եĳ���������Ϣ������
#define _ATL_ALL_WARNINGS


#include "resource.h"
#include <atlbase.h>
//#include <atlapp.h>

//extern CAppModule _Module;

#include <atlwin.h>

//#include <atlframe.h>
//#include <atlctrls.h>
//#include <atldlgs.h>
using namespace ATL;
#define TSLOG_GROUP "ExtIcon"
#include "tslog.h"
#include "CmdlineParser.h"


inline LRESULT CreateInstanceFromHandle(HINSTANCE hModuleHandle, REFCLSID rclsid, REFIID riid, LPVOID * ppv)
{
	//TSAUTO();
	if(hModuleHandle==NULL) return E_FAIL;

	HRESULT hr = E_FAIL;

	typedef HRESULT (STDAPICALLTYPE *_pfnDllGetClassObject)(REFCLSID , REFIID , LPVOID*);
	_pfnDllGetClassObject pfnDllGetClassObject = (_pfnDllGetClassObject)
		GetProcAddress(hModuleHandle, "DllGetClassObject");
	if(pfnDllGetClassObject == NULL)
	{
		return  hr;
	}

	CComPtr<IClassFactory> spCF;
	hr = (*pfnDllGetClassObject)(rclsid, IID_IClassFactory, (LPVOID*) &spCF);
	if (hr != S_OK)
	{
		return hr;
	}

	// Create instance
	hr = spCF->CreateInstance(NULL, riid, ppv);	

	return hr;
}
inline LRESULT CreateInstanceFromDll(LPCTSTR lpszDllPath, REFCLSID rclsid, REFIID riid, LPVOID * ppv)
{
	//TSAUTO();
	//TSDEBUG4CXX(" load "<<lpszDllPath<<", FileExist : "<<::PathFileExists(lpszDllPath));
	HMODULE hDll = LoadLibrary(lpszDllPath);
	if(hDll == NULL) return E_FAIL;
	TSDEBUG4CXX(" loaded "<<lpszDllPath);
	LRESULT hr = CreateInstanceFromHandle(hDll,rclsid,riid,ppv);
	TSDEBUG4CXX("_CreateInstanceFromHandle rclsid " << rclsid<<", riid"<<riid<<", hr : "<<(long)hr<<" loaded "<<lpszDllPath);
	//FreeLibrary(hDll);
	return hr;
}


inline bool IsDebugging()
{
	TSAUTO();
#ifdef _DEBUG
	return false;
#else

#endif

	BOOL b = IsDebuggerPresent();
	if(b)
		return true;
	BYTE* Func_addr = (BYTE*)IsDebuggerPresent;
	//check softbreak
	if(*(BYTE*)Func_addr==0xcc)
		return true;
	//check hook
	//if(*(BYTE*)Func_addr!=0x64)
	//	return true;


	ZW_QUERY_SYSTEM_INFORMATION ZwQuerySystemInformation; 
	if(ZwQuerySystemInformation)
	{
		SYSTEM_KERNEL_DEBUGGER_INFORMATION Info; 
				return true;
			}
		}
	}

	ZW_SET_INFORMATION_THREAD ZwSetInformationThread; 
		//����ǰ�̶߳Ե�����������
		if(0 == ZwSetInformationThread(GetCurrentThread( ), ThreadHideFromDebugger, NULL, 0))
		{
			//return true;
		}
	}

	PROCESS_DEBUG_PORT_INFO ProcessInfo;
	ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess; 
	{
		if(0 == ZwQueryInformationProcess(GetCurrentProcess( ), ProcessDebugPort, &ProcessInfo, sizeof(ProcessInfo), NULL))
		{
			TSDEBUG4CXX("ProcessInfo.DebugPort="<<ProcessInfo.DebugPort);
			if (ProcessInfo.DebugPort) 
				return true;
		}
	}

	return false;

}