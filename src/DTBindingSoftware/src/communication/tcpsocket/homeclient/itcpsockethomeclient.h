#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
#define INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
/**************************************************************/
#ifndef INCLUDED_COMMUNICATION_DEF_H
	#include "communication\base\communication_def.h"
#endif
#ifndef INCLUDED_TCPSOCKET_TCPSOCKETHOMECLIENTCMN_H
	#include "tcpsockethomeclientcmn.h"
#endif 
/**************************************************************/
struct ITcpSocketHomeClient_Host;
class CTCPSocketCommSettings;
/**************************************************************/
struct ITcpSocketHomeClient {

	virtual ~ITcpSocketHomeClient() = 0;

	// Functies connect & disconnect enkel nodig wnr connecties met een andere master
	virtual bool connect(ConnectionMethod_t mehod, const char* hostName, int port, const char* password, 
						 const char* cloudServerLoginToken) = 0;

	virtual bool disconnect( ) = 0;

	// Opvragen van de communicatie settings...
	virtual const CTCPSocketCommSettings& getTcpSocketCommSettings() const = 0;	

	virtual bool isConnected() = 0;
	virtual bool isMaster() = 0;
	virtual bool isStateIdle() = 0;		// Zijn er taken bezig ?

	virtual bool startPnp(const char* pwd) = 0;
	virtual bool startBrowse(const char* pwd) = 0;
	virtual bool startBrowseClear(const char* pwd) = 0;
	virtual bool startUpdateNodeDatabade(const char* pwd) = 0;
	virtual bool cancelTask(const char* pwd) = 0;

	// Zie USE_TCPIP_DALI
	virtual bool sendSubscribeDALI() = 0;
	virtual bool sendMsgDali(int clientIdentifier, BYTE msgIdentifier, int prioLevel, int nrOfBits, const BYTE* pbData) = 0;

	virtual void subscribe(ITcpSocketHomeClient_Host* pHost) = 0;
	virtual void unSubscribe(ITcpSocketHomeClient_Host* pHost) = 0;

	// Opvragen met welke taak de applicatie bezig is.
	virtual TcpSocketHomeClient_Tasks::State_t getAppTaskState( ) = 0;
	virtual TcpSocketHomeClient_NodeDbase::Status_t getAppDbaseStatus() = 0;

	// Sedert V16.36:	
	// Doorgeven aan de observers dat de toestand van de socket veranderd is.
	virtual void notifyConnectStatusChanged( ) = 0;
};
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
struct ITcpSocketHomeClient_Host {


	typedef enum {
		HOMECLIENT_MSG_CATEGORY_CONNECT_FSM = 0,			// Bericht van de socket connect fsm.
		HOMECLIENT_MSG_CATEGORY_MASTERTASK_FSM = 1			// Bericht van de socket master tasks.
	} CategoryMsg_t;


	// In de functies een 'id' meegeven - Deze kan gebruikt worden wanneer we meerdere 
	// homeclient sockets gebruiken.
	virtual ~ITcpSocketHomeClient_Host() = 0;

	virtual void onStateConnectChanged(int id,bool connected) = 0;
	virtual void onStateChanged(int id, TcpSocketHomeClient_Tasks::State_t state) = 0;
	virtual void onSocketHomeClientMsgInfo(int id, CategoryMsg_t category, const CString& msg) = 0;
	virtual void onSocketHomeClientMsgError(int id, CategoryMsg_t category, const CString& msg) = 0;
	virtual void onTaskStatus(int id, TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) = 0;
	virtual void onTaskCancelStatus(int id, TcpSocketHomeClient_Tasks::Status_t status) = 0;
	virtual void onNodeDbaseStatus(int id, TcpSocketHomeClient_NodeDbase::Status_t status) = 0;

	// Integratie DALI in TCP/IP protocol.
	virtual void onStatusDALIRxMsgSubscribe(int id,bool status) = 0;
	virtual void onStatusDALIResponse(int id, int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData) = 0;
	virtual void onRxDALIMessage(int id, BYTE lengthBits, uint32_t recievedFrame) = 0;
};

/**************************************************************/
#endif 
/**************************************************************/