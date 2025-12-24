#include "stdafx.h"
#include "tcpsocketmessages.h"
#include "tcpsocketmessages_def.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
ITCPSocketMessageTransmitter::~ITCPSocketMessageTransmitter() {} 

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketMessageData::CTCPSocketMessageData( ) 
{
	memset(m_hostData,0,sizeof(m_hostData));
	memset(m_networkData,0,sizeof(m_networkData));	
	m_txCounter = 0;
	m_rxCounter = 0;		
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketMessageProcessor::CTCPSocketMessageProcessor(CSocketMsgObservers& observers,CTCpSocketMsgFormatter& parser) : 
	m_observers(observers),m_parser(parser),m_pszTempBuffer(NULL), m_bufferLength(0) { }

CTCPSocketMessageProcessor::~CTCPSocketMessageProcessor() {
	if (NULL != m_pszTempBuffer) {
		delete [] m_pszTempBuffer;
		m_pszTempBuffer = NULL;
	}	
}

char* CTCPSocketMessageProcessor::normalizeBuffer(const BYTE* pbDataSource,int length) {
	if ((NULL == m_pszTempBuffer) || (m_bufferLength <= length)) {
		delete m_pszTempBuffer;
		m_pszTempBuffer = NULL;
	}
	if (NULL == pbDataSource || length<=0) {
		return NULL;
	}

	if (NULL == m_pszTempBuffer) {
		m_bufferLength = length + 1;
		m_pszTempBuffer = new char[m_bufferLength];
	}
	
	ASSERT(NULL != m_pszTempBuffer);
	ASSERT(length<m_bufferLength);

	memset(m_pszTempBuffer,0,m_bufferLength);
	for (int i=0,j=0;i<length;++i) {
		if ((pbDataSource[i] != 0) && (pbDataSource[i] != '\r')) {
			m_pszTempBuffer[j++] = pbDataSource[i];
		} 	
	}
	return m_pszTempBuffer;
}

BOOL CTCPSocketMessageProcessor::RecieveMessage(ConnectionMethod_t method, CTCPSocketMessageData* dest,const BYTE* pbDataSource,int length) {
	if (NULL == pbDataSource || NULL==dest || length<=0) {
		ASSERT(length>0);
		ASSERT(NULL != pbDataSource);
		ASSERT(NULL != dest);
		return FALSE;
	}

	// CHANGES_CLOUDBRIDGE_POC:
	// Wanneer de connectie via de cloud verloopt worden er bij de start van de connectie tunnel headers verstuurd.
	// Momenteel is dit enkel een antwoord op het login token [OK] of [ERROR-ID]
	//
	// TODO_CLOUDBRIDGE (LOW-PRIO)
	// Wanneer er geen extra tunnel berichten verstuurd worden kunnen we bijhouden of de tunnel registratie verlopen is.
	// Wanneer deze ok is dan moet deze berichten niet meer geparsed worden.
	// Moet dan enkel de eerste keer geparsed worden op [OK] en [ERROR-ID]
	// 
	// TODO_CLOUDBRIDGE (LOW-PRIO)
	// Moeten een buffer bijhouden van ontvangen data wanneer het tcp bericht opgesplitst werd... 
	// Een tcp bericht kan in meerdere tcp paketten ontvangen worden.
	// Moet starten met een '['			
	if (CONNECTION_METHOD_REMOTE_CLOUD == method) {
		CTCPSocketMessageTunnelHeader msgTunnelHeader(this->m_observers);
		int numberOfBytes = 0;
		if (msgTunnelHeader.process(pbDataSource, length, &numberOfBytes)) {
			if (length <= numberOfBytes) {
				return TRUE;
			}
			// Wanneer er nog data aan het bericht hangt deze doorgeven 
			// De pointer van de data verwijst naar de andere data.
			pbDataSource = &pbDataSource[numberOfBytes + 1];
 		}
	}


	// TRACE("--> CTCPSocketMessageProcessor::RecieveMessage() - length = %d\r\n",length);

	// Null char & andere '\r' verwijderen uit de data...
	char* const pszSource = normalizeBuffer(pbDataSource,length);
	if (NULL == pszSource) {
		return FALSE;
	}
	enum { MAX_RX_DATALENGTH = 512};		// maximum lengte van ontvangen data '[ , , , ]'
	char tempData[MAX_RX_DATALENGTH];
	memset(tempData, 0, sizeof(tempData));
	if (m_parser.ParseString(pszSource, tempData, TRUE)) {
		do {
			if (m_parser.Build(tempData,dest->m_hostData,CTCPSocketMessageData::MAX_HOSTBUFFER)) 
			{
				const int msgCode = dest->m_hostData[ARRAY_INDEX_MESSAGECODE];
				switch(msgCode) {
					case Connect_pr2::EV_CLIENTCONNECTSTATUS: {						
						CTCPSocketConnectStatusMessage_pr2 msg(*dest,NULL);	// geen transmitter meegeven.
						msg.parse(m_observers);												 
					} break;
					case Heartbeat::EV_HEARTBEATSTATUS: {														
						CSocketHeartbeatStatusMessage_pr2 msg(*dest,NULL);	// geen transmitter meegeven.
						msg.parse(m_observers);
					} break;
					case ProtocolMsg::EV_PROTOCOLMSG: {												  
						CSocketProtocolRxMessage_pr2 msg(*dest,NULL);			// geen transmitter meegeven.
						msg.parse(m_observers);												  
					} break;	
					case NodeManagement::EV_NODEMANAGEMENTINFO: {
						CSocketNodeManagementStatus_pr2 msg(*dest,NULL);	// geen transmitter meegeven.
						msg.parse(m_observers);
					} break;		
					case ApplicationTask::EV_APPLICATIONTASKSTATUS: {
						CSocketNodeMasterTaskStatus_pr2 msg(*dest,NULL);	// geen transmitter meegeven.
						msg.parse(m_observers);
					} break;
					case NodeDabase::EV_NODEDATABASEINFO: {
						CSocketNodeDatabaseStatus_pr2 msg(*dest,NULL);		// geen transmitter meegeven.
						msg.parse(m_observers);					  
					} break;
				}
			}		
		} while(m_parser.ParseString(pszSource, tempData));
		return TRUE;
	}
	return FALSE;
}
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
const char* const CTCPSocketMessageTunnelHeader::STRING_TUNNEL_RESPONSE_LOGIN_SUCCESS = "[OK]";
const char* const CTCPSocketMessageTunnelHeader::STRING_TUNNEL_RESPONSE_LOGIN_REGISTRATION_DOUBLE = "[OK-2]";
const char* const CTCPSocketMessageTunnelHeader::STRING_TUNNEL_RESPONSE_LOGIN_ERROR = "[ERROR-ID]";
/*****************************************************************/
CTCPSocketMessageTunnelHeader::CTCPSocketMessageTunnelHeader(CSocketMsgObservers& observers) : m_observers(observers) {  }

bool CTCPSocketMessageTunnelHeader::process(const BYTE* pbDataSource, int length, int* numberOfBytes) {
	const ParserResult_t res = parse(pbDataSource, length, numberOfBytes);
	if (PARSER_EC_UNKNOWN == res || PARSER_EC_GENERAL == res) {
		return false;
	}
	// doorgeven aan de observers...
	ISocketMsgObserver *pEventHandler = 0;
	for (pEventHandler = m_observers.Begin(); pEventHandler != 0; pEventHandler = m_observers.Next()) {		
		if (PARSER_LOGIN_SUCCESS == res) {
			pEventHandler->onRecievedTunnelLoginResponse(TRUE);
		}
		else  if (PARSER_EC_REFUSED == res) {
			pEventHandler->onRecievedTunnelLoginResponse(FALSE);
		}
	}
	return true;
}

CTCPSocketMessageTunnelHeader::ParserResult_t CTCPSocketMessageTunnelHeader::parse(const BYTE* pbDataSource, int length, int* numberOfBytes) {
	ParserResult_t result = PARSER_EC_UNKNOWN;	
	REQUIRE(NULL != pbDataSource);
	if (parse(pbDataSource, length, STRING_TUNNEL_RESPONSE_LOGIN_SUCCESS, numberOfBytes)) {
		result = PARSER_LOGIN_SUCCESS;
	}
	else if (parse(pbDataSource, length, STRING_TUNNEL_RESPONSE_LOGIN_ERROR, numberOfBytes)) {
		result = PARSER_EC_REFUSED;
	}
	else if (parse(pbDataSource, length, STRING_TUNNEL_RESPONSE_LOGIN_REGISTRATION_DOUBLE, numberOfBytes)) {
		result = PARSER_LOGIN_SUCCESS;
	}
	return result;
}

bool CTCPSocketMessageTunnelHeader::parse(const BYTE* pbDataSource, int length, const char* str, int* numberOfBytes) {

	REQUIRE(NULL != pbDataSource);
	REQUIRE(NULL != str);

	const int stringLength = strlen(str);
	if (length < stringLength) {
		return false;
	}
	for (int i = 0; i<stringLength; ++i) {
		if (pbDataSource[i] != str[i]) {
			return false;
		}
	}
	*numberOfBytes = stringLength;
	return true;
}
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketMessage::CTCPSocketMessage(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) :
	m_refDataBuffer(refDataBuffer), m_pTransmitter(pTransmitter)  {  }
CTCPSocketMessage::~CTCPSocketMessage( ) { }

BOOL CTCPSocketMessage::build(CTCpSocketMsgFormatter& formatter) {	
	return formatter.Build(m_refDataBuffer.m_hostData,m_refDataBuffer.m_networkData,CTCPSocketMessageData::MAX_NETWORKBUFFER);
}

BOOL CTCPSocketMessage::SendMessage(void) {
	if (NULL == m_pTransmitter) {
		ASSERT(NULL != m_pTransmitter);		
		return FALSE;
	}	
	const int length = strlen(m_refDataBuffer.m_networkData);
	if (length<=0) {
		ASSERT(length>0);
		return FALSE;
	}
	return m_pTransmitter->send((const BYTE*)(&m_refDataBuffer.m_networkData[0]),length);
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketConnectMessage::CTCPSocketConnectMessage(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) : 
	Base(refDataBuffer,pTransmitter) { }
CTCPSocketConnectMessage::~CTCPSocketConnectMessage() { }

const char* CTCPSocketConnectMessage::getStringToken(int identifier) {
	
	static const char* const m_arrTokens[16] = {
		"iCgbtdVxpNr0bUaU", // token 0
		"OP6cFNudnIsuDwkr", // token 1
		"RaAKedvBnHgh8Hc3", // token 2
		"bxBqOh8Nm1yThEzW", // token 3
		"db5rpPWAcjNJAJUi", // token 4
		"OcVRGY6VB7lXbx6t", // token 5
		"j3WoOhfmEAAYxYgo", // token 6
		"2ImV0TrD7yk6SmAw", // token 7
		"UquAWx6B7Wirnjof", // token 8
		"krtBrc6HhO7RFtNA", // token 9
		"GX2vL4gdpb6sCRrU", // token 10
		"pw5jitkAp0wJtAJe", // token 11
		"fY40VHMPQdyu0PhR", // token 12
		"as2OUemhVX5oJbEV", // token 13
		"QezPOu7n6TRStdgy", // token 14
		"2UTQLGWJu1PxOgqX", // token 15
	};

	if (identifier>=16) {
		ASSERT(identifier<16);
		return "undefined";
	}
	return m_arrTokens[identifier];
}

/*****************************************************************/

/*****************************************************************/
CTCPSocketConnectStatusMessage_pr2::CTCPSocketConnectStatusMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer,pTransmitter) { }

CTCPSocketConnectStatusMessage_pr2::~CTCPSocketConnectStatusMessage_pr2( ) { }

BOOL CTCPSocketConnectStatusMessage_pr2::parse(CSocketMsgObservers& observers) {

	if (Connect_pr2::EV_CLIENTCONNECTSTATUS !=getMsgCode()) {
		ASSERT(0);
		return FALSE;
	}

	const int method = getMethod();
	switch(method) {
		case Connect_pr2::OBJECT_KEY: {

			uint16_t protocolVersion = 0;
			uint16_t softwareVersion = 0;
			uint8_t dummy = 0;
			uint8_t tokenNumber = 0;

			RX_DATA_BEGIN();					
			protocolVersion = (RX_DATA_GET() << 8);
			protocolVersion += RX_DATA_GET();
			softwareVersion = (RX_DATA_GET() << 8);
			softwareVersion += RX_DATA_GET();
			dummy = RX_DATA_GET();
			dummy = RX_DATA_GET();
			tokenNumber = RX_DATA_GET();
			dummy = RX_DATA_GET();
			
			char strClientID[32];
			const int length = RX_DATA_GET();
			if (length >= sizeof(strClientID)) {
				ASSERT(length < sizeof(strClientID));
				return FALSE;
			}
			memset(strClientID, 0, sizeof(strClientID));
			for (int i = 0; i < length; ++i) {
				strClientID[i] = RX_DATA_GET();
			}

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onStatusConnectRequestKey(protocolVersion, softwareVersion, tokenNumber, strClientID);
			}
		} return TRUE;

		case Connect_pr2::OBJECT_PASSWORD: {							
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET( );			
			
			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin();pEventHandler != 0;pEventHandler = observers.Next()) {
				pEventHandler->onStatusConnectPassword(1 == status ? TRUE : FALSE);
			}			
		} return TRUE;
	}		
	return FALSE;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketConnectMessage_pr2::CSocketConnectMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) : 
	Base(refDataBuffer, pTransmitter) { } 
CSocketConnectMessage_pr2::~CSocketConnectMessage_pr2() { }

BOOL CSocketConnectMessage_pr2::setRequestKey(CTCpSocketMsgFormatter& formatter) {

	TX_BEGIN();
	TX_ADD(Connect_pr2::FC_CLIENTCONNECTSET);		
	TX_ADD(Connect_pr2::OBJECT_KEY);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

BOOL CSocketConnectMessage_pr2::setConnect(CTCpSocketMsgFormatter& formatter,int length,const BYTE* string) {

	// const int length = strlen(string);
	if (length<=0) {
		return FALSE;
	}

	TX_BEGIN();
	TX_ADD(Connect_pr2::FC_CLIENTCONNECTSET);		
	TX_ADD(Connect_pr2::OBJECT_PASSWORD_SECURE_V2);
	TX_ADD(length);
	for (int i=0;i<length;++i) {
		TX_ADD(string[i]);
	}
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

// Versturen van de client connection. (oude login)
BOOL CSocketConnectMessage_pr2::setConnect(CTCpSocketMsgFormatter& formatter,const char* password) {
	if (NULL == password) {
		return FALSE;
	}
	const int length = strlen(password);
	if (length<=0) {
		return FALSE;
	}

	TX_BEGIN();
	TX_ADD(Connect_pr2::FC_CLIENTCONNECTSET);		
	TX_ADD(Connect_pr2::OBJECT_PASSWORD);
	TX_ADD(length);
	for (int i=0;i<length;++i) {
		TX_ADD(password[i]);
	}
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

BOOL CSocketConnectMessage_pr2::SetDisconnect(CTCpSocketMsgFormatter& formatter) {

	TX_BEGIN();
	TX_ADD(Connect_pr2::FC_CLIENTCONNECTSET);		
	TX_ADD(Connect_pr2::OBJECT_DISCONNECT);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}


// pbDest [Out]
// src [in]
// privateKey [in]
// maxLength [IN]
// returnwaarde: Lengte van de nieuwe string
static int encode(BYTE* pbDest,CString& src,CString& privateKey,int maxLength) {
	int retValue = -1;
	if (NULL == pbDest) {
		ASSERT(NULL != pbDest);
		return -1;
	}
	if (src.IsEmpty() || privateKey.IsEmpty()) {
		ASSERT(!src.IsEmpty());
		ASSERT(!privateKey.IsEmpty());
		return -1;
	}
	
	const int length = src.GetLength();
	if (maxLength<length) {
		ASSERT(maxLength>=length);
		return -1;
	}

	int i;
	const int lengthPrivateKey = privateKey.GetLength();
	for (i=0;i<length;++i) {
		pbDest[i] = src.GetAt(i) ^ privateKey.GetAt(i % lengthPrivateKey);	
	}	
	ASSERT(i<maxLength);
	return i;
}

BOOL CSocketConnectMessage_pr2::formatLoginString(BYTE* pbDest,const char* client,const char* password,int tokenNumber,int maxLength,int* length) {

	if (tokenNumber<0 || tokenNumber>=16) {
		ASSERT(tokenNumber>=0);
		ASSERT(tokenNumber<16);
		return FALSE;
	}
	if (NULL == client || NULL == password) {
		ASSERT(NULL != client);
		ASSERT(NULL != password);
		return FALSE;
	}

	CString loginString;
	loginString.Format("%s:%s",client,password);	
	CString privateKey(getStringToken(tokenNumber));	

	TRACE1("TokenNumber=%d\r\n",tokenNumber);
	TRACE1("PrivateKey='%s'\r\n",privateKey);
	TRACE1("Client='%s'\r\n",client);

	const int temp_length = encode(pbDest,loginString,privateKey,maxLength);
	if (temp_length<=0) {
		return FALSE;
	}
	*length = temp_length;
	return TRUE;
}

/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketHeartbeatMessage_pr2::CSocketHeartbeatMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) : 
	Base(refDataBuffer,pTransmitter) { }
CSocketHeartbeatMessage_pr2::~CSocketHeartbeatMessage_pr2() { }

BOOL CSocketHeartbeatMessage_pr2::setHeartbeat(CTCpSocketMsgFormatter& formatter) {

	TX_BEGIN();
	TX_ADD(Heartbeat::FC_HEARTBEATREQUESTSTATUS);
	TX_ADD(Heartbeat::OBJECT_POLL);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketHeartbeatStatusMessage_pr2::CSocketHeartbeatStatusMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) : 
	Base(refDataBuffer,pTransmitter) { }
CSocketHeartbeatStatusMessage_pr2::~CSocketHeartbeatStatusMessage_pr2() { }

BOOL CSocketHeartbeatStatusMessage_pr2::parse(CSocketMsgObservers& observers) {
	if (Heartbeat::EV_HEARTBEATSTATUS != getMsgCode()) {
		ASSERT(0);
		return FALSE;
	}

	const int method = getMethod();
	switch (method) {
		case Heartbeat::OBJECT_POLL: {

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onStatusHeartbeat( );
			}
		} return TRUE;
	}
	return FALSE;
}
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketProtocolMessage_pr2::CSocketProtocolMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer,pTransmitter) { }
CSocketProtocolMessage_pr2::~CSocketProtocolMessage_pr2() { }

BOOL CSocketProtocolMessage_pr2::setCanBusSubscribe(CTCpSocketMsgFormatter& formatter, BOOL state) {
	return this->setSubscribe(formatter, ProtocolMsg::ATTR_SUBSCRIBE_CANBUS_MSG, state);
}

BOOL CSocketProtocolMessage_pr2::setDaliBusSubscribe(CTCpSocketMsgFormatter& formatter, BOOL state) {
	return this->setSubscribe(formatter, ProtocolMsg::ATTR_SUBSCRIBE_DALIBUS, state);
}

BOOL CSocketProtocolMessage_pr2::setCanBusMessage(CTCpSocketMsgFormatter& formatter, int nodeAddress, int unitAddress,int APDUHeader,
	int messageCode, int length, const BYTE* pbData) 
{
	TX_BEGIN();
	TX_ADD(ProtocolMsg::FC_PROTOCOLMSG);
	TX_ADD(ProtocolMsg::OBJECT_CANBUS_MSG);

	// MessagePacket info...
	TX_ADD(nodeAddress);
	TX_ADD(unitAddress);
	TX_ADD(0xff);							// Source unit is altijd 0xFF
	TX_ADD(0xff);
	TX_ADD(APDUHeader);			
	TX_ADD(messageCode);

	// Lengte + dataLengte.
	TX_ADD(length);
	for (int i = 0; i<length; ++i) {
		TX_ADD(pbData[i]);
	}
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

// Versturen van een DALI-Msg
BOOL CSocketProtocolMessage_pr2::setDaliBusMessage(CTCpSocketMsgFormatter& formatter, int clientIdentifier, BYTE msgIdentifier, int prioLevel, int nrOfBits, const BYTE* pbData) {
	TX_BEGIN();
	TX_ADD(ProtocolMsg::FC_PROTOCOLMSG);
	TX_ADD(ProtocolMsg::OBJECT_DALIBUS_MSG);

	// DALI-bus message
	TX_ADD((clientIdentifier & 0xFF00) >> 8);
	TX_ADD(clientIdentifier & 0xFF);
	TX_ADD(msgIdentifier);
	TX_ADD(prioLevel);
	TX_ADD(0xff);				// bReservedMaxRetries
	TX_ADD(0xff);
	TX_ADD(0xff);
	TX_ADD(nrOfBits);
	TX_ADD(pbData[0]);
	TX_ADD(pbData[1]);
	TX_ADD(pbData[2]);
	TX_ADD(pbData[3]);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

BOOL CSocketProtocolMessage_pr2::setSubscribe(CTCpSocketMsgFormatter& formatter, BYTE bObjectMember, BOOL state) {

	TX_BEGIN();
	TX_ADD(ProtocolMsg::FC_PROTOCOLMSG);
	TX_ADD(bObjectMember);
	TX_ADD(TRUE == state ? 1 : 0);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketProtocolRxMessage_pr2::CSocketProtocolRxMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer,pTransmitter) { }

CSocketProtocolRxMessage_pr2::~CSocketProtocolRxMessage_pr2( ) { }

BOOL CSocketProtocolRxMessage_pr2::parse(CSocketMsgObservers& observers) {

	if (ProtocolMsg::EV_PROTOCOLMSG !=getMsgCode()) {
		ASSERT(0);
		return FALSE;
	}

	const int method = getMethod();
	switch(method) {
		case ProtocolMsg::OBJECT_CANBUS_MSG: {			
			int dummy = 0;
			int nodeAddress = -1;
			int unitAddress = -1;
			int messageCode = -1; 
			int length = 0; 
			BYTE* pbData = NULL;

			RX_DATA_BEGIN();
			nodeAddress = RX_DATA_GET();
			unitAddress = RX_DATA_GET();
			dummy = RX_DATA_GET();
			dummy = RX_DATA_GET();
			dummy = RX_DATA_GET();
			messageCode = RX_DATA_GET();
			length = RX_DATA_GET();

			if (length>0) {
				pbData = new BYTE[length+1];
				memset(pbData,0,length);
				for (int i=0;i<length;++i) {
					pbData[i] = RX_DATA_GET();
				}
			}

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onRecievedCanRxMsg(nodeAddress,unitAddress,messageCode,length,pbData);
			}

			if (NULL != pbData) {
				delete [] pbData;
			}		
		} break;
		case ProtocolMsg::ATTR_SUBSCRIBE_CANBUS_MSG: {		
			RX_DATA_BEGIN();
			const bool status = (1 == RX_DATA_GET());

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onStatusCanRxMsgSubscribe(status);
			}
		} break;

		case ProtocolMsg::ATTR_SUBSCRIBE_DALIBUS: {									
			RX_DATA_BEGIN();
			const bool status = (1 == RX_DATA_GET());
			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onStatusDALIRxMsgSubscribe(status);
			}												  
		} break;
			

		// Sedert Firmware V66.13.06 / Binding software V16.40
		case ProtocolMsg::OBJECT_DALIBUS_MSG: {
			BYTE headerReservedFlags = 0;
			BYTE headerReservedType = 0;
			BYTE lengthBits = 0;
			uint32_t recievedFrame = 0;

			RX_DATA_BEGIN();
			headerReservedFlags = RX_DATA_GET();
			headerReservedType = RX_DATA_GET();
			lengthBits = RX_DATA_GET();
			recievedFrame = (RX_DATA_GET() << 24);
			recievedFrame +=(RX_DATA_GET() << 16);
			recievedFrame +=(RX_DATA_GET() << 8);
			recievedFrame += RX_DATA_GET();

			// static const unsigned short usClientIdentifier = 0xffff;	// Broadcast bericht naar alle.
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onRxDALIMessage(lengthBits, recievedFrame);
			}

		} break;

		case ProtocolMsg::OBJECT_DALIBUS_RESPONSE: {						
			unsigned short usClientIdentifier = 0;
			BYTE msgIdentifier = 0;
			BYTE type = 0;
			BYTE dummy = 0;
			BYTE responseData =0;

			RX_DATA_BEGIN();
			usClientIdentifier = RX_DATA_GET() << 8;
			usClientIdentifier += RX_DATA_GET();
			msgIdentifier = RX_DATA_GET();
			type = RX_DATA_GET();
			dummy = RX_DATA_GET();
			dummy = RX_DATA_GET();
			responseData = RX_DATA_GET();

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onStatusDALIResponse(usClientIdentifier, msgIdentifier, type, responseData);
			}	
		} break;
	}
	return FALSE;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketRequestNodeManagement_pr2::CSocketRequestNodeManagement_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) : 
	Base(refDataBuffer,pTransmitter) { }
CSocketRequestNodeManagement_pr2::~CSocketRequestNodeManagement_pr2() { }

BOOL CSocketRequestNodeManagement_pr2::sendRequestNodeInfo(CTCpSocketMsgFormatter& formatter) {

	TX_BEGIN();
	TX_ADD(NodeManagement::FC_REQUESTNODEMANAGEMENT);
	TX_ADD(NodeManagement::ATTR_NODEINFO);	
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

BOOL CSocketRequestNodeManagement_pr2::sendRequestMasterSupported(CTCpSocketMsgFormatter& formatter) {
	TX_BEGIN();
	TX_ADD(NodeManagement::FC_REQUESTNODEMANAGEMENT);
	TX_ADD(NodeManagement::ATTR_MASTER_SUPPORTED);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());	
}

BOOL CSocketRequestNodeManagement_pr2::sendRequestMaster(CTCpSocketMsgFormatter& formatter) {
	TX_BEGIN();
	TX_ADD(NodeManagement::FC_REQUESTNODEMANAGEMENT);
	TX_ADD(NodeManagement::ATTR_MASTER);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Sedert V16.26: Instellen als master.
CSocketSetNodeManagement_pr2::CSocketSetNodeManagement_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer, pTransmitter) { }

CSocketSetNodeManagement_pr2::~CSocketSetNodeManagement_pr2() { }

BOOL CSocketSetNodeManagement_pr2::sendSetMaster(CTCpSocketMsgFormatter& formatter) {
	TX_BEGIN();
	TX_ADD(NodeManagement::FC_NODEMANAGEMENTSET);
	TX_ADD(NodeManagement::OBJECT_SET_MASTER);
	TX_ADD('1');
	TX_ADD('2');
	TX_ADD('4');
	TX_ADD('8');
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Sedert V16.26: Ondersteuning van de functies om de server als master te configuren.
CSocketNodeManagementStatus_pr2::CSocketNodeManagementStatus_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter) : 
	Base(refDataBuffer, pTransmitter) { }
CSocketNodeManagementStatus_pr2::~CSocketNodeManagementStatus_pr2( ) { }

BOOL CSocketNodeManagementStatus_pr2::parse(CSocketMsgObservers& observers) {

	if (NodeManagement::EV_NODEMANAGEMENTINFO != getMsgCode()) {
		ASSERT(0);
		return FALSE;
	}

	const int method = getMethod();
	switch(method) {
		case NodeManagement::ATTR_MASTER_SUPPORTED: {					
			int temp = 0;
			RX_DATA_BEGIN();
			temp = RX_DATA_GET();

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onReceivedServerNodeStatusMasterSupported(1 == temp);
			}
		} break;

		case NodeManagement::ATTR_NODEINFO: {
				
			int temp = 0;
			int nodeAddress = 0xff;
			unsigned long ulPhysicalAddress = 0;
			int nameLength = 0;		
			char* szNodeName = 0;
			int numberOfUnits = 0;
			int nodeType = 0;
			int nodeFlags = 0;


			RX_DATA_BEGIN();
			temp = RX_DATA_GET();
			nodeAddress = RX_DATA_GET();
			temp = RX_DATA_GET();
			ulPhysicalAddress += (temp << 24);
			temp = RX_DATA_GET();
			ulPhysicalAddress += (temp << 16);
			temp = RX_DATA_GET();
			ulPhysicalAddress += (temp << 8);
			temp = RX_DATA_GET();
			ulPhysicalAddress += temp;
			nameLength = RX_DATA_GET();

			if (nameLength>0) {
				szNodeName = new char[nameLength+1];
				memset(szNodeName,'\0',nameLength+1);
				for (int i=0;i<nameLength;++i) {
					szNodeName[i] = (char) RX_DATA_GET();
				}			
			}

			numberOfUnits = RX_DATA_GET();
			nodeType = RX_DATA_GET();
			nodeFlags = RX_DATA_GET();

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onRecievedServerNodeInfo(nodeAddress,ulPhysicalAddress,szNodeName,numberOfUnits,nodeType,nodeFlags);
			}

			if (0 != szNodeName) {
				delete [] szNodeName;
				szNodeName = 0;
			}											
		} break;

		case NodeManagement::OBJECT_SET_MASTER_OK: {		
			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onReceivedServerNodeMasterSet(TRUE);
			}
		} break;

		case NodeManagement::OBJECT_SET_MASTER_EC: {
			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onReceivedServerNodeMasterSet(FALSE);
			}
		} break;

		case NodeManagement::ATTR_MASTER: {		
			int temp = 0;
			RX_DATA_BEGIN();
			temp = RX_DATA_GET();

			// doorgeven aan de observers...
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onReceivedServerNodeStatusMaster(1 == temp);
			}
		} break;
	}
	return FALSE;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketNodeMasterSetTask_pr2::CSocketNodeMasterSetTask_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer, pTransmitter) { }

CSocketNodeMasterSetTask_pr2::~CSocketNodeMasterSetTask_pr2() { }

BOOL CSocketNodeMasterSetTask_pr2::sendCancelTask(CTCpSocketMsgFormatter& formatter,const char* password) {
	if (!checkSyntaxPassword(password)) {
		ASSERT(0);
		return FALSE;
	}

	TX_BEGIN();
	TX_ADD(ApplicationTask::FC_APPLICATIONTASKSET);
	TX_ADD(ApplicationTask::OBJECT_CANCEL);
	TX_ADD(password[0]);
	TX_ADD(password[1]);
	TX_ADD(password[2]);
	TX_ADD(password[3]);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

BOOL CSocketNodeMasterSetTask_pr2::sendStartTask(CTCpSocketMsgFormatter& formatter,const char* password, TcpSocketHomeClient_Tasks::Task_t task) {
	if (!checkSyntaxPassword(password)) {
		ASSERT(0);
		return FALSE;
	}
	TX_BEGIN();
	TX_ADD(ApplicationTask::FC_APPLICATIONTASKSET);
	TX_ADD((int) task);
	TX_ADD(password[0]);
	TX_ADD(password[1]);
	TX_ADD(password[2]);
	TX_ADD(password[3]);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

BOOL CSocketNodeMasterSetTask_pr2::sendRequestState(CTCpSocketMsgFormatter& formatter) {

	TX_BEGIN();
	TX_ADD(ApplicationTask::FC_APPLICATIONTASKREQUEST);
	TX_ADD(ApplicationTask::ATTR_STATE);

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}

// Helper functie.
BOOL CSocketNodeMasterSetTask_pr2::checkSyntaxPassword(const char* password) {
	if (NULL == password) {
		return FALSE;
	}

	const int length = strlen(password);
	if (4 != length) {
		return FALSE;
	}
	for (int i = 0; i < length; ++i) {
		if (!isdigit(password[i])) {
			return FALSE;
		}
	}
	return TRUE;
}
/*****************************************************************/
CSocketNodeMasterTaskStatus_pr2::CSocketNodeMasterTaskStatus_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer, pTransmitter) { }

CSocketNodeMasterTaskStatus_pr2::~CSocketNodeMasterTaskStatus_pr2() { }

BOOL CSocketNodeMasterTaskStatus_pr2::parse(CSocketMsgObservers& observers) {

	if (ApplicationTask::EV_APPLICATIONTASKSTATUS != getMsgCode()) {
		ASSERT(0);
		return FALSE;
	}

	const int method = getMethod();
	switch (method) {
		case ApplicationTask::OBJECT_BROWSE: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			notifyTaskStatus(observers, TcpSocketHomeClient_Tasks::TASK_BROWSE, 
				0 == status ? TcpSocketHomeClient_Tasks::STATUS_FAILED : TcpSocketHomeClient_Tasks::STATUS_OK);
		} break;
		case ApplicationTask::OBJECT_BROWSECLEAR: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			notifyTaskStatus(observers, TcpSocketHomeClient_Tasks::TASK_BROWSE_CLEAR,
				0 == status ? TcpSocketHomeClient_Tasks::STATUS_FAILED : TcpSocketHomeClient_Tasks::STATUS_OK);
		} break;
		case ApplicationTask::OBJECT_PNP: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			notifyTaskStatus(observers, TcpSocketHomeClient_Tasks::TASK_PNP,
				0 == status ? TcpSocketHomeClient_Tasks::STATUS_FAILED : TcpSocketHomeClient_Tasks::STATUS_OK);
		} break;
		case ApplicationTask::OBJECT_UPDATEDBASE: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			notifyTaskStatus(observers, TcpSocketHomeClient_Tasks::TASK_UPDATE_DBASE,
				0 == status ? TcpSocketHomeClient_Tasks::STATUS_FAILED : TcpSocketHomeClient_Tasks::STATUS_OK);
		} break;
		case ApplicationTask::OBJECT_RESETALL: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			notifyTaskStatus(observers, TcpSocketHomeClient_Tasks::TASK_RESETALL,
				0 == status ? TcpSocketHomeClient_Tasks::STATUS_FAILED : TcpSocketHomeClient_Tasks::STATUS_OK);
		} break;
		case ApplicationTask::OBJECT_FACTORY_RESETALL: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			notifyTaskStatus(observers, TcpSocketHomeClient_Tasks::TASK_FACTORY_RESETALL ,
				0 == status ? TcpSocketHomeClient_Tasks::STATUS_FAILED : TcpSocketHomeClient_Tasks::STATUS_OK);
		} break;
		case ApplicationTask::ATTR_STATE: {
			RX_DATA_BEGIN();
			const int state = RX_DATA_GET();
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onRecievedAppTaskState(convertToState(state));
			}
		} break;
		case ApplicationTask::OBJECT_CANCEL: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onRecievedAppTaskCancel(0 == status ? TcpSocketHomeClient_Tasks::STATUS_FAILED : TcpSocketHomeClient_Tasks::STATUS_OK);
			}
		} break;
		case ApplicationTask::OBJECT_INFO: {
		
			RX_DATA_BEGIN();
			const int length = RX_DATA_GET();
			if (length > 0) {
				char* msg = new char[length + 1];
				memset(msg, '\0', length+1);
				for (int i = 0; i < length; ++i) {
					msg[i] = RX_DATA_GET();
				}

				const CString strMsg(msg);
				ISocketMsgObserver *pEventHandler = 0;
				for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
					pEventHandler->onRecievedAppTaskInfo(strMsg);
				}
				delete[] msg;
			}
		} break;
	}
	return TRUE;
}

void CSocketNodeMasterTaskStatus_pr2::notifyTaskStatus(CSocketMsgObservers& observers,TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) {
	ISocketMsgObserver *pEventHandler = 0;
	for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
		pEventHandler->onRecievedAppTaskStatus(task, status);
	}
}

TcpSocketHomeClient_Tasks::State_t CSocketNodeMasterTaskStatus_pr2::convertToState(int v) {
	return (TcpSocketHomeClient_Tasks::State_t)(v);
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSocketNodeDatabase_pr2::CSocketNodeDatabase_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer, pTransmitter) { }

CSocketNodeDatabase_pr2::~CSocketNodeDatabase_pr2() { }

BOOL CSocketNodeDatabase_pr2::sendRequestStatus(CTCpSocketMsgFormatter& formatter) {
	
	TX_BEGIN();
	TX_ADD(NodeDabase::FC_REQUESTNODEDATABASE);
	TX_ADD(NodeDabase::ATTR_STATE);
	TX_END();

	if (!build(formatter)) {
		return FALSE;
	}
	return (SendMessage());
}



CSocketNodeDatabaseStatus_pr2::CSocketNodeDatabaseStatus_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter) :
	Base(refDataBuffer, pTransmitter) { }

CSocketNodeDatabaseStatus_pr2::~CSocketNodeDatabaseStatus_pr2() { }

BOOL CSocketNodeDatabaseStatus_pr2::parse(CSocketMsgObservers& observers) {

	if (NodeDabase::EV_NODEDATABASEINFO != getMsgCode()) {
		ASSERT(0);
		return FALSE;
	}

	const int method = getMethod();
	switch (method) {
		case NodeDabase::ATTR_STATE: {
			RX_DATA_BEGIN();
			const int status = RX_DATA_GET();
			ISocketMsgObserver *pEventHandler = 0;
			for (pEventHandler = observers.Begin(); pEventHandler != 0; pEventHandler = observers.Next()) {
				pEventHandler->onRecievedNodedatabaseState(convertToStatus(status));
			}									 
		} break;
	}	
	return TRUE;
}

TcpSocketHomeClient_NodeDbase::Status_t CSocketNodeDatabaseStatus_pr2::convertToStatus(int status) {
	return (TcpSocketHomeClient_NodeDbase::Status_t)(status);
}
/*****************************************************************/