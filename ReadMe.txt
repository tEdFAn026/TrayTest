
ͨ��Spy++���Կ�����ϵͳ����ͼ����һ��ToolbarWindow32�ؼ���Ҫ������п��ƣ�ֻҪ��FindWindow��FindWindowEx�ҵ�������������ToolbarWindow32֧�ֵ���Ϣ���ɡ�

1.��ȡͼ������:TB_BUTTONCOUNT
DWORD dwCount=::SendMessage(hToolbarWindow32, TB_BUTTONCOUNT, NULL, NULL);

2.��ȡͼ��ľ�����Ϣ:TB_GETBUTTON
::SendMessage(hToolbarWindow32, TB_GETBUTTON, iIndex, (LPARAM)(LPTBBUTTON) lpButton);
�ѵ����ڣ��˴���SendMessage�ǿ���̲�����lpButtonҪ��ToolbarWindow32���ڵĽ��������롣

����������£�
//��ToolbarWindow32���ڵĽ��̣�����explorer.exe
HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,x_GetWndProcessId(hToolbarWindow32));

//�ڽ���explorer.exe�������ڴ�
LPVOID lpButton=VirtualAllocEx(hProcess,NULL,sizeof(TBBUTTON),MEM_COMMIT, PAGE_READWRITE);

//ִ��TB_GETBUTTON
::SendMessage(hToolbarWindow32, TB_GETBUTTON, i, (LPARAM)lpButton);

//�ӽ���explorer.exe�ж�ȡ��Ҫ������
TBBUTTON TButton;
::ReadProcessMemory(hProcess, lpButton, &TButton, sizeof(TBBUTTON), NULL);

TRAYDATA TrayData;
::ReadProcessMemory(hProcess, (LPVOID)TButton.dwData, &TrayData, sizeof(TRAYDATA), NULL);

TCHAR szTips[1024];
::ReadProcessMemory(hProcess, (LPVOID)TButton.iString, szTips, 1024, NULL);

//szTips��Unicode�ַ�������Ҫת��
USES_CONVERSION; 
CString csTips=W2A((WCHAR*)(szTips));

//�ͷ��ڽ���explorer.exe��������ڴ�
::VirtualFreeEx(hProcess, lpButton, sizeof(TBBUTTON), MEM_FREE);
CloseHandle(hProcess);

��ע��TBBUTTON.dwData�ĸ�ʽ���£�
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

3.�����ͼ��Դ��
˫��ĳ�п�ʵ������ͼ������ػ���ʾ��
