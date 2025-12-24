// DlgBindingSwitchUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingSwitchUnit.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitrelais.h"

#include "BindingUIElements.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingSwitchUnit dialog

CDlgBindingSwitchUnit::CDlgBindingSwitchUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent),
	  m_pBindingSwitchUnit(0),
	  m_pStringEntries(0)
{
	//{{AFX_DATA_INIT(CDlgBindingSwitchUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgBindingSwitchUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingSwitchUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingSwitchUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingSwitchUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingSwitchUnit message handlers
void CDlgBindingSwitchUnit::InitControls(void)
{
	if(TRUE==RetrieveBindingUnit())
	{	
		CBindingStringEntries_Col stringCol;
		m_pStringEntries = stringCol.Get(m_pBindingSwitchUnit,CBindingStringEntries_Col::SWITCH_UNITS);

		CBindingStringEntries_Helper Helper(m_pStringEntries);
		Helper.Insert(m_CmbMethod);
		m_CmbMethod.ShowWindow(TRUE);
	}
	UpdateControls();
}

BOOL CDlgBindingSwitchUnit::RetrieveBindingUnit( )
{
	m_pBindingSwitchUnit = static_cast<CBindingRelaisUnit*>(GetBindingUnit());
	ASSERT(m_pBindingSwitchUnit);

	return ((m_pBindingSwitchUnit==0) ? FALSE : TRUE);
}

void CDlgBindingSwitchUnit::UpdateControls(void)
{
	ASSERT(m_pBindingSwitchUnit);
	ASSERT(m_pStringEntries);
		
	const BYTE bMethodData = m_pBindingSwitchUnit->Get_Function();
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.SetCurrSel(m_CmbMethod,bMethodData);
	
	switch(bMethodData)
	{
		case SWITCH_OBJECT_ATTR_ONOFF:
		{
			CString szEdit1;

			EnableControls(TRUE);
			if ((m_pBindingSwitchUnit->Get_Status() & 0x01)==0x01) 
				szEdit1.LoadString(IDS_ON);
			else 
				szEdit1.LoadString(IDS_OFF);
			
			m_EditSpin1.SetWindowText(szEdit1);
		} break;

		case SWITCH_OBJECT_METHOD_FSONOFF:
		case SWITCH_OBJECT_METHOD_OFF:
		case SWITCH_OBJECT_METHOD_ON:
		{
			EnableControls();
		} break;

		case SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR:
		{
			CString szEdit1;
			
			EnableControls(TRUE);
			BYTE bPirTimer = m_pBindingSwitchUnit->Get_PirTime();
			if (bPirTimer == 255) bPirTimer = 3;
		
			szEdit1.Format("%02d min.",bPirTimer);			
			m_EditSpin1.SetWindowText(szEdit1);
		} break;
	}	
}

void CDlgBindingSwitchUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(m_pBindingSwitchUnit);
	ASSERT(m_pStringEntries);
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	const BYTE bMethodData = Helper.GetCurrSelData(m_CmbMethod);
	if (bMethodData == 0xFF)
		m_pBindingSwitchUnit->Set_Function(SWITCH_OBJECT_METHOD_FSONOFF);
	else
		m_pBindingSwitchUnit->Set_Function(bMethodData);
}

void CDlgBindingSwitchUnit::OnSpin1DataChanged(int iStep)
{
	ASSERT(m_pBindingSwitchUnit);

	const BYTE bMethodData = m_pBindingSwitchUnit->Get_Function();

	switch(bMethodData)
	{
		case SWITCH_OBJECT_ATTR_ONOFF:
		{
			m_pBindingSwitchUnit->Set_Status((m_pBindingSwitchUnit->Get_Status()&0x01)^0x01);

		} break;

		case SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR:
		{
			BYTE bPirTime = m_pBindingSwitchUnit->Get_PirTime();
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
			m_pBindingSwitchUnit->Set_PirTime(bPirTime);
		} break;
	}
}

