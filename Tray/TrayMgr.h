#pragma once
class TrayMgr
{
private:
	struct STrayInfo
	{
		HICON hIcon;
		CString strTitle;
		CString strFilePath;
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
};

