// BindingUnitOperations.cpp: implementation of the CBindingUnitOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingUnitOperations.h"

#include "BindingConstants.h"
#include "BindingUnitFactory.h"

#include "NodeDatabaseOperations.h"

#include "app\appbindings\appbindingunit\bindingunit_api.h"

#include "app\appcfg\appcfginfo\cfginfounit.h"

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
CBindingUnitOperations::CBindingUnitOperations(CBindingUnit* pBindingUnit) :
m_pBindingUnit(pBindingUnit)
{
}	

CBindingUnitOperations::~CBindingUnitOperations()
{
}

BYTE CBindingUnitOperations::ConvertType(
	const CCanNode* pNode,
	STRUCT_UNIT* const pUnit)
{
	BYTE bType = 0xFF;

	if (pUnit != NULL)
	{
		const BYTE bUnitType = pUnit->bUnitType;

		switch(bUnitType)
		{
			case UNIT_TYPE_DIM:
			{
				if (TRUE==CCfgInfoUnit::isSupported(
							pNode,
							pUnit,
							CCfgInfoUnit::FLAG_SUPPORTED_BINDINGS_FORWARD))
				{
					bType = BINDING_UNIT_TYPE_DIM_FWD;
				}
				else
				{
					bType = BINDING_UNIT_TYPE_DIM;
				}
			}break;

			case UNIT_TYPE_SWITCH:
			{
				if (TRUE==CCfgInfoUnit::isSupported(
							pNode,
							pUnit,
							CCfgInfoUnit::FLAG_SUPPORTED_BINDINGS_FORWARD))
				{				
					bType = BINDING_UNIT_TYPE_SWITCH_FWD;
				}
				else
				{
					bType = BINDING_UNIT_TYPE_SWITCH;
				}
			} break;

			case UNIT_TYPE_CONTROL:
				bType = BINDING_UNIT_TYPE_CONTROL;
			break;

			case UNIT_TYPE_SENS:
			{
				/* Sedert versie 0x0D00 */
				if (CfgInfoUnitSens::TYPE_SENSOR_UNIVERSAL ==
					CfgInfoUnitSens::getType(pNode,pUnit))
				{
					bType = BINDING_UNIT_TYPE_SENSUNIV;
				}
				else
				{
					bType = BINDING_UNIT_TYPE_SENS;
				}
			} break;

			case UNIT_TYPE_AUDIO:
				bType = BINDING_UNIT_TYPE_AUDIO;
			break;

			case UNIT_TYPE_LCD:
				bType = BINDING_UNIT_TYPE_LCD;
			break;

			case UNIT_TYPE_LCD_VIRTUAL:
				bType = BINDING_UNIT_TYPE_LCD_VIRTUAL;
			break;

			case UNIT_TYPE_DUOSWITCH:
			{
				if (TRUE==CCfgInfoUnit::isSupported(
							pNode,
							pUnit,
							CCfgInfoUnit::FLAG_SUPPORTED_BINDINGS_FORWARD))
				{				
					bType = BINDING_UNIT_TYPE_DUOSWITCH_FWD;
				}
				else
				{
					bType = BINDING_UNIT_TYPE_DUOSWITCH;
				}
			} break;

			case UNIT_TYPE_SELECTOR:
				bType = BINDING_UNIT_TYPE_SELECTOR;
			break;

			case UNIT_TYPE_BOSERS:
			{
				if (pUnit->UnitSpecs.AudioUnit.bAudioConfig == 
					BOSE_LIFESTYLE_SYSTEM50)
				{
					bType = BINDING_UNIT_TYPE_BOSERS;
				}
				else if (pUnit->UnitSpecs.AudioUnit.bAudioConfig ==
						 BOSE_LIFESTYLE_SYSTEMV35)
				{
					bType = BINDING_UNIT_TYPE_BOSEV35;
				}
				else
				{
					bType = BINDING_UNIT_TYPE_BOSELF48;
				}

			} break;

			case UNIT_TYPE_AVMATRIX:		/* TM_AVMATRIX_INPUT */
			{
				const BYTE bConfig = pUnit->UnitSpecs.AVMatrixUnit.bAVMatrixConfig&AVMATRIX_TYPE_CONFIG_MASK;
				if (bConfig == AVMATRIX_TYPE_CONFIG_INPUT)
				{
					bType = BINDING_UNIT_TYPE_AVMATRIX_INPUT;
				}
				else
				{
					bType = BINDING_UNIT_TYPE_AVMATRIX;
				}
					
			} break;

			case UNIT_TYPE_IRTX:
				bType = BINDING_UNIT_TYPE_IRTX;
			break;

			case UNIT_TYPE_RC5RX:
				bType = BINDING_UNIT_TYPE_RC5RX;
			break;

			case UNIT_TYPE_VIDEOMUX:
				bType = BINDING_UNIT_TYPE_VIDEOMUX;
			break;

			case UNIT_TYPE_PS2INTELLIMOUSE:
				bType = BINDING_UNIT_TYPE_PS2INTELLIMOUSE;
			break;

			case UNIT_TYPE_SPEECHCOMM:
				bType = BINDING_UNIT_TYPE_SPEECHCOMM;
			break;

			case UNIT_TYPE_SPMATRIX:
				bType = BINDING_UNIT_TYPE_SPMATRIX;
			break;

			case UNIT_TYPE_ALARM:
			{
				// TM_CHANGES_ALARM_ARITECH + TM_CHANGES_ALARM_ANB + TM_CHANGES_ALARM_TEXECOM +
				// TM_CHANGES_ALARM_INTEGRA
				// Het alarm unit type heeft een verschilend binding unit type ifv de config.

				const BYTE bConfig = pUnit->UnitSpecs.AlarmUnit.bAlarmConfig & ALARM_TYPE_CONFIG_MASK;
				if (bConfig == ALARM_TYPE_CONFIG_ARITECH)
				{
					bType = BINDING_UNIT_TYPE_ALARM_ARITECH;
				}
				else if (bConfig == ALARM_TYPE_CONFIG_ANB_BIBUS)
				{
					bType = BINDING_UNIT_TYPE_ALARM_ANB;
				}
				else if (bConfig == ALARM_TYPE_CONFIG_TEXECOM_PREMIER48 ||
						 bConfig == ALARM_TYPE_CONFIG_TEXECOM_PREMIER88 ||
						 bConfig == ALARM_TYPE_CONFIG_TEXECOM_PREMIER168) 
				{
					bType = BINDING_UNIT_TYPE_ALARM_TEXECOM;
				}
				else if (bConfig == ALARM_TYPE_CONFIG_INTEGRA)
				{
					bType = BINDING_UNIT_TYPE_ALARM_INTEGRA;
				}
				else			
				{				
					bType = BINDING_UNIT_TYPE_ALARM_GALAXY;
				}
			} break;

			case UNIT_TYPE_AUDIO_HC4:
			{
				const BYTE bConfig = pUnit->UnitSpecs.AudioUnit.bAudioConfig & AUDIO_TYPE_CONFIG_MASK;
				if (bConfig == AUDIO_TYPE_CONFIG_BNO)
				{
					bType = BINDING_UNIT_TYPE_AUDIO_HC4_BNO;	/* TM_AUDIOHC4_BNO */
				}
				else
				{
					bType = BINDING_UNIT_TYPE_AUDIO_HC4;
				}

			} break;
		}
	}

	ASSERT(bType != BINDING_UNIT_TYPE_SPEECHCOMM);
	ASSERT(bType != BINDING_UNIT_TYPE_SPMATRIX);
	ASSERT(bType != BINDING_UNIT_TYPE_LCD);
	ASSERT(bType != 0xFF);

	return bType;
}

BOOL CBindingUnitOperations::verifyTypes(
	const CBindingUnit* pBindingUnit,
	const STRUCT_UNIT* pUnit)
{
	BOOL fResult=FALSE;

	const BYTE bBindingUnitType = pBindingUnit->GetBindingUnitType();
	const BYTE bUnitType = pUnit->bUnitType;

	if ((bBindingUnitType == bUnitType) ||
		(bUnitType == UNIT_TYPE_SENS && 
		 bBindingUnitType == BINDING_UNIT_TYPE_SENSUNIV) ||	
		(bUnitType == UNIT_TYPE_DIM && 
		 bBindingUnitType == BINDING_UNIT_TYPE_DIM_FWD) ||		
		(bUnitType == UNIT_TYPE_SWITCH && 
		 bBindingUnitType == BINDING_UNIT_TYPE_SWITCH_FWD) ||	
		(bUnitType == UNIT_TYPE_DUOSWITCH && 
		 bBindingUnitType == BINDING_UNIT_TYPE_DUOSWITCH_FWD) ||	
		(bUnitType == UNIT_TYPE_BOSERS &&
		 (bBindingUnitType == BINDING_UNIT_TYPE_BOSELF48 ||
		  bBindingUnitType == BINDING_UNIT_TYPE_BOSEV35 ||
		  bBindingUnitType == BINDING_UNIT_TYPE_BOSERS)) ||
		(bUnitType == UNIT_TYPE_AVMATRIX &&
		 (bBindingUnitType == BINDING_UNIT_TYPE_AVMATRIX ||
		  bBindingUnitType == BINDING_UNIT_TYPE_AVMATRIX_INPUT)) ||
		(bUnitType == UNIT_TYPE_AUDIO_HC4 &&
		 (bBindingUnitType == BINDING_UNIT_TYPE_AUDIO_HC4 ||
		  bBindingUnitType == BINDING_UNIT_TYPE_AUDIO_HC4_BNO)) ||
		(bUnitType == UNIT_TYPE_ALARM && 
		 (bBindingUnitType == BINDING_UNIT_TYPE_ALARM_GALAXY ||
		  bBindingUnitType == BINDING_UNIT_TYPE_ALARM_ARITECH ||
		  bBindingUnitType == BINDING_UNIT_TYPE_ALARM_ANB ||		/* TM_CHANGES_ALARM_ANB */
		  bBindingUnitType == BINDING_UNIT_TYPE_ALARM_TEXECOM ||	/* TM_CHANGES_ALARM_TEXECOM */
		  bBindingUnitType == BINDING_UNIT_TYPE_ALARM_INTEGRA)))	/* TM_CHANGES_ALARM_INTEGRA */
	{
		fResult=TRUE;
	}
	else
	{
		fResult=FALSE;
	}

	return fResult;
}


/**
 * BOOL CBindingUnitOperations::Upgrade(CBindingUnit** pNewBindingUnit)
 */
BOOL CBindingUnitOperations::UpgradeBindingUnitType(CBindingUnit** pNewBindingUnit)
{
	*pNewBindingUnit = 0;

	if (m_pBindingUnit == 0)
	{
		ASSERT( m_pBindingUnit );
		return FALSE;
	}

	CNodeDatabaseOperations Operations(TMGetDocumentNodes());

	STRUCT_UNIT* pUnit=0;
	CCanNode* pNode=0;

	if (Operations.Find(m_pBindingUnit,&pNode,&pUnit)==TRUE)	
	{
		ASSERT(0!=pNode);
		ASSERT(0!=pUnit);

		m_pBindingUnit->SetBindingUnitType(CBindingUnitOperations::ConvertType(pNode,pUnit));
		m_pBindingUnit->SetName(pUnit->szUnitName);

		// TM 20080122: Moeten hier het node address en unit adres upgraden.
		m_pBindingUnit->SetNodeAddress(pUnit->bNodeLogicalAddress);
		m_pBindingUnit->SetUnitAddress(pUnit->bUnitAddress);

		*pNewBindingUnit = CBindingUnitFactory::Clone(m_pBindingUnit);
			
		if (*pNewBindingUnit != 0) 
		{
			if ((*pNewBindingUnit)->GetBindingUnitType() == 0) 
			{
				ASSERT(0);
				return FALSE;
			}

			if (1)
			{
				const BOOL fResult = (*pNewBindingUnit)->UpgradeIoType();
				if (fResult == FALSE)
				{
					// ASSERT( fResult );

					delete *pNewBindingUnit;
					*pNewBindingUnit = 0;

					return fResult;
				}					
			}

			if (1)
			{
				const BOOL fResult = (*pNewBindingUnit)->ConvertFromBindingData();
				if (fResult == FALSE)
				{
					// ASSERT( fResult );

					delete *pNewBindingUnit;
					*pNewBindingUnit = 0;
				}
				return fResult;
			}
		}	
	}

	return FALSE;
}



BOOL CBindingUnitOperations::Export(FILE* fp)
{
	if (fp == 0) {
		return FALSE;
	}

	STRUCT_UNIT* pUnit = 0;
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());

	if (NodeDatabaseOperations.Find(m_pBindingUnit->GetNodeAddress(),
									m_pBindingUnit->GetUnitAddress(),
									&pUnit))
	{
		CString szMessage;
		szMessage.Format("\nName: '%s',Address: 0x%02x,0x%02x",
						 pUnit->szUnitName,
						 pUnit->bNodeLogicalAddress,
						 pUnit->bUnitAddress);
		
		fputs((LPCTSTR)szMessage,fp);	// CHANGES_0X0A01_USE_FPUTS	
		return TRUE;
	}
	return FALSE;
}



