
通过Spy++可以看出，系统托盘图标是一个ToolbarWindow32控件，要对其进行控制，只要用FindWindow和FindWindowEx找到其句柄，并发送ToolbarWindow32支持的消息即可。

1.获取图标数量:TB_BUTTONCOUNT
DWORD dwCount=::SendMessage(hToolbarWindow32, TB_BUTTONCOUNT, NULL, NULL);

2.获取图标的具体信息:TB_GETBUTTON
::SendMessage(hToolbarWindow32, TB_GETBUTTON, iIndex, (LPARAM)(LPTBBUTTON) lpButton);
难点在于，此处的SendMessage是跨进程操作，lpButton要在ToolbarWindow32所在的进程中申请。

具体代码如下：
//打开ToolbarWindow32所在的进程，就是explorer.exe
HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,x_GetWndProcessId(hToolbarWindow32));

//在进程explorer.exe中申请内存
LPVOID lpButton=VirtualAllocEx(hProcess,NULL,sizeof(TBBUTTON),MEM_COMMIT, PAGE_READWRITE);

//执行TB_GETBUTTON
::SendMessage(hToolbarWindow32, TB_GETBUTTON, i, (LPARAM)lpButton);

//从进程explorer.exe中读取需要的数据
TBBUTTON TButton;
::ReadProcessMemory(hProcess, lpButton, &TButton, sizeof(TBBUTTON), NULL);

TRAYDATA TrayData;
::ReadProcessMemory(hProcess, (LPVOID)TButton.dwData, &TrayData, sizeof(TRAYDATA), NULL);

TCHAR szTips[1024];
::ReadProcessMemory(hProcess, (LPVOID)TButton.iString, szTips, 1024, NULL);

//szTips是Unicode字符串，需要转换
USES_CONVERSION; 
CString csTips=W2A((WCHAR*)(szTips));

//释放在进程explorer.exe中申请的内存
::VirtualFreeEx(hProcess, lpButton, sizeof(TBBUTTON), MEM_FREE);
CloseHandle(hProcess);

备注：TBBUTTON.dwData的格式如下：
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

3.程序截图与源码
双击某行可实现托盘图标的隐藏或显示。
