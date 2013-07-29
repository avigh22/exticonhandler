// ecfileDlg.h : header file
//

#if !defined(AFX_ECFILEDLG_H__F282972B_CF5F_4913_9A02_309E132752EE__INCLUDED_)
#define AFX_ECFILEDLG_H__F282972B_CF5F_4913_9A02_309E132752EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEcfileDlg dialog

class CEcfileDlg : public CDialog
{
// Construction
public:
	

	__int64 epass();
	__int64 m_password;

	BOOL dcfile(LPCTSTR fpath);
	BOOL ecfile(LPCTSTR fpath);
	CEcfileDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEcfileDlg)
	enum { IDD = IDD_ECFILE_DIALOG };
	CString	m_path;
	CString	m_pass;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEcfileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEcfileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnE();
	afx_msg void OnD();
	afx_msg void OnBrow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECFILEDLG_H__F282972B_CF5F_4913_9A02_309E132752EE__INCLUDED_)
