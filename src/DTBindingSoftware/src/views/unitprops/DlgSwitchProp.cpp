// DlgSwitchProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgSwitchProp.h"

#include "MyWindowMessages.h"
#include "DlgSwitchPropChild.h"

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
// CDlgSwitchProp dialog


CDlgSwitchProp::CDlgSwitchProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgSwitchProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	fControlsCreated = FALSE;

}


void CDlgSwitchProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSwitchProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSwitchProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgSwitchProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSwitchProp message handlers

void CDlgSwitchProp::PostNcDestroy() 
{
	CDlgUnitProp::PostNcDestroy();
}


/////////////////////////////////////////////////////////////////////////////
void CDlgSwitchProp::ShowFullDialog(BOOL fShow)
{
	const CPoint topLeft(this->getPointTopLeft());
	if (fShow == TRUE)
	{	
		m_BtnExpand.SetWindowText("<<<");		
		MoveWindow(topLeft.x,topLeft.y,getRectFullFrame().Width(),getRectFullFrame().Height());		
		if (pDlgChild != NULL) pDlgChild->ShowWindow(TRUE);			
	}
	else
	{
		m_BtnExpand.SetWindowText(">>>");
		MoveWindow(topLeft.x,topLeft.y,getRectMainFrame().Width(),getRectMainFrame().Height());		
		if (pDlgChild != NULL) pDlgChild->ShowWindow(FALSE);
	}	
}

void CDlgSwitchProp::CleanUp(void)
{
	if (pDlgChild != NULL)
	{
		pDlgChild->DestroyWindow();
		pDlgChild = NULL;
	}
}

void CDlgSwitchProp::InitControls(void) // Sedert versie 0x0900 zonder params
{
	// CDlgUnitProp::InitControls( ); 

	RequestRuntimeProperties();
	UpdateRuntimeProperties();

	if (fChildCreated == FALSE) {
		CalculateDialogCoordinates();
		pDlgChild = new CDlgSwitchPropChild();		
		pDlgChild->Create(NULL,
						  NULL,
						  0,
						  getRectChildFrame(),
						  this,
						  IDD_DIALOG_PROP_UNIT_CHILD);
		fChildCreated = TRUE;
	}

	if (pDlgChild != NULL) 
		pDlgChild->InitControls(getAssignedUnit()->m_pBindingUnit);

	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CDlgSwitchProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXSwitchUnitMessage* const pTXUnitMsg = static_cast<CTXSwitchUnitMessage*> 
						(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);	
	pTXUnitMsg->RequestStopTimer(bNodeAddress,bUnitAddress);
}


