// DlgAudioHC4Prop.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgAudioHC4Prop.h"

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
// CDlgAudioHC4Prop dialog


CDlgAudioHC4Prop::CDlgAudioHC4Prop(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgAudioHC4Prop)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
}


void CDlgAudioHC4Prop::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAudioHC4Prop)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAudioHC4Prop, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgAudioHC4Prop)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAudioHC4Prop message handlers
void CDlgAudioHC4Prop::ShowFullDialog(BOOL fShow)
{
	CDlgUnitProp::ShowFullDialog(fShow);
}


void CDlgAudioHC4Prop::CleanUp(void)
{

}

void CDlgAudioHC4Prop::InitControls(void) // Sedert versie 0x0900 zonder params
{
	// CDlgUnitProp::InitControls(); 

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

void CDlgAudioHC4Prop::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
	//ASSERT(m_pAppGuiUnit);

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXAudioHC4UnitMessage* const pTXUnitMsg = static_cast<CTXAudioHC4UnitMessage*>
		(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);
}


/*
void CDlgAudioHC4Prop::RequestRuntimeProperties()
{
	ASSERT(pDocUnit);
	BYTE bNodeAddress = pDocUnit->bNodeLogicalAddress;
	BYTE bUnitAddress = pDocUnit->bUnitAddress;

	CTXAudioHC4UnitMessage* pTXUnitMsg = ((CTXAudioHC4UnitMessage*) 
					((pMainWnd->GetAppGuiUnit())->GetMessagePtr()));

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);
}
*/
