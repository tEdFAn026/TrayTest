#include "StdAfx.h"
#include "TrayIconMgr.h"

#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

#include "Markup.h"

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

CTrayIconMgr::CTrayIconMgr(void)
{
	m_TrayIconStateArray.RemoveAll();
}

CTrayIconMgr::~CTrayIconMgr(void)
{
}

HWND CTrayIconMgr::x_FindToolbarWindow32Wnd()
{
	HWND hWnd = ::FindWindow("Shell_TrayWnd", NULL);
	if (hWnd)
	{
		hWnd = ::FindWindowEx(hWnd, NULL, "TrayNotifyWnd", NULL);
		if (hWnd)
		{
			hWnd = ::FindWindowEx(hWnd, NULL, "SysPager", NULL);
			if (hWnd)
			{
				hWnd = ::FindWindowEx(hWnd, NULL, "ToolbarWindow32", NULL);
			}
		}
	}
	return hWnd;
}

DWORD CTrayIconMgr::x_GetWndProcessId(HWND hWnd)
{
	DWORD dwProcessId=0;
	DWORD dwThreadId=GetWindowThreadProcessId(hWnd,&dwProcessId);
	return dwProcessId;
}

void CTrayIconMgr::x_ListProcesses()
{
	m_ProcessPathArray.RemoveAll();

	DWORD pidProcess[1024];
	memset(pidProcess,-1,1024*sizeof(DWORD));
	DWORD cbNeeded=0;
	EnumProcesses(pidProcess,1024,&cbNeeded);
	for (DWORD i=0;pidProcess[i]!=-1;i++)
	{
		TCHAR szPath[MAX_PATH]={0};
		HANDLE hProcessTmp=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pidProcess[i]);
		int ret=GetModuleFileNameEx(hProcessTmp,NULL,szPath,MAX_PATH);
		CloseHandle(hProcessTmp);
		if(ret==0)
		{
			sprintf_s(szPath,"[%d]无法获取路径",pidProcess[i]);
		}
		m_ProcessPathArray.Add(szPath);
		
	}
}

BOOL CTrayIconMgr::IsWow64()
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

BOOL CTrayIconMgr::ListTrayIcons()
{
	x_ListProcesses();

	HWND hToolbarWindow32=x_FindToolbarWindow32Wnd();
	if (hToolbarWindow32==NULL)
	{
		return FALSE;
	}

	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,x_GetWndProcessId(hToolbarWindow32));
	if (hProcess == NULL) {
		return FALSE;
	}

	int nDataOffset = sizeof(TBBUTTON) - sizeof(INT_PTR) - sizeof(DWORD_PTR);
	int nStrOffset = 18;
	if (IsWow64()) {
		nDataOffset += 4;
		nStrOffset += 6;
	}

	LPVOID lpButton=VirtualAllocEx(hProcess,NULL,sizeof(TBBUTTON),MEM_COMMIT, PAGE_READWRITE);

	TCHAR szTips[1024];
	TCHAR szPath[MAX_PATH];
	TBBUTTON TButton;
	TRAYDATA TrayData;
	DWORD lTextAdr = 0;
	DWORD dwCount=::SendMessage(hToolbarWindow32, TB_BUTTONCOUNT, NULL, NULL);
	for (DWORD i = 0; i < dwCount; i++)
	{
		::SendMessage(hToolbarWindow32, TB_GETBUTTON, i, (LPARAM)lpButton);
		::ReadProcessMemory(hProcess, (LPVOID)((DWORD)lpButton + nDataOffset), &lTextAdr, 4, 0);
		::ReadProcessMemory(hProcess, lpButton, &TButton, sizeof(TBBUTTON), NULL);
		::ReadProcessMemory(hProcess, (LPVOID)TButton.dwData, &TrayData, sizeof(TRAYDATA), NULL);
		::ReadProcessMemory(hProcess, (LPVOID)TButton.iString, szTips, 1024, NULL);
		if (lTextAdr == -1)
			continue;
		CString strFilePath;
		CString strTile;
		HWND hMainWnd = NULL;
		BYTE buff[1024] = { 0 };
		ReadProcessMemory(hProcess, (LPCVOID)lTextAdr, buff, 1024, 0);
		hMainWnd = (HWND)(*((DWORD*)buff));
		HANDLE hProcessTmp=OpenProcess(PROCESS_ALL_ACCESS,FALSE,x_GetWndProcessId(hMainWnd));
		if (hProcessTmp==NULL)
		{
			continue;
		}
		GetModuleFileNameEx(hProcessTmp,NULL,szPath,MAX_PATH);
		CloseHandle(hProcessTmp);


		//CString strFilePath;
		//CString strTile;
		//BYTE buff[1024] = { 0 };
		//ReadProcessMemory(hProcess, (LPCVOID)lTextAdr, buff, 1024, 0);
		////hMainWnd = (HWND)(*((DWORD*)buff));
		//strFilePath = (WCHAR *)buff + nStrOffset;
		//wsprintf(szPath, _T("%s"), strFilePath);
		////strTile = (WCHAR *)buff + nStrOffset + MAX_PATH;

		OutputDebugString(szPath);

		HICON hIcon=NULL;
		ICONINFO iconinfo;
		if (GetIconInfo(TrayData.hIcon, &iconinfo) != 0)
		{
			hIcon = TrayData.hIcon;
		}
		else
		{
			SHFILEINFO SFI;
			SHGetFileInfo(szPath, 0, &SFI, sizeof(SFI), SHGFI_SMALLICON | SHGFI_ICON); 
			hIcon=SFI.hIcon;
		}

		USES_CONVERSION; 
		iconState.tips=W2A((WCHAR*)(szTips));
		iconState.hIcon=hIcon;
		iconState.hWnd=TrayData.hWnd;
		iconState.path=szPath;
		iconState.bShow=TRUE;
		iconState.uID=TrayData.uID;
		iconState.uCallbackMessage=TrayData.uCallbackMessage;

		CString csPath=iconState.path;
		csPath.MakeLower();
		if (csPath.Find("testtrayiconmgr")!=-1)
		{
			continue;
		}

		if (!x_CheckIconExsit(iconState))
		{
			m_TrayIconStateArray.Add(iconState);
		}
	}

	::VirtualFreeEx(hProcess, lpButton, sizeof(TBBUTTON), MEM_RELEASE);
	CloseHandle(hProcess);

	x_DeleteInValidIcon();

	return TRUE;
}

void CTrayIconMgr::x_DeleteInValidIcon()
{
	int nCount=GetTrayIconCount();
	for (int i=0;i<nCount;i++)
	{
		TrayIconState& TmpIconState=m_TrayIconStateArray[i];
		CString csTheProcessPath=TmpIconState.path;
		csTheProcessPath.MakeLower();
		//TRACE("%s\n",csTheProcessPath);

		BOOL bFoundProcessPath=FALSE;
		for (int j=0;j<m_ProcessPathArray.GetCount();j++)
		{
			CString csProcessPath=m_ProcessPathArray[j];
			csProcessPath.MakeLower();
			
			if (csProcessPath==csTheProcessPath)
			{
				bFoundProcessPath=TRUE;
				break;
			}
		}

		if (!bFoundProcessPath)
		{
			m_TrayIconStateArray.RemoveAt(i);
			nCount--;
			i--;
		}
	}
}

TrayIconState& CTrayIconMgr::GetTrayIcon(IN int iItem)
{
	return m_TrayIconStateArray[iItem];
}

int CTrayIconMgr::GetTrayIconCount()
{
	return m_TrayIconStateArray.GetCount();
}

void CTrayIconMgr::NotifyIcon(int iItem,UINT uCmd)
{
	TrayIconState& iconState=GetTrayIcon(iItem);

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = iconState.hWnd;
	nid.uID = iconState.uID;
	nid.uCallbackMessage = iconState.uCallbackMessage;
	nid.hIcon = iconState.hIcon;
	memcpy(nid.szTip, iconState.tips, sizeof(nid.szTip));
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	::Shell_NotifyIcon(uCmd, &nid);
}

BOOL CTrayIconMgr::x_CheckIconExsit( TrayIconState& iconState )
{
	for (int i=0;i<GetTrayIconCount();i++)
	{
		if (iconState == m_TrayIconStateArray[i])
		{
			m_TrayIconStateArray[i].tips=iconState.tips;
			m_TrayIconStateArray[i].hIcon=iconState.hIcon;
			return TRUE;
		}
	}
	return FALSE;
}

void CTrayIconMgr::Load()
{
	m_TrayIconStateArray.RemoveAll();

	CMarkup xml;
	xml.Load("TrayIconMgr.xml");
	xml.FindElem("TrayIconStates");
	xml.IntoElem();
	while (xml.FindElem("TrayIconState") )
	{
		TrayIconState iconState;

		xml.IntoElem();

		xml.FindElem("path");
		iconState.path=xml.GetData();

		xml.FindElem("tips");
		iconState.tips=xml.GetData();

		xml.FindElem("bShow");
		iconState.bShow=((xml.GetData()=="True")?1:0);

		xml.FindElem("hWnd");

		int n;
		sscanf(xml.GetData(),"%x",&n);
		iconState.hWnd=(HWND)n;

		xml.FindElem("hIcon");
		sscanf(xml.GetData(),"%x",&n);
		iconState.hIcon=(HICON)n;

		xml.FindElem("uID");
		sscanf(xml.GetData(),"%x",&n);
		iconState.uID=n;

		xml.FindElem("uCallbackMessage");
		sscanf(xml.GetData(),"%x",&n);
		iconState.uCallbackMessage=n;

		xml.OutOfElem();

		m_TrayIconStateArray.Add(iconState);
	}
}

void CTrayIconMgr::Save()
{
	CStdioFile fileXml;
	fileXml.Open("TrayIconMgr.xml",CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	fileXml.WriteString("<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\r\n");
	fileXml.WriteString("<TrayIconStates>\r\n");
	for (int i=0;i<GetTrayIconCount();i++)
	{
		TrayIconState& iconState=m_TrayIconStateArray[i];
		fileXml.WriteString("  <TrayIconState>\r\n");

		fileXml.WriteString("    <path>"+iconState.path+"</path>\r\n");
		fileXml.WriteString("    <tips>"+iconState.tips+"</tips>\r\n");

		CString csTemp;
		csTemp.Format("%s",iconState.bShow?"True":"False");
		fileXml.WriteString("    <bShow>"+csTemp+"</bShow>\r\n");

		csTemp.Format("0x%08X",iconState.hWnd);
		fileXml.WriteString("    <hWnd>"+csTemp+"</hWnd>\r\n");

		csTemp.Format("0x%08X",iconState.hIcon);
		fileXml.WriteString("    <hIcon>"+csTemp+"</hIcon>\r\n");

		csTemp.Format("0x%08X",iconState.uID);
		fileXml.WriteString("    <uID>"+csTemp+"</uID>\r\n");

		csTemp.Format("0x%08X",iconState.uCallbackMessage);
		fileXml.WriteString("    <uCallbackMessage>"+csTemp+"</uCallbackMessage>\r\n");

		fileXml.WriteString("  </TrayIconState>\r\n");
	}
	fileXml.WriteString("</TrayIconStates>\r\n");
	fileXml.Close();
}

TrayIconState::TrayIconState()
{
}

TrayIconState::TrayIconState( TrayIconState& obj )
{
	*this=obj;
}

TrayIconState& TrayIconState::operator=( TrayIconState& obj )
{
	path=obj.path;
	tips=obj.tips;
	bShow=obj.bShow;
	hWnd=obj.hWnd;
	hIcon=obj.hIcon;
	uID=obj.uID;
	uCallbackMessage=obj.uCallbackMessage;

	return *this;
}

void TrayIconState::Dump()
{
	TRACE("TrayIconState(0x%08X):\n",this);
	TRACE("path=%s\n",path);
	TRACE("tips=%s\n",tips);
	TRACE("bShow=%s\n",bShow?"TRUE":"FALSE");
	TRACE("hWnd=0x%08X\n",hWnd);
	TRACE("hIcon=0x%08X\n",hIcon);
	TRACE("uID=%08d\n",uID);
	TRACE("uCallbackMessage=%08d\n",uCallbackMessage);
	TRACE("\n");
}

BOOL TrayIconState::operator==( TrayIconState& obj )
{
	return (path==obj.path && 
			/*tips==obj.tips &&*/ 
			/*bShow==obj.bShow && */
			hWnd==obj.hWnd &&
			/*hIcon==obj.hIcon && */
			uID==obj.uID && 
			uCallbackMessage==obj.uCallbackMessage);
}

