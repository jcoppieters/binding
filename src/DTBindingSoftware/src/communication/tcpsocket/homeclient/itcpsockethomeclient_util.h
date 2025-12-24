#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_UTIL_H
#define INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_UTIL_H
/**************************************************************/
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
	#include "itcpsockethomeclient.h"
#endif 
/**************************************************************/
class CTCPSocketMessageAPI;
/**************************************************************/

class ITcpSocketHomeClient_Util
{
public:
	ITcpSocketHomeClient_Util();

	// Versturen van berichten - helper functies.
	static bool startPnp(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd);
	static bool startBrowse(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd);
	static bool startBrowseClear(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd);
	static bool startUpdateNodeDatabase(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd);
	static bool sendCancelTask(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd);

	// Versturen van berichten - Request functies - helper functies.
	static bool sendRequestAppState(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi);
	static bool sendRequestNodeDbaseStatus(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi);
};

/**************************************************************/
#endif 
/**************************************************************/