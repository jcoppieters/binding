// BindingPropertyUnitDefault.h: interface for the CBindingPropertyUnitDefault class.
//
// Changes: TM20120405: Sens Unit: Measured Offset value bijgevoegd
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGPROPERTYUNITDEFAULT_H__9F303A61_1840_40DE_99D4_DBD97606CA74__INCLUDED_)
#define AFX_BINDINGPROPERTYUNITDEFAULT_H__9F303A61_1840_40DE_99D4_DBD97606CA74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingPropertyUnit.h"

////////////////////////////////////////////////////////////////////////////////

class CBindingPropertyUnitDefault  // Basis klasse ...
{
public:
	CBindingPropertyUnitDefault();
	virtual ~CBindingPropertyUnitDefault();
};


////////////////////////////////////////////////////////////////////////////////



/**
 * Singleton !!!!!
 */
class CBindingPropertyUnitDimDefault : public CBindingPropertyUnitDefault
{
protected:
	const BYTE m_bDimSpeedUp;
	const BYTE m_bDimSpeedDown;
	const BYTE m_bDimMaxRange;
	const BYTE m_bDimMinRange;

	const BOOL m_fPirDelayedOn;
	const BOOL m_fPirDelayedOff;
	const BOOL m_fSolidState;

	BYTE m_bConfig;

protected:
	CBindingPropertyUnitDimDefault();
	CBindingPropertyUnitDimDefault(BYTE bSpeedUP, BYTE bSpeedDown, BYTE bMaxRange, 
								   BYTE bMinRange, BOOL fDelayedOn, 
								   BOOL fDelayedOff, BOOL fSolidState);

	void Calculate_Config( );

public:
	virtual ~CBindingPropertyUnitDimDefault();
	static const CBindingPropertyUnitDimDefault* Instance();

public:
	BYTE GetSpeedUp() const {
		return m_bDimSpeedUp;
	}
	BYTE GetSpeedDown() const {
		return m_bDimSpeedDown;
	}
	BYTE GetMaxRange() const {
		return m_bDimMaxRange;
	}
	BYTE GetMinRange() const {
		return m_bDimMinRange;
	}
	BOOL GetDelayedOn() const {
		return m_fPirDelayedOn;
	}
	BOOL GetPirDelayedOff() const {
		return m_fPirDelayedOff;
	}
	BOOL GetSolidState() const {
		return m_fSolidState;
	}		
	BYTE GetConfig() const {
		return m_bConfig;
	}
};


namespace VERSION_V82
{
	/**
	* Singleton !!!!!
	*/
	class CBindingPropertyUnitDimDefault : public ::CBindingPropertyUnitDimDefault		
	{
	protected:
		CBindingPropertyUnitDimDefault();

	public:
		virtual ~CBindingPropertyUnitDimDefault();

		static const CBindingPropertyUnitDimDefault* Instance();
	};
};

// Sedert versie 0641
namespace VERSION_V90_DALIDMX
{
	/**
	* Singleton !!!!!
	*/
	class CBindingPropertyUnitDimDefault : public ::CBindingPropertyUnitDimDefault		
	{
	protected:
		CBindingPropertyUnitDimDefault();

	public:
		virtual ~CBindingPropertyUnitDimDefault();

		static const CBindingPropertyUnitDimDefault* Instance();
	};
};


////////////////////////////////////////////////////////////////////////////////

/**
 * Singleton !!!!!!!
 */
class CBindingPropertyUnitSwitchDefault : public CBindingPropertyUnitDefault
{
protected:
	const BYTE m_bTimeValue;
	const BOOL m_fTimeEnabled;

protected:
	CBindingPropertyUnitSwitchDefault();

public:
	virtual ~CBindingPropertyUnitSwitchDefault();
	static const CBindingPropertyUnitSwitchDefault* Instance();

public:
	BOOL GetTimeValue() const {
		return m_bTimeValue;
	}
	BOOL GetPrecisionTimed() const {
		return m_fTimeEnabled;
	}
};


////////////////////////////////////////////////////////////////////////////////
class CBindingPropertyUnitDuoswitchDefault : public CBindingPropertyUnitDefault
{
protected:
	CBindingPropertyUnitDuoswitchDefault();
	CBindingPropertyUnitDuoswitchDefault(BYTE bStopTime,BYTE bSwitchTime);

public:
	virtual ~CBindingPropertyUnitDuoswitchDefault();
	static const CBindingPropertyUnitDuoswitchDefault* Instance();

public:
	BYTE GetStopTime() const {
		return m_bStopTime;
	}
	BYTE GetSwitchTime() const {
		return m_bSwitchTime;
	}

private:
	const BYTE m_bStopTime;
	const BYTE m_bSwitchTime;
};

// Voor de nxp modules vanaf V90.25
// Switch time op 1200 msec.
namespace VERSION_V9025
{
	class CBindingPropertyUnitDuoswitchDefault : public ::CBindingPropertyUnitDuoswitchDefault
	{
	public:
		static const CBindingPropertyUnitDuoswitchDefault* Instance();
		virtual ~CBindingPropertyUnitDuoswitchDefault();

	protected:
		CBindingPropertyUnitDuoswitchDefault();
	private:
		typedef ::CBindingPropertyUnitDuoswitchDefault Base;
	};
}; /* end namspace */

////////////////////////////////////////////////////////////////////////////////

class CBindingPropertyUnitSensDefault: public CBindingPropertyUnitDefault
{
protected:
	const BYTE m_bSensType;
	BYTE m_bSampleTime;
	BYTE m_bHysValues[2][4];
	BYTE m_bHysOffsetValues[2];
	short m_sMeasuredValOffset;		
	short m_setpointRangeMin;
	short m_setpointRangeMax;
	short m_pidParams[SENS_PIDCONTROL_WORKINGMODE_MAX][SENS_PIDCONTROL_PARAM_MAX];	// Sedert V16.00: USE_SENSOR_PIDCONTROL	

protected:
	CBindingPropertyUnitSensDefault();

	/* TM20130118: Constructor bijgemaakt voor initialiseren van waarden van V2 */
	CBindingPropertyUnitSensDefault(		
		BYTE bHysOffsetValuesHigh,BYTE m_bHysOffsetValuesLow,
		BYTE bHysValuesSunHigh,BYTE bHysValuesSunLow,
		BYTE bHysValuesHSunHigh,BYTE bHysValuesHSunLow,
		BYTE bHysValuesMoonHigh,BYTE bHysValuesMoonLow,
		BYTE bHysValuesHMoonHigh,BYTE bHysValuesHMoonLow);

	// Sedert versie 0x0701
	void setSampleTime(BYTE bSampleTime) {
		m_bSampleTime = bSampleTime;	
	}

	// sedert versie 0x0E00
	void setSetpointRange(short min,short max) {
		m_setpointRangeMin=min;
		m_setpointRangeMax=max;
	}

public:
	virtual ~CBindingPropertyUnitSensDefault();
	static const CBindingPropertyUnitSensDefault* Instance();

	BYTE GetSensType() const {
		return m_bSensType;
	}
	BYTE GetSampleTime() const {
		return m_bSampleTime;
	}
	BYTE GetHysOffsetHighValue() const {
		return m_bHysOffsetValues[HYST_OFFSET_HIGH];
	}
	BYTE GetHysOffsetLowValue() const {
		return m_bHysOffsetValues[HYST_OFFSET_LOW];
	}
	BYTE GetHysHighValue(BYTE bMode) const {
		return m_bHysValues[HYST_OFFSET_HIGH][bMode];
	}
	BYTE GetHysLowValue(BYTE bMode) const {
		return m_bHysValues[HYST_OFFSET_LOW][bMode];
	}
	short GetMeasuredValueOffset( ) const {	
		return m_sMeasuredValOffset;
	}
	short getSetpointRangeMin( ) const {
		return m_setpointRangeMin;
	}
	short getSetpointRangeMax( ) const {
		return m_setpointRangeMax;
	}
	int getPIDControlParam(int mode,int param) const;		// Sedert V16.00: USE_SENSOR_PIDCONTROL	
};

// Nieuwe standaard waarden voor hyst waarden.
namespace VERSION_V2	
{
	/** Singleton !!!!! */
	class CBindingPropertyUnitSensDefault : public ::CBindingPropertyUnitSensDefault		
	{
	protected:
		CBindingPropertyUnitSensDefault();

	public:
		virtual ~CBindingPropertyUnitSensDefault();
		static const CBindingPropertyUnitSensDefault* Instance();
	};
};

// Nieuwe waarden voor sample interval.
namespace VERSION_V3
{
	/** Singleton !!!!! */
	class CBindingPropertyUnitSensDefault : public ::CBindingPropertyUnitSensDefault		
	{
	protected:
		CBindingPropertyUnitSensDefault();

	public:
		virtual ~CBindingPropertyUnitSensDefault();
		static const CBindingPropertyUnitSensDefault* Instance(int idx);
	};
};

// Waarden voor beperken van setpoint.
namespace VERSION_V4 // Setpont range...
{
	/** Singleton !!!!! */
	class CBindingPropertyUnitSensDefault : public ::CBindingPropertyUnitSensDefault {
	protected:
		CBindingPropertyUnitSensDefault( );

	public:
		virtual ~CBindingPropertyUnitSensDefault();

		// Revisie nummer bepaalt kleine verschil tov van de vorige versie.
		static const CBindingPropertyUnitSensDefault* Instance(int revisionNumber,int idx);

	private:
		int m_revision;
	};
};
	
namespace VERSION_V5 
{
	/** Singleton !!!!! */
	class CBindingPropertyUnitSensDefault : public ::CBindingPropertyUnitSensDefault		
	{
	protected:
		CBindingPropertyUnitSensDefault();

	public:
		virtual ~CBindingPropertyUnitSensDefault();
		static const CBindingPropertyUnitSensDefault* Instance(int idx);
	};
};

// Sedert versie 0x0D00: Implementatie van de universele sensor unit.

class CBindingPropertyUnitSensUnivDefault : public ::CBindingPropertyUnitSensDefault		
{
protected:
	CBindingPropertyUnitSensUnivDefault();

public:
	virtual ~CBindingPropertyUnitSensUnivDefault();
	static const CBindingPropertyUnitSensUnivDefault* Instance(int idx=0);
};

////////////////////////////////////////////////////////////////////////////////

class CBindingPropertyUnitAlarmDefault: public CBindingPropertyUnitDefault
{
protected:
	BYTE m_bRemoteCode[ALARM_REMOTECODE_BUFFER_MAX];
	const BYTE m_bAlarmCentraleType;

protected:
	CBindingPropertyUnitAlarmDefault();
	CBindingPropertyUnitAlarmDefault(BYTE bAlarmCentraleType);

public:
	virtual ~CBindingPropertyUnitAlarmDefault();
	static const CBindingPropertyUnitAlarmDefault* Instance();

public:
	BYTE GetAlarmType() const {
		return m_bAlarmCentraleType;
	}
	const BYTE* GetRemoteCode() const {
		return (&m_bRemoteCode[0]);
	}
	BYTE GetRemoteCode(int index) const 
	{
		BYTE result = 0xFF;
		if (index < ALARM_REMOTECODE_BUFFER_MAX)
		{
			result = m_bRemoteCode[index];
		}
		return result;
	}
};


namespace VERSION_V2
{
	// TM_CHANGES_ALARM_TEXECOM
	class CBindingPropertyUnitAlarmDefault: public ::CBindingPropertyUnitAlarmDefault
	{
	protected:	
		enum 
		{ 
			ALARM_GALAXY_TYPE_PREMIER44		= 0x04,
			ALARM_GALAXY_TYPE_PREMIER88		= 0x05,		
			ALARM_GALAXY_TYPE_PREMIER168	= 0x06,		
		};		

	protected:
		CBindingPropertyUnitAlarmDefault();

	public:
		virtual ~CBindingPropertyUnitAlarmDefault();
		static const CBindingPropertyUnitAlarmDefault* Instance();
	};
}
////////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_BINDINGPROPERTYUNITDEFAULT_H__9F303A61_1840_40DE_99D4_DBD97606CA74__INCLUDED_)
