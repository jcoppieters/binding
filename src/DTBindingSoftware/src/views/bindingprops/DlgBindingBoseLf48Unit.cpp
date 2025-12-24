// DlgBindingBoseLf48Unit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingBoseLf48Unit.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitboself48.h"


#include "bindingUIElements.h"


#define BROADCAST_UNIT	0xFF


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingBoseLf48Unit dialog


CDlgBindingBoseLf48Unit::CDlgBindingBoseLf48Unit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingBoseLf48Unit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pBindingBoseLf48Unit = 0;
	m_pStringEntries = 0;
}


void CDlgBindingBoseLf48Unit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingBoseLf48Unit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingBoseLf48Unit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingBoseLf48Unit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingBoseLf48Unit message handlers
void CDlgBindingBoseLf48Unit::InitControls(void)
{	
	pBindingBoseLf48Unit = (CBindingBoseLf48Unit*) GetBindingUnit();
	ASSERT(pBindingBoseLf48Unit);

	CBindingStringEntries_Col stringCol;
	if (pBindingBoseLf48Unit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{
		m_pStringEntries = stringCol.Get(pBindingBoseLf48Unit,CBindingStringEntries_Col::BOSE_LF48_OUTPUT);
	}
	else
	{
		if (pBindingBoseLf48Unit->GetUnitAddress() == BROADCAST_UNIT) 
		{
			m_pStringEntries = stringCol.Get(pBindingBoseLf48Unit,CBindingStringEntries_Col::BOSE_LF48_INPUT_VIRTUALS);
		}
		else
		{
			m_pStringEntries = stringCol.Get(pBindingBoseLf48Unit,CBindingStringEntries_Col::BOSE_LF48_INPUT);
		}
	}

	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.Insert(m_CmbMethod);
	m_CmbMethod.ShowWindow(TRUE);

	UpdateControls();
}

void CDlgBindingBoseLf48Unit::UpdateControls(void)
{
	ASSERT(pBindingBoseLf48Unit);
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	if (pBindingBoseLf48Unit->GetInOutType() == UNIT_TYPE_OUTPUT) 
	{	
		const BYTE bMethodData = pBindingBoseLf48Unit->Get_Function();	
		Helper.SetCurrSel(m_CmbMethod,bMethodData);
	
		switch (bMethodData)
		{
			case AUDIO_OBJECT_ATTR_SOURCE_ON:
			{
				EnableControls(TRUE);				
												
				m_EditSpin1.SetWindowText(pBindingBoseLf48Unit->Get_strSource());

			} break;			

			case AUDIO_OBJECT_METHOD_SOURCEINC:
			case AUDIO_OBJECT_METHOD_SOURCEDEC:
			case AUDIO_OBJECT_METHOD_CSON:
			case AUDIO_OBJECT_METHOD_OFF:
			case AUDIO_OBJECT_METHOD_FSCSONOFF:
			case AUDIO_OBJECT_METHOD_VOLUP:
			case AUDIO_OBJECT_METHOD_VOLDOWN:
			case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
			case AUDIO_OBJECT_METHOD_PRESUP:
			case AUDIO_OBJECT_METHOD_PRESDOWN:
			{
				EnableControls();

			} break;

			case AUDIO_OBJECT_ATTR_ONOFF:
			{
				CString szEdit1;

				EnableControls(TRUE);

				if ((pBindingBoseLf48Unit->Get_State() & 0x01) == 0x01) 
					szEdit1.LoadString(IDS_ON);
				else 
					szEdit1.LoadString(IDS_OFF);
					
				m_EditSpin1.SetWindowText(szEdit1);

			} break;

			case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
			{
				CString szEdit1;
				CString szText((LPCSTR) IDS_FUNCTION);

				EnableControls(TRUE);
										
				szEdit1.Format(_T("%s %d"),szText,
										  pBindingBoseLf48Unit->Get_DestFunction() & IRAUDIO_FUNCTION_MASK);

				m_EditSpin1.SetWindowText(szEdit1);

			} break;

			case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
			{
				CString szEdit1;
				CString szText((LPCSTR) IDS_FUNCTION);

				EnableControls(TRUE);
					
				szEdit1.Format(_T("%s %d"),szText,
										   pBindingBoseLf48Unit->Get_SrcFunction() & IRAUDIO_FUNCTION_MASK);
					
				m_EditSpin1.SetWindowText(szEdit1);

			} break;

			default:
			{
				EnableControls();

			} break;
		}		
	}
	else 
	{	
		const BYTE bMethodData = pBindingBoseLf48Unit->Get_Event();
		Helper.SetCurrSel(m_CmbMethod,bMethodData);			

		switch (bMethodData)
		{
			case BOSELF48_EVENT_DESTON:
			case BOSELF48_EVENT_DESTOFF:
			case BOSELF48_EVENT_INPUTOFF:
			case BOSELF48_EVENT_INPUTON:
			{
				EnableControls();

			} break;
			
			case BOSELF48_EVENT_SOURCEOFF:
			case BOSELF48_EVENT_SOURCEON:
			{		
				EnableControls(TRUE);

				BYTE bSource = pBindingBoseLf48Unit->Get_Source();

				if (bSource < BOSELF48_SOURCE_DVD) 
				{
					bSource = BOSELF48_SOURCE_DVD;
					pBindingBoseLf48Unit->Set_Source(bSource);
				}
										
				m_EditSpin1.SetWindowText(CString((LPCSTR)(IDS_BOSELF_TUNER + bSource)));

			} break;

			case BOSELF48_EVENT_KEYPAD:
			{					
				EnableControls(TRUE,TRUE,TRUE);

				BYTE bSource = pBindingBoseLf48Unit->Get_Source();

				if (bSource < BOSELF48_SOURCE_AUX) 
				{
					bSource = BOSELF48_SOURCE_AUX;
					pBindingBoseLf48Unit->Set_Source(bSource);
				}
					
				m_EditSpin1.SetWindowText(CString((LPCSTR)(IDS_BOSELF_TUNER + bSource)));

				// Edit spin 2 : Buttons 0 ... 9
				
				BYTE bTemp = pBindingBoseLf48Unit->Get_Button();

				if (bTemp > BOSELF48_SOURCE_FUNCTION_KPENTER) 
				{
					bTemp = BOSELF48_SOURCE_FUNCTION_KPENTER;
					pBindingBoseLf48Unit->Set_Button(bTemp);
				}
			
				m_EditSpin2.SetWindowText(CString((LPCSTR)(IDS_BOSELF_KEY1 + bTemp)));

				// TODO KEYPAD  3	!!!!!!!!!!!!!!!!!
				CString szEdit3;
				bTemp = pBindingBoseLf48Unit->Get_ControlEvent();
					
				if (bTemp == EV_UNITCONTROLTOGGLE)
					szEdit3.LoadString(IDS_BOSELF_CONTROLTOGGLE);
				else 
					szEdit3.LoadString(IDS_BOSELF_CONTROLPULS);

				m_EditSpin3.SetWindowText(szEdit3);

			} break;

			case BOSELF48_EVENT_TPKEYS:
			{
				EnableControls(TRUE,TRUE);

				BYTE bSource = pBindingBoseLf48Unit->Get_Source();

				if (bSource < BOSELF48_SOURCE_AUX) 
				{
					bSource = BOSELF48_SOURCE_AUX;
					pBindingBoseLf48Unit->Set_Source(bSource);
				}									

				m_EditSpin1.SetWindowText(CString((LPCSTR)(IDS_BOSELF_TUNER + bSource)));

				// Edit spin 2 : TK Functions ...
				BYTE bTemp = pBindingBoseLf48Unit->Get_TKFunction();
				if (bTemp >= BOSELF48_SOURCE_FUNCTION_STOP) 
				{
					bTemp = BOSELF48_SOURCE_FUNCTION_STOP;
					pBindingBoseLf48Unit->Set_TKFunction(bTemp);
				}
				else if ((bTemp == BOSELF48_SOURCE_FUNCTION_ON) ||
						 (bTemp == BOSELF48_SOURCE_FUNCTION_OFF))
				{
					bTemp = BOSELF48_SOURCE_FUNCTION_STOP;
					pBindingBoseLf48Unit->Set_TKFunction(bTemp);
				}
			
				m_EditSpin2.SetWindowText(CString((LPCSTR) IDS_BOSELF_PLAY + 
												  (bTemp - BOSELF48_SOURCE_FUNCTION_PLAY)) );

			} break;
		}
	}		
}


// TM 20060418
void CDlgBindingBoseLf48Unit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingBoseLf48Unit);
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);

	if (pBindingBoseLf48Unit->GetInOutType() == UNIT_TYPE_OUTPUT) 
	{
		if (bMessageCode == 0xFF)	
			pBindingBoseLf48Unit->Set_Function(AUDIO_OBJECT_ATTR_SOURCE_ON);	//AUDIO_OBJECT_METHOD_FSVOLUPDOWN
		else		
			pBindingBoseLf48Unit->Set_Function(bMessageCode);		
	}
	else 
	{		
		if (bMessageCode == 0xFF)			
			pBindingBoseLf48Unit->Set_Event(BOSELF48_EVENT_KEYPAD);			
		else			
			pBindingBoseLf48Unit->Set_Event(bMessageCode);		
/*
		if (pBindingBoseLf48Unit->GetUnitAddress() == BROADCAST_UNIT) 
		{				
			if (bMessageCode == 0xFF)			
				pBindingBoseLf48Unit->Set_Event(BOSELF48_EVENT_SOURCEOFF);			
			else			
				pBindingBoseLf48Unit->Set_Event(bMessageCode);			
		}
		else 
		{			
			if (bMessageCode == 0xFF)		
				pBindingBoseLf48Unit->Set_Event(BOSELF48_EVENT_DESTOFF);			
			else			
				pBindingBoseLf48Unit->Set_Event(bMessageCode);			
		}
*/
	}
}


void CDlgBindingBoseLf48Unit::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingBoseLf48Unit);

	if (pBindingBoseLf48Unit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{	
		//OUTPUT		
		const BYTE bMethodData = pBindingBoseLf48Unit->Get_Function();

		switch(bMethodData)
		{
			case AUDIO_OBJECT_ATTR_SOURCE_ON:
			{
				BYTE bSource = pBindingBoseLf48Unit->Get_Source();

				if (iStep > 0) 
				{
					bSource++;

					if (bSource >= MAX_BOSE48_SOURCES) 
						bSource = MAX_BOSE48_SOURCES-1;
				}
				else 
				{
					bSource--;
					
					if (bSource >= MAX_BOSE48_SOURCES) 
						bSource = 0;
				}

				pBindingBoseLf48Unit->Set_Source(bSource);

			} break;		

			case AUDIO_OBJECT_ATTR_ONOFF:
			{
				pBindingBoseLf48Unit->Set_State((pBindingBoseLf48Unit->Get_State()&0x01)^0x01);

			} break;

			case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
			{
				pBindingBoseLf48Unit->Set_DestFunction((pBindingBoseLf48Unit->Get_DestFunction()&0x01)^0x01);

			} break;

			case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
			{			
				pBindingBoseLf48Unit->Set_SrcFunction((pBindingBoseLf48Unit->Get_SrcFunction()&0x01)^0x01);

			} break;		
		}
	}
	else 
	{
		const BYTE bMethodData = pBindingBoseLf48Unit->Get_Event();

		// Individual Rooms ...
		switch(bMethodData) 
		{
			case BOSELF48_EVENT_SOURCEOFF:
			case BOSELF48_EVENT_SOURCEON:		
			{
				BYTE bSource = pBindingBoseLf48Unit->Get_Source() + iStep;	
				
				if (iStep >0) 
				{
					if (bSource >= BOSELF48_SOURCE_LOCAL)	
						bSource = BOSELF48_SOURCE_LOCAL;
				}
				else 
				{
					if (bSource == BOSELF48_SOURCE_TUNER) 
						bSource = BOSELF48_SOURCE_DVD;
				}
				
				pBindingBoseLf48Unit->Set_Source(bSource);

			} break;

			case BOSELF48_EVENT_KEYPAD:
			case BOSELF48_EVENT_TPKEYS:
			{				
				BYTE bSource = pBindingBoseLf48Unit->Get_Source() + iStep;

				if (iStep >0) 
				{
					if (bSource >= BOSELF48_SOURCE_LOCAL)	
						bSource = BOSELF48_SOURCE_LOCAL;
				}
				else 
				{
					if (bSource <= BOSELF48_SOURCE_DVD) 
						bSource = BOSELF48_SOURCE_AUX;
				}
				
				pBindingBoseLf48Unit->Set_Source(bSource);

			} break;
		}			
	}
}

void CDlgBindingBoseLf48Unit::OnSpin2DataChanged(int iStep)
{
	ASSERT(pBindingBoseLf48Unit);

	if (pBindingBoseLf48Unit->GetInOutType() == UNIT_TYPE_INPUT) 
	{	
		const BYTE bMethodData = pBindingBoseLf48Unit->Get_Event();

		switch (bMethodData)
		{
			case BOSELF48_EVENT_KEYPAD:
			{				
				BYTE bTemp = pBindingBoseLf48Unit->Get_Button() + iStep;			

				if (iStep >0) 
				{
					if (bTemp >= BOSELF48_SOURCE_FUNCTION_KPENTER)
						bTemp = BOSELF48_SOURCE_FUNCTION_KPENTER;
				}
				else 
				{
					if (bTemp > BOSELF48_SOURCE_FUNCTION_KPENTER)
						bTemp = BOSELF48_SOURCE_FUNCTION_KEY1;
				}
				
				pBindingBoseLf48Unit->Set_Button(bTemp);

			} break;

			case BOSELF48_EVENT_TPKEYS:
			{
				BYTE bTemp = pBindingBoseLf48Unit->Get_TKFunction() + iStep;			
			
				if (iStep > 0) 
				{
					if (bTemp >= BOSELF48_SOURCE_FUNCTION_STOP) 
						bTemp = BOSELF48_SOURCE_FUNCTION_STOP;

					if ((bTemp == BOSELF48_SOURCE_FUNCTION_ON) || (bTemp == BOSELF48_SOURCE_FUNCTION_OFF)) 
						bTemp = BOSELF48_SOURCE_FUNCTION_PREV;
				}
				else 
				{					
					if (bTemp <= BOSELF48_SOURCE_FUNCTION_PLAY) 
						bTemp = BOSELF48_SOURCE_FUNCTION_PLAY;

					if ((bTemp == BOSELF48_SOURCE_FUNCTION_ON) || (bTemp == BOSELF48_SOURCE_FUNCTION_OFF)) 
						bTemp = BOSELF48_SOURCE_FUNCTION_INC;				
				}
	
				pBindingBoseLf48Unit->Set_TKFunction(bTemp);

			} break;		

			default:
				ASSERT(0);
			break;
		}
	}
}


void CDlgBindingBoseLf48Unit::OnSpin3DataChanged(int iStep)
{
	ASSERT(pBindingBoseLf48Unit);

	if (pBindingBoseLf48Unit->GetInOutType() == UNIT_TYPE_INPUT) 
	{	
		const BYTE bMethodData = pBindingBoseLf48Unit->Get_Event();

		switch (bMethodData)
		{
			case BOSELF48_EVENT_KEYPAD:
			{
				
				BYTE bTemp = pBindingBoseLf48Unit->Get_ControlEvent();

				if (bTemp == EV_UNITCONTROLTOGGLE) 
					bTemp = EV_UNITCONTROLPULS;
				else 
					bTemp = EV_UNITCONTROLTOGGLE;

				pBindingBoseLf48Unit->Set_ControlEvent(bTemp);

			} break;
		}
	}
}


