// NodeTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNodeTreeView

IMPLEMENT_DYNCREATE(CNodeTreeView, CTreeView)

CNodeTreeView::CNodeTreeView()
{
}

CNodeTreeView::~CNodeTreeView()
{
}


BEGIN_MESSAGE_MAP(CNodeTreeView, CTreeView)
	//{{AFX_MSG_MAP(CNodeTreeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodeTreeView drawing

void CNodeTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CNodeTreeView diagnostics

#ifdef _DEBUG
void CNodeTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNodeTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNodeTreeView message handlers
