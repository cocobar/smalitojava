// smali2javaDoc.cpp : Csmali2javaDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "smali2java.h"
#endif

#include "CsmaliDoc.h"
#include "JavaClass.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Csmali2javaDoc

IMPLEMENT_DYNCREATE(CsmaliDoc, CDocument)

BEGIN_MESSAGE_MAP(CsmaliDoc, CDocument)
END_MESSAGE_MAP()


// Csmali2javaDoc ����/����

CsmaliDoc::CsmaliDoc()
{
	// TODO:  �ڴ����һ���Թ������

}

CsmaliDoc::~CsmaliDoc()
{
}

BOOL CsmaliDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// Csmali2javaDoc ���л�

void CsmaliDoc::Serialize(CArchive& ar)
{

	//AfxMessageBox(_T("CsmaliDoc"));

	if (ar.IsStoring())
	{
		// TODO:  �ڴ���Ӵ洢����
	}
	else
	{
		// TODO:  �ڴ���Ӽ��ش���
		listString.clear();
		
		CString strLine;
		while (ar.ReadString(strLine)) {
			listString.push_back(strLine);
		}

		CJavaClass cJavaClass;

		// ��������
		if (cJavaClass.AnalyzeClassSmaliListString(listString)) {

		}

		this->UpdateAllViews(NULL);
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CsmaliDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CsmaliDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CsmaliDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// Csmali2javaDoc ���

#ifdef _DEBUG
void CsmaliDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CsmaliDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Csmali2javaDoc ����
