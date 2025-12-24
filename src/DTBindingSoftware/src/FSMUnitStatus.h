// FSMUnitStatus.h: interface for the CFSMUnitStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSMUNITSTATUS_H__2FA4BB3D_AD7F_4560_8234_35E6EFC5C0DD__INCLUDED_)
#define AFX_FSMUNITSTATUS_H__2FA4BB3D_AD7F_4560_8234_35E6EFC5C0DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//-----------------------------------------------------------//
class CTXUnitMessage;
class IRequestForNodeStatus;
//-----------------------------------------------------------//
class CFSMUnitStatus;
typedef void (CFSMUnitStatus::*Action) (void);	
//-----------------------------------------------------------//

typedef struct 
{
	BYTE bMethodData;
	Action action;

} STRUCT_UNITSTATUS_STATES;

//-----------------------------------------------------------//
//				Base Class : CFSMUnitStatus					 // 
//-----------------------------------------------------------//
class CFSMUnitStatus  
{
protected:	
	enum enum_Events 
	{	
		//FSM_EVENT_OPERATION_START	= 0,		//Event_Start = 0,
		FSM_EVENT_SIGNAL_STATUS_RESPONSE = 1,	//Event_Response,
		FSM_EVENT_OPERATION_CANCEL,				//Event_Cancel,
		FSM_EVENT_AFTER_TIMEOUT					//Event_Timeout
	};
	
	void Invoke(Action Actie) 
	{
		(this->*Actie)();
	}

	const STRUCT_UNITSTATUS_STATES *pCurrState;

	int nMaxStates;
	int nCurrentStateIndex;

	IRequestForNodeStatus *const m_pParent;
	const BYTE bNodeAddress;
	const BYTE bUnitAddress;	
	CTXUnitMessage *pTxUnitMessage;
	BYTE m_bMaxRetries;
	BYTE m_bNrRetries;

protected:
	void FSM(enum_Events e,BYTE bMethodData = 0xFF);
	void StartTimer(void);		
	void StopTimer(void);

	void Fire_UnitDone(void);
	void Fire_UnitError(void);
	void Fire_Cancelled(void);

	void Display(BYTE bNodeAddress,BYTE bUnitAddress,const char* s);

public:
	CFSMUnitStatus(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatus();

	void EvAfterTimerExpiry(void);		//Event from Parent : CRequest
	void EvOperationCancel(void);
	void EvSignalStatusStatusReceived(BYTE bMethodData);

	/**
	 * Must be overriden by all clients --> This operation triggers FSM !!!
	 */
	virtual void EvOperationStart(void) {};
};



//-----------------------------------------------------------//
//				Derived Class : CFSMUnitStatusDim			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusDim : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_DIMMER_STATES		2		// CHANGES_REQUEST_UNITSTATUS
	#else
		#define MAX_DIMMER_STATES		5	
	#endif

	const static STRUCT_UNITSTATUS_STATES DimStates[MAX_DIMMER_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);
	void RequestValue(void);
	void RequestSpeed(void);
	void RequestRange(void);


public:
	CFSMUnitStatusDim(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusDim();

	virtual void EvOperationStart(void);

};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusSwitch			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusSwitch : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_SWITCH_STATES		2 // CHANGES_REQUEST_UNITSTATUS		
	#else
		#define MAX_SWITCH_STATES		2	
	#endif

	const static STRUCT_UNITSTATUS_STATES SwitchStates[MAX_SWITCH_STATES + 1];

	void RequestState(void);
	void RequestConfig(void);

public:
	CFSMUnitStatusSwitch(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusSwitch();

	virtual void EvOperationStart(void);

};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusDuoSwitch			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusDuoSwitch : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_DUOSWITCH_STATES		2	// CHANGES_REQUEST_UNITSTATUS	
	#else
		#define MAX_DUOSWITCH_STATES		2	
	#endif

	const static STRUCT_UNITSTATUS_STATES DuoSwitchStates[MAX_DUOSWITCH_STATES + 1];


	void RequestState(void);
	void RequestConfig(void);

public:
	CFSMUnitStatusDuoSwitch(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusDuoSwitch();

	virtual void EvOperationStart(void);

};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusControl			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusControl : public CFSMUnitStatus
{
protected:

	#define MAX_CONTROL_STATES		1
	const static STRUCT_UNITSTATUS_STATES ControlStates[MAX_CONTROL_STATES + 1];

	void RequestState(void);

public:
	CFSMUnitStatusControl(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusControl();

	virtual void EvOperationStart(void);

};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusSens				 // 
//-----------------------------------------------------------//
class CFSMUnitStatusSens : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_SENS_STATES		1	
	#else
		#define MAX_SENS_STATES		9	
	#endif

	const static STRUCT_UNITSTATUS_STATES SensStates[MAX_SENS_STATES + 1];
	
	void RequestSensType(void);
	void RequestState(void);
	void RequestStateOnOff(void);
	void RequestMeasuredValue(void);
	void RequestPreset(void);
	void RequestSetpoint_Sun(void);
	void RequestSetpoint_HalfSun(void);
	void RequestSetpoint_Moon(void);
	void RequestSetpoint_HalfMoon(void);


public:
	CFSMUnitStatusSens(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusSens();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusAudio				 // 
//-----------------------------------------------------------//
class CFSMUnitStatusAudio : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_AUDIO_STATES		2		// CHANGES_REQUEST_UNITSTATUS
	#else
		#define MAX_AUDIO_STATES		4		
	#endif

	const static STRUCT_UNITSTATUS_STATES AudioStates[MAX_AUDIO_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);
	void RequestSource(void);
	void RequestOnOff(void);

public:
	CFSMUnitStatusAudio(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusAudio();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusBose				 // 
//-----------------------------------------------------------//
class CFSMUnitStatusBose : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_BOSE_STATES		2		// CHANGES_REQUEST_UNITSTATUS
	#else
		#define MAX_BOSE_STATES		5	
	#endif

	const static STRUCT_UNITSTATUS_STATES BoseStates[MAX_BOSE_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);
	void RequestSource(void);
	void RequestOnOff(void);
	void RequestVolume(void);

public:
	CFSMUnitStatusBose(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusBose();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusIRTX				 // 
//-----------------------------------------------------------//
class CFSMUnitStatusIRTX : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_IRTX_STATES		1	
	#else
		#define MAX_IRTX_STATES		2
	#endif

	const static STRUCT_UNITSTATUS_STATES IRTXStates[MAX_IRTX_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);

public:
	CFSMUnitStatusIRTX(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusIRTX();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusRC5RX				 // 
//-----------------------------------------------------------//
class CFSMUnitStatusRC5RX : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_IRRC5_STATES		1	
	#else
		#define MAX_IRRC5_STATES		2	
	#endif

	const static STRUCT_UNITSTATUS_STATES RC5RxStates[MAX_IRRC5_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);

public:
	CFSMUnitStatusRC5RX(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusRC5RX();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusAVMatrix			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusAVMatrix : public CFSMUnitStatus
{
protected:
	#define MAX_AVMATRIX_STATES		1
	const static STRUCT_UNITSTATUS_STATES AVMatrixStates[MAX_AVMATRIX_STATES + 1];

	void RequestConfig(void);

public:
	CFSMUnitStatusAVMatrix(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusAVMatrix();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusAlarm			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusAlarm : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_ALARM_STATES		1
	#else
		#define MAX_ALARM_STATES		2	
	#endif

	const static STRUCT_UNITSTATUS_STATES AlarmStates[MAX_ALARM_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);

public:
	CFSMUnitStatusAlarm(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusAlarm();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusVideomux			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusVideomux : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_VIDEOMUX_STATES		1
	#else
		#define MAX_VIDEOMUX_STATES		2
	#endif

	const static STRUCT_UNITSTATUS_STATES VideomuxStates[MAX_VIDEOMUX_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);

public:
	CFSMUnitStatusVideomux(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusVideomux();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusPS2Mouse			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusPS2Mouse : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_PS2MOUSE_STATES		1
	#else
		#define MAX_PS2MOUSE_STATES		2
	#endif

	const static STRUCT_UNITSTATUS_STATES PS2MouseStates[MAX_PS2MOUSE_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);

public:
	CFSMUnitStatusPS2Mouse(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusPS2Mouse();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusAudioHC4			 // 
//-----------------------------------------------------------//
class CFSMUnitStatusAudioHC4 : public CFSMUnitStatus
{
protected:

	#ifdef BINDING_SOFTWARE
		#define MAX_AUDIO_HC4_STATES		2	// CHANGES_REQUEST_UNITSTATUS
	#else
		#define MAX_AUDIO_HC4_STATES		7
	#endif

	const static STRUCT_UNITSTATUS_STATES AudioHC4States[MAX_AUDIO_HC4_STATES + 1];

	void RequestConfig(void);
	void RequestState(void);
	void RequestDestState(void);
	//void RequestVolState(void);
	void RequestVol_Dest0(void);
	void RequestVol_Dest1(void);
	void RequestVol_Dest2(void);
	void RequestVol_Dest3(void);

public:
	CFSMUnitStatusAudioHC4(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusAudioHC4();

	virtual void EvOperationStart(void);
};

//-----------------------------------------------------------//
//			Derived Class : CFSMUnitStatusEnergyMeasure		 // 
//-----------------------------------------------------------//

// IMPLEMENT_UNIT_ENERGY
class CFSMUnitStatusEnergyMeasure : public CFSMUnitStatus
{
protected:
	#define MAX_ENERGYMEASURE_STATES		1
	
	const static STRUCT_UNITSTATUS_STATES States[MAX_ENERGYMEASURE_STATES+1];

	void RequestConfig(void);
	void RequestState(void);	
	void RequestSupported(void);

public:
	CFSMUnitStatusEnergyMeasure(IRequestForNodeStatus *pParent,BYTE bNode,BYTE bUnit);
	virtual ~CFSMUnitStatusEnergyMeasure();

	virtual void EvOperationStart(void);
};



#endif // !defined(AFX_FSMUNITSTATUS_H__2FA4BB3D_AD7F_4560_8234_35E6EFC5C0DD__INCLUDED_)
