// XSHWindows.cpp : CXSHWindows 的实现

#include "stdafx.h"
#include "XSHWindows.h"
#include ".\xshwindows.h"
#include "Windows.h"

// CXSHWindows


STDMETHODIMP CXSHWindows::FindWindow(BSTR cls, BSTR caption, LONG* window)
{
	// TODO: 在此添加实现代码
	HANDLE h = NULL;
	BSTR _cls = NULL;
	BSTR _caption = NULL;
	if(cls[0] != '\0')
		_cls = cls;
	if(caption[0] != '\0')
		_caption = caption;

	h = ::FindWindow(_cls,_caption);
	*window = (LONG)(LONG_PTR)h;
	return S_OK;
}
struct __EnumWP{
	CComPtr<IDispatch> fun;
	CComVariant p;
};
BOOL CALLBACK EnumWindowsProc(          HWND hwnd,
							  LPARAM lParam
							  )
{
	TCHAR szText[1024] = {0};
	GetWindowText(hwnd, szText , 1024);

	struct __EnumWP* pst = (struct __EnumWP*)lParam;
	CComVariant avarParams[2];
	avarParams[0] = szText;
	avarParams[1] = (LONG)(LONG_PTR)hwnd;
	CComVariant varResult;
	DISPPARAMS params = { avarParams, NULL, 2, 0 };

	HRESULT hr = pst->fun->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params,
		&varResult,NULL,NULL);
	return varResult.vt != VT_I4 ? 1 :  varResult.intVal;
}
STDMETHODIMP CXSHWindows::EnumWindows(IDispatch* pfun, VARIANT* p)
{
	// TODO: 在此添加实现代码
	struct __EnumWP l;
	l.fun = pfun;
	//l.p = p;
	LONG b =(LONG) ::EnumWindows(EnumWindowsProc, (LPARAM)&l);
	CComVariant v(b);
	v.Detach(p);
	return S_OK;
}

STDMETHODIMP CXSHWindows::GetActiveWindow(LONG* h)
{
	// TODO: 在此添加实现代码
	*h = (LONG)(LONG_PTR)::GetActiveWindow();
	return S_OK;
}

STDMETHODIMP CXSHWindows::GetForegroundWindow(LONG* h)
{
	// TODO: 在此添加实现代码
	*h = (LONG)(LONG_PTR)::GetForegroundWindow();
	return S_OK;
}

STDMETHODIMP CXSHWindows::GetCapture(LONG* h)
{
	// TODO: 在此添加实现代码
	*h = (LONG)(LONG_PTR)::GetCapture();
	return S_OK;
}
LRESULT CALLBACK CXSHWindows::WindowProc(
   HWND hWnd,
   UINT uMsg,
   WPARAM wParam,
   LPARAM lParam 
)
{
	//CXSHWindows* pThis = (CXSHWindows*) hWnd;
	//CWindowImpl<CXSHWindows>* pThisWin = (CWindowImpl<CXSHWindows>*) hWnd;
	//CXSHWindows* pThis = dynamic_cast<CXSHWindows*> (pThisWin);
	if(uMsg == WM_POWERBROADCAST || uMsg == WM_CREATE )
	{
	//	pThis->Fire_OnNotify(L"WM_POWERBROADCAST", CComVariant(LONG(wParam)), CComVariant(LONG(-1)));
		
	}
	return __super::WindowProc(hWnd, uMsg, wParam, lParam);
}

WNDPROC CXSHWindows::GetWindowProc()
{
	// TODO: 在此添加专用代码和/或调用基类
	//this->Fire_OnNotify();
	return __super::GetWindowProc();
}

LRESULT CXSHWindows::OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return 0;
}
LRESULT CXSHWindows::OnPower(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	TSAUTO();
	bHandled = TRUE;
	HRESULT hr = S_OK;
	if(m_mapMsg2RetFun.find(uMsg) != m_mapMsg2RetFun.end())
	{
		CComVariant avarParams[2];
		avarParams[0] = (LONG)wParam;
		avarParams[1] = (LONG)uMsg;
		CComVariant varResult;
		DISPPARAMS params = { avarParams, NULL, 2, 0 }; 
		IDispatch* fun = m_mapMsg2RetFun[uMsg];
		HRESULT hr2 = fun->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params,
			&varResult,NULL,NULL);
		TSDEBUG4CXX(" hr : "<<hr2<<" varRet : "<<varResult);
		hr = (varResult.vt == VT_I4) ? varResult.intVal : 0;
	}
	return hr;

}

STDMETHODIMP CXSHWindows::AddMsg2RetFunCallback(LONG msg, IDispatch* fun)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	 
	m_mapMsg2RetFun[(UINT)msg] = fun;
	return S_OK;
}

STDMETHODIMP CXSHWindows::RemoveMsg2RetFunCallback(LONG l)
{
	// TODO: 在此添加实现代码
	TSAUTO();
//	BOOL bHandled;
//	OnPower(WM_POWERBROADCAST, PBT_APMQUERYSUSPEND, 0, bHandled);

	m_mapMsg2RetFun.erase((UINT)l);
	return S_OK;
}

STDMETHODIMP CXSHWindows::GetWindowLongPtr(OLE_HANDLE h, LONG index, LONG* ret)
{
	// TODO: 在此添加实现代码
	//TSAUTO();
	*ret = (LONG)(LONG_PTR)::GetWindowLongPtr((HWND)(UINT_PTR)h, (int)index);
	TSDEBUG4CXX(" hWnd : " <<h<<" , nIndex : "<<index<<", ret = "<<*ret);
	return S_OK;
}

/*
ATLASSERT(::IsWindow(m_hWnd));

DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
if(dwStyle == dwNewStyle)
return FALSE;

::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
if(nFlags != 0)
{
::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
}
*/


STDMETHODIMP CXSHWindows::ModifyStyleExByTitle(OLE_HANDLE h, LONG lRemove, LONG lAdd, LONG nFlags)
{
	// TODO: 在此添加实现代码
	HWND hWnd = (HWND)(ULONG_PTR)h;
	DWORD dwStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
	DWORD dwNewStyle = (dwStyle & ~lRemove) | lAdd;
	if(dwStyle == dwNewStyle)
		return S_OK;
	::SetWindowLong(hWnd, GWL_EXSTYLE, dwNewStyle);
	if(nFlags != 0)
	{
		::SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags); 

	}
	return S_OK;
}
STDMETHODIMP CXSHWindows::GetWindowThreadProcessId (OLE_HANDLE  hWnd, LONG* lProcessID)
{
	::GetWindowThreadProcessId ((HWND)(ULONG_PTR)hWnd, (LPDWORD)lProcessID);
	return S_OK;
}

STDMETHODIMP CXSHWindows::SetWindowPos(OLE_HANDLE  hWnd, OLE_HANDLE  hWndInsertAfter,LONG x, LONG y, LONG cx, LONG cy, LONG lflag)
{



	::SetWindowPos((HWND)(ULONG_PTR)hWnd, (HWND)(ULONG_PTR)hWndInsertAfter, x, y, cx, cy, lflag );
	return S_OK;
}