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
#define WINVER 0x0500		// ���˸���Ϊ����� Windows 98 �� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0500	// ���˸���Ϊ����� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0510 // ���˸���Ϊ����� Windows Me ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 4.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0500	// ���˸���Ϊ����� IE 5.0 ����߰汾�ĺ��ʵ�ֵ��
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯����Ϊ��ʽ��

// �ر� ATL ��ĳЩ������ȫ���Եĳ���������Ϣ������
#define _ATL_ALL_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>

using namespace ATL;

#define TSLOG_GROUP "ExtIcon"
#include "tslog.h"
//#include "CmdlineParser.h"
#include <ShellAPI.h>
#include <Psapi.h>
#include "windows.h"

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
	TSDEBUG4CXX(" load "<<lpszDllPath<<", FileExist : "<<::PathFileExists(lpszDllPath));
	HMODULE hDll = LoadLibrary(lpszDllPath);
	if(hDll == NULL) return E_FAIL;
	TSDEBUG4CXX(" loaded "<<lpszDllPath);
	LRESULT hr = CreateInstanceFromHandle(hDll,rclsid,riid,ppv);
	TSDEBUG4CXX("_CreateInstanceFromHandle rclsid " << rclsid<<", riid"<<riid<<", hr : "<<(unsigned long)hr);
	//FreeLibrary(hDll);
	return hr;
}
 
//#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0) 
//#define STATUS_SUCCESS    0L 

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

	TSDEBUG4CXX("*(BYTE*)Func_addr="<<*(BYTE*)Func_addr);
	typedef long NTSTATUS;
	typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION 
	{ 
		BOOLEAN DebuggerEnabled; 
		BOOLEAN DebuggerNotPresent; 
	} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION; 

	typedef struct _PROCESS_DEBUG_PORT_INFO 
	{ 
		HANDLE DebugPort; 
	}    PROCESS_DEBUG_PORT_INFO; 

	enum SYSTEM_INFORMATION_CLASS { SystemKernelDebuggerInformation = 35 }; 
	enum THREAD_INFO_CLASS        { ThreadHideFromDebugger          = 17 }; 
	enum PROCESS_INFO_CLASS      { ProcessDebugPort                = 7  }; 

	typedef NTSTATUS  (NTAPI *ZW_QUERY_SYSTEM_INFORMATION)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength); 
	typedef NTSTATUS  (NTAPI *ZW_SET_INFORMATION_THREAD)(IN HANDLE ThreadHandle, IN THREAD_INFO_CLASS ThreadInformationClass, IN PVOID ThreadInformation, IN ULONG ThreadInformationLength); 
	typedef NTSTATUS  (NTAPI *ZW_QUERY_INFORMATION_PROCESS)(IN HANDLE ProcessHandle, IN PROCESS_INFO_CLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength); 

	HMODULE hModule = GetModuleHandle(_T("ntdll.dll")); 
	ZW_QUERY_SYSTEM_INFORMATION ZwQuerySystemInformation; 
	ZwQuerySystemInformation = (ZW_QUERY_SYSTEM_INFORMATION)GetProcAddress(hModule, "ZwQuerySystemInformation"); 
	if(ZwQuerySystemInformation)
	{
		SYSTEM_KERNEL_DEBUGGER_INFORMATION Info; 
		if (0L == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL)) 
		{ 
			TSDEBUG4CXX("Info.DebuggerEnabled="<<Info.DebuggerEnabled);
			if (Info.DebuggerEnabled) 
			{
				return true;
			}
		}
	}

	ZW_SET_INFORMATION_THREAD ZwSetInformationThread; 
	ZwSetInformationThread = (ZW_SET_INFORMATION_THREAD)GetProcAddress(hModule, "ZwSetInformationThread"); 
	if (ZwSetInformationThread) 
	{ 
		//����ǰ�̶߳Ե�����������
		if(0 == ZwSetInformationThread(GetCurrentThread( ), ThreadHideFromDebugger, NULL, 0))
		{
			//return true;
		}
	}

	PROCESS_DEBUG_PORT_INFO ProcessInfo;
	ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess; 
	ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)GetProcAddress(hModule, "ZwQueryInformationProcess"); 
	if (ZwQueryInformationProcess)
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