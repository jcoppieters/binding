#ifndef INCLUDED_TCPSOCKET_CMN_H
#define INCLUDED_TCPSOCKET_CMN_H
/*****************************************************************/
#ifndef INCLUDED_COMMUNICATION_DEF_H
	#include "communication\base\communication_def.h"
#endif
#ifndef INCLUDED_TCPSOCKET_TEVENTLISTENER_H
	#include "TEventListeners.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_TCPSOCKETHOMECLIENTCMN_H
	#include "communication/tcpsocket/homeclient/tcpsockethomeclientcmn.h"
#endif 
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ISocketMsgObserver
{
public:
	virtual ~ISocketMsgObserver( ) = 0;

	// Antwoord van de server op een request key...
	virtual BOOL onStatusConnectRequestKey(unsigned short protocolVersion,
		unsigned short softwareVersion,int tokenNumber,
		const char* szClientKey) = 0;

	// Antwoord passwoord ok.
	virtual BOOL onStatusConnectPassword(BOOL stateConnected) = 0;

	// Antwoord op een heartbeat request.
	virtual BOOL onStatusHeartbeat() = 0;

	// Antwoord op een CAN-RX Msg subsribe.
	virtual BOOL onStatusCanRxMsgSubscribe(BOOL status) = 0;

	// Ontvangen van een CAN-RX msg.
	virtual BOOL onRecievedCanRxMsg(int nodeAddress, int unitAddress,
		int messageCode, int length, const BYTE* pbData) = 0;

	// Ontvangen van de server node info.
	virtual BOOL onRecievedServerNodeInfo(int nodeAddress,unsigned long ulPhysicalAddress,const char* name,
		int numberOfUnits,int nodeType,int nodeFlags) = 0;

	// Sedert V16.26: Master functies.
	virtual BOOL onReceivedServerNodeStatusMaster(BOOL masterNode) = 0;
	virtual BOOL onReceivedServerNodeStatusMasterSupported(BOOL masterSupported) = 0;
	virtual BOOL onReceivedServerNodeMasterSet(BOOL success) = 0;

	// HomeClient functies: Ontvangen van een status verandering van een taak.
	virtual BOOL onRecievedAppTaskStatus(TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) = 0;
	virtual BOOL onRecievedAppTaskCancel(TcpSocketHomeClient_Tasks::Status_t status) = 0;

	// HomeClient functies: Ontvangen van info bericht.
	virtual BOOL onRecievedAppTaskInfo(const CString& msg) = 0;

	// HomeClient functies: Ontvangen van de applicatie toestand.
	virtual BOOL onRecievedAppTaskState(TcpSocketHomeClient_Tasks::State_t state) = 0;

	// HomeClient functie: Nodedatabase toestand.
	virtual BOOL onRecievedNodedatabaseState(TcpSocketHomeClient_NodeDbase::Status_t status) = 0;

	// Integratie DALI in TCP/IP protocol.
	virtual BOOL onStatusDALIRxMsgSubscribe(BOOL status) = 0;
	virtual BOOL onStatusDALIResponse(int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData) = 0;
	virtual BOOL onRxDALIMessage(BYTE lengthBits, uint32_t recievedFrame) = 0;

	// Ontvangen bericht van de cloud server bridge.
	virtual BOOL onRecievedTunnelLoginResponse(BOOL status) = 0;

};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketCommSettings
{
public:
	CTCPSocketCommSettings();
	CTCPSocketCommSettings(ConnectionMethod_t method, const CString& hostName, int port, const CString& password, const CString& token);
	~CTCPSocketCommSettings();

	ConnectionMethod_t getConnectionMethod() const;
	const CString& getHostName() const;
	const CString& getPassword() const;
	const CString& getCloudServerToken() const;
	int getPort() const;

private:
	ConnectionMethod_t m_method;
	CString m_hostName;
	CString m_password;
	int m_port;		
	CString m_CloudServerToken;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
typedef TEventListeners<ISocketMsgObserver*> CSocketMsgObservers;
/*****************************************************************/

/*****************************************************************/
#endif
/*****************************************************************/