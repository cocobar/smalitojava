
// smali2javaView.h : Csmali2javaView ��Ľӿ�
//

#pragma once


class Csmali2javaView : public CView
{
protected: // �������л�����
	Csmali2javaView();
	DECLARE_DYNCREATE(Csmali2javaView)

// ����
public:
	Csmali2javaDoc* GetDocument() const;

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
	virtual ~Csmali2javaView();
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
};

#ifndef _DEBUG  // smali2javaView.cpp �еĵ��԰汾
inline Csmali2javaDoc* Csmali2javaView::GetDocument() const
   { return reinterpret_cast<Csmali2javaDoc*>(m_pDocument); }
#endif

