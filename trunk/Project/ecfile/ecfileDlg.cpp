// ecfileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ecfile.h"
#include "ecfileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEcfileDlg dialog

CEcfileDlg::CEcfileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEcfileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEcfileDlg)
	m_path = _T("");
	m_pass = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEcfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEcfileDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_path);
	DDX_Text(pDX, IDC_PASS1, m_pass);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEcfileDlg, CDialog)
	//{{AFX_MSG_MAP(CEcfileDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_E, OnE)
	ON_BN_CLICKED(IDC_D, OnD)
	ON_BN_CLICKED(IDC_BROW, OnBrow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEcfileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	return TRUE;
}

void CEcfileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CEcfileDlg::OnPaint() 
{
	CDialog::OnPaint();
}

HCURSOR CEcfileDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEcfileDlg::OnE() 
{
	UpdateData(TRUE);
	if(m_path == "")
	{
		AfxMessageBox("怎么没有选择要加密的文件就开始加密啊?");
		return;
	}

	UpdateData(TRUE);
	if(m_pass == "")
	{
		AfxMessageBox("没有写上密码");
		return;
	}


	if(ecfile(m_path))
	{
		MessageBox("加密成功了已经");
	}
	else
	{
		MessageBox("没加密成功");
	}
}

void CEcfileDlg::OnD() 
{
	UpdateData(TRUE);
	if(m_path == "")
	{
		AfxMessageBox("怎么没有选择要加密的文件就开始解密啊?");
		return;
	}

	UpdateData(TRUE);
	if(m_pass == "")
	{
		AfxMessageBox("没有写上密码");
		return;
	}

	if(dcfile(m_path))
	{
		MessageBox("解密成功了");
	}
	else
	{
		MessageBox("解密失败了");
	}
}

void CEcfileDlg::OnBrow() 
{
	CFileDialog dlg(TRUE);
	if(dlg.DoModal() == IDOK)
	{
		m_path = dlg.GetPathName();
		UpdateData(FALSE);
	}
	else
	{
		return;
	}
}


//给文件加密的函数
BOOL CEcfileDlg::ecfile(LPCTSTR fpath)
{
	char *data;
	CFile *file;
	DWORD flen;

	m_password = epass();

	file = new CFile;
	if ( !file->Open(fpath, CFile::shareDenyNone|CFile::modeReadWrite))
	{
		return FALSE;
	}

	flen = file->GetLength();
	data = new char[(int)flen];

	file->SeekToBegin();
	file->Read(data, flen);

	for(int i=0; i<(int)flen; i++)
	{
		data[i] ^= m_password;
		data[i] ^= flen;
	}

	file->SeekToBegin();
	file->Write(data, flen);
	delete[] data;

	//添加密码验证信息
	char cpass[5] = "love";
	for(int j=0; j<5; j++)
	{
		cpass[j] ^= m_password;
	}
	file->SeekToEnd();
	file->Write(&cpass, 5);
	file->Close();
	delete file;

	return TRUE;
}


//给文件解密的函数
BOOL CEcfileDlg::dcfile(LPCTSTR fpath)
{
	char *data;
	CFile *file;
	DWORD flen;
	char love[5];

	file = new CFile;
	if( !file->Open(fpath, CFile::shareDenyNone|CFile::modeReadWrite))
	{
		return FALSE;
	}

	flen = file->GetLength();
	data = new char[(int)flen];

	//检验密码是不是正确
	file->Seek(-5, CFile::end);
	file->Read(&love, 5);

	m_password = epass();

	for(int i=0; i<5; i++)
	{
		love[i] ^= m_password;
	}

	if(strcmp(love, "love")!=0)
	{
		return FALSE;
	}

	//解密
	file->SeekToBegin();
	file->Read(data, flen);

	for(int j=0; j<(int)flen; j++)
	{
		data[j] ^= m_password;
		data[j] ^= (flen-5);
	}
	file->SeekToBegin();
	file->Write(data, flen);
	file->SetLength(flen-5);
	file->Close();

	delete[] data;
	delete file;
	return TRUE;
}


//获得密码的函数
__int64 CEcfileDlg::epass()
{
	DWORD plen;
	char *ppass;
	__int64 mc= 8757735233305;
	UpdateData(TRUE);

	ppass = m_pass.GetBuffer(0);

	plen = strlen(ppass);

	for(int i=0; i<(int)plen; i++)
	{
		mc ^= ppass[i]|128;
	}
	return mc;
}