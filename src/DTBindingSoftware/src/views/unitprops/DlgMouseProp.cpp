// DlgMouseProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgMouseProp.h"

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
// CDlgMouseProp dialog


CDlgMouseProp::CDlgMouseProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgMouseProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	fControlsCreated = FALSE;
}


void CDlgMouseProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMouseProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMouseProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgMouseProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMouseProp message handlers
void CDlgMouseProp::ShowFullDialog(BOOL fShow)
{
	CDlgUnitProp::ShowFullDialog(fShow);
}


void CDlgMouseProp::CleanUp(void)
{

}

void CDlgMouseProp::InitControls(void) 
{
	// CDlgUnitProp::InitControls(); // Sedert versie 0x0900 zonder params

	RequestRuntimeProperties();
	UpdateRuntimeProperties();
	if (fChildCreated == FALSE) {
		CalculateDialogCoordinates();
		fChildCreated = TRUE;
	}
	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgMouseProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
	//ASSERT(m_pAppGuiUnit);

	BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXPS2MouseUnitMessage* pTXUnitMsg = static_cast<CTXPS2MouseUnitMessage*>
		(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->Requeststate(bNodeAddress,bUnitAddress);
}


/*
void CDlgMouseProp::RequestRuntimeProperties()
{
	ASSERT(pDocUnit);
	BYTE bNodeAddress = pDocUnit->bNodeLogicalAddress;
	BYTE bUnitAddress = pDocUnit->bUnitAddress;

	CTXPS2MouseUnitMessage* pTXUnitMsg = ((CTXPS2MouseUnitMessage*)
			((pMainWnd->GetAppGuiUnit())->GetMessagePtr()));

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->Requeststate(bNodeAddress,bUnitAddress);
}
*/

