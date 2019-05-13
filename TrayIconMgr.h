#pragma once

class TrayIconState
{
public:
	CString path;
	CString tips;
	BOOL bShow;
	HWND hWnd;
	HICON hIcon;
	UINT uID;
	UINT uCallbackMessage;
public:
	TrayIconState();
	TrayIconState(TrayIconState& obj);
	TrayIconState& operator=(TrayIconState& obj);
	BOOL operator==(TrayIconState& obj);
	void Dump();
};

class CTrayIconMgr
{
private:
	struct TRAYDATA
	{
		HWND hWnd;
		UINT uID;
		UINT uCallbackMessage;
		DWORD Reserved1[2];
		HICON hIcon;
		DWORD Reserved2[3];
		TCHAR szExePath[MAX_PATH];
		TCHAR szTip[128]; 
	};

private:
	CArray<TrayIconState,TrayIconState> m_TrayIconStateArray;
	CArray<CString,CString> m_ProcessPathArray;

	TrayIconState iconState;
private:
	HWND x_FindToolbarWindow32Wnd();
	DWORD x_GetWndProcessId(HWND hWnd);
	void x_ListProcesses();
	BOOL x_CheckIconExsit( TrayIconState& iconState );
	void x_DeleteInValidIcon();
public:
	CTrayIconMgr(void);
	~CTrayIconMgr(void);

	BOOL IsWow64();
	BOOL ListTrayIcons();

	TrayIconState& GetTrayIcon(IN int iItem);

	int GetTrayIconCount();

	void NotifyIcon(int iItem,UINT uCmd);

	void Load();
	void Save();
};
