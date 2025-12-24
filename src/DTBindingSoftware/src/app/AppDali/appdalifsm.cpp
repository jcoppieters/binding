#include "stdafx.h"
#include "appdalifsm.h"
#include "appdalitransmitter.h"
#include "appdaliprotocol_def.h"

/***********************************************************************************/
#define DEBUG_LOCAL_ON		1
/***********************************************************************************/
#if (DEBUG_LOCAL_ON==1)
	#define TRACER(x)						TRACE(x)
	#define TRACER_FSM(refContext,ev)		dumpFsm(refContext,ev)
#else
	#define TRACER(x ...)
	#define TRACER_FSM(refContext,ev)
#endif
/***********************************************************************************/

/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
static const char* toString(Task_t task, State_t state) {
	switch(state) {
		case TASK_SEARCH_DEVICES_STATE_INIT:
		//case TASK_COMMISSIONING_STATE_INIT:
			return "STATE_INIT";
		case TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_SEARCH_BROADCAST:
			return "STATE_CONTROLGEAR_SEARCH_BROADCAST";
		case TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_SEARCH_DEVICE:
			return "STATE_CONTROLGEAR_SEARCH_DEVICE";
		case TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_REQUEST_PROPERTIES:	
			return "STATE_CONTROLGEAR_REQUEST_PROPERTIES";
		case TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_READ_MEMORYMAP:
			return "STATE_CONTROLGEAR_READ_MEMORYMAP";
		case TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_GROUPS:
			return "STATE_CONTROLGEAR_QUERY_GROUPS";
		case TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_SCENELEVELS:
			return "STATE_CONTROLGEAR_QUERY_SCENELEVELS";
		case TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_WAIT_DONE:
			return "STATE_CONTROLGEAR_WAIT_DONE";
		case TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_BROADCAST:
			return "STATE_CONTROLDEVICES_SEARCH_BROADCAST";
		case TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_DEVICE:
			return "STATE_CONTROLDEVICES_SEARCH_DEVICE";
		case TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_REQUEST_PROPERTIES:
			return "STATE_CONTROLDEVICES_REQUEST_PROPERTIES";				
		case TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_READ_MEMORYMAP:
			return "STATE_CONTROLDEVICES_READ_MEMORYMAP";
		case TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_REQUEST_INSTANCES:
			return "STATE_CONTROLDEVICES_REQUEST_INSTANCES";
		case TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_WAIT_DONE:
			return "STATE_CONTROLDEVICES_WAIT_DONE";
		case TASK_COMMISSIONING_STATE_CONTROLGEAR_ERASE:
			return "TASK_COMMISSIONING_STATE_CONTROLGEAR_ERASE";
		case TASK_COMMISSIONING_STATE_CONTROLGEAR_SEARCH_UNADDRESSED:
			return "TASK_COMMISSIONING_STATE_CONTROLGEAR_SEARCH_UNADDRESSED";
		case TASK_COMMISSIONING_STATE_CONTROLGEAR_ADDRESS:
			return "TASK_COMMISSIONING_STATE_CONTROLGEAR_ADDRESS";
		case TASK_COMMISSIONING_STATE_CONTROLDEVICES_SEARCH_UNADDRESSED:
			return "TASK_COMMISSIONING_STATE_CONTROLDEVICES_SEARCH_UNADDRESSED";
		case TASK_COMMISSIONING_STATE_CONTROLDEVICES_ADDRESS:
			return "TASK_COMMISSIONING_STATE_CONTROLDEVICES_ADDRESS";
		case TASK_COMMISSIONING_STATE_CONTROLDEVICES_INSTANCES_AUTOCFG:
			return "TASK_COMMISSIONING_STATE_CONTROLDEVICES_INSTANCES_AUTOCFG";
		case TASK_ADDRESSING_SINGLE_DEVICE:
			return "TASK_ADDRESSING_SINGLE_DEVICE";	
		case TASK_ADDRESSING_DEVICES_SWAPADDRESSES:
			return "TASK_ADDRESSING_DEVICES_SWAPADDRESSES";
		case TASK_CONFIG_CONTROLGEAR_PROPERTIES:
			return "TASK_CONFIG_CONTROLGEAR_PROPERTIES";
		case TASK_CONFIG_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF:
			return "TASK_CONFIG_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF";
		case TASK_CONFIG_CONTROLDEVICE_INSTANCES:
			return "TASK_CONFIG_CONTROLDEVICE_INSTANCES";
		case TASK_CONTROLGEAR_QUERY_ACTUAL_VALUE:
			return "TASK_CONTROLGEAR_QUERY_ACTUAL_VALUE";
	} /* end switch(state) */
	return "TASK_STATE_UNKNOWN";
}
/***********************************************************************************/

#if (DEBUG_LOCAL_ON==1)
	
static void dumpFsm(const CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {	
	CString str;
	str.Format("FSM info: state=%s, event=%s, shortAddress=%d",
		toString(refContext.getTask(), refContext.getState()), ev.toString(), refContext.getShortAddress());	
	TRACER(str);
}

#endif 
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
IAppDaliFsm_HelperFsmEvents::~IAppDaliFsm_HelperFsmEvents( ) { }


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmTimer::CAppDaliFsmTimer( ) : m_state(STATE_IDLE), m_interval(-1) { }
CAppDaliFsmTimer::~CAppDaliFsmTimer() { }
		

void CAppDaliFsmTimer::start(int reloadValue) {
	REQUIRE(reloadValue>0);
	this->m_interval = reloadValue;
	this->m_state = STATE_RUNNING;
}

void CAppDaliFsmTimer::stop( ) {
	this->m_interval = -1;
	this->m_state = STATE_IDLE;
}
	
BOOL CAppDaliFsmTimer::tick(int interval) {
	if (STATE_RUNNING != this->m_state) {
		return FALSE;
	}
	
	this->m_interval -= interval;
	if (this->m_interval <=0) {
		this->m_state = STATE_EXPIRED;
	}	
	return TRUE;
}

BOOL CAppDaliFsmTimer::running( ) {
	return (this->m_state == STATE_RUNNING ? TRUE : FALSE);
}
	
BOOL CAppDaliFsmTimer::expired( ) {
	return (this->m_state == STATE_EXPIRED ? TRUE : FALSE);
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmEvent::CAppDaliFsmEvent(Event_t ev) : m_event(ev) { }
CAppDaliFsmEvent::~CAppDaliFsmEvent( ) { }

const char* CAppDaliFsmEvent::toString( ) const {
	switch(this->m_event) {
		case EVENT_UNKNOWN:
			return "EVENT_UNKNOWN";
		case EVENT_BACKWARD_RESPONSE:
			return "EVENT_BACKWARD_RESPONSE";
		case EVENT_BACKWARD_RESPONSE_NO_RESPONSE:
			return "EVENT_BACKWARD_RESPONSE_NO_RESPONSE";
		case EVENT_BACKWARD_RESPONSE_ERROR:
			return "EVENT_BACKWARD_RESPONSE_ERROR";
		case EVENT_TX_DONE:
			return "EVENT_TX_DONE";
		case EVENT_TIMER_EXPIRY:
			return "EVENT_TIMER_EXPIRY";
		case EVENT_CANCEL:
			return "EVENT_CANCEL";
	}
	ASSERT(0);
	return "EVENT_UNKNOWN";
}

/***********************************************************************************/
CAppDaliFsmEvent_BackwardResponse::CAppDaliFsmEvent_BackwardResponse(uint8_t data) : 
	Base(EVENT_BACKWARD_RESPONSE), m_data(data) { }

CAppDaliFsmEvent_BackwardResponse::~CAppDaliFsmEvent_BackwardResponse( ) { }

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const CAppDaliFsm::PF_STATE_HANDLERS CAppDaliFsm::m_handlers[TASK_COMMISSIONING_STATES_MAX] = {
	&CAppDaliFsm::stateInit,														// STATE_INIT
	&CAppDaliFsm::taskSearchDevices_enter,											// STATE_SEARCH_DEVICES
	&CAppDaliFsm::taskSearchDevices_stateSearchContolGearBroadcast,					// STATE_CONTROLGEAR_SEARCH_BROADCAST
	&CAppDaliFsm::taskSearchDevices_stateControlGearDeviceSearchContolGear,			// STATE_CONTROLGEAR_SEARCH_DEVICE
	&CAppDaliFsm::taskSearchDevices_stateControlGearDeviceRequestDeviceProperties,	// STATE_CONTROLGEAR_REQUEST_PROPERTIES
	&CAppDaliFsm::taskSearchDevices_stateControlGearReadMemorymap,					// STATE_CONTROLGEAR_READ_MEMORYMAP
	&CAppDaliFsm::taskSearchDevices_stateControlGearDeviceQueryGroups,				// STATE_CONTROLGEAR_QUERY_GROUPS
	&CAppDaliFsm::taskSearchDevices_stateControlGearDeviceQueryScenes,				// STATE_CONTROLGEAR_QUERY_SCENELEVELS
	&CAppDaliFsm::taskSearchDevices_stateWaitControlGearDone,						// STATE_CONTROLGEAR_WAIT_DONE
	&CAppDaliFsm::taskSearchDevices_stateSearchBroadcastControlInputDevices,		// STATE_CONTROLDEVICES_SEARCH_BROADCAST
	&CAppDaliFsm::taskSearchDevices_stateSearchControlInputDevice,					// STATE_CONTROLDEVICES_SEARCH_DEVICE = 7,
	&CAppDaliFsm::taskSearchDevices_stateControlInputDeviceRequestDeviceProperties,	// STATE_CONTROLDEVICES_REQUEST_PROPERTIES = 8
	&CAppDaliFsm::taskSearchDevices_stateReadMemorymapControlInputDevice,			// STATE_CONTROLDEVICES_READ_MEMORYMAP = 9
	&CAppDaliFsm::taskSearchDevices_stateControlInputDeviceRequestInstances,		// STATE_CONTROLDEVICES_REQUEST_INSTANCES
	&CAppDaliFsm::taskSearchDevices_stateWaitControlInputDevicesDone,				// STATE_CONTROLDEVICES_WAIT_DONE = 10,												// STATE_INIT
	&CAppDaliFsm::taskCommissioning_stateControlGearErase,							// TASK_COMMISSIONING_STATE_CONTROLGEAR_ERASE
	&CAppDaliFsm::taskCommissioning_stateControlGearSearchUnAddressed,				// TASK_COMMISSIONING_STATE_CONTROLGEAR_SEARCH_UNADDRESSED
	&CAppDaliFsm::taskCommissioning_stateControlGearAddress,						// TASK_COMMISSIONING_STATE_CONTROLGEAR_ADDRESS
	&CAppDaliFsm::taskCommissioning_stateControlDevicesErase,						// TASK_COMMISSIONING_STATE_CONTROLDEVICES_ERASE
	&CAppDaliFsm::taskCommissioning_stateControlDevicesSearchUnAddressed,			// TASK_COMMISSIONING_STATE_CONTROLDEVICES_SEARCH_UNADDRESSED
	&CAppDaliFsm::taskCommissioning_stateControlDevicesAddress,						// TASK_COMMISSIONING_STATE_CONTROLDEVICES_ADDRESS
	&CAppDaliFsm::taskCommissioning_stateControlDevicesInstanceAutoCfgParams,		// TASK_COMMISSIONING_STATE_CONTROLDEVICES_INSTANCES_AUTOCFG
	&CAppDaliFsm::taskCommissioning_stateAddressSingleDevice,						// TASK_ADDRESSING_SINGLE_DEVICE
	&CAppDaliFsm::taskCommissioning_stateSwapDeviceAddresses,						// TASK_ADDRESSING_DEVICES_SWAPADDRESSES
	&CAppDaliFsm::taskConfig_stateControlGearCfgProperties,							// TASK_CONFIG_CONTROLGEAR_PROPERTIES
	&CAppDaliFsm::taskConfig_stateControlGearCfgScenes,								// TASK_CONFIG_CONTROLGEAR_SCENES
	&CAppDaliFsm::taskConfig_stateControlGearCfgSingleScene,						// TASK_CONFIG_CONTROLGEAR_SINGLE_SCENE
	&CAppDaliFsm::taskConfig_stateControlGearCfgDT6DimmerCurve,						// TASK_CONFIG_CONTROLGEAR_DIMMERCURVE
	&CAppDaliFsm::taskConfig_stateControlDeviceInstanceEnableOnOff,					// TASK_CONFIG_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF
	&CAppDaliFsm::taskConfig_stateControlDeviceInstances,							// TASK_CONFIG_CONTROLDEVICE_INSTANCES
	&CAppDaliFsm::taskConfig_stateControlGearQueryActualLevel,						// TASK_CONTROLGEAR_QUERY_ACTUAL_VALUE	
};
/***********************************************************************************/

/***********************************************************************************/
CAppDaliFsm::CAppDaliFsm(IAppDaliFsm_Parent& refParent,CAppDaliTransmitter& refTransmitter) : 
	m_refParent(refParent), m_refTransmitter(refTransmitter), m_timer(), m_pAlgSearchDevices(NULL), m_pAlgControlGearReadMemory(NULL), 
	m_pAlgInputDeviceReadMemory(NULL), m_pAlgQueryControlGearGroups(NULL), m_pAlgQueryControlGearScenes(NULL), m_pAlgSearchControlGearDevice(NULL),
	m_pAlgSearchControlInputDevices(NULL), m_pAlgQueryControlGearDeviceProperties(NULL), m_pAlgQueryInputDeviceProperties(NULL), m_pAlgQueryInputDeviceInstances(NULL),
	m_pAlgSearchUnaddressedDevices(NULL), m_pAlgAddressDevices(NULL), m_pAlgControlGearEraseAddresses(NULL), m_pAlgControlInputDeviceEraseAddresses(NULL),
	m_pAlgControlDeviceInstanceAutoParams(NULL), m_pAlgAddressSingleDevice(NULL), m_pAlgCfgControlGearProperties(NULL), m_pAlgControlGearQueryActualLevel(NULL),
	m_pAlgControlGearCfgScenes(NULL), m_pAlgControlGearCfgSingleScene(NULL), m_pAlgControlDeviceCfgInstanceEnableOnOff(NULL), m_pAlgControlDeviceCfgInstances(NULL), 
	m_pAlgSwapShortAddresses(NULL), m_pAlgSelectDimmingCurve(NULL)
{
}

CAppDaliFsm::~CAppDaliFsm( ) { 
	this->stopTimer();				
	this->cleanUp();
}

bool CAppDaliFsm::startSearchDevices(CAppDaliFsmContext& refContext) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	if (false == this->getAlgSearchDevices().start()) {
		return false;
	}
	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_SEARCH_DEVICES;
	refContext.m_state = TASK_SEARCH_DEVICES_STATE_SEARCH_DEVICES;
	refContext.m_shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(-1, "Search for DALI devices");
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startAddressing(CAppDaliFsmContext& refContext, AddressingInstallation_t type)
{
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}

	if (APPDALI_ADDRESSING_INSTALLATION_EXTEND == type) {
		if (false == this->getAlgDaliDevicesSearchUnaddressed().start(DALI_DEVICE_TYPE_CONTROLGEAR)) {
			return false;
		}
			
		refContext.clearStats();
		refContext.m_task = APPDALI_FSM_TASK_COMMISSIONING_EXTEND;
		refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLGEAR_SEARCH_UNADDRESSED;
		refContext.m_shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;
		refContext.m_stateCounter = 0;

		// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
		// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
		this->startTimer(refContext);

		m_refParent.notifySetProgessInfo(-1, "Start commissioning control gear devices, extend installation.");
		m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	}
	else if (APPDALI_ADDRESSING_INSTALLATION_NEW == type) {
		if (false == this->getAlgControlGearEraseAddresses().start()) {
			return false;
		}
			
		refContext.clearStats();
		refContext.m_task = APPDALI_FSM_TASK_COMMISSIONING_NEW;
		refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLGEAR_ERASE;
		refContext.m_shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;
		refContext.m_stateCounter = 0;

		// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
		// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
		this->startTimer(refContext);

		m_refParent.notifySetProgessInfo(-1, "Start commissioning control gear devices, new installation.");
		m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	}
	else {
		return false;
	}
	return true;
}

bool CAppDaliFsm::startAddressSingleDevice(CAppDaliFsmContext& refContext, DaliDeviceType_t type, int shortAddress, int newShortAddress) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}

	if (false == this->getAlgAssingShortAddress().start(type,shortAddress,newShortAddress)) {
		return false;
	}

	CString strInfo;
	Task_t task = APPDALI_FSM_TASK_ADDRESSING_SINGLE_DEVICE;
	if ((newShortAddress >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (newShortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST)) {
		task = APPDALI_FSM_TASK_ADDRESSING_SINGLE_DEVICE;
		strInfo.Format("Device A%d assign new short address A%d",shortAddress,newShortAddress);
	}
	else {
		task = APPDALI_FSM_TASK_ADDRESSING_ERASE_DEVICE;
		strInfo.Format("Device A%d erase short address",shortAddress);
	}

	refContext.clearStats();
	refContext.m_task = task;
	refContext.m_state = TASK_ADDRESSING_SINGLE_DEVICE;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(-1, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startAddressingSwapDeviceAddresses(CAppDaliFsmContext& refContext, DaliDeviceType_t type, int address_a0, int address_a1) { 
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}

	if (false == this->getAlgSwapShortAddresses().start(type,address_a0,address_a1)) {
		return false;
	}

	CString strInfo;
	strInfo.Format("Swap devices addresses A%d and A%d",address_a0,address_a1);

	Task_t task = APPDALI_FSM_TASK_ADDRESSING_SWAP_DEVICES;

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_ADDRESSING_SWAP_DEVICES;
	refContext.m_state = TASK_ADDRESSING_DEVICES_SWAPADDRESSES;
	refContext.m_shortAddress = address_a0;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(-1, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startUploadControlGearDeviceProperties(CAppDaliFsmContext& refContext, int shortAddress, CAppDaliDeviceProperties_ControlGear* properties) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	if (NULL == properties) {
		REQUIRE(NULL != properties);
		return false;
	}	
	if (false == this->getAlgCfgControlGearProperties().start(shortAddress,properties)) {
		return false;
	}

	CString strInfo;
	strInfo.Format("Config control gear device A%d properties",shortAddress);
		
	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_PROPERTIES;
	refContext.m_state = TASK_CONFIG_CONTROLGEAR_PROPERTIES;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startUploadControlGearDeviceScenes(CAppDaliFsmContext& refContext, int shortAddress, CAppDaliDeviceProperties_ControlGear* properties) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	if (NULL == properties) {
		REQUIRE(NULL != properties);
		return false;
	}	
	if (false == this->getAlgControlGearCfgScenes().start(shortAddress,properties)) {
		return false;
	}

	CString strInfo;
	strInfo.Format("Config control gear device A%d scenes",shortAddress);

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_SCENES;
	refContext.m_state = TASK_CONFIG_CONTROLGEAR_SCENES;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startUploadControlGearDeviceSingleScene(CAppDaliFsmContext& refContext, int shortAddress, int sceneNumber, CAppDaliDeviceProperties_ControlGear* properties) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	if (NULL == properties) {
		REQUIRE(NULL != properties);
		return false;
	}	
	if (false == this->getAlgControlGearCfgSingleScene().start(shortAddress,sceneNumber,properties)) {
		return false;
	}

	CString strInfo;
	if (0xFF == shortAddress) {
		strInfo.Format("Config scene %d for all control gear devices", sceneNumber);
	}
	else {
		strInfo.Format("Config control gear device A%d scene %d", shortAddress, sceneNumber);
	}

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_SCENES;
	refContext.m_state = TASK_CONFIG_CONTROLGEAR_SINGLE_SCENE;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;

}

bool CAppDaliFsm::startUploadControlGearDimmerCurve(CAppDaliFsmContext& refContext, int shortAddress, int deviceType,int dimmerCurve) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}	
	if (false == this->getAlgSelectDimmingCurve().start(shortAddress,deviceType,dimmerCurve)) {
		return false;
	}

	CString strInfo;
	strInfo.Format("Config control gear device A%d dimmercurve",shortAddress);

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_DIMMERCURVE;
	refContext.m_state = TASK_CONFIG_CONTROLGEAR_DIMMERCURVE;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

// Moeten geen progressinfo naar de parent versturen.
bool CAppDaliFsm::startRequestControlGearActualValue(CAppDaliFsmContext& refContext, int shortAddress) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	if (false == this->getAlgCfgControlGearActualValueRequest().start(shortAddress)) {
		return false;
	}

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_QUERY_ACTUAL_LEVEL;
	refContext.m_state = TASK_CONTROLGEAR_QUERY_ACTUAL_VALUE;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;
#if(0)	// Geen progress info.
	CString strInfo;
	strInfo.Format("Query control gear device A%d actual level.", shortAddress);
	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
#endif

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startUploadControlInputDeviceInstanceEnableOnOff(CAppDaliFsmContext& refContext, int shortAddress,int instanceNumber,bool stateOnOff) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	if (false == this->getAlgControlDeviceInstanceEnableOnOff().start(shortAddress, instanceNumber, stateOnOff)) {
		return false;
	}

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF;
	refContext.m_state = TASK_CONFIG_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;
#if(0)	// Geen progress info.
	CString strInfo;
	strInfo.Format("Query control gear device A%d actual level.", shortAddress);
	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
#endif

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startUploadControlInputDeviceInstances(CAppDaliFsmContext& refContext, int shortAddress, CAppDaliInputControlDeviceInstances* properties) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	if (NULL == properties) {
		REQUIRE(NULL != properties);
		return false;
	}	
	if (false == this->getAlgControlInputDeviceInstancesCfg().start(shortAddress,properties)) {
		return false;
	}

	CString strInfo;
	strInfo.Format("Config control input device A%d properties",shortAddress);
	
	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCES;
	refContext.m_state = TASK_CONFIG_CONTROLDEVICE_INSTANCES;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startReadControlGearDeviceProperties(CAppDaliFsmContext& refContext, int shortAddress) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	static const bool ALGORITME_COMMISSIONING_NO = false;
	if (false == this->getAlgControlGearRequestProperties().start(shortAddress, ALGORITME_COMMISSIONING_NO)) {
		return false;
	}

	CString strInfo;
	strInfo.Format("Read control gear A%d properties", shortAddress);

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_PROPERTIES;
	refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_REQUEST_PROPERTIES;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startReadControlGearDeviceGroups(CAppDaliFsmContext& refContext, int shortAddress) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}
	static const bool ALGORITME_COMMISSIONING_NO = false;
	if (false == this->getAlgControlGearQueryGroups().start(shortAddress, ALGORITME_COMMISSIONING_NO)) {
		return false;
	}

	CString strInfo;
	strInfo.Format("Read control gear A%d groups", shortAddress);

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_GROUPS;
	refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_GROUPS;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}

bool CAppDaliFsm::startReadControlGearDeviceScenes(CAppDaliFsmContext& refContext, int shortAddress, bool requestDT8ColorInfo) {
	if (refContext.m_task != APPDALI_FSM_TASK_NONE) {
		return false;
	}

	static const bool ALGORITME_COMMISSIONING_NO = false;
	if (false == this->getAlgControlGearQueryScenes().start(shortAddress, requestDT8ColorInfo, ALGORITME_COMMISSIONING_NO)) {
		return false;
	}
	CString strInfo;
	strInfo.Format("Read control gear A%d scenes", shortAddress);

	refContext.clearStats();
	refContext.m_task = APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_SCENES;		
	refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_SCENELEVELS;
	refContext.m_shortAddress = shortAddress;
	refContext.m_stateCounter = 0;

	// Oppassen: Timer hier starten want is afhankelijk van de taak in de Fsm context klasse.
	// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	this->startTimer(refContext);

	m_refParent.notifySetProgessInfo(shortAddress, strInfo);
	m_refParent.notifyDaliCfgFsmStarted(refContext.m_task);
	return true;
}


bool CAppDaliFsm::cancelTask(CAppDaliFsmContext& refContext) {
	if (refContext.m_task == APPDALI_FSM_TASK_NONE) {
		return true;
	}
	CAppDaliFsmEvent* const pEvent = new CAppDaliFsmEvent(CAppDaliFsmEvent::EVENT_CANCEL);
	if (NULL == pEvent) {
		ASSERT(0);
		return false;
	}	

	this->stopTimer();				
	this->invoke(refContext,*pEvent);
	delete pEvent;

	refContext.m_task = APPDALI_FSM_TASK_NONE;
	refContext.m_state = TASK_SEARCH_DEVICES_STATE_INIT;
	return true;
}

// Conroleren of er timers aan het lopen zijn...
void CAppDaliFsm::poll(CAppDaliFsmContext& refContext, int intervalSec) {
	if (this->m_timer.tick(intervalSec)) {			
		if (this->m_timer.expired()) {		
			this->setDaliFsmEventTimerExpiry(refContext);
			this->m_timer.stop();
		}	
	}
}

/***********************************************************************************/
void CAppDaliFsm::setDaliFsmEventTransmitDone(CAppDaliFsmContext& refContext) {
	CAppDaliFsmEvent* const pEvent = new CAppDaliFsmEvent(CAppDaliFsmEvent::EVENT_TX_DONE);
	if (NULL == pEvent) {
		ASSERT(0);
		return;
	}	
	this->stopTimer();							
	this->invoke(refContext,*pEvent);		
	this->startTimer(refContext);			// Wanneer er geen taak bezig is dan wordt de timer niet gestart.
	delete pEvent;
}

void CAppDaliFsm::setDaliFsmEventBackwardResponse(CAppDaliFsmContext& refContext, int backwardResponseData) {
	CAppDaliFsmEvent* const pEvent = new CAppDaliFsmEvent_BackwardResponse(backwardResponseData);
	if (NULL == pEvent) {
		ASSERT(0);
		return;
	}
	this->stopTimer();							
	this->invoke(refContext,*pEvent);
	this->startTimer(refContext);			// Wanneer er geen taak bezig is dan wordt de timer niet gestart.				
	delete pEvent;
}

void CAppDaliFsm::setDaliFsmEventNoBackwardResponse(CAppDaliFsmContext& refContext) {
	CAppDaliFsmEvent* const pEvent = new CAppDaliFsmEvent(CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_NO_RESPONSE);
	if (NULL == pEvent) {
		ASSERT(0);
		return;
	}
	
	this->stopTimer();							
	this->invoke(refContext,*pEvent);
	this->startTimer(refContext);			// Wanneer er geen taak bezig is dan wordt de timer niet gestart.	
	delete pEvent;
}

void CAppDaliFsm::setDaliFsmEventBackwardResponseError(CAppDaliFsmContext& refContext) {
	CAppDaliFsmEvent* const pEvent = new CAppDaliFsmEvent(CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_ERROR);
	if (NULL == pEvent) {
		ASSERT(0);
		return;
	}	
	this->stopTimer();					
	this->invoke(refContext,*pEvent);
	this->startTimer(refContext);			// Wanneer er geen taak bezig is dan wordt de timer niet gestart.			
	delete pEvent;
}

void CAppDaliFsm::setDaliFsmEventTimerExpiry(CAppDaliFsmContext& refContext) {
	CAppDaliFsmEvent* const pEvent = new CAppDaliFsmEvent(CAppDaliFsmEvent::EVENT_TIMER_EXPIRY);
	if (NULL == pEvent) {
		ASSERT(0);
		return;
	}
	this->stopTimer();						
	this->invoke(refContext,*pEvent);
	delete pEvent;
}

void CAppDaliFsm::setDaliFsmEventSystemBusFailure(CAppDaliFsmContext& refContext) {
	this->stopTimer();						
	ASSERT(0);
}

/***********************************************************************************/
// IAppDaliFsm_HelperFsmEvents
// Events van de helper FSM algoritmes.	
void CAppDaliFsm::onNotifyFsmAlgoritmeStarted(DaliDeviceType_t daliDeviceType, int shortAddress, AlgoritmeName_t type,int param1,int param2) {
	CString strMessage;
	const CString strDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType ? "control gear" : "control input");
	switch(type) {	
		case ALGORITME_NAME_SEARCH_DEVICES: {
			strMessage.Format("Search for %s devices.",strDaliDeviceType);												 
		} break;
		case ALGORITME_NAME_SEARCH_DEVICE: {				
			strMessage.Format("Search for %s device.",strDaliDeviceType);													
		} break;
		case ALGORITME_NAME_REQUEST_PROPERTIES: {			
			strMessage.Format("Request %s properties.",strDaliDeviceType);											  
		} break;
		case ALGORITME_NAME_REQUEST_MEMORYMAP: {
			strMessage.Format("Request %s memory map.",strDaliDeviceType);		
		} break;
		case ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS: {
			strMessage.Format("Request %s group members.",strDaliDeviceType);				
		} break;
		case ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES: {
			strMessage.Format("Request %s scene info.",strDaliDeviceType);				
		} break;
		case ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE: {			
			const int instanceIdentifier = param1;
			strMessage.Format("Request %s device instance %d properties.",strDaliDeviceType, instanceIdentifier);		
		} break;
		case ALGORITME_NAME_COMMISSION_ERASEDATA: {
			strMessage.Format("Erase %s devices addressing.",strDaliDeviceType);
		} break;
		case ALGORITME_NAME_COMMISSION_SEARCH_UNADDRESSED_DEVICES: {
			strMessage.Format("Search for unaddressed %s devices.",strDaliDeviceType);														 
		} break;
		case ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES: {
			strMessage.Format("Address %s devices.",strDaliDeviceType);						   
		} break;
		case ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS: {
			strMessage.Format("Automatic configuration %s device instance params.",strDaliDeviceType);														  
		} break;
		case ALGORITME_NAME_CONFIG_CONTROLDEVICE_INSTANCE_PARAMS: {
			strMessage.Format("Config %s device instance params.",strDaliDeviceType);													  
		} break;
		case ALGORITME_NAME_SHORTADDRESS_ASSIGN: {
			ASSERT((param1>= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (param1<= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST));
			strMessage.Format("Assign Dali device %d new short address %d.",shortAddress,param1);	
		} break;
		case ALGORITME_NAME_SHORTADDRESS_ERASE: {
			strMessage.Format("Erase Dali device %d short address.",shortAddress);									
		} break;
		case ALGORITME_NAME_CONTROLGEAR_CFG_PROPERTIES: {
			strMessage.Format("Start config Dali control gear device %d properties.",shortAddress);															
		} break;
		case ALGORITME_NAME_CONTROLGEAR_CFG_SCENES: {
			strMessage.Format("Start config Dali control gear device %d scenes.",shortAddress);															
		} break;
		case ALGORITME_NAME_CONTROLGEAR_CFG_SINGLE_SCENE: {
			const int sceneNumber = param1;				
			if (0xff == shortAddress) {			
				strMessage.Format("Start config Dali control gear device %d scene broadcast.", sceneNumber);
			}
			else {
				strMessage.Format("Start config Dali control gear device %d scene %d.", shortAddress, sceneNumber);
			}
		} break;
		case ALGORITME_NAME_CONTROLGEAR_QUERY_ACTUALLEVEL: {
			// strMessage.Format("Request control gear %d actual level.", shortAddress);
		} break;
		case ALGORITME_NAME_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF: {
			const int instanceIdentifier = param1;
			strMessage.Format("Config %s device instance %d enable on/off.",strDaliDeviceType, instanceIdentifier);													 
		} break;
		case ALGORITME_NAME_SHORTADDRESSES_SWAP: {	
			strMessage.Format("Start swapping device addresses A%d and A%d",param1,param2);
		} break;
		case ALGORITME_NAME_CONTROLGEAR_SELECT_DIMMINGCURVE: {
			strMessage.Format("Start config Dali control gear dimmer A%d dimmer curve select.", shortAddress);
		} break;
	}
	if (!strMessage.IsEmpty()) {
		m_refParent.notifySetProgessInfo(shortAddress,strMessage);
	}
}

void CAppDaliFsm::onNotifyFsmAlgoritmeDone(DaliDeviceType_t daliDeviceType, int shortAddress,AlgoritmeName_t type,int param1,int param2) {
	CString strMessage;	
	const CString strDaliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType ? "control gear" : "control input");
	switch(type) {	
		case ALGORITME_NAME_SEARCH_DEVICES: {			
			bool foundDevices = false;
			if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {		
				CAppDaliFsmHelperSearchControlInputDevice& refAlg = this->getAlgSearchControlInputDevices();
				if (refAlg.foundDevices()) {
					foundDevices = true;
				}
			}
			else {
				CAppDaliFsmHelperSearchControlGearDevice& refAlg = this->getAlgSearchControlGearDevice();
				if (refAlg.foundDevices()) {
					foundDevices = true;
				}			
			}
			if (foundDevices) {
				strMessage.Format("Search for %s devices done.", strDaliDeviceType);
			}
			else {
				strMessage.Format("Search for %s devices done. No devices Found.", strDaliDeviceType);
			}
		} break;
	
		// Nieuw gevonden device toevoegen aan de network devices.
		case ALGORITME_NAME_SEARCH_DEVICE: {
			bool foundDevice = false;
			if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
				CAppDaliFsmHelperSearchControlInputDevice& refAlg = this->getAlgSearchControlInputDevices();
				if (refAlg.foundDevices()) {
					foundDevice = true;
				}
			}
			else {
				CAppDaliFsmHelperSearchControlGearDevice& refAlg = this->getAlgSearchControlGearDevice();
				if (refAlg.foundDevices()) {
					foundDevice = true;
				}
			}
			if (foundDevice) {
				if (false == (this->m_refParent.fsmDbaseAddNewDevice(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, daliDeviceType,shortAddress))) {
					// WAT doen ???
				}
				strMessage.Format("%s device found.", strDaliDeviceType);
			}														
		} break;

		// Network devices:Properties updaten.		
		case ALGORITME_NAME_REQUEST_PROPERTIES: {		
			if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
				// Dit algoritme kan uitgevoerd worden bij de opstart en ook tijdens de normale werking via de user interface.
				// Tijdens de opstart: Database=DEVICE_DATABASE_NETWORK
				// Via de user interface : Database=DEVICE_DATABASE_DOCUMENT
				CAppDaliFsmHelperRequestPropertiesControlGear& refAlg = this->getAlgControlGearRequestProperties();			
				const IAppDaliFsm_Parent::DeviceDatabaseType_t databaseType = refAlg.getWorkingmodeCommisioning() ? 
							IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK :IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT;

				this->m_refParent.fsmDbaseSetControlGearDeviceStatus(databaseType, shortAddress, refAlg.getDeviceStatus());
				this->m_refParent.fsmDbaseSetControlGearDeviceVersion(databaseType, shortAddress, refAlg.getDeviceVersion());
				this->m_refParent.fsmDbaseSetControlGearDeviceType(databaseType, shortAddress, refAlg.getDeviceType());
				this->m_refParent.fsmDbaseSetControlGearFadeTimeRate(databaseType, shortAddress, refAlg.getDeviceFadeTimeRate());
				this->m_refParent.fsmDbaseSetControlGearMaximumLevel(databaseType, shortAddress, refAlg.getDeviceMaximumLevel());
				this->m_refParent.fsmDbaseSetControlGearMinimumLevel(databaseType, shortAddress, refAlg.getDeviceMinimumLevel());
				this->m_refParent.fsmDbaseSetControlGearPhysicalMinValue(databaseType, shortAddress, refAlg.getDevicePhysicalMinimum());
				this->m_refParent.fsmDbaseSetControlGearDeviceExtendedFadeTime(databaseType, shortAddress, refAlg.getDeviceExtendedFadeTime());
				this->m_refParent.fsmDbaseSetControlGearDeviceFailureLevel(databaseType, shortAddress, refAlg.getDeviceFailureLevel());
				this->m_refParent.fsmDbaseSetControlGearDevicePowerOnLevel(databaseType, shortAddress, refAlg.getDevicePowerOnLevel());
				this->m_refParent.fsmDbaseControlGearUpdateActualLevel(databaseType, shortAddress, refAlg.getDeviceActualLevel());
				this->m_refParent.fsmDbaseSetControlGearDimmingCurve(databaseType, shortAddress, refAlg.getDimmingCurve());
				this->m_refParent.fsmDbaseSetControlGearDeviceTypes(databaseType, shortAddress, refAlg.getDeviceTypes());
				this->m_refParent.fsmDbaseSetControlGearColorFeatures(databaseType, shortAddress, refAlg.getColorFeatures());
			}
			else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
				CAppDaliFsmHelperRequestPropertiesControlInputDevice& refAlg = this->getAlgInputRequestProperties();
				m_refParent.fsmDbaseSetControlInputDeviceStatus(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getDeviceStatus());
				m_refParent.fsmDbaseSetControlInputDeviceVersion(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getDeviceVersion());
				m_refParent.fsmDbaseSetControlInputDeviceOperatingMode(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getDeviceOperatingMode());
				m_refParent.fsmDbaseSetControlInputDeviceGroupInfo(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getBitfieldGroups());
				m_refParent.fsmDbaseSetControlInputDeviceAppControllerEnabled(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getAppControllerEnabled());
				m_refParent.fsmDbaseSetControlInputDevicePowerCycleNotification(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getPowerCycleNotification());
				m_refParent.fsmDbaseSetControlInputDeviceDeviceCapabilities(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getDeviceCapabilities());
				m_refParent.fsmDbaseSetControlInputDeviceRandomAddress(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, refAlg.getRandomAddress());
			}
			strMessage.Format("Request %s properties done.",strDaliDeviceType);											  
		} break;

		case ALGORITME_NAME_REQUEST_MEMORYMAP: {
			if (DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType) {
				// Oppassen:
				// Bij oude control gear zijn niet alle params ingevuld.
				CAppDaliFsmHelperReadMemoryControlGear& refAlg = this->getAlgControlGearReadMemory( );
				this->m_refParent.fsmDbaseSetDeviceGTIN(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,refAlg.getGTIN());
				this->m_refParent.fsmDbaseSetDeviceSerialNumber(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,refAlg.getSerialNumber());
				this->m_refParent.fsmDbaseSetDeviceFirmwareVersion(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,refAlg.getVersionFirmware());
				this->m_refParent.fsmDbaseSetDeviceHardwareVersion(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,refAlg.getVersionHardware());
			}
			else if (DALI_DEVICE_TYPE_INPUTCONTROL == daliDeviceType) {
				CAppDaliFsmHelperReadMemoryInputDevice& refAlg = this->getAlgInputDeviceReadMemory( );		
				this->m_refParent.fsmDbaseSetDeviceGTIN(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,refAlg.getGTIN());
				this->m_refParent.fsmDbaseSetDeviceSerialNumber(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,refAlg.getSerialNumber());
				this->m_refParent.fsmDbaseSetDeviceFirmwareVersion(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,refAlg.getVersionFirmware());
				this->m_refParent.fsmDbaseSetDeviceHardwareVersion(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,refAlg.getVersionHardware());
			}
			strMessage.Format("Request %s memory map done.",strDaliDeviceType);	
		} break;

		case ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS: {
			// Dit algoritme kan uitgevoerd worden bij de opstart en ook tijdens de normale werking via de user interface.
			// Tijdens de opstart: Database=DEVICE_DATABASE_NETWORK
			// Via de user interface : Database=DEVICE_DATABASE_DOCUMENT
			CAppDaliFsmHelperQueryGroupsControlGear& refAlg = this->getAlgControlGearQueryGroups( );				
			const IAppDaliFsm_Parent::DeviceDatabaseType_t databaseType = refAlg.getWorkingmodeCommisioning() ? 
					IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK :IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT;
			this->m_refParent.fsmDbaseSetControlGearGroupMemberInfo(databaseType, shortAddress,refAlg.getGroupsinfo());
			strMessage.Format("Request %s group members done.",strDaliDeviceType);		
		} break;

		case ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES: {
			// Dit algoritme kan uitgevoerd worden bij de opstart en ook tijdens de normale werking via de user interface.
			// Tijdens de opstart: Database=DEVICE_DATABASE_NETWORK
			// Via de user interface : Database=DEVICE_DATABASE_DOCUMENT
			CAppDaliFsmHelperQueryScenesControlGear& refAlg = this->getAlgControlGearQueryScenes( );	
			const IAppDaliFsm_Parent::DeviceDatabaseType_t databaseType = refAlg.getWorkingmodeCommisioning() ? 
				IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK :IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT;

			// Bepalen of er DT8 Color info moet doorgegeven worden...
			const bool updateDT8ColorInfo = this->m_refParent.fsmDbaseControlGearIsDT8ColorObjectSupported(databaseType, shortAddress);

			for (int i=0;i<CAppDaliFsmHelperQueryScenesControlGear::MAX_SCENES;++i) {
				this->m_refParent.fsmDbaseSetControlGearSceneLevel(databaseType, shortAddress, i, refAlg.getSceneActualLevel(i));				
				// DT8 Color object info
				if (updateDT8ColorInfo) {									
					this->m_refParent.fsmDbaseControlGearDT8SetSceneColorObjectInfo(databaseType,shortAddress,i,refAlg.getSceneDT8ColorData(i));
				}				
			}	
			strMessage.Format("Request %s scene info done.",strDaliDeviceType);	
		} break;

		case ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE: {
			CAppDaliFsmHelperRequestControlInputDeviceInstances& refAlg = this->getAlgInputDeviceRequestInstances( );			
			const int instanceIdentifier = param1;						
			ASSERT(instanceIdentifier == refAlg.getCurrentInstanceID());			
			m_refParent.fsmDbaseSetControlInputDeviceInstanceType(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, instanceIdentifier, refAlg.getCurrInstanceType());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceEnabledOnOff(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress,instanceIdentifier, refAlg.getCurrInstanceEnabled());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceEventScheme(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress,instanceIdentifier, refAlg.getCurrInstanceEventScheme());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceEventPrio(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress,instanceIdentifier, refAlg.getCurrInstanceEventPrio());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceEventFilters(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, instanceIdentifier, refAlg.getCurrInstanceEventFilter());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceGroupInfo(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, instanceIdentifier, 0, refAlg.getCurrInstancePrimaryGroup());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceGroupInfo(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, instanceIdentifier, 1, refAlg.getCurrInstanceGroup1());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceGroupInfo(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, instanceIdentifier, 2, refAlg.getCurrInstanceGroup2());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceStatus(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, instanceIdentifier, refAlg.getCurrInstanceEventResolution());
			m_refParent.fsmDbaseSetControlInputDeviceInstanceResolution(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, shortAddress, instanceIdentifier, refAlg.getCurrInstanceStatus());
			strMessage.Format("Request %s device instance %d properties done.",strDaliDeviceType, instanceIdentifier);			
		} break;

		case ALGORITME_NAME_COMMISSION_ERASEDATA: {							
			strMessage.Format("Erase %s devices addressing done.",strDaliDeviceType);
		} break;

		case ALGORITME_NAME_COMMISSION_SEARCH_UNADDRESSED_DEVICES: {
			CAppDaliFsmHelperDaliDevicesSearchUnaddressed& refAlg = this->getAlgDaliDevicesSearchUnaddressed( );
			if (refAlg.foundDevice()) {
				strMessage.Format("Search for unaddressed %s devices. Found devices without an address.",strDaliDeviceType);														 
			}
			else {
				strMessage.Format("Search for unaddressed %s devices. Found no devices without an address.",strDaliDeviceType);	
			}
		} break;

		case ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES: {
			CAppDaliFsmHelperDaliDevicesAddress& refAlg = this->getAlgControlDaliDevicesAddressDevices( );			
			const int numberOfDevices = refAlg.getNumberOfDevices();
			strMessage.Format("Address %s devices done. Assigned %d addresses",strDaliDeviceType,numberOfDevices);
		} break;

		case ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS: {
			strMessage.Format("Automatic configuration %s device instance params done.",strDaliDeviceType);														  
		} break;

		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_CONFIG_CONTROLDEVICE_INSTANCE_PARAMS: {
			const CAppDaliInputControlDeviceInstances& refCfgData = this->getAlgControlInputDeviceInstancesCfg().getcfgData( );
			m_refParent.fsmDbaseControlInputDeviceInstancesCfgUpdate(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, shortAddress, refCfgData);
			strMessage.Format("Config %s device instance params done.",strDaliDeviceType);				
		} break;

		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_SHORTADDRESS_ASSIGN: {
			const bool success = this->getAlgAssingShortAddress().success();	
			const int newShortAddress = param1;
			ASSERT((newShortAddress>= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (newShortAddress<= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST));
			if (true == success) {				
				m_refParent.fsmDbaseTaskChangedDaliDeviceShortAddress(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, daliDeviceType,shortAddress,newShortAddress);
			}
			strMessage.Format("Assign Dali device %d new short address %d %s",shortAddress,newShortAddress,(success==true ? "done" : "ERROR"));			
		} break;

		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_SHORTADDRESS_ERASE: {
			const bool success = this->getAlgAssingShortAddress().success();	
			if (true == success) {
				m_refParent.fsmDbaseTaskRemovedDaliDevice(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, daliDeviceType,shortAddress);
			}
			strMessage.Format("Erase Dali device %d short address %s.",shortAddress,(success==true ? "done" : "ERROR"));	
		} break;

		
		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_CONTROLGEAR_CFG_PROPERTIES: {						
			const CAppDaliDeviceProperties_ControlGear& refCfgData = this->getAlgCfgControlGearProperties().getcfgData( );
			m_refParent.fsmDbaseControlGearPropertiesCfgUpdate(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, shortAddress,refCfgData);
			strMessage.Format("Config Dali control gear device %d properties done.",shortAddress);															
		} break;

		// Oppassen:	
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_CONTROLGEAR_CFG_SCENES: {
			const CAppDaliDeviceProperties_ControlGear& refCfgData = this->getAlgControlGearCfgScenes().getcfgData( );	
			m_refParent.fsmDbaseControlGearPropertiesCfgUpdate(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, shortAddress, refCfgData);
			strMessage.Format("Config Dali control gear device %d scenes done.",shortAddress);		
		} break;

		// Oppassen:	
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_CONTROLGEAR_CFG_SINGLE_SCENE: {
			// param1	-> SceneNumber.
			const int sceneNumber = param1;
			const CAppDaliDeviceProperties_ControlGear& refCfgData = this->getAlgControlGearCfgSingleScene().getcfgData();
			if (0xFF == shortAddress) {					
				m_refParent.fsmDbaseControlGearSetBroadcastSceneInfo(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, sceneNumber, refCfgData);
				strMessage.Format("Config Dali control gear device %d scene broadcast done.", sceneNumber);
			}
			else {				
				m_refParent.fsmDbaseControlGearPropertiesCfgUpdate(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, shortAddress, refCfgData);
				// Momenteel worden alle properties upgedate en niet enkel de sfeer info.					
				// Mogelijke optimalisatie is enkel de sfere info updaten.
				strMessage.Format("Config Dali control gear device %d scene %d done.", shortAddress, sceneNumber);
			}
		} break;

		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_CONTROLGEAR_QUERY_ACTUALLEVEL: {			
			const CAppDaliFsmHelperControlGearActualValueRequest& refAlg = this->getAlgCfgControlGearActualValueRequest();
			const int actualLevel = refAlg.getActualLevel();
			if (actualLevel >= 0 && actualLevel <= 255) {
				this->m_refParent.fsmDbaseControlGearUpdateActualLevel(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, shortAddress, actualLevel);
				// strMessage.Format("Request control gear %d actual level returned value %d.", shortAddress,actualLevel);
			}
		} break;

		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF: {
			const int instanceIdentifier = param1;
			CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff& refAlg = this->getAlgControlDeviceInstanceEnableOnOff();
			bool instanceEnabled = refAlg.getInstanceStateOnOff();
			m_refParent.fsmDbaseSetControlInputDeviceInstanceEnabledOnOff(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, shortAddress,instanceIdentifier, instanceEnabled);
			strMessage.Format("Config %s device instance %d enable %s done.",strDaliDeviceType, instanceIdentifier, (instanceEnabled == true ? "on" : "off"));	
		} break;

		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_SHORTADDRESSES_SWAP: {								
			const bool success = this->getAlgSwapShortAddresses().success();	
			const int address0 = param1;
			const int address1 = param2;
			if (true == success) {				
				m_refParent.fsmDbaseTaskSwappedDaliDeviceShortAddresses(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, daliDeviceType, address0, address1);			
			}
			strMessage.Format("Swapping addresses: Dali device address %d and %d %s",address0, address1,(success==true ? "done" : "ERROR"));													 
		} break;

		// Oppassen: 
		// Onderstaande gebruikt de offline/document database (zie IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT)
		case ALGORITME_NAME_CONTROLGEAR_SELECT_DIMMINGCURVE: {			
			CAppDaliFsmHelperSelectDimmingCurve& refAlg = this->getAlgSelectDimmingCurve();
			const bool success = refAlg.success();
			if (success) {
				const int dimmerCurve = refAlg.getDimmerCurve();	
				m_refParent.fsmDbaseSetControlGearDimmingCurve(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, shortAddress, dimmerCurve);			
			}
			strMessage.Format("Selecting dimming curve for Dali device address %d %s", shortAddress, (success == true ? "done" : "ERROR"));			
		} break;

		default: {
			ASSERT(0);	// TODO 
		} break;		
	}
	if (!strMessage.IsEmpty()) {
		m_refParent.notifySetProgessInfo(shortAddress,strMessage);
	}
}

// TODO_DALIFSM_ERROR:
// Moeten de foutcodes nog verder uitbreiden.
void CAppDaliFsm::onNotifyFsmAlgoritmeError(DaliDeviceType_t daliDeviceType,int shortAddress, AlgoritmeName_t type, int param1, int param2) {

	// TODO: Algemene fouten...
	//	- Verkeerd event. 
	//  - Timer expiry.

	CString strMessage;
	const DaliErrorCode_t errorCode = (DaliErrorCode_t)param1;
	switch (errorCode) {	
		case ERROR_CODE_SHORTADDRESS_DOUBLE: {		
			static const CString strErrorMessage("Error happened. Possible this device holds a double short address");
			switch (type) {				
				case ALGORITME_NAME_SEARCH_DEVICE:				
				case ALGORITME_NAME_REQUEST_MEMORYMAP:						
				case ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE:
				case ALGORITME_NAME_COMMISSION_SEARCH_UNADDRESSED_DEVICES:
				case ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES:{
					// Het device in de database aanpassen + een melding geven.
					// Deze algoritmes worden enkel uitgevoerd tijdens de opstart (=zoeken/addresseren van de devices.)
					// Het is altijd de network database die aangepast wordt.
					if (m_refParent.fsmDbaseSetDeviceError(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, daliDeviceType, shortAddress, errorCode)) {
						strMessage = strErrorMessage;
					}										
				} break;
				case ALGORITME_NAME_REQUEST_PROPERTIES: {
					// Het device in de database aanpassen + een melding geven.
					// Wanneer het event gebeurd tijdens de opstart --> Network database.
					// Wanneer het event gebeurd tijdens runtime werking (vanuit de GUI) --> Offline/doc database.
					CAppDaliFsmHelperRequestPropertiesControlGear& refAlg = this->getAlgControlGearRequestProperties();			
					const IAppDaliFsm_Parent::DeviceDatabaseType_t databaseType = refAlg.getWorkingmodeCommisioning() ? 
							IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK :IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT;		
					if (m_refParent.fsmDbaseSetDeviceError(databaseType, daliDeviceType, shortAddress, errorCode)) {
						strMessage = strErrorMessage;
					}															
				} break;
				case ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS: {
					// Het device in de database aanpassen + een melding geven.
					// Wanneer het event gebeurd tijdens de opstart --> Network database.
					// Wanneer het event gebeurd tijdens runtime werking (vanuit de GUI) --> Offline/doc database.
					CAppDaliFsmHelperQueryGroupsControlGear& refAlg = this->getAlgControlGearQueryGroups( );				
					const IAppDaliFsm_Parent::DeviceDatabaseType_t databaseType = refAlg.getWorkingmodeCommisioning() ? 
							IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK :IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT;
					if (m_refParent.fsmDbaseSetDeviceError(databaseType, daliDeviceType, shortAddress, errorCode)) {
						strMessage = strErrorMessage;
					}											
				} break;
				case ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES: {
					// Het device in de database aanpassen + een melding geven.
					// Wanneer het event gebeurd tijdens de opstart --> Network database.
					// Wanneer het event gebeurd tijdens runtime werking (vanuit de GUI) --> Offline/doc database.
					CAppDaliFsmHelperQueryScenesControlGear& refAlg = this->getAlgControlGearQueryScenes( );				
					const IAppDaliFsm_Parent::DeviceDatabaseType_t databaseType = refAlg.getWorkingmodeCommisioning() ? 
							IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK :IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT;
					if (m_refParent.fsmDbaseSetDeviceError(databaseType, daliDeviceType, shortAddress, errorCode)) {
						strMessage = strErrorMessage;
					}																										
				} break;				
				case ALGORITME_NAME_SHORTADDRESS_ASSIGN: 
				case ALGORITME_NAME_SHORTADDRESSES_SWAP: {
					// Database aanpassen + Melding.
					// Deze algoritmes worden enkel uitgevoerd tijdens runtime werking (vanuit de GUI) --> Offline/doc database.			
					if (m_refParent.fsmDbaseSetDeviceError(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, daliDeviceType, shortAddress, errorCode)) {
						strMessage = strErrorMessage;
					}										
				} break;
				case ALGORITME_NAME_CONTROLGEAR_SELECT_DIMMINGCURVE: {
					// Database aanpassen + Melding.
					// Dit algoritme worden enkel uitgevoerd tijdens runtime werking (vanuit de GUI) --> Offline/doc database.			
					ASSERT(DALI_DEVICE_TYPE_CONTROLGEAR == daliDeviceType);
					if (m_refParent.fsmDbaseSetDeviceError(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, daliDeviceType, shortAddress, errorCode)) {
						strMessage = strErrorMessage;
					}										
				} break;
				case ALGORITME_NAME_SEARCH_DEVICES: {
					// Absorbe:
					// Niet behandelen als een fout.
				} break;
			}
		} break; // ERROR_CODE_SHORTADDRESS_DOUBLE

		case ERROR_CODE_PROPERTY_NORESPONSE: {		
			strMessage = CString("Error happened. No response from this device.");
		} break;
		case ERROR_CODE_DATABASE_DEVICE_NOT_FOUND: {
			if ((ALGORITME_NAME_SHORTADDRESSES_SWAP == type) || (ALGORITME_NAME_SHORTADDRESS_ASSIGN == type)) {
				if (m_refParent.fsmDbaseSetDeviceError(IAppDaliFsm_Parent::DEVICE_DATABASE_DOCUMENT, daliDeviceType, shortAddress, errorCode)) {					
					strMessage.Format("The DALI device A%d was not found in the network.",shortAddress);
				}	
			}							  
		} break;
	}
	if (!strMessage.IsEmpty()) {
		m_refParent.notifySetProgessInfo(shortAddress,strMessage);
	}	
}

void CAppDaliFsm::onNotifyFsmAlgoritmeShortAddressAssigned(DaliDeviceType_t type, int shortAddress) {
	CString strMessage;
	strMessage.Format("Assigned short address to %s", type == DALI_DEVICE_TYPE_CONTROLGEAR ? "control gear device" : "input control device");
	m_refParent.notifySetProgessInfo(shortAddress, strMessage);
	m_refParent.fsmAssignedNewShortAddress(type, shortAddress);
}

void CAppDaliFsm::onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(int shortAddress, int instanceNumber,int instanceType) {
	CString strMessage;
	strMessage.Format("Instance number %d, instance type %d enabled and auto configured.",instanceNumber,instanceType);
	m_refParent.notifySetProgessInfo(shortAddress, strMessage);
}

void CAppDaliFsm::onNotifyFsmAlgoritmeMsgInfo(const CString& str) {
	m_refParent.notifySetProgessInfo(-1, str);
}

/***********************************************************************************/
// Protected ...
CAppDaliFsm::PF_STATE_HANDLERS CAppDaliFsm::getHandler(CAppDaliFsmContext& refContext) {
	PF_STATE_HANDLERS retValue = &CAppDaliFsm::stateUndefined;
	const int stateIndex = refContext.m_state;
	if (stateIndex < TASK_COMMISSIONING_STATE_INIT || stateIndex >= TASK_COMMISSIONING_STATES_MAX) {
		ASSERT(0);
		return retValue;
	}
	retValue =  m_handlers[stateIndex];
	REQUIRE(NULL != retValue);	
	return retValue;
}

bool CAppDaliFsm::invoke(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	TRACER_FSM(refContext,ev);
	PF_STATE_HANDLERS pHandler = this->getHandler(refContext);
	ASSERT(NULL != pHandler);
	if (NULL == pHandler) {
		return false;
	}		
	(this->*pHandler)(refContext,ev);	
	return true;
}

/***********************************************************************************/
// state handlers...
void CAppDaliFsm::stateUndefined(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	// Absorbe events.
	// Wanneer we een taak cancelen komen we in deze toestand...
	// ASSERT(0);
}

void CAppDaliFsm::stateInit(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	// absorbe...
}

// Helper functie voor de tasks
bool CAppDaliFsm::helper_state_handleEvent(CAppDaliFsmHelper_Base& refAlg,CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	bool result = true;		// TODO???
	switch (ev.m_event) {
		case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE: {
			const CAppDaliFsmEvent_BackwardResponse* const pEvent = dynamic_cast<const CAppDaliFsmEvent_BackwardResponse*>(&ev);
			if (NULL == pEvent) {
				ASSERT(0);
			}
			if (false == refAlg.onFsmEventResponse(refContext, pEvent->m_data)) {
				ASSERT(0);				// error toestand - nog iets anders doen (zie isError() hieronder)
			}
		} break;
		case CAppDaliFsmEvent::EVENT_TX_DONE: {
			if (false == refAlg.onFsmEventTxDone(refContext)) {
				// ASSERT(0);				// error toestand - nog iets anders doen (zie isError() hieronder)
			}
		} break;
		case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_NO_RESPONSE: {
			if (false == refAlg.onFsmEventNoResponse(refContext)) {
				ASSERT(0);				// error toestand - nog iets anders doen (zie isError() hieronder)
			}
		} break;
		case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_ERROR: {
			if (false == refAlg.onFsmEventResponseError(refContext)) {
				ASSERT(0);				// error toestand - nog iets anders doen (zie isError() hieronder)
			}
		} break;
		case CAppDaliFsmEvent::EVENT_CANCEL: {
			refAlg.cancel();
		} break;

		case CAppDaliFsmEvent::EVENT_TIMER_EXPIRY: {
			refAlg.onFsmEventTimerExpiry(refContext);
		} break;		
		default: {
			return false; 
		} break;
	}
	return result;
}

void CAppDaliFsm::taskSearchDevices_enter(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {		
	CAppDaliFsmHelperSearchDevices& refAlg = this->getAlgSearchDevices();

	// AFhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());			
		m_refParent.notifySetProgessInfo(-1, "Search for DALI control gear devices failed");
	}
	else if (refAlg.isCancelled()) {
		// Stop Timer ????????????????
		m_refParent.notifySetProgessInfo(-1, "Search for DALI control gear devices cancelled");
	}
	else if (refAlg.isDone()) {
		// Broadcast: control gear aanwezig?
		this->getAlgSearchControlGearDevice().start();
		refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_SEARCH_BROADCAST;
	}
	else {
		refAlg.next();
	}	
}

void CAppDaliFsm::taskSearchDevices_stateSearchContolGearBroadcast(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperSearchControlGearDevice& refAlg = this->getAlgSearchControlGearDevice();
	
	// AFhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());			
		m_refParent.notifySetProgessInfo(-1, "Search for DALI control gear devices failed");
	}
	else if (refAlg.isCancelled()) {
		// Stop Timer ????????????????
		m_refParent.notifySetProgessInfo(-1, "Search for DALI control gear devices cancelled");
	}
	else if (refAlg.isDone()) {		
		if (refAlg.foundDevices()) {
			refAlg.start(refContext.m_shortAddress);
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_SEARCH_DEVICE;
		}
		else {
			refContext.m_stateCounter = 0;
			// m_refParent.notifySetProgessInfo(-1, "No control gear devices found.");
			this->getTransmitter().stopQuienscentMode();
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_WAIT_DONE;
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskSearchDevices_stateControlGearDeviceSearchContolGear(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperSearchControlGearDevice& refAlg = this->getAlgSearchControlGearDevice();
	
	// AFhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());			
		m_refParent.notifySetProgessInfo(-1, "Search for DALI control gear devices failed");
	}
	else if (refAlg.isCancelled()) {
		// Stop Timer ????????????????
		m_refParent.notifySetProgessInfo(-1,"Search for DALI control gear devices cancelled");
	}
	else if (refAlg.isDone()) {			
		if (refAlg.foundDevices()) {	
			if (false == this->getAlgControlGearRequestProperties().start(refContext.m_shortAddress)) {
				ASSERT(0);
			}
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_REQUEST_PROPERTIES;		
		}
		else {			
			if (++refContext.m_shortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {					
				// Opnieuw zoeken naar een nieuw device...
				if (false == refAlg.start(refContext.m_shortAddress)) {
					ASSERT(0);
				}
			}
			else {			
				this->getTransmitter().stopQuienscentMode( );
				refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_WAIT_DONE;
			}
		}
	}
	else {
		refAlg.next();
	}
}


// Sedert V16.46: 
// Deze toestand wordt gebruikt tijdens de opstart (Commissioning) en voor het opvragen van de properties.
// zie CAppDaliFsmHelperRequestPropertiesControlGear::getWorkingmodeCommisioning()
void CAppDaliFsm::taskSearchDevices_stateControlGearDeviceRequestDeviceProperties(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperRequestPropertiesControlGear& refAlg = this->getAlgControlGearRequestProperties();
	
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"DALI control gear read properties failed.");						
	}
	else if (refAlg.isCancelled()) {	
		// Stop Timer ????????????????	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"DALI control gear read properties cancelled");
	}
	else if (refAlg.isDone()) {
		if (refAlg.getWorkingmodeCommisioning()) {
			// FSM: Memory map uitlezen.
			static const int memoryLocation = 0x0000;
			static const int nrOfBytes = 27;			// 0x00 - 0x1A
			refContext.m_stateCounter = 0;
			this->getAlgControlGearReadMemory().start(refContext.m_shortAddress, memoryLocation, nrOfBytes);
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_READ_MEMORYMAP;
		}
		else {		
			m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Read control gear properties done.");
			this->setStatusDone(refContext);
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskSearchDevices_stateControlGearReadMemorymap(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperReadMemoryControlGear& refAlg = this->getAlgControlGearReadMemory();
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.	
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"DALI control gear read memory bank failed.");
	}
	else if (refAlg.isCancelled()) {	
		// Stop Timer ????????????????	
		m_refParent.notifySetProgessInfo(-1,"DALI control gear read memory bank cancelled");
	}
	else if (refAlg.isDone()) {
		// Opvragen van de groepen van een device.
		this->getAlgControlGearQueryGroups().start(refContext.m_shortAddress);
		refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_GROUPS;
	}
	else {
		refAlg.next();
	}
}


// Sedert V16.46: 
// Deze toestand wordt gebruikt tijdens de opstart (Commissioning) en voor het opvragen van de properties.
// zie CAppDaliFsmHelperQueryGroupsControlGear::getWorkingmodeCommisioning()
void CAppDaliFsm::taskSearchDevices_stateControlGearDeviceQueryGroups(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperQueryGroupsControlGear& refAlg = this->getAlgControlGearQueryGroups();
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"DALI control gear read group info failed.");
	}
	else if (refAlg.isCancelled()) {	
		// Stop Timer ????????????????	
		m_refParent.notifySetProgessInfo(-1,"DALI control gear read group info cancelled");
	}
	else if (refAlg.isDone()) {
		if (refAlg.getWorkingmodeCommisioning()) {
			const bool requestDT8ColorInfo = m_refParent.fsmDbaseControlGearIsDT8ColorObjectSupported(IAppDaliFsm_Parent::DEVICE_DATABASE_NETWORK, refContext.m_shortAddress);
			this->getAlgControlGearQueryScenes().start(refContext.m_shortAddress, requestDT8ColorInfo,true);
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_SCENELEVELS;
		}
		else {
			m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Read control gear groups done.");
			this->setStatusDone(refContext);
		}
	}
	else {
		refAlg.next();
	}
}

// Sedert V16.46: 
// Deze toestand wordt gebruikt tijdens de opstart (Commissioning) en voor het opvragen van de properties.
// zie CAppDaliFsmHelperQueryScenesControlGear::getWorkingmodeCommisioning()
void CAppDaliFsm::taskSearchDevices_stateControlGearDeviceQueryScenes(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperQueryScenesControlGear& refAlg = this->getAlgControlGearQueryScenes();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"DALI control gear read scene info failed.");
	}
	else if (refAlg.isCancelled()) {	
		// Stop Timer ????????????????	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"DALI control gear read scene info cancelled");
	}
	else if (refAlg.isDone()) {		
		if (refAlg.getWorkingmodeCommisioning()) {
			if (++refContext.m_shortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
				m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Search for DALI control gear device.");
				if (false == this->getAlgSearchControlGearDevice().start(refContext.m_shortAddress)) {
					ASSERT(0);	// wat doen...
				}
				refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_SEARCH_DEVICE;
			}
			else {
				m_refParent.notifySetProgessInfo(-1,"Exit quienscent mode.");
				refContext.m_stateCounter = 0;
				this->getTransmitter().stopQuienscentMode();
				refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_WAIT_DONE;
			}
		}
		else {
			m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Read control gear scenes done.");
			this->setStatusDone(refContext);
		}
	}
	else {
		refAlg.next();
	}
}

// TODO_DALI_FSM_ALGORITMES
// Onderstaande moet via een Algoritme..
void CAppDaliFsm::taskSearchDevices_stateWaitControlGearDone(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	switch (ev.m_event) {
		case CAppDaliFsmEvent::EVENT_TX_DONE: {
			if (0 == refContext.m_stateCounter) {		
				this->getTransmitter().sendControlgearBroadcastTerminate();
			}
			else if (1 == refContext.m_stateCounter) {
				this->getAlgSearchControlInputDevices().startBroadcast();
				refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_BROADCAST;
			}
			refContext.m_stateCounter++;
		} break;

		case CAppDaliFsmEvent::EVENT_CANCEL: {
			m_refParent.notifySetProgessInfo(-1, "Search for DALI input control devices cancelled");
			this->getTransmitter().stopQuienscentMode();
		} break;

		default: {
			// andere events todo... 
		} break;
	}
}

void CAppDaliFsm::taskSearchDevices_stateSearchBroadcastControlInputDevices(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperSearchControlInputDevice& refAlg = this->getAlgSearchControlInputDevices();
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg, refContext, ev)) {

	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		this->handleAlgoritmeError(refContext, refAlg.getFatalErrorInfo());
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Search control input devices failed.");
	}
	else if (refAlg.isCancelled()) {
		m_refParent.notifySetProgessInfo(-1, "Search control input devices cancelled");
	}
	else if (refAlg.isDone()) {
		if (refAlg.foundDevices()) {
			refContext.m_shortAddress = 0;
			refContext.m_stateCounter = 0;
			m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Search for DALI input control device.");
			refAlg.start(refContext.m_shortAddress);
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_DEVICE;
		}
		else {
			// Geen control devices aanwezig...
			refContext.m_stateCounter = 0;
			this->getTransmitter().stopQuienscentMode();
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_WAIT_DONE;
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskSearchDevices_stateSearchControlInputDevice(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperSearchControlInputDevice& refAlg = this->getAlgSearchControlInputDevices();
	
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg, refContext, ev)) {

	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		this->handleAlgoritmeError(refContext, refAlg.getFatalErrorInfo());
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Search control input devices failed.");
	}
	else if (refAlg.isCancelled()) {
		m_refParent.notifySetProgessInfo(-1, "Search control input devices cancelled");
	}
	else if (refAlg.isDone()) {
		if (refAlg.foundDevices()) {			
			// FSM: Memory map uitlezen.
			static const int memoryLocation = 0x0000;
			static const int nrOfBytes = 27;			// 0x00 - 0x1A
			refContext.m_stateCounter = 0;
			this->getAlgInputDeviceReadMemory().start(refContext.getShortAddress(), memoryLocation, nrOfBytes);
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_READ_MEMORYMAP;
		}
		else {
			if (++refContext.m_shortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
				refAlg.start(refContext.m_shortAddress);
			}
			else {
				refContext.m_stateCounter = 0;
				this->getTransmitter().stopQuienscentMode();
				refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_WAIT_DONE;
			}
		}
	}
	else {
		refAlg.next();
	}
}



#if(0)
// TODO_DALI_FSM_ALGORITMES
// Onderstaande moet via een Algoritme..
void CAppDaliFsm::taskSearchDevices_stateWaitControlGearDone(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	switch (ev.m_event) {
	case CAppDaliFsmEvent::EVENT_TX_DONE: {
		if (0 == refContext.m_stateCounter) {
			// this->getTransmitter().sendControlgearBroadcastInitialiseAll(0x00);		// 0=all controi gear
			this->getTransmitter().sendControlgearBroadcastTerminate();
		}
		else if (1 == refContext.m_stateCounter) {
			m_refParent.notifySetProgessInfo(-1, "Search for DALI input control devices.");
			this->getTransmitter().sendControlInputDevicesBroadcastinitialiseAll(0xff);
		}
		else {
			this->getTransmitter().sendQueryBroadcastInputControlDeviceStatus();
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_BROADCAST;
		}
		refContext.m_stateCounter++;
	} break;

	case CAppDaliFsmEvent::EVENT_CANCEL: {
		m_refParent.notifySetProgessInfo(-1, "Search for DALI input control devices cancelled");
		this->getTransmitter().stopQuienscentMode();
	} break;

	default: {
		// andere events todo... 
	} break;
	}
}


// TODO_DALI_FSM_ALGORITMES
// Onderstaande moet via een Algoritme..
bool CAppDaliFsm::startSearchFirstControlInputDevice(CAppDaliFsmContext& refContext) {
	bool result = false;
	refContext.m_shortAddress = 0;
	refContext.m_stateCounter = 0;
	m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Search for DALI input control device.");
	result = this->getTransmitter().sendQueryInputControlDeviceStatus(refContext.m_shortAddress);
	refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_DEVICE;
	return result;
}

void CAppDaliFsm::taskSearchDevices_stateSearchBroadcastControlInputDevices(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	switch(ev.m_event) {
		case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE: {
			const CAppDaliFsmEvent_BackwardResponse* const pEvent = dynamic_cast<const CAppDaliFsmEvent_BackwardResponse*>(&ev);
			if (NULL == pEvent) {
				ASSERT(0);
			}			
			
			if (DALI_BACWARDRESPONSE_MASK == pEvent->m_data) {	
				// Control devices aanwezig: Starten met zoeken naar het eerste device...
				this->startSearchFirstControlInputDevice(refContext);				
			}
			else {
				// Normaal antwoorden de devices met een 0xFF (Mask).
				// Wat moeten we doen wanneeer we een ander antwoord krijgen?
				// Momenteel gaan we er vanuit dat er input devices aanwezig zijn.
				// Todo: 
				//	- Moeten we een retry versturen tot we een antwoord van 1 device ontvangen?
				this->startSearchFirstControlInputDevice(refContext);	
			}
		} break;

		case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_NO_RESPONSE: {			
			// Geen control devices aanwezig...
			refContext.m_stateCounter = 0;
			this->getTransmitter().stopQuienscentMode();
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_WAIT_DONE;
		} break;

		case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_ERROR: {				
			// Als er meerdere antwoorden op de bericht zal er een invalid frame ontvangen worden...
			// Momenteel gaan we er vanuit dat er input devices aanwezig zijn.
			// Todo: 
			//	- Moeten we een retry versturen tot we een antwoord van 1 device ontvangen?
			this->startSearchFirstControlInputDevice(refContext);	
		} break;

		case CAppDaliFsmEvent::EVENT_TX_DONE: {	
			ASSERT(0);		
		} break;

		case CAppDaliFsmEvent::EVENT_CANCEL: {		
			m_refParent.notifySetProgessInfo(-1,"Search for DALI input control devices cancelled");
			this->getTransmitter().stopQuienscentMode();
		} break;
	}
}


void CAppDaliFsm::taskSearchDevices_stateSearchControlInputDevice(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	switch (ev.m_event) {
	case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE: {
		// Toevoegen aan de database...			
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Found DALI input control device.");
		if (false == (this->m_refParent.fsmDbaseAddNewDevice(DALI_DEVICE_TYPE_INPUTCONTROL, refContext.m_shortAddress))) {
			// WAT doen ???
		}

		// FSM: Memory map uitlezen.
		static const int memoryLocation = 0x0000;
		static const int nrOfBytes = 27;			// 0x00 - 0x1A
		refContext.m_stateCounter = 0;
		this->getAlgInputDeviceReadMemory().start(refContext.getShortAddress(), memoryLocation, nrOfBytes);
		refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_READ_MEMORYMAP;
	} break;

	case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_NO_RESPONSE: {
		// control device is niet aanwezig...
		if (++refContext.m_shortAddress <= DALI_DEVICE_SHORTADDRESS_LAST) {
			m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Search for DALI input control device.");
			this->getTransmitter().sendQueryInputControlDeviceStatus(refContext.m_shortAddress);
		}
		else {
			refContext.m_stateCounter = 0;
			this->getTransmitter().stopQuienscentMode();
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_WAIT_DONE;
		}
	} break;

	case CAppDaliFsmEvent::EVENT_BACKWARD_RESPONSE_ERROR: {
		// TODO: 
		// Mogelijks antwoorden er 2 devices (met hetzelfde adres).
		// Wat doen ?			
		ASSERT(0);
	} break;

	case CAppDaliFsmEvent::EVENT_TX_DONE: {
		// Komt hier niet voor ???
		ASSERT(0);		// Probleem CAN-NIC
	} break;

	case CAppDaliFsmEvent::EVENT_CANCEL: {
		m_refParent.notifySetProgessInfo(-1, "Search for DALI input control devices cancelled");
		this->getTransmitter().stopQuienscentMode();
	} break;
	}
}
#endif



// Uitlezen van de memory map van een control input device.
void CAppDaliFsm::taskSearchDevices_stateReadMemorymapControlInputDevice(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperReadMemoryInputDevice& refAlg = this->getAlgInputDeviceReadMemory();
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Read DALI input control device memory failed.");
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Read DALI input control device memory cancelled");
	}
	else if (refAlg.isDone()) {
		// Opvragen van de device input properties.
		refContext.m_stateCounter = 0;
		this->getAlgInputRequestProperties().start(refContext.getShortAddress());
		refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_REQUEST_PROPERTIES;		
	}
	else {
		refAlg.next();
	}
}

// Properties opvragen van het control device...
void CAppDaliFsm::taskSearchDevices_stateControlInputDeviceRequestDeviceProperties(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperRequestPropertiesControlInputDevice& refAlg = this->getAlgInputRequestProperties();
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Read DALI input control device properties failed.");
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Read DALI input control device properties cancelled");
	}
	else if (refAlg.isDone()) {
		// Updaten van de properties...
		// Wordt gedaan in de notify().		

		// Opvragen van de instance properties van het control device...
		refContext.m_stateCounter = 0;
		this->getAlgInputDeviceRequestInstances().start(refContext.getShortAddress());
		refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_REQUEST_INSTANCES;
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskSearchDevices_stateControlInputDeviceRequestInstances(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperRequestControlInputDeviceInstances& refAlg = this->getAlgInputDeviceRequestInstances();
	
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Read DALI input control device instance properties failed.");
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Read DALI input control device instance properties cancelled");
	}
	else if (refAlg.isDone()) {
		// Oppassen:
		// De properties van de instances worden upgedate in de callback functie onNotifyDataReadDone()
			
		// FSM verder done...
		if (++refContext.m_shortAddress  <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
/*
			m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Search for DALI input control device.");
			this->getTransmitter().sendQueryInputControlDeviceStatus(refContext.m_shortAddress);
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_DEVICE;
*/
			this->getAlgSearchControlInputDevices().start(refContext.m_shortAddress);
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_DEVICE;
		}
		else {
			refContext.m_stateCounter = 0;		
			this->getTransmitter().sendControlgearBroadcastTerminate( );
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_WAIT_DONE;
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskSearchDevices_stateWaitControlInputDevicesDone(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	switch(ev.m_event) {
		case CAppDaliFsmEvent::EVENT_TX_DONE: {	
			if (0 == refContext.m_stateCounter) {
				//this->getTransmitter().sendControlgearBroadcastTerminate( );			
				this->getTransmitter().stopQuienscentMode();
			}
			else if (1 == refContext.m_stateCounter) {
				this->getTransmitter().sendControlInputDevicesBroadcastTerminate( );				
			}			
			else {								
				m_refParent.notifySetProgessInfo(-1,"Search for DALI devices done.");
				this->setStatusDone(refContext);
			}
			refContext.m_stateCounter++;
		} break;
	}	
}

/***********************************************************************************/
// state handlers: Task Commissioning devices.
//
// New installation:
//	- Erase Control gear devices
//	- Address Control gear devices.
//
// Extend installation.
//	- Search unaddressed control gear devices.
//	- Address Control gear devices.


void CAppDaliFsm::taskCommissioning_stateControlGearErase(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperControlGearErase& refAlg = this->getAlgControlGearEraseAddresses();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg, refContext, ev)) {

	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(-1,"Addressing control gear erase data failed.");
	}
	else if (refAlg.isCancelled()) {
		m_refParent.notifySetProgessInfo(-1, "Addressing control gear erase data cancelled");
	}
	else if (refAlg.isDone()) {		
		static const bool assignOnlyUnAddressedDevices_no = false;	// Nieuwe installatie: Alle devices addresseren.
		refContext.m_stateCounter = 0;		
		if (false == this->getAlgControlDaliDevicesAddressDevices().start(DALI_DEVICE_TYPE_CONTROLGEAR,assignOnlyUnAddressedDevices_no)) {
			ASSERT(0);
		}
		refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLGEAR_ADDRESS;
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskCommissioning_stateControlGearSearchUnAddressed(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperDaliDevicesSearchUnaddressed& refAlg = this->getAlgDaliDevicesSearchUnaddressed();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg, refContext, ev)) {

	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(-1,"Addressing control gear search unaddressed devices failed.");
	}
	else if (refAlg.isCancelled()) {
		m_refParent.notifySetProgessInfo(-1, "Addressing control gear search unaddressed devices cancelled");
	}
	else if (refAlg.isDone()) {
		if (refAlg.foundDevice()) {
			static const bool assignOnlyUnAddressedDevices_yes = true;	// Extend installation, Enkel devices die geen address hebben.
			refContext.m_stateCounter = 0;				
			if (false == this->getAlgControlDaliDevicesAddressDevices().start(DALI_DEVICE_TYPE_CONTROLGEAR,assignOnlyUnAddressedDevices_yes)) {
				ASSERT(0);
			}
			m_refParent.notifySetProgessInfo(-1, "Addressing control gear. Found new devices. Start addressing.");
			refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLGEAR_ADDRESS;
		}
		else {
			m_refParent.notifySetProgessInfo(-1, "Addressing control gear done. Found no new devices.");
			this->getAlgDaliDevicesSearchUnaddressed( ).start(DALI_DEVICE_TYPE_INPUTCONTROL);
			refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLDEVICES_SEARCH_UNADDRESSED;
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskCommissioning_stateControlGearAddress(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperDaliDevicesAddress& refAlg = this->getAlgControlDaliDevicesAddressDevices();
	
	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(-1,"Addressing control gear failed.");
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control gear cancelled");
	}
	else if (refAlg.isDone()) {	
		// Bij een nieuwe installatie: Alle data wissen.
		// Bij een uitbreiding: Zoeken naar devices die geen adres hebben.
		if (APPDALI_FSM_TASK_COMMISSIONING_NEW == refContext.m_task) {
			this->getAlgControlInputDevicesEraseAddresses().start();
			refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLDEVICES_ERASE;
		}
		else {
			this->getAlgDaliDevicesSearchUnaddressed( ).start(DALI_DEVICE_TYPE_INPUTCONTROL);
			refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLDEVICES_SEARCH_UNADDRESSED;
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskCommissioning_stateControlDevicesErase(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperControlInputDeviceErase& refAlg = this->getAlgControlInputDevicesEraseAddresses();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices failed.");
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices cancelled");
	}
	else if (refAlg.isDone()) {	
		static const bool assignOnlyUnAddressedDevices_no = false;	// Nieuwe installatie: Alle devices addresseren.
		this->getAlgControlDaliDevicesAddressDevices().start(DALI_DEVICE_TYPE_INPUTCONTROL,assignOnlyUnAddressedDevices_no);
		refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLDEVICES_ADDRESS;
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskCommissioning_stateControlDevicesSearchUnAddressed(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperDaliDevicesSearchUnaddressed& refAlg = this->getAlgDaliDevicesSearchUnaddressed( );

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());				
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices cancelled");
	}
	else if (refAlg.isDone()) {	
		if (refAlg.foundDevice()) {
			static const bool assignOnlyUnAddressedDevices_yes = true;	// Extend installation, Enkel devices die geen address hebben.
			m_refParent.notifySetProgessInfo(-1,"Addressing control devices, found unaddressed devices.");
			this->getAlgControlDaliDevicesAddressDevices().start(DALI_DEVICE_TYPE_INPUTCONTROL,assignOnlyUnAddressedDevices_yes);
			refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLDEVICES_ADDRESS;
		}
		else {
			m_refParent.notifySetProgessInfo(-1,"Addressing control devices done, found no unaddressed devices.");			
			refContext.m_shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;
			refContext.m_stateCounter = 0;
			this->getAlgSearchDevices().start();
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_SEARCH_DEVICES;
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskCommissioning_stateControlDevicesAddress(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {
	
	CAppDaliFsmHelperDaliDevicesAddress& refAlg = this->getAlgControlDaliDevicesAddressDevices();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());		
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices cancelled");
	}
	else if (refAlg.isDone()) {				
		if (refContext.m_listAssignedInputDevices.IsEmpty()) {		
			m_refParent.notifySetProgessInfo(-1,"Addressing control devices done.");			
			refContext.m_shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;
			refContext.m_stateCounter = 0;
			this->getAlgSearchDevices().start();
			refContext.m_state = TASK_SEARCH_DEVICES_STATE_SEARCH_DEVICES;
		}
		else {
			m_refParent.notifySetProgessInfo(-1,"Addressing control devices, Auto config control device instances.");
			this->getAlgControlInputDeviceInstanceAutoParams().start(refContext.m_listAssignedInputDevices);
			refContext.m_state = TASK_COMMISSIONING_STATE_CONTROLDEVICES_INSTANCES_AUTOCFG;	
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskCommissioning_stateControlDevicesInstanceAutoCfgParams(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperControlInputDeviceInstanceAutoParams& refAlg = this->getAlgControlInputDeviceInstanceAutoParams();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices cancelled");
	}
	else if (refAlg.isDone()) {		
		m_refParent.notifySetProgessInfo(-1,"Addressing control devices done.");		

		// Na het addresseren wordt er altijd gezocht naar alle devices.
		refContext.m_shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;
		refContext.m_stateCounter = 0;
		this->getAlgSearchDevices().start();
		refContext.m_state = TASK_SEARCH_DEVICES_STATE_SEARCH_DEVICES;	
	}
	else {
		refAlg.next();
	}
}

/***********************************************************************************/
// Single device addressing.
void CAppDaliFsm::taskCommissioning_stateAddressSingleDevice(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperAssingShortAddress& refAlg = this->getAlgAssingShortAddress();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing single device failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Addressing single device cancelled");
	}
	else if (refAlg.isDone()) {	
		// oppassen:
		// Moeten de status van het algoritme controleren.
		// Er wordt een verify uitgevoerd...
		if (refAlg.success()) {
			m_refParent.notifySetProgessInfo(-1,"Addressing single device done.");		
			this->setStatusDone(refContext);
		}
		else {
			static const bool FLAG_CANCEL_TASK_FALSE = false;
			m_refParent.notifySetProgessInfo(-1,"Addressing single device failed.");
			this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo(),FLAG_CANCEL_TASK_FALSE);	
			this->setStatusDone(refContext);	// NIEUW...
		}
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskCommissioning_stateSwapDeviceAddresses(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperSwapShortAddresses& refAlg = this->getAlgSwapShortAddresses();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(-1,"Swapping device addresses failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(-1,"Swapping device addresses cancelled");
	}
	else if (refAlg.isDone()) {	
		// oppassen:
		// Moeten de status van het algoritme controleren.
		// Er wordt een verify uitgevoerd...
		if (refAlg.success()) {
			m_refParent.notifySetProgessInfo(-1,"Swapping device addresses done.");		
			this->setStatusDone(refContext);
		}
		else {
			static const bool FLAG_CANCEL_TASK_FALSE = false;
			m_refParent.notifySetProgessInfo(-1,"Swapping device addresses failed.");
			this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo(),FLAG_CANCEL_TASK_FALSE);	
			this->setStatusDone(refContext);	// NIEUW...
		}
	}
	else {
		refAlg.next();
	}
}
/***********************************************************************************/
// Single control gear cfg params.
void CAppDaliFsm::taskConfig_stateControlGearCfgProperties(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperControlGearDeviceCfg& refAlg = this->getAlgCfgControlGearProperties();	

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear device failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear device cancelled");
	}
	else if (refAlg.isDone()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear device done.");		
		this->setStatusDone(refContext);
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskConfig_stateControlGearCfgScenes(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperControlGearCfgScenes& refAlg = this->getAlgControlGearCfgScenes();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear scenes failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear scenes cancelled");
	}
	else if (refAlg.isDone()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear scenes done.");		
		this->setStatusDone(refContext);
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskConfig_stateControlGearCfgSingleScene(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperControlGearCfgSingleScene& refAlg = this->getAlgControlGearCfgSingleScene();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear single scene failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear single scene cancelled");
	}
	else if (refAlg.isDone()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear single scene done.");		
		this->setStatusDone(refContext);
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskConfig_stateControlGearCfgDT6DimmerCurve(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperSelectDimmingCurve& refAlg = this->getAlgSelectDimmingCurve();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear dimmercurve failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear dimmercurve cancelled");
	}
	else if (refAlg.isDone()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config control gear dimmercurve done.");		
		this->setStatusDone(refContext);
	}
	else {
		refAlg.next();
	}
}

// Opvragen van de status / actual value van het control gear device.
void CAppDaliFsm::taskConfig_stateControlGearQueryActualLevel(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {

	CAppDaliFsmHelperControlGearActualValueRequest& refAlg = this->getAlgCfgControlGearActualValueRequest();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg, refContext, ev)) {

	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Query actual level failed.");
		this->handleAlgoritmeError(refContext, refAlg.getFatalErrorInfo());
	}
	else if (refAlg.isCancelled()) {
		// m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Query actual level cancelled");
	}
	else if (refAlg.isDone()) {
		// m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Query actual level done.");
		this->setStatusDone(refContext);
	}
	else {
		refAlg.next();
	}
}
	
void CAppDaliFsm::taskConfig_stateControlDeviceInstanceEnableOnOff(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff& refAlg = this->getAlgControlDeviceInstanceEnableOnOff();

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg, refContext, ev)) {

	}
	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Control device instance enable on/off failed.");
		this->handleAlgoritmeError(refContext, refAlg.getFatalErrorInfo());
	}
	else if (refAlg.isCancelled()) {
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Control device instance enable on/off cancelled.");
	}
	else if (refAlg.isDone()) {
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress, "Control device instance enable on/off done.");
		this->setStatusDone(refContext);
	}
	else {
		refAlg.next();
	}
}

void CAppDaliFsm::taskConfig_stateControlDeviceInstances(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev) {
	CAppDaliFsmHelperControlInputDeviceInstancesCfg& refAlg = this->getAlgControlInputDeviceInstancesCfg();	

	// Afhandelen van het event.
	if (!this->helper_state_handleEvent(refAlg,refContext,ev)) {
	
	}

	// Controleren toestand van het algoritme.
	if (refAlg.isError()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config input device failed.");
		this->handleAlgoritmeError(refContext,refAlg.getFatalErrorInfo());	
	}
	else if (refAlg.isCancelled()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config input device cancelled");
	}
	else if (refAlg.isDone()) {	
		m_refParent.notifySetProgessInfo(refContext.m_shortAddress,"Config input device done.");		
		this->setStatusDone(refContext);
	}
	else {
		refAlg.next();
	}
}

/***********************************************************************************/
void CAppDaliFsm::setStatusDone(CAppDaliFsmContext& refContext) {
	TRACER("CAppDaliFsm::setStatusDone()\r\n");
			
	const Task_t task = refContext.m_task;		
	refContext.m_task = APPDALI_FSM_TASK_NONE;
	refContext.m_state = TASK_SEARCH_DEVICES_STATE_INIT;		

	this->stopTimer( );			
	this->cleanUp();
	this->m_refParent.notifyDaliCfgFsmDone(task);			
}

void CAppDaliFsm::handleAlgoritmeError(CAppDaliFsmContext& refContext, const ErrorInfo& refInfo, bool flagCancelTask) {
	this->m_refParent.notifyDaliCfgFsmError(refContext.getTask(), refInfo.getErrorCode(), refInfo.getShortAddress(),refInfo.getErrorMsg());
	if (flagCancelTask) {
		this->cancelTask(refContext);
	}
}

// Timer interval op 10 sec.
// Timer wordt enkel gestart wanneer er een FSM taak bezig is.
void CAppDaliFsm::startTimer(CAppDaliFsmContext& refContext) {
	if (APPDALI_FSM_TASK_NONE == refContext.getTask()) {
		return;
	}
	enum { INTERVAL_TIMER_SEC = 10 };					
	this->m_timer.start(INTERVAL_TIMER_SEC);
}
void CAppDaliFsm::stopTimer( ) {
	this->m_timer.stop();
}

template <typename T>
static void safeDelete(T*& pointer) {
    if (NULL != pointer) {
        delete pointer;
        pointer = NULL;
    }
}

void CAppDaliFsm::cleanUp( ) {
	safeDelete(m_pAlgSearchDevices);							// 0
	safeDelete(m_pAlgControlGearReadMemory);					// 1
	safeDelete(m_pAlgInputDeviceReadMemory);					// 2
	safeDelete(m_pAlgQueryControlGearGroups);					// 3
	safeDelete(m_pAlgQueryControlGearScenes);					// 4
	safeDelete(m_pAlgSearchControlGearDevice);					// 5
	safeDelete(m_pAlgSearchControlInputDevices);				// 6
	safeDelete(m_pAlgQueryControlGearDeviceProperties);			// 7
	safeDelete(m_pAlgQueryInputDeviceProperties);				// 8
	safeDelete(m_pAlgQueryInputDeviceInstances);				// 9
	safeDelete(m_pAlgSearchUnaddressedDevices);					// 10
	safeDelete(m_pAlgAddressDevices);							// 11
	safeDelete(m_pAlgControlGearEraseAddresses);				// 12
	safeDelete(m_pAlgControlInputDeviceEraseAddresses);			// 13
	safeDelete(m_pAlgControlDeviceInstanceAutoParams);			// 14
	safeDelete(m_pAlgAddressSingleDevice);						// 15
	safeDelete(m_pAlgCfgControlGearProperties);					// 16
	safeDelete(m_pAlgControlGearQueryActualLevel);				// 17
	safeDelete(m_pAlgControlGearCfgScenes);						// 18
	safeDelete(m_pAlgControlGearCfgSingleScene);				// 19
	safeDelete(m_pAlgControlDeviceCfgInstanceEnableOnOff);		// 20
	safeDelete(m_pAlgControlDeviceCfgInstances);				// 21
	safeDelete(m_pAlgSwapShortAddresses);						// 22
	safeDelete(m_pAlgSelectDimmingCurve);						// 23
}


template <typename T>
T& CAppDaliFsm::getAlgorithmHelper(T*& pAlgorithmHelper) {
    if (NULL == pAlgorithmHelper) {
        pAlgorithmHelper = new T(*this, this->getTransmitter());
    }
    ASSERT(NULL != pAlgorithmHelper);
    return *pAlgorithmHelper;
}


CAppDaliFsmHelperSearchDevices& CAppDaliFsm::getAlgSearchDevices() {
	return getAlgorithmHelper(m_pAlgSearchDevices);
}

CAppDaliFsmHelperReadMemoryControlGear& CAppDaliFsm::getAlgControlGearReadMemory( ) {
	return getAlgorithmHelper(m_pAlgControlGearReadMemory);
}

CAppDaliFsmHelperReadMemoryInputDevice& CAppDaliFsm::getAlgInputDeviceReadMemory( ) {
	return getAlgorithmHelper(m_pAlgInputDeviceReadMemory);
}

CAppDaliFsmHelperQueryGroupsControlGear& CAppDaliFsm::getAlgControlGearQueryGroups( ) {
	return getAlgorithmHelper(m_pAlgQueryControlGearGroups);
}

CAppDaliFsmHelperQueryScenesControlGear& CAppDaliFsm::getAlgControlGearQueryScenes( ) {
	return getAlgorithmHelper(m_pAlgQueryControlGearScenes);
}

CAppDaliFsmHelperSearchControlGearDevice& CAppDaliFsm::getAlgSearchControlGearDevice( ) {
	return getAlgorithmHelper(m_pAlgSearchControlGearDevice);
}

CAppDaliFsmHelperSearchControlInputDevice& CAppDaliFsm::getAlgSearchControlInputDevices() {
	return getAlgorithmHelper(m_pAlgSearchControlInputDevices);
}

CAppDaliFsmHelperRequestPropertiesControlGear& CAppDaliFsm::getAlgControlGearRequestProperties() {
	return getAlgorithmHelper(m_pAlgQueryControlGearDeviceProperties);
}

CAppDaliFsmHelperRequestPropertiesControlInputDevice& CAppDaliFsm::getAlgInputRequestProperties( ) {
	return getAlgorithmHelper(m_pAlgQueryInputDeviceProperties);
}

CAppDaliFsmHelperRequestControlInputDeviceInstances& CAppDaliFsm::getAlgInputDeviceRequestInstances( ) {
	return getAlgorithmHelper(m_pAlgQueryInputDeviceInstances);
}

CAppDaliFsmHelperDaliDevicesSearchUnaddressed& CAppDaliFsm::getAlgDaliDevicesSearchUnaddressed( ) {
	return getAlgorithmHelper(m_pAlgSearchUnaddressedDevices);
}

CAppDaliFsmHelperDaliDevicesAddress& CAppDaliFsm::getAlgControlDaliDevicesAddressDevices( ) {
	return getAlgorithmHelper(m_pAlgAddressDevices);
}

CAppDaliFsmHelperControlGearErase& CAppDaliFsm::getAlgControlGearEraseAddresses() {
	return getAlgorithmHelper(m_pAlgControlGearEraseAddresses);
}

CAppDaliFsmHelperControlInputDeviceErase& CAppDaliFsm::getAlgControlInputDevicesEraseAddresses() {
	return getAlgorithmHelper(m_pAlgControlInputDeviceEraseAddresses);
}

CAppDaliFsmHelperControlInputDeviceInstanceAutoParams& CAppDaliFsm::getAlgControlInputDeviceInstanceAutoParams() {
	return getAlgorithmHelper(m_pAlgControlDeviceInstanceAutoParams);
}

CAppDaliFsmHelperAssingShortAddress& CAppDaliFsm::getAlgAssingShortAddress( ) {
	return getAlgorithmHelper(m_pAlgAddressSingleDevice);
}

CAppDaliFsmHelperControlGearDeviceCfg& CAppDaliFsm::getAlgCfgControlGearProperties() {
	return getAlgorithmHelper(m_pAlgCfgControlGearProperties);
}

CAppDaliFsmHelperControlGearActualValueRequest& CAppDaliFsm::getAlgCfgControlGearActualValueRequest() {
	return getAlgorithmHelper(m_pAlgControlGearQueryActualLevel);
}

CAppDaliFsmHelperControlGearCfgScenes& CAppDaliFsm::getAlgControlGearCfgScenes() {
	return getAlgorithmHelper(m_pAlgControlGearCfgScenes);
}

CAppDaliFsmHelperControlGearCfgSingleScene& CAppDaliFsm::getAlgControlGearCfgSingleScene() {
	return getAlgorithmHelper(m_pAlgControlGearCfgSingleScene);
}

CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff& CAppDaliFsm::getAlgControlDeviceInstanceEnableOnOff() {
	return getAlgorithmHelper(m_pAlgControlDeviceCfgInstanceEnableOnOff);
}

CAppDaliFsmHelperControlInputDeviceInstancesCfg& CAppDaliFsm::getAlgControlInputDeviceInstancesCfg() {
	return getAlgorithmHelper(m_pAlgControlDeviceCfgInstances);
}

CAppDaliFsmHelperSwapShortAddresses& CAppDaliFsm::getAlgSwapShortAddresses() {
	return getAlgorithmHelper(m_pAlgSwapShortAddresses);
}

CAppDaliFsmHelperSelectDimmingCurve& CAppDaliFsm::getAlgSelectDimmingCurve() {
	return getAlgorithmHelper(m_pAlgSelectDimmingCurve);
}
/***********************************************************************************/
};
};
/***********************************************************************************/