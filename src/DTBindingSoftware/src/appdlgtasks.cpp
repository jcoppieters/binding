#include "stdafx.h"
#include "BindingSoftware.h"
#include "appdlgtasks.h"
#include "appdlgtasks_base.h"
#include "app/appstate/appstate.h"
#include "mainfrm.h"
#include "TXNodeMessage.h"									// Bericht configuren van het nodeAddress.
#include "app/appudpcfgserver/AppUDPServerCfg.h"
#include "communication/tcpsocket/tcpsocketbase/tcpsocketcmn.h"

#include "app/appremotecfg/AppRemoteCfgNodes.h"				// App wrapper
#include "appremotecfg/AppRemoteCfgLoginInfo.h"				// Library.
#include "appremotecfg/AppRemoteCfgNodeService.h"			// Library.
#include "appremotecfg/iappremotecfgapi.h"					// Library.

// Evt onderstaande naar de CMainFrame brengen via een sendMessage...
// #include "DlgNodeServicesCfg.h"							// TODO !!!!

#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"			// controle op product code.

/*****************************************************************/
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalSettings;
using namespace TMNodeDatabase;

/*****************************************************************/
// Doet een controle op de productcode van de UDP-Devices.
// Enkel de Homeserver wordt toeglaten om me te connecterern.
// Wanneer deze waarde op 0 staat dan worden alle devices getoond die gevonden werden.
#define CHECK_UDP_DEVICE_PRODUCTCODE			1
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
using namespace DUOTECNO;
/*****************************************************************/
namespace {
	void setStateAppMode(CAppMode::Types type) {
		CAppStateUtil::setAppMode(CAppState::getInstance(),type);
	}
};
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
IAppDlgStartDialog_Host::~IAppDlgStartDialog_Host() { }
/*****************************************************************/
CAppDlgStartDialog_Host::CAppDlgStartDialog_Host(DUOTECNO::AppRemoteCfgLoginInfo& refLoginInfo,AppUDPServerCfg& refUDPServerCfg,AppRemoteCfgNodes& refCfgNodeServices) : 
	m_refLoginInfo(refLoginInfo), m_refUDPServerCfg(refUDPServerCfg), m_refCfgNodeServices(refCfgNodeServices)  {  }

CAppDlgStartDialog_Host::~CAppDlgStartDialog_Host( ) {  }

void CAppDlgStartDialog_Host::setStateAppIdle( ) {
	setStateAppMode(CAppMode::APP_IDLE);
}
void CAppDlgStartDialog_Host::SetStateAppModeOfflineNew( ) {
	ASSERT(0);	// kan niet voorkomen.
	setStateAppMode(CAppMode::APP_NEW_OFFLINE);	
}
void CAppDlgStartDialog_Host::SetStateAppModeOfflineContinue( ) {
	setStateAppMode(CAppMode::APP_CONTINUE_OFFLINE);
}
void CAppDlgStartDialog_Host::SetStateAppModeOnlineNew( ) {
	setStateAppMode(CAppMode::APP_NEW_ONLINE);	
}
void CAppDlgStartDialog_Host::SetStateAppModeOnlineContinue( ) {
	setStateAppMode(CAppMode::APP_CONTINUE_ONLINE);
}

void CAppDlgStartDialog_Host::setAppUsbConnect(int comport) {
	ASSERT(comport>0 && comport<=255);	// compoort van 1 tem 255
	TMGetCommunicationApp()->setSerialUsbConnect(comport);	
}

void CAppDlgStartDialog_Host::setAppTcpConnect(ConnectionMethod_t method, const CString& hostName, int port, const CString& pwd, const CString& cloudServerLoginToken) {
	TMGetCommunicationApp()->setTcpConnect(method,hostName,port,pwd, cloudServerLoginToken);
}

void CAppDlgStartDialog_Host::setAppDisconnect() {
	TMGetCommunicationApp()->setDisconnect();
}
void CAppDlgStartDialog_Host::setDefaultUSBPort(int comport) {
	ASSERT(comport>0 && comport<=255);			// compoort van 1 tem 255
	if (TMGetCommunicationSettings() != NULL) {			
		TMGetCommunicationSettings()->SetDefaultComPort(comport);	
	}
}
int CAppDlgStartDialog_Host::getDefaultUSBPort( ) {
	int comport = -1;
	if (TMGetCommunicationSettings() != 0) {
		const int temp = TMGetCommunicationSettings()->GetDefaultComPort();
		if (temp>0 && temp<=255) {
			comport = temp;
		}		
	}
	return comport;
}

BOOL CAppDlgStartDialog_Host::isConnected() {
	return TMIsConnected();
}

BOOL CAppDlgStartDialog_Host::loadDocumentFile(CString* pszProjectName) {
	
	BOOL result = FALSE;
	CMainFrame* const pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (0 != pMainWnd) {
		CStringArray strArrayErrors;
		result = pMainWnd->LoadDocumentFile(&strArrayErrors);
		if (result) {
			pMainWnd->VerifyDocumentBindingDatabases(&strArrayErrors);

			// Naam van het ingeladen project...
			if (0 != TMGetFileSettings()) {
				*pszProjectName = TMGetFileSettings()->GetTitle();
			}			
		}
	}
	return result;
}

// Opvragen van de ingeladen properties.
CAppDlgStartDialog_Host::ConnectMode_t CAppDlgStartDialog_Host::getDefaultConnectMode() {
	ConnectMode_t result = OFFLINE;	
	const AppRemoteCfgLoginInfo::LoginMethodType_t m = m_refLoginInfo.getMethod();
	switch(m) {
		case AppRemoteCfgLoginInfo::LOGIN_METHOD_OFFLINE: {
			result = OFFLINE;
		} break;
		case AppRemoteCfgLoginInfo::LOGIN_METHOD_TCPIP: {
			result = TCP;										
		} break;
		case AppRemoteCfgLoginInfo::LOGIN_METHOD_USB: {
			result = USB;									  
		} break;
	}	
	return result;
}

BOOL CAppDlgStartDialog_Host::getServerInfoLocal(CString* hostName,int* port) {
	const CString& localHostName = m_refLoginInfo.getLocalHostAddress();
	*hostName = localHostName;
	*port = m_refLoginInfo.getLocalPort();
	return TRUE;
}

BOOL CAppDlgStartDialog_Host::getServerInfoRemote(CString* hostName,int* port) {
	const CString& remoteHostName = m_refLoginInfo.getRemoteHostAddress();
	*hostName = remoteHostName;
	*port = m_refLoginInfo.getRemotePort();
	return TRUE;
}

BOOL CAppDlgStartDialog_Host::getPreferedServerLocal() {	
	return (HOSTADDRESS_TYPE_LOCAL == m_refLoginInfo.getPreferedLogin());	
}

BOOL CAppDlgStartDialog_Host::getServerPassword(CString* password) {
	const CString& strPassword = m_refLoginInfo.getPassword();
	*password = strPassword;
	return TRUE;
}

BOOL CAppDlgStartDialog_Host::searchRemoteDns(int nodeAddress,CString* hostAddress) {
	return AppRemoteCfgNodes_Util::searchNodesForRemoteDns(m_refCfgNodeServices, nodeAddress, hostAddress);
}

// Is de LAN Server info ingevuld?
BOOL CAppDlgStartDialog_Host::getServerInfoLocalValid( ) {

	const CString& strPassword = m_refLoginInfo.getPassword();
	if (strPassword.IsEmpty()) {
		return FALSE;
	}
	const CString& localHostName = m_refLoginInfo.getLocalHostAddress();
	if (localHostName.IsEmpty()) {
		return FALSE;
	}
	int port = m_refLoginInfo.getLocalPort();
	if (port <=0) {
		return FALSE;
	}
	return TRUE;
}

// Updaten van de login properties.
void CAppDlgStartDialog_Host::setDefaultConnectMode(ConnectMode_t mode) 
{
	switch(mode) {
		case OFFLINE: {
			m_refLoginInfo.setMethod(AppRemoteCfgLoginInfo::LOGIN_METHOD_OFFLINE);
		} break; 
		case USB: {
			m_refLoginInfo.setMethod(AppRemoteCfgLoginInfo::LOGIN_METHOD_USB); 
		} break; 			
		case TCP: {
			m_refLoginInfo.setMethod(AppRemoteCfgLoginInfo::LOGIN_METHOD_TCPIP);				  
		} break;
	}
}

void CAppDlgStartDialog_Host::setServerTCPIPInfo(const CString& localHostName, int localPort, 
						const CString& remoteHostName, int remotePort, const CString& pwd, BOOL connectLocal) 
{
	m_refLoginInfo.setLocalHostAddress(localHostName);
	m_refLoginInfo.setLocalPort(localPort);
	m_refLoginInfo.setRemoteHostAddress(remoteHostName);
	m_refLoginInfo.setRemotePort(remotePort);
	m_refLoginInfo.setPassword(pwd);
	m_refLoginInfo.setPreferedLogin(TRUE == connectLocal ? HOSTADDRESS_TYPE_LOCAL : HOSTADDRESS_TYPE_REMOTE);
}

AppUDPDevices& CAppDlgStartDialog_Host::getUDPDeviceList() {
	return m_refUDPServerCfg.getDeviceList();
}
void CAppDlgStartDialog_Host::clearUDPDeviceList() {
	return m_refUDPServerCfg.eraseData();
}

bool CAppDlgStartDialog_Host::removeDevice(const CString& macAddress) {
	return m_refUDPServerCfg.getDeviceList().remove(macAddress);
}
bool CAppDlgStartDialog_Host::updateNetworkSettings(AppUDPDevice* pDevice) {
	return AppUDPDevices_Utils::updateNetworkSettings(m_refUDPServerCfg.getDeviceList(),pDevice);
}

// Sedert V16.32: Integratie sbox ultra.
bool CAppDlgStartDialog_Host::getTCPIPCfgSupported(const AppUDPDevice& refDevice) {
#if (CHECK_UDP_DEVICE_PRODUCTCODE == 1)
	const int productCode = refDevice.getProductCode();
	if (!(productCode == PRODUCT_ID_GATEWAY_HS ||
		  productCode == PRODUCT_ID_GATEWAY_HS_32VIRT ||
		  productCode == PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT ||
		  productCode == PRODUCT_ID_HC_BOX_ULTRA))
	{
		return false;
	}
#endif 
	return true;
}

void CAppDlgStartDialog_Host::selectNetworkAdapter(const CString& strNwkAdapter) {
	m_refUDPServerCfg.selectNetworkAdapter(strNwkAdapter);
}

#define TEST_PROPERTYPAGE8			0			// Om te testen return true.

bool CAppDlgStartDialog_Host::getServerNodeAddressFactory() const {
	bool result = true;
#if (TEST_PROPERTYPAGE8 == 0)
	result = (0xFE == TMCommunicationGetStatus().getServerInfo().getNodeAddress());
#endif 
	return result;
}

BYTE CAppDlgStartDialog_Host::getServerNodeAddress() const {
	return TMCommunicationGetStatus().getServerInfo().getNodeAddress();
}
unsigned long CAppDlgStartDialog_Host::getServerPhysicalAddress() const {
	return TMCommunicationGetStatus().getServerInfo().getPhysicalNodeAddress();
}

bool CAppDlgStartDialog_Host::getServerNodeMaster() const {
	return TMCommunicationGetStatus().getServerInfo().getServerMasterNode();
}
bool CAppDlgStartDialog_Host::getServerNodeMasterSupported() const{
	return TMCommunicationGetStatus().getServerInfo().getServerMasterNodeSupported();
}

bool CAppDlgStartDialog_Host::sendServerTCPIPGetNodeInfo() {
	if (TMGetCommunicationApp()->sendServerTCPIPGetNodeInfo()) {
		return true;
	}
	return false;
}

bool CAppDlgStartDialog_Host::sendServerTCPIPSetMaster() {
	if (TMGetCommunicationApp()->sendServerTCPIPSetMaster()) {
		return true;
	}
	return false;
}

// Sedert V16.26: zie USE_WORKAROUND_MESSAGEERROR_ABSORBE
// Bij het veranderen van een logisch adres krijgen we een EV_MESSAGEERROR van de tcp/ip server.
// Daarom doorgeven aan de CComApplication dat de eerst EV_MESSAGEEEROR die ontvangen wordt van de tcp/ip server
// niet mag getoond worden.
bool CAppDlgStartDialog_Host::sendServerTCPIPNodeAddress(BYTE bNodeAddress) {
	const int physicalAddress = TMCommunicationGetStatus().getServerInfo().getPhysicalNodeAddress();
	if (bNodeAddress < 0x03 || bNodeAddress >= 0xfc) {
		ASSERT(bNodeAddress >= 0x03 && bNodeAddress < 0xfc);
		return false;
	}	
	if (physicalAddress <= 0 && physicalAddress >= 0xffffff) {
		ASSERT(physicalAddress > 0 && physicalAddress < 0xffffff);
		return false;	
	}
	CTXBrowsePnpMessages* const pTxMessages = CTXBrowsePnpMessages::getInstance();
	if (NULL == pTxMessages) {
		return false;
	}

	if (pTxMessages->AssignPhysicalAddress(physicalAddress,bNodeAddress)) {
		TMGetCommunicationApp()->setAbsorbeMessageError(bNodeAddress);
		return TRUE;
	}
	return FALSE;
}

bool CAppDlgStartDialog_Host::SearchDbaseNodeLogicalAddress(int nodeAddress,CString* pstrName,int* physicalAddress) {
	int iEntryNr = 0;
	CCanNode* pNode = NULL;
	if (!TMGetDocumentNodes()->FindNode(nodeAddress,&iEntryNr,&pNode)){
		return false;
	}
	if (NULL == pNode) {
		return false;
	}
	*pstrName = CString(pNode->getNodeName());
	*physicalAddress = pNode->getPhysicalAddress();
	return true;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
IAppDlgUDPLanCfg_Host::~IAppDlgUDPLanCfg_Host() { }
/*****************************************************************/
CAppDlgUDPLanCfg_Host::CAppDlgUDPLanCfg_Host(AppUDPServerCfg& refUDPServerCfg,AppRemoteCfgNodes& refAppRemoteCfgNodes,AppRemoteCfgLoginInfo& refAppRemoteCfgLoginInfo) :
	m_refUDPServerCfg(refUDPServerCfg), m_refAppRemoteCfgNodes(refAppRemoteCfgNodes), m_refAppRemoteCfgLoginInfo(refAppRemoteCfgLoginInfo) { }

CAppDlgUDPLanCfg_Host::~CAppDlgUDPLanCfg_Host() { }

bool CAppDlgUDPLanCfg_Host::removeDevice(const CString& macAddress) {
	return m_refUDPServerCfg.getDeviceList().remove(macAddress);
}
bool CAppDlgUDPLanCfg_Host::updateNetworkSettings(AppUDPDevice* pDevice) {
	return AppUDPDevices_Utils::updateNetworkSettings(m_refUDPServerCfg.getDeviceList(),pDevice);
}
void CAppDlgUDPLanCfg_Host::setUDPScanModeFast() {
	m_refUDPServerCfg.setTimingsScanMode(AppUDPServerCfg::SCANMODE_FAST);
}
void CAppDlgUDPLanCfg_Host::setUDPScanModeNormal() {
	m_refUDPServerCfg.setTimingsScanMode(AppUDPServerCfg::SCANMODE_NORMAL);
}
void CAppDlgUDPLanCfg_Host::updateNodeHomeserverService(AppUDPDevice& r) {
	AppRemoteCfgNodes_Util::updateDeviceByMACAddress(&m_refAppRemoteCfgNodes,r,AppRemoteCfgNodes_Util::MASK_CONFIG_HOMESERVER);
	AppRemoteCfgLoginInfo_Util::updateLoginAfterUdpDeviceCfg(&m_refAppRemoteCfgLoginInfo,r,AppRemoteCfgLoginInfo_Util::MASK_CONFIG_HOMESERVER);
}
void CAppDlgUDPLanCfg_Host::updateNodeDynDnHostname(AppUDPDevice& r) {
	AppRemoteCfgNodes_Util::updateDeviceByMACAddress(&m_refAppRemoteCfgNodes,r,AppRemoteCfgNodes_Util::MASK_CONFIG_DYNDNS);
	AppRemoteCfgLoginInfo_Util::updateLoginAfterUdpDeviceCfg(&m_refAppRemoteCfgLoginInfo,r,AppRemoteCfgLoginInfo_Util::MASK_CONFIG_DYNDNS);
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
IAppDlgHomeClientMasterTasks_Host::~IAppDlgHomeClientMasterTasks_Host() { }
IAppDlgHomeClientMasterTasks_Host::IAppDlgEvents_Host::~IAppDlgEvents_Host() { }
/*****************************************************************/
CAppDlgHomeClientMasterTasks_Host::CAppDlgHomeClientMasterTasks_Host( ) : 
	m_pHost(NULL), m_strAppTask("Online idle"), m_strNodeDbaseState("Database empty"), m_pNode(NULL) { }

CAppDlgHomeClientMasterTasks_Host::~CAppDlgHomeClientMasterTasks_Host() { }

void CAppDlgHomeClientMasterTasks_Host::init(CCanNode* pNode) {	
	if (NULL == pNode) {
		ASSERT(NULL != pNode);
		return;
	}
	this->m_pNode = pNode;
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(this->m_pNode);
	if (NULL != pSocketHomeClient) {
		pSocketHomeClient->subscribe(this);

		// Status van de applicatie opvragen.
		const TcpSocketHomeClient_Tasks::State_t state = pSocketHomeClient->getAppTaskState();
		updateStringAppState(state);

		// Status van de nodedatabase opvragen.
		const TcpSocketHomeClient_NodeDbase::Status_t status = pSocketHomeClient->getAppDbaseStatus();
		updateStringNodeDatabase(status);
	}
}

void CAppDlgHomeClientMasterTasks_Host::close() {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(this->m_pNode);
	if (NULL != pSocketHomeClient) {
		pSocketHomeClient->unSubscribe(this);
	}	
}

bool CAppDlgHomeClientMasterTasks_Host::disconnect( ) {
	return CAppDlgTasksBaseHomeClientUser::disconnectHomeClientSocket(this->m_pNode);
}
bool CAppDlgHomeClientMasterTasks_Host::isStateIdle() {
	return CAppDlgTasksBaseHomeClientUser::getHomeClientSocketStateIDLE(this->m_pNode);
}
bool CAppDlgHomeClientMasterTasks_Host::isConnected() {
	return (CAppDlgTasksBaseHomeClientUser::getHomeClientSocketConnected(this->m_pNode));
}

// Uitvoeren van de taken...
static const char* const pwd = "2222";		// Tijdelijk hier...
bool CAppDlgHomeClientMasterTasks_Host::startTaskPnp() {
	return CAppDlgTasksBaseHomeClientUser::startHomeclientTaskPnp(this->m_pNode,pwd);
}
bool CAppDlgHomeClientMasterTasks_Host::startTaskBrowse() {
	return CAppDlgTasksBaseHomeClientUser::startHomeclientTaskBrowse(this->m_pNode,pwd);
}
bool CAppDlgHomeClientMasterTasks_Host::startTaskBrowseClear() {
	return CAppDlgTasksBaseHomeClientUser::startHomeclientTaskBrowseClear(this->m_pNode,pwd);
}
bool CAppDlgHomeClientMasterTasks_Host::startTaskUpdateDbase() {
	return CAppDlgTasksBaseHomeClientUser::startHomeclientTaskUpdateNodeDatabase(this->m_pNode,pwd);
}
bool CAppDlgHomeClientMasterTasks_Host::cancelTask() {
	return CAppDlgTasksBaseHomeClientUser::cancelHomeclientTask(this->m_pNode,pwd);
}

// Subscriben.
void CAppDlgHomeClientMasterTasks_Host::subscribe(IAppDlgEvents_Host* pHost) {
	m_pHost = pHost;
}
void CAppDlgHomeClientMasterTasks_Host::unsubscribe(IAppDlgEvents_Host* pHost) {
	m_pHost = NULL;
}

// Dialoog tonen...
void CAppDlgHomeClientMasterTasks_Host::showCfgNodeServices( ) {	
	if (NULL == this->m_pNode) {
		ASSERT(this->m_pNode != NULL);
		return;
	}
	// Oppassen:
	// Deze mag enkel synschroon verstuurd worden want geeft een ptr naar CCanNode mee.
	AfxGetMainWnd()->SendMessage(WM_APPLICATION_NODE_SHOW_SERVICES, (WPARAM) this->m_pNode);
}

bool CAppDlgHomeClientMasterTasks_Host::connectLocalPossible( ) {
	return CAppDlgTasksBaseHomeClientUser::getHomeClientConnectPossible(this->m_pNode, CONNECTION_METHOD_LOCAL);
}
bool CAppDlgHomeClientMasterTasks_Host::connectRemotePossible( ) {
	return CAppDlgTasksBaseHomeClientUser::getHomeClientConnectPossible(this->m_pNode, CONNECTION_METHOD_REMOTE_DIRECT);
}
bool CAppDlgHomeClientMasterTasks_Host::connectTCPHomeClient(ConnectionMethod_t method) {
	return CAppDlgTasksBaseHomeClientUser::connectHomeClient(this->m_pNode, method);
}

HomeClientConnectionState_t CAppDlgHomeClientMasterTasks_Host::getTypeConnected( ) {
	return CAppDlgTasksBaseHomeClientUser::getHomeClientConnectionState(this->m_pNode);
}

// ITcpSocketHomeClient_Host

void CAppDlgHomeClientMasterTasks_Host::onStateConnectChanged(int id,bool connected) {
	if (m_pHost) {
		m_pHost->setStateConnected(connected);
	}
}

void CAppDlgHomeClientMasterTasks_Host::onStateChanged(int id, TcpSocketHomeClient_Tasks::State_t state) {
	switch(state) {
		case TcpSocketHomeClient_Tasks::STATE_IDLE: {
			if (m_pHost) {
				m_pHost->setStateIdle();
			}
		} break;
		case TcpSocketHomeClient_Tasks::STATE_BUSY_PNP: 
		case TcpSocketHomeClient_Tasks::STATE_BUSY_BROWSE:
		case TcpSocketHomeClient_Tasks::STATE_BUSY_UPDATEDBASE: 
		case TcpSocketHomeClient_Tasks::STATE_BUSY_GENERAL: {
			if (m_pHost) {
				m_pHost->setStateBusy();
			}					 
		} break;
		default: {
			// Absorbe... ?				 
		} break;
	}
	updateStringAppState(state);
}

void CAppDlgHomeClientMasterTasks_Host::updateStringAppState(TcpSocketHomeClient_Tasks::State_t state) {
	switch(state) {
		case TcpSocketHomeClient_Tasks::STATE_IDLE: 
			m_strAppTask = CString("Online idle");		
		break;
		case TcpSocketHomeClient_Tasks::STATE_BUSY_PNP: 
			m_strAppTask = CString("Pnp busy");																
		break;
		case TcpSocketHomeClient_Tasks::STATE_BUSY_BROWSE:
			m_strAppTask = CString("Browse busy");
		break;
		case TcpSocketHomeClient_Tasks::STATE_BUSY_UPDATEDBASE: 
			m_strAppTask = CString("Update database busy");
		break;
		case TcpSocketHomeClient_Tasks::STATE_BUSY_GENERAL:	
		default: 
			m_strAppTask = CString("Online busy");	
		break;
	}	
}

void CAppDlgHomeClientMasterTasks_Host::updateStringNodeDatabase(TcpSocketHomeClient_NodeDbase::Status_t status) {
	switch(status) {
		case TcpSocketHomeClient_NodeDbase::STATUS_EMPTY:
			m_strNodeDbaseState = CString("Database empty");
		break;
		case TcpSocketHomeClient_NodeDbase::STATUS_BUSY:
			m_strNodeDbaseState = CString("Database busy");
		break;
		case TcpSocketHomeClient_NodeDbase::STATUS_READY:
			m_strNodeDbaseState = CString("Database ready");
		break;
	}
}

void CAppDlgHomeClientMasterTasks_Host::onSocketHomeClientMsgInfo(int id, CategoryMsg_t category, const CString& msg) {
	if (NULL == m_pHost) {
		return;
	}
	m_pHost->addInfoMsg(msg);
}

void CAppDlgHomeClientMasterTasks_Host::onSocketHomeClientMsgError(int id, CategoryMsg_t category, const CString& msg) {
	if (NULL == m_pHost) {
		return;
	}
	m_pHost->addErrorMsg(msg);
}

static bool helper_format(TcpSocketHomeClient_Tasks::Task_t task,char* szBuffer,int maxLength,int* length)
{
	bool result = false;
	enum { MAX_TASKS = TcpSocketHomeClient_Tasks::TASK_FACTORY_RESETALL + 1 };

	static const char* const _szSTasks[ MAX_TASKS ] =
	{
		"Browse for nodes",					// TASK_BROWSE				= 0,	// NIET VERANDEREN !!		
		"Browse for nodes/Clr dbase",		// TASK_BROWSE_CLEAR		= 1,	// NIET VERANDEREN !!
		"Plug and play",					// TASK_PNP					= 2,	// NIET VERANDEREN !!
		"Update database",					// TASK_UPDATE_DBASE	    = 3,	// NIET VERANDEREN !!
		"Reset all nodes",					// TASK_RESETALL			= 4,	// NIET VERANDEREN !!
		"Factory reset all nodes",			// TASK_FACTORY_RESETALL	= 5,	// NIET VERANDEREN !!	
	};

	if (task < MAX_TASKS) {
		const int len = strlen(_szSTasks[task]);
		if (len < maxLength) {
			strcpy(szBuffer,_szSTasks[task]);
			*length = len+1;
			result = true;
		}		
	}
	return result;
}

void CAppDlgHomeClientMasterTasks_Host::onTaskStatus(int id, TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) {

	char msg[64];
	int temp;
	memset(msg,0,sizeof(msg));
	if (helper_format(task,msg,sizeof(msg),&temp)) {
		const CString infoMsg = CString(msg) + (TcpSocketHomeClient_Tasks::STATUS_FAILED == status ? " failed to start" : " started");
		m_pHost->addInfoMsg(infoMsg);
	}
}

void CAppDlgHomeClientMasterTasks_Host::onTaskCancelStatus(int id, TcpSocketHomeClient_Tasks::Status_t status) {
	const CString infoMsg = CString("Cancel current task") + (TcpSocketHomeClient_Tasks::STATUS_FAILED == status ? " failed" : ".");
	m_pHost->addInfoMsg(infoMsg);
}

void CAppDlgHomeClientMasterTasks_Host::onNodeDbaseStatus(int id, TcpSocketHomeClient_NodeDbase::Status_t status) {
	updateStringNodeDatabase(status);
	m_pHost->update();				

}

void CAppDlgHomeClientMasterTasks_Host::onStatusDALIRxMsgSubscribe(int id,bool status) {
	// absorbe...
}

void CAppDlgHomeClientMasterTasks_Host::onStatusDALIResponse(int id, int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData) {
	// absorbe.
}
void CAppDlgHomeClientMasterTasks_Host::onRxDALIMessage(int id, BYTE lengthBits, uint32_t recievedFrame) {
	// absorbe.
}
// ITcpSocketHomeClient_Host - end

/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
IAppDlgNodeServicesCfg::~IAppDlgNodeServicesCfg() { }
/*****************************************************************/
CAppDlgNodeServicesCfg_Host::CAppDlgNodeServicesCfg_Host(AppRemoteCfgNodes& refCfg) : 
	m_refCfg(refCfg) { }
CAppDlgNodeServicesCfg_Host::~CAppDlgNodeServicesCfg_Host() { }

bool CAppDlgNodeServicesCfg_Host::getNodePassword(int nodeAddress,CString* pwd) {
	return AppRemoteCfgNodes_Util::getNodePassword(m_refCfg, nodeAddress, pwd);
}
bool CAppDlgNodeServicesCfg_Host::getLocalIPSettings(int nodeAddress,CString* hostAddress,int* port) {
	return AppRemoteCfgNodesTCPIPService::getSettingsLocal(m_refCfg, nodeAddress, hostAddress, port);
}
bool CAppDlgNodeServicesCfg_Host::getRemoteIPSettings(int nodeAddress,CString* hostAddress,int* port) {
	return AppRemoteCfgNodesTCPIPService::getSettingsRemote(m_refCfg, nodeAddress, hostAddress, port);
}
bool CAppDlgNodeServicesCfg_Host::searchRemoteDns(int nodeAddress, CString* hostAddress) {
	return AppRemoteCfgNodes_Util::searchNodesForRemoteDns(m_refCfg, nodeAddress, hostAddress);
}
bool CAppDlgNodeServicesCfg_Host::updateNodeSettings(int nodeAddress,const CString& pwd,const CString& ipAddress,int portLocal,
		const CString& hostAddress,int portRemote)
{
	return AppRemoteCfgNodesTCPIPService::updateNodeService(&m_refCfg,nodeAddress,pwd,ipAddress,portLocal,hostAddress,portRemote);
}
/*****************************************************************/