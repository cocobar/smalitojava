// smaliView.cpp : CsmaliView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "smali2java.h"
#endif

#include "MainFrm.h"
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
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
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
	font.CreateFont(
		18,                        // nHeight
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
		_T("Consolas"));          // lpszFacename
								   // Do something with the font just created...
	CFont* def_font = pDC->SelectObject(&font);

#if 0
	CPoint scrollPos = GetScrollPosition();
	for (unsigned int i = 0; i < pDoc->listString.size(); i++) {

		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->TextOut(0 - scrollPos.x, i * 16 - scrollPos.y, pDoc->listString[i], pDoc->listString[i].GetLength());
	}
#else 
	//for (unsigned int i = 0; i < pDoc->listString.size(); i++) {
	//	pDC->SetTextColor(RGB(0, 255, 0));
	//	pDC->TextOut(0, i * 16, pDoc->listString[i], pDoc->listString[i].GetLength());
	//}

	CSize csizetotal;

	csizetotal.cy = 0;

	//pDoc->listOutString = pDoc->listString;

	if (((CMainFrame *)AfxGetApp()->m_pMainWnd)->bShowJavaCode) {
		for (unsigned int i = 0; i < pDoc->listOutString.size(); i++) {
			pDC->SetTextColor(RGB(0, 255, 0));
			pDC->TextOut(0, i * 16, pDoc->listOutString[i]);
			CSize textSize = pDC->GetTextExtent(pDoc->listOutString[i]);

			csizetotal.cy = ((i + 1) * 16);

			if (textSize.cx > csizetotal.cx) {
				csizetotal.cx = textSize.cx;
			}
		}
	}
	else {
		for (unsigned int i = 0; i < pDoc->listString.size(); i++) {
			pDC->SetTextColor(RGB(0, 255, 0));
			pDC->TextOut(0, i * 16, pDoc->listString[i]);
			CSize textSize = pDC->GetTextExtent(pDoc->listString[i]);

			csizetotal.cy = ((i + 1) * 16);

			if (textSize.cx > csizetotal.cx) {
				csizetotal.cx = textSize.cx;
			}
		}

	}


	SetScrollSizes(MM_TEXT, csizetotal);


#endif

	pDC->SelectObject(def_font);
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
	//OnContextMenu(this, point);
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

#if 0
	csizetotal.cx = 1000;
	csizetotal.cy = pDoc->listString.size() * 16;
	SetScrollSizes(MM_TEXT, csizetotal);

#else
	csizetotal.cx = 10;
	csizetotal.cy = 10;
	SetScrollSizes(MM_TEXT, csizetotal);
#endif
}

BOOL CsmaliView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// TODO: �ڴ����ר�ô����/����û���
	BOOL rtnCod = CView::OnScroll(nScrollCode, nPos, bDoScroll);

	BYTE vcod = (nScrollCode >> 8)&0xFF;	// ���ﴦ��ֱ�����������
	BYTE hcod = (nScrollCode & 0xFF);		// ���ﴦ��ˮƽ�����������
	if (SB_THUMBTRACK == vcod || SB_THUMBPOSITION == vcod){
		SCROLLINFO si;
		memset(&si, 0, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SIF_TRACKPOS;
		if (GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS))
		{
			//m_nScrollPos = si.nTrackPos;  // ȡ����ȷ�Ļ���λ��
			SetScrollPos(SB_VERT, si.nTrackPos); // �趨
			RedrawWindow();    // �ػ�
		}
	}
	else if (SB_LINEDOWN == vcod) {
		int nPos = GetScrollPos(SB_VERT);
		nPos += 16;
		SetScrollPos(SB_VERT, nPos);
		RedrawWindow();
	}
	else if (SB_LINEUP == vcod) {
		int nPos = GetScrollPos(SB_VERT);
		nPos -= 16;
		SetScrollPos(SB_VERT, nPos);
		RedrawWindow();
	}
	else if (SB_PAGEDOWN == vcod) {
		int nPos = GetScrollPos(SB_VERT);
		nPos += 16 * 20;
		SetScrollPos(SB_VERT, nPos);
		RedrawWindow();
	}
	else if (SB_PAGEUP == vcod) {
		int nPos = GetScrollPos(SB_VERT);
		nPos -= 16 * 20;
		SetScrollPos(SB_VERT, nPos);
		RedrawWindow();
	}

	if (SB_THUMBTRACK == hcod || SB_THUMBPOSITION == hcod) {
		SCROLLINFO si;
		memset(&si, 0, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SIF_TRACKPOS;
		if (GetScrollInfo(SB_HORZ, &si, SIF_TRACKPOS))
		{
			//m_nScrollPos = si.nTrackPos;  // ȡ����ȷ�Ļ���λ��
			SetScrollPos(SB_HORZ, si.nTrackPos);
			RedrawWindow();
		}
	}
	else if (SB_LINEDOWN == hcod) {
		int nPos = GetScrollPos(SB_HORZ);
		nPos += 16;
		SetScrollPos(SB_HORZ, nPos);
		RedrawWindow();
	}
	else if (SB_LINEUP == hcod) {
		int nPos = GetScrollPos(SB_HORZ);
		nPos -= 16;
		SetScrollPos(SB_HORZ, nPos);
		RedrawWindow();
	}
	else if (SB_PAGEDOWN == hcod) {
		int nPos = GetScrollPos(SB_HORZ);
		nPos += 16 * 20;
		SetScrollPos(SB_HORZ, nPos);
		RedrawWindow();
	}
	else if (SB_PAGEUP == hcod) {
		int nPos = GetScrollPos(SB_HORZ);
		nPos -= 16 * 20;
		SetScrollPos(SB_HORZ, nPos);
		RedrawWindow();
	}

	return rtnCod;
}


BOOL CsmaliView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CBrush brush(RGB(0, 0, 0));
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	pDC->SetBkMode(TRANSPARENT);

	CRect rect;
	pDC->GetClipBox(&rect);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;
	//return CScrollView::OnEraseBkgnd(pDC);
}


void CsmaliView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CScrollView::OnRButtonDown(nFlags, point);
}
