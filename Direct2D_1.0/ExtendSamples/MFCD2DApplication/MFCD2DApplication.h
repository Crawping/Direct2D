
// MFCD2DApplication.h : MFCD2DApplication Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CMFCD2DApplicationApp:
// �йش����ʵ�֣������ MFCD2DApplication.cpp
//

class CMFCD2DApplicationApp : public CWinAppEx
{
public:
	CMFCD2DApplicationApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCD2DApplicationApp theApp;
