// smaliView.cpp : CsmaliView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CsmaliView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// Csmali2javaView 构造/析构

CsmaliView::CsmaliView()
{
	// TODO:  在此处添加构造代码
}

CsmaliView::~CsmaliView()
{
}

BOOL CsmaliView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style |= WS_VSCROLL;

	return CView::PreCreateWindow(cs);
}

// Csmali2javaView 绘制

void CsmaliView::OnDraw(CDC* pDC)
{
	CsmaliDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码

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


// Csmali2javaView 打印


void CsmaliView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CsmaliView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CsmaliView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CsmaliView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
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


// Csmali2javaView 诊断

#ifdef _DEBUG
void CsmaliView::AssertValid() const
{
	CView::AssertValid();
}

void CsmaliView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CsmaliDoc* CsmaliView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CsmaliDoc)));
	return (CsmaliDoc*)m_pDocument;
}
#endif //_DEBUG


// Csmali2javaView 消息处理程序


void CsmaliView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

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
	// TODO: 在此添加专用代码和/或调用基类

	return CView::OnScroll(nScrollCode, nPos, bDoScroll);
}
