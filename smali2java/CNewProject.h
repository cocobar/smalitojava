#pragma once

#include <vector>
// CNewProject 对话框

class CNewProject : public CDialog
{
	DECLARE_DYNAMIC(CNewProject)

private:
	std::vector<CString> listFileName;
	CString strProjectRootPath;

public:
	CNewProject(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNewProject();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEW_PROJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenOnesmali();
	BOOL CheckAllProjectPathSmaliFile(CString strProjectPath, CString strSearchPath);
	CString mStrOpenPath;
	CListBox mListFile;
	CString mStrFileListInfo;

	CString GetProjectRootPath();
};
