// DlgAudioBasicProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgAudioBasicProp.h"

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
// CDlgAudioBasicProp dialog


CDlgAudioBasicProp::CDlgAudioBasicProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgAudioBasicProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
}


void CDlgAudioBasicProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAudioBasicProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAudioBasicProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgAudioBasicProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAudioBasicProp message handlers

void CDlgAudioBasicProp::ShowFullDialog(BOOL fShow)
{
	CDlgUnitProp::ShowFullDialog(fShow);
}


void CDlgAudioBasicProp::CleanUp(void)
{

}

void CDlgAudioBasicProp::InitControls(void) // Sedert versie 0x0900 zonder params
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

////////////////////////////////////////////////////////////////////
void CDlgAudioBasicProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
	//ASSERT(m_pAppGuiUnit);

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXAudioBasicUnitMessage* const pTXUnitMsg = static_cast<CTXAudioBasicUnitMessage*>
		(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestOnOff(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestSourceOn(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);	
}




/*
void CDlgAudioBasicProp::RequestRuntimeProperties()
{
	ASSERT(pDocUnit);
	BYTE bNodeAddress = pDocUnit->bNodeLogicalAddress;
	BYTE bUnitAddress = pDocUnit->bUnitAddress;

	CTXAudioBasicUnitMessage* pTXUnitMsg = ((CTXAudioBasicUnitMessage*) 
					((pMainWnd->GetAppGuiUnit())->GetMessagePtr()));

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestOnOff(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestSourceOn(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);	
}
*/

