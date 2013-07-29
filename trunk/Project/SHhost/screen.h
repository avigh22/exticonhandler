// screen.h : Cscreen µÄÉùÃ÷

#pragma once
#include "resource.h"       // Ö÷·ûºÅ

#include "SHhost.h"


// Cscreen

class ATL_NO_VTABLE Cscreen : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<Cscreen, &CLSID_screen>,
	public IDispatchImpl<Iscreen, &IID_Iscreen, &LIBID_SHhostLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	Cscreen()
	{
	}

DECLARE_NO_REGISTRY();
//DECLARE_REGISTRY_RESOURCEID(IDR_SCREEN)


BEGIN_COM_MAP(Cscreen)
	COM_INTERFACE_ENTRY(Iscreen)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
		TSAUTO();
	}

public:

	STDMETHOD(get_width)(LONG* pVal);
	STDMETHOD(get_height)(LONG* pVal);
	STDMETHOD(get_availHeight)(LONG* pVal);
	STDMETHOD(get_availWidth)(LONG* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(screen), Cscreen)
