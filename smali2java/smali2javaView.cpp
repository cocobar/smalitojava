
// smali2javaView.cpp : Csmali2javaView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "smali2java.h"
#endif

#include "smali2javaDoc.h"
#include "smali2javaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Csmali2javaView

IMPLEMENT_DYNCREATE(Csmali2javaView, CView)

BEGIN_MESSAGE_MAP(Csmali2javaView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Csmali2javaView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// Csmali2javaView ����/����

Csmali2javaView::Csmali2javaView()
{
	// TODO:  �ڴ˴���ӹ������

}

Csmali2javaView::~Csmali2javaView()
{
}

BOOL Csmali2javaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// Csmali2javaView ����

void Csmali2javaView::OnDraw(CDC* /*pDC*/)
{
	Csmali2javaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// Csmali2javaView ��ӡ


void Csmali2javaView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Csmali2javaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void Csmali2javaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void Csmali2javaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}

void Csmali2javaView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Csmali2javaView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Csmali2javaView ���

#ifdef _DEBUG
void Csmali2javaView::AssertValid() const
{
	CView::AssertValid();
}

void Csmali2javaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Csmali2javaDoc* Csmali2javaView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Csmali2javaDoc)));
	return (Csmali2javaDoc*)m_pDocument;
}
#endif //_DEBUG


// Csmali2javaView ��Ϣ�������
