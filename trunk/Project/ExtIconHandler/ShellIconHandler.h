// ShellIconHandler.h : CShellIconHandler µÄÉùÃ÷

#pragma once
#include "resource.h"       // Ö÷·ûºÅ

#include "ExtIconHandler.h"
#include <ShlObj.h>
#include <comdef.h>
#include "comdefsp.h"
#include <shlguid.h>
#include <shobjidl.h>
#include "atlwin.h"
#include <vector>
#include <string>
typedef   std::basic_string<TCHAR>    tstring;




struct __declspec(uuid("000214E8-0000-0000-C000-000000000046")) 
	IShellExtInit; 
_COM_SMARTPTR_TYPEDEF(IShellExtInit, __uuidof(IShellExtInit)); 

struct __declspec(uuid("000214e4-0000-0000-c000-000000000046")) 
	IContextMenu; 
_COM_SMARTPTR_TYPEDEF(IContextMenu, __uuidof(IContextMenu)); 

struct __declspec(uuid("000214fa-0000-0000-c000-000000000046")) 
	IExtractIcon; 
_COM_SMARTPTR_TYPEDEF(IExtractIcon, __uuidof(IExtractIcon)); 

struct __declspec(uuid("0c6c4200-c589-11d0-999a-00c04fd655e1")) IShellIconOverlayIdentifier;
struct __declspec(uuid("000214fb-0000-0000-c000-000000000046")) IShellExecuteHookW;

MIDL_INTERFACE("368AA651-9CD9-4D2C-A6C6-BF767AB215AB")
IContextMenuExt : public IShellExtInit
{
public:
};

#ifdef _DEBUG
	 DWORD   __INTERVAL_ONE_HOUR = 1000*1*60;
#else
	 DWORD  __INTERVAL_ONE_HOUR = 1000*60*60; 
#endif

//
// CShellIconHandler

class ATL_NO_VTABLE CShellIconHandler : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShellIconHandler, &CLSID_ShellIconHandler>,
	public IPersistFile,
	 public IExtractIcon  ,
	public IContextMenuExt, public IContextMenu, public IDropTarget,
	//public IShellExecuteHook,
	public IShellIconOverlayIdentifier,  
	public CWindowImpl<CShellIconHandler>,
	public IDispatchImpl<IShellIconHandler, &IID_IShellIconHandler, &LIBID_ExtIconHandlerLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	CShellIconHandler()
	{
		m_hMe = NULL;
		m_lEnable = 0;
		m_hConfigFileChanged = INVALID_HANDLE_VALUE;
	}
	DECLARE_CLASSFACTORY_SINGLETON(CShellIconHandler)
	//DECLARE_NO_REGISTRY()
	DECLARE_REGISTRY_RESOURCEID(IDR_SHELLICONHANDLER)

	BEGIN_COM_MAP(CShellIconHandler)
		COM_INTERFACE_ENTRY(IShellIconHandler)
		COM_INTERFACE_ENTRY(IDispatch)	
		COM_INTERFACE_ENTRY(IPersistFile)
		COM_INTERFACE_ENTRY(IExtractIcon)
		COM_INTERFACE_ENTRY(IShellExtInit) 	
		COM_INTERFACE_ENTRY(IContextMenuExt)
		COM_INTERFACE_ENTRY(IContextMenu) 
		COM_INTERFACE_ENTRY(IDropTarget)
		COM_INTERFACE_ENTRY(IShellIconOverlayIdentifier)
		//COM_INTERFACE_ENTRY(IShellExecuteHook)
	END_COM_MAP()

 	DECLARE_WND_CLASS(_T("ShellUIWindow32"))

	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? WS_CHILD: dwStyle;
	}

	BEGIN_MSG_MAP(CShellIconHandler)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		
	END_MSG_MAP()
 

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease() 
	{
		TSAUTO();
		if(INVALID_HANDLE_VALUE != m_hConfigFileChanged)
            FindCloseChangeNotification(m_hConfigFileChanged);
		if(m_hWnd)
			DestroyWindow();
 		if(m_spMe)
			m_spMe.Release();
		//if(m_hMe)
		//	FreeLibrary(m_hMe);
	}

public:

	//////////////////////////////////////////////////////////////////////////
	// IPersistFile
	STDMETHOD(GetClassID)(LPCLSID pClassID);
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)(LPCOLESTR pszFileName, DWORD dwMode);
	STDMETHOD(Save)(LPCOLESTR pszFileName,BOOL fRemember);
	STDMETHOD(SaveCompleted)(LPCOLESTR pszFileName);
	STDMETHOD(GetCurFile)(LPOLESTR* ppszFileName);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// IExtractIcon
	STDMETHOD(GetIconLocation)(UINT uFlags, LPWSTR pszIconFile, UINT cchMax, int* piIndex, UINT* pwFlags);
	STDMETHOD(Extract)(LPCWSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize);
	//////////////////////////////////////////////////////////////////////////


	STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY); 

	STDMETHOD(GetCommandString)(UINT_PTR, UINT, UINT*, LPSTR, UINT){ TSAUTO(); return E_NOTIMPL;}
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO); 
	STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);

	STDMETHOD(DragEnter)(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect); 
	STDMETHOD(DragOver)(DWORD grfKeyState,POINTL pt,DWORD* pdwEffect)
	{ UNREFERENCED_PARAMETER(grfKeyState); UNREFERENCED_PARAMETER(pt); UNREFERENCED_PARAMETER(pdwEffect); return E_NOTIMPL; } 
	STDMETHOD(DragLeave)(); 
	STDMETHOD(Drop)(IDataObject* pDataObj,DWORD grfKeyState,POINTL pt,DWORD* pdwEffect); 

	// interface of IShellIconOverlayIdentifier
	STDMETHOD(GetOverlayInfo)(LPWSTR pwszIconFile,int cchMax,int *pIndex,DWORD* pdwFlags);
	STDMETHOD(GetPriority)(int* pPriority);
	STDMETHOD(IsMemberOf)(LPCWSTR pwszPath,DWORD dwAttrib);

		// IShellExecuteHook
	STDMETHOD(Execute)(LPSHELLEXECUTEINFO pei);

	LRESULT OnTimer(UINT , WPARAM , LPARAM , BOOL& );

protected:
	CComBSTR m_bstrFilePath;
	std::vector<tstring> m_szFiles;
	CComPtr<IShellIconHandler> m_spMe;
	HMODULE m_hMe;
	DWORD m_dwLastLaunch;
	UINT_PTR m_uTimerID15min;
	UINT_PTR m_uTimerID5min;
	UINT_PTR m_uTimerID1min;
	HANDLE m_hConfigFileChanged;
public:
	static  CShellIconHandler* s_pThis;
protected:
	bool CreateWorkerMe();
	bool FindFirstForFileExt(TCHAR*, TCHAR* pszFileExt, TCHAR* pszPath);
public:
	
	STDMETHOD(Enable)(LONG b);

	LONG m_lEnable;
	void LaunchRundll32(TCHAR* pszCmdline = NULL );
	BOOL eXclusive(void);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	bool EnableShellIconOverlayIdentifier();

	

};

OBJECT_ENTRY_AUTO(__uuidof(ShellIconHandler), CShellIconHandler)
