// DlgSwitchPropChild.cpp : implementation file
//
// TM20120406: CSpinCtrlButton + Editctrl weergave
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgSwitchPropChild.h"

#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesProperty.h"

#if(USE_TOOLTIP_CONTROLS == 1)
#include "app\apphelper\apphelpermfc\ToolTipCtrl.h"
#endif 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSwitchPropChild dialog


CDlgSwitchPropChild::CDlgSwitchPropChild(CWnd* pParent /*=NULL*/)
	: CDlgUnitChildProp(pParent),
	m_bindingSwitchUnit( BINDING_UNIT_TYPE_SWITCH,0xFF,0xFF)
{
	//{{AFX_DATA_INIT(CDlgSwitchPropChild)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}


void CDlgSwitchPropChild::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitChildProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSwitchPropChild)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSwitchPropChild, CDlgUnitChildProp)
	//{{AFX_MSG_MAP(CDlgSwitchPropChild)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_PRECISIONTIME, OnUpDownPrecisionTime)
	ON_BN_CLICKED(IDC_CHECK_PRECISION_TIMING, OnCheckPrecisionTiming)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgSwitchPropChild::OnNotifyClose(void)
{
	if (m_fChangesMade == TRUE) 
	{
		CString szMessage;
		szMessage.Format(_T("Store Changes of Relais Unit 0x%02X,0x%02X ?"),bNodeAddress,bUnitAddress);

		if (AfxMessageBox(szMessage,MB_YESNO) == IDYES) 
		{								
			// Construct property Bindings ...		
			*ptrOriginalSwitchUnit = m_bindingSwitchUnit;
			CBindingConversionToNodesProperty ConvertBindingToNodes(ptrOriginalSwitchUnit);
			ConvertBindingToNodes.ProcessPropertyBindings();			
		}
	}
}

void CDlgSwitchPropChild::InitControls(CBindingPropertyUnit *ptr)
{
	ASSERT(ptr);
	ptrOriginalSwitchUnit = (CBindingPropertySwitchUnit*)ptr;


	m_fChangesMade = FALSE;
	bNodeAddress = ptr->GetNodeAddress();
	bUnitAddress = ptr->GetUnitAddress();

	InitControlsToDefaults();						// First
	m_bindingSwitchUnit = *ptrOriginalSwitchUnit;		// Second

	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSwitchPropChild message handlers

void CDlgSwitchPropChild::InitStaticControls(void)
{
	CDlgUnitChildProp::InitStaticControls();

	{
		CPoint TopLeft(15,15);
		CSize szControlSize(150,25);

		m_BtnCheckTiming.Create("Precision Timing",
								WS_CHILD | WS_VISIBLE | BS_CHECKBOX ,
								CRect(TopLeft,szControlSize),
								this,
								IDC_CHECK_PRECISION_TIMING);
	
		m_BtnCheckTiming.SetFont(GetFont());
	}
	{
		CPoint TopLeft(15,50);
		CSize szControlSize(15,20);
		m_SpinTime.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ARROWKEYS,
						  CRect(TopLeft,szControlSize),
						  this,
						  IDC_UPDOWN_PRECISIONTIME);
	}
	{
		CPoint TopLeft(35,50);
		CSize szControlSize(150,20);

		m_EditBuddyTime.Create(WS_CHILD |WS_VISIBLE | WS_BORDER | ES_LEFT ,
							  CRect(TopLeft,szControlSize),						
							  this,
							  IDC_BUDDY_EDIT_PRECISIONTIME);		
		
		m_EditBuddyTime.SetFont(GetFont());

#if(USE_TOOLTIP_CONTROLS == 1)		
		// Deze zijn ok.
		CMFCToolTipCtrl::Create( IDC_CHECK_PRECISION_TIMING, this->GetSafeHwnd(), IDS_TOOLTIP_SWITCH_CHECK_PRECISIONTIME );
		CMFCToolTipCtrl::Create( IDC_BUDDY_EDIT_PRECISIONTIME, this->GetSafeHwnd(), IDS_TOOLTIP_SWITCH_EDIT_PRECISIONTIME );
#endif 	
	}
}

void CDlgSwitchPropChild::InitControlProperties(void)
{ }

////////////////////////////////////////////////////////

void CDlgSwitchPropChild::OnCheckPrecisionTiming()
{
	m_bindingSwitchUnit.fTimeEnabled ^= TRUE;
	UpdateControls();
	m_fChangesMade = TRUE;
}

void CDlgSwitchPropChild::OnUpDownPrecisionTime(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	if (pNMUpDown->iDelta > 0) 
	{
		BYTE bValue = m_bindingSwitchUnit.bTimeValue - pNMUpDown->iDelta;
		if ((bValue > PRECISION_TIME_MIN) && (bValue < PRECISION_TIME_MAX))
			m_bindingSwitchUnit.bTimeValue += ((pNMUpDown->iDelta)*(-1));
		else
			m_bindingSwitchUnit.bTimeValue = PRECISION_TIME_MIN;
	}	
	else 
	{
		m_bindingSwitchUnit.bTimeValue += ((pNMUpDown->iDelta)*(-1));		
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSwitchPropChild::UpdateControls(void)
{
	if (m_bindingSwitchUnit.fTimeEnabled) 
	{
		CString szValue;

		if (m_bindingSwitchUnit.bTimeValue == 0) 
			m_bindingSwitchUnit.bTimeValue = PRECISION_TIME_MIN;
		else if (m_bindingSwitchUnit.bTimeValue > PRECISION_TIME_MAX-1) 
			m_bindingSwitchUnit.bTimeValue = PRECISION_TIME_MAX-1;		

		if ((m_bindingSwitchUnit.bTimeValue >=1) && (m_bindingSwitchUnit.bTimeValue <= 59))
			szValue.Format("%02d seconds",m_bindingSwitchUnit.bTimeValue);
		else if (m_bindingSwitchUnit.bTimeValue >=60) 
			szValue.Format("%02d minutes",m_bindingSwitchUnit.bTimeValue - 59);
	
		m_EditBuddyTime.SetWindowText(szValue);	
	}

	m_BtnCheckTiming.SetCheck(m_bindingSwitchUnit.fTimeEnabled);
	m_SpinTime.ShowWindow(m_bindingSwitchUnit.fTimeEnabled);
	m_EditBuddyTime.ShowWindow(m_bindingSwitchUnit.fTimeEnabled);
}

/*
void CDlgSwitchPropChild::AcceptSettings(void)
{
	MessageBox("Switch Prop Dialog Accept\nTodo Construct bindings");

	// TODO Construct Bindings
}
*/

void CDlgSwitchPropChild::RestoreDefaults(void)
{
	InitControlsToDefaults();
	UpdateControls();
	m_fChangesMade = TRUE;
}

void CDlgSwitchPropChild::InitControlsToDefaults(void)
{
	m_bindingSwitchUnit.SetToDefaults();
}
