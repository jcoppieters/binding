#include "stdafx.h"
#include "AppRemoteCfgCtx.h"

/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgClientCredentials::AppRemoteCfgClientCredentials( ) : 
	m_password(), m_useSecure(false), m_currTokenExpiryMsec(0)
{ 
} 

// Deze wordt aangeroepen wanneer er nieuwe credentails ontvangen werden...
void AppRemoteCfgClientCredentials::updateCredentials(AppCfgClientCredentials& newCredentials) {
#if(0)
	enum { TIME_REFRESH_ACC_CREDENTIALS_MSEC = (1000 * 60 * 2) };			// 2min Om te debuggen.
#endif 
	enum { TIME_REFRESH_ACC_CREDENTIALS_MSEC = (1000 * 60 * 60 * 2) };		// 2h refreshen.

	const int timeExpirySec = newCredentials.getExpiryTimeSec();
	if (timeExpirySec < ((TIME_REFRESH_ACC_CREDENTIALS_MSEC)/1000)) {
		m_currTokenExpiryMsec = timeExpirySec * 1000;
	}
	else {
		m_currTokenExpiryMsec = TIME_REFRESH_ACC_CREDENTIALS_MSEC;
	}	
	this->Base::update(newCredentials);
}
void AppRemoteCfgClientCredentials::clearClientCredentialsInfo() {
	m_currTokenExpiryMsec = 0;
	m_password.Empty();	
	m_useSecure = false;
	this->Base::clear();
}

void AppRemoteCfgClientCredentials::setPassword(const CString& s) {
	m_password = s;
}
const CString& AppRemoteCfgClientCredentials::getPassword() const {
	return m_password;
}
const CString& AppRemoteCfgClientCredentials::getPassword() {
	return m_password;
}

void AppRemoteCfgClientCredentials::setUseSecure(bool state) {
	m_useSecure = state;
}
bool AppRemoteCfgClientCredentials::getUseSecure() const {
	return 	m_useSecure;
}

void AppRemoteCfgClientCredentials::setCurrTokenExpiryMsec(int value) {
	m_currTokenExpiryMsec = value;
}
int AppRemoteCfgClientCredentials::getCurrTokenExpiryMsec() const {
	return m_currTokenExpiryMsec;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgCtx::AppRemoteCfgCtx() : m_servers(), m_credentials(), m_serverInfo(), m_firmwareInfo(), 
	m_directory(), m_fileName(), m_hostName(), m_port(0), m_connected(false), 
	m_pCurrentTask(0), m_state(STATE_IDLE), m_LogErrors() { }

AppRemoteCfgCtx::~AppRemoteCfgCtx() {
	destroyTask();
}

void AppRemoteCfgCtx::setWorkingDirectory(const CString& s) {
	REQUIRE(!s.IsEmpty());
	m_directory = s;
}
const CString& AppRemoteCfgCtx::getWorkingDirectory( ) const {
	return m_directory;
}
const CString& AppRemoteCfgCtx::getWorkingDirectory( ) {
	return m_directory;
}

void AppRemoteCfgCtx::setFileName(const CString& s) {
	REQUIRE(!s.IsEmpty());
	m_fileName = s;
}
const CString& AppRemoteCfgCtx::getFileName( ) const {
	return m_fileName;
}
const CString& AppRemoteCfgCtx::getFileName( ) {
	return m_fileName;
}

void AppRemoteCfgCtx::setHost(const CString& hostName,int port) { 
	REQUIRE(!hostName.IsEmpty());
	REQUIRE(port>0);
	m_hostName = hostName;
	m_port = port;
}
const CString& AppRemoteCfgCtx::getHostName() const {
	return m_hostName;
}
int AppRemoteCfgCtx::getHostPort() const {
	return m_port;
}

void AppRemoteCfgCtx::setConnected(bool state) {
	m_connected = state;
}
bool AppRemoteCfgCtx::getConnected( ) const {
	return m_connected;
}

AppRcfgTask* AppRemoteCfgCtx::getCurrentTask( ) {
	return m_pCurrentTask;
}
const AppRcfgTask* AppRemoteCfgCtx::getCurrentTask( ) const {
	return m_pCurrentTask;
}
void AppRemoteCfgCtx::setCurrentTask(AppRcfgTask* pTask) {
	destroyTask();	
	m_pCurrentTask = pTask;
}

AppRemoteCfgCtx::State_t AppRemoteCfgCtx::getTaskState( ) const {
	return m_state;
}
void AppRemoteCfgCtx::setTaskState(State_t state) {
	m_state = state;
}

void AppRemoteCfgCtx::clearLogErrors() {
	m_LogErrors.RemoveAll();
}
void AppRemoteCfgCtx::setLogErrors(const CStringList& errors) {
	m_LogErrors.RemoveAll( );
	if (!errors.IsEmpty()) {
		m_LogErrors.AddTail((CStringList*)&errors);
	}
}

// protected:
void AppRemoteCfgCtx::destroyTask() {
	if (0 != m_pCurrentTask) {
		delete m_pCurrentTask;
		m_pCurrentTask = 0;
	}
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/

bool AppRemoteCfgCtx_Util::taskIdle(AppRemoteCfgCtx& refCtx) {
	if (AppRemoteCfgCtx::STATE_IDLE != refCtx.getTaskState()) {
		return false;
	}
	ASSERT(NULL == refCtx.getCurrentTask());
	return true;
};

bool AppRemoteCfgCtx_Util::taskDone(AppRemoteCfgCtx& refCtx) {
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return false;
	}
	if (pTask->Running()) {
		return false;
	}
	const AppRcfgTask::Status_t status = pTask->getStatus();
	if (AppRcfgTask::STATUS_DONE == status) {
		return true;
	}
	return false;
};

bool AppRemoteCfgCtx_Util::taskFailed(AppRemoteCfgCtx& refCtx) {
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return false;
	}
	if (pTask->Running()) {
		return false;
	}

	const AppRcfgTask::Status_t status = pTask->getStatus();
	if (AppRcfgTask::STATUS_ERROR == status) {
		return true;
	}
	return false;
};

bool AppRemoteCfgCtx_Util::taskRunning(AppRemoteCfgCtx& refCtx) {
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return false;
	}
	return (TRUE == pTask->Running() ? true : false);
}
bool AppRemoteCfgCtx_Util::taskClear(AppRemoteCfgCtx& refCtx) {
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return false;
	}
	if (!pTask->Running()) {	
		refCtx.setCurrentTask(0);
		refCtx.setTaskState(AppRemoteCfgCtx::STATE_IDLE);
		return true;
	}
	return false;
}

bool AppRemoteCfgCtx_Util::taskError(AppRemoteCfgCtx& refCtx) {
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return false;
	}
	const AppRcfgTask::Status_t status = pTask->getStatus();
	if (AppRcfgTask::STATUS_ERROR == status ||
		AppRcfgTask::STATUS_CANCEL == status)
	{
		return true;
	}
	return false;
}

const char* AppRemoteCfgCtx_Util::taskErrorMsg(AppRemoteCfgCtx& refCtx) {
	AppRcfgTask* const pTask = refCtx.getCurrentTask();
	if (0 == pTask) {
		return 0;
	}
	return pTask->getMsgError();
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/