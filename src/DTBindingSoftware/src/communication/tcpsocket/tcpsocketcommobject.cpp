#include "stdafx.h"
#include "tcpsocketcommobject.h"
#include "tcpsocketbase/tcpsocketmsgqueue.h"
#include "tcpsocketbase/tcpsocketcommframe.h"
#include "tcpsocketbase/tcpsocketmsgformatter.h"
#include "tcpsocketbase/tcpsocketmessages.h"
#include "tcpsocketbase/tcpsocketmessages_api.h"
#include "tcpsocketmsgapduheader.h"
#include "nodegenestructs.h"				// NODE_MASTER_LCD flag 
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
#define DEBUG_LOCAL_ON				1
#define DEBUG_LOCAL_ERRORS_ON		1
#define DEBUG_LOCAL_TX_ON			1
#define DEBUG_LOCAL_RX_ON			1
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

#if(USE_CHANGES_CHECK_DUPLICATES == 1)
static unsigned long long GetTimestampInMilliseconds() {
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);

	// Combineer high en low delen handmatig
	unsigned long long high = static_cast<unsigned long long>(ft.dwHighDateTime);
	unsigned long long low  = static_cast<unsigned long long>(ft.dwLowDateTime);
	unsigned long long timestamp = (high << 32) | low;

	// FILETIME is in 100-nanoseconden sinds 1601, dus delen door 10.000 = ms
	return timestamp / 10000;
}
#endif

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketCommObject::CTCPSocketCommObject(ITCPCommunicationObject_Host& refHost) : 
	CTCPSocketBase(refHost), 
	m_pObserver(NULL), m_pCurrentRxFrame(new CTCPSocketCommFrame()), m_rxFramesCanMessages(new CTCPSocketMsgQueue(MAX_RX_FRAMES)),
	m_currentTxFrame()
#if(USE_CHANGES_CHECK_DUPLICATES == 1)
	,m_previousRxFrame(), m_prevRxTimestamp(0)
#endif
{ 
	ASSERT(NULL != m_pCurrentRxFrame);	
	ASSERT(NULL != m_rxFramesCanMessages);

	m_pObserver = new CSocketMsgObserver_Impl(*this);
	ASSERT(NULL != m_pObserver);
}

CTCPSocketCommObject::~CTCPSocketCommObject( ) {	
	if (NULL != m_pObserver) {
		delete m_pObserver;
		m_pObserver = NULL;
	}	
	if (NULL != m_pCurrentRxFrame) {
		delete m_pCurrentRxFrame;
		m_pCurrentRxFrame = NULL;
	}
	if (NULL != m_rxFramesCanMessages) {
		delete m_rxFramesCanMessages;
		m_rxFramesCanMessages = NULL;
	}
}

void CTCPSocketCommObject::Init(HWND hParent) { 

	CTCPSocketOptions socketOptions;
	socketOptions.setOptionLoginSecure(TRUE);				// Login secure - Sedert V66.08
	socketOptions.setOptionLoginSubscribeCanMsg(TRUE);		// Nodig voor de configuratie.
	this->setSocketOptions(socketOptions);

	this->initInstance();
	
	// Observer koppelen om events van de ontvangen berichten.
	ASSERT(NULL != m_pObserver);
	this->subscribeObserver(m_pObserver);
}

BOOL CTCPSocketCommObject::IsConnectionOpen(void) {
	return (SOCKET_STATE_CONNECT_LOGIN_SUCCESS == this->getSocketConnectState());
}

BOOL CTCPSocketCommObject::OpenConnection() {
	SetConnect();
	return TRUE;
}

BOOL CTCPSocketCommObject::CloseConnection() {
	SetDisconnect();
	return FALSE;
}

BOOL CTCPSocketCommObject::setSettings(const CTCPSocketCommSettings& settings) {
	return this->setCommunicationSettings(settings);
}

// Toevoegen aan de Queue.
// Event geven aan pCommApplication.
BOOL CTCPSocketCommObject::onRxNewFrame(CTCPSocketCommFrame* pFrame) {	
	if (this->addRxCanMsgFrame(pFrame)) {	
		this->getParentHost().onTcpReceivedMessage(this->getSocketIdentifier());	
		return TRUE;
	}	
	return FALSE;		
}


// Deze wordt aangreoepen doot pCommApplication na het krijgen van een event.
// Updaten van de hudige modemframe en deze uit de queue verwijderen...
BOOL CTCPSocketCommObject::GetRXMsgFromQueue(void) {	
	if (NULL == m_pCurrentRxFrame) {
		ASSERT(NULL != m_pCurrentRxFrame);
		return FALSE;
	}	
	if (this->getRxCanMsgFrame(m_pCurrentRxFrame)) {
		return TRUE;
	}
	return FALSE;
}

const ICommunicationFrame* CTCPSocketCommObject::GetRxModemFrame() const {	
	ASSERT(NULL != m_pCurrentRxFrame);
	return m_pCurrentRxFrame;	
}

ITcpSocketConnectFsmFunctions* CTCPSocketCommObject::getTcpSocketConnectFsmFunctions() {
	return this->temp_getTcpSocketConnectFsmFunctions();
}

static int A2H(char ch)
{
	if ((ch >= '0') && (ch <= '9')) return(ch-48);
	else if ((ch >= 'A') && (ch <= 'F')) return(ch-55);
	else if ((ch >= 'a') && (ch <= 'f')) return(ch-87);
	return(0);
}

BYTE CTCPSocketCommObject::MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
										   BYTE bLength,const CString& szData) 
{
	if (SOCKET_STATE_CONNECT_LOGIN_SUCCESS != this->getSocketConnectState()) {
		TRACER("CTCPSocketCommObject::MessageTransfer enter: State not connected - return 0\r\n");
		return 0;	
	}
	const int stringDataLength = szData.GetLength();
	if ((bLength*2)>stringDataLength) {
		ASSERT((bLength*2)<=stringDataLength);
		return 0;
	}

	BYTE data[256];
	memset(data,0,sizeof(data));	
	for (int i=0,j=0;i<bLength;i++) {
		data[i] = A2H(szData.GetAt(j++))<<4; 	
		data[i] += A2H(szData.GetAt(j++)); 
	}

	const int header = getADPUHeader(bNodeAddress,bUnitAddress,bMsgCode,bLength,data);
	if (header<0) {
		ASSERT(0);	
		return FALSE;
	}
	const BOOL result = getSocketMessageAPI().sendCanBusMsg(bNodeAddress,bUnitAddress,header,bMsgCode,bLength,data);		
	if (result) {	
		// Set Last send frame.
		m_currentTxFrame = CTCPSocketCommFrame(bMsgCode,bNodeAddress,bUnitAddress,bLength,data,CTCPSocketCommFrame::WRITE);
		this->getParentHost().onTcpMessageSend(this->getSocketIdentifier(),&m_currentTxFrame);
	}
	else {
		CString dwErrorMsg;
		CString dwErrorMsgDetail;
		if (this->getSocketErrorMsg(dwErrorMsg,dwErrorMsgDetail)) {
			TRACER_EC(dwErrorMsg);
			TRACER_EC(dwErrorMsgDetail);
			this->getParentHost().displayMsgError(this->getSocketIdentifier(),dwErrorMsg);
		}
	}
	return (TRUE == result ? 1 : 0);	
}

#if(USE_CHANGES_CHECK_DUPLICATES == 1)
BOOL CTCPSocketCommObject::checkDuplicate(CTCPSocketCommFrame* frame, unsigned long long timeStamp) {
	if (NULL == frame) {
		REQUIRE(NULL != frame);
		return FALSE;
	}
	if (!(*frame == this->m_previousRxFrame)) {
		return FALSE;
	}	
	// Om te testen nog geen controle op de timestamp...
	return TRUE;
}
#endif 

BOOL CTCPSocketCommObject::addRxCanMsgFrame(CTCPSocketCommFrame* frame) {
	if (0 == m_rxFramesCanMessages || 0 == frame) {
		ASSERT(0 != m_rxFramesCanMessages);
		ASSERT(0 != frame);
		return FALSE;
	}

#if(USE_CHANGES_CHECK_DUPLICATES == 1)
	BOOL duplicateMessage = FALSE;
	const unsigned long long timeStamp = GetTimestampInMilliseconds();
	if (checkDuplicate(frame,timeStamp)) {
		enum { DUPLICATE_FRAMES_THRESSHOLD_DIFFERENCE_MSEC = 10 };
		unsigned long long diff = timeStamp - this->m_prevRxTimestamp;		
		if (diff < DUPLICATE_FRAMES_THRESSHOLD_DIFFERENCE_MSEC) {		
			duplicateMessage = TRUE;
		}
		CString str;
		str.Format("-----------> Detected duplicated frame: time between=%d msec.",(int) diff);
		if (duplicateMessage) {
			str += ("IGNORE MESSAGE");
		}
		str+="\r\n";
		TRACE(str);
	}
	this->m_previousRxFrame = *frame;
	this->m_prevRxTimestamp = timeStamp;

	if (duplicateMessage) {	
		return FALSE;
	}
#endif

	return m_rxFramesCanMessages->add(frame);
}
BOOL CTCPSocketCommObject::getRxCanMsgFrame(CTCPSocketCommFrame* frame) {
	if (0 == m_rxFramesCanMessages || 0 == frame) {
		ASSERT(0 != m_rxFramesCanMessages);
		ASSERT(0 != frame);
		return FALSE;
	}
	return m_rxFramesCanMessages->get(frame);
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketMsgObserver_Impl::CSocketMsgObserver_Impl(CTCPSocketCommObject& parent) : m_parent( parent ) { }
CSocketMsgObserver_Impl::~CSocketMsgObserver_Impl() { }

BOOL CSocketMsgObserver_Impl::onStatusConnectRequestKey(unsigned short protocolVersion,
														unsigned short softwareVersion,int tokenNumber,const char* szClientKey) 
{
	return TRUE;
}

BOOL CSocketMsgObserver_Impl::onStatusConnectPassword(BOOL stateConnected) {
	return TRUE;
}
BOOL CSocketMsgObserver_Impl::onStatusCanRxMsgSubscribe(BOOL status) {
	return TRUE;
}
 
BOOL CSocketMsgObserver_Impl::onStatusHeartbeat() {	
	// Moeten niks doen: Bij het versturen wordt er automatisch een socket fout gegeven
	// wanneer de verbinding niet meer aktief is.
	return FALSE;		
}

// Sedert V16.26: Updaten van de server node info.
// Nodig voor de integratie van DT18-PRO.
// Mbv de nodevlaggen kunnen we bepalen of de server de master is in het systeem.
BOOL CSocketMsgObserver_Impl::onRecievedServerNodeInfo(int nodeAddress,unsigned long ulPhysicalAddress,const char* name,
							  int numberOfUnits,int nodeType,int nodeFlags) 
{
	const SocketConnectState_t state = m_parent.getSocketConnectState();
	if (SOCKET_STATE_CONNECT_LOGIN_SUCCESS == state) {	
		m_parent.getParentHost().updateServerInfo(nodeAddress, name, numberOfUnits, nodeFlags);
		return TRUE;
	}
	return FALSE;
}

BOOL CSocketMsgObserver_Impl::onReceivedServerNodeStatusMaster(BOOL masterNode) {
	return FALSE;	// Geen Implementatie: Info is ook beschikbaar in de nodeflags. zie updateServerInfo()
}
BOOL CSocketMsgObserver_Impl::onReceivedServerNodeStatusMasterSupported(BOOL masterSupported) {
	return FALSE;	// Geen implementatie : Wordt opgevraagd in de FSM.
}
BOOL CSocketMsgObserver_Impl::onReceivedServerNodeMasterSet(BOOL success) {
	return FALSE;	// Geen implementatie.
}

/******************************************************************************************/
// Ontvangen berichten van de cloud server bridge.
BOOL CSocketMsgObserver_Impl::onRecievedTunnelLoginResponse(BOOL status) {
	return TRUE;	// absorbe: Wordt enkel gebruikt bij het maken van een connectie.
}

/***************************************************************************************************/

		
BOOL CSocketMsgObserver_Impl::onRecievedCanRxMsg(int nodeAddress, int unitAddress,
												 int messageCode, int length, const BYTE* pbData) 
{	
	CTCPSocketCommFrame* const pNewFrame = new CTCPSocketCommFrame(messageCode,nodeAddress,unitAddress,length,pbData,CTCPSocketCommFrame::READ);
	if (NULL != pNewFrame) {		
		if (!m_parent.onRxNewFrame(pNewFrame)) {
			delete pNewFrame;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
/*****************************************************************/
