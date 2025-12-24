// DlgBindingBoseV35Unit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingBoseV35Unit.h"
#include "Mainfrm.h"
#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitbosev35.h"

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
// CDlgBindingBoseV35Unit dialog


CDlgBindingBoseV35Unit::CDlgBindingBoseV35Unit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp( pParent ),
	m_pBindingBoseV35Unit( 0 ),
	m_pStringEntries( 0 ),
	m_pIteratorBindingFunctions( 0 )
	
{
	//{{AFX_DATA_INIT(CDlgBindingBoseV35Unit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	

/*
#ifdef _DEBUG

	const int result = CBindingBoseV35Unit_Helper::testDriver( );

#endif	
*/

}


void CDlgBindingBoseV35Unit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingBoseV35Unit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingBoseV35Unit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingBoseV35Unit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingBoseV35Unit message handlers


void CDlgBindingBoseV35Unit::InitControls(void)
{	
	m_pBindingBoseV35Unit = (CBindingBoseV35Unit*) GetBindingUnit();
	ASSERT(m_pBindingBoseV35Unit);
	
	CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE key = CBindingStringEntries_Col::BOSE_V35_OUTPUT;

	if (m_pBindingBoseV35Unit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{
		if (m_pBindingBoseV35Unit->GetUnitAddress() == BROADCAST_UNIT)
		{
			key = CBindingStringEntries_Col::BOSE_V35_OUTPUT_VIRTUALS;	
		}
		else
		{
			key = CBindingStringEntries_Col::BOSE_V35_OUTPUT;
		}
	}
	else
	{
		if (m_pBindingBoseV35Unit->GetUnitAddress() == BROADCAST_UNIT) 
		{
			key = CBindingStringEntries_Col::BOSE_V35_INPUT_VIRTUALS;
		}
		else
		{
			key = CBindingStringEntries_Col::BOSE_V35_INPUT;
		}
	}

	CBindingStringEntries_Col stringCol;
	m_pStringEntries = stringCol.Get(m_pBindingBoseV35Unit,key);
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.Insert(m_CmbMethod);
	m_CmbMethod.ShowWindow(TRUE);

	if (m_pIteratorBindingFunctions == 0)
	{
		CBindingBoseV35Unit_Helper::create_Iterator(m_pBindingBoseV35Unit->Get_Source(),
												    m_pBindingBoseV35Unit->Get_Button(),
													&m_pIteratorBindingFunctions);
	
	}

	UpdateControls();
}

void CDlgBindingBoseV35Unit::UpdateControls(void)
{
	ASSERT(m_pBindingBoseV35Unit);
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	if (m_pBindingBoseV35Unit->GetInOutType() == UNIT_TYPE_OUTPUT) 
	{	
		const BYTE bMethodData = m_pBindingBoseV35Unit->Get_Function();	

		Helper.SetCurrSel(m_CmbMethod,bMethodData);
	
		switch (bMethodData)
		{
			case BOSERS_OBJECT_ATTR_SOURCE_ON:
			{
				EnableControls(TRUE);				
												
				m_EditSpin1.SetWindowText(CBindingBoseV35Unit::Get_strSource(
					m_pBindingBoseV35Unit->Get_Source()));

			} break;			

			case BOSERS_OBJECT_METHOD_SOURCEINC:
			case BOSERS_OBJECT_METHOD_SOURCEDEC:
			case BOSERS_OBJECT_METHOD_CSON:
			case BOSERS_OBJECT_METHOD_OFF:
			case BOSERS_OBJECT_METHOD_FSCSONOFF:
			case BOSERS_OBJECT_METHOD_VOLUP:
			case BOSERS_OBJECT_METHOD_VOLDOWN:
			case BOSERS_OBJECT_METHOD_FSVOLUPDOWN:
			case BOSERS_OBJECT_METHOD_PRESUP:
			case BOSERS_OBJECT_METHOD_PRESDOWN:
			case BOSERS_OBJECT_METHOD_PLAY:
			case BOSERS_OBJECT_METHOD_STOP:
			case BOSERS_OBJECT_METHOD_PAUSE:
			case BOSERS_OBJECT_METHOD_FF:
			case BOSERS_OBJECT_METHOD_RR:
			case BOSERS_OBJECT_METHOD_FFWD:
			case BOSERS_OBJECT_METHOD_FRWD:
			{
				EnableControls();

			} break;

			case BOSERS_OBJECT_METHOD_EXTRAIR:
			{
				CString szEdit1;

				EnableControls(TRUE);

				m_EditSpin1.SetWindowText(CBindingBoseV35Unit::Get_strFunctionExtraIR(
					m_pBindingBoseV35Unit->Get_FunctionExtraIR()));
				
			} break;

			case BOSERS_OBJECT_ATTR_ONOFF:
			{
				CString szEdit1;

				EnableControls(TRUE);

				if ((m_pBindingBoseV35Unit->Get_State() & 0x01) == 0x01) 
					szEdit1.LoadString(IDS_ON);
				else 
					szEdit1.LoadString(IDS_OFF);
					
				m_EditSpin1.SetWindowText(szEdit1);

			} break;

			case BOSERS_OBJECT_METHOD_DESTINATION_FUNCTION:
			{
				CString szEdit1;
				CString szText((LPCSTR) IDS_FUNCTION);

				EnableControls(TRUE);
										
				szEdit1.Format(_T("%s %d"),szText,
										  m_pBindingBoseV35Unit->Get_DestFunction() & IRAUDIO_FUNCTION_MASK);

				m_EditSpin1.SetWindowText(szEdit1);

			} break;

			case BOSERS_OBJECT_METHOD_SOURCE_FUNCTION:
			{
				CString szEdit1;
				CString szText((LPCSTR) IDS_FUNCTION);

				EnableControls(TRUE);
					
				szEdit1.Format(_T("%s %d"),szText,
										   m_pBindingBoseV35Unit->Get_SrcFunction() & IRAUDIO_FUNCTION_MASK);
					
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
		const BYTE bMethodData = m_pBindingBoseV35Unit->Get_Event();
		Helper.SetCurrSel(m_CmbMethod,bMethodData);			

		switch (bMethodData)
		{
			case BOSEV35_EVENT_DESTON:
			case BOSEV35_EVENT_DESTOFF:		
			{
				EnableControls();

			} break;
			
			case BOSEV35_EVENT_SOURCEOFF:
			case BOSEV35_EVENT_SOURCEON:
			{		
				EnableControls(TRUE);

				BYTE bSource = m_pBindingBoseV35Unit->Get_Source();

				if (bSource < BOSEV35_SOURCE_TUNER) 
				{
					bSource = BOSEV35_SOURCE_TUNER;
					m_pBindingBoseV35Unit->Set_Source(bSource);
				}
																							
				m_EditSpin1.SetWindowText(CBindingBoseV35Unit::Get_strSource(
					m_pBindingBoseV35Unit->Get_Source()));

			} break;

			case BOSEV35_EVENT_KEYPAD:
			{									
				if (m_pIteratorBindingFunctions)
				{
					EnableControls(TRUE,TRUE);

					int source = m_pIteratorBindingFunctions->getSource( );

					m_pBindingBoseV35Unit->Set_Source(source);
					
					m_EditSpin1.SetWindowText(CBindingBoseV35Unit::Get_strSource(
						m_pBindingBoseV35Unit->Get_Source()));

					
					int temp = m_pIteratorBindingFunctions->getFunction( );

					m_pBindingBoseV35Unit->Set_Button(temp);

					m_EditSpin2.SetWindowText(CBindingBoseV35Unit::Get_strFunctionExtraIR(
						m_pBindingBoseV35Unit->Get_Button()));
				}

			} break;

			case BOSEV35_EVENT_EXTRA:
			{
				EnableControls(TRUE);

				int func = m_pBindingBoseV35Unit->Get_DestFunctionExtraIR( );

				m_EditSpin1.SetWindowText(CBindingBoseV35Unit::Get_strFunctionExtraIR(func));					

			} break;
		}
	}		
}


void CDlgBindingBoseV35Unit::OnCmbBoxDataChanged(void)
{
	ASSERT(m_pBindingBoseV35Unit);
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);

	if (m_pBindingBoseV35Unit->GetInOutType() == UNIT_TYPE_OUTPUT) 
	{		
		m_pBindingBoseV35Unit->Set_Function(bMessageCode == 0xFF ? BOSERS_OBJECT_ATTR_SOURCE_ON : bMessageCode );							
	}
	else 
	{					
		m_pBindingBoseV35Unit->Set_Event(bMessageCode == 0xFF ? BOSELF48_EVENT_KEYPAD : bMessageCode);				
	}
}

void CDlgBindingBoseV35Unit::OnSpin1DataChanged(int iStep)
{
	ASSERT(m_pBindingBoseV35Unit);

	if (m_pBindingBoseV35Unit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{	
		//OUTPUT		
		const BYTE bMethodData = m_pBindingBoseV35Unit->Get_Function();

		switch(bMethodData)
		{
			case BOSERS_OBJECT_ATTR_SOURCE_ON:
			{
				int source = m_pBindingBoseV35Unit->Get_Source() + iStep;

				if (source > BOSEV35_SOURCE_LOCAL)	
					source = BOSEV35_SOURCE_LOCAL;	
				else if (source < BOSEV35_SOURCE_TUNER)	
					source = BOSEV35_SOURCE_TUNER;
					
				m_pBindingBoseV35Unit->Set_Source((BYTE) source);


			} break;		

			case BOSERS_OBJECT_ATTR_ONOFF:
			{
				m_pBindingBoseV35Unit->Set_State((m_pBindingBoseV35Unit->Get_State()&0x01)^0x01);

			} break;

			case BOSERS_OBJECT_METHOD_DESTINATION_FUNCTION:
			{
				m_pBindingBoseV35Unit->Set_DestFunction((m_pBindingBoseV35Unit->Get_DestFunction()&0x01)^0x01);

			} break;

			case BOSERS_OBJECT_METHOD_SOURCE_FUNCTION:
			{			
				m_pBindingBoseV35Unit->Set_SrcFunction((m_pBindingBoseV35Unit->Get_SrcFunction()&0x01)^0x01);

			} break;
			
			case BOSERS_OBJECT_METHOD_EXTRAIR:
			{
				// Sedert v0643:
				// Extra functies: Rood,groen,geel & blauw.

				int func = m_pBindingBoseV35Unit->Get_FunctionExtraIR( ) + iStep;
				
				if (func > BOSEV35_SOURCE_FUNCTION_DOWN2)
					func = BOSEV35_SOURCE_FUNCTION_DOWN2;
				else if (func < BOSEV35_SOURCE_FUNCTION_KEY1)
					func = BOSEV35_SOURCE_FUNCTION_KEY1;

				m_pBindingBoseV35Unit->Set_FunctionExtraIR( func );

			} break;
		}
	}
	else 
	{
		const BYTE bMethodData = m_pBindingBoseV35Unit->Get_Event();

		// Individual Rooms ...
		switch(bMethodData) 
		{
			case BOSELF48_EVENT_SOURCEOFF:
			case BOSELF48_EVENT_SOURCEON:		
			{
				int source = m_pBindingBoseV35Unit->Get_Source() + iStep;

				if (source > BOSEV35_SOURCE_LOCAL)	
					source = BOSEV35_SOURCE_LOCAL;	
				else if (source < BOSEV35_SOURCE_TUNER)	
					source = BOSEV35_SOURCE_TUNER;
					
				m_pBindingBoseV35Unit->Set_Source((BYTE) source);

			} break;	

			case BOSEV35_EVENT_KEYPAD:
			{
				if (m_pIteratorBindingFunctions)
				{
					if (iStep>0)
					{
						m_pIteratorBindingFunctions->incSource( );
					}
					else
					{
						m_pIteratorBindingFunctions->decSource( );
					}

					m_pBindingBoseV35Unit->Set_Source((BYTE) m_pIteratorBindingFunctions->getSource());
				}

			} break;	
			
			case BOSEV35_EVENT_EXTRA:
			{
				int func = m_pBindingBoseV35Unit->Get_DestFunctionExtraIR() + iStep;
				
				if (iStep>0)
				{
					if (func >= BOSEV35_DEST_FUNCTION_TV_INPUT)
						func = BOSEV35_DEST_FUNCTION_TV_INPUT;
				}
				else
				{
					if (func <= BOSEV35_SOURCE_FUNCTION_LAST_CHANNEL)
						func = BOSEV35_SOURCE_FUNCTION_LAST_CHANNEL;
				}

				m_pBindingBoseV35Unit->Set_DestFunctionExtraIR( func );			

			} break;
		}			
	}
}

void CDlgBindingBoseV35Unit::OnSpin2DataChanged(int iStep)
{
	ASSERT(m_pBindingBoseV35Unit);

	if (m_pBindingBoseV35Unit->GetInOutType() == UNIT_TYPE_INPUT) 
	{	
		const BYTE bMethodData = m_pBindingBoseV35Unit->Get_Event();

		switch (bMethodData)
		{	
			case BOSELF48_EVENT_KEYPAD:
			{	
				if (m_pIteratorBindingFunctions)
				{
					if (iStep>0)
					{
						m_pIteratorBindingFunctions->incFunction( );
					}
					else
					{
						m_pIteratorBindingFunctions->decFunction( );
					}

					m_pBindingBoseV35Unit->Set_Button((BYTE) m_pIteratorBindingFunctions->getFunction());
				}				

			} break;

			default:
				ASSERT(0);
			break;
		}
	}
}


void CDlgBindingBoseV35Unit::OnSpin3DataChanged(int iStep)
{
	ASSERT(m_pBindingBoseV35Unit);


}

void CDlgBindingBoseV35Unit::PostNcDestroy() 
{
	CBindingBoseV35Unit_Helper::delete_Iterator( m_pIteratorBindingFunctions );
	CDlgBindingUnitProp::PostNcDestroy();
}


