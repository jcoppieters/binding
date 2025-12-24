// DlgBindingVirtualUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingVirtualUnit.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitvirtual.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingVirtualUnit dialog


CDlgBindingVirtualUnit::CDlgBindingVirtualUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingVirtualUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingVirtualUnit = NULL;
}


void CDlgBindingVirtualUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingVirtualUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingVirtualUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingVirtualUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingVirtualUnit message handlers
void CDlgBindingVirtualUnit::InitControls(void)
{	
	pBindingVirtualUnit = (CBindingVirtualUnit*) GetBindingUnit();
	ASSERT(pBindingVirtualUnit);

	//Fill ComboBox Strings ...
	m_CmbMethod.ResetContent();
	m_CmbMethod.ShowWindow(TRUE);

	if (pBindingVirtualUnit->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		for (int i=0;i<MAX_VIRTUALINPUT_FUNCTIONS;i++)
		{	
			CString szString((LPCSTR)VirtualInputStrings[i].ulStringID);		
			m_CmbMethod.InsertString(i,szString);	
		}
	}
	else 
	{		// UNIT_TYPE_OUTPUT
		for (int i=0;i<MAX_VIRTUALOUTPUT_FUNCTIONS;i++)
		{			
			CString szString((LPCSTR) VirtualOutputStrings[i].ulStringID);
			m_CmbMethod.InsertString(i,szString);	
		}
	
	}

	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgBindingVirtualUnit::UpdateControls(void)
{	
	ASSERT(pBindingVirtualUnit);
	int i;
		
	if (pBindingVirtualUnit->GetInOutType() == UNIT_TYPE_INPUT) 
	{	
		const BYTE bMethodData = pBindingVirtualUnit->Get_Event();

		for (i=0;i<MAX_VIRTUALINPUT_FUNCTIONS;i++) 
		{
			if (bMethodData == VirtualInputStrings[i].bMethodData) break;	
		}
		if (i<MAX_VIRTUALINPUT_FUNCTIONS) 
		{
			m_CmbMethod.SetCurSel(i);
			EnableControls();
		}
	}
	else 
	{		// UNIT_TYPE_OUTPUT
		const BYTE bMethodData = pBindingVirtualUnit->Get_Function();

		for (i=0;i<MAX_VIRTUALOUTPUT_FUNCTIONS;i++) 
		{
			if (bMethodData == VirtualOutputStrings[i].bMethodData) break;	
		}
		if (i<MAX_VIRTUALOUTPUT_FUNCTIONS) 
		{
			m_CmbMethod.SetCurSel(i);

			switch(bMethodData)
			{
				case VIRTUAL_OBJECT_METHOD_PULS:
					EnableControls();
				break;

				case VIRTUAL_OBJECT_METHOD_PULSTOGGLE:
				case VIRTUAL_OBJECT_METHOD_TOGGLE:
				{
					CString szEdit1;

					EnableControls(TRUE);
										
					if ((pBindingVirtualUnit->Get_ControlState() & 0x01) == 0x01)
						szEdit1.LoadString(IDS_ON);
					else
						szEdit1.LoadString(IDS_OFF);
					
					m_EditSpin1.SetWindowText(szEdit1);

				} break;	
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////

void CDlgBindingVirtualUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingVirtualUnit);
	const int nCurrSel = m_CmbMethod.GetCurSel();

	if (pBindingVirtualUnit->GetInOutType() == UNIT_TYPE_INPUT) 
	{		
		if (VirtualInputStrings[nCurrSel].bMethodData == 0xFF)
			pBindingVirtualUnit->Set_Event(EV_UNITCONTROLPULS);
		else
			pBindingVirtualUnit->Set_Event(VirtualInputStrings[nCurrSel].bMethodData);			
	}
	else 
	{			
		if (VirtualOutputStrings[nCurrSel].bMethodData == 0xFF)
			pBindingVirtualUnit->Set_Function(VIRTUAL_OBJECT_METHOD_PULS);
		else
			pBindingVirtualUnit->Set_Function(VirtualOutputStrings[nCurrSel].bMethodData);		
	}
}

void CDlgBindingVirtualUnit::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingVirtualUnit);
	if (pBindingVirtualUnit->GetInOutType() == UNIT_TYPE_OUTPUT) 
	{	
		const BYTE bMethodData = pBindingVirtualUnit->Get_Function();
	
		switch(bMethodData)
		{	
			case VIRTUAL_OBJECT_METHOD_PULSTOGGLE:
			case VIRTUAL_OBJECT_METHOD_TOGGLE:
			{
				const BYTE bState = (pBindingVirtualUnit->Get_ControlState() & 0x01) ^ 0x01;							
				pBindingVirtualUnit->Set_ControlState(bState);

			} break;
		}
	}
}
