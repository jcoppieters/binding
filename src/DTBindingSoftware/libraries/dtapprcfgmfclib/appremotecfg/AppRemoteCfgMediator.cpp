#include "stdafx.h"
#include "AppRemoteCfgMediator.h"

#include "AppRemoteCfgCtx.h"
#include "AppRemoteCfgUtils.h"
/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
IAppRemoteCfgFsm_Host::~IAppRemoteCfgFsm_Host() { }

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgMediator::AppRemoteCfgMediator(AppRemoteCfgCtx& refCtx,IAppRemoteCfgGUI* refGui, IAppRemoteCfgSystemEvents* ptrSystemEvents) :
	m_refCtx(refCtx), m_ptrGui(refGui), m_ptrSystemEvents(ptrSystemEvents) { }
AppRemoteCfgMediator::~AppRemoteCfgMediator() { }

void AppRemoteCfgMediator::setSubscriberGui(IAppRemoteCfgGUI* ptrGui) {
	m_ptrGui = ptrGui;
}
void AppRemoteCfgMediator::setSubscriberSystemEvents(IAppRemoteCfgSystemEvents* ptr) {
	m_ptrSystemEvents = ptr;
}
// IAppRemoteCfgFsm_Host
void AppRemoteCfgMediator::addMessage(const char* msg, bool eraseOld) {
	if (m_ptrGui) {
		m_ptrGui->addMessage(msg,eraseOld);
	}
}
void AppRemoteCfgMediator::addMessages(CStringList& messages, bool eraseOld) {
	if (m_ptrGui) {		
		m_ptrGui->addMessages(messages,eraseOld);
	}
}

void AppRemoteCfgMediator::taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status, 
		HttpRCfgResult_t errorCode, const char* msgError) 
{
	IAppRemoteCfgGUI::HttpError_t ec = IAppRemoteCfgGUI::HTTP_EC_NONE;

	switch(task) 
	{
		case APPREMOTE_CFG_TASK_CONNECT:
		case APPREMOTE_CFG_TASK_REFRESHACCTOKENS:
		{			
			switch(status) 
			{
				case APPREMOTE_CFG_TASK_STATUS_STARTED:
				{
					if (m_ptrGui) {
						m_ptrGui->updateGuiButtons();
					}
				} break;
				case APPREMOTE_CFG_TASK_STATUS_DONE:
				{
					if (m_refCtx.getCredentials().getUseSecure()) {
						saveClientCredentials();
					}	
					if (m_ptrGui) {
						m_ptrGui->updateGuiButtons();
					}
				} break;
				case APPREMOTE_CFG_TASK_STATUS_FAILED:
				{
					if (m_ptrGui) {
						m_ptrGui->updateGuiButtons();
					}
					
					if (RESULT_EC_HTTP_CURL == errorCode) {
						ec = IAppRemoteCfgGUI::HTTP_EC_CONNECT;
					}
					else if (RESULT_EC_HTTP_STATUSCODE_AUTHORIZE == errorCode) {						
						deleteClientCredentials(m_refCtx.getHostName(), m_refCtx.getHostPort());
						ec = IAppRemoteCfgGUI::HTTP_EC_AUTHORIZATION;
					}					
				} break;
			}
		} break;

		case APPREMOTE_CFG_TASK_BINDINGS:
		case APPREMOTE_CFG_TASK_UPLOADBACKUP: 
		{
			if (m_ptrGui) {
				m_ptrGui->updateGuiButtons();
			}

			if (RESULT_EC_HTTP_STATUSCODE_AUTHORIZE == errorCode) {
				deleteClientCredentials(m_refCtx.getHostName(), m_refCtx.getHostPort());
				ec = IAppRemoteCfgGUI::HTTP_EC_AUTHORIZATION;
			}
		} break;
	}		

	if (IAppRemoteCfgGUI::HTTP_EC_NONE != ec) {
		if (m_ptrGui) {
			m_ptrGui->onHttpTaskError(ec);
		}
	}

	if (m_ptrSystemEvents) {
		m_ptrSystemEvents->taskStatus(task,status,errorCode,msgError);
	}
}

void AppRemoteCfgMediator::taskBindingsProgressInfo(const AppCfgBindInfoNodes& refInfo) {
	if (m_ptrSystemEvents) {
		m_ptrSystemEvents->taskBindingsProgressInfo(refInfo);
	}
}

bool AppRemoteCfgMediator::loadClientCredentials(const char* hostName, int port) {
	bool result = false;
	clearClientCredentials( );
	const CString dir = getPathClientCredentials();
	if (!dir.IsEmpty()) {		
		AppCfgClientCredentials credentials;
		if (AppRemoteCfgUtils::loadCredentials(&credentials, (LPCSTR) dir, hostName, port)) {
			AppRemoteCfgClientCredentials& ref = m_refCtx.getCredentials();
			ref.updateCredentials(credentials);					
			result = true;
		}
	}
	return result;
}

// Protected:

CString AppRemoteCfgMediator::getPathClientCredentials() {
	const CString& projectDir = m_refCtx.getWorkingDirectory();
	if (!projectDir.IsEmpty()) {
		return projectDir + "/data";
	}

#if(0)
	// Indien er geen project dir is de huidige directory gebruiken....
	char pbBuffer[MAX_PATH];
	DWORD dwRet = GetCurrentDirectory(sizeof(pbBuffer), pbBuffer);
	if (dwRet>0 && dwRet<sizeof(pbBuffer)) {
		return CString(pbBuffer);
	}
#endif

	return CString();
}

bool AppRemoteCfgMediator::deleteClientCredentials(const char* hostName, int port) {
	bool result = false;
	clearClientCredentials();
	const CString dir = getPathClientCredentials();
	if (!dir.IsEmpty()) {
		result = AppRemoteCfgUtils::deleteCredentials((LPCSTR)dir,hostName,port);
	}
	return result;
}

bool AppRemoteCfgMediator::clearClientCredentials() {	
	m_refCtx.getCredentials().clear();
	return true;
}

bool AppRemoteCfgMediator::saveClientCredentials() {
	bool result=false;
	const CString dir = getPathClientCredentials();
	if (!dir.IsEmpty()) {
		AppCfgClientCredentials& refCredentials = m_refCtx.getCredentials();
		result = AppRemoteCfgUtils::saveCredentials((LPCSTR)dir, m_refCtx.getHostName(), m_refCtx.getHostPort(), refCredentials);
	}
	return result;
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/

