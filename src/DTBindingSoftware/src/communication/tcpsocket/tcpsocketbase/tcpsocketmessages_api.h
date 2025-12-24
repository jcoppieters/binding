#ifndef INCLUDED_TCPSOCKET_TCPSOCKETMESSAGES_API_H
#define INCLUDED_TCPSOCKET_TCPSOCKETMESSAGES_API_H
/*****************************************************************/
#include "tcpsocketcmn.h"
/*****************************************************************/
struct ITCPSocketMessageTransmitter;
struct CTCPSocketMessageData;
class CTCpSocketMsgFormatter;
class CTCPSocketMessageProcessor;
/*****************************************************************/
// API voor versturen en ontvangen van berichten.
class CTCPSocketMessageAPI
{
public:
	CTCPSocketMessageAPI(ITCPSocketMessageTransmitter& transmitter,CSocketMsgObservers& refObservers);
	virtual ~CTCPSocketMessageAPI();

	BOOL recieveMessage(ConnectionMethod_t method,const BYTE* pbData,int length);

	// Todo: Moeten ook nog op de oude manier kunnen connecteren voor connectie met de oude homeserver !

	// Transmit berichten...
	BOOL sendLoginRequestKey();
	BOOL sendConnect(const char* clientID,const char* password,int tokenNumber);	// Securedc login
	BOOL sendConnect(const char* password);	// Oude not secured login							
	BOOL sendDisconnect();
	BOOL sendHeartbeatRequest();
	BOOL sendCanBusMsgSubscribe(BOOL state);
	BOOL sendCanBusMsg(int nodeAddress, int unitAddress, int APDUHeader,int messageCode, int length, const BYTE* pbData);
	BOOL sendRequestNodeInfo();
	BOOL sendRequestMasterSupported( );
	BOOL sendCfgDeviceAsMaster();

	// Homeclient taken voor de master
	BOOL sendStartTaskPnp(const char* password);
	BOOL sendStartTaskBrowse(const char* password);
	BOOL sendStartTaskBrowseClear(const char* password);
	BOOL sendStartTaskUpdateDbase(const char* password);
	BOOL sendCancelTask(const char* password);
	BOOL sendRequestTaskState();
	BOOL sendRequestNodeDbaseState();

	// Homeclient DALI specifieke functies.	
	BOOL sendDaliMsgSubscribe(BOOL state);	// zie USE_TCPIP_DALI
	BOOL sendDALIMessage(int clientIdentifier, BYTE msgIdentifier, int prioLevel, int nrOfBits, const BYTE* pbData);	// zie USE_TCPIP_DALI

private:
	ITCPSocketMessageTransmitter& m_refTransmitter;
	CTCPSocketMessageProcessor* m_pMsgProcessor;
	CTCpSocketMsgFormatter* m_pFormatter;				// Formatter
	CTCpSocketMsgFormatter* m_pParser;
	CTCPSocketMessageData* m_pTcpipSocketMsgData;
};
/*****************************************************************/
#endif
/*****************************************************************/