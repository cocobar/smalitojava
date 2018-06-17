
// smali2javaView.cpp : Csmali2javaView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Csmali2javaView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// Csmali2javaView 构造/析构

Csmali2javaView::Csmali2javaView()
{
	// TODO:  在此处添加构造代码

}

Csmali2javaView::~Csmali2javaView()
{
}

BOOL Csmali2javaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// Csmali2javaView 绘制

void Csmali2javaView::OnDraw(CDC* /*pDC*/)
{
	Csmali2javaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// Csmali2javaView 打印


void Csmali2javaView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Csmali2javaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void Csmali2javaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void Csmali2javaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
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


// Csmali2javaView 诊断

#ifdef _DEBUG
void Csmali2javaView::AssertValid() const
{
	CView::AssertValid();
}

void Csmali2javaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Csmali2javaDoc* Csmali2javaView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Csmali2javaDoc)));
	return (Csmali2javaDoc*)m_pDocument;
}
#endif //_DEBUG


// Csmali2javaView 消息处理程序
