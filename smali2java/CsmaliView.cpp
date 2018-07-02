// smaliView.cpp : CsmaliView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "smali2java.h"
#endif

#include "CsmaliDoc.h"
#include "CsmaliView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Csmali2javaView

IMPLEMENT_DYNCREATE(CsmaliView, CView)

BEGIN_MESSAGE_MAP(CsmaliView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CsmaliView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// Csmali2javaView ����/����

CsmaliView::CsmaliView()
{
	// TODO:  �ڴ˴���ӹ������

}

CsmaliView::~CsmaliView()
{
}

BOOL CsmaliView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// Csmali2javaView ����

void CsmaliView::OnDraw(CDC* pDC)
{
	CsmaliDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���

	CDC* pDc = GetDC();                          // ע�⣺CPaintDCֻ������OnPaint
	CRect rect;
	GetClientRect(&rect);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	

	for (unsigned int i = 0; i < pDoc->listString.size(); i++) {

		pDC->TextOut(rect.right / 2, rect.bottom / 2, pDoc->listString[i], pDoc->listString[i].GetLength());
	}
}


// Csmali2javaView ��ӡ


void CsmaliView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CsmaliView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CsmaliView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CsmaliView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}

void CsmaliView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CsmaliView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Csmali2javaView ���

#ifdef _DEBUG
void CsmaliView::AssertValid() const
{
	CView::AssertValid();
}

void CsmaliView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CsmaliDoc* CsmaliView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CsmaliDoc)));
	return (CsmaliDoc*)m_pDocument;
}
#endif //_DEBUG


// Csmali2javaView ��Ϣ�������
