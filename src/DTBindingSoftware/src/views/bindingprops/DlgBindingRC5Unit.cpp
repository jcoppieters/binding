// DlgBindingRC5Unit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingRC5Unit.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitrc5rx.h"
#include "app\appbindings\appbindingunit\bindingunitrc5rx_def.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingRC5Unit dialog


CDlgBindingRC5Unit::CDlgBindingRC5Unit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingRC5Unit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingRC5RXUnit = NULL;
}


void CDlgBindingRC5Unit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingRC5Unit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingRC5Unit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingRC5Unit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingRC5Unit message handlers
void CDlgBindingRC5Unit::InitControls(void)
{	
	pBindingRC5RXUnit = (CBindingRC5RXUnit*) GetBindingUnit();
	ASSERT(pBindingRC5RXUnit);

	//Fill ComboBox Strings ...
	CString szString;
	m_CmbMethod.ResetContent();
	m_CmbMethod.ShowWindow(TRUE);

	for (int i=0;i<MAX_RC5RX_FUNCTIONS;i++) {	
		szString.LoadString(RC5RXStrings[i].ulStringID);
		m_CmbMethod.InsertString(i,szString);	
	}
	
	UpdateControls();
}

void CDlgBindingRC5Unit::UpdateControls(void)
{
	ASSERT(pBindingRC5RXUnit);
	int i;	
	CString szEdit1,szEdit2;
	BYTE bRC5RXEvent = pBindingRC5RXUnit->Get_Event();
	BYTE bIRCode = pBindingRC5RXUnit->Get_IRCode();

	for (i=0;i<MAX_RC5RX_FUNCTIONS;i++) {
		if (bRC5RXEvent == RC5RXStrings[i].bMethodData) break;	
	}

	if (i<MAX_RC5RX_FUNCTIONS) {
		m_CmbMethod.SetCurSel(i);
		switch(bRC5RXEvent)
		{
			case RC5RX_EVENT_PULS :
				EnableControls(TRUE);
				szEdit1.Format("RX Code: %02d",bIRCode);
				m_EditSpin1.SetWindowText(szEdit1);
			break;

			case RC5RX_EVENT_TOGGLE	:
			case RC5RX_EVENT_PULSTOGGLE	:
				// TM 20040610
				EnableControls(TRUE);	//(TRUE,TRUE);
				szEdit1.Format("RX Code: %02d",bIRCode);
				m_EditSpin1.SetWindowText(szEdit1);
				//if ((pBindingRC5RXUnit->Get_State() & 0x01)==0x01) szEdit2.LoadString(IDS_ON);
				//else szEdit2.LoadString(IDS_OFF);
				//m_EditSpin2.SetWindowText(szEdit2);
			break;		
		}
	}	
}

void CDlgBindingRC5Unit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingRC5RXUnit);
	int nCurrSel = m_CmbMethod.GetCurSel();
	if (RC5RXStrings[nCurrSel].bMethodData == 0xFF)
		pBindingRC5RXUnit->Set_Event(RC5RX_EVENT_PULS);
	else
		pBindingRC5RXUnit->Set_Event(RC5RXStrings[nCurrSel].bMethodData);
}


void CDlgBindingRC5Unit::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingRC5RXUnit);
	BYTE bIRCode;

	bIRCode = pBindingRC5RXUnit->Get_IRCode();

	bIRCode += iStep;
	if (iStep >0) {
		if (bIRCode > RC5_IRCODE_ENTRY_BASIC_MAX) bIRCode = RC5_IRCODE_ENTRY_BASIC_MAX;
	}
	else {
		if (bIRCode > RC5_IRCODE_ENTRY_BASIC_MAX) bIRCode = 0;
	}
	pBindingRC5RXUnit->Set_IRCode(bIRCode);
}

void CDlgBindingRC5Unit::OnSpin2DataChanged(int iStep)
{
	ASSERT(pBindingRC5RXUnit);
	BYTE bMethodData,bState;
	bMethodData = pBindingRC5RXUnit->Get_Event();
	switch(bMethodData)
	{
		case RC5RX_EVENT_TOGGLE	:
		case RC5RX_EVENT_PULSTOGGLE	:		
			bState = pBindingRC5RXUnit->Get_State() & 0x01;
			bState ^= 0x01;
			pBindingRC5RXUnit->Set_State(bState);
		break;
	}
}