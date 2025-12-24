// DlgAlarmProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgAlarmProp.h"

#include "MyWindowMessages.h"
#include "DlgAlarmChildProp.h"
#include "FactoryDlgUnitChildProp.h"


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
// CDlgAlarmProp dialog


CDlgAlarmProp::CDlgAlarmProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgAlarmProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
}


void CDlgAlarmProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAlarmProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAlarmProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgAlarmProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmProp message handlers
void CDlgAlarmProp::ShowFullDialog(BOOL fShow)
{
	// const CPoint topLeft(getRectWindow().TopLeft());
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

void CDlgAlarmProp::CleanUp(void)
{
	if (pDlgChild != NULL) 
	{
		pDlgChild->DestroyWindow();
		pDlgChild = NULL;
	}
}

void CDlgAlarmProp::InitControls(void) // Sedert versie 0x0900 zonder params
{
	//CDlgUnitProp::InitControls(); 

	RequestRuntimeProperties();
	UpdateRuntimeProperties();
	CBindingPropertyUnit* const pBindingUnit = getAssignedUnit()->m_pBindingUnit;
	if (fChildCreated == FALSE) {
		CalculateDialogCoordinates();

		// TM_CHANGES_ALARM_ARITECH:
		// In functie van het binding unit type wordt een andere child property dialoog getoond.
		// Bij de Aritech alarm centrale kan er geen type alarm centrale ingesteld worden.
		// Er moet bij beide centrales nog steeds een remote code ingesteld worden.

		pDlgChild = CFactoryDlgUnitChildProp::createDlgAlarm(pBindingUnit);					
		if (pDlgChild)
		{
			pDlgChild->Create(NULL,NULL,0,getRectChildFrame(),this,IDD_DIALOG_PROP_UNIT_CHILD);
		}

		fChildCreated = TRUE;
	}

	if (pDlgChild != NULL) 
	{
		pDlgChild->InitControls(pBindingUnit);
	}

	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAlarmProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
	//ASSERT(m_pAppGuiUnit);

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXAlarmUnitMessage* const pTXUnitMsg = static_cast<CTXAlarmUnitMessage*>
		(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);
	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->Requeststate(bNodeAddress,bUnitAddress);
}

