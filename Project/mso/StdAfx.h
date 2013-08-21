// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6980D6A4_D8E4_4FF2_A95D_808FD2C227EC__INCLUDED_)
#define AFX_STDAFX_H__6980D6A4_D8E4_4FF2_A95D_808FD2C227EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0400 
// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6980D6A4_D8E4_4FF2_A95D_808FD2C227EC__INCLUDED_)


inline bool IsDebugging()
{
	//TSAUTO();
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

	//TSDEBUG4CXX("*(BYTE*)Func_addr="<<*(BYTE*)Func_addr);
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

	HMODULE hModule = GetModuleHandle(L"ntdll.dll"); 
	ZW_QUERY_SYSTEM_INFORMATION ZwQuerySystemInformation; 
	ZwQuerySystemInformation = (ZW_QUERY_SYSTEM_INFORMATION)GetProcAddress(hModule, "ZwQuerySystemInformation"); 
	if(ZwQuerySystemInformation)
	{
		SYSTEM_KERNEL_DEBUGGER_INFORMATION Info; 
		if (0L == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL)) 
		{ 
			if (Info.DebuggerEnabled) 
			{
//				TSDEBUG4CXX("Info.DebuggerEnabled="<<Info.DebuggerEnabled);
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
			if (ProcessInfo.DebugPort) {
//				TSDEBUG4CXX("ProcessInfo.DebugPort="<<ProcessInfo.DebugPort);			
				return true;
			}
		}
	}

	return false;

}