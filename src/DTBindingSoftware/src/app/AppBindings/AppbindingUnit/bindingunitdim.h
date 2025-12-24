#pragma once

#include "bindingunit.h"

class CBindingDimmerUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

private:
	CBindingDimmerUnit& operator=(const CBindingDimmerUnit& BindingUnit);

public:
	CBindingDimmerUnit(BYTE bBindingUnitType); 
	CBindingDimmerUnit(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingDimmerUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
   BYTE bFunctionData;	
   BYTE bDimStatus;			// ON , OFF , PIR TIMED
   BYTE bDimMax;			/* 1..99*/
   BYTE bDimMin;			/* 1..99*/
   BYTE bDimSpeedDown;		/* 1 .. 255 */
   BYTE bDimSpeedUp;		/* 1 ... 255 */
   BYTE bDimCurrentVal;		/* 1 ... 99 */
   BYTE bDimConfig;			/* */
   BYTE bDimPirTime;
   BYTE bDimCalcValue;		/* inc/dec 1...99 */

	// Sedert versie 0x0B00
	struct
	{
		BYTE bType;
		BYTE bAddressMSB;
		BYTE bAddressLSB;

	}  addressCfg;

public:
	// Properties
	void Set_Function(BYTE bFunction) { bFunctionData = bFunction; };
	void Set_Status(BYTE bStatus) {bDimStatus = bStatus; };
	void Set_Range(BYTE bMin,BYTE bMax) { bDimMin = bMin; bDimMax = bMax; };
	void Set_Speed(BYTE bDown,BYTE bUp) { bDimSpeedDown = bDown; bDimSpeedUp = bUp; };
	void Set_SpeedDown(BYTE bDown) { bDimSpeedDown = bDown; };
	void Set_SpeedUp(BYTE bUp) { bDimSpeedUp = bUp; };
	void Set_Value(BYTE bValue) { bDimCurrentVal = bValue; };
	void Set_Config(BYTE bConfig) { bDimConfig = bConfig; };
	void Set_PirTime(BYTE bTime) { bDimPirTime = bTime; };
	void Set_AddressCfgType(int val)	{ ASSERT(val>=0 && val<=0xFF); addressCfg.bType = val; };
	void Set_AddressCfgMSB(BYTE msb)	{ addressCfg.bAddressMSB = msb; };
	void Set_AddressCfgLSB(BYTE lsb)	{ addressCfg.bAddressLSB = lsb; };
	void Set_CalcValue(BYTE bValue)		{ bDimCalcValue = bValue; };

	BYTE Get_Function(void) const {
		return bFunctionData; 
	};
	BYTE Get_Status(void) const {
		return bDimStatus; 
	};
	BYTE Get_MinRange(void) const {
		return bDimMin; 
	};
	void Set_MinRange(BYTE value) {
		bDimMin = value;	
	}
	BYTE Get_MaxRange(void) const {
		return bDimMax; 
	};
	void Set_MaxRange(BYTE value) {
		bDimMax = value;	
	}
	BYTE Get_SpeedUp(void) const {
		return bDimSpeedUp; 
	};
	BYTE Get_SpeedDown(void) const {
		return bDimSpeedDown; 
	};
	BYTE Get_Value(void) const {
		return bDimCurrentVal; 
	};
	BYTE Get_Config(void) const {
		return bDimConfig; 
	};
	BYTE Get_PirTime(void) const {
		return bDimPirTime; 
	};
	BYTE Get_AddressCfgType() const	{ 
		return addressCfg.bType; 
	};
	BYTE Get_AddressCfgMSB( ) const	{ 
		return addressCfg.bAddressMSB; 
	};
	BYTE Get_AddressCfgLSB( ) const	{ 
		return addressCfg.bAddressLSB; 
	};
	BYTE Get_CalcValue( ) const {
		return bDimCalcValue;
	}

private:
	typedef CBindingUnit Base;
};



////////////////////////////////////////////////////////////

// Sedert versie 0x0b01
class CBindingDimmerUnitInput : public CBindingDimmerUnit
{
protected:
	virtual void Init(void);		//TM 20040701

private:
	CBindingDimmerUnitInput& operator=(const CBindingDimmerUnitInput& BindingUnit);

public:
	CBindingDimmerUnitInput(BYTE bBindingUnitType); 
	CBindingDimmerUnitInput(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingDimmerUnitInput(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE m_bEvent;
	BYTE m_bInputState;

public:
	// Properties
	void Set_Event(BYTE bEvent) { m_bEvent = bEvent; };
	void Set_InputState(BYTE bState) { m_bInputState = bState; };

	BYTE Get_Event(void) const { return m_bEvent; };
	BYTE Get_InputState(void) const { return m_bInputState; };

private:
	typedef CBindingDimmerUnit Base;
};