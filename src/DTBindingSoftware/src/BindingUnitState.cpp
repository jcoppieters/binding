// BindingUnitState.cpp: implementation of the CBindingUnitState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingUnitState.h"

//#include "BindingUnit.h"				// only for the macro's.
#include "app\appbindings\appbindingunit\bindingunit_api.h"

#include "App\Appbindings\appbindingproperty\bindingPropertyStrings.h"		// only for the macro's.

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingUnitState* CBindingUnitState::Instance()
{
	static CBindingUnitState _instance;
	return &_instance;
}

//////////////////////////////////////////////////////////////////////

CBindingUnitState::CBindingUnitState() :
m_pIRTXUnitState(0),
m_pRC5RXUnitState(0),
m_pAlarmUnitState(0),
m_pBoseLF48UnitState(0),
m_pBoseV35UnitState(0),
m_pAudioHC4UnitState(0),
m_pAvMatrixState(0)
{
}

CBindingUnitState::~CBindingUnitState()
{
	if (m_pIRTXUnitState) delete m_pIRTXUnitState;
	if (m_pRC5RXUnitState) delete m_pRC5RXUnitState;
	if (m_pAlarmUnitState) delete m_pAlarmUnitState;
	if (m_pBoseLF48UnitState) delete m_pBoseLF48UnitState;
	if (m_pAudioHC4UnitState) delete m_pAudioHC4UnitState;
	if (m_pBoseV35UnitState) delete m_pBoseV35UnitState;
	if (m_pAvMatrixState) delete m_pAvMatrixState;
}

void CBindingUnitState::Reset()
{
	if (m_pIRTXUnitState)	
		m_pIRTXUnitState->Reset();
	
	if (m_pRC5RXUnitState)	
		m_pRC5RXUnitState->Reset();
	
	if (m_pAlarmUnitState)	
		m_pAlarmUnitState->Reset();	

	if (m_pBoseLF48UnitState)
		m_pBoseLF48UnitState->Reset();

	if (m_pAudioHC4UnitState)
		m_pAudioHC4UnitState->Reset();

	if (m_pBoseV35UnitState)
		m_pBoseV35UnitState->Reset();

	if (m_pAvMatrixState)
		m_pAvMatrixState->Reset();
}


CBindingIRTXUnitState* CBindingUnitState::
get_IRTXUnit()
{
	if (m_pIRTXUnitState == 0)
	{
		m_pIRTXUnitState = new CBindingIRTXUnitState();
	}

	ASSERT(m_pIRTXUnitState);
	return m_pIRTXUnitState;
}

CBindingRC5RXUnitState* CBindingUnitState::
get_RC5RXUnit()
{
	if (m_pRC5RXUnitState == 0)
	{
		m_pRC5RXUnitState = new CBindingRC5RXUnitState();
	}

	ASSERT(m_pRC5RXUnitState);
	return m_pRC5RXUnitState;
}

CBindingAlarmUnitState* CBindingUnitState::
get_AlarmUnit()
{
	if (m_pAlarmUnitState == 0)
	{
		m_pAlarmUnitState = new CBindingAlarmUnitState();
	}

	ASSERT(m_pAlarmUnitState);
	return m_pAlarmUnitState;
}

CBindingBoseLF48UnitState* CBindingUnitState::
get_BoseLF48Unit()
{
	if (m_pBoseLF48UnitState == 0)
	{
		m_pBoseLF48UnitState = new CBindingBoseLF48UnitState(); 		
	}

	ASSERT(m_pBoseLF48UnitState);
	return m_pBoseLF48UnitState;
}


CBindingBoseV35UnitState* CBindingUnitState::
get_BoseV35Unit()
{
	if (m_pBoseV35UnitState == 0)
	{
		m_pBoseV35UnitState = new CBindingBoseV35UnitState(); 		
	}

	ASSERT(m_pBoseV35UnitState);
	return m_pBoseV35UnitState;
}

CBindingAudioHC4UnitState* CBindingUnitState::
get_AudioHC4Unit()
{
	if (m_pAudioHC4UnitState == 0)
	{
		m_pAudioHC4UnitState = new CBindingAudioHC4UnitState(); 		
	}

	ASSERT(m_pAudioHC4UnitState);
	return m_pAudioHC4UnitState;
}

CBindingAVMatrixUnitState* CBindingUnitState::
get_AvMatrixUnit()
{
	if (m_pAvMatrixState == 0)
	{
		m_pAvMatrixState = new CBindingAVMatrixUnitState( );
	}
	ASSERT( m_pAvMatrixState );
	return m_pAvMatrixState;
}

//////////////////////////////////////////////////////////////////////

CBindingAVMatrixUnitState::CBindingAVMatrixUnitState()		/* TM_AVMATRIX_INPUT */
{
	Reset();
}

CBindingAVMatrixUnitState::~CBindingAVMatrixUnitState()
{
}

void CBindingAVMatrixUnitState::Reset()
{	
	set_Function(AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE);
	set_Volume(25);
	set_Bass(25);
	set_Treble(25);
	set_Input(0);
	set_Output(0);
	set_Config(0);
	set_State(0);
	set_Event(EV_UNITCONTROLPULS);
	set_InpMacro(0);
	set_ControlState(1);
}

//////////////////////////////////////////////////////////////////////
CBindingIRTXUnitState::CBindingIRTXUnitState()
{
	Reset();
}

CBindingIRTXUnitState::~CBindingIRTXUnitState()
{

}


void CBindingIRTXUnitState::
set(int code)	
{ 
	m_Code = code; 
};
int CBindingIRTXUnitState::
get() const		
{ 
	return m_Code; 
};

void CBindingIRTXUnitState::
Reset()	
{ 
	m_Code = 0; 
};

//////////////////////////////////////////////////////////////////////
CBindingRC5RXUnitState::CBindingRC5RXUnitState()
{
	Reset();
}

CBindingRC5RXUnitState::~CBindingRC5RXUnitState()
{

}

void CBindingRC5RXUnitState::Reset()	
{ 
	m_Code = 0; 
	m_Event = RC5RX_EVENT_PULS;
};

void CBindingRC5RXUnitState::
set_Code(int Code)
{
	m_Code = Code & 0x7F;
}

int CBindingRC5RXUnitState::
get_Code() const		
{ 
	return m_Code; 
};

void CBindingRC5RXUnitState::
set_Event(BYTE ev)
{
	m_Event = ev;
}

int CBindingRC5RXUnitState::
get_Event() const		
{ 
	return m_Event; 
};

//////////////////////////////////////////////////////////////////////

CBindingAlarmUnitState::CBindingAlarmUnitState()
{
	Reset();
}

CBindingAlarmUnitState::~CBindingAlarmUnitState()
{

}

int CBindingAlarmUnitState::
get() const		
{ 
	return m_Zone; 
};

void CBindingAlarmUnitState::
Reset()		
{ 
	m_Zone = 0; 
};


void CBindingAlarmUnitState::
set(int zone)
{
	m_Zone = zone & 0x7F;
}

//////////////////////////////////////////////////////////////////////
CBindingBoseLFUnitState_Base::CBindingBoseLFUnitState_Base( )
{
	TRACE("CBindingBoseLFUnitState_Base::CBindingBoseLFUnitState_Base( )\n");
};

CBindingBoseLFUnitState_Base::~CBindingBoseLFUnitState_Base()
{

}

void CBindingBoseLFUnitState_Base::
set_Source(BYTE bSource)
{
	m_Source = bSource;
}

void CBindingBoseLFUnitState_Base::
set_Button(BYTE button)
{
	m_Button = button;
}
void CBindingBoseLFUnitState_Base::
Reset()
{
	this->reset( );
}

BYTE CBindingBoseLFUnitState_Base::
get_Source() const
{
	return m_Source;
}

BYTE CBindingBoseLFUnitState_Base::
get_Button() const
{
	return m_Button;
}

void CBindingBoseLFUnitState_Base::
set_ControlEvent(BYTE bEvent)
{
	m_ControlEvent = bEvent;
}

BYTE CBindingBoseLFUnitState_Base::
get_ControlEvent() const
{
	return m_ControlEvent;
}

void CBindingBoseLFUnitState_Base::
set_FunctionTK(BYTE function)
{
	m_FunctionTK = function;
}

BYTE CBindingBoseLFUnitState_Base::
get_FunctionTK() const
{
	return m_FunctionTK;
}

void CBindingBoseLFUnitState_Base::
set_Event(BYTE ev)
{
	m_Event = ev;
}

BYTE CBindingBoseLFUnitState_Base::
get_Event() const
{
	return m_Event;
}

void CBindingBoseLFUnitState_Base::
set_State(BOOL fState)
{
	m_fState = fState;
}

BOOL CBindingBoseLFUnitState_Base::
get_State() const
{
	return m_fState;
}

void CBindingBoseLFUnitState_Base::
set_Function(BYTE bFunction)
{
	m_Function = bFunction;
}
void CBindingBoseLFUnitState_Base::
set_FunctionSrc(BYTE bFunction)
{
	m_FunctionSrc = bFunction;
}
void CBindingBoseLFUnitState_Base::
set_FunctionDest(BYTE bFunction)
{
	m_FunctionDest = bFunction;
}

BYTE CBindingBoseLFUnitState_Base::
get_Function() const
{
	return m_Function;
}
BYTE CBindingBoseLFUnitState_Base::
get_FunctionSrc() const
{
	return m_FunctionSrc;
}
BYTE CBindingBoseLFUnitState_Base::
get_FunctionDest() const
{
	return m_FunctionDest;
}

//////////////////////////////////////////////////////////////////////


CBindingBoseLF48UnitState::CBindingBoseLF48UnitState( ) :
CBindingBoseLFUnitState_Base( )
{
	TRACE("CBindingBoseLF48UnitState::CBindingBoseLF48UnitState( )\n");

	this->CBindingBoseLF48UnitState::reset();
};

CBindingBoseLF48UnitState::~CBindingBoseLF48UnitState()
{

}

void CBindingBoseLF48UnitState::
reset()
{
	m_Source = BOSELF48_SOURCE_AUX;
	m_Button = 0;
	m_ControlEvent = EV_UNITCONTROLPULS;
	m_FunctionTK = BOSELF48_SOURCE_FUNCTION_PLAY;
	m_Event = BOSELF48_EVENT_KEYPAD;
	m_fState = FALSE;	
	m_Function = AUDIO_OBJECT_ATTR_SOURCE_ON;
	m_FunctionSrc = 0;
	m_FunctionDest = 0;
}


//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
CBindingBoseV35UnitState::CBindingBoseV35UnitState( ) :
CBindingBoseLFUnitState_Base( )
{
	TRACE("CBindingBoseV35UnitState::CBindingBoseLF48UnitState( )\n");

	this->CBindingBoseV35UnitState::reset();
};

CBindingBoseV35UnitState::~CBindingBoseV35UnitState()
{

}

void CBindingBoseV35UnitState::
reset()
{
	m_Source = BOSEV35_SOURCE_HDMI1;
	m_Button = BOSEV35_SOURCE_FUNCTION_KEY1;
	m_ControlEvent = EV_UNITCONTROLPULS;
	m_FunctionTK = BOSEV35_SOURCE_FUNCTION_PLAY;
	m_Event = BOSEV35_EVENT_KEYPAD;
	m_fState = FALSE;	
	m_Function = BOSERS_OBJECT_ATTR_SOURCE_ON;
	m_FunctionSrc = 0;
	m_FunctionDest = 0;
	m_FunctionExtraIR = BOSELF48_SOURCE_FUNCTION_KEY1;
	m_bDestFunctionExtraIR = BOSEV35_DEST_FUNCTION_TV_POWER;
	m_bVolume = 20;
}

void CBindingBoseV35UnitState::
set_FunctionExtraIR(BYTE func)
{
	m_FunctionExtraIR = func;	
}

BYTE CBindingBoseV35UnitState::
get_FunctionExtraIR( ) const
{
	return m_FunctionExtraIR;
}

void CBindingBoseV35UnitState::
set_Volume(BYTE vol)
{
	m_bVolume = vol;
}

BYTE CBindingBoseV35UnitState::
get_Volume( ) const
{
	return m_bVolume;
}

void CBindingBoseV35UnitState::
set_DestFunctionExtraIR(BYTE func)
{
	m_bDestFunctionExtraIR = func;
}

BYTE CBindingBoseV35UnitState::
get_DestFunctionExtraIR( ) const
{
	return m_bDestFunctionExtraIR;
}

//////////////////////////////////////////////////////////////////////
CBindingAudioHC4UnitState::CBindingAudioHC4UnitState()
{
	Reset();	
}
CBindingAudioHC4UnitState::~CBindingAudioHC4UnitState()
{
}

void CBindingAudioHC4UnitState::Reset()
{
	m_Source = 0;
	m_Function = AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN;
	m_Destination = 0;
	m_fState = FALSE;
	m_Volume = 25;
	m_Keypress = 0;
	m_Config = 0;
	m_Event = EV_UNITCONTROLPULS;				/* TM_AUDIOHC4_BINDING_INPUT */
	m_InpMacro = 0;								/* TM_AUDIOHC4_BINDING_INPUT */
	m_ControlState = 1;							/* TM_AUDIOHC4_BINDING_INPUT */
}

void CBindingAudioHC4UnitState::
set_Source(BYTE source)
{
	m_Source = source;
}
void CBindingAudioHC4UnitState::
set_Function(BYTE function)
{
	m_Function = function;
}
void CBindingAudioHC4UnitState::
set_Destination(BYTE dest)
{
	m_Destination = dest;
}
void CBindingAudioHC4UnitState::
set_State(BOOL state)
{
	m_fState = state;
}
void CBindingAudioHC4UnitState::
set_Volume(BYTE vol)
{
	m_Volume = vol;
}
void CBindingAudioHC4UnitState::
set_Keypress(BYTE kp)
{
	m_Keypress = kp;
}
void CBindingAudioHC4UnitState::
set_Config(BYTE config)
{
	m_Config = config;
}

void CBindingAudioHC4UnitState::
set_Event(BYTE bEvent)				/* TM_AUDIOHC4_BINDING_INPUT */
{
	m_Event = bEvent;
}

void CBindingAudioHC4UnitState::
set_InpMacro(BYTE bMacro)			/* TM_AUDIOHC4_BINDING_INPUT */
{
	m_InpMacro = bMacro;
}

void CBindingAudioHC4UnitState::
set_ControlState(BYTE state)		/* TM_AUDIOHC4_BINDING_INPUT */
{
	m_ControlState = state;
}

BYTE CBindingAudioHC4UnitState::
get_Source() const
{
	return m_Source;
}
BYTE CBindingAudioHC4UnitState::
get_Function() const
{
	return m_Function;
}

BYTE CBindingAudioHC4UnitState::
get_Destination() const
{
	return m_Destination;
}

BOOL CBindingAudioHC4UnitState::
get_State() const
{
	return m_fState;
}

BYTE CBindingAudioHC4UnitState::
get_Volume() const
{
	return m_Volume;
}

BYTE CBindingAudioHC4UnitState::
get_Keypress() const
{
	return m_Keypress;
}

BYTE CBindingAudioHC4UnitState::
get_Config() const
{
	return m_Config;
}

BYTE CBindingAudioHC4UnitState::
get_Event() const					/* TM_AUDIOHC4_BINDING_INPUT */
{
	return m_Event;
}

BYTE CBindingAudioHC4UnitState::
get_InpMacro() const				/* TM_AUDIOHC4_BINDING_INPUT */
{
	return m_InpMacro;
}

BYTE CBindingAudioHC4UnitState::
get_ControlState() const			/* TM_AUDIOHC4_BINDING_INPUT */
{
	return m_ControlState;
}