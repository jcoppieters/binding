// MySplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MySplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySplitterWnd


/****************************************************************************

Overwritten CSplitterWnd class :

  To Prevent Static Splitter Bars from tracking !!!!


*****************************************************************************/



CMySplitterWnd::CMySplitterWnd()
{
}

CMySplitterWnd::~CMySplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CMySplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CMySplitterWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMySplitterWnd message handlers

void CMySplitterWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CMySplitterWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CMySplitterWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CMySplitterWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnMouseMove(nFlags, point);
}
