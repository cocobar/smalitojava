
// smali2java.h : smali2java Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// Csmali2javaApp:
// �йش����ʵ�֣������ smali2java.cpp
//

class Csmali2javaApp : public CWinAppEx
{
public:
	Csmali2javaApp();


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


	void CreateProject();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Csmali2javaApp theApp;
