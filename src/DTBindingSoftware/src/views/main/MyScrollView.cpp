// MyScrollView.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyScrollView.h"
#include "MyWindowMessages.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyScrollView

IMPLEMENT_DYNCREATE(CMyScrollView, CScrollView)

CMyScrollView::CMyScrollView()
{
	Messages.RemoveAll();
	nNumLines = 0;
	fUpdate = FALSE;

	uCurrentPosition = 0;
}

CMyScrollView::~CMyScrollView()
{
}


BEGIN_MESSAGE_MAP(CMyScrollView, CScrollView)
	//{{AFX_MSG_MAP(CMyScrollView)
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_DISPLAY_MESSAGE,OnAddScrollMessage)
	ON_MESSAGE(WM_CLEAR_MESSAGES,OnClearScrollView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyScrollView drawing

void CMyScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	NewColor = RGB(0,128,250);

	memset(&logFont, 0, sizeof(LOGFONT));          
	logFont.lfHeight = 16;
	
	CFont* const pFont = TMGetGraphics()->GetFont(FONT_TIMES_NEW_ROMAN_16);
	if (pFont != NULL)
	{
		if (pFont->GetLogFont(&logFont) == 0)
		{
			CString szError("Unable to retrieve logfont information");
			AfxMessageBox(szError);
		}	
	}
}

void CMyScrollView::OnDraw(CDC* pDC)
{
	POINT CurrentPos;	
	CDocument* pDoc = GetDocument();
	UINT position = 0;
	CFont* const pOldFont = pDC->SelectObject(TMGetGraphics()->GetFont(FONT_TIMES_NEW_ROMAN_16));
	const COLORREF OldColor = pDC->SetTextColor(NewColor);

	for (int i=0;i<nNumLines;i++)
	{
		pDC->TextOut(10,position,Messages.GetAt(i));
		position += logFont.lfHeight;
	}

	CSize docSize(100,nNumLines*logFont.lfHeight);
	CRect rect;
	CSize pagesize(rect.right,rect.bottom);
	CSize linesize(0,logFont.lfHeight);
	SetScrollSizes(MM_TEXT,docSize,pagesize,linesize);
	
	if (fUpdate == TRUE) 
	{	
		CurrentPos.x = 0;
		CurrentPos.y = nNumLines*logFont.lfHeight;

		ScrollToPosition(CurrentPos);
		fUpdate = FALSE;
	}
		
	pDC->SetTextColor(OldColor);
	pDC->SelectObject(pOldFont);
}

/////////////////////////////////////////////////////////////////////////////
// CMyScrollView diagnostics

#ifdef _DEBUG
void CMyScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMyScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyScrollView message handlers

void CMyScrollView::ClearMessages()
{
	Messages.RemoveAll();
	Messages.FreeExtra();
	nNumLines = 0;
	fUpdate = TRUE;	
	uCurrentPosition = 0;
	Invalidate();
}


/*////////////////////////////////////////////////////////////////////////////////

  TM 20040702 : 

	1) Text is also output to Device context of view in OnAddFlowMessage()
	   this is done so, because by load/save/Exporting files the screen
	   is not frequently enough updated by function OnDraw ...
	2) ScrollToPosition() MFC function fails !!!  (Q219282)
	   Normally we should also update the screen to have the position of the 
	   last text output. This is done via Function : "SCROLLTOPOSITION()"
	   this doesn't work under VC5,VC6 ...
	   Therefore (To Reduce unnecessary overhead) the function is only called 
	   in OnDraw Function ....

////////////////////////////////////////////////////////////////////////////////*/
LONG CMyScrollView::OnAddScrollMessage(WPARAM wParam,LPARAM lParam)
{
	ASSERT (wParam );
		
	if (Messages.GetSize() < 100) 
	{
		try 
		{		
			CString szText;
			szText.Format("%s",wParam);

			Messages.Add(szText);		
			nNumLines++;	

			CDC *pDC = GetDC();
			
			if (pDC != NULL) 
			{
				CFont* const pOldFont = pDC->SelectObject(TMGetGraphics()->
					GetFont(FONT_TIMES_NEW_ROMAN_16));
							
				const COLORREF OldColor = pDC->SetTextColor(NewColor);

				pDC->TextOut(10,uCurrentPosition,szText);
				uCurrentPosition += logFont.lfHeight;

				pDC->SetTextColor(OldColor);
				pDC->SelectObject(pOldFont);

				if (fUpdate == FALSE) 
				{
					Invalidate();
					fUpdate = TRUE;
				}

			
				// 20051107 : Solved Bug !
				// ReleaseDC must be called --> GDI Resource Leak ...
				ReleaseDC(pDC);
				pDC = NULL;
			}
		}
		catch (CMemoryException *e) 
		{		
			e->ReportError();
			e->Delete();

			ClearMessages();
		}
	}
	else 
	{
		ClearMessages();
	}
	
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
LONG CMyScrollView::OnClearScrollView(WPARAM wParam,LPARAM lParam) 
{
	ClearMessages();
	return TRUE;
}

void CMyScrollView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	ClearMessages();
	CScrollView::OnRButtonDblClk(nFlags, point);
}


