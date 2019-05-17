#pragma once
class TrayMgr
{
private:
	struct STrayInfo
	{
		CString strName;
		CString strFilePath;
		NOTIFYICONDATA nid;
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
private:
	HWND m_hToolbarWindow32;
};

