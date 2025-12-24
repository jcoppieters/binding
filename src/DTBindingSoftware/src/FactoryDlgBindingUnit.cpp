#include "stdafx.h"
#include "BindingSoftware.h"
#include "factorydlgbindingunit.h"

#include "views/bindingprops/DlgBindingVideomuxProp.h"
#include "views/bindingprops/DlgBindingAVMatrixProp.h"
#include "views/bindingprops/DlgBindingAVMatrixInput.h"		// TM_AVMATRIX_INPUT
#include "views/bindingprops/DlgBindingAlarmProp.h"
#include "views/bindingprops/DlgBindingAudioHC4Unit.h"
#include "views/bindingprops/DlgBindingAudioHC4BnoUnit.h"		// TM_AUDIOHC4_BNO
#include "views/bindingprops/DlgBindingBoseLf48Unit.h"
#include "views/bindingprops/DlgBindingUnitprop.h"
#include "views/bindingprops/DlgBindingDimmerProp.h"
#include "views/bindingprops/DlgBindingDimmerInputProp.h"
#include "views/bindingprops/DlgBindingSwitchUnit.h"
#include "views/bindingprops/dlgbindingswitchunitinput.h"
#include "views/bindingprops/DlgBindingControlUnit.h"
#include "views/bindingprops/DlgBindingVirtualUnit.h"
#include "views/bindingprops/DlgBindingSensUnit.h"
#include "views/bindingprops/DlgBindingDuoSwitchUnit.h"
#include "views/bindingprops/dlgbindingduoswitchunitinput.h"
#include "views/bindingprops/DlgBindingIRTXUnit.h"
#include "views/bindingprops/DlgBindingBoseRsUnit.h"
#include "views/bindingprops/DlgBindingAudioBasicUnit.h"
#include "views/bindingprops/DlgBindingRC5Unit.h"
#include "views/bindingprops/DlgBindingBoseV35Unit.h"
#include "views/bindingprops/DlgBindingOperator.h"

#include "app\appbindings\appbindingunit\bindingunit.h"


CDlgBindingUnitProp* CFactoryDlgBindingUnit::Create(
	const CBindingUnit *pBindingUnit)
{
	CDlgBindingUnitProp* m_pBindingDlgUnit = 0;
	const BYTE bBindingUnitType = pBindingUnit->GetBindingUnitType();

	switch(bBindingUnitType)
	{
		case BINDING_UNIT_TYPE_DIM: 
			m_pBindingDlgUnit = new CDlgBindingDimmerProp();
		break;
		case BINDING_UNIT_TYPE_DIM_FWD: 
			m_pBindingDlgUnit = new CDlgBindingDimmerInputProp();
		break;
		case BINDING_UNIT_TYPE_SWITCH: 
			m_pBindingDlgUnit = new CDlgBindingSwitchUnit();
		break;
		case BINDING_UNIT_TYPE_SWITCH_FWD: 
			m_pBindingDlgUnit = new CDlgBindingSwitchUnitInput();
		break;
		case BINDING_UNIT_TYPE_DUOSWITCH: 
			m_pBindingDlgUnit = new CDlgBindingDuoSwitchUnit();
		break;
		case BINDING_UNIT_TYPE_DUOSWITCH_FWD: 
			m_pBindingDlgUnit = new CDlgBindingDuoSwitchUnitInput();
		break;
		case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 - TODO */
			m_pBindingDlgUnit = new CDlgBindingSensUnit();
		break;
		case BINDING_UNIT_TYPE_SENS: 
			m_pBindingDlgUnit = new CDlgBindingSensUnit();
		break;
		case BINDING_UNIT_TYPE_CONTROL: 
			m_pBindingDlgUnit = new CDlgBindingControlUnit();
		break;
		case BINDING_UNIT_TYPE_LCD_VIRTUAL: 
			m_pBindingDlgUnit = new CDlgBindingVirtualUnit();
		break;
		case BINDING_UNIT_TYPE_IRTX: 
			m_pBindingDlgUnit = new CDlgBindingIRTXUnit(); 
		break;		
		case BINDING_UNIT_TYPE_BOSERS: 
			m_pBindingDlgUnit = new CDlgBindingBoseRsUnit(); 
		break;
		case BINDING_UNIT_TYPE_AUDIO: 
			m_pBindingDlgUnit = new CDlgBindingAudioBasicUnit();
		break;
		case BINDING_UNIT_TYPE_RC5RX: 
			m_pBindingDlgUnit = new CDlgBindingRC5Unit();
		break;
		case BINDING_UNIT_TYPE_VIDEOMUX : 
			m_pBindingDlgUnit = new CDlgBindingVideomuxProp();
		break;
		case BINDING_UNIT_TYPE_AVMATRIX : 
			m_pBindingDlgUnit = new CDlgBindingAVMatrixProp();; 
		break;
		case BINDING_UNIT_TYPE_AVMATRIX_INPUT : 
			m_pBindingDlgUnit = new CDlgBindingAVMatrixInput();		// TM_AVMATRIX_INPUT
		break;
		case BINDING_UNIT_TYPE_ALARM_GALAXY : 
			m_pBindingDlgUnit = new CDlgBindingAlarmPropGalaxy();	// TM_CHANGES_ALARM_ARITECH:
		break;
		case BINDING_UNIT_TYPE_ALARM_ARITECH:			
			m_pBindingDlgUnit = new CDlgBindingAlarmPropAritech();	// TM_CHANGES_ALARM_ARITECH:
		break;
		case BINDING_UNIT_TYPE_ALARM_ANB:			
			m_pBindingDlgUnit = new CDlgBindingAlarmPropAnbBibus();	// TM_CHANGES_ALARM_ANB
		break;
		case BINDING_UNIT_TYPE_ALARM_TEXECOM:			
			m_pBindingDlgUnit = new CDlgBindingAlarmPropTexecom();	// TM_CHANGES_ALARM_TEXECOM
		break;
		case BINDING_UNIT_TYPE_ALARM_INTEGRA:			
			m_pBindingDlgUnit = new CDlgBindingAlarmPropIntegra();	// TM_CHANGES_ALARM_INTEGRA
		break;
		case BINDING_UNIT_TYPE_AUDIO_HC4: 
			m_pBindingDlgUnit = new CDlgBindingAudioHC4Unit();; 
		break;
		case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:						/* TM_AUDIOHC4_BNO */
			m_pBindingDlgUnit = new CDlgBindingAudioHC4BnoUnit();
		break;
		case BINDING_UNIT_TYPE_BOSELF48:
			m_pBindingDlgUnit = new CDlgBindingBoseLf48Unit();
		break;
		case BINDING_UNIT_TYPE_BOSEV35:
			m_pBindingDlgUnit = new CDlgBindingBoseV35Unit();
		break;
		default: 
			m_pBindingDlgUnit = new CDlgBindingUnitProp();
		break;
	}
	
	ASSERT(0!=m_pBindingDlgUnit);
	return m_pBindingDlgUnit;
}
