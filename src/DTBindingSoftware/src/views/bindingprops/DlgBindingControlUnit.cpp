// DlgBindingControlUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingControlUnit.h"
#include "Mainfrm.h"

#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitcontrol.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingControlUnit dialog


CDlgBindingControlUnit::CDlgBindingControlUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingControlUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingControlUnit = NULL;
}


void CDlgBindingControlUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingControlUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingControlUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingControlUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingControlUnit message handlers
void CDlgBindingControlUnit::InitControls(void)
{
	pBindingControlUnit = (CBindingControlUnit*) GetBindingUnit();
	ASSERT(pBindingControlUnit);	

	//Fill ComboBox Strings ...
	CString szString;
	m_CmbMethod.ResetContent();
	m_CmbMethod.ShowWindow(TRUE);

	for (int i=0;i<MAX_CONTROL_FUNCTIONS;i++)
	{	
		szString.LoadString(ControlStrings[i].ulStringID);
		m_CmbMethod.InsertString(i,szString);	
	}
	
	UpdateControls();
}

void CDlgBindingControlUnit::UpdateControls(void)
{
	ASSERT(pBindingControlUnit);
	int i;	
	
	const BYTE bControlEvent = pBindingControlUnit->Get_Event();
	
	for (i=0;i<MAX_CONTROL_FUNCTIONS;i++) 
	{
		if (bControlEvent == ControlStrings[i].bMethodData) 
		{
			break;	
		}
	}

	if (i<MAX_CONTROL_FUNCTIONS) 
	{
		m_CmbMethod.SetCurSel(i);
	}

	EnableControls();
}

void CDlgBindingControlUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingControlUnit);
	const int nCurrSel = m_CmbMethod.GetCurSel();

	if (ControlStrings[nCurrSel].bMethodData == 0xFF)
		pBindingControlUnit->Set_Event(EV_UNITCONTROLPULS);
	else
		pBindingControlUnit->Set_Event(ControlStrings[nCurrSel].bMethodData);
}
