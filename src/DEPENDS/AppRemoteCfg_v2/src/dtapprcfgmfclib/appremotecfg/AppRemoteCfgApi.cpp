#include "stdafx.h"
#include "AppRemoteCfgApi.h"

#include "AppRemoteCfgCtx.h"
#include "AppRemoteCfgFsm.h"
#include "AppRemoteCfgMediator.h"
#include "AppRemoteCfgCmn.h"

#include "httprcfg/httprcfg_api.h"				// Nodig voor de init & de shutdown ...
#include "apprcfg/apprcfg.h"					// Utilities

#include "json/jsonlogininfo.h"					// Voor de login info json.
#include "json/jsonservernodes.h"				// Node services.
#include "httprcfg/httprcfg_http.h"				//AuthHelper

#include "appremotecfg/AppRemoteCfgNodeService.h"		// API functie...
#include "appremotecfg/AppRemoteCfgLoginInfo.h"
/**************************************************************/
#define REQUIRE				ASSERT
#define ALLEGE				ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
enum {
	SOFTWARE_VERSION_MAJOR = 2,
	SOFTWARE_VERSION_MINOR = 3
};
/**************************************************************/
IAppRemoteCfgApi::~IAppRemoteCfgApi() { }
IAppRemoteCfgGUI::~IAppRemoteCfgGUI() { }
IAppRemoteCfgSystemEvents::~IAppRemoteCfgSystemEvents() {}
/**************************************************************/

IAppRemoteCfgApi* appremotecfg_create() {
	return new AppRemoteCfgApi();
}
void appremotecfg_destroy(IAppRemoteCfgApi** ppInstance) {
	if (0 != *ppInstance) {
		delete *ppInstance;
		*ppInstance = 0;
	}
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgApi::AppRemoteCfgApi() :
	m_pAppRemoteCfgCtx(new AppRemoteCfgCtx()), m_pAppRemoteCfgFsm(0),m_pAppRemoteCfgMediator(0)
{ 
	m_pAppRemoteCfgMediator = new AppRemoteCfgMediator(*m_pAppRemoteCfgCtx, 0, 0);
	m_pAppRemoteCfgFsm = new AppRemoteCfgFsm(*m_pAppRemoteCfgCtx, *m_pAppRemoteCfgMediator);

	httprcfg_init();
}

AppRemoteCfgApi::~AppRemoteCfgApi()
{
	httprcfg_shutdown();

	if (m_pAppRemoteCfgCtx) {
		delete m_pAppRemoteCfgCtx;
		m_pAppRemoteCfgCtx = 0;
	}
	if (m_pAppRemoteCfgFsm) {
		delete m_pAppRemoteCfgFsm;
		m_pAppRemoteCfgFsm = 0;
	}
	if (m_pAppRemoteCfgMediator) {
		delete m_pAppRemoteCfgMediator;
		m_pAppRemoteCfgMediator = 0;
	}
}

void AppRemoteCfgApi::getVersion(int* major, int* minor) {
	*major = SOFTWARE_VERSION_MAJOR;
	*minor = SOFTWARE_VERSION_MINOR;
}

void AppRemoteCfgApi::setSubscriberGui(IAppRemoteCfgGUI* ptrGui) {
	if (0 == m_pAppRemoteCfgMediator) {
		return;
	}
	m_pAppRemoteCfgMediator->setSubscriberGui(ptrGui);	
}

void AppRemoteCfgApi::setSubscriberSystemEvents(IAppRemoteCfgSystemEvents* ptr) {
	if (0 == m_pAppRemoteCfgMediator) {
		return;
	}
	m_pAppRemoteCfgMediator->setSubscriberSystemEvents(ptr);
}

AppRemoteCfgApi::Result_t AppRemoteCfgApi::setHostInfo(const char* hostName, int port, bool useHttpsSecure, const char* password) {
	if (0 == m_pAppRemoteCfgMediator || 0 == m_pAppRemoteCfgCtx) {
		return RESULT_EC_FAILED;
	}
	if ((0 == hostName) || (0 == strcmp(hostName,"")) || (port <= 0)) {
		return RESULT_EC_PARAMS;
	}
	if (m_pAppRemoteCfgCtx->getConnected()) {
		// Moeten eerst disconnecteren...
		return RESULT_EC_CONNECTED;		
	}

	if (useHttpsSecure) {
		if (!m_pAppRemoteCfgMediator->loadClientCredentials(hostName, port)) {
			if (0 == password || (0 == strcmp(hostName, ""))) {
				return RESULT_EC_AUTH_PASSWORD;
			}
			m_pAppRemoteCfgCtx->getCredentials().setPassword(password);		
		}
	}
	m_pAppRemoteCfgCtx->getCredentials().setUseSecure(useHttpsSecure);
	m_pAppRemoteCfgCtx->setHost(hostName, port);
	return RESULT_SUCCESS;
}

AppRemoteCfgApi::Result_t AppRemoteCfgApi::disConnect() {
	if (0 == m_pAppRemoteCfgMediator || 0 == m_pAppRemoteCfgCtx) {
		return RESULT_EC_FAILED;
	}
	if (!taskIdle()) {
		return RESULT_EC_BUSY;
	}
	m_pAppRemoteCfgCtx->getCredentials().clearClientCredentialsInfo();
	m_pAppRemoteCfgCtx->setConnected(false);
	return RESULT_SUCCESS;
}


void AppRemoteCfgApi::setProjectInfo(const char* projectDir, const char* fileName) {
	if (0 == m_pAppRemoteCfgCtx) {
		return;
	}
	m_pAppRemoteCfgCtx->setWorkingDirectory(projectDir);
	m_pAppRemoteCfgCtx->setFileName(fileName);

	if (0 == m_pAppRemoteCfgMediator) {
		ASSERT(0 != m_pAppRemoteCfgMediator);
		return;
	}
	m_pAppRemoteCfgMediator->loadServersInfo();
}

const char* AppRemoteCfgApi::getProjectDir() {
	if (0 == m_pAppRemoteCfgCtx) {
		return 0;
	}
	return (LPCSTR)m_pAppRemoteCfgCtx->getWorkingDirectory();
}

const char* AppRemoteCfgApi::getProjectFileName() {
	if (0 == m_pAppRemoteCfgCtx) {
		return 0;
	}
	return (LPCSTR)m_pAppRemoteCfgCtx->getFileName();
}

AppRemoteCfgApi::Result_t AppRemoteCfgApi::startTask(AppRemoteCfgTasks_t task, void* pvUserData) {
	if (0 == m_pAppRemoteCfgFsm || 0 == m_pAppRemoteCfgCtx) {
		return RESULT_EC_FAILED;
	}
	if ((APPREMOTE_CFG_TASK_CONNECT != task) && (APPREMOTE_CFG_TASK_REFRESHACCTOKENS != task) && 
		(APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS != task) && (APPREMOTE_CFG_TASK_DOWNLOADFILE != task)) {
		if (!m_pAppRemoteCfgCtx->getConnected()) {
			return RESULT_EC_NOT_CONNECTED;	
		}
	}
	if (!taskIdle()) {
		return RESULT_EC_BUSY;
	}
	if (!m_pAppRemoteCfgFsm->startTask(task, pvUserData)) {
		return RESULT_EC_FSM;
	}
	return RESULT_SUCCESS;
}

bool AppRemoteCfgApi::poll(int msec) {
	if (0 == m_pAppRemoteCfgFsm) {
		return false;
	}
	return m_pAppRemoteCfgFsm->poll(msec);
}

bool AppRemoteCfgApi::taskIdle() {
	if (0 == m_pAppRemoteCfgCtx) {
		return false;
	}
	return AppRemoteCfgCtx_Util::taskIdle(*m_pAppRemoteCfgCtx);
}

// Opvragen van de info van de server.
AppRemoteCfgApi::Result_t AppRemoteCfgApi::getServerInfo(const AppRemoteCfgServerInfo** ppInfo) {
	if (0 == m_pAppRemoteCfgCtx) {
		return RESULT_EC_FAILED;
	}
	if (!m_pAppRemoteCfgCtx->getConnected()) {
		return RESULT_EC_NOT_CONNECTED;
	}
	*ppInfo = &m_pAppRemoteCfgCtx->getServerInfo();
	return RESULT_SUCCESS;
}


// Opvragen van de firmware info.
AppRemoteCfgApi::Result_t AppRemoteCfgApi::getFirmwareVersionsInfo(const AppRemoteCfgFirmwareVersionInfo** ppInfo) {
	if (0 == m_pAppRemoteCfgCtx) {
		return RESULT_EC_FAILED;
	}
	*ppInfo = &m_pAppRemoteCfgCtx->getFirmwareInfoVersions();
	return RESULT_SUCCESS;
}

bool AppRemoteCfgApi::connected() {
	if (0 == m_pAppRemoteCfgCtx) {
		return false;
	}
	return m_pAppRemoteCfgCtx->getConnected();
}

// Opvragen van de servers info.
AppRemoteCfgApi::Result_t AppRemoteCfgApi::getInfoServers(AppRemoteCfgServers** ppServers) {
	if (0 == m_pAppRemoteCfgCtx) {
		return RESULT_EC_FAILED;
	}
	*ppServers = &(m_pAppRemoteCfgCtx->getServers());
	return RESULT_SUCCESS;
}

int AppRemoteCfgApi::addServer(const AppRemoteCfgServer& newServer) {
	if (0 == m_pAppRemoteCfgCtx) {
		return -1;
	}

	// Toevoegen van de server indien deze nog niet bestaat.
	AppRemoteCfgServers& refServers = m_pAppRemoteCfgCtx->getServers();
	if (AppRemoteCfgServers::add(&refServers,newServer)) {			
		// Opslaan van de servers...
		if (!m_pAppRemoteCfgMediator->saveServersInfo()) {
			
		}
	}
	return refServers.getCount();
}

int AppRemoteCfgApi::removeServer(const CString& hostName, int port) {
	if (0 == m_pAppRemoteCfgCtx) {
		return -1;
	}

	// Toevoegen van de server indien deze nog niet bestaat.
	AppRemoteCfgServers& refServers = m_pAppRemoteCfgCtx->getServers();
	if (AppRemoteCfgServers::remove(&refServers, hostName, port)) {
		// Opslaan van de servers...
		if (!m_pAppRemoteCfgMediator->saveServersInfo()) {
			// Moeten we hier iets doen ???
		}		
	}	
	return refServers.getCount();
}

AppRemoteCfgApi::Result_t AppRemoteCfgApi::getServer(int index,AppRemoteCfgServer** ppServer) {
	if (0 == m_pAppRemoteCfgCtx) {
		return RESULT_EC_FAILED;
	}
	AppRemoteCfgServers& refServers = m_pAppRemoteCfgCtx->getServers();
	if (index >= refServers.getCount()) {
		return RESULT_EC_FAILED;
	}
	return (true == refServers.get(index,ppServer) ? RESULT_SUCCESS : RESULT_EC_FAILED);
}

bool AppRemoteCfgApi::getLogErrors(const CStringList** pStringList) const {
	if (0 == m_pAppRemoteCfgCtx) {
		return false;
	}
	*pStringList = &(m_pAppRemoteCfgCtx->getLogErrors());
	return true;
}
/**************************************************************/

AppRemoteCfgMediator& AppRemoteCfgApi::getMediator() {
	ALLEGE(NULL != m_pAppRemoteCfgMediator);
	return *m_pAppRemoteCfgMediator;
}

/**************************************************************/


/*************************************************************/
/************************** utilities ************************/
/*************************************************************/

// Downloaden van een bestand.
bool appremotecfg_startDownloadFile(IAppRemoteCfgApi* instance, const char* url, const char* destFileName) {
	if (NULL == url || NULL == destFileName) {
		return false;
	}
	CStringList listParams;
	listParams.AddTail(CString(url));					// [0] = Url.
	listParams.AddTail(CString(destFileName));			// [1] = destfilename.
	return (AppRemoteCfgApi::RESULT_SUCCESS == instance->startTask(APPREMOTE_CFG_TASK_DOWNLOADFILE,&listParams));
}

// Bindings cfg a.d.h. de gevonden binding bestanden in de project directory...
bool appremotecfg_startTaskPrjBindingsWrite(IAppRemoteCfgApi* instance, const char* projectDir) {	
	std::list<int> list;
	if (!apprcfg_findNodeBindingFiles(projectDir, &list)) {
		return false;
	}
	return appremotecfg_startTaskNodesBindingsWrite(instance,list);
}

bool appremotecfg_startTaskPrjBindingsErase(IAppRemoteCfgApi* instance, const char* projectDir) {	
	std::list<int> list;
	if (!apprcfg_findNodeBindingFiles(projectDir, &list)) {
		return false;
	}
	return appremotecfg_startTaskNodesBindingsErase(instance,list);
}

// Bindings cfg a.d.h. de lijst van de nodes.
bool appremotecfg_startTaskNodesBindingsWrite(IAppRemoteCfgApi* instance, std::list<int>& list) {
	AppCfgBindAction action(AppCfgBindAction::WRITE);
	for (std::list<int>::iterator it = list.begin(); it != list.end(); ++it) {
		action.add(*it);
	}
	const IAppRemoteCfgApi::Result_t res = instance->startTask(APPREMOTE_CFG_TASK_BINDINGS, &action);
	return (IAppRemoteCfgApi::RESULT_SUCCESS == res);
}

bool appremotecfg_startTaskNodesBindingsErase(IAppRemoteCfgApi* instance, std::list<int>& list) {
	AppCfgBindAction action(AppCfgBindAction::ERASE);
	for (std::list<int>::iterator it = list.begin(); it != list.end(); ++it) {
		action.add(*it);
	}
	const IAppRemoteCfgApi::Result_t res = instance->startTask(APPREMOTE_CFG_TASK_BINDINGS, &action);
	return (IAppRemoteCfgApi::RESULT_SUCCESS == res);
}

// Namen doorsturen.
bool appremotecfg_startTaskNamesWrite(IAppRemoteCfgApi* instance, const AppCfgRemoteActionList& listNames) {
	const IAppRemoteCfgApi::Result_t res = instance->startTask(APPREMOTE_CFG_TASK_WRITENAMES, (void*)&listNames);
	return (IAppRemoteCfgApi::RESULT_SUCCESS == res);
}


/*************************************************************/
/******************* utilities Servers cfg *******************/
/*************************************************************/
// Opvragen van het aantal servers.
bool appremotecfg_serversGetCount(IAppRemoteCfgApi* instance, int* numberOfServers) {
	if (NULL == instance || NULL == numberOfServers) {
		ASSERT(NULL != instance);
		ASSERT(NULL != numberOfServers);
		return false;
	}
	AppRemoteCfgApi* pImpl = dynamic_cast<AppRemoteCfgApi*>(instance);
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	AppRemoteCfgServers* pServers = 0;
	if (!pImpl->getInfoServers(&pServers)) {
		return false;
	}
	
	*numberOfServers = pServers->getCount();
	return true;
}

bool appremoteCfg_serversGetIndex(IAppRemoteCfgApi* instance, int index, AppRemoteCfgServer** ppServer) {
	if (NULL == instance || NULL == ppServer) {
		ASSERT(NULL != instance);
		ASSERT(NULL != ppServer);
		return false;
	}
	AppRemoteCfgApi* pImpl = dynamic_cast<AppRemoteCfgApi*>(instance);
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	AppRemoteCfgServers* pServers = 0;
	if (!pImpl->getInfoServers(&pServers)) {
		return false;
	}

	AppRemoteCfgServer* pServerInfo = 0;
	if (!pServers->get(index, &pServerInfo)) {
		return false;
	}
	ASSERT(NULL != pServerInfo);
	*ppServer = pServerInfo;
	return true;
}

// Een server toevoegen.
bool appremoteCfg_serversAdd(IAppRemoteCfgApi* instance, const AppRemoteCfgServer& newServer, int *count) {
	if (NULL == instance || NULL == count) {
		ASSERT(NULL != instance);
		ASSERT(NULL != count);
		return false;
	}
	AppRemoteCfgApi* pImpl = dynamic_cast<AppRemoteCfgApi*>(instance);
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	*count = pImpl->addServer(newServer);
	return true;
}

// Een server verwijderen.
bool appremoteCfg_serversRemove(IAppRemoteCfgApi* instance, const CString& hostName, int port, int *count) {
	if (NULL == instance || NULL == count) {
		ASSERT(NULL != instance);
		ASSERT(NULL != count);	
		return false;
	}
	AppRemoteCfgApi* pImpl = dynamic_cast<AppRemoteCfgApi*>(instance);
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}
	*count = pImpl->removeServer(hostName,port);
	return true;
}

// Opvragen + updaten van het paswoord van een server...
bool appremotecfg_serversGetPassword(IAppRemoteCfgApi* instance, int index, CString* pDest) {
	if (NULL == instance || NULL == pDest) {
		ASSERT(NULL != instance);
		ASSERT(NULL != pDest);
		return false;
	}

	AppRemoteCfgServer* pServer = NULL;
	if (!appremoteCfg_serversGetIndex(instance,index,&pServer)) {
		return false;
	}

	*pDest = pServer->getPassword();
	return true;
}

bool appremotecfg_serversUpdatePassword(IAppRemoteCfgApi* instance, int index, const CString& pwd) {
	if (NULL == instance) {
		ASSERT(NULL != instance);
		return false;
	}
	AppRemoteCfgApi* pImpl = dynamic_cast<AppRemoteCfgApi*>(instance);
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	AppRemoteCfgServer* pServer = NULL;
	if (!appremoteCfg_serversGetIndex(pImpl, index, &pServer)) {
		return false;
	}

	// Paswoord veranderen
	pServer->setPassword(pwd);

	// server data oplsaan.
	pImpl->getMediator().saveServersInfo();
	return true;
}


bool appremotecfg_serversGetFriendlyNames(IAppRemoteCfgApi* instance, CStringList* pDest) {
	if (NULL == instance || NULL == pDest) {
		ASSERT(NULL != instance);
		ASSERT(NULL != pDest);
		return false;
	}
	AppRemoteCfgApi* pImpl = dynamic_cast<AppRemoteCfgApi*>(instance);
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	pDest->RemoveAll();

	AppRemoteCfgServers* pServers = 0;
	if (IAppRemoteCfgApi::RESULT_SUCCESS == pImpl->getInfoServers(&pServers)) {
		const int count = pServers->getCount();
		for (int i = 0; i<count; ++i) {
			AppRemoteCfgServer* pServer = 0;
			if (pServers->get(i, &pServer)) {
				pDest->AddTail(pServer->getFriendlyName());
			}
		}
	}
	return true;
}

bool appremoteCfg_serversGetIndexConnectedServer(IAppRemoteCfgApi* instance, int* index) {

	*index = -1;

	if (NULL == instance || NULL == index) {
		ASSERT(NULL != instance);
		ASSERT(NULL != index);
		return false;
	}
	if (!instance->connected()) {
		*index = -1;
		return false;
	}

	AppRemoteCfgApi* pImpl = dynamic_cast<AppRemoteCfgApi*>(instance);
	if (NULL == pImpl) {
		ASSERT(NULL != pImpl);
		return false;
	}

	AppRemoteCfgServers* pServers = 0;
	if (IAppRemoteCfgApi::RESULT_SUCCESS == pImpl->getInfoServers(&pServers)) {
		const int count = pServers->getCount();
		if (count > 0) {
			const AppRemoteCfgServerInfo* pInfo = NULL;
			if (instance->getServerInfo(&pInfo)) {
				ASSERT(NULL != pInfo);
				const CString& hostName = pInfo->getHostName();
				const int port = pInfo->getHostPort();
				for (int i = 0; i<count; ++i) {
					AppRemoteCfgServer* pServer = 0;
					if (pServers->get(i, &pServer)) {
						ASSERT(NULL != pServer);
						if (AppRemoteCfgServer::equal(*pServer, hostName, port)) {
							*index = i;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/**************************************************************/

/**************************************************************/
/********************** private helper functie ****************/
/**************************************************************/
template <class DATA,class PARSER>
static bool helper_load(const char* fileName,DATA* dest,bool secured) {
	CFile f;
	if (!f.Open(fileName, CFile::modeRead)) {
		return false;
	}

	// load data.
	std::string strData;
	DWORD dwRead = 0;
	do {
		BYTE buffer[4096];
		memset(buffer, 0, sizeof(buffer));
		dwRead = f.Read(buffer, 4096);
		if (dwRead>0) {
			strData.append((const char*)buffer, (int)dwRead);
		}
	} while (dwRead > 0);
	f.Close();

	bool result = false;

	try {
		PARSER parser;		
		std::string strParse;
		if (secured) {						
			strParse = AuthHelper::decode(strData.c_str(), json_getAuthPrivateKey());
		}
		else {
			strParse = strData;
		}
		// parse data.									
		if (RESULT_SUCCESS == parser.parse(dest, strParse)) {
			result = true;
		}			
	}
	catch(std::runtime_error& /* e */) {	
		// Suppress error: base 64 encode/decode - Deze smijt een exceptie wanner de data geen base64 is...
		result = false;
	}
	return result;
}

template <class DATA,class FORMATTER>
static bool helper_save(const char* fileName,const DATA& src,bool secured) {
	CFile f;
	if (!f.Open(fileName, CFile::modeWrite | CFile::modeCreate)) {
		return false;
	}
	
	bool result = false;	
	FORMATTER formatter;
	std::string strData;
	if (TRUE == formatter.format(&strData, src)) {
		try {

			std::string strSaveData;
			if (secured) {			
				strSaveData = AuthHelper::encode(strData.c_str(), json_getAuthPrivateKey());
			}
			else {
				strSaveData = strData;
			}

			// save data...
			f.Write(strSaveData.c_str(), strSaveData.length());
			f.Flush();
			result = true;
		}				
		catch(std::runtime_error& /* e */) {	
			// Suppress error: base 64 encode/decode - Deze smijt een exceptie wanner de data geen base64 is...
			result = false;
		}
	}
	f.Close();
	return result;
}

// Sedert V1.7: Info om in te loggen.
bool appremotecfg_loadLoginInfo(const char* fileName, AppRemoteCfgLoginInfo* dest) {
	return helper_load<AppRemoteCfgLoginInfo,JsonServerLoginInfo>(fileName,dest,true);
}
bool appremotecfg_saveLoginInfo(const char* fileName, const AppRemoteCfgLoginInfo& src) {
	return helper_save<AppRemoteCfgLoginInfo,JsonServerLoginInfo>(fileName,src,true);
}

// Sedert V2.1: Export info login.
bool appremotecfg_exportLoginInfo(const char* fileName,const AppRemoteCfgLoginInfo& src) {
	return helper_save<AppRemoteCfgLoginInfo,JsonServerLoginInfoExport>(fileName,src,false);
}

// Sedert V1.8: Laden & opslaan van de node services.
bool appremotecfg_loadNodeServices(const char* fileName,AppRemoteCfgNodes* dest) {
	return helper_load<AppRemoteCfgNodes,JsonServerNodes>(fileName,dest,true);				
}
bool appremotecfg_saveNodeServices(const char* fileName,const AppRemoteCfgNodes& src) {
	return helper_save<AppRemoteCfgNodes,JsonServerNodes>(fileName,src,true);				
}
bool appremotecfg_exportNodeServices(const char* fileName, const AppRemoteCfgNodes& src) {
	return helper_save<AppRemoteCfgNodes, JsonServerNodes>(fileName, src, false);
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/