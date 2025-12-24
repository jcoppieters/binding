#ifndef INCLUDED_TCPSOCKET_BASE_H
#define INCLUDED_TCPSOCKET_BASE_H
/*****************************************************************/
#ifndef INCLUDED_TCPSOCKETBASE_TCPSOCKETWIN_H
	#include "tcpsocketwin.h"
#endif	
#ifndef INCLUDED_TCPSOCKET_CMN_H
	#include "tcpsocketcmn.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_TCPSOCKETMESSAGES_H
	#include "tcpsocketmessages.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_BASE_DEF_H
	#include "tcpsocketbase_def.h"
#endif 
#ifndef INCLUDED_ITCPSOCKET_BASE_H
	#include "itcpsocketbase.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETCOMMOBJECT_H
	#include "communication/tcpsocket/itcpsocketcommobject.h"			// Tijdelijk - verplaatsen naar ander component ???
#endif
#ifndef INCLUDED_TCPSOCKET_OPTIONS_H
	#include "tcpsocketoptions.h"
#endif
/*****************************************************************/
class CTCPSocketMessageAPI;
class CTCPSocketMsgQueue;
class CTCPSocketCommFrame;
class CTCPSocketBaseConnectFsmFunctions;

struct ITCPCommunicationObject_Host;
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketBase : public ITCPSocketMessageTransmitter 
{
public:
	CTCPSocketBase(ITCPCommunicationObject_Host& refHost);
	virtual ~CTCPSocketBase( );
	
	BOOL initInstance();	// Deze moet afzonderlijk aangeroepen worden - niet vanuit de ctor.
	void SetConnect();
	void SetDisconnect();
	int getSocketIdentifier() const { 
		return m_identifier;
	}

	// ITCPSocketMessageTransmitter
	virtual BOOL send(const BYTE* msg,int length);

protected:		
	BOOL socketConnected( );
	BOOL getSocketErrorMsg(CString& errorMsg,CString& errorMsgDetail);
	void subscribeObserver(ISocketMsgObserver* observer);
	void unsubscribeObserver(ISocketMsgObserver* observer);	
	SocketConnectState_t getSocketConnectState() const;
	CTCPSocketMessageAPI& getSocketMessageAPI();
	CSocketMsgObservers& getObservers();
	ITCPCommunicationObject_Host& getParentHost();
	BOOL setCommunicationSettings(const CTCPSocketCommSettings& settings);
	const CTCPSocketCommSettings& getCommunicationSettings() const {
		return m_settings;
	}

	ITcpSocketConnectFsmFunctions* temp_getTcpSocketConnectFsmFunctions();
	CString getStringState(SocketConnectState_t state);
	void setSocketConnectState(SocketConnectState_t state);

	const CTCPSocketOptions& getSocketOptions( ) const {
		return m_socketOptions;
	}
	void setSocketOptions(const CTCPSocketOptions& ref) {
		m_socketOptions = ref;
	}

private:
	BOOL createSocketWin(ITCPSocketWinHost& refHost);
	CTCPSocketWin::Result_t connectSocket(const char* strHostName,int socketport);
	CTCPSocketWin::Result_t closeSocket();
	CTCPSocketWin::Result_t sendSocketMsg(const BYTE* pbData,int length);

private:
	const int m_identifier;
	CTCPSocketCommSettings m_settings;
	ITCPCommunicationObject_Host& m_refHost;
	CSocketMsgObservers* m_pObservers;
	ITCPSocketWinHost* m_pSocketEvents;
	CTCPSocketWin* m_pSocket;
	CTCPSocketMessageAPI* m_pSocketMsg;
	SocketConnectState_t m_socketConnectState;
	CTCPSocketBaseConnectFsmFunctions* m_pFsmConnectFunctions;
	CTCPSocketOptions m_socketOptions;

	friend class CTCPSocketWinHost_Impl;
	friend class CTCPSocketBaseConnectFsmFunctions;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketBaseConnectFsmFunctions : public ITcpSocketConnectFsmFunctions {

public:
	CTCPSocketBaseConnectFsmFunctions(CTCPSocketBase& parent);
	virtual ~CTCPSocketBaseConnectFsmFunctions();

	virtual BOOL IsTcpConnectionOpen();
	virtual BOOL openTcpConnection();
	virtual BOOL closeTcpConnection();

	virtual void setStateTcpConnectLoginOn();
	virtual SocketConnectState_t getTcpSocketConnectState();

	virtual void subscribeMsgObserver(ISocketMsgObserver* observer);
	virtual void unsubscribeMsgObserver(ISocketMsgObserver* observer);

	virtual const CTCPSocketCommSettings& getTcpCommSettings() const;
	virtual void setTcpCommSettings(const CTCPSocketCommSettings& settings);

	virtual BOOL sendMsgRequestKey();
	virtual BOOL sendMsgConnect(const char* clientID,const char* password,int tokenNumber);
	virtual BOOL sendMsgConnect(const char* password);
	virtual BOOL sendMsgDisconnect();
	virtual BOOL sendMsgCanBusMsgSubscribe(BOOL state);
	virtual BOOL sendMsgDaliMsgSubscribe(BOOL state);
	virtual BOOL sendMsgRequestNodeInfo();	
	virtual BOOL sendMsgRequestMasterSupported();
	virtual BOOL sendMsgCfgAsMaster();

	virtual BOOL getCfgOptionSubscribeCanMsg() const {
		return m_parent.getSocketOptions().getOptionLoginSubscribeCanMsg();
	}	
	virtual BOOL getCfgOptionSecureLogin() const {
		return m_parent.getSocketOptions().getOptionLoginSecure();
	}
	virtual BOOL getCfgOptionSubscribeDaliMsg( ) const {
		return m_parent.getSocketOptions().getOptionLoginSubscribeDALIMsg();
	}

	// Sedert V17.03: Cloud bridge functies.
	virtual BOOL sendRawMessage(const char* msg);		// Wordt niet meer gebruikt.
	virtual BOOL isUseCloudServer() const;				// Connecteren met de cloud server of rechtstreeks connectie maken.
	virtual BOOL sendCloudServerLoginToken();			// Login token versturen naar de cloud server.

private:
	CTCPSocketBase& m_parent;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Events van de windows socket...
class CTCPSocketWinHost_Impl : public ITCPSocketWinHost
{
public:
	CTCPSocketWinHost_Impl(CTCPSocketBase& parent);
	virtual ~CTCPSocketWinHost_Impl();

	virtual void onSocketConnected(BOOL status);
	virtual void onDataRecieved(const BYTE* pbData,int length);
	virtual void onMsgInfo(const CString& str);
	virtual void onMsgError(const CString& str);
	virtual void onTracerDataSend(const CString& msg);
	virtual void onTracerDataRecieved(const CString& msg);
	virtual void onRequestSendKeepAlive( );	// zie NOTE_WINSOCK_KEEPALIVE

private:
	CTCPSocketBase& m_parent;
};
/*****************************************************************/


/*****************************************************************/
#endif 
/*****************************************************************/
