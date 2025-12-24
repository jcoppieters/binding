// DlgBindingDimmerProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingDimmerProp.h"
#include "Mainfrm.h"

#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitdim.h"

#include "BindingUIElements.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////

using namespace DUOTECNO::BINDINGS::UI;

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingDimmerProp dialog

CDlgBindingDimmerProp::CDlgBindingDimmerProp(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent),
	  m_pBindingDimUnit(0),
	  m_pStringEntries(0)
{
	//{{AFX_DATA_INIT(CDlgBindingDimmerProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgBindingDimmerProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingDimmerProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingDimmerProp, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingDimmerProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingDimmerProp message handlers

void CDlgBindingDimmerProp::InitControls(void)
{
	if (RetrieveBindingUnit())
	{
		CBindingStringEntries_Col stringCol;
		m_pStringEntries = stringCol.Get(getBindingDimmerUnit(),CBindingStringEntries_Col::DIMMER_UNITS);

		CBindingStringEntries_Helper Helper(m_pStringEntries);
		Helper.Insert(m_CmbMethod);
		m_CmbMethod.ShowWindow(TRUE);
	}

	UpdateControls();
}

BOOL CDlgBindingDimmerProp::RetrieveBindingUnit( )
{
	m_pBindingDimUnit = static_cast<CBindingDimmerUnit*>(GetBindingUnit());
	ASSERT(m_pBindingDimUnit);

	return ((m_pBindingDimUnit==0) ? FALSE : TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CDlgBindingDimmerProp::SetDimmerValue(CEdit& ctrl)
{
	CString szText;

	if (0!=m_pBindingDimUnit)
	{
		BYTE bDimValue = m_pBindingDimUnit->Get_Value();

		if (bDimValue >= 99) bDimValue = 99;
		szText.Format(_T("%02d%%"),bDimValue); 				
	}
	ctrl.SetWindowText(szText);
}

void CDlgBindingDimmerProp::SetPIRDimmerValue(CEdit& ctrl)
{
	CString szText;
	
	if (0!=m_pBindingDimUnit)
	{
		BYTE bDimValue = m_pBindingDimUnit->Get_Value();

		if (bDimValue == 255) 
		{
			szText.LoadString(IDS_CURRENT);
		}
		else 
		{
			if (bDimValue >= 99) bDimValue = 99;
			szText.Format(_T("%02d%%"),bDimValue); 
		}
	}

	ctrl.SetWindowText(szText);
}

void CDlgBindingDimmerProp::SetPIRTimerValue(CEdit& ctrl)
{
	CString szText;

	if (0!=m_pBindingDimUnit)
	{
		BYTE bPirTimer = m_pBindingDimUnit->Get_PirTime();
		if (bPirTimer == 255) bPirTimer = 3;
	
		szText.Format("%02d min.",bPirTimer);		
	}
	ctrl.SetWindowText(szText);
}

void CDlgBindingDimmerProp::SetSpeed(CEdit& ctrl,BYTE bSpeed)
{
	CString szText;

	szText.Format("Speed : %d",bSpeed);	
	ctrl.SetWindowText(szText);
}

void CDlgBindingDimmerProp::SetDimCalcValue(CEdit& Ctrl)
{
	CString szText;

	if (0!=m_pBindingDimUnit)
	{
		BYTE bValue = m_pBindingDimUnit->Get_CalcValue();

		ASSERT(bValue>=0 && bValue<=99);
		szText.Format("%d",bValue);

		Ctrl.SetWindowText(szText);
	}
}

// DIMMER_CHANGES_RANGE
void CDlgBindingDimmerProp::setDimRangeValue(CEdit& ctrl,const CString& msg,int value) {
	CString szText;	
	szText.Format("%s: %d",msg,value);
	ctrl.SetWindowText(szText);
}

// DIMMER_CHANGES_RANGE
void CDlgBindingDimmerProp::setDimRangeMinValue(CEdit& ctrl) {
	if (0!=m_pBindingDimUnit) {
		const int value = m_pBindingDimUnit->Get_MinRange();
		ASSERT(value>0 && value<100);
		setDimRangeValue(ctrl,"Min value",value);
	}
}

// DIMMER_CHANGES_RANGE
void CDlgBindingDimmerProp::setDimRangeMaxValue(CEdit& ctrl) {
	if (0!=m_pBindingDimUnit) {
		const int value = m_pBindingDimUnit->Get_MaxRange();
		ASSERT(value>0 && value<100);
		setDimRangeValue(ctrl,"Max value",value);
	}
}

/////////////////////////////////////////////////////////////////////////////

void CDlgBindingDimmerProp::UpdateControls(void)
{
	ASSERT(m_pBindingDimUnit);
	ASSERT(m_pStringEntries);
	
	const BYTE bMethodData = m_pBindingDimUnit->Get_Function();
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.SetCurrSel(m_CmbMethod,bMethodData);
		
	switch(bMethodData)
	{
		case DIMMER_OBJECT_ATTR_ONOFF:
		{
			EnableControls(TRUE);

			CString szEdit1;
			if ((m_pBindingDimUnit->Get_Status() & 0x01) == 0x01) szEdit1.LoadString(IDS_ON);
			else szEdit1.LoadString(IDS_OFF);			
			m_EditSpin1.SetWindowText(szEdit1);

		} break;
			
		case DIMMER_OBJECT_METHOD_OFF:
		case DIMMER_OBJECT_METHOD_ONCV:		
		case DIMMER_OBJECT_METHOD_FSUD:
		case DIMMER_OBJECT_METHOD_FSUDONOFF:
		case DIMMER_OBJECT_METHOD_FSCVONOFF:
		case DIMMER_OBJECT_METHOD_DN:
		case DIMMER_OBJECT_METHOD_UP:
		{
			EnableControls();				

		} break;

		// Solved BUG-0151
		case DIMMER_OBJECT_METHOD_INC:
		case DIMMER_OBJECT_METHOD_DEC:
		{
			EnableControls(TRUE);
			SetDimCalcValue(m_EditSpin1);

		} break;

		case DIMMER_OBJECT_METHOD_FSCVOFF:
		case DIMMER_OBJECT_ATTR_VALUE:
		{
			EnableControls(TRUE);				
			SetDimmerValue(m_EditSpin1);

		} break;
			
		case DIMMER_OBJECT_METHOD_ONUP:
		{
			EnableControls(TRUE,TRUE,TRUE);									

			SetDimmerValue(m_EditSpin1);
			SetSpeed(m_EditSpin2,m_pBindingDimUnit->Get_SpeedUp());

			// Status - Switch On or Stay in up State
			CString szEdit3;

			if ((m_pBindingDimUnit->Get_Status() & 0x01) == 0x01) 
				szEdit3.LoadString(IDS_DIM_SW_ON);
			else 
				szEdit3.LoadString(IDS_DIM_STAY_UP);
					
			m_EditSpin3.SetWindowText(szEdit3);
		} break;

		case DIMMER_OBJECT_METHOD_ONDN:
		{
			EnableControls(TRUE,TRUE,TRUE);			
			
			SetDimmerValue(m_EditSpin1);
			SetSpeed(m_EditSpin2,m_pBindingDimUnit->Get_SpeedDown());

			// Status - Switch Off or Stay in dn State
			CString szEdit3;
			if ((m_pBindingDimUnit->Get_Status() & 0x01) == 0x01) 
				szEdit3.LoadString(IDS_DIM_SW_OFF);
			else 
				szEdit3.LoadString(IDS_DIM_STAY_DOWN);	

			m_EditSpin3.SetWindowText(szEdit3);

		} break;

		case DIMMER_OBJECT_METHOD_PIRON:
		{
			EnableControls(TRUE,TRUE);	

			SetPIRTimerValue(m_EditSpin1);
			SetPIRDimmerValue(m_EditSpin2);	

		} break;		

		case DIMMER_OBJECT_ATTR_RANGE:	// DIMMER_CHANGES_RANGE
		{
			EnableControls(TRUE,TRUE);	
			setDimRangeMinValue(m_EditSpin1);	
			setDimRangeMaxValue(m_EditSpin2);		
		} break;
		
		case DIMMER_OBJECT_ATTR_CONFIG:
		case DIMMER_OBJECT_ATTR_STATE:	
		case DIMMER_OBJECT_ATTR_SPEED:
		case DIMMER_OBJECT_USE_SELECTOR_DATA:
		default:
			EnableControls();
		break;			
	}	
}


/////////////////////////////////////////////////////////////////////////////
// Do When CmboBox Changed !!
void CDlgBindingDimmerProp::OnCmbBoxDataChanged(void)
{
	ASSERT(m_pBindingDimUnit);
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);
	
	if (bMessageCode == 0xFF)
		m_pBindingDimUnit->Set_Function(DIMMER_OBJECT_METHOD_FSCVONOFF);	
	else
		m_pBindingDimUnit->Set_Function(bMessageCode);
}


void CDlgBindingDimmerProp::OnSpin1DataChanged(int iStep)
{
	ASSERT(m_pBindingDimUnit);
	
	const BYTE bMethodData = m_pBindingDimUnit->Get_Function();
	switch(bMethodData)
	{
		case DIMMER_OBJECT_ATTR_ONOFF:
		{		
			m_pBindingDimUnit->Set_Status((m_pBindingDimUnit->Get_Status()&0x01)^0x01);
		} break;

		case DIMMER_OBJECT_METHOD_FSCVOFF:
		case DIMMER_OBJECT_ATTR_VALUE:
		case DIMMER_OBJECT_METHOD_ONUP:
		case DIMMER_OBJECT_METHOD_ONDN:
		{
			BYTE bDimValue = m_pBindingDimUnit->Get_Value();
			if (iStep >0) 
			{
				bDimValue = bDimValue + iStep;
				if (bDimValue >= 99) bDimValue = 99;
			}
			else 
			{
				bDimValue = bDimValue + iStep;
				if (bDimValue > 99) bDimValue = 0; 
			}			
			m_pBindingDimUnit->Set_Value(bDimValue);
		} break;

		case DIMMER_OBJECT_METHOD_PIRON:
		{
			BYTE bPirTime = m_pBindingDimUnit->Get_PirTime();
			if (iStep >0) 
			{
				if (bPirTime == 0xFF) bPirTime = 4;
				else 
				{
					bPirTime += iStep;
					if (bPirTime >= 59) bPirTime = 59;
				}
			}
			else 
			{
				if (bPirTime == 0xFF) bPirTime = 2;
				else 
				{
					bPirTime += iStep;
					if ((bPirTime < 1) || (bPirTime >= 59)) bPirTime = 1;
				}
			}
			m_pBindingDimUnit->Set_PirTime(bPirTime);
		} break;

		// Solved BUG-0151
		case DIMMER_OBJECT_METHOD_INC:
		case DIMMER_OBJECT_METHOD_DEC:
		{
			int temp = m_pBindingDimUnit->Get_CalcValue() + iStep;

			if (temp >= 99)
				temp=99;
			
			if (temp <= 0)
				temp = 1;

			m_pBindingDimUnit->Set_CalcValue((BYTE)temp);
		} break;

		// DIMMER_CHANGES_RANGE: De minimum waarde.
		case DIMMER_OBJECT_ATTR_RANGE: {
			int temp = m_pBindingDimUnit->Get_MinRange() + iStep;	
			if (temp > 99) 
				temp=99;			
			else if (temp < 1)
				temp = 1;
			m_pBindingDimUnit->Set_MinRange(temp);									
		} break;
	}
}

void CDlgBindingDimmerProp::OnSpin2DataChanged(int iStep)
{
	ASSERT(m_pBindingDimUnit);
	
	const BYTE bMethodData = m_pBindingDimUnit->Get_Function();

	switch(bMethodData)
	{
		case DIMMER_OBJECT_METHOD_ONUP:
		{			
			m_pBindingDimUnit->Set_SpeedUp(m_pBindingDimUnit->Get_SpeedUp()+iStep);
		} break;

		case DIMMER_OBJECT_METHOD_ONDN:
		{				
			m_pBindingDimUnit->Set_SpeedDown(m_pBindingDimUnit->Get_SpeedDown()+iStep);
		} break;

		case DIMMER_OBJECT_METHOD_PIRON:
		{
			BYTE bDimValue = m_pBindingDimUnit->Get_Value();

			if (iStep >0) 
			{
				if (bDimValue == 99) bDimValue = 255;
				else 
				{
					if (bDimValue != 255) 
					{
						bDimValue = bDimValue + iStep;
						if (bDimValue >= 99) bDimValue = 99;
					}
				}
			}
			else 
			{
				if (bDimValue == 0xFF) bDimValue = 99;
				bDimValue = bDimValue + iStep;
				if (bDimValue > 99) bDimValue = 0; 
			}			
			m_pBindingDimUnit->Set_Value(bDimValue);			

		} break;
			
		// DIMMER_CHANGES_RANGE: De maximum waarde.
		case DIMMER_OBJECT_ATTR_RANGE: {
			int temp = m_pBindingDimUnit->Get_MaxRange() + iStep;	
			if (temp > 99) 
				temp=99;			
			else if (temp < 1)
				temp = 1;
			m_pBindingDimUnit->Set_MaxRange(temp);									
		} break;
	}
}

void CDlgBindingDimmerProp::OnSpin3DataChanged(int iStep)
{
	ASSERT(m_pBindingDimUnit);
	
	const BYTE bMethodData = m_pBindingDimUnit->Get_Function();
	switch(bMethodData)
	{
		case DIMMER_OBJECT_METHOD_ONDN:	
		case DIMMER_OBJECT_METHOD_ONUP:		
		{		
			m_pBindingDimUnit->Set_Status((m_pBindingDimUnit->Get_Status()&0x01)^0x01);

		} break;
	}
}

/////////////////////////////////////////////////////////////////////////////
