// BindingPropertyUnit.cpp: implementation of the CBindingPropertyUnit class.
//
// Changes: TM20120402: CBindingPropertySensUnit private members
// Changes: TM20160317: Extra var Supported flags.
// Changes: TM20160406: Switch & Duoswitch unit gebruik van defaults.
// Changes: TM20160406: Var fMakeNoDefaults toegevoegd.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingPropertyUnit.h"

#include "BindingPropertyUnitDefaultFactory.h"
#include "BindingPropertyUnitDefault.h"

#include "App/Appbindings/AppBindingConv2Nodes/BindingConversionToNodesProperty.h"
#include "app/appcfg/appcfginfo/cfgInfoUnit.h"

#include "nodemess.h"

//////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingPropertyUnit::CBindingPropertyUnit(
	BYTE bType, 
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlags) :
m_bType(bType),
m_bNodeAddress(bNodeAddress),
m_bUnitAddress(bUnitAddress),
m_usSupportedFlags(usSupportedFlags),
pConversionToNodes(0)
{

}

CBindingPropertyUnit::~CBindingPropertyUnit()
{

}

//--------------------------------------------------------------------

BYTE CBindingPropertyUnit::ConstructPBindings(
	CBindingConversionToNodesProperty *ptr,
	BOOL fMakeNoDefaults)
{
	ASSERT(ptr);
	pConversionToNodes = ptr;

	return(MakePropertyBindings(fMakeNoDefaults));
}

BOOL CBindingPropertyUnit::MakePropertyBinding(
	CString *szBinding)
{
	szBinding->Empty();
	szBinding->Format(_T("0000%02x_00_0000_PU%02X"),m_bNodeAddress,m_bUnitAddress);
	return TRUE;
}


BOOL CBindingPropertyUnit::AddToNodes(
	CString szString) 
{
	if (pConversionToNodes == NULL) ASSERT(0);

	if (pConversionToNodes != NULL) 
		pConversionToNodes->AddBinding(szString);
	
	return TRUE;
}
//-------------------------------------------------------------------------//
///////////////////// CBindingPropertyDimUnit ///////////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertyDimUnit::CBindingPropertyDimUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) : 
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag),
m_pDefaults(0)
{	
	// TM 20130328:
	// Structuur gebruiken om de params mee te geven aan de factory.
	// Binding unit type meegeven omdat de unit mogelijk nog niet aanwezig is 
	// in de nodedatabase.
	const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
	{
		bNodeAddress,
		bUnitAddress,
		bType			// Binding unit type.
	};

	m_pDefaults = static_cast<const CBindingPropertyUnitDimDefault* const>
					(CBindingPropertyUnitDefaultFactory::Instance()->
						Create(UNIT_TYPE_DIM,unitInfo));

	SetToDefaults(); 
}

void CBindingPropertyDimUnit::SetToDefaults(void)
{
	ASSERT( m_pDefaults );

	if (m_pDefaults != 0)
	{
		m_fSolidState = m_pDefaults->GetSolidState();
		m_fPirDelayedOn = m_pDefaults->GetDelayedOn();
		m_fPirDelayedOff = m_pDefaults->GetPirDelayedOff();

		m_bDimSpeedUp = m_pDefaults->GetSpeedUp();
		m_bDimSpeedDown = m_pDefaults->GetSpeedDown();
		m_bDimMaxRange = m_pDefaults->GetMaxRange();
		m_bDimMinRange = m_pDefaults->GetMinRange();

		// sedert versie 0x0B00
		m_addressCfg.bType = 0xFF;
		m_addressCfg.bAddressMSB=0xFF;
		m_addressCfg.bAddressLSB=0xFF;
	}
}

//--------------------------------------------------------------------------//

BYTE CBindingPropertyDimUnit::MakePropertyBindings(
	BOOL fMakeNoDefaults)
{
	BYTE bCnt = 0;

	CString szGeneral,szConfig;

	if (!MakePropertyBinding(&szGeneral)) 
		return 0;

	if (MakeSpeedBinding(&szConfig,fMakeNoDefaults)) 
	{	
		bCnt++;
			
		if (!AddToNodes(CString(szGeneral+szConfig))) 
			return 0;
	}

	if (MakeRangeBinding(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
			
		if (!AddToNodes(CString(szGeneral+szConfig))) 
			return 0;
	}

	if (MakeConfigBinding(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
	
		if (!AddToNodes(CString(szGeneral+szConfig))) 
			return 0;
	}

	if(MakeAddressCfgBinding(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
	
		if (!AddToNodes(CString(szGeneral+szConfig))) 
			return 0;
	}

	return (bCnt);
}

BOOL CBindingPropertyDimUnit::MakeSpeedBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	ASSERT( m_pDefaults );

	if (m_pDefaults != 0)
	{
		if (m_bDimSpeedUp==m_pDefaults->GetSpeedUp() && 
			m_bDimSpeedDown==m_pDefaults->GetSpeedDown()) 
		{
			return FALSE;
		}
	}

	szBinding->Format(_T("FA2D0301%02X%02XS"),
					  m_bDimSpeedUp,
					  m_bDimSpeedDown);

	return TRUE;
}

BOOL CBindingPropertyDimUnit::MakeRangeBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	if (m_pDefaults != 0)
	{
		if (m_bDimMaxRange == m_pDefaults->GetMaxRange() &&
			m_bDimMinRange == m_pDefaults->GetMinRange()) 
		{
			return FALSE;
		}
	}

	szBinding->Format(_T("FA2D0302%02X%02XS"),
					  m_bDimMaxRange,
					  m_bDimMinRange);

	return TRUE;
}

//
// Sedert versie 0x0643:
// Indien solid state mag de Delayed On/Off niet aanliggen, hebben anders een vertraging bij
// het uitschakelen van de dimmer
//
// Sedert versie 0x0900:
// Controleren variabel fMakeNoDefaults.
// Indien fMakeNoDefaults==TRUE enkel een binding aanmaken indien het nodig is.
//
BOOL CBindingPropertyDimUnit::MakeConfigBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	BYTE bData = 0;

	if (TRUE==fMakeNoDefaults)
	{
		if (m_pDefaults != 0)
		{
			const BYTE bTempData = (m_fSolidState<<1) +
								   (m_fPirDelayedOn<<6) +
								   (m_fPirDelayedOff<<7);
	
			if (m_pDefaults->GetConfig()==bTempData)
			{
				return FALSE;
			}
		}
	}
	
	if (m_fSolidState == TRUE) 
	{
		bData |= (m_fSolidState<<1);
	}
	else
	{
		bData |= ((m_fPirDelayedOn<<6)+(m_fPirDelayedOff<<7));
	}

	szBinding->Format(_T("FA2D020E%02XS"),bData);

	return TRUE;
}

BOOL CBindingPropertyDimUnit::MakeAddressCfgBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	BOOL fResult=FALSE;
	const BYTE bType=this->m_addressCfg.bType;

	switch (bType)
	{
		case CBindingPropertyDimUnit::ADDRESS_CFG_TYPE_DALI_GROUP:
		{
			szBinding->Format(_T("F%02XD04%02X%02X%02X%02XS"),
				FC_UNITDIMSET,
				DIMMER_OBJECT_ATTR_CFG_ADDRESS,
				m_addressCfg.bType,
				m_addressCfg.bAddressMSB,
				m_addressCfg.bAddressLSB);

			fResult=TRUE;
		
		} break; /* CBindingPropertyDimUnit::ADDRESS_CFG_TYPE_DALI_GROUP */

		case CBindingPropertyDimUnit::ADDRESS_CFG_TYPE_NONE:
		{
			// Absorbe...
			fResult=FALSE;
		} break; /* CBindingPropertyDimUnit::ADDRESS_CFG_TYPE_NONE */

		default:
		{
			fResult=FALSE;
		} break;

	} /* switch (this->m_addressCfg.bType) */

	return fResult;
}


void CBindingPropertyDimUnit::UpdateSpeed(BYTE speedDown,BYTE speedUp)
{
	m_bDimSpeedUp = speedUp;
	m_bDimSpeedDown = speedDown;
}
void CBindingPropertyDimUnit::UpdateRange(BYTE Min,BYTE Max)
{
	m_bDimMaxRange = Max;
	m_bDimMinRange = Min;
}
void CBindingPropertyDimUnit::UpdateConfig(BOOL DelayedOn,BOOL DelayedOff,BOOL SolidState)
{
	m_fPirDelayedOn = DelayedOn;
	m_fPirDelayedOff =  DelayedOff;
	m_fSolidState = SolidState;
}

void CBindingPropertyDimUnit::UpdateConfig(BYTE bConfig)
{
	m_fPirDelayedOff = ((bConfig & 0x80) == 0x80);
	m_fPirDelayedOn = ((bConfig & 0x40) == 0x40);
	m_fSolidState = ((bConfig & 0x02) == 0x02);
}

void CBindingPropertyDimUnit::UpdateCfgAddress(
	BYTE bType,
	BYTE bAddressMSB,
	BYTE bAddressLSB)
{	
	m_addressCfg.bType=bType;
	m_addressCfg.bAddressMSB=bAddressMSB;
	m_addressCfg.bAddressLSB=bAddressLSB;
}

//-------------------------------------------------------------------------//
////////////////////// CBindingPropertySwitchUnit ///////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertySwitchUnit::CBindingPropertySwitchUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) : 
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag),
m_pDefaults(0)
{	
	const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
	{
		bNodeAddress,
		bUnitAddress,
		bType			// Binding unit type.
	};

	m_pDefaults = static_cast<const CBindingPropertyUnitSwitchDefault* const>
					(CBindingPropertyUnitDefaultFactory::Instance()->
						Create(UNIT_TYPE_SWITCH,unitInfo));


	ASSERT(0!=m_pDefaults);

	SetToDefaults(); 
};

void CBindingPropertySwitchUnit::SetToDefaults(void)
{
	if(0!=m_pDefaults)
	{
		bTimeValue=m_pDefaults->GetTimeValue();
		fTimeEnabled=m_pDefaults->GetPrecisionTimed();
	}
	else
	{
		ASSERT(0!=m_pDefaults);

		bTimeValue = PRECISION_TIME_DEFAULT;
		fTimeEnabled = FALSE;
	}
}

void CBindingPropertySwitchUnit::UpdatePrecisionTiming(BOOL fEnable,BYTE TimeValue)
{
	bTimeValue = TimeValue;
	fTimeEnabled = fEnable;
}

//--------------------------------------------------------------------------//

BYTE CBindingPropertySwitchUnit::MakePropertyBindings(
	BOOL fMakeNoDefaults)
{
	BYTE bCnt = 0;
	CString szPropertyBinding;
	CString szGeneral,szConfig;

	if (!MakePropertyBinding(&szGeneral)) 
		return 0;

	if (MakePrecisionTimeBinding(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
		szPropertyBinding = szGeneral + szConfig;

		if (!AddToNodes(szPropertyBinding)) return 0;
	}
	return bCnt;
}


//
// Sedert versie 0x0900:
// Controleren variabel fMakeNoDefaults.
// Indien fMakeNoDefaults==TRUE enkel een binding aanmaken indien het nodig is.
//
BOOL CBindingPropertySwitchUnit::MakePrecisionTimeBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	ASSERT(0!=m_pDefaults);

	if (TRUE==fMakeNoDefaults)
	{
		if (m_pDefaults != 0)
		{
			if ((fTimeEnabled==FALSE)&&
				(m_pDefaults->GetPrecisionTimed()==FALSE))
			{
				return FALSE;
			}
		}
	}

	if (fTimeEnabled==FALSE) 
	{		
		szBinding->Format(_T("FA3D020800S"));
		return TRUE;		
	}

	szBinding->Format(_T("FA3D0208%02XS"),bTimeValue);
	return TRUE;		
}


//-------------------------------------------------------------------------//
//////////////////// CBindingPropertyDuoSwitchUnit //////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertyDuoSwitchUnit::CBindingPropertyDuoSwitchUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag),
m_pDefaults(0),
m_bStopTime(DUOSWITCH_DEFAULT_STOPTIME),
m_bSwitchTime(DUOSWITCH_DEFAULT_SWITCHTIME)
{
	const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
	{
		bNodeAddress,
		bUnitAddress,
		bType			// Binding unit type.
	};

	m_pDefaults = static_cast<const CBindingPropertyUnitDuoswitchDefault* const>
					(CBindingPropertyUnitDefaultFactory::Instance()->
						Create(UNIT_TYPE_DUOSWITCH,unitInfo));


	ASSERT(0!=m_pDefaults);


	SetToDefaults(); 
};

void CBindingPropertyDuoSwitchUnit::SetToDefaults(void)
{
	if(0!=m_pDefaults)
	{
		m_bStopTime =m_pDefaults->GetStopTime();
		m_bSwitchTime=m_pDefaults->GetSwitchTime();
	}
	else
	{
		ASSERT(0!=m_pDefaults);

		m_bStopTime = DUOSWITCH_DEFAULT_STOPTIME;
		m_bSwitchTime = DUOSWITCH_DEFAULT_SWITCHTIME;
	}
}

//--------------------------------------------------------------------------//

BYTE CBindingPropertyDuoSwitchUnit::MakePropertyBindings(
	BOOL fMakeNoDefaults)
{
	BYTE bCnt = 0;
	CString szPropertyBinding;
	CString szGeneral,szConfig;

	if (!MakePropertyBinding(&szGeneral)) return 0;

	if (MakeStopTimeBinding(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
		szPropertyBinding = szGeneral + szConfig;
		if (!AddToNodes(szPropertyBinding)) return 0;
	}
	
	if (MakeSwitchTimeBinding(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
		szPropertyBinding = szGeneral + szConfig;
		if (!AddToNodes(szPropertyBinding)) return 0;
	}
	return bCnt;
}

//
// Sedert versie 0x0900:
// Controleren variabel fMakeNoDefaults.
// Indien fMakeNoDefaults==TRUE enkel een binding aanmaken indien het nodig is.
//
BOOL CBindingPropertyDuoSwitchUnit::MakeStopTimeBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	ASSERT(0!=m_pDefaults);

	if (fMakeNoDefaults==TRUE)
	{
		if (m_pDefaults!=0)
		{
			if (this->getStopTime()==m_pDefaults->GetStopTime())
			{
				return FALSE;
			}
		}
	}

	szBinding->Format(_T("FB6D0209%02XS"),this->getStopTime());
	return TRUE;
}

//
// Sedert versie 0x0900:
// Controleren variabel fMakeNoDefaults.
// Indien fMakeNoDefaults==TRUE enkel een binding aanmaken indien het nodig is.
//
BOOL CBindingPropertyDuoSwitchUnit::MakeSwitchTimeBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	ASSERT(0!=m_pDefaults);

	if (fMakeNoDefaults==TRUE)
	{
		if (m_pDefaults!=0)
		{
			if (this->getSwitchTime()==m_pDefaults->GetSwitchTime())
			{
				return FALSE;
			}
		}
	}
	szBinding->Format(_T("FB6D020A%02XS"),this->getSwitchTime());
	return TRUE;
}

void CBindingPropertyDuoSwitchUnit::UpdateStopTime(BYTE value)
{
	setStopTime(value);
}
void CBindingPropertyDuoSwitchUnit::UpdateSwitchTime(BYTE value)
{
	setSwitchTime(value);
}

// Afhankelijk van de supported vlaggen....
BYTE CBindingPropertyDuoSwitchUnit::getStopTimeMinValue( ) const	
{
	BYTE retValue = DUOSWITCH_TIMER_MINVALUE;
	if (CfgInfoUnitDuoswitch::FLAG_SUPPORTED_STOPTIME_NONE == (this->GetSupportedFlags() & CfgInfoUnitDuoswitch::FLAG_SUPPORTED_STOPTIME_NONE))
	{
		retValue = 0;
	}
	return retValue;
}

void CBindingPropertyDuoSwitchUnit::setStopTime(BYTE value) 
{
	if (value<this->getStopTimeMinValue()) {
		value=this->getStopTimeMinValue();
	}
	else if (value>this->getStopTimeMaxValue()) {
		value=this->getStopTimeMaxValue();
	}
	m_bStopTime = value;	
}

void CBindingPropertyDuoSwitchUnit::setSwitchTime(BYTE value)
{
	if (value<this->getSwitchTimeMinValue()) {
		value=this->getSwitchTimeMinValue();
	}
	else if (value>this->getSwitchTimeMaxValue()) {
		value=this->getSwitchTimeMaxValue();
	}
	m_bSwitchTime = value;
}
	

//-------------------------------------------------------------------------//
//////////////////////// CBindingPropertySensUnit ///////////////////////////
//-------------------------------------------------------------------------//

CBindingPropertySensUnit::CBindingPropertySensUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag),
m_pDefaults(0)
{
	// TM 20130328:
	// Structuur gebruiken om de params mee te geven aan de factory.
	// Binding unit type meegeven omdat de unit mogelijk nog niet aanwezig is 
	// in de nodedatabase.
	const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
	{
		bNodeAddress,
		bUnitAddress,
		bType			// Binding unit type.
	};


	m_pDefaults = static_cast<const CBindingPropertyUnitSensDefault* const>
					(CBindingPropertyUnitDefaultFactory::Instance()->
						Create(UNIT_TYPE_SENS,unitInfo));

	ASSERT(m_pDefaults);

	SetToDefaults(); 
};

void CBindingPropertySensUnit::SetToDefaults(void)
{
	if (m_pDefaults)
	{
		// Hysteris waarden - High
		m_bHysValues[HYST_OFFSET_HIGH][MODE_SUN] = m_pDefaults->GetHysHighValue(MODE_SUN);	
		m_bHysValues[HYST_OFFSET_HIGH][MODE_HSUN] = m_pDefaults->GetHysHighValue(MODE_HSUN);
		m_bHysValues[HYST_OFFSET_HIGH][MODE_MOON] = m_pDefaults->GetHysHighValue(MODE_MOON);
		m_bHysValues[HYST_OFFSET_HIGH][MODE_HMOON] = m_pDefaults->GetHysHighValue(MODE_HMOON);

		// Hysteris waarden - Low
		m_bHysValues[HYST_OFFSET_LOW][MODE_SUN] = m_pDefaults->GetHysLowValue(MODE_SUN);	
		m_bHysValues[HYST_OFFSET_LOW][MODE_HSUN] = m_pDefaults->GetHysLowValue(MODE_HSUN);
		m_bHysValues[HYST_OFFSET_LOW][MODE_MOON] = m_pDefaults->GetHysLowValue(MODE_MOON);
		m_bHysValues[HYST_OFFSET_LOW][MODE_HMOON] = m_pDefaults->GetHysLowValue(MODE_HMOON);

		// Hysteris offset waarden.
		m_bHysOffsetValues[HYST_OFFSET_HIGH] = m_pDefaults->GetHysOffsetHighValue();
		m_bHysOffsetValues[HYST_OFFSET_LOW] = m_pDefaults->GetHysOffsetLowValue();	

		m_bPrevHysOffsetValues[HYST_OFFSET_HIGH] = m_bHysOffsetValues[HYST_OFFSET_HIGH];
		m_bPrevHysOffsetValues[HYST_OFFSET_LOW] = m_bHysOffsetValues[HYST_OFFSET_LOW];	

		// Varia
		m_bSensType = m_pDefaults->GetSensType( );
		m_bSampleTime = m_pDefaults->GetSampleTime( );
		m_sMeasuredValOffset = m_pDefaults->GetMeasuredValueOffset();

		// Sedert 0x0E00
		m_setpointRangeMin = m_pDefaults->getSetpointRangeMin();
		m_setpointRangeMax = m_pDefaults->getSetpointRangeMax();

		// Delegation - Deze is standaard disabled...
		m_delegation.m_bNodeAddress = 0xFF;
		m_delegation.m_bUnitAddress = 0xFF;
		m_delegation.m_bType = CBindingPropertySensUnit::DELEGATION_TYPE_NONE;

		// Sedert V16.00: USE_SENSOR_PIDCONTROL
		for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) {
			m_pidControl[mode].m_bNodeAddress = 0xff;
			m_pidControl[mode].m_bUnitAddress = 0xff;
			for (int param=0;param<SENS_PIDCONTROL_PARAM_MAX;++param) {
				m_pidControl[mode].m_params[param]  = m_pDefaults->getPIDControlParam(mode,param);
			}
		}			
	}
}

//--------------------------------------------------------------------------//

BYTE CBindingPropertySensUnit::MakePropertyBindings(BOOL fMakeNoDefaults)
{
	CString szGeneral;	
	CString szConfig;
	BYTE bCnt = 0;

	if (!MakePropertyBinding(&szGeneral)) 
		return 0;

	if (MakeSensTypeBinding(&szConfig,fMakeNoDefaults)) 
	{
		const CString szPropertyBinding(szGeneral+szConfig);
		bCnt++;	

		if (!AddToNodes(szPropertyBinding)) 
			return 0;
	}

	if (MakeSampleIntervalBinding(&szConfig,fMakeNoDefaults)) 
	{
		const CString szPropertyBinding(szGeneral+szConfig);
		bCnt++;				

		if (!AddToNodes(szPropertyBinding)) 
			return 0;
	}

	if (MakeHystOffsetBindings(&szConfig,fMakeNoDefaults)) 
	{
		const CString szPropertyBinding(szGeneral+szConfig);
		bCnt++;				

		if (!AddToNodes(szPropertyBinding)) 
			return 0;
	}

	for (int i=0;i<4;i++) 
	{
		if (MakeHystOffsetValBindings(&szConfig,i,fMakeNoDefaults)) 
		{
			const CString szPropertyBinding(szGeneral+szConfig);
			bCnt++;			

			if (!AddToNodes(szPropertyBinding)) 
				return 0;
		}
	}

	if (MakeMeasuredValOffsetBinding(&szConfig,fMakeNoDefaults))		
	{
		const CString szPropertyBinding(szGeneral+szConfig);
		bCnt++;	

		if (!AddToNodes(szPropertyBinding)) 
			return 0;
	}

	// Sedert versie 0x0900
	if(MakeDelegationBinding(&szConfig,fMakeNoDefaults))
	{
		const CString szPropertyBinding(szGeneral+szConfig);
		bCnt++;				

		if (!AddToNodes(szPropertyBinding)) 
			return 0;
	}

	// Sedert versie 0x0E00
	if (MakeSetpointRangeBinding(&szConfig,fMakeNoDefaults))
	{
		const CString szPropertyBinding(szGeneral+szConfig);
		bCnt++;				

		if (!AddToNodes(szPropertyBinding)) 
			return 0;
	}

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) {
		// P-Binding voor de unit.
		if (MakePIDControlUnitBinding(&szConfig,mode,fMakeNoDefaults)) {
			const CString szPropertyBinding(szGeneral+szConfig);
			bCnt++;		
			if (!AddToNodes(szPropertyBinding)) 
				return 0;
		}
		// P-Binding voor de params.
		if (MakePIDControlParamsBinding(&szConfig,mode,fMakeNoDefaults)) {
			const CString szPropertyBinding(szGeneral+szConfig);
			bCnt++;		
			if (!AddToNodes(szPropertyBinding)) 
				return 0;
		}		
	}

	return bCnt;
}

// Sedert versie 0x0900
BOOL CBindingPropertySensUnit::MakeDelegationBinding(CString* szBinding,BOOL fMakeNoDefaults)
{
	BOOL fResult=FALSE;
	
	switch(m_delegation.m_bType)
	{
		case CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE:
		{
			if ((m_delegation.m_bNodeAddress!=0xFF)&&
				(m_delegation.m_bUnitAddress!=0xFF))
			{	
				szBinding->Format(_T("F%02XD03%02X%02X%02XS"),
						FC_UNITSENSSET,
						SENS_OBJECT_ATTR_DELEGATE,
						m_delegation.m_bNodeAddress,
						m_delegation.m_bUnitAddress);

				fResult=TRUE;
			}
			else
			{
				ASSERT(m_delegation.m_bNodeAddress!=0xFF);
				ASSERT(m_delegation.m_bUnitAddress!=0xFF);
			}
		} break; /* CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE */

		case CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR:
		{
			if ((m_delegation.m_bNodeAddress!=0xFF)&&
				(m_delegation.m_bUnitAddress!=0xFF))
			{
				szBinding->Format(_T("F%02XD03%02X%02X%02XS"),
						FC_UNITSENSSET,
						SENS_OBJECT_ATTR_DELEGATOR,
						m_delegation.m_bNodeAddress,
						m_delegation.m_bUnitAddress);

				fResult=TRUE;
			}
			else
			{
				ASSERT(m_delegation.m_bNodeAddress!=0xFF);
				ASSERT(m_delegation.m_bUnitAddress!=0xFF);
			}
		} break; /* CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR */

		// Sedert versie 0x0C02
		case CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE:
		{
			if ((m_delegation.m_bNodeAddress!=0xFF)&&
				(m_delegation.m_bUnitAddress!=0xFF))
			{
				szBinding->Format(_T("F%02XD03%02X%02X%02XS"),
						FC_UNITSENSSET,
						SENS_OBJECT_ATTR_CFG_MEASURESENSOR,
						m_delegation.m_bNodeAddress,
						m_delegation.m_bUnitAddress);

				fResult=TRUE;
			}
			else
			{
				ASSERT(m_delegation.m_bNodeAddress!=0xFF);
				ASSERT(m_delegation.m_bUnitAddress!=0xFF);
			}
		} break; /* CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE */

		case CBindingPropertySensUnit::DELEGATION_TYPE_NONE:
		{
			fResult=FALSE;

		} break;
	}	
	return fResult;
}

BOOL CBindingPropertySensUnit::MakeSetpointRangeBinding(CString* szBinding,BOOL fMakeNoDefaults)			// sedert versie 0x0E00
{
	if ((m_setpointRangeMin == m_pDefaults->getSetpointRangeMin())&&
		(m_setpointRangeMax == m_pDefaults->getSetpointRangeMax()))
	{
		return FALSE;
	}

	szBinding->Format(_T("F88D05%02X%04X%04XS"),
		SENS_OBJECT_ATTR_SP_RANGE,
		m_setpointRangeMin,
		m_setpointRangeMax);

	return TRUE;
}

BOOL CBindingPropertySensUnit::MakeHystOffsetValBindings(
	CString *szBinding,
	BYTE bMode,
	BOOL fMakeNoDefaults)
{
	// BUG-0135
	// Moeten vergelijken met de defaults en niet met de macros.

	if (m_bHysValues[HYST_OFFSET_HIGH][bMode] == m_pDefaults->GetHysHighValue(bMode) &&
		m_bHysValues[HYST_OFFSET_LOW][bMode] == m_pDefaults->GetHysLowValue(bMode))
	{
		return FALSE;
	}
	
	szBinding->Format(_T("F88D0402%02X%02X%02XS"),m_bHysValues[HYST_OFFSET_HIGH][bMode],
												  m_bHysValues[HYST_OFFSET_LOW][bMode],
												  bMode);

	return TRUE;
}

// BUG-0154:
// Indien de waarden veranderd werden dan de bindings aanmaken.
// Indien de waarden niet veranderd werden dan vergelijken met de standaard waarden.
BOOL CBindingPropertySensUnit::MakeHystOffsetBindings(CString *szBinding,BOOL fMakeNoDefaults)
{
	if ((m_bPrevHysOffsetValues[HYST_OFFSET_HIGH] == m_bHysOffsetValues[HYST_OFFSET_HIGH])&&
		(m_bPrevHysOffsetValues[HYST_OFFSET_LOW] == m_bPrevHysOffsetValues[HYST_OFFSET_LOW]))
	{	
		// BUG-0135
		// Moeten vergelijken met de defaults en niet met de macros.
		if (m_bHysOffsetValues[HYST_OFFSET_HIGH] == m_pDefaults->GetHysOffsetHighValue()&&
			m_bHysOffsetValues[HYST_OFFSET_LOW] == m_pDefaults->GetHysOffsetLowValue()) 
		{
			return FALSE; 
		}
	}
	szBinding->Format(_T("F88D030C%02X%02XS"),m_bHysOffsetValues[HYST_OFFSET_HIGH],m_bHysOffsetValues[HYST_OFFSET_LOW]);
	m_bPrevHysOffsetValues[HYST_OFFSET_HIGH] = m_bHysOffsetValues[HYST_OFFSET_HIGH];
	m_bPrevHysOffsetValues[HYST_OFFSET_LOW] = m_bHysOffsetValues[HYST_OFFSET_LOW];		
	return TRUE;
}

BOOL CBindingPropertySensUnit::MakeSensTypeBinding(CString *szBinding,BOOL fMakeNoDefaults)
{
	// TM 20041115
	// Only if different type of SENS_CONFIG_TEMPERATURE we must make property bindings.
	// Problem is that when Property binding is made all Settings go back to defaults
	// Setpoints, Hys Values, ...

	if (m_bSensType == SENS_CONFIG_TEMPERATURE) 
		return FALSE;	

	szBinding->Format(_T("F88D0208%02XS"),m_bSensType);
	return TRUE;
}

BOOL CBindingPropertySensUnit::MakeSampleIntervalBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	// BUG-0135
	// Moeten vergelijken met de defaults en niet met de macros.

	if (m_bSampleTime == m_pDefaults->GetSampleTime()) 
	{
		return FALSE;
	}

	szBinding->Format(_T("F88D0207%02XS"),m_bSampleTime);
	return TRUE;
}

BOOL CBindingPropertySensUnit::MakeMeasuredValOffsetBinding(CString *szBinding,BOOL fMakeNoDefaults)
{
	if (m_sMeasuredValOffset == m_pDefaults->GetMeasuredValueOffset()) {
		return FALSE;
	}
	
	//(-1 = 0xFFFF, -2 = 0xFFFE)
	const int LENGTH = 3;
	const unsigned short usValue = (unsigned short) m_sMeasuredValOffset;
	const BYTE bMSB = usValue/256;
	const BYTE bLSB = usValue%256;
	szBinding->Format(_T("F88D%02X%02X%02X%02XS"),LENGTH,SENS_OBJECT_ATTR_MEASUREDOFFSET,bMSB,bLSB);
	return TRUE;
}


// Sedert V16.00: USE_SENSOR_PIDCONTROL	
BOOL CBindingPropertySensUnit::MakePIDControlUnitBinding(CString *szBinding,int workingsmode, BOOL fMakeNoDefaults) {
	ASSERT(workingsmode>=0 && workingsmode<=SENS_PIDCONTROL_WORKINGMODE_MAX);
	UNIT_SENS_PIDCONTROL* const pidControl = &this->m_pidControl[workingsmode];
	if (pidControl->m_bNodeAddress == 0xff && pidControl->m_bUnitAddress == 0xff) {
		return FALSE;
	}
	szBinding->Format(_T("F88D05%02X%02X%02X%02X%02XS"),					
					  SENS_OBJECT_ATTR_PIDCONTROLUNIT,
					  workingsmode,
					  UNIT_TYPE_DIM,			// voorlopig enkel dimmer unit...
				      pidControl->m_bNodeAddress,
					  pidControl->m_bUnitAddress);
	return TRUE;
}
BOOL CBindingPropertySensUnit::MakePIDControlParamsBinding(CString *szBinding,int workingsmode, BOOL fMakeNoDefaults) {
	ASSERT(workingsmode>=0 && workingsmode<=SENS_PIDCONTROL_WORKINGMODE_MAX);
	UNIT_SENS_PIDCONTROL* const pidControl = &this->m_pidControl[workingsmode];
	if ((pidControl->m_params[SENS_PIDCONTROL_PARAM_KP] == m_pDefaults->getPIDControlParam(workingsmode,SENS_PIDCONTROL_PARAM_KP)) && 
		(pidControl->m_params[SENS_PIDCONTROL_PARAM_KI] == m_pDefaults->getPIDControlParam(workingsmode,SENS_PIDCONTROL_PARAM_KI)) && 
		(pidControl->m_params[SENS_PIDCONTROL_PARAM_KD] == m_pDefaults->getPIDControlParam(workingsmode,SENS_PIDCONTROL_PARAM_KD))) {
		return FALSE;
	}
	szBinding->Format(_T("F88D05%02X%02X%02X%02X%02XS"),					
					  SENS_OBJECT_ATTR_PIDCONTROLPARAMS,
					  workingsmode,
					  pidControl->m_params[SENS_PIDCONTROL_PARAM_KP],
				      pidControl->m_params[SENS_PIDCONTROL_PARAM_KI],
					  pidControl->m_params[SENS_PIDCONTROL_PARAM_KD]);
	return TRUE;
}

// ...
void CBindingPropertySensUnit::SetHysOffsetHighValue(BYTE val) {
	if (m_bHysOffsetValues[HYST_OFFSET_HIGH] != val) {
		m_bPrevHysOffsetValues[HYST_OFFSET_HIGH] = m_bHysOffsetValues[HYST_OFFSET_HIGH];
		m_bHysOffsetValues[HYST_OFFSET_HIGH] = val;			
	}
}

void CBindingPropertySensUnit::SetHysOffsetLowValue(BYTE val) {
	if (m_bHysOffsetValues[HYST_OFFSET_LOW] != val) {
		m_bPrevHysOffsetValues[HYST_OFFSET_LOW] = m_bHysOffsetValues[HYST_OFFSET_LOW];
		m_bHysOffsetValues[HYST_OFFSET_LOW] = val;		
	}
}

void CBindingPropertySensUnit::SetHysOffsetValues(BYTE bOffset,BYTE val) {
	ASSERT( bOffset <= HYST_OFFSET_LOW );
	if (m_bHysOffsetValues[bOffset] != val) {
		m_bPrevHysOffsetValues[bOffset] = m_bHysOffsetValues[bOffset];
		m_bHysOffsetValues[bOffset] = val;
	}
}


// ...
void CBindingPropertySensUnit::UpdateType(BYTE type)
{
	m_bSensType = type;
}

void CBindingPropertySensUnit::UpdateHysValues(BYTE bMode,BYTE bValHigh,BYTE bValLow)
{
	if (bMode <= MODE_HMOON)
	{
		m_bHysValues[HYST_OFFSET_HIGH][bMode] = bValHigh;
		m_bHysValues[HYST_OFFSET_LOW][bMode] = bValLow;
	}
}

void CBindingPropertySensUnit::UpdateOffsetValues(BYTE bValHigh,BYTE bValLow)
{
	m_bHysOffsetValues[HYST_OFFSET_HIGH] = bValHigh;
	m_bHysOffsetValues[HYST_OFFSET_LOW] = bValLow;
}

void CBindingPropertySensUnit::UpdateSampleTime(BYTE bValue)
{
	m_bSampleTime = bValue;
}

void CBindingPropertySensUnit::UpdateMeasuredValueOffset(short sValue)
{
	m_sMeasuredValOffset = sValue;
}

void CBindingPropertySensUnit::UpdateDelegation(
	int type,
	BYTE bNodeAddress,
	BYTE bUnitAddress)
{
	SetDelegationType(type);
	SetDelegationNodeAddress(bNodeAddress);
	SetDelegationUnitAddress(bUnitAddress);
}

void CBindingPropertySensUnit::updateSetpointRange(short minValue,short maxValue){
	setSetpointRangeValueMin(minValue);
	setSetpointRangeValueMax(maxValue);
}


// Sedert V16.00: USE_SENSOR_PIDCONTROL
void CBindingPropertySensUnit::setPropertyPIDControlUnit(int workingsmode,BYTE bNodeAddress,BYTE bUnitAddress) {	
	if (workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX) {
		m_pidControl[workingsmode].m_bNodeAddress = bNodeAddress;
		m_pidControl[workingsmode].m_bUnitAddress = bUnitAddress;
	}
	else { ASSERT(workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX); };
}
BOOL CBindingPropertySensUnit::getPropertyPIDControlUnit(int workingsmode,BYTE* pbNodeAddress,BYTE* pbUnitAddress) const {
	if (workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX) {
		*pbNodeAddress = m_pidControl[workingsmode].m_bNodeAddress;
		*pbUnitAddress = m_pidControl[workingsmode].m_bUnitAddress;
		return TRUE;
	}
	ASSERT(workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX); 
	return FALSE;
}
void CBindingPropertySensUnit::setPropertyPIDParam(int workingsmode,int param,int value) {
	if (value>=0 && value<=0xff) {
		if (workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX) {
			if (param>=0 && param<SENS_PIDCONTROL_PARAM_MAX) {
				m_pidControl[workingsmode].m_params[param] = (BYTE) value;
			} else { ASSERT(param>=0 && param<=SENS_PIDCONTROL_PARAM_MAX); }
		}
		else { ASSERT(workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX); };	
	} else { ASSERT(value>=0 && value<=0xff); }
}

BYTE CBindingPropertySensUnit::getPropertyPIDParam(int workingsmode,int param) const {
	if (workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX) {
		if (param>=0 && param<SENS_PIDCONTROL_PARAM_MAX) {
			return m_pidControl[workingsmode].m_params[param];	
		} 
	}
	ASSERT(param>=0 && param<=SENS_PIDCONTROL_PARAM_MAX);
	ASSERT(workingsmode>=0 && workingsmode<SENS_PIDCONTROL_WORKINGMODE_MAX);
	return 0;
}

//-------------------------------------------------------------------------//
//////////////////// CBindingPropertyBasicAudioUnit /////////////////////////
//-------------------------------------------------------------------------//

CBindingPropertyBasicAudioUnit::CBindingPropertyBasicAudioUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag)
{
	SetToDefaults(); 
};

void CBindingPropertyBasicAudioUnit::SetToDefaults(void) { }

//--------------------------------------------------------------------------//


//-------------------------------------------------------------------------//
////////////////////// CBindingPropertyBoseRsUnit ///////////////////////////
//-------------------------------------------------------------------------//

CBindingPropertyBoseRsUnit::CBindingPropertyBoseRsUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlags) :
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlags)
{
	SetToDefaults(); 
};


void CBindingPropertyBoseRsUnit::SetToDefaults(void)
{
	// Temp ??
	m_bConfig = BOSE_LIFESTYLE_SYSTEM48;
}

//--------------------------------------------------------------------------//

//-------------------------------------------------------------------------//
/////////////////////// CBindingPropertyIRTXUnit ////////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertyIRTXUnit::CBindingPropertyIRTXUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType, bNodeAddress,bUnitAddress,usSupportedFlag)
{
	SetToDefaults(); 
};

void CBindingPropertyIRTXUnit::SetToDefaults(void)
{

}

//--------------------------------------------------------------------------//

//-------------------------------------------------------------------------//
/////////////////////// CBindingPropertyRC5RXUnit ///////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertyRC5RXUnit::CBindingPropertyRC5RXUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType,bNodeAddress, bUnitAddress,usSupportedFlag)
{
	SetToDefaults(); 
};

void CBindingPropertyRC5RXUnit::SetToDefaults(void)
{

}

//--------------------------------------------------------------------------//


//-------------------------------------------------------------------------//
///////////////////// CBindingPropertyControlUnit ///////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertyControlUnit::CBindingPropertyControlUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType, bNodeAddress, bUnitAddress,usSupportedFlag)
{
	SetToDefaults(); 
};


void CBindingPropertyControlUnit::SetToDefaults(void)
{

}


//--------------------------------------------------------------------------//

//-------------------------------------------------------------------------//
///////////////////// CBindingPropertyVirtualUnit ///////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertyVirtualUnit::CBindingPropertyVirtualUnit(
	BYTE bType, 
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag)
{
	SetToDefaults(); 
};


void CBindingPropertyVirtualUnit::SetToDefaults(void)
{

}



//--------------------------------------------------------------------------//

//-------------------------------------------------------------------------//
//////////////////// CBindingPropertyDuoSwitchUnit //////////////////////////
//-------------------------------------------------------------------------//

CBindingPropertyAlarmUnit::CBindingPropertyAlarmUnit(
	BYTE bType,
	BYTE bNodeAddress, 
	BYTE bUnitAddress,
	unsigned short usSupportedFlag) :
CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag),
m_pDefaults(0)
{
	DEBUG_TRACE("CBindingPropertyAlarmUnit::ctor - Enter\n");

	// TM 20130328:
	// Structuur gebruiken om de params mee te geven aan de factory.
	// Binding unit type meegeven omdat de unit mogelijk nog niet aanwezig is 
	// in de nodedatabase.
	const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
	{
		bNodeAddress,
		bUnitAddress,
		bType			// Binding unit type.
	};

	m_pDefaults = static_cast<const CBindingPropertyUnitAlarmDefault* const>
					(CBindingPropertyUnitDefaultFactory::Instance()->
						Create(UNIT_TYPE_ALARM,unitInfo));

	ASSERT(m_pDefaults);

	SetToDefaults(); 

	DEBUG_TRACE("CBindingPropertyAlarmUnit::ctor - Exit\n");
};

void CBindingPropertyAlarmUnit::SetToDefaults(void)
{
	DEBUG_TRACE("CBindingPropertyAlarmUnit::SetToDefaults - Enter\n");

	if (m_pDefaults)
	{
		bAlarmCentraleType = m_pDefaults->GetAlarmType( );

		DEBUG_TRACE("Remote Code=");

		for (int i=0;i<ALARM_REMOTECODE_BUFFER_MAX;i++)
		{
			bRemoteCode[i] = m_pDefaults->GetRemoteCode(i);

			DEBUG_TRACE_2("%c",bRemoteCode[i] + 0x30);
		}

		DEBUG_TRACE("\n");
	}

	DEBUG_TRACE("CBindingPropertyAlarmUnit::SetToDefaults - Exit\n");
}

//--------------------------------------------------------------------------//
BYTE CBindingPropertyAlarmUnit::MakePropertyBindings(
	BOOL fMakeNoDefaults)
{
	BYTE bCnt = 0;
	CString szPropertyBinding;
	CString szGeneral,szConfig;

	if (!MakePropertyBinding(&szGeneral)) return 0;

	if (MakeConfigBinding(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
		szPropertyBinding = szGeneral + szConfig;
		if (!AddToNodes(szPropertyBinding)) return 0;
	}
	
	if (MakeRemotecodeBinding1(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
		szPropertyBinding = szGeneral + szConfig;
		if (!AddToNodes(szPropertyBinding)) return 0;
	}

	if (MakeRemotecodeBinding2(&szConfig,fMakeNoDefaults)) 
	{
		bCnt++;
		szPropertyBinding = szGeneral + szConfig;
		if (!AddToNodes(szPropertyBinding)) return 0;
	}
	
	return bCnt;
}

BOOL CBindingPropertyAlarmUnit::MakeRemotecodeBinding1(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	// TM20130329:
	// Oppassen: Moeten altijd een P-Binding aanmaken omdat de deze waarden in
	// NOV geheugen worden bijgehouden.

	szBinding->Format(_T("FCCD060800%02X%02X%02X%02XS"),
							bRemoteCode[0],
							bRemoteCode[1],
							bRemoteCode[2],
							bRemoteCode[3]);	
	return TRUE;
}

BOOL CBindingPropertyAlarmUnit::MakeRemotecodeBinding2(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	// TM20130329:
	// Oppassen: Moeten altijd een P-Binding aanmaken omdat de deze waarden in
	// NOV geheugen worden bijgehouden.

	szBinding->Format(_T("FCCD060804%02X%02X%02X%02XS"),
						bRemoteCode[4],
						bRemoteCode[5],
						bRemoteCode[6],
						bRemoteCode[7]);
	return TRUE;
}

BOOL CBindingPropertyAlarmUnit::MakeConfigBinding(
	CString *szBinding,
	BOOL fMakeNoDefaults)
{
	// TM20130329:
	// Oppassen: Moeten altijd een P-Binding aanmaken omdat de deze waarden in
	// NOV geheugen worden bijgehouden.

	if (fMakeNoDefaults==TRUE)
	{
		if (m_pDefaults!=0)
		{
			// Absorbe momenteel...
		}
	}

	szBinding->Format(_T("FCCD0209%02XS"),bAlarmCentraleType);
	return TRUE;
}


void CBindingPropertyAlarmUnit::UpdateType(BYTE bType)
{
	bAlarmCentraleType = bType;
}

void CBindingPropertyAlarmUnit::UpdateRemotecode(const BYTE* pbData)
{
	for (int i=0;i<ALARM_REMOTECODE_BUFFER_MAX;i++)
	{
		bRemoteCode[i] = pbData[i];
	}
}

void CBindingPropertyAlarmUnit::UpdateRemotecode(int index,const BYTE bData)	// CHANGES_MVS_2008
{
	if (index < ALARM_REMOTECODE_BUFFER_MAX) 
	{
		bRemoteCode[index] = bData;
	}
}

// TM20130328
void CBindingPropertyAlarmUnit::SetRemoteCode(int index,BYTE bData)
{
	if (index < ALARM_REMOTECODE_BUFFER_MAX)
	{
		bRemoteCode[index] = bData;	
	}
}

BYTE CBindingPropertyAlarmUnit::GetRemoteCode(int index) const
{
	BYTE bRetVal = 0xFF;

	if (index < ALARM_REMOTECODE_BUFFER_MAX)
	{
		bRetVal = bRemoteCode[index];
	}
	return bRetVal;
}

void CBindingPropertyAlarmUnit::SetAlarmCentraleType(BYTE bType)
{
	bAlarmCentraleType = bType;
}

BYTE CBindingPropertyAlarmUnit::GetAlarmCentraleType( ) const
{
	return bAlarmCentraleType;
}

//-------------------------------------------------------------------------//
/////////////////// CBindingPropertyAVMatrixUnit	/////////////////////////
//-------------------------------------------------------------------------//
CBindingPropertyAVMatrixUnit::CBindingPropertyAVMatrixUnit(
	BYTE bType, BYTE bNodeAddress, BYTE bUnitAddress, AVMatrixType_t AVMatrixType,unsigned short usSupportedFlag) :
	CBindingPropertyUnit(bType,bNodeAddress,bUnitAddress,usSupportedFlag),
	m_type(AVMatrixType)
{
	SetToDefaults(); 
}

void CBindingPropertyAVMatrixUnit::SetToDefaults() {
	this->m_subType = 0;
}

void CBindingPropertyAVMatrixUnit::UpdateSubType(BYTE bSubType) {
	SetSubType(bSubType);
}
void CBindingPropertyAVMatrixUnit::SetSubType(BYTE bSubType) {
	this->m_subType = bSubType;
}
BYTE CBindingPropertyAVMatrixUnit::GetSubType( ) const {
	return this->m_subType;
}
BYTE CBindingPropertyAVMatrixUnit::MakePropertyBindings(BOOL fMakeNoDefaults) {	
	int count=0;
	CString szGeneral,szConfig;
	if (!MakePropertyBinding(&szGeneral)) return 0;

	if (TRUE==MakeConfigBinding(&szConfig,fMakeNoDefaults)) {
		count++;
		CString szPropertyBinding = szGeneral + szConfig;
		if (!AddToNodes(szPropertyBinding)) return 0;
	}
	ENSURE(count>=0 && count<=0xFF);
	return count;
}

BOOL CBindingPropertyAVMatrixUnit::MakeConfigBinding(CString *szBinding,BOOL fMakeNoDefaults)
{
	// Oppassen: Moeten altijd een P-Binding aanmaken omdat de deze waarden in
	// NOV geheugen worden bijgehouden.
	if (fMakeNoDefaults==TRUE){
		if (0 != this->GetSubType()) {
			// Hier de binding aanmaken.
		}
	}

	// Sedert 0x0F10: zie BUG-0164
	// Property binding aanmaken enkel wanneer type niet onbekend is (= wanneer integra/audac)
	// Anders wordt het geconfigureerd type van de universele av-matrix overschreven.
	if (AVMATRIX_TYPE_UNKNOWN == getMatrixType()){
		return FALSE;
	}

	// Zie opm: Altijd de binding aanmaken.
	szBinding->Format(_T("FCAD020E%02XS"),this->GetSubType());
	return TRUE;
}
