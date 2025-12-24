// DlgBindingDuoSwitchUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingDuoSwitchUnit.h"
#include "Mainfrm.h"

#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitduoswitch.h"
#include "BindingUIElements.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingDuoSwitchUnit dialog


CDlgBindingDuoSwitchUnit::CDlgBindingDuoSwitchUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent),
	  m_pBindingDuoSwitchUnit(0),
	  m_pStringEntries(0)
{
	//{{AFX_DATA_INIT(CDlgBindingDuoSwitchUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgBindingDuoSwitchUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingDuoSwitchUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingDuoSwitchUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingDuoSwitchUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingDuoSwitchUnit message handlers
void CDlgBindingDuoSwitchUnit::InitControls(void)
{
	if (TRUE==RetrieveBindingUnit())
	{
		CBindingStringEntries_Col stringCol;
		m_pStringEntries = stringCol.Get(m_pBindingDuoSwitchUnit,CBindingStringEntries_Col::DUOSWITCH_UNITS);

		CBindingStringEntries_Helper Helper(m_pStringEntries);
		Helper.Insert(m_CmbMethod);
		
		m_CmbMethod.ShowWindow(TRUE);
	}
	UpdateControls();
}

BOOL CDlgBindingDuoSwitchUnit::RetrieveBindingUnit( )
{
	m_pBindingDuoSwitchUnit = static_cast<CBindingDuoSwitchUnit*>(GetBindingUnit());
	ASSERT(m_pBindingDuoSwitchUnit);

	return ((m_pBindingDuoSwitchUnit==0) ? FALSE : TRUE);
}

void CDlgBindingDuoSwitchUnit::UpdateControls(void)
{
	ASSERT(m_pBindingDuoSwitchUnit);
	ASSERT(m_pStringEntries);

	const BYTE bMethodData = m_pBindingDuoSwitchUnit->Get_Function();

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.SetCurrSel(m_CmbMethod,bMethodData);
			
	switch(bMethodData)
	{
		case DUOSWITCH_OBJECT_ATTR_UPSTOP:
		{
			CString szEdit1;
			EnableControls(TRUE);			

			if ((m_pBindingDuoSwitchUnit->Get_Status() & 0x01)==0x01) 
				szEdit1.LoadString(IDS_UP);
			else 
				szEdit1.LoadString(IDS_STOP);
			
			m_EditSpin1.SetWindowText(szEdit1);
		} break;

		case DUOSWITCH_OBJECT_ATTR_DOWNSTOP:
		{
			CString szEdit1;
			EnableControls(TRUE);

			if ((m_pBindingDuoSwitchUnit->Get_Status() & 0x01)==0x01) 
				szEdit1.LoadString(IDS_DOWN);
			else 
				szEdit1.LoadString(IDS_STOP);
			
			m_EditSpin1.SetWindowText(szEdit1);

		} break;

		case DUOSWITCH_OBJECT_METHOD_STOP:
		case DUOSWITCH_OBJECT_METHOD_UP:
		case DUOSWITCH_OBJECT_METHOD_DOWN:
		case DUOSWITCH_OBJECT_METHOD_FSUPDOWN:
		{
			EnableControls();

		} break;

	}
}
void CDlgBindingDuoSwitchUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(m_pBindingDuoSwitchUnit);
	ASSERT(m_pStringEntries);
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMethodData = Helper.GetCurrSelData(m_CmbMethod);

	if (bMethodData == 0xFF)
		m_pBindingDuoSwitchUnit->Set_Function(DUOSWITCH_OBJECT_METHOD_FSUPDOWN);
	else
		m_pBindingDuoSwitchUnit->Set_Function(bMethodData);
}

void CDlgBindingDuoSwitchUnit::OnSpin1DataChanged(int iStep)
{
	ASSERT(m_pBindingDuoSwitchUnit);

	const BYTE bMethodData = m_pBindingDuoSwitchUnit->Get_Function();

	switch(bMethodData)
	{
		case DUOSWITCH_OBJECT_ATTR_UPSTOP:
		case DUOSWITCH_OBJECT_ATTR_DOWNSTOP:
		{			
			m_pBindingDuoSwitchUnit->Set_Status((m_pBindingDuoSwitchUnit->Get_Status()&0x01)^0x01);

		} break;
	}
}
