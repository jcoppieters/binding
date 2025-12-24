#pragma once

#include "bindingunit.h"
#include "nodegenestructs.h"

class CBindingAlarmUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		

public:
	CBindingAlarmUnit(BYTE bBindingUnitType, BYTE maxInputs);
	CBindingAlarmUnit(const CBindingUnit& pBindingUnit, BYTE maxInputs);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingAlarmUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;
	BYTE bEvent;
	BYTE bControlState;	// BindingFlags ?
	BYTE bZoneNr;
	BYTE bKeypress;		// 0 ... 0x0D
	BYTE bConfig;
	BYTE bGroupNr;		// 0 ... 0x0F
	BYTE bArmSet;		// part or Full, Integra is modes 0-3
	BYTE bRemoteCode[ALARM_REMOTECODE_BUFFER_MAX];

	BYTE m_bMaxInputs;	// TM_CHANGES_ALARM_ARITECH: Oppassen: Geen serializable IO !!!!!!!!!!!!!!!!!!!


public:
	void Set_Function(BYTE Data)		{bFunctionData = Data;};
	void Set_Event(BYTE Event)			{bEvent = Event; };
	void Set_ControlState(BYTE State)	{bControlState = State; };
	void Set_ZoneNr(BYTE zone)			{bZoneNr = zone; };
	void Set_Keypress(BYTE key)			{bKeypress = key; };
	void Set_Config(BYTE Config)		{bConfig = Config; };
	void Set_GroupNr(BYTE group)		{bGroupNr = group; };
	void Set_ArmSet(BYTE Arm)			{bArmSet = Arm; };
	
	void Set_RemoteCode(BYTE index,BYTE Data) 
	{
		if (index < ALARM_REMOTECODE_BUFFER_MAX) {
			bRemoteCode[index] = Data;
		}
	}

	BYTE Get_Function(void) const {
		return bFunctionData; 
	};
	BYTE Get_Event(void) const {
		return bEvent;
	};
	BYTE Get_ControlState(void) const {
		return bControlState; 
	};
	BYTE Get_ZoneNr(void) const	{
		return bZoneNr; 
	};
	BYTE Get_Keypress(void)	const {
		return bKeypress; 
	};
	BYTE Get_Config(void) const	{
		return bConfig;
	};
	BYTE Get_GroupNr(void) const {
		return bGroupNr; 
	};
	BYTE Get_ArmSet(void) const	{
		return bArmSet; 
	};
	BYTE Get_RemoteCode(BYTE index) const {
		return bRemoteCode[index];
	}
	const BYTE* Get_RemoteCode() const {
		return bRemoteCode;
	}
	BYTE Get_MaxInputs( ) const {
		return m_bMaxInputs;
	}

private:
	typedef CBindingUnit Base;
};
