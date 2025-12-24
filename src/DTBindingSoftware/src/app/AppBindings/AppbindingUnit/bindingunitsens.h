#pragma once

#include "bindingunit.h"
#include "bindingunitsens_def.h"


class CBindingSensUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingSensUnit(BYTE bBindingUnitType);	
	CBindingSensUnit(const CBindingUnit& pBindingUnit);// Oppassen: Dit is geen COPY Constructor !!!!!

	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingSensUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

public:
	enum
	{
		DELEGATION_TYPE_DELEGATOR				= 0x00,
		DELEGATION_TYPE_DELEGATE				= 0x01,	
		DELEGATION_TYPE_REQUESTVALUE			= 0x02,	/* sedert versie 0x0C03 */
		DELEGATION_TYPE_NONE 					= 0xFF
	};

	// Sedert V16.00: USE_SENSOR_PIDCONTROL
	enum {
		PIDCONTROL_WORKINGMODE_COOLING = 0,
		PIDCONTROL_WORKINGMODE_HEATING = 1,
		PIDCONTROL_WORKINGMODE_MAX = 2,
	}; 

	// Sedert V16.00: USE_SENSOR_PIDCONTROL
	enum {
		PIDCONTROL_PARAM_KP = 0,
		PIDCONTROL_PARAM_KI = 1,
		PIDCONTROL_PARAM_KD = 2,
		PIDCONTROL_PARAM_MAX =3,
	};

protected:
	void InitDelegation(void);
	void InitSensorExtended(void);
	void InitSensorsOutput(void);
	void InitSensorInput(void);
	void InitSensorUsingDefaults(void);

protected:
	BYTE bSensEvent;
	BYTE bFunctionData;	
	BYTE bSensEventState;		// On,Off (heating,cooling)	
	BYTE bPreset;
	BYTE bState;	//On,Off
	BYTE bSensType;
	BYTE bHysLowValue[4];
	BYTE bHysHighValue[4];	
	BYTE bHysLowOffsetValue;
	BYTE bHysHighOffsetValue;
	BYTE bSampleTime;
	unsigned short usSetpoint;
	short sMeasuredValOffset;		
	short m_setpointRangeMin;	
	short m_setpointRangeMax;

	// Extended sensor unit
	struct
	{
		unsigned short usAngle;			/* TM20120917 */ // 0 tem 360° ?
		BYTE bWorkingMode;				/* TM20120917 */ // off,auto,cooling,heating,...
		BYTE bFanSpeed;					/* TM20120917 */ // 0xff = auto
		BYTE bSwingMode;				/* TM20120917 */ // 0=off, 1=on

	} m_extSensor;

	// Sedert versie 0x0900: Delegation.
	struct 
	{
		BYTE m_bType;
		BYTE m_bNodeAddress;
		BYTE m_bUnitAddress;		

	} m_delegation;

	// Sedert V16.00: USE_SENSOR_PIDCONTROL

	BYTE m_pidWorkingMode;		// Sedert V16.02

	struct UNIT_SENS_PIDCONTROL{
		BYTE m_bNodeAddress;
		BYTE m_bUnitAddress;		
		BYTE m_params[PIDCONTROL_PARAM_MAX];

	} m_pidControl[PIDCONTROL_WORKINGMODE_MAX];

public:
	void Set_Function(BYTE bData)				{bFunctionData = bData;};
	void Set_Event(BYTE bEvent)					{bSensEvent = bEvent; };
	void Set_EventState(BYTE bState)			{bSensEventState= bState; };
	void Set_Setpoint(unsigned short usValue)	{usSetpoint = usValue; };
	void Set_Mode(BYTE bMode)					{bPreset = bMode; };
	void Set_State(BYTE bWorking)				{bState = bWorking; };

	void Set_HysLowValue(BYTE bMode,BYTE bVal) {
		if (bMode < 4) bHysLowValue[bMode] = bVal; 
	};
	void Set_HysHighValue(BYTE bMode,BYTE bVal)	{
		if (bMode < 4) bHysHighValue[bMode] = bVal; 
	};

	void Set_HysLowOffsetValue(BYTE bVal) {
		bHysLowOffsetValue = bVal; 
	};
	void Set_HysHighOffsetValue(BYTE bVal) {
		bHysHighOffsetValue = bVal; 
	};
	void Set_SampleTime(BYTE bTime)	{
		bSampleTime = bTime;
	};
	void SetMeasuredValueOffset(short val ) {	/* TM20120405 */
		sMeasuredValOffset = val;
	}

	BYTE Get_Function(void)	const {
		return bFunctionData; 
	};
	BYTE Get_Event(void) const {
		return bSensEvent; 
	};
	BYTE Get_EventState(void) const {
		return bSensEventState; 
	};
	unsigned short Get_SetPoint(void) const	{
		return usSetpoint; 
	};
	BYTE Get_Mode(void)	const {
		return bPreset; 
	};
	BYTE Get_State(void) const {
		return bState; 
	};
	BYTE Get_HysLowValue(BYTE bMode) const {
		return bHysLowValue[bMode]; 
	};
	BYTE Get_HysHighValue(BYTE bMode) const {
		return bHysHighValue[bMode]; 
	};

	BYTE Get_HysLowOffsetValue() const {
		return bHysLowOffsetValue;
	};
	BYTE Get_HysHighOffsetValue() const {
		return bHysHighOffsetValue;
	};
	BYTE Get_SampleTime(void) const	{
		return bSampleTime; 
	};
	short GetMeasuredValueOffset( ) const {	/* TM20120405 */
		return sMeasuredValOffset;
	}

	unsigned short Get_Angle( ) const		/* TM20120917 */
	{
		return m_extSensor.usAngle;
	}
	void Set_Angle(unsigned short v)		/* TM20120917 */
	{
		m_extSensor.usAngle = v;
	}
	BYTE Get_WorkingMode() const			/* TM20120917 */
	{
		return m_extSensor.bWorkingMode;
	}
	void Set_WorkingMode(BYTE v)			/* TM20120917 */
	{
		m_extSensor.bWorkingMode = v;
	}
	BYTE Get_FanSpeed() const				/* TM20120917 */
	{
		return m_extSensor.bFanSpeed;
	}
	void Set_FanSpeed(BYTE v)				/* TM20120917 */
	{
		m_extSensor.bFanSpeed = v;
	}
	BYTE Get_SwingMode( ) const				/* TM20120917 */
	{
		return m_extSensor.bSwingMode;
	}
	void Set_SwingMode(BYTE v)				/* TM20120917 */
	{
		m_extSensor.bSwingMode = v;
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

	void setPIDWorkingMode(int workingMode);
	int getPIDWorkingMode( ) const;

	void setPIDControlUnit(int workingsmode,int nodeAddress,int unitAddress);
	BOOL getPIDControlUnit(int workingsmode,int *pNodeAddress,int *pUnitAddress) const;

	void setPIDControlParams(int workingsmode, int kp,int ki,int kd);
	BOOL getPIDControlParams(int workingsmode, int *kp,int *ki,int *kd) const;

private:
	typedef CBindingUnit Base;
};


// Sedert versie 0x0D00.
class CBindingSensUnivUnit : public CBindingSensUnit
{
public:
	CBindingSensUnivUnit(BYTE bBindingUnitType);	
	CBindingSensUnivUnit(const CBindingUnit& pBindingUnit);// Oppassen: Dit is geen COPY Constructor !!!!!
	virtual ~CBindingSensUnivUnit( );

	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	virtual void Init(void);
	void InitSensorInput(void);

private:
	typedef CBindingSensUnit Base;
};


// Utility klasse 
struct CBindingSensUnit_Utils
{
	static BOOL getStrSampleTime(const CBindingSensUnit& ref,CString* pString);
};