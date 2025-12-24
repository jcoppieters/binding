#include "stdafx.h"
#include "BindingSoftware.h"

#if (USE_TOOLS_APPREMOTECFG == 1)

#include "AppRemoteCfg.h"
#include "appremotecfg/AppRemoteCfgServers.h"
//////////////////////////////////////////////////////////////////////
namespace DUOTECNO {
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
enum { TMR_INTERVAL_POLL_MSEC = 50 };
//////////////////////////////////////////////////////////////////////
AppRemoteCfg* AppRemoteCfg::getInstance() {
	static AppRemoteCfg instance;
	return (&instance);
}
//////////////////////////////////////////////////////////////////////

// Controle doen van de versie...
static bool verifyVersion(IAppRemoteCfgApi* pAppRemoteCfg) {	
	enum {
		APPREMOTECFG_LIB_VERSION_MAJOR = 1,
		APPREMOTECFG_LIB_VERSION_MINOR = 7
	};

	int version[2];
	pAppRemoteCfg->getVersion(&version[0],&version[1]);
	if (APPREMOTECFG_LIB_VERSION_MAJOR == version[0] && APPREMOTECFG_LIB_VERSION_MINOR == version[1]) {
		return true;
	}
	ASSERT(APPREMOTECFG_LIB_VERSION_MAJOR == version[0] && APPREMOTECFG_LIB_VERSION_MINOR == version[1]);
	return false;
}

//////////////////////////////////////////////////////////////////////
AppRemoteCfg::AppRemoteCfg( ) : m_pAppRemoteCfg(NULL), m_timerID( 0 ), m_currentTask(TASK_NONE)
{ 

	m_pAppRemoteCfg = appremotecfg_create();
	ASSERT(NULL != m_pAppRemoteCfg);
	if (NULL != m_pAppRemoteCfg) {
		if (!verifyVersion(m_pAppRemoteCfg)) {
			// Absorbe 4 the moment.
		}			
		m_pAppRemoteCfg->setSubscriberSystemEvents(this);
	}
}

AppRemoteCfg::~AppRemoteCfg( ) { 
	if (0 != m_timerID) {
		KillTimer(NULL, m_timerID);
	}
	if (NULL != m_pAppRemoteCfg) {
		m_pAppRemoteCfg->setSubscriberSystemEvents(NULL);
	}
	appremotecfg_destroy(&m_pAppRemoteCfg);
}
//////////////////////////////////////////////////////////////////////

void AppRemoteCfg::subscribeGui(DUOTECNO::IAppRemoteCfgGUI* pGui) {
	if (0 != m_pAppRemoteCfg) {
		m_pAppRemoteCfg->setSubscriberGui(pGui);
	}
}

// Polling timer..
void AppRemoteCfg::timerCallback(HWND unnamedParam1,UINT unnamedParam2,UINT_PTR unnamedParam3,DWORD unnamedParam4) {
	AppRemoteCfg* pCfg =  AppRemoteCfg::getInstance();
	if (NULL != pCfg) {
		ASSERT(NULL != pCfg->m_pAppRemoteCfg);
		pCfg->m_pAppRemoteCfg->poll(TMR_INTERVAL_POLL_MSEC);
	}
}

IAppRemoteCfgApi::Result_t AppRemoteCfg::setConnect(const char* hostName,int port,bool useHttpsSecure,const char* password) {
	IAppRemoteCfgApi::Result_t res = IAppRemoteCfgApi::RESULT_EC_FAILED;
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return res;
	}	

	AppRemoteCfgUtils::updateProjectFileInfo(*this);

	res = m_pAppRemoteCfg->setHostInfo(hostName, port, useHttpsSecure);
	if (IAppRemoteCfgApi::RESULT_EC_AUTH_PASSWORD == res) {
		if (0 == password) {
			// Er zijn geen client credentials gevonden voor deze server.
			// Opnieuw proberen en een paswoord meegegeven.
			return res;
		}
	}

	if (0 == m_timerID) {
		// De timer pas starten wanneer er een connect uitgevoerd werd ...
		m_timerID = SetTimer(NULL, 1, TMR_INTERVAL_POLL_MSEC, (TIMERPROC)(&AppRemoteCfg::timerCallback));
	}
		
	res = m_pAppRemoteCfg->setHostInfo(hostName, port, useHttpsSecure, password);
	if (IAppRemoteCfgApi::RESULT_SUCCESS == res) {
		res = m_pAppRemoteCfg->startTask(APPREMOTE_CFG_TASK_CONNECT,0);
		if (res == IAppRemoteCfgApi::RESULT_SUCCESS) {		
		}	
	}
	return res;
}

IAppRemoteCfgApi::Result_t AppRemoteCfg::setDisconect( ) {
	IAppRemoteCfgApi::Result_t res = IAppRemoteCfgApi::RESULT_EC_FAILED;
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return res;
	}	
	res = m_pAppRemoteCfg->disConnect();
	return res;
}

bool AppRemoteCfg::connected() {
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return false;
	}	
	return m_pAppRemoteCfg->connected();
}

IAppRemoteCfgApi::Result_t AppRemoteCfg::getServerInfo(const AppRemoteCfgServerInfo** ppInfo) {
	IAppRemoteCfgApi::Result_t res = IAppRemoteCfgApi::RESULT_EC_FAILED;
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return res;
	}	
	res = m_pAppRemoteCfg->getServerInfo(ppInfo);
	return res;
}

// Taken...
bool AppRemoteCfg::startBindingsWrite(bool onlyModified) {
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return false;
	}	

	if (AppRemoteCfgUtils::updateProjectFileInfo(*this)) {
		// Oppassen: Moeten eerst het project opslaan want de binding bestanden worden verstuurd naar de 
		// gateway. 
		AppRemoteCfgUtils::saveModifiedProject(*this);			
	}


	bool res = false;
	std::list<int> nodes;
	if (!AppRemoteCfgUtils::getNodeBindingFiles(*this,&nodes,onlyModified)) {
		// Geen binding bestanden gevonden.
		ASSERT(false == onlyModified);
		return false;
	}

	m_currentTask = TASK_BINDINGS_WRITE;
	res = appremotecfg_startTaskNodesBindingsWrite(m_pAppRemoteCfg, nodes);			
	return res;
}

bool AppRemoteCfg::startBindingsErase( ) {
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return false;
	}	

	AppRemoteCfgUtils::updateProjectFileInfo(*this);

	bool res = false;
	std::list<int> nodes;
	if (!AppRemoteCfgUtils::getNodeBindingFiles(*this,&nodes,false)) {
		ASSERT(0);
		return false;
	}

	m_currentTask = TASK_BINDINGS_ERASE;
	res = appremotecfg_startTaskNodesBindingsErase(m_pAppRemoteCfg, nodes);	
	return res;

/*
	bool res = false;
	const char* const projectDir = m_pAppRemoteCfg->getProjectDir();
	if (0 != projectDir) {
		m_currentTask = TASK_BINDINGS_ERASE;
		res = appremotecfg_startTaskPrjBindingsErase(m_pAppRemoteCfg, projectDir);			
	}
	return res;
*/
}
bool AppRemoteCfg::startBackupUpload( ) {
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return false;
	}	

	if (AppRemoteCfgUtils::updateProjectFileInfo(*this)) {
		// Oppassen: Moeten eerst het project opslaan zodat de backup de laatste bestanden bevat...
		AppRemoteCfgUtils::saveModifiedProject(*this);			
	}

	m_currentTask = TASK_BACKUP_UPLOAD;
	if (IAppRemoteCfgApi::RESULT_SUCCESS == m_pAppRemoteCfg->startTask(APPREMOTE_CFG_TASK_UPLOADBACKUP,0)) {
		return true;
	}
	return false;
}

bool AppRemoteCfg::startUploadNames( ) {
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return false;
	}	

	AppCfgRemoteActionList list;
	if (!AppRemoteCfgUtils::getListNames(*this,&list)) {
		return false;
	}

	m_currentTask = TASK_NAMES_WRITE;
	const bool res = appremotecfg_startTaskNamesWrite(m_pAppRemoteCfg, list);			
	return res;
}

bool AppRemoteCfg::getLogErrors(const CStringList** pStringList) const {
	if (NULL == m_pAppRemoteCfg) {
		ASSERT(NULL != m_pAppRemoteCfg);
		return false;
	}	
	return m_pAppRemoteCfg->getLogErrors(pStringList);
}

// DUOTECNO::IAppRemoteCfgSystemEvents events
void AppRemoteCfg::taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
							  DUOTECNO::HttpRCfgResult_t errorCode, const char* msgError) 
{
	switch(task) {
		case APPREMOTE_CFG_TASK_IDLE: /* absorbe */ break;
		case APPREMOTE_CFG_TASK_CONNECT: /* absorbe */ break;
		case APPREMOTE_CFG_TASK_REFRESHACCTOKENS: /* absorbe */ break;
		case APPREMOTE_CFG_TASK_BINDINGS: 
		{		
			switch(status) {
				case APPREMOTE_CFG_TASK_STATUS_STARTED: { /* absorbe... */ } break;
				case APPREMOTE_CFG_TASK_STATUS_DONE: {
					if (TASK_BINDINGS_ERASE == m_currentTask) {
						// Alle bindings werden gewist.
						// Update nodedatabase, binding entries & views
						AppRemoteCfgUtils::updateBindingsErased(*this);
					}
					else if (TASK_BINDINGS_WRITE == m_currentTask) {
						// Alle bindings werden doorgestuurd.
						// Update nodedatabase, binding entries & views
						AppRemoteCfgUtils::updateBindingsWritten(*this);
					}
				} break;
				case APPREMOTE_CFG_TASK_STATUS_FAILED: {
					if (TASK_BINDINGS_WRITE == m_currentTask) {
						// Bindings werden niet doorgestuurd naar alle nodes.
						// Update nodedatabase, binding entries & views
						AppRemoteCfgUtils::updateBindingsWrittenFailed(*this);
					}
				} break;
			}
		} break;
		case APPREMOTE_CFG_TASK_UPLOADBACKUP: /* absorbe */ break;
		case APPREMOTE_CFG_TASK_WRITENAMES: /* absorbe */ break;
	}

	// Wissen van de huidige taak info...
	if (APPREMOTE_CFG_TASK_STATUS_DONE == status || APPREMOTE_CFG_TASK_STATUS_FAILED == status) {
		m_currentTask = TASK_NONE;
	}
}

// Deze krijgt de laatste progressinfo: Deze gebruiken wanneer een taak gedaan / mislukt is.
void AppRemoteCfg::taskBindingsProgressInfo(const AppCfgBindInfoNodes& refInfo) {
	m_bindInfoNodes = refInfo;
}

void AppRemoteCfg::taskUploadNamesProgressInfo(const AppCfgCmnProgressInfo& refInfo) {
	// Todo: Moeten we iets doen met deze status ?????
}

//////////////////////////////////////////////////////////////////////
}; /* namespace DUOTECNO */
//////////////////////////////////////////////////////////////////////

#endif

