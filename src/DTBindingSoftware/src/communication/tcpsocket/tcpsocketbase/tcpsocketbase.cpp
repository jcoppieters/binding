#include "stdafx.h"
#include "tcpsocketbase.h"
#include "tcpsocketmessages_api.h"
#include "tcpsocketmsgqueue.h"
#include "communication/tcpsocket/itcpsocketcommobject.h"		// VOOR HET MOMENT HIER - TODO ????????????????
/*****************************************************************/
#define DEBUG_LOCAL_ON				1
#define DEBUG_LOCAL_ERRORS_ON		1
#define DEBUG_LOCAL_TX_ON			0
#define DEBUG_LOCAL_RX_ON			0
/*****************************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define TRACER(x)				TRACE(x)
	#define TRACER_1(x,y)			TRACE(x,y)
	#define TRACER_2(x,y,z)			TRACE2(x,y,z)			
#else
	#define TRACER(x)
	#define TRACER_1(x,y)		
	#define TRACER_2(x,y,z)		
#endif 
/*****************************************************************/
#if (DEBUG_LOCAL_TX_ON == 1)
	#define TRACER_TX(x,y)			TRACE1(x,y)
#else
	#define TRACER_TX(x,y)
#endif 
/*****************************************************************/
#if (DEBUG_LOCAL_RX_ON == 1)
	#define TRACER_RX(x,y)			TRACE1(x,y)
#else
	#define TRACER_RX(x,y)
#endif 
/*****************************************************************/
#if(DEBUG_LOCAL_ERRORS_ON == 1)
	#define TRACER_EC(x)			TRACE(x)
#else
	#define TRACER_EC(x)	
#endif 
/*****************************************************************/

/*****************************************************************/
static int _getSocketBaseIdentifier() {
	static int identifier = 0;
	return identifier++;
}
/*****************************************************************/



/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketBase::CTCPSocketBase(ITCPCommunicationObject_Host& refHost) : 
	m_identifier(_getSocketBaseIdentifier()),
	m_settings(), m_refHost(refHost), m_pObservers(new CSocketMsgObservers()), m_pSocketEvents(0), m_pSocket(0), m_pSocketMsg(0),
	m_pFsmConnectFunctions(0), m_socketConnectState(SOCKET_STATE_DISCONNECTED),
	m_socketOptions( )
{ 
	ASSERT(0 != m_pObservers);
}

CTCPSocketBase::~CTCPSocketBase( ) 
{ 
	if (0 != m_pSocket) {
		delete m_pSocket;
		m_pSocket = 0;
	}
	if (0 != m_pSocketEvents) {
		delete m_pSocketEvents;
		m_pSocketEvents = 0;
	}
	if (NULL != m_pSocketMsg) {
		delete m_pSocketMsg;
		m_pSocketMsg = NULL;
	}
	if (0 != m_pObservers) {
		delete m_pObservers;
		m_pObservers = 0;
	}
	if (0 != m_pFsmConnectFunctions) {
		delete m_pFsmConnectFunctions;
		m_pFsmConnectFunctions = 0;
	}
}

CString CTCPSocketBase::getStringState(SocketConnectState_t state) {
	switch(state) {
		case SOCKET_STATE_DISCONNECTED:
			return "SOCKET_STATE_DISCONNECTED";
		case SOCKET_STATE_WAIT_SOCKET_CONNECTED:
			return "SOCKET_STATE_WAIT_SOCKET_CONNECTED";
		case SOCKET_STATE_CONNECT_WAIT_LOGIN:
			return "SOCKET_STATE_CONNECT_WAIT_LOGIN";
		case SOCKET_STATE_CONNECT_LOGIN_SUCCESS:
			return "SOCKET_STATE_CONNECT_LOGIN_SUCCESS";
		default:
			return "UNDEFINED";
	}
}

void CTCPSocketBase::setSocketConnectState(SocketConnectState_t state) {
	if (state != m_socketConnectState) {
#if(DEBUG_LOCAL_ON == 1)
		TRACE1("CTCPSocketBase: socketConnectState='%s'\r\n",getStringState(state));
#endif 
		m_socketConnectState = state;
	}
}

/*****************************************************************/
BOOL CTCPSocketBase::initInstance() {
	ASSERT(0 == m_pSocket);
	ASSERT(0 == m_pSocketEvents);
	ASSERT(0 == m_pSocketMsg);
	ASSERT(0 == m_pFsmConnectFunctions);

	m_pSocketEvents = new CTCPSocketWinHost_Impl(*this);
	if (0 == m_pSocketEvents) {
		ASSERT(0 != m_pSocketEvents);
		return FALSE;
	}
	if (!createSocketWin(*m_pSocketEvents)) {
		ASSERT(0);
		return FALSE;
	}

	m_pSocketMsg = new CTCPSocketMessageAPI(*this,this->getObservers());
	if (0 == m_pSocketMsg) {
		ASSERT(0 != m_pSocketMsg);
		return FALSE;
	}

	m_pFsmConnectFunctions = new CTCPSocketBaseConnectFsmFunctions(*this);
	if (0 == m_pFsmConnectFunctions) {
		ASSERT(0 != m_pFsmConnectFunctions);
		return FALSE;
	}
	return TRUE;
}

BOOL CTCPSocketBase::socketConnected( ) {
	if (NULL == m_pSocket) {		
		return FALSE;
	}
	return m_pSocket->connected();
}

CTCPSocketWin::Result_t CTCPSocketBase::connectSocket(const char* strHostName,int socketport) {
	if (NULL == m_pSocket) {
		ASSERT(NULL != m_pSocket);
		return CTCPSocketWin::RESULT_EC_GENERAL;
	}
	return m_pSocket->connect(strHostName,socketport);
}

CTCPSocketWin::Result_t CTCPSocketBase::closeSocket() {
	if (NULL == m_pSocket) {
		ASSERT(NULL != m_pSocket);
		return CTCPSocketWin::RESULT_EC_GENERAL;
	}
	return m_pSocket->close();
}

CTCPSocketWin::Result_t CTCPSocketBase::sendSocketMsg(const BYTE* pbData,int length) {
	if (NULL == m_pSocket) {
		ASSERT(NULL != m_pSocket);
		return CTCPSocketWin::RESULT_EC_SOCKET;
	}

/*
#if(_DEBUG)
	TRACE("--> Send socket message '%s' (length=%d)\r\n",pbData,length);
#endif 
*/
	return m_pSocket->send(pbData,length);
}

BOOL CTCPSocketBase::getSocketErrorMsg(CString& errorMsg,CString& errorMsgDetail) {
	if (NULL == m_pSocket) {
		ASSERT(NULL != m_pSocket);
		return FALSE;	
	}
	errorMsg = m_pSocket->getErrorMsg();
	errorMsgDetail = m_pSocket->getErrorMsgDetail();
	return TRUE;
}

CTCPSocketMessageAPI& CTCPSocketBase::getSocketMessageAPI() {
	if (0 == m_pSocketMsg) {
		ASSERT(0 != m_pSocketMsg);
		exit(-1);
	}
	return *m_pSocketMsg;
}

CSocketMsgObservers& CTCPSocketBase::getObservers() {
	if (0 == m_pObservers) {
		ASSERT(0 != m_pObservers);
		exit(-1);
	}
	return *m_pObservers;
}

ITCPCommunicationObject_Host& CTCPSocketBase::getParentHost() {
	return m_refHost;
}

void CTCPSocketBase::SetConnect() {
	TRACE("CTCPSocketBase::SetConnect()\r\n");
	if (this->getSocketConnectState() == SOCKET_STATE_CONNECT_LOGIN_SUCCESS) {		
		ASSERT(this->socketConnected());
		return;
	}

	const int port = m_settings.getPort();
	const CString& hostName = m_settings.getHostName();
	ASSERT(port>0);
	ASSERT(!hostName.IsEmpty());	

	const CTCPSocketWin::Result_t res = this->connectSocket((LPCTSTR)hostName,port);
	if (CTCPSocketWin::RESULT_SUCCESS == res) {				
		this->setSocketConnectState(SOCKET_STATE_WAIT_SOCKET_CONNECTED);	
	}
	else {
		CString dwErrorMsg;
		CString dwErrorMsgDetail;
		if (this->getSocketErrorMsg(dwErrorMsg,dwErrorMsgDetail)) {
			TRACER_EC(dwErrorMsg);
			TRACER_EC(dwErrorMsgDetail);
			this->getParentHost().displayMsgError(this->getSocketIdentifier(),dwErrorMsg);
		}	
		this->setSocketConnectState(SOCKET_STATE_DISCONNECTED);
	}
}

void CTCPSocketBase::SetDisconnect() {
	TRACE("CTCPSocketBase::SetDisconnect()\r\n");
	if (!this->socketConnected()) {		
		this->setSocketConnectState(SOCKET_STATE_DISCONNECTED);
		return;
	}	
	const CTCPSocketWin::Result_t res = this->closeSocket();
	if (CTCPSocketWin::RESULT_SUCCESS != res) {
		CString dwErrorMsg;
		CString dwErrorMsgDetail;
		if (this->getSocketErrorMsg(dwErrorMsg,dwErrorMsgDetail)) {
			TRACER_EC(dwErrorMsg);
			TRACER_EC(dwErrorMsgDetail);		
		}	
	}
	this->setSocketConnectState(SOCKET_STATE_DISCONNECTED);	
}


BOOL CTCPSocketBase::send(const BYTE* msg,int length) {
	const CTCPSocketWin::Result_t res = this->sendSocketMsg(msg,length);
	if (CTCPSocketWin::RESULT_SUCCESS != res) {
		CString dwErrorMsg;
		CString dwErrorMsgDetail;
		if (this->getSocketErrorMsg(dwErrorMsg,dwErrorMsgDetail)) {
			TRACER_EC(dwErrorMsg);
			TRACER_EC(dwErrorMsgDetail);
			this->getParentHost().displayMsgError(this->getSocketIdentifier(),dwErrorMsg);
		}
			
		if (CTCPSocketWin::RESULT_EC_CONNECT == res) {
			// Sedert V16.07: zie NOTE_DISCONNECT_WINSOCK
			// De Host heeft de connectie gesloten.
			// Om zeker te zijn roepen we hier nog eens de close() functie aan.
			// Deze close() zorgt ervoor dat de applicate het socket disconnected 
			// event ontvangt.
			this->closeSocket();											
		}
	}
	return (CTCPSocketWin::RESULT_SUCCESS == res);
}

/*****************************************************************/
// protected...
BOOL CTCPSocketBase::createSocketWin(ITCPSocketWinHost& refHost) {
	if (0 != m_pSocket) {
		ASSERT(0 == m_pSocket);
		return FALSE;
	}

	m_pSocket = new CTCPSocketWin(refHost);
	if (0 == m_pSocket) {
		ASSERT(0 != m_pSocket);
		return FALSE;
	}

	m_pSocket->Create();

#if (DEBUG_LOCAL_TX_ON==1)
	m_pSocket->setTracerTxOnOff(TRUE);
#endif 
#if(DEBUG_LOCAL_RX_ON==1)
	m_pSocket->setTracerRxOnOff(TRUE);
#endif 
	return TRUE;
}

void CTCPSocketBase::subscribeObserver(ISocketMsgObserver* observer) {
	if (0 == m_pObservers) {
		ASSERT(0 != m_pObservers);
		return;
	}
	*m_pObservers += observer;
}
void CTCPSocketBase::unsubscribeObserver(ISocketMsgObserver* observer) {
	if (0 == m_pObservers) {
		ASSERT(0 != m_pObservers);
		return;
	}
	*m_pObservers -= observer;
}

SocketConnectState_t CTCPSocketBase::getSocketConnectState() const {
	return m_socketConnectState;
}

BOOL CTCPSocketBase::setCommunicationSettings(const CTCPSocketCommSettings& settings) {
	m_settings = settings;
	return TRUE;
}

ITcpSocketConnectFsmFunctions* CTCPSocketBase::temp_getTcpSocketConnectFsmFunctions() {
	return m_pFsmConnectFunctions;
}
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
ITcpSocketConnectFsmFunctions::~ITcpSocketConnectFsmFunctions() { }
/*****************************************************************/
CTCPSocketBaseConnectFsmFunctions::CTCPSocketBaseConnectFsmFunctions(CTCPSocketBase& parent) :
	m_parent(parent) { }
	
CTCPSocketBaseConnectFsmFunctions::~CTCPSocketBaseConnectFsmFunctions() {
	
}

BOOL CTCPSocketBaseConnectFsmFunctions::IsTcpConnectionOpen() {
	return m_parent.socketConnected();
}

BOOL CTCPSocketBaseConnectFsmFunctions::openTcpConnection() {
	m_parent.SetConnect();
	return TRUE;
}
BOOL CTCPSocketBaseConnectFsmFunctions::closeTcpConnection() {
	m_parent.SetDisconnect();
	return TRUE;
}
void CTCPSocketBaseConnectFsmFunctions::setStateTcpConnectLoginOn() {
	m_parent.setSocketConnectState(SOCKET_STATE_CONNECT_LOGIN_SUCCESS);
}
SocketConnectState_t CTCPSocketBaseConnectFsmFunctions::getTcpSocketConnectState() {
	return m_parent.getSocketConnectState();
}
void CTCPSocketBaseConnectFsmFunctions::subscribeMsgObserver(ISocketMsgObserver* observer) {
	m_parent.subscribeObserver(observer);
}
void CTCPSocketBaseConnectFsmFunctions::unsubscribeMsgObserver(ISocketMsgObserver* observer) {
	m_parent.unsubscribeObserver(observer);
}
const CTCPSocketCommSettings& CTCPSocketBaseConnectFsmFunctions::getTcpCommSettings() const {
	return m_parent.getCommunicationSettings();
}
void CTCPSocketBaseConnectFsmFunctions::setTcpCommSettings(const CTCPSocketCommSettings& settings) {
	m_parent.setCommunicationSettings(settings);
}

BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgRequestKey() {
	return m_parent.getSocketMessageAPI().sendLoginRequestKey();
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgConnect(const char* clientID,const char* password,int tokenNumber) {
	return m_parent.getSocketMessageAPI().sendConnect(clientID,password,tokenNumber);
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgConnect(const char* password) {
	return m_parent.getSocketMessageAPI().sendConnect(password);
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgDisconnect() {
	return m_parent.getSocketMessageAPI().sendDisconnect();
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgCanBusMsgSubscribe(BOOL state) {
	return m_parent.getSocketMessageAPI().sendCanBusMsgSubscribe(state);
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgDaliMsgSubscribe(BOOL state) {
	return m_parent.getSocketMessageAPI().sendDaliMsgSubscribe(state);
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgRequestNodeInfo() {
	return m_parent.getSocketMessageAPI().sendRequestNodeInfo();
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgRequestMasterSupported() {
	return m_parent.getSocketMessageAPI().sendRequestMasterSupported();
}
BOOL CTCPSocketBaseConnectFsmFunctions::sendMsgCfgAsMaster() {
	return m_parent.getSocketMessageAPI().sendCfgDeviceAsMaster();
}

// Cloud server login message.
BOOL CTCPSocketBaseConnectFsmFunctions::sendRawMessage(const char* msg) {		
	if (nullptr == msg) {
		REQUIRE(nullptr != msg);
		return FALSE;
	}
	const int length = strlen(msg);
	if (length <= 0) {
		ASSERT(length>0);
		return FALSE;
	}
	return m_parent.send((const BYTE*)msg,length);
}

BOOL CTCPSocketBaseConnectFsmFunctions::isUseCloudServer() const {
	return (CONNECTION_METHOD_REMOTE_CLOUD == m_parent.getCommunicationSettings().getConnectionMethod());
}

BOOL CTCPSocketBaseConnectFsmFunctions::sendCloudServerLoginToken() {
	const CString& msg = m_parent.getCommunicationSettings().getCloudServerToken();
	if (msg.IsEmpty()) {
		return FALSE;
	}
	return m_parent.send((const BYTE*)(const char*)msg, msg.GetLength());
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketWinHost_Impl::CTCPSocketWinHost_Impl(CTCPSocketBase& parent) : m_parent( parent ) { }
CTCPSocketWinHost_Impl::~CTCPSocketWinHost_Impl() { }

void CTCPSocketWinHost_Impl::onSocketConnected(BOOL status) {
	if (status) {
		if (SOCKET_STATE_WAIT_SOCKET_CONNECTED == m_parent.getSocketConnectState()) {					
			m_parent.setSocketConnectState(SOCKET_STATE_CONNECT_WAIT_LOGIN);
		}
		else {
			ASSERT(SOCKET_STATE_WAIT_SOCKET_CONNECTED != m_parent.getSocketConnectState());
		}
	}
	else {	
		m_parent.setSocketConnectState(SOCKET_STATE_DISCONNECTED);
		m_parent.getParentHost().onTcpSocketClosed(m_parent.getSocketIdentifier());
	}
}

void CTCPSocketWinHost_Impl::onDataRecieved(const BYTE* pbData,int length) {
	ASSERT(SOCKET_STATE_DISCONNECTED != m_parent.getSocketConnectState());
	if (NULL == pbData) {	
		ASSERT(NULL != pbData);	
		return;
	}

	const ConnectionMethod_t method = m_parent.getCommunicationSettings().getConnectionMethod();
	m_parent.getSocketMessageAPI().recieveMessage(method,pbData,length);
}

void CTCPSocketWinHost_Impl::onMsgInfo(const CString& str) {
	m_parent.getParentHost().displayMsgInfo(m_parent.getSocketIdentifier(),str);
}

void CTCPSocketWinHost_Impl::onMsgError(const CString& str) {
	m_parent.getParentHost().displayMsgError(m_parent.getSocketIdentifier(),str);
}

void CTCPSocketWinHost_Impl::onTracerDataSend(const CString& msg) {
	TRACER_TX("TX:%s",msg);		
}
void CTCPSocketWinHost_Impl::onTracerDataRecieved(const CString& msg) {
	TRACER_RX("RX:%s",msg);	
}

// zie NOTE_DISCONNECT_WINSOCK en NOTE_WINSOCK_KEEPALIVE.
// Wanneer de socket send() mislukt omdat de socket gesloten is dan krijgen we een socket error 
// en wordt de socket gesloten. De applicatie kan het NotifyConnectStatusChanged( ) event implementeren.
//
// Opmerking: Duurt een aantal seconden na het mislukken van de send( ) dat we een FD_CLOSE event krijgen
// van windows OS.
void CTCPSocketWinHost_Impl::onRequestSendKeepAlive( ) {
	m_parent.getSocketMessageAPI().sendHeartbeatRequest();
}
/*****************************************************************/