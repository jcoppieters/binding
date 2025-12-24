// BindingPropertyUnit.h: interface for the CBindingPropertyUnit class.
//
// Changes: TM20120402: Software versie 0x0600
// Changes: TM20120402: CBindingPropertySensUnit private members
// Changes: TM20160315: CBindingPropertySensUnit - Principe van delegation
// Changes: TM20160317: Extra var Supported flags.
// Changes: TM20160406: Switch & Duoswitch unit gebruik van defaults.
// Changes: TM20160406: Var fMakeNoDefaults toegevoegd.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGPROPERTYUNIT_H__B1236F41_A281_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGPROPERTYUNIT_H__B1236F41_A281_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "BindingUnit.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
#include "nodegenestructs.h"


/////////////////////////////////////////////////////////////////////////////
class CBindingPropertyUnitDimDefault;
class CBindingPropertyUnitSwitchDefault;
class CBindingPropertyUnitDuoswitchDefault;
class CBindingPropertyUnitSensDefault;
class CBindingPropertyUnitAlarmDefault;

class CBindingConversionToNodesProperty;

//-------------------------------------------------------------------------//
/////////////////////// CBindingPropertyUnit ////////////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyUnit  
{
public:
	CBindingPropertyUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

	virtual ~CBindingPropertyUnit();

	virtual void SetToDefaults(void) = 0;

	// Sedert versie 0x0902: 
	// var fMakeNoDefaults: Indien TRUE dan worden er geen bindings aangemaakt 
	// voor de default bindings.
	BYTE ConstructPBindings(
		CBindingConversionToNodesProperty *ptr,
		BOOL fMakeNoDefaults);


	// TM20120402: Geen set functies voor Node en Unit address.
	// Wordt meegegeven in de ctor en via de default operator= 
/*
	void SetNodeAddress(BYTE bAddress) {
		m_bNodeAddress = bAddress; 
	}
	void SetUnitAddress(BYTE bAddress){
		m_bUnitAddress = bAddress; 
	}
*/
	BYTE GetNodeAddress(void) const	{
		return m_bNodeAddress; 
	}
	BYTE GetUnitAddress(void) const {
		return m_bUnitAddress; 
	}
	BYTE GetBindingUnitType() const {
		return m_bType;
	}
	unsigned short GetSupportedFlags() const {
		return m_usSupportedFlags;
	}
	void SetSupportedFlags(unsigned short flags) {
		m_usSupportedFlags=flags;
	}

private:	
	BYTE m_bType;						// Wordt meegeven in de constructor BINDING_UNIT_TYPE_XXX - Kan niet constant zijn omwille van operator=				
	BYTE m_bNodeAddress;				// Wordt meegeven in de constructor - Kan niet constant zijn omwille van operator=
	BYTE m_bUnitAddress;				// Wordt meegeven in de constructor - Kan niet constant zijn omwille van operator=
	unsigned short m_usSupportedFlags;	// Wordt meegeven in de constructor - Kan niet constant zijn omwille van operator= Is Specifeik voor ieder unittype.

protected:
	CBindingConversionToNodesProperty *pConversionToNodes;

protected:
	BOOL MakePropertyBinding(CString *szBinding);
	BOOL AddToNodes(CString szString);

protected:
	virtual BYTE MakePropertyBindings(BOOL fMakeNoDefaults) = 0;
};

//-------------------------------------------------------------------------//
///////////////////// CBindingPropertyDimUnit ///////////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyDimUnit : public CBindingPropertyUnit
{
protected:
	const CBindingPropertyUnitDimDefault* m_pDefaults;

public:
	enum
	{	
		// DALI Device Address - Enkel visualisatie.
		DIM_SUPPORT_FLAGS_DALI_DEVICE			= (0x01<<0),

		// DALI Group Address - Slave toekennen aan de groep.
		DIM_SUPPORT_FLAGS_DALI_GROUP			= (0x01<<1),

/*
		// De properties worden geconfigureerd in de DALI configuratie.
		DIM_SUPPORT_FLAGS_DALI_PROPERTIES		= (0x01<<2)
*/
	};

	enum
	{
		ADDRESS_CFG_TYPE_DALI_GROUP				= 0x00,		// btype..
		ADDRESS_CFG_TYPE_NONE 					= 0xFF
	};

public:
	CBindingPropertyDimUnit(
		BYTE bType,
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

	void SetToDefaults(void);

	void UpdateSpeed(BYTE speedDown,BYTE speedUp);
	void UpdateRange(BYTE Min,BYTE Max);
	void UpdateConfig(BOOL DelayedOn,BOOL DelayedUp,BOOL SolidState);
	void UpdateConfig(BYTE bConfig);
	void UpdateCfgAddress(BYTE bType,BYTE bNodeAddress,BYTE bUnitAddress);

	void set_DelayedOn(BOOL flag)		{ m_fPirDelayedOn = flag; };
	void set_DelayedOff(BOOL flag)		{ m_fPirDelayedOff = flag; };
	void set_SolidState(BOOL flag)		{ m_fSolidState = flag; };
	void set_DimSpeedUp(BYTE val)		{ m_bDimSpeedUp = val; }
	void set_DimSpeedDown(BYTE val)		{ m_bDimSpeedDown = val; }
	void set_DimMaxRange(BYTE val)		{ m_bDimMaxRange = val; }
	void set_DimMinRange(BYTE val)		{ m_bDimMinRange = val; }
	void set_AddressCfgType(int val)	{ ASSERT(val>=0 && val<=0xFF); m_addressCfg.bType = val; };
	void set_AddressCfgMSB(BYTE msb)	{ m_addressCfg.bAddressMSB = msb; };
	void set_AddressCfgLSB(BYTE lsb)	{ m_addressCfg.bAddressLSB = lsb; };


	BOOL get_DelayedOn() const			{ return m_fPirDelayedOn;};
	BOOL get_DelayedOff() const			{ return m_fPirDelayedOff;};
	BOOL get_SolidState() const			{ return m_fSolidState; };
	BYTE get_DimSpeedUp() const			{ return m_bDimSpeedUp; };
	BYTE get_DimSpeedDown() const		{ return m_bDimSpeedDown; };
	BYTE get_DimMaxRange() const		{ return m_bDimMaxRange; };
	BYTE get_DimMinRange() const		{ return m_bDimMinRange; };
	BYTE get_AddressCfgType() const		{ return m_addressCfg.bType; };
	BYTE get_AddressCfgMSB( ) const		{ return m_addressCfg.bAddressMSB; };
	BYTE get_AddressCfgLSB( ) const		{ return m_addressCfg.bAddressLSB; };

protected:
	BOOL MakeSpeedBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeRangeBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeConfigBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeAddressCfgBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults);

private:
	BOOL m_fPirDelayedOn;
	BOOL m_fPirDelayedOff;
	BOOL m_fSolidState;;
	BYTE m_bDimSpeedUp;
	BYTE m_bDimSpeedDown;
	BYTE m_bDimMaxRange;
	BYTE m_bDimMinRange;

	// Sedert versie 0x0B00
	struct
	{
		BYTE bType;
		BYTE bAddressMSB;
		BYTE bAddressLSB;

	}  m_addressCfg;
};


//-------------------------------------------------------------------------//
////////////////////// CBindingPropertySwitchUnit ///////////////////////////
//-------------------------------------------------------------------------//

#define PRECISION_TIME_MIN		1
#define PRECISION_TIME_MAX		120
#define PRECISION_TIME_DEFAULT	1

class CBindingPropertySwitchUnit : public CBindingPropertyUnit
{
protected:
	// Sedert 0x0902.
	const CBindingPropertyUnitSwitchDefault* m_pDefaults;

public:
	CBindingPropertySwitchUnit(
		BYTE bType,
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

	BYTE bTimeValue;			// Todo - Private maken
	BOOL fTimeEnabled;			// Todo - Private maken

public:
	void SetToDefaults(void);
	void UpdatePrecisionTiming(BOOL fEnable,BYTE TimeValue =  PRECISION_TIME_DEFAULT);

protected:
	BOOL MakePrecisionTimeBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults);
};


//-------------------------------------------------------------------------//
//////////////////// CBindingPropertyDuoSwitchUnit //////////////////////////
//-------------------------------------------------------------------------//

#define DUOSWITCH_TIMER_MINVALUE				1
#define DUOSWITCH_TIMER_MAXVALUE				255

#define DUOSWITCH_DEFAULT_STOPTIME				100
#define DUOSWITCH_DEFAULT_SWITCHTIME			2
#define DUOSWITCH_DEFAULT_SWITCHTIME_V9025		20	// Voor de nxp modules is de std switch tijd 1200 msec.


// Sedert V14.12: Ondersteuning stoptijd waarde 0.
// Implementatie via set/get functies.
class CBindingPropertyDuoSwitchUnit : public CBindingPropertyUnit
{
protected:
	// Sedert 0x0902
	const CBindingPropertyUnitDuoswitchDefault* m_pDefaults;

public:
	CBindingPropertyDuoSwitchUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);
	
	void SetToDefaults(void);
	void UpdateStopTime(BYTE value);
	void UpdateSwitchTime(BYTE value);

	BYTE getStopTime() const { return m_bStopTime; }
	void setStopTime(BYTE stopTime);
	BYTE getSwitchTime() const { return m_bSwitchTime; }
	void setSwitchTime(BYTE switchTime);
	BYTE getStopTimeMaxValue( ) const { return DUOSWITCH_TIMER_MAXVALUE; }
	BYTE getStopTimeMinValue( ) const;	// Afhankelijk van de supported vlaggen....
	BYTE getSwitchTimeMaxValue( ) const { return DUOSWITCH_TIMER_MAXVALUE; };
	BYTE getSwitchTimeMinValue( ) const { return DUOSWITCH_TIMER_MINVALUE; };
	
protected:
	BOOL MakeStopTimeBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeSwitchTimeBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults);

private:
	BYTE m_bStopTime;		
	BYTE m_bSwitchTime;		
};


//-------------------------------------------------------------------------//
//////////////////////// CBindingPropertySensUnit ///////////////////////////
//-------------------------------------------------------------------------//

#define SENS_INTERVAL_MINVALUE			2
#define SENS_INTERVAL_MAXVALUE			120
#define SENS_INTERVAL_DEFAULT			20

#define SENS_HYSVAL_MINVALUE			0
#define SENS_HYSVAL_MAXVALUE			250
#define SENS_HYSVAL_STEP				1

#define SENS_HYSVAL_DEFAULT_SUN			0
#define SENS_HYSVAL_DEFAULT_HSUN		50
#define SENS_HYSVAL_DEFAULT_MOON		100
#define SENS_HYSVAL_DEFAULT_HMOON		150

#define SENS_HYSVAL_DEFAULT_V2_SUN		0		// Nieuwe waarden op 0
#define SENS_HYSVAL_DEFAULT_V2_HSUN		0		// Nieuwe waarden op 0
#define SENS_HYSVAL_DEFAULT_V2_MOON		0		// Nieuwe waarden op 0
#define SENS_HYSVAL_DEFAULT_V2_HMOON	0		// Nieuwe waarden op 0

#define SENS_HYSOFFSET_MINVALUE			0
#define SENS_HYSOFFSET_MAXVALUE			250
#define SENS_HYSOFFSET_STEP				1
#define SENS_HYSOFFSET_DEFAULT			10
#define SENS_HYSOFFSET_DEFAULT_V2		10		// Zie BUG-0154: was 0, moet 10 worden.
#define SENS_HYSOFFSET_DEFAULT_V5		5		// Sedert V66.13.06: 0,5C

#define SENS_OFFSETMEASUREDVAL_MIN		-50		/* TM20120402 */
#define SENS_OFFSETMEASUREDVAL_MAX		50		/* TM20120402 */
#define SENS_OFFSETMEASUREDVAL_STEP		1		/* TM20120402 */
#define SENS_OFFSETMEASUREDVAL_DEFAULT	0

///////////////////////////////////////////////
// Sedert versie 0x0D00
enum
{
	SENSUNIV_INTERVAL_DEFAULT		= 20,	// 20 seconden.
	SENSUNIV_HYSOFFSET_DEFAULT		= 100,	// komt overeen met 10%
	SENSUNIV_HYSVAL_DEFAULT_SUN		= 0,
	SENSUNIV_HYSVAL_DEFAULT_HSUN	= 0,		
	SENSUNIV_HYSVAL_DEFAULT_MOON	= 0,	
	SENSUNIV_HYSVAL_DEFAULT_HMOON	= 0,	
};
///////////////////////////////////////////////

// Sedert V16.00: USE_SENSOR_PIDCONTROL
// Workingsmodes: Cooling & heating.
#define SENS_PIDCONTROL_WORKINGMODE_NONE		-1
#define SENS_PIDCONTROL_WORKINGMODE_COOLING		0
#define SENS_PIDCONTROL_WORKINGMODE_HEATING		1
#define SENS_PIDCONTROL_WORKINGMODE_MAX			2

// De PID params per workingsmode: pid.
#define SENS_PIDCONTROL_PARAM_KP				0
#define SENS_PIDCONTROL_PARAM_KI				1
#define SENS_PIDCONTROL_PARAM_KD				2
#define SENS_PIDCONTROL_PARAM_MAX				3

///////////////////////////////////////////////
#define HYST_OFFSET_HIGH	0
#define HYST_OFFSET_LOW		1	

#define MODE_SUN			0
#define MODE_HSUN			1
#define MODE_MOON			2
#define MODE_HMOON			3


class CBindingPropertySensUnit : public CBindingPropertyUnit
{
/*
protected:
	const CBindingPropertyUnitSensDefault* m_pDefaults;
*/
public:

	// Delegation is opgesplits in 2 delen.
	// - Delegation: Delegator (Sensor van de Sbox+) & Delegate (Sensor van de Room-e).
	// - Request Measured Value: Sensor van de Modbus
	//
	// Deze vlaggen bepalen wat er ondersteund wordt van delegation.
	// Mogelijke combinaties zijn:
	//	- SENS_SUPPORT_FLAGS_DELEGATION + SENS_SUPPORT_FLAGS_ATTACH_DELEGATE
	//  - SENS_SUPPORT_FLAGS_DELEGATION + SENS_SUPPORT_FLAGS_ATTACH_DELEGATOR
	//  - SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE
	//
	enum
	{
		// Ondersteund delegation.
		SENS_SUPPORT_FLAGS_DELEGATION		= (0x01<<0),

		// Deze vlag is gezet bij de sensor van de delegator. (Sbox+)
		// Dit is de delegate die het werk uitvoert.
		SENS_SUPPORT_FLAGS_ATTACH_DELEGATE	= (0x01<<1),

		// Deze vlag is gezet bij de sensor van de delegate.  (Oleds)
		// Dit is de delegator die de delegate het werk doet uitvoeren.
		SENS_SUPPORT_FLAGS_ATTACH_DELEGATOR	= (0x01<<2),

		// Dit is een sensor die de gemeten waarde van een andere sensor gebruikt.
		SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE = (0x01<<3),	// Sedert versie 0x0c02

		// Sedert 0x0E00: Setpoint waarde begrenzen tussen een min en max waarde.
		SENS_SUPPORT_FLAGS_SETPOINTRANGE = (0x01<<4), 

		// Sedert V16.00: USE_SENSOR_PIDCONTROL
		// PID Control voor de sens units met versie V90.26.00
		SENS_SUPPORT_FLAGS_PIDCONTROL = (0x01 << 5)

	}; /* enum */

public:
	CBindingPropertySensUnit(BYTE bType,BYTE bNodeAddress, BYTE bUnitAddress,unsigned short usSupportedFlags = 0);
	void SetToDefaults(void);
	void UpdateType(BYTE type);
	void UpdateHysValues(BYTE bMode,BYTE bValHigh,BYTE bValLow);
	void UpdateOffsetValues(BYTE bValHigh,BYTE bValLow);
	void UpdateSampleTime(BYTE bValue);
	void UpdateMeasuredValueOffset(short sValue);			/* TM20120406 */
	void UpdateDelegation(int type,BYTE bNodeAddress,BYTE bUnitAddress);
	void updateSetpointRange(short minValue,short maxValue);

	BYTE GetSensType() const {
		return m_bSensType;
	}
	BYTE GetSampleTime() const {
		return m_bSampleTime;
	}
	BYTE GetHysOffsetHighValue() const {
		return m_bHysOffsetValues[HYST_OFFSET_HIGH];		// HYST_OFFSET_HIGH = 0
	}
	BYTE GetHysOffsetLowValue() const {
		return m_bHysOffsetValues[HYST_OFFSET_LOW];			// HYST_OFFSET_LOW = 1
	}
	BYTE GetHysHighValue(BYTE bMode) const {
		return m_bHysValues[HYST_OFFSET_HIGH][bMode];		// HYST_OFFSET_HIGH = 0
	}
	BYTE GetHysLowValue(BYTE bMode) const {
		return m_bHysValues[HYST_OFFSET_LOW][bMode];		// HYST_OFFSET_LOW = 1
	}
	BYTE GetHysValues(BYTE bOffset,BYTE bMode) {
		ASSERT( bOffset <= HYST_OFFSET_LOW );
		ASSERT( bMode <= 4);
		return m_bHysValues[bOffset][bMode];
	}
	BYTE GetHysOffsetValues(BYTE bOffset) {
		ASSERT( bOffset <= HYST_OFFSET_LOW );
		return m_bHysOffsetValues[bOffset];
	}
	short GetMeasuredValueOffset( ) const {					/* TM20120402 */
		return m_sMeasuredValOffset;
	}

	void SetSensType(BYTE bSensType) {
		m_bSensType = bSensType;
	}
	void SetSampleTime(BYTE bSampleTime) {
		m_bSampleTime = bSampleTime;
	}

	void SetHysOffsetHighValue(BYTE val);
	void SetHysOffsetLowValue(BYTE val);

	void SetHysHighValue(BYTE bMode,BYTE val) {
		m_bHysValues[HYST_OFFSET_HIGH][bMode] = val;		// HYST_OFFSET_HIGH = 0
	}
	void SetHysLowValue(BYTE bMode,BYTE val) {
		m_bHysValues[HYST_OFFSET_LOW][bMode] = val;			// HYST_OFFSET_LOW = 1
	}
	void SetHysValues(BYTE bOffset,BYTE bMode,BYTE val) {
		ASSERT( bOffset <= HYST_OFFSET_LOW );
		ASSERT( bMode <= 4);
		m_bHysValues[bOffset][bMode] = val;
	}
	void SetHysOffsetValues(BYTE bOffset,BYTE val);

	void SetMeasuredValueOffset(short sMeasuredValOffset) {	
		m_sMeasuredValOffset = sMeasuredValOffset;
	}

	// Sedert versie 0x0900: Delegation.
	BYTE GetDelegationType(void) const
	{
		return m_delegation.m_bType;
	}
	BYTE GetDelegationNodeAddress(void) const
	{
		return m_delegation.m_bNodeAddress;
	}
	BYTE GetDelegationUnitAddress(void) const
	{
		return m_delegation.m_bUnitAddress;
	}
	void SetDelegationType(int type)
	{
		REQUIRE(type>=0 && type<=0xFF);
		m_delegation.m_bType = (BYTE)type;
	}
	void SetDelegationNodeAddress(int nodeAddress)
	{
		REQUIRE(nodeAddress>=0 && nodeAddress<=0xFF);
		m_delegation.m_bNodeAddress = (BYTE)nodeAddress;
	}
	void SetDelegationUnitAddress(int unitAddress)
	{
		REQUIRE(unitAddress>=0 && unitAddress<=0xFF);
		m_delegation.m_bUnitAddress = (BYTE)unitAddress;
	}

	// Setpoint range.
	BOOL getSetpointRangeSupported() const {
		return (GetSupportedFlags() & SENS_SUPPORT_FLAGS_SETPOINTRANGE ? TRUE : FALSE);
	}
	short getSetpointRangeValueMin() const {
		return m_setpointRangeMin;
	}
	short getSetpointRangeValueMax() const {
		return m_setpointRangeMax;
	}
	void setSetpointRangeValueMin(short v) {
		m_setpointRangeMin = v;
	}
	void setSetpointRangeValueMax(short v) {
		m_setpointRangeMax = v;
	}

	// Sedert V16.00: USE_SENSOR_PIDCONTROL
	BOOL getPIDControlSupported() const {
		return (GetSupportedFlags() & SENS_SUPPORT_FLAGS_PIDCONTROL ? TRUE : FALSE);
	}
	void setPropertyPIDControlUnit(int workingmode,BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL getPropertyPIDControlUnit(int workingmode,BYTE* pbNodeAddress,BYTE* pbUnitAddress) const;	
	void setPropertyPIDParam(int workingmode,int param,int value);
	BYTE getPropertyPIDParam(int workingmode,int param) const;

protected:
	BOOL MakeHystOffsetValBindings(CString *szBinding,BYTE bMode,BOOL fMakeNoDefaults);
	BOOL MakeHystOffsetBindings(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeSensTypeBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeSampleIntervalBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeMeasuredValOffsetBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeDelegationBinding(CString* szBinding,BOOL fMakeNoDefaults);			// sedert versie 0x0900
	BOOL MakeSetpointRangeBinding(CString* szBinding,BOOL fMakeNoDefaults);			// sedert versie 0x0E00	
	BOOL MakePIDControlUnitBinding(CString *szBinding,int workingsmode, BOOL fMakeNoDefaults);	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	BOOL MakePIDControlParamsBinding(CString *szBinding,int workingsmode, BOOL fMakeNoDefaults); // Sedert V16.00: USE_SENSOR_PIDCONTROL	
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults);


public:
	// Dit is het type van delegation.
	// Voor dit type worden er property bindings aangemaakt.
	enum
	{
		DELEGATION_TYPE_DELEGATOR				= 0x00,
		DELEGATION_TYPE_DELEGATE				= 0x01,
		DELEGATION_TYPE_REQUESTVALUE			= 0x02,		// Sedert versie 0x0c02
		DELEGATION_TYPE_NONE 					= 0xFF
	};

private:

	// Sedert versie 0x0900
	struct UNIT_SENS_DELEGATION
	{
		BYTE m_bType;			// Type: Zie enum.
		BYTE m_bNodeAddress;
		BYTE m_bUnitAddress;		
	};

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	struct UNIT_SENS_PIDCONTROL{
		BYTE m_bNodeAddress;
		BYTE m_bUnitAddress;		
		BYTE m_params[SENS_PIDCONTROL_PARAM_MAX];
	};

	const CBindingPropertyUnitSensDefault* m_pDefaults;				// zie BUG-0179: Gekoppeld aan een singleton.
	BYTE m_bSensType;					// temp, lux, ...				
	BYTE m_bHysValues[2][4];			
	BYTE m_bHysOffsetValues[2];	
	BYTE m_bPrevHysOffsetValues[2];	
	BYTE m_bSampleTime;			
	short m_sMeasuredValOffset;		
	short m_setpointRangeMin;
	short m_setpointRangeMax;
	UNIT_SENS_DELEGATION m_delegation;
	UNIT_SENS_PIDCONTROL m_pidControl[SENS_PIDCONTROL_WORKINGMODE_MAX];	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
};

//-------------------------------------------------------------------------//
//////////////////// CBindingPropertyBasicAudioUnit /////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyBasicAudioUnit : public CBindingPropertyUnit
{
public:
	CBindingPropertyBasicAudioUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

public:
	void SetToDefaults(void);

protected:
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults) {return 0;};
};


//-------------------------------------------------------------------------//
////////////////////// CBindingPropertyBoseRsUnit ///////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyBoseRsUnit : public CBindingPropertyUnit
{
public:
	CBindingPropertyBoseRsUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);
	void SetToDefaults(void);

public:
	void SetConfig(BYTE bConfig);
	BYTE GetConfig( ) const;

protected:
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults) {return 0;};

private:
	BYTE m_bConfig;		// Temp : nodig om te bepalen welke type Bose systeem.
};

// inline functies 

inline void CBindingPropertyBoseRsUnit::SetConfig(BYTE bConfig)
{
	m_bConfig = bConfig;
}

inline BYTE CBindingPropertyBoseRsUnit::GetConfig( ) const
{
	return m_bConfig;
}

//-------------------------------------------------------------------------//
/////////////////////// CBindingPropertyIRTXUnit ////////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyIRTXUnit : public CBindingPropertyUnit
{

public:
	CBindingPropertyIRTXUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

public:
	void SetToDefaults(void);

protected:
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults) {return 0;};
};


//-------------------------------------------------------------------------//
/////////////////////// CBindingPropertyRC5RXUnit ///////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyRC5RXUnit : public CBindingPropertyUnit
{

public:
	CBindingPropertyRC5RXUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

public:
	void SetToDefaults(void);

protected:
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults) {return 0;};
};


//-------------------------------------------------------------------------//
///////////////////// CBindingPropertyControlUnit ///////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyControlUnit : public CBindingPropertyUnit
{
public:
	CBindingPropertyControlUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

public:
	void SetToDefaults(void);

protected:
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults) {return 0;};
};



//-------------------------------------------------------------------------//
///////////////////// CBindingPropertyVirtualUnit ///////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyVirtualUnit : public CBindingPropertyUnit
{
public:
	CBindingPropertyVirtualUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

public:
	void SetToDefaults(void);

protected:
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults) {return 0;};
};


//-------------------------------------------------------------------------//
//////////////////// CBindingPropertyAlarmUnit	/////////////////////////////
//-------------------------------------------------------------------------//

class CBindingPropertyAlarmUnit : public CBindingPropertyUnit
{
protected:
	const CBindingPropertyUnitAlarmDefault* m_pDefaults;

public:
	CBindingPropertyAlarmUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		unsigned short usSupportedFlags = 0);

public:
	void SetToDefaults(void);
	void UpdateType(BYTE bType);
	void UpdateRemotecode(const BYTE* pbData);	
	void UpdateRemotecode(int index,const BYTE bData);		// CHANGES_MVS_2008

	// TM20130328: Get & Set Functies.
	void SetRemoteCode(int index,BYTE bDigit);
	BYTE GetRemoteCode(int index) const;
	void SetAlarmCentraleType(BYTE bType);
	BYTE GetAlarmCentraleType( ) const;

protected:
	BOOL MakeRemotecodeBinding1(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeRemotecodeBinding2(CString *szBinding,BOOL fMakeNoDefaults);
	BOOL MakeConfigBinding(CString *szBinding,BOOL fMakeNoDefaults);
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults);

private:
	// TM20130328: Deze private gemaakt.
	BYTE bRemoteCode[ALARM_REMOTECODE_BUFFER_MAX];			
	BYTE bAlarmCentraleType;	
};


//-------------------------------------------------------------------------//
/////////////////// CBindingPropertyAVMatrixUnit	/////////////////////////
//-------------------------------------------------------------------------//
class CBindingPropertyAVMatrixUnit : public CBindingPropertyUnit
{
public:

	typedef enum {
		AVMATRIX_TYPE_UNKNOWN = 0,
		AVMATRIX_TYPE_AUDAC   = 1,
		AVMATRIX_TYPE_INTEGRA = 2
	} AVMatrixType_t;

	CBindingPropertyAVMatrixUnit(
		BYTE bType, 
		BYTE bNodeAddress, 
		BYTE bUnitAddress,
		AVMatrixType_t AVMatrixType = AVMATRIX_TYPE_UNKNOWN,
		unsigned short usSupportedFlags = 0);

	void SetToDefaults(void);
/*
	// Zie enum AVMatrixType_t
	BYTE getType() const;
	void setType(BYTE type);
*/
	AVMatrixType_t getMatrixType( ) const { return m_type; }
	void setMatrixType(AVMatrixType_t type) { m_type = type; }

	void UpdateSubType(BYTE bSubType);
	void SetSubType(BYTE bSubType);
	BYTE GetSubType( ) const;

protected:
	BYTE MakePropertyBindings(BOOL fMakeNoDefaults);
	BOOL MakeConfigBinding(CString *szBinding,BOOL fMakeNoDefaults);

private:
	AVMatrixType_t m_type;	// Basis family.
	BYTE m_subType;			// Subtype deze kan ingesteld worden...
};

///////////////////////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_BINDINGPROPERTYUNIT_H__B1236F41_A281_11D8_B865_0050BAC412B1__INCLUDED_)
