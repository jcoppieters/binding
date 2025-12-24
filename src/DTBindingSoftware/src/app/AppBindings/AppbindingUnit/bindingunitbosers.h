#pragma once

#include "bindingunit.h"
#include "bindingunitaudiobasic_def.h"
#include "bindingunitbosers_def.h"


class CBindingBoseRsUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingBoseRsUnit(BYTE bBindingUnitType);
	CBindingBoseRsUnit(const CBindingUnit& pBindingUnit);// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingBoseRsUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;
	BYTE bBoseRsEvent;

	// used as input ...
	BYTE bSelKeypad;		// PRESET-AUX, TUNE-AUX , ...
	BYTE bSelSource;		// CD / FM / DVD / V1 / V2 / TAPE
	BYTE bSelKeyButton;		// 0 ... 9
	BYTE bTKFunction;		// 0 ... 6

	// used as output ...
	BYTE bSourceSelection;			// 0 - 6
	BYTE bDestState;				// On/Off
	BYTE bAudioConfig;
	BYTE bSrcFunction;				// f0-f1
	BYTE bDestFunction;				// f0-f1
	BYTE bVolume;
	
public:
	void Set_Event(BYTE bEvent)				{bBoseRsEvent = bEvent;};
	void Set_Function(BYTE bData)			{bFunctionData = bData;};
	void Set_Source(BYTE bSource)			{bSourceSelection = bSource;};
	void Set_State(BYTE bState)				{bDestState = bState; };
	void Set_Config(BYTE bConfig)			{bAudioConfig = bConfig; };
	void Set_SrcFunction(BYTE bFunction)	{bSrcFunction = bFunction; };
	void Set_DestFunction(BYTE bFunction)	{bDestFunction =  bFunction; };
	void Set_KeyPad(BYTE bKeypad)			{bSelKeypad = bKeypad; };
	void Set_Button(BYTE bButton)			{bSelKeyButton = bButton; };
	void Set_Volume(BYTE bVol)				{bVolume = bVol; };
	void Set_TKFunction(BYTE bFunction)		{bTKFunction = bFunction; };

	BYTE Get_Function(void)	const {
		return bFunctionData; 
	};
	BYTE Get_Event(void) const {
		return bBoseRsEvent; 
	};
	BYTE Get_Source(void) const	{
		return bSourceSelection;
	};
	BYTE Get_State(void) const {
		return bDestState; 
	};
	BYTE Get_Config(void) const	{
		return bAudioConfig; 
	};
	BYTE Get_SrcFunction(void) const {
		return bSrcFunction; 
	};
	BYTE Get_DestFunction(void) const {
		return bDestFunction; 
	};
	BYTE Get_Keypad(void) const	{
		return bSelKeypad; 
	};
	BYTE Get_Button(void) const	{
		return bSelKeyButton; 
	};
	BYTE Get_Volume(void) const	{
		return bVolume; 
	};
	BYTE Get_TKFunction(void) const {
		return bTKFunction; 
	};

	// 
	CString Get_strSource(void) const;

private:
	typedef CBindingUnit Base;
};