#include "stdafx.h"
#include "AppRemoteCfgApi.h"

#include "AppRemoteCfgCtx.h"
#include "AppRemoteCfgFsm.h"
#include "AppRemoteCfgMediator.h"

#include "httprcfg/httprcfg_api.h"				// Nodig voor de init & de shutdown ...
#include "apprcfg/apprcfg.h"					// Utilities
/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
enum {
	SOFTWARE_VERSION_MAJOR = 1,
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
	m_pAppRemoteCfgCtx->getCredentials().clear();
	m_pAppRemoteCfgCtx->setConnected(false);
	return RESULT_SUCCESS;
}


void AppRemoteCfgApi::setProjectInfo(const char* projectDir, const char* fileName) {
	if (0 == m_pAppRemoteCfgCtx) {
		return;
	}
	m_pAppRemoteCfgCtx->setWorkingDirectory(projectDir);
	m_pAppRemoteCfgCtx->setFileName(fileName);
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
	if (APPREMOTE_CFG_TASK_CONNECT != task && APPREMOTE_CFG_TASK_REFRESHACCTOKENS != task) {
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

bool AppRemoteCfgApi::connected() {
	if (0 == m_pAppRemoteCfgCtx) {
		return false;
	}
	return m_pAppRemoteCfgCtx->getConnected();
}
/**************************************************************/


/*************************************************************/
/************************** utilities ************************/
/*************************************************************/

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
/**************************************************************/
}; /* end namespace*/
/**************************************************************/