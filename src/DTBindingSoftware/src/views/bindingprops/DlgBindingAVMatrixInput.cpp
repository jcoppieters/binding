#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingAVMatrixInput.h"

#include "app\appbindings\appbindingunit\bindingunitavmatrix.h"

#include "bindingUIElements.h"

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////

CDlgBindingAVMatrixInput::CDlgBindingAVMatrixInput(CWnd* pParent /*=NULL*/) :
Base( pParent )
{
}


CDlgBindingAVMatrixInput::~CDlgBindingAVMatrixInput(void)
{
}

void CDlgBindingAVMatrixInput::InitControls(void)
{
	if (RetrieveBindingUnit())
	{
		CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE key = CBindingStringEntries_Col::AVMATRIX_UNITS;

		if (getBindingAvMatrixUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			key = CBindingStringEntries_Col::AVMATRIX_UNITS;
		}
		else
		{
			key = CBindingStringEntries_Col::AVMATRIX_INPUT_UNITS;
		}

		if (LoadStringEntryResources(key))
		{
			CBindingStringEntries_Helper Helper(getStringEntries());
			Helper.Insert(m_CmbMethod);

			m_CmbMethod.ShowWindow(TRUE);
		}
	}

	UpdateControls();
}

void CDlgBindingAVMatrixInput::UpdateControls(void)
{
	if (getBindingAvMatrixUnit()->GetBindingUnitType() != BINDING_UNIT_TYPE_AVMATRIX_INPUT ||
		getBindingAvMatrixUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
	{			
		this->Base::UpdateControls( );
	}
	else
	{				
		CBindingAVMatrixUnitInput* pAVMatrixUnit = static_cast<CBindingAVMatrixUnitInput*>(getBindingAvMatrixUnit());
		ASSERT(pAVMatrixUnit->GetBindingUnitType() == BINDING_UNIT_TYPE_AVMATRIX_INPUT);

		const BYTE bMethodData = pAVMatrixUnit->Get_Event();

		CBindingStringEntries_Helper Helper(getStringEntries());		
		Helper.SetCurrSel(m_CmbMethod,bMethodData);
			
		switch (bMethodData)
		{						
			case EV_UNITCONTROLPULS:			
			{				
				CString szEdit1;
				
				EnableControls(TRUE);

				const BYTE bMacro = pAVMatrixUnit->Get_InpMacro( );
				ASSERT( bMacro < 128 );

				szEdit1.Format(_T("macro %d"),bMacro);
				m_EditSpin1.SetWindowText(szEdit1);

			} break;		

			default :
			{							
				EnableControls(TRUE);

				CString szEdit1("Unknown event");
				m_EditSpin1.SetWindowText(szEdit1);
			
			} break;
		}	
	}	
}

void CDlgBindingAVMatrixInput::OnSpin1DataChanged(int iStep)
{	
	if (getBindingAvMatrixUnit()->GetBindingUnitType() != BINDING_UNIT_TYPE_AVMATRIX_INPUT ||
		getBindingAvMatrixUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
	{			
		this->Base::OnSpin1DataChanged( iStep );
	}
	else
	{	
		CBindingAVMatrixUnitInput* pAVMatrixUnit = static_cast<CBindingAVMatrixUnitInput*>(getBindingAvMatrixUnit());
		ASSERT(pAVMatrixUnit->GetBindingUnitType() == BINDING_UNIT_TYPE_AVMATRIX_INPUT);

		const BYTE bMethodData = pAVMatrixUnit->Get_Event();
		
		switch(bMethodData)
		{					
			case EV_UNITCONTROLPULS:				
			{			
				BYTE bMacro = pAVMatrixUnit->Get_InpMacro( );

				bMacro += iStep;

				if (iStep >0) 
				{
					if (bMacro >= CBindingAVMatrixUnitInput::MAX_INPUT_FUNCTIONS) 
						bMacro = CBindingAVMatrixUnitInput::MAX_INPUT_FUNCTIONS - 1;
				}
				else 
				{
					if (bMacro >= CBindingAVMatrixUnitInput::MAX_INPUT_FUNCTIONS) 
						bMacro = 0;
				}

				pAVMatrixUnit->Set_InpMacro( bMacro );

			} break;
		
			default:
			{
				
			} break;		
		}
	}
}

