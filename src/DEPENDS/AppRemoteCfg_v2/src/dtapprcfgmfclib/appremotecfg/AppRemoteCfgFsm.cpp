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
		case APPREMOTE_CFG_TASK_WRITENAMES:
			return startTaskUploadNames(parent,pvUserData);
		case APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS:
			return startTaskRequestFirmwareVersions(parent, pvUserData);
		case APPREMOTE_CFG_TASK_DOWNLOADFILE:
			return startTaskDownloadFile(parent, pvUserData);
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
	refCtx.clearLogErrors();
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
	refCtx.clearLogErrors();
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
	refCtx.clearLogErrors();
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
	refCtx.clearLogErrors();
	refCtx.setCurrentTask(pTask);
	refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_BACKUP_UPLOAD);
	pTask->Start();
	parent.gotoTask(APPREMOTE_CFG_TASK_UPLOADBACKUP);	
	return true;
}

bool AppRCfgFsmTask_IDLE::startTaskUploadNames(AppRemoteCfgFsm& parent, void* pvUserData) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx_Util::taskIdle(refCtx));

	AppCfgRemoteActionList* const pActionList = reinterpret_cast<AppCfgRemoteActionList*>(pvUserData);
	if (0 == pActionList) {
		ASSERT(0 != pActionList);
		return false;
	}

	AppRcfgTask* pTask = 0;
	const AppRemoteCfgClientCredentials& refCredentials = refCtx.getCredentials();
	if (refCredentials.getUseSecure()) {
		pTask = new AppRcfgTaskNamesHttps((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(), (LPCSTR)refCtx.getWorkingDirectory(), *pActionList, refCredentials);
	}
	else {
		pTask = new AppRcfgTaskNames((LPCSTR)refCtx.getHostName(), refCtx.getHostPort(), (LPCSTR)refCtx.getWorkingDirectory(), *pActionList);
	}
	if (0 == pTask) {
		ASSERT(0 != pTask);
		return false;
	}
	refCtx.clearLogErrors();
	refCtx.setCurrentTask(pTask);
	pTask->Start(pActionList);
	parent.gotoTask(APPREMOTE_CFG_TASK_WRITENAMES);
	return true;
}

bool AppRCfgFsmTask_IDLE::startTaskRequestFirmwareVersions(AppRemoteCfgFsm& parent, void* pvUserData) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx_Util::taskIdle(refCtx));

	static const char* const hostAddress = "www.duotecno-cloud.be";	
	static const int port = 80;

	AppRcfgTask* pTask = new AppRcfgTaskRequestCloudFirmwareInfo(hostAddress, port);
	
	if (0 == pTask) {
		ASSERT(0 != pTask);
		return false;
	}
	refCtx.clearLogErrors();
	refCtx.setCurrentTask(pTask);
	refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_REQUEST_FIRMWAREVERSIONS);
	pTask->Start();
	parent.gotoTask(APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS);
	return true;
}

bool AppRCfgFsmTask_IDLE::startTaskDownloadFile(AppRemoteCfgFsm& parent, void* pvUserData) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx_Util::taskIdle(refCtx));

	CStringList* const pStringList = reinterpret_cast<CStringList*>(pvUserData);
	if (NULL == pStringList) {
		ASSERT(0);
		return false;
	}
	if (pStringList->GetCount()<2) {
		ASSERT(0);
		return false;
	}

	// index 0
	POSITION pos = pStringList->FindIndex(0);
	if (NULL == pos) {
		return false;
	}
	const CString& url = pStringList->GetAt(pos);

	// index 1
	pos = pStringList->FindIndex(1);
	if (NULL == pos) {
		return false;
	}
	const CString& destFileName = pStringList->GetAt(pos);

	if (url.IsEmpty() || destFileName.IsEmpty()) {
		return false;
	}

/*
	static const char* const hostAddress = "www.duotecno-cloud.be";		// <--- Is Deze nodig ???????????
	static const int port = 80;											// <--- Is Deze nodig ???????????
*/
	AppRcfgTask* pTask = new AppRcfgTaskDownloadFile((LPCSTR) url, (LPCSTR) destFileName);

	if (0 == pTask) {
		ASSERT(0 != pTask);
		return false;
	}
	refCtx.clearLogErrors();
	refCtx.setCurrentTask(pTask);
	refCtx.setTaskState(AppRemoteCfgCtx::STATE_BUSY_DOWNLOAD_FILE);
	pTask->Start();
	parent.gotoTask(APPREMOTE_CFG_TASK_DOWNLOADFILE);
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_BASE::AppRCfgFsmTask_BUSY_BASE(AppRemoteCfgTasks_t taskID, const char* szTaskMessage) : 
	Base(taskID), m_szTaskMessage(szTaskMessage) { }
AppRCfgFsmTask_BUSY_BASE::~AppRCfgFsmTask_BUSY_BASE() { }

void AppRCfgFsmTask_BUSY_BASE::onExit(AppRemoteCfgFsm& parent) {
	if (!this->checkTaskStatusDone(parent)) {
		this->checkTaskStatusFailed(parent);
	}
	this->updateLogErrors(parent);
	this->Base::onExit(parent);
}

bool AppRCfgFsmTask_BUSY_BASE::poll(AppRemoteCfgFsm& parent, int msec) {
	// Indien taak gedaan is de ctx wissen en naar Idle task gaan...
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (!AppRemoteCfgCtx_Util::taskRunning(refCtx)) {
		parent.gotoTask(APPREMOTE_CFG_TASK_IDLE);
		AppRemoteCfgCtx_Util::taskClear(refCtx);
	}
	return true;
}

// Opvragen van de logberichten en deze doorgeven aan de host (=client.)
void AppRCfgFsmTask_BUSY_BASE::updateLogMessages(AppRemoteCfgFsm& parent) {
	AppRcfgTask* const pTask = parent.getCtx().getCurrentTask();
	if (0 != pTask) {
		CStringList stringList;
		if (pTask->logChanged(AppRcfgTask::TYPE_LOGGING_INFO, &stringList)) {
			parent.getHost().addMessages(stringList, true);
		}
	}
}

bool AppRCfgFsmTask_BUSY_BASE::updateLogErrors(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 != pTask) {
		CStringList list;
		if (pTask->logChanged(AppRcfgTask::TYPE_LOGGING_ERRORS, &list)) {
			refCtx.setLogErrors(list);
			return true;
		}
	}
	return false;
}

bool AppRCfgFsmTask_BUSY_BASE::checkTaskStatusDone(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
		const char* const szTaskMessage = this->getTaskMessage();
		if (0 != szTaskMessage) {
			char msg[256];
			sprintf(msg, "%s done", szTaskMessage);
			parent.getHost().addMessage(msg);
		}
		this->setTaskDone(parent);
		return true;
	}
	return false;
}
bool AppRCfgFsmTask_BUSY_BASE::checkTaskStatusFailed(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) {
		const char* const szTaskMessage = this->getTaskMessage();
		if (0 != szTaskMessage) {
			char msg[256];
			sprintf(msg, "%s failed", szTaskMessage);
			parent.getHost().addMessage(msg);
		}
		const char* errorMsg = 0;
		HttpRCfgResult_t errorCode = RESULT_SUCCESS;
		AppRcfgTask* pTask = refCtx.getCurrentTask();
		if (0 != pTask) {
			errorCode = pTask->getErrorCode();
			errorMsg = pTask->getMsgError();
		}
		else { ASSERT(0 != pTask); }
		this->setTaskFailed(parent, errorCode, errorMsg);
		return true;
	}
	return false;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_CONNECT_BASE::AppRCfgFsmTask_BUSY_CONNECT_BASE(AppRemoteCfgTasks_t taskID, const char* taskMessage) :
	Base(taskID, taskMessage) { }
AppRCfgFsmTask_BUSY_CONNECT_BASE::~AppRCfgFsmTask_BUSY_CONNECT_BASE() { }

void AppRCfgFsmTask_BUSY_CONNECT_BASE::onEnter(AppRemoteCfgFsm& parent) {
	parent.getCtx().getServerInfo().clear();	
	parent.getCtx().setConnected(FALSE);
	const char* const szTaskMessage = this->getTaskMessage();
	if (0 != szTaskMessage) {
		parent.getHost().addMessage(szTaskMessage, true);
	}
	this->setTaskStarted(parent);
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
	refServerInfo.setSecureLogin(refCtx.getCredentials().getUseSecure());
	refServerInfo.setProductName(nodeInfo.getProductName());
	refServerInfo.setPhysicalAddress(nodeInfo.getPhysicalNodeAddress());
	refServerInfo.setNodeAddress(nodeInfo.getLogicalNodeAddress());
	refServerInfo.setSoftwareVersion(nodeInfo.getSoftwareVersion());	
	refServerInfo.setHardware(nodeInfo.getHardware());
	refServerInfo.setQtVersion(nodeInfo.getQtVersion());
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

	this->updateLogMessages(parent);

	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
		updateNodeInfo(parent);
		updateCredentials(parent);		
	}

#if(0)
	else if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) 
	{ 
		/* absorbe... */ 
	}
	else {
		this->updateLogMessages(parent);	
	}
#endif 

	return this->Base::poll(parent,msec);
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS::AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS(AppRemoteCfgTasks_t taskID) :
	Base(taskID, "Request cloud firmware versions") { }
AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS::~AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS() { }

bool AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS::poll(AppRemoteCfgFsm& parent, int msec) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx::STATE_BUSY_REQUEST_FIRMWAREVERSIONS == refCtx.getTaskState());

	this->updateLogMessages(parent);

	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
		updateFirmwareVersionInfo(parent);	
	}
	return this->Base::poll(parent, msec);
}

bool AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS::updateFirmwareVersionInfo(AppRemoteCfgFsm& parent) {

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
	AppRcfgTaskRequestCloudFirmwareInfo* const pCurrentTask = dynamic_cast<AppRcfgTaskRequestCloudFirmwareInfo*>(pTask);
	if (0 == pCurrentTask) {
		return false;
	}

	AppRemoteCfgFirmwareVersionInfo& refFirmwareInfo = refCtx.getFirmwareInfoVersions();
	if (pCurrentTask->getInfo(&refFirmwareInfo)) {
		return true;
	}
	return false;
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_DOWNLOAD_FILE::AppRCfgFsmTask_BUSY_DOWNLOAD_FILE(AppRemoteCfgTasks_t taskID) :
	Base(taskID, "Download file") { }
	
AppRCfgFsmTask_BUSY_DOWNLOAD_FILE::~AppRCfgFsmTask_BUSY_DOWNLOAD_FILE() { }

bool AppRCfgFsmTask_BUSY_DOWNLOAD_FILE::poll(AppRemoteCfgFsm& parent, int msec) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	ASSERT(AppRemoteCfgCtx::STATE_BUSY_DOWNLOAD_FILE == refCtx.getTaskState());
	
	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {
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
	}
	return this->Base::poll(parent,msec);
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

	this->updateLogMessages(parent);

	if (AppRemoteCfgCtx_Util::taskDone(refCtx)) {	
		updateCredentials(parent);
		refCtx.setConnected(TRUE);
	}
#if(0)
	else if (AppRemoteCfgCtx_Util::taskFailed(refCtx)) {
		// absorbe...
	}
	else {
		this->updateLogMessages(parent);
	}
#endif 

	return this->Base::poll(parent, msec);
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

bool AppRCfgFsmTask_BUSY_BINDINGS::poll(AppRemoteCfgFsm& parent,int msec) {
	this->updateLogMessages(parent);
	this->requestProgressInfo(parent);	
	return this->Base::poll(parent, msec);
}

bool AppRCfgFsmTask_BUSY_BINDINGS::requestProgressInfo(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 != pTask) {
		AppRcfgTaskBindings_Base* const pTaskBindings = dynamic_cast<AppRcfgTaskBindings_Base*>(pTask);
		if (pTaskBindings) {
			AppCfgBindInfoNodes infoNodes;
			if (pTaskBindings->progressChanged(&infoNodes)) {
				parent.getHost().taskBindingsProgressInfo(infoNodes);
				return true;
			}
		}
	}
	return false;
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
bool AppRCfgFsmTask_BUSY_UPLOADBACKUP::poll(AppRemoteCfgFsm& parent, int msec) {
	this->updateLogMessages(parent);
	return this->Base::poll(parent, msec);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgFsmTask_BUSY_UPLOADNAMES::AppRCfgFsmTask_BUSY_UPLOADNAMES(AppRemoteCfgTasks_t taskID) : Base(taskID) { }
AppRCfgFsmTask_BUSY_UPLOADNAMES::~AppRCfgFsmTask_BUSY_UPLOADNAMES() { }

void AppRCfgFsmTask_BUSY_UPLOADNAMES::onEnter(AppRemoteCfgFsm& parent) {
	parent.getHost().addMessage("Uploading names", true);
	this->setTaskStarted(parent);
}

bool AppRCfgFsmTask_BUSY_UPLOADNAMES::poll(AppRemoteCfgFsm& parent, int msec) {
	this->updateLogMessages(parent);
	this->requestProgressInfo(parent);
	return this->Base::poll(parent, msec);
}

bool AppRCfgFsmTask_BUSY_UPLOADNAMES::requestProgressInfo(AppRemoteCfgFsm& parent) {
	AppRemoteCfgCtx& refCtx = parent.getCtx();
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 != pTask) {
		AppRcfgTaskNames_Base* const pTaskWriteNames = dynamic_cast<AppRcfgTaskNames_Base*>(pTask);
		if (pTaskWriteNames) {
			AppCfgCmnProgressInfo progressInfo;
			if (pTaskWriteNames->progressChanged(&progressInfo)) {
				parent.getHost().taskUploadNamesProgressInfo(progressInfo);
				return true;
			}
		}
	}
	return false;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgFsm::AppRemoteCfgFsm(AppRemoteCfgCtx& ctx,IAppRemoteCfgFsm_Host& refHost) :
	m_refCtx(ctx), m_refHost(refHost), m_pCurrentTask(0), m_pTaskIdle(new AppRCfgFsmTask_IDLE(APPREMOTE_CFG_TASK_IDLE)),
	m_pTaskBusyConnect(new AppRCfgFsmTask_BUSY_CONNECT(APPREMOTE_CFG_TASK_CONNECT)), 
	m_pTaskBusyRefreshAccTokens(new AppRCfgFsmTask_BUSY_REFRESHACCTOKENS(APPREMOTE_CFG_TASK_REFRESHACCTOKENS)),
	m_pTaskBusyBindings(new AppRCfgFsmTask_BUSY_BINDINGS(APPREMOTE_CFG_TASK_BINDINGS)),
	m_pTaskBusyUploadBackup(new AppRCfgFsmTask_BUSY_UPLOADBACKUP(APPREMOTE_CFG_TASK_UPLOADBACKUP)),
	m_pTaskBusyUploadNames(new AppRCfgFsmTask_BUSY_UPLOADNAMES(APPREMOTE_CFG_TASK_WRITENAMES)),
	m_pTaskRequestFirmwareInfo(new AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS(APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS)),
	m_pTaskDownloadFile(new AppRCfgFsmTask_BUSY_DOWNLOAD_FILE(APPREMOTE_CFG_TASK_DOWNLOADFILE))
{ 
	ASSERT(0 != m_pTaskIdle);
	ASSERT(0 != m_pTaskBusyConnect);
	ASSERT(0 != m_pTaskBusyRefreshAccTokens);
	ASSERT(0 != m_pTaskBusyBindings);
	ASSERT(0 != m_pTaskBusyUploadBackup);
	ASSERT(0 != m_pTaskBusyUploadNames);
	ASSERT(0 != m_pTaskRequestFirmwareInfo);
	ASSERT(0 != m_pTaskDownloadFile);

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
	if (0 != m_pTaskBusyUploadNames) {
		delete m_pTaskBusyUploadNames;
		m_pTaskBusyUploadNames = 0;
	}
	if (0 != m_pTaskRequestFirmwareInfo) {
		delete m_pTaskRequestFirmwareInfo;
		m_pTaskRequestFirmwareInfo  = 0;
	}
	if (0 != m_pTaskDownloadFile) {
		delete m_pTaskDownloadFile;
		m_pTaskDownloadFile = 0;
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
		case APPREMOTE_CFG_TASK_WRITENAMES:
			pTask = m_pTaskBusyUploadNames;
		break;
		case APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS:
			pTask = m_pTaskRequestFirmwareInfo;
		break;
		case APPREMOTE_CFG_TASK_DOWNLOADFILE:
			pTask = m_pTaskDownloadFile;
		break;
	}
	ASSERT(0 != pTask);
	m_pCurrentTask = pTask;
	m_pCurrentTask->onEnter(*this);
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/