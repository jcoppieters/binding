#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETCOMMOBJECT_H
#define INCLUDED_TCPSOCKET_ITCPSOCKETCOMMOBJECT_H
/**************************************************************/
#ifndef INCLUDED_ICOMMUNICATIONOBJECT_H
	#include "../base/icommunicationobject.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_BASE_DEF_H
	#include "tcpsocketbase/tcpsocketbase_def.h"
#endif 
#ifndef INCLUDED_ITCPSOCKET_BASE_H
	#include "tcpsocketbase/itcpsocketbase.h"
#endif 
/**************************************************************/
struct ISocketMsgObserver;
class CTCPSocketCommSettings;
class CTCPSocketMessageAPI;
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ICommunicationObject_Tcp : public ICommunicationObject 
{
	virtual ~ICommunicationObject_Tcp( ) = 0;
	virtual BOOL setSettings(const CTCPSocketCommSettings& settings) = 0;
	virtual int GetIdentifier() const = 0;		// Opvragen van de socket identifier.
	virtual ITcpSocketConnectFsmFunctions* getTcpSocketConnectFsmFunctions() = 0;
};
/**************************************************************/

/**************************************************************/
// CTCPSocketCommObject gebruikt deze om events / acties door te geven 
// naar de upper layer ( CCommunication )
//
// Geven telkens een socketIdentifier mee. Deze kan gebruikt worden om te controleren van welke 
// socket implementatie het bericht afkomstig is.
struct ITCPCommunicationObject_Host : public ICommunicationObject_Host
{
	virtual ~ITCPCommunicationObject_Host() = 0;

	virtual void onTcpReceivedMessage(int socketIdentifier) = 0;
	virtual void onTcpMessageSend(int socketIdentifier,ICommunicationFrame* pFrame) = 0;
	virtual void onTcpSocketClosed(int socketIdentifier) = 0;

	// Sedert V16.26: Nodig voor de DT18-PRO
	// Updaten van de server node info wanneer het logisch node address veranderd (instellen van master/slave node address).
	virtual void updateServerInfo(int nodeAddress, const char* name, int numberOfUnits, int nodeFlags) = 0;
};

/**************************************************************/
#endif
/**************************************************************/