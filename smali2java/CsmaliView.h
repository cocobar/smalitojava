// CsmaliView.h : CsmaliView ��Ľӿ�
//

#pragma once


class CsmaliView : public CScrollView
{
protected: // �������л�����
	CsmaliView();
	DECLARE_DYNCREATE(CsmaliView)

private:
 

	// ����
public:
	CsmaliDoc * GetDocument() const;

	// ����
public:

	// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// ʵ��
public:
	virtual ~CsmaliView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
};

#ifndef _DEBUG  // smaliView.cpp �еĵ��԰汾
inline CsmaliDoc* CsmaliView::GetDocument() const
{
	return reinterpret_cast<CsmaliDoc*>(m_pDocument);
}
#endif


