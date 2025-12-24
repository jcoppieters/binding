#include "stdafx.h"
#include "sshfirmwareupdate_impl.h"
#include "appwrapperssh.h"
#include "apptaskssh.h"
#include <string.h>
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/
static const int VERSION_MAJOR = 1;
static const int VERSION_MINOR = 1;

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
#if(0)
int sshfirmwareUpdate_update(const char* hostname, 
						     const char* username,
							 const char* password,
							 const char* applicationName,
							 const char* loclfile,
							 void (*pf_callback)(bool errorMsg, const char* msg))
{
	
	char sftppath[1024];
    int exitcode = 0;

	if (NULL == hostname || NULL == username || NULL == password || NULL == applicationName || NULL == loclfile) {
		// dumpErrorArguments();
		return -1;
	}
	memset(sftppath,0,sizeof(sftppath));
	sprintf(sftppath,"/tmp/%s.ipk",applicationName);
	
	////////////////////////////////////////////////////////////////
	// Init.
	////////////////////////////////////////////////////////////////
	fprintf(stderr, "Initialising ssh.\n");
	STRUCT_SSH_CONTECT_PTR sshSession = ssh_init(pf_callback);
	if (NULL == sshSession) {
		return -1;
	}

	////////////////////////////////////////////////////////////////
	// Connectie maken.
	////////////////////////////////////////////////////////////////
	fprintf(stderr, "Connect to remote ssh server.\n");
	if (ssh_connectSocket(sshSession, hostname, 22, pf_callback)<0) {
		return -1;
	}
	fprintf(stderr, "login to remote ssh server.\n");
	if (ssh_connectSSHServer(sshSession,username,password, pf_callback)<0) {
		return -1;
	}

	////////////////////////////////////////////////////////////////
	// Uploaden van de firmware.
	////////////////////////////////////////////////////////////////
	fprintf(stderr, "Uploading firmware to target. please wait ...\n");
	if (ssh_sendFile(sshSession,loclfile,sftppath,pf_callback)<0) {
		return -1;
	}
	fprintf(stderr, "Uploading firmware done.\n");

	//////////////////////////////////////////////////////////////////
	// Controle of de applicatie aan het draaien is...
	//////////////////////////////////////////////////////////////////
	bool runningApplication = false;
	fprintf(stderr, "Check if the application is running\n");
	if (!appwrapperssh_checkAppRunning(sshSession, applicationName , &runningApplication, pf_callback)) {		
		runningApplication = true;
	}

	//////////////////////////////////////////////////////////////////
	// Afsluiten van de applicatie.
	//////////////////////////////////////////////////////////////////
	if (runningApplication) {
		if (appwrapperssh_shutdownApp(sshSession, applicationName, pf_callback)) {	
			fprintf(stderr, "Application is running - shutdown application done\n");
		}
		else {
			fprintf(stderr, "Application is running - shutdown application failed - continue\n");
			// Todo: NIET GELUKT OM DE APPLICATIE AF TE SLUITEN...	<--------------------- moeten we iets doen ????????????
		}
	}

	//////////////////////////////////////////////////////////////////
	// Verwijderen van de oude applicatie.
	//////////////////////////////////////////////////////////////////
	if (appwrapperssh_remove(sshSession, applicationName, pf_callback)) {
		fprintf(stderr, "Removing old application.\n");
	}
	else {
		fprintf(stderr, "Removing old application failed - possible already removed - continue.\n");
	}

	//////////////////////////////////////////////////////////////////
	// Installeren van de IPK.
	//////////////////////////////////////////////////////////////////
	if (appwrapperssh_install(sshSession,sftppath, pf_callback)) {
		fprintf(stderr, "Updating firmware done.\n");
	}
	else {
		fprintf(stderr, "Updating firmware failed - TRY AGAIN\n");
	}
	
	//////////////////////////////////////////////////////////////////
	// Rebooten van de target...
	//////////////////////////////////////////////////////////////////
	fprintf(stderr, "Rebooting target...");
	appwrapperssh_reboot(sshSession, pf_callback);

// shutdown:
	ssh_shutdown(sshSession,true);

    return exitcode;
}
#endif

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
CSSHFirmwareUpdate_Impl::CSSHFirmwareUpdate_Impl( ): m_pTask(NULL) { }

CSSHFirmwareUpdate_Impl::~CSSHFirmwareUpdate_Impl( ) {
	if (NULL != m_pTask) {	
		delete m_pTask;
		m_pTask = NULL;
	}
}

// NOTE:
// Het bestand wordt op de target onder de '/tmp' directory geplaatst.
// Na een reboot van de target wordt het bestand automatisch verwijderd.
int CSSHFirmwareUpdate_Impl::startTaskUpdate(
	const char* hostName, 
	const char* userName,
	const char* password,
	const char* applicationName,
	const char* localFileName) 
{
	if (NULL == hostName || NULL == userName || NULL == password || NULL == applicationName || NULL == localFileName) {	
		return -1;
	}	
	if (NULL != m_pTask) {
		return -1;
	}	
	if (NULL == m_pTask) {
		char sftppath[1024];   
		memset(sftppath,0,sizeof(sftppath));
		sprintf(sftppath,"/tmp/%s.ipk",applicationName); // zie NOTE
		m_pTask = new AppTaskSSH(hostName, userName, password, applicationName, localFileName, sftppath); 
	}	
	return m_pTask->Start();
}

SshTaskStatus_t CSSHFirmwareUpdate_Impl::poll(int milliseconds) {
	if (NULL == m_pTask) {
		return TASK_STATUS_IDLE;
	}
	const SshTaskStatus_t status = m_pTask->getStatus();
	if (TASK_STATUS_DONE == status || TASK_STATUS_ERROR == status) {
		delete m_pTask;
		m_pTask = NULL;
	}
	return status;
}

bool CSSHFirmwareUpdate_Impl::getMessage(SshMessage* pMessage) {
	if (NULL == m_pTask) {
		return false;
	}
	const SshMessage* temp = m_pTask->pop( );
	if (NULL == temp) {
		return false;
	}
	*pMessage = *temp;	// Copy by value...
	delete temp;		// delete memory !!
	return true;
}

void CSSHFirmwareUpdate_Impl::getVersion(int* major, int* minor) {
	if (NULL == major || NULL == minor) {
		return;
	}
	*major = VERSION_MAJOR;
	*minor = VERSION_MINOR;
}
/***************************************************************************************************/
};	/* namespace */
/***************************************************************************************************/
