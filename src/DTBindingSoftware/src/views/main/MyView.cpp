// MyView.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyView.h"
#include "MyFormView.h"
#include "NodesFormView.h"
#include "MyFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyView

IMPLEMENT_DYNCREATE(CMyView, CView)

CMyView::CMyView() : m_wndSplitterCreated(FALSE)
{
}

CMyView::~CMyView()
{
}


BEGIN_MESSAGE_MAP(CMyView, CView)
	//{{AFX_MSG_MAP(CMyView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyView drawing

void CMyView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMyView diagnostics

#ifdef _DEBUG
void CMyView::AssertValid() const
{
	CView::AssertValid();
}

void CMyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyView message handlers

BOOL CMyView::CreateViews(CCreateContext *pContext)
{	
	// Max Row = 1; Max Col = 2 
	if (!m_wndSplitter.CreateStatic(this,1,2,WS_CHILD | WS_VISIBLE )) return FALSE;

	// row 0, Col 0
	m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CMyFormView),CSize(100,0),pContext);
	// row 0, col 1 
	m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CMyFrameWnd),CSize(100,0),pContext);

	
	pMyFrameWnd = ((CMyFrameWnd*) m_wndSplitter.GetPane(0,1));
	pMyTreeView = ((CMyFormView*) m_wndSplitter.GetPane(0,0));


	// Sedert V16.46
	enum {
		COLUMN_INFO_TREEVIEW_SIZE_IDEAL		= 220,	// 205,
		COLUMN_INFO_TREEVIEW_SIZE_MIN		= 200,	// 10
	};

	m_wndSplitter.SetColumnInfo(0, COLUMN_INFO_TREEVIEW_SIZE_IDEAL , COLUMN_INFO_TREEVIEW_SIZE_MIN);
	m_wndSplitter.RecalcLayout();
	m_wndSplitterCreated = TRUE;

	return TRUE;
}

void CMyView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (m_wndSplitterCreated == TRUE)
		m_wndSplitter.MoveWindow(0,0,cx,cy);
	
}
