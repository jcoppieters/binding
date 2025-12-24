#ifndef INCLUDED_TCPSOCKET_TCPSOCKETHOMECLIENT_H
#define INCLUDED_TCPSOCKET_TCPSOCKETHOMECLIENT_H
/**************************************************************/
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
	#include "itcpsockethomeclient.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_BASE_H
	#include "../tcpsocketbase/tcpsocketbase.h"
#endif 
#ifndef INCLUDED_COMMUNICATION_CONNECTFSM_H
	#include "../../base/icommunicationconnectfsm.h"			// ???????????????????????
#endif 
/**************************************************************/
struct ITCPCommunicationObject_Host;
class CCommunicationServerInfo;
class CTCPSocketConnectFsm;
/**************************************************************/

// Implementatie van de socket homeclient om master functies uit te voeren...
class CTCPSocketHomeClient : public CWnd,	/* nodig voor de timers */
							 public CTCPSocketBase, public ITcpSocketHomeClient, public ISocketMsgObserver, public ICCommunicationConnectFsm_Host
{
public:
	CTCPSocketHomeClient(ITCPCommunicationObject_Host& refHost,const CTCPSocketOptions& refSocketOptions);
	virtual ~CTCPSocketHomeClient();

	// Connect & disconnect
	virtual bool connect(ConnectionMethod_t mehod, const char* hostName,int port,const char* paswoord,const char* cloudServerLoginToken);
	virtual bool disconnect( );

	// Opvragen van de communicatie settings...
	virtual const CTCPSocketCommSettings& getTcpSocketCommSettings() const {
		return this->CTCPSocketBase::getCommunicationSettings();
	}

	// ITcpSocketHomeClient
	virtual bool isConnected();
	virtual bool isMaster();
	virtual bool isStateIdle();

	virtual bool startPnp(const char* pwd);
	virtual bool startBrowse(const char* pwd);
	virtual bool startBrowseClear(const char* pwd);
	virtual bool startUpdateNodeDatabade(const char* pwd);
	virtual bool cancelTask(const char* pwd);

	// Zie USE_TCPIP_DALI
	virtual bool sendSubscribeDALI();
	virtual bool sendMsgDali(int clientIdentifier, BYTE msgIdentifier, int prioLevel, int nrOfBits, const BYTE* pbData);

	virtual void subscribe(ITcpSocketHomeClient_Host* pHost);
	virtual void unSubscribe(ITcpSocketHomeClient_Host* pHost);

	virtual TcpSocketHomeClient_Tasks::State_t getAppTaskState( ) {
		return m_stateAppTasks;
	}
	virtual TcpSocketHomeClient_NodeDbase::Status_t getAppDbaseStatus() {
		return m_statusDbase;
	}

	// Sedert V16.36:	
	virtual void notifyConnectStatusChanged( );

public: // ISocketMsgObserver
	// Observer berichten: De gewone msg Observer berichten.						
	virtual BOOL onStatusConnectRequestKey(unsigned short protocolVersion,
		unsigned short softwareVersion,int tokenNumber,
		const char* szClientKey);
	virtual BOOL onStatusConnectPassword(BOOL stateConnected);
	virtual BOOL onStatusHeartbeat();	
	virtual BOOL onStatusCanRxMsgSubscribe(BOOL status);
	virtual BOOL onRecievedCanRxMsg(int nodeAddress, int unitAddress,
		int messageCode, int length, const BYTE* pbData);
	virtual BOOL onRecievedServerNodeInfo(int nodeAddress,unsigned long ulPhysicalAddress,const char* name,
		int numberOfUnits,int nodeType,int nodeFlags);
	// Sedert V16.26: Master functies.
	virtual BOOL onReceivedServerNodeStatusMaster(BOOL masterNode);
	virtual BOOL onReceivedServerNodeStatusMasterSupported(BOOL masterSupported);
	virtual BOOL onReceivedServerNodeMasterSet(BOOL success);
	// Observer berichten: Specifieke homeClient functies.
	virtual BOOL onRecievedAppTaskStatus(TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status);
	virtual BOOL onRecievedAppTaskCancel(TcpSocketHomeClient_Tasks::Status_t status);
	virtual BOOL onRecievedAppTaskInfo(const CString& msg);
	virtual BOOL onRecievedAppTaskState(TcpSocketHomeClient_Tasks::State_t state);
	virtual BOOL onRecievedNodedatabaseState(TcpSocketHomeClient_NodeDbase::Status_t status);

	// Integratie DALI in TCP/IP protocol.
	virtual BOOL onStatusDALIRxMsgSubscribe(BOOL status);
	virtual BOOL onStatusDALIResponse(int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData);
	virtual BOOL onRxDALIMessage(BYTE lengthBits, uint32_t recievedFrame);

	// Ontvangen berichten van de cloud server bridge.
	virtual BOOL onRecievedTunnelLoginResponse(BOOL status);


public:	// ICCommunicationConnectFsm_Host: Nodig voor de connect fsm. 
	virtual void displayMsgError(const CString& msg);
	virtual void displayMsgInfo(const CString& msg);
	virtual void startTimer(unsigned int timerID,unsigned int value);
	virtual void stopTimer(unsigned int timerID);
	virtual void notifyFsmDone( );
	virtual void notifyFsmError( );
	bool checkFirmwareVersion(int version);
	bool checkNodeProperties(int nodeAddress, unsigned long ulPhysicalAddress, int nodeType, int nodeFlags);

protected:
	enum { TIMER_ID_CONNECTFSM = 1 };
	HWND Create();	// Window create functie.	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void PostNcDestroy();

private:
	CCommunicationServerInfo* m_pServerInfo;
	CTCPSocketConnectFsm* m_pConnectFsm;
	CList<ITcpSocketHomeClient_Host*> m_listObservers;
	TcpSocketHomeClient_Tasks::State_t m_stateAppTasks;
	TcpSocketHomeClient_NodeDbase::Status_t m_statusDbase;
	bool m_flagMaster;
	bool m_initDone;
	bool m_stateIdle;
};

/**************************************************************/
#endif 
/**************************************************************/