#include "stdafx.h"
#include "appdali.h"
#include "appdalitransmitter.h"
#include "appdalidevices.h"
#include "appdalidevices_t.h"
#include "appdaliprotocol_def.h"
#include "appdalijson.h"
#include "appdalifsm.h"
/***********************************************************************************/
#define DEBUG_LOCAL_ON		0
/***********************************************************************************/
#undef DEBUG_SIMULATE_FSM_ERRORS
/***********************************************************************************/
#if (_DEBUG) 
	#define DEBUG_TEST_CREATE_DATABASE				// appdalidevices_test_buildDatabase(&this->getDevices())	
	#define DEBUG_SIMULATE_FSM_ERRORS				0 // 1
#else
	#define DEBUG_TEST_CREATE_DATABASE
	#define DEBUG_SIMULATE_FSM_ERRORS				0
#endif

/***********************************************************************************/

/***********************************************************************************/
#if (DEBUG_LOCAL_ON==1)
	#define TRACER(x)				TRACE x
#else
	#define TRACER(x)
#endif
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
template <typename T>
static void safeDelete(T*& pointer) {
    if (NULL != pointer) {
        delete pointer;
        pointer = NULL;
    }
}
/***********************************************************************************/
IAppDaliFsm_Parent::~IAppDaliFsm_Parent( ) { }
IAppDaliHost_App::~IAppDaliHost_App() { }

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDali::CAppDali(IAppDaliHost_App* parent, BYTE bNodeAddress, ITcpSocketHomeClient* pSocket) :  
    m_parent(parent), m_bNodeAddress(bNodeAddress), m_pSocket(pSocket),
    m_pTransmitter(NULL), m_pFsm(NULL), m_pFsmContext(NULL), m_pPendingErrors(NULL),
	m_pDaliDevices(NULL), m_pNwkDaliDevices(NULL), m_pDaliNamesGroups(NULL), m_pDaliNamesScenes(NULL),
	m_pObserver(NULL), m_flags(0)  
{
	m_pTransmitter = new CAppDaliTransmitter(pSocket);
	ASSERT(NULL != m_pTransmitter);

	// Subscribe.
	if (NULL != pSocket) {
		pSocket->subscribe(this);
	}

	DEBUG_TEST_CREATE_DATABASE;				// Debug code...
}

CAppDali::~CAppDali( ) {

	// Unsubscribe...
	if (NULL != m_pSocket) {
		m_pSocket->unSubscribe(this);
	}	
	// Deallocatie.
	safeDelete(m_pTransmitter);
	safeDelete(m_pFsm);
	safeDelete(m_pFsmContext);
	safeDelete(m_pPendingErrors);
	safeDelete(m_pNwkDaliDevices);
	safeDelete(m_pDaliDevices);
	safeDelete(m_pDaliNamesGroups);
	safeDelete(m_pDaliNamesScenes);
		
	m_pObserver = NULL;
}

void CAppDali::subscribeGUI(IAppDlgDaliGui_Host* pObserver) {
	if (NULL !=pObserver) {
		TRACE("CAppDali::subscribeGUI\r\n");
	}
	else {
		TRACE("CAppDali::subscribeGUI(NULL)\r\n");
	}
	this->m_pObserver = pObserver;
}

// Zoeken naar devices:
// - Fouten wissen.
// - Fsm starten.
// - Database wissen.
bool CAppDali::startSearchDevices( ) {
	bool result = false;
	
	this->clearPendingErrors();

	result = this->getFsm().startSearchDevices(this->getContext());
	if (result) {
		CAppDaliDevices& refDevices = this->getNwkDevices();
		refDevices.erase();	
	}
	return result;
}

// Addressering van devices.
// - Fouten wissen.
// - Fsm starten.
// - Database wissen.
bool CAppDali::startAddressing(AddressingInstallation_t type) {
	bool result = false;

	this->clearPendingErrors();

	result = this->getFsm().startAddressing(this->getContext(), type);
	if (result) {		
		this->getNwkDevices().erase();			
		if (APPDALI_ADDRESSING_INSTALLATION_NEW == type) {
			this->getDevices().erase();
		}
	}
	return result;
}

bool CAppDali::startAddressingSingleDevice(DaliDeviceType_t type, int shortAddress, int newShortAddress) {	
	return this->getFsm().startAddressSingleDevice(this->getContext(), type, shortAddress, newShortAddress);
}

bool CAppDali::startAddressingSwapDevices(DaliDeviceType_t type, int a0, int a1) {
	REQUIRE((a0>=DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (a0 <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST));
	REQUIRE((a1>=DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (a1 <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST));
	return this->getFsm().startAddressingSwapDeviceAddresses(this->getContext(), type, a0, a1);
}

// Oppassen:
// Er wordt een clone gemaakt van de control gear properties.
// Deze wordt bijgehouden in de FSM/Worker.
// NOTE: Indien de functie mislukt moeten we hier de properties dealloceren.
bool CAppDali::startUploadControlGearDeviceProperties(int shortAddress, CAppDaliDeviceProperties_ControlGear& refProperties) {
	CAppDaliDeviceProperties_ControlGear* const properties = refProperties.clone();
	if (NULL == properties) {
		return false;
	}
	if (!this->getFsm().startUploadControlGearDeviceProperties(this->getContext(),shortAddress,properties)) {
		delete properties;	// NOTE.
		return false;
	}
	return true;
}

bool CAppDali::startUploadControlGearDeviceScenes(int shortAddress, CAppDaliDeviceProperties_ControlGear& refProperties) {
	CAppDaliDeviceProperties_ControlGear* const properties = refProperties.clone();
	if (NULL == properties) {
		return false;
	}
	if (!this->getFsm().startUploadControlGearDeviceScenes(this->getContext(),shortAddress,properties)) {
		delete properties;	// NOTE.
		return false;
	}
	return true;
}

bool CAppDali::startUploadControlGearDeviceSingleScene(int shortAddress, int sceneNumber, CAppDaliDeviceProperties_ControlGear& refProperties) {
	CAppDaliDeviceProperties_ControlGear* const properties = refProperties.clone();
	if (NULL == properties) {
		return false;
	}
	if (!this->getFsm().startUploadControlGearDeviceSingleScene(this->getContext(),shortAddress,sceneNumber, properties)) {
		delete properties;	// NOTE.
		return false;
	}
	return true;
}

bool CAppDali::startUploadControlGearDimmerCurve(int shortAddress, int deviceType,int dimmerCurve) {
	return this->getFsm().startUploadControlGearDimmerCurve(this->getContext(),shortAddress,deviceType,dimmerCurve);
}

bool CAppDali::startControlGearQueryActualLevel(int shortAddress) {
	return this->getFsm().startRequestControlGearActualValue(this->getContext(), shortAddress);
}

bool CAppDali::startUploadControlInputDeviceInstanceEnableOnOff(int shortAddress,int instanceNumber,bool stateOnOff) {
	return this->getFsm().startUploadControlInputDeviceInstanceEnableOnOff(this->getContext(), shortAddress, instanceNumber, stateOnOff);
}

bool CAppDali::startUploadControlInputDeviceInstances(int shortAddress, CAppDaliInputControlDeviceInstances& refProperties) {
	CAppDaliInputControlDeviceInstances* const properties = refProperties.clone();
	if (NULL == properties) {
		return false;
	}
	if (!this->getFsm().startUploadControlInputDeviceInstances(this->getContext(),shortAddress,properties)) {
		delete properties;	// NOTE.
		return false;
	}
	return true;
}

bool CAppDali::startReadControlGearDeviceProperties(int shortAddress) {
	return this->getFsm().startReadControlGearDeviceProperties(this->getContext(), shortAddress);
}
bool CAppDali::startReadControlGearDeviceGroups(int shortAddress) {
	return this->getFsm().startReadControlGearDeviceGroups(this->getContext(), shortAddress);
}
bool CAppDali::startReadControlGearDeviceScenes(int shortAddress) {
	const bool requestDT8ColorInfo = this->fsmDbaseControlGearIsDT8ColorObjectSupported(DEVICE_DATABASE_DOCUMENT,shortAddress);	
	return this->getFsm().startReadControlGearDeviceScenes(this->getContext(), shortAddress, requestDT8ColorInfo);
}

bool CAppDali::cancelTask( ) {
	bool result = false;
	result = this->getFsm().cancelTask(this->getContext());
	return result;
}

void CAppDali::pollSystem(int intervalSec) {					
	this->getFsm().poll(this->getContext(), intervalSec);
}

Task_t CAppDali::getFsmCurrentTask( ) {
	return this->getContext().getTask();
}

CAppDaliTransmitter& CAppDali::getTransmitter( ) {
	return *m_pTransmitter;
}

CAppDaliFsm& CAppDali::getFsm( ) {
	if (NULL == m_pFsm) {
		m_pFsm = new CAppDaliFsm(*this, this->getTransmitter());
	}
	ASSERT(NULL != m_pFsm);
	return *m_pFsm;
}

CAppDaliFsmContext& CAppDali::getContext() {
	if (NULL == m_pFsmContext) {
		m_pFsmContext = new CAppDaliFsmContext( );
	}
	ASSERT(NULL != m_pFsmContext);
	return *m_pFsmContext;
}

CAppDaliDevices& CAppDali::getNwkDevices() {
	if (NULL == m_pNwkDaliDevices) {
		m_pNwkDaliDevices = new CAppDaliDevices( );
	}
	ASSERT(NULL != m_pNwkDaliDevices);
	return *m_pNwkDaliDevices;
}

CAppDaliDevices& CAppDali::getDevices() {
	if (NULL == m_pDaliDevices) {
		m_pDaliDevices = new CAppDaliDevices( );
	}
	ASSERT(NULL != m_pDaliDevices);
	return *m_pDaliDevices;
}

CAppDaliCollectionNames& CAppDali::getNamesGroups( ) {
	if (NULL == m_pDaliNamesGroups) {
		m_pDaliNamesGroups = new CAppDaliCollectionNames(DALI_SYSTEM_CONTROLGEAR_GROUP_MAX);
		ASSERT(NULL != m_pDaliNamesGroups);
		for (int i=DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST;i<DALI_SYSTEM_CONTROLGEAR_GROUP_MAX;++i) {
			CString str;
			str.Format("Dali group %d",i);
			m_pDaliNamesGroups->setName(i,str);
		}
	}
	ASSERT(NULL != m_pDaliNamesGroups);
	return *m_pDaliNamesGroups;
}

CAppDaliCollectionNames& CAppDali::getNamesScenes( ) {
	if (NULL == m_pDaliNamesScenes) {
		m_pDaliNamesScenes = new CAppDaliCollectionNames(DALI_SYSTEM_CONTROLGEAR_SCENE_MAX);
		ASSERT(NULL != m_pDaliNamesScenes);
		for (int i=DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST;i<DALI_SYSTEM_CONTROLGEAR_SCENE_MAX;++i) {
			CString str;
			str.Format("Dali scene %d",i);
			m_pDaliNamesScenes->setName(i,str);
		}
	}
	ASSERT(NULL != m_pDaliNamesScenes);
	return *m_pDaliNamesScenes;
}


// TODO_HOMECLIENT_DALI
// Status van de connectie weergeven aan de parent dialoog.

// public ITcpSocketHomeClient_Host
void CAppDali::onStateConnectChanged(int id,bool connected) {
	if (NULL == this->m_pObserver) {
		return;
	}
	this->m_pObserver->onNotifyGuiStatusConnected(this->m_bNodeAddress,connected);
}

void CAppDali::onStateChanged(int id, TcpSocketHomeClient_Tasks::State_t state) {

}

// Bericht...
void CAppDali::onSocketHomeClientMsgInfo(int id, CategoryMsg_t category, const CString& msg) {
	if (HOMECLIENT_MSG_CATEGORY_MASTERTASK_FSM == category) {
		return;
	}
	if (NULL != this->m_pObserver) {
		this->m_pObserver->onNotifyGuiMsgCommunication(this->m_bNodeAddress,IAppDlgDaliGui_Host::TYPE_MSG_INFO, msg);
	}
}

void CAppDali::onSocketHomeClientMsgError(int id, CategoryMsg_t category, const CString& msg) {
	if (HOMECLIENT_MSG_CATEGORY_MASTERTASK_FSM == category) {
		return;
	}
	if (NULL != this->m_pObserver) {
		this->m_pObserver->onNotifyGuiMsgCommunication(this->m_bNodeAddress,IAppDlgDaliGui_Host::TYPE_MSG_ERROR, msg);
	}
}

void CAppDali::onTaskStatus(int id, TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) {

}

void CAppDali::onTaskCancelStatus(int id, TcpSocketHomeClient_Tasks::Status_t status) {

}

void CAppDali::onNodeDbaseStatus(int id, TcpSocketHomeClient_NodeDbase::Status_t status) {

}


void CAppDali::onStatusDALIRxMsgSubscribe(int id,bool status) {
	// todo implement...
}

void CAppDali::onStatusDALIResponse(int id, int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData) {
	
#if (DEBUG_LOCAL_ON==1)
	CString strInfo;
	strInfo.Format("onStatusDALIResponse(%s, data=0x%02x\r\n",CAppDali_Utils::getStrResponseCode(type),responseData);
	TRACE(strInfo);
#endif

	//
	// TODO: Controle van de client & message identifier...
	//

	switch(type) {
		case DALI_RESULT_TX_OK: {
			// Transmit ok, geen backward frame.
			this->getFsm().setDaliFsmEventTransmitDone(this->getContext());
		} break;	
		case DALI_RESULT_RESPONSE_BACKWARD_FRAME: {	
			// Transmit ok, Backward response frame in data.
#if (DEBUG_SIMULATE_FSM_ERRORS == 1)
			typedef enum { FREQUENCY = 100 };
			const int randomValue = rand();
			if (0 == ((randomValue+1) % FREQUENCY)) {			
				if ((randomValue%2) == 1) {
					this->getFsm().setDaliFsmEventBackwardResponseError(this->getContext());				
				}
				else {
					this->getFsm().setDaliFsmEventNoBackwardResponse(this->getContext());
				}
			}
			else {
				this->getFsm().setDaliFsmEventBackwardResponse(this->getContext(), responseData);
			}
#else			
			this->getFsm().setDaliFsmEventBackwardResponse(this->getContext(), responseData);
#endif
		} break;
		case DALI_RESULT_EC_RESPONSE_TIMEOUT: {		
			// Backward response niet/te laat ontvangen.
			this->getFsm().setDaliFsmEventNoBackwardResponse(this->getContext());
		} break;
		case DALI_RESULT_EC_RESPONSE_INVALID:		// Backward response data error - discard data.
		case DALI_RESULT_EC_RESPONSE_TOOEARLY: {	// Backward response data te vroeg ontvangen - discard data.
			// Backward response verkeerd antwoord:
			// Meerdere die samen anwtoorden / invalid frames...
			this->getFsm().setDaliFsmEventBackwardResponseError(this->getContext());
		} break;
		case DALI_RESULT_TX_EC_BUS_SYSTEMFAILURE: {
			// Kunnen geen frame versturen omdat de bus actief blijft (bus niet gevoed ???)									  			
			const Task_t currentTask = this->getContext().getTask();
			if (APPDALI_FSM_TASK_NONE != currentTask) {
				this->getFsm().cancelTask(this->getContext());
				if (NULL != this->m_pObserver) {
					const CString strErrorMsg("DALI-bus system failure. Cannot transmit messages to the DALI bus system. "
										      "Check the DALI-bus system.");
					this->m_pObserver->onNotifyGuiTaskError(this->m_bNodeAddress,currentTask,strErrorMsg);
				}
			}
		} break;

		case DALI_RESULT_EC_GENERAL:
		case DALI_RESULT_TX_EC_WRONGSTATE:			// Kunnen geen frame versturen want de tx van een vorige is nog bezig.		
		case DALI_RESULT_NONE: {				
			ASSERT(0); // todo...
		} break;
	}	
}

void CAppDali::onRxDALIMessage(int id, BYTE lengthBits, uint32_t recievedFrame) {

#if (DEBUG_LOCAL_ON==1)
	CString strInfo;
	strInfo.Format("onRxDALIMessage(lengthBits=%d, data=0x%04x)\r\n",lengthBits,recievedFrame);
	TRACE(strInfo);
#endif 

	if (NULL != this->m_pObserver) {
		this->m_pObserver->onNotifyGuiRxDaliMessage(this->m_bNodeAddress,lengthBits,recievedFrame);
	}
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Interface voor de FSM...

// Na het zoeken naar devices op de DALI bus wordt de configuratie 
// opgeslagen onder de CONFIG directory.
// Bestandsnaam: 'dalibus_%02X.json'	(vb:'dalibus_FC.json')
void CAppDali::notifyDaliCfgFsmDone(Task_t task) {
	bool saveOnlyWhenChanges = true;		// Sedert V16.47: Oplossing BUG-0181: Bepaalt of de data altijd mag opgeslagen worden.
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task ||
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task) 
	{		
		// Document database aanmaken. 
		const bool mergeData = (APPDALI_FSM_TASK_COMMISSIONING_NEW == task ? false : true);		

		// Openstaande founten wissen en opvragen van de nieuwe fouten die gedecteerd werd in het netwerk via de fsm/algoritmes.
		this->clearPendingErrors();
		this->m_pPendingErrors = this->getContext().getCopyFsmErrors();
		ASSERT(NULL != this->m_pPendingErrors);

		// Database vergelijken.
		// Bij verschillen wordt de setPropertyChanged() aangeroepen.
		// Afhankelijk van deze weten we of de data moet opgeslagen worden + of er nieuwe Duotecno Dali bridge config 
		// nodig is.
		this->databaseDetectChanges();

		// Database zoeken op fouten.
		// Mogelijke fouten: 
		//	- Een device in het netwerk staat in fout toestand. (Dubbele addressen).
		//	- Een device die aanwezig is in de offline database werd niet gevonden in het netwerk.
		// Wanneer er fouten zijn dan wordt de globale fout toestand vlag gezet zie. (FLAG_DATABASE_PENDING_ERRORS)	
		// Er worden fout entries aangemaakt en bijgevoegd aan de lijst van de pending errors.
		this->databaseDectectErrors();

		// Database copieren... (MergeData is enkel actief wanneer we NIET starten met een nieuwe installatie.)
		// Alle namen van de database mergen.
		// Wanneer een devices niet gevonden werd in het netwerk maar wel aanwezig is in de offline database 
		// wordt deze toegevoegd met een errorCode Not found.
		this->copyDatabaseNetwork2DocumentDevices(mergeData);

		// Sedert V16.47: zie BUG-0181
		// Zorgt ervoor dat na het uitvoeren van een taak de database altijd opgeslagen wordt en 
		// er een refresh gebeurd in de DALI Duotecno bridge configuartie.
		saveOnlyWhenChanges = false;
	}

	// Sedert V16.47: Oplossing probleem: zie BUG-0181
	// Na het uitvoeren van een taak wordt de database opnieuw opgeslagen en wordt er een refresh naar de configurator verstuurd.
	// Dit is enkel nodig bij het uitvoeren van adressering (=opstarten) van de installatie.
	// Vroeger werd dit ook uitgevoerd bij het opvragen van de actual value van een control gear device.
	// Gebruiken nu de vlag 'saveOnlyWhenChanges'. Bij het addresseren is deze vlag altijd false -> er wordt altijd data opgeslagen.
	// Bij de andere algoritmes wordt de data enkel opgeslagen bij veranderingen.
	//
	// Opslaan van de DALI devices.
	this->saveDatabaseDevices(saveOnlyWhenChanges);

	// Gui updaten.
	if (NULL != m_pObserver) {
		m_pObserver->onNotifyGuiTaskDone(this->m_bNodeAddress,task);
	}
}


void CAppDali::notifyDaliCfgFsmError(Task_t task, DaliErrorCode_t error,int shortAddress,const CString& strErrorDetail) {

	CString strMessageError;	// ("Error occured - todo description");						// TODO TODO_FSM_DALI_ERROR

	switch(error) {
		case ERROR_CODE_NONE: {
			ASSERT(0);							  
		} break;

		case ERROR_CODE_COMMISSIONING_SHORTADDRESS_EXCEED_MAX: {
			strMessageError = CString("Error addressing devices. Exceeded maximum short addresses");
		} break;

		case ERROR_CODE_TIMER_EXPIRY: {
			strMessageError = CString("Error: No response from the DALI bus system\r\n\r\n"
									  "This error is likely caused by a hardware issue. "
									  "Check the physical connections and bus power on the DALI system bus.");				  
		} break;

		case ERROR_CODE_SHORTADDRESS_DOUBLE:
		case ERROR_CODE_COMMISSIONING_SHORTADDRESS_VERIFY: {
			strMessageError = CString("Error addressing devices.\r\n" 
									  "Possible devices share the same short address.");										   
		} break;

		case ERROR_CODE_WRONG_EVENT: {
			strMessageError = CString("Recieved a wrong event which could not be processed.\r\n"
									  "Check the DALI systembus and Duotecno firmware version if the problem persists.");
		} break;

		case ERROR_CODE_PROPERTY_NORESPONSE: {
			strMessageError.Format("No response from DALI device %d",shortAddress);
		} break;

		case ERROR_CODE_PROPERTY_WRONGRESPONSE: {
			strMessageError.Format("Received frame error from DALI device %d",shortAddress);				
		} break;
		// TODO... TODO_FSM_DALI_ERROR

		case ERROR_CODE_DATABASE_DEVICE_NOT_FOUND: {
			strMessageError = CString("The DALI device was not found in the DALI network.");								   
		} break;
		default: {
			ASSERT(0);
			strMessageError = CString("Error occurred.");
		} break;
	}

	if (!strErrorDetail.IsEmpty()) {
		strMessageError += "\r\n\r\nError detail: '";
		strMessageError += strErrorDetail;
		strMessageError += "'";
	}
	if (NULL != m_pObserver) {
		m_pObserver->onNotifyGuiTaskError(this->m_bNodeAddress,task,strMessageError);
	}
}

void CAppDali::notifyDaliCfgFsmStarted(Task_t task) {
	if (NULL != m_pObserver) {
		m_pObserver->onNotifyGuiTaskStarted(this->m_bNodeAddress,task);
	}
}
/***********************************************************************************/
BOOL CAppDali::getDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,CString* pStrName) {
	CAppDaliDevice* pDevice = NULL;
	if (!this->getDevices().find(deviceType,shortAddress,&pDevice)) {
		return FALSE;
	}
	*pStrName = pDevice->getName();
	return TRUE;
}

BOOL CAppDali::setDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,const CString& strName,CString* pStrNormalized) {
	CAppDaliDevice* pDevice = NULL;
	if (!this->getDevices().find(deviceType,shortAddress,&pDevice)) {
		return FALSE;
	}

	int maxLength = CONTROLGEAR_DEVICE_MAX_NAME_LENGTH;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == deviceType) {
		maxLength = CONTROLGEAR_DEVICE_MAX_NAME_LENGTH;
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == deviceType) {
		maxLength = CONTROLINPUT_DEVICE_MAX_NAME_LENGTH;	
	}

	const CString strNormalized = strName.Left(maxLength);
	if (0 == strNormalized.Compare(pDevice->getName())) {
		if (NULL != pStrNormalized) {
			*pStrNormalized = strNormalized;
		}
		return TRUE;
	}

	pDevice->setName(strNormalized);
	if (NULL != pStrNormalized) {
		*pStrNormalized = strNormalized;
	}
	this->setPropertyChanged(PROPERTY_CHANGED_NAME);	// Vragen om data op te slaan + IPC melding naar de Configurator.
	return TRUE;
}

BOOL CAppDali::getControlDeviceInstanceName(int shortAddress,int instanceNumber, CString* pStrName) {
	CAppDaliInputControlDeviceInstance* properties = NULL;
	if (!this->getDeviceInstanceProperties(this->getDevices(),shortAddress,instanceNumber,&properties,false)) {
		return FALSE;
	}
	ASSERT(NULL != properties);
	*pStrName = properties->getName();
	return TRUE;
}

BOOL CAppDali::setControlDeviceInstanceName(int shortAddress,int instanceNumber,const CString& strName,CString* pStrNormalized) {
	CAppDaliInputControlDeviceInstance* properties = NULL;
	if (!this->getDeviceInstanceProperties(this->getDevices(),shortAddress,instanceNumber,&properties,false)) {
		return FALSE;
	}	
	const CString strNormalized = strName.Left(CONTROLINPUT_DEVICE_INSTANCE_MAX_NAME_LENGTH);
	ASSERT(NULL != properties);
	if (0 == strNormalized.Compare(properties->getName())) {
		if (NULL != pStrNormalized) {
			*pStrNormalized = strNormalized;
		}
		return TRUE;
	}
	properties->setName(strNormalized);
	if (NULL != pStrNormalized) {
		*pStrNormalized = strNormalized;
	}
	this->setPropertyChanged(PROPERTY_CHANGED_NAME); // Vragen om data op te slaan + IPC melding naar de Configurator.
	return TRUE;
}

/***********************************************************************************/

// Context stats aanvullen
void CAppDali::fsmAssignedNewShortAddress(DaliDeviceType_t type, int shortAddress) {
	this->getContext().addAssigned(type, shortAddress);

	if (NULL != this->m_pObserver) {
		static const bool newDevice_yes = true;
		this->m_pObserver->onNotifyGuiFoundDevice(this->m_bNodeAddress,type,shortAddress, newDevice_yes);
	}
}

bool CAppDali::fsmDbaseSetDeviceError(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress,DaliErrorCode_t errorCode) {
	if (shortAddress<DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST || shortAddress>DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
		return false;
	}

	// Globale fout vlag zetten (indien nog niet aktief staat).
	this->m_flags |= FLAG_DATABASE_PENDING_ERRORS;

	// Device markeren in fout toestand.
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;
	if (refDevices.find(type, shortAddress,&pDevice)) {
		pDevice->setErrorCode(errorCode);
		pDevice->setErrorStatus(ERROR_STATUS_PENDING);
		return true;
	}

	pDevice = new CAppDaliDevice(type);
	if (NULL == pDevice) {
		return false;
	}
	pDevice->setShortAddress(shortAddress);
	if (!refDevices.add(pDevice)) {
		delete pDevice;
		return false;
	}
	pDevice->setErrorCode(errorCode);
	pDevice->setErrorStatus(ERROR_STATUS_PENDING);
	return true;
}

bool CAppDali::fsmDbaseAddNewDevice(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress) {
	if (shortAddress<DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST || shortAddress>DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
		return false;
	}
	CAppDaliDevice* pDevice = NULL;
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	if (refDevices.find(type, shortAddress,&pDevice)) {
		return false;
	}
	pDevice = new CAppDaliDevice(type);
	if (NULL == pDevice) {
		return false;
	}
	pDevice->setShortAddress(shortAddress);
	if (!refDevices.add(pDevice)) {
		delete pDevice;
		return false;
	}
	if (NULL != this->m_pObserver) {
		static const bool newDevice_no = false;
		this->m_pObserver->onNotifyGuiFoundDevice(this->m_bNodeAddress,type,shortAddress, newDevice_no);
	}
	return true;
}


// TODO LOW_PRIO: Kunnen we hiervoor een proxy gebruiken (dmv het shortAddress)
// Evt in de klasse CAppDaliDevices ??
bool CAppDali::fsmDbaseSetDeviceGTIN(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, unsigned long long GTIN) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;	
	if (!refDevices.find(type,shortAddress,&pDevice)) {
		return false;
	}
	pDevice->setGTIN(GTIN);
	if (NULL != this->m_pObserver) {
		this->m_pObserver->onNotifyGuiUpdateDeviceGTIN(this->m_bNodeAddress,type,shortAddress,GTIN);
	}
	return true;
}

bool CAppDali::fsmDbaseSetDeviceSerialNumber(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, unsigned long long serialNumber) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;	
	if (!refDevices.find(type,shortAddress,&pDevice)) {
		return false;
	}
	pDevice->setSerialcode(serialNumber);

	if (NULL != this->m_pObserver) {
		this->m_pObserver->onNotifyGuiUpdateDeviceSerialNumber(this->m_bNodeAddress,type,shortAddress,serialNumber);
	}
	return true;
}

bool CAppDali::fsmDbaseSetDeviceFirmwareVersion(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, int v) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;	
	if (!refDevices.find(type,shortAddress,&pDevice)) {
		return false;
	}
	pDevice->setVersionFirmware(v);
	return true;
}

bool CAppDali::fsmDbaseSetDeviceHardwareVersion(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, int v) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;	
	if (!refDevices.find(type,shortAddress,&pDevice)) {
		return false;
	}
	pDevice->setVersionHardware(v);
	return true;
}

// TODO LOW_PRIO: Kunnen we hiervoor een proxy gebruiken (dmv het shortAddress)?
// Interface voor de FSM: control gear properties.
bool CAppDali::fsmDbaseSetControlGearGroupMemberInfo(DeviceDatabaseType_t databaseType, int shortAddress, unsigned short value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (!this->getDeviceProperties(refDevices,shortAddress, &properties)) {
		return false;
	}
	properties->setGroupInfo(value);
	return true;
}

bool CAppDali::fsmDbaseSetControlGearSceneLevel(DeviceDatabaseType_t databaseType, int shortAddress, int sceneNumber, int level) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (!this->getDeviceProperties(refDevices, shortAddress, &properties)) {
		return false;
	}
	properties->setSceneLevel(sceneNumber, level);
	return true;
}

bool CAppDali::fsmDbaseSetControlGearDeviceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setDeviceStatus);	
}

bool CAppDali::fsmDbaseSetControlGearDeviceVersion(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setVersionNumber);	
}

bool CAppDali::fsmDbaseSetControlGearDeviceExtendedFadeTime(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setExtendedFadeTime);	
}

bool CAppDali::fsmDbaseSetControlGearDevicePowerOnLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setLevelPowerOn);	
}

bool CAppDali::fsmDbaseSetControlGearDeviceFailureLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setLevelSystemFailure);
}

bool CAppDali::fsmDbaseSetControlGearDeviceType(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setDeviceType);
}

bool CAppDali::fsmDbaseSetControlGearMinimumLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setLevelMin);
}

bool CAppDali::fsmDbaseSetControlGearMaximumLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setLevelMax);
}

bool CAppDali::fsmDbaseSetControlGearFadeTimeRate(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setFadeTimeRate);
}

bool CAppDali::fsmDbaseSetControlGearPhysicalMinValue(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setPhysicalMinValue);
}

bool CAppDali::fsmDbaseSetControlGearPowerOnLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setLevelPowerOn);
}

bool CAppDali::fsmDbaseSetControlGearSystemFailureLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress,value,&CAppDaliDeviceProperties_ControlGear::setLevelSystemFailure);
}
bool CAppDali::fsmDbaseSetControlGearDimmingCurve(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	const bool result = this->helper_setControlGearProperty(refDevices, shortAddress, value, &CAppDaliDeviceProperties_ControlGear::setDimmingCurve);
	if (result && (databaseType == DEVICE_DATABASE_DOCUMENT)) {
		this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);	// Vragen om data op te slaan.
	}
	return result;
}

bool CAppDali::fsmDbaseSetControlGearDeviceTypes(DeviceDatabaseType_t databaseType, int shortAddress, CList<int>& list) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (!this->getDeviceProperties(refDevices, shortAddress, &properties)) {
		return false;
	}
	ASSERT(NULL != properties);
	properties->setDeviceTypes(list);
	if (databaseType == DEVICE_DATABASE_DOCUMENT) {
		this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);	// Vragen om data op te slaan.
	}
	return true;
}

bool CAppDali::fsmDbaseSetControlGearColorFeatures(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	const bool result = this->helper_setControlGearProperty(refDevices, shortAddress, value, &CAppDaliDeviceProperties_ControlGear::setColorFeatures);
	if (result && (databaseType == DEVICE_DATABASE_DOCUMENT)) {
		this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);	// Vragen om data op te slaan.
	}
	return result;
}


// ControlInputDevice
bool CAppDali::fsmDbaseSetControlInputDeviceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceProperty(refDevices, shortAddress, value, &CAppDaliDeviceProperties_InputControlDevice::setDeviceStatus);
}

bool CAppDali::fsmDbaseSetControlInputDeviceVersion(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress, value, &CAppDaliDeviceProperties_InputControlDevice::setVersionNumber);
}

bool CAppDali::fsmDbaseSetControlInputDeviceOperatingMode(DeviceDatabaseType_t databaseType, int shortAddress, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlGearProperty(refDevices, shortAddress, value, &CAppDaliDeviceProperties_InputControlDevice::setOperatingMode);
}

bool CAppDali::fsmDbaseSetControlInputDeviceGroupInfo(DeviceDatabaseType_t databaseType, int shortAddress, uint32_t groupInfo) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceProperty(refDevices, shortAddress, groupInfo, &CAppDaliDeviceProperties_InputControlDevice::setGroupInfo);
}

bool CAppDali::fsmDbaseSetControlInputDeviceAppControllerEnabled(DeviceDatabaseType_t databaseType, int shortAddress, bool v) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceProperty(refDevices, shortAddress, v, &CAppDaliDeviceProperties_InputControlDevice::setAppControllerEnabled);
}

bool CAppDali::fsmDbaseSetControlInputDevicePowerCycleNotification(DeviceDatabaseType_t databaseType, int shortAddress, bool v) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceProperty(refDevices, shortAddress, v, &CAppDaliDeviceProperties_InputControlDevice::setPowerCycleNotification);
}

bool CAppDali::fsmDbaseSetControlInputDeviceDeviceCapabilities(DeviceDatabaseType_t databaseType, int shortAddress, int v) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceProperty(refDevices, shortAddress, v, &CAppDaliDeviceProperties_InputControlDevice::setDeviceCapabilities);
}

bool CAppDali::fsmDbaseSetControlInputDeviceRandomAddress(DeviceDatabaseType_t databaseType, int shortAddress, uint32_t v) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceProperty(refDevices, shortAddress, v, &CAppDaliDeviceProperties_InputControlDevice::setRandomAddress);
}

// ControlInputDevice Instances.
bool CAppDali::fsmDbaseSetControlInputDeviceInstanceType(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceInstanceProperty(refDevices, shortAddress, instanceID, value, &CAppDaliInputControlDeviceInstance::setInstanceType);
}

bool CAppDali::fsmDbaseSetControlInputDeviceInstanceEnabledOnOff(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,bool value){
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceInstanceProperty(refDevices, shortAddress, instanceID, value, &CAppDaliInputControlDeviceInstance::setInstanceActive);
}

bool CAppDali::fsmDbaseSetControlInputDeviceInstanceEventScheme(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value){
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceInstanceProperty(refDevices, shortAddress, instanceID, value, &CAppDaliInputControlDeviceInstance::setEventScheme);
}

bool CAppDali::fsmDbaseSetControlInputDeviceInstanceEventPrio(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value){
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceInstanceProperty(refDevices, shortAddress, instanceID, value, &CAppDaliInputControlDeviceInstance::setEventPriority);
}

bool CAppDali::fsmDbaseSetControlInputDeviceInstanceEventFilters(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,uint32_t value){
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceInstanceProperty(refDevices, shortAddress, instanceID, value, &CAppDaliInputControlDeviceInstance::setEventFilter);
}

bool CAppDali::fsmDbaseSetControlInputDeviceInstanceGroupInfo(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int idxGroup, int value){
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliInputControlDeviceInstance* pProperties = NULL;
	if (!getDeviceInstanceProperties(refDevices, shortAddress, instanceID, &pProperties, true)) {
		return false;
	}
	pProperties->setInstanceGroup(idxGroup, value);
	return true;
}

bool CAppDali::fsmDbaseSetControlInputDeviceInstanceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int instanceID, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceInstanceProperty(refDevices, shortAddress, instanceID, value, &CAppDaliInputControlDeviceInstance::setStatus);
}

bool CAppDali::fsmDbaseSetControlInputDeviceInstanceResolution(DeviceDatabaseType_t databaseType, int shortAddress, int instanceID, int value) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	return this->helper_setControlDeviceInstanceProperty(refDevices, shortAddress, instanceID, value, &CAppDaliInputControlDeviceInstance::setEventResolution); 
}

///////////////////////////////////////////////////
// Document database acties.
bool CAppDali::fsmDbaseTaskRemovedDaliDevice(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress) {

	// Verwijderen uit de database.
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	if (false == refDevices.remove(type, shortAddress)) {
		return false;
	}

	// Verwijderen uit de lijst met pending errors.
	// Indien de lijst met fouten leeg dealloceren.
	if (CAppDaliFsmErrorEntries::removeErrorEntry(this->m_pPendingErrors, type, shortAddress)) {	
		if (NULL != this->m_pPendingErrors) {
			if (this->m_pPendingErrors->getCount() <= 0) {			
				delete this->m_pPendingErrors;
				this->m_pPendingErrors = NULL;
			}
		}
	}

	// Doorgeven dat de database gewijzigd werd -> opslaan.
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_DATABASE);

	// Na het verwijderen van een device controleren of de database toestand ok is...
	// In deze functie wordt de globale FLAG_DATABASE_PENDING_ERRORS gewist.
	this->databaseCheckErrorState();		
	return true;
}

bool CAppDali::fsmDbaseTaskChangedDaliDeviceShortAddress(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress,int newAddress) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;
	if (false == refDevices.find(type,shortAddress,&pDevice)) {
		return false;
	}
	if (NULL == pDevice) {
		INVARIANT(NULL != pDevice);
		return false;
	}
	pDevice->setShortAddress(newAddress);
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_DATABASE);
	return true;
}

bool CAppDali::fsmDbaseTaskSwappedDaliDeviceShortAddresses(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int a0,int a1) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	if (a0 == a1) {
		REQUIRE(a0 != a1);
		return false;
	}
	CAppDaliDevice* p0 = NULL;
	if (false == refDevices.find(type,a0,&p0)) {
		return false;
	}
	CAppDaliDevice* p1 = NULL;
	if (false == refDevices.find(type,a1,&p1)) {
		return false;
	}
	INVARIANT(NULL != p0 && NULL != p1);
	p0->setShortAddress(a1);
	p1->setShortAddress(a0);
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_DATABASE);
	return true;
}

bool CAppDali::fsmDbaseControlGearPropertiesCfgUpdate(DeviceDatabaseType_t databaseType, int shortAddress,const CAppDaliDeviceProperties_ControlGear& properties) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;
	if (false == refDevices.find(DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,&pDevice)) {
		return false;
	}
	INVARIANT(NULL != pDevice);
	CAppDaliDeviceProperties_ControlGear* const pDest = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear::copy(pDest, properties);
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);		// Mag opgeslagen worden...
	return true;
}

bool CAppDali::fsmDbaseControlGearUpdateActualLevel(DeviceDatabaseType_t databaseType, int shortAddress, int actualLevel) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;
	if (false == refDevices.find(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &pDevice)) {
		return false;
	}
	INVARIANT(NULL != pDevice);
	CAppDaliDeviceProperties_ControlGear* const pDest = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return false;
	}
	pDest->setActualLevel(actualLevel);
	// Data wordt niet opgeslagen --> setPropertyChanged() wordt niet aangeroepen.
	return true;
}

bool CAppDali::fsmDbaseControlInputDeviceInstancesCfgUpdate(DeviceDatabaseType_t databaseType, int shortAddress,const CAppDaliInputControlDeviceInstances& properties) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	CAppDaliDevice* pDevice = NULL;
	if (false == refDevices.find(DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,&pDevice)) {
		return false;
	}
	INVARIANT(NULL != pDevice);
	CAppDaliDeviceProperties_InputControlDevice* const pDest = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return false;
	}
	CAppDaliInputControlDeviceInstances::copy(&pDest->getInstances(), properties);
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);		// Mag opgeslagen worden...
	return true;
}

bool CAppDali::fsmDbaseControlGearIsDT8ColorObjectSupported(DeviceDatabaseType_t databaseType,int shortAddress) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());	
	CAppDaliDevice* pDevice = NULL;
	bool requestDT8ColorInfo = false;
	if (refDevices.find(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &pDevice)) {	
		if (CAppDaliDevice_Utils::isControlGearDeviceTypeDT8(*pDevice)) {
			requestDT8ColorInfo = true;
		}
	}
	return requestDT8ColorInfo;
}

bool CAppDali::fsmDbaseControlGearDT8SetSceneColorObjectInfo(DeviceDatabaseType_t databaseType, int shortAddress, int sceneNumber, const CAppDaliObjectDT8ColorData& refData) {
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());	
	CAppDaliDevice* pDevice = NULL;
	if (!refDevices.find(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &pDevice)) {	
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* const pDest = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return false;
	}
	const bool result = pDest->setDT8SceneColorInfo(sceneNumber, refData);
	if (result) {
		this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);	// Vragen om data op te slaan.
	}
	return result;
}

bool CAppDali::fsmDbaseControlGearSetBroadcastSceneInfo(DeviceDatabaseType_t databaseType, int sceneNumber, const CAppDaliDeviceProperties_ControlGear& r) { 
	if ((sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) || (sceneNumber > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST)) {
		REQUIRE((sceneNumber >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) && (sceneNumber <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST));
		return false;
	}
	CAppDaliDevices& refDevices = (databaseType == DEVICE_DATABASE_NETWORK ? this->getNwkDevices() : this->getDevices());
	const int sceneLevelValue = r.getSceneLevel(sceneNumber);
	const CAppDaliObjectDT8ColorData& refColorData = r.getDT8SceneColorInfo(sceneNumber);
	const bool result =  CAppDaliDevices_Utils::setAllDevicesSceneInfo(&refDevices, sceneNumber, sceneLevelValue, refColorData);
	if (result) {
		this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);		// Mag opgeslagen worden...
	}
	return result;
}

////////////////////////////////////////////////////
// TODO_DALI_FSM_ALGORITMES
void CAppDali::notifySetProgessInfo(int shortAddress, const char* strInfo) {
	if (NULL == strInfo) {
		return;
	}
	CString strMessage;
	if (shortAddress < 0 || shortAddress>=64) {		
		strMessage = CString(strInfo);									// broadcast.
	}
	else {					
		strMessage.Format("DALI device %d: %s",shortAddress, strInfo);	// device	
	}
	if (NULL != this->m_pObserver) {
		this->m_pObserver->onNotifyGuiMsgCommunication(this->m_bNodeAddress,IAppDlgDaliGui_Host::TYPE_MSG_INFO, strMessage);
	}
	else {
		TRACE("this->m_pObserver == NULL");
	}
}

bool CAppDali::setControlGearDevicePropertiesAddToGroup(CAppDaliDevice& refDevice,int groupAddress) {
	bool result = CAppDaliDevice_Utils::addDeviceToGroup(refDevice,groupAddress);
	if (false == result) {
		return false;
	}
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);	// Vragen om data op te slaan.
	return true;
}

bool CAppDali::setControlGearDevicePropertiesRemoveFromGroup(CAppDaliDevice& refDevice,int groupAddress) {
	bool result = CAppDaliDevice_Utils::removeDeviceFromGroup(refDevice,groupAddress);
	if (false == result) {
		return false;
	}
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);	// Vragen om data op te slaan.
	return true;
}

bool CAppDali::setControlGearDevicePropertiesSetSceneInfo(CAppDaliDevice& refDevice,int sceneAddress,int data) {
	bool result = CAppDaliDevice_Utils::setDeviceSceneInfo(refDevice,sceneAddress,data);
	if (false == result) {
		return false;
	}
	this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_PROPERTY);	// Vragen om data op te slaan.
	return true;
}

// Helper functie...
bool CAppDali::helper_setControlGearProperty(CAppDaliDevices& refDevices, int shortAddress, int value, void (CAppDaliDeviceProperties_ControlGear::*pf)(int)) {
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (!this->getDeviceProperties(refDevices,shortAddress, &properties)) {
		return false;
	}
	(properties->*pf)(value);
	return true;
}


// Control input device property via een functie pointer.
bool CAppDali::helper_setControlDeviceProperty(CAppDaliDevices& refDevices, int shortAddress, int value, void (CAppDaliDeviceProperties_InputControlDevice::*pf)(int)) {
	CAppDaliDeviceProperties_InputControlDevice* properties = NULL;	
	if (!this->getDeviceProperties(refDevices, shortAddress, &properties)) {
		return false;
	}
	(properties->*pf)(value);
	return true;
}
bool CAppDali::helper_setControlDeviceProperty(CAppDaliDevices& refDevices, int shortAddress, bool value, void (CAppDaliDeviceProperties_InputControlDevice::*pf)(bool)) {
	CAppDaliDeviceProperties_InputControlDevice* properties = NULL;
	if (!this->getDeviceProperties(refDevices, shortAddress, &properties)) {
		return false;
	}
	(properties->*pf)(value);
	return true;
}

bool CAppDali::helper_setControlDeviceProperty(CAppDaliDevices& refDevices, int shortAddress, uint32_t value, void (CAppDaliDeviceProperties_InputControlDevice::*pf)(uint32_t)) {
	CAppDaliDeviceProperties_InputControlDevice* properties = NULL;
	if (!this->getDeviceProperties(refDevices, shortAddress, &properties)) {
		return false;
	}
	(properties->*pf)(value);
	return true;
}

// Control input device instance property via een functie pointer.
bool CAppDali::helper_setControlDeviceInstanceProperty(CAppDaliDevices& refDevices, int shortAddress, int instance, int value, void (CAppDaliInputControlDeviceInstance::*pf)(int)) {
	CAppDaliInputControlDeviceInstance* properties = NULL;
	if (!this->getDeviceInstanceProperties(refDevices, shortAddress, instance, &properties,true)) {
		return false;
	}
	(properties->*pf)(value);
	return true;
}

bool CAppDali::helper_setControlDeviceInstanceProperty(CAppDaliDevices& refDevices, int shortAddress, int instance, bool value, void (CAppDaliInputControlDeviceInstance::*pf)(bool)) {
	CAppDaliInputControlDeviceInstance* properties = NULL;
	if (!this->getDeviceInstanceProperties(refDevices, shortAddress, instance, &properties,true)) {
		return false;
	}
	(properties->*pf)(value);
	return true;
}

// Zoeken naar verschillen in de databases.
bool CAppDali::databaseDetectChanges() {
	CAppDaliDevices& refNetworkDevices = this->getNwkDevices();
	CAppDaliDevices& refDatabaseDevices = this->getDevices();

	// Aantal verschillend...
	if (refNetworkDevices.getCount() != refDatabaseDevices.getCount()) {
		this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_DATABASE);
		return true;
	}

	// Zoeken in de databases naar verschillende devices.
	const int count = refNetworkDevices.getCount();
	for (int i = 0; i < count; ++i) {
		CAppDaliDevice* pDevice = NULL;
		if (refNetworkDevices.get(i, &pDevice)) {
			ASSERT(NULL != pDevice);
			CAppDaliDevice* p = NULL;
			if (false == refDatabaseDevices.find(pDevice->getType(), pDevice->getShortAddress(), &p)) {
				// Device is niet aanwezig in de oude database - Nieuw device.
				this->setPropertyChanged(PROPERTY_CHANGED_DEVICE_DATABASE);
				return true;;
			}
		}
	}
	return false;
}


// Zoeken naar fouten in de databases.
// Stap1 : Controle zijn er devices aanwezig die in error toestand staan.
// Stap2 : Werden alle device in de offline database gevonden in de netwerk database.
bool CAppDali::databaseDectectErrors( ) {

	bool detectErrors = false;

	// Wissen openstaande fouten.
	this->m_flags &= ~FLAG_DATABASE_PENDING_ERRORS;	

	CAppDaliDevices& refNetworkDevices = this->getNwkDevices();

	{
		// Stap1 : Controle zijn er devices aanwezig die in error toestand staan.
		const int count = refNetworkDevices.getCount();
		for (int i=0;i<count;++i) {
			CAppDaliDevice* pDevice = NULL;
			if (refNetworkDevices.get(i,&pDevice)) {
				if (ERROR_STATUS_NONE != pDevice->getErrorStatus()) {					
					detectErrors =  true;	// fout: Er staat een device in error status.
				}
			}
		}
	}

	{
		// Stap2 : Werden alle device in de offline database gevonden in de netwerk database.
		const int count = this->m_pDaliDevices->getCount();
		for (int i=0; i<count;++i) {
			CAppDaliDevice* pDevice = NULL;
			if (this->m_pDaliDevices->get(i,&pDevice)) {
				ASSERT(NULL != pDevice);
				CAppDaliDevice* pNewDevice = NULL;
				const int shortAddress = pDevice->getShortAddress();
				const DaliDeviceType_t type = pDevice->getType();
				if (!refNetworkDevices.find(type,shortAddress,&pNewDevice)) {
					// Foutmelding entry.
					const CString strEmpty;
					const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_DATABASE_DEVICE_NOT_FOUND,type,shortAddress,strEmpty);
					CAppDaliFsmErrorEntries::addErrorEntry(this->m_pPendingErrors,errorEntry);
					
					// fout er werd een device niet teruggevonden.
					detectErrors =  true;	
				}
			}
		}	
	}

	if (detectErrors) {
		this->m_flags |= FLAG_DATABASE_PENDING_ERRORS;
	}
	return detectErrors;
}

bool CAppDali::databaseCheckErrorState() {
	bool errorState = false;
	const int count = this->m_pDaliDevices->getCount();

	this->m_flags &= ~FLAG_DATABASE_PENDING_ERRORS;

	for (int i = 0; i<count; ++i) {
		CAppDaliDevice* pDevice = NULL;
		if (this->m_pDaliDevices->get(i, &pDevice)) {
			if (ERROR_STATUS_NONE != pDevice->getErrorStatus()) {
				errorState = true;	// fout: Er staat een device in error status.
			}
		}
	}	
	if (errorState) {
		this->m_flags |= FLAG_DATABASE_PENDING_ERRORS;
	}
	return errorState;
}


// Taken:
//  *) Nieuwe database aanmaken mbv de gevonden devices.
//	*) Mergen van de namen van de document database met de nieuwe database.
//  *) Verwijderen van de oude document database.
//  *) De nieuwe database gebruiken als document database.
bool CAppDali::copyDatabaseNetwork2DocumentDevices(bool mergeData) {	
	CAppDaliDevices* const pNewDevices = this->getNwkDevices().clone();
	if (NULL == pNewDevices) {
		ASSERT(0);					// Wat doen...
		return false;
	}	

	if (this->m_pDaliDevices != NULL) {
		if (true == mergeData) {			
			// De Database overlopen. 		
			// Controleren of alle devices aanwezig zijn...
			const int count = this->m_pDaliDevices->getCount();
			for (int i=0; i<count;++i) {
				CAppDaliDevice* pDevice = NULL;
				if (this->m_pDaliDevices->get(i,&pDevice)) {
					ASSERT(NULL != pDevice);
					CAppDaliDevice* pNewDevice = NULL;
					const int shortAddress = pDevice->getShortAddress();
					const DaliDeviceType_t type = pDevice->getType();
					if (!pNewDevices->find(type,shortAddress,&pNewDevice)) {
						// Indien een device niet gevonden werd in de network database (maar wel in de offline database.)

						// Foutmelding entry.
						const CString strEmpty;
						const CAppDaliFsmErrorEntry errorEntry(ERROR_CODE_DATABASE_DEVICE_NOT_FOUND,type,shortAddress,strEmpty);
						CAppDaliFsmErrorEntries::addErrorEntry(this->m_pPendingErrors,errorEntry);
											 
						// Device Clone van de offline database aanmaken en toevoegen aan de nieuwe database.
						// Het device staat in fout toestand (Device not found).
						pNewDevice = pDevice->clone();
						ASSERT(NULL != pNewDevice);
						if (NULL != pNewDevice) {							
							pNewDevice->setErrorStatus(ERROR_STATUS_PENDING);
							pNewDevice->setErrorCode(ERROR_CODE_DATABASE_DEVICE_NOT_FOUND);	
							pNewDevices->add(pNewDevice);
						}
					}					
				}												
			}

			// Copieren van de namen.
			CAppDaliDevices_Utils::mergeNames(pNewDevices,*this->m_pDaliDevices);
		}
		delete this->m_pDaliDevices;
		this->m_pDaliDevices = NULL;
	}
	ASSERT(NULL != pNewDevices);
	ASSERT(NULL == this->m_pDaliDevices);
	this->m_pDaliDevices = pNewDevices;
	return true;
}

// Na het inladen van de devices de error status van de devices nakijken.
// Indien er devices in error status staan dan moeten deze in de lijst van pending errors komen...
void CAppDali::checkDatabaseDevicesErrorStatus( ) {

	if (NULL == this->m_pDaliDevices) {
		INVARIANT(NULL != this->m_pDaliDevices);
		return;
	}

	bool detectErrors = false;

	// Alle fouten wissen.	
	this->clearPendingErrors( );

	const int count = this->m_pDaliDevices->getCount();
	for (int i=0;i<count;++i) {
		CAppDaliDevice* pDevice = NULL;
		if (this->m_pDaliDevices->get(i,&pDevice)) {
			ASSERT(NULL != pDevice);		
			if (ERROR_STATUS_NONE != pDevice->getErrorStatus()) {
				this->addPendingError(pDevice);
				detectErrors = true;
			}
		}
	}

	// Globale error vlag zetten.
	if (detectErrors) {
		this->m_flags |= FLAG_DATABASE_PENDING_ERRORS;
	}
}

// Duotecno DALi Bridge configuratie.
void CAppDali::setChangesCfgDuotecnoBridge(bool flag) {
	if (flag) {
		this->m_flags |= FLAG_CHANGES_CFG_DUOTECNOBRIDGE_NEEDED;
	}
	else {
		this->m_flags &= ~FLAG_CHANGES_CFG_DUOTECNOBRIDGE_NEEDED;
	}
}

bool CAppDali::getChangesCfgDuotecnoBridgeNeeded() {
	return (FLAG_CHANGES_CFG_DUOTECNOBRIDGE_NEEDED == (this->m_flags & FLAG_CHANGES_CFG_DUOTECNOBRIDGE_NEEDED));
}

bool CAppDali::getStatePendingErrors( ) {
	return (FLAG_DATABASE_PENDING_ERRORS == (this->m_flags & FLAG_DATABASE_PENDING_ERRORS));
}

bool CAppDali::saveDatabaseDevices(bool saveOnlyWhenChanges) {
	if (true == saveOnlyWhenChanges) {
		if (!(this->m_flags & FLAG_DATABASE_REQUEST_SAVE)) {
			return false;
		}
	}
	const CString fileName = m_parent->getFullyQualifiedName(this->m_bNodeAddress);
	if (fileName.IsEmpty()) {
		return false;
	}		
	CAppDaliJson jsonFormatter;
	if (!jsonFormatter.save(fileName,this->getDevices(),this->getNamesGroups(),this->getNamesScenes())) {
		return false;
	}
	m_parent->notifyDataSaved();

	this->m_flags &= ~FLAG_DATABASE_REQUEST_SAVE;
	return true;
}

bool CAppDali::loadDatabaseDevices(int nodeAddress) {
	const CString fileName = m_parent->getFullyQualifiedName(nodeAddress);
	if (fileName.IsEmpty()) {
		return false;
	}		
	CAppDaliJson jsonFormatter;
	if (!jsonFormatter.load(fileName,&this->getDevices(),&this->getNamesGroups(),&this->getNamesScenes())) {
		return false;
	}
	this->m_flags &= ~FLAG_DATABASE_REQUEST_SAVE;


#if(DEBUG_INIT_TESTMODEL == 1)
	this->clearPendingErrors( );
	this->m_pPendingErrors = createTestModel();
#endif

	return true;
}

CAppDaliFsmErrorEntries* CAppDali::getPendingErrors( ) {
	return this->m_pPendingErrors;
}

/**************************************** Protected ********************************/
void CAppDali::clearPendingErrors( ) {
	this->m_flags &= ~FLAG_DATABASE_PENDING_ERRORS;
	safeDelete(m_pPendingErrors);
}

bool CAppDali::addPendingError(CAppDaliDevice* pDevice) {
	if (NULL == pDevice) {
		REQUIRE(NULL != pDevice);
		return false;
	}

	if (NULL == this->m_pPendingErrors) {
		this->m_pPendingErrors = new CAppDaliFsmErrorEntries( );
	}

	ASSERT(this->m_pPendingErrors != NULL);

	const CString strEmpty;
	CAppDaliFsmErrorEntry entryError(pDevice->getErrorCode(), pDevice->getType(), pDevice->getShortAddress(), strEmpty);
	entryError.setSerialNumber(pDevice->getSerialcode());
	entryError.setDeviceName(pDevice->getName());
	return CAppDaliFsmErrorEntries::addErrorEntry(this->m_pPendingErrors,entryError);
}

bool CAppDali::getDeviceProperties(CAppDaliDevices& refDevices, int shortAddress, CAppDaliDeviceProperties_ControlGear** ppProperties) {
	CAppDaliDevice* pDevice = NULL;
	if (!refDevices.find(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &pDevice)) {
		return false;
	}	
	CAppDaliDeviceProperties_ControlGear* const p = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
	if (NULL == p) {
		return false;
	}
	*ppProperties = p;
	return true;
}

bool CAppDali::getDeviceProperties(CAppDaliDevices& refDevices, int shortAddress, CAppDaliDeviceProperties_InputControlDevice** ppProperties) {
	CAppDaliDevice* pDevice = NULL;
	if (!refDevices.find(DALI_DEVICE_TYPE_INPUTCONTROL, shortAddress, &pDevice)) {
		return false;
	}
	CAppDaliDeviceProperties_InputControlDevice* const p = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
	if (NULL == p) {
		return false;
	}
	*ppProperties = p;
	return true;
}

bool CAppDali::getDeviceInstanceProperties(CAppDaliDevices& refDevices, int shortAddress, int instance, CAppDaliInputControlDeviceInstance** ppProperties,bool create) {
	CAppDaliDeviceProperties_InputControlDevice* pInputControlDevice = NULL;
	if (!this->getDeviceProperties(refDevices, shortAddress, &pInputControlDevice)) {
		return false;
	}
	ASSERT(NULL != pInputControlDevice);
	if (pInputControlDevice->getInstances().find(instance, ppProperties)) {
		return true;
	}

	if (!create) {
		return false;
	}		
	CAppDaliInputControlDeviceInstance* pNew = new CAppDaliInputControlDeviceInstance(instance);
	if (NULL == pNew) {
		return false;
	}	
	if (!pInputControlDevice->getInstances().add(instance,pNew)) {
		delete pNew;
		return false;
	}
	*ppProperties = pNew;
	return true;
}

void CAppDali::setPropertyChanged(PropertyChangedType_t type) {
	this->m_flags |= FLAG_DATABASE_REQUEST_SAVE + FLAG_DATABASE_PROPERTY_CHANGED;

	if ((PROPERTY_CHANGED_DEVICE_PROPERTY == type) || (PROPERTY_CHANGED_DEVICE_DATABASE == type)) {
		this->m_flags |= FLAG_CHANGES_CFG_DUOTECNOBRIDGE_NEEDED;
	}
}
/***********************************************************************************/	 

/***********************************************************************************/	 
/***********************************************************************************/	 
/***********************************************************************************/	 
const char* CAppDali_Utils::getStrResponseCode(BYTE type) {
	switch(type) {
		case DALI_RESULT_TX_OK:						
			return "DALI_RESULT_TX_OK";
		case DALI_RESULT_RESPONSE_BACKWARD_FRAME:
			return "DALI_RESULT_RESPONSE_BACKWARD_FRAME";
		case DALI_RESULT_EC_RESPONSE_TIMEOUT:
			return "DALI_RESULT_EC_RESPONSE_TIMEOUT";
		case DALI_RESULT_EC_RESPONSE_INVALID:
			return "DALI_RESULT_EC_RESPONSE_INVALID";
		case DALI_RESULT_EC_RESPONSE_TOOEARLY:
			return "DALI_RESULT_EC_RESPONSE_TOOEARLY";
		case DALI_RESULT_EC_GENERAL:
			return "DALI_RESULT_EC_GENERAL";
		case DALI_RESULT_TX_EC_WRONGSTATE:
			return "DALI_RESULT_TX_EC_WRONGSTATE";
		case DALI_RESULT_TX_EC_BUS_SYSTEMFAILURE:
			return "DALI_RESULT_TX_EC_BUS_SYSTEMFAILURE";
		case DALI_RESULT_NONE:
			return "DALI_RESULT_NONE";
	}
	return "Undefined";
}

/***********************************************************************************/
}; /* namespace DUOTECNO */
}; /* namespace DALI */
/***********************************************************************************/