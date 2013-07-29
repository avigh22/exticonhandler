// ecfile.h : main header file for the ECFILE application
//

#if !defined(AFX_ECFILE_H__312C7E5B_CCDE_457F_80EB_01DE9D318A48__INCLUDED_)
#define AFX_ECFILE_H__312C7E5B_CCDE_457F_80EB_01DE9D318A48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEcfileApp:
// See ecfile.cpp for the implementation of this class
//

class CEcfileApp : public CWinApp
{
public:
	CEcfileApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEcfileApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEcfileApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECFILE_H__312C7E5B_CCDE_457F_80EB_01DE9D318A48__INCLUDED_)
