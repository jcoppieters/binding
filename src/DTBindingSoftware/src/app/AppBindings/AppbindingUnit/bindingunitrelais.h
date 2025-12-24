#pragma once

#include "bindingunit.h"

class CBindingRelaisUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701
public:
	CBindingRelaisUnit(BYTE bBindingUnitType); 
	CBindingRelaisUnit(const CBindingUnit& pBindingUnit);// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingRelaisUnit(); 	

	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:	
	BYTE bFunctionData;
	BYTE bSwitchStatus;	// ON OFF PIR TIMED                       
	BYTE bSwitchConfig;	/* for the LCD node */
	BYTE bSwitchStopTimerReloadValue;
	BYTE bSwitchPirTime;

public:
	void Set_Function(BYTE bData) {bFunctionData = bData;};
	void Set_Status(BYTE bState) {bSwitchStatus = bState; };
	void Set_Config(BYTE bConfig) {bSwitchConfig = bConfig; };
	void Set_PirTime(BYTE bTime) {bSwitchPirTime = bTime; };
	void Set_StopTime(BYTE bTime) {bSwitchStopTimerReloadValue = bTime; };
	
	BYTE Get_Function(void) const {
		return bFunctionData; 
	};
	BYTE Get_Status(void) const {
		return bSwitchStatus; 
	};
	BYTE Get_Config(void) const {
		return bSwitchConfig; 
	};
	BYTE Get_PirTime(void) const {
		return bSwitchPirTime; 
	};
	BYTE Get_StopTime(void) const {
		return bSwitchStopTimerReloadValue; 
	};

private:
	typedef CBindingUnit Base;
};


////////////////////////////////////////////////////////////

// Sedert versie 0x0C01
class CBindingRelaisUnitInput : public CBindingRelaisUnit
{
protected:
	virtual void Init(void);		//TM 20040701

private:
	CBindingRelaisUnitInput& operator=(const CBindingRelaisUnitInput& BindingUnit);

public:
	CBindingRelaisUnitInput(BYTE bBindingUnitType); 
	CBindingRelaisUnitInput(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingRelaisUnitInput(); 

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
	typedef CBindingRelaisUnit Base;
};