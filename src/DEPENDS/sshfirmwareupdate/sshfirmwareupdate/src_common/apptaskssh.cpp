#include "stdafx.h"
#include "apptaskssh.h"
#include "sshfirmwareupdate_api.h"
#include "appwrapperssh.h"
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
AppTaskSSH* AppTaskSSH::m_pInstance = NULL;
/***************************************************************************************************/
AppTaskSSH::AppTaskSSH(const char* hostName, const char* userName, const char* password,
					   const char* applicationName, const char* localFileName, const char* remoteFileName) : Base(),
	m_hostName(hostName), m_userName(userName), m_password(password), 
	m_applicationName(applicationName), m_localFileName(localFileName), 
	m_remoteFileName(remoteFileName),
	m_status(TASK_STATUS_IDLE),  m_mutexStatus( ), m_mutexQueueMsg( ), m_queueMessages( )
{
	m_pInstance = this;
}

AppTaskSSH::~AppTaskSSH() {
	m_pInstance = NULL;

	if (m_mutexQueueMsg.Reserve(-1)) {
		while (m_queueMessages.size()>0) {
			SshMessage* msg = m_queueMessages.front();
			m_queueMessages.pop();
			delete msg;
		}
		m_mutexQueueMsg.Release();
	}
}

DWORD AppTaskSSH::Start(void* arg) {
	return this->Base::Start(arg);
}

SshTaskStatus_t AppTaskSSH::getStatus() {
	SshTaskStatus_t status = TASK_STATUS_IDLE;
	if (m_mutexStatus.Reserve(-1)) {
		status = m_status;
		m_mutexStatus.Release();
		return status;
	}	
	return TASK_STATUS_UNKNOWN;
}

SshMessage* AppTaskSSH::pop( ) {
	SshMessage* msg = NULL;
	if (m_mutexQueueMsg.Reserve(-1)) {
		if (m_queueMessages.size()>=1) {
			msg = m_queueMessages.front();
			m_queueMessages.pop();
		}
		m_mutexQueueMsg.Release();
		return msg;
	}	
	return msg;
}

void AppTaskSSH::push(const SshMessage& msg) {
	if (m_mutexQueueMsg.Reserve(-1)) {
		m_queueMessages.push(new SshMessage(msg));
		m_mutexQueueMsg.Release();
	}
}

// Worker thread
void AppTaskSSH::notifyStatus(bool flagErrorMsg,const char* msg) {
	if (flagErrorMsg) {
		const SshMessage newMsg(SshMessage::TYPE_MSG_ERROR,msg);
		this->push(newMsg);
	}
	else {
		const SshMessage newMsg(SshMessage::TYPE_MSG_INFO,msg);
		this->push(newMsg);
	}
}

// Worker thread
void AppTaskSSH::notifyTaskStatus(SshTaskStatus_t status, const char* msg) {
	const SshMessage newMsg(status,msg);
	this->push(newMsg);
}

// Worker thread
DWORD AppTaskSSH::Run(LPVOID arg) {

	DWORD dwExitCode = 0;

	setStatus(TASK_STATUS_BUSY_CONNECT);

	////////////////////////////////////////////////////////////////
	// Init.
	////////////////////////////////////////////////////////////////
	notifyStatus(false, "Initialising SSH.");
	STRUCT_SSH_CONTECT_PTR sshSession = ssh_init(&callbackInfo);
	if (NULL == sshSession) {
		setStatus(TASK_STATUS_ERROR);
		notifyTaskStatus(TASK_STATUS_ERROR, "Initialising SSH failed");
		return -1;
	}

	////////////////////////////////////////////////////////////////
	// Connectie maken.
	////////////////////////////////////////////////////////////////	
	const char* const hostName = m_hostName.c_str();
	notifyStatus(false, "Connect to the host.");
	if (ssh_connectSocket(sshSession, hostName, 22, &callbackInfo)<0) {
		setStatus(TASK_STATUS_ERROR);
		notifyTaskStatus(TASK_STATUS_ERROR, "Connect to the host failed.");
		ssh_shutdown(sshSession, false);		// sedert v1.1
		return -2;
	}

	notifyStatus(false, "Start SSH session");
	if (ssh_connectSSHServer(sshSession,m_userName.c_str(),m_password.c_str(), &callbackInfo)<0) {
		setStatus(TASK_STATUS_ERROR);
		notifyTaskStatus(TASK_STATUS_ERROR, "Start SSH session failed.");
		ssh_shutdown(sshSession, false);		// sedert v1.1
		return -3;
	}

	////////////////////////////////////////////////////////////////
	// Uploaden van de firmware.
	////////////////////////////////////////////////////////////////

	setStatus(TASK_STATUS_BUSY_UPLOAD);

	notifyStatus(false, "Upload firmware to the target.");
	if (ssh_sendFile(sshSession,m_localFileName.c_str(),m_remoteFileName.c_str(),&callbackInfo)<0) {
		setStatus(TASK_STATUS_ERROR);
		notifyTaskStatus(TASK_STATUS_ERROR, "Upload firmware to the target failed.");
		ssh_shutdown(sshSession, true);		// sedert v1.1
		return -4;
	}
	notifyStatus(false, "Upload firmware to the target done.");
	

	setStatus(TASK_STATUS_BUSY_INSTALLING);
	//////////////////////////////////////////////////////////////////
	// Controle of de applicatie aan het draaien is...
	//////////////////////////////////////////////////////////////////
	bool runningApplication = false;
	notifyStatus(false, "Check target if application is running.");	
	if (!appwrapperssh_checkAppRunning(sshSession, m_applicationName.c_str() , &runningApplication, &callbackInfo)) {		
		runningApplication = true;
	}

	//////////////////////////////////////////////////////////////////
	// Afsluiten van de applicatie.
	//////////////////////////////////////////////////////////////////
	if (runningApplication) {
		if (appwrapperssh_shutdownApp(sshSession, m_applicationName.c_str(), &callbackInfo)) {	
			notifyStatus(false, "Shutdown application");				
		}
		else {
			notifyStatus(true, "Shutdown application failed - continue...");	
			// Todo: NIET GELUKT OM DE APPLICATIE AF TE SLUITEN...	<--------------------- moeten we iets doen ????????????
		}
	}

	
	//////////////////////////////////////////////////////////////////
	// Verwijderen van de oude applicatie.
	//////////////////////////////////////////////////////////////////
	notifyStatus(false, "Removing the old version.");	
	if (appwrapperssh_remove(sshSession, m_applicationName.c_str(), &callbackInfo)) {
		notifyStatus(false, "Removing the old version done.");	
	}
	else {
		notifyStatus(true, "Removing the old version failed - continue...");	
	}


	//////////////////////////////////////////////////////////////////
	// Installeren van de IPK.
	//////////////////////////////////////////////////////////////////	
	notifyStatus(false, "Installing the new version.");	
	if (appwrapperssh_install(sshSession,m_remoteFileName.c_str(), &callbackInfo)) {
		notifyStatus(false, "Installing the new version done.");	
	}
	else {
		notifyStatus(true, "Installing the new version failed!");
		notifyTaskStatus(TASK_STATUS_ERROR, "Installing the new version failed!");
		ssh_shutdown(sshSession, true);		// sedert v1.1
		return -4;
	}

	//////////////////////////////////////////////////////////////////
	// Rebooten van de target...
	//////////////////////////////////////////////////////////////////
	notifyStatus(false, "Rebooting the target.");	
	appwrapperssh_reboot(sshSession, &callbackInfo);


	// Done...
	ssh_shutdown(sshSession,true);	// Afsluiten ...

	notifyTaskStatus(TASK_STATUS_DONE, "Updating firmware done.");
	setStatus(TASK_STATUS_DONE);

	return 0;
}

// Worker thread
void AppTaskSSH::ThreadCtor() {	
	this->Base::ThreadCtor();		// !!!
}

// Worker thread
void AppTaskSSH::ThreadDtor() { 
	this->Base::ThreadDtor();		// !!!
}


// Worker thread
void AppTaskSSH::callbackInfo(bool errorMsg, const char* msg) {
	if (NULL !=m_pInstance) { 
		m_pInstance->notifyStatus(errorMsg, msg);  
	}
}

// Worker thread
void AppTaskSSH::setStatus(SshTaskStatus_t status) {
	if (m_mutexStatus.Reserve(-1)) {
		m_status = status;
		m_mutexStatus.Release();
	}
}
/***************************************************************************************************/
};
/***************************************************************************************************/