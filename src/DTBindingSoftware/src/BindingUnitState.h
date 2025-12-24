// BindingUnitState.h: interface for the CBindingUnitState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGUNITSTATE_H__C7EAC28B_8FE8_4152_A8A4_6019D8190451__INCLUDED_)
#define AFX_BINDINGUNITSTATE_H__C7EAC28B_8FE8_4152_A8A4_6019D8190451__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////////////////
class CBindingIRTXUnitState;
class CBindingRC5RXUnitState;
class CBindingAlarmUnitState;
class CBindingBoseLF48UnitState;
class CBindingAudioHC4UnitState;
class CBindingBoseV35UnitState;
class CBindingAVMatrixUnitState;
///////////////////////////////////////////////////////////////////////

class CBindingUnitState  
{
protected:
	CBindingIRTXUnitState* m_pIRTXUnitState;
	CBindingRC5RXUnitState* m_pRC5RXUnitState;
	CBindingAlarmUnitState* m_pAlarmUnitState;
	CBindingBoseLF48UnitState* m_pBoseLF48UnitState;
	CBindingAudioHC4UnitState* m_pAudioHC4UnitState;
	CBindingBoseV35UnitState* m_pBoseV35UnitState;
	CBindingAVMatrixUnitState* m_pAvMatrixState;

	CBindingUnitState();

public:
	virtual ~CBindingUnitState();
	static CBindingUnitState* Instance();
	void Reset();

	CBindingIRTXUnitState* get_IRTXUnit();
	CBindingRC5RXUnitState* get_RC5RXUnit();
	CBindingAlarmUnitState* get_AlarmUnit();
	CBindingBoseLF48UnitState* get_BoseLF48Unit();
	CBindingAudioHC4UnitState* get_AudioHC4Unit();
	CBindingBoseV35UnitState* get_BoseV35Unit();
	CBindingAVMatrixUnitState* get_AvMatrixUnit();
};

///////////////////////////////////////////////////////////////////////

class CBindingAVMatrixUnitState		/* TM_AVMATRIX_INPUT */
{
public:
	CBindingAVMatrixUnitState( );
	~CBindingAVMatrixUnitState( );
	void Reset();

	void set_Event(BYTE Event)		{ m_bEvent = Event; }	
	void set_InpMacro(BYTE Macro)	{ m_bInpMacroCode = Macro; }
	void set_ControlState(BYTE bState)	{ m_bControlState = bState;}	
	void set_Function(BYTE bData)	{m_bFunctionData = bData;};
	void set_Volume(BYTE vol)		{m_bVolume = vol; };
	void set_Bass(BYTE bass)		{m_bBass = bass; };
	void set_Treble(BYTE Treble)	{m_bTreble = Treble;};
	void set_Input(BYTE In)			{m_bInput = In;};
	void set_Output(BYTE Out)		{m_bOutput = Out; };
	void set_Config(BYTE Config)	{m_bConfig = Config; };
	void set_State(BYTE state)		{m_bState = state; };

	BYTE get_Function(void)	const {
		return m_bFunctionData; 
	};
	BYTE get_Volume(void) const {
		return m_bVolume; 
	};
	BYTE get_Bass(void) const {
		return m_bBass; 
	};
	BYTE get_Treble(void) const {
		return m_bTreble; 
	};
	BYTE get_Input(void) const	{
		return m_bInput;
	};
	BYTE get_Output(void) const	{
		return m_bOutput; 
	};
	BYTE get_Config(void) const {
		return m_bConfig; 
	};
	BYTE get_State(void) const {
		return m_bState; 
	};
	BYTE get_Event(void) const 
	{		
		return m_bEvent;
	}
	BYTE get_InpMacro(void) const 
	{		
		return m_bInpMacroCode;
	}
	BYTE get_ControlState(void) const 
	{		
		return m_bControlState;
	}	

private:
	BYTE m_bFunctionData;
	BYTE m_bVolume;			// 0 ... 100
	BYTE m_bBass;			// 0 ... 100
	BYTE m_bTreble;			// 0 ... 100
	BYTE m_bInput;			// 0 ... 7
	BYTE m_bOutput;			// 0 ... 31
	BYTE m_bConfig;
	BYTE m_bState;			// On/Off

	BYTE m_bEvent;							
	BYTE m_bInpMacroCode;						
	BYTE m_bControlState;		
};

///////////////////////////////////////////////////////////////////////

class CBindingIRTXUnitState  
{
protected:
	int m_Code;	

public:
	CBindingIRTXUnitState();
	~CBindingIRTXUnitState();

	void set(int code);
	int get() const	;

	void Reset();
};

///////////////////////////////////////////////////////////////////////

class CBindingRC5RXUnitState
{
protected:
	int m_Code;	
	BYTE m_Event;

public:
	CBindingRC5RXUnitState();
	~CBindingRC5RXUnitState();

	void set_Code(int code);
	void set_Event(BYTE ev);

	int get_Code() const;
	int get_Event() const;

	void Reset();
};


///////////////////////////////////////////////////////////////////////

class CBindingAlarmUnitState
{
protected:
	int m_Zone;	

public:
	CBindingAlarmUnitState();
	~CBindingAlarmUnitState();

	void set(int zone);
	int get() const;

	void Reset();
};
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
class CBindingBoseLFUnitState_Base
{
protected:
	BOOL m_fState;
	BYTE m_Source;
	BYTE m_Button;
	BYTE m_ControlEvent;
	BYTE m_FunctionTK;
	BYTE m_Event;
	BYTE m_Function;
	BYTE m_FunctionSrc;
	BYTE m_FunctionDest;


	virtual void reset(void) = 0;

public:
	CBindingBoseLFUnitState_Base();
	virtual ~CBindingBoseLFUnitState_Base();

	void Reset();
	void set_Source(BYTE bSource);
	void set_Button(BYTE button);
	void set_ControlEvent(BYTE bEvent);
	void set_FunctionTK(BYTE function);
	void set_Event(BYTE ev);
	void set_State(BOOL fState);
	void set_Function(BYTE bFunction);
	void set_FunctionSrc(BYTE bFunction);
	void set_FunctionDest(BYTE bFunction);
	
	BYTE get_Source() const;
	BYTE get_Button() const;
	BYTE get_ControlEvent() const;
	BYTE get_FunctionTK() const;
	BYTE get_Event() const;
	BOOL get_State() const;
	BYTE get_Function() const;
	BYTE get_FunctionSrc() const;
	BYTE get_FunctionDest() const;
};

///////////////////////////////////////////////////////////////////////

class CBindingBoseLF48UnitState : public CBindingBoseLFUnitState_Base
{
protected:
	virtual void reset(void);

public:
	CBindingBoseLF48UnitState();
	virtual ~CBindingBoseLF48UnitState();
};

///////////////////////////////////////////////////////////////////////

class CBindingBoseV35UnitState : public CBindingBoseLFUnitState_Base
{
protected:
	BYTE m_FunctionExtraIR;
	BYTE m_bVolume;
	BYTE m_bDestFunctionExtraIR;

	virtual void reset(void);

public:
	CBindingBoseV35UnitState();
	virtual ~CBindingBoseV35UnitState();

	void set_FunctionExtraIR(BYTE func);
	BYTE get_FunctionExtraIR( ) const;
	void set_DestFunctionExtraIR(BYTE func);
	BYTE get_DestFunctionExtraIR( ) const;
	void set_Volume(BYTE func);
	BYTE get_Volume( ) const;
};

///////////////////////////////////////////////////////////////////////
class CBindingAudioHC4UnitState
{
protected:
	BYTE m_Source;
	BYTE m_Function;
	BYTE m_Event;			/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE m_InpMacro;		/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE m_ControlState;	/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE m_Destination;
	BOOL m_fState;
	BYTE m_Volume;
	BYTE m_Keypress;
	BYTE m_Config;

public:
	CBindingAudioHC4UnitState();
	~CBindingAudioHC4UnitState();

	void Reset();

	void set_Source(BYTE source);
	void set_Function(BYTE function);
	void set_Event(BYTE Event);			/* TM_AUDIOHC4_BINDING_INPUT */
	void set_InpMacro(BYTE Macro);		/* TM_AUDIOHC4_BINDING_INPUT */
	void set_ControlState(BYTE state);	/* TM_AUDIOHC4_BINDING_INPUT */	
	void set_Destination(BYTE dest);
	void set_State(BOOL state);
	void set_Volume(BYTE vol);
	void set_Keypress(BYTE kp);
	void set_Config(BYTE config);

	BYTE get_Source() const;
	BYTE get_Function() const;
	BYTE get_Event() const;			/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE get_InpMacro() const;		/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE get_ControlState() const;	/* TM_AUDIOHC4_BINDING_INPUT */
	BYTE get_Destination() const;
	BOOL get_State() const;
	BYTE get_Volume() const;
	BYTE get_Keypress() const;
	BYTE get_Config() const;
};

///////////////////////////////////////////////////////////////////////




#endif // !defined(AFX_BINDINGUNITSTATE_H__C7EAC28B_8FE8_4152_A8A4_6019D8190451__INCLUDED_)
