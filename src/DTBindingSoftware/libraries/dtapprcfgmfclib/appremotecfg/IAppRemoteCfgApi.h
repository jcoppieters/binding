#ifndef INCLUDED_IAPPREMOTECFGAPI_H
#define INCLUDED_IAPPREMOTECFGAPI_H
/**************************************************************/
#ifndef INCLUDED_APPREMOTECFGCMN_H
	#include "appRemoteCfgCmn.h"
#endif 
#ifndef INCLUDED_APPREMOTECFG_DEF_H
	#include "appremotecfgdef.h"
#endif 
#ifndef _LIST_
	#include <list>			// std::list
#endif 
/**************************************************************/

/**************************************************************/

/**************************************************************************************************
 * - V1.2: Used by binding software V15.10.
 *	- Release date: 2021/10/28. 
 *  - changes: Eerste versie.
 *
 * - V1.3: Used by binding software V15.11.
 *	- Release date: 2021/11/18
 *	- changes: Upload van de namen.
 *
 * - V1.4: Used by binding software V15.12.
 *	- Release date: 2021/11/19
 *	- changes: 
 *	  - Probleem met upload van de backups: Moeten abs path nemen ipv relatief path.
 *
 * - V1.5: Used by binding software V15.13.
 *	- Release date: 2021/12/14
 *	- Changes: 
 *    - Credentials worden opgeslagen in een *.dat bestand en beveiligd met een private key.
 *	  - Server Info: Opvragen of de connectie beveiligd is.
 *	  - HTTP request getlogInfo: JSon data omzetten van UTF-8 naar een ANSI string.
 *
 * - V1.6: Used by binding software V15.14.
 *	- Release date: 2021/12/16
 *	- Changes: 
 *	  - Configuratie van servers per project file.
 *	  - Connectie procedure aangepast.
 *
 * - V1.7: Used by binding software V16.09
 *	- Release date: 2022/09/22
 *	- Changes: 
 *	  - class AppRemoteCfgLoginInfo + Json Login info load / save.
 *
 * - V1.8: Used by binding software V16.10
 *	- Release date: 2022/09/30
 *	- Changes: 
 *	  - Nodeservices: Bijhouden van een lijst van nodes + hun services.
 *		Data wordt ingeladen/opgeslagen in een secured json bestand.
 *	    zie klassen: AppRemoteCfgNodes, AppRemoteCfgNode, AppRemoteCfgNodeService
 *
 * - V2.0: 
 *	- Release date:
 *	- Changes: 
 *	  - Firmware versie info ophalen van de cloud.
 *	  - Json parser: Controle op type.
 *	  
 * - V2.1: Used by binding software V16.19
 *	- Release date: 20230321
 *	- Changes:
 *		- Export van de connect login info.
 *
 * - V2.2: Used by binding software V16.19
 *	- Start date: 2023/03/21
 *	- Release date: 2024/04/28
 *	- Changes:
 *		- Added project to git + rebuild of libraries.
 *
 * - V2.3: Used by binding software V16.28
 *	- Start date: 2023/06/12
 *	- Release date: 
 *	- Changes:
 *		- AppRemoteCfgNodes: Device zoeken dmv het physical node address.
 *		- export van de node services in een json formaat.
 *		- Utility functies voor AppRemoteCfgNodes verwijderd (nu in de binding software herwerkt.)
 *		- Connect info: load/save and export server logisch en fysisch adres.
 *
 ************************************************************************************************/

/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgServerInfo;
class AppRemoteCfgServers;
class AppRemoteCfgServer;
class AppRemoteCfgLoginInfo;
class AppRemoteCfgNodes;
class AppRemoteCfgFirmwareVersionInfo;
/**************************************************************/
/**************************************************************/
/**************************************************************/
class IAppRemoteCfgApi
{
public:
	virtual ~IAppRemoteCfgApi() = 0;

	typedef enum {		
		RESULT_SUCCESS = 1,
		RESULT_EC_FAILED = -1,
		RESULT_EC_PARAMS = -2,
		RESULT_EC_BUSY = -101,
		RESULT_EC_FSM = -102,	
		RESULT_EC_NOT_CONNECTED = -200,
		RESULT_EC_CONNECTED = -201,
		RESULT_EC_AUTH_PASSWORD = -301,
	} Result_t;

	// Opvragen van de software versie.
	virtual void getVersion(int* major,int* minor) = 0;

	// De GUI koppelen en ontkoppelen (nullptr)
	virtual void setSubscriberGui(IAppRemoteCfgGUI* ptrGui) = 0;
	virtual void setSubscriberSystemEvents(IAppRemoteCfgSystemEvents* ptr) = 0;

	// Connect info van de host.
	// Indien een secure zal probereren de credentials in te lezen, indien mislukt moet er een paswoord meegegeven worden...
	virtual Result_t setHostInfo(const char* hostName,int port,bool useHttpsSecure = false,const char* password = 0) = 0;

	// Dit is een disconnect.
	// Wist alle info ivm de server...
	virtual Result_t disConnect( ) = 0;

	// ProjectInfo doorgeven.
	// Projectdir & bestandsnaam van het project...
	virtual void setProjectInfo(const char* projectDir, const char* fileName) = 0;
	virtual const char* getProjectDir( ) = 0;
	virtual const char* getProjectFileName( ) = 0;

	// Een taak starten
	virtual Result_t startTask(AppRemoteCfgTasks_t task, void* pvUserData) = 0;

	// Pollen 
	virtual bool poll(int msec) = 0;

	// task busy
	virtual bool taskIdle() = 0;

	// Opvragen van de info van de server.
	// Kunnen de helper klasse 'AppRemoteCfgServerInfo_Utils' gebruiken om de properties op te vragen...
	virtual Result_t getServerInfo(const AppRemoteCfgServerInfo** ppInfo) = 0;

	// Connected ?
	virtual bool connected() = 0;

	// Opvragen van de laastse errors - Oppassen krijgen een reference...
	virtual bool getLogErrors(const CStringList** pStringList) const = 0;

	// Opvragen van de firmware info.
	virtual Result_t getFirmwareVersionsInfo(const AppRemoteCfgFirmwareVersionInfo** ppInfo) = 0;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
extern IAppRemoteCfgApi* appremotecfg_create();
extern void appremotecfg_destroy(IAppRemoteCfgApi** ppInstance);

/*************************************************************/
/************************** utilities ************************/
/*************************************************************/

// Downloaden van een bestand.
extern bool appremotecfg_startDownloadFile(IAppRemoteCfgApi* instance,const char* url,const char* destFileName);

// Bindings cfg a.d.h. de gevonden binding bestanden in de project directory...
extern bool appremotecfg_startTaskPrjBindingsWrite(IAppRemoteCfgApi* instance, const char* projectDir);
extern bool appremotecfg_startTaskPrjBindingsErase(IAppRemoteCfgApi* instance, const char* projectDir);

// Bindings cfg a.d.h. de lijst van de nodes.
extern bool appremotecfg_startTaskNodesBindingsWrite(IAppRemoteCfgApi* instance, std::list<int>& nodes);
extern bool appremotecfg_startTaskNodesBindingsErase(IAppRemoteCfgApi* instance, std::list<int>& nodes);

// Namen doorsturen.
extern bool appremotecfg_startTaskNamesWrite(IAppRemoteCfgApi* instance, const AppCfgRemoteActionList& listNames);


/**************************************************************************/
/******************* Servers cfg: Interface & utilities *******************/
/**************************************************************************/
// Opvragen van het aantal servers.
extern bool appremotecfg_serversGetCount(IAppRemoteCfgApi* instance,int* numberOfServers);

// Opvragen van een server ifv de index
extern bool appremoteCfg_serversGetIndex(IAppRemoteCfgApi* instance, int index, AppRemoteCfgServer** ppServer);

// Een server toevoegen.
extern bool appremoteCfg_serversAdd(IAppRemoteCfgApi* instance, const AppRemoteCfgServer& newServer,int *count);

// Een server verwijderen.
extern bool appremoteCfg_serversRemove(IAppRemoteCfgApi* instance, const CString& hostName,int port, int *count);

// Opvragen van de namen van de servers.
extern bool appremotecfg_serversGetFriendlyNames(IAppRemoteCfgApi* instance, CStringList* pDest);

// Opvragen + updaten van het paswoord van een server...
extern bool appremotecfg_serversGetPassword(IAppRemoteCfgApi* instance, int index,CString* pDest);
extern bool appremotecfg_serversUpdatePassword(IAppRemoteCfgApi* instance, int index, const CString& pwd);


// Opvragen van de index van de server die geconnecteerd is.
// Controle via de serverInfo.
// De client kan de index gebruiken om de info van de server op te vragen.
extern bool appremoteCfg_serversGetIndexConnectedServer(IAppRemoteCfgApi* instance,int* index);

/**************************************************************/
/*********************** test functies ************************/
/**************************************************************/
// Test functie om de fsm te testen.
extern bool appremotecfg_test_startTaskNamesWrite(IAppRemoteCfgApi* instance);


/**************************************************************/
/*********************** Free functions ***********************/
/**************************************************************/
// Sedert V1.7: Laden & opslaan van de login info.
// Deze info wordt gecodeerd opgeslagen.
// Om deze functies te gebruiken moet er geen instantie van IAppRemoteCfgApi aangemaakt worden.
extern bool appremotecfg_loadLoginInfo(const char* fileName,AppRemoteCfgLoginInfo* dest);
extern bool appremotecfg_saveLoginInfo(const char* fileName,const AppRemoteCfgLoginInfo& src);

// Sedert V2.1: Export van de login info voor de configurator.
// Deze info wordt niet gecodeerd opgeslagen.
// Enkel het paswoord wordt gecodeerd opgeslagen.
extern bool appremotecfg_exportLoginInfo(const char* fileName,const AppRemoteCfgLoginInfo& src);

/**************************************************************/
// Sedert V1.8: Laden & opslaan van de node services.
// Om deze functies te gebruiken moet er geen instantie van IAppRemoteCfgApi aangemaakt worden.
extern bool appremotecfg_loadNodeServices(const char* fileName,AppRemoteCfgNodes* dest);
extern bool appremotecfg_saveNodeServices(const char* fileName,const AppRemoteCfgNodes& src);


// Sedert V2.3: Export van de node services info voor de configurator.
// Deze info wordt niet gecodeerd opgeslagen.
// Enkel het paswoord wordt gecodeerd opgeslagen.
extern bool appremotecfg_exportNodeServices(const char* fileName, const AppRemoteCfgNodes& src);

/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/
