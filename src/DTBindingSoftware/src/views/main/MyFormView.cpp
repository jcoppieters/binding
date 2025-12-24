// MyFormView.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyFormView.h"

#include "MainFrm.h"
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


///////////////////////////////////////////////////////////////////////////
#define IDC_TREEVIEW_NODES		70000
#define IDC_TREEVIEW_BINDINGS	70001
#define IDC_TREEVIEW_TEMPLATES	70002
#define IDC_TABCTRL				80001
#define TABCTRL_MIN_YSIZE		30

///////////////////////////////////////////////////////////////////////////

// CMyFormView

IMPLEMENT_DYNCREATE(CMyFormView, CFormView)

CMyFormView::CMyFormView()
	: CFormView(CMyFormView::IDD)
{
	//{{AFX_DATA_INIT(CMyFormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fTreeCreated = FALSE;
	fTabCreated = NULL;
	CurrFormState = STATE_VIEW_DISABLED;
}

CMyFormView::~CMyFormView()
{	
}

void CMyFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyFormView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyFormView, CFormView)
	//{{AFX_MSG_MAP(CMyFormView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
	ON_MESSAGE(WM_APPLICATION_UPDATEVIEWS,OnApplicationUpdateViews)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyFormView diagnostics

#ifdef _DEBUG
void CMyFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMyFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////
LONG CMyFormView::OnApplicationClearViews(WPARAM wParam,LPARAM lParam)
{
	m_treeNodes.SendMessage(WM_APPLICATION_CLEARVIEWS);
	m_treeBindings.SendMessage(WM_APPLICATION_CLEARVIEWS);
	m_treeTemplates.SendMessage(WM_APPLICATION_CLEARVIEWS);

	DisplayFormState( STATE_VIEW_DISABLED );

	return TRUE;
}

LONG CMyFormView::OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam)
{
	BOOL fResult = m_treeNodes.SendMessage(WM_APPLICATION_UPDATEVIEWS);

	m_treeBindings.SendMessage(WM_APPLICATION_UPDATEVIEWS);
	m_treeTemplates.SendMessage(WM_APPLICATION_UPDATEVIEWS);

	if (fResult == TRUE) 
	{
		DisplayFormState( STATE_VIEW_NODES );
	}
	else 
	{
		DisplayFormState( STATE_VIEW_DISABLED );
	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMyFormView message handlers

void CMyFormView::OnInitialUpdate() 
{	
	CFormView::OnInitialUpdate();
	
	if (fTreeCreated == NULL)
	{	
		CRect rect;
		GetClientRect( &rect );
		rect.bottom -= TABCTRL_MIN_YSIZE;
	

		m_treeNodes.Create(WS_BORDER | TVS_HASLINES | TVS_LINESATROOT |  
						   TVS_HASBUTTONS |  TVS_EDITLABELS,
						   rect,
						   this,
						   IDC_TREEVIEW_NODES);		

		m_treeBindings.Create(WS_BORDER | TVS_HASLINES | TVS_LINESATROOT |  
							  TVS_HASBUTTONS |  TVS_EDITLABELS,
							  rect,
							  this,
							  IDC_TREEVIEW_BINDINGS);

		m_treeTemplates.Create(WS_BORDER | TVS_HASLINES | TVS_LINESATROOT |  
							   TVS_HASBUTTONS |  TVS_EDITLABELS,
							   rect,
							   this,
							   IDC_TREEVIEW_TEMPLATES);
		
		fTreeCreated = TRUE;
	
	

		GetClientRect( &rect );
		rect.top = rect.bottom - TABCTRL_MIN_YSIZE;

		fTabCreated = m_TabCtrl.Create(TCS_FORCEICONLEFT | TCS_FLATBUTTONS |				
									   TCS_RIGHTJUSTIFY | WS_CHILD | 
									   WS_VISIBLE | TCS_BOTTOM,
									   rect,
									   this,
									   IDC_TABCTRL);	
	
		if (fTabCreated == TRUE) 
		{										

			m_TabCtrl.SetFont(TMGetGraphics()->GetFont(FONT_TIMES_NEW_ROMAN_12));

			CImageList* const pImageList = TMGetGraphics()->GetTabControlImageList();

			if (pImageList != NULL)
			{
				m_TabCtrl.SetImageList(pImageList);			
			}									

			TCITEM tcItem;
			tcItem.mask = TCIF_IMAGE | TCIF_TEXT;	
			tcItem.iImage = 0;
			tcItem.pszText = "Nodes";
			m_TabCtrl.InsertItem(0,&tcItem);	
			tcItem.iImage = 1;
			tcItem.pszText = "Bindings";
			m_TabCtrl.InsertItem(1,&tcItem);	
#if(0)
			tcItem.iImage = 2;
			tcItem.pszText = "Templates";
			m_TabCtrl.InsertItem(2,&tcItem);
#endif 
		}

		
		DisplayFormState( STATE_VIEW_DISABLED );

		
		if (m_treeNodes.GetSafeHwnd() != NULL)
		{				
			m_treeNodes.SetFont(TMGetGraphics()->GetFont(FONT_TREEVIEW_FIXED));
		}

		if (m_treeBindings.GetSafeHwnd() != NULL)
		{				
			m_treeBindings.SetFont(TMGetGraphics()->GetFont(FONT_TREEVIEW_FIXED));
		}

		if (m_treeTemplates.GetSafeHwnd() != NULL)
		{				
			m_treeTemplates.SetFont(TMGetGraphics()->GetFont(FONT_TREEVIEW_FIXED));
		}				
	}
}

void CMyFormView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	CRect rect,myrect;
	GetClientRect(&rect);

	if (fTreeCreated == TRUE) 
	{
		rect.bottom -= TABCTRL_MIN_YSIZE;
		m_treeNodes.MoveWindow(rect);
		m_treeBindings.MoveWindow(rect);
		m_treeTemplates.MoveWindow(rect);
	}
	
	this->ShowScrollBar(SB_BOTH,FALSE);
	GetClientRect(&rect);
	
	if (fTabCreated == TRUE) 
	{
		rect.top = rect.bottom - TABCTRL_MIN_YSIZE;		
		m_TabCtrl.MoveWindow(rect, TRUE);   
		m_TabCtrl.SetMinTabWidth(50);
	}
}


void CMyFormView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

}


BOOL CMyFormView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	const TV_DISPINFO *tv_dispinfo = (TV_DISPINFO*) lParam;	
	
	if (tv_dispinfo->hdr.code == TCN_SELCHANGE) 
	{				
		DisplayFormState( (FormState) m_TabCtrl.GetCurSel() );
		return TRUE;
	}
	return CFormView::OnNotify(wParam, lParam, pResult);
}

void CMyFormView::DisplayFormState(FormState State)
{
	CurrFormState = State;

	switch (CurrFormState)
	{
		case STATE_VIEW_NODES:													
			m_treeBindings.ShowWindow(FALSE);
			m_treeTemplates.ShowWindow(FALSE);		
			m_treeNodes.ShowWindow(TRUE);
			m_TabCtrl.ShowWindow(TRUE);
			m_TabCtrl.SetCurSel(0);
		break;

		case STATE_VIEW_BINDINGS:									
			m_treeNodes.ShowWindow(FALSE);		
			m_treeBindings.ShowWindow(TRUE);
			m_treeTemplates.ShowWindow(FALSE);
			m_TabCtrl.ShowWindow(TRUE);
			m_TabCtrl.SetCurSel(1);
		break;

		case STATE_VIEW_TEMPLATES:						
			m_treeBindings.ShowWindow(FALSE);
			m_treeNodes.ShowWindow(FALSE);
			m_treeTemplates.ShowWindow(TRUE);		
			m_TabCtrl.ShowWindow(TRUE);
			m_TabCtrl.SetCurSel(2);
		break;
	
		case STATE_VIEW_DISABLED :		
			m_treeTemplates.ShowWindow(FALSE);
			m_treeBindings.ShowWindow(FALSE);
			m_treeNodes.ShowWindow(FALSE);
			m_TabCtrl.ShowWindow(FALSE);
		break;
	}
}

void CMyFormView::OnDestroy() 
{
	CFormView::OnDestroy();
	
	if (fTreeCreated != NULL) 
	{
		m_treeNodes.DeleteAllItems();
		m_treeBindings.DeleteAllItems();	
		m_treeTemplates.DeleteAllItems();
	}
}

	

