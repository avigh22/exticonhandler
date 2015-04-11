// XSHWindows.h : CXSHWindows µÄÉùÃ÷

#pragma once
#include "resource.h"       // Ö÷·ûºÅ

#include "SHhost.h"
#include "_IXSHWindowsEvents_CP.h"


// CXSHWindows

class ATL_NO_VTABLE CXSHWindows : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXSHWindows, &CLSID_XSHWindows>,
	public IConnectionPointContainerImpl<CXSHWindows>,
	public CProxy_IXSHWindowsEvents<CXSHWindows>, 
	public CWindowImpl<CXSHWindows>,
	public IProvideClassInfo2Impl<&CLSID_XSHWindows, &__uuidof(_IXSHWindowsEvents), &LIBID_SHhostLib>,
	//public IObjectSafetyImpl<CXSHWindows, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA> ,
	public IDispatchImpl<IXSHWindows, &IID_IXSHWindows, &LIBID_SHhostLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	CXSHWindows()
	{
	}
DECLARE_NO_REGISTRY();
//DECLARE_REGISTRY_RESOURCEID(IDR_XSHWINDOWS)


BEGIN_COM_MAP(CXSHWindows)
	COM_INTERFACE_ENTRY(IXSHWindows)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CXSHWindows)
	CONNECTION_POINT_ENTRY(__uuidof(_IXSHWindowsEvents))
END_CONNECTION_POINT_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
//		CWindowImplBaseT< CWindow, CControlWinTraits >*  pWin =this;
//		CWindowImpl<CXSHWindows>* pWin1 = pWin;
//		CXSHWindows* pWin2 = dynamic_cast<CXSHWindows*> (pWin);

		Create(GetDesktopWindow());
		return S_OK;
	}
	
	void FinalRelease() 
	{
		if( m_hWnd)
			DestroyWindow();


		TSAUTO();
	}

	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? WS_CHILD: dwStyle;
	}
	
	static LRESULT CALLBACK WindowProc(
   HWND hWnd,
   UINT uMsg,
   WPARAM wParam,
   LPARAM lParam 
);

	DECLARE_WND_CLASS(_T("XSHWindows"))
	BEGIN_MSG_MAP(CXSHWindows)
		MESSAGE_HANDLER(WM_POWERBROADCAST, OnPower)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

private:
	std::map<UINT, IDispatch*>m_mapMsg2RetFun;
public:

	STDMETHOD(FindWindow)(BSTR cls, BSTR caption, LONG* window);
	STDMETHOD(EnumWindows)(IDispatch* pfun, VARIANT *p);
	STDMETHOD(GetActiveWindow)(LONG* h);
	STDMETHOD(GetForegroundWindow)(LONG* h);
	STDMETHOD(GetCapture)(LONG* h);
	virtual WNDPROC GetWindowProc();
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPower(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	
	STDMETHOD(AddMsg2RetFunCallback)(LONG msg, IDispatch* fun);
	STDMETHOD(RemoveMsg2RetFunCallback)(LONG l);
	STDMETHOD(GetWindowLongPtr)(OLE_HANDLE h, LONG index, LONG* ret);
	STDMETHOD(ModifyStyleExByTitle)(OLE_HANDLE  , LONG lRemove, LONG lAdd, LONG nFlags);

	STDMETHOD(GetWindowThreadProcessId )(OLE_HANDLE  hWnd, LONG* fun);
	STDMETHOD(SetWindowPos)(OLE_HANDLE  hWnd, OLE_HANDLE  hWndInsertAfter,LONG x, LONG y, LONG cx, LONG cy, LONG lflag);
	
};

OBJECT_ENTRY_AUTO(__uuidof(XSHWindows), CXSHWindows)
