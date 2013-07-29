// ActiveXContainer.h : CActiveXContainer µÄÉùÃ÷

#pragma once
#include "resource.h"       // Ö÷·ûºÅ

#include "SHhost.h"
#include "_IActiveXContainerEvents_CP.h"
#include <atlctl.h>


// CActiveXContainer

class ATL_NO_VTABLE CActiveXContainer : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CActiveXContainer, &CLSID_ActiveXContainer>,
	public IConnectionPointContainerImpl<CActiveXContainer>,
	public CProxy_IActiveXContainerEvents<CActiveXContainer>, 
	public IProvideClassInfo2Impl<&CLSID_ActiveXContainer, &__uuidof(_IActiveXContainerEvents), &LIBID_SHhostLib>,
	public IDispatchImpl<IActiveXContainer, &IID_IActiveXContainer, &LIBID_SHhostLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>,
	public IObjectSafetyImpl<CActiveXContainer, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CActiveXContainer();

	
DECLARE_NO_REGISTRY();
	
//DECLARE_REGISTRY_RESOURCEID(IDR_ACTIVEXCONTAINER)


BEGIN_COM_MAP(CActiveXContainer)
	COM_INTERFACE_ENTRY(IActiveXContainer)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CActiveXContainer)
	CONNECTION_POINT_ENTRY(__uuidof(_IActiveXContainerEvents))
END_CONNECTION_POINT_MAP()

BEGIN_SINK_MAP(CActiveXContainer)
END_SINK_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

public:
	STDMETHOD( GetTypeInfoCount )( OUT UINT *pctinfo );
	STDMETHOD( GetTypeInfo )( IN UINT iTInfo, IN LCID lcid, OUT ITypeInfo **ppTInfo );
	STDMETHOD( GetIDsOfNames )( IN REFIID riid, /* [size_is] */ IN LPOLESTR *rgszNames, IN UINT cNames, IN LCID lcid, /* [size_is] */ OUT DISPID *rgDispId );
	STDMETHOD( Invoke )( IN DISPID dispIdMember, IN REFIID riid, IN LCID lcid, IN WORD wFlags, OUT IN DISPPARAMS *pDispParams, OUT VARIANT *pVarResult, OUT EXCEPINFO *pExcepInfo, OUT UINT *puArgErr );

public:
	STDMETHOD(createObject)(BSTR clsid, BSTR path);
	STDMETHOD(get_object)(IDispatch** pVal);
	STDMETHOD(put_object)(IDispatch* newVal);
	STDMETHOD(attachEvent)(BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal);
	STDMETHOD(detachEvent)(BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal);

protected:
	//CLSID				m_ObjectClsid;
	CComPtr<IDispatch>	m_spObject;
};

OBJECT_ENTRY_AUTO(__uuidof(ActiveXContainer), CActiveXContainer)
