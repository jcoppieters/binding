#include "stdafx.h"
#include "BindingSoftware.h"
#include "appdlgdaliworker.h"
#include "appdlgtasks_base.h"
#include "app/AppDali/appdali.h"
#include "app/AppDali/appdalitransmitter.h"
#include "app/appdali/appdalimanager.h"
#include "app/appdali/appdalidevices.h"
#include "app/appdali/appdalimsg_utils.h"
#include "app/AppDali/appdali_def.h"
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"				// sedert V16.36 bepalen van de sbox ultra

#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
using namespace TMVaria;
/*****************************************************************/
using namespace DUOTECNO::DALI; 
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
IAppDlgDaliGui_Host::~IAppDlgDaliGui_Host() { }

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CAppDlgDaliWorker::CAppDlgDaliWorker( ) { }
CAppDlgDaliWorker::~CAppDlgDaliWorker( ) { }


// Initialisatie van de manager.
// Indien er al instanties aanwezig zijn, niks doen + return TRUE;
// Indien er nog geen instanties aanwezig zijn voor iedere Smartbox ultra een implementatie aanmaken.
BOOL CAppDlgDaliWorker::initDaliManager(DUOTECNO::DALI::CAppDaliManager& refAppDaliManager) {

	int count = refAppDaliManager.getCount();
	if (count > 0) {	
		return TRUE;		
	}

	int counter = 0;
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		if ((NULL != pNode) && (PRODUCT_ID_HC_BOX_ULTRA == pNode->GetProductID())) {				
			ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
			ASSERT(NULL != pSocketHomeClient);
			if (NULL != pSocketHomeClient) {
				CAppDali* pNewInstance = NULL;				
				if (refAppDaliManager.create(this, pNode->getNodeAddress(), pSocketHomeClient,&pNewInstance)) {
					ASSERT(NULL != pNewInstance);
					if (pNewInstance) {					
						if (pNewInstance->loadDatabaseDevices(pNode->getNodeAddress())) {
							pNewInstance->checkDatabaseDevicesErrorStatus();
						}
						counter++;
					}
				}
			}
		}
	}	
	ENSURE(refAppDaliManager.getCount() == counter);
	return (counter>0 ? TRUE : FALSE);
}


// Opvragen van de lijst van nodes voor wie er een implementatie is.
// Krijgen een lijst van de node addressen waarvoor er een DALI bus implementatie is.
// return waarde: TRUE lijst aanwezig, FALSE lijst bevat geen items.
BOOL CAppDlgDaliWorker::getListDaliNodes(CList<int,int>* pListNodeAddresses) {
	if (NULL == pListNodeAddresses) {
		REQUIRE(NULL != pListNodeAddresses);
		return FALSE;
	}
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		if (NULL != pNode) {
			CAppDali* pInstance = NULL;
			if (this->getAppDaliManager().find(pNode->getNodeAddress(),&pInstance)) {
				if (NULL != pInstance) {
					pListNodeAddresses->AddTail(pNode->getNodeAddress());		
				}	
			}
		}
	}
	return (pListNodeAddresses->GetCount() > 0 ? TRUE : FALSE);
}


// Moeten voor alle AppDali implementaties de GUI subscriben.
BOOL CAppDlgDaliWorker::initOnOpenDialog(DUOTECNO::DALI::IAppDlgDaliGui_Host* pObserver) {
	BOOL result = FALSE;
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		if (NULL != pNode) {
			CAppDali* pInstance = NULL;
			if (this->getAppDaliManager().find(pNode->getNodeAddress(),&pInstance)) {
				if (NULL != pInstance) {
					pInstance->subscribeGUI(pObserver);
					result = TRUE;
				}
			}
		}
	}
	return result;
}

// Huidige taak afsluiten.
// Alle instanties unsubscribe GUI.
BOOL CAppDlgDaliWorker::closeDialog( ) {

	BOOL result = FALSE;
	DUOTECNO::DALI::CAppDali* const pCurrent = this->getCurrent( );
	if (NULL != pCurrent) {
		pCurrent->cancelTask();
	}
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		if (NULL != pNode) {
			CAppDali* pInstance = NULL;
			if (this->getAppDaliManager().find(pNode->getNodeAddress(),&pInstance)) {
				if (NULL != pInstance) {
					pInstance->subscribeGUI(NULL);
					result = TRUE;
				}
			}
		}
	}
	return result;
}

BOOL CAppDlgDaliWorker::pollByGuiSaveChanges( ) {
	DUOTECNO::DALI::CAppDali* const pCurrent = this->getCurrent( );
	if (NULL == pCurrent) {
		return FALSE;
	}
	static const bool SAVE_ONLY_WHEN_CHANGES_YES = true;
	if (pCurrent->saveDatabaseDevices(SAVE_ONLY_WHEN_CHANGES_YES)) {
		return TRUE;
	}
	return FALSE;
}

CAppDaliDevices* CAppDlgDaliWorker::getDaliDevices( ) {
	DUOTECNO::DALI::CAppDali* const pCurrent = this->getCurrent( );
	if (NULL == pCurrent) {
		return NULL;
	}
	CAppDaliDevices* const pDevices = &pCurrent->getDevices();
	ENSURE(NULL != pDevices);
	return pDevices;
}


// Evt onderstaande naar de CMainFrame brengen via een sendMessage...
// Nodig voor het tonen van de tcp/ip comm settings.
// Zelfde code als HomeClient master
void CAppDlgDaliWorker::showCfgNodeServices( ) {	
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return;
	}

	// Oppassen:
	// Deze mag enkel synschroon verstuurd worden want geeft een ptr naar CCanNode mee.
	AfxGetMainWnd()->SendMessage(WM_APPLICATION_NODE_SHOW_SERVICES, (WPARAM) pNode);
}

bool CAppDlgDaliWorker::disconnect( ) {
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return false;
	}
	return CAppDlgTasksBaseHomeClientUser::disconnectHomeClientSocket(pNode);
}

bool CAppDlgDaliWorker::isConnected() {
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return false;
	}
	return (CAppDlgTasksBaseHomeClientUser::getHomeClientSocketConnected(pNode));
}

bool CAppDlgDaliWorker::connectLocalPossible( ) {
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return false;
	}
	return CAppDlgTasksBaseHomeClientUser::getHomeClientConnectPossible(pNode, CONNECTION_METHOD_LOCAL);		
}

bool CAppDlgDaliWorker::connectRemotePossible( ){
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return false;
	}
	return CAppDlgTasksBaseHomeClientUser::getHomeClientConnectPossible(pNode, CONNECTION_METHOD_REMOTE_DIRECT);
}

bool CAppDlgDaliWorker::connectTCPHomeClient(ConnectionMethod_t method) {
	CCanNode* const pNode = this->getCurrentNode();
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return false;
	}
	return CAppDlgTasksBaseHomeClientUser::connectHomeClient(pNode, method);
}

HomeClientConnectionState_t CAppDlgDaliWorker::getTypeConnected( ) {
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return HOMECLIENT_STATE_DISCONNECTED;
	}
	return CAppDlgTasksBaseHomeClientUser::getHomeClientConnectionState(pNode);
}
/**************************** protected **************************/
/*
bool CAppDlgDaliWorker::connect(const char* hostName, int port, const char* pwd) {
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return false;
	}
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}
	return pSocketHomeClient->connect(hostName,port,pwd);
}
*/

DUOTECNO::DALI::CAppDali* CAppDlgDaliWorker::getCurrent( ) {
	const int current = this->getCurrentNodeAddress( );
	if (0xff == current) {
		return NULL;
	}
	DUOTECNO::DALI::CAppDali* pInstance = NULL;
	if (FALSE == this->getAppDaliManager().find(current,&pInstance)) {
		return NULL;
	}
	ENSURE(NULL != pInstance);
	return pInstance;
}

BYTE CAppDlgDaliWorker::getCurrentNodeAddress( ) { 
	int currentSelected = this->getAppDaliManager().getCurrentSelectedNode();
	if (currentSelected < 0) {
		return 0xff;
	}
	ENSURE(currentSelected >= 0 && currentSelected<=0xff);
	return (BYTE) currentSelected;
}

BOOL CAppDlgDaliWorker::setCurrentNodeAddress(int nodeAddress) {
	REQUIRE(nodeAddress>=0 && nodeAddress<=0xFF);
	return this->getAppDaliManager().setCurrentSelectedNode(nodeAddress);
}

CCanNode* CAppDlgDaliWorker::getCurrentNode( ) {
	const BYTE current = this->getCurrentNodeAddress( );
	if (0xFF == current) {
		ASSERT(0xFF != current);
		return NULL;
	}
	int dummy=0;
	CCanNode* pNode = NULL;
	if (FALSE == TMGetDocumentNodes()->FindNode(current,&dummy,&pNode)) {
		ASSERT(0);
		return NULL;
	}
	ENSURE(NULL != pNode);
	return pNode;
}

// Implementatie van de generieke helperfunctie voor geen parameters
template <typename Func>
bool CAppDlgDaliWorker::startDaliAction(Func function) {
    if (!this->isConnected()) {
        ASSERT(0);
        return false;
    }
    CAppDali* const pImpl = this->getCurrent();
    if (NULL == pImpl) {
        ASSERT(NULL != pImpl);
        return false;
    }
    return (pImpl->*function)();
}

// Implementatie van de overloaded helperfunctie voor twee parameters
template <typename Func, typename Arg1>
bool CAppDlgDaliWorker::startDaliAction(Func function, Arg1 arg1) {
    if (!this->isConnected()) {
        ASSERT(0);
        return false;
    }
    CAppDali* const pImpl = this->getCurrent();
    if (NULL == pImpl) {
        ASSERT(NULL != pImpl);
        return false;
    }
    return (pImpl->*function)(arg1);
}

// Implementatie van de overloaded helperfunctie voor drie parameters
template <typename Func, typename Arg1, typename Arg2,typename Arg3>
bool CAppDlgDaliWorker::startDaliAction(Func function, Arg1 arg1, Arg2 arg2,Arg3 arg3) {
    if (!this->isConnected()) {
        ASSERT(0);
        return false;
    }

    CAppDali* const pImpl = this->getCurrent();
    if (NULL == pImpl) {
        ASSERT(NULL != pImpl);
        return false;
    }
    return (pImpl->*function)(arg1, arg2, arg3);
}

// Generieke helperfunctie voor het starten van een uploadoperatie
template <typename Func, typename Properties>
bool CAppDlgDaliWorker::startUploadDeviceProperties(int shortAddress, Properties& refProperties, Func function) {
    if (!this->isConnected()) {
        ASSERT(0);
        return false;
    }    
    CAppDali* const pImpl = this->getCurrent();
    if (NULL == pImpl) {
        ASSERT(NULL != pImpl);
        return false;
    }    
    return (pImpl->*function)(shortAddress, refProperties);
}

// Generieke helperfunctie voor het starten van een leesoperatie
template <typename Func>
bool CAppDlgDaliWorker::startReadControlGearDevice(int shortAddress, Func function) {
    if (!this->isConnected()) {
        ASSERT(0);
        return false;
    }    
    CAppDali* const pImpl = this->getCurrent();
    if (NULL == pImpl) {
        ASSERT(NULL != pImpl);
        return false;
    }
    
    return (pImpl->*function)(shortAddress);
}

bool CAppDlgDaliWorker::startSearchDevices() {
	return startDaliAction(&CAppDali::startSearchDevices);
}
bool CAppDlgDaliWorker::startAddressing(DUOTECNO::DALI::AddressingInstallation_t type) {
    return startDaliAction(&CAppDali::startAddressing, type);
}
bool CAppDlgDaliWorker::startAddressingSingleDevice(DUOTECNO::DALI::DaliDeviceType_t deviceType, int shortAddress,int newShortAddress) {	
	return startDaliAction(&CAppDali::startAddressingSingleDevice,deviceType,shortAddress,newShortAddress);
}
bool CAppDlgDaliWorker::startAddressingSwapDevices(DUOTECNO::DALI::DaliDeviceType_t deviceType, int address0,int address1) {
	return startDaliAction(&CAppDali::startAddressingSwapDevices,deviceType,address0,address1);
}
bool CAppDlgDaliWorker::startEraseDeviceAddress(DUOTECNO::DALI::DaliDeviceType_t deviceType, int shortAddress) {
	return this->startAddressingSingleDevice(deviceType,shortAddress,0xff);
}
bool CAppDlgDaliWorker::startUploadControlGearDimmerCurve(int shortAddress, int deviceType,int dimmerCurve) {
	return startDaliAction(&CAppDali::startUploadControlGearDimmerCurve,shortAddress,deviceType,dimmerCurve);
}
bool CAppDlgDaliWorker::startControlDeviceInstanceEnableOnOff(int shortAddress, int instanceNumber, bool stateOnOff) {
	return startDaliAction(&CAppDali::startUploadControlInputDeviceInstanceEnableOnOff,shortAddress,instanceNumber,stateOnOff);
}
bool CAppDlgDaliWorker::startUploadControlGearDeviceProperties(int shortAddress, CAppDaliDeviceProperties_ControlGear& refProperties) {
	return (startUploadDeviceProperties(shortAddress, refProperties, &CAppDali::startUploadControlGearDeviceProperties));
}
bool CAppDlgDaliWorker::startUploadControlInputDeviceInstances(int shortAddress, CAppDaliInputControlDeviceInstances& refProperties) {
	return (startUploadDeviceProperties(shortAddress, refProperties, &CAppDali::startUploadControlInputDeviceInstances));
}
bool CAppDlgDaliWorker::startUploadControlGearDeviceScenes(int shortAddress, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear& refProperties) {
	return (startUploadDeviceProperties(shortAddress, refProperties, &CAppDali::startUploadControlGearDeviceScenes));
}
bool CAppDlgDaliWorker::startUploadControlGearDeviceSingleScene(int shortAddress, int sceneNumber, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear& refProperties) {
	if (!this->isConnected()) {
        ASSERT(0);
        return false;
    }
    CAppDali* const pImpl = this->getCurrent();
    if (NULL == pImpl) {
        ASSERT(NULL != pImpl);
        return false;
    }
    return (pImpl->startUploadControlGearDeviceSingleScene)(shortAddress, sceneNumber, refProperties);

	// return startDaliAction(&CAppDali::startUploadControlGearDeviceSingleScene,shortAddress,sceneNumber,refProperties);
}
bool CAppDlgDaliWorker::startControlGearQueryActualLevel(int shortAddress) {
	return startReadControlGearDevice(shortAddress, &CAppDali::startControlGearQueryActualLevel);
}
bool CAppDlgDaliWorker::startReadControlGearDeviceProperties(int shortAddress) {
	return startReadControlGearDevice(shortAddress, &CAppDali::startReadControlGearDeviceProperties);
}
bool CAppDlgDaliWorker::startReadControlGearDeviceGroups(int shortAddress) {
	return startReadControlGearDevice(shortAddress, &CAppDali::startReadControlGearDeviceGroups);
}
bool CAppDlgDaliWorker::startReadControlGearDeviceScenes(int shortAddress) {
	return startReadControlGearDevice(shortAddress, &CAppDali::startReadControlGearDeviceScenes);
}
bool CAppDlgDaliWorker::cancelTask( ) {
	return startDaliAction(&CAppDali::cancelTask);
}

//////////////////////////////////////	
// Sedert V16.45
// DALI-2 Control Input Devices: Ozuno Universele Interface
// Een Device/instance herstellen naar het type 'push-button' instance.
//
// Snelle/Tijdelijke oplossing (We gaan hier niet speciaal een FSM voor ontwikkelen...)
//
// Let op: De instelling van het instance type is enkel mogelijk in de operatingmode=0x80 (Manufacture).
//		   Na het instellen opnieuw naar de normale operating mode gaan.
bool CAppDlgDaliWorker::sendControlInputDeviceRestoreInstanceType(int shortAddress,int instanceAddress) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	
	// Enter manunifacting mode (128)
	if (!pImpl->getTransmitter().sendControlInputDevicesBroadcastDTR0(128)) {
		return false;
	}
	if (!pImpl->getTransmitter().sendControlInputDevicesSetOperatingModeDTR0(shortAddress)) {
		return false;
	}
	
	// Restore instance type 'Push-button'
	const BYTE value = DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON; 
	if (!pImpl->getTransmitter().sendControlInputDevicesBroadcastDTR0(value)) {
		return false;
	}
	if (!pImpl->getTransmitter().sendControlInputDeviceInstanceType(shortAddress,instanceAddress)) {
		return false;
	}

	// Esit manunifacting mode (128)
	if (!pImpl->getTransmitter().sendControlInputDevicesBroadcastDTR0(0)) {
		return false;
	}
	if (!pImpl->getTransmitter().sendControlInputDevicesSetOperatingModeDTR0(shortAddress)) {
		return false;
	}
	return true;
}

//////////////////////////////////////
// Status van de FSM taken.
bool CAppDlgDaliWorker::getDaliCfgTaskBusy( ) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	return (APPDALI_FSM_TASK_NONE != pImpl->getFsmCurrentTask());
}

Task_t CAppDlgDaliWorker::getDaliCfgCurrentTask( ) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return APPDALI_FSM_TASK_NONE;
	}
	return (pImpl->getFsmCurrentTask());
}

CAppDaliFsmErrorEntries* CAppDlgDaliWorker::getPendingErrors( ) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return NULL;
	}
	return pImpl->getPendingErrors();
}

// Helper functies voor de transmit.
bool CAppDlgDaliWorker::sendDeviceOff(ControlGearMsgType_t type, int address) {	
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearDeviceOff(address);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupOff(address);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearDeviceOff(0xFF);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceMaxLevel(ControlGearMsgType_t type, int address) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearRecallMaxLevel(address);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupRecallMaxLevel(address);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearRecallMaxLevel(0xFF);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceMinLevel(ControlGearMsgType_t type, int address) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearRecallMinLevel(address);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupRecallMinLevel(address);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearRecallMinLevel(0xFF);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceDimStepUp(ControlGearMsgType_t type, int address) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearDimStepUp(address);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupStepUp(address);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearDimStepUp(0xFF);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceDimStepDown(ControlGearMsgType_t type, int address) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearDimStepDown(address);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupStepDown(address);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearDimStepDown(0xFF);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceDimActualLevel(ControlGearMsgType_t type, int address,int level) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearDeviceActualLevel(address,level);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupActualLevel(address,level);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearDeviceActualLevel(0xff,level);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendGotoScene(ControlGearMsgType_t type, int address,int sceneNumber) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearGoToScene(address,sceneNumber);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			ASSERT(0);	// TODO ????
			result = false;
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearGoToScene(0xff,sceneNumber);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceDimUp(ControlGearMsgType_t type, int address) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearDimUp(address);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupDimUp(address);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearDimUp(0xFF);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceDimDown(ControlGearMsgType_t type, int address) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	bool result = false;
	switch(type) {
		case CONTROLGEAR_MSG_DEVICE: {
			result = pImpl->getTransmitter().sendControlGearDimDown(address);
		} break;
		case CONTROLGEAR_MSG_GROUP: {
			result = pImpl->getTransmitter().sendControlGearGroupDimDown(address);
		} break;
		case CONTROLGEAR_MSG_BROADCAST: {
			result = pImpl->getTransmitter().sendControlGearDimDown(0xFF);
		} break;	
	}
	return result;
}

bool CAppDlgDaliWorker::sendDeviceQueryStatus(int shortAddress) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	return pImpl->getTransmitter().sendQueryControlGearDeviceStatus(shortAddress);
}
bool CAppDlgDaliWorker::sendInputDeviceIdentify(int shortAddress) {
	CAppDali* const pImpl = this->getCurrent( );
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	return pImpl->getTransmitter().sendControlInputDeviceIdentify(shortAddress);
}


// Control gear groepen.
// Toevoegen/verwijderen uit een groep.
// Versturen op de bus + aanpassen in de database.
// Export van de Json database...
	// TODO: Mogen we een REFRESH versturen naar de configurator, (evt met params DALI).

bool CAppDlgDaliWorker::controlGearAddToGroup(int shortAddress, int groupAddress) {
	CAppDaliDevices* const pDevices = this->getDaliDevices( );
	if (NULL == pDevices) {
		return false;
	}
	CAppDaliDevice* pDevice = NULL;
	if (false == pDevices->find(DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,&pDevice)) {
		return false;
	}
	ASSERT(NULL != pDevice);
	return this->controlGearAddToGroup(*pDevice,groupAddress);
}

bool CAppDlgDaliWorker::controlGearAddToGroup(DUOTECNO::DALI::CAppDaliDevice& refDevice, int groupAddress) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	if ((groupAddress < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) || (groupAddress>DALI_SYSTEM_CONTROLGEAR_GROUP_LAST)) {
		ASSERT(0);
		return false;
	}
	const int shortAddress = refDevice.getShortAddress();
	if ((shortAddress < DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) || (shortAddress> DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST)) {
		ASSERT(0);
		return false;
	}
	if (false == pImpl->getTransmitter().sendControlGearAddtogroup(shortAddress, groupAddress)) {
		return false;
	}

	if (pImpl->setControlGearDevicePropertiesAddToGroup(refDevice,groupAddress)) {
		// Property veranderd + database werd opgeslagen...
	}	
	return true;
}

bool CAppDlgDaliWorker::controlGearRemoveFromGroup(DUOTECNO::DALI::CAppDaliDevice& refDevice, int groupAddress) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	if ((groupAddress < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) || (groupAddress>DALI_SYSTEM_CONTROLGEAR_GROUP_LAST)) {
		ASSERT(0);
		return false;
	}
	const int shortAddress = refDevice.getShortAddress();
	if ((shortAddress < DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) || (shortAddress> DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST)) {
		ASSERT(0);
		return false;
	}
	if (false == pImpl->getTransmitter().sendControlGearRemoveFromgroup(shortAddress, groupAddress)) {
		return false;
	}

	if (pImpl->setControlGearDevicePropertiesRemoveFromGroup(refDevice,groupAddress)) {
		// Property veranderd + database werd opgeslagen...
	}	
	return true;
}


// TRICKY?
// Er worden 2 commandos verstuurd.
// 1) DTR0 laden.
// 2) group commando om de property van DTR0 in te laden 
bool CAppDlgDaliWorker::sendControlGearCfgProperty(int groupAddress, TypeProperty_t type,int value) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	if ((groupAddress < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) || (groupAddress>DALI_SYSTEM_CONTROLGEAR_GROUP_LAST)) {
		ASSERT(0);
		return false;
	}
	if (value<0 || value>255) {
		REQUIRE(0);
		return false;
	}

	// Stap1: transmit -> DTR0 laden.
	if (false == pImpl->getTransmitter().sendControlGearDTR0(value)) {
		ASSERT(0);
		return false;
	}

	// stap2: transmit -> Property voor de groep.
	bool result = false;
	switch(type) {
		case PROPERTY_LEVEL_MIN: {
			result = pImpl->getTransmitter().sendControlGearGroupMinLevel(groupAddress);
		} break;
		case PROPERTY_LEVEL_MAX: {
			result = pImpl->getTransmitter().sendControlGearGroupMaxLevel(groupAddress);
		} break;
		case PROPERTY_LEVEL_POWERON: {
			result = pImpl->getTransmitter().sendControlGearGroupPowerOnLevel(groupAddress);
		} break;
		case PROPERTY_LEVEL_SYSTEMFAILURE: {
			result = pImpl->getTransmitter().sendControlGearGroupSystemFailureLevel(groupAddress);
		} break;
		case PROPERTY_FADE_TIME: {
			result = pImpl->getTransmitter().sendControlGearGroupFadeTime(groupAddress);
		} break;
		case PROPERTY_FADE_RATE: {
			result = pImpl->getTransmitter().sendControlGearGroupFadeRate(groupAddress);
		} break;			
		default: { ASSERT(0); } break;
	}		
	return result;
}

bool CAppDlgDaliWorker::controlGearGroupsAdjustPropertyMembers(int groupAddress, TypeProperty_t type,int value) {	
	if (groupAddress<DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || groupAddress>DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		REQUIRE(0);
		return false;
	}
	if (value<0 || value>255) {
		REQUIRE(0);
		return false;
	}
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	bool result=false;
	CAppDaliDevices& refDevices = pImpl->getDevices();
	for (int shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST; shortAddress<=DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST; ++shortAddress) {
		CAppDaliDevice* pDevice = NULL;
		if (refDevices.find(DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,&pDevice)) {
			ASSERT(NULL != pDevice);	
			if (CAppDaliDevice_Utils::isDeviceGroupMember(*pDevice,groupAddress)) {
				CAppDaliDeviceProperties_ControlGear* const properties = 
					dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
				ASSERT(NULL != properties);
				if (NULL != properties) {
					switch(type) {
						case PROPERTY_LEVEL_MIN: {
							properties->setLevelMin(value);
						} break;
						case PROPERTY_LEVEL_MAX: {
							properties->setLevelMax(value); 
						} break;
						case PROPERTY_LEVEL_POWERON: {
							properties->setLevelPowerOn(value);		 
						} break;
						case PROPERTY_LEVEL_SYSTEMFAILURE: {
							properties->setLevelSystemFailure(value);		 
						} break;
						case PROPERTY_FADE_TIME: {
							CAppDaliDeviceProperties_ControlGear::setFadeTime(properties,value);
						} break;
						case PROPERTY_FADE_RATE: {
							CAppDaliDeviceProperties_ControlGear::setFadeRate(properties,value);
						} break;			
						default: { ASSERT(0); } break;
					}
					result = true;
				}
			}
		}	
	}
	if (result) {
		pImpl->setPropertyChanged(CAppDali::PROPERTY_CHANGED_DEVICE_PROPERTY);		
	}
	return false;
}

bool CAppDlgDaliWorker::copyPropertiesFromFirstGroupMember(int groupAddress, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* pDestProperties, int* deviceAddress) {
	if (groupAddress<DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || groupAddress>DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		REQUIRE(0);
		return false;
	}
	if (NULL == pDestProperties || NULL == deviceAddress) {
		REQUIRE(0);
		return false;
	}
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	const int address = this->helper_getFirstDeviceInGroup(groupAddress, pDestProperties);
	if (address >= 0) {
		*deviceAddress = address;
		return true;
	}
	return false;
}

int CAppDlgDaliWorker::getFirstDeviceInGroup(int groupNR) {
	return this->helper_getFirstDeviceInGroup(groupNR, NULL);
}

// protected.
int CAppDlgDaliWorker::helper_getFirstDeviceInGroup(int groupAddress,DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* pDestProperties) {
	if (groupAddress<DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || groupAddress>DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		REQUIRE(0);
		return -1;
	}
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return -1;
	}
	
	CAppDaliDevices& refDevices = pImpl->getDevices();
	for (int shortAddress = DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST; shortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST; ++shortAddress) {
		CAppDaliDevice* pDevice = NULL;
		if (refDevices.find(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &pDevice)) {
			ASSERT(NULL != pDevice);
			if (CAppDaliDevice_Utils::isDeviceGroupMember(*pDevice, groupAddress)) {			
				if (NULL != pDestProperties) {
					CAppDaliDeviceProperties_ControlGear* const properties =
						dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
					ASSERT(NULL != properties);
					if (NULL != properties) {
						CAppDaliDeviceProperties_ControlGear::copy(pDestProperties, *properties);
					}
				}
				return shortAddress;	// Return waarde: short address van het eerste device.
			}
		}
	}
	return -1;
}

bool CAppDlgDaliWorker::controlDeviceSetApplicationControllerOnOff(int shortAddress, bool stateOnOff) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	if ((shortAddress<DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) || (shortAddress > DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST)) {
		ASSERT(0);
		return false;
	}

	bool result = false;
	CAppDaliTransmitter& refTransmitter = pImpl->getTransmitter();
	if (stateOnOff) {
		result = refTransmitter.sendControlInputDeviceApplicationControllerEnable(shortAddress);
	}
	else {
		result = refTransmitter.sendControlInputDeviceApplicationControllerDisable(shortAddress);
	}
	return result;
}

bool CAppDlgDaliWorker::controlGearRemoveFromGroup(int shortAddress, int groupAddress) {
	CAppDaliDevices* const pDevices = this->getDaliDevices( );
	if (NULL == pDevices) {
		return false;
	}
	CAppDaliDevice* pDevice = NULL;
	if (false == pDevices->find(DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,&pDevice)) {
		return false;
	}
	ASSERT(NULL != pDevice);
	return this->controlGearRemoveFromGroup(*pDevice,groupAddress);
}

// Openen van de Duotecno DALI Bridge configuration.
void CAppDlgDaliWorker::openDALIDuotecnoBridgeConfiguration( ) {
	const int nodeAddress = this->getCurrentNodeAddress();	
	// Oplossing BUG-0181: 
	// Indien er veranderingen zijn in de configuratie eerst de data opslaan voor de configurator geopend wordt.
	DUOTECNO::DALI::CAppDali* const pCurrent = this->getCurrent( );
	if (NULL != pCurrent) {
		ASSERT(pCurrent->getNodeAddress() == nodeAddress);
		// Opslaan bij veranderingen.
		static const bool SAVE_ONLY_WHEN_CHANGES_YES = true;
		pCurrent->saveDatabaseDevices(SAVE_ONLY_WHEN_CHANGES_YES);

		// Vlag wissen die een melding geeft in de GUI dat er veranderingen zijn.
		pCurrent->setChangesCfgDuotecnoBridge(false);
	}
	// Openen van de configurator.
	TMShowConfigurator(DUOTECNO::PLUGIN::VIEW_DALI, nodeAddress);
}

CAppDlgDaliWorker::StateDaliBridgeCfg_t CAppDlgDaliWorker::getDALIDuotecnoBridgeCfgState( ) {	
	CAppDaliDevices* const pDevices = this->getDaliDevices( );
	if (this->getDaliCfgTaskBusy()) {
		const Task_t task = this->getDaliCfgCurrentTask();
		if ((APPDALI_FSM_TASK_SEARCH_DEVICES == task) || (APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) ||
			(APPDALI_FSM_TASK_COMMISSIONING_NEW == task)) {
			return STATE_DALIBRIDGE_CFG_BUSY;
		}
	}
	if (NULL == pDevices || pDevices->getCount() <= 0) {
		return STATE_DALIBRIDGE_CFG_DISABLED;
	}
	if (this->getStatePendingErrors()) {
		return STATE_DALIBRIDGE_CFG_PENDING_ERRORS;
	}
	if (this->getChangesCfgDuotecnoBridgeNeeded()) {
		return STATE_DALIBRIDGE_CFG_PENDING_CHANGES;
	}
	return STATE_DALIBRIDGE_CFG_ENABLED;
}

void CAppDlgDaliWorker::setChangesCfgDuotecnoBridge(bool flag) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return;
	}
	pImpl->setChangesCfgDuotecnoBridge(flag);
}

bool CAppDlgDaliWorker::getChangesCfgDuotecnoBridgeNeeded() {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	return pImpl->getChangesCfgDuotecnoBridgeNeeded();
}


bool CAppDlgDaliWorker::getStatePendingErrors( ) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	return pImpl->getStatePendingErrors();
}


// Namen van de groepen en sferen.
CString CAppDlgDaliWorker::getGroupName(int groupNumber)  {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return CString();
	}
	return pImpl->getNamesGroups().getName(groupNumber);
}
void CAppDlgDaliWorker::setGroupName(int groupNumber,const CString& str) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return;
	}
	pImpl->getNamesGroups().setName(groupNumber,str);
	pImpl->setPropertyChanged(CAppDali::PROPERTY_CHANGED_NAME);	// Vragen om data op te slaan + IPC melding naar de Configurator.
}

bool CAppDlgDaliWorker::getGroupsNames(CMap<int,int,CString,CString>* dest) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	return pImpl->getNamesGroups().copy(dest);
}

CString CAppDlgDaliWorker::getSceneName(int sceneNumber)  {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return CString();
	}
	return pImpl->getNamesScenes().getName(sceneNumber);
}
void CAppDlgDaliWorker::setSceneName(int sceneNumber,const CString& str) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return;
	}
	pImpl->getNamesScenes().setName(sceneNumber,str);
	pImpl->setPropertyChanged(CAppDali::PROPERTY_CHANGED_NAME);	// Vragen om data op te slaan + IPC melding naar de Configurator.
}

bool CAppDlgDaliWorker::getSceneNames(CMap<int,int,CString,CString>* dest) {
	CAppDali* const pImpl = this->getCurrent();
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	return pImpl->getNamesScenes().copy(dest);
}


// V16.48: zie CHANGES_DALI_TCPIP_COMMUNICATION_SETTINGS
// Bij het openen van de DALI configuratie controleren of er communicatie settings aanwezig zijn.
// Er wordt een pop-up getoond en de UDP configuratie wordt geopend.
// Wanneer de module niet gevonden wordt in het netwerk wordt er een melding getoond.
bool CAppDlgDaliWorker::checkDaliConnectionSettingsAndPromptUser( ) {

	// controle connectie mogelijk (ip-poort, password,...)
	if (this->connectLocalPossible() || this->connectRemotePossible()) {
		return true;
	}

	// Geen connectie mogelijk.
	CString strHeader("TCP/IP Communication settings missing.");
	CString strMessage("DALI-bus configuration can only be done via the TCP/IP connection. "
					   "Make sure the Smartbox ultra is connected to the Ethernet LAN network and configure "
					   "the TCP/IP settings.\r\n\r\n"
					   "Press ok to continue.");
	if (IDOK == AfxGetMainWnd()->MessageBox(strMessage, strHeader, MB_OK | MB_ICONEXCLAMATION)) {
		AfxGetMainWnd()->PostMessage(WM_SHOW_NODE_UDPLANCFG, (LPARAM)(this->getCurrentNodeAddress()), 0xFFFFFFFF);
	}
	return false;
}

/*****************************************************************/
// Utility functies...
bool CAppDlgDaliWorker::formatStringAddress(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,CString* pStr) {
	if (shortAddress < DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST || shortAddress > DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
		REQUIRE(0);
		return false;
	}

	CString str;
	switch(deviceType) {
		case DALI_DEVICE_TYPE_CONTROLGEAR: {
			str.Format("A%d",shortAddress);						   
		} break;

		case DALI_DEVICE_TYPE_INPUTCONTROL: {
			str.Format("A%2d²",shortAddress);										
		} break;

		default: {
			ASSERT(0);
		} return false;
	}
	ENSURE(!str.IsEmpty());
	*pStr = str;
	return true;
}

bool CAppDlgDaliWorker::formatStringProblem(DUOTECNO::DALI::DaliErrorCode_t errorCode,CString* pStr) {
	if (NULL == pStr) {
		REQUIRE(NULL != pStr);
		return false;
	}

	CString str;
	switch(errorCode) {
		case ERROR_CODE_NONE: {
			ASSERT(0);
			str = "No Error";			  
		} break;
		case ERROR_CODE_SHORTADDRESS_DOUBLE: {
			str = "Possible double short address.";									  
		} break;
		case ERROR_CODE_COMMISSIONING_SHORTADDRESS_VERIFY: {
			str = "Problem addressing device.";											   
		} break;
		case ERROR_CODE_PROPERTY_WRONGRESPONSE: {
			str = "Wrong response received.";											   
		} break;
		case ERROR_CODE_PROPERTY_NORESPONSE: {
			str = "No response.";											   
		} break;
		case ERROR_CODE_WRONG_EVENT: {
			str = "Wrong event received.";											   
		} break;
		case ERROR_CODE_COMMISSIONING_SHORTADDRESS_EXCEED_MAX: {
			str = "Addressing max addresses exceeded.";											   
		} break;
		case ERROR_CODE_DATABASE_DEVICE_NOT_FOUND: {
			str = "Device not found";								  
		} break;
		case ERROR_CODE_TIMER_EXPIRY: {
			str = "Timer expired. No response from the DALI system.";					  
		} break;
		default: {
			ASSERT(0);
			str = "General error";	 
		} break;
	}
	ENSURE(!str.IsEmpty());
	*pStr = str;
	return true;
}

int CAppDlgDaliWorker::getDeviceActualLevel(DUOTECNO::DALI::CAppDaliDevice& refDevice) {
	CAppDaliDeviceProperties_ControlGear* pProperties = NULL;
	if (false == CAppDaliDevice_Utils::getControlGearProperties(refDevice,&pProperties)) {
		return -1;
	}
	ASSERT(NULL != pProperties);
	return pProperties->getActualLevel();
}

bool CAppDlgDaliWorker::checkValidSettingForHomeAutomation(CAppDaliInputControlDeviceInstance& r) {
	if (false == r.getInstanceActive()) {
		return true;
	}
	if (r.getEventScheme() != DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE_NUMBER) {
		return false;
	}

	const int eventFilters = r.getEventFilter();
	const int instanceType = r.getInstanceType();
	switch(instanceType) {
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON: {					
			if (DALI_CONTROLDEVICE_PUSHBUTTON_DEFAULT_EVENTFILTER != (DALI_CONTROLDEVICE_PUSHBUTTON_DEFAULT_EVENTFILTER & eventFilters)) {
				return false;	
			}
		} break;

		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY: {
			// Geen event filters of enkel de Repeat is niet ok...
			if (0 == eventFilters) {
				return false;
			}
			if (DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_STILL_OCCUPIED_VACANT == (eventFilters & DALI_CONTROLDEVICE_DETECTOR_DEFAULT_EVENTFILTER)) {
				return false;
			}					
		} break;

		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT: {
			if (DALI_CONTROLDEVICE_ABSINPUT_DEFAULT_EVENTFILTER != (DALI_CONTROLDEVICE_ABSINPUT_DEFAULT_EVENTFILTER & eventFilters)) {
				return false;
			}
		} break;

		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_LIGHT_SENSOR: {
			// todo ...										
		} break;

		default: {
			ASSERT(0);		// Todo ???? 
		} break;
	}
	return true;
}

// Weergave van de params is nu de abs waarden.
// TODO:
//	- Level waarden volgens de DALI dimming curve.	
//  - Extendend fadetime & faderate.
BOOL CAppDlgDaliWorker::getStringFadeRate(CString* pstrValue, int value) {
	const char* strValues[16] = {
		"EXT",					// 0	
		"358 step/s",			// 1
		"253 step/s",			// 2
		"179 step/s",			// 3
		"127 step/s",			// 4
		"89 step/s",			// 5
		"63 step/s",			// 6
		"45 step/s",			// 7
		"32 step/s",			// 8
		"22 step/s",			// 9
		"16 step/s",			// 10
		"11 step/s",			// 11
		"8 step/s",				// 12
		"6 step/s",				// 13
		"4 step/s",				// 14
		"3 step/s"				// 15
	};

	if (value<0 || value>=16) {
		ASSERT(0);
		return FALSE;
	}	
	pstrValue->Format("[%d] %s",value,strValues[value]);
	return TRUE;
}


BOOL CAppDlgDaliWorker::getStringFadeTime(CString* pstrValue, int value) {
	const char* strValues[16] = {
		"No fade",				// 0
		"0,7 sec",				// 1		
		"1 sec",				// 2
		"1,4 sec",				// 3
		"2,0 sec",				// 4
		"2,8 sec",				// 5
		"4,0 sec",				// 6
		"5,7 sec",				// 7
		"8,0 sec",				// 8
		"11,3 sec",				// 9
		"16,0 sec",				// 10
		"22,6 sec",				// 11
		"32,0 sec",				// 12
		"45,3 sec",				// 13
		"64,0 sec",				// 14
		"90,5 sec"				// 15
	};

	if (value<0 || value>=16) {
		ASSERT(0);
		return FALSE;
	}	
	pstrValue->Format("[%d] %s",value,strValues[value]);
	return TRUE;
}


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

static CString getFileNameDali(int nodeAddress) {	
	CString str;
	str.Format("dalibus_%02X.json",nodeAddress);
	return str;
}

// IAppDaliHost_App
// Wordt opgeslagen onder de config map.
CString CAppDlgDaliWorker::getFullyQualifiedName(int nodeAddress) {
	ASSERT(nodeAddress >= 0x03 && nodeAddress <= 0xFC);
	CString szReturn;
	CString szSubDir;
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	szReturn.Format("%s%s\\%s",szPathName,szSubDir,getFileNameDali(nodeAddress));	
	return szReturn;
}

// Melding naar de Duotecno configurator dat het json bestand gewijzigd werd.
void CAppDlgDaliWorker::notifyDataSaved( ) {
	CWnd* const pWnd = AfxGetMainWnd();
	if (NULL == pWnd) {
		ASSERT(NULL != pWnd);
		return;
	}
	CCanNode* const pNode = this->getCurrentNode();	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return;
	}
	pWnd->PostMessage(WM_APPLICATION_CONFIGURATOR_DALI_RELOAD, pNode->getNodeAddress());


#if(0)
	if (NULL == this->m_pNode) {
		ASSERT(NULL != this->m_pNode);
		return;
	}

	static const BOOL bFailIfExists = FALSE;	
	CString strFileNameData;
	CString strFileNameConfig;
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	CString subStringData;
	subStringData.LoadString(IDS_FILENAME_SUBDIR_DATA);
	CString subStringConfig;
	subStringConfig.LoadString(IDS_FILENAME_SUBDIRCONFIG);

	strFileNameData.Format("%s%s\\%s",szPathName,subStringData,getFileNameDali(this->m_pNode));
	strFileNameConfig.Format("%s%s\\%s",szPathName,subStringConfig,getFileNameDali(this->m_pNode));

	// src: strFileNameData
	// dest: strFileNameConfig
	if (!CopyFile(strFileNameData,strFileNameConfig,bFailIfExists)) {	
		ASSERT(0);
	}
#endif
}

bool CAppDlgDaliWorker::convertEventMsgToString(BYTE lengthBits,uint32_t recievedFrame,int* pShortAddress,
												int* pInstanceNumber,int* pInstanceType,CString* pStrEventMsg,CString* pStrInstanceType) {
	if ((NULL == pStrEventMsg) || (NULL == pStrInstanceType) || (NULL == pShortAddress) || (NULL == pInstanceNumber) || (NULL == pInstanceType)) {
		REQUIRE(0);
		return false;
	}
	if (lengthBits != 24) {
		return false;
	}
	STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE msgParser(recievedFrame);
	if (!msgParser.isEvent()) {
		return false;
	}

	if (!msgParser.isType()) {
		*pShortAddress=-1;
		*pInstanceNumber = 1;
		*pStrEventMsg = "Wrong scheme";
		*pStrInstanceType = "?";
		*pInstanceType = -1;
		return true;
	}
	*pShortAddress= msgParser.getShortAddress();
	*pInstanceNumber = msgParser.getInstanceAddress();
	return this->getStrEventMsg(*pShortAddress,*pInstanceNumber,msgParser.getData(),pInstanceType, pStrEventMsg,pStrInstanceType);
}

void CAppDlgDaliWorker::getListControlGearAddress(CList<int>* pList) {
	this->getListDeviceAddresses(DALI_DEVICE_TYPE_CONTROLGEAR,pList);
}

void CAppDlgDaliWorker::getListDeviceAddresses(DUOTECNO::DALI::DaliDeviceType_t deviceType,CList<int>* pList) {
	if (NULL == pList) {
		REQUIRE(NULL != pList);
		return;
	}

	pList->RemoveAll();

	CAppDaliDevices* const pDevices = this->getDaliDevices( );
	if (NULL == pDevices) {
		return;
	}
	for (int shortAddress = 0;shortAddress <= 63; ++shortAddress) {
		CAppDaliDevice* pDevice = NULL;
		if (pDevices->find(deviceType,shortAddress,&pDevice)) {
			ASSERT(NULL != pDevice);
			pList->AddTail(shortAddress);
		}	
	}
}

BOOL CAppDlgDaliWorker::checkDeviceAndPromptAddToGroup(CWnd* parent, int groupAddress, int shortAddress) {
	CList<int> list;
	this->getListOfGroups(shortAddress, &list);
	if (list.IsEmpty()) {
		return TRUE;		// Ok in geen group aanwezig.
	}
	if ((1 == list.GetCount()) && (list.Find(groupAddress))) {
		return TRUE;		// Al aanwezig in deze groep... 
	}

	const CString strTitle("Add device to the group?");
	CString msg;
	msg.Format("The device A%d is already a member of other groups.\r\n"
			   "Continue and add this device to the group %d?", shortAddress, groupAddress);
	if (IDYES == parent->MessageBox(msg, strTitle , MB_YESNO | MB_ICONWARNING)) {
		return TRUE;
	}
	return FALSE;
}

void CAppDlgDaliWorker::getListControlGearGroupMembers(int groupAddress,CList<int>* pList) {
	if (NULL == pList) {
		REQUIRE(NULL != pList);
		return;
	}

	pList->RemoveAll();

	CAppDaliDevices* const pDevices = this->getDaliDevices( );
	if (NULL == pDevices) {
		return;
	}
	for (int shortAddress = 0;shortAddress <= 63; ++shortAddress) {
		if (CAppDaliDevices_Utils::isDeviceGroupMember(pDevices,DALI_DEVICE_TYPE_CONTROLGEAR,shortAddress,groupAddress)) {
			pList->AddTail(shortAddress);
		}
	}
}

void CAppDlgDaliWorker::getListOfGroups(int shortAddress, CList<int>* pList) {
	if (NULL == pList) {
		REQUIRE(NULL != pList);
		return;
	}
	pList->RemoveAll();

	CAppDaliDevices* const pDevices = this->getDaliDevices();
	if (NULL == pDevices) {
		return;
	}
	CAppDaliDevice* pDevice = NULL;
	if (!pDevices->find(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &pDevice)) {
		return;
	}
	for (int groupAddress = DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST; groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST; ++groupAddress) {
		if (CAppDaliDevice_Utils::isDeviceGroupMember(*pDevice, groupAddress)) {
			pList->AddTail(groupAddress);
		}
	}
}

/*
BOOL CAppDlgDaliWorker::getDevice(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,DUOTECNO::DALI::CAppDaliDevice** ppDevice) {
	CAppDali* const pAppDali = this->getCurrent();
	if (NULL == pAppDali) {
		return FALSE;
	}	
	CAppDaliDevice* pDevice = NULL;
	if (FALSE == pAppDali->getDevices().find(deviceType,shortAddress,&pDevice)) {
		return FALSE;
	}
	*ppDevice = pDevice;
	ENSURE(NULL != *ppDevice);
	return TRUE;
}
*/

// Opvragen van de naam van een device.
BOOL CAppDlgDaliWorker::getDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,CString* pStrName) {
	CAppDali* const pAppDali = this->getCurrent();
	if (NULL == pAppDali) {
		return FALSE;
	}
	return pAppDali->getDeviceName(deviceType,shortAddress,pStrName);
}

BOOL CAppDlgDaliWorker::setDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,const CString& strName,CString* pStrNormalized) {
	CAppDali* const pAppDali = this->getCurrent();
	if (NULL == pAppDali) {
		return FALSE;
	}
	return pAppDali->setDeviceName(deviceType,shortAddress,strName,pStrNormalized);
}

BOOL CAppDlgDaliWorker::getControlDeviceInstanceName(int shortAddress,int instanceNumber, CString* pStrName) {
	CAppDali* const pAppDali = this->getCurrent();
	if (NULL == pAppDali) {
		return FALSE;
	}
	return pAppDali->getControlDeviceInstanceName(shortAddress,instanceNumber,pStrName);
}

BOOL CAppDlgDaliWorker::setControlDeviceInstanceName(int shortAddress,int instanceNumber,const CString& strName,CString* pStrNormalized) {
	CAppDali* const pAppDali = this->getCurrent();
	if (NULL == pAppDali) {
		return FALSE;
	}
	return pAppDali->setControlDeviceInstanceName(shortAddress,instanceNumber,strName,pStrNormalized);
}

CString CAppDlgDaliWorker::getStringInstanceType(int instanceType) {
	CString strName;
	switch(instanceType) {
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON:  {
			strName = CString("Push buttons");
		} break;		
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT: {
			strName = CString("Abs input devices");
		} break;		
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY: {
			strName = CString("Occupancy sensor");
		} break;		
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_LIGHT_SENSOR: {
			strName = CString("Light sensor (N/A)");
		} break;		
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_COLOR_SENSOR: {
			strName = CString("Color sensor (N/A)");
		} break;		
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_GENERAL_SENSOR: {
			strName = CString("General sensor (N/A)");
		} break;
		default: {
			strName = CString("Unknown (N/A)");		 
		} break;
	}
	return strName;
}

bool CAppDlgDaliWorker::getStrEventMsg(int shortAddress,int instanceNumber,uint8_t eventData,int* pInstanceType,CString* pStrEventMsg,CString* pStrInstanceType) {
	using namespace DUOTECNO::DALI;

	if (NULL == pStrEventMsg || NULL == pStrInstanceType || NULL == pInstanceType) {
		return false;
	}

	CAppDali* const pAppDali = this->getCurrent();
	if (NULL == pAppDali) {
		return false;
	}		
	
	CAppDaliDevice* pDevice = NULL;
	if (FALSE == pAppDali->getDevices().find(DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,&pDevice)) {
		return false;
	}
	
	ASSERT(NULL != pDevice);
	CAppDaliDeviceProperties_InputControlDevice* properties = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
	if (NULL == properties) {
		return false;
	}

	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (FALSE == properties->getInstances().find(instanceNumber,&pInstance)) {
		return false;
	}
	ASSERT(NULL != pInstance);

	CString msgInfo;
	const int instanceType = pInstance->getInstanceType();
	switch(instanceType) {
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON: {

			*pStrInstanceType = getStringInstanceType(instanceType);

			enum {
				DALI_PUSHBUTTON_EVENT_RELEASED		= 0x00,
				DALI_PUSHBUTTON_EVENT_PRESSED		= 0x01,
				DALI_PUSHBUTTON_EVENT_PULS			= 0x02,
				DALI_PUSHBUTTON_EVENT_DOUBLEPRESS	= 0x05,
				DALI_PUSHBUTTON_EVENT_LONG_ON		= 0x09,
				DALI_PUSHBUTTON_EVENT_LONG_REPEAT	= 0x0B,
				DALI_PUSHBUTTON_EVENT_LONG_OFF		= 0x0C,
				DALI_PUSHBUTTON_EVENT_FREE			= 0x0E,
				DALI_PUSHBUTTON_EVENT_STUCK			= 0x0F,
			};			
			switch(eventData) {
				case DALI_PUSHBUTTON_EVENT_RELEASED: {
					*pStrEventMsg = "Released (N/U)";					 
				} break;
				case DALI_PUSHBUTTON_EVENT_PRESSED: {
					*pStrEventMsg = "Pressed (N/U)";									
				} break;
				case DALI_PUSHBUTTON_EVENT_PULS: {
					*pStrEventMsg = "Puls";				  
				} break;
				case DALI_PUSHBUTTON_EVENT_DOUBLEPRESS: {
					*pStrEventMsg = "Double press (N/U)";								
				} break;
				case DALI_PUSHBUTTON_EVENT_LONG_ON: {
					*pStrEventMsg = "Long on";									
				} break;
				case DALI_PUSHBUTTON_EVENT_LONG_REPEAT: {
					*pStrEventMsg = "Long repeat (N/U)";								
				} break;
				case DALI_PUSHBUTTON_EVENT_LONG_OFF: {
					*pStrEventMsg = "Long off";									
				} break;
				case DALI_PUSHBUTTON_EVENT_FREE: {
					*pStrEventMsg = "Button stuck (free)";									
				} break;
				case DALI_PUSHBUTTON_EVENT_STUCK: {
					*pStrEventMsg = "Button stuck";									
				} break;
				default: {
					*pStrEventMsg = "?"; 
				} break;
			}			
		} break;
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY: {

			*pStrInstanceType = getStringInstanceType(instanceType);				

			if (eventData & (1<<0)) {
				*pStrEventMsg = "Movement";
			}
			else {
				*pStrEventMsg = "No movement";
			}

			if (eventData & (1<<1)) {
				if (eventData & (1<<2)) {
					*pStrEventMsg += "-occupied (repeat)";
				}
				else {
					*pStrEventMsg += "-occupied";
				}
			}
			else {
				if (eventData & (1<<2)) {
					*pStrEventMsg += "-vacant (repeat)";
				}
				else {
					*pStrEventMsg += "-vacant";
				}
			}
		} break;

		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_LIGHT_SENSOR: {
			*pStrInstanceType = getStringInstanceType(instanceType);																
			*pStrEventMsg = "Not supported";
		} break;

		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT: {
			*pStrInstanceType = getStringInstanceType(instanceType);	
			if (0 == eventData) {
				*pStrEventMsg = "Idle";
			}
			else {
				*pStrEventMsg = "Active";			
			}
		} break;

		default: {
			*pStrInstanceType = "Not supported";																
			*pStrEventMsg = "Not supported";
		} break;
	}

	*pInstanceType = instanceType;
	return true;			
}


DUOTECNO::DALI::CAppDaliManager& CAppDlgDaliWorker::getAppDaliManager( ) {
	return CAppDaliManager::getInstance();
}

const DUOTECNO::DALI::CAppDaliManager& CAppDlgDaliWorker::getAppDaliManager( ) const {
	return CAppDaliManager::getInstance();
}
/*****************************************************************/