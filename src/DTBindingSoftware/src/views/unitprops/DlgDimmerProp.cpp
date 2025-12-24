// DlgDimmerProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDimmerProp.h"

#include "MyWindowMessages.h"

#include "bindingunitoperations.h"

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
// CDlgDimmerProp dialog


CDlgDimmerProp::CDlgDimmerProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent), m_pDlgChild(NULL)
{
	//{{AFX_DATA_INIT(CDlgDimmerProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
	//pDlgChild = NULL;
}


void CDlgDimmerProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDimmerProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDimmerProp, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgDimmerProp)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDimmerProp message handlers
void CDlgDimmerProp::CleanUp(void)
{
	if (this->m_pDlgChild != NULL) {
		this->m_pDlgChild->DestroyWindow();
		this->m_pDlgChild = NULL;
	}
}

void CDlgDimmerProp::PostNcDestroy() 
{	
	CDlgUnitProp::PostNcDestroy();
}


// Sedert V16.49:
// Wanneer de dialoog getoond wordt voor een DALI (bridge) unit dan kunnen er geen properties ingesteld worden.
// Er wordt geen expand + properties dialoog getoond.
void CDlgDimmerProp::InitControls(void) 
{		
	this->RequestRuntimeProperties();
	this->InitStaticControls();
	this->bPreviousDimValue = 0xFF;
	this->UpdateRuntimeProperties();
	this->fShowBindingProperties = FALSE;

	BOOL enableBtnExpand = TRUE;	// Bepalen of de dialoog met properties kan getoond worden.
	if ((NULL != getAssignedUnit()->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == getAssignedUnit()->m_pAppUnitData->getUnitDataType())) {				
		enableBtnExpand = FALSE;
	}

	if (this->fChildCreated == FALSE)
	{
		CalculateDialogCoordinates();

		if (1)
		{
			const BYTE bBindingUnitType = CBindingUnitOperations::ConvertType(getAssignedNode(),getAssignedUnit());
			ASSERT((bBindingUnitType==BINDING_UNIT_TYPE_DIM_FWD)|| (bBindingUnitType==BINDING_UNIT_TYPE_DIM));
			ASSERT(getAssignedNode()->getNodeAddress()== getAssignedUnit()->bNodeLogicalAddress);
		}

		// Oppassen:
		// Moeten de child dialoog altijd aanmaken omdat de CalculateDialogCoordinates() anders niet ok is.

		this->m_pDlgChild = new CDlgDimmerChildProp();
		if (0!=this->m_pDlgChild) {
			this->m_pDlgChild->Create(NULL,
							  NULL,
							  0,
							  getRectChildFrame(),
							  this,
							  IDD_DIALOG_PROP_UNIT_CHILD);
			this->fChildCreated = TRUE;
		}
	}

	if (this->m_pDlgChild != NULL) { 
		this->m_pDlgChild->InitControls(getAssignedUnit()->m_pBindingUnit);
	}
	
	this->ShowFullDialog(fShowBindingProperties);
	this->m_BtnExpand.ShowWindow(enableBtnExpand);	// Sedert V16.49
	UpdateData(FALSE);
}


void CDlgDimmerProp::InitStaticControls()
{
	if (fControlsCreated == FALSE)
	{
		// Create Trackbar
		CPoint TopLeft(5,75);
		CSize szControlSize(150,30);

		m_trackbar.Create(WS_CHILD | WS_VISIBLE |
						  TBS_AUTOTICKS | TBS_TOP | WS_TABSTOP,
						  CRect(TopLeft,szControlSize),
						  this,
						  IDC_TRACKBAR_DIMVALUE);

		m_trackbar.SetRange(0,99,TRUE);
		m_trackbar.SetTicFreq(10);
		m_trackbar.SetLineSize(1);
		m_trackbar.SetPageSize(10);

		// Create Static-Value
		CPoint TopLeft2(160,80);
		CSize szControlSize2(25,25);

		m_StaticValue.Create("",WS_CHILD | WS_VISIBLE | SS_LEFT,
							 CRect(TopLeft2,szControlSize2),
							 this);

		m_StaticValue.SetFont(pDefaultFont);

		fControlsCreated = TRUE;
	}
}

void CDlgDimmerProp::UpdateRuntimeProperties()
{
	CDlgUnitProp::UpdateRuntimeProperties();

	if (fStatusReceived == TRUE)
	{
		const BYTE bValue = pRuntimeUnit->UnitSpecs.DimmerUnit.bDimCurrentVal;
	
		if ((bValue >=0) && (bValue < 100))
		{
			if (bValue != bPreviousDimValue)
			{
				CString szValue;	
				szValue.Format("%02d",bValue);
				m_StaticValue.SetWindowText(szValue);	

				m_trackbar.SetPos(bValue);
			}
		}
		
		bPreviousDimValue = bValue;
	}
	else
	{
		m_StaticValue.SetWindowText("N/A");	
		m_trackbar.SetPos(0);
	}
}

void CDlgDimmerProp::RequestRuntimeProperties()
{
	ASSERT(getAssignedUnit());

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;
	CTXDimmerUnitMessage* const pTXUnitMsg = static_cast<CTXDimmerUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);
	// Get Runtime properties of Unit
	// pTXUnitMsg->RequestOnOff(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestRange(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestSpeed(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestValue(bNodeAddress,bUnitAddress);
}

void CDlgDimmerProp::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	BOOL fUpdate;
	
	//ASSERT(m_pAppGuiUnit);
	ASSERT(pRuntimeUnit);

	const BYTE bValue = pRuntimeUnit->UnitSpecs.DimmerUnit.bDimCurrentVal;

	switch(nSBCode)
	{
		case SB_RIGHT:
		case SB_LEFT:		
			fUpdate = FALSE;
		break;
				
		case SB_ENDSCROLL:
			fUpdate = FALSE;
		break;

		case SB_LINERIGHT:
		case SB_LINELEFT:		
			fUpdate = FALSE;
		break;

		case SB_PAGELEFT:
		case SB_PAGERIGHT:		
			fUpdate = TRUE;
		break;

		case SB_THUMBPOSITION:			
			fUpdate = TRUE;
		break;

		case SB_THUMBTRACK:					
			fUpdate = FALSE;		
		break;

		default:
			fUpdate = FALSE;
		break;
	}

	if (fUpdate == TRUE)
	{
		KillTimer(TIMERID_DIALOG_REQUEST_UPDATE);
		KillTimer(TIMERID_DIALOG_SEND_VALUE);

		SetTimer(TIMERID_DIALOG_SEND_VALUE,
				 250,
				 (TIMERPROC) NULL);

		SetTimer( TIMERID_DIALOG_REQUEST_UPDATE,
				  1000,
				  (TIMERPROC) NULL);				
	}
	
	CDlgUnitProp::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CDlgDimmerProp::ShowFullDialog(BOOL fShow) {
	//const CPoint topLeft(getRectWindow().TopLeft());
	const CPoint topLeft(this->getPointTopLeft());
	if (fShow == TRUE) {	
		m_BtnExpand.SetWindowText("<<<");		
		MoveWindow(topLeft.x,topLeft.y,getRectFullFrame().Width(),getRectFullFrame().Height());		
		if (this->m_pDlgChild != NULL) {
			this->m_pDlgChild->ShowWindow(TRUE);	
		}
	}
	else {
		m_BtnExpand.SetWindowText(">>>");
		MoveWindow(topLeft.x,topLeft.y,getRectMainFrame().Width(),getRectMainFrame().Height());	
		if (this->m_pDlgChild != NULL) {
			this->m_pDlgChild->ShowWindow(FALSE);
		}
	}
}

void CDlgDimmerProp::OnTimer(UINT nIDEvent) 
{
	KillTimer(nIDEvent);

	CTXDimmerUnitMessage* const pTXUnitMsg = static_cast<CTXDimmerUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());

	if ((pTXUnitMsg != NULL) && (getAssignedUnit() != NULL))
	{
		const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
		const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

		if (nIDEvent == TIMERID_DIALOG_SEND_VALUE)
		{
			if (m_trackbar.GetSafeHwnd() != NULL)
			{
				const int position = m_trackbar.GetPos();

				pTXUnitMsg->SetAttrValue(bNodeAddress,
										 bUnitAddress,
										 position);			

				CString s;
				s.Format("%02d",position);
				m_StaticValue.SetWindowText(s);
			}
	
		}
		else if (nIDEvent == TIMERID_DIALOG_REQUEST_UPDATE)
		{							
			bPreviousDimValue = 0xFF;
			pTXUnitMsg->RequestValue(bNodeAddress,bUnitAddress);
		}
	}
	
	CDlgUnitProp::OnTimer(nIDEvent);
}
