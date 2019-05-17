
// TrayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tray.h"
#include "TrayDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTrayDlg dialog



CTrayDlg::CTrayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRAY_DIALOG, pParent)
	, m_iID(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_TrayListCtrl);
	DDX_Text(pDX, IDC_EDIT_NAME, m_iID);
}

BEGIN_MESSAGE_MAP(CTrayDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CTrayDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CTrayDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CTrayDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CTrayDlg message handlers

BOOL CTrayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_TrayListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_TrayListCtrl.InsertColumn(0, L"图标", LVCFMT_LEFT, 38);
	m_TrayListCtrl.InsertColumn(1, L"应用程序路径", LVCFMT_LEFT, 239);
	m_TrayListCtrl.InsertColumn(2, L"提示信息", LVCFMT_LEFT, 170);

	m_tr.EnumNotifyWindow();
	m_tr.ShowTray(&m_TrayListCtrl);

	SetTimer(0, 1000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTrayDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//m_tr.EnumNotifyWindow();
	//m_tr.ShowTray(&m_TrayListCtrl);

	CDialogEx::OnTimer(nIDEvent);
}


void CTrayDlg::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	m_tr.EnumNotifyWindow();
	m_tr.ShowTray(&m_TrayListCtrl);
}


void CTrayDlg::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_tr.AddBtn(m_iID);
}


void CTrayDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_tr.DeleteBtn(m_iID);
}
