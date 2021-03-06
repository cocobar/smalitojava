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
	, mStrOpenPath(_T(""))
	, mStrFileListInfo(_T(""))
{

}

CNewProject::~CNewProject()
{
}

void CNewProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OPEN_PATH, mStrOpenPath);
	DDX_Control(pDX, IDC_LIST_FILE, mListFile);
	DDX_Text(pDX, IDC_EDIT_FILE_LIST_INFO, mStrFileListInfo);
}


BEGIN_MESSAGE_MAP(CNewProject, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ONESMALI, &CNewProject::OnBnClickedButtonOpenOnesmali)
END_MESSAGE_MAP()


// CNewProject 消息处理程序

BOOL CNewProject::CheckAllProjectPathSmaliFile(CString strProjectPath, CString strSearchPath) {
	CString strMatch = strSearchPath + _T("*.*");
	CString strFullName;
	CFileFind finder;

	BOOL bWorking = finder.FindFile(strMatch);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory()) {
			if (!CheckAllProjectPathSmaliFile(strProjectPath, finder.GetFilePath() + _T("\\"))) {
				finder.Close();
				return FALSE;
			}
		}
		else if (finder.GetFileName().Find(_T(".smali")) > 0)
		{
			CStdioFile cStdFile;
			strFullName = finder.GetFilePath();
			CString strSmaliProjectPath;
			if (cStdFile.Open(strFullName, CFile::modeRead)) {
				CString strLine;
				while (cStdFile.ReadString(strLine)) {
					if (strLine.Find(_T(".class")) == 0) {
						int nFindL = strLine.Find(_T("L"));
						if (nFindL > 0) {
							CString strClassName = strLine.Right(strLine.GetLength() - nFindL);
							strClassName = strClassName.Right(strClassName.GetLength() - 1);
							strClassName = strClassName.Left(strClassName.GetLength() - 1);
							strClassName.Replace('/', '\\');
							strClassName += CString(_T(".smali"));

							listFileName.push_back(strClassName);

							if (strFullName.Find(strClassName) > 0) {
								strSmaliProjectPath = strFullName.Left(strFullName.GetLength() - strClassName.GetLength());
								break;
							}
						}

					}
				}
				cStdFile.Close();
			}

			if (!strSmaliProjectPath.IsEmpty()) {
				if (strSmaliProjectPath.CompareNoCase(strProjectPath)) {
					finder.Close();
					return FALSE;
				}
			}
		}
	}

	finder.Close();

	return  TRUE;
}


void CNewProject::OnBnClickedButtonOpenOnesmali()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString(_T("Smali(*.smali)|*.smali|All File|*|| ")), NULL);
	if (IDOK == fileDlg.DoModal()) {

		CStdioFile cStdFile;

		CString strSmaliProjectPath;
		CString strSmaliFilePath = fileDlg.GetPathName();

		// 寻找到工程目录
		if (cStdFile.Open(strSmaliFilePath, CFile::modeRead)) {
			CString strLine;
			while (cStdFile.ReadString(strLine)) {
				if (strLine.Find(_T(".class")) == 0) {
					int nFindL = strLine.Find(_T("L"));
					if (nFindL > 0) {
						CString strClassName = strLine.Right(strLine.GetLength() - nFindL);
						strClassName = strClassName.Right(strClassName.GetLength() - 1);
						strClassName = strClassName.Left(strClassName.GetLength() - 1);
						strClassName.Replace('/', '\\');
						strClassName += CString(_T(".smali"));
						if (strSmaliFilePath.Find(strClassName) > 0) {
							strSmaliProjectPath = strSmaliFilePath.Left(strSmaliFilePath.GetLength() - strClassName.GetLength());
							break;
						}
					}
					
				}
			}
			cStdFile.Close();
		}

		listFileName.clear();

		if (!CheckAllProjectPathSmaliFile(strSmaliProjectPath, strSmaliProjectPath)) {
			strSmaliProjectPath.Empty();
			mStrFileListInfo.Empty();
		}
		else {

			while (mListFile.GetCount() > 0) {
				mListFile.DeleteString(0);
			}
			for (unsigned int i = 0; i < listFileName.size(); i++) {
				mListFile.AddString(listFileName[i]);
			}

			mStrFileListInfo.Format(_T("总计 %d 个 smali 文件"), listFileName.size());
		}

		if (!strSmaliProjectPath.IsEmpty()) {
			mStrOpenPath = strSmaliProjectPath;
		}
		else {
			mStrOpenPath = strSmaliFilePath;
		}

		strProjectRootPath = mStrOpenPath;

		this->UpdateData(FALSE);
	}
}

CString CNewProject::GetProjectRootPath() {
	return strProjectRootPath;
}
