
// TestTrayIconMgr.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestTrayIconMgrApp:
// �йش����ʵ�֣������ TestTrayIconMgr.cpp
//

class CTestTrayIconMgrApp : public CWinAppEx
{
public:
	CTestTrayIconMgrApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestTrayIconMgrApp theApp;