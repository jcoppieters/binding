// DlgDuoSwitchProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDuoSwitchProp.h"

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
// CDlgDuoSwitchProp dialog


CDlgDuoSwitchProp::CDlgDuoSwitchProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgDuoSwitchProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
}


void CDlgDuoSwitchProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDuoSwitchProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDuoSwitchProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgDuoSwitchProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDuoSwitchProp message handlers
/////////////////////////////////////////////////////////////////////////////

void CDlgDuoSwitchProp::ShowFullDialog(BOOL fShow)
{
	//const CPoint topLeft(getRectWindow().TopLeft());
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


void CDlgDuoSwitchProp::CleanUp(void)
{
	if (pDlgChild != NULL)
	{
		pDlgChild->DestroyWindow();
		pDlgChild = NULL;
	}
}

void CDlgDuoSwitchProp::InitControls(void) 
{
	// this->CDlgUnitProp::InitControls( ); // Sedert versie 0x0900 zonder params

	RequestRuntimeProperties();
	UpdateRuntimeProperties();
	if (fChildCreated == FALSE) {
		CalculateDialogCoordinates();
		
		pDlgChild = new CDlgDuoSwitchChildProp();				
		pDlgChild->Create(NULL,NULL,0,getRectChildFrame(),this,IDD_DIALOG_PROP_UNIT_CHILD);

		fChildCreated = TRUE;
	}
/*
	if (pDlgChild != NULL)
		((CDlgUnitChildProp*)pDlgChild)->UpdateControls(pDocUnit->bNodeLogicalAddress,
													    pDocUnit->bUnitAddress);
*/
	if (pDlgChild!=NULL) pDlgChild->InitControls(getAssignedUnit()->m_pBindingUnit);

	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////

void CDlgDuoSwitchProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
	//ASSERT(m_pAppGuiUnit);

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXDuoSwitchUnitMessage* const pTXUnitMsg = static_cast<CTXDuoSwitchUnitMessage*>
			(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);
	
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestStoptime(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestSwitchtime(bNodeAddress,bUnitAddress);
}



/*
void CDlgDuoSwitchProp::RequestRuntimeProperties()
{
	ASSERT(pDocUnit);
	BYTE bNodeAddress = pDocUnit->bNodeLogicalAddress;
	BYTE bUnitAddress = pDocUnit->bUnitAddress;

	CTXDuoSwitchUnitMessage* pTXUnitMsg = ((CTXDuoSwitchUnitMessage*) 
					((pMainWnd->GetAppGuiUnit())->GetMessagePtr()));

	ASSERT(pTXUnitMsg);
	
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestStoptime(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestSwitchtime(bNodeAddress,bUnitAddress);
}

*/

