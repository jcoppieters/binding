#include "stdafx.h"
#include "tcpsocketconnectfsm.h"
#include "tcpsocketmessages.h"
#include "tcpsocketmessages_api.h"
#include "MyWindowTimers.h"							// Applicatie	- Tijdelijk					--> TODO OPKUISEN.
#include "nodegenestructs.h"						// Applicatie - nodig voor het node type...
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
#ifdef _DEBUG
	#define DEBUG_LOCAL_ON			1
#else
	#define DEBUG_LOCAL_ON			0
#endif 
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
enum {
	TIMEOUT_WAIT_FOR_SOCKET_CONNECTION_SEC = 60,		// 60 sec
	TIMEOUT_WAIT_FOR_RESPONSE_SEC = 10,					// 10 sec
	TIMEOUT_WAIT_FOR_CLOUDSERVER_RESPONSE_SEC = 10		// Tijd die er gewacht wordt op een response van de cloud server na het versturen van de unieke id (=login token)
};
/*****************************************************************/
CTCPSocketConnectFsm::CTCPSocketConnectFsm(ICCommunicationConnectFsm_Host& refHost ,ITcpSocketConnectFsmFunctions* pCommObject,CCommunicationServerInfo& refServerInfo) : 
m_refHost(refHost) , m_pCommObject(pCommObject) , m_refServerInfo(refServerInfo),
	m_State(STATE_DISCONNECTED), m_timerValue(0) , m_timerReloadValue(TIMEOUT_WAIT_FOR_SOCKET_CONNECTION_SEC) {
	ASSERT(NULL != pCommObject);
	m_pCommObject->subscribeMsgObserver(this);
}

CTCPSocketConnectFsm::~CTCPSocketConnectFsm()  { 
	m_pCommObject->unsubscribeMsgObserver(this);	
}

void CTCPSocketConnectFsm::EvOperationConnect() 
{
	if (NULL == m_pCommObject) {
		ASSERT(NULL != m_pCommObject);
		return;
	}

	if (m_pCommObject->IsTcpConnectionOpen()) {		
		// TODO: Zijn ingelogd bij de server. 
		// Opnieuw de server info opvragen en controle doen of het ok is.
		startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
		this->setState(STATE_CONNECTED);
		return;
	}
	if (m_pCommObject->openTcpConnection()) {	
		startTimer(TIMEOUT_WAIT_FOR_SOCKET_CONNECTION_SEC);
		this->setState(STATE_CONNECT_WAIT_SOCKETCONNECT);
	}
	else{
		// Bij de timer expiry zal de FSM error naar de parent versturen.
		this->setState(STATE_CONNECT_ERROR);	
		startTimer(1);
	}
}

void CTCPSocketConnectFsm::notifyErrorTimeout() {
	switch(getState()) {
		case STATE_WAIT_FOR_RESPONSE_TOKEN: {
			// Er is een socket connectie met de server maar het login commando wordt niet ondersteund.
			// Mogelijk omdat we connectie maken met oude firmware of een verkeerd node type (Smartbox+)
			const CString msg("Connect failed - No login response from the server. Connection not supported by the server. Wrong tcp-port? Check server maybe upgrade firmware?");
			m_refHost.displayMsgError(msg);
		} break;
		case STATE_CONNECT_WAIT_SOCKETCONNECT: {
			const CString msg("Connect failed - Could not connect to the server.");
			m_refHost.displayMsgError(msg);											   
		} break;
		case STATE_CLOUDSERVER_WAIT_LOGIN_RESPONSE: {
			const CString msg("Connect failed - No response from the cloud server.");
			m_refHost.displayMsgError(msg);
		} break;
		default: {
			const CString msg("Connect failed - No response from the server. ");
			m_refHost.displayMsgError(msg);				 
		} break;
	}
}

CString CTCPSocketConnectFsm::getString(State_t state) {
	switch(state) {
		case STATE_DISCONNECTED:
			return "STATE_DISCONNECTED";
		case STATE_CONNECT_WAIT_SOCKETCONNECT:
			return "STATE_CONNECT_WAIT_SOCKETCONNECT";
		case STATE_CLOUDSERVER_WAIT_LOGIN_RESPONSE:
			return "STATE_CLOUDSERVER_WAIT_LOGIN_RESPONSE";
		case STATE_WAIT_FOR_RESPONSE_TOKEN:
			return "STATE_WAIT_FOR_RESPONSE_TOKEN";
		case STATE_WAIT_FOR_RESPONSE_CONNECT:
			return "STATE_WAIT_FOR_RESPONSE_CONNECT";
		case STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_CANMSG:
			return "STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_CANMSG";
		case STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_DALIMSG:
			return "STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_DALIMSG";
		case STATE_WAIT_FOR_RESPONSE_NODEINFO:
			return "STATE_WAIT_FOR_RESPONSE_NODEINFO";
		case STATE_WAIT_FOR_RESPONSE_MASTERSUPPORTED:
			return "STATE_WAIT_FOR_RESPONSE_MASTERSUPPORTED";
		case STATE_CONNECT_ERROR:
			return "STATE_CONNECT_ERROR";
		case STATE_CONNECTED:
			return "STATE_CONNECTED";
		default:
			return "UNDEFINED";
	}
}

void CTCPSocketConnectFsm::setState(State_t state) {
	if (state != m_State) {
#if(DEBUG_LOCAL_ON == 1)
		TRACE1("CTCPSocketConnectFsm --> new state='%s'\r\n",getString(state));
#endif
		m_State = state;
	}
}

BOOL CTCPSocketConnectFsm::taskRequestLoginKey( ) {
	if (m_pCommObject->sendMsgRequestKey()) {	
		const CString msg("Connect - send login request to the server.");
		m_refHost.displayMsgInfo(msg);
		startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
		this->setState(STATE_WAIT_FOR_RESPONSE_TOKEN);				
		return TRUE;
	}
				
	{
		const CString msg("Connect failed - Failed to send login request to the server.");
		m_refHost.displayMsgError(msg);
	}

	this->setState(STATE_CONNECT_ERROR);
	startTimer(1);					
	return FALSE;
}

// Niet beveiligde login
BOOL CTCPSocketConnectFsm::taskSendLogin() {

	const CTCPSocketCommSettings& refSettings = m_pCommObject->getTcpCommSettings();
	const CString& pwd = refSettings.getPassword();
	if (pwd.IsEmpty()) {
		ASSERT(!pwd.IsEmpty());			
		const CString msg("Connect failed - Cannot connect to the server. Internal error - password is empty");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
		return TRUE;
	}

	if (m_pCommObject->sendMsgConnect((LPCSTR) pwd)) {	
		const CString msg("Connect - send login to the server.");
		m_refHost.displayMsgInfo(msg);
		startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
		this->setState(STATE_WAIT_FOR_RESPONSE_CONNECT);				
		return TRUE;
	}

	{
		const CString msg("Connect failed - Failed to send login to the server.");
		m_refHost.displayMsgError(msg);
	}

	this->setState(STATE_CONNECT_ERROR);
	startTimer(1);					
	return FALSE;
}

BOOL CTCPSocketConnectFsm::taskRequestNodeInfo( ) {
	// Opvragen van de server node info.
	if (!m_pCommObject->sendMsgRequestNodeInfo()) {
		const CString msg("Connect failed - Failed to request server node info.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
		return TRUE;
	}

	{
		// Hier een info bericht sturen...
		const CString msg("Connect - Request server node info.");
		m_refHost.displayMsgInfo(msg);
	}
	startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
	this->setState(STATE_WAIT_FOR_RESPONSE_NODEINFO);
	return TRUE;
}

BOOL CTCPSocketConnectFsm::taskRequestMasterSupported( ) {
	
	if (!m_pCommObject->sendMsgRequestMasterSupported()) {
		const CString msg("Connect failed - Failed to request if the server is supported to be set as master.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
		return TRUE;
	}

	{
		// Hier een info bericht sturen...
		const CString msg("Connect - Request if server is supported to be set as master.");
		m_refHost.displayMsgInfo(msg);
	}

	startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
	this->setState(STATE_WAIT_FOR_RESPONSE_MASTERSUPPORTED);
	return TRUE;
}

// Sedert V17.03: Cloud server login
BOOL CTCPSocketConnectFsm::taskSendCloudServerIdentifier() {
	if (!m_pCommObject->sendCloudServerLoginToken( )) {
		const CString msg("Connect failed - Failed to send the login token to the cloud server");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
		return TRUE;
	}
	{
		// Hier een info bericht sturen...
		const CString msg("Connect - Send cloudserver login token.");
		m_refHost.displayMsgInfo(msg);
	}		
	this->setState(STATE_CLOUDSERVER_WAIT_LOGIN_RESPONSE);
	startTimer(TIMEOUT_WAIT_FOR_CLOUDSERVER_RESPONSE_SEC);
	return TRUE;
}


BOOL CTCPSocketConnectFsm::taskSubscribeCanRxMsg( ) {
	if (!m_pCommObject->sendMsgCanBusMsgSubscribe(TRUE)) {
		const CString msg("Connect failed - Failed to send subscribe message to the server.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
		return TRUE;
	}

	{
		// Hier een info bericht sturen...
		const CString msg("Connect - Subscribe for CAN messages.");
		m_refHost.displayMsgInfo(msg);
	}
	startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
	this->setState(STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_CANMSG);
	return TRUE;
}

BOOL CTCPSocketConnectFsm::taskSubscribeDALIRxMsg( ) {

	if (!m_pCommObject->sendMsgDaliMsgSubscribe(TRUE)) {
		const CString msg("Connect failed - Failed to send subscribe DALI message to the server.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
		return TRUE;
	}

	{
		// Hier een info bericht sturen...
		const CString msg("Connect - Subscribe for DALI messages.");
		m_refHost.displayMsgInfo(msg);
	}
	startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
	this->setState(STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_DALIMSG);
	return TRUE;
}


void CTCPSocketConnectFsm::EvAfterTimeout() {
	stopTimer();

	if (NULL == m_pCommObject) {
		ASSERT(NULL != m_pCommObject);
		m_refHost.notifyFsmError();
		return;
	}

	switch(getState()) {
		case STATE_CONNECTED: {
			// Login is ok.
			m_pCommObject->setStateTcpConnectLoginOn();					
			m_refHost.notifyFsmDone();							  
			return;
		} break;

		case STATE_CONNECT_ERROR: {
			m_refHost.notifyFsmError();	
			return;
		} break;

		case STATE_CONNECT_WAIT_SOCKETCONNECT: {
			const SocketConnectState_t socketState = m_pCommObject->getTcpSocketConnectState();
			if (SOCKET_STATE_CONNECT_WAIT_LOGIN == socketState) {		
				if (m_pCommObject->isUseCloudServer()) {
					taskSendCloudServerIdentifier( );		// transitie naar de toestand STATE_CLOUDSERVER_WAIT_LOGIN_RESPONSE
					return;
				}
				else {
					// De socket is geconnecteerd. Kunnen nu beginnen met de fsm.			
					// Secure login of gewone (oude) login methode.
					const BOOL secureLoging = m_pCommObject->getCfgOptionSecureLogin();
					if (secureLoging) {
						taskRequestLoginKey( );
					}
					else {
						taskSendLogin();
					}
					return;
				}
			}
			else if (SOCKET_STATE_WAIT_SOCKET_CONNECTED == socketState) {
				if (++m_timerValue >= m_timerReloadValue) {
					notifyErrorTimeout();
					this->setState(STATE_CONNECT_ERROR);
					startTimer(1);
					return;		
				}
			}
			else if (SOCKET_STATE_DISCONNECTED == socketState) {
				notifyErrorTimeout();
				this->setState(STATE_CONNECT_ERROR);
				startTimer(1);
				return;		
			}
		} break;

		// Sedert V17.03: CHANGES_CLOUDBRIDGE_POC
		// Er werd geen antwoord ontvangen op het login bericht die verstuurd werd naar de cloud server.
		case STATE_CLOUDSERVER_WAIT_LOGIN_RESPONSE: {		
			const SocketConnectState_t socketState = m_pCommObject->getTcpSocketConnectState();
			if (SOCKET_STATE_DISCONNECTED == socketState) {
				notifyErrorTimeout();
				this->setState(STATE_CONNECT_ERROR);
				startTimer(1);
				return;
			}			

			if (++m_timerValue >= m_timerReloadValue) {
				m_pCommObject->closeTcpConnection();
				notifyErrorTimeout();
				this->setState(STATE_CONNECT_ERROR);
				startTimer(1);
				return;								
			}
		} break;

		default: {
			if (++m_timerValue >= m_timerReloadValue) {
				notifyErrorTimeout();
				this->setState(STATE_CONNECT_ERROR);
				startTimer(1);
				return;		
			}
		} break;
	}
	startTimer();
}

void CTCPSocketConnectFsm::startTimer(int reloadValue) {	
	if (reloadValue>0) {
		m_timerReloadValue = reloadValue;		
	}
	m_refHost.startTimer( TIMERID_MODEMCONNECT_TIMEOUT, 1000);	
}

void CTCPSocketConnectFsm::stopTimer() {
	m_refHost.stopTimer( TIMERID_MODEMCONNECT_TIMEOUT);	
}
/*****************************************************************/
// Antwoord van de server op een request key...
BOOL CTCPSocketConnectFsm::onStatusConnectRequestKey(unsigned short protocolVersion,
	unsigned short softwareVersion,int tokenNumber,const char* szClientKey)
{
	if (STATE_WAIT_FOR_RESPONSE_TOKEN == getState()) { 

		// Server info: software & protocol versie...
		m_refServerInfo.setCommunicationType(CCommunicationServerInfo::TCPIP);
		m_refServerInfo.setProtocolVersion(protocolVersion);
		m_refServerInfo.setSoftwareVersion(softwareVersion);
		m_refServerInfo.setModemCanLogSupported(false);

		if (false == m_refHost.checkFirmwareVersion(softwareVersion)) {
			const CString msg("Connect failed - Cannot connect to the server. Upgrade the firmware.");
			m_refHost.displayMsgError(msg);
			this->setState(STATE_CONNECT_ERROR);
			startTimer(1);
			return TRUE;
		}	

		const CTCPSocketCommSettings& refSettings = m_pCommObject->getTcpCommSettings();
		const CString& pwd = refSettings.getPassword();
		if (pwd.IsEmpty() || NULL == szClientKey) {
			ASSERT(!pwd.IsEmpty());
			ASSERT(NULL != szClientKey);
			const CString msg("Connect failed - Cannot connect to the server. Internal error");
			m_refHost.displayMsgError(msg);
			this->setState(STATE_CONNECT_ERROR);
			startTimer(1);
			return TRUE;
		}

		if (!m_pCommObject->sendMsgConnect(szClientKey,pwd,tokenNumber)) {
			const CString msg("Connect failed - Failed to send connect message to the server.");
			m_refHost.displayMsgError(msg);
			this->setState(STATE_CONNECT_ERROR);
			startTimer(1);
			return TRUE;
		}
		
		{
			// Hier een info bericht sturen...
			const CString msg("Connect - Send server login credentials.");
			m_refHost.displayMsgInfo(msg);
		}
		startTimer(TIMEOUT_WAIT_FOR_RESPONSE_SEC);
		this->setState(STATE_WAIT_FOR_RESPONSE_CONNECT);
		return TRUE;						
	}
	else {		
		const CString msg("Connect failed - internal error - wrong state.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
	}
	return TRUE;
}


// Antwoord passwoord ok.
BOOL CTCPSocketConnectFsm::onStatusConnectPassword(BOOL stateConnected) {
	if (STATE_WAIT_FOR_RESPONSE_CONNECT == getState()) {
		if (stateConnected) {			
			{
				// Hier een info bericht sturen...
				const CString msg("Connect - server replied: credentials ok.");
				m_refHost.displayMsgInfo(msg);
			}

			// Om de configuratie te doen moeten we subscriben om CAN-Berichten te ontvangen.
			// Bij de gewone hoemclient functies is dit niet nodig.
			if (m_pCommObject->getCfgOptionSubscribeCanMsg()) {				
				return taskSubscribeCanRxMsg();				
			}
			else {				
				// Sedert V16.36: Integratie van DALI berichten...
				if (m_pCommObject->getCfgOptionSubscribeDaliMsg()) {
					return this->taskSubscribeDALIRxMsg( );
				}
				else {
					return this->taskRequestNodeInfo();									
				}
			}
		}
		else {		
			const CString msg("Connect failed - wrong password!");
			m_refHost.displayMsgError(msg);
			this->setState(STATE_CONNECT_ERROR);
			startTimer(1);
			return TRUE;
		}		
	}
	else {		
		ASSERT(STATE_WAIT_FOR_RESPONSE_CONNECT != getState());
		const CString msg("Connect failed - internal error - wrong state.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);		
		startTimer(1);
	}
	return TRUE;
}

// Antwoord op een heartbeat request.
BOOL CTCPSocketConnectFsm::onStatusHeartbeat() { return TRUE; }

// Antwoord op een CAN-RX Msg subsribe.
BOOL CTCPSocketConnectFsm::onStatusCanRxMsgSubscribe(BOOL status) {
	if (STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_CANMSG == getState()) {
		if (status) {			
			// Sedert V16.36: Integratie van DALI berichten...
			if (m_pCommObject->getCfgOptionSubscribeDaliMsg()) {
				return this->taskSubscribeDALIRxMsg( );
			}
			else {				
				// Hier een info bericht sturen...
				const CString msg("Connect - server replied: credentials ok.");
				m_refHost.displayMsgInfo(msg);				
				return taskRequestNodeInfo();
			}
		}
		else{
			const CString msg("Connect failed - could not subscribe to the server.");
			m_refHost.displayMsgError(msg);

			// Proberen om een clean disconnect te doen...
			// Indien deze mislukt dan een gewone socket close()
			if (!m_pCommObject->sendMsgDisconnect()) {
				m_pCommObject->closeTcpConnection();
			}

			this->setState(STATE_CONNECT_ERROR);
			startTimer(1);
		}
		return TRUE;
	}
	else {		
		ASSERT(STATE_WAIT_FOR_RESPONSE_CONNECT != getState());
		const CString msg("Connect failed - internal error - wrong state.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);		
		startTimer(1);
	}
	return TRUE;
}

// Antwoord op een DALI-RX Msg subsribe.
BOOL CTCPSocketConnectFsm::onStatusDALIRxMsgSubscribe(BOOL status) {

	if (STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_DALIMSG == getState()) {
		if (status) {			
			const CString msg("Connect - server replied: credentials ok.");
			m_refHost.displayMsgInfo(msg);			
			return taskRequestNodeInfo();
		}
		else{
			const CString msg("Connect failed - could not subscribe for DALI messages.");
			m_refHost.displayMsgError(msg);

			// Proberen om een clean disconnect te doen...
			// Indien deze mislukt dan een gewone socket close()
			if (!m_pCommObject->sendMsgDisconnect()) {
				m_pCommObject->closeTcpConnection();
			}
			this->setState(STATE_CONNECT_ERROR);
			startTimer(1);
		}
		return TRUE;
	}
	else {
		ASSERT(STATE_WAIT_FOR_RESPONSE_CONNECT != getState());
		const CString msg("Connect failed - internal error - wrong state.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);		
		startTimer(1);
	}
	return TRUE;
}



// Ontvangen van een CAN-RX msg.
BOOL CTCPSocketConnectFsm::onRecievedCanRxMsg(int nodeAddress, int unitAddress,
		int messageCode, int length, const BYTE* pbData) {
	return TRUE;
}

// Ontvangen van server node info.
BOOL CTCPSocketConnectFsm::onRecievedServerNodeInfo(int nodeAddress,unsigned long ulPhysicalAddress,const char* name,
							  int numberOfUnits,int nodeType,int nodeFlags) 
{
	if (STATE_WAIT_FOR_RESPONSE_NODEINFO == getState()) {

		{   // Hier een info bericht sturen...
			const CString msg("Connect - server replied with node info.");
			m_refHost.displayMsgInfo(msg);		
		}

		// Properties controleren.
		CCommunicationServerInfo::NodeType_t type = CCommunicationServerInfo::NODE_UNKNOWN;
		if (NODE_PIN_GATEWAY == nodeType) {
			type = CCommunicationServerInfo::NODE_HOMESERVER;
		}
		else if (NODE_LCD == nodeType) {
			type = CCommunicationServerInfo::NODE_TOUCHSCREEN;
		}

 		if (false == m_refHost.checkNodeProperties(nodeAddress,ulPhysicalAddress,nodeType,nodeFlags)) {

			const CString msg("Connect failed -Not supported by the server.");
			m_refHost.displayMsgError(msg);

			// Proberen om een clean disconnect te doen...
			// Indien deze mislukt dan een gewone socket close()
			if (!m_pCommObject->sendMsgDisconnect()) {
				m_pCommObject->closeTcpConnection();
			}

			this->setState(STATE_CONNECT_ERROR);		
			startTimer(1);
			return TRUE;
		}

		// Properties invullen...
		m_refServerInfo.setNodeAddress(nodeAddress);
		m_refServerInfo.setPhysicalAddress(ulPhysicalAddress);	
		m_refServerInfo.setNodeType(type);		
		if (NULL != name) {
			const CString strName(name);
			m_refServerInfo.setNodeName(strName);
		}
		m_refServerInfo.setServerMasterNode((NODE_MASTER_LCD == (nodeFlags & NODE_MASTER_LCD)));		

		// Sedert V16.26: Extra toestand.
		return taskRequestMasterSupported( );
	}
	else {
		ASSERT(STATE_WAIT_FOR_RESPONSE_NODEINFO != getState());
		const CString msg("Connect failed - internal error - wrong state.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);		
		startTimer(1);
	}
	return TRUE;
}


BOOL CTCPSocketConnectFsm::onReceivedServerNodeStatusMaster(BOOL masterNode) {
	// Deze info kan ook bepaald worden mbv de node vlaggen.
	// Zie status onRecievedServerNodeInfo()
	return FALSE;
}

// Sedert V16.26
BOOL CTCPSocketConnectFsm::onReceivedServerNodeStatusMasterSupported(BOOL masterSupported) {

	if (STATE_WAIT_FOR_RESPONSE_MASTERSUPPORTED == getState()) {
		m_refServerInfo.setServerMasterNodeSupported((TRUE == masterSupported));
		this->setState(STATE_CONNECTED);
		startTimer(1);
		return TRUE;
	}
	else {
		ASSERT(STATE_WAIT_FOR_RESPONSE_MASTERSUPPORTED != getState());
		const CString msg("Connect failed - internal error - wrong state.");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);		
		startTimer(1);
	}
	return TRUE;
}

BOOL CTCPSocketConnectFsm::onReceivedServerNodeMasterSet(BOOL success) {
	return FALSE;
}

/******************************************************************************************/
// Ontvangen bericht van de cloud server bridge.
BOOL CTCPSocketConnectFsm::onRecievedTunnelLoginResponse(BOOL status) {
	stopTimer();
	if (status) {
		// Connectie met de cloud server is ok.	
		// Secure login of gewone (oude) login methode.
		const SocketConnectState_t socketState = m_pCommObject->getTcpSocketConnectState();
		ASSERT(SOCKET_STATE_CONNECT_WAIT_LOGIN == socketState);
		const BOOL secureLoging = m_pCommObject->getCfgOptionSecureLogin();
		if (secureLoging) {
			taskRequestLoginKey();
		}
		else {
			taskSendLogin();
		}	
	}
	else {
		const CString msg("Connect failed - The cloud server refused the connection");
		m_refHost.displayMsgError(msg);
		this->setState(STATE_CONNECT_ERROR);
		startTimer(1);
		return TRUE;
	}
	return TRUE;
}
/***************************************************************************************************/