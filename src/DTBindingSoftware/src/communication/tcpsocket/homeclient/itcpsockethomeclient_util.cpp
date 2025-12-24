#include "stdafx.h"
#include "itcpsockethomeclient_util.h"

#include "../tcpsocketbase/tcpsocketmessages_api.h"
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/

static bool send(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,
				 BOOL (CTCPSocketMessageAPI::*pf)(const char* password),
				 const char* pwd)
{
	if (!ref.isConnected()) {
		ASSERT(ref.isConnected());
		return false;
	}
	return (TRUE == (refMsgApi.*pf)(pwd));
}

static bool send(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,
				 BOOL (CTCPSocketMessageAPI::*pf)())
{
	return (TRUE == (refMsgApi.*pf)());
}

/**************************************************************/
ITcpSocketHomeClient_Util::ITcpSocketHomeClient_Util() { }

bool ITcpSocketHomeClient_Util::startPnp(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd) {
	return send(ref,refMsgApi,&CTCPSocketMessageAPI::sendStartTaskPnp,pwd);
}
bool ITcpSocketHomeClient_Util::startBrowse(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd) {
	return send(ref,refMsgApi,&CTCPSocketMessageAPI::sendStartTaskBrowse,pwd);
}
bool ITcpSocketHomeClient_Util::startBrowseClear(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd) {
	return send(ref,refMsgApi,&CTCPSocketMessageAPI::sendStartTaskBrowseClear,pwd);
}
bool ITcpSocketHomeClient_Util::startUpdateNodeDatabase(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd) {
	return send(ref,refMsgApi,&CTCPSocketMessageAPI::sendStartTaskUpdateDbase,pwd);
}
bool ITcpSocketHomeClient_Util::sendCancelTask(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi,const char* pwd) {
	return send(ref,refMsgApi,&CTCPSocketMessageAPI::sendCancelTask,pwd);
}

bool ITcpSocketHomeClient_Util::sendRequestAppState(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi) {
	return send(ref,refMsgApi,&CTCPSocketMessageAPI::sendRequestTaskState);
}
bool ITcpSocketHomeClient_Util::sendRequestNodeDbaseStatus(ITcpSocketHomeClient& ref,CTCPSocketMessageAPI& refMsgApi) {
	return send(ref,refMsgApi,&CTCPSocketMessageAPI::sendRequestNodeDbaseState);
}
/**************************************************************/
