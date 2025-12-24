#pragma once

#include "bindingunit.h"

class CBindingHC4AudioUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:	
	CBindingHC4AudioUnit(BYTE bBindingUnitType);
	CBindingHC4AudioUnit(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingHC4AudioUnit(); 

	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;
	BYTE bSource;					// 0 - 8
	BYTE bDestination;
	BYTE bDestState;				// On/Off
	BYTE bAudioConfig;
	BYTE bVolume;
	BYTE bKeyPress;
	BYTE bConfig;

#ifdef SKIP
	BYTE bEvent;					/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE bInpMacroCode;				/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE bControlState;				/* TM_AUDIOHC4_BINDING_INPUT */
#endif

public:

#ifdef SKIP
	void Set_Event(BYTE Event)		{bEvent = Event; }			/* TM_AUDIOHC4_BINDING_INPUT */
	void Set_InpMacro(BYTE Macro)	{bInpMacroCode = Macro; }	/* TM_AUDIOHC4_BINDING_INPUT */
	void Set_ControlState(BYTE bState)		{bControlState = bState; }	/* TM_AUDIOHC4_BINDING_INPUT */
#endif

	void Set_Function(BYTE bData)			{bFunctionData = bData;};
	void Set_Source(BYTE Source)			{bSource = Source;};
	void Set_Destination(BYTE Dest)			{bDestination = Dest;};
	void Set_State(BYTE bState)				{bDestState = bState; };
	void Set_Config(BYTE bConfig)			{bAudioConfig = bConfig; };
	void Set_Volume(BYTE vol)				{bVolume = vol;};
	void Set_Keypress(BYTE key)				{bKeyPress = key; };

#ifdef SKIP

	BYTE Get_Event(void) const {	/* TM_AUDIOHC4_BINDING_INPUT */
		return bEvent;
	}
	BYTE Get_InpMacro(void) const {	/* TM_AUDIOHC4_BINDING_INPUT */
		return bInpMacroCode;
	}
	BYTE Get_ControlState(void) const {		/* TM_AUDIOHC4_BINDING_INPUT */
		return bControlState;
	}
#endif

	BYTE Get_Function(void) const {
		return bFunctionData; 
	};
	BYTE Get_Source(void) const	{
		return bSource;
	};
	BYTE Get_Destination(void) const {
		return bDestination; 
	}
	BYTE Get_State(void) const {
		return bDestState; 
	};
	BYTE Get_Config(void) const {
		return bAudioConfig; 
	};
	BYTE Get_Volume(void) const	{
		return bVolume; 
	};
	BYTE Get_Keypress(void) const {
		return bKeyPress; 
	};

private:
	typedef CBindingUnit Base;
};

////////////////////////////////////////////////////////////////////////

class CBindingHC4AudioUnit_Input : public CBindingHC4AudioUnit
{
protected:
	virtual void Init(void);

public:
	enum { MAX_AUDIO_HC4_INPUT_MACROS = 120 };

	CBindingHC4AudioUnit_Input(BYTE bBindingUnitType);
	CBindingHC4AudioUnit_Input(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingHC4AudioUnit_Input(); 

	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bEvent;							/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE bInpMacroCode;						/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE bControlState;						/* TM_AUDIOHC4_BINDING_INPUT */

public:
	void Set_Event(BYTE Event)		{bEvent = Event; }			/* TM_AUDIOHC4_BINDING_INPUT */
	void Set_InpMacro(BYTE Macro)	{bInpMacroCode = Macro; }	/* TM_AUDIOHC4_BINDING_INPUT */
	void Set_ControlState(BYTE bState)		{bControlState = bState; }	/* TM_AUDIOHC4_BINDING_INPUT */	

	BYTE Get_Event(void) const {			/* TM_AUDIOHC4_BINDING_INPUT */
		return bEvent;
	}
	BYTE Get_InpMacro(void) const {			/* TM_AUDIOHC4_BINDING_INPUT */
		return bInpMacroCode;
	}
	BYTE Get_ControlState(void) const {		/* TM_AUDIOHC4_BINDING_INPUT */
		return bControlState;
	}	
private:
	typedef CBindingHC4AudioUnit Base;
};

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//////////				HC4 AUDIO BNO UNIT					////////////
////////////////////////////////////////////////////////////////////////

class CBindingHC4AudioBnoUnit : public CBindingHC4AudioUnit_Input //public CBindingHC4AudioUnit
{
	typedef struct
	{
		const char* szName;
		BYTE bDTFunction;
		BYTE bType;						/* 0=light, 1= Control */
		BYTE bBeo4Command;

	} STRUCT_AVMATRIX_MACRO_BEO4_EVENT;

public:
	CBindingHC4AudioBnoUnit(BYTE bBindingUnitType);
	CBindingHC4AudioBnoUnit(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingHC4AudioBnoUnit(); 

/*
	virtual void Set_InpMacro(BYTE Macro);
	virtual void Set_Event(BYTE Event);
	virtual BYTE Get_Event(void) const;
	virtual BYTE Get_InpMacro(void) const;
*/

	enum { MAX_AUDIO_HC4_BNO_INPUT_MACROS = 100 };
	static const STRUCT_AVMATRIX_MACRO_BEO4_EVENT tableMacroBeo4Events[ MAX_AUDIO_HC4_BNO_INPUT_MACROS ];

private:
	typedef CBindingHC4AudioUnit_Input Base;
};