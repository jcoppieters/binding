// DlgVideomuxProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgVideomuxProp.h"

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
// CDlgVideomuxProp dialog


CDlgVideomuxProp::CDlgVideomuxProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgVideomuxProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
}


void CDlgVideomuxProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVideomuxProp)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgVideomuxProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgVideomuxProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVideomuxProp message handlers
void CDlgVideomuxProp::ShowFullDialog(BOOL fShow)
{
	CDlgUnitProp::ShowFullDialog(fShow);
}


void CDlgVideomuxProp::CleanUp(void)
{

}

void CDlgVideomuxProp::InitControls(void) // Sedert versie 0x0900 zonder params
{
	// CDlgUnitProp::InitControls( ); 

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

void CDlgVideomuxProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
	//ASSERT(m_pAppGuiUnit);

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXVideomuxUnitMessage* const pTXUnitMsg = static_cast<CTXVideomuxUnitMessage*>
					(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->Requeststate(bNodeAddress,bUnitAddress);
}





/*
void CDlgVideomuxProp::RequestRuntimeProperties()
{
	ASSERT(pDocUnit);
	BYTE bNodeAddress = pDocUnit->bNodeLogicalAddress;
	BYTE bUnitAddress = pDocUnit->bUnitAddress;

	CTXVideomuxUnitMessage* pTXUnitMsg = ((CTXVideomuxUnitMessage*)
			((pMainWnd->GetAppGuiUnit())->GetMessagePtr()));

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->Requeststate(bNodeAddress,bUnitAddress);
}
*/

