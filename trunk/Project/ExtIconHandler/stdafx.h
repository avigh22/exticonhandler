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