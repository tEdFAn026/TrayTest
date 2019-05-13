
// TestTrayIconMgrDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "TrayIconMgr.h"

#define WM_NOTIFYICON   WM_USER+5

// CTestTrayIconMgrDlg �Ի���
class CTestTrayIconMgrDlg : public CDialog
{
// ����
public:
	CTestTrayIconMgrDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTTRAYICONMGR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDblclk_ChangeIconState(NMHDR *pNMHDR, LRESULT *pResult);	

	void ShowTrayIcons();
	CListCtrl m_TrayListCtrl;
	CImageList m_ImgList;
	CTrayIconMgr m_TrayIconMgr;
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	int FindIdInList(DWORD dwID);
	void DeleteInValidItem();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
};
