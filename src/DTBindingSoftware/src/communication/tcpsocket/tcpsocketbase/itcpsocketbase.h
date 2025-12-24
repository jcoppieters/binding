#ifndef INCLUDED_ITCPSOCKET_BASE_H
#define INCLUDED_ITCPSOCKET_BASE_H
/*****************************************************************/
#ifndef INCLUDED_TCPSOCKET_BASE_DEF_H
	#include "tcpsocketbase_def.h"
#endif 
/*****************************************************************/
struct ISocketMsgObserver;
class CTCPSocketCommSettings;
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ITcpSocketConnectFsmFunctions {
	
	virtual ~ITcpSocketConnectFsmFunctions() = 0;

	virtual BOOL IsTcpConnectionOpen() = 0;
	virtual BOOL openTcpConnection() = 0;
	virtual BOOL closeTcpConnection() = 0;

	virtual void setStateTcpConnectLoginOn() = 0;
	virtual SocketConnectState_t getTcpSocketConnectState() = 0;

	virtual void subscribeMsgObserver(ISocketMsgObserver* observer) = 0;
	virtual void unsubscribeMsgObserver(ISocketMsgObserver* observer) = 0;

	virtual const CTCPSocketCommSettings& getTcpCommSettings() const = 0;
	virtual void setTcpCommSettings(const CTCPSocketCommSettings& settings) = 0;

	virtual BOOL sendMsgRequestKey() = 0;
	virtual BOOL sendMsgConnect(const char* clientID,const char* password,int tokenNumber) = 0;
	virtual BOOL sendMsgConnect(const char* password) = 0;
	virtual BOOL sendMsgDisconnect() = 0;
	virtual BOOL sendMsgCanBusMsgSubscribe(BOOL state) = 0;
	virtual BOOL sendMsgDaliMsgSubscribe(BOOL state) = 0;
	virtual BOOL sendMsgRequestNodeInfo() = 0;	
	virtual BOOL sendMsgRequestMasterSupported() = 0;
	virtual BOOL sendMsgCfgAsMaster() = 0;			// Deze wordt runtime gebruikt, niet in de fsm.

	// Configure opties opvragen hoe de connect FSM moet uitgevoerd worden.
	// Moet deze subscriben om CAN-Berichten te ontvangen.
	virtual BOOL getCfgOptionSubscribeCanMsg() const = 0;
	// Moet deze aanloggen via secure login of via de oude manier.
	virtual BOOL getCfgOptionSecureLogin() const = 0;
	// Subscribe for dali berichten?
	virtual BOOL getCfgOptionSubscribeDaliMsg( ) const = 0;

	// Sedert V17.03: Cloud bridge functies.
	virtual BOOL sendRawMessage(const char* msg) = 0;		// Wordt niet meer gebruikt.
	virtual BOOL isUseCloudServer() const = 0;				// Connecteren met de cloud server of rechtstreeks connectie maken.
	virtual BOOL sendCloudServerLoginToken( ) = 0;			// Login token versturen naar de cloud server.
};
/*****************************************************************/
#endif 
/*****************************************************************/