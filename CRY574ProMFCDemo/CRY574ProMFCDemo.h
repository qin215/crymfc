
// CRY574ProMFCDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCRY574ProMFCDemoApp:
// �йش����ʵ�֣������ CRY574ProMFCDemo.cpp
//

class CCRY574ProMFCDemoApp : public CWinApp
{
public:
	CCRY574ProMFCDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCRY574ProMFCDemoApp theApp;