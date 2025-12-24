#include "stdafx.h"
#include "tcpsocketmessages_api.h"
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
CTCPSocketMessageAPI::CTCPSocketMessageAPI(ITCPSocketMessageTransmitter& transmitter,CSocketMsgObservers& refObservers) :  
	m_refTransmitter(transmitter) , m_pFormatter(new CTCpSocketMsgFormatter()), 
	m_pParser(new CTCpSocketMsgFormatter()), m_pTcpipSocketMsgData(new CTCPSocketMessageData()),
	m_pMsgProcessor(NULL)
{ 
	ASSERT(NULL != m_pParser);

	m_pMsgProcessor = new CTCPSocketMessageProcessor(refObservers,*m_pParser);
	ASSERT(NULL != m_pMsgProcessor);
}

CTCPSocketMessageAPI::~CTCPSocketMessageAPI() { 
	if (NULL != m_pFormatter) {
		delete m_pFormatter;
		m_pFormatter = NULL;
	}
	if (NULL != m_pTcpipSocketMsgData) {
		delete m_pTcpipSocketMsgData;
		m_pTcpipSocketMsgData = NULL;
	}
	if (NULL != m_pParser) {
		delete m_pParser;
		m_pParser = NULL;
	}
	if (NULL != m_pMsgProcessor) {
		delete m_pMsgProcessor;
		m_pMsgProcessor = NULL;
	}
}

BOOL CTCPSocketMessageAPI::recieveMessage(ConnectionMethod_t method, const BYTE* pbData,int length) {
	if (NULL == m_pMsgProcessor) {
		ASSERT(NULL != m_pMsgProcessor);
		return FALSE;
	}
	return m_pMsgProcessor->RecieveMessage(method, m_pTcpipSocketMsgData,pbData,length);
}

BOOL CTCPSocketMessageAPI::sendLoginRequestKey() {
	CSocketConnectMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.setRequestKey(*m_pFormatter);
}

BOOL CTCPSocketMessageAPI::sendConnect(const char* clientID,const char* password,int tokenNumber) {
	CSocketConnectMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	BYTE dataEncoded[128];
	int length = -1;
	memset(dataEncoded,0,sizeof(dataEncoded));
	if (!msg.formatLoginString(dataEncoded,clientID,password,tokenNumber,sizeof(dataEncoded),&length)) {
		return FALSE;
	}				
	return msg.setConnect(*m_pFormatter,length,dataEncoded);	
}

BOOL CTCPSocketMessageAPI::sendConnect(const char* password) {
	ASSERT(NULL != password);
	CSocketConnectMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.setConnect(*m_pFormatter,password);
}

BOOL CTCPSocketMessageAPI::sendDisconnect() {
	CSocketConnectMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.SetDisconnect(*m_pFormatter);
}

BOOL CTCPSocketMessageAPI::sendHeartbeatRequest() {
	CSocketHeartbeatMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.setHeartbeat(*m_pFormatter);
}

BOOL CTCPSocketMessageAPI::sendCanBusMsgSubscribe(BOOL state) {
	CSocketProtocolMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.setCanBusSubscribe(*m_pFormatter,state);
}

BOOL CTCPSocketMessageAPI::sendCanBusMsg(int nodeAddress, int unitAddress, int APDUHeader,int messageCode, int length, const BYTE* pbData) {
	CSocketProtocolMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.setCanBusMessage(*m_pFormatter,nodeAddress,unitAddress,APDUHeader,messageCode,length,pbData);
}

BOOL CTCPSocketMessageAPI::sendRequestNodeInfo() {
	CSocketRequestNodeManagement_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendRequestNodeInfo(*m_pFormatter);
}

BOOL CTCPSocketMessageAPI::sendRequestMasterSupported( ) {
	CSocketRequestNodeManagement_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendRequestMasterSupported(*m_pFormatter);
}

BOOL CTCPSocketMessageAPI::sendCfgDeviceAsMaster() {
	CSocketSetNodeManagement_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendSetMaster(*m_pFormatter);
}

/***************** Homeclient taken voor de master ***************/
BOOL CTCPSocketMessageAPI::sendStartTaskPnp(const char* password) {
	CSocketNodeMasterSetTask_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendStartTask(*m_pFormatter, password, TcpSocketHomeClient_Tasks::TASK_PNP);
}

BOOL CTCPSocketMessageAPI::sendStartTaskBrowse(const char* password)  {
	CSocketNodeMasterSetTask_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendStartTask(*m_pFormatter, password, TcpSocketHomeClient_Tasks::TASK_BROWSE);
}

BOOL CTCPSocketMessageAPI::sendStartTaskBrowseClear(const char* password)  {
	CSocketNodeMasterSetTask_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendStartTask(*m_pFormatter, password, TcpSocketHomeClient_Tasks::TASK_BROWSE_CLEAR);
}

BOOL CTCPSocketMessageAPI::sendStartTaskUpdateDbase(const char* password)  {
	CSocketNodeMasterSetTask_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendStartTask(*m_pFormatter, password, TcpSocketHomeClient_Tasks::TASK_UPDATE_DBASE);
}

BOOL CTCPSocketMessageAPI::sendCancelTask(const char* password) {
	CSocketNodeMasterSetTask_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendCancelTask(*m_pFormatter, password);
}

BOOL CTCPSocketMessageAPI::sendRequestTaskState() {
	CSocketNodeMasterSetTask_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendRequestState(*m_pFormatter);
}

BOOL CTCPSocketMessageAPI::sendRequestNodeDbaseState() {
	CSocketNodeDatabase_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.sendRequestStatus(*m_pFormatter);
}

// zie USE_TCPIP_DALI
BOOL CTCPSocketMessageAPI::sendDaliMsgSubscribe(BOOL state) {
	CSocketProtocolMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.setDaliBusSubscribe(*m_pFormatter, state);
}

// zie USE_TCPIP_DALI
BOOL CTCPSocketMessageAPI::sendDALIMessage(int clientIdentifier, BYTE msgIdentifier, int prioLevel, int nrOfBits, const BYTE* pbData) {
	CSocketProtocolMessage_pr2 msg(*m_pTcpipSocketMsgData, &m_refTransmitter);
	return msg.setDaliBusMessage(*m_pFormatter, clientIdentifier, msgIdentifier, prioLevel, nrOfBits, pbData);
}

/*****************************************************************/