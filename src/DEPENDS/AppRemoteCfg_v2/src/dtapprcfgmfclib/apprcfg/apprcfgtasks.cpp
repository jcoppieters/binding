#include "stdafx.h"
#include "apprcfgtasks.h"

#include "apprcfg.h"
#include "httprcfg/httprcfg_api.h"
/**************************************************************/
#define REQUIRE			ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
static void merge(CStringList* dest,const CStringList& src, bool removeOld) {
	if (removeOld) {
		dest->RemoveAll( );
	}
	dest->AddTail((CStringList*)&src);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTask::DataInfo::DataInfo() : m_mutexLock(), m_eventChanged() { }

AppRcfgTask::LogInfo::LogInfo() : DataInfo(), m_data() { }

BOOL AppRcfgTask::LogInfo::Changed(CStringList* pStringList) {
	BOOL result = this->m_eventChanged.Wait(0);
	if (result) {
		if (0 != pStringList) {
			result = FALSE;
			if (this->m_mutexLock.Reserve(1)) {
				merge(pStringList, this->m_data, true);
				this->m_mutexLock.Release();
				result = TRUE;
			}
		}
		this->m_eventChanged.Reset();
	}
	return result;
}

void AppRcfgTask::LogInfo::addMessage(const char* str) {
	if (0 == str) {
		return;
	}
	if (this->m_mutexLock.Reserve(1)) {
		this->m_data.AddTail(str);
		this->m_mutexLock.Release();
	}
	this->m_eventChanged.Set();
}

void AppRcfgTask::LogInfo::addMessages(const CStringList& list, bool removeOld) {
	if (this->m_mutexLock.Reserve(1)) {
		merge(&this->m_data, list, removeOld);
		this->m_mutexLock.Release();
	}
	this->m_eventChanged.Set();
}
/**************************************************************/


AppRcfgTask::AppRcfgTask(const char* hostAddress,int port) : Base( ),
	m_status(STATUS_IDLE), m_port(port), m_hostAddress(hostAddress), 
	m_msgError(), m_errorCode(RESULT_SUCCESS), m_mutexStatus( ) , m_logInfo( ), m_logErrors( )
{ 
	REQUIRE(0 != hostAddress);
	REQUIRE(port>0);
}


AppRcfgTask::Status_t AppRcfgTask::getStatus() {
	Status_t status = STATUS_IDLE;
	if (m_mutexStatus.Reserve(-1)) {
		status = m_status;
		m_mutexStatus.Release();
		return status;
	}	
	return STATUS_IDLE;
}

const char* AppRcfgTask::getMsgError( ) {
	const char* s = 0;
	if (m_mutexStatus.Reserve(-1)) {
		s = m_msgError.c_str();
		m_mutexStatus.Release();
	}
	return s;
}

HttpRCfgResult_t AppRcfgTask::getErrorCode() {
	HttpRCfgResult_t res = RESULT_SUCCESS;
	if (m_mutexStatus.Reserve(-1)) {
		res = m_errorCode;
		m_mutexStatus.Release();
	}
	return res;
}



BOOL AppRcfgTask::logChanged(LogType_t type, CStringList* pStringList) {
	BOOL result = FALSE;	
	if (TYPE_LOGGING_INFO == type) {
		result = this->m_logInfo.Changed(pStringList);
	}
	else if (TYPE_LOGGING_ERRORS == type) {
		result = this->m_logErrors.Changed(pStringList);
	}
	return result;
}

// protected

void AppRcfgTask::setStatus(Status_t status) {
	if (m_mutexStatus.Reserve(-1)) {
		m_status = status;
		m_mutexStatus.Release();
	}
}

void AppRcfgTask::setError(HttpRCfgResult_t ec,const char* msg) {
	if (m_mutexStatus.Reserve(-1)) {
		m_errorCode = ec;
		m_msgError = msg;
		m_mutexStatus.Release();
	}	
}

const char* AppRcfgTask::getHostAddress( ) const {
	return m_hostAddress.c_str();
}
int AppRcfgTask::getPort() const {
	return m_port;
}

void AppRcfgTask::ThreadCtor() {
	setStatus(STATUS_BUSY);
	this->Base::ThreadCtor();
}

void AppRcfgTask::ThreadDtor() {
	const Status_t status = getStatus();
	ASSERT(STATUS_BUSY != status);
	this->Base::ThreadDtor();
}

// protected:
void AppRcfgTask::addLogMessage(const char* str) {
	this->m_logInfo.addMessage(str);
}

void AppRcfgTask::addLogMessages(const CStringList& list, bool removeOld) {
	this->m_logInfo.addMessages(list, removeOld);
}

void AppRcfgTask::addErrorMessage(const CStringList& list) {
	this->m_logErrors.addMessages(list, true);
}

/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskConnect_Base::AppRcfgTaskConnect_Base(const char* hostAddress, int port) :
	Base(hostAddress, port), m_nodeInfo(), m_mutexLock() { }

AppRcfgTaskConnect_Base::~AppRcfgTaskConnect_Base() { }

BOOL AppRcfgTaskConnect_Base::getNodeInfo(AppCfgNodeInfo* pNodeInfo)
{
	if (!m_mutexLock.Reserve(1)) {
		return FALSE;
	}
	*pNodeInfo = m_nodeInfo;
	m_mutexLock.Release();
	return TRUE;
}

void AppRcfgTaskConnect_Base::setNodeInfo(const AppCfgNodeInfo& nodeInfo) {
	if (m_mutexLock.Reserve(-1)) {
		m_nodeInfo = nodeInfo;
		m_mutexLock.Release();
	}
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskConnect::AppRcfgTaskConnect(const char* hostAddress,int port) :
	Base(hostAddress,port) { }

AppRcfgTaskConnect::~AppRcfgTaskConnect( ) { }

DWORD AppRcfgTaskConnect::Run(LPVOID arg) {
	taskRequestNodeInfo( );		
	while(STATUS_BUSY == this->getStatus());
	::Sleep(1000);
	return m_ThreadCtx.m_dwExitCode;
}

bool AppRcfgTaskConnect::taskRequestNodeInfo() {
	AppCfgNodeInfo nodeInfo;
	const HttpRCfgResult_t res = httprcfg_sendGetNodeInfo(getHostAddress(), getPort() , &nodeInfo);
	if (RESULT_SUCCESS != res) {	
		char errorMsg[256];
		sprintf(errorMsg,"Request node info failed. error='%s'",toString(res));			
		this->setError(res, errorMsg);		
		this->setStatus(STATUS_ERROR);	
		return false;
	}
	this->setNodeInfo(nodeInfo);
	this->setStatus(STATUS_DONE);
	return true;
}
/**************************************************************/



/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskRequestCloudFirmwareInfo::AppRcfgTaskRequestCloudFirmwareInfo(const char* hostAddress, int port) :
	Base(hostAddress, port), m_info(), m_mutexLock() { }

AppRcfgTaskRequestCloudFirmwareInfo::~AppRcfgTaskRequestCloudFirmwareInfo() { }

DWORD AppRcfgTaskRequestCloudFirmwareInfo::Run(LPVOID arg) {
	taskRequestInfo();
	while (STATUS_BUSY == this->getStatus());
	::Sleep(1000);
	return m_ThreadCtx.m_dwExitCode;
}

bool AppRcfgTaskRequestCloudFirmwareInfo::taskRequestInfo() {
	AppRemoteCfgFirmwareVersionInfo info;
	const HttpRCfgResult_t res = httprcfg_sendGetCloudFirmwareVersions(getHostAddress(), getPort(), &info);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Request firmware version information. error='%s'", toString(res));
		this->setError(res, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	this->setInfo(info);
	this->setStatus(STATUS_DONE);
	return true;
}

void AppRcfgTaskRequestCloudFirmwareInfo::setInfo(const AppRemoteCfgFirmwareVersionInfo& info) {
	if (m_mutexLock.Reserve(-1)) {
		m_info = info;
		m_mutexLock.Release();
	}
}

BOOL AppRcfgTaskRequestCloudFirmwareInfo::getInfo(AppRemoteCfgFirmwareVersionInfo* pInfo) {
	if (!m_mutexLock.Reserve(1)) {
		return FALSE;
	}
	*pInfo = m_info;
	m_mutexLock.Release();
	return TRUE;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskDownloadFile::AppRcfgTaskDownloadFile(const char* url, const char* destFileName) :
	Base("localhost", 8080), m_url(url) , m_destFileName(destFileName) { }

AppRcfgTaskDownloadFile::~AppRcfgTaskDownloadFile() { }

DWORD AppRcfgTaskDownloadFile::Run(LPVOID arg) {
	taskDownloadFile();
	while (STATUS_BUSY == this->getStatus());
	::Sleep(1000);
	return m_ThreadCtx.m_dwExitCode;
}

bool AppRcfgTaskDownloadFile::taskDownloadFile() {
	const HttpRCfgResult_t res = httprcfg_sendDownloadFile((LPCSTR) m_url, (LPCSTR)m_destFileName);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Request download file. error='%s'", toString(res));
		this->setError(res, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	this->setStatus(STATUS_DONE);
	return true;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTask_https::AppRcfgTask_https(AppCfgClientCredentials credentials) :
	m_credentials(credentials) { }
AppRcfgTask_https::~AppRcfgTask_https() { }
	
const AppCfgClientCredentials AppRcfgTask_https::getCredentials() const {
	return m_credentials;
}

void AppRcfgTask_https::updateCredentials(const AppCfgClientCredentials& r) {
	m_credentials = r;
}

const char* AppRcfgTask_https::getClientID() {
	return (LPCSTR) m_credentials.getClientID();
}
const char* AppRcfgTask_https::getAccessToken() {
	return (LPCSTR)m_credentials.getAccessToken();
}
const char* AppRcfgTask_https::getRefreshToken() {
	return (LPCSTR)m_credentials.getRefreshToken();
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskConnectHttps::AppRcfgTaskConnectHttps(const char* hostAddress, int port, const char* password, AppCfgClientCredentials credentials) :
	Base(hostAddress, port), AppRcfgTask_https(credentials), m_password(password), m_mode(MODE_CONNECT) { }

AppRcfgTaskConnectHttps::AppRcfgTaskConnectHttps(const char* hostAddress, int port, AppCfgClientCredentials credentials) :
	Base(hostAddress, port), AppRcfgTask_https(credentials), m_password(), m_mode(MODE_REFRESH)  { }

AppRcfgTaskConnectHttps::~AppRcfgTaskConnectHttps() {
}

DWORD AppRcfgTaskConnectHttps::Run(LPVOID arg) {

	if (MODE_CONNECT == m_mode) {
		if (!taskCreateClient()) {
			goto exit;
		}
	}
	if (!taskRefreshToken()) {
		goto exit;
	}
	if (!taskRequestNodeInfo()) {
		goto exit;
	}
	this->setStatus(STATUS_DONE);

exit:
	// eventje wachten tot de mainthread alle info heeft....
	::Sleep(1000);
	return m_ThreadCtx.m_dwExitCode;
}

bool AppRcfgTaskConnectHttps::taskCreateClient() {

	// Bij een leeg paswoord kunnen we geen client aanmaken en gaan proberen de credentials te gebruiken...
	const char* const password = (LPCSTR)m_password;
	if (0 == password || 0==strcmp(password,"")) {
		return true;
	}

	AppCfgClientCredentials response;
	const char* const clientID = this->getClientID();	

	addLogMessage("Authentication create client, please wait...");

	const HttpRCfgResult_t res = httprcfg_sendAuthCreateClient(getHostAddress(), getPort(), clientID, password, &response);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Authentication create client failed. error='%s'", toString(res));
		addLogMessage(errorMsg);
		this->setError(res,errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}

	this->updateCredentials(response);
	addLogMessage("Authentication create client done");
	return true;
}

bool AppRcfgTaskConnectHttps::taskRefreshToken() {

	addLogMessage("Refresh Access token.");

	AppCfgClientCredentials response;
	const char* clientID = this->getClientID();
	const char* refreshToken = this->getRefreshToken();
	if (0 == clientID || 0 == strcmp(clientID, "")) {
		const char* const errorMsg = "No clientid.";
		addLogMessage(errorMsg);
		this->setError(RESULT_EC_FAILED, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	if (0 == refreshToken || 0 == strcmp(refreshToken, "")) {
		const char* const errorMsg = "No refresh accesstoken.";	
		addLogMessage(errorMsg);
		this->setError(RESULT_EC_FAILED, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	const HttpRCfgResult_t res = httprcfg_sendAuthRefreshAccessCode(getHostAddress(), getPort(), clientID, refreshToken, &response);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Refresh Access token failed. error='%s'", toString(res));
		addLogMessage(errorMsg);
		this->setError(res,errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}

	this->updateCredentials(response);
	addLogMessage("Refresh Access token done");
	return true;
}

bool AppRcfgTaskConnectHttps::taskRequestNodeInfo() {

	const char* clientID = this->getClientID();
	const char* accessToken = this->getAccessToken();
	if (0 == clientID || 0 == strcmp(clientID, "")) {
		return false;
	}
	if (0 == accessToken || 0 == strcmp(accessToken, "")) {
		return false;
	}

	AppCfgNodeInfo nodeInfo;
	addLogMessage("Connecting to the node");
	const HttpRCfgResult_t res = httpsrcfg_sendGetNodeInfo(getHostAddress(), getPort(), clientID, accessToken, &nodeInfo);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Connecting to the node failed. error='%s'", toString(res));
		addLogMessage(errorMsg);
		this->setError(res,errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}

	this->setNodeInfo(nodeInfo);
	addLogMessage("Connecting to the node done");
	return true;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskBindings_Base::ProgressInfo::ProgressInfo() : DataInfo(), m_data() { }
/**************************************************************/
AppRcfgTaskBindings_Base::AppRcfgTaskBindings_Base(const char* hostAddress, int port, const char* path, const AppCfgBindAction& refAction) :
	Base(hostAddress, port), m_action(refAction), m_path(path), m_progressInfo() { }

AppRcfgTaskBindings_Base::~AppRcfgTaskBindings_Base() { }

BOOL AppRcfgTaskBindings_Base::progressChanged(AppCfgBindInfoNodes* pInfoNodes) {
	BOOL result = m_progressInfo.m_eventChanged.Wait(0);
	if (result) {
		if (0 != pInfoNodes) {
			result = FALSE;
			if (m_progressInfo.m_mutexLock.Reserve(1)) {
				*pInfoNodes = m_progressInfo.m_data;
				m_progressInfo.m_mutexLock.Release();
				result = TRUE;
			}
		}
		m_progressInfo.m_eventChanged.Reset();
	}
	return result;
}

DWORD AppRcfgTaskBindings_Base::Run(LPVOID arg) {

	if (!taskUploadBindingFiles()) {
		goto exit;
	}

	if (!taskStartTask()) {
		goto exit;
	}

	do
	{
		if (!taskRequestProgressInfo()) {
			goto exit;
		}
		Sleep(250);

		if (!taskRequestLogInfo()) {
			goto exit;
		}
		Sleep(500);

	} while (STATUS_BUSY == this->getStatus());

	// Hier eventje wachten zodat de GUI-Thread alle data ontvangen heeft...
	::Sleep(500);
exit:
	return m_ThreadCtx.m_dwExitCode;
}

bool AppRcfgTaskBindings_Base::taskUploadBindingFiles() {
	const AppCfgBindAction& refAction = this->getAction();
	if (AppCfgBindAction::WRITE != refAction.getAction()) {
		return true;	// Enkel bij bindings write moeten er bestanden geupload worden.
	}

	const int nrOfNodes = refAction.getCount();
	if (0 == nrOfNodes) {
		char errorMsg[256];
		sprintf(errorMsg, "Upload binding files failed. Reason:'No nodes selected");
		this->setError(RESULT_EC_FAILED,errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}

	for (int i = 0; i<nrOfNodes; ++i) {
		char fileName[1024];
		AppRCfgResult response;
		const int nodeAddress = refAction.get(i);
		sprintf(fileName, "%s\\bind%02x.txt", this->getPath(), nodeAddress);

		const HttpRCfgResult_t res = this->sendUploadBindFile(nodeAddress, fileName, &response);
		if (RESULT_SUCCESS != res) {
			char errorMsg[256];
			sprintf(errorMsg, "Upload binding file failed. error='%s'", toString(res));
			this->setError(res,errorMsg);
			this->setStatus(STATUS_ERROR);
			return false;
		}

		if (AppRCfgResult::RESULT_SUCCESS != response.getResultCode()) {
			char errorMsg[512];
			const CString& ecMsg = response.getErrorMsg();
			/* const CString& ecMsgDetail = response.getErrorDetailMsg(); */
			sprintf(errorMsg, "Upload binding file failed. Server replied '%s'.", (LPCSTR)ecMsg);
			this->setError(RESULT_EC_FAILED,errorMsg);
			this->setStatus(STATUS_ERROR);
			return false;
		}
	}
	return true;
}

bool AppRcfgTaskBindings_Base::taskStartTask() {

	AppRCfgResult response;
	const HttpRCfgResult_t res = this->sendStartBindingsAction(this->getAction(), &response);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Upload binding file failed. error='%s'", toString(res));
		this->setError(res,errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}

	if (AppRCfgResult::RESULT_SUCCESS == response.getResultCode()) {
		return true;
	}

	char errorMsg[1024];
	const CString& ecMsg = response.getErrorMsg();
	/* const CString& ecMsgDetail = response.getErrorDetailMsg(); */
	sprintf(errorMsg, "Start bindings failed. Server replied '%s'.", (LPCSTR)ecMsg);
	this->setError(RESULT_EC_FAILED, errorMsg);
	this->setStatus(STATUS_ERROR);
	return false;
}

bool AppRcfgTaskBindings_Base::taskRequestLogInfo() {

	AppCfgLogInfo progressLogInfo;
	const HttpRCfgResult_t res = this->sendBindingsGetLog(&progressLogInfo);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Request log info failed. error='%s'", toString(res));
		this->setError(res,errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	addLogMessages(progressLogInfo.getStrings());
	return true;
}

bool AppRcfgTaskBindings_Base::taskRequestProgressInfo() {

	AppCfgBindInfoNodes progessInfoNodes;
	const HttpRCfgResult_t res = this->sendBindingsGetProgress(&progessInfoNodes);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Request progress info failed. error='%s'", toString(res));
		this->setError(res, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	this->handleResponseBindingsProgress(progessInfoNodes.getStatus());
	this->setProgressInfo(progessInfoNodes);
	return true;
}

void AppRcfgTaskBindings_Base::setProgressInfo(const AppCfgBindInfoNodes& r) {
	if (m_progressInfo.m_mutexLock.Reserve(1)) {
		m_progressInfo.m_data = r;
		m_progressInfo.m_mutexLock.Release();
	}
	m_progressInfo.m_eventChanged.Set();
}

const char* AppRcfgTaskBindings_Base::getPath() const {
	return m_path.c_str();
}

void AppRcfgTaskBindings_Base::handleResponseBindingsProgress(BindingFileFsmStatus_t status) {
	if (BINDINGFILE_FSM_STATUS_DONE == status) {
		this->setStatus(STATUS_DONE);
	}
	else if (BINDINGFILE_FSM_STATUS_CANCELLED == status) {
		char errorMsg[256];
		sprintf(errorMsg, "Binding fsm cancelled");
		this->setStatus(STATUS_CANCEL);
	}
	else if (BINDINGFILE_FSM_STATUS_ERROR == status) {
		char errorMsg[256];
		sprintf(errorMsg, "Binding fsm error");
		this->setStatus(STATUS_ERROR);
	}
	else {
		// absorbe...
	}
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskBindings::AppRcfgTaskBindings(const char* hostAddress,int port,const char* path, const AppCfgBindAction& refAction) : 
	Base(hostAddress,port,path,refAction) { }

AppRcfgTaskBindings::~AppRcfgTaskBindings( ) { }

// Virtuele functies...
HttpRCfgResult_t AppRcfgTaskBindings::sendUploadBindFile(int nodeAddress, const char* fileName, AppRCfgResult* response) {
	return httprcfg_sendUploadBindFile(getHostAddress(), getPort(), nodeAddress, fileName, response);
}
HttpRCfgResult_t AppRcfgTaskBindings::sendStartBindingsAction(const AppCfgBindAction& action, AppRCfgResult* response) {
	return httprcfg_sendStartBindingsAction(getHostAddress(), getPort(), this->getAction(), response);
}
HttpRCfgResult_t AppRcfgTaskBindings::sendBindingsGetLog(AppCfgLogInfo* progressLogInfo) {
	return httprcfg_sendBindingsGetLog(getHostAddress(), getPort(), progressLogInfo);
}
HttpRCfgResult_t AppRcfgTaskBindings::sendBindingsGetProgress(AppCfgBindInfoNodes* progessInfoNodes) {
	return httprcfg_sendBindingsGetProgress(getHostAddress(), getPort(), progessInfoNodes);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskBindingsHttps::AppRcfgTaskBindingsHttps(const char* hostAddress, int port, const char* path, const AppCfgBindAction& refAction, AppCfgClientCredentials credentials) :
	Base(hostAddress, port, path, refAction), AppRcfgTask_https(credentials) { }
AppRcfgTaskBindingsHttps::~AppRcfgTaskBindingsHttps() { }

// Virtuele functies...
HttpRCfgResult_t AppRcfgTaskBindingsHttps::sendUploadBindFile(int nodeAddress, const char* fileName, AppRCfgResult* response) {
	return httpsrcfg_sendUploadBindFile(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(),
		nodeAddress, fileName, response);
}
HttpRCfgResult_t AppRcfgTaskBindingsHttps::sendStartBindingsAction(const AppCfgBindAction& action, AppRCfgResult* response) {
	return httpsrcfg_sendStartBindingsAction(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), 
		this->getAction(), response);
}
HttpRCfgResult_t AppRcfgTaskBindingsHttps::sendBindingsGetLog(AppCfgLogInfo* progressLogInfo) {
	return httpsrcfg_sendBindingsGetLog(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), progressLogInfo);
}
HttpRCfgResult_t AppRcfgTaskBindingsHttps::sendBindingsGetProgress(AppCfgBindInfoNodes* progessInfoNodes) {
	return httpsrcfg_sendBindingsGetProgress(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), progessInfoNodes);
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const AppRcfgTaskUploadBackupBindPrj_Base::DESTINATION_PATH = "/dt/backup/bind/";
/**************************************************************/

AppRcfgTaskUploadBackupBindPrj_Base::AppRcfgTaskUploadBackupBindPrj_Base(const char* hostAddress,int port,
															   const char* path, const char* fileName) : 
	Base(hostAddress, port), m_path(path), m_archiveName( ),  m_oldFiles( )
{ 
	REQUIRE(0 != fileName);
	m_archiveName = std::string(path) + std::string("//") + std::string(fileName) + std::string(".zip");
}

AppRcfgTaskUploadBackupBindPrj_Base::~AppRcfgTaskUploadBackupBindPrj_Base( ) {	 }

DWORD AppRcfgTaskUploadBackupBindPrj_Base::Run(LPVOID arg) {
	
	// Connectie aanmaken met de host: Testen of de fileserver aanwezig is...
	// TODO: Kunnen evt op de software versie controleren of de 'fs' ondersteund wordt...
	if (!taskRequestNodeInfo()) {
		goto exit;
	}

	// Archive bestand aanmaken.
	if (!taskArchiveFiles()) {
		goto exit;
	}

	// Opvragen van de bestandsnamen van de oude backup bestanden in de dir.
	taskRequestDirInfo( );

	// Verwijderen van de oude backup bestaden...
	// Doen deze voor de upload van de nieuwe backup want als deze dezelfde naam heeft dan wordt deze verwijderd...
	taskDeleteOldFiles( );

	// Upload van de nieuwe backup...
	if (!taskUploadBackup()) {
		goto exit;
	}	

	this->setStatus(STATUS_DONE);
		
exit:
	// Archive bestand verwijderen..
	taskRemoveArchive( );

	// eventje wachten tot de mainthread alle info heeft....
	::Sleep(1000);
	return m_ThreadCtx.m_dwExitCode;
}

bool AppRcfgTaskUploadBackupBindPrj_Base::taskRequestNodeInfo() {
	AppCfgNodeInfo nodeInfo;
	addLogMessage("Connecting to the file server");
	const HttpRCfgResult_t res = this->sendGetNodeInfo(&nodeInfo);
	if (RESULT_SUCCESS != res) {	
		char errorMsg[256];
		sprintf(errorMsg,"Connecting to the file server failed. error='%s'",toString(res));		
		addLogMessage(errorMsg);
		this->setError(res, errorMsg);		
		this->setStatus(STATUS_ERROR);	
		return false;
	}	

	// TODO: Kunnen evt op de software versie controleren of de 'fs' ondersteund wordt...

	addLogMessage("Connecting to the file server done");
	return true;
}

bool AppRcfgTaskUploadBackupBindPrj_Base::taskRequestDirInfo( ) {
	
	addLogMessage("Request directory info");
	AppCfgFileServerDir dirInfo;
	const HttpRCfgResult_t res = httprcfg_sendRequestDirInfo(getHostAddress(), getPort(), DESTINATION_PATH, &dirInfo);
	if (RESULT_SUCCESS == res) {
		const int count = dirInfo.getNrOfFiles( );
		for (int i=0;i<count;++i) {
			AppCfgFileServerFile *pFile = 0;
			if (dirInfo.getFile(i,&pFile)) {
				if (AppCfgFileServerFile::TYPE_FILE == pFile->getType()) {
					const CString& fileName = pFile->getFileName();
					char msg[256];
					sprintf(msg,"Found old backup:'%s'",(LPCSTR)fileName);
					addLogMessage(msg);
					m_oldFiles.AddTail(fileName);
				}
			}
		}
		return true;
	}
	else if (RESULT_EC_HTTP_STATUSCODE_NOTFOUND == res) {
		addLogMessage("No old backup files found.");		
	}						 
	else {
		
	}
	return false;
}

bool AppRcfgTaskUploadBackupBindPrj_Base::taskDeleteOldFiles( ) {

	bool result = false;
	POSITION pos = m_oldFiles.GetHeadPosition();
	while (pos) {
		CString& tempFileName = m_oldFiles.GetNext(pos);
		char fileName[MAX_PATH];
		sprintf(fileName,"%s%s",DESTINATION_PATH,(LPCSTR) tempFileName);

		AppRCfgResult response;
		const HttpRCfgResult_t res = this->sendDeleteFile(fileName,& response);
		if (RESULT_SUCCESS == res) {
		
			char msg[256];
			sprintf(msg,"remove old backup:'%s'",(LPCSTR)tempFileName);
			addLogMessage(msg);

			result = true;
		}		
	}	
	return result;
}

bool AppRcfgTaskUploadBackupBindPrj_Base::taskArchiveFiles( ) {
		
	addLogMessage("Archiving files...");	
	if (!apprcfg_createProjectCfgZipFile(m_path.c_str(),m_archiveName.c_str())) {
		const char* const errorMsg = "Archiving files failed";
		addLogMessage(errorMsg);
		this->setError(RESULT_EC_FAILED, errorMsg);	
		this->setStatus(STATUS_ERROR);	
		return false;
	}
	addLogMessage("Archiving files done");
	return true;
}

bool AppRcfgTaskUploadBackupBindPrj_Base::taskUploadBackup( ) {
	addLogMessage("Upoading backup ...");

	AppRCfgResult response;
	const char* const name = m_archiveName.c_str();
	ASSERT(0 != name);
	const HttpRCfgResult_t res = this->sendUploadFile(name, DESTINATION_PATH, &response);
	if (RESULT_SUCCESS != res) {	
		char errorMsg[256];
		sprintf(errorMsg,"Upload backup failed. error='%s'",toString(res));			
		addLogMessage(errorMsg);
		this->setError(res,errorMsg);		
		this->setStatus(STATUS_ERROR);	
		return false;	
	}

	if (AppRCfgResult::RESULT_SUCCESS == response.getResultCode()) {
		char infoMsg[256];
		sprintf(infoMsg,"Uploading backup '%s' done", m_archiveName.c_str());
		addLogMessage(infoMsg);
		return true;
	}		
		
	char errorMsg[1024];
	const CString& ecMsg = response.getErrorMsg();
	sprintf(errorMsg,"Upload backup failed. Server replied '%s'.",(LPCSTR)ecMsg);
	addLogMessage(errorMsg);
	this->setError(RESULT_EC_FAILED,errorMsg);		
	this->setStatus(STATUS_ERROR);	
	return false;		
}

bool AppRcfgTaskUploadBackupBindPrj_Base::taskRemoveArchive( ) {

	if (DeleteFile(m_archiveName.c_str())) {
		addLogMessage("Deleting temp archive file done");
		return true;
	}
	return false;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskUploadBackupBindPrj::AppRcfgTaskUploadBackupBindPrj(const char* hostAddress, int port, const char* path, const char* fileName) :
	Base(hostAddress, port, path, fileName) { }
AppRcfgTaskUploadBackupBindPrj::~AppRcfgTaskUploadBackupBindPrj() { }

// protected...
HttpRCfgResult_t AppRcfgTaskUploadBackupBindPrj::sendGetNodeInfo(AppCfgNodeInfo* nodeInfo) {
	return  httprcfg_sendGetNodeInfo(getHostAddress(), getPort(), nodeInfo);
}
HttpRCfgResult_t AppRcfgTaskUploadBackupBindPrj::sendDeleteFile(const char* fileName, AppRCfgResult* response) {
	return httprcfg_sendDeleteFile(getHostAddress(), getPort(), fileName, response);
}
HttpRCfgResult_t AppRcfgTaskUploadBackupBindPrj::sendUploadFile(const char* archiveName, const char* destPath, AppRCfgResult* response) {
	REQUIRE(0 != archiveName);
	return httprcfg_sendUploadFile(getHostAddress(), getPort(), archiveName, destPath, response);		
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskUploadBackupBindPrjHttps::AppRcfgTaskUploadBackupBindPrjHttps(const char* hostAddress, int port, const char* path, const char* fileName, AppCfgClientCredentials credentials) :
	Base(hostAddress, port, path, fileName), AppRcfgTask_https(credentials) { }
AppRcfgTaskUploadBackupBindPrjHttps::~AppRcfgTaskUploadBackupBindPrjHttps() { }

// protected...
HttpRCfgResult_t AppRcfgTaskUploadBackupBindPrjHttps::sendGetNodeInfo(AppCfgNodeInfo* nodeInfo) {
	return  httpsrcfg_sendGetNodeInfo(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), nodeInfo);
}
HttpRCfgResult_t AppRcfgTaskUploadBackupBindPrjHttps::sendDeleteFile(const char* fileName, AppRCfgResult* response) {
	return httpsrcfg_sendDeleteFile(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), fileName, response);
}
HttpRCfgResult_t AppRcfgTaskUploadBackupBindPrjHttps::sendUploadFile(const char* archiveName, const char* destPath, AppRCfgResult* response) {
	REQUIRE(0 != archiveName);
	return httpsrcfg_sendUploadFile(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), archiveName, destPath, response);
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskNames_Base::ProgressInfo::ProgressInfo() : DataInfo(), m_data() { }
/**************************************************************/
AppRcfgTaskNames_Base::AppRcfgTaskNames_Base(const char* hostAddress, int port, const char* path, const AppCfgRemoteActionList& refActionList) :
	Base(hostAddress, port), m_listActions(refActionList), m_progressInfo() { }

AppRcfgTaskNames_Base::~AppRcfgTaskNames_Base() { }

BOOL AppRcfgTaskNames_Base::progressChanged(AppCfgCmnProgressInfo* pInfoProgress) {
	BOOL result = m_progressInfo.m_eventChanged.Wait(0);
	if (result) {
		if (0 != pInfoProgress) {
			result = FALSE;
			if (m_progressInfo.m_mutexLock.Reserve(1)) {
				*pInfoProgress = m_progressInfo.m_data;
				m_progressInfo.m_mutexLock.Release();
				result = TRUE;
			}
		}
		m_progressInfo.m_eventChanged.Reset();
	}
	return result;
}

DWORD AppRcfgTaskNames_Base::Run(LPVOID arg) {
	if (!taskStartTask()) {
		goto exit;
	}
	do
	{
		if (!taskRequestProgressInfo()) {
			goto exit;
		}
		Sleep(250);

		if (!taskRequestLogInfo()) {
			goto exit;
		}
		Sleep(500);

	} while (STATUS_BUSY == this->getStatus());

	::Sleep(250);

	// Opvragen van de error log...
	taskRequestErrorsInfo();

	// Hier eventje wachten zodat de GUI-Thread alle data ontvangen heeft...
	::Sleep(500);

exit:
	return m_ThreadCtx.m_dwExitCode;
}

bool AppRcfgTaskNames_Base::taskStartTask() {

	AppRCfgResult response;
	const HttpRCfgResult_t res = this->sendStartWriteNames(this->getListActions(), &response);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Start write names failed. error='%s'", toString(res));
		this->setError(res, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}

	if (AppRCfgResult::RESULT_SUCCESS == response.getResultCode()) {
		return true;
	}

	char errorMsg[1024];
	const CString& ecMsg = response.getErrorMsg();
	sprintf(errorMsg, "Start write names failed. Server replied '%s'.", (LPCSTR)ecMsg);
	this->setError(RESULT_EC_FAILED, errorMsg);
	this->setStatus(STATUS_ERROR);
	return false;
}

bool AppRcfgTaskNames_Base::taskRequestLogInfo() {
	AppCfgLogInfo progressLogInfo;
	const HttpRCfgResult_t res = this->sendBindingsGetLogInfo(&progressLogInfo);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Request log info failed. error='%s'", toString(res));
		this->setError(res, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	addLogMessages(progressLogInfo.getStrings());
	return true;
}

bool AppRcfgTaskNames_Base::taskRequestErrorsInfo() {
	AppCfgLogInfo progressLogInfo;
	const HttpRCfgResult_t res = this->sendBindingsGetLogErrors(&progressLogInfo);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Request error log info failed. error='%s'", toString(res));
		this->setError(res, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	addErrorMessage(progressLogInfo.getStrings());
	return true;
}

bool AppRcfgTaskNames_Base::taskRequestProgressInfo() {
	AppCfgCmnProgressInfo progessInfoNodes;
	const HttpRCfgResult_t res = this->sendBindingsGetProgress(&progessInfoNodes);
	if (RESULT_SUCCESS != res) {
		char errorMsg[256];
		sprintf(errorMsg, "Request progress info failed. error='%s'", toString(res));
		this->setError(res, errorMsg);
		this->setStatus(STATUS_ERROR);
		return false;
	}
	this->handleProgressInfo(progessInfoNodes);
	this->setProgressInfo(progessInfoNodes);
	return true;
}

void AppRcfgTaskNames_Base::handleProgressInfo(const AppCfgCmnProgressInfo& info) {
	const AppCfgCmnProgressInfo::State_t state = info.getState();
	switch(state) {
		case AppCfgCmnProgressInfo::STATE_IDLE:
			// absorbe...
		break;
		case AppCfgCmnProgressInfo::STATE_BUSY:
			// absorbe
		break;
		case AppCfgCmnProgressInfo::STATE_DONE_SUCCESS:
			this->setStatus(STATUS_DONE);
		break;
		case AppCfgCmnProgressInfo::STATE_DONE_ERROR:
#if(0)
			char errorMsg[256];
			sprintf(errorMsg, "Write names fsm error");
#endif 
			this->setStatus(STATUS_ERROR);
		break;
		case AppCfgCmnProgressInfo::STATE_DONE_CANCELLED:
#if(0)
			char errorMsg[256];
			sprintf(errorMsg, "Write names fsm cancelled");		
#endif
			this->setStatus(STATUS_CANCEL);
		break;
	}
}

void AppRcfgTaskNames_Base::setProgressInfo(const AppCfgCmnProgressInfo& r) {
	if (m_progressInfo.m_mutexLock.Reserve(1)) {
		m_progressInfo.m_data = r;
		m_progressInfo.m_mutexLock.Release();
	}
	m_progressInfo.m_eventChanged.Set();
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskNames::AppRcfgTaskNames(const char* hostAddress, int port, const char* path, const AppCfgRemoteActionList& refActionList) :
	Base(hostAddress, port, path, refActionList) { }
AppRcfgTaskNames::~AppRcfgTaskNames() { }

HttpRCfgResult_t AppRcfgTaskNames::sendStartWriteNames(const AppCfgRemoteActionList& listActions, AppRCfgResult* response) {
	return httprcfg_sendStartSendNames(getHostAddress(), getPort(), this->getListActions(), response);
}
HttpRCfgResult_t AppRcfgTaskNames::sendBindingsGetLogInfo(AppCfgLogInfo* progressLogInfo) {
	return httprcfg_sendNamesGetLogInfo(getHostAddress(), getPort(), progressLogInfo);
}
HttpRCfgResult_t AppRcfgTaskNames::sendBindingsGetLogErrors(AppCfgLogInfo* progressLogInfo) {
	return httprcfg_sendNamesGetLogErrors(getHostAddress(), getPort(), progressLogInfo);
}
HttpRCfgResult_t AppRcfgTaskNames::sendBindingsGetProgress(AppCfgCmnProgressInfo* progressInfo) {
	return httprcfg_sendNamesGetProgressInfo(getHostAddress(), getPort(), progressInfo);
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRcfgTaskNamesHttps::AppRcfgTaskNamesHttps(const char* hostAddress, int port, const char* path, const AppCfgRemoteActionList& refActionList, AppCfgClientCredentials credentials) :
	Base(hostAddress, port, path, refActionList), AppRcfgTask_https(credentials) { }
AppRcfgTaskNamesHttps::~AppRcfgTaskNamesHttps() { }

HttpRCfgResult_t AppRcfgTaskNamesHttps::sendStartWriteNames(const AppCfgRemoteActionList& listActions, AppRCfgResult* response) {
	return httpsrcfg_sendStartSendNames(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), this->getListActions(), response);
}
HttpRCfgResult_t AppRcfgTaskNamesHttps::sendBindingsGetLogInfo(AppCfgLogInfo* progressLogInfo) {
	return httpsrcfg_sendNamesGetLogInfo(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), progressLogInfo);
}
HttpRCfgResult_t AppRcfgTaskNamesHttps::sendBindingsGetLogErrors(AppCfgLogInfo* progressLogInfo) {
	return httpsrcfg_sendNamesGetLogErrors(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(), progressLogInfo);
}
HttpRCfgResult_t AppRcfgTaskNamesHttps::sendBindingsGetProgress(AppCfgCmnProgressInfo* progressInfo) {
	return httpsrcfg_sendNamesGetProgressInfo(getHostAddress(), getPort(), this->getClientID(), this->getAccessToken(),  progressInfo);
}
/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/