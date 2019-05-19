#include "stdafx.h"
#include "TrayMgr.h"
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

TrayMgr::TrayMgr()
{
}


TrayMgr::~TrayMgr()
{
}

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

BOOL TrayMgr::IsWow64()
{
	BOOL bIsWow64 = FALSE;

	LPFN_ISWOW64PROCESS
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
			GetModuleHandle(_T("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
}

HWND TrayMgr::FindTrayWnd()
{
	HWND hWnd = NULL;

	hWnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	hWnd = FindWindowEx(hWnd, NULL, _T("TrayNotifyWnd"), NULL);
	hWnd = FindWindowEx(hWnd, NULL, _T("SysPager"), NULL);
	hWnd = FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);

	return hWnd;
}

DWORD TrayMgr::GetWndProcessId(HWND hWnd)
{
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	return dwProcessId;
}

void TrayMgr::EnumNotifyWindow(HWND hWnd)
{
	m_hToolbarWindow32 = hWnd;
	m_TrayInfoArray.RemoveAll();

	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, GetWndProcessId(hWnd));
	if (hProcess == NULL) {
		return;
	}

	LPVOID lAddress = VirtualAllocEx(hProcess, 0, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lAddress == NULL) {
		return;
	}
	DWORD lTextAdr = 0;
	BYTE buff[1024] = { 0 };
	TCHAR ptb[256] = { 0 };
	//HWND hMainWnd = NULL;
	STrayInfo currInfo;
	//RECT rccc;
	int nDataOffset = sizeof(TBBUTTON) - sizeof(INT_PTR) - sizeof(DWORD_PTR);
	int nStrOffset = 18;
	if (IsWow64()) {
		nDataOffset += 4;
		nStrOffset += 6;
	}

	//获取图标个数
	DWORD lButton = ::SendMessage(hWnd, TB_BUTTONCOUNT, 0, 0);
	for (DWORD i = 0; i < lButton; i++) {
		::SendMessage(hWnd, TB_GETBUTTON, i, (LPARAM)lAddress);
		//读坐标
		//读文本地址
		::ReadProcessMemory(hProcess, (LPVOID)((DWORD)lAddress + nDataOffset), &lTextAdr, 4, 0);
		if (lTextAdr != -1) {
			//读文本,得到TRAYDATA的地址
			::ReadProcessMemory(hProcess, (LPCVOID)lTextAdr, buff, 1024, 0);
			//hMainWnd = (HWND)(*((DWORD*)buff));
			currInfo.strFilePath = (WCHAR *)buff + nStrOffset;
			currInfo.nid_target.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
			currInfo.nid_target.hWnd = (HWND)(*((DWORD*)buff));
			currInfo.nid_target.uID = (UINT)(*((UINT*)buff + 1));
			currInfo.nid_target.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			currInfo.nid_target.uCallbackMessage = (UINT)(*((DWORD*)buff + 3));
			currInfo.nid_target.hIcon = (HICON)(*((DWORD*)buff + 6));
			memcpy(currInfo.nid_target.szTip, (WCHAR *)buff + nStrOffset + MAX_PATH, sizeof(currInfo.nid_target.szTip));

			HANDLE hProcessTmp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetWndProcessId(currInfo.nid_target.hWnd));
			if (hProcessTmp != NULL)
			{
				GetModuleFileNameEx(hProcessTmp, NULL, (LPWSTR)(LPCWSTR)currInfo.strFilePath, MAX_PATH);
				CloseHandle(hProcessTmp);
			}
			else 
			{
				int err = GetLastError();
				OutputDebugString(currInfo.strFilePath);
			}

			currInfo.strName = currInfo.strFilePath; 
			currInfo.strName.Delete(0, currInfo.strFilePath.ReverseFind(L'\\') + 1);

			ICONINFO iconinfo;
			if (GetIconInfo(currInfo.nid_target.hIcon, &iconinfo) == 0)
			{
				SHFILEINFO SFI;
				SHGetFileInfo(currInfo.strFilePath, 0, &SFI, sizeof(SFI), SHGFI_SMALLICON | SHGFI_ICON);
				if (SFI.hIcon != NULL)
					currInfo.nid_target.hIcon = SFI.hIcon;
				else
					currInfo.nid_target.hIcon = (HICON)LoadIcon(NULL, IDI_ERROR);
			}

			if (currInfo.strName.MakeLower().Left(wcslen(currInfo.strName) - 4) == _T("bellring")) {
				//HICON _nerIcon = (HICON)LoadIcon(NULL, IDI_ERROR);
				//memcpy(((DWORD*)buff + 6), &_nerIcon, sizeof(_nerIcon));
				//DWORD dwNumberOfBytesRead;
				//int write_ = ::WriteProcessMemory(hProcess, (LPVOID)lTextAdr, buff, 1024, &dwNumberOfBytesRead);	//写入内存
				//if (write_ == 0) {
				//	MessageBox(hWnd, L"写入内存值失败!", NULL, 0);
				//}
				//else ::SendMessage(m_hToolbarWindow32, WM_PAINT, 0, 0);

				//if (Shell_NotifyIcon(NIM_DELETE, &currInfo.nid_target)) {
				//	currInfo.nid_target.hIcon = (HICON)LoadIcon(NULL, IDI_ERROR);
				//	Shell_NotifyIcon(NIM_ADD, &currInfo.nid_target);
				//}
				//else
				//	MessageBox(hWnd, L"change icon failed!", NULL, 0);

				wchar_t buf[128];
				swprintf_s(buf, L"MESSAGE:%d,%d\n", WM_SHOWTASK, currInfo.nid_target.uCallbackMessage);
				OutputDebugString(buf);
				for (int i = 0; i < sizeof(buff); i++) {
					UINT a = (UINT)(*((DWORD*)buff + i));
					if (a == WM_SHOWTASK) {
						wchar_t _buf[128];
						swprintf_s(_buf, L"WM_SHOWTASK:%d\n", i);
						OutputDebugString(_buf);
					}
				}
			}

			m_TrayInfoArray.Add(currInfo);
		}
		

	}
	::VirtualFreeEx(hProcess, lAddress, 4096, MEM_RELEASE);
	CloseHandle(hProcess);
}

void TrayMgr::ShowTray(CListCtrl* pListCtrl)
{
	pListCtrl->DeleteAllItems();
	static CImageList imgList;
	imgList.DeleteImageList();
	imgList.Create(16, 16, ILC_COLOR24, 16, 16);
	pListCtrl->SetImageList(&imgList, LVSIL_SMALL);

	for (int i=0;i<m_TrayInfoArray.GetCount();i++)
	{
		CString strTemp;
		strTemp.Format(L"%d", i);
		pListCtrl->InsertItem(i, strTemp, i);
		imgList.Add(m_TrayInfoArray.GetAt(i).nid_target.hIcon);
		pListCtrl->SetItemText(i, 1, m_TrayInfoArray.GetAt(i).strFilePath);
		pListCtrl->SetItemText(i, 2, m_TrayInfoArray.GetAt(i).nid_target.szTip);
	}
}

void TrayMgr::AddBtn(int iID)
{
	STrayInfo currinfo = m_TrayInfoArray.GetAt(iID);
	currinfo.nid_self.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	currinfo.nid_self.hWnd = m_hParent;
	currinfo.nid_self.uID = iID;
	currinfo.nid_self.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	currinfo.nid_self.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
	currinfo.nid_self.hIcon = (HICON)LoadIcon(NULL, IDI_ERROR);
	//wcscpy(nid.szTip, L"BellRing"); //信息提示条
	memcpy(currinfo.nid_self.szTip, currinfo.nid_target.szTip, sizeof(currinfo.nid_self.szTip));
	Shell_NotifyIcon(NIM_ADD, &currinfo.nid_self); //在托盘区添加图标
}

void TrayMgr::DeleteBtn(int iID)
{
	if (!m_hToolbarWindow32)
		return;

	::SendMessage(m_hToolbarWindow32, TB_DELETEBUTTON, iID, 0);
	::SendMessage(m_hToolbarWindow32, WM_PAINT, 0, 0);
}

void TrayMgr::SendMessage(UINT nID, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_RBUTTONUP: {
		OutputDebugString(L"WM_RBUTTONUP\n");
	}
	break;
	}
	STrayInfo currinfo = m_TrayInfoArray.GetAt(nID);
	if(currinfo.nid_target.uCallbackMessage == WM_SHOWTASK)
		OutputDebugString(L"YES\n");
	int a = ::SendMessage(currinfo.nid_target.hWnd, currinfo.nid_target.uCallbackMessage, currinfo.nid_target.uID, lParam);
	int err = GetLastError();
	wchar_t _buf[128];
	swprintf_s(_buf, L"GetLastError:%d\n", a);
	OutputDebugString(_buf);
}
