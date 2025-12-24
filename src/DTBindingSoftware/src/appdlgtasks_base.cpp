#include "stdafx.h"
#include "appdlgtasks_base.h"
#include "mainfrm.h"
#include "app/appremotecfg/AppRemoteCfgNodes.h"						// App wrapper
#include "appremotecfg/AppRemoteCfgLoginInfo.h"						// Library.
#include "appremotecfg/AppRemoteCfgNodeService.h"					// Library.
#include "appremotecfg/iappremotecfgapi.h"							// Library.
#include "communication/tcpsocket/tcpsocketbase/tcpsocketcmn.h"
#include "cannode.h"
/*****************************************************************/
#include "TMGlobals.h"
using namespace TMCommunication;
/*****************************************************************/
using namespace DUOTECNO;
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
bool CAppDlgTasksBaseHomeClientUser::getHomeClientSocketConnected(CCanNode* pNode) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}	
	return pSocketHomeClient->isConnected();
}

bool CAppDlgTasksBaseHomeClientUser::getHomeClientSocketStateIDLE(CCanNode* pNode) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return true;
	}	
	return pSocketHomeClient->isStateIdle();
}

// Disconnecteren.
bool CAppDlgTasksBaseHomeClientUser::disconnectHomeClientSocket(CCanNode* pNode) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}
	return pSocketHomeClient->disconnect();
}


// Uitvoeren van taken...
bool CAppDlgTasksBaseHomeClientUser::startHomeclientTaskPnp(CCanNode* pNode, const char* pwd) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}
	return pSocketHomeClient->startPnp(pwd);
}

bool CAppDlgTasksBaseHomeClientUser::startHomeclientTaskBrowse(CCanNode* pNode, const char* pwd) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}
	return pSocketHomeClient->startBrowse(pwd);
}

bool CAppDlgTasksBaseHomeClientUser::startHomeclientTaskBrowseClear(CCanNode* pNode, const char* pwd) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}
	return pSocketHomeClient->startBrowseClear(pwd);
}

bool CAppDlgTasksBaseHomeClientUser::startHomeclientTaskUpdateNodeDatabase(CCanNode* pNode, const char* pwd) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}
	return pSocketHomeClient->startUpdateNodeDatabade(pwd);
}

bool CAppDlgTasksBaseHomeClientUser::cancelHomeclientTask(CCanNode* pNode, const char* pwd) {
	ITcpSocketHomeClient* const pSocketHomeClient = CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(pNode);
	if (NULL == pSocketHomeClient) {
		return false;
	}
	return pSocketHomeClient->cancelTask(pwd);
}

//
bool CAppDlgTasksBaseHomeClientUser::getHomeClientConnectPossible(CCanNode* pNode, ConnectionMethod_t method) {
	if (0 == pNode) {
		ASSERT(0 != pNode);
		return false;
	}		
	AppRemoteCfgNodes& refCfg = ((CMainFrame*)AfxGetMainWnd())->getAppRemoteCfgNodeServices();
	bool result = false;


	// zie CHANGES_DALI_TCPIP_COMMUNICATION_SETTINGS
	// Sedert V16.48: Extra controle of er een paswoord aanwezig is.
	// Indien er geen password ingevuld is dan is geen verbinding mogelijk.
	CString strPassword;
	if (!AppRemoteCfgNodes_Util::getNodePassword(refCfg,pNode->getNodeAddress(),&strPassword)) {
		return false;
	}
	if (strPassword.IsEmpty()) {
		return false;
	}

	// Controleren of de lokale/remote instellingen ingevuld zijn.
	if (CONNECTION_METHOD_LOCAL == method) {
		result = AppRemoteCfgNodesTCPIPService::getSettingsLocalValid(refCfg,pNode->getNodeAddress());
	}
	else {
		result = AppRemoteCfgNodesTCPIPService::getSettingsRemoteValid(refCfg,pNode->getNodeAddress());
	}
	return result;
}



static bool helper_getSettings(AppRemoteCfgNodes& refCfg, ConnectionMethod_t method, int nodeAddress,
			CString* hostName, int* port, CString* password, CString* strCloudServerLoginToken)
{
	switch(method) {
		case CONNECTION_METHOD_LOCAL: {
			if (!AppRemoteCfgNodesTCPIPService::getSettingsLocal(refCfg, nodeAddress, hostName, port)) {
				return false;
			}
		} break;
		case CONNECTION_METHOD_REMOTE_DIRECT: {
			if (!AppRemoteCfgNodesTCPIPService::getSettingsRemote(refCfg, nodeAddress, hostName, port)) {
				return false;
			}
		} break;

		// TODO_CLOUDBRIDGE
		// Voorlopig wordt de login token gevormd door de dns_name:port
		// Het server adres en poort nummer zijn momenteel nog hardcoded (zie CLOUD_SERVER_TCPPORT, CLOUD_SERVER_ADDRESS)
		case CONNECTION_METHOD_REMOTE_CLOUD: {
			CString tempHostName;
			int tempPort = 0;
			if (!AppRemoteCfgNodesTCPIPService::getSettingsRemote(refCfg, nodeAddress, &tempHostName, &tempPort)) {
				return false;
			}
			ASSERT(!tempHostName.IsEmpty());
			ASSERT(tempPort >= 0);
			strCloudServerLoginToken->Format("[%s:%i]", tempHostName, tempPort);
			*port = CLOUD_SERVER_TCPPORT;													
			*hostName = CLOUD_SERVER_ADDRESS;												
		} break;
	}
	if (!AppRemoteCfgNodes_Util::getNodePassword(refCfg, nodeAddress, password)) {
		return false;
	}
	ASSERT(!password->IsEmpty());
	ASSERT(!hostName->IsEmpty());
	ASSERT(*port >= 0 && *port <= 0xFFFF);
	return true;
}

bool CAppDlgTasksBaseHomeClientUser::connectHomeClient(CCanNode* pNode, ConnectionMethod_t method) {
	if (0 == pNode) {
		ASSERT(0 != pNode);
		return false;
	}
	AppRemoteCfgNodes& refCfg = ((CMainFrame*)AfxGetMainWnd())->getAppRemoteCfgNodeServices();
	const int nodeAddress = pNode->getNodeAddress();
	ASSERT(nodeAddress>=0 && nodeAddress<=0xff);	

	CString password;
	CString hostName;
	CString strCloudServerLoginToken;
	int port = -1;
	if (!helper_getSettings(refCfg, method, nodeAddress, &hostName, &port, &password, &strCloudServerLoginToken)) {
		return false;
	}
	ITcpSocketHomeClient* const pSocketHomeClient = TMGetCommunicationApp()->getSocketHomeClient(*pNode);
	if (NULL == pSocketHomeClient) {
		ASSERT(NULL != pSocketHomeClient);
		return false;
	}
	return pSocketHomeClient->connect(method, hostName, port, password, strCloudServerLoginToken);
}


// Controle van de hostname & poort waarmee we geconnecteerd zijn.
// Deze vergelijke met deze van de service.
// Hierdoor kunnen we bepalen of we remote/lokaal geconnecteerd zijn.
//
// Aanpassing V17.05: Ondersteuning van de cloud.
HomeClientConnectionState_t CAppDlgTasksBaseHomeClientUser::getHomeClientConnectionState(CCanNode* pNode) {
	if (NULL == pNode) {	
		ASSERT(NULL != pNode);
		return HOMECLIENT_STATE_DISCONNECTED;
	}
	ITcpSocketHomeClient* const pSocketHomeClient = TMGetCommunicationApp()->getSocketHomeClient(*pNode);
	if (NULL == pSocketHomeClient) {
		ASSERT(NULL != pSocketHomeClient);
		return HOMECLIENT_STATE_DISCONNECTED;
	}

	if (!pSocketHomeClient->isConnected()) {
		return HOMECLIENT_STATE_DISCONNECTED;
	}

	const CTCPSocketCommSettings& refSettings = pSocketHomeClient->getTcpSocketCommSettings();
	const CString& hostName = refSettings.getHostName();
	const CString& pwd = refSettings.getPassword();
	const int port = refSettings.getPort();

	// Verkeerde weergave connectie local/remote.
	// Ondersteuning van de cloud.
	HomeClientConnectionState_t state = HOMECLIENT_STATE_CONNECTED_LOCAL_IP;	
	if (0 == hostName.Compare(CLOUD_SERVER_ADDRESS) && (port == CLOUD_SERVER_TCPPORT)) {
		// Bij connectie met de cloud is het host address en poort deze van de cloud server.		
		state = HOMECLIENT_STATE_CONNECTED_REMOTE_DNS;
	}
	else {	
		AppRemoteCfgNodes& refCfg = ((CMainFrame*)AfxGetMainWnd())->getAppRemoteCfgNodeServices();
		CString servicehostName;
		int servicePort = -1;
		if (AppRemoteCfgNodesTCPIPService::getSettingsRemote(refCfg,pNode->getNodeAddress(),&servicehostName,&servicePort)) {
			if ((0 == servicehostName.Compare(hostName)) && (servicePort == port)) {
				 state = HOMECLIENT_STATE_CONNECTED_REMOTE_DNS;
			}
		}
	}
	return state;
}

// Protected.
ITcpSocketHomeClient* CAppDlgTasksBaseHomeClientUser::getSocketHomeClient(CCanNode* pNode) {
	if (NULL == pNode) {		
		return NULL;
	}
	ITcpSocketHomeClient* const pSocketHomeClient = TMGetCommunicationApp()->getSocketHomeClient(*pNode);
	ASSERT(NULL != pSocketHomeClient);
	return pSocketHomeClient;
}

/*****************************************************************/
