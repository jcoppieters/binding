#pragma once

#include "bindingunit.h"


class CBindingVideomuxUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		

public:
	CBindingVideomuxUnit(BYTE bBindingUnitType);
	CBindingVideomuxUnit(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingVideomuxUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;

	BYTE bInput;			// 0 ... 7
	BYTE bOutput;			// 0 ... 31
	BYTE bConfig;
	BYTE bState;			// On/Off

	BYTE bKeypress;
	BYTE bButtonNr;
	BYTE bDvmrFunction;

public:

	void Set_Function(BYTE bData)		{bFunctionData = bData;};
	void Set_Input(BYTE In)				{bInput = In;};
	void Set_Output(BYTE Out)			{bOutput = Out; };
	void Set_Config(BYTE Config)		{bConfig = Config; };
	void Set_State(BYTE state)			{bState = state; };
	void Set_Keypress(BYTE key)			{bKeypress = key; };
	void Set_ButtonNr(BYTE key)			{bButtonNr = key; };
	void Set_DvmrFunction(BYTE func)	{bDvmrFunction = func;};


	BYTE Get_Function(void)	const {
		return bFunctionData; 
	};
	BYTE Get_Input(void) const {
		return bInput;
	};
	BYTE Get_Output(void) const {
		return bOutput; 
	};
	BYTE Get_Config(void) const {
		return bConfig; 
	};
	BYTE Get_State(void) const {
		return bState; 
	};
	BYTE Get_Keypress(void) const {
		return bKeypress; 
	};
	BYTE Get_ButtonNr(void)	const {
		return bButtonNr; 
	};
	BYTE Get_DvmrFunction(void) const {
		return bDvmrFunction; 
	};

private:
	typedef CBindingUnit Base;
};