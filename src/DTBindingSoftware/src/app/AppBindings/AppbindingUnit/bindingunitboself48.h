#pragma once

#include "bindingunit.h"
#include "bindingunitboself48_def.h"


class CBindingBoseLf48Unit : public CBindingUnit
{
private:
	static const BYTE BoseLF48BindingFlagTabel[MAX_BOSE48_FUNCTIONS][MAX_BOSE48_SOURCES];

protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingBoseLf48Unit(BYTE bBindingUnitType);
	CBindingBoseLf48Unit(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingBoseLf48Unit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;
	BYTE bBoseLfEvent;
	
	// As input ...
	BYTE bControlEvent;		// E01 - E03
	BOOL fControlState;		// Bool ...


	// used as input ...
	BYTE bSelSource;		
	BYTE bSelKeyButton;		// 0 ... 9 + left,right,up,down,enter
	BYTE bTKFunction;		// 0 ... 6

	// used as output ...
	BYTE bSourceSelection;			// 0 - 8
	BYTE bDestState;				// On/Off
	//BYTE bAudioConfig;
	BYTE bSrcFunction;				// f0-f1
	BYTE bDestFunction;				// f0-f1

	BYTE ConvertFromBindingTable(BYTE Src,BYTE func);
	BYTE GetSourceFromBindingTable(BYTE flags);
	BYTE GetFunctionFromBindingTable(BYTE flags);

	
public:
	void Set_Event(BYTE bEvent)				{bBoseLfEvent = bEvent;};
	void Set_Function(BYTE bData)			{bFunctionData = bData;};
	void Set_Source(BYTE bSource)			{bSourceSelection = bSource;};
	void Set_State(BYTE bState)				{bDestState = bState; };
	//void Set_Config(BYTE bConfig)			{bAudioConfig = bConfig; };
	void Set_SrcFunction(BYTE bFunction)	{bSrcFunction = bFunction; };
	void Set_DestFunction(BYTE bFunction)	{bDestFunction =  bFunction; };
	void Set_Button(BYTE bButton)			{bSelKeyButton = bButton; };
	void Set_TKFunction(BYTE bFunction)		{bTKFunction = bFunction; };
	void Set_ControlState(BOOL flag)		{fControlState = flag; };
	void Set_ControlEvent(BYTE bEvent)		{bControlEvent = bEvent; };

	//BYTE Get_Config(void)					{return bAudioConfig; };

	BYTE Get_Function(void)	const {
		return bFunctionData; 
	};
	BYTE Get_Event(void) const {
		return bBoseLfEvent; 
	};
	BYTE Get_Source(void) const {
		return bSourceSelection;
	};
	BYTE Get_State(void) const {
		return bDestState; 
	};
	BYTE Get_SrcFunction(void) const {
		return bSrcFunction; 
	};
	BYTE Get_DestFunction(void)	const {
		return bDestFunction; 
	};
	BYTE Get_Button(void) const {
		return bSelKeyButton; 
	};
	BYTE Get_TKFunction(void) const	{
		return bTKFunction; 
	};
	BOOL Get_ControlState(void) const {
		return fControlState; 
	};
	BYTE Get_ControlEvent(void) const {
		return bControlEvent; 
	};
	
	
	CString Get_strSource(void) const;

private:
	typedef CBindingUnit Base;
};