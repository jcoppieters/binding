#ifndef INCLUDED_TCPSOCKET_TCPSOCKETMESSAGES_H
#define INCLUDED_TCPSOCKET_TCPSOCKETMESSAGES_H
/*****************************************************************/
#ifndef INCLUDED_TCPSOCKETMSGFORMATTER_H
	#include "tcpsocketmsgformatter.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_CMN_H
	#include "tcpsocketcmn.h"
#endif 
/*****************************************************************/
struct ITCPSocketMessageTransmitter
{
	virtual ~ITCPSocketMessageTransmitter() = 0;
	virtual BOOL send(const BYTE* msg,int length) = 0;
};
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct CTCPSocketMessageData
{
	enum { 
		MAX_HOSTBUFFER = 255,				// aantal bytes in array.
		MAX_NETWORKBUFFER = 512				// max aantal tx char nwk data [ , , , ]
	};
	CTCPSocketMessageData( );

	BYTE m_hostData[MAX_HOSTBUFFER];				// Byte array
	char m_networkData[MAX_NETWORKBUFFER];			// transmit nwk data [ , , , , ]
	int m_txCounter;								// tx counter m_hostData
	int m_rxCounter;								// tx counter m_hostData
};

/*****************************************************************/
class CTCPSocketMessageProcessor
{
public:
	CTCPSocketMessageProcessor(CSocketMsgObservers& observers,CTCpSocketMsgFormatter& parser);
	~CTCPSocketMessageProcessor();
	BOOL RecieveMessage(ConnectionMethod_t method,CTCPSocketMessageData* destData,const BYTE* src,int length);

protected:
	char* normalizeBuffer(const BYTE* src,int length);

private:
	CSocketMsgObservers& m_observers;
	CTCpSocketMsgFormatter& m_parser;
	char* m_pszTempBuffer;
	int m_bufferLength;
};


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketMessageTunnelHeader {
public: 
	CTCPSocketMessageTunnelHeader(CSocketMsgObservers& observers);
	bool process(const BYTE* pbDataSource, int length, int* numberOfBytes);

protected:
	typedef enum {
		PARSER_EC_GENERAL = -3,
		PARSER_EC_UNKNOWN = -2,
		PARSER_EC_REFUSED = -1,
		PARSER_LOGIN_SUCCESS,		
	} ParserResult_t;

	ParserResult_t parse(const BYTE* pbDataSource, int length, int* numberOfBytes);
	bool parse(const BYTE* pbDataSource, int length, const char* str, int* numberOfBytes);

private: 
	static const char* const STRING_TUNNEL_RESPONSE_LOGIN_SUCCESS;
	static const char* const STRING_TUNNEL_RESPONSE_LOGIN_REGISTRATION_DOUBLE;
	static const char* const STRING_TUNNEL_RESPONSE_LOGIN_ERROR;

	CSocketMsgObservers& m_observers;
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketMessage  
{	
public:
	CTCPSocketMessage(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CTCPSocketMessage();

	BOOL SendMessage(void);	

	const char* GetNetworkData(void) {
		return m_refDataBuffer.m_networkData;
	}
	void TxBuild(int counter = 1) {
		m_refDataBuffer.m_txCounter = 1;
	}
	BYTE GetMsb(int i) {
		return ((BYTE)((int) (i & 0xFF00) >> 8));
	}
	BYTE GetLsb(int i) {
		return ((BYTE)((int) (i & 0x00FF)));
	}	
	BYTE GetHWordMsb(unsigned long i) {
		return ((BYTE) ((unsigned long) (i & 0xFF000000) >> 24));
	}
	BYTE GetHWordLsb(unsigned long i) {
		return ((BYTE) ((unsigned long) (i & 0x00FF0000) >> 16));
	}
	BYTE GetLWordMsb(unsigned long i) {
		return ((BYTE) ((unsigned long) (i & 0x0000FF00) >> 8));
	}
	BYTE GetLWordLsb(unsigned long i) {
		return ((BYTE) ((unsigned long) (i & 0x000000FF)));
	}

protected:
	inline void TX_BEGIN() { 
		m_refDataBuffer.m_txCounter = 1; 
	}
	inline void TX_ADD(int v) {	
		m_refDataBuffer.m_hostData[m_refDataBuffer.m_txCounter++] = v; 
	};
	inline void TX_END() {
		m_refDataBuffer.m_hostData[ARRAY_LENGTH] = m_refDataBuffer.m_txCounter-1;
	}

	inline int getMsgCode( ) {
		return m_refDataBuffer.m_hostData[ARRAY_INDEX_MESSAGECODE];
	}
	inline int getMethod( ) {
		return m_refDataBuffer.m_hostData[ARRAY_INDEX_METHOD];
	}

	inline void RX_DATA_BEGIN() { 
		m_refDataBuffer.m_rxCounter = ARRAY_INDEX_DATA0; 
	}
	int RX_DATA_GET( ){
		return m_refDataBuffer.m_hostData[m_refDataBuffer.m_rxCounter++];
	}

	BOOL build(CTCpSocketMsgFormatter& formatter);
	

private:
	CTCPSocketMessageData& m_refDataBuffer;
	ITCPSocketMessageTransmitter* m_pTransmitter;

	friend class CTCPSocketMessageProcessor;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketConnectMessage :  public CTCPSocketMessage
{
public:
	CTCPSocketConnectMessage(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CTCPSocketConnectMessage();

protected:
	static const char* getStringToken(int identifier);

private:
	typedef CTCPSocketMessage Base;
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketConnectStatusMessage_pr2 : public CTCPSocketConnectMessage
{
public:
	CTCPSocketConnectStatusMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CTCPSocketConnectStatusMessage_pr2( );

	BOOL parse(CSocketMsgObservers& observers);

private:
	typedef CTCPSocketConnectMessage Base;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketConnectMessage_pr2 : public CTCPSocketConnectMessage  
{
public:
	CSocketConnectMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketConnectMessage_pr2();

public:
	BOOL Parse(void) { return FALSE; }
	
	// Opvragen van het token number & client id.
	BOOL setRequestKey(CTCpSocketMsgFormatter& formatter);
	// Versturen van de client connection token string. (Secured login)
	BOOL setConnect(CTCpSocketMsgFormatter& formatter,int length,const BYTE* string);
	// Versturen van de client connection. (oude login)
	BOOL setConnect(CTCpSocketMsgFormatter& formatter,const char* password);
	// Disconnect.
	BOOL SetDisconnect(CTCpSocketMsgFormatter& formatter);		

	// Utility helper...
	static BOOL formatLoginString(BYTE* pbDest,const char* client,const char* password,int tokenNumber,int maxLength,int* length);

private:
	typedef CTCPSocketConnectMessage Base;
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketHeartbeatMessage_pr2 : public CTCPSocketMessage
{
public:
	CSocketHeartbeatMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketHeartbeatMessage_pr2();

	// Versturen van een heartbeat bericht...
	BOOL setHeartbeat(CTCpSocketMsgFormatter& formatter);

public:
	BOOL Parse(void) { return FALSE; }

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketHeartbeatStatusMessage_pr2 : public CTCPSocketMessage
{
public:
	CSocketHeartbeatStatusMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketHeartbeatStatusMessage_pr2();

	BOOL parse(CSocketMsgObservers& observers);

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketProtocolMessage_pr2 : public CTCPSocketMessage
{
public:
	CSocketProtocolMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketProtocolMessage_pr2();

	// Versturen van een CAN subscribe bericht.
	BOOL setCanBusSubscribe(CTCpSocketMsgFormatter& formatter,BOOL state);

	// Versturen van een DALI-bus subscribe bericht.
	BOOL setDaliBusSubscribe(CTCpSocketMsgFormatter& formatter, BOOL state);

	// Versturen van een CAN-Msg
	BOOL setCanBusMessage(CTCpSocketMsgFormatter& formatter,int nodeAddress, int unitAddress, int APDUHeader,
		int messageCode, int length, const BYTE* pbData);

	// Versturen van een DALI-Msg
	BOOL setDaliBusMessage(CTCpSocketMsgFormatter& formatter, int clientIdentifier, BYTE msgIdentifier, int prioLevel, int nrOfBits, const BYTE* pbData);

protected:
	BOOL setSubscribe(CTCpSocketMsgFormatter& formatter, BYTE bObjectMember, BOOL state);

public:
	BOOL Parse(void) { return FALSE; }

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// --> Ontvangen van CAN-Bercihten.
// --> Ontvangen van subscribe.
class CSocketProtocolRxMessage_pr2 : public CTCPSocketMessage
{
public:
	CSocketProtocolRxMessage_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketProtocolRxMessage_pr2();

	BOOL parse(CSocketMsgObservers& observers);

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketRequestNodeManagement_pr2 : public CTCPSocketMessage
{
public:
	CSocketRequestNodeManagement_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketRequestNodeManagement_pr2();

	BOOL sendRequestNodeInfo(CTCpSocketMsgFormatter& formatter);
	BOOL sendRequestMasterSupported(CTCpSocketMsgFormatter& formatter);		// sedert v16.26 - wordt nog niet gebruikt.
	BOOL sendRequestMaster(CTCpSocketMsgFormatter& formatter);				// sedert v16.26 - wordt nog niet gebruikt.

private:
	typedef CTCPSocketMessage Base;
};

// Sedert V16.26: Instellen als master.
class CSocketSetNodeManagement_pr2 : public CTCPSocketMessage
{
public:
	CSocketSetNodeManagement_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketSetNodeManagement_pr2();

	BOOL sendSetMaster(CTCpSocketMsgFormatter& formatter);

private:
	typedef CTCPSocketMessage Base;
};


class CSocketNodeManagementStatus_pr2 : public CTCPSocketMessage
{
public:
	CSocketNodeManagementStatus_pr2(CTCPSocketMessageData& refDataBuffer,ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketNodeManagementStatus_pr2();

	BOOL parse(CSocketMsgObservers& observers);

private:
	typedef CTCPSocketMessage Base;
};

/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketNodeMasterSetTask_pr2 : public CTCPSocketMessage
{
public:
	CSocketNodeMasterSetTask_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketNodeMasterSetTask_pr2();

	BOOL sendCancelTask(CTCpSocketMsgFormatter& formatter,const char* password);
	BOOL sendStartTask(CTCpSocketMsgFormatter& formatter,const char* password, TcpSocketHomeClient_Tasks::Task_t task);
	BOOL sendRequestState(CTCpSocketMsgFormatter& formatter);

protected:
	BOOL checkSyntaxPassword(const char* password);

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/
class CSocketNodeMasterTaskStatus_pr2 : public CTCPSocketMessage
{
public:
	CSocketNodeMasterTaskStatus_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketNodeMasterTaskStatus_pr2();

	BOOL parse(CSocketMsgObservers& observers);

protected:
	void notifyTaskStatus(CSocketMsgObservers& observers,TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status);
	TcpSocketHomeClient_Tasks::State_t convertToState(int);

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketNodeDatabase_pr2 : public CTCPSocketMessage
{
public:
	CSocketNodeDatabase_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketNodeDatabase_pr2();

	BOOL sendRequestStatus(CTCpSocketMsgFormatter& formatter);

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/

class CSocketNodeDatabaseStatus_pr2 : public CTCPSocketMessage
{
public:
	CSocketNodeDatabaseStatus_pr2(CTCPSocketMessageData& refDataBuffer, ITCPSocketMessageTransmitter* pTransmitter);
	virtual ~CSocketNodeDatabaseStatus_pr2();

	BOOL parse(CSocketMsgObservers& observers);

protected:	
	TcpSocketHomeClient_NodeDbase::Status_t convertToStatus(int status);

private:
	typedef CTCPSocketMessage Base;
};
/*****************************************************************/


/*****************************************************************/
#endif 
/*****************************************************************/