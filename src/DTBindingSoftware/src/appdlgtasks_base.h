#ifndef INCLUDED_APPDLGTASKS_BASE_H
#define INCLUDED_APPDLGTASKS_BASE_H
/*****************************************************************/
#ifndef INCLUDED_IAPPDLGTASKS_H
	#include "iappdlgtasks.h"
#endif 
/*****************************************************************/
class CCanNode;
struct ITcpSocketHomeClient;
/*****************************************************************/

/*****************************************************************/
struct CAppDlgTasksBaseHomeClientUser {

public:
	// Opvragen of de socket geconnecteerd is.
	static bool getHomeClientSocketConnected(CCanNode* pNode);

	// Opvragen of de socket in IDLE toestand is.
	static bool getHomeClientSocketStateIDLE(CCanNode* pNode);

	// Disconnecteren.
	static bool disconnectHomeClientSocket(CCanNode* pNode);

	// Uitvoeren van taken...
	static bool startHomeclientTaskPnp(CCanNode* pNode, const char* pwd);
	static bool startHomeclientTaskBrowse(CCanNode* pNode, const char* pwd);
	static bool startHomeclientTaskBrowseClear(CCanNode* pNode, const char* pwd);
	static bool startHomeclientTaskUpdateNodeDatabase(CCanNode* pNode, const char* pwd);
	static bool cancelHomeclientTask(CCanNode* pNode, const char* pwd);

	// Opvragen of er kan geconnecteerd worden met de homeClient.
	// Deze functie doet een controle op de instellingen.
	static bool getHomeClientConnectPossible(CCanNode* pNode, ConnectionMethod_t method);
	
	// Connecteren met de homeclient via deze socket.
	static bool connectHomeClient(CCanNode* pNode, ConnectionMethod_t method);

	// Opvragen hoe de socket van de homeclient geconnecteerd is.
	// return waarde: zie enum
	static HomeClientConnectionState_t getHomeClientConnectionState(CCanNode* pNode);

	// Opvragen van de homeclient socket die bij de node hoort.
	// Oppassen kan NULL zijn.
	static ITcpSocketHomeClient* getSocketHomeClient(CCanNode* pNode);
};

/*****************************************************************/
#endif 
/*****************************************************************/