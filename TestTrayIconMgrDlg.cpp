
// TestTrayIconMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestTrayIconMgr.h"
#include "TestTrayIconMgrDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CTestTrayIconMgrDlg 对话框


CTestTrayIconMgrDlg::CTestTrayIconMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestTrayIconMgrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestTrayIconMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_TrayListCtrl);
}

BEGIN_MESSAGE_MAP(CTestTrayIconMgrDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTestTrayIconMgrDlg::OnNMDblclk_ChangeIconState)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_NOTIFYICON,OnNotifyIcon)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

LRESULT CTestTrayIconMgrDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	// 响应在托盘图标上的单击
	if ((wParam == IDR_MAINFRAME)&&(
		lParam == WM_LBUTTONDBLCLK || 
		lParam == WM_RBUTTONDOWN))
	{       
		this->ShowWindow(SW_SHOW);
		this->SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}

	return 0;
}

BOOL CTestTrayIconMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 将图标放入系统托盘
	NOTIFYICONDATA nd;
	nd.cbSize = sizeof (NOTIFYICONDATA);
	nd.hWnd = m_hWnd;
	nd.uID = IDR_MAINFRAME;
	nd.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	nd.uCallbackMessage = WM_NOTIFYICON;
	nd.hIcon = m_hIcon;//使用的图标
	strcpy_s(nd.szTip, "托盘图标管理器");
	Shell_NotifyIcon(NIM_ADD, &nd);

	m_TrayListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER); 
	m_TrayListCtrl.InsertColumn(0,  "图标" , LVCFMT_LEFT, 38); 
	m_TrayListCtrl.InsertColumn(1,  "标识" , LVCFMT_LEFT, 70); 
	m_TrayListCtrl.InsertColumn(2,  "状态" , LVCFMT_LEFT, 36); 
	m_TrayListCtrl.InsertColumn(3,  "应用程序路径" , LVCFMT_LEFT, 239); 
	m_TrayListCtrl.InsertColumn(4,  "提示信息" , LVCFMT_LEFT,170);

	m_TrayIconMgr.Load();
	m_TrayIconMgr.ListTrayIcons();
	ShowTrayIcons();

	//SetTimer(1,1000,NULL);

	return TRUE; 
}

void CTestTrayIconMgrDlg::OnNMDblclk_ChangeIconState(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	TrayIconState& iconState=m_TrayIconMgr.GetTrayIcon(pNMItemActivate->iItem);
	
	CString csTmp=iconState.path;
	csTmp.MakeLower();
	if(csTmp.Find("explorer")==-1 && csTmp.Find("testtrayiconmgr")==-1)
	{
		iconState.bShow=!iconState.bShow;
		m_TrayListCtrl.SetItemText(pNMItemActivate->iItem,2,iconState.bShow?"显示":"隐藏");
		m_TrayIconMgr.NotifyIcon(pNMItemActivate->iItem,iconState.bShow?NIM_ADD:NIM_DELETE);
	}
	else
	{
		AfxMessageBox("本程序不对"
			+m_TrayListCtrl.GetItemText(pNMItemActivate->iItem,3)
			+"进程的图标进行操作！");
	}

	*pResult = 0;
}

void CTestTrayIconMgrDlg::DeleteInValidItem()
{
	int nCount=m_TrayListCtrl.GetItemCount();
	for (int i=0;i<nCount;i++)
	{
		BOOL bFoundInTrayIconMgr=FALSE;
		DWORD theId=atoi(m_TrayListCtrl.GetItemText(i,1));
		for (int j=0;j<m_TrayIconMgr.GetTrayIconCount();j++)
		{
			DWORD tmpId=(DWORD)&m_TrayIconMgr.GetTrayIcon(j);
			if (theId==tmpId)
			{
				bFoundInTrayIconMgr=TRUE;
				break;
			}
		}

		if (!bFoundInTrayIconMgr)
		{
			m_TrayListCtrl.DeleteItem(i);
			i--;
			nCount--;
		}
	}
}

void CTestTrayIconMgrDlg::ShowTrayIcons()
{	
	m_ImgList.DeleteImageList();
	m_ImgList.Create(16,16,ILC_COLOR24,16,16);
	m_TrayListCtrl.SetImageList(&m_ImgList,LVSIL_SMALL);

	//m_TrayListCtrl.SetRedraw(FALSE); 
	//m_TrayListCtrl.LockWindowUpdate(); 

	DeleteInValidItem();

	for (int i=0;i<m_TrayIconMgr.GetTrayIconCount();i++)
	{
		m_ImgList.Add(m_TrayIconMgr.GetTrayIcon(i).hIcon);
		DWORD dwID=(DWORD)&m_TrayIconMgr.GetTrayIcon(i);
		int iItem=FindIdInList(dwID);
		if (iItem==-1)
		{
			CString csTmp;
			csTmp.Format("%d",i+1);
			m_TrayListCtrl.InsertItem(i,csTmp,i);

			csTmp.Format("%d",dwID);
			m_TrayListCtrl.SetItemText(i,1,csTmp);

			m_TrayListCtrl.SetItemText(i,2,m_TrayIconMgr.GetTrayIcon(i).bShow?"显示":"隐藏");
			m_TrayListCtrl.SetItemText(i,3,m_TrayIconMgr.GetTrayIcon(i).path);
			m_TrayListCtrl.SetItemText(i,4,m_TrayIconMgr.GetTrayIcon(i).tips);
		}
		else
		{
			m_TrayListCtrl.SetItemText(iItem,2,m_TrayIconMgr.GetTrayIcon(i).bShow?"显示":"隐藏");
			m_TrayListCtrl.SetItemText(iItem,3,m_TrayIconMgr.GetTrayIcon(i).path);
			m_TrayListCtrl.SetItemText(iItem,4,m_TrayIconMgr.GetTrayIcon(i).tips);
		}
	}
	//m_TrayListCtrl.UnlockWindowUpdate(); 
	//m_TrayListCtrl.SetRedraw(TRUE);
}

int CTestTrayIconMgrDlg::FindIdInList(DWORD dwID)
{
	for (int i=0;i<m_TrayListCtrl.GetItemCount();i++)
	{
		DWORD theId=atoi(m_TrayListCtrl.GetItemText(i,1));
		if (dwID==theId)
		{
			return i;
		}
	}

	return -1;
}

void CTestTrayIconMgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		if ((nID&0xFFF0) == SC_MAXIMIZE)
			return;
		if ((nID&0xFFF0) == SC_MINIMIZE)
			this->ShowWindow(SW_HIDE);
		else
			CWnd::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestTrayIconMgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTestTrayIconMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestTrayIconMgrDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_TrayListCtrl.GetSafeHwnd()!=NULL)
	{
		CRect rcWnd;
		GetClientRect(&rcWnd);

		CRect rcList;
		rcList.left=rcWnd.left+10;
		rcList.right=rcWnd.right-10;
		rcList.top=rcWnd.top+10;
		rcList.bottom=rcWnd.bottom-10;

		m_TrayListCtrl.MoveWindow(&rcList);
	}
}


void CTestTrayIconMgrDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_TrayIconMgr.ListTrayIcons();
	ShowTrayIcons();

	CDialog::OnTimer(nIDEvent);
}

BOOL CTestTrayIconMgrDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_ESCAPE)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTestTrayIconMgrDlg::OnOK()
{
}

void CTestTrayIconMgrDlg::OnClose()
{
	for (int i=0;i<m_TrayIconMgr.GetTrayIconCount();i++)
	{
		if (!m_TrayIconMgr.GetTrayIcon(i).bShow)
		{
			if(IDYES==MessageBox(m_TrayIconMgr.GetTrayIcon(i).path 
				+ " 进程的图标被隐藏!!\r\n\r\n是否仍然坚持退出操作？","提示",MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
			{
				goto _exit;
				return;
			}
			else
			{
				return;
			}
		}
	}
_exit:
	m_TrayIconMgr.Save();

	NOTIFYICONDATA nd;
	nd.cbSize = sizeof (NOTIFYICONDATA);
	nd.hWnd = m_hWnd;
	nd.uID = IDR_MAINFRAME;
	nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_DELETE, &nd);	

	CDialog::OnClose();
}
