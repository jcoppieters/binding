#include "stdafx.h"
#include "tcpsockethomeclient.h"
#include "itcpsockethomeclient_util.h"

#include "../tcpsocketbase/tcpsocketconnectfsm.h"
#include "../tcpsocketbase/tcpsocketmessages_api.h"		// Versturen van berichten.
#include "../../base/communicationserverinfo.h"	
#include "nodegenestructs.h"							// NODE_MASTER_LCD flag
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
ITcpSocketHomeClient::~ITcpSocketHomeClient() { }
ITcpSocketHomeClient_Host::~ITcpSocketHomeClient_Host() { }

/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
BEGIN_MESSAGE_MAP(CTCPSocketHomeClient, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketHomeClient::CTCPSocketHomeClient(ITCPCommunicationObject_Host& refHost,const CTCPSocketOptions& refSocketOptions) :
	CTCPSocketBase(refHost), m_pConnectFsm(0), m_listObservers( ), m_pServerInfo(0), m_flagMaster(false), m_initDone(false), 
		m_stateIdle(true), m_stateAppTasks(TcpSocketHomeClient_Tasks::STATE_IDLE),
		m_statusDbase(TcpSocketHomeClient_NodeDbase::STATUS_EMPTY) { 
	this->setSocketOptions(refSocketOptions);
	this->Create();	
	this->initInstance();
	this->subscribeObserver(this);;
}
	
CTCPSocketHomeClient::~CTCPSocketHomeClient() 
{ 
	this->unsubscribeObserver(this);

	if (0 != m_pConnectFsm) {
		delete m_pConnectFsm;
		m_pConnectFsm = 0;
	}
	if (0 != m_pServerInfo) {
		delete m_pServerInfo;
		m_pServerInfo = 0;
	}

	while (!m_listObservers.IsEmpty()) {
		ITcpSocketHomeClient_Host* const p = m_listObservers.RemoveHead( );		
	}
	ASSERT(m_listObservers.GetCount( )==0 );
}

HWND CTCPSocketHomeClient::Create()
{		
	LPCTSTR classname = AfxRegisterWndClass(0);
	CWnd::CreateEx(NULL,classname,NULL,NULL,1,1,1,1,NULL,NULL);
	ASSERT(m_hWnd!=NULL);		
	return m_hWnd;
}

void CTCPSocketHomeClient::PostNcDestroy() {	
	delete(this);
}

void CTCPSocketHomeClient::OnTimer(UINT nIDEvent) {
	KillTimer(nIDEvent);
	if (TIMER_ID_CONNECTFSM == nIDEvent) {		
		if (m_pConnectFsm) {
			m_pConnectFsm->EvAfterTimeout();
		}		
	}	
}
/*****************************************************************/

bool CTCPSocketHomeClient::connect(ConnectionMethod_t mehod, const char* hostName, int port, const char* password, const char* cloudServerLoginToken) {

	// Controle nog niet geconnecteerd...
	if (this->isConnected()) {
		return true;
	}

	CTCPSocketCommSettings settings(mehod, hostName, port, password, cloudServerLoginToken);
	this->setCommunicationSettings(settings);

	if (0 == m_pServerInfo) {	
		m_pServerInfo = new CCommunicationServerInfo();		
	}	
	if (0 == m_pConnectFsm) {
		ASSERT(0 != m_pServerInfo);
		m_pConnectFsm = new CTCPSocketConnectFsm(*this,temp_getTcpSocketConnectFsmFunctions(),*m_pServerInfo);
	}

	this->m_initDone = false;
	m_pConnectFsm->EvOperationConnect();
	return true;
}

bool CTCPSocketHomeClient::disconnect( ) {
	this->SetDisconnect();
	this->m_initDone = false;
	return true;
}

/*****************************************************************/
// ITcpSocketHomeClient
bool CTCPSocketHomeClient::isConnected() {
	if (SOCKET_STATE_CONNECT_LOGIN_SUCCESS == this->getSocketConnectState()) {
		return true;
	}
	return false;
}

bool CTCPSocketHomeClient::isMaster() {
	return m_flagMaster;
}	
bool CTCPSocketHomeClient::isStateIdle() {
	return m_stateIdle;
}
bool CTCPSocketHomeClient::startPnp(const char* pwd) {
	return ITcpSocketHomeClient_Util::startPnp(*this,this->getSocketMessageAPI(),pwd);
}
bool CTCPSocketHomeClient::startBrowse(const char* pwd) {
	return ITcpSocketHomeClient_Util::startBrowse(*this,this->getSocketMessageAPI(),pwd);
}
bool CTCPSocketHomeClient::startBrowseClear(const char* pwd) {
	return ITcpSocketHomeClient_Util::startBrowseClear(*this,this->getSocketMessageAPI(),pwd);
}
bool CTCPSocketHomeClient::startUpdateNodeDatabade(const char* pwd) {
	return ITcpSocketHomeClient_Util::startUpdateNodeDatabase(*this,this->getSocketMessageAPI(),pwd);
}
bool CTCPSocketHomeClient::cancelTask(const char* pwd) {
	return ITcpSocketHomeClient_Util::sendCancelTask(*this,this->getSocketMessageAPI(),pwd);
}

// Zie USE_TCPIP_DALI
bool CTCPSocketHomeClient::sendSubscribeDALI() {
	if (!this->isConnected()) {
		ASSERT(this->isConnected());
		return false;
	}
	return (TRUE == this->getSocketMessageAPI().sendDaliMsgSubscribe(TRUE));
}

// Zie USE_TCPIP_DALI
bool CTCPSocketHomeClient::sendMsgDali(int clientIdentifier, BYTE msgIdentifier, int prioLevel, int nrOfBits, const BYTE* pbData) {
	if (!this->isConnected()) {
		ASSERT(this->isConnected());
		return false;
	}
	return (TRUE == this->getSocketMessageAPI().sendDALIMessage(clientIdentifier, msgIdentifier, prioLevel, nrOfBits, pbData));
}

void CTCPSocketHomeClient::subscribe(ITcpSocketHomeClient_Host* pHost) {
	TRACE("CTCPSocketHomeClient::subscribe()\r\n");
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {
		ITcpSocketHomeClient_Host* pEntry = 0;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {	
			if (pHost == pEntry) {
				break;	// Can only register once...
			}
		}
	}
	m_listObservers.AddTail(pHost);
}

void CTCPSocketHomeClient::unSubscribe(ITcpSocketHomeClient_Host* pHost) {
	TRACE("CTCPSocketHomeClient::unsubscribe()\r\n");
	int index = 0;
	int indexInList = -1;

	{
		POSITION pos = m_listObservers.GetHeadPosition();
		while(pos!=NULL)  {		
			ITcpSocketHomeClient_Host* pEntry = NULL;
			if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
				if (pHost == pEntry) {
					indexInList = index;	
					break;
				}
				index++;
			}
		}
	}

	if (indexInList >=0) {
		ASSERT(indexInList < m_listObservers.GetCount());
		const POSITION tempPos = m_listObservers.FindIndex(indexInList);
		if (NULL != tempPos) {				
			m_listObservers.RemoveAt(tempPos);			
		}
	}
}

// Sedert V16.36
void CTCPSocketHomeClient::notifyConnectStatusChanged( ) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {	
			pEntry->onStateConnectChanged(this->CTCPSocketBase::getSocketIdentifier(),this->isConnected());
		}
	}
}


// Observer berichten: De gewone msg Observer berichten.						
BOOL CTCPSocketHomeClient::onStatusConnectRequestKey(unsigned short protocolVersion,
	unsigned short softwareVersion,int tokenNumber,
	const char* szClientKey)
{
	return TRUE;		// geen implementatie...
}

BOOL CTCPSocketHomeClient::onStatusConnectPassword(BOOL stateConnected) {
	return TRUE;		// geen implementatie...
}

BOOL CTCPSocketHomeClient::onStatusHeartbeat() {
	return TRUE;		// geen implementatie...
}

BOOL CTCPSocketHomeClient::onStatusCanRxMsgSubscribe(BOOL status) {
	return TRUE;		// geen implementatie...
}

BOOL CTCPSocketHomeClient::onRecievedCanRxMsg(int nodeAddress, int unitAddress,
						int messageCode, int length, const BYTE* pbData) 
{
	return TRUE;		// geen implementatie...
}

BOOL CTCPSocketHomeClient::onRecievedServerNodeInfo(int nodeAddress,unsigned long ulPhysicalAddress,const char* name,
							  int numberOfUnits,int nodeType,int nodeFlags) 
{
	m_flagMaster = (nodeFlags & NODE_MASTER_LCD ? true : false);	
	if (!m_initDone) {
		ITcpSocketHomeClient_Util::sendRequestAppState(*this,getSocketMessageAPI());
	}
	return TRUE;
}

BOOL CTCPSocketHomeClient::onReceivedServerNodeStatusMaster(BOOL masterNode) {
	return FALSE;	// geen implementatie...
}
BOOL CTCPSocketHomeClient::onReceivedServerNodeStatusMasterSupported(BOOL masterSupported) {
	return FALSE;	// geen implementatie...
}
BOOL CTCPSocketHomeClient::onReceivedServerNodeMasterSet(BOOL success) {
	return FALSE;	// geen implementatie...
}

// Observer berichten: Specifieke homeClient functies.
BOOL CTCPSocketHomeClient::onRecievedAppTaskStatus(TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			pEntry->onTaskStatus(this->CTCPSocketBase::getSocketIdentifier(),task,status);			
		}
	}
	return TRUE;
}

BOOL CTCPSocketHomeClient::onRecievedAppTaskCancel(TcpSocketHomeClient_Tasks::Status_t status) {
	// TODO - IMPLEMENT !!!!!!!!!!!!!!!!
	return TRUE;
}

BOOL CTCPSocketHomeClient::onRecievedAppTaskInfo(const CString& msg) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			static const ITcpSocketHomeClient_Host::CategoryMsg_t category = ITcpSocketHomeClient_Host::HOMECLIENT_MSG_CATEGORY_MASTERTASK_FSM;
			pEntry->onSocketHomeClientMsgInfo(this->CTCPSocketBase::getSocketIdentifier(),category, msg);			
		}
	}
	return TRUE;
}

BOOL CTCPSocketHomeClient::onRecievedAppTaskState(TcpSocketHomeClient_Tasks::State_t state) {
	m_stateIdle = (TcpSocketHomeClient_Tasks::STATE_IDLE == state ? true : false);
	m_stateAppTasks = state;


	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			pEntry->onStateChanged(this->CTCPSocketBase::getSocketIdentifier(),state);	

			if (!m_initDone) {			
				ITcpSocketHomeClient_Util::sendRequestNodeDbaseStatus(*this,getSocketMessageAPI());			
				m_initDone = true;
			}
		}
	}
	return TRUE;
}

BOOL CTCPSocketHomeClient::onRecievedNodedatabaseState(TcpSocketHomeClient_NodeDbase::Status_t status) {
	m_statusDbase = status;

	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			pEntry->onNodeDbaseStatus(this->CTCPSocketBase::getSocketIdentifier(), status);
		}
	}
	return TRUE;
}

// ICCommunicationConnectFsm_Host: Nodig voor de connect fsm. 
void CTCPSocketHomeClient::displayMsgError(const CString& msg) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			static const ITcpSocketHomeClient_Host::CategoryMsg_t category = ITcpSocketHomeClient_Host::HOMECLIENT_MSG_CATEGORY_CONNECT_FSM;
			pEntry->onSocketHomeClientMsgError(this->CTCPSocketBase::getSocketIdentifier(), category,  msg);
		}
	}
}

// Integratie DALI in TCP/IP protocol.
BOOL CTCPSocketHomeClient::onStatusDALIRxMsgSubscribe(BOOL status) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			pEntry->onStatusDALIRxMsgSubscribe(this->CTCPSocketBase::getSocketIdentifier(), (TRUE == status));
		}
	}
	return TRUE;
}

BOOL CTCPSocketHomeClient::onStatusDALIResponse(int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			pEntry->onStatusDALIResponse(this->CTCPSocketBase::getSocketIdentifier(), clientIdentifier, msgIdentifier, type, responseData);
		}
	}
	return TRUE;
}

BOOL CTCPSocketHomeClient::onRxDALIMessage(BYTE lengthBits, uint32_t recievedFrame) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			pEntry->onRxDALIMessage(this->CTCPSocketBase::getSocketIdentifier(), lengthBits, recievedFrame);
		}
	}
	return TRUE;
}

// Ontvangen bericht van de cloud server bridge.
BOOL CTCPSocketHomeClient::onRecievedTunnelLoginResponse(BOOL status) {
	return TRUE;	// Absorbe wordt enkel gebruikt bij het maken van een connectie.
}

void CTCPSocketHomeClient::displayMsgInfo(const CString& msg) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			static const ITcpSocketHomeClient_Host::CategoryMsg_t category = ITcpSocketHomeClient_Host::HOMECLIENT_MSG_CATEGORY_CONNECT_FSM;
			pEntry->onSocketHomeClientMsgInfo(this->CTCPSocketBase::getSocketIdentifier(),category, msg);
		}
	}
}

void CTCPSocketHomeClient::startTimer(unsigned int timerID,unsigned int value) {
	this->SetTimer(TIMER_ID_CONNECTFSM,value,NULL);
}

void CTCPSocketHomeClient::stopTimer(unsigned int timerID) {
	this->KillTimer(TIMER_ID_CONNECTFSM);
}

void CTCPSocketHomeClient::notifyFsmDone( ) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {	
			pEntry->onStateConnectChanged(this->CTCPSocketBase::getSocketIdentifier(),this->isConnected());
		}
	}
}

void CTCPSocketHomeClient::notifyFsmError( ) {
	POSITION pos = m_listObservers.GetHeadPosition();
	while(pos!=NULL)  {		
		ITcpSocketHomeClient_Host* pEntry = NULL;
		if ((pEntry=m_listObservers.GetNext(pos)) != NULL) {
			pEntry->onStateConnectChanged(this->CTCPSocketBase::getSocketIdentifier(),this->isConnected());
		}
	}
}

// Configuratie via de homeclient voor alle nodes 0x65 & 0x66.
// Bij de V65 nodes is het protocol enkel beschikbaar in de tcp/ip homeserver.
bool CTCPSocketHomeClient::checkFirmwareVersion(int softwareVersion) {
	const int major = ((softwareVersion & 0xff00) >> 8);
	return (major == 0x65 || major == 0x66);
/*
	bool result = false;
	const int major = ((softwareVersion & 0xff00) >> 8);
	const int minor = (softwareVersion & 0xFF);
	if (0x66 == major && minor >= 0x06) {
		result = true;
	}
	return result;
*/
}

// Configuratie enkel voor de master modules.
// V65: tcp/ip homeserver.
// V66: LCD en tcp/ip homeserver.
// V66: Smartbox ultra als een slave (zie socketOptions): Nodig voor de dali config.
bool CTCPSocketHomeClient::checkNodeProperties(int nodeAddress, unsigned long ulPhysicalAddress, int nodeType, int nodeFlags) {
	if ((NODE_PIN_GATEWAY == nodeType) || (NODE_LCD == nodeType)) {
		if (nodeFlags & NODE_MASTER_LCD) {
			return true;
		}

		// Sedert V16.38: Nodig voor de smartbox ultra DALI configuratie.
		const CTCPSocketOptions& refSocketOptions = this->getSocketOptions();
		if (refSocketOptions.getOptionLoginAllowSlaveConnect()) {
			return true;
		}
	}
	return false;
}
/*****************************************************************/