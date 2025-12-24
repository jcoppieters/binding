#include "stdafx.h"
#include "AppRemoteCfgfsm.h"
#include "AppRemoteCfgCtx.h"

/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask::AppRCfgFsmTask(AppRemoteCfgTasks_t taskID) : m_taskID(taskID) { }
AppRCfgFsmTask::~AppRCfgFsmTask() { }

void AppRCfgFsmTask::setTaskStarted(AppRemoteCfgFsm& parent) {
	parent.getHost().taskStatus(m_taskID, APPREMOTE_CFG_TASK_STATUS_STARTED);
}
void AppRCfgFsmTask::setTaskDone(AppRemoteCfgFsm& parent) {
	parent.getHost().taskStatus(m_taskID, APPREMOTE_CFG_TASK_STATUS_DONE);
}
void AppRCfgFsmTask::setTaskFailed(AppRemoteCfgFsm& parent, HttpRCfgResult_t errorCode, const char* msg) {
	parent.getHost().taskStatus(m_taskID, APPREMOTE_CFG_TASK_STATUS_FAILED, errorCode, msg);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_IDLE::AppRCfgFsmTask_IDLE(AppRemoteCfgTasks_t taskID) : Base(taskID) { }
AppRCfgFsmTask_IDLE::~AppRCfgFsmTask_IDLE() { }

bool AppRCfgFsmTask_IDLE::startTask(AppRemoteCfgFsm& parent, AppRemoteCfgTasks_t task, void* pvUserData) {
	switch(task) {
		case APPREMOTE_CFG_TASK_IDLE:
			ASSERT(0);
		break;
		case APPREMOTE_CFG_TASK_CONNECT:
			return startTaskConnect(parent);
		case APPREMOTE_CFG_TASK_REFRESHACCTOKENS:
			return startTaskRefreshAccTokens(parent);
		case APPREMOTE_CFG_TASK_BINDINGS:
			return startTaskBindings(parent, pvUserData);
		case APPREMOTE_CFG_TASK_UPLOADBACKUP:
			return startTaskUploadBackup(parent);
	}
	return false;
}

// Controle van de refresh access tokens...
bool AppRCfgFsmTask_IDLE::poll(AppRemoteCfgFsm& parent, int msec) {
	enum { OFFSET_TIME = (1000 * 60) };		// 1 minuut.

	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (refCtx.getConnected()) {
		AppRemoteCfgClientCredentials& refCredentials = refCtx.getCredentials();
		if (refCredentials.getUseSecure()) {		
			const int currentTime = refCredentials.getCurrTokenExpiryMsec() - msec;			
			if (currentTime < OFFSET_TIME) {
				startTask(parent, APPREMOTE_CFG_TASK_REFRESHACCTOKENS, 0);
			}
			refCredentials.setCurrTokenExpiryMsec(currentTime);
		}
	}
	return true;
}

bool AppRCfgFsmTask_IDLE::startTaskConnect(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx_Util::taskIdle(refCtx));
	AppRcfgTask* pTask = 0;
	const AppRemoteCfgClientCredentials& refCredentials = refCtx.getCredentials();
	if (refCredentials.getUseSecure()) {
		pTask = new AppRcfgTaskConnectHttps((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(), (LPCSTR)refCredentials.getPassword(), refCredentials);
	}
	else {
		pTask = new AppRcfgTaskConnect((LPCSTR)refCtx.getHostName(), refCtx.getHostPort());
	}
	if (0 == pTask) {
		ASSERT(0 != pTask);
		return false;
	}
	refCtx.setCurrentTask(pTask);
	refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_CONNECT);
	pTask->Start();		
	parent.gotoTask(APPREMOTE_CFG_TASK_CONNECT);
	return true;
}

bool AppRCfgFsmTask_IDLE::startTaskRefreshAccTokens(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx_Util::taskIdle(refCtx));	
	const AppRemoteCfgClientCredentials& refCredentials = refCtx.getCredentials();
	if (!refCredentials.getUseSecure()) {
		return false;
	}
	AppRcfgTask* pTask = 0;
	pTask = new AppRcfgTaskConnectHttps((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(), refCredentials);
	refCtx.setCurrentTask(pTask);
	refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_REFRESH_ACCTOKENS);
	pTask->Start();
	parent.gotoTask(APPREMOTE_CFG_TASK_REFRESHACCTOKENS);
	return true;
}

bool AppRCfgFsmTask_IDLE::startTaskBindings(AppRemoteCfgFsm& parent, void* pvUserData) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx_Util::taskIdle(refCtx));

	AppCfgBindAction* const pAction = reinterpret_cast<AppCfgBindAction*>(pvUserData);
	ASSERT(pAction);

	AppRcfgTask* pTask = 0;
	const AppRemoteCfgClientCredentials& refCredentials = refCtx.getCredentials();
	if (refCredentials.getUseSecure()) {
		pTask = new AppRcfgTaskBindingsHttps((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(),
			(LPCSTR)refCtx.getWorkingDirectory(), *pAction, refCredentials);
	}
	else {
		pTask = new AppRcfgTaskBindings((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(),
					(LPCSTR)refCtx.getWorkingDirectory(), *pAction);
	}
	if (0 == pTask) {
		ASSERT(0 != pTask);
		return false;
	}
	refCtx.setCurrentTask(pTask);
	pTask->Start(pAction);
	const AppCfgBindAction::Action_t action = pAction->getAction();
	if (AppCfgBindAction::ERASE == action) {
		refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_BINDINGS_ERASE);
		parent.getHost().addMessage("Bindings erase", true);
	}
	else if (AppCfgBindAction::WRITE == action) {
		refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_BINDINGS_WRITE);
		parent.getHost().addMessage("Bindings write", true);
	}		
	parent.gotoTask(APPREMOTE_CFG_TASK_BINDINGS);
	return true;
}

bool AppRCfgFsmTask_IDLE::startTaskUploadBackup(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx_Util::taskIdle(refCtx));

	AppRcfgTask* pTask = 0;
	const AppRemoteCfgClientCredentials& refCredentials = refCtx.getCredentials();
	if (refCredentials.getUseSecure()) {
		pTask = new AppRcfgTaskUploadBackupBindPrjHttps((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(),
			(LPCSTR)refCtx.getWorkingDirectory(), (LPCSTR)refCtx.getFileName(), refCredentials);
	}
	else {
		pTask = new AppRcfgTaskUploadBackupBindPrj((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(),
			(LPCSTR)refCtx.getWorkingDirectory(), (LPCSTR)refCtx.getFileName());
	}
	if (0 == pTask) {
		ASSERT(0 != pTask);
		return false;
	}
	refCtx.setCurrentTask(pTask);
	refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_BACKUP_UPLOAD);
	pTask->Start();
	parent.gotoTask(APPREMOTE_CFG_TASK_UPLOADBACKUP);	
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_BASE::AppRCfgFsmTask_BUSY_BASE(AppRemoteCfgTasks_t taskID) : Base(taskID) { }
AppRCfgFsmTask_BUSY_BASE::~AppRCfgFsmTask_BUSY_BASE() { }

bool AppRCfgFsmTask_BUSY_BASE::getTaskLogInfo(AppRemoteCfgFsm& parent, CStringList* pDest) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if ((0 != pTask) && (pTask->logChanged(pDest))) {
		return true;
	}
	return false;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_CONNECT_BASE::AppRCfgFsmTask_BUSY_CONNECT_BASE(AppRemoteCfgTasks_t taskID, const char* taskMessage) :
	Base(taskID), m_szTaskMessage(taskMessage) { }
AppRCfgFsmTask_BUSY_CONNECT_BASE::~AppRCfgFsmTask_BUSY_CONNECT_BASE() { }

void AppRCfgFsmTask_BUSY_CONNECT_BASE::onEnter(AppRemoteCfgFsm& parent) {
	parent.getCtx().getServerInfo().clear();
	parent.getCtx().setConnected(FALSE);
	parent.getHost().addMessage(m_szTaskMessage, true);
	this->setTaskStarted(parent);
}
void AppRCfgFsmTask_BUSY_CONNECT_BASE::onExit(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
		char msg[256];
		sprintf(msg,"%s done", m_szTaskMessage);
		parent.getHost().addMessage(msg);
		this->setTaskDone(parent);
	}
	else if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) {
		char msg[256];
		sprintf(msg, "%s failed", m_szTaskMessage);
		parent.getHost().addMessage(msg);
		const char* errorMsg = 0;
		HttpRCfgResult_t errorCode = RESULT_SUCCESS;
		AppRcfgTask* pTask = refCtx.getCurrentTask();
		if (0 != pTask) {
			errorCode = pTask->getErrorCode();
			errorMsg = pTask->getMsgError();
		}
		else { ASSERT(0 != pTask); }
		this->setTaskFailed(parent, errorCode, errorMsg);
	}
}

bool AppRCfgFsmTask_BUSY_CONNECT_BASE::updateCredentials(AppRemoteCfgFsm& parent) {
	bool result = false;
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	AppRemoteCfgClientCredentials& refCredentials = refCtx.getCredentials();
	if (refCredentials.getUseSecure()) {
		AppCfgClientCredentials newCredentials;
		if (getCredentials(parent, &newCredentials)) {
			refCredentials.updateCredentials(newCredentials);
			return result;
		}
	}
	return result;
}

bool AppRCfgFsmTask_BUSY_CONNECT_BASE::updateNodeInfo(AppRemoteCfgFsm& parent) {

	AppCfgNodeInfo nodeInfo;
	if (!getNodeInfo(parent, &nodeInfo)) {
		return false;	// WAT DOEN ???????
	}

	AppRemoteCfgCtx& refCtx = parent.getCtx();
	parent.getHost().addMessage("Node Info:", true);
	CStringList stringList;
	nodeInfo.format(&stringList);
	parent.getHost().addMessages(stringList);

	refCtx.setConnected(TRUE);

	AppRemoteCfgServerInfo& refServerInfo = refCtx.getServerInfo();
	refServerInfo.setHostName(refCtx.getHostName());
	refServerInfo.setHostPort(refCtx.getHostPort());
	refServerInfo.setProductName(nodeInfo.getProductName());
	refServerInfo.setPhysicalAddress(nodeInfo.getPhysicalNodeAddress());
	refServerInfo.setNodeAddress(nodeInfo.getLogicalNodeAddress());
	refServerInfo.setSoftwareVersion(nodeInfo.getSoftwareVersion());
	return true;
}

bool AppRCfgFsmTask_BUSY_CONNECT_BASE::getNodeInfo(AppRemoteCfgFsm& parent, AppCfgNodeInfo* pDest) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return false;
	}
	if (pTask->Running()) {
		return false;
	}
	const AppRcfgTask::Status_t status = pTask->getStatus();
	if (AppRcfgTask::STATUS_DONE != status) {
		return false;
	}	
	AppRcfgTaskConnect_Base* const pCurrentTask = dynamic_cast<AppRcfgTaskConnect_Base*>(pTask);
	if (0 == pCurrentTask) {
		return false;
	}
	if (pCurrentTask->getNodeInfo(pDest)) {
		return true;
	}
	return false;
}

bool AppRCfgFsmTask_BUSY_CONNECT_BASE::getCredentials(AppRemoteCfgFsm& parent, AppCfgClientCredentials* pDest) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return false;
	}
	if (pTask->Running()) {
		return false;
	}
	const AppRcfgTask::Status_t status = pTask->getStatus();
	if (AppRcfgTask::STATUS_DONE != status) {
		return false;
	}

	AppRcfgTaskConnectHttps* const pCurrentTask = dynamic_cast<AppRcfgTaskConnectHttps*>(pTask);
	if (0 == pCurrentTask) {
		return false;
	}
	*pDest = pCurrentTask->getCredentials();
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_CONNECT::AppRCfgFsmTask_BUSY_CONNECT(AppRemoteCfgTasks_t taskID) : 
	Base(taskID, "Connect to the node") { }
AppRCfgFsmTask_BUSY_CONNECT::~AppRCfgFsmTask_BUSY_CONNECT() { }

bool AppRCfgFsmTask_BUSY_CONNECT::poll(AppRemoteCfgFsm& parent, int msec) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx::STATE_BUSY_CONNECT == refCtx.getTaskState());

	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
		updateNodeInfo(parent);
		updateCredentials(parent);		
	}
	else if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) {
		// absorbe...
	}
	else {
		CStringList stringList;
		if (getTaskLogInfo(parent, &stringList)) {		
			parent.getHost().addMessages(stringList,true);
		}
	}

	// Indien taak gedaan is de ctx wissen en naar Idle task gaan...
	if (!AppRemoteCfgCtx_Util::taskRunning(refCtx)) {
		parent.gotoTask(APPREMOTE_CFG_TASK_IDLE);
		AppRemoteCfgCtx_Util::taskClear(refCtx);
	}
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_REFRESHACCTOKENS::AppRCfgFsmTask_BUSY_REFRESHACCTOKENS(AppRemoteCfgTasks_t taskID) : 
	Base(taskID,"Refresh access code") { }
AppRCfgFsmTask_BUSY_REFRESHACCTOKENS::~AppRCfgFsmTask_BUSY_REFRESHACCTOKENS() { }

bool AppRCfgFsmTask_BUSY_REFRESHACCTOKENS::poll(AppRemoteCfgFsm& parent, int msec) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx::STATE_BUSY_REFRESH_ACCTOKENS == refCtx.getTaskState());

	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {	
		updateCredentials(parent);
		refCtx.setConnected(TRUE);
	}
	else if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) {
		// absorbe...
	}
	else {
		CStringList stringList;
		if (getTaskLogInfo(parent, &stringList)) {
			parent.getHost().addMessages(stringList, true);
		}
	}

	// Indien taak gedaan is de ctx wissen en naar Idle task gaan...
	if (!AppRemoteCfgCtx_Util::taskRunning(refCtx)) {
		parent.gotoTask(APPREMOTE_CFG_TASK_IDLE);
		AppRemoteCfgCtx_Util::taskClear(refCtx);
	}
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_BINDINGS::AppRCfgFsmTask_BUSY_BINDINGS(AppRemoteCfgTasks_t taskID) : Base(taskID) { }
AppRCfgFsmTask_BUSY_BINDINGS::~AppRCfgFsmTask_BUSY_BINDINGS() { }

void AppRCfgFsmTask_BUSY_BINDINGS::onEnter(AppRemoteCfgFsm& parent) {

	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (AppRemoteCfgCtx::STATE_BUSY_BINDINGS_ERASE == refCtx.getTaskState()) {	
		parent.getHost().addMessage("Bindings erase", true);
	}
	else if (AppRemoteCfgCtx::STATE_BUSY_BINDINGS_WRITE == refCtx.getTaskState()) {	
		parent.getHost().addMessage("Bindings write", true);
	}
	this->setTaskStarted(parent);
}
void AppRCfgFsmTask_BUSY_BINDINGS::onExit(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
		this->setTaskDone(parent);
	}
	else if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) {
		const char* errorMsg = 0;
		HttpRCfgResult_t errorCode = RESULT_SUCCESS;
		AppRcfgTask* pTask = refCtx.getCurrentTask();
		if (0 != pTask) {
			errorCode = pTask->getErrorCode();
			errorMsg = pTask->getMsgError();			
		}
		else { ASSERT(0 != pTask); }

		if ((0 != errorMsg) && (0 != strcmp(errorMsg, ""))) {
			parent.getHost().addMessage(errorMsg);
		}

		this->setTaskFailed(parent, errorCode, errorMsg);
	}
}

bool AppRCfgFsmTask_BUSY_BINDINGS::poll(AppRemoteCfgFsm& parent,int msec) {

	// Log info opvragen en doorgeven aan de host.
	CStringList stringList;
	if (getTaskLogInfo(parent, &stringList)) {
		parent.getHost().addMessages(stringList, true);
	}
	// Log info opvragen en doorgeven aan de host... tot hier.

	// Progress info opvragen en doorgeven naar de host...
	AppRemoteCfgCtx& refCtx = parent.getCtx();	
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 != pTask) {
		AppRcfgTaskBindings_Base* const pTaskBindings = dynamic_cast<AppRcfgTaskBindings_Base*>(pTask);
		if (pTaskBindings) {
			AppCfgBindInfoNodes infoNodes;
			if (pTaskBindings->progressChanged(&infoNodes)) {
				parent.getHost().taskBindingsProgressInfo(infoNodes);
			}
		}
	}
	// Progress info opvragen en doorgeven naar de host... tot hier...
	
	if (!AppRemoteCfgCtx_Util::taskRunning(refCtx)) {
		parent.gotoTask(APPREMOTE_CFG_TASK_IDLE);
		AppRemoteCfgCtx_Util::taskClear(refCtx);
	}
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_UPLOADBACKUP::AppRCfgFsmTask_BUSY_UPLOADBACKUP(AppRemoteCfgTasks_t taskID) : Base(taskID) { }
AppRCfgFsmTask_BUSY_UPLOADBACKUP::~AppRCfgFsmTask_BUSY_UPLOADBACKUP() { }

void AppRCfgFsmTask_BUSY_UPLOADBACKUP::onEnter(AppRemoteCfgFsm& parent) {
	parent.getHost().addMessage("Uploading backup", true);
	this->setTaskStarted(parent);
}
void AppRCfgFsmTask_BUSY_UPLOADBACKUP::onExit(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
		this->setTaskDone(parent);
	}	
	else if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) {
		const char* errorMsg = 0;
		HttpRCfgResult_t errorCode = RESULT_SUCCESS;
		AppRcfgTask* pTask = refCtx.getCurrentTask();
		if (0 != pTask) {
			errorCode = pTask->getErrorCode();
			errorMsg = pTask->getMsgError();
		}
		else { ASSERT(0 != pTask); }

		if ((0 != errorMsg) && (0 != strcmp(errorMsg, ""))) {
			parent.getHost().addMessage(errorMsg);
		}
		this->setTaskFailed(parent, errorCode, errorMsg);
	}	
}


bool AppRCfgFsmTask_BUSY_UPLOADBACKUP::poll(AppRemoteCfgFsm& parent, int msec) {
	CStringList stringList;
	if (getTaskLogInfo(parent, &stringList)) {
		parent.getHost().addMessages(stringList, true);
	}

	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (!AppRemoteCfgCtx_Util::taskRunning(refCtx)) {		
		parent.gotoTask(APPREMOTE_CFG_TASK_IDLE);
		AppRemoteCfgCtx_Util::taskClear(refCtx);
	}
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgFsm::AppRemoteCfgFsm(AppRemoteCfgCtx& ctx,IAppRemoteCfgFsm_Host& refHost) :
	m_refCtx(ctx), m_refHost(refHost), m_pCurrentTask(0), m_pTaskIdle(new AppRCfgFsmTask_IDLE(APPREMOTE_CFG_TASK_IDLE)),
	m_pTaskBusyConnect(new AppRCfgFsmTask_BUSY_CONNECT(APPREMOTE_CFG_TASK_CONNECT)), 
	m_pTaskBusyRefreshAccTokens(new AppRCfgFsmTask_BUSY_REFRESHACCTOKENS(APPREMOTE_CFG_TASK_REFRESHACCTOKENS)),
	m_pTaskBusyBindings(new AppRCfgFsmTask_BUSY_BINDINGS(APPREMOTE_CFG_TASK_BINDINGS)),
	m_pTaskBusyUploadBackup(new AppRCfgFsmTask_BUSY_UPLOADBACKUP(APPREMOTE_CFG_TASK_UPLOADBACKUP))
{ 
	ASSERT(0 != m_pTaskIdle);
	ASSERT(0 != m_pTaskBusyConnect);
	ASSERT(0 != m_pTaskBusyRefreshAccTokens);
	ASSERT(0 != m_pTaskBusyBindings);
	ASSERT(0 != m_pTaskBusyUploadBackup);

	m_pCurrentTask = m_pTaskIdle;
}
AppRemoteCfgFsm::~AppRemoteCfgFsm() { 
	if (0 != m_pTaskIdle) {
		delete m_pTaskIdle;
		m_pTaskIdle = 0;
	}
	if (0 != m_pTaskBusyConnect) {
		delete m_pTaskBusyConnect;
		m_pTaskBusyConnect = 0;
	}
	if (0 != m_pTaskBusyRefreshAccTokens) {
		delete m_pTaskBusyRefreshAccTokens;
		m_pTaskBusyRefreshAccTokens = 0;
	}
	if (0 != m_pTaskBusyBindings) {
		delete m_pTaskBusyBindings;
		m_pTaskBusyBindings = 0;
	}
	if (0 != m_pTaskBusyUploadBackup) {
		delete m_pTaskBusyUploadBackup;
		m_pTaskBusyUploadBackup = 0;
	}
}

bool AppRemoteCfgFsm::startTask(AppRemoteCfgTasks_t task, void* pvUserData) {
	ASSERT(0 != m_pCurrentTask);
	return m_pCurrentTask->startTask(*this,task, pvUserData);
}

bool AppRemoteCfgFsm::poll(int msec) {
	ASSERT(0 != m_pCurrentTask);
	return m_pCurrentTask->poll(*this, msec);
}

void AppRemoteCfgFsm::gotoTask(AppRemoteCfgTasks_t task) {
	ASSERT(0 != m_pCurrentTask);
	AppRCfgFsmTask* pTask = 0;
	m_pCurrentTask->onExit(*this);
	switch(task) {
		case APPREMOTE_CFG_TASK_IDLE:
			pTask = m_pTaskIdle;
		break;
		case APPREMOTE_CFG_TASK_CONNECT:
			pTask = m_pTaskBusyConnect;
		break;
		case APPREMOTE_CFG_TASK_REFRESHACCTOKENS:
			pTask = m_pTaskBusyRefreshAccTokens;
		break;
		case APPREMOTE_CFG_TASK_BINDINGS:
			pTask = m_pTaskBusyBindings;
		break;
		case APPREMOTE_CFG_TASK_UPLOADBACKUP:
			pTask = m_pTaskBusyUploadBackup;
		break;
	}
	ASSERT(0 != pTask);
	m_pCurrentTask = pTask;
	m_pCurrentTask->onEnter(*this);
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/