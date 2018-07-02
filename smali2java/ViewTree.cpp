
#include "stdafx.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)

	ON_NOTIFY_REFLECT(NM_DBLCLK, &CViewTree::OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

CString CViewTree::GetRootPath(HTREEITEM hTreeItem) {

	CString strRootPath;

	while (hTreeItem) {
		CString strNode = GetItemText(hTreeItem);
		if (strRootPath.IsEmpty()) {
			strRootPath = strNode;
		}else {
			strRootPath = strNode + CString(_T("\\")) + strRootPath;
		}
		
		hTreeItem = GetParentItem(hTreeItem);
	}

	return strRootPath;
}


void CViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	// 这个是选中的列表
	HTREEITEM hTreeItem = GetSelectedItem();
	if (this->GetItemData(hTreeItem) == 1) {
		CString S1 = GetRootPath(hTreeItem);
		AfxMessageBox(CString(_T("双击")) + S1);

		AfxGetApp()->OpenDocumentFile(S1);
	}

	

	*pResult = 0;

	
}
