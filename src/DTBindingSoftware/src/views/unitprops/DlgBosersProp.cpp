// DlgBosersProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBosersProp.h"

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
// CDlgBosersProp dialog






/************************************************************************

  To show CDlgBoseChildProp :
	First make special binding unit ...


  Dit is een tijdelijke implementatie:
	- CBindingPropertyBoseRsUnit wordt niet ingeladen via serializable I/O.

	Moeten een tijdelijk CBindingPropertyBoseRsUnit type aanmaken om
	child property dialoog te initialiseren.

*************************************************************************/



CDlgBosersProp::CDlgBosersProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBosersProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
	pDlgChild = NULL;
}


void CDlgBosersProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBosersProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBosersProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgBosersProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBosersProp message handlers
void CDlgBosersProp::ShowFullDialog(BOOL fShow)
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


void CDlgBosersProp::CleanUp(void)
{
	if (pDlgChild != NULL)
	{
		pDlgChild->DestroyWindow();
		pDlgChild = NULL;
	}
}

void CDlgBosersProp::InitControls(void) // Sedert versie 0x0900 zonder params
{
	// CDlgUnitProp::InitControls();

	RequestRuntimeProperties();
	UpdateRuntimeProperties();
	if (fChildCreated == FALSE)
	{
		CalculateDialogCoordinates();

		pDlgChild = new CDlgBoseChildProp();				
		if (pDlgChild)
		{
			pDlgChild->Create(NULL,NULL,0,getRectChildFrame(),this,IDD_DIALOG_PROP_UNIT_CHILD);
			fChildCreated = TRUE;
		}
	}

	if (pDlgChild) 
	{
		// Moeten een tijdelijke CBindingPropertyBoseRsUnit aanmaken.
		// Normaal wordt deze property unit ingeladen via serializabke IO
		// en dan aan een pDocUnit gekoppeld.
		// Hebben deze tijdelijke nodig om een child dialoog te kunnen tonen.

		CBindingPropertyBoseRsUnit tempBindingUnit(
			BINDING_UNIT_TYPE_BOSERS,
			getAssignedUnit()->bNodeLogicalAddress,
			getAssignedUnit()->bUnitAddress);

		tempBindingUnit.SetConfig(getAssignedUnit()->UnitSpecs.AudioUnit.bAudioConfig);
		
		pDlgChild->InitControls(&tempBindingUnit);
	}

	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
void CDlgBosersProp::RequestRuntimeProperties()
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

	pTXUnitMsg->RequestVolume(bNodeAddress,bUnitAddress);
}



