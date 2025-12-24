// BindingPropertyUnitDefault.cpp: implementation of the CBindingPropertyUnitDefault class.
//
// Changes: TM20120405: Sens Unit: Measured Offset value bijgevoegd
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingPropertyUnitDefault.h"

#include "app\appbindings\appbindingunit\bindingunitdim_def.h"

/***********************************************************/
#define DEBUG_LOCAL_ON		0
/***********************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define DEBUG_TRACE(x)		TRACE(x)
	#define DEBUG_TRACE_2(x,y)	TRACE(x,y)
#else
	#define	DEBUG_TRACE(x)
	#define DEBUG_TRACE_2(x,y)	
#endif
/***********************************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingPropertyUnitDefault::CBindingPropertyUnitDefault() { }
CBindingPropertyUnitDefault::~CBindingPropertyUnitDefault() { }

////////////////////////////////VERSION_DEFAULT///////////////////////////
/**
 * CBindingPropertyUnitDimDefault
 */
CBindingPropertyUnitDimDefault::CBindingPropertyUnitDimDefault() :
m_bDimSpeedUp( DIM_SPEED_DEFAULT ),
m_bDimSpeedDown( DIM_SPEED_DEFAULT ),
m_bDimMaxRange( DIM_RANGE_MAX_DEFAULT ),
m_bDimMinRange( DIM_RANGE_MIN_DEFAULT ),
m_fPirDelayedOn( FALSE ),
m_fPirDelayedOff( FALSE ),
m_fSolidState( FALSE )
{
	Calculate_Config( );
}

CBindingPropertyUnitDimDefault::CBindingPropertyUnitDimDefault(
								   BYTE bSpeedUP, BYTE bSpeedDown, BYTE bMaxRange, 
								   BYTE bMinRange, BOOL fDelayedOn, 
								   BOOL fDelayedOff, BOOL fSolidState) :
m_bDimSpeedUp( bSpeedUP ),
m_bDimSpeedDown( bSpeedDown ),
m_bDimMaxRange( bMaxRange ),
m_bDimMinRange( bMinRange ),
m_fPirDelayedOn( fDelayedOn ),
m_fPirDelayedOff( fDelayedOff ),
m_fSolidState( fSolidState )
{
	Calculate_Config( );					   
}

void CBindingPropertyUnitDimDefault::Calculate_Config( )
{
	m_bConfig = m_fPirDelayedOff << 7 |
	 		    m_fPirDelayedOn << 6 | 			  
			    m_fSolidState << 1;
}

CBindingPropertyUnitDimDefault::~CBindingPropertyUnitDimDefault() { }

const CBindingPropertyUnitDimDefault* CBindingPropertyUnitDimDefault::
Instance()
{
	static CBindingPropertyUnitDimDefault _instance;
	return &_instance;
}



/////////////////////////////VERSION V82//////////////////////////////
/**
 * CBindingPropertyUnitDimDefault
 */
VERSION_V82::CBindingPropertyUnitDimDefault::CBindingPropertyUnitDimDefault() :
::CBindingPropertyUnitDimDefault( DIM_SPEED_UP_V82_DEFAULT, DIM_SPEED_DOWN_V82_DEFAULT, 
								  DIM_RANGE_MAX_DEFAULT,DIM_RANGE_MIN_DEFAULT, 
								  TRUE, FALSE, FALSE)
{
	
}

VERSION_V82::CBindingPropertyUnitDimDefault::~CBindingPropertyUnitDimDefault() { }

const VERSION_V82::CBindingPropertyUnitDimDefault* VERSION_V82::CBindingPropertyUnitDimDefault::
Instance()
{
	static VERSION_V82::CBindingPropertyUnitDimDefault _instance;
	return &_instance;
}

// Sedert versie 0641
///////////////////////////VERSION_V90_DALIDMX/////////////////////////////
/**
 * CBindingPropertyUnitDimDefault
 */
VERSION_V90_DALIDMX::CBindingPropertyUnitDimDefault::CBindingPropertyUnitDimDefault() :
::CBindingPropertyUnitDimDefault( DIM_SPEED_DEFAULT, DIM_SPEED_DEFAULT, 
								  DIM_RANGE_MAX_DEFAULT,DIM_RANGE_MIN_DEFAULT, 
								  FALSE, FALSE, FALSE)
{	
}

VERSION_V90_DALIDMX::CBindingPropertyUnitDimDefault::~CBindingPropertyUnitDimDefault() { }

const VERSION_V90_DALIDMX::CBindingPropertyUnitDimDefault* 
VERSION_V90_DALIDMX::CBindingPropertyUnitDimDefault::
Instance()
{
	static VERSION_V90_DALIDMX::CBindingPropertyUnitDimDefault _instance;
	return &_instance;
}


///////////////////////////////////////////////////////////////////////
/**
 * CBindingPropertyUnitSwitchDefault
 */
CBindingPropertyUnitSwitchDefault::CBindingPropertyUnitSwitchDefault() :
m_bTimeValue( PRECISION_TIME_DEFAULT ),
m_fTimeEnabled( FALSE )
{
	
}
CBindingPropertyUnitSwitchDefault::~CBindingPropertyUnitSwitchDefault() { }

const CBindingPropertyUnitSwitchDefault* CBindingPropertyUnitSwitchDefault::
Instance()
{
	static CBindingPropertyUnitSwitchDefault _instance;
	return &_instance;
}

///////////////////////////////////////////////////////////////////////
/**
 * CBindingPropertyUnitDuoswitchDefault
 */
CBindingPropertyUnitDuoswitchDefault::CBindingPropertyUnitDuoswitchDefault() :
m_bStopTime( DUOSWITCH_DEFAULT_STOPTIME ), m_bSwitchTime( DUOSWITCH_DEFAULT_SWITCHTIME) { }

CBindingPropertyUnitDuoswitchDefault::CBindingPropertyUnitDuoswitchDefault(BYTE bStopTime,BYTE bSwitchTime) :
m_bStopTime(bStopTime), m_bSwitchTime(bSwitchTime) { } 

CBindingPropertyUnitDuoswitchDefault::~CBindingPropertyUnitDuoswitchDefault() { }

const CBindingPropertyUnitDuoswitchDefault* CBindingPropertyUnitDuoswitchDefault::
Instance()
{
	static CBindingPropertyUnitDuoswitchDefault _instance;
	return &_instance;
}


// Voor de nxp modules vanaf V90.25
namespace VERSION_V9025
{
	CBindingPropertyUnitDuoswitchDefault::CBindingPropertyUnitDuoswitchDefault( ) :
		Base( DUOSWITCH_DEFAULT_STOPTIME, DUOSWITCH_DEFAULT_SWITCHTIME_V9025) { }
		
	CBindingPropertyUnitDuoswitchDefault::~CBindingPropertyUnitDuoswitchDefault() { }

	const CBindingPropertyUnitDuoswitchDefault* CBindingPropertyUnitDuoswitchDefault::Instance() {
		static CBindingPropertyUnitDuoswitchDefault _instance;
		return &_instance;
	}	
}; /*  end namespace */

/*******************************************************************************/
/*********************** CBindingPropertyUnitSensDefault ***********************/
/*******************************************************************************/
CBindingPropertyUnitSensDefault::CBindingPropertyUnitSensDefault() :
m_bSensType( 0 ),
m_bSampleTime( SENS_INTERVAL_DEFAULT )					
{
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_SUN ] =		SENS_HYSVAL_DEFAULT_SUN;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_SUN ]=		SENS_HYSVAL_DEFAULT_SUN;
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_HSUN ]=		SENS_HYSVAL_DEFAULT_HSUN;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_HSUN ]=		SENS_HYSVAL_DEFAULT_HSUN;
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_MOON ]=		SENS_HYSVAL_DEFAULT_MOON;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_MOON ]=		SENS_HYSVAL_DEFAULT_MOON;
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_HMOON ] =		SENS_HYSVAL_DEFAULT_HMOON;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_HMOON ] =	SENS_HYSVAL_DEFAULT_HMOON;

	m_bHysOffsetValues[ HYST_OFFSET_HIGH ] =			SENS_HYSOFFSET_DEFAULT;
	m_bHysOffsetValues[ HYST_OFFSET_LOW ] =				SENS_HYSOFFSET_DEFAULT;

	m_sMeasuredValOffset = SENS_OFFSETMEASUREDVAL_DEFAULT;	/* TM20120405 */

	m_setpointRangeMin = 0;
	m_setpointRangeMax = TEMP_MAX;

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KP] = 80;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KI] = 40;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KD] = 20;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KP] = 80;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KI] = 40;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KD] = 20;
}

CBindingPropertyUnitSensDefault::CBindingPropertyUnitSensDefault(
	BYTE bHysOffsetValuesHigh,BYTE m_bHysOffsetValuesLow,
	BYTE bHysValuesSunHigh,BYTE bHysValuesSunLow,
	BYTE bHysValuesHSunHigh,BYTE bHysValuesHSunLow,
	BYTE bHysValuesMoonHigh,BYTE bHysValuesMoonLow,
	BYTE bHysValuesHMoonHigh,BYTE bHysValuesHMoonLow) :
	m_bSensType( 0 ),
	m_bSampleTime( SENS_INTERVAL_DEFAULT ),
	m_sMeasuredValOffset(SENS_OFFSETMEASUREDVAL_DEFAULT)
{
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_SUN ] = bHysValuesSunLow;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_SUN ] = bHysValuesSunHigh;
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_HSUN ] = bHysValuesHSunLow;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_HSUN ] =	bHysValuesHSunHigh;
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_MOON ] = bHysValuesMoonLow;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_MOON ] = bHysValuesMoonHigh;
	m_bHysValues[ HYST_OFFSET_LOW ][ MODE_HMOON ] = bHysValuesHMoonLow;
	m_bHysValues[ HYST_OFFSET_HIGH ][ MODE_HMOON ] = bHysValuesHMoonHigh;

	m_bHysOffsetValues[ HYST_OFFSET_HIGH ] = bHysOffsetValuesHigh;
	m_bHysOffsetValues[ HYST_OFFSET_LOW ] = m_bHysOffsetValuesLow;

	m_setpointRangeMin = 0;
	m_setpointRangeMax = TEMP_MAX;

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KP] = 80;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KI] = 40;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KD] = 20;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KP] = 80;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KI] = 40;
	m_pidParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KD] = 20;
}

CBindingPropertyUnitSensDefault::~CBindingPropertyUnitSensDefault() { }

const CBindingPropertyUnitSensDefault* CBindingPropertyUnitSensDefault::
Instance()
{
	DEBUG_TRACE("CBindingPropertyUnitSensDefault::Instance()-Enter\n");
	static CBindingPropertyUnitSensDefault _instance;	
	DEBUG_TRACE("CBindingPropertyUnitSensDefault::Instance()-Exit\n");
	return &_instance;
}


// Sedert V16.00: USE_SENSOR_PIDCONTROL	
int CBindingPropertyUnitSensDefault::getPIDControlParam(int mode,int param) const {
	if (mode<0 || mode>=SENS_PIDCONTROL_WORKINGMODE_MAX) {
		ASSERT(mode>=0 && mode<SENS_PIDCONTROL_WORKINGMODE_MAX);
		return -1;
	}
	if (param<0 || param>=SENS_PIDCONTROL_PARAM_MAX) {
		ASSERT(param>=0 && param<SENS_PIDCONTROL_PARAM_MAX);
		return -1;
	}
	return m_pidParams[mode][param];
}
/*******************************************************************************/

/*******************************************************************************/
/********************* VERSION_V2::CBindingPropertyUnitSensDefault *************/
/*******************************************************************************/
VERSION_V2::CBindingPropertyUnitSensDefault::CBindingPropertyUnitSensDefault() :
::CBindingPropertyUnitSensDefault(					
	SENS_HYSOFFSET_DEFAULT_V2,SENS_HYSOFFSET_DEFAULT_V2,
	SENS_HYSVAL_DEFAULT_V2_SUN,SENS_HYSVAL_DEFAULT_V2_SUN,
	SENS_HYSVAL_DEFAULT_V2_HSUN,SENS_HYSVAL_DEFAULT_V2_HSUN,
	SENS_HYSVAL_DEFAULT_V2_MOON,SENS_HYSVAL_DEFAULT_V2_MOON,
	SENS_HYSVAL_DEFAULT_V2_HMOON,SENS_HYSVAL_DEFAULT_V2_HMOON)
{	
}

VERSION_V2::CBindingPropertyUnitSensDefault::~CBindingPropertyUnitSensDefault() { }

const VERSION_V2::CBindingPropertyUnitSensDefault* VERSION_V2::CBindingPropertyUnitSensDefault::
Instance()
{
	DEBUG_TRACE("VERSION_V2::CBindingPropertyUnitSensDefault::Instance()-Enter\n");
	static VERSION_V2::CBindingPropertyUnitSensDefault _instance;

	DEBUG_TRACE("VERSION_V2::CBindingPropertyUnitSensDefault::Instance()-Exit\n");
	return &_instance;
}
/*******************************************************************************/

VERSION_V3::CBindingPropertyUnitSensDefault::CBindingPropertyUnitSensDefault() :
::CBindingPropertyUnitSensDefault(											
	SENS_HYSOFFSET_DEFAULT_V2,SENS_HYSOFFSET_DEFAULT_V2,
	SENS_HYSVAL_DEFAULT_V2_SUN,SENS_HYSVAL_DEFAULT_V2_SUN,
	SENS_HYSVAL_DEFAULT_V2_HSUN,SENS_HYSVAL_DEFAULT_V2_HSUN,
	SENS_HYSVAL_DEFAULT_V2_MOON,SENS_HYSVAL_DEFAULT_V2_MOON,
	SENS_HYSVAL_DEFAULT_V2_HMOON,SENS_HYSVAL_DEFAULT_V2_HMOON)
{	
}

VERSION_V3::CBindingPropertyUnitSensDefault::~CBindingPropertyUnitSensDefault() { }

const VERSION_V3::CBindingPropertyUnitSensDefault* VERSION_V3::CBindingPropertyUnitSensDefault::
Instance(int idx)
{
	DEBUG_TRACE("VERSION_V3::CBindingPropertyUnitSensDefault::Instance()-Enter\n");
	static VERSION_V3::CBindingPropertyUnitSensDefault _instance;

	_instance.setSampleTime(SENS_INTERVAL_DEFAULT+(3*idx));

	DEBUG_TRACE("VERSION_V3::CBindingPropertyUnitSensDefault::Instance()-Exit\n");
	return &_instance;
}


// Versie 4: 
VERSION_V4::CBindingPropertyUnitSensDefault::CBindingPropertyUnitSensDefault( ) :
::CBindingPropertyUnitSensDefault(											
	SENS_HYSOFFSET_DEFAULT_V2,SENS_HYSOFFSET_DEFAULT_V2,
	SENS_HYSVAL_DEFAULT_V2_SUN,SENS_HYSVAL_DEFAULT_V2_SUN,
	SENS_HYSVAL_DEFAULT_V2_HSUN,SENS_HYSVAL_DEFAULT_V2_HSUN,
	SENS_HYSVAL_DEFAULT_V2_MOON,SENS_HYSVAL_DEFAULT_V2_MOON,
	SENS_HYSVAL_DEFAULT_V2_HMOON,SENS_HYSVAL_DEFAULT_V2_HMOON) { }

VERSION_V4::CBindingPropertyUnitSensDefault::~CBindingPropertyUnitSensDefault() { }

const VERSION_V4::CBindingPropertyUnitSensDefault* VERSION_V4::CBindingPropertyUnitSensDefault::
Instance(int revisionNumber,int idx)
{
	DEBUG_TRACE("VERSION_V4::CBindingPropertyUnitSensDefault::Instance()-Enter\n");
	static VERSION_V4::CBindingPropertyUnitSensDefault _instance;
	_instance.setSampleTime(SENS_INTERVAL_DEFAULT + (3 * idx));		
	_instance.setSetpointRange(SENS_DEFAULT_SP_RANGE_MIN, SENS_DEFAULT_SP_RANGE_MAX);
	if (revisionNumber >= 1) {
		// Bij de NXP firmware werd de Hysteris waarden verlaagd van 1 naar 0,5C
		_instance.m_bHysOffsetValues[HYST_OFFSET_HIGH] = SENS_HYSOFFSET_DEFAULT_V5;
		_instance.m_bHysOffsetValues[HYST_OFFSET_LOW] = SENS_HYSOFFSET_DEFAULT_V5;
	}
	DEBUG_TRACE("VERSION_V4::CBindingPropertyUnitSensDefault::Instance()-Exit\n");
	return &_instance;
}

/************************************************************************************************/

// V5
// - Alle units offset waarden voor de 4 modes offset hysteris waarden op 0.
// - Extra setpoint range std op 12 tem 30C.
// - Hysteris offset waarden op 0,5°C
// - Sample interval op 100, 102 sec.
VERSION_V5::CBindingPropertyUnitSensDefault::CBindingPropertyUnitSensDefault() :
::CBindingPropertyUnitSensDefault(											
	SENS_HYSOFFSET_DEFAULT_V5,SENS_HYSOFFSET_DEFAULT_V5,
	SENS_HYSVAL_DEFAULT_V2_SUN,SENS_HYSVAL_DEFAULT_V2_SUN,
	SENS_HYSVAL_DEFAULT_V2_HSUN,SENS_HYSVAL_DEFAULT_V2_HSUN,
	SENS_HYSVAL_DEFAULT_V2_MOON,SENS_HYSVAL_DEFAULT_V2_MOON,
	SENS_HYSVAL_DEFAULT_V2_HMOON,SENS_HYSVAL_DEFAULT_V2_HMOON)
{	
}

VERSION_V5::CBindingPropertyUnitSensDefault::~CBindingPropertyUnitSensDefault() { }

const VERSION_V5::CBindingPropertyUnitSensDefault* VERSION_V5::CBindingPropertyUnitSensDefault::
Instance(int idx)
{
	DEBUG_TRACE("VERSION_V5::CBindingPropertyUnitSensDefault::Instance()-Enter\n");
	static VERSION_V5::CBindingPropertyUnitSensDefault _instance;

	// In de firmware van de RPI kunnen we niet bepalen welke index de unit heeft.
	// Dit wordt gedaan door een foefel.
	// Ifv de index wordt de waarde 100 of 102. (Bij de smartbox ultra U00=100, U01=102)
	static const int values[2] = { 100, 102 };	
	_instance.setSampleTime(values[idx%2]);

	// Version V4 - Setpoint range.
	_instance.setSetpointRange(SENS_DEFAULT_SP_RANGE_MIN,SENS_DEFAULT_SP_RANGE_MAX);

	DEBUG_TRACE("VERSION_V5::CBindingPropertyUnitSensDefault::Instance()-Exit\n");
	return &_instance;
}

/************************************************************************************************/
// Sedert versie 0x0D00: Implementatie van de universele sensor unit.

CBindingPropertyUnitSensUnivDefault::CBindingPropertyUnitSensUnivDefault( ) :
CBindingPropertyUnitSensDefault( 
	SENSUNIV_HYSOFFSET_DEFAULT, SENSUNIV_HYSOFFSET_DEFAULT,	
	SENSUNIV_HYSVAL_DEFAULT_SUN,SENSUNIV_HYSVAL_DEFAULT_SUN,
	SENSUNIV_HYSVAL_DEFAULT_HSUN,SENSUNIV_HYSVAL_DEFAULT_HSUN,
	SENSUNIV_HYSVAL_DEFAULT_MOON,SENSUNIV_HYSVAL_DEFAULT_MOON,
	SENSUNIV_HYSVAL_DEFAULT_HMOON,SENSUNIV_HYSVAL_DEFAULT_HMOON)
{
}

CBindingPropertyUnitSensUnivDefault::~CBindingPropertyUnitSensUnivDefault( ) { }

const CBindingPropertyUnitSensUnivDefault* 
CBindingPropertyUnitSensUnivDefault::Instance(int idx)
{
	DEBUG_TRACE("CBindingPropertyUnitSensUnivDefault::Instance()-Enter\n");
	static CBindingPropertyUnitSensUnivDefault _instance;

	_instance.setSampleTime(SENSUNIV_INTERVAL_DEFAULT+(3*idx));

	DEBUG_TRACE("CBindingPropertyUnitSensUnivDefault::Instance()-Exit\n");
	return &_instance;
};

///////////////////////////////////////////////////////////////////////
CBindingPropertyUnitAlarmDefault::CBindingPropertyUnitAlarmDefault() :
m_bAlarmCentraleType( ALARM_TYPE_CONFIG_GALAXY60 )
{
	DEBUG_TRACE("CBindingPropertyUnitAlarmDefault::ctor()\n");

	m_bRemoteCode[0] = 6;
	m_bRemoteCode[1] = 5;
	m_bRemoteCode[2] = 4;
	m_bRemoteCode[3] = 3;
	m_bRemoteCode[4] = 2;
	m_bRemoteCode[5] = 1;
	m_bRemoteCode[6] = 0;
	m_bRemoteCode[7] = 0;
}

CBindingPropertyUnitAlarmDefault::CBindingPropertyUnitAlarmDefault(BYTE bAlarmCentraleType) :
m_bAlarmCentraleType( bAlarmCentraleType )
{

}

CBindingPropertyUnitAlarmDefault::~CBindingPropertyUnitAlarmDefault()
{

}

const CBindingPropertyUnitAlarmDefault* CBindingPropertyUnitAlarmDefault::
Instance()
{
	DEBUG_TRACE("CBindingPropertyUnitAlarmDefault::Instance()-Enter\n");
	static CBindingPropertyUnitAlarmDefault _instance;

#if (DEBUG_LOCAL_ON == 1)
	for (int i=0;i<_instance.m_bRemoteCode[0];i++)
	{
		DEBUG_TRACE_2("%c",_instance.m_bRemoteCode[i+1]+0x30);
	}
#endif

	DEBUG_TRACE("\nCBindingPropertyUnitAlarmDefault::Instance()-Exit\n");
	return &_instance;
}

////////////////////////////////////////////////////////////////////////////

VERSION_V2::CBindingPropertyUnitAlarmDefault::CBindingPropertyUnitAlarmDefault() :
::CBindingPropertyUnitAlarmDefault(ALARM_GALAXY_TYPE_PREMIER44)
{	
	DEBUG_TRACE("VERSION_V2::CBindingPropertyUnitAlarmDefault::ctor()\n");

	m_bRemoteCode[0] = 4;
	m_bRemoteCode[1] = 1;
	m_bRemoteCode[2] = 2;
	m_bRemoteCode[3] = 3;
	m_bRemoteCode[4] = 4;
	m_bRemoteCode[5] = 0;
	m_bRemoteCode[6] = 0;
	m_bRemoteCode[7] = 0;
}

VERSION_V2::CBindingPropertyUnitAlarmDefault::~CBindingPropertyUnitAlarmDefault()
{

}

const VERSION_V2::CBindingPropertyUnitAlarmDefault* VERSION_V2::CBindingPropertyUnitAlarmDefault::
Instance()
{
	DEBUG_TRACE("VERSION_V2::CBindingPropertyUnitAlarmDefault::Instance()-Enter\n");
	static VERSION_V2::CBindingPropertyUnitAlarmDefault _instance;

#if (DEBUG_LOCAL_ON == 1)
	for (int i=0;i<_instance.m_bRemoteCode[0];i++)
	{
		DEBUG_TRACE_2("%c",_instance.m_bRemoteCode[i+1]+0x30);
	}
#endif
	
	DEBUG_TRACE("\nVERSION_V2::CBindingPropertyUnitAlarmDefault::Instance()-Exit\n");
	return &_instance;
}

////////////////////////////////////////////////////////////////////////////