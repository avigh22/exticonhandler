// Utility.h : CUtility µÄÉùÃ÷

#pragma once
#include "resource.h"       // Ö÷·ûºÅ

#include "atlctl.h"
#include "SHhost.h"
#include "vector"
#include "AFileSignInfo.h"

#define WM_DOWNLOADFILE WM_USER+200
// CUtility

class ATL_NO_VTABLE CUtility : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUtility, &CLSID_Utility>,
 	public IObjectSafetyImpl<CUtility, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA> ,
	public CWindowImpl<CUtility>,
	public IDispatchImpl<IUtility, &IID_IUtility, &LIBID_SHhostLib, 0xffff, /*wMinor =*/ 0xffff>
{
public:
	CUtility()
	{
	}
DECLARE_NO_REGISTRY();
//DECLARE_REGISTRY_RESOURCEID(IDR_UTILITY)
		//DECLARE_WND_CLASS(_T("external_{B9689BCB-F2B7-4D43-B6B3-6BFB66DB1745}"))

BEGIN_COM_MAP(CUtility)
	COM_INTERFACE_ENTRY(IUtility)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		TSAUTO();
		Create((HWND)-3);
		return S_OK;
	}
	
	void FinalRelease() 
	{
		TSAUTO();
		if( m_hWnd)
			DestroyWindow();
	}

	BEGIN_MSG_MAP(CUtility)  
		 
		MESSAGE_HANDLER(WM_DOWNLOADFILE, OnDownloadFile)
	END_MSG_MAP()
private:
	std::vector<LONG> m_lChildProcesses;
public:

	STDMETHOD(GetPID)(BSTR* ppid);
	STDMETHOD(GetHostVersion)(BSTR* pver);
	STDMETHOD(GetHostBuild)(LONG* pv);
	STDMETHOD(GetEIVersion)(BSTR* pver);
	STDMETHOD(GetEIBuild)(LONG* pv);
	STDMETHOD(ShellExcute)(BSTR app, BSTR param, BSTR workdir, LONG startupflag, LONG *pProcessId);
	STDMETHOD(lnterrupted)(LONG l);
	STDMETHOD(GetLastInputInfo)(LONG* l);
	STDMETHOD(SetEnvironmentVariable)(BSTR name, BSTR value);
	STDMETHOD(GetEnvironmentVariable)(BSTR name, BSTR* pvalue);
	STDMETHOD(GetTickCount)(LONG* l);
	STDMETHOD(get_windows)(IDispatch** pVal);
	STDMETHOD(IsSupportOpencl)(LONG* pl);
	STDMETHOD(Open)(BSTR url);
	STDMETHOD(OpenUrl)(BSTR url, LONG mode);

	LRESULT OnDownloadFile(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/); 
	STDMETHOD(DownloadURL)(BSTR url, BSTR dest, VARIANT expression,  VARIANT autoload);

	STDMETHOD(TerminateProcess)(LONG processid, LONG tree);
	STDMETHOD(GetOrganizationName)(BSTR path, BSTR* name);
	STDMETHOD(WaitForSingleObject)(LONG h, LONG ms, LONG* ret);
	STDMETHOD(SetThreadExecutionState)(LONG l);
};

OBJECT_ENTRY_AUTO(__uuidof(Utility), CUtility)
