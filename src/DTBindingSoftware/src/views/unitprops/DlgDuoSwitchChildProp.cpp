// DlgDuoSwitchChildProp.cpp : implementation file
//
// TM20120406: CSpinCtrlButton + EditCtrl weergave aangepast
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDuoSwitchChildProp.h"

#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesProperty.h"
#include "app/appcfg/appcfginfo/cfginfounit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDuoSwitchChildProp dialog


CDlgDuoSwitchChildProp::CDlgDuoSwitchChildProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitChildProp(pParent),
	m_bindingDuoSwitchUnit(BINDING_UNIT_TYPE_DUOSWITCH,0xFF,0xFF)
{
	//{{AFX_DATA_INIT(CDlgDuoSwitchChildProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgDuoSwitchChildProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitChildProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDuoSwitchChildProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDuoSwitchChildProp, CDlgUnitChildProp)
	//{{AFX_MSG_MAP(CDlgDuoSwitchChildProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_STOPTIME, OnUpDownStopTimer)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_SWITCHTIME, OnUpDownSwitchTimer)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

void CDlgDuoSwitchChildProp::OnNotifyClose(void)
{
	if (m_fChangesMade == TRUE) 
	{
		CString szMessage;
		szMessage.Format(_T("Store Changes of DuoSwitchUnit 0x%02X,0x%02X ?"),bNodeAddress,bUnitAddress);

		if (AfxMessageBox(szMessage,MB_YESNO) == IDYES) 
		{					
			// Construct property Bindings ...		
			*ptrOriginalDuoSwitchUnit = m_bindingDuoSwitchUnit;
			CBindingConversionToNodesProperty ConvertBindingToNodes(ptrOriginalDuoSwitchUnit);
			ConvertBindingToNodes.ProcessPropertyBindings();
		}
	}
}

void CDlgDuoSwitchChildProp::InitControls(CBindingPropertyUnit *ptr)
{
	ASSERT(ptr);
	ptrOriginalDuoSwitchUnit = (CBindingPropertyDuoSwitchUnit*)ptr;

	m_fChangesMade = FALSE;
	bNodeAddress = ptr->GetNodeAddress();
	bUnitAddress = ptr->GetUnitAddress();

	InitControlsToDefaults();							// First

	m_bindingDuoSwitchUnit = *ptrOriginalDuoSwitchUnit;	// Second
	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgDuoSwitchChildProp message handlers

void CDlgDuoSwitchChildProp::InitStaticControls(void)
{
	CDlgUnitChildProp::InitStaticControls();
	CString szStatic;

	{
		CPoint TopLeft(15,50);
		CSize szControlSize(80,20);
		szStatic.Format(_T("Stop Timer :"));
		m_StaticStopTime.Create((LPCTSTR) szStatic,					// CHANGES_MVS_2008_CSTRING
								WS_CHILD | WS_VISIBLE | SS_LEFT,
								CRect(TopLeft,szControlSize),
								this);

		TopLeft.y += 30;
		szStatic.Format(_T("Switch Time :"));
		m_StaticSwitchTime.Create((LPCTSTR) szStatic,				// CHANGES_MVS_2008_CSTRING
								  WS_CHILD | WS_VISIBLE | SS_LEFT,
								  CRect(TopLeft,szControlSize),
								  this);
		
		m_StaticStopTime.SetFont(GetFont());
		m_StaticSwitchTime.SetFont(GetFont());
	}

	{
		CPoint TopLeft(95,50);		
		CSize szControlSize(15,20);
		m_SpinStopTime.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ARROWKEYS,
						  CRect(TopLeft,szControlSize),
						  this,
						  IDC_UPDOWN_STOPTIME);
		
		TopLeft.y += 30;
		m_SpinSwitchTime.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ARROWKEYS,
						  CRect(TopLeft,szControlSize),
						  this,
						  IDC_UPDOWN_SWITCHTIME);
	}
	
	{
		CPoint TopLeft(115,50);
		CSize szControlSize(100,20);		
		m_EditStopTime.Create(WS_CHILD |WS_VISIBLE | WS_BORDER | 
							  ES_LEFT ,
							  CRect(TopLeft,szControlSize),						
							  this,
							  IDC_BUDDY_EDIT_STOPTIME);		

		TopLeft.y += 30;
		m_EditSwitchTime.Create(WS_CHILD |WS_VISIBLE | WS_BORDER | 
							  ES_LEFT ,
							  CRect(TopLeft,szControlSize),						
							  this,
							  IDC_BUDDY_EDIT_SWITCHTIME);

		m_EditStopTime.SetFont(GetFont());
		m_EditSwitchTime.SetFont(GetFont());
	}
}

void CDlgDuoSwitchChildProp::InitControlProperties(void) { }

void CDlgDuoSwitchChildProp::OnUpDownStopTimer(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	BYTE value = m_bindingDuoSwitchUnit.getStopTime( );
	CheckBorders(&value,fDirection,m_bindingDuoSwitchUnit.getStopTimeMinValue(),m_bindingDuoSwitchUnit.getStopTimeMaxValue());
	m_bindingDuoSwitchUnit.setStopTime(value);

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgDuoSwitchChildProp::OnUpDownSwitchTimer(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	BYTE value = m_bindingDuoSwitchUnit.getSwitchTime( );
	CheckBorders(&value,fDirection,m_bindingDuoSwitchUnit.getSwitchTimeMinValue(),m_bindingDuoSwitchUnit.getSwitchTimeMaxValue());
	m_bindingDuoSwitchUnit.setSwitchTime(value);

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgDuoSwitchChildProp::CheckBorders(BYTE* pValue,BOOL fDirection,BYTE bMinValue,BYTE bMaxValue)
{
	if (fDirection == FALSE) 
	{
		if (*pValue <= bMaxValue-1) 
			*pValue += 1;
		else 
			*pValue = bMinValue;
	}
	else 
	{
		if (*pValue >= bMinValue+1)  
			*pValue -= 1;	
		else 
			*pValue = bMaxValue;
	}
	ASSERT(*pValue>=bMinValue && *pValue<=bMaxValue);
}

// Timer waarde stoptimer:
//	- 1 tem 255:	value*600 msec.
// Nxp Firmware v90.24.00.
//	- 1 tem 200:	Value*600 msec.
//  - 201 tem 255:	2 tem 57 minutes.
void CDlgDuoSwitchChildProp::UpdateControls(void)
{
	CString szValue;	
	if (1)
	{	
		const int nValue = m_bindingDuoSwitchUnit.getStopTime() *10 *60;

		if (0 == nValue)  {
			szValue.Format("Continue drive");
		}
		else if (nValue <= 1000) {					
			szValue.Format("%d msec.",nValue);
		}
		else 
		{
			if (m_bindingDuoSwitchUnit.GetSupportedFlags() & CfgInfoUnitDuoswitch::FLAG_SUPPORTED_STOPTIME_LONGERVALUES)
			{
				if (nValue<(200*10*60)) {
					const int seconds = nValue/1000;
					const int milliseconds = (nValue%1000);							
					szValue.Format("%d,%d sec.",seconds,milliseconds);	
				}
				else {
					const int minutes = (((nValue/600)-200)+2);
					szValue.Format("%d min.",minutes);	
				}
			}
			else 
			{		
				if (nValue>1000) {							
					const int seconds = nValue/1000;
					const int milliseconds = (nValue%1000);							
					szValue.Format("%d,%d sec.",seconds,milliseconds);
				}
			}		
		}				
		m_EditStopTime.SetWindowText(szValue);
	}

	if (1)
	{
		const int nValue = m_bindingDuoSwitchUnit.getSwitchTime() *60;
		if (nValue < 1000)  {			
			szValue.Format("%d msec.",nValue);
		}
		else if (nValue > 1000) {			
			szValue.Format("%d,%d sec.",nValue/1000,(nValue%1000));
		}
		m_EditSwitchTime.SetWindowText(szValue);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
void CDlgDuoSwitchChildProp::AcceptSettings(void)
{
	MessageBox("DuoSwitch Prop Dialog Accept\nTodo Construct bindings");

	// TODO Construct Bindings
}
*/

void CDlgDuoSwitchChildProp::RestoreDefaults(void)
{
	InitControlsToDefaults();
	UpdateControls();
	m_fChangesMade = TRUE;
}

void CDlgDuoSwitchChildProp::InitControlsToDefaults(void) {
	m_bindingDuoSwitchUnit.SetToDefaults();
}
