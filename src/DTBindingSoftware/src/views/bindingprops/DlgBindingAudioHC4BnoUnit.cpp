#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingAudioHC4BnoUnit.h"

#include "app\appbindings\appbindingunit\bindingunitaudiohc4.h"

#include "bindingUIElements.h"

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////

CDlgBindingAudioHC4BnoUnit::CDlgBindingAudioHC4BnoUnit(CWnd* pParent /*=NULL*/) :
CDlgBindingAudioHC4Unit( pParent )
{
}

CDlgBindingAudioHC4BnoUnit::~CDlgBindingAudioHC4BnoUnit(void)
{
}


void CDlgBindingAudioHC4BnoUnit::InitControls(void)
{
	if (RetrieveBindingUnit())
	{
		CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE key = CBindingStringEntries_Col::MATRIXAUDIO_UNITS;

		if (getBindingHC4AudioUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			key = CBindingStringEntries_Col::MATRIXAUDIO_UNITS;
		}
		else
		{
			key = CBindingStringEntries_Col::MATRIXAUDIOBNO_INPUT_UNITS;
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


void CDlgBindingAudioHC4BnoUnit::UpdateControls(void)
{
	if (getBindingHC4AudioUnit()->GetInOutType() == UNIT_TYPE_INPUT &&
		getBindingHC4AudioUnit()->GetBindingUnitType( ) == BINDING_UNIT_TYPE_AUDIO_HC4_BNO)
	{		
		CBindingHC4AudioBnoUnit* const pBindingHC4AudioBnoUnit =
			static_cast<CBindingHC4AudioBnoUnit*>(getBindingHC4AudioUnit());

		CBindingStringEntries_Helper Helper(getStringEntries());		
		const BYTE bMethodData = pBindingHC4AudioBnoUnit->Get_Event();

		Helper.SetCurrSel(m_CmbMethod,bMethodData);
	
		switch (bMethodData)
		{						
			case EV_UNITCONTROLPULS:			
			{				
				CString szEdit1;
				
				EnableControls(TRUE);

				const BYTE bMacro = pBindingHC4AudioBnoUnit->Get_InpMacro( );				
				for (int i=0;i<CBindingHC4AudioBnoUnit::MAX_AUDIO_HC4_BNO_INPUT_MACROS;i++)
				{
					if (CBindingHC4AudioBnoUnit::tableMacroBeo4Events[i].bDTFunction == bMacro)
					{
						szEdit1.Format(_T("%s"),CBindingHC4AudioBnoUnit::tableMacroBeo4Events[i].szName);
						break;
					}
				}

				if (szEdit1.IsEmpty( ))
				{
					// Dit kan voorkomen wanneer binding werd aangemaakt via normale
					// Audio HC4 unit en nu geconverteerd wordt naar een Bno audio unit.

					szEdit1.Format("Undefined macro %d",bMacro);	
				}
				

				m_EditSpin1.SetWindowText(szEdit1);

			} break;
				
			default :
			{			
				// Dit kan voorkomen wanneer binding werd aangemaakt via normale
				// Audio HC4 unit en nu geconverteerd wordt naar een Bno audio unit.

				EnableControls(TRUE);

				CString szEdit1("Unknown event");
				m_EditSpin1.SetWindowText(szEdit1);

				// ASSERT(0);

			} break;
		}
	}
	else
	{
		this->Base::UpdateControls( );	
	}
}

void CDlgBindingAudioHC4BnoUnit::OnSpin1DataChanged(int iStep)
{	
	if (getBindingHC4AudioUnit()->GetInOutType() == UNIT_TYPE_INPUT &&
		getBindingHC4AudioUnit()->GetBindingUnitType( ) == BINDING_UNIT_TYPE_AUDIO_HC4_BNO)
	{		
		CBindingHC4AudioBnoUnit* const pBindingHC4AudioBnoUnit =
			static_cast<CBindingHC4AudioBnoUnit*>(getBindingHC4AudioUnit());
		
		const BYTE bMethodData = pBindingHC4AudioBnoUnit->Get_Event();
		
		switch(bMethodData)
		{	
			case EV_UNITCONTROLPULSTOGGLE:
			case EV_UNITCONTROLPULS:
			case EV_UNITCONTROLTOGGLE:			
			{			
				BYTE bMacro = pBindingHC4AudioBnoUnit->Get_InpMacro( );

				bMacro += iStep;

				if (iStep >0) 
				{
					if (bMacro >= CBindingHC4AudioBnoUnit::MAX_AUDIO_HC4_BNO_INPUT_MACROS) 
						bMacro = CBindingHC4AudioBnoUnit::MAX_AUDIO_HC4_BNO_INPUT_MACROS - 1;
				}
				else 
				{
					if (bMacro >= CBindingHC4AudioBnoUnit::MAX_AUDIO_HC4_BNO_INPUT_MACROS) 
						bMacro = 0;
				}

				pBindingHC4AudioBnoUnit->Set_InpMacro( bMacro );

			} break;

			default:
			{
				ASSERT( 0 );

			} break;
		}
	}
	else
	{
		this->Base::OnSpin1DataChanged( iStep );
	}
}