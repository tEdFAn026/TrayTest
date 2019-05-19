#pragma once
#define WM_SHOWTASK (WM_USER +1)
class TrayMgr
{
private:
	struct STrayInfo
	{
		CString strName;
		CString strFilePath;
		NOTIFYICONDATA nid_target;
		NOTIFYICONDATA nid_self;
	};

private:
	CArray<STrayInfo, STrayInfo> m_TrayInfoArray;
public:
	TrayMgr();
	~TrayMgr();
private:
	static BOOL IsWow64();
	static HWND FindTrayWnd();
private:
	DWORD GetWndProcessId(HWND hWnd);
public:
	void EnumNotifyWindow(HWND hWnd = FindTrayWnd());
	void ShowTray(CListCtrl* pListCtrl);
	void AddBtn(int iID);
	void DeleteBtn(int iID);
	void SendMessage(UINT nID, LPARAM lParam);
private:
	HWND m_hToolbarWindow32;
public:
	HWND m_hParent;
};

