// BindingPropertyFactory.cpp: implementation of the CBindingPropertyFactory class.
//
// Changes: TM20160317: Referentie naar CCanNode meegeven in de create functie.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingPropertyFactory.h"
#include "BindingPropertyUnit.h"
#include "CanNode.h"
#include "bindingconstants.h"

#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	
#include "app\appCfg\AppCfginfo\cfginfounit.h"	


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/***********************************************************/
#define DEBUG_LOCAL_ON		0
/***********************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define DEBUG_TRACE(x)			TRACE(x)
	#define DEBUG_TRACE_2(x,y)		TRACE(x,y)
#else
	#define	DEBUG_TRACE(x)
	#define DEBUG_TRACE_2(x,y)
#endif
/***********************************************************/


// private:

static
CBindingPropertyUnit* createDimmer(
	const CCanNode* const pNode,
	const STRUCT_UNIT* const pUnit)
{
	CBindingPropertyUnit* p = 0;

	REQUIRE(pNode!=0);
	REQUIRE(pUnit!=0);

	if (0!=pUnit)
	{
		BYTE bindingUnitType=BINDING_UNIT_TYPE_DIM;
		unsigned short usSupportedFlags = CfgInfoUnitDim::getSupportedFlags(pNode,pUnit);

		if (CCfgInfoUnit::isSupported(pNode,pUnit,CCfgInfoUnit::FLAG_SUPPORTED_BINDINGS_FORWARD))
		{
			bindingUnitType=BINDING_UNIT_TYPE_DIM_FWD;
		}
		else
		{
			bindingUnitType=BINDING_UNIT_TYPE_DIM;
		}


		p = new CBindingPropertyDimUnit(
					bindingUnitType,
					pUnit->bNodeLogicalAddress,
					pUnit->bUnitAddress,
					usSupportedFlags);		
	}

	ASSERT(0!=p);
	return p;
}

static
CBindingPropertyUnit* createSensor(
	const CCanNode* const pNode,
	const STRUCT_UNIT* const pUnit)
{
	CBindingPropertyUnit*  p = 0;

	REQUIRE(pNode!=0);
	REQUIRE(pUnit!=0);

	if (0!=pUnit)
	{
		// Sedert versie 0x0D00:
		// Controle op het type sensor unit.

		BYTE bBindingUnitType = BINDING_UNIT_TYPE_SENS;
		const unsigned short usSupportedFlags=CfgInfoUnitSens::getSupportedFlags(pNode,pUnit);
		const CfgInfoUnitSens::Type_t type= CfgInfoUnitSens::getType(pNode,pUnit);

		switch(type)
		{
			case CfgInfoUnitSens::TYPE_SENSOR_NORMAL:
			{
				bBindingUnitType = BINDING_UNIT_TYPE_SENS;
			} break;			
			case CfgInfoUnitSens::TYPE_SENSOR_UNIVERSAL:
			{
				bBindingUnitType = BINDING_UNIT_TYPE_SENSUNIV;
			} break;
			default:
			{
				ASSERT(0);
			} break;

		} /* switch(type) */

		p=new CBindingPropertySensUnit(
					bBindingUnitType,
					pUnit->bNodeLogicalAddress,
					pUnit->bUnitAddress,
					usSupportedFlags);	
	}

	ASSERT(0!=p);
	return p;
}

static
CBindingPropertyUnit* createSwitch(
	const CCanNode* const pNode,
	const STRUCT_UNIT* const pUnit)
{
	const unsigned short usSupportedFlags = 0;
	BYTE bindingUnitType=BINDING_UNIT_TYPE_SWITCH;

	if (CCfgInfoUnit::isSupported(pNode,pUnit,CCfgInfoUnit::FLAG_SUPPORTED_BINDINGS_FORWARD))
	{
		bindingUnitType=BINDING_UNIT_TYPE_SWITCH_FWD;
	}
	else
	{
		bindingUnitType=BINDING_UNIT_TYPE_SWITCH;
	}

	CBindingPropertyUnit* const p =
		new CBindingPropertySwitchUnit(
			bindingUnitType,
			pUnit->bNodeLogicalAddress,
			pUnit->bUnitAddress,
			usSupportedFlags);

	ASSERT(0!=p);
	return p;
}

static
CBindingPropertyUnit* createDuoswitch(
	const CCanNode* const pNode,
	const STRUCT_UNIT* const pUnit)
{
	const unsigned short usSupportedFlags = CfgInfoUnitDuoswitch::getSupportedFlags(pNode,pUnit);
	BYTE bindingUnitType=BINDING_UNIT_TYPE_DUOSWITCH;

	if (CCfgInfoUnit::isSupported(pNode,pUnit,CCfgInfoUnit::FLAG_SUPPORTED_BINDINGS_FORWARD))
	{
		bindingUnitType=BINDING_UNIT_TYPE_DUOSWITCH_FWD;
	}
	else
	{
		bindingUnitType=BINDING_UNIT_TYPE_DUOSWITCH;
	}

	CBindingPropertyUnit* const p = 
		new CBindingPropertyDuoSwitchUnit(
			bindingUnitType,
			pUnit->bNodeLogicalAddress,
			pUnit->bUnitAddress,
			usSupportedFlags);	

	ASSERT(0!=p);
	return p;
}

// TM_CHANGES_ALARM_ARITECH + TM_CHANGES_ALARM_ANB + TM_CHANGES_ALARM_TEXECOM +
// TM_CHANGES_ALARM_INTEGRA
// ifv het type alarm centrale wordt er een verschillend id van binding unit type
// meegegeven in de constructor van CBindingPropertyAlarmUnit.
// Mbv het biding unit type kan er een andere property dialoog getoond worden.
static
CBindingPropertyUnit* createAlarm(			// TODO: Via CfgInfoUnitAlarm !!!!!!!!!!!!!!!
	const CCanNode* const pNode,
	const STRUCT_UNIT* const pUnit)
{
	const unsigned short usSupportedFlags = 0;
	BYTE bType = 0xFF;
	CBindingPropertyUnit* p = 0;
	const BYTE bConfig = pUnit->UnitSpecs.AlarmUnit.bAlarmConfig & ALARM_TYPE_CONFIG_MASK;

	if (bConfig == ALARM_TYPE_CONFIG_ARITECH)
	{
		bType = BINDING_UNIT_TYPE_ALARM_ARITECH;
	}
	else if (bConfig == ALARM_TYPE_CONFIG_ANB_BIBUS)
	{
		bType = BINDING_UNIT_TYPE_ALARM_ANB;
	}
	else if (bConfig==ALARM_TYPE_CONFIG_TEXECOM_PREMIER48 ||
			 bConfig==ALARM_TYPE_CONFIG_TEXECOM_PREMIER88 ||
			 bConfig==ALARM_TYPE_CONFIG_TEXECOM_PREMIER168) 
	{
		bType = BINDING_UNIT_TYPE_ALARM_TEXECOM;
	}
	else if (bConfig==ALARM_TYPE_CONFIG_INTEGRA)
	{
		bType = BINDING_UNIT_TYPE_ALARM_INTEGRA;
	}
	else
	{				
		bType = BINDING_UNIT_TYPE_ALARM_GALAXY;
	}				

	p = new CBindingPropertyAlarmUnit(
				bType,
				pUnit->bNodeLogicalAddress,
				pUnit->bUnitAddress,
				usSupportedFlags);

	ASSERT(0!=p);
	return p;
}

static 
CBindingPropertyUnit* createAVMatrix(const CCanNode* const pNode,const STRUCT_UNIT* const pUnit)
{
	const BYTE bindingUnitType=BINDING_UNIT_TYPE_AVMATRIX;
	const unsigned short usSupportedFlags = CfgInfoUnitAVMatrix::getSupportedFlags(pNode,pUnit);
	CBindingPropertyAVMatrixUnit::AVMatrixType_t type = CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_UNKNOWN;
	CfgInfoUnitAVMatrix::Type_t configType = CfgInfoUnitAVMatrix::getType(pNode,pUnit);
	if (CfgInfoUnitAVMatrix::TYPE_AVMATRIX_AUDAC == configType) {
		type = CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_AUDAC;	
	}
	else if (CfgInfoUnitAVMatrix::TYPE_AVMATRIX_INTEGRA == configType) {
		type = CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_INTEGRA;	
	}
	CBindingPropertyUnit* const p = new CBindingPropertyAVMatrixUnit(
				bindingUnitType,	// BINDING_UNIT_TYPE_AVMATRIX
				pUnit->bNodeLogicalAddress,
				pUnit->bUnitAddress,
				type,				// type unknown of audac
				usSupportedFlags);

	ASSERT(0!=p);
	return p;
}

// ...


// Public:

CBindingPropertyUnit* CBindingPropertyFactory::Create(
	const CCanNode* const pNode,
	const STRUCT_UNIT* const pUnit)
{	
	CBindingPropertyUnit* pNew = 0;	
	DEBUG_TRACE("CBindingPropertyFactory::Create()-Enter\n");	
	ASSERT(pNode);
	ASSERT(pUnit);

	try
	{
		const BYTE bUnitType = pUnit->bUnitType;
		switch (bUnitType)
		{
			case UNIT_TYPE_DIM:		
			{
				pNew=createDimmer(pNode,pUnit);
			} break;	
			case UNIT_TYPE_SENS:	
			{
				pNew=createSensor(pNode,pUnit);
			} break;
			case UNIT_TYPE_SWITCH:	
			{
				pNew=createSwitch(pNode,pUnit);
			} break;		
			case UNIT_TYPE_DUOSWITCH:	
			{
				pNew = createDuoswitch(pNode,pUnit);
			} break;
			case UNIT_TYPE_ALARM:
			{
				pNew=createAlarm(pNode,pUnit);			
			} break;
			case UNIT_TYPE_AVMATRIX:
			{
				pNew=createAVMatrix(pNode,pUnit);
			} break;
			default :					
			{
				pNew = NULL;		
			} break;
		}
	}
	catch(CMemoryException *e)
	{
		ASSERT(0);
		e->ReportError();
		e->Delete();
		return NULL;
	}

	DEBUG_TRACE("CBindingPropertyFactory::Create()-Exit\n");
	return pNew;
}

void CBindingPropertyFactory::destroy(STRUCT_UNIT* pUnit)
{
	if (pUnit && pUnit->m_pBindingUnit != NULL) {
		delete pUnit->m_pBindingUnit;
		pUnit->m_pBindingUnit = NULL;
	}
}

