// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#ifndef STRICT
#define STRICT
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows 95 和 Windows NT 4 或更高版本的功能。
#define WINVER 0x0500		// 将此更改为针对于 Windows 98 和 Windows 2000 或更高版本的合适的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows NT 4 或更高版本的功能。
#define _WIN32_WINNT 0x0500	// 将此更改为针对于 Windows 2000 或更高版本的合适的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0510 // 将此更改为针对于 Windows Me 或更高版本的合适的值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 4.0 或更高版本的功能。
#define _WIN32_IE 0x0500	// 将此更改为针对于 IE 5.0 或更高版本的合适的值。
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将为显式的

// 关闭 ATL 对某些常被安全忽略的常见警告消息的隐藏
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
		//将当前线程对调试器隐藏了
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