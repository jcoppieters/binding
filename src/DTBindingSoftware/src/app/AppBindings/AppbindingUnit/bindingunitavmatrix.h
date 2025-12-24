#pragma once

#include "bindingunit.h"


class CBindingAVMatrixUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		

public:
	CBindingAVMatrixUnit(BYTE bBindingUnitType);
	CBindingAVMatrixUnit(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingAVMatrixUnit(); 

	enum
	{
		AVMATRIX_MAX_OUTPUTS		= 64,		/* TM 20110808 */
		AVMATRIX_MAX_INPUTS			= 32,		/* TM 20110808 */
		AVMATRIX_MAX_VOLUME			= 100,		/* TM 20110808 */
		AVMATRIX_MAX_MACRO			= 0x3fff,	/* TM 20110808 : 14 bits*/
	};

public:
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;
	BYTE bVolume;				// AVMATRIX_MAX_VOLUME
	BYTE bBass,bTreble;			// AVMATRIX_MAX_VOLUME
	BYTE bInput;				// AVMATRIX_MAX_INPUTS
	BYTE bOutput;				// AVMATRIX_MAX_OUTPUTS
	BYTE bConfig;				// AVMATRIX_MAX_MACRO
	BYTE bState;				// On/Off
	unsigned short usMacroCode;
	BYTE bDestMasterIdx;		// Sedert versie 0x0903
	BYTE bDestSlaveIdx;			// Sedert versie 0x0903

public:
	void Set_Function(BYTE bData)	{bFunctionData = bData;};
	void Set_Volume(BYTE vol)		{bVolume = vol; };
	void Set_Bass(BYTE bass)		{bBass = bass; };
	void Set_Treble(BYTE Treble)	{bTreble = Treble;};
	void Set_Input(BYTE In)			{bInput = In;};
	void Set_Output(BYTE Out)		{bOutput = Out; };
	void Set_Config(BYTE Config)	{bConfig = Config; };
	void Set_State(BYTE state)		{bState = state; };
	void Set_MacroCode(unsigned short val) { usMacroCode = val; };
	void Set_DestMasterIdx(BYTE idx)	{ bDestMasterIdx = idx; };
	void Set_DestSlaveIdx(BYTE idx)	{ bDestSlaveIdx = idx; };


	BYTE Get_Function(void)	const {
		return bFunctionData; 
	};
	BYTE Get_Volume(void) const {
		return bVolume; 
	};
	BYTE Get_Bass(void) const {
		return bBass; 
	};
	BYTE Get_Treble(void) const {
		return bTreble; 
	};
	BYTE Get_Input(void) const	{
		return bInput;
	};
	BYTE Get_Output(void) const	{
		return bOutput; 
	};
	BYTE Get_Config(void) const {
		return bConfig; 
	};
	BYTE Get_State(void) const {
		return bState; 
	};
	unsigned short Get_MacroCode(void) const {
		return usMacroCode;
	}
	BYTE Get_DestMasterIdx(void) const	{ 
		return bDestMasterIdx;
	};
	BYTE Get_DestSlaveIdx(void) const { 
		return bDestSlaveIdx; 
	};

private:
	typedef CBindingUnit Base;
};

////////////////////////////////////////////////////////////////////////
/////////			AVMATRIX INPUT UNIT						////////////
////////////////////////////////////////////////////////////////////////
class CBindingAVMatrixUnitInput : public CBindingAVMatrixUnit	/* TM_AVMATRIX_INPUT */
{
protected:
	virtual void Init(void);		

public:
	enum { MAX_INPUT_FUNCTIONS = 112 };	

	CBindingAVMatrixUnitInput(BYTE bBindingUnitType);
	CBindingAVMatrixUnitInput(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingAVMatrixUnitInput(); 

	virtual BOOL UpgradeIoType();
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
			
	void Set_Event(BYTE Event)		
	{
		bEvent = Event; 
	}	
	void Set_InpMacro(BYTE Macro)	
	{
		bInpMacroCode = Macro; 
	}
	void Set_ControlState(BYTE bState)		
	{
		bControlState = bState; 
	}	
	BYTE Get_Event(void) const 
	{		
		return bEvent;
	}
	BYTE Get_InpMacro(void) const 
	{		
		return bInpMacroCode;
	}
	BYTE Get_ControlState(void) const 
	{		
		return bControlState;
	}	

private:
	BYTE bEvent;							
	BYTE bInpMacroCode;						
	BYTE bControlState;						
	
	typedef CBindingAVMatrixUnit Base;
};

