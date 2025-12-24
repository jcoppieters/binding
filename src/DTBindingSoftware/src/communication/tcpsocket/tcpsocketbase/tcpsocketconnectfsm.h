#ifndef INCLUDED_COMMUNICATION_TCPIPCONNECTFSM_H
#define INCLUDED_COMMUNICATION_TCPIPCONNECTFSM_H
/*****************************************************************/
#ifndef INCLUDED_COMMUNICATION_CONNECTFSM_H
	#include "../../base/icommunicationconnectfsm.h"
#endif 
#ifndef INCLUDED_COMMUNICATION_SERVERINFO_H
	#include "../../base/communicationserverinfo.h"
#endif
#ifndef INCLUDED_TCPSOCKET_CMN_H
	#include "tcpsocketcmn.h"
#endif 
#ifndef INCLUDED_ITCPSOCKET_BASE_H
	#include "itcpsocketbase.h"
#endif 
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Sedert V16.26:
// Opvragen of de server node kan ingesteld worden als master.
// Deze info wordt doorgegeven aan de CCommunicationServerInfo klasse.
class CTCPSocketConnectFsm : public ICCommunicationConnectFsm, public ISocketMsgObserver
{
public:
	CTCPSocketConnectFsm(ICCommunicationConnectFsm_Host& refHost ,ITcpSocketConnectFsmFunctions* pCommObject, CCommunicationServerInfo& refServerInfo);
	virtual ~CTCPSocketConnectFsm();	

	virtual void EvOperationConnect(); 
	virtual void EvAfterTimeout();

public: // ISocketMsgObserver
	
	// Antwoord van de server op een request key...
	virtual BOOL onStatusConnectRequestKey(unsigned short protocolVersion,
		unsigned short softwareVersion,int tokenNumber,
		const char* szClientKey);

	// Antwoord passwoord ok.
	virtual BOOL onStatusConnectPassword(BOOL stateConnected);

	// Antwoord op een heartbeat request.
	virtual BOOL onStatusHeartbeat();

	// Antwoord op een CAN-RX Msg subsribe.
	virtual BOOL onStatusCanRxMsgSubscribe(BOOL status);

	// Ontvangen van een CAN-RX msg.
	virtual BOOL onRecievedCanRxMsg(int nodeAddress, int unitAddress,
		int messageCode, int length, const BYTE* pbData);

	// Ontvangen van de server node info.
	virtual BOOL onRecievedServerNodeInfo(int nodeAddress,unsigned long ulPhysicalAddress,const char* name,
		int numberOfUnits,int nodeType,int nodeFlags);

	// Sedert V16.26: Master functies.
	virtual BOOL onReceivedServerNodeStatusMaster(BOOL masterNode);
	virtual BOOL onReceivedServerNodeStatusMasterSupported(BOOL masterSupported);
	virtual BOOL onReceivedServerNodeMasterSet(BOOL success);

	// HomeClient functies.
	virtual BOOL onRecievedAppTaskStatus(TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) {
		return FALSE;
	}
	virtual BOOL onRecievedAppTaskCancel(TcpSocketHomeClient_Tasks::Status_t status) {
		return FALSE;
	}
	virtual BOOL onRecievedAppTaskInfo(const CString& msg) {
		return FALSE;
	}
	virtual BOOL onRecievedAppTaskState(TcpSocketHomeClient_Tasks::State_t state) {
		return FALSE;
	}
	virtual BOOL onRecievedNodedatabaseState(TcpSocketHomeClient_NodeDbase::Status_t status) {
		return FALSE;
	}

	// Antwoord op een DALI Subscribe bericht.	
	virtual BOOL onStatusDALIRxMsgSubscribe(BOOL status);

	virtual BOOL onStatusDALIResponse(int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData) {
		return FALSE;
	}
	virtual BOOL onRxDALIMessage(BYTE lengthBits, uint32_t recievedFrame) {
		return FALSE;
	}
	
	// Sedert V17.03: Integratie cloud server.
	// Ontvangen bericht van de cloud server bridge.		
	virtual BOOL onRecievedTunnelLoginResponse(BOOL status);

protected:
	void startTimer(int reloadValue =-1);
	void stopTimer();
	void notifyErrorTimeout();

	// Taken.
	BOOL taskRequestLoginKey( );
	BOOL taskSendLogin();						// Niet beveiligde login.
	BOOL taskSubscribeCanRxMsg( );
	BOOL taskSubscribeDALIRxMsg( );				// Sedert V16.35
	BOOL taskRequestNodeInfo( );
	BOOL taskRequestMasterSupported( );			// Sedert V16.26
	BOOL taskSendCloudServerIdentifier( );		// V17.03: Versturen van een uniek token naar de cloud server.
	
private:
	typedef enum {
		STATE_DISCONNECTED = 0,
		STATE_CONNECT_WAIT_SOCKETCONNECT,																
		STATE_CLOUDSERVER_WAIT_LOGIN_RESPONSE,			// V17.03: Na het versturen van het token naar de cloude server wachten op response van de server.
		STATE_WAIT_FOR_RESPONSE_TOKEN,					
		STATE_WAIT_FOR_RESPONSE_CONNECT,
		STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_CANMSG,	
		STATE_WAIT_FOR_RESPONSE_SUBSCRIBED_DALIMSG,
		STATE_WAIT_FOR_RESPONSE_NODEINFO,	
		STATE_WAIT_FOR_RESPONSE_MASTERSUPPORTED,		// Sedert V16.26
		STATE_CONNECT_ERROR,
		STATE_CONNECTED
	} State_t;

	State_t getState() const { return m_State; }
	void setState(State_t state);
	CString getString(State_t state);

private:
	ICCommunicationConnectFsm_Host& m_refHost;
	ITcpSocketConnectFsmFunctions* m_pCommObject;
	CCommunicationServerInfo& m_refServerInfo;		// Deze wordt ingevuld na het ontvangen van de info van de Duotecno server 
	State_t m_State;
	int m_timerValue;	
	int m_timerReloadValue;
};
/*****************************************************************/
#endif 
/*****************************************************************/