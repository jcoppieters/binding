#include "stdafx.h"
#include "appdalifsm_helper.h"
#include "appdalitransmitter.h"
#include "appdaliprotocol_def.h"
#include "appdalidevices.h"				
#include "appdali_common.h"		
#include "appdali_def.h"
/***********************************************************************************/
#define DEBUG_LOCAL_ON		1
/***********************************************************************************/
#if (DEBUG_LOCAL_ON==1)
	#define TRACER(x)						TRACE(x)
#else
	#define TRACER(x ...)
#endif
/***********************************************************************************/

/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelper_Base::CAppDaliFsmHelper_Base(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	m_refParent(refParent), m_refTransmitter(refTransmitter), m_state(FSM_BASE_STATE_READY), m_shortAddress(-1), m_daliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR), 
	m_counterValue(0), m_fatalErrorInfo() { }

CAppDaliFsmHelper_Base::~CAppDaliFsmHelper_Base( ) { }

bool CAppDaliFsmHelper_Base::cancelTask( ) {
	this->getTransmitter().stopQuienscentMode();
	return true;
}

bool CAppDaliFsmHelper_Base::onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) {
	CString stringEmpty;
	this->setFatalErrorInfo(ERROR_CODE_TIMER_EXPIRY,-1,stringEmpty);
	this->m_state = FSM_BASE_STATE_ERROR;		// errorCode=ERROR_CODE_TIMER_EXPIRY
	return true;
}

bool CAppDaliFsmHelper_Base::isIdle() {
	const int state = this->getState();
	return ((FSM_BASE_STATE_READY == state) || (FSM_BASE_STATE_DONE == state) ||
		    (FSM_BASE_STATE_CANCEL == state) || (FSM_BASE_STATE_ERROR == state));
}

bool CAppDaliFsmHelper_Base::isDone() {
	return (FSM_BASE_STATE_DONE == this->getState());
}
bool CAppDaliFsmHelper_Base::isError() {
	return (FSM_BASE_STATE_ERROR == this->getState());
}
bool CAppDaliFsmHelper_Base::isCancelled() {
	return (FSM_BASE_STATE_CANCEL == this->getState());
}

void CAppDaliFsmHelper_Base::setFatalErrorInfo(DaliErrorCode_t errorCode, int shortAddress,const CString& strError) {
	this->m_fatalErrorInfo.set(errorCode, shortAddress, strError);
}
void CAppDaliFsmHelper_Base::clearFatalErrorInfo() {
	this->m_fatalErrorInfo.clear();
}

void CAppDaliFsmHelper_Base::setErrorEntry(CAppDaliFsmContext& refContext, const CAppDaliFsmErrorEntry& refErrorEntry) {
	refContext.addError(refErrorEntry);
}

// Algemene counter waarde voor de algoritmes.
int CAppDaliFsmHelper_Base::getCounterValue() const {
	return this->m_counterValue;
}
void CAppDaliFsmHelper_Base::clearCounterValue() {
	this->m_counterValue = 0;
}
void CAppDaliFsmHelper_Base::incrementCounterValue( ) {
	++this->m_counterValue;
}
void CAppDaliFsmHelper_Base::decrementCounterValue( ) {
	--this->m_counterValue;
}
/***********************************************************************************/
// Transmit functies ifv het type DALI device.
bool CAppDaliFsmHelper_Base::sendBroadcastInitialiseAll(DaliDeviceType_t daliDeviceType,int address) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(address);			// 0xff = search all unaddressed.
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastinitialiseAll(address);	// TODO_DALI_FSM 0xff of 0x7f.
	}
	else { ASSERT(0); }
	return result;
}

bool CAppDaliFsmHelper_Base::sendBroadcastRandomize(DaliDeviceType_t daliDeviceType) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastRandomize( );	
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastRandomize( );
	}
	else { ASSERT(0); }
	return result;
}

bool CAppDaliFsmHelper_Base::sendQueryDeviceStatus(DaliDeviceType_t daliDeviceType, int address) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendQueryControlGearDeviceStatus(address);
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendQueryInputControlDeviceStatus(address);
	}
	else { ASSERT(0); };
	return result;
}

bool CAppDaliFsmHelper_Base::sendBroadcastSearchAddrL(DaliDeviceType_t daliDeviceType, int data) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastSearchAddrL(data);
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastSearchAddrL(data);
	}
	else { ASSERT(0); };
	return result;
}

bool CAppDaliFsmHelper_Base::sendBroadcastSearchAddrM(DaliDeviceType_t daliDeviceType,int data) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastSearchAddrM(data);
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastSearchAddrM(data);
	}
	else { ASSERT(0); };
	return result;
}

bool CAppDaliFsmHelper_Base::sendBroadcastSearchAddrH(DaliDeviceType_t daliDeviceType,int data) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastSearchAddrH(data);
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastSearchAddrH(data);
	}
	else { ASSERT(0); };
	return result;
}


bool CAppDaliFsmHelper_Base::sendBroadcastTerminate(DaliDeviceType_t daliDeviceType) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastTerminate();
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastTerminate();
	}
	else { ASSERT(0); };
	return result;
}

bool CAppDaliFsmHelper_Base::sendBroadcastCompare(DaliDeviceType_t daliDeviceType) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastCompare( );
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastCompare();
	}
	else { ASSERT(0); }
	return result;
}

bool CAppDaliFsmHelper_Base::sendProgramShortAddress(DaliDeviceType_t daliDeviceType,int address) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearProgramShortAddress(address);
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesProgramShortAddress(address);
	}
	else { ASSERT(0); };
	return result;
}

bool CAppDaliFsmHelper_Base::sendQueryShortAddress(DaliDeviceType_t daliDeviceType) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendQueryControlGearShortAddress();
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendQueryControlInputDevicesShortAddress();
	}
	else { ASSERT(0); };
	return result;
}

bool CAppDaliFsmHelper_Base::sendBroadcastWithdraw(DaliDeviceType_t daliDeviceType) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		result = this->getTransmitter().sendControlgearBroadcastWithdraw();
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		result = this->getTransmitter().sendControlInputDevicesBroadcastWithdraw();
	}
	else { ASSERT(0); };
	return result;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperSearchDevices::CAppDaliFsmHelperSearchDevices(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter) { }

CAppDaliFsmHelperSearchDevices::~CAppDaliFsmHelperSearchDevices() {}

bool CAppDaliFsmHelperSearchDevices::start() {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	this->clearFatalErrorInfo();
	this->setState(STATE_READY);
	return this->next();
}

bool CAppDaliFsmHelperSearchDevices::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperSearchDevices::next() {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {			
			result = this->getTransmitter().startQuiescentMode();
			if (result) {
				this->setState(STATE_QUIESCENT_MODE);
			}
		} break;
		case STATE_QUIESCENT_MODE: {
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(0x00);
			if (result) {
				this->setState(STATE_INITIALISE);
			}
		} break;
		case STATE_INITIALISE: {
			ASSERT(0);
			result = false;
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperSearchDevices::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_QUIESCENT_MODE: {
			// absorbe.
		} break;
		case STATE_INITIALISE: {
			// absorbe.
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
		default: {
			ASSERT(0);	// todo afhandelen als een fout...
			result = false;
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperSearchDevices::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_QUIESCENT_MODE: {
			result = true;
		} break;
		case STATE_INITIALISE: {
			result = true;
			this->setState(STATE_DONE);
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
		default: {
			ASSERT(0);	// todo afhandelen als een fout...
			result = false;
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperSearchDevices::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	CString stringEmpty;
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,-1,stringEmpty);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperSearchDevices::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	CString stringEmpty;
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,-1,stringEmpty);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperSearchControlGearDevice::CAppDaliFsmHelperSearchControlGearDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_foundDevices(false), m_modeSearchBroadcast(false), m_retryCounter(0) { }

CAppDaliFsmHelperSearchControlGearDevice::~CAppDaliFsmHelperSearchControlGearDevice( ) { } 

bool CAppDaliFsmHelperSearchControlGearDevice::start(int shortAddress) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// basis
	this->clearFatalErrorInfo();
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);

	// eigen properties/
	this->m_foundDevices = false;
	this->m_modeSearchBroadcast = false;
	this->m_retryCounter = 0;
	return this->next();
}

bool CAppDaliFsmHelperSearchControlGearDevice::start() {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	// basis
	this->setState(STATE_READY);
	this->setShortAddress(-1);
	this->clearFatalErrorInfo();
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);

	// eigen properties/
	this->m_foundDevices = false;	
	this->m_modeSearchBroadcast = true;
	this->m_retryCounter = 0;
	return this->next();
}

bool CAppDaliFsmHelperSearchControlGearDevice::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperSearchControlGearDevice::next( ) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			if (this->m_modeSearchBroadcast) {
					this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
					result = this->getTransmitter().sendQueryControlGearBroadcastPresent();
					if (result) {
						this->setState(STATE_QUERY_DEVICES_BROADCAST_PRESENT);
					}
			}
			else {
				this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
				result = this->getTransmitter().sendQueryControlGearPresent(this->getShortAddress());
				if (result) {
					this->setState(STATE_QUERY_DEVICE_PRESENT);
				}
			}
		} break;
		case STATE_QUERY_DEVICES_BROADCAST_PRESENT: {
			// Door de retries kunnenn we opnieuw in deze toestand komen.
			ASSERT(TRUE == this->m_modeSearchBroadcast);
			ASSERT(FALSE == this->m_foundDevices);
			result = this->getTransmitter().sendQueryControlGearBroadcastPresent();
			ASSERT(TRUE == result);
		} break;

		case STATE_QUERY_DEVICES_DEVICE_PRESENT: {
			ASSERT(this->getShortAddress() >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST && this->getShortAddress() <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST);
			result = this->getTransmitter().sendQueryControlGearPresent(this->getShortAddress());
			if (result) {				
				CString msg;
				msg.Format("Search for control gear device A%d", this->getShortAddress());
				this->getParent().onNotifyFsmAlgoritmeMsgInfo(msg);
				// Blijven in deze toestand.
			}			
		} break;

		case STATE_QUERY_DEVICE_PRESENT: {
			ASSERT(FALSE == this->m_modeSearchBroadcast);
			result = this->getTransmitter().sendQueryControlGearDeviceStatus(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_STATUS);
			}			  
		} break;
		case STATE_QUERY_DEVICE_STATUS: {
			ASSERT(0);
			result=false;
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

bool CAppDaliFsmHelperSearchControlGearDevice::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_QUERY_DEVICES_BROADCAST_PRESENT: {
			this->m_foundDevices = true;		// Device(s) hebben geantwoord op een broadcast.
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
			this->setState(STATE_DONE);
		} break;
		case STATE_QUERY_DEVICES_DEVICE_PRESENT: {
			ASSERT(this->getShortAddress() >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST && this->getShortAddress() <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST);
			this->m_foundDevices = true;		// Individueel device heeft geantwoord op de query.
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
			this->setState(STATE_DONE);
		} break;
		case STATE_QUERY_DEVICE_PRESENT: {
			// Ok Absorbe : device heeft geantwoord.
			// Nog wachten op een antwoord op de status.
		} break;
		case STATE_QUERY_DEVICE_STATUS: {
			const int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);
			this->m_foundDevices = true;		
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
			this->setState(STATE_DONE);
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
		default: {
			ASSERT(0);	// todo afhandelen als een fout...
			result = false; 
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperSearchControlGearDevice::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_QUERY_DEVICES_BROADCAST_PRESENT: {
			enum { MAX_RETRIES = 3};
			this->m_foundDevices = false;
			if (++this->m_retryCounter >= MAX_RETRIES) {
				// Sedert V17.04: Niet alle control gear devices reageren op de broadcast query.
				// Wanneer er nog geen control devices gevonden zijn dan naar de toestand 'STATE_QUERY_DEVICES_DEVICE_PRESENT' gaan.
				// In deze toestand wordt er een query verstuurd naar de afzonderlijke devices.
				this->setShortAddress(DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST);
				this->setState(STATE_QUERY_DEVICES_DEVICE_PRESENT);
			}
			result = true;
		} break;

		case STATE_QUERY_DEVICES_DEVICE_PRESENT: {
			ASSERT(this->m_foundDevices == false);
			int shortAddress = this->getShortAddress();
			if (++shortAddress > DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
				this->setShortAddress(-1);
				this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
				this->m_foundDevices = false;
				this->setState(STATE_DONE);
			}
			else {
				// In deze toestand blijven...
				ASSERT((shortAddress>= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (shortAddress<= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST));
				this->setShortAddress(shortAddress);
			}
			result = true;
		} break;

		case STATE_QUERY_DEVICE_PRESENT: {
			this->m_foundDevices = false;
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
			this->setState(STATE_DONE);
			result = true;
		} break;
		
		// Komen enkel in onderstaande toestand wanneer een device eerder geantwoord heeft op de device present query.
		// Gaan via een retry mechanisme nog eens opnieuw opvragen.
		// Indien we geen  antwoord krijgen na deze retries dan stoppen we het algoritme.
		case STATE_QUERY_DEVICE_STATUS: {
			enum { MAX_RETRIES = 3};		
			if (++this->m_retryCounter >= MAX_RETRIES) {
				CString strErrorDetail;
				strErrorDetail.Format("No response from short address %d, previously this device answered to a control gear present query.", this->getShortAddress());
				const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress(), strErrorDetail);
				this->setErrorEntry(refContext,errorEntry);
				this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, ERROR_CODE_PROPERTY_NORESPONSE, -1);
				this->m_foundDevices = false;
				this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
				this->setState(STATE_DONE);
				result = true;
			}
			else {
				this->m_foundDevices = false;
				this->setState(STATE_READY);
				result=true;
			}
		} break;

		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;

		default: {
			ASSERT(0); 
		} break;
	}
	return result;
}	

bool CAppDaliFsmHelperSearchControlGearDevice::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// Sedert V16.40
// Wanneer er meerdere devices aanwezig zijn en gelijktijdig antwoorden op de broadcast request.
bool CAppDaliFsmHelperSearchControlGearDevice::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_QUERY_DEVICES_BROADCAST_PRESENT == state) {
		this->m_foundDevices = true;
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
		this->setState(STATE_DONE);
		return true;
	}

	// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES.
	// Dit komt soms voor wanneer er 2 control gear devices een zelfde short address hebben.
	if ((STATE_QUERY_DEVICE_STATUS == state) || (STATE_QUERY_DEVICE_PRESENT == state) || (STATE_QUERY_DEVICES_DEVICE_PRESENT == state)) {
		const int shortAddress = this->getShortAddress();
		ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);

		CString strErrorDetail;
		strErrorDetail.Format("Device A%d: Received error when quering the device status. Possible double address.",shortAddress);
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), shortAddress, strErrorDetail);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);

		// Oppassen: Moeten de het algoritme ook stoppen 
		this->m_foundDevices = true;
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
		this->setState(STATE_DONE);

		return true;	// Continue algoritme + fsm.
	}
	ASSERT(0);	// afhandelen naar error toestand gaan...
	return false;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperSearchControlInputDevice::CAppDaliFsmHelperSearchControlInputDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_mode(MODE_BROADCAST_ALL), m_CounterfoundDevices(0) ,m_busySearchAllDevices(false) {
	for (int i = 0; i < DALI_SHORTADDRESS_MAX; ++i) {
		this->m_listFoundDevices[i] = false;
	}
}
CAppDaliFsmHelperSearchControlInputDevice::~CAppDaliFsmHelperSearchControlInputDevice() { }

bool CAppDaliFsmHelperSearchControlInputDevice::startBroadcast() {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	// basis
	this->setState(STATE_READY);
	this->clearFatalErrorInfo();
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->setShortAddress(-1);

	// eigen properties
	this->m_mode = MODE_BROADCAST_ALL;
	this->m_CounterfoundDevices = 0;
	for (int i = 0; i < DALI_SHORTADDRESS_MAX; ++i) {
		this->m_listFoundDevices[i] = false;
	}	
	this->m_busySearchAllDevices = false;
	return this->next();
}

bool CAppDaliFsmHelperSearchControlInputDevice::start(int shortAddress) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// basis
	this->setState(STATE_READY);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->setShortAddress(shortAddress);

	// eigen properties
	this->m_mode = MODE_REQUEST_SINGLE_DEVICE;
	this->m_CounterfoundDevices = 0;
	for (int i = 0; i < DALI_SHORTADDRESS_MAX; ++i) {
		this->m_listFoundDevices[i] = false;
	}
	this->m_busySearchAllDevices = false;
	return this->next();
}

bool CAppDaliFsmHelperSearchControlInputDevice::startIterate() {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	// basis
	this->setState(STATE_READY);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->setShortAddress(-1);

	// eigen properties/
	this->m_mode = MODE_ITERATE_ALL_DEVICES;
	this->m_CounterfoundDevices = 0;
	for (int i = 0; i < DALI_SHORTADDRESS_MAX; ++i) {
		this->m_listFoundDevices[i] = false;
	}
	this->m_busySearchAllDevices = false;
	return this->next();
}

bool CAppDaliFsmHelperSearchControlInputDevice::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperSearchControlInputDevice::next() {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			int shortAddress = -1;
			if (MODE_BROADCAST_ALL == this->m_mode || MODE_ITERATE_ALL_DEVICES == this->m_mode) {
				result = this->getTransmitter().sendControlInputDevicesBroadcastinitialiseAll(0xff);
				this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
				if (result) {
					this->setState(STATE_INITIALSE);
				}
			}
			else if (MODE_REQUEST_SINGLE_DEVICE == this->m_mode) {
				result = this->getTransmitter().sendQueryInputControlDeviceStatus(this->getShortAddress());
				this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
				if (result) {
					this->setState(STATE_QUERY_DEVICE_STATUS);
				}
			}							
		} break;

		case STATE_INITIALSE: {
			if (MODE_BROADCAST_ALL == this->m_mode) {
				result = this->getTransmitter().sendQueryBroadcastInputControlDeviceStatus( );
			}
			else if (MODE_REQUEST_SINGLE_DEVICE == this->m_mode || MODE_ITERATE_ALL_DEVICES == this->m_mode) {
				result = this->getTransmitter().sendQueryInputControlDeviceStatus(this->getShortAddress());
			}
			if (result) {
				this->setState(STATE_QUERY_DEVICE_STATUS);
			}
		} break;

		case STATE_QUERY_DEVICE_STATUS: {
			const int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);
			result = this->getTransmitter().sendQueryInputControlDeviceStatus(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_STATUS);		// hier blijven.
			}
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperSearchControlInputDevice::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	const int state = this->getState();
	if (STATE_QUERY_DEVICE_STATUS == state) {
		if (MODE_BROADCAST_ALL == this->m_mode) {
			this->m_CounterfoundDevices = 1;
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
			this->setState(STATE_DONE);
			return true;
		}
		else if (MODE_REQUEST_SINGLE_DEVICE == this->m_mode) {
			const int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);
			this->m_CounterfoundDevices++;
			this->m_listFoundDevices[shortAddress] = true;
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
			this->setState(STATE_DONE);
			return true;
		}
		else if (MODE_ITERATE_ALL_DEVICES == this->m_mode) {
			int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);
			this->m_CounterfoundDevices++;
			this->m_listFoundDevices[shortAddress] = true;
			if (++shortAddress >= DALI_SHORTADDRESS_LAST) {
				this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
				this->setState(STATE_DONE);
				return true;
			}
			this->setShortAddress(shortAddress);
			return true;
		}
		else { ASSERT(0); };
	}

	// Fatal error: Verwachten enkel in de toestand STATE_QUERY_DEVICE_STATUS een Backward no reponse.
	CString stringErrorDetail("Recieved backward frame response from DALI controller but expected other response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperSearchControlInputDevice::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_QUERY_DEVICE_STATUS == state) {	
		if (MODE_BROADCAST_ALL == this->m_mode) {
			if (!m_busySearchAllDevices) {
				// Broadcast verstuurd. Geen devices gevonden. Nog eens alle devices afzonderlijk proberen te vinden.
				this->setShortAddress(DALI_SHORTADDRESS_FIRST);
				m_busySearchAllDevices = true;
			}
			else {
				int shortAddress = this->getShortAddress();
				ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);
				if (++shortAddress >= DALI_SHORTADDRESS_LAST) {
					// Laatste adres bereikt. Geen input control devices gevonden.
					this->m_CounterfoundDevices = 0;
					this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
					this->setState(STATE_DONE);
					return true;
				}
				this->setShortAddress(shortAddress);
			}
			return true;
		}
		else if (MODE_REQUEST_SINGLE_DEVICE == this->m_mode) {
			this->m_CounterfoundDevices = 0;
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
			this->setState(STATE_DONE);
			return true;
		}
		else if (MODE_ITERATE_ALL_DEVICES == this->m_mode) {
			int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);
			if (++shortAddress >= DALI_SHORTADDRESS_LAST) {
				this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
				this->setState(STATE_DONE);
				return true;
			}
			this->setShortAddress(shortAddress);
			return true;
		}		
	}

	// Fatal error: Verwachten enkel in de toestand STATE_QUERY_DEVICE_STATUS een Backward no reponse.
	CString stringEmpty;
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringEmpty);
	this->setState(STATE_ERROR_FATAL);			// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Soms een probleem wanneer 1 of meerdere DALI input devices een dubbel adres hebben.
// Probleem wordt hier niet altijd gedetecteerd...
bool CAppDaliFsmHelperSearchControlInputDevice::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_QUERY_DEVICE_STATUS == state) {	
		if (MODE_BROADCAST_ALL == this->m_mode) {
			this->m_CounterfoundDevices = 1;
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICES, -1, -1);
			this->setState(STATE_DONE);
			return true;
		}
		else if (MODE_REQUEST_SINGLE_DEVICE == this->m_mode) {
			// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
			const int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);
	
			CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), shortAddress);
			this->setErrorEntry(refContext, errorEntry);
			this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);

			//this->m_CounterfoundDevices++;						// OK ??????????????
			this->m_listFoundDevices[shortAddress] = false;			// OK ??????????????
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
			this->setState(STATE_DONE);
			return true;
		}
		else if (MODE_ITERATE_ALL_DEVICES == this->m_mode) {

			int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress <= DALI_SHORTADDRESS_LAST);

			// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
			CString strErrorDetail;
			strErrorDetail.Format("Frame error received for short address %d", shortAddress);
			CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(),shortAddress, strErrorDetail);
			this->setErrorEntry(refContext, errorEntry);
			this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
			
			if (++shortAddress >= DALI_SHORTADDRESS_LAST) {
				this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SEARCH_DEVICE, -1, -1);
				this->setState(STATE_DONE);
				return true;
			}
			this->setShortAddress(shortAddress);
			return true;
		}
		else { ASSERT(0); }
	}

	// Fatal error: Verwachten enkel in de toestand STATE_QUERY_DEVICE_STATUS een Backward no reponse.
	CString stringErrorDetail("Recieved error frame response from DALI controller but expected other response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperSearchControlInputDevice::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	if (STATE_INITIALSE == this->getState()) {
		if (MODE_ITERATE_ALL_DEVICES == this->m_mode) {
			this->setShortAddress(0);
		}
		return true;
	}

	// Fatal error.
	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperSearchControlInputDevice::foundDevice(int shortAddress) const {
	if ((shortAddress < DALI_SHORTADDRESS_FIRST) || (shortAddress > DALI_SHORTADDRESS_LAST))
		return false;
	return this->m_listFoundDevices[shortAddress];
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperRequestPropertiesControlGear::CAppDaliFsmHelperRequestPropertiesControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_workingFlags(0), m_requestNextDeviceType(false), m_deviceType(-1), m_deviceVersion(-1), m_deviceStatus(-1),
	m_deviceOperatingMode(-1), m_deviceActualLevel(-1), m_deviceMinLevel(-1), m_deviceMaxLevel(-1), m_deviceFadeTimeRate(-1),
	m_devicePhysicalMinimum(-1),m_devicePoweronLevel(-1), m_deviceFailureLevel(-1), m_deviceExtFadeTime(-1), m_dimmingCurve(-1),
	m_colorFeatures(-1), m_deviceTypes() { }

CAppDaliFsmHelperRequestPropertiesControlGear::~CAppDaliFsmHelperRequestPropertiesControlGear( ) { } 

bool CAppDaliFsmHelperRequestPropertiesControlGear::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperRequestPropertiesControlGear::start(int shortAddress, bool busyCommisioning) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// basis
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->clearFatalErrorInfo();

	// eigen properties/
	if (busyCommisioning) {
		this->m_workingFlags = 0;
	}	
	else {
		this->m_workingFlags = (FLAG_CONFIG_MODE_ENTER + FLAG_CONFIG_MODE_EXIT + FLAG_USE_STANDALONE);
	}

	this->m_requestNextDeviceType = false;
	this->m_deviceType=-1;
	this->m_deviceTypes.RemoveAll();
	this->m_deviceVersion=-1;
	this->m_deviceStatus=-1;
	this->m_deviceOperatingMode=-1;
	this->m_deviceActualLevel=-1;
	this->m_deviceMinLevel=-1;
	this->m_deviceMaxLevel=-1;
	this->m_deviceFadeTimeRate=-1;
	this->m_devicePhysicalMinimum=-1;
	this->m_devicePoweronLevel = -1;
	this->m_deviceFailureLevel = -1;
	this->m_deviceExtFadeTime = -1;
	this->m_dimmingCurve = -1;
	this->m_colorFeatures = -1;

	return this->next();
}

bool CAppDaliFsmHelperRequestPropertiesControlGear::next( ) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
			if (this->useAlgEnterConfigMode()) {
				static const int dataByte = 0x00;	// Initialise (all)
				result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(dataByte);
				ASSERT(result);
				if (result) {					
					this->setState(STATE_CFG_MODE_INITIALISE);
				}
			}
			else {
				result = this->getTransmitter().sendQueryControlGearDeviceType(this->getShortAddress());
				if (result) {
					this->setState(STATE_QUERY_DEVICE_TYPE);
				}
			}
		} break;

		case STATE_CFG_MODE_INITIALISE: {
			ASSERT(this->useAlgEnterConfigMode());
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {			
				this->setState(STATE_CFG_MODE_ENTER_QUIESCENT_MODE);
			}
		} break;
				
		case STATE_CFG_MODE_ENTER_QUIESCENT_MODE: {
			ASSERT(this->useAlgEnterConfigMode());
			result = this->getTransmitter().sendQueryControlGearDeviceType(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_TYPE);
			}		
		} break;

		case STATE_QUERY_DEVICE_TYPE: {		
			if (this->m_deviceType == 255) {
				// Sedert V16.48
				// Indien we mask(0xff) ontvangen dan zijn er meerder types aanwezig in het DALi device.
				// Moeten deze opvragen via 'DALI_CMD_QUERY_NEXT_DEVICE_TYPE'.
				result = this->getTransmitter().sendQueryControlGearNextDeviceType(this->getShortAddress());
				if (result) {
					this->setState(STATE_QUERY_NEXT_DEVICE_TYPE);
				}				
			}
			else if (this->m_deviceType == 6) {			
				// Bij device type 6 gaan we extra de dimming curve gaan opvragen.
				// Oppassen:
				//	- Bij andere devices types is dit ook mogelijk maar de waarden zijn anders.
				//	  bvb: Bij een 0-10V (dt5) is de curve standaard lineair en is dit waarde 0
				result = this->getTransmitter().sendControlGearEnableDeviceType(this->m_deviceType);
				if (result) {
					this->setState(STATE_ENABLE_DEVICETYPE_DT6_DIMMINGCURVE);
				}
			}
			else if (this->m_deviceType == 8) {	
				result = this->getTransmitter().sendControlGearEnableDeviceType(this->m_deviceType);
				if (result) {
					this->setState(STATE_ENABLE_DEVICETYPE_DT8_COLORFEATURES);
				}	
			}
			else {
				result = this->getTransmitter().sendQueryControlGearVersionNumber(this->getShortAddress());
				if (result) {
					this->setState(STATE_QUERY_DEVICE_VERSION);
				}
			}
		} break;		

		case STATE_QUERY_NEXT_DEVICE_TYPE: {			
			if (this->m_requestNextDeviceType) {
				result = this->getTransmitter().sendQueryControlGearNextDeviceType(this->getShortAddress());
				if (result) {
					this->setState(STATE_QUERY_NEXT_DEVICE_TYPE);
				}					
			}
			else {
				// We gaan eerst controleren of er DT6 opbjecten aanwezig zijn.
				// Bij de aanwezigheid van dt6 objecten gaan we de properties opvragen.
				const POSITION pos = this->m_deviceTypes.Find(6);
				if (NULL !=pos) {
					// Controle in de lijst.
					// Indien er een dt6 object aanwezig is in de lijst moeten we dimmer curve opvragen.
					result = this->getTransmitter().sendControlGearEnableDeviceType(6);
					if (result) {
						this->setState(STATE_ENABLE_DEVICETYPE_DT6_DIMMINGCURVE);
					}
				}
				else {				
					const POSITION pos = this->m_deviceTypes.Find(8);
					if (NULL !=pos) {
						result = this->getTransmitter().sendControlGearEnableDeviceType(8);
						if (result) {
							this->setState(STATE_ENABLE_DEVICETYPE_DT8_COLORFEATURES);
						}
					}
					else { 
						result = this->getTransmitter().sendQueryControlGearVersionNumber(this->getShortAddress());
						if (result) {
							this->setState(STATE_QUERY_DEVICE_VERSION);
						}				
					}
				}
			}				
		} break;

		case STATE_ENABLE_DEVICETYPE_DT6_DIMMINGCURVE: {					
			result = this->getTransmitter().sendQueryControlGearDT6DimmerCurve(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICETYPE_DT6_DIMMING_CURVE);
			}
		} break;

		// Indien een DALI device meerdere device types ondersteund dan gaan we altijd eerst de DT6
		// properties opvragen en daarna de dt8 properties.
		case STATE_QUERY_DEVICETYPE_DT6_DIMMING_CURVE: {			
			const POSITION pos = this->m_deviceTypes.Find(8);
			if (NULL !=pos) {
				result = this->getTransmitter().sendControlGearEnableDeviceType(8);
				if (result) {
					this->setState(STATE_ENABLE_DEVICETYPE_DT8_COLORFEATURES);
				}
			}
			else {
				result = this->getTransmitter().sendQueryControlGearVersionNumber(this->getShortAddress());
				if (result) {
					this->setState(STATE_QUERY_DEVICE_VERSION);
				}
			}
		} break;

		case STATE_ENABLE_DEVICETYPE_DT8_COLORFEATURES: {				
			result = this->getTransmitter().sendQueryControlGearDT8ColorTypeFeatures(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICETYPE_DT8_COLORFEATURES);
			}														
		} break;

		case STATE_QUERY_DEVICETYPE_DT8_COLORFEATURES: {
			// Verder doen met het opvragen van de algemene device properties.
			result = this->getTransmitter().sendQueryControlGearVersionNumber(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_VERSION);
			}								   													   
		} break;

		case STATE_QUERY_DEVICE_VERSION: {
			result = this->getTransmitter().sendQueryControlGearDeviceStatus(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_STATUS);
			}
		} break;

		// sendQueryControlGearOperatingMode( )
		// Oppassen: 'IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
		case STATE_QUERY_DEVICE_STATUS: {
			result = this->getTransmitter().sendQueryControlGearOperatingMode(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_OPERATING_MODE);
			}
		} break;
		case STATE_QUERY_DEVICE_OPERATING_MODE: {
			result = this->getTransmitter().sendQueryControlGearActualLevel(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_ACTUAL_LEVEL);
			}
		} break;
		case STATE_QUERY_DEVICE_ACTUAL_LEVEL: {
			result = this->getTransmitter().sendQueryControlGearMinLevel(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_MIN_LEVEL);
			}
		} break;
		case STATE_QUERY_DEVICE_MIN_LEVEL:{
			result = this->getTransmitter().sendQueryControlGearMaxLevel(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_MAX_LEVEL);
			}
		} break;
		case STATE_QUERY_DEVICE_MAX_LEVEL:{
			result = this->getTransmitter().sendQueryControlGearFadeTimeRate(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_FADE_TIME_RATE);
			}
		} break;
		case STATE_QUERY_DEVICE_FADE_TIME_RATE: {
			result = this->getTransmitter().sendQueryControlGearExtendedFadeTime(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_EXTENDED_FADETIME);
			}						
		} break;
		case STATE_QUERY_DEVICE_EXTENDED_FADETIME: {
			result = this->getTransmitter().sendQueryControlGearPhysicalMinValue(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_PHYSICAL_MINIMUM);
			}						   
		} break;
		case STATE_QUERY_DEVICE_PHYSICAL_MINIMUM: {
			result = this->getTransmitter().sendQueryControlGearPowerOnLevel(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_POWERON_LEVEL);
			}
		} break;
		case STATE_QUERY_DEVICE_POWERON_LEVEL:{
			result = this->getTransmitter().sendQueryControlGearSystemFailureLevel(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_DEVICE_FAILURE_LEVEL);
			}								   
		} break;
		case STATE_QUERY_DEVICE_FAILURE_LEVEL: {
			if (this->useAlgExitConfigMode()) {
				result = this->getTransmitter().sendControlgearBroadcastTerminate();
				if (result) {
					this->setState(STATE_CFG_MODE_TERMINATE);
				}			
			}
			else {
				ASSERT(0);
				result = false;
			}
		} break;
		case STATE_CFG_MODE_TERMINATE: {
			ASSERT(this->useAlgExitConfigMode());
			result = this->getTransmitter().stopQuienscentMode();
			if (result) {			
				this->setState(STATE_CFG_MODE_EXIT_QUIESCENT_MODE);
			}
		} break;
		case STATE_CFG_MODE_EXIT_QUIESCENT_MODE: {
			ASSERT(0);
			result = false;
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Zelfs wanneer we dubbele addressen hebben krijgen we soms nog correcte antwoorden.
// Dit is afhankelijk van de control gear devices.
// 
// Bij de LCM reeks krijgen we enkel een fout op de Query 'DALI_CMD_QUERY_EXTENDED_FADE_TIME'.
//
// Sedert V16.48: Ondersteuning meerdere device types. (zie STATE_QUERY_NEXT_DEVICE_TYPE).
bool CAppDaliFsmHelperRequestPropertiesControlGear::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	bool result = true;	
	const int state = this->getState();
	switch (state) {
		case STATE_QUERY_DEVICE_TYPE: {
			this->m_deviceType = data;
		} break;

		case STATE_QUERY_NEXT_DEVICE_TYPE: {
			if ((data == 0xFF) || (data == 0xFE)) {
				this->m_requestNextDeviceType = false;
			}
			else {
				this->m_requestNextDeviceType = true;
				this->m_deviceTypes.AddTail(data);
			}		
		} break;	

		case STATE_QUERY_DEVICETYPE_DT6_DIMMING_CURVE: {
			if (data == DALI_CONTROLGEAR_DIMCURVE_DALI || data == DALI_CONTROLGEAR_DIMCURVE_LINEAIR) {
				this->m_dimmingCurve = data;
			}
			else {
				ASSERT(0);
				this->m_dimmingCurve = DALI_CONTROLGEAR_DIMCURVE_UNSUPPORTED;
			}
		} break;
		case STATE_QUERY_DEVICETYPE_DT8_COLORFEATURES: {
			this->m_colorFeatures = data;									   													   
		} break;
		case STATE_QUERY_DEVICE_VERSION: {
			this->m_deviceVersion = data;				  
		} break;
		case STATE_QUERY_DEVICE_STATUS: {
			this->m_deviceStatus = data;			
		} break;
		// Oppassen: 'IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
		case STATE_QUERY_DEVICE_OPERATING_MODE:{
			this->m_deviceOperatingMode = data;
		} break;
		case STATE_QUERY_DEVICE_ACTUAL_LEVEL: {
			this->m_deviceActualLevel = data;
		} break;
		case STATE_QUERY_DEVICE_MIN_LEVEL :{
			this->m_deviceMinLevel = data;
		} break;
		case STATE_QUERY_DEVICE_MAX_LEVEL :{
			this->m_deviceMaxLevel = data;
		} break;
		case STATE_QUERY_DEVICE_FADE_TIME_RATE: {
			this->m_deviceFadeTimeRate = data;
		} break;
		case STATE_QUERY_DEVICE_EXTENDED_FADETIME: {
			this->m_deviceExtFadeTime = data;									   
		} break;
		case STATE_QUERY_DEVICE_PHYSICAL_MINIMUM: {
			this->m_devicePhysicalMinimum = data;			
		} break;
		case STATE_QUERY_DEVICE_POWERON_LEVEL: {
			this->m_devicePoweronLevel = data;							   
		} break;
		case STATE_QUERY_DEVICE_FAILURE_LEVEL:{
			this->m_deviceFailureLevel = data;		
			if (!this->useAlgExitConfigMode()) {
				this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
				this->setState(STATE_DONE);			
			}
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
		default: {
			ASSERT(0);	// todo afhandelen als een fout...
			result = false; 
		} break;
	}
	return result;
}

// Oppassen: 'IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
bool CAppDaliFsmHelperRequestPropertiesControlGear::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if ((STATE_QUERY_DEVICE_OPERATING_MODE == state) || (STATE_QUERY_DEVICE_EXTENDED_FADETIME == state)) {
		// IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
		// Control gear die dit bericht niet herkend zal niet antwoorden.
		return true;
	}
	if (STATE_QUERY_DEVICETYPE_DT6_DIMMING_CURVE == state) {
		// Wanneer de Dimming curve niet ondersteund wordt krijgen we geen antwoord.
		this->m_dimmingCurve = DALI_CONTROLGEAR_DIMCURVE_UNSUPPORTED;
		return true;
	}
	if (STATE_QUERY_NEXT_DEVICE_TYPE == state) {
		// Oppassen: Niet altijd aanwezig. 
		this->m_requestNextDeviceType = false;
		return true;
	}
	if (STATE_QUERY_DEVICETYPE_DT8_COLORFEATURES == state) {
		this->m_colorFeatures = -1;
		return true;
	}

	const CString stringEmpty;
	const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress(), stringEmpty);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, ERROR_CODE_PROPERTY_NORESPONSE, -1);

	// De laatste toestand.
	if ((STATE_QUERY_DEVICE_FAILURE_LEVEL == state) && (!useAlgExitConfigMode())) {
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
		this->setState(STATE_DONE);			// De laatste
	}
	return true;
}

// Oppassen: 'IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
// TODO: Als we de NXP V90.30.06 aanpassen en deze als release plaatsen dan onderstaande code verwijderen?
bool CAppDaliFsmHelperRequestPropertiesControlGear::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if ((STATE_CFG_MODE_INITIALISE == state) || (STATE_CFG_MODE_ENTER_QUIESCENT_MODE == state) || (STATE_CFG_MODE_TERMINATE == state)) {
		ASSERT(this->useAlgEnterConfigMode() && this->useAlgExitConfigMode());
		return true;
	}
	if (STATE_CFG_MODE_EXIT_QUIESCENT_MODE == state) {
		ASSERT(this->useAlgEnterConfigMode() && this->useAlgExitConfigMode());
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
		this->setState(STATE_DONE);			// De laatste
		return true;
	}
	if ((STATE_QUERY_DEVICE_OPERATING_MODE == state) || (STATE_QUERY_DEVICE_EXTENDED_FADETIME == state)) {
		// IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
		// Control gear die dit bericht niet herkend zal niet antwoorden.
		// Vanaf NXP V90.30.06 wordt dit commando ondersteund als een Query.
		return true;
	}	
	if (STATE_ENABLE_DEVICETYPE_DT6_DIMMINGCURVE == state || STATE_ENABLE_DEVICETYPE_DT8_COLORFEATURES == state) {
		return true;
	}

	if (STATE_QUERY_DEVICETYPE_DT6_DIMMING_CURVE == state) {
		// CAN-NIC versies < V90.31.00 ondersteunen deze extended commandos niet.
		// Hierdoor wordt er niet gewacht op een backward response en krijgen we een 
		// tx-done.
		// Bij deze versies CAN-NIC kan de dimming curve niet opgevraagd/ingesteld worden.
		this->m_dimmingCurve = -1;
		return true;
	}

	// Fatal error
	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Wanneer er bij de search devices control gear aanwezig is met een dubbel address dan komen dan krijgen we hier een response error.
// Oppassen: Het is afhankelijk van de control gear devices. Zelfs wanneer we dubbele addressen hebben krijgen we soms nog correcte antwoorden.
bool CAppDaliFsmHelperRequestPropertiesControlGear::onFsmEventResponseError(CAppDaliFsmContext& refContext) {	
	CString stringError;
	stringError.Format("Device A%d: Received error when requesting the properties. Possible double address.",this->getShortAddress());
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);

	if (STATE_QUERY_NEXT_DEVICE_TYPE == this->getState()) {
		this->m_requestNextDeviceType = false;	
	}

	if (STATE_QUERY_DEVICE_FAILURE_LEVEL == this->getState() && (!this->useAlgExitConfigMode())) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
		this->setState(STATE_DONE);			// De laatste
	}
	return true;
}

bool CAppDaliFsmHelperRequestPropertiesControlGear::getWorkingmodeStandalone() const {
	return (FLAG_USE_STANDALONE == (this->m_workingFlags & FLAG_USE_STANDALONE));
}
bool CAppDaliFsmHelperRequestPropertiesControlGear::getWorkingmodeCommisioning() const {
	return (!(this->m_workingFlags & FLAG_USE_STANDALONE));
}

bool CAppDaliFsmHelperRequestPropertiesControlGear::useAlgEnterConfigMode() const {
	return (FLAG_CONFIG_MODE_ENTER == (this->m_workingFlags & FLAG_CONFIG_MODE_ENTER));
}

bool CAppDaliFsmHelperRequestPropertiesControlGear::useAlgExitConfigMode() const {
	return (FLAG_CONFIG_MODE_EXIT == (this->m_workingFlags & FLAG_CONFIG_MODE_EXIT));
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperRequestPropertiesControlInputDevice::CAppDaliFsmHelperRequestPropertiesControlInputDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) : 
	Base(refParent, refTransmitter), m_randomAddress(0), m_deviceVersion(-1), m_appControllerEnabled(false), m_powerCycleNotification(false),
	m_deviceCapabilities(-1), m_deviceStatus(-1), m_deviceNumberInstances(-1), m_deviceOperatingMode(-1), 
	m_bitfieldGroups(0) {}

CAppDaliFsmHelperRequestPropertiesControlInputDevice::~CAppDaliFsmHelperRequestPropertiesControlInputDevice( ) { }

bool CAppDaliFsmHelperRequestPropertiesControlInputDevice::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperRequestPropertiesControlInputDevice::start(int shortAddress) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// basis
	this->setState(STATE_READY);
	this->clearFatalErrorInfo();
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);

	// eigen properties/	
	this->m_randomAddress = 0;
	this->m_deviceVersion = -1;
	this->m_appControllerEnabled = false;
	this->m_powerCycleNotification = false;
	this->m_deviceCapabilities = -1;
	this->m_deviceStatus = -1;
	this->m_deviceNumberInstances = -1;
	this->m_deviceOperatingMode = -1;
	this->m_bitfieldGroups = 0;	
	return this->next();
}

bool CAppDaliFsmHelperRequestPropertiesControlInputDevice::next( ) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
			result = this->getTransmitter().sendQueryControlInputDeviceRandomAddressH(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_RANDOM_ADDRESS_H);
			}						
		} break;
		case STATE_QUERY_RANDOM_ADDRESS_H: {
			result = this->getTransmitter().sendQueryControlInputDeviceRandomAddressM(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_RANDOM_ADDRESS_M);
			}				
		} break;
		case STATE_QUERY_RANDOM_ADDRESS_M: {
			result = this->getTransmitter().sendQueryControlInputDeviceRandomAddressL(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_RANDOM_ADDRESS_L);
			}				
		} break;
		case STATE_QUERY_RANDOM_ADDRESS_L: {
			result = this->getTransmitter().sendQueryControlInputDeviceVersionNumber(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_VESRION_NUMBER);
			}				
		} break;
		case STATE_QUERY_VESRION_NUMBER: {
			result = this->getTransmitter().sendQueryControlInputDeviceNumberOfInstances(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES);
			}				
		} break;
		case STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES: {
			result = this->getTransmitter().sendQueryControlInputDeviceAppControllerEnabled(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_APPLICATIONCRTL_ENABLED);
			}			   
		} break;		
		case STATE_QUERY_APPLICATIONCRTL_ENABLED: {
			result = this->getTransmitter().sendQueryControlInputDevicePowerCycleNotification(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_POWERCYCLE_NOTIFICATION);
			}									   												   
		} break;
		case STATE_QUERY_DEVICE_POWERCYCLE_NOTIFICATION: {
			result = this->getTransmitter().sendQueryInputControlDeviceStatus(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_STATUS);
			}										   												   
		} break;	
		case STATE_QUERY_DEVICE_STATUS: {
			result = this->getTransmitter().sendQueryControlInputDeviceOperatingMode(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_OPERATINGMODE);
			}										   												   
		} break;
		case STATE_QUERY_DEVICE_OPERATINGMODE: {
			result = this->getTransmitter().sendQueryControlInputDeviceGroups_0_7(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_GROUPS_0_7);
			}									   												   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_0_7: {
			result = this->getTransmitter().sendQueryControlInputDeviceGroups_8_15(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_GROUPS_8_15);
			}													   												   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_8_15: {												   
			result = this->getTransmitter().sendQueryControlInputDeviceGroups_16_23(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_GROUPS_16_23);
			}									   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_16_23: {
			result = this->getTransmitter().sendQueryControlInputDeviceGroups_24_31(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_GROUPS_24_31);
			}													   												   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_24_31: {
			result = this->getTransmitter().sendQueryControlInputDeviceCapabilities(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_CAPABILITIES);
			}																											   
		} break;
		case STATE_QUERY_DEVICE_CAPABILITIES: {							
			ASSERT(0);
			result=false;														   												   
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperRequestPropertiesControlInputDevice::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_QUERY_RANDOM_ADDRESS_H: {
			this->m_randomAddress = data << 16;
		} break;
		case STATE_QUERY_RANDOM_ADDRESS_M: {
			this->m_randomAddress += data << 8;
		} break;
		case STATE_QUERY_RANDOM_ADDRESS_L: {
			this->m_randomAddress += data;
		} break;
		case STATE_QUERY_VESRION_NUMBER: {
			this->m_deviceVersion = data;						 
		} break;
		case STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES: {
			this->m_deviceNumberInstances = data;							
		} break;
		case STATE_QUERY_APPLICATIONCRTL_ENABLED: {
			if (0 != data) {
				this->m_appControllerEnabled = true;
			}												   												   
		} break;
		case STATE_QUERY_DEVICE_POWERCYCLE_NOTIFICATION: {
			if (0 != data) {
				this->m_powerCycleNotification = true;
			}									   												   
		} break;		
		case STATE_QUERY_DEVICE_STATUS: {
			this->m_deviceStatus = data;							   												   
		} break;
		case STATE_QUERY_DEVICE_OPERATINGMODE: {
			this->m_deviceOperatingMode = data;							   												   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_0_7: {
			this->m_bitfieldGroups = data;												   												   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_8_15: {
			this->m_bitfieldGroups += data<<8;						   												   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_16_23: {
			this->m_bitfieldGroups += data<<16;							   												   
		} break;
		case STATE_QUERY_DEVICE_GROUPS_24_31: {
			this->m_bitfieldGroups += data<<24;										   												   
		} break;
		case STATE_QUERY_DEVICE_CAPABILITIES: {
			this->m_deviceCapabilities = data;		
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
			this->setState(STATE_DONE);			// Voorlopig de laatste...
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperRequestPropertiesControlInputDevice::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_QUERY_APPLICATIONCRTL_ENABLED == state) {
		// answer no: geen antwoord.
		this->m_appControllerEnabled = false;
		return true;
	}
	if (STATE_QUERY_DEVICE_POWERCYCLE_NOTIFICATION == state) {
		// answer no: geen antwoord.
		this->m_powerCycleNotification = false;
		return true;
	}
	const CString stringEmpty;
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress(), stringEmpty);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, ERROR_CODE_PROPERTY_NORESPONSE, -1);
	if (STATE_QUERY_DEVICE_CAPABILITIES == state) {
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
		this->setState(STATE_DONE);				
	}
	return true;
}

// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Wanneer er control input devices aanwezig zijn die een dubbel short address hebben.
bool CAppDaliFsmHelperRequestPropertiesControlInputDevice::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress());
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
	if (STATE_QUERY_DEVICE_CAPABILITIES == this->getState()) {
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_PROPERTIES, -1, -1);
		this->setState(STATE_DONE);			
	}
	return true;
}

bool CAppDaliFsmHelperRequestPropertiesControlInputDevice::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperRequestControlInputDeviceInstances::CAppDaliFsmHelperRequestControlInputDeviceInstances(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
Base(refParent, refTransmitter), m_numberOfInstances(-1), m_currentInstance(-1), m_instanceType(-1), 
m_instanceEnabled(false), m_instanceEventScheme(-1), m_instanceEventPrio(-1), m_instanceEventResolution(-1), m_instanceStatus(-1), m_instanceEventFilter(-1) { 
	this->m_instanceGroupInfo[0] = -1;
	this->m_instanceGroupInfo[1] = -1;
	this->m_instanceGroupInfo[2] = -1;
}

CAppDaliFsmHelperRequestControlInputDeviceInstances::~CAppDaliFsmHelperRequestControlInputDeviceInstances( ) { }

bool CAppDaliFsmHelperRequestControlInputDeviceInstances::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperRequestControlInputDeviceInstances::start(int shortAddress) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	// basis
	this->clearFatalErrorInfo();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);

	// eigen properties
	this->m_numberOfInstances = -1;
	this->m_currentInstance = -1;	
	this->initDataInstance();			// Instance properties.	
	return this->next();
}

bool CAppDaliFsmHelperRequestControlInputDeviceInstances::next( ) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			result = this->getTransmitter().sendQueryControlInputDeviceNumberOfInstances(this->getShortAddress());	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES);
			}						
		} break;
		case STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES: {	
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);

			this->initDataInstance();

			// Event naar de master FSM:
			// Start opvragen van data...
			// param bevat de identifier van de instantie.
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE, this->m_currentInstance, -1);

			result = this->getTransmitter().sendQueryControlInputDeviceInstanceType(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_TYPE);						
			}
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_TYPE: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceEnabled(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_ENABLED);						
			}								   											   
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_ENABLED: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstancePrimaryGroup(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_GROUP_PRIMARY);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_GROUP_PRIMARY: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceGroup1(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_GROUP_1);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_GROUP_1: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceGroup2(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_GROUP_2);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_GROUP_2: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceEventScheme(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_EVENTSCHEME);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_EVENTSCHEME: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceEventPriority(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_EVENTPRIO);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_EVENTPRIO: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceResolution(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_RESOLUTION);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_RESOLUTION: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceStatus(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_STATUS);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_STATUS: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceEventFilter0_7(this->getShortAddress(),this->m_currentInstance);	
			if (result) {
				this->setState(STATE_QUERY_DEVICE_INSTANCE_FILTER_0_7);						
			}																	  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_FILTER_0_7: {
			// Oppassen:
			// this->m_currentInstance wordt verhoogd bij het ontvangen van een response.
			// TODO --> DEZE OOK VERHOGEN WANNEER WE GEEN RESPONSE KRIJGEN ??????				<----------- ??????????
			if (this->m_currentInstance < this->m_numberOfInstances) {

				this->initDataInstance();

				// Event naar de master FSM:
				// Start opvragen van data...
				// param bevat de identifier van de instantie.
				this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE, this->m_currentInstance, -1);

				result = this->getTransmitter().sendQueryControlInputDeviceInstanceType(this->getShortAddress(),this->m_currentInstance);	
				if (result) {
					this->setState(STATE_QUERY_DEVICE_INSTANCE_TYPE);						
				}
			}
			else {
				ASSERT(0);			
				result=false;						   
			}
		} break;		
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperRequestControlInputDeviceInstances::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES: {
			if (data > DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX) {
				ASSERT(0);
				data = DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX;
			}			
			this->m_numberOfInstances = data;		
			this->m_currentInstance = 0;
			if (this->m_numberOfInstances == 0) {
				this->setState(STATE_DONE);		
			}
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_TYPE: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceType = data;
		} break;

		case STATE_QUERY_DEVICE_INSTANCE_ENABLED: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			if (0 != data) {
				this->m_instanceEnabled = true;
			}								  
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_GROUP_PRIMARY: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceGroupInfo[0] = data;
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_GROUP_1: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceGroupInfo[1] = data;
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_GROUP_2: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceGroupInfo[2] = data;
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_EVENTSCHEME: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceEventScheme = data;
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_EVENTPRIO: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceEventPrio = data;
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_RESOLUTION: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceEventResolution = data;
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_STATUS: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);
			this->m_instanceStatus = data;
		} break;
		case STATE_QUERY_DEVICE_INSTANCE_FILTER_0_7: {
			ASSERT(this->m_currentInstance < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX);		
			this->m_instanceEventFilter = data;

			// Event naar de master FSM:
			// Data van een instantie is opgevraagd - Parent fsm kan dan deze toevoegen aan de database.
			// param bevat de identifier van de instantie.
			if ((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <=  DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST)) {
				this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE, this->m_currentInstance, -1);
			}

			// Volgende instanctie of gedaan?
			if (++this->m_currentInstance >= this->m_numberOfInstances) {
				this->setState(STATE_DONE);		
			}
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperRequestControlInputDeviceInstances::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_QUERY_DEVICE_INSTANCE_ENABLED == state) {
		this->m_instanceEnabled = false;
		return true;
	}

	CString strEmpty;
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress(), strEmpty);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE, ERROR_CODE_PROPERTY_NORESPONSE, -1);

	if (STATE_QUERY_DEVICE_INSTANCE_FILTER_0_7 == state) {
		// Event naar de master FSM:
		// Data van een instantie is opgevraagd - Parent fsm kan dan deze toevoegen aan de database.
		// param bevat de identifier van de instantie.
		if ((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <=  DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST)) {
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE, this->m_currentInstance, -1);
		}
		if (++this->m_currentInstance >= this->m_numberOfInstances) {
			this->setState(STATE_DONE);		
		}
	}
	return true;
}

// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Wanneer er control input devices aanwezig zijn die een dubbel short address hebben.
bool CAppDaliFsmHelperRequestControlInputDeviceInstances::onFsmEventResponseError(CAppDaliFsmContext& refContext) {	
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress());
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
	if (STATE_QUERY_DEVICE_INSTANCE_FILTER_0_7 == this->getState()) {
		// Event naar de master FSM:
		// Data van een instantie is opgevraagd - Parent fsm kan dan deze toevoegen aan de database.
		// param bevat de identifier van de instantie.
		if ((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <=  DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST)) {
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE, this->m_currentInstance, -1);
		}
		if (++this->m_currentInstance >= this->m_numberOfInstances) {
			this->setState(STATE_DONE);		
		}
	}
	return true;
}

bool CAppDaliFsmHelperRequestControlInputDeviceInstances::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

void CAppDaliFsmHelperRequestControlInputDeviceInstances::initDataInstance( ) {
	this->m_instanceType = -1;
	this->m_instanceEnabled = false;	
	this->m_instanceGroupInfo[0]=-1;
	this->m_instanceGroupInfo[1]=-1;
	this->m_instanceGroupInfo[2]=-1;
	this->m_instanceEventScheme=-1;
	this->m_instanceEventPrio=-1;
	this->m_instanceEventResolution=-1;
	this->m_instanceStatus=-1;
	this->m_instanceEventFilter=-1;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperQueryGroupsControlGear::CAppDaliFsmHelperQueryGroupsControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) : 
	Base(refParent, refTransmitter), m_bitfieldGroups(0), m_workingFlags(0)  {}
CAppDaliFsmHelperQueryGroupsControlGear::~CAppDaliFsmHelperQueryGroupsControlGear( ) {	
}

bool CAppDaliFsmHelperQueryGroupsControlGear::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperQueryGroupsControlGear::start(int shortAddress,bool busyCommisioning) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	// basis
	this->clearFatalErrorInfo();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);

	// eigen properties/
	if (busyCommisioning) {
		this->m_workingFlags = 0;
	}
	else {
		this->m_workingFlags = (FLAG_CONFIG_MODE_ENTER + FLAG_CONFIG_MODE_EXIT + FLAG_USE_STANDALONE);
	}
	this->m_bitfieldGroups = 0;
	return this->next();
}

bool CAppDaliFsmHelperQueryGroupsControlGear::next( ) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS, -1, -1);
			if (this->useAlgEnterConfigMode()) {
				static const int dataByte = 0x00;	// Initialise (all)
				result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(dataByte);
				ASSERT(result);
				if (result) {					
					this->setState(STATE_CFG_MODE_INITIALISE);
				}
			}
			else {
				result = this->getTransmitter().sendQueryControlGearGroups_0_7(this->getShortAddress());
				if (result) {
					this->setState(STATE_QUERY_0_7);
				}						
			}
		} break;

		case STATE_CFG_MODE_INITIALISE: {
			ASSERT(this->useAlgEnterConfigMode());
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {			
				this->setState(STATE_CFG_MODE_ENTER_QUIESCENT_MODE);
			}
		} break;

		case STATE_CFG_MODE_ENTER_QUIESCENT_MODE: {
			ASSERT(this->useAlgEnterConfigMode());
			result = this->getTransmitter().sendQueryControlGearGroups_0_7(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_0_7);
			}		
		} break;

		case STATE_QUERY_0_7: {
			result = this->getTransmitter().sendQueryControlGearGroups_8_15(this->getShortAddress());
			if (result) {
				this->setState(STATE_QUERY_8_15);
			}			  
		} break;
		case STATE_QUERY_8_15: {
			if (this->useAlgExitConfigMode()) {
				result = this->getTransmitter().sendControlgearBroadcastTerminate();
				if (result) {
					this->setState(STATE_CFG_MODE_TERMINATE);
				}			
			}
			else {
				ASSERT(0);				
				result = false;							   
			}
		} break;
		case STATE_CFG_MODE_TERMINATE: {
			ASSERT(this->useAlgExitConfigMode());
			result = this->getTransmitter().stopQuienscentMode();
			if (result) {			
				this->setState(STATE_CFG_MODE_EXIT_QUIESCENT_MODE);
			}
		} break;
		case STATE_CFG_MODE_EXIT_QUIESCENT_MODE: {
			ASSERT(0);
			result = false;
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperQueryGroupsControlGear::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_QUERY_0_7: {
			this->m_bitfieldGroups = data;					  
		} break;
		case STATE_QUERY_8_15: {
			this->m_bitfieldGroups += (data << 8);
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS, -1, -1);
			this->setState(STATE_DONE);
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
		default: {
			ASSERT(0);	// todo afhandelen als een fout...
			result = false; 
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperQueryGroupsControlGear::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
	const int shortAddress = this->getShortAddress();
	const CString stringEmpty;
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, daliDeviceType, shortAddress, stringEmpty);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS, ERROR_CODE_PROPERTY_NORESPONSE, -1);
	if ((STATE_QUERY_8_15 == this->getState()) && (!this->useAlgExitConfigMode())) {
		this->getParent().onNotifyFsmAlgoritmeDone(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS, -1, -1);
		this->setState(STATE_DONE);
	}
	return true;
}

bool CAppDaliFsmHelperQueryGroupsControlGear::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if ((STATE_CFG_MODE_INITIALISE == state) || (STATE_CFG_MODE_ENTER_QUIESCENT_MODE == state) || (STATE_CFG_MODE_TERMINATE == state)) {
		ASSERT(this->useAlgEnterConfigMode() || this->useAlgExitConfigMode());
		return true;
	}
	if (STATE_CFG_MODE_EXIT_QUIESCENT_MODE == state) {
		ASSERT(this->useAlgExitConfigMode());
		const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
		const int shortAddress = this->getShortAddress();
		this->getParent().onNotifyFsmAlgoritmeDone(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS, -1, -1);
		this->setState(STATE_DONE);
		return true;
	}
	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperQueryGroupsControlGear::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
	const int shortAddress = this->getShortAddress();
	CString stringError;
	stringError.Format("Device A%d: Received error when requesting the group properties. Possible double address.",shortAddress);
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, daliDeviceType, shortAddress, stringError);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
	if ((STATE_QUERY_8_15 == this->getState()) && (!this->useAlgExitConfigMode())) {
		this->getParent().onNotifyFsmAlgoritmeDone(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS, -1, -1);
		this->setState(STATE_DONE);
	}
	return true;
}

bool CAppDaliFsmHelperQueryGroupsControlGear::getWorkingmodeStandalone() const {
	return (FLAG_USE_STANDALONE == (this->m_workingFlags & FLAG_USE_STANDALONE));
}
bool CAppDaliFsmHelperQueryGroupsControlGear::getWorkingmodeCommisioning() const {
	return (!(this->m_workingFlags & FLAG_USE_STANDALONE));
}

bool CAppDaliFsmHelperQueryGroupsControlGear::useAlgEnterConfigMode() const {
	return (FLAG_CONFIG_MODE_ENTER == (this->m_workingFlags & FLAG_CONFIG_MODE_ENTER));
}

bool CAppDaliFsmHelperQueryGroupsControlGear::useAlgExitConfigMode() const {
	return (FLAG_CONFIG_MODE_EXIT == (this->m_workingFlags & FLAG_CONFIG_MODE_EXIT));
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

/***********************************************************************************/
CAppDaliFsmHelperQueryScenesControlGear::CAppDaliFsmHelperQueryScenesControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
Base(refParent,refTransmitter) ,m_workingFlags(0), m_sceneNumber(0) ,m_flagTerminate(false), 
m_supportDT8ColorObject(false), m_requestDT8ColorInfo(false), m_stateDT8ColorObject(SUB_STATE_DT8_NONE) {
	this->clear( );
}
CAppDaliFsmHelperQueryScenesControlGear::~CAppDaliFsmHelperQueryScenesControlGear( ) {
}

bool CAppDaliFsmHelperQueryScenesControlGear::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperQueryScenesControlGear::start(int shortAddress,bool supportDT8Color, bool busyCommisioning) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	// basis
	this->clearFatalErrorInfo();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);

	// eigen properties/
	this->clear();	

	// Bepaald hoe de fsm zal werken.
	this->m_supportDT8ColorObject = supportDT8Color;		
	if (busyCommisioning) {
		this->m_workingFlags = 0;
	}	
	else {
		this->m_workingFlags = (FLAG_CONFIG_MODE_ENTER + FLAG_CONFIG_MODE_EXIT + FLAG_USE_STANDALONE);
	}
	return this->next();
}

bool CAppDaliFsmHelperQueryScenesControlGear::next( ) {
	bool result = true;

	if (this->m_flagTerminate) {			
		result = this->getTransmitter().sendControlgearBroadcastTerminate();
		ASSERT(result);
		if (result) {
			TRACE("TX: sendControlgearBroadcastTerminate( )\r\n");			
			this->setState(STATE_CFG_MODE_TERMINATE);
		}						
		this->m_flagTerminate = false;
		return true;
	}

	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES, -1, -1);
			if (this->useAlgEnterConfigMode()) {
				static const int dataByte = 0x00;	// Initialise (all)
				result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(dataByte);				
				if (result) {					
					this->setState(STATE_CFG_MODE_INITIALISE);
				}
			}
			else {
				result = this->sendControlGearQuerySceneLevel();									
			}
		} break;
		case STATE_CFG_MODE_INITIALISE: {
			ASSERT(this->useAlgEnterConfigMode());
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {			
				this->setState(STATE_CFG_MODE_ENTER_QUIESCENT_MODE);
			}
		} break;
		case STATE_CFG_MODE_ENTER_QUIESCENT_MODE: {
			ASSERT(this->useAlgEnterConfigMode());	
			result = this->sendControlGearQuerySceneLevel();
		} break;

		case STATE_QUERY_SCENE_LEVEL: {	
			if (this->m_requestDT8ColorInfo) {						
				ASSERT(this->m_supportDT8ColorObject);			
				// Laden DTR0 voor het commando QUERY_COLOR_VALUE.
				result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_COLOUR_TYPE);
				if (result) {
					TRACE("TX: sendControlGearDTR0(%d)\r\n",DALI_DT8_COLOURVALUE_REPORT_COLOUR_TYPE);
					this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORTYPE);
					this->setState(STATE_DT8_LOAD_DTR); // _REPORT_COLOUR_TYPE);
				}			
				this->m_requestDT8ColorInfo = false;
			}
			else {
				if (++this->m_sceneNumber < MAX_SCENES) {
					result = this->sendControlGearQuerySceneLevel();
				} else { ASSERT(0); }
			}
		} break;
	
		case STATE_DT8_LOAD_DTR: { 
			// Versturen van een Enable Device Type.			
			ASSERT(this->m_supportDT8ColorObject);
			ASSERT(this->getStateDT8ColorObject() != SUB_STATE_DT8_NONE);
			result = this->getTransmitter().sendControlGearEnableDeviceType(8);
			if (result) {
				TRACE("TX: sendControlGearEnableDeviceType(%d)\r\n",8);				
				this->setState(STATE_DT8_ENABLE_DT8_DEVICE);
			}													  													  
		} break;

		case STATE_DT8_ENABLE_DT8_DEVICE: {
			// Na het versturen van een Enable device type kunnen we QUERY_COLOR_VALUE versturen.
			// Het antwoord is afhankelijk van de inhoud van DTR0
			ASSERT(this->m_supportDT8ColorObject);
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			ASSERT(stateDT8ColorObject != SUB_STATE_DT8_NONE);
			switch(stateDT8ColorObject) {
				case SUB_STATE_DT8_NONE: {
					ASSERT(0);				 
				} break;		
				case SUB_STATE_DT8_REQUEST_COLORTYPE: 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_TEMP_TC: 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_X_COORDINATE:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_Y_COORDINATE: 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_RED:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_GREEN:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_BLUE:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_WHITE:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_AMBER:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_FREECOLOR: 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL0:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL1:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL2:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL3:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL4:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL5: {
					result =  this->getTransmitter().sendQueryControlGearDT8ColourValue(this->getShortAddress());
					ASSERT(result);
					if (result) {
						TRACE("TX: sendQueryControlGearDT8ColourValue(A%d)\r\n",this->getShortAddress());
						this->setState(STATE_DT8_QUERY_COLOR_VALUE);
					}								  
				} break;			
			}
		} break;

		case STATE_DT8_QUERY_COLOR_VALUE: {	
			ASSERT(this->m_supportDT8ColorObject);
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			ASSERT(stateDT8ColorObject != SUB_STATE_DT8_NONE);
			switch(stateDT8ColorObject) {				
				case SUB_STATE_DT8_NONE: {
					ASSERT(0);				 
				} break;
				case SUB_STATE_DT8_REQUEST_COLORTYPE: {			
					ASSERT(this->m_sceneNumber <= MAX_SCENES-1);
					const int colorType = this->m_colorData[this->m_sceneNumber].getColorType();
					if ((colorType == 255) || (colorType == -1)) {						
						// Indien er geen kleur info voor deze scene moet opgevraagd worden mogen we opnieuw actual level opvragen.
						if (++this->m_sceneNumber < MAX_SCENES) {
							result = this->sendControlGearQuerySceneLevel();
						} else { ASSERT(0); }					
					}	
					else {			
						result = this->startRequestDT8ColorObject(colorType);																								
					}
				} break; /* SUB_STATE_DT8_REQUEST_COLORTYPE */

				case SUB_STATE_DT8_REQUEST_COLORVALUE_TEMP_TC: 					
				case SUB_STATE_DT8_REQUEST_COLORVALUE_X_COORDINATE:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_Y_COORDINATE: 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL0:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL1:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL2:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL3:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL4:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL5: {
					result = this->getTransmitter().sendQueryControlGearDeviceReadDTR0(this->getShortAddress());
					ASSERT(result);
					if (result) {						
						this->setState(STATE_DT8_QUERY_CONTENT_DTR);
					}																			   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_RED: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_GREEN);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_GREEN);
						this->setState(STATE_DT8_LOAD_DTR);		
					}									   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_GREEN: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_BLUE);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_BLUE);
						this->setState(STATE_DT8_LOAD_DTR);		
					}														 
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_BLUE: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_WHITE);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_WHITE);
						this->setState(STATE_DT8_LOAD_DTR);		
					}												
				} break;				
				case SUB_STATE_DT8_REQUEST_COLORVALUE_WHITE: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_AMBER);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_AMBER);
						this->setState(STATE_DT8_LOAD_DTR);		
					}											 
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_AMBER: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_FREECOLOR);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_FREECOLOR);
						this->setState(STATE_DT8_LOAD_DTR);		
					}													 
				} break;	
				case SUB_STATE_DT8_REQUEST_COLORVALUE_FREECOLOR: {
					if (++this->m_sceneNumber < MAX_SCENES) {
						result = this->sendControlGearQuerySceneLevel();
					} else { ASSERT(0); }								 
				} break;
			} /* switch */
		} break;

		case STATE_DT8_QUERY_CONTENT_DTR: {		
			ASSERT(this->m_supportDT8ColorObject);
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			ASSERT(stateDT8ColorObject != SUB_STATE_DT8_NONE);
			switch(stateDT8ColorObject) {									
				case SUB_STATE_DT8_NONE:{
					ASSERT(0);
				} break;
				case SUB_STATE_DT8_REQUEST_COLORTYPE: {
					ASSERT(0);							  
				} break;

				case SUB_STATE_DT8_REQUEST_COLORVALUE_TEMP_TC: 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_Y_COORDINATE:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL5: 
				{				
					ASSERT(this->m_sceneNumber <= MAX_SCENES-1);					
					if (++this->m_sceneNumber < MAX_SCENES) {
						result = this->sendControlGearQuerySceneLevel();
					}
					else { ASSERT(0); }
				} break; 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_X_COORDINATE: {
					result = this->startRequestDT8ColorObjectYCoordinate( );					
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL0: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_CHANNEL1);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL1);
						this->setState(STATE_DT8_LOAD_DTR);		
					}																	
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL1: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_CHANNEL2);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL2);
						this->setState(STATE_DT8_LOAD_DTR);		
					}																	
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL2: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_CHANNEL3);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL3);
						this->setState(STATE_DT8_LOAD_DTR);		
					}																	
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL3: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_CHANNEL4);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL4);
						this->setState(STATE_DT8_LOAD_DTR);		
					}																	
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL4: {
					result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_CHANNEL5);
					ASSERT(result);
					if (result) {
						this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL5);
						this->setState(STATE_DT8_LOAD_DTR);		
					}																	
				} break;		
			} /* end switch */		
		} break;

		case STATE_CFG_MODE_TERMINATE: {
			ASSERT(this->useAlgExitConfigMode());
			result = this->getTransmitter().stopQuienscentMode();
			if (result) {			
				this->setState(STATE_CFG_MODE_EXIT_QUIESCENT_MODE);
			}
		} break;
		case STATE_CFG_MODE_EXIT_QUIESCENT_MODE: {
			ASSERT(0);
			result = false;
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

bool CAppDaliFsmHelperQueryScenesControlGear::sendControlGearQuerySceneLevel( ) {
	INVARIANT(this->m_sceneNumber < MAX_SCENES);
	const bool result = this->getTransmitter().sendControlGearQuerySceneLevel(this->getShortAddress(), this->m_sceneNumber);
	if (result) {
		TRACE("TX: sendControlGearQuerySceneLevel(A%d,%d)\r\n",this->getShortAddress(), this->m_sceneNumber);
		this->m_requestDT8ColorInfo = false;
		this->setState(STATE_QUERY_SCENE_LEVEL);
	}	
	return result;
}

bool CAppDaliFsmHelperQueryScenesControlGear::startRequestDT8ColorObject(int colorType) {
	bool result = false;
	switch(colorType) {
		case DALI_DT8_COLOUR_TYPE_XY_COORDINATE: {
			result = this->startRequestDT8ColorObjectXCoordinate(colorType);										 
		} break;
		case DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC: {
			result = this->startRequestDT8ColorObjectTemperatureTC(colorType);
		} break;		
		case DALI_DT8_COLOUR_TYPE_PRIMARY_N: {
			result = this->startRequestDT8ColorObjectPrimaryNChannels(colorType);												 
		} break;
		case DALI_DT8_COLOUR_TYPE_RGBWAF: {
			result = this->startRequestDT8ColorObjectRGBWAF(colorType);	
		} break;				
		default: {
			ASSERT(0);
		} break;
	} /* switch (colorType) */
	return result;
}

// Laden DTR0 voor het commando QUERY_COLOR_VALUE (DALI_DT8_COLOURVALUE_REPORT_COLOUR_TEMP_TC = 226)
bool CAppDaliFsmHelperQueryScenesControlGear::startRequestDT8ColorObjectTemperatureTC(int colorType) {
	bool result = false;
	this->m_colorData[this->m_sceneNumber].setColorType(colorType);							
	result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_COLOUR_TEMP_TC);
	ASSERT(result);
	if (result) {
		this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_TEMP_TC);
		this->setState(STATE_DT8_LOAD_DTR);		
	}		
	return result;
}

bool CAppDaliFsmHelperQueryScenesControlGear::startRequestDT8ColorObjectRGBWAF(int colorType) {
	bool result = false;
	this->m_colorData[this->m_sceneNumber].setColorType(colorType);	
	result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_RED);
	ASSERT(result);
	if (result) {
		this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_RED);
		this->setState(STATE_DT8_LOAD_DTR);		
	}	
	return result;
}

bool CAppDaliFsmHelperQueryScenesControlGear::startRequestDT8ColorObjectPrimaryNChannels(int colorType) {
	bool result = false;
	this->m_colorData[this->m_sceneNumber].setColorType(colorType);	
	result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_DIMLEVEL_CHANNEL0);
	ASSERT(result);
	if (result) {
		this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL0);
		this->setState(STATE_DT8_LOAD_DTR);		
	}	
	return result;
}

// Laden DTR0 voor het commando QUERY_COLOR_VALUE (DALI_DT8_COLOURVALUE_REPORT_X_COORDINATE = 224)
bool CAppDaliFsmHelperQueryScenesControlGear::startRequestDT8ColorObjectXCoordinate(int colorType) {
	bool result = false;
	this->m_colorData[this->m_sceneNumber].setColorType(colorType);
	result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_X_COORDINATE);
	ASSERT(result);
	if (result) {
		this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_X_COORDINATE);
		this->setState(STATE_DT8_LOAD_DTR);		
	}		
	return result;
}

// Laden DTR0 voor het commando QUERY_COLOR_VALUE (DALI_DT8_COLOURVALUE_REPORT_X_COORDINATE = 224)
bool CAppDaliFsmHelperQueryScenesControlGear::startRequestDT8ColorObjectYCoordinate( ) {
	bool result = false;
	result = this->getTransmitter().sendControlGearDTR0(DALI_DT8_COLOURVALUE_REPORT_Y_COORDINATE);
	ASSERT(result);
	if (result) {
		this->setStateDT8ColorObject(SUB_STATE_DT8_REQUEST_COLORVALUE_Y_COORDINATE);
		this->setState(STATE_DT8_LOAD_DTR);		
	}		
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperQueryScenesControlGear::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	if (this->m_sceneNumber >= MAX_SCENES) {
		ASSERT(0);
		return false;
	}	
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_QUERY_SCENE_LEVEL: {					  
			TRACE("Scene %d: Level = %d\r\n",this->m_sceneNumber, data);
			this->m_scenesInfo[this->m_sceneNumber] = data;
			if (this->m_supportDT8ColorObject) {
				this->m_requestDT8ColorInfo = true;				
			}
			else {		
				this->m_requestDT8ColorInfo = false;
				this->isAlgoritmeDone( );
			}
		} break;	
		case STATE_DT8_QUERY_COLOR_VALUE: {		
			ASSERT(this->m_supportDT8ColorObject);
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			ASSERT(stateDT8ColorObject != SUB_STATE_DT8_NONE);
			switch(stateDT8ColorObject) {			
				case SUB_STATE_DT8_REQUEST_COLORTYPE: {
					this->m_colorData[this->m_sceneNumber].setColorType(data);
					if (data == 255) {
						this->isAlgoritmeDone( );
					}
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_TEMP_TC: {			
					// OPPASSEN: LSB Waarde
					this->m_colorData[this->m_sceneNumber].setColorTemperature(data << 8);															   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_X_COORDINATE: {
					// OPPASSEN: LSB Waarde
					this->m_colorData[this->m_sceneNumber].setCoordinateX(data << 8);												
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_Y_COORDINATE: {
					// OPPASSEN: LSB Waarde
					this->m_colorData[this->m_sceneNumber].setCoordinateY(data << 8);												
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_RED: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelRed(data);									   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_GREEN: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelGreen(data);									   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_BLUE: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelBlue(data);								   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_WHITE: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelWhite(data);										   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_AMBER: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelAmber(data);									   
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_FREECOLOR: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelFreeColor(data);	
					this->isAlgoritmeDone( );	
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL0: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(0,data << 8);			
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL1: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(1,data << 8);			
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL2:{
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(2,data << 8);			
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL3:{
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(3,data << 8);			
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL4:{
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(4,data << 8);			
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL5: {
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(5,data << 8);						
				} break;
				case SUB_STATE_DT8_NONE: {
					ASSERT(0);		// TODO FOUT....		 
				} break;				
			}
		} break;
		case STATE_DT8_QUERY_CONTENT_DTR: {	
			ASSERT(this->m_supportDT8ColorObject);
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			ASSERT(stateDT8ColorObject != SUB_STATE_DT8_NONE);
			switch(stateDT8ColorObject) {					 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_TEMP_TC: {	
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getColorTemperature()) + data;
					this->m_colorData[this->m_sceneNumber].setColorTemperature(value);
					TRACE("Scene %d, Color temperature TC = %d\r\n",this->m_sceneNumber,value);				
					this->isAlgoritmeDone( );	
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_X_COORDINATE: {
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getCoordinateX()) + data;
					this->m_colorData[this->m_sceneNumber].setCoordinateX(value);
					TRACE("Scene %d, Color X coordinate = %d\r\n",this->m_sceneNumber,value);				
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_Y_COORDINATE: {
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getCoordinateY()) + data;
					this->m_colorData[this->m_sceneNumber].setCoordinateY(value);
					TRACE("Scene %d, Color Y coordinate = %d\r\n",this->m_sceneNumber,value);					
					this->isAlgoritmeDone( );	
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL0: {
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getColorDimLevelChannel(0)) + data;
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(0,value);
					TRACE("Scene %d, Color channel0 = %d\r\n",this->m_sceneNumber,value);								
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL1: {
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getColorDimLevelChannel(1)) + data;
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(1,value);
					TRACE("Scene %d, Color channel1 = %d\r\n",this->m_sceneNumber,value);					
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL2:{
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getColorDimLevelChannel(2)) + data;
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(2,value);
					TRACE("Scene %d, Color channel2 = %d\r\n",this->m_sceneNumber,value);			
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL3:{
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getColorDimLevelChannel(3)) + data;
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(3,value);
					TRACE("Scene %d, Color channel3 = %d\r\n",this->m_sceneNumber,value);		
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL4:{
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getColorDimLevelChannel(4)) + data;
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(4,value);
					TRACE("Scene %d, Color channel4 = %d\r\n",this->m_sceneNumber,value);				
				} break;
				case SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL5: {
					// Oppassen MSB Value
					const int value = (this->m_colorData[this->m_sceneNumber].getColorDimLevelChannel(5)) + data;
					this->m_colorData[this->m_sceneNumber].setColorDimLevelChannel(5,value);
					TRACE("Scene %d, Color channel5 = %d\r\n",this->m_sceneNumber,value);		
					this->isAlgoritmeDone( );	
				} break;

				case SUB_STATE_DT8_NONE:
				case SUB_STATE_DT8_REQUEST_COLORTYPE: 
				case SUB_STATE_DT8_REQUEST_COLORVALUE_RED:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_GREEN:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_BLUE:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_WHITE:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_AMBER:
				case SUB_STATE_DT8_REQUEST_COLORVALUE_FREECOLOR: {
					ASSERT(0);				 
				} break;
			}										  
		} break;
		case STATE_CANCEL: {
			// absorbe.
			// Mogelijk dat er nog een event ontvangen wordt in deze toestand. Gewoon negeren.
		} break;
		default: {
			ASSERT(0);	// todo afhandelen als een fout...
			result = false; 
		} break;
	}
	return result;
}

CString CAppDaliFsmHelperQueryScenesControlGear::getStrState(State_t state) {
	switch(state) {
		case STATE_READY: {
			return ("STATE_READY"); 
		} break;
		case STATE_DONE: {
			return ("STATE_DONE"); 
		} break;
		case STATE_ERROR_FATAL: {
			return ("STATE_ERROR_FATAL"); 
		} break;
		case STATE_CANCEL: {
			return ("STATE_CANCEL"); 
		} break;
		case STATE_CFG_MODE_INITIALISE: {
			return ("STATE_CFG_MODE_INITIALISE"); 
		} break;
		case STATE_QUERY_SCENE_LEVEL: {
			return ("STATE_QUERY_SCENE_LEVEL"); 
		} break;
		case STATE_DT8_LOAD_DTR: {
			return ("STATE_DT8_LOAD_DTR"); 
		} break;
		case STATE_DT8_ENABLE_DT8_DEVICE: {
			return ("STATE_DT8_ENABLE_DT8_DEVICE"); 
		} break;
		case STATE_DT8_QUERY_COLOR_VALUE: {
			return ("STATE_DT8_QUERY_COLOR_VALUE"); 
		} break;
		case STATE_DT8_QUERY_CONTENT_DTR: {
			return ("STATE_DT8_QUERY_CONTENT_DTR"); 
		} break;	
		case STATE_CFG_MODE_TERMINATE: {
			return ("STATE_CFG_MODE_TERMINATE"); 
		} break;
		case STATE_CFG_MODE_EXIT_QUIESCENT_MODE: {
			return ("STATE_CFG_MODE_EXIT_QUIESCENT_MODE"); 
		} break;
	}
	return ("Undefined");
}
 
void CAppDaliFsmHelperQueryScenesControlGear::setStateDT8ColorObject(StateDT8ColorObject state) {
	this->m_stateDT8ColorObject = state;
}

CAppDaliFsmHelperQueryScenesControlGear::StateDT8ColorObject CAppDaliFsmHelperQueryScenesControlGear::getStateDT8ColorObject() const {
	return this->m_stateDT8ColorObject;
}

void CAppDaliFsmHelperQueryScenesControlGear::dumpState(State_t state) {
	TRACE("CAppDaliFsmHelperQueryScenesControlGear::State = %s\r\n",this->getStrState(state));
}

bool CAppDaliFsmHelperQueryScenesControlGear::isAlgoritmeDone( ) {
	bool result = false;
	if (this->m_sceneNumber >= MAX_SCENES-1) {				
		if (this->useAlgExitConfigMode()) {						
			this->m_flagTerminate = true;
		}
		else {
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES, -1, -1);
			this->setState(STATE_DONE);						
		}				
		result = true;
	}
	return result;
}

bool CAppDaliFsmHelperQueryScenesControlGear::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
	const int shortAddress = this->getShortAddress();
	const CString stringEmpty;
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, daliDeviceType, shortAddress, stringEmpty);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES, ERROR_CODE_PROPERTY_NORESPONSE, -1);
	if ((STATE_QUERY_SCENE_LEVEL == this->getState()) && (this->m_sceneNumber >= MAX_SCENES-1) && (!this->useAlgExitConfigMode())) {
		this->getParent().onNotifyFsmAlgoritmeDone(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES, -1, -1);
		this->setState(STATE_DONE);
	}
	return true;
}

// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
bool CAppDaliFsmHelperQueryScenesControlGear::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
	const int shortAddress = this->getShortAddress();
	CString stringError;
	stringError.Format("Device A%d: Received error when requesting the scene info. Possible double address.",shortAddress);
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, daliDeviceType, shortAddress, stringError);
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
	if ((STATE_QUERY_SCENE_LEVEL == this->getState()) && (this->m_sceneNumber >= MAX_SCENES-1) && (!this->useAlgExitConfigMode())) {
		this->getParent().onNotifyFsmAlgoritmeDone(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES, -1, -1);
		this->setState(STATE_DONE);
	}
	return true;
}

bool CAppDaliFsmHelperQueryScenesControlGear::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if ((STATE_CFG_MODE_INITIALISE == state) || (STATE_CFG_MODE_ENTER_QUIESCENT_MODE == state)) {
		ASSERT(this->useAlgEnterConfigMode());
		return true;
	}
	if (STATE_CFG_MODE_TERMINATE == state) {
		ASSERT(this->useAlgExitConfigMode());
		return true;
	}
	if (STATE_DT8_LOAD_DTR == state || STATE_DT8_ENABLE_DT8_DEVICE == state) {
		return true;
	}
	if (STATE_CFG_MODE_EXIT_QUIESCENT_MODE == state) {
		ASSERT(this->useAlgExitConfigMode());
		const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
		const int shortAddress = this->getShortAddress();
		this->getParent().onNotifyFsmAlgoritmeDone(daliDeviceType, shortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES, -1, -1);
		this->setState(STATE_DONE);
		return true;
	}

	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

BYTE CAppDaliFsmHelperQueryScenesControlGear::getSceneActualLevel(int sceneNumber) {
	if (sceneNumber<0 || sceneNumber>=MAX_SCENES) {
		ASSERT(0);
		return 0xFF;
	}
	return this->m_scenesInfo[sceneNumber];
}

const CAppDaliObjectDT8ColorData& CAppDaliFsmHelperQueryScenesControlGear::getSceneDT8ColorData(int sceneNumber) {
	if (sceneNumber<0 || sceneNumber>=MAX_SCENES) {
		ASSERT(0);
		static const CAppDaliObjectDT8ColorData empty;	
		return empty;
	}
	return this->m_colorData[sceneNumber];
}

void CAppDaliFsmHelperQueryScenesControlGear::clear( ) {
	this->m_sceneNumber = 0;
	this->m_flagTerminate = false;
	this->m_supportDT8ColorObject = false;
	this->m_requestDT8ColorInfo = false;
	this->setStateDT8ColorObject(SUB_STATE_DT8_NONE);
	for (int i=0;i<MAX_SCENES;++i) {
		this->m_scenesInfo[i] = 255;
		this->m_colorData[i].clear();	
	}
}

bool CAppDaliFsmHelperQueryScenesControlGear::getWorkingmodeStandalone() const {
	return (FLAG_USE_STANDALONE == (this->m_workingFlags & FLAG_USE_STANDALONE));
}
bool CAppDaliFsmHelperQueryScenesControlGear::getWorkingmodeCommisioning() const {
	return (!(this->m_workingFlags & FLAG_USE_STANDALONE));
}
bool CAppDaliFsmHelperQueryScenesControlGear::useAlgEnterConfigMode() const {
	return (FLAG_CONFIG_MODE_ENTER == (this->m_workingFlags & FLAG_CONFIG_MODE_ENTER));
}
bool CAppDaliFsmHelperQueryScenesControlGear::useAlgExitConfigMode() const {
	return (FLAG_CONFIG_MODE_EXIT == (this->m_workingFlags & FLAG_CONFIG_MODE_EXIT));
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
static void dbgDumpSendQueryControlGearMemory(int shortAddress,int memoryLocation) {
	CString str;
	str.Format("DALI %d: QueryControlGearMemory: Address=0%04x\r\n",shortAddress,memoryLocation);
	TRACER(str);
}
/***********************************************************************************/
CAppDaliFsmHelperReadMemory::CAppDaliFsmHelperReadMemory(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_currentAddress(0xffff), m_nrofBytesToRead(0), m_currentBytesRead(0),
	m_waitForResponse(false), m_waitForVerify(false), m_stateErrorActive(false), m_GTIN(0), m_serialNumber(0) {  }

void CAppDaliFsmHelperReadMemory::initContext(int shortAddress,uint16_t address,int numberOfBytes) {

	// Basisklasse
	this->clearFatalErrorInfo();
	this->setShortAddress(shortAddress);

	// werking
	this->m_waitForResponse = false;
	this->m_waitForVerify = false;
	this->m_stateErrorActive = false;
	this->m_currentAddress = address;
	this->m_nrofBytesToRead = numberOfBytes;	
	this->m_currentBytesRead = 0;
	// properties.
	this->m_GTIN = 0;
	this->m_serialNumber = 0;
}

void CAppDaliFsmHelperReadMemory::setLastMemoryLocation(int value) {
	const int nrOfBytes = value + 1;			// Oppassen:	te lezen bytes = laatste locatie + 1;
	if (this->m_nrofBytesToRead > nrOfBytes) {
		this->m_nrofBytesToRead = nrOfBytes;
	}
}

bool CAppDaliFsmHelperReadMemory::busyWaitingForResponse( ) {
	return this->m_waitForResponse;
}
bool CAppDaliFsmHelperReadMemory::busyVerifyMemoryRead( ) {
	return this->m_waitForVerify;
}
void CAppDaliFsmHelperReadMemory::setFlagErrorActive( ) {
	this->m_stateErrorActive = true;
}
bool CAppDaliFsmHelperReadMemory::getFlagErrorActive( ) const {
	return this->m_stateErrorActive;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperReadMemory_Bank0::CAppDaliFsmHelperReadMemory_Bank0(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent,refTransmitter), m_versionFirmware(0),m_versionHardware(0), m_versionNumber(-1),m_versionNumberControlGear(-1),
	m_versionNumberInputDevices(-1),m_numberOfLogicalUnits(-1),m_numberOfLogicalUnitsControlDevices(-1),
	m_numberOfLogicalUnitsControlGear(-1) { }

void CAppDaliFsmHelperReadMemory_Bank0::initData( ) {
	this->m_versionFirmware = 0;
	this->m_versionHardware= 0;
	this->m_versionNumber= -1;
	this->m_versionNumberControlGear= -1;
	this->m_versionNumberInputDevices= -1;
	this->m_numberOfLogicalUnits= -1;
	this->m_numberOfLogicalUnitsControlDevices= -1;
	this->m_numberOfLogicalUnitsControlGear= -1;
}

void CAppDaliFsmHelperReadMemory_Bank0::updateRecievedData(int data) {
	const int currentMemoryLocation = this->getCurrMemoryLocation();
	switch(currentMemoryLocation) {
		case 0x0000: {
			const int lastMemoryLocation = data;
			this->initData();			
			this->setLastMemoryLocation(lastMemoryLocation);
		} break;

		case 0x0003:
		case 0x0004:
		case 0x0005:
		case 0x0006:
		case 0x0007:
		case 0x0008: {
			// 6 bytes GTIN.
			// Unieke product code....
			this->m_GTIN = this->m_GTIN << 8;
			this->m_GTIN += data;						
		} break;

		case 0x0009:
		case 0x000A: {
			this->m_versionFirmware = this->m_versionFirmware << 8;
			this->m_versionFirmware += data;
		} break;

		case 0x000B:
		case 0x000C:
		case 0x000D:
		case 0x000E:
		case 0x000F:
		case 0x0010:
		case 0x0011:
		case 0x0012: {
			// Serial number.
			this->m_serialNumber = this->m_serialNumber << 8;
			this->m_serialNumber += data;		
		} break;	
		case 0x0013:
		case 0x0014: {
			this->m_versionHardware = this->m_versionHardware << 8;
			this->m_versionHardware += data;
		} break;
		case 0x0015: {
			this->m_versionNumber = data;			
		} break;
		case 0x0016: {						   
			this->m_versionNumberControlGear = data;
		} break;
		case 0x0017: {											
			this->m_versionNumberInputDevices = data;
		} break;
		case 0x0018: {
			this->m_numberOfLogicalUnits = data;												
		} break;	
		case 0x0019: {						   					
			this->m_numberOfLogicalUnitsControlGear = data;
		} break;	
		case 0x001A: {
			this->m_numberOfLogicalUnitsControlDevices = data;
		} break;
	}	
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperReadMemoryControlGear::CAppDaliFsmHelperReadMemoryControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter) {  }

bool CAppDaliFsmHelperReadMemoryControlGear::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperReadMemoryControlGear::start(int shortAddress,uint16_t address,int numberOfBytes) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	this->CAppDaliFsmHelperReadMemory::initContext(shortAddress, address, numberOfBytes);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->initData();	
	this->setState(STATE_READY);
	return this->next();
}

// Versturen van een volgende byte op de bus.
// indien er niks verstuurd werd dan return false.
bool CAppDaliFsmHelperReadMemoryControlGear::next( ) {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
			const BYTE data = (this->m_currentAddress & 0xFF00)	>> 8;
			result = this->getTransmitter().sendControlGearDTR1(data);
			if (result) {
				this->setState(STATE_MSB);
			}
		} break;
		case STATE_MSB: {
			const BYTE data = (this->m_currentAddress & 0xFF);
			result = this->getTransmitter().sendControlGearDTR0(data);
			if (result) {
				this->setState(STATE_LSB);
			}		
		} break;
		case STATE_LSB: {			
			result = this->getTransmitter().sendQueryControlGearMemory(this->getShortAddress());	
			if (result) {
				dbgDumpSendQueryControlGearMemory(this->getShortAddress(),this->m_currentAddress);
				this->m_waitForResponse = true;				
				this->m_currentBytesRead++;	
				this->setState(STATE_MEMORY_READ);
			}
		} break;
		case STATE_MEMORY_READ: {
			if (this->m_currentBytesRead<this->m_nrofBytesToRead) {
				result = this->getTransmitter().sendQueryControlGearMemory(this->getShortAddress());	
				if (result) {					
					this->m_waitForVerify = false;
					this->m_waitForResponse = true;
					this->m_currentBytesRead++;
					this->m_currentAddress++;
					dbgDumpSendQueryControlGearMemory(this->getShortAddress(),this->m_currentAddress);
				}
			}
			else {
				result = this->getTransmitter().sendQueryControlGearDTR0(this->getShortAddress());	
				if (result) {
					this->m_waitForResponse = true;
					this->m_waitForVerify = true;				
				}
			}								
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// Response ontvangen.
bool CAppDaliFsmHelperReadMemoryControlGear::onFsmEventResponse(CAppDaliFsmContext& refContext,int data)
{
	if (false == this->busyWaitingForResponse()) {
		return false;
	}
	REQUIRE(data>=0 && data<=0xff);

	bool result = true;
	const int currentMemoryLocation = this->getCurrMemoryLocation();
	if (this->busyVerifyMemoryRead()) {		
		const int numberOfBytesRead = data;
		const int lastMemoryLocation = 0x0000 + (numberOfBytesRead - 1);			
		
		if (currentMemoryLocation != lastMemoryLocation) {
			// TM20251218: SOLVED BUG-0186 - Mogen hier geen fatal error geven.
			// Bij sommige(oudere) DALI - apparaten die niet volledig volgens de specificaties werken, kon de configuratie stoppen bij het verifiëren van de geheugenmap.
			// Wijziging: Het proces gaat nu door, ook als de controle mislukt, waardoor parameters alsnog opgevraagd worden.
			// Let op: GTIN of serienummer kan mogelijk niet correct in de software worden weergegeven.
#if(0)
			this->m_GTIN = 0;
			this->m_serialNumber = 0;
			this->initData();
			const CString stringErrorDetail("Requesting control gear memory. Wrong memory location. Cannot handle this error.");
			this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
			this->setState(STATE_ERROR_FATAL);			// errorCode=ERROR_CODE_WRONG_EVENT
			result = false;
			return false;
#endif
		}

		// TM20251218: SOLVED BUG-0186 - Fout negeren
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
		this->setState(STATE_DONE);
	}
	else {
		ASSERT(this->busyReadingMemory());
		this->updateRecievedData(data);
	}
	return result;
}

// Sedert V16.40: Address 0x0001 => Answer no.
// Krijgen geen response.
//
// Oppassen: zie SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Wanneer we een dubbel adres hebben krijgen we geen response meer op bepaalde geheugen locaties.
// We gaan een controle doen of alle geheugen properties uitgelezen werden. (Het gevraagde aantal).
// Indien deze uitgelezen werden gaan we naar de toestand done...
// Als extra controle gebruiken we de variabele m_stateErrorActive (zie getFlagErrorActive()).
// Deze vlag wordt gezet wanneer we backward response fouten ontvangen.
bool CAppDaliFsmHelperReadMemoryControlGear::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int currentMemoryLocation = this->getCurrMemoryLocation();
	if (0x0001 == currentMemoryLocation) {
		return true;
	}

	// Fout entry.
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress());
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, ERROR_CODE_PROPERTY_NORESPONSE, -1);

	// Laatste toestand moeten we het algoritme afsluiten.
	if ((this->getFlagErrorActive()) && (this->m_currentBytesRead>=this->m_nrofBytesToRead)) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
		this->setState(STATE_DONE);	
	}
	return true;
}

// Oppassen: zie SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Indien alle data bytes uitgelezen zijn moeten we naar de STATE_DONE toestand gaan.
bool CAppDaliFsmHelperReadMemoryControlGear::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	if (!this->busyWaitingForResponse()) {
		CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
		this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
		return false;
	}

	// Fout entry.
	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress());
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);

	// zie SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
	// Dubbele addressen gedecteerd.
	// We gaan dit bijhouden in het algoritme. Dit is nodig omdat het aantal te lezen geheugen locaties 
	// mogelijk verkeerd is (omdat we deze verkeerd ontvangen hebben.)
	// Mogelijks zullen er geheugen locaties opgevraagd worden die niet aanwezig zijn waardoor we een
	// no-response ontvangen.
	this->setFlagErrorActive(); 

	// Bij de laatste read moeten we het algoritme verlaten. (Zelfde controle als bij de no-response).
	if (this->m_currentBytesRead>=this->m_nrofBytesToRead) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
		this->setState(STATE_DONE);	
	}
	return true;
}

bool CAppDaliFsmHelperReadMemoryControlGear::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	if (this->busyWaitingForResponse()) {
		CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
		this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
		return false;
	}
	return true;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperReadMemoryInputDevice::CAppDaliFsmHelperReadMemoryInputDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
Base(refParent,refTransmitter) { }

bool CAppDaliFsmHelperReadMemoryInputDevice::cancel( ) {	
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperReadMemoryInputDevice::start(int shortAddress,uint16_t address,int numberOfBytes) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	this->Base::initContext(shortAddress, address, numberOfBytes);
	this->setState(STATE_READY);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->initData();	
	return this->next();
}

// Response ontvangen.
bool CAppDaliFsmHelperReadMemoryInputDevice::onFsmEventResponse(CAppDaliFsmContext& refContext, int data)
{
	if (false == this->busyWaitingForResponse()) {
		return false;
	}
	REQUIRE(data>=0 && data<=0xff);

	bool result = true;
	const int currentMemoryLocation = this->getCurrMemoryLocation();
	if (this->busyVerifyMemoryRead()) {		
		const int numberOfBytesRead = data;
		const int lastMemoryLocation = 0x0000 + (numberOfBytesRead - 1);			// TODO: Start locatie staat nu op 0x0000 (property in de basisklasse?)
		if (currentMemoryLocation == lastMemoryLocation) {			
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
			this->setState(STATE_DONE);	
		}
		else {
			this->m_GTIN = 0;
			this->m_serialNumber = 0;
			this->initData();
			
			const CString stringErrorDetail("Requesting control gear memory. Wrong memory location. Cannot handle this error.");
			this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
			this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
			result = false;
		}
	}
	else {
		ASSERT(this->busyReadingMemory());
		this->updateRecievedData(data);
	}
	return result;
}

bool CAppDaliFsmHelperReadMemoryInputDevice::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int address = this->getCurrMemoryLocation();	
	if (address == 0x0001) {			
		return true;
	}
	if (false == this->busyWaitingForResponse()) {
		CString stringErrorDetail("Recieved no Backward response event from DALI controller but expected tx done response.");
		this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
		return false;
	}

	CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress());
	this->setErrorEntry(refContext, errorEntry);
	this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, ERROR_CODE_PROPERTY_NORESPONSE, -1);

	// Laatste toestand moeten we het algoritme afsluiten.
	if ((this->getFlagErrorActive()) && (this->m_currentBytesRead>=this->m_nrofBytesToRead)) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
		this->setState(STATE_DONE);	
	}
	return true;
}

// SOLUTION_HANDLE_DOUBLE_SHORTADDRESSES
// Wanneer er control input devices aanwezig zijn die een dubbel short address hebben.
bool CAppDaliFsmHelperReadMemoryInputDevice::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	if ((STATE_MEMORY_READ == this->getState()) && (true == this->m_waitForResponse)) {
		const int shortAddress = this->getShortAddress();
		CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress()); // , stringError);
		this->setErrorEntry(refContext, errorEntry);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
		return true;
	}

	// Laatste toestand.
	this->setFlagErrorActive(); 

	// Bij de laatste read moeten we het algoritme verlaten. (Zelfde controle als bij de no-response).
	if (this->m_currentBytesRead>=this->m_nrofBytesToRead) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
		this->setState(STATE_DONE);	
	}
	return true;
}

bool CAppDaliFsmHelperReadMemoryInputDevice::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	if (this->busyWaitingForResponse()) {
		CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
		this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
		return false;
	}				
	return true;
}

// Versturen van een volgende byte op de bus.
// indien er niks verstuurd werd dan return false.
bool CAppDaliFsmHelperReadMemoryInputDevice::next( ) {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_REQUEST_MEMORYMAP, -1, -1);
			const BYTE dtr1 = (this->m_currentAddress & 0xFF00) >> 8;
			const BYTE dtr0 = (this->m_currentAddress & 0xFF);
			result = this->getTransmitter().sendControlInputDevicesBroadcastWriteDTR1_DTR0(dtr0,dtr1);
			if (result) {
				this->setState(STATE_LOAD_ADDRESS);
			}
		} break;
		case STATE_LOAD_ADDRESS: {			
			result = this->getTransmitter().sendQueryControlInputDeviceReadMemoryLocation(this->getShortAddress());	
			if (result) {
				dbgDumpSendQueryControlGearMemory(this->getShortAddress(),this->m_currentAddress);
				this->m_waitForResponse = true;				
				this->m_currentBytesRead++;	
				this->setState(STATE_MEMORY_READ);
			}
		} break;
		case STATE_MEMORY_READ: {
			if (this->m_currentBytesRead<this->m_nrofBytesToRead) {
				result = this->getTransmitter().sendQueryControlInputDeviceReadMemoryLocation(this->getShortAddress());	
				if (result) {					
					this->m_waitForVerify = false;
					this->m_waitForResponse = true;
					this->m_currentBytesRead++;
					this->m_currentAddress++;
					dbgDumpSendQueryControlGearMemory(this->getShortAddress(),this->m_currentAddress);
				}
			}
			else {
				result = this->getTransmitter().sendQueryControlInputDeviceReadDTR0(this->getShortAddress());	
				if (result) {
					this->m_waitForResponse = true;
					this->m_waitForVerify = true;					
				}
			}								
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
CAppDaliFsmHelperDaliDevicesSearchUnaddressed::CAppDaliFsmHelperDaliDevicesSearchUnaddressed(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent,refTransmitter), m_foundDevices(false) { }

bool CAppDaliFsmHelperDaliDevicesSearchUnaddressed::cancel( ) {
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}
	
bool CAppDaliFsmHelperDaliDevicesSearchUnaddressed::start(DaliDeviceType_t deviceTypes) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	this->setDaliDeviceType(deviceTypes);
	this->setState(STATE_READY);
	this->m_foundDevices = false;
	return this->next();
}

bool CAppDaliFsmHelperDaliDevicesSearchUnaddressed::next( ) {
	bool result = true;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_SEARCH_UNADDRESSED_DEVICES, -1, -1);

			// Oppassen:
			// Bij Control gear = 0xFF,
			// Bij Control input devices = 0x7F (enkel unaddressed).
		
			const int address = (DALI_DEVICE_TYPE_CONTROLGEAR == this->getDaliDeviceType() ? 0xFF : 0x7F);	
			result = this->sendBroadcastInitialiseAll(this->getDaliDeviceType(), address);
			ASSERT(true == result);
			if (result) {
				this->setState(STATE_INITIALISE);
			}		
		} break;

		case STATE_INITIALISE: {
			result = this->sendBroadcastRandomize(this->getDaliDeviceType());
			ASSERT(true == result);
			if (result) {
				this->setState(STATE_RANDOMIZE);
			}		
		} break;

		case STATE_RANDOMIZE: {
			result = this->sendBroadcastSearchAddrL(this->getDaliDeviceType(),0xff);
			ASSERT(true == result);
			if (result) {
				this->setState(STATE_SEARCHADDR_L);
			}
		} break;
		case STATE_SEARCHADDR_L: {
			result = this->sendBroadcastSearchAddrM(this->getDaliDeviceType(),0xff);
			ASSERT(true == result);
			if (result) {
				this->setState(STATE_SEARCHADDR_M);
			}
		} break;
		case STATE_SEARCHADDR_M: {
			result = this->sendBroadcastSearchAddrH(this->getDaliDeviceType(),0xff);
			ASSERT(true == result);
			if (result) {
				this->setState(STATE_SEARCHADDR_H);
			}
		} break;
		case STATE_SEARCHADDR_H: {
			result = this->sendBroadcastCompare(this->getDaliDeviceType());	
			ASSERT(true == result);
			if (result) {
				this->setState(STATE_COMPARE);
			}
		} break;
		case STATE_COMPARE: {
			result = this->sendBroadcastTerminate(this->getDaliDeviceType());	
			ASSERT(true == result);			
			if (result) {
				this->setState(STATE_TERMINATE);
			}
		} break;
		case STATE_TERMINATE: {
			ASSERT(0);
			result = false;			  
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// Wanneer we een response krijgen op de Compare Query dan zijn er devices aanwezig zonder adres.
// Geen controle doen op de data (mogelijks antwoorden er meerdere en krijgen we geen 0xFF - normaal wordt dit gezien als een frame error.)
bool CAppDaliFsmHelperDaliDevicesSearchUnaddressed::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	if (STATE_COMPARE == this->getState()) {		
		this->m_foundDevices = true;			
		return true;
	}

	// Fatale fout
	CString stringErrorDetail("Recieved backward response event done from DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// ErrorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// Wanneer we geen response krijgen op de Compare Query dan zijn er GEEN devices aanwezig zonder adres.
bool CAppDaliFsmHelperDaliDevicesSearchUnaddressed::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	if (STATE_COMPARE == this->getState()) {
		this->m_foundDevices = false;	
		return true;
	}

	// Fatale fout.
	CString stringErrorDetail("Recieved no backward response event done from DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// ErrorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// Na het versturen van de randomize moeten we 100 msec wachten voor het versturen van het searchaddress.
bool CAppDaliFsmHelperDaliDevicesSearchUnaddressed::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_RANDOMIZE == state) {
		::Sleep(80);		// zie NOTE
		return true;
	}
	if (STATE_TERMINATE == state) {
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_SEARCH_UNADDRESSED_DEVICES, -1, -1);
		this->setState(STATE_DONE);
		return true;
	
	}
	if ((STATE_INITIALISE == state) || (STATE_SEARCHADDR_L ==  state) || (STATE_SEARCHADDR_M ==  state) || (STATE_SEARCHADDR_H == state)) {
		return true;
	} 

	// Fatale fout.
	CString stringErrorDetail("Recieved TX event done from DALI controller but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// ErrorCode=ERROR_CODE_WRONG_EVENT
	return false;
}


// Sedert V16.40: Wanneer er meerdere antwoorden hebben we een framing error.
// Moeten found devices op 1 zetten.
bool CAppDaliFsmHelperDaliDevicesSearchUnaddressed::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	if (STATE_COMPARE == this->getState()) {		
		this->m_foundDevices = true;			
		return true;
	}

	// Fatale fout.
	CString stringErrorDetail("Recieved backward error response frame from the DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// ErrorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
CAppDaliFsmHelperDaliDevicesAddress::CAppDaliFsmHelperDaliDevicesAddress(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent,refTransmitter), m_currShortAddress(0), m_searchAddress(0x800000), m_searchAddrAddSub(0x400000), 
	m_searchAddressPrevious(0xFFFFFF), m_searchBusy(false), m_assignShortAddress(false), m_terminate(false), m_assignOnlyUnAddressed(false)  { }

bool CAppDaliFsmHelperDaliDevicesAddress::cancel( ) {
	this->dbgDumpContext("--> Cancel algoritme\r\n");
	this->cancelTask( );
	this->setState(STATE_CANCEL);
	return true;
}
	
bool CAppDaliFsmHelperDaliDevicesAddress::start(DaliDeviceType_t daliDeviceType,bool assignOnlyUnAddressed) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	this->dbgDumpContext("--> Start algoritme\r\n");

	this->setState(STATE_READY);
	this->getFatalErrorInfo().clear();
	this->clearCounterValue();

	// eigen properties/		
	this->setDaliDeviceType(daliDeviceType);
	this->m_assignOnlyUnAddressed = assignOnlyUnAddressed;
	this->m_currShortAddress = 0;
	this->m_searchAddress = 0x800000;
	this->m_searchAddrAddSub = 0x400000;
	this->m_searchAddressPrevious = 0xFFFFFF;
	this->m_searchBusy = true;
	this->m_assignShortAddress = false;
	this->m_terminate = false;
	for (int i=0;i<DALI_SHORTADDRESS_MAX;++i) {
		this->m_assignedAddr[i] = 0;
	}
	return this->next();
}

#if (DEBUG_LOCAL_ON == 1)
	#define TRACER_SEARCHADDRESS(lsb,msb,hsb)								dbgDumpSendSearchAddress(lsb,msb,hsb)
	#define TRACER_QUERYDEVICESTATUS(shortAddress)							dbgDumpQueryDeviceStatus(shortAddress)
	#define TRACER_PROGRAMSHORTADDRESS(shortAddress,searchAddress)			dbgDumpProgramShortAddress(shortAddress,searchAddress)
#else 
	#define TRACER_SEARCHADDRESS(lsb,msb,hsb)
	#define TRACER_QUERYDEVICESTATUS(shortAddress)
	#define TRACER_PROGRAMSHORTADDRESS(shortAddress,searchAddress)	
#endif


#if (DEBUG_LOCAL_ON == 1)
static void dbgDumpSendSearchAddress(int lsb, int msb, int hsb) {
	CString strDebug;
	if (lsb >= 0) {
		strDebug.Format("TX: sendControlgearBroadcastSearchAddrL(%02X)\r\n", lsb);
	}
	else if (msb >= 0) {
		strDebug.Format("TX: sendControlgearBroadcastSearchAddrM(%02X)\r\n", msb);
	}
	else if (hsb >= 0) {
		strDebug.Format("TX: sendControlgearBroadcastSearchAddrH(%02X)\r\n", hsb);
	}
	else { ASSERT(0); }
	TRACER(strDebug);
}

static void dbgDumpQueryDeviceStatus(int shortAddress) {
	CString strDebug;
	strDebug.Format("TX: sendQueryControlGearDeviceStatus(%d)\r\n", shortAddress);
	TRACER(strDebug);
}

static void dbgDumpProgramShortAddress(int shortAddress, uint32_t searchAddress) {
	CString strDebug;
	strDebug.Format("--> SearchAddress=0x%08x: assign short address %d\r\n", searchAddress, shortAddress);
	TRACER(strDebug);
}
#endif 

bool CAppDaliFsmHelperDaliDevicesAddress::next( ) {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES, -1, -1);
			// Sedert V16.39:
			// control devices: 
			//	- Alle addressen een nieuw adres toekennen -> 0xFF.
			//  - Enkel de devices die geen adres hebben -> 0x7F 		

			BYTE bData = 0xFF;
			if ((DALI_DEVICE_TYPE_INPUTCONTROL == this->getDaliDeviceType()) && (true == this->m_assignOnlyUnAddressed)) {
				bData = 0x7F;
			}
			result = this->sendBroadcastInitialiseAll(this->getDaliDeviceType(), bData);
			ASSERT(true == result);
			if (result) {
				TRACER("TX: sendBroadcastInitialiseAll()\r\n");
				this->setState(STATE_INITIALISE);
			}					
		} break;
		case STATE_INITIALISE: {
			result = this->sendBroadcastRandomize(this->getDaliDeviceType());
			ASSERT(true == result);
			if (result) {
				TRACER("TX: sendBroadcastRandomize()\r\n");
				this->setState(STATE_RANDOMIZE);
			}		
		} break;
		case STATE_RANDOMIZE: {	
			ASSERT((this->m_currShortAddress>=DALI_SHORTADDRESS_FIRST) && (this->m_currShortAddress<=DALI_SHORTADDRESS_LAST));
			result = this->sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_currShortAddress);			
			ASSERT(true == result);
			if (result) {
				TRACER_QUERYDEVICESTATUS(this->m_currShortAddress);
				this->setState(STATE_DEVICES_SEARCH);
			}	
		} break;
		case STATE_DEVICES_SEARCH: {			
			ASSERT((this->m_currShortAddress>=DALI_SHORTADDRESS_FIRST) && (this->m_currShortAddress<=DALI_SHORTADDRESS_LAST));
			// Blijven in deze toestand tot alle devices opgevraagd zijn...
			result = this->sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_currShortAddress);			
			ASSERT(true == result);
			if (result) {
				TRACER_QUERYDEVICESTATUS(this->m_currShortAddress);
			}
		} break;
		case STATE_SEARCHADDRESS: {					
			if (this->diff_lsb()) {
				const uint8_t data = this->m_searchAddress & 0xff;
				result = this->sendBroadcastSearchAddrL(this->getDaliDeviceType(),data);
				ASSERT(true==result);
				if (result) {					
					TRACER_SEARCHADDRESS(data, -1, -1);
					this->setState(STATE_SEARCHADDR_L);
				}
			}
			else if (this->diff_msb()) {
				const uint8_t data = (this->m_searchAddress & 0xff00) >> 8;
				result = this->sendBroadcastSearchAddrM(this->getDaliDeviceType(),data);
				ASSERT(true==result);
				if (result) {
					TRACER_SEARCHADDRESS(-1, data, -1);
					this->setState(STATE_SEARCHADDR_M);
				}
			}
			else if (this->diff_hsb()) {
				const uint8_t data = (this->m_searchAddress & 0xff0000) >> 16;
				result = this->sendBroadcastSearchAddrH(this->getDaliDeviceType(),data);
				ASSERT(true==result);
				if (result) {
					TRACER_SEARCHADDRESS(-1, -1, data);
					this->setState(STATE_SEARCHADDR_H);
				}
			}
			else { ASSERT(0); }
		} break;
		case STATE_SEARCHADDR_L: {			
			if (this->diff_msb()) {
				const uint8_t data = (this->m_searchAddress & 0xff00) >> 8;
				result = this->sendBroadcastSearchAddrM(this->getDaliDeviceType(),data);
				ASSERT(true==result);
				if (result) {
					TRACER_SEARCHADDRESS(-1, data, -1);
					this->setState(STATE_SEARCHADDR_M);
				}
			}
			else if (this->diff_hsb()) {
				const uint8_t data = (this->m_searchAddress & 0xff0000) >> 16;
				result = this->sendBroadcastSearchAddrH(this->getDaliDeviceType(),data);
				ASSERT(true==result);
				if (result) {
					TRACER_SEARCHADDRESS(-1, -1, data);
					this->setState(STATE_SEARCHADDR_H);
				}
			}
			else {
				if (this->m_assignShortAddress) {
					this->m_currShortAddress = this->getNextAvailableAddress();
					if ((this->m_currShortAddress < DALI_SHORTADDRESS_FIRST) || (this->m_currShortAddress > DALI_SHORTADDRESS_LAST)) {			
						this->setFatalErrorInfo(ERROR_CODE_COMMISSIONING_SHORTADDRESS_EXCEED_MAX, -1,"No more short addresses available");
						result = this->sendBroadcastTerminate(this->getDaliDeviceType());
						ASSERT(true == result);
						if (result) {
							this->setState(STATE_TERMINATE);
							TRACER("TX: sendBroadcastTerminate()\r\n");
						}					
					}
					else {
						result = this->sendProgramShortAddress(this->getDaliDeviceType(),this->m_currShortAddress);
						ASSERT(true == result);
						if (result) {
							TRACER_PROGRAMSHORTADDRESS(this->m_currShortAddress, this->m_searchAddress);	
							this->incrementCounterValue();
							this->getParent().onNotifyFsmAlgoritmeShortAddressAssigned(this->getDaliDeviceType(), this->m_currShortAddress);
							this->setState(STATE_ASSIGN);
						}
					}
				}
				else {			
					result = this->sendBroadcastCompare(this->getDaliDeviceType());
					ASSERT(true == result);
					if (result) {
						TRACER("TX: sendBroadcastCompare()\r\n");
						this->setState(STATE_COMPARE);					
					}
				}
			}
		} break;
		case STATE_SEARCHADDR_M: {			
			if (this->diff_hsb()) {
				const uint8_t data = (this->m_searchAddress & 0xff0000) >> 16;
				result = this->sendBroadcastSearchAddrH(this->getDaliDeviceType(),data);
				ASSERT(true==result);
				if (result) {
					TRACER_SEARCHADDRESS(-1, -1, data);
					this->setState(STATE_SEARCHADDR_H);
				}
			}
			else {
				if (this->m_assignShortAddress) {
					this->m_currShortAddress = this->getNextAvailableAddress();
					result = this->sendProgramShortAddress(this->getDaliDeviceType(),this->m_currShortAddress);
					ASSERT(true == result);
					if (result) {
						TRACER_PROGRAMSHORTADDRESS(this->m_currShortAddress, this->m_searchAddress);
						this->incrementCounterValue();
						this->getParent().onNotifyFsmAlgoritmeShortAddressAssigned(this->getDaliDeviceType(), this->m_currShortAddress);
						this->setState(STATE_ASSIGN);
					}
				}
				else {
					result = this->sendBroadcastCompare(this->getDaliDeviceType());
					ASSERT(true == result);
					if (result) {
						TRACER("TX: sendBroadcastCompare()\r\n");
						this->setState(STATE_COMPARE);						
					}
				}
			}
		} break;
		case STATE_SEARCHADDR_H: {
			if (this->m_assignShortAddress) {
				this->m_currShortAddress = this->getNextAvailableAddress();
				result = this->sendProgramShortAddress(this->getDaliDeviceType(),this->m_currShortAddress);
				ASSERT(true == result);
				if (result) {
					TRACER_PROGRAMSHORTADDRESS(this->m_currShortAddress, this->m_searchAddress);
					this->incrementCounterValue();
					this->getParent().onNotifyFsmAlgoritmeShortAddressAssigned(this->getDaliDeviceType(), this->m_currShortAddress);
					this->setState(STATE_ASSIGN);
				}
			}
			else {
				result = this->sendBroadcastCompare(this->getDaliDeviceType());
				ASSERT(true == result);
				if (result) {
					TRACER("TX: sendBroadcastCompare()\r\n");
					this->setState(STATE_COMPARE);					
				}
			}
		} break;
		case STATE_COMPARE: {			
			if (this->m_assignShortAddress) {
				this->m_currShortAddress = this->getNextAvailableAddress();
				if ((this->m_currShortAddress < DALI_SHORTADDRESS_FIRST) || (this->m_currShortAddress > DALI_SHORTADDRESS_LAST)) {
					this->setFatalErrorInfo(ERROR_CODE_COMMISSIONING_SHORTADDRESS_EXCEED_MAX, -1,"No more short addresses available");
					result = this->sendBroadcastTerminate(this->getDaliDeviceType());
					ASSERT(true == result);
					if (result) {
						this->setState(STATE_TERMINATE);
						TRACER("TX: sendBroadcastTerminate()\r\n");
					}					
				}
				else {														
					result = this->sendProgramShortAddress(this->getDaliDeviceType(),this->m_currShortAddress);
					ASSERT(true == result);
					if (result) {
						TRACER_PROGRAMSHORTADDRESS(this->m_currShortAddress,this->m_searchAddress);
						this->incrementCounterValue();
						this->getParent().onNotifyFsmAlgoritmeShortAddressAssigned(this->getDaliDeviceType(), this->m_currShortAddress);
						this->setState(STATE_ASSIGN);
					}
				}
			}
			else if (this->m_terminate) {			
				result = this->sendBroadcastTerminate(this->getDaliDeviceType());
				ASSERT(true == result);
				if (result) {
					this->setState(STATE_TERMINATE);
					TRACER("TX: sendBroadcastTerminate()\r\n");
				}				
			}
			else {
				ASSERT(0);
				result = false;
			}	
		} break;
		case STATE_ASSIGN: {		
			ASSERT(true == this->m_assignShortAddress);
			result = this->sendQueryShortAddress(this->getDaliDeviceType());
			ASSERT(true == result);
			if (result) {				
				TRACER("TX: sendQueryShortAddress()\r\n");
				this->setState(STATE_VERIFY_SHORTADDRESS);
			}
		} break;

		case STATE_VERIFY_SHORTADDRESS: {		
			ASSERT(true == this->m_assignShortAddress);
			// Reloaden voor de volgende search...			
			this->m_assignShortAddress = false;

			result = this->sendBroadcastWithdraw(this->getDaliDeviceType());
			ASSERT(true == result);
			if (result) {
				this->setState(STATE_WITHDRAWN);
				TRACER("TX: sendBroadcastWithdraw()\r\n");
			}
		} break;

		case STATE_WITHDRAWN: {			
			ASSERT(this->m_currShortAddress >= 0 && this->m_currShortAddress <= 63);			
			ASSERT(DALI_DEVICE_TYPE_CONTROLGEAR == this->getDaliDeviceType());
			result = this->getTransmitter().sendControlGearRecallMaxLevel(this->m_currShortAddress);
			if (result) {
				TRACER("TX: sendControlGearRecallMaxLevel()\r\n");
				this->setState(STATE_DEVICE_RECALL_MAX_LEVEL);				
			}
		} break;	
		case STATE_DEVICE_RECALL_MAX_LEVEL: {
			// Event, transitie naar een andere toestand in de onFsmEventTxDone()
			ASSERT(0);	
			result=false;
		} break;
		case STATE_TERMINATE: {
			// Event, transitie naar een andere toestand in de onFsmEventTxDone()
			ASSERT(0);
			result=false;
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	

		default: {
			ASSERT(0);
			result=false; 
		} break;
	}
	return result;
}

// Wanneer we een response krijgen op de Compare Query dan zijn er devices aanwezig 
// die een short address hebben lager dan het search address...
bool CAppDaliFsmHelperDaliDevicesAddress::handleResponseCompareState( ) {
	this->m_searchAddressPrevious = this->m_searchAddress;
	if (this->m_searchBusy) {
		this->m_searchAddress -= this->m_searchAddrAddSub;
		this->m_searchAddrAddSub >>= 1;
		if (0 == this->m_searchAddrAddSub) {
			this->m_searchBusy = false;					
		}
		this->setState(STATE_SEARCHADDRESS);
	}
	else {						
		this->m_assignShortAddress = true;
	}
	return true;
}

bool CAppDaliFsmHelperDaliDevicesAddress::goToNextSearchDeviceOrStateSearchAddress( ) {
	ASSERT(STATE_DEVICES_SEARCH == this->getState());
	if (++this->m_currShortAddress >= DALI_SHORTADDRESS_MAX) {
		if (DALI_DEVICE_TYPE_CONTROLGEAR == this->getDaliDeviceType()) {
			this->m_currShortAddress = DALI_SHORTADDRESS_FIRST;										
		}
		else {
			this->m_currShortAddress = DALI_SHORTADDRESS_LAST;
		}
		this->setState(STATE_SEARCHADDRESS);
		return true;
	}
	return false;
}

// Toestand: STATE_DEVICES_SEARCH
// - bezig met het opvragen of een device aanwezig is.
//	 adres bezet.
// Toestand: STATE_COMPARE
//  - Bezig met het zoeken naar een device via het SearchAddress/Compare.
//    Wanneer een device antwoord is zijn randomAddress >= searchAddress.
// Toestand: STATE_VERIFY_SHORTADDRESS
//  - Na het toekennen van een short address wordt er nog eens een verify uitgevoerd.
bool CAppDaliFsmHelperDaliDevicesAddress::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	this->dbgDumpContext("--> onFsmEventResponse()\r\n");

	const int state = this->getState();
	if (STATE_DEVICES_SEARCH == state) {
		ASSERT((this->m_currShortAddress>=DALI_SHORTADDRESS_FIRST) && (this->m_currShortAddress<=DALI_SHORTADDRESS_LAST));
		this->m_assignedAddr[this->m_currShortAddress] = 1;
		// Naar het volgende short address gaan om te zoeken, indien alle short addresses overlopen zijn gaan we naar de volgende toestand.
		this->goToNextSearchDeviceOrStateSearchAddress( );
		return true;
	}

	if (STATE_COMPARE == state) {			
		return this->handleResponseCompareState( );	
	}

	if (STATE_VERIFY_SHORTADDRESS == state) {
		int receivedShortAddress = 0xff;
		if (0xff != data) {
			if (DALI_DEVICE_TYPE_CONTROLGEAR == this->getDaliDeviceType()) {
				receivedShortAddress = (data >> 1);
			}
			else{
				receivedShortAddress = data;
			}
		}
		if (this->m_currShortAddress == receivedShortAddress) {
			return true;
		}
		CString strMsgErrorDetail;
		strMsgErrorDetail.Format("Query short address returned %d, short address should be %d. Possible the shortaddress was not assigned.",receivedShortAddress,this->m_currShortAddress);
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_COMMISSIONING_SHORTADDRESS_VERIFY, this->getDaliDeviceType(), this->getShortAddress(), strMsgErrorDetail);
		this->setErrorEntry(refContext, errorEntry);
		return true;
	}

	// Fatale fout.
	CString stringErrorDetail("Recieved Backward response frame event from the DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->m_currShortAddress,stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// Toestand: STATE_DEVICES_SEARCH
// - bezig met het opvragen of een device aanwezig is.
//	 Geen antwoord, dus het adres is vrij.
//
// Toestand: STATE_COMPARE
//  - Bezig met het zoeken naar een device via het SearchAddress/Compare.
//    Wanneer een device niet antwoord is zijn randomAddress < searchAddress.
//
// Toestand STATE_VERIFY_SHORTADDRESS
// - Na het toekennen van het adres nog eens een verify uitvoeren.
//   Geen response dus adres is niet toegekend.
//		Actie: Foutmelding toevoegen. 
//			   We gaan het algoritme laten doorgaan.
//
bool CAppDaliFsmHelperDaliDevicesAddress::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_DEVICES_SEARCH == state) {
		ASSERT((this->m_currShortAddress>=DALI_SHORTADDRESS_FIRST) && (this->m_currShortAddress<=DALI_SHORTADDRESS_LAST));
		this->m_assignedAddr[this->m_currShortAddress] = 0;
		// Naar het volgende short address gaan om te zoeken, indien alle short addresses overlopen zijn gaan we naar de volgende toestand.
		this->goToNextSearchDeviceOrStateSearchAddress( );
		return true;
	}

	if (STATE_VERIFY_SHORTADDRESS == state) {	
		CString stringError;
		stringError.Format("Busy assigning short addresses. Address A%d did not respond to a verify query. Possible the address was not assigned.",this->m_currShortAddress);
		CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->m_currShortAddress, stringError);
		this->setErrorEntry(refContext, errorEntry);	
		return true;
	}

	if (STATE_COMPARE == state) {
		this->m_searchAddressPrevious = this->m_searchAddress;
		if (this->m_searchBusy) {
			this->m_searchAddress += this->m_searchAddrAddSub;
			this->m_searchAddrAddSub >>= 1;
			if (0 == this->m_searchAddrAddSub) {
				this->m_searchBusy = false;					
			}	
			this->setState(STATE_SEARCHADDRESS);
		}
		else {
			this->m_searchAddress++;
			if (this->m_searchAddress >= DALI_SEARCHADDRESS_LAST) {
				this->m_terminate = true;			
			}		
			else{				
				this->m_assignShortAddress = true;
				this->setState(STATE_SEARCHADDRESS);
			}			
		}
		return true;
	}

	// Fatale fout.
	CString stringErrorDetail("Recieved Backward no response event from the DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->m_currShortAddress,stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// Toestand: STATE_COMPARE
// Moeten een fout antwoord op dezelfde manier verwerken als wnr we een geldig antwoord krijgen.
// Wanneer er meerdere antwoorden dan krijgen we een fout result.
//
// Toestand: STATE_DEVICES_SEARCH
// Wanneer we een verkeerd antwoord ontvangen zijn er mogelijk meerdere devices met hetzelfde short address 
// aanwezig.
//		Actie: Het adres als gebruikt markeren zodat het zeker geen tweede keer toegekend wordt.
//			   Fout entry aanmaken en toevoegen aan de fouten + doorgeven aan de parent zodat het device address
//			   in fout toestand staat.
//
// Toestand STATE_VERIFY_SHORTADDRESS
// Na het toekennen van het adres nog eens een verify uitvoeren.
// Error response wijst erop dat er meerdere devices geantwoord hebben.
//		Actie: Het adres als gebruikt markeren zodat het zeker geen tweede keer toegekend wordt.
//			   Fout entry aanmaken en toevoegen aan de fouten + doorgeven aan de parent zodat het device address
//			   in fout toestand staat.
//
bool CAppDaliFsmHelperDaliDevicesAddress::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_DEVICES_SEARCH == state) {		
		const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
		ASSERT((this->m_currShortAddress>=DALI_SHORTADDRESS_FIRST) && (this->m_currShortAddress<=DALI_SHORTADDRESS_LAST));

		// Het adres bezet zetten zodat het zeker niet nog eens toegekend wordt aan een ander device.	
		if ((this->m_currShortAddress>= DALI_SHORTADDRESS_FIRST) && (this->m_currShortAddress<=DALI_SHORTADDRESS_LAST)) {			
			this->m_assignedAddr[this->m_currShortAddress] = 1;
		}

		// Fout entry.
		CString strErrorDetail;
		strErrorDetail.Format("Busy assigning short addresses. Detected possible double address for device A%d while searching for free addresses.",this->m_currShortAddress);
		CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, daliDeviceType, this->m_currShortAddress, strErrorDetail);
		this->setErrorEntry(refContext, errorEntry);

		// Doorgeven aan de parent zodat de status van het device address in error toestand komt.
		this->getParent().onNotifyFsmAlgoritmeError(daliDeviceType, this->m_currShortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);

		// Naar het volgende short address gaan om te zoeken, indien alle short addresses overlopen zijn gaan we naar de volgende toestand.
		this->goToNextSearchDeviceOrStateSearchAddress( );
		return true;
	}

	if (STATE_VERIFY_SHORTADDRESS == state) {
		const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
		// Fout entry.
		CString strErrorDetail;
		strErrorDetail.Format("Busy assigning short addresses. Detected possible double address for device A%d while verifying the adress.",this->m_currShortAddress);
		CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, daliDeviceType, this->m_currShortAddress, strErrorDetail);
		this->setErrorEntry(refContext, errorEntry);

		// Doorgeven aan de parent zodat de status van het device address in error toestand komt.
		this->getParent().onNotifyFsmAlgoritmeError(daliDeviceType, this->m_currShortAddress, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
		return true;
	}

	if (STATE_COMPARE == state) {				
		return this->handleResponseCompareState( );
	}

	// Fatale fout.
	CString stringErrorDetail("Recieved Backward error frame event from the DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}


// Toestand: STATE_DEVICE_RECALL_MAX_LEVEL
// - Na het toekennen van een adres wordt het device aangeschakeld op zijn maximum level.
//   Bij het tx event done gaan we het algoritme laden om een nieuwe device te zoeken + addresseren.
bool CAppDaliFsmHelperDaliDevicesAddress::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	this->dbgDumpContext("--> onFsmEventTxDone()\r\n");

	const int state = this->getState();
	if (STATE_RANDOMIZE == state) {		
		return true;
	}

	if (STATE_TERMINATE == state) {		
		if (ERROR_CODE_NONE == this->getFatalErrorInfo().getErrorCode()) {
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES, -1, -1);
			this->setState(STATE_DONE);
		}
		else {
			this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES, -1, -1);
			this->setState(STATE_ERROR_FATAL);		// Na het uitvoeren van alle stappen.
		}
		return true;	
	}

	if ((STATE_DEVICE_RECALL_MAX_LEVEL == state) || 
		((STATE_WITHDRAWN == state && DALI_DEVICE_TYPE_INPUTCONTROL == this->getDaliDeviceType()))) {		
		// Herladen voor een nieuwe search.
		this->m_searchBusy = true;
		this->m_searchAddress = 0x800000;
		this->m_searchAddrAddSub = 0x400000;
		this->m_searchAddressPrevious = 0xFFFFFF;
		this->setState(STATE_SEARCHADDRESS);
		return true;
	}

	if (STATE_ASSIGN == state) {
		ASSERT(this->m_currShortAddress >= DALI_SHORTADDRESS_FIRST && this->m_currShortAddress<=DALI_SHORTADDRESS_LAST);
		if (this->m_currShortAddress >= DALI_SHORTADDRESS_FIRST && this->m_currShortAddress<=DALI_SHORTADDRESS_LAST) {
			this->m_assignedAddr[this->m_currShortAddress] = 1;
		}
		return true;
	}

	if ((STATE_INITIALISE == state) ||  (STATE_SEARCHADDR_L ==  state) || (STATE_SEARCHADDR_M ==  state) || 
		(STATE_SEARCHADDR_H == state) || (STATE_SEARCHADDRESS == state) || (STATE_WITHDRAWN == state)) {
		return true;
	} 

	// Fatale fout.
	CString stringErrorDetail("Recieved TX-Done event from the DALI controller but expected backward response frame.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}


// protected helper...
bool CAppDaliFsmHelperDaliDevicesAddress::diff_lsb() {
	return ((this->m_searchAddress & 0xFF) != (this->m_searchAddressPrevious & 0xFF));
}
bool CAppDaliFsmHelperDaliDevicesAddress::diff_msb() {
	return ((this->m_searchAddress & 0xFF00) != (this->m_searchAddressPrevious & 0xFF00));
}
bool CAppDaliFsmHelperDaliDevicesAddress::diff_hsb() {
	return ((this->m_searchAddress & 0xFF0000) != (this->m_searchAddressPrevious & 0xFF0000));
}
int CAppDaliFsmHelperDaliDevicesAddress::getNextAvailableAddress( ) {
	int retVal = -1;
	const DaliDeviceType_t daliDeviceType =  this->getDaliDeviceType();
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
		for (int i=DALI_SHORTADDRESS_FIRST;i<DALI_SHORTADDRESS_MAX;++i) {
			if (0 == this->m_assignedAddr[i]) {
				retVal = i;
				break;
			}
		}
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
		for (int i=DALI_SHORTADDRESS_LAST;i>=DALI_SHORTADDRESS_FIRST;--i) {
		if (0 == this->m_assignedAddr[i]) {
				retVal = i;
				break;
			}
		}	
	}
	return retVal;
}

CString CAppDaliFsmHelperDaliDevicesAddress::getStrState( ) const {
	const int state = this->getState();
	switch (state) {
		case STATE_READY: 
			return "STATE_READY";
		case STATE_DONE:
			return "STATE_DONE";
		case STATE_ERROR_FATAL:
			return "STATE_ERROR_FATAL";
		case STATE_CANCEL:
			return "STATE_CANCEL";
		case STATE_INITIALISE:
			return "STATE_INITIALISE";
		case STATE_RANDOMIZE:
			return "STATE_RANDOMIZE";
		case STATE_DEVICES_SEARCH:
			return "STATE_DEVICES_SEARCH";
		case STATE_SEARCHADDRESS:
			return "STATE_SEARCHADDRESS";
		case STATE_SEARCHADDR_L:
			return "STATE_SEARCHADDR_L";
		case STATE_SEARCHADDR_M:
			return "STATE_SEARCHADDR_M";
		case STATE_SEARCHADDR_H:
			return "STATE_SEARCHADDR_H";
		case STATE_COMPARE:
			return "STATE_COMPARE";
		case STATE_ASSIGN:
			return "STATE_ASSIGN";
		case STATE_VERIFY_SHORTADDRESS:
			return "STATE_VERIFY_SHORTADDRESS";
		case STATE_DEVICE_RECALL_MAX_LEVEL:
			return "STATE_DEVICE_RECALL_MAX_LEVEL";
		case STATE_WITHDRAWN:
			return "STATE_WITHDRAWN";
		case STATE_TERMINATE:
			return "STATE_TERMINATE";	
	}
	ASSERT(0);
	return "UNKNOWN";
}

void CAppDaliFsmHelperDaliDevicesAddress::dbgDumpContext(const CString& strHeader) {
	CString strAddresses;
	CString strState;
	CString strShortAddress;
	CString strVariables;

	strState.Format("CurrentState = %s\r\n",this->getStrState());
	strAddresses.Format("search Address=0x%08x, prev search address=0x%08x, add sub address=0x%06x\r\n", 
		this->m_searchAddress, this->m_searchAddressPrevious, this->m_searchAddrAddSub);
	strShortAddress.Format("Current shortAddress=%d\r\n",this->m_currShortAddress);
	strVariables.Format("search buys=%d, assign address=%d, terminate=%d\r\n", 
		this->m_searchBusy, this->m_assignShortAddress, this->m_terminate);

	if (!strHeader.IsEmpty()) {
		TRACER(strHeader);
	}
	TRACER(strState);
	TRACER(strAddresses);
	TRACER(strShortAddress);
	TRACER(strVariables);
}
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlGearErase::CAppDaliFsmHelperControlGearErase(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_currGroup(0) { }

bool CAppDaliFsmHelperControlGearErase::cancel() {	
	this->cancelTask();
	this->setState(STATE_CANCEL);	
	return true;
}

bool CAppDaliFsmHelperControlGearErase::start() {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;	
	}

	this->setState(STATE_READY);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->getFatalErrorInfo().clear();
	this->clearCounterValue();

	this->m_currGroup = DALI_CONTROLGEAR_GROUP_FIRST;	
	return this->next();
}


// Sedert V17.04: Niet alle (=oudere) conrol gear devices reageren op het Broadcast command 'DALI_CMD_SET_DTR0_SHORTADDRESS'.
// Nog eens naar alle device individueel het commando 'DALI_CMD_SET_DTR0_SHORTADDRESS' versturen.
// Nodig omdat niet alle devices het commando ondersteunen wanneer deze via een broadcast bericht verstuurd wordt.
//
// NOTE: Bij de devices die de broadcast commandos niet verwerken worden ook de erase group commandos niet verwerkt.
// Maar omdat er veel meer communicatie nodig is om voor alle devices de groepen te wissen wordt dit genegeerd.
// Bij deze devices zal de groep info niet gewist worden...

// acties.
bool CAppDaliFsmHelperControlGearErase::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ERASEDATA, -1, -1);
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(0xff);					// 0xff = search all unaddressed.
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll()\r\n");
				this->setState(STATE_INITIALISE);
			}
		} break;	
		case STATE_INITIALISE: {					
			result = this->getTransmitter().sendControlGearDTR0(0xff);										// MASK 
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll()\r\n");
				this->setState(STATE_SHORTADDRESS_DRT0);
			}			
		} break;			
		case STATE_SHORTADDRESS_DRT0: {
			this->setShortAddress(-1);
			result = this->getTransmitter().sendControlSetShortAddress(0xff);								// Broadcast.
			if (result) {
				TRACER("TX: sendControlSetShortAddress()\r\n");
				this->setState(STATE_SHORTADDRESS);
			}
		} break;		
		case STATE_SHORTADDRESS: {
			const int shortAddress = this->getShortAddress();
			ASSERT((shortAddress >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (shortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST));
			result = this->getTransmitter().sendControlSetShortAddress(shortAddress);
			if (result) {
				CString msg;
				msg.Format("DALI device %d: Erase control gear device address", shortAddress);
				this->getParent().onNotifyFsmAlgoritmeMsgInfo(msg);
				TRACER("TX: sendControlSetShortAddress()\r\n");
			}
		} break;
		case STATE_REMOVE_FROM_GROUP: {
			ASSERT(this->m_currGroup >= DALI_CONTROLGEAR_GROUP_FIRST && this->m_currGroup <= DALI_CONTROLGEAR_GROUP_LAST);
			result = this->getTransmitter().sendControlGearRemoveFromgroup(0xff, this->m_currGroup);			// Broadcast.
			// Blijven in deze toestand.
			// Transitie naar een andere toestand gebeurd bij het ontvangen van een tx done event.
		} break;
		case STATE_REMOVE_FROM_GROUP_DONE: {
			result = this->getTransmitter().sendControlGearRecallMinLevel(0xff);
			if (result) {
				TRACER("TX: sendControlGearRecallMinLevel()\r\n");
				this->setState(STATE_DEVICE_RECALL_MIN_LEVEL);
			}
		} break;
		case STATE_DEVICE_RECALL_MIN_LEVEL: {
			result = this->getTransmitter().sendControlgearBroadcastTerminate();
			if (result) {
				this->setState(STATE_TERMINATE);
				TRACER("TX: sendControlgearBroadcastTerminate()\r\n");
			}
		} break;				
		case STATE_TERMINATE: {
			ASSERT(0);
		} break;
									   
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;
		default: {
			ASSERT(0);
			result = false;
		} break;
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperControlGearErase::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	ASSERT(0);
	return false;
}

bool CAppDaliFsmHelperControlGearErase::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	ASSERT(0);
	return false;
}

bool CAppDaliFsmHelperControlGearErase::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();

	if (STATE_SHORTADDRESS == state) {
		int shortAddress = this->getShortAddress();
		if (-1 == shortAddress) {
			// Na het versturen van de broadcast nog eens alle devices afzonderlijk. Starten met het eerste control gear address.
			this->setShortAddress(DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST);
		}
		else {
			shortAddress += 1;
			if (shortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
				// Volgend control gear device.
				this->setShortAddress(shortAddress);
			}
			else {
				// Wanneer het adres van alle control gear devices gewist is de groepen van de devices wissen.
				this->setShortAddress(-1);
				this->m_currGroup = DALI_CONTROLGEAR_GROUP_FIRST;
				this->setState(STATE_REMOVE_FROM_GROUP);
			}
		}
		return true;
	}

	if (STATE_INITIALISE == state ||
		STATE_SHORTADDRESS_DRT0 == state ||
		STATE_REMOVE_FROM_GROUP_DONE == state ||
		STATE_DEVICE_RECALL_MIN_LEVEL == state) {
		return true;
	}

	if (STATE_REMOVE_FROM_GROUP == state) {
		if (++this->m_currGroup >= DALI_CONTROLGEAR_GROUP_MAX) {
			this->setState(STATE_REMOVE_FROM_GROUP_DONE);
		}	
		return true;
	}
	if (STATE_TERMINATE == state) {
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ERASEDATA, -1, -1);
		this->setState(STATE_DONE);
		return true;
	}
	ASSERT(0);
	return false;
}

bool CAppDaliFsmHelperControlGearErase::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	ASSERT(0);
	return false;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlInputDeviceErase::CAppDaliFsmHelperControlInputDeviceErase(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter) { }

bool CAppDaliFsmHelperControlInputDeviceErase::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperControlInputDeviceErase::start() {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->clearCounterValue();	
	return this->next();
}

// acties.
bool CAppDaliFsmHelperControlInputDeviceErase::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ERASEDATA, -1, -1);
			result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(0xFF);
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastDTR0(0xff)\r\n");
				this->setState(STATE_SHORTADDRESS_DRT0);
			}
		} break;
		case STATE_SHORTADDRESS_DRT0: {
			result = this->getTransmitter().sendControlInputDevicesBroadcastSetShortAddressDTR0();						
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastSetShortAddressDTR0()\r\n");
				this->setState(STATE_SHORTADDRESS);
			}
		} break;

		case STATE_SHORTADDRESS: {		
			result = this->getTransmitter().sendControlInputDevicesBroadcastWriteDTR2_DTR1(0xff, 0xff);			// Broadcast.
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastMemoryLocation()\r\n");
				this->setState(STATE_DTR1_DTR2);
			}
		} break;
		case STATE_DTR1_DTR2: {		
			result = this->getTransmitter().sendControlInputDevicesBroadcastRemoveFromDeviceGroups0_15( );		// Broadcast.
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastRemoveFromDeviceGroups0_15()\r\n");
				this->setState(STATE_REMOVE_FROM_GROUP_0_15);
			}
		} break;
		case STATE_REMOVE_FROM_GROUP_0_15: {
			result = this->getTransmitter().sendControlInputDevicesBroadcastRemoveFromDeviceGroups16_31();		// Broadcast.
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastRemoveFromDeviceGroups16_31()\r\n");
				this->setState(STATE_REMOVE_FROM_GROUP_16_31);
			}
		} break;
		case STATE_REMOVE_FROM_GROUP_16_31: {
			ASSERT(0);
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;

		default: {
			ASSERT(0);
			result = false;
		} break;
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperControlInputDeviceErase::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), "Received backward response.");		
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceErase::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), "Received no response.");
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceErase::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_SHORTADDRESS_DRT0 == state ||
		STATE_SHORTADDRESS == state ||	
		STATE_DTR1_DTR2 == state ||
		STATE_REMOVE_FROM_GROUP_0_15 == state) {
		return true;
	}
	if (STATE_REMOVE_FROM_GROUP_16_31 == state) {	
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_ERASEDATA, -1, -1);
		this->setState(STATE_DONE);
		return true;		
	}	
	ASSERT(0);
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceErase::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), "Received a frame error.");
	this->setState(STATE_ERROR_FATAL);				// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::CAppDaliFsmHelperControlInputDeviceInstanceAutoParams(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_listDevices(), m_currListPosition(NULL), m_numberOfInstances(0), m_currentInstance(0), m_instanceType(-1), 
	m_instanceEnabled(false), m_foundDevice(false), m_cfgAutoParams(false), m_cfgDisableInstance(false), m_deviceDetectedLightSensor(false) { }

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::setListShortAddresses(CList<int>& src) {
	if (src.IsEmpty()) {
		ASSERT(0);
		return false;
	}

	this->m_listDevices.RemoveAll();

	for(POSITION pos = src.GetHeadPosition(); NULL != pos;) {
		const int shortAddress = src.GetNext(pos);
		if ((shortAddress < DALI_SHORTADDRESS_FIRST) || (shortAddress > DALI_SHORTADDRESS_LAST)) {
			ASSERT(0);
			return false;
		}
		this->m_listDevices.AddTail(shortAddress);
	}
	return true;
}

// NOTE-MFC_CLIST
// Moeten eerst de Positie goed zetten via GetHeadPosition().
// Wanneer de positie op het eerste element staat moeten we het eerste element ophalen via GetNext()
// De functie GetNext() haalt het element waar POS naar verwijst en verhoogt POS.
// Voor het ophalen van een volgend element controleren of POS != NULL
bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::start(CList<int>& listDevices) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
	this->m_currListPosition = NULL;
	if (!this->setListShortAddresses(listDevices)) {
		return false;
	}

	// Eerst short address ophalen van de lijst.
	this->m_currListPosition = this->m_listDevices.GetHeadPosition();
	if (NULL == this->m_currListPosition) {
		ASSERT(NULL != this->m_currListPosition);
		return false;
	}
		
	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(listDevices.GetNext(m_currListPosition));		// zie NOTE-MFC_CLIST
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->clearCounterValue();

	// Eigen members.	
	this->m_numberOfInstances = 0;
	this->m_currentInstance = 0;
	this->m_instanceType = -1;
	this->m_instanceEnabled = false;
	this->m_foundDevice = false;
	this->m_cfgAutoParams = false;
	this->m_cfgDisableInstance = false;
	this->m_deviceDetectedLightSensor = false;
	ENSURE(this->verifyShortAddress());	
	return this->next();
}

// acties.
bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), -1, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS, -1, -1);
			result = this->getTransmitter().sendQueryInputControlDeviceStatus(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryInputControlDeviceStatus( )\r\n");
				this->setState(STATE_SEARCH_DEVICE);
			}
		} break;
		case STATE_SEARCH_DEVICE: {
			// Oppassen: Wanneer we een device gevonden hebben gaan we naar een andere toestand. (zie m_foundDevice).
			// Oppassen: Wanneer alle devices gevonden zijn gaan we naar de terminate.
			ASSERT(true==this->verifyShortAddress());	
			if (this->m_foundDevice) {
				this->m_numberOfInstances = 0;
				this->m_currentInstance = 0;	
				this->m_deviceDetectedLightSensor = false;
				result = this->getTransmitter().sendQueryControlInputDeviceNumberOfInstances(this->getShortAddress());	
				ASSERT(result);
				if (result) {
					this->m_foundDevice = false;
					TRACER("TX: sendQueryControlInputDeviceNumberOfInstances( )\r\n");
					this->setState(STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES);		
				}
			}
			else {
				result = this->getTransmitter().sendQueryInputControlDeviceStatus(this->getShortAddress());
				ASSERT(result);
				if (result) {
					TRACER("TX: sendQueryInputControlDeviceStatus( )\r\n");
					this->setState(STATE_SEARCH_DEVICE);			// blijven hier, transitie in een andere event.
				}								  
			}
		} break;
		
		case STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES: {
			ASSERT(true==this->verifyShortAddress());	
			if (this->m_numberOfInstances > 0) {
				result = this->getTransmitter().sendControlInputDeviceApplicationControllerDisable(this->getShortAddress());
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlInputDeviceApplicationControllerDisable( )\r\n");
					this->setState(STATE_SET_DEVICE_APPLICATION_CONTROLLER_DISABLE);
				}
			}
			else {			
				ASSERT(true==this->verifyControlDeviceInstanceNumber());
				result = this->getTransmitter().sendQueryControlInputDeviceInstanceType(this->getShortAddress(),this->m_currentInstance);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendQueryControlInputDeviceInstanceType( )\r\n");				
					this->m_cfgAutoParams = false;
					this->m_cfgDisableInstance = false;
					this->setState(STATE_QUERY_DEVICE_INSTANCE_TYPE);
				}
			}
		} break;

		case STATE_SET_DEVICE_APPLICATION_CONTROLLER_DISABLE: {
			ASSERT(true==this->verifyShortAddress());
			ASSERT(true==this->verifyControlDeviceInstanceNumber());
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceType(this->getShortAddress(),this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlInputDeviceInstanceType( )\r\n");
				this->m_cfgAutoParams = false;
				this->m_cfgDisableInstance = false;
				this->setState(STATE_QUERY_DEVICE_INSTANCE_TYPE);
			}															 															  
		} break;

		case STATE_QUERY_DEVICE_INSTANCE_TYPE: {
			ASSERT(true==this->verifyShortAddress());	
			ASSERT(true==this->verifyControlDeviceInstanceNumber());

			result = this->getTransmitter().sendQueryControlInputDeviceInstanceEnabled(this->getShortAddress(),this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlInputDeviceInstanceEnabled( )\r\n");
				this->setState(STATE_QUERY_DEVICE_INSTANCE_ENABLED);
			}
		} break;

		case STATE_QUERY_DEVICE_INSTANCE_ENABLED: {
			ASSERT(true==this->verifyShortAddress());
			ASSERT(true==this->verifyControlDeviceInstanceNumber());
			
			if (this->m_cfgAutoParams) {		
				if (this->m_cfgDisableInstance) {
					result = this->getTransmitter().sendControlInputDeviceInstanceDisableInstance(this->getShortAddress(),this->m_currentInstance);
					ASSERT(result);
					if (result) {
						TRACER("TX: sendControlInputDeviceInstanceDisableInstance( )\r\n");
						this->setState(STATE_SET_DEVICE_INSTANCE_DISABLE);
					}	
				}
				else {
					result = this->getTransmitter().sendControlInputDeviceInstanceEnableInstance(this->getShortAddress(),this->m_currentInstance);
					ASSERT(result);
					if (result) {
						TRACER("TX: sendControlInputDeviceInstanceEnableInstance( )\r\n");
						this->setState(STATE_SET_DEVICE_INSTANCE_ENABLE);
					}
				}
			}
			else if (this->m_cfgDisableInstance) {
				result = this->getTransmitter().sendControlInputDeviceInstanceDisableInstance(this->getShortAddress(),this->m_currentInstance);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlInputDeviceInstanceDisableInstance( )\r\n");
					this->setState(STATE_SET_DEVICE_INSTANCE_DISABLE);
				}			
			}
			else {						
				// Oppassen:
				// De instance wordt verhoogd bij het ontvangen van de backward response...
				result = this->getTransmitter().sendQueryControlInputDeviceInstanceType(this->getShortAddress(),this->m_currentInstance);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendQueryControlInputDeviceInstanceType( )\r\n");
					this->m_cfgAutoParams = false;
					this->m_cfgDisableInstance = false;
					this->setState(STATE_QUERY_DEVICE_INSTANCE_TYPE);
				}
			}								  
		} break;

		case STATE_SET_DEVICE_INSTANCE_DISABLE: {
			if (this->m_cfgAutoParams) {
				// Zelfde code als de toestand 'STATE_SET_DEVICE_INSTANCE_ENABLE'
				// Starten met de automatische configuratie.
				ASSERT(true==this->verifyShortAddress());	
				ASSERT(true==this->verifyControlDeviceInstanceNumber());	
				result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(this->getCfgEventAddressingScheme(this->m_instanceType));
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
					this->setState(STATE_SET_DEVICE_INSTANCE_SCHEME);
				}			
			}
			else {
				// Indien er geen automatische config nodig is.
				// Oppassen: De instance wordt verhoogd bij het ontvangen van de backward response...
				result = this->getTransmitter().sendQueryControlInputDeviceInstanceType(this->getShortAddress(),this->m_currentInstance);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendQueryControlInputDeviceInstanceType( )\r\n");
					this->m_cfgAutoParams = false;
					this->m_cfgDisableInstance = false;
					this->setState(STATE_QUERY_DEVICE_INSTANCE_TYPE);
				}							
			}
		} break;

		case STATE_SET_DEVICE_INSTANCE_ENABLE: {
			ASSERT(true==this->verifyShortAddress());	
			ASSERT(true==this->verifyControlDeviceInstanceNumber());	
			ASSERT(true==this->m_cfgAutoParams);
			result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(this->getCfgEventAddressingScheme(this->m_instanceType));
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
				this->setState(STATE_SET_DEVICE_INSTANCE_SCHEME);
			}						   											   
		} break;

		case STATE_SET_DEVICE_INSTANCE_SCHEME: {
			ASSERT(true==this->verifyShortAddress());	
			ASSERT(true==this->verifyControlDeviceInstanceNumber());				   

			result = this->getTransmitter().sendControlInputDeviceInstanceEventSchemeDTR0(this->getShortAddress(),this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDeviceInstanceEventSchemeDTR0( )\r\n");
				this->setState(STATE_SET_DEVICE_INSTANCE_EVENTFILTER_DTR0);
			}											   
		} break;

		case STATE_SET_DEVICE_INSTANCE_EVENTFILTER_DTR0: {
			ASSERT(true==this->verifyShortAddress());
			ASSERT(true==this->verifyControlDeviceInstanceNumber());
			ASSERT(true==this->verifyControlDeviceInstanceType());	
		
			result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(this->getCfgEventFilter(this->m_instanceType));
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
				this->setState(STATE_SET_DEVICE_INSTANCE_EVENTFILTER);
			}											   
		} break;
		
		case STATE_SET_DEVICE_INSTANCE_EVENTFILTER: {
			ASSERT(true==this->verifyShortAddress());
			ASSERT(true==this->verifyControlDeviceInstanceNumber());
			ASSERT(true==this->verifyControlDeviceInstanceType());	

			result = this->getTransmitter().sendControlInputDeviceInstanceEventFilterDTR0DTR1DTR2(this->getShortAddress(),this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDeviceInstanceEventFilterDTR0DTR1DTR2( )\r\n");				
				this->setState(STATE_SET_DEVICE_INSTANCE_EVENTFILTER_WAIT);	// Blijven in deze toestand - transitie bij het ontvangen van een ACK.
			}											   
		} break;

		// Oppassen:	
		// Bij de gewone pushbutton komen we nooit in deze toestand. (Zie de ASSERT failures)
		// 
		// Onderstaande toestanden zijn specifiek voor de DALI Occupancy detector.
		// We gaan standaard de Report timer interval plaatsen.
		// Afhankelijk van de 'counter' variabelen gaan we deze op andere timer intervals plaatsen tussen 33 en 50 seconden.	
		case STATE_SET_DEVICE_INSTANCE_EVENTFILTER_WAIT: {		
			ASSERT(DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == this->m_instanceType);
			if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == this->m_instanceType) {				
				enum { MAX_VALUES = 20 };													  				
				//											   0   1  2    3   4   5   6   7   8  9   10  11  12  13  14  15  16  17  18  19 
 				static const int TIMER_INTERVAL_VALUES_SEC[MAX_VALUES] = { 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33 };			

				static int counter = 0;
				const int timerValue = TIMER_INTERVAL_VALUES_SEC[counter%MAX_VALUES];
				result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(timerValue);
				ASSERT(result);
				if (result) {
					counter++;	
					TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
					this->setState(STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER);
				}							
			}
			else { ASSERT(0); }
		} break;

		case STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER: {
			ASSERT(DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == this->m_instanceType);											  
			result = this->getTransmitter().sendControlInputDeviceInstanceOccupancyReportTimerInterval(this->getShortAddress(),this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDeviceInstanceOccupancyReportTimerInterval( )\r\n");				
				this->setState(STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER_WAIT);	// Blijven in deze toestand - transitie bij het ontvangen van een ACK.
			}																  
		} break;

		case STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER_WAIT: {
			ASSERT(0);
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;

		default: {
			ASSERT(0);
			result = false;
		} break;
	}
	return result;
}

// - Pushbutton filter
//	- bit2: Short press
//	- bit4: Long press start.
//	- bit6: long press stop.
// - Occupancy filter.
//  - bit0: occupied
//  - bit1: vacant
//  - bit2: Repeat
//  - bit3: Movement
//  - bit4: No movement
// - Abs input controldevice
//	- bit0: Position
int CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::getCfgEventFilter(int deviceInstanceType) {	
	switch(deviceInstanceType) {
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON: {
			return DALI_CONTROLDEVICE_PUSHBUTTON_DEFAULT_EVENTFILTER;
		} break;
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY: {						  																  
			return DALI_CONTROLDEVICE_DETECTOR_DEFAULT_EVENTFILTER;
	    } break;
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT: {
			return DALI_CONTROLDEVICE_ABSINPUT_DEFAULT_EVENTFILTER;												
		} break;
		default: {
			ASSERT(0);
			return 0xff; 
		} break;
	}
	return 0xff;
}

int CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::getCfgEventAddressingScheme(int deviceInstanceType) {
	return DALI_CONTROLDEVICE_INSTANCE_ADDRESSSCHEME_DEFAULT;		// Device + instance addressing.
}

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::verifyShortAddress( ) {
	return ((this->getShortAddress() >= DALI_SHORTADDRESS_FIRST) && (this->getShortAddress() <= DALI_SHORTADDRESS_LAST));
}

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::verifyControlDeviceInstanceNumber( ) {
	return ((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST) && 
			(this->m_currentInstance < this->m_numberOfInstances));
			
}

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::verifyControlDeviceInstanceType( ) {
	return ((DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON == this->m_instanceType) ||
		    (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == this->m_instanceType) ||
			(DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT ==  this->m_instanceType));
}

// Events van de FSM...
bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	const int state = this->getState();
	if (STATE_SEARCH_DEVICE == state) {
		this->m_foundDevice = true;
		return true;
	}
	if (STATE_QUERY_DEVICE_INSTANCE_ENABLED == state) {
		this->m_instanceEnabled = true;
		return true;
	}
	if (STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES == state) {

		// Wanneer we een ongeldige waarde ontvangen.
		// Foutmelding + Aantal instances op 0 plaatsen.
		if (data >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX) {
			CString stringError;
			stringError.Format("Device A%d² responded with a wrong value in the backward response on Query Number of instances. Defaulting to 0 instances and no auto configure.",  this->getShortAddress());
			const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_WRONGRESPONSE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
			this->setErrorEntry(refContext, errorEntry);
			data = 0;		// Op 0 plaatsen zodat er geen automatische config gebeurd.
		}

		// Wanneer het device geen instances heeft dan naar hte volgende device gaan.
		// NOTE: de helper functie gaat niet naar een volgende instantie maar naar een volgend device...
		if (0 == data) {
			this->setState(this->helper_gotoNextInstance());
			return true;
		}
		this->m_numberOfInstances = data;
		return true;
	}

	// bepalen of er automatische config moet gebeuren.
	// sedert V16.42:
	// Indien het input device een light sensor bevat dan gaan we de push button instances 
	// van dit device disablen.
	if (STATE_QUERY_DEVICE_INSTANCE_TYPE == state) {
		this->m_instanceType = data;
		if (data == DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON) { 			
			this->m_cfgDisableInstance = this->m_deviceDetectedLightSensor;		// sedert V16.42
			this->m_cfgAutoParams = true;
		}
		else if (data == DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY) {
			this->m_cfgDisableInstance = false;
			this->m_cfgAutoParams = true;
		}
		else if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_LIGHT_SENSOR == data) {
			this->m_cfgAutoParams = false;
			this->m_cfgDisableInstance = true;
			this->m_deviceDetectedLightSensor = true;							// sedert V16.42
		}
		else if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT == data) {
			this->m_cfgDisableInstance = false;
			this->m_cfgAutoParams = true;
		}
		else {			
			this->setState(helper_gotoNextInstance());	
			this->m_cfgAutoParams = false;			
			this->m_cfgDisableInstance = false;
		}
		return true;
	}
	
	// Fatale fout.
	CString stringErrorDetail("Recieved Backward response event from the DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode = ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_SEARCH_DEVICE == state) {
		this->setState(this->helper_gotoNextDevice( ));
		this->m_foundDevice = false;
		return true;
	}

	if (STATE_QUERY_DEVICE_INSTANCE_ENABLED == state) {
		this->m_instanceEnabled = false;
		return true;
	}

	if (STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES == state) {	
		// Error.
		CString stringError;
		stringError.Format("Device A%d² responded with no backward response on Query Number of instances. Defaulting to 0 instances and no auto configure.",  this->getShortAddress());
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
		this->setErrorEntry(refContext, errorEntry);
	
		// Properties + naar de volgende toestand.
		this->m_numberOfInstances = 0;
		this->setState(this->helper_gotoNextInstance());

		// Melden aan de FSM parent.
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS, ERROR_CODE_PROPERTY_NORESPONSE, -1);
		return true;
	}

	if (STATE_QUERY_DEVICE_INSTANCE_TYPE == state) {	

		// Error.
		CString stringError;
		stringError.Format("Device A%d² responded with no backward response on Query instance type. Therefore the instance type is unknown and cannot be configured",  this->getShortAddress());
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_PROPERTY_NORESPONSE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
		this->setErrorEntry(refContext, errorEntry);

		// Properties invullen default.
		this->m_instanceType = -1;			
		this->setState(helper_gotoNextInstance());	
		this->m_cfgAutoParams = false;			
		this->m_cfgDisableInstance = false;

		// Melden aan de FSM parent.
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS, ERROR_CODE_PROPERTY_NORESPONSE, -1);
		return true;
	}

	// Fatale fout.
	CString stringErrorDetail("Recieved Backward no response event from the DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode = ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_SEARCH_DEVICE == state) {
		CString stringError;
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
		this->setErrorEntry(refContext, errorEntry);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
		return true;
	}	

	if (STATE_QUERY_DEVICE_INSTANCE_ENABLED == state) {	
		CString stringError;
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
		this->setErrorEntry(refContext, errorEntry);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
		return true;
	}

	if (STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES == state) {	

		// Error.
		CString stringError;
		stringError.Format("Device A%d² responded with a backward response frame error on Query Number of instances. Defaulting to 0 instances and no auto configure.",  this->getShortAddress());
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
		this->setErrorEntry(refContext, errorEntry);
	
		// Properties.
		this->m_numberOfInstances = 0;
		this->setState(this->helper_gotoNextInstance());

		// Melden aan de FSM parent.
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
		return true;
	}

	if (STATE_QUERY_DEVICE_INSTANCE_TYPE == state) {
		
		// Error.
		CString stringError;
		stringError.Format("Device A%d² responded with a backward response frame error on Query instance type. Therefore the instance type is unknown and cannot be configured",  this->getShortAddress());
		const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getDaliDeviceType(), this->getShortAddress(), stringError);
		this->setErrorEntry(refContext, errorEntry);

		// Properties invullen default.
		this->m_instanceType = -1;			
		this->setState(helper_gotoNextInstance());	
		this->m_cfgAutoParams = false;			
		this->m_cfgDisableInstance = false;

		// Melden aan de FSM parent.
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS, ERROR_CODE_SHORTADDRESS_DOUBLE, -1);
		return true;
	}
		
	// Fatale fout.
	CString stringErrorDetail("Recieved Backward frame error event from the DALI controller but expected TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode = ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if ((STATE_SET_DEVICE_INSTANCE_SCHEME_DTR0 == state) || (STATE_SET_DEVICE_INSTANCE_EVENTFILTER_DTR0 == state)){
		return true;
	}
	if ((STATE_SET_DEVICE_INSTANCE_SCHEME == state) || (STATE_SET_DEVICE_INSTANCE_ENABLE == state) || 
		(STATE_SET_DEVICE_INSTANCE_EVENTFILTER==state)) {
		return true;
	}

	if (STATE_SET_DEVICE_APPLICATION_CONTROLLER_DISABLE == state) {
		return true;
	}

#if(0)
	// Speciale toestand waar er gewacht wordt op een ACK.
	if (STATE_SET_DEVICE_INSTANCE_EVENTFILTER_WAIT == this->m_state) {		
		this->getParent().onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(this->getShortAddress(),this->m_currentInstance,this->m_instanceType);
		this->m_state = helper_gotoNextInstance();				
		return true;
	}
#endif 


	// Speciale toestand waar er gewacht wordt op een ACK.
	if (STATE_SET_DEVICE_INSTANCE_EVENTFILTER_WAIT == state) {		
		if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == this->m_instanceType) {
			// Bij de Occupancy detector moeten we nog de Report timer verhogen naar 50 sec.
			// Standaard staat deze ingesteld op 20 sec.
			return true;				
		}
		else {
			this->getParent().onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(this->getShortAddress(),this->m_currentInstance,this->m_instanceType);
			this->setState(helper_gotoNextInstance());				
			return true;
		}
	}
	
/*
	// Na het automatisch uitschakelen van de instance naar de volgend gaan.
	if (STATE_SET_DEVICE_INSTANCE_DISABLE == state) {
		ASSERT(this->m_cfgDisableInstance == true);
		this->setState(helper_gotoNextInstance());	
		this->m_cfgAutoParams = false;			
		this->m_cfgDisableInstance = false;
		return true;
	}
*/

	// Na het automatisch uitschakelen van de instance.
	// Indien er geen auto configuratie nodig is dan naar de volgende gaan,
	// anders gewoon verder doen...
	if (STATE_SET_DEVICE_INSTANCE_DISABLE == state) {
		ASSERT(this->m_cfgDisableInstance == true);
		if (false == this->m_cfgAutoParams) {
			this->setState(helper_gotoNextInstance());	
			this->m_cfgAutoParams = false;			
			this->m_cfgDisableInstance = false;
		}
		return true;
	}


	// Extra toestanden voor de DALI Occupancy detector.
	if ((STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER_DTR0 == state) || (STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER == state)) {
		INVARIANT(DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == this->m_instanceType);
		return true;
	}	
	if (STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER_WAIT == state) {
		INVARIANT(DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == this->m_instanceType);
		this->getParent().onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(this->getShortAddress(),this->m_currentInstance,this->m_instanceType);
		this->setState(helper_gotoNextInstance());				
		return true;
	}

	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, -1, "Expected a backward response but recieved a tx-done event for the DALI controller");
	this->setState(STATE_ERROR_FATAL);	// ErrorInfo=ERROR_CODE_WRONG_EVENT
	return false;
}

// Helper functie.
CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::State_t CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::helper_gotoNextInstance() {
	if (++this->m_currentInstance < this->m_numberOfInstances) {
		return STATE_SET_DEVICE_APPLICATION_CONTROLLER_DISABLE;	//  STATE_QUERY_DEVICE_INSTANCE_TYPE;
	}
	if (NULL == this->m_currListPosition) {		// zie NOTE-MFC_CLIST
		return STATE_DONE;
	}
	const int shortAddress = this->m_listDevices.GetNext(this->m_currListPosition);
	this->m_foundDevice = false;
	this->setShortAddress(shortAddress);
	ENSURE(this->verifyShortAddress());	
	return STATE_SEARCH_DEVICE;
}

CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::State_t CAppDaliFsmHelperControlInputDeviceInstanceAutoParams::helper_gotoNextDevice() {
	REQUIRE(STATE_SEARCH_DEVICE == this->getState());
	if (NULL == this->m_currListPosition) {		// zie NOTE-MFC_CLIST
		return STATE_DONE;
	}
	const int shortAddress = this->m_listDevices.GetNext(this->m_currListPosition);	
	this->setShortAddress(shortAddress);
	ENSURE(this->verifyShortAddress());	
	return STATE_SEARCH_DEVICE;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
/*
	RX :2024-10-16 14:41:24.891:  a500 - Initialise (all) (address=not used)
	RX :2024-10-16 14:41:24.917:  a500 - Initialise (all) (address=not used)
	RX :2024-10-16 14:41:24.955:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
	RX :2024-10-16 14:41:24.987:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
	RX :2024-10-16 14:41:25.016:  a3ff - DTR0=255 (0xff) (address=not used)
	RX :2024-10-16 14:41:25.043:  1f80 - Set shortaddress (DTR0) D=15
	RX :2024-10-16 14:41:25.070:  1f80 - Set shortaddress (DTR0) D=15
	RX :2024-10-16 14:41:25.200:  a100 - Terminate (address=not used)
	RX :2024-10-16 14:41:25.237:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)
	RX :2024-10-16 14:41:25.270:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)
*/
CAppDaliFsmHelperAssingShortAddress::CAppDaliFsmHelperAssingShortAddress(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) : 
	Base(refParent, refTransmitter), m_task(TASK_UNKNOWN), m_newShortAddress(-1), m_stateSuccess(false), m_verifyCounter(0) {  }

bool CAppDaliFsmHelperAssingShortAddress::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	this->m_task = TASK_UNKNOWN;
	return true;
}		

bool CAppDaliFsmHelperAssingShortAddress::start(DaliDeviceType_t deviceType,int shortAddress,int newShortAddress) {
	REQUIRE(((newShortAddress >= DALI_SHORTADDRESS_FIRST) && (newShortAddress<=DALI_SHORTADDRESS_LAST)) || (newShortAddress == 255));
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
		
	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(deviceType);
	this->clearCounterValue();
	
	// Eigen members.	
	if (newShortAddress >= DALI_SHORTADDRESS_FIRST && newShortAddress<=DALI_SHORTADDRESS_LAST) {
		this->m_task = TASK_ASSIGN_NEW_ADDRESS;
	}
	else {
		this->m_task = TASK_ERASE_ADDRESS;
	}
	this->m_verifyCounter = 0;
	this->m_newShortAddress = newShortAddress;
	this->m_stateSuccess = false;	
	// ENSURE(this->verifyShortAddress());			<-- TODO
	return this->next();
}


// acties.
bool CAppDaliFsmHelperAssingShortAddress::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
			IAppDaliFsm_HelperFsmEvents::AlgoritmeName_t name = IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESS_ASSIGN;
			if (TASK_ERASE_ADDRESS == this->m_task) {
				name = IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESS_ERASE;
			}
			this->getParent().onNotifyFsmAlgoritmeStarted(daliDeviceType, this->getShortAddress(), name, this->m_newShortAddress, -1);
			// if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
			static const int dataByte = 0x00;	// Initialise (all)
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(dataByte);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll( )\r\n");
				this->setState(STATE_INITIALISE);
			}
/*
			}
			else {
				ASSERT(0);		// TODO
				return false;
			}
*/
		} break; /* STATE_READY */

		case STATE_INITIALISE: {
			result = this->getTransmitter().startQuiescentMode( );
			ASSERT(result);
			if (result) {
				TRACER("TX: startQuiescentMode( )\r\n");
				this->setState(STATE_ENTER_QUIESCENT_MODE);
			}									   
		} break;
		
		// NOTE_DTR0_SHORTADDRESS.
		// - Wissen adres: 0xff in DTR0 laden.
		// - Adres toekennen: Adres data in bit 1 tem 7, bit 0 op 1.
		case STATE_ENTER_QUIESCENT_MODE: {		
			const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
			if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
				int valueDTR0 = 0xff;	// zie NOTE_DTR0_SHORTADDRESS				
				if (TASK_ASSIGN_NEW_ADDRESS == this->m_task) {
					ASSERT(this->m_newShortAddress >=0 && this->m_newShortAddress<0xff);
					valueDTR0 = (this->m_newShortAddress << 1) + 1;
				}
				result = this->getTransmitter().sendControlGearDTR0(valueDTR0);
				ASSERT(result);				
			}
			else {			
				int valueDTR0 = 0xff;	// zie NOTE_DTR0_SHORTADDRESS				
				if (TASK_ASSIGN_NEW_ADDRESS == this->m_task) {
					ASSERT(this->m_newShortAddress >=0 && this->m_newShortAddress<0xff);
					valueDTR0 = this->m_newShortAddress;
				}
				result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(valueDTR0);
				ASSERT(result);						
			}
			if (result) {
				TRACER("TX: sendControlGearDTR0( )\r\n");
				this->setState(STATE_DTR0_LOAD_ADDRESS);
			}
		} break;

		case STATE_DTR0_LOAD_ADDRESS: {
			const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
			if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {						
				result = this->getTransmitter().sendControlSetShortAddress(this->getShortAddress());
				ASSERT(result);					
			}
			else {
				result = this->getTransmitter().sendControlInputDevicesSetShortAddressDTR0(this->getShortAddress());
				ASSERT(result);					
			}
			if (result) {					
				this->setState(STATE_SHORTADDRESS_SET);				
			}
		} break;
	
		case STATE_SHORTADDRESS_SET: {		
			result = this->getTransmitter().sendControlgearBroadcastTerminate( );
			if (result) {
				TRACER("TX: sendQueryControlGearPresent( )\r\n");					
				this->setState(STATE_TERMINATE);				
			}			
		} break;

		// Oppassen: Verify is afhankelijk van het algoritme.
		// - Bij een erase: (this->m_newShortAddress == 0xff)
		//   Moeten controleren of het device met het vorige short address nog antwoord.
		// - Bij het toekennen van een nieuw adres.
		//   Moeten controleren of er een device antwoord op het nieuwe adres.
		case STATE_TERMINATE: {
			const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
			int verifyAddress = 0xFF;						// Oppassen: verifyAddress is afhankelijk van algoritme.
			if (TASK_ASSIGN_NEW_ADDRESS == this->m_task) {
				verifyAddress = this->m_newShortAddress;
			}
			else {
				verifyAddress = this->getShortAddress();
			}
			if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {					
				result = this->getTransmitter().sendQueryControlGearPresent(verifyAddress);									
			}
			else {
				result = this->getTransmitter().sendQueryControlInputDeviceVersionNumber(verifyAddress);
			}			
			ASSERT(result);	
			if (result) {
				TRACER("TX: sendQueryControlGearPresent( )\r\n");					
				this->setState(STATE_VERIFY_ANSWER);				
			}	
		} break;
		
		// zie NOTE_CONTROLGEAR_ADDRESSING_PROBLEM:
		// Verify een aantal keren na elkaar proberen.
		case STATE_VERIFY_ANSWER: {					
			if ((TASK_ERASE_ADDRESS == this->m_task) || (false == this->m_stateSuccess) || (++this->m_verifyCounter >= COUNTER_MAX_VERIFY)) {
				result = this->getTransmitter().stopQuienscentMode();
				if (result) {
					TRACER("TX: stopQuienscentMode( )\r\n");					
					this->setState(STATE_EXIT_QUIESCENT_MODE);				
				}			
			}
			else {
				const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
				const int verifyAddress = this->m_newShortAddress;	
				if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {					
					result = this->getTransmitter().sendQueryControlGearDeviceStatus(verifyAddress);									
				}
				else {
					result = this->getTransmitter().sendQueryControlInputDeviceVersionNumber(verifyAddress);
				}			
				ASSERT(result);	
			}
		} break;

		case STATE_EXIT_QUIESCENT_MODE: {			
			ASSERT(0);
		} break;
		
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;

		default: {
			ASSERT(0);
			result = false;
		} break;		
	} /* end switch */
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperAssingShortAddress::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	const int state = this->getState();
	if (STATE_VERIFY_ANSWER == state) {
		const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();		
		if (TASK_ASSIGN_NEW_ADDRESS == this->m_task) {
			// Antwoord ontvangen van het device met het nieuwe short address.
			this->m_stateSuccess = true;	
		}
		else if (TASK_ERASE_ADDRESS == this->m_task) {
			// Antwoord ontvangen van een device met het short address die moest gewist worden.
			// Niet ok.
			this->setFatalErrorInfo(ERROR_CODE_PROPERTY_WRONGRESPONSE,this->getShortAddress(),"Possible the address has not been erased.");
			this->m_stateSuccess = false;	
		}
		else { ASSERT(0); }
		return true;
	}
	// Foutmelding...
	ASSERT(0);		// Todo ???
	return false;
}

bool CAppDaliFsmHelperAssingShortAddress::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	if (STATE_VERIFY_ANSWER == this->getState()) {
		if (TASK_ASSIGN_NEW_ADDRESS == this->m_task) {
			// Geen antwoord ontvangen van het device met het nieuwe short address.
			CString strMessage;
			strMessage.Format("Device %d. Failed to assign a new device address %d.",this->getShortAddress(), this->m_newShortAddress);
			this->setFatalErrorInfo(ERROR_CODE_PROPERTY_NORESPONSE,this->getShortAddress(),strMessage);
			this->m_stateSuccess = false;	
		}
		else if (TASK_ERASE_ADDRESS == this->m_task) {
			// Geen antwoord meer van het oude adres. Ok.
			this->m_stateSuccess = true;
		}
		else { ASSERT(0); }
		return true;
	}
	// Foutmelding...
	ASSERT(0);		// Todo ???
	return false;
}

// Sedert V16.41: USE_WORKAROUND_IGNORE_TXDONE_EVENT_IDENTIFY_BUSY
// Dit algoritme wordt uitgevoerd via de property page van de devices.
// Mogelijk is het identify algoritme ook aktief tijdens het uitvoeren van dit algoritme.
// Het TX-Done event kan afkomstig zijn van het Identify bericht.
// Daarom deze fout negeren.
bool CAppDaliFsmHelperAssingShortAddress::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_EXIT_QUIESCENT_MODE == state) {	
		IAppDaliFsm_HelperFsmEvents::AlgoritmeName_t name = IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESS_ASSIGN;
		if (TASK_ERASE_ADDRESS == this->m_task) {
			name = IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESS_ERASE;
		}
		else if (TASK_ASSIGN_NEW_ADDRESS == this->m_task) {		
			name = IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESS_ASSIGN;
		}	
		else { ASSERT(0); }
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(),name , this->m_newShortAddress, -1);
		this->setState(STATE_DONE);
		return true;		
	}
	if ((STATE_INITIALISE == state) || (STATE_ENTER_QUIESCENT_MODE == state) || 
		(STATE_DTR0_LOAD_ADDRESS == state) || (STATE_SHORTADDRESS_SET == state) || 
		(STATE_TERMINATE == state) || (STATE_EXIT_QUIESCENT_MODE == state)) {
		return true;
	}

#if(0)
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, -1, "Expected a backward response but recieved a tx-done event for the DALI controller");
	this->m_state = STATE_ERROR_FATAL;		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
#endif 

	return true;
}

// TASK_ASSIGN_NEW_ADDRESS
// Wanneer we een adres toekennen en er is reeds een device op de DALI bus die dit adres heeft krijgen we 
// een error frame terug.
bool CAppDaliFsmHelperAssingShortAddress::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	if (STATE_VERIFY_ANSWER == this->getState()) {		
		// Voor iedere taak.
		// Mogelijk dubbel antwoord omdat er meerdere devices antwoorden.
		CString str;
		if (TASK_ASSIGN_NEW_ADDRESS == this->m_task) {
			str.Format("An error occurred while assigning a new address to device A%d. "
					   "The new address A%d that was assigned may already be in use by another device.",
					   this->getShortAddress(), this->m_newShortAddress);


			this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESS_ASSIGN , ERROR_CODE_SHORTADDRESS_DOUBLE, -1);

		}
		else {
			str.Format("An error occurred while removing the address of device A%d. "
					   "The address may not have been removed, or there might be another device on the bus that also has this address.", 
					   this->getShortAddress());
		}


		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,this->getShortAddress(),str);			
		this->m_stateSuccess = false;			
		return true;
	}	
	// Foutmelding...
	ASSERT(0);		// Todo ???
	return false;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlInputDeviceInstancesCfg::CAppDaliFsmHelperControlInputDeviceInstancesCfg(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_pDataCfg(NULL), m_currentInstance(0) { }

CAppDaliFsmHelperControlInputDeviceInstancesCfg::~CAppDaliFsmHelperControlInputDeviceInstancesCfg() {
	this->deallocateCfgData();
}

bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::start(int shortAddress, CAppDaliInputControlDeviceInstances* data) {
	ASSERT(NULL != data);
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->clearCounterValue();

	// Eigen members.
	this->deallocateCfgData();
	this->m_currentInstance = 0;
	this->m_pDataCfg = data;
	return this->next();
}

bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

// acties.
bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			const bool enable = this->getInstanceEnabled(this->m_currentInstance);	
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONFIG_CONTROLDEVICE_INSTANCE_PARAMS, -1, -1);
			if (enable) {
				result = this->getTransmitter().sendControlInputDeviceInstanceEnableInstance(this->getShortAddress(), this->m_currentInstance);
			}
			else {
				result = this->getTransmitter().sendControlInputDeviceInstanceDisableInstance(this->getShortAddress(), this->m_currentInstance);
			}		
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDeviceInstanceEnableInstance / Disable( )\r\n");
				this->setState(STATE_CFG_ENABLE);
			}
		} break;

		// Bij de eerste instantie wordt er een broadcast DTR2=0,DTR1=0 verstuurd.
		// Deze zorgt ervoor dat de registers op 0 staan.
		// In de volgende stappen wordt enkel DTR0 gebruikt.
		case STATE_CFG_ENABLE: {	
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));		
			if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST == this->m_currentInstance) {
				result = this->getTransmitter().sendControlInputDevicesBroadcastWriteDTR2_DTR1(0, 0);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlInputDevicesBroadcastWriteDTR2_DTR1( )\r\n");
					this->setState(STATE_CFG_CLR_DTR_REGISTERS);
				}
			}
			else {				
				const int eventAddressSchema = this->getInstanceEventAddressScheme(this->m_currentInstance);
				result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(eventAddressSchema);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
					this->setState(STATE_CFG_SCHEMA_DTR0);
				}
			}
		} break;
		// Wissen van de registers: wacht op response.
		case STATE_CFG_CLR_DTR_REGISTERS: {
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			const int eventAddressSchema = this->getInstanceEventAddressScheme(this->m_currentInstance);
			result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(eventAddressSchema);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
				this->setState(STATE_CFG_SCHEMA_DTR0);
			}
		} break;
		case STATE_CFG_SCHEMA_DTR0: {
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			result = this->getTransmitter().sendControlInputDeviceInstanceEventSchemeDTR0(this->getShortAddress(), this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDeviceInstanceEventSchemeDTR0( )\r\n");
				this->setState(STATE_CFG_SCHEMA);
			}
		} break;
		case STATE_CFG_SCHEMA: {
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			const int eventFilters = this->getInstanceEventFilters(this->m_currentInstance);
			result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(eventFilters);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
				this->setState(STATE_CFG_FILTERS_DTR0);
			}		
		} break;
		case STATE_CFG_FILTERS_DTR0: {
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			result = this->getTransmitter().sendControlInputDeviceInstanceEventFilterDTR0DTR1DTR2(this->getShortAddress(), this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDeviceInstanceEventFilterDTR0DTR1DTR2( )\r\n");
				this->setState(STATE_CFG_FILTERS);
			}
		} break;
		case STATE_CFG_FILTERS: {
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			// Versturen waarde prio naar DTR0 register
			const int eventPrio = this->getInstanceEventPrio(this->m_currentInstance);
			result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(eventPrio);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
				this->setState(STATE_CFG_PRIO_DTR0);
			}		
		} break;
		case STATE_CFG_PRIO_DTR0: {
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			// Laden waarde DTR0 voor prio.
			result = this->getTransmitter().sendControlInputDeviceInstanceEventPrioDTR0(this->getShortAddress(), this->m_currentInstance);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlInputDevicesBroadcastDTR0( )\r\n");
				this->setState(STATE_CFG_PRIO);
			}					
		} break;
		case STATE_CFG_PRIO: {
			// ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));			
			ASSERT(0);
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;

		default: {
			ASSERT(0);
			result = false;
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, -1, "Expected no backward response.");
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, -1, "Expected no backward no-response.");
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
	if (STATE_CFG_PRIO == this->getState()) {
		const int next = this->getNextInstanceNumber(this->m_currentInstance);
		if (next>=0) {
			// Volgende instantie...
			this->m_currentInstance = next;
			ASSERT((this->m_currentInstance >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (this->m_currentInstance <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
			this->setState(STATE_READY);
		}
		else {
			this->getParent().onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(this->getShortAddress(), this->m_currentInstance, this->getInstanceType(this->m_currentInstance));
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONFIG_CONTROLDEVICE_INSTANCE_PARAMS, -1, -1);
			this->setState(STATE_DONE);
		}

/*
		if (++this->m_currentInstance>DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST) {
			this->getParent().onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(this->getShortAddress(), this->m_currentInstance, this->getInstanceType(this->m_currentInstance));
			this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONFIG_CONTROLDEVICE_INSTANCE_PARAMS, -1, -1);
			this->setState(STATE_DONE);
		}
		else {
			this->setState(STATE_CFG_ENABLE);
		}			
*/
	}
	return true;
}

bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, -1, "Expected no backward error frame response.");
	this->setState(STATE_ERROR_FATAL);			// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}


	// Nodig voor het updaten van de database wanneer het algoritme gedaan is.
const CAppDaliInputControlDeviceInstances& CAppDaliFsmHelperControlInputDeviceInstancesCfg::getcfgData( ) const {
	ASSERT(NULL != this->m_pDataCfg);
	return *this->m_pDataCfg;
}

CAppDaliInputControlDeviceInstances& CAppDaliFsmHelperControlInputDeviceInstancesCfg::getcfgData( ) {
	ASSERT(NULL != this->m_pDataCfg);
	return *this->m_pDataCfg;
}

// protected.
void CAppDaliFsmHelperControlInputDeviceInstancesCfg::deallocateCfgData() {
	if (NULL != this->m_pDataCfg) {
		delete this->m_pDataCfg;
		this->m_pDataCfg = NULL;
	}
}

int CAppDaliFsmHelperControlInputDeviceInstancesCfg::getNextInstanceNumber(int instanceNumber) {	
	for (int nextInstanceNumber = instanceNumber+1;
		 (nextInstanceNumber>=DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (nextInstanceNumber<=DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST);
		 nextInstanceNumber++) {
		CAppDaliInputControlDeviceInstance* pDummy = NULL;
		if (this->getInstance(nextInstanceNumber,&pDummy)) {		
			ASSERT(NULL != pDummy);
			return nextInstanceNumber;

		}
	}
	return -1;
}


// Helper functies.
bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::getInstance(int instanceNumber, CAppDaliInputControlDeviceInstance** ppInstance) {
	if (NULL == this->m_pDataCfg) {
		return false;
	}
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (false == this->m_pDataCfg->find(instanceNumber, &pInstance)) {
		return false;
	}
	*ppInstance = pInstance;
	return true;
}

bool CAppDaliFsmHelperControlInputDeviceInstancesCfg::getInstanceEnabled(int instanceNumber) {
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (false == this->getInstance(instanceNumber, &pInstance)) {
		return false;
	}
	return pInstance->getInstanceActive();
}


// TODO: DEZE IN DE BASISKLASSE STOPPEN...

static const int DALI_DEFAULT_PUSHBUTTON_EVENTFILTERS = DALI_CONTROLDEVICE_PUSHBUTTON_DEFAULT_EVENTFILTER;
static const int DALI_DEFAULT_PUSHBUTTON_ADDRESS_SCHEME = DALI_CONTROLDEVICE_EVENT_SCHEME_DEFAULT;
static const int DALI_DEFAULT_PUSHBUTTON_PRIORITY = 3;
static const int DALI_DEFAULT_INSTANCE_TYPE = DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON;


int CAppDaliFsmHelperControlInputDeviceInstancesCfg::getInstanceEventFilters(int instanceNumber) {
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (false == this->getInstance(instanceNumber, &pInstance)) {
		return DALI_DEFAULT_PUSHBUTTON_EVENTFILTERS;
	}
	return pInstance->getEventFilter();
}

int CAppDaliFsmHelperControlInputDeviceInstancesCfg::getInstanceEventAddressScheme(int instanceNumber) {
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (false == this->getInstance(instanceNumber, &pInstance)) {
		return DALI_DEFAULT_PUSHBUTTON_ADDRESS_SCHEME;
	}
	return pInstance->getEventScheme();
}

int CAppDaliFsmHelperControlInputDeviceInstancesCfg::getInstanceEventPrio(int instanceNumber) {
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (false == this->getInstance(instanceNumber, &pInstance)) {
		return DALI_DEFAULT_PUSHBUTTON_PRIORITY;
	}
	return pInstance->getEventPriority();
}

int CAppDaliFsmHelperControlInputDeviceInstancesCfg::getInstanceType(int instanceNumber) {
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (false == this->getInstance(instanceNumber, &pInstance)) {
		return DALI_DEFAULT_INSTANCE_TYPE;
	}
	return pInstance->getInstanceType();
}
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlGearProperties_Base::CAppDaliFsmHelperControlGearProperties_Base(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_pDataCfg(NULL) { }
	
CAppDaliFsmHelperControlGearProperties_Base::~CAppDaliFsmHelperControlGearProperties_Base() {
	this->deallocateCfgData();
}

const CAppDaliDeviceProperties_ControlGear& CAppDaliFsmHelperControlGearProperties_Base::getcfgData( ) const {
	ASSERT(NULL != this->m_pDataCfg);
	return *this->m_pDataCfg;
}

CAppDaliDeviceProperties_ControlGear& CAppDaliFsmHelperControlGearProperties_Base::getcfgData( ) {
	ASSERT(NULL != this->m_pDataCfg);
	return *this->m_pDataCfg;
}

// protected.
void CAppDaliFsmHelperControlGearProperties_Base::deallocateCfgData() {
	if (NULL != m_pDataCfg) {
		delete m_pDataCfg;
		m_pDataCfg = NULL;
	}
}

// Oppassen: Indien deze functie false terug geeft moet de client de Config data dealloceren.
// Indien deze functie true antwoord is deze worker verantwoordelijk voor de deallocatie.
void CAppDaliFsmHelperControlGearProperties_Base::clearCfgData( ) {
	this->m_pDataCfg = NULL;
}

void CAppDaliFsmHelperControlGearProperties_Base::setCfgData(CAppDaliDeviceProperties_ControlGear* data) {
	REQUIRE(NULL != data);
	this->deallocateCfgData();
	this->m_pDataCfg = data;
	ENSURE(NULL != this->m_pDataCfg);
}
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlGearDeviceCfg::CAppDaliFsmHelperControlGearDeviceCfg(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_supportedExtFadeTime(false) { }

CAppDaliFsmHelperControlGearDeviceCfg::~CAppDaliFsmHelperControlGearDeviceCfg() {
}

bool CAppDaliFsmHelperControlGearDeviceCfg::start(int shortAddress, CAppDaliDeviceProperties_ControlGear* data) {
	ASSERT(NULL != data);
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->clearCounterValue();

	// Eigen members.
	this->setCfgData(data);	


	// Todo: zie m_supportedExtFadeTime
	// Bepalen of dit device DALI v2.0 ondersteund.
	// Indien ja wan wordt m_supportedExtFadeTime == true.
	
	if (!this->next()) {
		// Oppassen: Indien deze functie false terug geeft moet de client de Config data dealloceren.
		// Indien deze functie true antwoord is deze worker verantwoordelijk voor de deallocatie.
		this->clearCfgData();
		return false;
	}
	return true;
}

bool CAppDaliFsmHelperControlGearDeviceCfg::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

// acties.
bool CAppDaliFsmHelperControlGearDeviceCfg::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_CFG_PROPERTIES, -1, -1);									  
			result = this->getTransmitter().sendQueryControlGearDeviceStatus(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlGearDeviceStatus( )\r\n");
				this->setState(STATE_QUERY_DEVICE_STATUS); 
			}
		} break;

		case STATE_QUERY_DEVICE_STATUS: {	
			result = this->getTransmitter().sendQueryControlGearVersionNumber(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlGearVersionNumber( )\r\n");
				this->setState(STATE_QUERY_DEVICE_VERSION); 
			}																
		} break;

		case STATE_QUERY_DEVICE_VERSION: {		
			// Initialise all control gear...			
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(0x00);			
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll( )\r\n");
				this->setState(STATE_INITIALISE); 
			}
		} break;

		case STATE_INITIALISE: {										 															
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {
				TRACER("TX: startQuiescentMode( )\r\n");
				this->setState(STATE_ENTER_QUIESCENT_MODE); 
			}
		} break;

		// Fade time
		case STATE_ENTER_QUIESCENT_MODE: {
			int data = CAppDaliDeviceProperties_ControlGear::getFadeTime(this->getcfgData());
			if (data < 0) {
				ASSERT(data >= 0);
				data = 0;
			}
			result = this->getTransmitter().sendControlGearDTR0(data);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearDTR0( )\r\n");
				this->setState(STATE_FADETIME_DTR0);
			}								  
		} break;
		case STATE_FADETIME_DTR0: {		
			result = this->getTransmitter().sendControlGearFadeTime(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearFadeTime( )\r\n");
				this->setState(STATE_FADETIME);
			}								  
		} break;

		// Extended Fade time.
		case STATE_FADETIME: {
			// Controle doen of deze ondersteund wordt door het device + of deze een geldige waarde heeft.
			const int extendedFadeTime = this->getcfgData().getExtendedFadeTime();
			if ((this->m_supportedExtFadeTime) && (extendedFadeTime >= 0)) {
				const int data = extendedFadeTime;
				result = this->getTransmitter().sendControlGearDTR0(data);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlGearDTR0( )\r\n");
					this->setState(STATE_FADETIME_EXT_DTR0);
				}					 
			}
			else {			
				int data = CAppDaliDeviceProperties_ControlGear::getFadeRate(this->getcfgData());
				if (data < 0) {
					ASSERT(data >= 0);
					data = 0;
				}
				result = this->getTransmitter().sendControlGearDTR0(data);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlGearDTR0( )\r\n");
					this->setState(STATE_FADERATE_DTR0);
				}				
			}
		} break;
		case STATE_FADETIME_EXT_DTR0: {			
			result = this->getTransmitter().sendControlGearExtendedFadeTime(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearExtendedFadeTime( )\r\n");
				this->setState(STATE_FADETIME_EXT);
			}					 
		} break;

		// Fade rate
		case STATE_FADETIME_EXT: {
			int data = CAppDaliDeviceProperties_ControlGear::getFadeRate(this->getcfgData());
			if (data < 0) {
				ASSERT(data >= 0);
				data = 0;
			}
			result = this->getTransmitter().sendControlGearDTR0(data);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearDTR0( )\r\n");
				this->setState(STATE_FADERATE_DTR0);
			}					 
		} break;
		case STATE_FADERATE_DTR0: {			
			result = this->getTransmitter().sendControlGearFadeRate(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearFadeRate( )\r\n");
				this->setState(STATE_FADERATE);
			}					 
		} break;

		// Min level.
		case STATE_FADERATE: {
			const int data = this->getcfgData().getLevelMin();
			result = this->getTransmitter().sendControlGearDTR0(data);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearDTR0( )\r\n");
				this->setState(STATE_LEVEL_MIN_DTR0);
			}					 
		} break;
		case STATE_LEVEL_MIN_DTR0: {			
			result = this->getTransmitter().sendControlGearMinLevel(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearMinLevel( )\r\n");
				this->setState(STATE_LEVEL_MIN);
			}					 
		} break;

		// Max level.
		case STATE_LEVEL_MIN: {
			const int data = this->getcfgData().getLevelMax();
			result = this->getTransmitter().sendControlGearDTR0(data);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearDTR0( )\r\n");
				this->setState(STATE_LEVEL_MAX_DTR0);
			}					 
		} break;
		case STATE_LEVEL_MAX_DTR0: {			
			result = this->getTransmitter().sendControlGearMaxLevel(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearMaxLevel( )\r\n");
				this->setState(STATE_LEVEL_MAX);
			}					 
		} break;

		// Power on level.
		case STATE_LEVEL_MAX: {
			const int data = this->getcfgData().getLevelPowerOn();
			result = this->getTransmitter().sendControlGearDTR0(data);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearDTR0( )\r\n");
				this->setState(STATE_LEVEL_POWERON_DTR0);
			}					 
		} break;
		case STATE_LEVEL_POWERON_DTR0: {			
			result = this->getTransmitter().sendControlGearPowerOnLevel(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearPowerOnLevel( )\r\n");
				this->setState(STATE_LEVEL_POWERON);
			}					 
		} break;

		// System failure level.
		case STATE_LEVEL_POWERON: {
			const int data = this->getcfgData().getLevelSystemFailure();
			result = this->getTransmitter().sendControlGearDTR0(data);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearDTR0( )\r\n");
				this->setState(STATE_LEVEL_SYSTEM_FAILURE_DTR0);
			}					 
		} break;
		case STATE_LEVEL_SYSTEM_FAILURE_DTR0: {			
			result = this->getTransmitter().sendControlGearSystemFailureLevel(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearSystemFailureLevel( )\r\n");
				this->setState(STATE_LEVEL_SYSTEM_FAILURE);
			}					 
		} break;

		// Exit
		case STATE_LEVEL_SYSTEM_FAILURE: {
			result = this->getTransmitter().stopQuienscentMode();
			ASSERT(result);
			if (result) {
				TRACER("TX: stopQuiescentMode( )\r\n");
				this->setState(STATE_EXIT_QUIESCENT_MODE);
			}														
		} break;

		case STATE_EXIT_QUIESCENT_MODE: {
			result = this->getTransmitter().sendControlgearBroadcastTerminate();
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastTerminate( )\r\n");
				this->setState(STATE_TERMINATE);
			}																	
		} break;

		case STATE_TERMINATE: {
			ASSERT(0);	
			result = false;
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperControlGearDeviceCfg::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		return true;
	}
	CString stringErrorDetail("Recieved event recieved backward response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlGearDeviceCfg::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		CString stringErrorDetail("Recieved No backward response.");
		this->setFatalErrorInfo(ERROR_CODE_PROPERTY_NORESPONSE,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);	// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	CString stringErrorDetail("Recieved event backward no response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}


bool CAppDaliFsmHelperControlGearDeviceCfg::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		CString stringErrorDetail("Recieved event TX done but expected a backward response.");
		this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);	// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	else if (state  == STATE_TERMINATE) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_CFG_PROPERTIES, -1, -1);
		this->setState(STATE_DONE);			// errorCode=ERROR_CODE_WRONG_EVENT
		return true;	
	}
	return true;
}

bool CAppDaliFsmHelperControlGearDeviceCfg::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		CString stringErrorDetail("Recieved a backward response error message.");
		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);	// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	CString stringErrorDetail("Recieved event backward response error but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
#define TRACER_QUERY_ACTUAL_VALUE					TRACE
/***********************************************************************************/
CAppDaliFsmHelperControlGearActualValueRequest::CAppDaliFsmHelperControlGearActualValueRequest(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_actualLevel(-1) { }

CAppDaliFsmHelperControlGearActualValueRequest::~CAppDaliFsmHelperControlGearActualValueRequest() { }

bool CAppDaliFsmHelperControlGearActualValueRequest::start(int shortAddress) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->clearCounterValue();

	// Eigen members.
	this->m_actualLevel = -1;
	return this->next();
}


bool CAppDaliFsmHelperControlGearActualValueRequest::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

// acties.
bool CAppDaliFsmHelperControlGearActualValueRequest::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			TRACER_QUERY_ACTUAL_VALUE("-> Start Query actual value\r\n");

			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_QUERY_ACTUALLEVEL, -1, -1);
			result = this->getTransmitter().sendQueryControlGearActualLevel(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlGearActualLevel( )\r\n");
				this->setState(STATE_QUERY_ACTUAL_LEVEL);
			}
		} break;
		case STATE_QUERY_ACTUAL_LEVEL: {
			ASSERT(0);
			result = false;
		} break;
		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL: {
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result = false;
		} break;
	}
	return result;
}

bool CAppDaliFsmHelperControlGearActualValueRequest::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	REQUIRE(data >= 0 && data <= 0xff);
	if (STATE_QUERY_ACTUAL_LEVEL == this->getState()) {		
		this->m_actualLevel = data;
		this->setState(STATE_DONE);
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_QUERY_ACTUALLEVEL, -1, -1);
		TRACER_QUERY_ACTUAL_VALUE("Query actual value done\r\n");
		return true;
	}
	CString stringEmpty;
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringEmpty);
	this->setState(STATE_ERROR_FATAL);
	return false;
}

bool CAppDaliFsmHelperControlGearActualValueRequest::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	CString stringEmpty;
	this->setFatalErrorInfo(ERROR_CODE_PROPERTY_NORESPONSE, this->getShortAddress(), stringEmpty);
	this->setState(STATE_ERROR_FATAL);
	return false;
}

// Soms krijgen we een TX done event op een vorig bericht.
// We gaan deze fout negeren...
bool CAppDaliFsmHelperControlGearActualValueRequest::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	if (STATE_QUERY_ACTUAL_LEVEL == this->getState()) {		
		this->m_actualLevel = -1;
		this->setState(STATE_DONE);
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_QUERY_ACTUALLEVEL, -1, -1);
		TRACER_QUERY_ACTUAL_VALUE("Query actual value done\r\n");
		return true;
	}

	CString stringErrorDetail("Recieved TX done event but expected backward response");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);
	return false;
}

// Soms krijgen we een frame error wanneer het device aan het faden is.
// We gaan deze fout negeren maar de waarde op -1 zetten.
bool CAppDaliFsmHelperControlGearActualValueRequest::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	if (STATE_QUERY_ACTUAL_LEVEL == this->getState()) {		
		this->m_actualLevel = -1;
		this->setState(STATE_DONE);
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_QUERY_ACTUALLEVEL, -1, -1);
		return true;
	}
	CString stringEmpty;
	this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE, this->getShortAddress(), stringEmpty);
	this->setState(STATE_ERROR_FATAL);
	return false;	
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlGearCfgScenes_Base::CAppDaliFsmHelperControlGearCfgScenes_Base(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_sceneNumber(-1), m_channelNumber(0), m_supportDT8ColorObject(false), m_stateDT8ColorObject(SUB_STATE_DT8_NONE) { }

CAppDaliFsmHelperControlGearCfgScenes_Base::~CAppDaliFsmHelperControlGearCfgScenes_Base( ) { }

void CAppDaliFsmHelperControlGearCfgScenes_Base::setSceneNumber(int sceneNumber) {
	//REQUIRE(sceneNumber>=DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST && sceneNumber<=DALI_SYSTEM_CONTROLGEAR_SCENE_LAST);
	this->m_sceneNumber = sceneNumber;
}
int CAppDaliFsmHelperControlGearCfgScenes_Base::getSceneNumber() const {
	//ENSURE(this->m_sceneNumber>=DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST && this->m_sceneNumber<=DALI_SYSTEM_CONTROLGEAR_SCENE_LAST);
	return this->m_sceneNumber;
}

void CAppDaliFsmHelperControlGearCfgScenes_Base::setChannelNumber(int channelNumber) {
	REQUIRE(channelNumber>=0 && channelNumber<DT8_DALI_MAX_PRIMARY_CHANNELS);
	this->m_channelNumber = channelNumber;
}

int CAppDaliFsmHelperControlGearCfgScenes_Base::getChannelNumber() const {
	ENSURE(this->m_channelNumber>=0 && this->m_channelNumber<DT8_DALI_MAX_PRIMARY_CHANNELS);
	return this->m_channelNumber;
}

void CAppDaliFsmHelperControlGearCfgScenes_Base::setSupportDT8ColorObject(bool flag) {
	this->m_supportDT8ColorObject = flag;
}
bool CAppDaliFsmHelperControlGearCfgScenes_Base::getSupportDT8ColorObject() const {
	return this->m_supportDT8ColorObject;
}

void CAppDaliFsmHelperControlGearCfgScenes_Base::setStateDT8ColorObject(StateDT8ColorObject state) {
	this->m_stateDT8ColorObject = state;
}

CAppDaliFsmHelperControlGearCfgScenes_Base::StateDT8ColorObject CAppDaliFsmHelperControlGearCfgScenes_Base::getStateDT8ColorObject() const {
	return this->m_stateDT8ColorObject;
}


bool CAppDaliFsmHelperControlGearCfgScenes_Base::getRemoveScene(int sceneNumber) {
	bool removeScene = false;	
	if (this->getSupportDT8ColorObject()) {
		const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(sceneNumber);
		if (!CAppDaliObjectDT8ColorData_Utils::isActive(refColorData)) {
			// Wannneer een DT8 sfeer niet actief is moeten we deze altijd eerst wissen.	
			// Dit zorgt ervoor dat alle DT8 color data in het device gewist wordt.
			removeScene = true;
		}
	}
	else {
		const int scenelevel = this->getcfgData().getSceneLevel(sceneNumber);
		if ((scenelevel < 0) || (scenelevel >= 255)) {
			removeScene = true;
		}
	}
	return removeScene;
}


static int NormalizeValueShort(int value) {
	if (value < 0 || value > CAppDaliObjectDT8ColorData::MASK_VALUE_SHORT) {
		value = CAppDaliObjectDT8ColorData::MASK_VALUE_SHORT;
	}	
	return value;
}

static int NormalizeValueBYTE(int value) {
	if (value < 0 || value > CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE) {
		value = CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE;
	}	
	return value;
}

static int _MSB(int value) {
	REQUIRE(value>=0);
	int retValue = (value & 0xFF00) >> 8;
	ENSURE(retValue>=0 && retValue<=0xFF);
	return retValue;
}

static int _LSB(int value) {
	REQUIRE(value>=0);
	int retValue = (value & 0xFF);
	ENSURE(retValue>=0 && retValue<=0xFF);
	return retValue;
}

// LSB in DTR0 plaatsen.
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorTemperatureValueLSB( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(refColorData));
	const int data = _LSB(NormalizeValueShort(refColorData.getColorTemperature()));
	const bool result = this->getTransmitter().sendControlGearDTR0(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorTemperatureValueLSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
	}																		
	return result;
}

// MSB in DTR0 plaatsen.
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorTemperatureValueMSB( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(refColorData));
	const int data = _MSB(NormalizeValueShort(refColorData.getColorTemperature()));
	const bool result = this->getTransmitter().sendControlGearDTR1(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorTemperatureValueMSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);
	}																		
	return result;
}

// load DTR0 LSB
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorCoordinateXValueLSB( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData));
	const int data = _LSB(NormalizeValueShort(refColorData.getCoordinateX()));
	const bool result = this->getTransmitter().sendControlGearDTR0(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorTemperatureValueLSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_X);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
	}																		
	return result;
}


// load DTR1 MSB
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorCoordinateXValueMSB( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData));
	const int data = _MSB(NormalizeValueShort(refColorData.getCoordinateX()));
	const bool result = this->getTransmitter().sendControlGearDTR1(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorTemperatureValueLSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_X);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);
	}																		
	return result;
}

// load DTR0 LSB
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorCoordinateYValueLSB( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData));
	const int data = _LSB(NormalizeValueShort(refColorData.getCoordinateY()));
	const bool result = this->getTransmitter().sendControlGearDTR0(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorTemperatureValueLSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_Y);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
	}																		
	return result;
}

// load DTR1 MSB
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorCoordinateYValueMSB( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData));
	const int data = _MSB(NormalizeValueShort(refColorData.getCoordinateY()));
	const bool result = this->getTransmitter().sendControlGearDTR1(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorTemperatureValueLSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_Y);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);
	}																		
	return result;

}

// DTR0
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorDimlevelRed( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData));
	const int data = NormalizeValueBYTE(refColorData.getColorDimLevelRed());
	const bool result = this->getTransmitter().sendControlGearDTR0(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorDimlevelRed( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_RGB);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
	}																		
	return result;
}

// DTR1
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorDimlevelGreen( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData));
	const int data = NormalizeValueBYTE(refColorData.getColorDimLevelGreen());
	const bool result = this->getTransmitter().sendControlGearDTR1(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorDimlevelGreen( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_RGB);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);
	}																		
	return result;
}

// DTR2
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorDimlevelBlue( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData));
	const int data = NormalizeValueBYTE(refColorData.getColorDimLevelBlue());
	const bool result = this->getTransmitter().sendControlGearDTR2(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorDimlevelBlue( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_RGB);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
	}																		
	return result;
}

// DTR0
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorDimlevelWhite( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData));
	const int data = NormalizeValueBYTE(refColorData.getColorDimLevelWhite());
	const bool result = this->getTransmitter().sendControlGearDTR0(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorDimlevelWhite( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_WAF);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
	}																		
	return result;
}

// DTR1
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorDimlevelAmber( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData));
	const int data = NormalizeValueBYTE(refColorData.getColorDimLevelAmber());
	const bool result = this->getTransmitter().sendControlGearDTR1(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorDimlevelAmber( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_WAF);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);
	}																		
	return result;
}

// DTR2
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorDimlevelFreeColor( ) {
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData));
	const int data = NormalizeValueBYTE(refColorData.getColorDimLevelFreeColor());
	const bool result = this->getTransmitter().sendControlGearDTR2(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorDimlevelFreeColor( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_WAF);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
	}																		
	return result;
}

// load DTR0 LSB	
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorChannelDimLevelLSB(int channel) {
	REQUIRE(channel>=0 && channel<DT8_DALI_MAX_PRIMARY_CHANNELS);
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(refColorData));
	const int data = _LSB(NormalizeValueShort(refColorData.getColorDimLevelChannel(channel)));
	const bool result = this->getTransmitter().sendControlGearDTR0(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorChannelDimLevelLSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_CHANNEL);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
	}																		
	return result;
}

// load DTR1 MSB
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorChannelDimLevelMSB(int channel) {
	REQUIRE(channel>=0 && channel<DT8_DALI_MAX_PRIMARY_CHANNELS);
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(refColorData));
	const int data = _MSB(NormalizeValueShort(refColorData.getColorDimLevelChannel(channel)));
	const bool result = this->getTransmitter().sendControlGearDTR1(data);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorChannelDimLevelMSB( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_CHANNEL);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);
	}																		
	return result;
}

// DTR2
bool CAppDaliFsmHelperControlGearCfgScenes_Base::sendDT8ColorChannelNumber(int channel) {
	REQUIRE(channel>=0 && channel<DT8_DALI_MAX_PRIMARY_CHANNELS);
	const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
	ASSERT(CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(refColorData));	
	const bool result = this->getTransmitter().sendControlGearDTR2(channel);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendDT8ColorChannelNumber( )\r\n");
		this->setStateDT8ColorObject(SUB_STATE_DT8_SET_COLORVALUE_CHANNEL);
		// this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
	}																		
	return result;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlGearCfgScenes::CAppDaliFsmHelperControlGearCfgScenes(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter) { }

CAppDaliFsmHelperControlGearCfgScenes::~CAppDaliFsmHelperControlGearCfgScenes() { }

bool CAppDaliFsmHelperControlGearCfgScenes::start(int shortAddress, CAppDaliDeviceProperties_ControlGear* data) {
	ASSERT(NULL != data);
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->clearCounterValue();	
	this->setSceneNumber(DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST);
	this->setChannelNumber(0);
	this->setSupportDT8ColorObject(CAppDaliDeviceProperties_ControlGear::isDT8ColorObjectSupported(*data));
	this->setCfgData(data);	
	this->setStateDT8ColorObject(SUB_STATE_DT8_NONE);
	
	if (!this->next()) {
		// Oppassen: Indien deze functie false terug geeft moet de client de Config data dealloceren.
		// Indien deze functie true antwoord is deze worker verantwoordelijk voor de deallocatie.
		this->clearCfgData();
		return false;
	}
	return true;
}

bool CAppDaliFsmHelperControlGearCfgScenes::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperControlGearCfgScenes::sendSceneLevelValue(int value) {
	const bool result = this->getTransmitter().sendControlGearDTR0(value);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendControlGearDTR0( )\r\n");
		this->setState(STATE_SCENE_LOAD_DTR);
	}	
	return result;
}
	
bool CAppDaliFsmHelperControlGearCfgScenes::sendStopQuiescentMode() {
	const bool result = this->getTransmitter().stopQuienscentMode();
	ASSERT(result);
	if (result) {
		TRACER("TX: startQuiescentMode( )\r\n");
		this->setState(STATE_EXIT_QUIESCENT_MODE); 
	}
	return result;
}

bool CAppDaliFsmHelperControlGearCfgScenes::startUploadScene(int sceneNumber) {
	
	REQUIRE((sceneNumber >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) && (sceneNumber <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST));	

	bool result = false;
	const bool removeScene = this->getRemoveScene(sceneNumber);
	if (removeScene) {
		result = this->getTransmitter().sendControlGearRemoveFromScene(this->getShortAddress(),sceneNumber);
		ASSERT(result);
		if (result) {
			TRACER("TX: sendControlGearDTR0( )\r\n");
			this->setState(STATE_SCENE_REMOVE);
		}	
	}
	else {
		if (this->getSupportDT8ColorObject()) {					
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(sceneNumber);
			if (CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(refColorData)) {
				result = this->sendDT8ColorTemperatureValueLSB();															
			}				
			else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData)) {
				result = this->sendDT8ColorCoordinateXValueLSB();								
			}
			else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData)) {
				result = this->sendDT8ColorDimlevelRed();
			}
			else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(refColorData)) {	
				this->setChannelNumber(0);
				result = this->sendDT8ColorChannelDimLevelLSB(this->getChannelNumber());
			}
			else { ASSERT(0); }	
			if (result) {
				this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
			}
		}
		else {					
			const int scenelevel = this->getcfgData().getSceneLevel(sceneNumber);
			result = this->sendSceneLevelValue(scenelevel);					
		}
	}		
	return result;
}

// acties.
bool CAppDaliFsmHelperControlGearCfgScenes::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_CFG_SCENES, -1, -1);									  
			result = this->getTransmitter().sendQueryControlGearDeviceStatus(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlGearDeviceStatus( )\r\n");
				this->setState(STATE_QUERY_DEVICE_STATUS); 
			}																
		} break;

		case STATE_QUERY_DEVICE_STATUS: {	
			result = this->getTransmitter().sendQueryControlGearVersionNumber(this->getShortAddress());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlGearVersionNumber( )\r\n");
				this->setState(STATE_QUERY_DEVICE_VERSION); 
			}																
		} break;

		case STATE_QUERY_DEVICE_VERSION: {		
			// Initialise all control gear...			
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(0x00);			
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll( )\r\n");
				this->setState(STATE_INITIALISE); 
			}
		} break;

		case STATE_INITIALISE: {										 															
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {
				TRACER("TX: startQuiescentMode( )\r\n");
				this->setState(STATE_ENTER_QUIESCENT_MODE); 
			}
		} break;
		
		case STATE_ENTER_QUIESCENT_MODE: {
			ASSERT((this->getSceneNumber() >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) && (this->getSceneNumber() <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST));	
			result = this->startUploadScene(this->getSceneNumber());
		} break;

		case STATE_SCENE_REMOVE: {				
			bool sendDT8ColorObject = false;
			if (this->getSupportDT8ColorObject()) {
				const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
				if (CAppDaliObjectDT8ColorData_Utils::isActive(refColorData)) {					
					if (CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(refColorData)) {
						result = this->sendDT8ColorTemperatureValueLSB();															
					}	
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData)) {
						result = this->sendDT8ColorCoordinateXValueLSB();								
					}
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData)) {
						result = this->sendDT8ColorDimlevelRed();
					}
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(refColorData)) {
						this->setChannelNumber(0);
						result = this->sendDT8ColorChannelDimLevelLSB(this->getChannelNumber());
					}
					else { ASSERT(0); }	
					if (result) {
						sendDT8ColorObject = true;
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
					}
				}				
			}		
			if (!sendDT8ColorObject) {
				// Wanneer er geen DT8 color data moet verstuurd worden. Controle doen op de intensiteit.				
				const int scenelevel = this->getcfgData().getSceneLevel(this->getSceneNumber());
				if ((scenelevel < 0) || (scenelevel >= 255)) {				
					// Indien deze masked is dan kunnen we een volgende sfeer uploaden / of terminate.
					const int sceneNumber = this->getSceneNumber() + 1;
					if (sceneNumber >= DALI_SYSTEM_CONTROLGEAR_SCENE_MAX) {
						result = this->sendStopQuiescentMode();						
					}
					else {
						this->setSceneNumber(sceneNumber);
						result = this->startUploadScene(sceneNumber);														
					}
				}
				else {				
					result = this->sendSceneLevelValue(scenelevel);						
				}
			}														 
		} break; /* STATE_SCENE_REMOVE */

		case STATE_SCENE_COLORVALUE_LOAD_DTR0: {
			ASSERT(this->getSupportDT8ColorObject());
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();
			switch(stateDT8ColorObject) {
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: {
					result = this->sendDT8ColorTemperatureValueMSB();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: {
					result = this->sendDT8ColorCoordinateXValueMSB();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_Y: {
					result = this->sendDT8ColorCoordinateYValueMSB();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					result = this->sendDT8ColorDimlevelGreen();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {
					result = this->sendDT8ColorDimlevelAmber();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {
					result = this->sendDT8ColorChannelDimLevelMSB(this->getChannelNumber());									   
				} break;
				default: { ASSERT(0); } break;			
			}
			if (result) {
				this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);	
			}
		} break;

		case STATE_SCENE_COLORVALUE_LOAD_DTR1: {
			ASSERT(this->getSupportDT8ColorObject());
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();
			switch(stateDT8ColorObject) {
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: 
				case SUB_STATE_DT8_SET_COLORVALUE_X:
				case SUB_STATE_DT8_SET_COLORVALUE_Y: {						
					result = this->getTransmitter().sendControlGearEnableDeviceType(8);
					if (result) {
						this->setState(STATE_SCENE_ENABLE_DT8);
					}	
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					result = this->sendDT8ColorDimlevelBlue();	
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
					}
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {
					result = this->sendDT8ColorDimlevelFreeColor();		
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
					}
				} break;					
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {
					result = this->sendDT8ColorChannelNumber(this->getChannelNumber());								   
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
					}
				} break;
				default: {
					ASSERT(0); 
				} break;
			}
		} break;

		case STATE_SCENE_COLORVALUE_LOAD_DTR2: {
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();
			switch(stateDT8ColorObject) {
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_Y: { ASSERT(0); } break;	
				
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: 
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: 
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {
					result = this->getTransmitter().sendControlGearEnableDeviceType(8);
					if (result) {
						this->setState(STATE_SCENE_ENABLE_DT8);
					}								   
				} break;
			}
		} break;

		case STATE_SCENE_ENABLE_DT8: {
			bool (CAppDaliTransmitter::*pf)(int shortAddress) = NULL;
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			switch(stateDT8ColorObject) {		 
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorTemp;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorCoordinateX;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_Y: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorCoordinateY;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorDimLevelRGB;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorDimLevelWAF;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {				
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorPrimaryChannelN;
				} break;
			} /* end switch() */
			if (NULL != pf) {
				result = (this->getTransmitter().*pf)(this->getShortAddress());
				ASSERT(result);
				if (result) {
					this->setState(STATE_SCENE_COLORVALUE_SET);
				}						
			}									 
		} break;

		case STATE_SCENE_COLORVALUE_SET: {		
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			switch(stateDT8ColorObject) {		 
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC:
				case SUB_STATE_DT8_SET_COLORVALUE_Y:
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {				
					const int scenelevel = this->getcfgData().getSceneLevel(this->getSceneNumber());
					result = this->sendSceneLevelValue(scenelevel);
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: {
					result = this->sendDT8ColorCoordinateYValueLSB();
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
					}
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					result = this->sendDT8ColorDimlevelWhite();	
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
					}
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {	
					const int channelNumber = this->getChannelNumber() + 1;								
					this->setChannelNumber(channelNumber);
					if (channelNumber >= DT8_DALI_MAX_PRIMARY_CHANNELS) {
						const int scenelevel = this->getcfgData().getSceneLevel(this->getSceneNumber());						
						result = this->sendSceneLevelValue(scenelevel);
					}						
					else {
						result = this->sendDT8ColorChannelDimLevelLSB(channelNumber);		
					}
				} break;
			}										 										 
		} break;	

		case STATE_SCENE_LOAD_DTR: {		
			ASSERT((this->getSceneNumber() >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) && (this->getSceneNumber() <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST));			   
			result = this->getTransmitter().sendControlGearAddToScene(this->getShortAddress(),this->getSceneNumber());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearAddToScene( )\r\n");
				this->setState(STATE_SCENE_LEVEL_STORE);
			}									   
		} break;

		case STATE_SCENE_LEVEL_STORE: {
			const int sceneNumber = this->getSceneNumber() + 1;
			if (sceneNumber >= DALI_SYSTEM_CONTROLGEAR_SCENE_MAX) {
				result = this->sendStopQuiescentMode();						
			}
			else {
				this->setSceneNumber(sceneNumber);
				result = this->startUploadScene(sceneNumber);														
			}									  
		} break;

		case STATE_EXIT_QUIESCENT_MODE: {
			result = this->getTransmitter().sendControlgearBroadcastTerminate();
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastTerminate( )\r\n");
				this->setState(STATE_TERMINATE);
			}																	
		} break;

		case STATE_TERMINATE: {
			ASSERT(0);	
			result = false;
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperControlGearCfgScenes::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		return true;
	}
	CString stringErrorDetail("Recieved event recieved backward response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlGearCfgScenes::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		CString stringErrorDetail("Recieved No backward response.");
		this->setFatalErrorInfo(ERROR_CODE_PROPERTY_NORESPONSE,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	CString stringErrorDetail("Recieved event backward no response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);			// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlGearCfgScenes::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		CString stringErrorDetail("Recieved event TX done but expected a backward response.");
		this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	else if (state  == STATE_TERMINATE) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_CFG_SCENES, -1, -1);
		this->setState(STATE_DONE);
		return true;	
	}
	else if ((state == STATE_SCENE_REMOVE) || (state == STATE_SCENE_LEVEL_STORE)) {

#if(0)
// TODO: 
// Moeten onderstaande opsplitsen.
// Scene nummer niet hier verhogen????

		// Mogen hier gewoon het scene number verhogen.
		// in de next() wordt er gecontroleerd of er nog data moet verstuurd worden.
		const int sceneNumber = this->getSceneNumber() + 1;
		this->setSceneNumber(sceneNumber);
#endif 

	}
	return true;
}

bool CAppDaliFsmHelperControlGearCfgScenes::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_QUERY_DEVICE_STATUS || state == STATE_QUERY_DEVICE_VERSION) {
		CString stringErrorDetail("Recieved a backward response error message.");
		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	CString stringErrorDetail("Recieved event backward response error but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);			// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlGearCfgSingleScene::CAppDaliFsmHelperControlGearCfgSingleScene(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
Base(refParent, refTransmitter){  } 

CAppDaliFsmHelperControlGearCfgSingleScene::~CAppDaliFsmHelperControlGearCfgSingleScene() { }

bool CAppDaliFsmHelperControlGearCfgSingleScene::start(int shortAddress, int sceneNumber, CAppDaliDeviceProperties_ControlGear* data) {	
	if (!this->isIdle() || NULL==data) {
		REQUIRE(NULL != data);
		ASSERT(0);
		return false;
	}

	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);
	this->clearCounterValue();
	this->setSceneNumber(sceneNumber);	
	this->setChannelNumber(0);
	this->setSupportDT8ColorObject(CAppDaliDeviceProperties_ControlGear::isDT8ColorObjectSupported(*data));
	this->setCfgData(data);	
	this->setStateDT8ColorObject(SUB_STATE_DT8_NONE);
	
	if (!this->next()) {
		// Oppassen: Indien deze functie false terug geeft moet de client de Config data dealloceren.
		// Indien deze functie true antwoord is deze worker verantwoordelijk voor de deallocatie.
		this->clearCfgData();
		return false;
	}
	return true;
}

bool CAppDaliFsmHelperControlGearCfgSingleScene::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperControlGearCfgSingleScene::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_CFG_SINGLE_SCENE, this->getSceneNumber(), -1);									  
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {			
				this->setState(STATE_ENTER_QUIESCENT_MODE);
			}										
		} break;	
		case STATE_ENTER_QUIESCENT_MODE: {				
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(0x00);			
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll( )\r\n");
				this->setState(STATE_INITIALISE); 
			}				
		} break;	/* STATE_ENTER_QUIESCENT_MODE */
		case STATE_INITIALISE: {			
			ASSERT((this->getSceneNumber() >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) && (this->getSceneNumber() <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST));	
			const bool removeScene = this->getRemoveScene(this->getSceneNumber());
			if (removeScene) {
				result = this->getTransmitter().sendControlGearRemoveFromScene(this->getShortAddress(),this->getSceneNumber());
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlGearDTR0( )\r\n");
					this->setState(STATE_SCENE_REMOVE);
				}	
			}
			else {
				if (this->getSupportDT8ColorObject()) {					
					const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
					if (CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(refColorData)) {
						result = this->sendDT8ColorTemperatureValueLSB();															
					}				
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData)) {
						result = this->sendDT8ColorCoordinateXValueLSB();								
					}
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData)) {
						result = this->sendDT8ColorDimlevelRed();
					}
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(refColorData)) {	
						this->setChannelNumber(0);
						result = this->sendDT8ColorChannelDimLevelLSB(this->getChannelNumber());
					}
					else { ASSERT(0); }	
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
					}
				}
				else {					
					const int scenelevel = this->getcfgData().getSceneLevel(this->getSceneNumber());
					result = this->sendSceneLevelValue(scenelevel);					
				}
			}				 										 
		} break;

		case STATE_SCENE_REMOVE: {
			bool sendDT8ColorObject = false;
			if (this->getSupportDT8ColorObject()) {
				const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
				if (CAppDaliObjectDT8ColorData_Utils::isActive(refColorData)) {					
					if (CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(refColorData)) {
						result = this->sendDT8ColorTemperatureValueLSB();															
					}	
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(refColorData)) {
						result = this->sendDT8ColorCoordinateXValueLSB();								
					}
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(refColorData)) {
						result = this->sendDT8ColorDimlevelRed();
					}
					else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(refColorData)) {
						this->setChannelNumber(0);
						result = this->sendDT8ColorChannelDimLevelLSB(this->getChannelNumber());
					}
					else { ASSERT(0); }	
					if (result) {
						sendDT8ColorObject = true;
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
					}
				}				
			}		
			if (!sendDT8ColorObject) {
				// Wanneer er geen DT8 color data moet verstuurd worden. Controle doen op de intensiteit.
				// Indien deze masked is dan kunnen we terminate versturen...
				const int scenelevel = this->getcfgData().getSceneLevel(this->getSceneNumber());
				if ((scenelevel < 0) || (scenelevel >= 255)) {
					result = this->sendTerminate();					
				}
				else {				
					result = this->sendSceneLevelValue(scenelevel);						
				}
			}
		} break;

		case STATE_SCENE_COLORVALUE_LOAD_DTR0: {
			ASSERT(this->getSupportDT8ColorObject());
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();
			switch(stateDT8ColorObject) {
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: {
					result = this->sendDT8ColorTemperatureValueMSB();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: {
					result = this->sendDT8ColorCoordinateXValueMSB();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_Y: {
					result = this->sendDT8ColorCoordinateYValueMSB();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					result = this->sendDT8ColorDimlevelGreen();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {
					result = this->sendDT8ColorDimlevelAmber();								   
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {
					result = this->sendDT8ColorChannelDimLevelMSB(this->getChannelNumber());									   
				} break;
				default: { ASSERT(0); } break;			
			}
			if (result) {
				this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR1);	
			}
		} break;

		case STATE_SCENE_COLORVALUE_LOAD_DTR1: {
			ASSERT(this->getSupportDT8ColorObject());
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();
			switch(stateDT8ColorObject) {
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: 
				case SUB_STATE_DT8_SET_COLORVALUE_X:
				case SUB_STATE_DT8_SET_COLORVALUE_Y: {						
					result = this->getTransmitter().sendControlGearEnableDeviceType(8);
					if (result) {
						this->setState(STATE_SCENE_ENABLE_DT8);
					}	
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					result = this->sendDT8ColorDimlevelBlue();	
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
					}
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {
					result = this->sendDT8ColorDimlevelFreeColor();		
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
					}
				} break;					
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {
					result = this->sendDT8ColorChannelNumber(this->getChannelNumber());								   
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR2);
					}
				} break;
				default: {
					ASSERT(0); 
				} break;
			}
		} break;

		case STATE_SCENE_COLORVALUE_LOAD_DTR2: {
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();
			switch(stateDT8ColorObject) {
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_Y: { ASSERT(0); } break;	
				
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: 
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: 
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {
					result = this->getTransmitter().sendControlGearEnableDeviceType(8);
					if (result) {
						this->setState(STATE_SCENE_ENABLE_DT8);
					}								   
				} break;
			}
		} break;

		case STATE_SCENE_ENABLE_DT8: {
			bool (CAppDaliTransmitter::*pf)(int shortAddress) = NULL;
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			switch(stateDT8ColorObject) {		 
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorTemp;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorCoordinateX;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_Y: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorCoordinateY;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorDimLevelRGB;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorDimLevelWAF;
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {				
					pf = &CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorPrimaryChannelN;
				} break;
			} /* end switch() */
			if (NULL != pf) {
				result = (this->getTransmitter().*pf)(this->getShortAddress());
				ASSERT(result);
				if (result) {
					this->setState(STATE_SCENE_COLORVALUE_SET);
				}						
			}									 
		} break;

		case STATE_SCENE_COLORVALUE_SET: {		
			const CAppDaliObjectDT8ColorData& refColorData = this->getcfgData().getDT8SceneColorInfo(this->getSceneNumber());
			const StateDT8ColorObject stateDT8ColorObject = this->getStateDT8ColorObject();			
			switch(stateDT8ColorObject) {		 
				case SUB_STATE_DT8_NONE: { ASSERT(0); } break;
				case SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC:
				case SUB_STATE_DT8_SET_COLORVALUE_Y:
				case SUB_STATE_DT8_SET_COLORVALUE_WAF: {				
					const int scenelevel = this->getcfgData().getSceneLevel(this->getSceneNumber());
					result = this->sendSceneLevelValue(scenelevel);
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_X: {
					result = this->sendDT8ColorCoordinateYValueLSB();
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
					}
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_RGB: {
					result = this->sendDT8ColorDimlevelWhite();	
					if (result) {
						this->setState(STATE_SCENE_COLORVALUE_LOAD_DTR0);
					}
				} break;
				case SUB_STATE_DT8_SET_COLORVALUE_CHANNEL: {	
					const int channelNumber = this->getChannelNumber() + 1;								
					this->setChannelNumber(channelNumber);
					if (channelNumber >= DT8_DALI_MAX_PRIMARY_CHANNELS) {
						const int scenelevel = this->getcfgData().getSceneLevel(this->getSceneNumber());						
						result = this->sendSceneLevelValue(scenelevel);
					}						
					else {
						result = this->sendDT8ColorChannelDimLevelLSB(channelNumber);		
					}
				} break;
			}										 										 
		} break;									
		case STATE_SCENE_LOAD_DTR: {
			ASSERT((this->getSceneNumber() >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) && (this->getSceneNumber() <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST));			   
			result = this->getTransmitter().sendControlGearAddToScene(this->getShortAddress(),this->getSceneNumber());
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlGearAddToScene( )\r\n");
				this->setState(STATE_SCENE_LEVEL_STORE);
			}			   
		} break;
		case STATE_SCENE_LEVEL_STORE: {
			result = this->sendTerminate();					
		} break;	
		case STATE_TERMINATE: {
			result = this->getTransmitter().stopQuienscentMode();
			ASSERT(result);
			if (result) {			
				this->setState(STATE_EXIT_QUIESCENT_MODE);
			}	
		} break;
		case STATE_EXIT_QUIESCENT_MODE: {										
			result = this->sendTerminate();								
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	} /* end switch() */
	return result;
}

// Alle events verwachten een ACK.
bool CAppDaliFsmHelperControlGearCfgSingleScene::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	CString stringErrorDetail("Recieved event recieved backward response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// Alle events verwachten een ACK.
bool CAppDaliFsmHelperControlGearCfgSingleScene::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	CString stringErrorDetail("Recieved event backward no response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);			// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

// Alle events verwachten een ACK.
bool CAppDaliFsmHelperControlGearCfgSingleScene::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	CString stringErrorDetail("Recieved event backward response error but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);			// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlGearCfgSingleScene::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state  == STATE_EXIT_QUIESCENT_MODE) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_CFG_SINGLE_SCENE, this->getSceneNumber(), -1);
		this->setState(STATE_DONE);
		return true;	
	}	
	return true;
}

// load DTR0 value
bool CAppDaliFsmHelperControlGearCfgSingleScene::sendSceneLevelValue(int value) {
	const bool result = this->getTransmitter().sendControlGearDTR0(value);
	ASSERT(result);
	if (result) {
		TRACER("TX: sendControlGearDTR0( )\r\n");
		this->setState(STATE_SCENE_LOAD_DTR);
	}	
	return result;
}

bool CAppDaliFsmHelperControlGearCfgSingleScene::sendTerminate() {
	const bool result = this->getTransmitter().sendControlgearBroadcastTerminate();
	ASSERT(result);
	if (result) {
		TRACER("TX: sendControlgearBroadcastTerminate( )\r\n");
		this->setState(STATE_TERMINATE);
	}	
	return result;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent,refTransmitter), m_instanceAddress(-1), m_stateOnOff(false), m_resultVerifyStateOnOff(false) { }

CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::~CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff( ) { } 

bool CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::start(int shortAddress, int instanceNumber, bool stateOnOff) {
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}

	// Basisklasse.
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	this->setShortAddress(shortAddress);
	this->setDaliDeviceType(DALI_DEVICE_TYPE_INPUTCONTROL);
	this->clearCounterValue();

	// Eigen members.
	this->m_instanceAddress = instanceNumber;
	this->m_stateOnOff = stateOnOff;
	this->m_resultVerifyStateOnOff = !stateOnOff;
	return this->next();
}

bool CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

// acties.
bool CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			this->getParent().onNotifyFsmAlgoritmeStarted(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF, this->m_instanceAddress, -1);									  
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceType(this->getShortAddress(), this->m_instanceAddress);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlInputDeviceInstanceType( )\r\n");
				this->setState(STATE_INSTANCE_QUERY_TYPE); 
			}
		} break;
		case STATE_INSTANCE_QUERY_TYPE: {
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(0x00);			
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll( )\r\n");
				this->setState(STATE_INITIALISE); 
			}
		} break;
		case STATE_INITIALISE: {										 															
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {
				TRACER("TX: startQuiescentMode( )\r\n");
				this->setState(STATE_ENTER_QUIESCENT_MODE); 
			}
		} break;
		
		case STATE_ENTER_QUIESCENT_MODE: {			
			if (this->m_stateOnOff) {
				result = this->getTransmitter().sendControlInputDeviceInstanceEnableInstance(this->getShortAddress(),this->m_instanceAddress);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlInputDeviceInstanceEnableInstance( )\r\n");
					this->setState(STATE_INSTANCE_ENABLE);
				}
			}
			else {
				result = this->getTransmitter().sendControlInputDeviceInstanceDisableInstance(this->getShortAddress(),this->m_instanceAddress);
				ASSERT(result);
				if (result) {
					TRACER("TX: sendControlInputDeviceInstanceEnableInstance( )\r\n");
					this->setState(STATE_INSTANCE_DISABLE);
				}
			}				
		} break;

		case STATE_INSTANCE_ENABLE:
		case STATE_INSTANCE_DISABLE:  {
			result = this->getTransmitter().sendQueryControlInputDeviceInstanceEnabled(this->getShortAddress(),this->m_instanceAddress);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendQueryControlInputDeviceInstanceEnabled( )\r\n");
				this->setState(STATE_INSTANCE_VERIFY);
			}									 
		} break;

		case STATE_INSTANCE_VERIFY: {
			result = this->getTransmitter().stopQuienscentMode();
			ASSERT(result);
			if (result) {
				TRACER("TX: stopQuiescentMode( )\r\n");
				this->setState(STATE_EXIT_QUIESCENT_MODE);
			}												
		} break;

		case STATE_EXIT_QUIESCENT_MODE: {
			result = this->getTransmitter().sendControlgearBroadcastTerminate();
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastTerminate( )\r\n");
				this->setState(STATE_TERMINATE);
			}																	
		} break;

		case STATE_TERMINATE: {
			ASSERT(0);	
			result = false;
		} break;

		case STATE_DONE:
		case STATE_ERROR_FATAL:
		case STATE_CANCEL:{
			ASSERT(STATE_DONE != state);
			ASSERT(STATE_ERROR_FATAL != state);
			ASSERT(STATE_CANCEL != state);
			result=false;
		} break;	
	}
	return result;
}

bool CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::onFsmEventResponse(CAppDaliFsmContext& refContext,int data) {
	const int state = this->getState();
	if (state == STATE_INSTANCE_VERIFY) { 
		this->m_resultVerifyStateOnOff = (data != 0 ? true : false);		
		return true;
	}
	else if (state == STATE_INSTANCE_QUERY_TYPE) {
		return true;
	}
	CString stringErrorDetail("Recieved event recieved backward response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_INSTANCE_VERIFY) {
		this->m_resultVerifyStateOnOff = false;	
		return true;
	}
	else if (state == STATE_INSTANCE_QUERY_TYPE) {
		CString stringErrorDetail;
		stringErrorDetail.Format("Control device A%d², failed to get info about instance number %d",this->getShortAddress(),this->m_instanceAddress);
		this->setFatalErrorInfo(ERROR_CODE_PROPERTY_NORESPONSE,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);		
		return false;
	}
	CString stringErrorDetail("Recieved event backward no response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_INSTANCE_VERIFY || state == STATE_INSTANCE_QUERY_TYPE) {
		CString stringErrorDetail("Recieved event TX done but expected a backward response.");
		this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);	// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	else if (state  == STATE_TERMINATE) {		
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF, this->m_instanceAddress, -1);
		this->setState(STATE_DONE);
		return true;	
	}
	return true;
}

bool CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (state == STATE_INSTANCE_VERIFY || state == STATE_INSTANCE_QUERY_TYPE) {
		CString stringErrorDetail("Recieved a backward response error message.");
		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,this->getShortAddress(),stringErrorDetail);
		this->setState(STATE_ERROR_FATAL);	// Fatal error omdat dit algoritme afzonderlijk uitgevoerd wordt.
		return false;
	}
	CString stringErrorDetail("Recieved event backward response error but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT,this->getShortAddress(),stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);	// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

CAppDaliFsmHelperSwapShortAddresses::CAppDaliFsmHelperSwapShortAddresses(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) : 
	Base(refParent, refTransmitter), m_address_atemp(-1), m_address_a0(-1), m_address_a1(-1), m_stateSuccess(false), m_verifyError(false), 
    m_errorNoTempAddressFound(false) {  }

bool CAppDaliFsmHelperSwapShortAddresses::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);	
	return true;
}

bool CAppDaliFsmHelperSwapShortAddresses::start(DaliDeviceType_t deviceType,int a0,int a1) {
	REQUIRE(a0 != a1);
	if (a0 < DALI_SHORTADDRESS_FIRST || a0 > DALI_SHORTADDRESS_LAST) {
		REQUIRE((a0 >= DALI_SHORTADDRESS_FIRST) && (a0<=DALI_SHORTADDRESS_LAST));
		return false;
	}
	if (a1 < DALI_SHORTADDRESS_FIRST || a1 > DALI_SHORTADDRESS_LAST) {
		REQUIRE((a1 >= DALI_SHORTADDRESS_FIRST) && (a1<=DALI_SHORTADDRESS_LAST));
		return false;
	}
	if (!this->isIdle()) {
		ASSERT(0);
		return false;
	}
		
	// Basisklasse.	
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);
	// Short address wordt hier gebruikt om een vrij adres te zoeken die tijdelijk kan gebruikt worden.
	if (DALI_DEVICE_TYPE_CONTROLGEAR == deviceType) {
		this->setShortAddress(DALI_SHORTADDRESS_LAST);		
	}
	else {
		this->setShortAddress(DALI_SHORTADDRESS_FIRST);
	}
	this->setDaliDeviceType(deviceType);
	this->clearCounterValue();
	
	// eigen members.
	this->m_address_atemp = -1;		// Om te starten op -1 zetten.
	this->m_address_a0 = a0;
	this->m_address_a1 = a1;
	this->m_verifyError = false;
	this->m_stateSuccess = false;		
	this->m_errorNoTempAddressFound = false;
	return this->next();
}

bool CAppDaliFsmHelperSwapShortAddresses::helper_sendQueryDeviceStatus(DaliDeviceType_t daliDeviceType, int address) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {				
		result = this->getTransmitter().sendQueryControlGearDeviceStatus(address);
		ASSERT(result);				
	}
	else {			
		result = this->getTransmitter().sendQueryInputControlDeviceStatus(address);
		ASSERT(result);
	}
	return result;
}

bool CAppDaliFsmHelperSwapShortAddresses::helper_sendBroadcastDTR0ShortAddress(DaliDeviceType_t daliDeviceType, int address) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {				
		const int valueDTR0 = (address << 1) + 1;
		result = this->getTransmitter().sendControlGearDTR0(valueDTR0);
		ASSERT(result);
	}
	else {
		const int valueDTR0 = address;
		result = this->getTransmitter().sendControlInputDevicesBroadcastDTR0(valueDTR0);
		ASSERT(result);
	}
	return result;
}

bool CAppDaliFsmHelperSwapShortAddresses::helper_sendAssignShortAddressDTR0(DaliDeviceType_t daliDeviceType, int address) {
	bool result = false;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {						
		result = this->getTransmitter().sendControlSetShortAddress(address);
		ASSERT(result);					
	}
	else {
		result = this->getTransmitter().sendControlInputDevicesSetShortAddressDTR0(address);
		ASSERT(result);					
	}
	return result;
}

// acties.
bool CAppDaliFsmHelperSwapShortAddresses::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
			IAppDaliFsm_HelperFsmEvents::AlgoritmeName_t name = IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP;			
			this->getParent().onNotifyFsmAlgoritmeStarted(daliDeviceType, this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP, this->m_address_a0, this->m_address_a1);			
			static const int dataByte = 0x00;	// Initialise (all)
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(dataByte);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll( )\r\n");
				this->setState(STATE_INITIALISE);
			}
		} break; /* STATE_READY */
		case STATE_INITIALISE: {
			result = this->getTransmitter().startQuiescentMode( );
			ASSERT(result);
			if (result) {
				TRACER("TX: startQuiescentMode( )\r\n");
				this->setState(STATE_ENTER_QUIESCENT_MODE);
			}									   
		} break;
		case STATE_ENTER_QUIESCENT_MODE: {	
			// Zoeken naar een adres die vrij is.
			const int shortAddress = this->getShortAddress();
			ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress<=DALI_SHORTADDRESS_LAST);
			result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),shortAddress); // this->m_address_atemp);			
			if (result) {				
				this->setState(STATE_SEARCH_FREE_ADDRESS);
			}
		} break;

		case STATE_SEARCH_FREE_ADDRESS: {	
			if (this->m_errorNoTempAddressFound) {
				ASSERT(this->m_address_atemp == -1);
				result = this->getTransmitter().sendControlgearBroadcastTerminate( );
				this->setFatalErrorInfo(ERROR_CODE_COMMISSIONING_SHORTADDRESS_EXCEED_MAX, -1,"No more short addresses available");
				if (result) {			
					this->m_stateSuccess = false;			
					this->m_verifyError = true;
					this->setState(STATE_TERMINATE);				
				}	
			}
			else if (this->m_address_atemp >= DALI_SHORTADDRESS_FIRST && this->m_address_atemp<=DALI_SHORTADDRESS_LAST) {
				result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_address_atemp);			
				if (result) {				
					this->setState(STATE_VERIFY_SHORTADDRESS_ATEMP_FREE);
				}		
			}
			else {
				const int shortAddress = this->getShortAddress();
				ASSERT(shortAddress >= DALI_SHORTADDRESS_FIRST && shortAddress<=DALI_SHORTADDRESS_LAST);
				result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),shortAddress);				
				if (result) {	
					// Blijven in deze toestand...
				}	
			}
		} break;
		
		case STATE_VERIFY_SHORTADDRESS_ATEMP_FREE: {
			if (this->m_verifyError) {
				result = this->getTransmitter().sendControlgearBroadcastTerminate( );
				if (result) {						
					this->setState(STATE_TERMINATE);				
				}	
			}
			else {
				// Opvragen address a_0 bezet?
				result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_address_a0);				
				if (result) {				
					this->setState(STATE_VERIFY_SHORTADDRESS_A0_USED);
				}								   												   
			}
		} break;
		case STATE_VERIFY_SHORTADDRESS_A0_USED: {
			if (this->m_verifyError) {
				result = this->getTransmitter().sendControlgearBroadcastTerminate( );
				if (result) {						
					this->setState(STATE_TERMINATE);				
				}	
			}
			else {
				// Opvragen address a_1 bezet?
				result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_address_a1);	
				if (result) {				
					this->setState(STATE_VERIFY_SHORTADDRESS_A1_USED);
				}	
			}
		} break;

// A0 wordt adres temp adres (START)
		case STATE_VERIFY_SHORTADDRESS_A1_USED: {	
			if (this->m_verifyError) {
				result = this->getTransmitter().sendControlgearBroadcastTerminate( );
				if (result) {						
					this->setState(STATE_TERMINATE);				
				}	
			}
			else {
				// A0 wordt adres temp adress: DTR Register laden met atemp
				result = helper_sendBroadcastDTR0ShortAddress(this->getDaliDeviceType(),this->m_address_atemp);
				if (result) {
					this->setState(STATE_DTR0_LOAD_ADDRESS_ATEMP);			
				}		
			}
		} break;
		case STATE_DTR0_LOAD_ADDRESS_ATEMP: {		
			// A0 wordt adres temp adress: A0 STORE DTR0 as address (atemp)
			result = this->helper_sendAssignShortAddressDTR0(this->getDaliDeviceType(),this->m_address_a0);
			if (result) {
				this->setState(STATE_A0_SHORTADDRESS_DTR0);			
			}	
		} break;		
		case STATE_A0_SHORTADDRESS_DTR0: {	
			if (this->m_verifyError) {
				result = this->getTransmitter().sendControlgearBroadcastTerminate( );
				if (result) {						
					this->setState(STATE_TERMINATE);				
				}	
			}
			else {
				// Controle atemp toegekend?
				result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_address_atemp);			
				if (result) {				
					this->setState(STATE_VERIFY_ADDRESS_ATEMP);
				}		
			}
		} break;
// A0 wordt adres temp adres (EINDE)


// A1 wordt adres 0
		case STATE_VERIFY_ADDRESS_ATEMP: {
			// A1 wordt adres 0: DTR Register laden met m_address_a0										 
			result = this->helper_sendBroadcastDTR0ShortAddress(this->getDaliDeviceType(),this->m_address_a0);
			if (result) {
				this->setState(STATE_DTR0_LOAD_ADDRESS_A0);			
			}							 
		} break;	
		case STATE_DTR0_LOAD_ADDRESS_A0: {
			// A1 toekennen adres a0 (A1 load DTR0);										 
			result = this->helper_sendAssignShortAddressDTR0(this->getDaliDeviceType(),this->m_address_a1);
			if (result) {
				this->setState(STATE_A1_SHORTADDRESS_DTR0);			
			}							 										 										 
		} break;

// A1 wordt adres 1
	

// Address temp wordt A1
		case STATE_A1_SHORTADDRESS_DTR0: {	
			// Address temp wordt A1: DTR Register laden met m_address_a1
			result = helper_sendBroadcastDTR0ShortAddress(this->getDaliDeviceType(),this->m_address_a1);
			if (result) {
				this->setState(STATE_DTR0_LOAD_ADDRESS_A1);			
			}						
		} break;
		case STATE_DTR0_LOAD_ADDRESS_A1: {
			// Address temp wordt A1: (A temp load DTR0);									 
			result = this->helper_sendAssignShortAddressDTR0(this->getDaliDeviceType(),this->m_address_atemp);
			if (result) {
				this->setState(STATE_ATEMP_SHORTADDRESS_DTR0);			
			}							 
		} break;	
// Address temp wordt A1 - done.

		case STATE_ATEMP_SHORTADDRESS_DTR0: {
			result = this->getTransmitter().sendControlgearBroadcastTerminate( );
			if (result) {						
				this->setState(STATE_TERMINATE);				
			}					
		} break;

		case STATE_TERMINATE: {
			if (this->m_verifyError) {
				result = this->getTransmitter().stopQuienscentMode();
				if (result) {
					TRACER("TX: stopQuienscentMode( )\r\n");					
					this->setState(STATE_EXIT_QUIESCENT_MODE);				
				}				
			}
			else {
				// Opvragen address a_0 bezet?
				result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_address_a0);				
				if (result) {				
					this->setState(STATE_VERIFY_ADDRESS_A0);
				}		
			}
		} break;
		case STATE_VERIFY_ADDRESS_A0: {
			if (this->m_verifyError) {
				result = this->getTransmitter().stopQuienscentMode();
				if (result) {
					TRACER("TX: stopQuienscentMode( )\r\n");					
					this->setState(STATE_EXIT_QUIESCENT_MODE);				
				}	
			}
			else {
				// Opvragen address a_1 bezet?
				result = this->helper_sendQueryDeviceStatus(this->getDaliDeviceType(),this->m_address_a1);				
				if (result) {				
					this->setState(STATE_VERIFY_ADDRESS_A1);
				}		
			}
		} break;
		case STATE_VERIFY_ADDRESS_A1: {
			// Opvragen address a_1 bezet?
			result = this->getTransmitter().stopQuienscentMode();
			if (result) {
				TRACER("TX: stopQuienscentMode( )\r\n");					
				this->setState(STATE_EXIT_QUIESCENT_MODE);				
			}										  
		} break;
		case STATE_EXIT_QUIESCENT_MODE: {			
			ASSERT(0);
		} break;
	}
	return result;
}

// Events van de FSM...
bool CAppDaliFsmHelperSwapShortAddresses::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	const int state = this->getState();
	if (STATE_SEARCH_FREE_ADDRESS == state) {
		int shortAddress = this->getShortAddress();
		ASSERT(this->m_address_atemp == -1);
		if (DALI_DEVICE_TYPE_CONTROLGEAR == this->getDaliDeviceType()) {
			if (++shortAddress <= DALI_SHORTADDRESS_LAST) {
				this->setShortAddress(shortAddress);
			}
			else {					
				this->m_errorNoTempAddressFound = true;
			}
		}
		else {
			if (--shortAddress >= DALI_SHORTADDRESS_FIRST) {
				this->setShortAddress(shortAddress);
			}
			else {
				this->m_errorNoTempAddressFound = true;
			}			
		}
		return true;
	}
	else if (STATE_VERIFY_SHORTADDRESS_ATEMP_FREE == state) {
		// Fout Temp adres is reeds toegekend aan een device.
		CString str;
		str.Format("Temporary address A%d is in use",this->m_address_atemp);
		this->setFatalErrorInfo(ERROR_CODE_PROPERTY_WRONGRESPONSE,this->m_address_atemp,str);
		this->m_verifyError = true;
		this->m_stateSuccess = false;
		return true;	
	}
	else if (STATE_VERIFY_SHORTADDRESS_A0_USED == state || STATE_VERIFY_SHORTADDRESS_A1_USED == state) {
		// ok adressen A0 en A1 zijn aanwezig in het systeem.
		return true;
	}
	else if (STATE_VERIFY_ADDRESS_A0 == state || STATE_VERIFY_ADDRESS_A1 == state) {		
		// ok adressen A0 en A1 zijn aanwezig in het systeem.
		this->m_stateSuccess = true;
		return true;
	}
	else if (STATE_VERIFY_ADDRESS_ATEMP == state) {
		// ok adressen atemp werd toegekend.
		return true;
	}
	// TODO: Verwachten geen antwoord in de andere toestanden.
	ASSERT(0);
	return false;
}

bool CAppDaliFsmHelperSwapShortAddresses::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_SEARCH_FREE_ADDRESS == state) {
		// Adres niet gevonden. Kunnen deze gebruiken als temp adres.
		this->m_address_atemp = this->getShortAddress();		
		ASSERT((this->m_address_atemp >= DALI_SHORTADDRESS_FIRST) && (this->m_address_atemp<=DALI_SHORTADDRESS_LAST));
		return true;
	}
	else if (STATE_VERIFY_SHORTADDRESS_ATEMP_FREE == state) {
		// ok adres atemp is niet toegkend en kan gebruikt worden.
		return true;
	}
	else if (STATE_VERIFY_SHORTADDRESS_A0_USED == state || STATE_VERIFY_SHORTADDRESS_A1_USED == state) {
		// Address is niet aanwezig in het systeem.
		const int address = (STATE_VERIFY_SHORTADDRESS_A0_USED == state ? this->m_address_a0 : this->m_address_a1);		
		CString str;
		str.Format("DALI device address A%d is not present - cannot swap addresses",address);
		this->setFatalErrorInfo(ERROR_CODE_DATABASE_DEVICE_NOT_FOUND,address,str);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), address, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP , ERROR_CODE_DATABASE_DEVICE_NOT_FOUND, -1);	
		this->m_verifyError = true;
		this->m_stateSuccess = false;
		return true;
	}
	else if (STATE_VERIFY_ADDRESS_A0 == state || STATE_VERIFY_ADDRESS_A1 == state) {
		// Address A0 werd niet toegekend.
		const int address = (STATE_VERIFY_ADDRESS_A0 == state ? this->m_address_a0 : this->m_address_a1);		
		CString str;
		str.Format("The device address A%d was not assigned to a device - failed to swap addresses",address);
		this->setFatalErrorInfo(ERROR_CODE_DATABASE_DEVICE_NOT_FOUND,address,str);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), address, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP , ERROR_CODE_DATABASE_DEVICE_NOT_FOUND, -1);	
		this->m_verifyError = true;
		this->m_stateSuccess = false;
		return true;	
	} 
	// TODO: Verwachten geen antwoord in de andere toestanden.
	ASSERT(0);
	return false;
}

bool CAppDaliFsmHelperSwapShortAddresses::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_EXIT_QUIESCENT_MODE == state) {	
		if (this->m_verifyError) {
			// TODO: Moeten we een onderscheid maken???
		}
		else {
			// TODO: Moeten we een onderscheid maken???
		}
		this->getParent().onNotifyFsmAlgoritmeDone(this->getDaliDeviceType(), this->getShortAddress(),IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP , this->m_address_a0,  this->m_address_a1);
		this->setState(STATE_DONE);
		return true;		
	}
	if ((STATE_INITIALISE == state) || (STATE_ENTER_QUIESCENT_MODE == state) || 
		(STATE_DTR0_LOAD_ADDRESS_ATEMP == state) || (STATE_A0_SHORTADDRESS_DTR0 == state) ||
		(STATE_DTR0_LOAD_ADDRESS_A0 == state) || (STATE_A1_SHORTADDRESS_DTR0 == state) ||
		(STATE_DTR0_LOAD_ADDRESS_A1 == state) || (STATE_ATEMP_SHORTADDRESS_DTR0 == state) ||
		(STATE_TERMINATE == state)) {	
		return true;
	}
#if(0)
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, -1, "Expected a backward response but recieved a tx-done event for the DALI controller");
	this->m_state = STATE_ERROR_FATAL;		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
#endif 
	return true;
}

bool CAppDaliFsmHelperSwapShortAddresses::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_VERIFY_SHORTADDRESS_ATEMP_FREE == state) {
		CString str;
		str.Format("An error occurred while verifying the temporary address A%d. This address is possible in use by more then one device",this->m_address_atemp);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->m_address_atemp, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP , ERROR_CODE_SHORTADDRESS_DOUBLE, -1);	
		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,this->m_address_atemp,str);			
		this->m_stateSuccess = false;		
		this->m_verifyError = true;
		return true;
	}
	else if (STATE_VERIFY_SHORTADDRESS_A0_USED == state || STATE_VERIFY_SHORTADDRESS_A1_USED == state) {
		const int address = (STATE_VERIFY_SHORTADDRESS_A0_USED == state ? this->m_address_a0 : this->m_address_a1);
		CString str;
		str.Format("An error occurred while verifying the address A%d. This address is possible in use by more then one device",address);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), address, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP , ERROR_CODE_SHORTADDRESS_DOUBLE, -1);	
		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,address,str);			
		this->m_stateSuccess = false;		
		this->m_verifyError = true;
		return true;	
	}	
	else if (STATE_VERIFY_ADDRESS_ATEMP == state) {
		CString str;
		str.Format("An error occurred while assigning a temporary address to device A%d.",this->m_address_atemp);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), this->m_address_atemp, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP , ERROR_CODE_SHORTADDRESS_DOUBLE, -1);	
		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,this->m_address_atemp,str);			
		this->m_stateSuccess = false;		
		this->m_verifyError = true;
		return true;
	}
	else if (STATE_VERIFY_ADDRESS_A0 == state || STATE_VERIFY_ADDRESS_A1 == state) {
		const int address = (STATE_VERIFY_ADDRESS_A0 == state ? this->m_address_a0 : this->m_address_a1);
		CString str;
		str.Format("An error occurred while assigning a address to device A%d.",address);
		this->getParent().onNotifyFsmAlgoritmeError(this->getDaliDeviceType(), address, IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_SHORTADDRESSES_SWAP , ERROR_CODE_SHORTADDRESS_DOUBLE, -1);	
		this->setFatalErrorInfo(ERROR_CODE_SHORTADDRESS_DOUBLE,address,str);			
		this->m_stateSuccess = false;		
		this->m_verifyError = true;
		return true;
	}
	// Todo - foutmelding?
	ASSERT(0);
	return false;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmHelperSelectDimmingCurve::CAppDaliFsmHelperSelectDimmingCurve(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter) :
	Base(refParent, refTransmitter), m_stateSuccess(false), m_deviceType(6), m_dimmerCurve(0) { }

bool CAppDaliFsmHelperSelectDimmingCurve::cancel() {
	this->cancelTask();
	this->setState(STATE_CANCEL);
	return true;
}

bool CAppDaliFsmHelperSelectDimmingCurve::start(int shortAddress, int deviceType, int dimmerCurve) {
	// Basisklasse.	
	this->getFatalErrorInfo().clear();
	this->setState(STATE_READY);	
	this->setShortAddress(shortAddress);	
	this->setDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR);

	// Eigen members.
	this->m_deviceType = deviceType;
	this->m_dimmerCurve = dimmerCurve;
	this->m_stateSuccess = false;
	return this->next();
}

// acties.
bool CAppDaliFsmHelperSelectDimmingCurve::next() {
	bool result = false;
	const int state = this->getState();
	switch (state) {
		case STATE_READY: {
			const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
			this->getParent().onNotifyFsmAlgoritmeStarted(daliDeviceType, this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_SELECT_DIMMINGCURVE,-1,-1);
			static const int dataByte = 0x00;	// Initialise (all)
			result = this->getTransmitter().sendControlgearBroadcastInitialiseAll(dataByte);
			ASSERT(result);
			if (result) {
				TRACER("TX: sendControlgearBroadcastInitialiseAll( )\r\n");
				this->setState(STATE_INITIALISE);
			}
		} break; /* STATE_READY */
		case STATE_INITIALISE: {
			result = this->getTransmitter().startQuiescentMode();
			ASSERT(result);
			if (result) {
				TRACER("TX: startQuiescentMode( )\r\n");
				this->setState(STATE_ENTER_QUIESCENT_MODE);
			}
		} break;
		case STATE_ENTER_QUIESCENT_MODE: {
			result = this->getTransmitter().sendControlGearDTR0(this->m_dimmerCurve);
			if (result) {
				this->setState(STATE_DIMMING_CURVE_LOAD_DTR0);
			}
		} break;
		case STATE_DIMMING_CURVE_LOAD_DTR0: {
			// 
			result = this->getTransmitter().sendControlGearEnableDeviceType(6); // this->m_deviceType);
			if (result) {
				this->setState(STATE_ENABLE_DEVICE_TYPE);
			}
		} break;
		case STATE_ENABLE_DEVICE_TYPE: {
			result = this->getTransmitter().sendControlGearDT6SelectDimmerCurve(this->getShortAddress());
			if (result) {
				this->setState(STATE_DIMMING_CURVE_SELECT);
			}
		} break;
		case STATE_DIMMING_CURVE_SELECT: {
			result = this->getTransmitter().sendControlgearBroadcastTerminate();
			if (result) {
				this->setState(STATE_TERMINATE);
			}
		} break;
		case STATE_TERMINATE: {
			result = this->getTransmitter().stopQuienscentMode();
			if (result) {
				TRACER("TX: stopQuienscentMode( )\r\n");
				this->setState(STATE_EXIT_QUIESCENT_MODE);
			}
		} break;
		case STATE_EXIT_QUIESCENT_MODE: {
			ASSERT(0);
		} break;
	}
	return true;
}

bool CAppDaliFsmHelperSelectDimmingCurve::onFsmEventResponse(CAppDaliFsmContext& refContext, int data) {
	CString stringErrorDetail("Recieved event backward response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperSelectDimmingCurve::onFsmEventNoResponse(CAppDaliFsmContext& refContext) {
	CString stringErrorDetail("Recieved event backward no response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}

bool CAppDaliFsmHelperSelectDimmingCurve::onFsmEventTxDone(CAppDaliFsmContext& refContext) {
	const int state = this->getState();
	if (STATE_EXIT_QUIESCENT_MODE == state) {
		const DaliDeviceType_t daliDeviceType = this->getDaliDeviceType();
		this->m_stateSuccess = true;
		this->getParent().onNotifyFsmAlgoritmeDone(daliDeviceType, this->getShortAddress(), IAppDaliFsm_HelperFsmEvents::ALGORITME_NAME_CONTROLGEAR_SELECT_DIMMINGCURVE, -1, -1);
		this->setState(STATE_DONE);
		return true;
	}
	return true;
}

bool CAppDaliFsmHelperSelectDimmingCurve::onFsmEventResponseError(CAppDaliFsmContext& refContext) {
	CString stringErrorDetail("Recieved event backward error response but expected a TX done event.");
	this->setFatalErrorInfo(ERROR_CODE_WRONG_EVENT, this->getShortAddress(), stringErrorDetail);
	this->setState(STATE_ERROR_FATAL);		// errorCode=ERROR_CODE_WRONG_EVENT
	return false;
}
/***********************************************************************************/
};
};
/***********************************************************************************/
