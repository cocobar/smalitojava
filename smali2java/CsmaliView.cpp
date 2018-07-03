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

IMPLEMENT_DYNCREATE(CsmaliView, CScrollView)

BEGIN_MESSAGE_MAP(CsmaliView, CScrollView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
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

	cs.style |= WS_VSCROLL;

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

	CFont font;

	VERIFY(font.CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Fixedsys")));          // lpszFacename

								   // Do something with the font just created...
	CClientDC dc(this);
	CFont* def_font = dc.SelectObject(&font);

	CPoint scrollPos = GetScrollPosition();


	for (unsigned int i = 0; i < pDoc->listString.size(); i++) {
		dc.TextOut(0 - scrollPos.x, i * 16 - scrollPos.y, pDoc->listString[i], pDoc->listString[i].GetLength());
	}

	dc.SelectObject(def_font);

	font.DeleteObject();
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


void CsmaliView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���

	CsmaliDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CSize csizetotal;

	csizetotal.cx = 1000;
	csizetotal.cy = pDoc->listString.size() * 16;
	SetScrollSizes(MM_TEXT, csizetotal);
}

BOOL CsmaliView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CView::OnScroll(nScrollCode, nPos, bDoScroll);
}
