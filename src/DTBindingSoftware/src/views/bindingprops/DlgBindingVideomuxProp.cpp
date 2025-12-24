// DlgBindingVideomuxProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingVideomuxProp.h"
#include "Mainfrm.h"

#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitvideomux.h"
#include "bindingUIElements.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingVideomuxProp dialog


CDlgBindingVideomuxProp::CDlgBindingVideomuxProp(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingVideomuxProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingVideomuxUnit = NULL;
	m_pStringEntries = 0;
}


void CDlgBindingVideomuxProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingVideomuxProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingVideomuxProp, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingVideomuxProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingVideomuxProp message handlers
void CDlgBindingVideomuxProp::InitControls(void)
{
	pBindingVideomuxUnit = static_cast<CBindingVideomuxUnit*>(GetBindingUnit());
	ASSERT(pBindingVideomuxUnit);

	CBindingStringEntries_Col stringCol;
	m_pStringEntries = stringCol.Get(pBindingVideomuxUnit,CBindingStringEntries_Col::VIDEOMUX_UNITS);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.Insert(m_CmbMethod);
	m_CmbMethod.ShowWindow(TRUE);

	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
void CDlgBindingVideomuxProp::UpdateControls(void)
{
	ASSERT(pBindingVideomuxUnit);
	ASSERT(m_pStringEntries);
	
	const BYTE bMethodData = pBindingVideomuxUnit->Get_Function();
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.SetCurrSel(m_CmbMethod,bMethodData);

	const BYTE bOutput = (pBindingVideomuxUnit->Get_Output() & 0x0F);
	const BYTE bInput = (pBindingVideomuxUnit->Get_Input() & 0x0F);

	switch(bMethodData)
	{
		case VIDEOMUX_OBJECT_ATTR_CONNECTION :
		{
			CString szEdit1,szEdit2,szEdit3;

			EnableControls(TRUE,TRUE,TRUE);

			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);

			szEdit2.Format(_T("Input %d"),bInput);
			m_EditSpin2.SetWindowText(szEdit2);

			if (pBindingVideomuxUnit->Get_State() & 0x01) 
				szEdit3.LoadString(IDS_ON);
			else 
				szEdit3.LoadString(IDS_OFF);
			m_EditSpin3.SetWindowText(szEdit3);			

		} break;

		case VIDEOMUX_OBJECT_METHOD_CONNECT :
		case VIDEOMUX_OBJECT_METHOD_DISCONNECT :
		{
			CString szEdit1,szEdit2;

			EnableControls(TRUE,TRUE);
				
			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);
				
			szEdit2.Format(_T("Input %d"),bInput);
			m_EditSpin2.SetWindowText(szEdit2);

		} break;

		case VIDEOMUX_OBJECT_METHOD_DISCONNECT_ALL :
		{
			EnableControls();	
		} break;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS:				
		{				
			EnableControls(TRUE);

			BYTE bTemp = (pBindingVideomuxUnit->Get_Keypress()) & 0x0F;
			if (bTemp > 0x0a) 
				bTemp = 0x0A;
		
			m_EditSpin1.SetWindowText(CString((LPCSTR) IDS_RAS_MULTISCREEN + bTemp));

		} break;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS_CAMERA :
		{
			CString szEdit1;

			EnableControls(TRUE);
				
			const BYTE bTemp = (pBindingVideomuxUnit->Get_ButtonNr()) & 0x0F;
				
			szEdit1.Format(_T("Key %d"),bTemp);
			m_EditSpin1.SetWindowText(szEdit1);

		} break;

		case VIDEOMUX_OBJECT_METHOD_DVMR :
		{			
			EnableControls(TRUE);
				
			BYTE bTemp = (pBindingVideomuxUnit->Get_DvmrFunction()) & 0x0F;
			if (bTemp > 0x07) 
				bTemp = 0x07;							

			m_EditSpin1.SetWindowText(CString((LPCSTR) IDS_RAS_DVMRLIVE + bTemp));

		} break;

		default:
		{
			ASSERT(0);
		} break;
	}
}

// Do When CmboBox Changed !!
void CDlgBindingVideomuxProp::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingVideomuxUnit);
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);
	
	if (bMessageCode == 0xFF)
		pBindingVideomuxUnit->Set_Function(VIDEOMUX_OBJECT_METHOD_CONNECT);
	else
		pBindingVideomuxUnit->Set_Function(bMessageCode);
}



static void Normalize(BYTE& bValue,int iStep,const BYTE MAX)
{
	bValue += iStep;

	if (iStep>0)	
	{
		if (bValue > MAX) 
			bValue = MAX;	
	}
	else	
	{
		if (bValue > MAX) 
			bValue = 0;	
	}
} 


void CDlgBindingVideomuxProp::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingVideomuxUnit);	

	const BYTE bMethodData = pBindingVideomuxUnit->Get_Function();
	switch(bMethodData)
	{
		case VIDEOMUX_OBJECT_ATTR_CONNECTION:
		case VIDEOMUX_OBJECT_METHOD_CONNECT :
		case VIDEOMUX_OBJECT_METHOD_DISCONNECT :
		{
			// Should always be 0
			pBindingVideomuxUnit->Set_Output(0);

		} break;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS :
		{
			BYTE bTemp = pBindingVideomuxUnit->Get_Keypress();
			Normalize(bTemp,iStep,0x0A);
			pBindingVideomuxUnit->Set_Keypress(bTemp);

		} break;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS_CAMERA :
		{
			BYTE bTemp = pBindingVideomuxUnit->Get_ButtonNr();
			Normalize(bTemp,iStep,0x0F);				
			pBindingVideomuxUnit->Set_ButtonNr(bTemp);

		} break;

		case VIDEOMUX_OBJECT_METHOD_DVMR :
		{
			BYTE bTemp = pBindingVideomuxUnit->Get_DvmrFunction();
			Normalize(bTemp,iStep,0x07);		
			pBindingVideomuxUnit->Set_DvmrFunction(bTemp);

		} break;
	}
}

void CDlgBindingVideomuxProp::OnSpin2DataChanged(int iStep)
{
	ASSERT(pBindingVideomuxUnit);

	const BYTE bMethodData = pBindingVideomuxUnit->Get_Function();
	switch(bMethodData)
	{
		case VIDEOMUX_OBJECT_ATTR_CONNECTION:
		case VIDEOMUX_OBJECT_METHOD_CONNECT :
		case VIDEOMUX_OBJECT_METHOD_DISCONNECT :
		{
			BYTE bInput = pBindingVideomuxUnit->Get_Input();
			Normalize(bInput,iStep,0x07);			
			pBindingVideomuxUnit->Set_Input(bInput);

		} break;
	}
}

void CDlgBindingVideomuxProp::OnSpin3DataChanged(int iStep)
{
	ASSERT(pBindingVideomuxUnit);

	const BYTE bMethodData = pBindingVideomuxUnit->Get_Function();
	switch(bMethodData)
	{
		case VIDEOMUX_OBJECT_ATTR_CONNECTION:
		{
			pBindingVideomuxUnit->Set_State((pBindingVideomuxUnit->Get_State()&0x01)^0x01);
		
		} break;
	}
}
