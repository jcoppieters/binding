// FSMUnitStatus.cpp: implementation of the CFSMUnitStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FSMUnitStatus.h"
#include "RequestforNodeStatus.h"

#ifdef BINDING_SOFTWARE	
	#include "TXUnitMessage.h"
	#include "utils/app/DisplayDevice.h"
	#include "nodemess.h"

	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#else
	#include "..\Helper\TXUnitMessage.h"
	#include "..\Helper\DisplayDevice.h"
	#include "..\..\nodemess.h"
#endif


//-----------------------------------------------------------//
#define TRAN(x)		(CurrState = (x))
//-----------------------------------------------------------//

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFSMUnitStatus::CFSMUnitStatus(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
	m_pParent(pParent), bNodeAddress(bNode), bUnitAddress(bUnit)
{
	pCurrState = NULL;

	pTxUnitMessage = NULL;
	m_bMaxRetries = 3;
	m_bNrRetries = 0;
	nCurrentStateIndex = 0;
}

CFSMUnitStatus::~CFSMUnitStatus()
{
	if (pTxUnitMessage != NULL) 
	{
		delete pTxUnitMessage;
		pTxUnitMessage = NULL;
	}
}

//-----------------------------------------------------------//

void CFSMUnitStatus::FSM(enum_Events e,BYTE bMethodData)
{
	ASSERT(pCurrState);
	ASSERT(pCurrState->action != NULL);

	switch(e)
	{
		case FSM_EVENT_SIGNAL_STATUS_RESPONSE :
		{
			if (bMethodData == pCurrState->bMethodData) 
			{
				m_bNrRetries = 0;
				StopTimer();
			
				if (++nCurrentStateIndex < nMaxStates) 
				{
					pCurrState++;
					Invoke(pCurrState->action);
					StartTimer();
				}
				else 
				{
					Fire_UnitDone();
				}
			}
		} break;

		case FSM_EVENT_AFTER_TIMEOUT :
		{			
			StopTimer();
			if (m_bNrRetries++ < m_bMaxRetries) {			
				Invoke(pCurrState->action);
				StartTimer();
			}
			else  {
				Fire_UnitError();
			}
		} break;

		case FSM_EVENT_OPERATION_CANCEL :
		{
			StopTimer();
			pCurrState = NULL;
			Fire_Cancelled();

		} break;
			
		default:
		break;

/*
		case FSM_EVENT_OPERATION_START :
			ASSERT(0);
		break;
*/
	}
}


//-----------------------------------------------------------//
//		Events fired to Parent : CRequestForStatussen		 //
//-----------------------------------------------------------//
inline void CFSMUnitStatus::StartTimer(void) 
{
	if (m_pParent != NULL) 
	{
		m_pParent->StartTimer();
	}
}

inline void CFSMUnitStatus::StopTimer(void) {

	if (m_pParent != NULL) 
	{
		m_pParent->StopTimer();
	}
}

inline void CFSMUnitStatus::Fire_UnitDone(void) 
{	
	if (m_pParent != NULL) 
	{
		m_pParent->NotifyEvUnitDone();
	}
}

inline void CFSMUnitStatus::Fire_UnitError(void) 
{	
	if (m_pParent != NULL) 
	{
		m_pParent->NotifyEvUnitError();
	}
}

inline void CFSMUnitStatus::Fire_Cancelled(void) 
{		
	if (m_pParent != NULL) 
	{
		m_pParent->NotifyEvCancelled();
	}
}

void CFSMUnitStatus::Display(BYTE bNodeAddress,BYTE bUnitAddress,const char* s)
{
	char szMessage[100];
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : %s",bNodeAddress,bUnitAddress,s);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

//-----------------------------------------------------------//
//	Events Received from Parent : CRequestForStatussen		 //
//-----------------------------------------------------------//
void CFSMUnitStatus::EvAfterTimerExpiry(void) 
{
	FSM(FSM_EVENT_AFTER_TIMEOUT);
}
void CFSMUnitStatus::EvSignalStatusStatusReceived(BYTE bMethodData) 
{
	FSM(FSM_EVENT_SIGNAL_STATUS_RESPONSE,bMethodData);
}
void CFSMUnitStatus::EvOperationCancel(void) 
{	
	FSM(FSM_EVENT_OPERATION_CANCEL);
}
//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusDim			 // 
//-----------------------------------------------------------//

#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusDim::DimStates[MAX_DIMMER_STATES+1] =
	{
		{ DIMMER_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusDim::RequestConfig) },	// CHANGES_MVS_2008
		{ DIMMER_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusDim::RequestState) },	// CHANGES_REQUEST_UNITSTATUS
		{ 0xFF, NULL }
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusDim::DimStates[MAX_DIMMER_STATES + 1] =
	{
		{ DIMMER_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusDim::RequestConfig) },	// CHANGES_MVS_2008
		{ DIMMER_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusDim::RequestState) },	// CHANGES_MVS_2008
		{ DIMMER_OBJECT_ATTR_VALUE, static_cast<Action> (&CFSMUnitStatusDim::RequestValue) },	// CHANGES_MVS_2008
		{ DIMMER_OBJECT_ATTR_SPEED, static_cast<Action> (&CFSMUnitStatusDim::RequestSpeed) },	// CHANGES_MVS_2008
		{ DIMMER_OBJECT_ATTR_RANGE, static_cast<Action> (&CFSMUnitStatusDim::RequestRange) },	// CHANGES_MVS_2008
		{ 0xFF, NULL }
	};
#endif

CFSMUnitStatusDim::CFSMUnitStatusDim(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXDimmerUnitMessage();
	nMaxStates = MAX_DIMMER_STATES;
}

CFSMUnitStatusDim::~CFSMUnitStatusDim()
{
}


void CFSMUnitStatusDim::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(DimStates[nCurrentStateIndex].action);
	pCurrState = &DimStates[nCurrentStateIndex];
	StartTimer();
}

void CFSMUnitStatusDim::RequestConfig(void)  
{	
	CTXDimmerUnitMessage* const pTxUnitDimMessage = static_cast<CTXDimmerUnitMessage*>(pTxUnitMessage);
	if (0 != pTxUnitDimMessage)
	{
		pTxUnitDimMessage->RequestConfig(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request config");
	}
}

void CFSMUnitStatusDim::RequestState(void)  
{
	CTXDimmerUnitMessage* const pTxUnitDimMessage = static_cast<CTXDimmerUnitMessage*>(pTxUnitMessage);
	if (0 != pTxUnitDimMessage)
	{
		pTxUnitDimMessage->RequestState(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request state");
	}
}

void CFSMUnitStatusDim::RequestValue(void) 
{
	CTXDimmerUnitMessage* const pTxUnitDimMessage = static_cast<CTXDimmerUnitMessage*>(pTxUnitMessage);
	if (0 != pTxUnitDimMessage)
	{
		pTxUnitDimMessage->RequestValue(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request value");
	}
}

void CFSMUnitStatusDim::RequestSpeed(void) 
{
	CTXDimmerUnitMessage* const pTxUnitDimMessage = static_cast<CTXDimmerUnitMessage*>(pTxUnitMessage);
	if (0 != pTxUnitDimMessage)
	{
		pTxUnitDimMessage->RequestSpeed(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request speed");
	}
}

void CFSMUnitStatusDim::RequestRange(void) 
{
	CTXDimmerUnitMessage* const pTxUnitDimMessage = static_cast<CTXDimmerUnitMessage*>(pTxUnitMessage);
	if (0 != pTxUnitDimMessage)
	{
		pTxUnitDimMessage->RequestRange(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request range");
	}
}


//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusSwitch			 // 
//-----------------------------------------------------------//

#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusSwitch::SwitchStates[MAX_SWITCH_STATES+1] = 
	{
		{ SWITCH_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusSwitch::RequestConfig) },		// CHANGES_MVS_2008
		{ SWITCH_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusSwitch::RequestState) },		// CHANGES_REQUEST_UNITSTATUS
		{ 0xFF, NULL },
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusSwitch::SwitchStates[MAX_SWITCH_STATES + 1] = 
	{
		{ SWITCH_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusSwitch::RequestConfig) },		// CHANGES_MVS_2008
		{ SWITCH_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusSwitch::RequestState) },		// CHANGES_MVS_2008
		{ 0xFF, NULL },
	};
#endif

CFSMUnitStatusSwitch::CFSMUnitStatusSwitch(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXDimmerUnitMessage();
	nMaxStates = MAX_SWITCH_STATES;
}

CFSMUnitStatusSwitch::~CFSMUnitStatusSwitch()
{

}

void CFSMUnitStatusSwitch::RequestState(void) 
 {
	CTXSwitchUnitMessage* const pTxUnitSwitchMessage = static_cast<CTXSwitchUnitMessage*>(pTxUnitMessage);
	if (0 != pTxUnitSwitchMessage)
	{	
		pTxUnitSwitchMessage->RequestState(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request state");
	}
}

void CFSMUnitStatusSwitch::RequestConfig(void) 
 {
	CTXSwitchUnitMessage* const pTxUnitSwitchMessage = static_cast<CTXSwitchUnitMessage*>(pTxUnitMessage);
	if (0 != pTxUnitSwitchMessage)
	{	
		pTxUnitSwitchMessage->RequestConfig(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request config");
	}
}

void CFSMUnitStatusSwitch::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(SwitchStates[nCurrentStateIndex].action);
	pCurrState = &SwitchStates[nCurrentStateIndex];
	StartTimer();
}

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusDuoSwitch			 // 
//-----------------------------------------------------------//
#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusDuoSwitch::DuoSwitchStates[MAX_DUOSWITCH_STATES+1] = 
	{
		{ DUOSWITCH_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusDuoSwitch::RequestConfig) },	// CHANGES_MVS_2008
		{ DUOSWITCH_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusDuoSwitch::RequestState) },		// CHANGES_REQUEST_UNITSTATUS
		{ 0xFF, NULL }
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusDuoSwitch::DuoSwitchStates[MAX_DUOSWITCH_STATES + 1] = 
	{
		{ DUOSWITCH_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusDuoSwitch::RequestConfig) },	// CHANGES_MVS_2008
		{ DUOSWITCH_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusDuoSwitch::RequestState) },		// CHANGES_MVS_2008
		{ 0xFF, NULL }
	};
#endif

CFSMUnitStatusDuoSwitch::CFSMUnitStatusDuoSwitch(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXDuoSwitchUnitMessage();
	nMaxStates = MAX_DUOSWITCH_STATES;
}

CFSMUnitStatusDuoSwitch::~CFSMUnitStatusDuoSwitch()
{
}

void CFSMUnitStatusDuoSwitch::RequestState(void)  
{	
	CTXDuoSwitchUnitMessage* const pTxUnitDuoSwitchMessage = static_cast <CTXDuoSwitchUnitMessage*> (pTxUnitMessage);
	if (0 != pTxUnitDuoSwitchMessage)
	{
		pTxUnitDuoSwitchMessage->RequestState(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request state");
	}
}

void CFSMUnitStatusDuoSwitch::RequestConfig(void)  
{
	CTXDuoSwitchUnitMessage* const pTxUnitDuoSwitchMessage = static_cast <CTXDuoSwitchUnitMessage*> (pTxUnitMessage);
	if (0 != pTxUnitDuoSwitchMessage)
	{
		pTxUnitDuoSwitchMessage->RequestConfig(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request config");
	}
}

void CFSMUnitStatusDuoSwitch::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(DuoSwitchStates[nCurrentStateIndex].action);
	pCurrState = &DuoSwitchStates[nCurrentStateIndex];
	StartTimer();
}

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusControl			 // 
//-----------------------------------------------------------//
const STRUCT_UNITSTATUS_STATES CFSMUnitStatusControl::ControlStates[MAX_CONTROL_STATES + 1] = 
{
	{ CONTROL_OBJECT_STATUS, static_cast<Action> (&CFSMUnitStatusControl::RequestState) },		// CHANGES_MVS_2008
	{ 0xFF, NULL }
};

CFSMUnitStatusControl::CFSMUnitStatusControl(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXControlUnitMessage();
	nMaxStates = MAX_CONTROL_STATES;
}

CFSMUnitStatusControl::~CFSMUnitStatusControl()
{
}

void CFSMUnitStatusControl::RequestState(void)  
{	
	CTXControlUnitMessage* const pTxControlMessage = static_cast <CTXControlUnitMessage*> (pTxUnitMessage);
	if (0 != pTxControlMessage)
	{
		pTxControlMessage->RequestControlStatus(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request State");
	}
}

void CFSMUnitStatusControl::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(ControlStates[nCurrentStateIndex].action);
	pCurrState = &ControlStates[nCurrentStateIndex];
	StartTimer();
}

//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusSens			 // 
//-----------------------------------------------------------//

// CHANGES_REQUEST_UNITSTATUS:
// Geen aanpassing aan opvragen van de status van de sensor want wordt enkel getoond in de dialoog.
// Voor wanneer de dialoog getoond wordt worden de statussen toch opgevraagd.
#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusSens::SensStates[MAX_SENS_STATES + 1] = 
	{
		{ SENS_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusSens::RequestSensType) },		// CHANGES_MVS_2008	
		{ 0xFF, NULL }
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusSens::SensStates[MAX_SENS_STATES + 1] = 
	{
		{ SENS_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusSens::RequestSensType) },		// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusSens::RequestState) },		// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_CONTROL_ONOFF, static_cast<Action> (&CFSMUnitStatusSens::RequestStateOnOff) },	// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_CURRENT_MEASURE, static_cast<Action> (&CFSMUnitStatusSens::RequestMeasuredValue) },	// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_PRESET, static_cast<Action> (&CFSMUnitStatusSens::RequestPreset) },	// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_WORKING_SET_POINT, static_cast<Action> (&CFSMUnitStatusSens::RequestSetpoint_Sun) },	// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_WORKING_SET_POINT, static_cast<Action> (&CFSMUnitStatusSens::RequestSetpoint_HalfSun) },	// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_WORKING_SET_POINT, static_cast<Action> (&CFSMUnitStatusSens::RequestSetpoint_Moon) },	// CHANGES_MVS_2008
		{ SENS_OBJECT_ATTR_WORKING_SET_POINT, static_cast<Action> (&CFSMUnitStatusSens::RequestSetpoint_HalfMoon) },	// CHANGES_MVS_2008
		{ 0xFF, NULL }
	};

#endif


CFSMUnitStatusSens::CFSMUnitStatusSens(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXSensUnitMessage();
	nMaxStates = MAX_SENS_STATES;
}

CFSMUnitStatusSens::~CFSMUnitStatusSens()
{
}

void CFSMUnitStatusSens::RequestState(void)  
{	
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestState(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request State");
	}
}

void CFSMUnitStatusSens::RequestSensType(void)  
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestSensType(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request Type");
	}
}

void CFSMUnitStatusSens::RequestStateOnOff(void)
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestOnOff(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request State on/off");
	}
}

void CFSMUnitStatusSens::RequestMeasuredValue(void)
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestMeasuredVal(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request measured value");
	}
}

void CFSMUnitStatusSens::RequestPreset(void)
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestPreset(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request preset");
	}
}

void CFSMUnitStatusSens::RequestSetpoint_Sun(void)
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestSetPoint(bNodeAddress,bUnitAddress,0);
		Display(bNodeAddress,bUnitAddress,"Request setpoint sun");
	}
}

void CFSMUnitStatusSens::RequestSetpoint_HalfSun(void)
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestSetPoint(bNodeAddress,bUnitAddress,1);
		Display(bNodeAddress,bUnitAddress,"Request setpoint half sun");
	}
}

void CFSMUnitStatusSens::RequestSetpoint_Moon(void)
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestSetPoint(bNodeAddress,bUnitAddress,2);
		Display(bNodeAddress,bUnitAddress,"Request setpoint moon");
	}
}

void CFSMUnitStatusSens::RequestSetpoint_HalfMoon(void)
{
	CTXSensUnitMessage* const pTxSensMessage = static_cast <CTXSensUnitMessage*> (pTxUnitMessage);
	if (0 != pTxSensMessage)
	{
		pTxSensMessage->RequestSetPoint(bNodeAddress,bUnitAddress,3);
		Display(bNodeAddress,bUnitAddress,"Request setpoint half moon");
	}
}

void CFSMUnitStatusSens::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(SensStates[nCurrentStateIndex].action);
	pCurrState = &SensStates[nCurrentStateIndex];
	StartTimer();
}


//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusAudio			 // 
//-----------------------------------------------------------//
#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusAudio::AudioStates[MAX_AUDIO_STATES+1] = 
	{
		{ AUDIO_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusAudio::RequestConfig) },	// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusAudio::RequestState) },		// CHANGES_REQUEST_UNITSTATUS
		{ 0xFF, NULL },
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusAudio::AudioStates[MAX_AUDIO_STATES + 1] = 
	{
		{ AUDIO_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusAudio::RequestConfig) },		// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusAudio::RequestState) },			// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_SOURCE_ON, static_cast<Action> (&CFSMUnitStatusAudio::RequestSource) },		// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_ONOFF, static_cast<Action> (&CFSMUnitStatusAudio::RequestOnOff) },			// CHANGES_MVS_2008
		{ 0xFF, NULL },
	};
#endif

CFSMUnitStatusAudio::CFSMUnitStatusAudio(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXAudioBasicUnitMessage();
	nMaxStates = MAX_AUDIO_STATES;
}

CFSMUnitStatusAudio::~CFSMUnitStatusAudio()
{
}

void CFSMUnitStatusAudio::RequestConfig(void) 
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusAudio::RequestState(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestState(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request State",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusAudio::RequestSource(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestSourceOn(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Source",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusAudio::RequestOnOff(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestOnOff(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Dest on/off",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusAudio::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(AudioStates[nCurrentStateIndex].action);
	pCurrState = &AudioStates[nCurrentStateIndex];
	StartTimer();
}


//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusBose			 // 
//-----------------------------------------------------------//

#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusBose::BoseStates[MAX_BOSE_STATES+1] = 
	{
		{ AUDIO_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusBose::RequestConfig) },		// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusBose::RequestState) },		// CHANGES_REQUEST_UNITSTATUS
		{ 0xFF, NULL },
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusBose::BoseStates[MAX_BOSE_STATES + 1] = 
	{
		{ AUDIO_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusBose::RequestConfig) },		// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusBose::RequestState) },		// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_SOURCE_ON, static_cast<Action> (&CFSMUnitStatusBose::RequestSource) },	// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_ONOFF, static_cast<Action> (&CFSMUnitStatusBose::RequestOnOff) },		// CHANGES_MVS_2008
		{ AUDIO_OBJECT_ATTR_VOLUME, static_cast<Action> (&CFSMUnitStatusBose::RequestVolume) },		// CHANGES_MVS_2008
		{ 0xFF, NULL },
	};
#endif

CFSMUnitStatusBose::CFSMUnitStatusBose(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXAudioBasicUnitMessage();
	nMaxStates = MAX_BOSE_STATES;
}

CFSMUnitStatusBose::~CFSMUnitStatusBose()
{
}

void CFSMUnitStatusBose::RequestConfig(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusBose::RequestState(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestState(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request State",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusBose::RequestSource(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestSourceOn(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Source",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusBose::RequestOnOff(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestOnOff(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Dest on/off",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusBose::RequestVolume(void)  
{
	char szMessage[100];
	CTXAudioBasicUnitMessage *pTxAudioMessage = NULL;
	pTxAudioMessage = static_cast <CTXAudioBasicUnitMessage*> (pTxUnitMessage);
	pTxAudioMessage->RequestVolume(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request volume",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusBose::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(BoseStates[nCurrentStateIndex].action);
	pCurrState = &BoseStates[nCurrentStateIndex];
	StartTimer();
}



//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusIRTX			 // 
//-----------------------------------------------------------//

#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusIRTX::IRTXStates[MAX_IRTX_STATES + 1] =
	{
		{ IRTX_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusIRTX::RequestConfig) },		// CHANGES_MVS_2008
		{ 0xFF, NULL }
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusIRTX::IRTXStates[MAX_IRTX_STATES + 1] =
	{
		{ IRTX_OBJECT_ATTR_STATE, static_cast<Action> (&CFSMUnitStatusIRTX::RequestState) },		// CHANGES_MVS_2008
		{ IRTX_OBJECT_ATTR_CONFIG, static_cast<Action> (&CFSMUnitStatusIRTX::RequestConfig) },		// CHANGES_MVS_2008
		{ 0xFF, NULL }
	};
#endif

CFSMUnitStatusIRTX::CFSMUnitStatusIRTX(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CIRTXUnitMessage();
	nMaxStates = MAX_IRTX_STATES;
}

CFSMUnitStatusIRTX::~CFSMUnitStatusIRTX()
{
}

void CFSMUnitStatusIRTX::RequestState(void) 
{
	char szMessage[100];
	CIRTXUnitMessage *pTxIRTXMessage = NULL;
	pTxIRTXMessage = static_cast <CIRTXUnitMessage*> (pTxUnitMessage);
	pTxIRTXMessage->RequestState(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request State",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusIRTX::RequestConfig(void)  
{
	char szMessage[100];
	CIRTXUnitMessage *pTxIRTXMessage = NULL;
	pTxIRTXMessage = static_cast <CIRTXUnitMessage*> (pTxUnitMessage);
	pTxIRTXMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusIRTX::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(IRTXStates[nCurrentStateIndex].action);
	pCurrState = &IRTXStates[nCurrentStateIndex];
	StartTimer();
}


//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusRC5RX			 // 
//-----------------------------------------------------------//
#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusRC5RX::RC5RxStates[MAX_IRRC5_STATES + 1] =
	{	
		{RC5RX_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusRC5RX::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL },
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusRC5RX::RC5RxStates[MAX_IRRC5_STATES + 1] =
	{
		{RC5RX_OBJECT_ATTR_STATE,static_cast<Action> (&CFSMUnitStatusRC5RX::RequestState) },		// CHANGES_MVS_2008
		{RC5RX_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusRC5RX::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL },
	};
#endif

CFSMUnitStatusRC5RX::CFSMUnitStatusRC5RX(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CIRRC5UnitMessage();
	nMaxStates = MAX_IRRC5_STATES;
}

CFSMUnitStatusRC5RX::~CFSMUnitStatusRC5RX()
{
}

void CFSMUnitStatusRC5RX::RequestState(void)  
{
	char szMessage[100];
	CIRRC5UnitMessage *pTxIRRXMessage = NULL;
	pTxIRRXMessage = static_cast <CIRRC5UnitMessage*> (pTxUnitMessage);
	pTxIRRXMessage->RequestState(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request State",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusRC5RX::RequestConfig(void)  
{
	char szMessage[100];
	CIRRC5UnitMessage *pTxIRRXMessage = NULL;
	pTxIRRXMessage = static_cast <CIRRC5UnitMessage*> (pTxUnitMessage);
	pTxIRRXMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusRC5RX::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(RC5RxStates[nCurrentStateIndex].action);
	pCurrState = &RC5RxStates[nCurrentStateIndex];
	StartTimer();
}

//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusAVMatrix		 // 
//-----------------------------------------------------------//
const STRUCT_UNITSTATUS_STATES CFSMUnitStatusAVMatrix::AVMatrixStates[MAX_AVMATRIX_STATES + 1] =
{
	{AVMATRIX_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusAVMatrix::RequestConfig) },	// CHANGES_MVS_2008
	{0xFF,NULL },
};

CFSMUnitStatusAVMatrix::CFSMUnitStatusAVMatrix(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXAVMatrixUnitMessage();
	nMaxStates = MAX_AVMATRIX_STATES;
}

CFSMUnitStatusAVMatrix::~CFSMUnitStatusAVMatrix()
{
}

void CFSMUnitStatusAVMatrix::RequestConfig(void)  
{
	char szMessage[100];
	CTXAVMatrixUnitMessage *pTxAVMatrixMessage = NULL;
	pTxAVMatrixMessage = static_cast <CTXAVMatrixUnitMessage*> (pTxUnitMessage);
	pTxAVMatrixMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusAVMatrix::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(AVMatrixStates[nCurrentStateIndex].action);
	pCurrState = &AVMatrixStates[nCurrentStateIndex];
	StartTimer();
}

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusAlarm			 // 
//-----------------------------------------------------------//
#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusAlarm::AlarmStates[MAX_ALARM_STATES + 1] =
	{	
		{ALARM_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusAlarm::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL },
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusAlarm::AlarmStates[MAX_ALARM_STATES + 1] =
	{
		{ALARM_OBJECT_ATTR_STATE,static_cast<Action> (&CFSMUnitStatusAlarm::RequestState) },		// CHANGES_MVS_2008
		{ALARM_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusAlarm::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL },
	};
#endif

CFSMUnitStatusAlarm::CFSMUnitStatusAlarm(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXAlarmUnitMessage();
	nMaxStates = MAX_AVMATRIX_STATES;
}

CFSMUnitStatusAlarm::~CFSMUnitStatusAlarm()
{
}

void CFSMUnitStatusAlarm::RequestConfig(void)  
{
	char szMessage[100];
	CTXAlarmUnitMessage *pTxAlarmMessage = NULL;
	pTxAlarmMessage = static_cast <CTXAlarmUnitMessage*> (pTxUnitMessage);
	pTxAlarmMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusAlarm::RequestState(void)  
{
	char szMessage[100];
	CTXAlarmUnitMessage *pTxAlarmMessage = NULL;
	pTxAlarmMessage = static_cast <CTXAlarmUnitMessage*> (pTxUnitMessage);
	pTxAlarmMessage->Requeststate(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request State",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusAlarm::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(AlarmStates[nCurrentStateIndex].action);
	pCurrState = &AlarmStates[nCurrentStateIndex];
	StartTimer();
}



//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusVideomux			 // 
//-----------------------------------------------------------//
#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusVideomux::VideomuxStates[MAX_VIDEOMUX_STATES + 1] =
	{	
		{VIDEOMUX_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusVideomux::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL },
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusVideomux::VideomuxStates[MAX_VIDEOMUX_STATES + 1] =
	{
		{VIDEOMUX_OBJECT_ATTR_STATE,static_cast<Action> (&CFSMUnitStatusVideomux::RequestState) },			// CHANGES_MVS_2008
		{VIDEOMUX_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusVideomux::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL },
	};
#endif

CFSMUnitStatusVideomux::CFSMUnitStatusVideomux(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXVideomuxUnitMessage();
	nMaxStates = MAX_VIDEOMUX_STATES;
}

CFSMUnitStatusVideomux::~CFSMUnitStatusVideomux()
{
}

void CFSMUnitStatusVideomux::RequestConfig(void)  
{
	char szMessage[100];
	CTXVideomuxUnitMessage *pTxVideomuxMessage = NULL;
	pTxVideomuxMessage = static_cast <CTXVideomuxUnitMessage*> (pTxUnitMessage);
	pTxVideomuxMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusVideomux::RequestState(void)  
{
	char szMessage[100];
	CTXVideomuxUnitMessage *pTxVideomuxMessage = NULL;
	pTxVideomuxMessage = static_cast <CTXVideomuxUnitMessage*> (pTxUnitMessage);
	pTxVideomuxMessage->Requeststate(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request State",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusVideomux::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(VideomuxStates[nCurrentStateIndex].action);
	pCurrState = &VideomuxStates[nCurrentStateIndex];
	StartTimer();
}


//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusPS2Mouse			 // 
//-----------------------------------------------------------//
#ifdef BINDING_SOFTWARE
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusPS2Mouse::PS2MouseStates[MAX_PS2MOUSE_STATES + 1] =
	{	
		{PS2INTELLIMOUSE_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusPS2Mouse::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL }
	};
#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusPS2Mouse::PS2MouseStates[MAX_PS2MOUSE_STATES + 1] =
	{
		{PS2INTELLIMOUSE_OBJECT_ATTR_STATE,static_cast<Action> (&CFSMUnitStatusPS2Mouse::RequestState) },		// CHANGES_MVS_2008
		{PS2INTELLIMOUSE_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusPS2Mouse::RequestConfig) },		// CHANGES_MVS_2008
		{0xFF,NULL }
	};
#endif



CFSMUnitStatusPS2Mouse::CFSMUnitStatusPS2Mouse(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXPS2MouseUnitMessage();
	nMaxStates = MAX_PS2MOUSE_STATES;
}

CFSMUnitStatusPS2Mouse::~CFSMUnitStatusPS2Mouse()
{
}

void CFSMUnitStatusPS2Mouse::RequestConfig(void)  
{
	char szMessage[100];
	CTXPS2MouseUnitMessage *pTxPS2MouseMessage = NULL;
	pTxPS2MouseMessage = static_cast <CTXPS2MouseUnitMessage*> (pTxUnitMessage);
	pTxPS2MouseMessage->RequestConfig(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request Config",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusPS2Mouse::RequestState(void)  
{
	char szMessage[100];
	CTXPS2MouseUnitMessage *pTxPS2MouseMessage = NULL;
	pTxPS2MouseMessage = static_cast <CTXPS2MouseUnitMessage*> (pTxUnitMessage);
	pTxPS2MouseMessage->Requeststate(bNodeAddress,bUnitAddress);
	sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Request State",bNodeAddress,bUnitAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CFSMUnitStatusPS2Mouse::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(PS2MouseStates[nCurrentStateIndex].action);
	pCurrState = &PS2MouseStates[nCurrentStateIndex];
	StartTimer();
}

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusAudioHC4			 // 
//-----------------------------------------------------------//

#ifdef BINDING_SOFTWARE
	
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusAudioHC4::AudioHC4States[MAX_AUDIO_HC4_STATES + 1] =
	{
		{AUDIO_MATRIX_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestConfig) },	// CHANGES_MVS_2008
		{AUDIO_MATRIX_OBJECT_ATTR_STATE,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestState) },		// CHANGES_REQUEST_UNITSTATUS
		{0xFF,NULL },
	};

#else
	const STRUCT_UNITSTATUS_STATES CFSMUnitStatusAudioHC4::AudioHC4States[MAX_AUDIO_HC4_STATES + 1] =
	{
		{AUDIO_MATRIX_OBJECT_ATTR_STATE,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestState) },		// CHANGES_MVS_2008
		{AUDIO_MATRIX_OBJECT_ATTR_CONFIG,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestConfig) },	// CHANGES_MVS_2008	
		{AUDIO_MATRIX_OBJECT_ATTR_DEST_STATUS,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestDestState) },	// CHANGES_MVS_2008
		{AUDIO_MATRIX_OBJECT_ATTR_VOLUME,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestVol_Dest0) },	// CHANGES_MVS_2008
		{AUDIO_MATRIX_OBJECT_ATTR_VOLUME,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestVol_Dest1) }, // CHANGES_MVS_2008
		{AUDIO_MATRIX_OBJECT_ATTR_VOLUME,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestVol_Dest2) },	// CHANGES_MVS_2008
		{AUDIO_MATRIX_OBJECT_ATTR_VOLUME,static_cast<Action> (&CFSMUnitStatusAudioHC4::RequestVol_Dest3) },	// CHANGES_MVS_2008
		{0xFF,NULL },
	};
#endif

CFSMUnitStatusAudioHC4::CFSMUnitStatusAudioHC4(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXAudioHC4UnitMessage();
	nMaxStates = MAX_AUDIO_HC4_STATES;
}

CFSMUnitStatusAudioHC4::~CFSMUnitStatusAudioHC4()
{
}

void CFSMUnitStatusAudioHC4::RequestConfig(void)  
{
	CTXAudioHC4UnitMessage* const pTxAudioHC4Message = static_cast<CTXAudioHC4UnitMessage*>(pTxUnitMessage);
	if (0 != pTxAudioHC4Message)
	{	
		pTxAudioHC4Message->RequestConfig(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request config");
	}
}

void CFSMUnitStatusAudioHC4::RequestState(void)  
{
	CTXAudioHC4UnitMessage* const pTxAudioHC4Message = static_cast<CTXAudioHC4UnitMessage*>(pTxUnitMessage);
	if (0 != pTxAudioHC4Message)
	{	
		pTxAudioHC4Message->RequestState(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request state");
	}
}


void CFSMUnitStatusAudioHC4::RequestVol_Dest0(void)  
{
	CTXAudioHC4UnitMessage* const pTxAudioHC4Message = static_cast<CTXAudioHC4UnitMessage*>(pTxUnitMessage);
	if (0 != pTxAudioHC4Message)
	{	
		pTxAudioHC4Message->RequestVolume(bNodeAddress,bUnitAddress,0);
		Display(bNodeAddress,bUnitAddress,"Request destination 0 volume");
	}
}

void CFSMUnitStatusAudioHC4::RequestVol_Dest1(void)  
{
	CTXAudioHC4UnitMessage* const pTxAudioHC4Message = static_cast<CTXAudioHC4UnitMessage*>(pTxUnitMessage);
	if (0 != pTxAudioHC4Message)
	{	
		pTxAudioHC4Message->RequestVolume(bNodeAddress,bUnitAddress,1);
		Display(bNodeAddress,bUnitAddress,"Request destination 1 volume");
	}
}

void CFSMUnitStatusAudioHC4::RequestVol_Dest2(void)  
{
	CTXAudioHC4UnitMessage* const pTxAudioHC4Message = static_cast<CTXAudioHC4UnitMessage*>(pTxUnitMessage);
	if (0 != pTxAudioHC4Message)
	{	
		pTxAudioHC4Message->RequestVolume(bNodeAddress,bUnitAddress,2);
		Display(bNodeAddress,bUnitAddress,"Request destination 2 volume");
	}
}

void CFSMUnitStatusAudioHC4::RequestVol_Dest3(void)  
{
	CTXAudioHC4UnitMessage* const pTxAudioHC4Message = static_cast<CTXAudioHC4UnitMessage*>(pTxUnitMessage);
	if (0 != pTxAudioHC4Message)
	{	
		pTxAudioHC4Message->RequestVolume(bNodeAddress,bUnitAddress,3);
		Display(bNodeAddress,bUnitAddress,"Request destination 3 volume");
	}
}

void CFSMUnitStatusAudioHC4::RequestDestState(void)  
{
	CTXAudioHC4UnitMessage* const pTxAudioHC4Message = static_cast<CTXAudioHC4UnitMessage*>(pTxUnitMessage);
	if (0 != pTxAudioHC4Message)
	{	
		pTxAudioHC4Message->RequestDestStatus(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request Destination States");
	}
}

void CFSMUnitStatusAudioHC4::EvOperationStart(void) 
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(AudioHC4States[nCurrentStateIndex].action);
	pCurrState = &AudioHC4States[nCurrentStateIndex];
	StartTimer();
}


//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusEnergyMeasure		 // 
//-----------------------------------------------------------//

// IMPLEMENT_UNIT_ENERGY
const STRUCT_UNITSTATUS_STATES CFSMUnitStatusEnergyMeasure::States[MAX_ENERGYMEASURE_STATES+1] =
{
	{EM_ATTR_CONFIG,static_cast<Action>(&CFSMUnitStatusEnergyMeasure::RequestConfig)},
/*
	{EM_ATTR_STATE,static_cast<Action>(&CFSMUnitStatusEnergyMeasure::RequestState)},
	{EM_ATTR_SUPPORTED,static_cast<Action>(&CFSMUnitStatusEnergyMeasure::RequestSupported)},
*/
	{0xFF,NULL },
};

CFSMUnitStatusEnergyMeasure::CFSMUnitStatusEnergyMeasure(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit) :
CFSMUnitStatus(pParent,bNode,bUnit)
{
	pTxUnitMessage = new CTXEnergyMeasureUnitMessage();
	nMaxStates = MAX_ENERGYMEASURE_STATES;
}

CFSMUnitStatusEnergyMeasure::~CFSMUnitStatusEnergyMeasure()
{

}

void CFSMUnitStatusEnergyMeasure::RequestConfig(void)
{	
	CTXEnergyMeasureUnitMessage* const pTxMessage = static_cast<CTXEnergyMeasureUnitMessage*>(pTxUnitMessage);
	if (0 != pTxMessage)
	{
		pTxMessage->RequestConfig(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request config");		
	}
}

void CFSMUnitStatusEnergyMeasure::RequestState(void)
{
	CTXEnergyMeasureUnitMessage* const pTxMessage = static_cast<CTXEnergyMeasureUnitMessage*>(pTxUnitMessage);
	if (0 != pTxMessage)
	{
		pTxMessage->RequestState(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request state");		
	}
}
void CFSMUnitStatusEnergyMeasure::RequestSupported(void)
{
	CTXEnergyMeasureUnitMessage* const pTxMessage = static_cast<CTXEnergyMeasureUnitMessage*>(pTxUnitMessage);
	if (0 != pTxMessage)
	{
		pTxMessage->RequestSupportedFlags(bNodeAddress,bUnitAddress);
		Display(bNodeAddress,bUnitAddress,"Request supported");		
	}
}

void CFSMUnitStatusEnergyMeasure::EvOperationStart(void)
{
	nCurrentStateIndex = 0;
	m_bNrRetries = 0;
	Invoke(States[nCurrentStateIndex].action);
	pCurrState = &States[nCurrentStateIndex];
	StartTimer();
}

