// DlgBindingAVMatrixProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingAVMatrixProp.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitavmatrix.h"
#include "bindingUIElements.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////

// private functions:

static void Normalize(
	int& value,
	int iStep,
	const int MAX = 255,
	const int MIN = 0)
{
	value += iStep;

	if (iStep>0)	
	{
		if (value > MAX) 
			value = MAX;	
	}
	else	
	{
		if (value < MIN) 
			value = MIN;	
	}
} 


// CDlgBindingAVMatrixProp dialog

CDlgBindingAVMatrixProp::CDlgBindingAVMatrixProp(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingAVMatrixProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pBindingAVMatrixUnit = 0;
	m_pStringEntries = 0;
}


void CDlgBindingAVMatrixProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingAVMatrixProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingAVMatrixProp, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingAVMatrixProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAVMatrixProp message handlers
void CDlgBindingAVMatrixProp::InitControls(void)
{
#ifdef SKIP
	m_pBindingAVMatrixUnit = static_cast<CBindingAVMatrixUnit*>(GetBindingUnit());
	ASSERT(m_pBindingAVMatrixUnit);
#endif

	if (RetrieveBindingUnit())
	{
		if (LoadStringEntryResources(CBindingStringEntries_Col::AVMATRIX_UNITS))
		{	
			CBindingStringEntries_Helper Helper(m_pStringEntries);

			Helper.Insert(m_CmbMethod);
			m_CmbMethod.ShowWindow(TRUE);
		}		
	}

	UpdateControls();
}

BOOL CDlgBindingAVMatrixProp::RetrieveBindingUnit( )
{
	m_pBindingAVMatrixUnit = static_cast<CBindingAVMatrixUnit*>(GetBindingUnit());
	ASSERT(m_pBindingAVMatrixUnit);

	return (m_pBindingAVMatrixUnit == 0 ? FALSE : TRUE);
}

BOOL CDlgBindingAVMatrixProp::LoadStringEntryResources(int key)
{
	CBindingStringEntries_Col stringCol;
	m_pStringEntries = stringCol.Get(m_pBindingAVMatrixUnit,(CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE) key);

	return (m_pStringEntries == 0 ? FALSE : TRUE);
}

void CDlgBindingAVMatrixProp::UpdateControls(void)
{
	ASSERT(m_pBindingAVMatrixUnit);
	ASSERT(m_pStringEntries);
	
	const BYTE bMethodData = m_pBindingAVMatrixUnit->Get_Function();
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.SetCurrSel(m_CmbMethod,bMethodData);
		
	BYTE bOutput = m_pBindingAVMatrixUnit->Get_Output();
	if (bOutput >= CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS)
	{
		bOutput = CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS-1;
	}

	BYTE bInput = m_pBindingAVMatrixUnit->Get_Input();
	if (bInput >= CBindingAVMatrixUnit::AVMATRIX_MAX_INPUTS)
	{
		bInput = CBindingAVMatrixUnit::AVMATRIX_MAX_INPUTS-1;
	}
	
	switch(bMethodData)
	{
		case AVMATRIX_OBJECT_ATTR_CONNECTION :
		{
			CString szEdit1,szEdit2,szEdit3;
	
			EnableControls(TRUE,TRUE,TRUE);

			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);
				
			szEdit2.Format(_T("Input %d"),bInput);
			m_EditSpin2.SetWindowText(szEdit2);
				
			if (m_pBindingAVMatrixUnit->Get_State() & 0x01) 
				szEdit3.LoadString(IDS_ON);
			else 
				szEdit3.LoadString(IDS_OFF);
				
			m_EditSpin3.SetWindowText(szEdit3);			

		} break;

		case AVMATRIX_OBJECT_METHOD_CONNECT :
		case AVMATRIX_OBJECT_METHOD_DISCONNECT :
		{
			CString szEdit1,szEdit2;
	
			EnableControls(TRUE,TRUE);
				
			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);
				
			szEdit2.Format(_T("Input %d"),bInput);
			m_EditSpin2.SetWindowText(szEdit2);

		} break;

		case AVMATRIX_OBJECT_METHOD_DISCONNECT_ALL :
		{
			EnableControls();	
		} break;

		case AVMATRIX_OBJECT_METHOD_VOLUME_UP :
		case AVMATRIX_OBJECT_METHOD_VOLUME_DOWN :
		case AVMATRIX_OBJECT_METHOD_VOLUME_STOP :
		case AVMATRIX_OBJECT_METHOD_VOLUME_INC :
		case AVMATRIX_OBJECT_METHOD_VOLUME_DEC :
		case AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE :
		{
			CString szEdit1;

			EnableControls(TRUE);	

			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);

		} break;

		case AVMATRIX_OBJECT_ATTR_VOLUME :
		{
			CString szEdit1,szEdit2;
	
			EnableControls(TRUE,TRUE);
				
			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);
				
			BYTE bTemp = m_pBindingAVMatrixUnit->Get_Volume();
			if (bTemp > CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME) 
				bTemp = CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME;

			szEdit2.Format(_T("Vol. %d"),bTemp);
			m_EditSpin2.SetWindowText(szEdit2);

		} break;

		case AVMATRIX_OBJECT_ATTR_BASS :
		{
			CString szEdit1,szEdit2;
	
			EnableControls(TRUE,TRUE);

			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);

			BYTE bTemp = m_pBindingAVMatrixUnit->Get_Bass();
			if (bTemp > CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME) 
				bTemp = CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME;
			
			szEdit2.Format(_T("Bass : %d"),bTemp);
			m_EditSpin2.SetWindowText(szEdit2);
		} break;

		case AVMATRIX_OBJECT_ATTR_TREBLE :
		{
			CString szEdit1,szEdit2;
			EnableControls(TRUE,TRUE);
				
			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);

			BYTE bTemp = m_pBindingAVMatrixUnit->Get_Treble();
			if (bTemp > CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME) 
				bTemp = CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME;

			szEdit2.Format(_T("Treble : %d"),bTemp);
			m_EditSpin2.SetWindowText(szEdit2);

		} break;

		case AVMATRIX_OBJECT_METHOD_MACRO:
		{
			CString szEdit1,szEdit2;
			EnableControls(TRUE,TRUE);
				
			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);
			
			szEdit2.Format(_T("Macro %d"),m_pBindingAVMatrixUnit->Get_MacroCode());
			m_EditSpin2.SetWindowText(szEdit2);
			
		} break;

		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE:
		{
			CString szEdit1,szEdit2,szEdit3;
			BYTE bDestMasterIdx=m_pBindingAVMatrixUnit->Get_DestMasterIdx();
			BYTE bDestSlaveIdx=m_pBindingAVMatrixUnit->Get_DestSlaveIdx();
			EnableControls(TRUE,TRUE,TRUE);

			// Edit 1: Output idx 0-63
			szEdit1.Format(_T("Output %d"),bOutput);
			m_EditSpin1.SetWindowText(szEdit1);

			// Edit 2: Master output idx 0-63
			if (bDestMasterIdx>=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS)
			{
				szEdit2.Format(_T("Undefined"));
			}
			else
			{
				szEdit2.Format(_T("Master output %d"),bDestMasterIdx);
			}
			m_EditSpin2.SetWindowText(szEdit2);

			// Edit 3: Slave output idx 0-63 or 0xFF
			if (bDestSlaveIdx>=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS)
			{
				szEdit3.Format(_T("None"));
			}
			else
			{
				szEdit3.Format(_T("Slave output %d"),bDestSlaveIdx);
			}
			m_EditSpin3.SetWindowText(szEdit3);

		} break;

		default:
		{
			ASSERT(0);
		} break;
	}
}

void CDlgBindingAVMatrixProp::OnCmbBoxDataChanged(void)
{
	ASSERT(m_pBindingAVMatrixUnit);
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);
	
	if (bMessageCode == 0xFF)
		m_pBindingAVMatrixUnit->Set_Function(AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE);
	else
		m_pBindingAVMatrixUnit->Set_Function(bMessageCode);
}

void CDlgBindingAVMatrixProp::OnSpin1DataChanged(int iStep)
{
	ASSERT(m_pBindingAVMatrixUnit);
	
	const BYTE bMethodData = m_pBindingAVMatrixUnit->Get_Function();	
	switch(bMethodData)
	{
		case AVMATRIX_OBJECT_METHOD_CONNECT :
		case AVMATRIX_OBJECT_METHOD_DISCONNECT :
		case AVMATRIX_OBJECT_ATTR_CONNECTION :
		case AVMATRIX_OBJECT_METHOD_VOLUME_UP :
		case AVMATRIX_OBJECT_METHOD_VOLUME_DOWN :
		case AVMATRIX_OBJECT_METHOD_VOLUME_STOP :
		case AVMATRIX_OBJECT_METHOD_VOLUME_INC :
		case AVMATRIX_OBJECT_METHOD_VOLUME_DEC :
		case AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE :
		case AVMATRIX_OBJECT_ATTR_VOLUME :
		case AVMATRIX_OBJECT_ATTR_BASS :
		case AVMATRIX_OBJECT_ATTR_TREBLE :
		case AVMATRIX_OBJECT_METHOD_MACRO:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE:
		{
			int temp = m_pBindingAVMatrixUnit->Get_Output();

			Normalize(temp,iStep,CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS);
			ASSERT(temp>=0 && temp <= 255);
			m_pBindingAVMatrixUnit->Set_Output((BYTE) temp);

		} break;	
	}
}

void CDlgBindingAVMatrixProp::OnSpin2DataChanged(int iStep)
{
	ASSERT(m_pBindingAVMatrixUnit);
	
	const BYTE bMethodData = m_pBindingAVMatrixUnit->Get_Function();

	switch(bMethodData)
	{
		case AVMATRIX_OBJECT_METHOD_CONNECT :
		case AVMATRIX_OBJECT_METHOD_DISCONNECT :
		case AVMATRIX_OBJECT_ATTR_CONNECTION :	
		{
			int temp = m_pBindingAVMatrixUnit->Get_Input();			
			Normalize(temp,iStep,CBindingAVMatrixUnit::AVMATRIX_MAX_INPUTS);
			ASSERT(temp>=0 && temp <= 255);
			m_pBindingAVMatrixUnit->Set_Input((BYTE) temp);

		} break;

		case AVMATRIX_OBJECT_ATTR_VOLUME :
		{
			int temp = m_pBindingAVMatrixUnit->Get_Volume();
			Normalize(temp,iStep,CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME);		
			ASSERT(temp>=0 && temp <= 255);
			m_pBindingAVMatrixUnit->Set_Volume((BYTE) temp);

		} break;

		case AVMATRIX_OBJECT_ATTR_BASS:
		{
			int temp = m_pBindingAVMatrixUnit->Get_Bass();
			Normalize(temp,iStep,CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME);
			ASSERT(temp>=0 && temp <= 255);
			m_pBindingAVMatrixUnit->Set_Bass((BYTE) temp);

		} break;
		
		case AVMATRIX_OBJECT_ATTR_TREBLE :
		{
			int temp = m_pBindingAVMatrixUnit->Get_Treble();
			Normalize(temp,iStep,CBindingAVMatrixUnit::AVMATRIX_MAX_VOLUME);		
			ASSERT(temp>=0 && temp <= 255);
			m_pBindingAVMatrixUnit->Set_Treble(temp);

		} break;

		case AVMATRIX_OBJECT_METHOD_MACRO:
		{			
			int value = m_pBindingAVMatrixUnit->Get_MacroCode();
			Normalize(value, iStep, CBindingAVMatrixUnit::AVMATRIX_MAX_MACRO);
			m_pBindingAVMatrixUnit->Set_MacroCode(value);
		} break;

		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE:
		{
			// Master Idx: 0-63 of 255
			int value = m_pBindingAVMatrixUnit->Get_DestMasterIdx();
			value+=iStep;

			if(value<0) 
			{
				value=0;
			}
			else if (value>=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS && 
				     value<0xFF)
			{
				value=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS-1;
			}
			else if (value>=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS)
			{
				value=0xFF;
			}

			ASSERT(value>=0 && value<= 0xFF);

			m_pBindingAVMatrixUnit->Set_DestMasterIdx(value);

		} break;
	}
}

void CDlgBindingAVMatrixProp::OnSpin3DataChanged(int iStep)
{
	ASSERT(m_pBindingAVMatrixUnit);

	const BYTE bMethodData = m_pBindingAVMatrixUnit->Get_Function();
	switch(bMethodData)
	{
		case AVMATRIX_OBJECT_ATTR_CONNECTION:
		{
			if (m_pBindingAVMatrixUnit->Get_State() & 0x01)
				m_pBindingAVMatrixUnit->Set_State(0);
			else 
				m_pBindingAVMatrixUnit->Set_State(1);

		} break;

		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE:
		{
			// Slave Idx: 0-63 of 255
			int value = m_pBindingAVMatrixUnit->Get_DestSlaveIdx();

			value+=iStep;

			if(value<0) 
			{
				value=0;
			}
			else if (value>=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS && 
				     value<0xFF)
			{
				value=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS-1;
			}
			else if (value>=CBindingAVMatrixUnit::AVMATRIX_MAX_OUTPUTS)
			{
				value=0xFF;
			}

			ASSERT(value>=0 && value<= 0xFF);

			m_pBindingAVMatrixUnit->Set_DestSlaveIdx(value);

		} break;
	}
}



