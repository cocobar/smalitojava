// CNewProject.cpp: 实现文件
//

#include "stdafx.h"
#include "smali2java.h"
#include "CNewProject.h"
#include "afxdialogex.h"


// CNewProject 对话框

IMPLEMENT_DYNAMIC(CNewProject, CDialog)

CNewProject::CNewProject(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_NEW_PROJECT, pParent)
{

}

CNewProject::~CNewProject()
{
}

void CNewProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNewProject, CDialog)
END_MESSAGE_MAP()


// CNewProject 消息处理程序
