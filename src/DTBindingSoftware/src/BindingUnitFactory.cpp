// BindingUnitFactory.cpp: implementation of the CBindingUnitFactory class.
//
// TM_CHANGES_ALARM_ARITECH:
// In de contructor van CBindingAlarmUnit moet het maximum aantal inputs
// meegegeven worden .
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingUnitFactory.h"

#include "NodeDataBaseoperations.h"

#include "CanNode.h"
#include "BindingConstants.h"
#include "BindingUnitOperations.h"

#include "app\appbindings\appbindingunit\bindingunit_api.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingUnit* CBindingUnitFactory::Create()
{
	CBindingUnit* pNew = NULL;
	try
	{
		pNew = new CBindingUnit();
		return pNew;
	}
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();	
		ASSERT(0);
	}
	return NULL;
}


// Sedert versie 0x0B01:
// pointer naar STRUCT_UNIT meegeven ipv bBindingUnitType.
CBindingUnit* CBindingUnitFactory::Create(STRUCT_UNIT* pUnit)
{		
	CBindingUnit *pNew = NULL;
	CNodeDatabaseOperations operations(TMGetDocumentNodes());
	CCanNode* const pNode = operations.Find(pUnit->bNodeLogicalAddress);
	if (0!=pNode) {
		const BYTE bBindingUnitType = CBindingUnitOperations::ConvertType(pNode,pUnit);
		// Kunnen hier de product code van een node opvragen ifv de unit...
		// NodeConfigInfo ??
		try
		{			
			switch (bBindingUnitType)
			{
				case BINDING_UNIT_TYPE_DIM:	
					pNew = new CBindingDimmerUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_DIM_FWD:	
					pNew = new CBindingDimmerUnitInput(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_SWITCH: 
					pNew = new CBindingRelaisUnit(bBindingUnitType); 
				break;		
				case BINDING_UNIT_TYPE_SWITCH_FWD: 
					pNew = new CBindingRelaisUnitInput(bBindingUnitType); 
				break;		
				case BINDING_UNIT_TYPE_DUOSWITCH: 
					pNew = new CBindingDuoSwitchUnit(bBindingUnitType);
				break;		
				case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
					pNew = new CBindingDuoSwitchUnitInput(bBindingUnitType);
				break;
				case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */				
					pNew = new CBindingSensUnivUnit(bBindingUnitType);
				break;
				case BINDING_UNIT_TYPE_SENS: 
					pNew = new CBindingSensUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_CONTROL: 
					pNew = new CBindingControlUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_LCD_VIRTUAL: 
					pNew = new CBindingVirtualUnit(bBindingUnitType); 
				break;		
				case BINDING_UNIT_TYPE_AUDIO: 
					pNew = new CBindingBasicAudioUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_BOSERS: 		
					pNew = new CBindingBoseRsUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_IRTX: 
					pNew = new CBindingIRTXUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_RC5RX: 
					pNew = new CBindingRC5RXUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_AVMATRIX: 
					pNew = new CBindingAVMatrixUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
					pNew = new CBindingAVMatrixUnitInput(bBindingUnitType);
				break;
				case BINDING_UNIT_TYPE_VIDEOMUX: 
					pNew = new CBindingVideomuxUnit(bBindingUnitType);	
				break;		
				case BINDING_UNIT_TYPE_ALARM_GALAXY: 
					pNew = new CBindingAlarmUnit(bBindingUnitType,ALARM_GALAXY_MAX_INPUTS);		// TM_CHANGES_ALARM_ARITECH
				break;
				case BINDING_UNIT_TYPE_ALARM_ARITECH:
					pNew = new CBindingAlarmUnit(bBindingUnitType,ALARM_ARITECH_MAX_INPUTS);	// TM_CHANGES_ALARM_ARITECH
				break;
				case BINDING_UNIT_TYPE_ALARM_ANB:
					pNew = new CBindingAlarmUnit(bBindingUnitType,ALARM_ANB_MAX_INPUTS);		// TM_CHANGES_ALARM_ANB
				break;
				case BINDING_UNIT_TYPE_ALARM_TEXECOM:
					pNew = new CBindingAlarmUnit(bBindingUnitType,ALARM_TEXECOM_MAX_INPUTS);	// TM_CHANGES_ALARM_TEXECOM
				break;
				case BINDING_UNIT_TYPE_ALARM_INTEGRA:
					pNew = new CBindingAlarmUnit(bBindingUnitType,ALARM_INTEGRA_MAX_INPUTS);	// TM_CHANGES_ALARM_INTEGRA
				break;				
				case BINDING_UNIT_TYPE_AUDIO_HC4: 		
					pNew = new CBindingHC4AudioUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
					pNew = new CBindingHC4AudioBnoUnit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_BOSELF48:
					pNew = new CBindingBoseLf48Unit(bBindingUnitType); 
				break;
				case BINDING_UNIT_TYPE_BOSEV35:			
					pNew = new CBindingBoseV35Unit(bBindingUnitType); 
				break;
				default: 
					pNew = new CBindingUnit(bBindingUnitType); 
				break;
			}		

			// Extra vlaggen ifv het type.
			INVARIANT(NULL != pNew);
			if ((NULL !=pNew) && (NULL != pUnit)) {
				unsigned long applicationFlags = 0;
				if ((NULL != pUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pUnit->m_pAppUnitData->getUnitDataType())) {	
					applicationFlags |= CBindingUnit::APPFLAG_DALI_BINDING_UNIT;
				}						
				pNew->SetApplicationFlags(applicationFlags);		
			}

		}
		catch(CMemoryException *e) {
			e->ReportError();
			e->Delete();
			ASSERT(0);
			return NULL;
		}
	}
	return pNew;
}

/**
 * CBindingUnit* CBindingUnitFactory::Clone(CBindingUnit *const pBindingUnit)
 *
 * Clone Derived classes of CBindingUnit with base class CBindingUnit.
 * --> Custom copy constructor for derived classes can be invoked ...
 *
 * Oppassen: Niet alle members worden gekopieerd !
 */
CBindingUnit* CBindingUnitFactory::Clone(const CBindingUnit *const pBindingUnit)
{
	CBindingUnit *pNew = NULL;
	
	try
	{
		if (pBindingUnit != NULL)
		{
			ASSERT(pBindingUnit->GetNodeAddress() != 0xFF);

			const BYTE bType = pBindingUnit->GetBindingUnitType();

			switch (bType)
			{
				case BINDING_UNIT_TYPE_DIM:	
				{				
					pNew = new CBindingDimmerUnit(*pBindingUnit);				
				} break;
				case BINDING_UNIT_TYPE_DIM_FWD:	
				{
					pNew = new CBindingDimmerUnitInput(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_SWITCH: 
				{				
					pNew = new CBindingRelaisUnit(*pBindingUnit); 
				} break;		
				case BINDING_UNIT_TYPE_SWITCH_FWD: 
				{
					pNew = new CBindingRelaisUnitInput(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_DUOSWITCH: 
				{				
					pNew = new CBindingDuoSwitchUnit(*pBindingUnit);
				} break;
				case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
				{
					pNew = new CBindingDuoSwitchUnitInput(*pBindingUnit);
				} break;
				case BINDING_UNIT_TYPE_SENSUNIV:	
				{
					pNew = new CBindingSensUnivUnit(*pBindingUnit);
				} break;
				case BINDING_UNIT_TYPE_SENS: 
				{				
					pNew = new CBindingSensUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_CONTROL: 
				{				
					pNew = new CBindingControlUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_LCD_VIRTUAL: 
				{				
					pNew = new CBindingVirtualUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_AUDIO: 
				{					
					pNew = new CBindingBasicAudioUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_BOSERS: 		
				{				
					pNew = new CBindingBoseRsUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_IRTX: 
				{				
					pNew = new CBindingIRTXUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_RC5RX: 
				{					
					pNew = new CBindingRC5RXUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_AVMATRIX: 
				{					
					pNew = new CBindingAVMatrixUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
				{
					pNew = new CBindingAVMatrixUnitInput(*pBindingUnit);
				} break;
				case BINDING_UNIT_TYPE_VIDEOMUX: 
				{				
					pNew = new CBindingVideomuxUnit(*pBindingUnit); 
				} break;		
				case BINDING_UNIT_TYPE_ALARM_GALAXY: 			
				{			
					pNew = new CBindingAlarmUnit(*pBindingUnit,ALARM_GALAXY_MAX_INPUTS);		// TM_CHANGES_ALARM_ARITECH
				} break;
				case BINDING_UNIT_TYPE_ALARM_ARITECH:
				{			
					pNew = new CBindingAlarmUnit(*pBindingUnit,ALARM_ARITECH_MAX_INPUTS);		// TM_CHANGES_ALARM_ARITECH
				} break;
				case BINDING_UNIT_TYPE_ALARM_ANB:
				{
					pNew = new CBindingAlarmUnit(*pBindingUnit,ALARM_ANB_MAX_INPUTS);			// TM_CHANGES_ALARM_ANB
				} break;
				case BINDING_UNIT_TYPE_ALARM_TEXECOM:
				{
					pNew = new CBindingAlarmUnit(*pBindingUnit,ALARM_TEXECOM_MAX_INPUTS);		// TM_CHANGES_ALARM_TEXECOM
				} break;
				case BINDING_UNIT_TYPE_ALARM_INTEGRA:
				{
					pNew = new CBindingAlarmUnit(*pBindingUnit,ALARM_INTEGRA_MAX_INPUTS);		// TM_CHANGES_ALARM_INTEGRA
				} break;
				case BINDING_UNIT_TYPE_AUDIO_HC4: 			
				{				
					pNew = new CBindingHC4AudioUnit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
				{
					pNew = new CBindingHC4AudioBnoUnit(*pBindingUnit);
				} break;
				case BINDING_UNIT_TYPE_BOSELF48:
				{				
					pNew = new CBindingBoseLf48Unit(*pBindingUnit); 
				} break;
				case BINDING_UNIT_TYPE_BOSEV35:
				{				
					pNew = new CBindingBoseV35Unit(*pBindingUnit); 
				} break;
				default: 
				{
					pNew = new CBindingUnit(*pBindingUnit);	
				} break;
			}
		}
	}

	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		ASSERT(0);

		return NULL;
	}

	if (0!=pNew)
	{
		ENSURE(pNew->GetBindingUnitType() == pBindingUnit->GetBindingUnitType());
		ENSURE(pNew->GetNodeAddress() == pBindingUnit->GetNodeAddress());
		ENSURE(pNew->GetUnitAddress() == pBindingUnit->GetUnitAddress());
		ENSURE(0==strcmp(pNew->GetName(),pBindingUnit->GetName()));
	}

	return pNew;
}


/**
 * CBindingUnit* CBindingUnitFactory::_Clone(CBindingUnit *const pBindingUnit)
 *
 * Clone Derived classes of CBindingUnit. 
 * Explicit Upcast pBindingUnit to derived classes !!
 *
 * Oppassen: Roept impliciet de copy constructor aan waardoor alle members
 * gekopieerd worden.
 */
CBindingUnit* CBindingUnitFactory::_Clone(const CBindingUnit *const pBindingUnit)
{
	CBindingUnit *pNew = NULL;
	
	try
	{
		if (pBindingUnit != NULL)
		{
			ASSERT(pBindingUnit->GetNodeAddress() != 0xFF);

			const BYTE bType = pBindingUnit->GetBindingUnitType();

			switch (bType)
			{
				case BINDING_UNIT_TYPE_DIM:	
				{
					const CBindingDimmerUnit* pTemp = 
						static_cast<const CBindingDimmerUnit*>(pBindingUnit);					
					pNew = new CBindingDimmerUnit(*pTemp);				
				} break;
				case BINDING_UNIT_TYPE_DIM_FWD:	
				{
					const CBindingDimmerUnitInput* pTemp = 
						static_cast<const CBindingDimmerUnitInput*>(pBindingUnit);					
					pNew = new CBindingDimmerUnitInput(*pTemp);						
				} break;
				case BINDING_UNIT_TYPE_SWITCH: 
				{
					const CBindingRelaisUnit* pTemp = 
						static_cast<const CBindingRelaisUnit*>(pBindingUnit);
					pNew = new CBindingRelaisUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_SWITCH_FWD: 
				{
					const CBindingRelaisUnitInput* pTemp = 
						static_cast<const CBindingRelaisUnitInput*>(pBindingUnit);
					pNew = new CBindingRelaisUnitInput(*pTemp); 				
				} break;
				case BINDING_UNIT_TYPE_DUOSWITCH: 
				{
					const CBindingDuoSwitchUnit* pTemp = 
						static_cast<const CBindingDuoSwitchUnit*>(pBindingUnit);
					pNew = new CBindingDuoSwitchUnit(*pTemp);
				} break;
				case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
				{
					const CBindingDuoSwitchUnitInput* pTemp = 
						static_cast<const CBindingDuoSwitchUnitInput*>(pBindingUnit);
					pNew = new CBindingDuoSwitchUnitInput(*pTemp);
				} break;
				case BINDING_UNIT_TYPE_SENSUNIV:	
				{					
					const CBindingSensUnivUnit* pTemp = 
						static_cast<const CBindingSensUnivUnit*>(pBindingUnit);
					pNew = new CBindingSensUnivUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_SENS: 
				{
					const CBindingSensUnit* pTemp = 
						static_cast<const CBindingSensUnit*>(pBindingUnit);
					pNew = new CBindingSensUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_CONTROL: 
				{
					const CBindingControlUnit* pTemp = 
						static_cast<const CBindingControlUnit*>(pBindingUnit);
					pNew = new CBindingControlUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_LCD_VIRTUAL: 
				{
					const CBindingVirtualUnit* pTemp = 
						static_cast<const CBindingVirtualUnit*>(pBindingUnit);
					pNew = new CBindingVirtualUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_AUDIO: 
				{
					const CBindingBasicAudioUnit* pTemp = 
						static_cast<const CBindingBasicAudioUnit*>(pBindingUnit);
					pNew = new CBindingBasicAudioUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_BOSERS: 		
				{
					const CBindingBoseRsUnit* pTemp = 
						static_cast<const CBindingBoseRsUnit*>(pBindingUnit);
					pNew = new CBindingBoseRsUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_IRTX: 
				{
					const CBindingIRTXUnit* pTemp = 
						static_cast<const CBindingIRTXUnit*>(pBindingUnit);
					pNew = new CBindingIRTXUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_RC5RX: 
				{
					const CBindingRC5RXUnit* pTemp = 
						static_cast<const CBindingRC5RXUnit*>(pBindingUnit);
					pNew = new CBindingRC5RXUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_AVMATRIX: 
				{
					const CBindingAVMatrixUnit* pTemp = 
						static_cast<const CBindingAVMatrixUnit*>(pBindingUnit);
					pNew = new CBindingAVMatrixUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
				{
					const CBindingAVMatrixUnitInput* pTemp = 
						static_cast<const CBindingAVMatrixUnitInput*>(pBindingUnit);
					pNew = new CBindingAVMatrixUnitInput(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_VIDEOMUX: 
				{
					const CBindingVideomuxUnit* pTemp = 
						static_cast<const CBindingVideomuxUnit*>(pBindingUnit);
					pNew = new CBindingVideomuxUnit(*pTemp); 
				} break;		
				case BINDING_UNIT_TYPE_ALARM_GALAXY: 
				{
					const CBindingAlarmUnit* pTemp = 
						static_cast<const CBindingAlarmUnit*>(pBindingUnit);
					pNew = new CBindingAlarmUnit(*pTemp); 	
				} break;
				case BINDING_UNIT_TYPE_ALARM_ARITECH:
				case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */
				case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
				case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
				{
					const CBindingAlarmUnit* pTemp = 
						static_cast<const CBindingAlarmUnit*>(pBindingUnit);
					pNew = new CBindingAlarmUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_AUDIO_HC4: 				
				{
					const CBindingHC4AudioUnit* pTemp = 
						static_cast<const CBindingHC4AudioUnit*>(pBindingUnit);
					pNew = new CBindingHC4AudioUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
				{
					const CBindingHC4AudioBnoUnit* pTemp = 
						static_cast<const CBindingHC4AudioBnoUnit*>(pBindingUnit);
					pNew = new CBindingHC4AudioBnoUnit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_BOSELF48:
				{
					const CBindingBoseLf48Unit* pTemp = 
						static_cast<const CBindingBoseLf48Unit*>(pBindingUnit);
					pNew = new CBindingBoseLf48Unit(*pTemp); 
				} break;
				case BINDING_UNIT_TYPE_BOSEV35:
				{				
					const CBindingBoseV35Unit* pTemp = 
						static_cast<const CBindingBoseV35Unit*>(pBindingUnit);
					pNew = new CBindingBoseV35Unit(*pTemp); 
				} break;
				default: 
				{
					pNew = new CBindingUnit(*pBindingUnit);	
				} break;
			}
		}
	}

	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		ASSERT(0);

		return NULL;
	}
	return pNew;
}


