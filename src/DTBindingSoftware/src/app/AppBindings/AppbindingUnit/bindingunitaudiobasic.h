#pragma once

#include "bindingunit.h"
#include "bindingunitaudiobasic_def.h"


class CBindingBasicAudioUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingBasicAudioUnit(BYTE bBindingUnitType);
	CBindingBasicAudioUnit(const CBindingUnit& pBindingUnit); // Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingBasicAudioUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;
	BYTE bSourceSelection;			// 0 - 6
	BYTE bDestState;				// On/Off
	BYTE bAudioConfig;
	BYTE bSrcFunction;				// f0-f1
	BYTE bDestFunction;				// f0-f1

public:
	void Set_Function(BYTE bData)			{bFunctionData = bData;};
	void Set_Source(BYTE bSource)			{bSourceSelection = bSource;};
	void Set_State(BYTE bState)				{bDestState = bState; };
	void Set_Config(BYTE bConfig)			{bAudioConfig = bConfig; };
	void Set_SrcFunction(BYTE bFunction)	{bSrcFunction = bFunction; };
	void Set_DestFunction(BYTE bFunction)	{bDestFunction =  bFunction; };

	BYTE Get_Function(void)	const {
		return bFunctionData; 
	};
	BYTE Get_Source(void) const	{
		return bSourceSelection;
	};
	BYTE Get_State(void) const {
		return bDestState; 
	};
	BYTE Get_Config(void) const {
		return bAudioConfig; 
	};
	BYTE Get_SrcFunction(void) const {
		return bSrcFunction; 
	};
	BYTE Get_DestFunction(void) const {
		return bDestFunction; 
	};

private:
	typedef CBindingUnit Base;
};

