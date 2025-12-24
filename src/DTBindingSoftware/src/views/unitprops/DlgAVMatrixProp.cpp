// DlgAVMatrixProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgAVMatrixProp.h"

#include "DlgAVMatrixChildProp.h"
#include "FactoryDlgUnitChildProp.h"

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
// CDlgAVMatrixProp dialog

CDlgAVMatrixProp::CDlgAVMatrixProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent), m_fControlsCreated(FALSE), m_pDlgChild(NULL)
{
	//{{AFX_DATA_INIT(CDlgAVMatrixProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgAVMatrixProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAVMatrixProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAVMatrixProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgAVMatrixProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAVMatrixProp message handlers

void CDlgAVMatrixProp::ShowFullDialog(BOOL fShow)
{
	if (NULL==m_pDlgChild) {
		CDlgUnitProp::ShowFullDialog(fShow);
	}
	else {
		const int width = (fShow==TRUE ? getRectFullFrame().Width() : getRectMainFrame().Width());
		const int height = (fShow==TRUE ? getRectFullFrame().Height() : getRectMainFrame().Height());

		//const CPoint topLeft(getRectWindow().TopLeft());
		const CPoint topLeft(this->getPointTopLeft());
		MoveWindow(topLeft.x,topLeft.y,width,height);				
		m_BtnExpand.SetWindowText(fShow==TRUE ? "<<<" : ">>>");	
		m_pDlgChild->ShowWindow(fShow);	
	}
}

void CDlgAVMatrixProp::CleanUp(void)
{
	if (m_pDlgChild != NULL) 
	{
		m_pDlgChild->DestroyWindow();
		m_pDlgChild = NULL;
	}
}

// Sedert versie 0x0900 zonder params
void CDlgAVMatrixProp::InitControls(void) 
{
	// CDlgUnitProp::InitControls(); 

	RequestRuntimeProperties();
	UpdateRuntimeProperties();
	CBindingPropertyUnit* const pBindingUnit = getAssignedUnit()->m_pBindingUnit;
	// De coordinaten mogen maar 1x berekend worden !!!
	if (getRectChildFrame() == CRect(0,0,0,0)) {
		CalculateDialogCoordinates();
	}

	if (fChildCreated == FALSE) {				
		m_pDlgChild = CFactoryDlgUnitChildProp::createDlgAVMatrix(pBindingUnit);					
		if (m_pDlgChild) {
			if (TRUE==m_pDlgChild->Create(NULL,NULL,0,getRectChildFrame(),this,IDD_DIALOG_PROP_UNIT_CHILD)) {
				fChildCreated = TRUE;
			}
		}		
	}

	if (m_pDlgChild != NULL) {		
		CBindingPropertyAVMatrixUnit* p = dynamic_cast<CBindingPropertyAVMatrixUnit*>(pBindingUnit);
		if (NULL!=p) {
			const CBindingPropertyAVMatrixUnit::AVMatrixType_t type = p->getMatrixType();
			if (CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_AUDAC == type ||
				CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_INTEGRA == type) {
				m_BtnExpand.ShowWindow(TRUE);
			}
			else {
				fShowBindingProperties = FALSE;
				m_BtnExpand.SetWindowText("");
				m_BtnExpand.ShowWindow(FALSE);
			} 
		}
	}

	if (m_pDlgChild != NULL) {	
		m_pDlgChild->InitControls(pBindingUnit);
	}

	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAVMatrixProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());
//	ASSERT(m_pAppGuiUnit);

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXAVMatrixUnitMessage* const pTXUnitMsg = static_cast<CTXAVMatrixUnitMessage*>
		(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);

	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);	
}




