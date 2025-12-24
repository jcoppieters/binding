// DlgControlProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgControlProp.h"

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
// CDlgControlProp dialog


CDlgControlProp::CDlgControlProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgControlProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	fControlsCreated = FALSE;
}


void CDlgControlProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgControlProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgControlProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgControlProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgControlProp message handlers

void CDlgControlProp::ShowFullDialog(BOOL fShow)
{
	CDlgUnitProp::ShowFullDialog(fShow);
}


void CDlgControlProp::CleanUp(void)
{

}

void CDlgControlProp::InitControls(void) 
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
void CDlgControlProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
	//ASSERT(m_pAppGuiUnit);

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXControlUnitMessage* const pTXUnitMsg = static_cast<CTXControlUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());
	ASSERT(pTXUnitMsg);
	pTXUnitMsg->RequestControlStatus(bNodeAddress,bUnitAddress);
}



/*
void CDlgControlProp::RequestRuntimeProperties()
{
	ASSERT(pDocUnit);
	BYTE bNodeAddress = pDocUnit->bNodeLogicalAddress;
	BYTE bUnitAddress = pDocUnit->bUnitAddress;

	CTXControlUnitMessage* pTXUnitMsg = ((CTXControlUnitMessage*) 
					((pMainWnd->GetAppGuiUnit())->GetMessagePtr()));

	ASSERT(pTXUnitMsg);
	
	pTXUnitMsg->RequestControlStatus(bNodeAddress,bUnitAddress);
}
*/

