// firmwareUpdateConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../src_common/sshfirmwareupdate_api.h"	
#include <string.h>
/***************************************************************************************************/
using namespace DUOTECNO;
/***************************************************************************************************/
#define USE_THREADED_VERSION			1
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void setMsg(bool errorMsg, const char* msg) {
	if (NULL == msg) {
		return;
	}
	char* pNewMsg = new char[strlen(msg) + 32];
	if (errorMsg) {
		sprintf(pNewMsg,"ERROR: %s\r\n",msg);
		fprintf(stderr, pNewMsg);
	}
	else {
		sprintf(pNewMsg,"INFO: %s\r\n",msg);
		fprintf(stderr, pNewMsg);
	}
	delete [] pNewMsg;
}

static void dumpErrorArguments( ) {
	fprintf(stderr,"Error - Commandline arguments needed:\n"
				   "[ip-address][userName][password][applicationName][localfilename]\r\n");
}


#if(USE_THREADED_VERSION == 1)

static void checkPendingMessages(ISSHFirmwareUpdate* m_pWorker) {
	SshMessage msg;
	while (m_pWorker->getMessage(&msg)) {
		if (SshMessage::TYPE_MSG_ERROR == msg.getType()) {
			setMsg(true,msg.getMsg());
		}
		else if (SshMessage::TYPE_MSG_INFO == msg.getType()) {
			setMsg(false,msg.getMsg());
		}
	}
}

static bool checkStatusBusy(ISSHFirmwareUpdate* m_pWorker) {

	SshTaskStatus_t status =  m_pWorker->poll(100);
	if ((status == TASK_STATUS_ERROR) || (TASK_STATUS_DONE == status)) {
		return false;			
	}
	
	if (status == TASK_STATUS_BUSY_CONNECT) {
		static int m_counter = 0;
		if ((++m_counter%10)==0) {
			setMsg(false, "Connect to the host busy, please wait...");
		}
	}
	return true;
}

static int execute(const char* hostname, 
				   const char* username,
				   const char* password,
				   const char* applicationName,
				   const char* loclfile)				 
{
	ISSHFirmwareUpdate* m_pWorker = sshfirmwareupdate_create(); 
	if (NULL != m_pWorker) {
		int retValue = 0;
		retValue = m_pWorker->startTaskUpdate(hostname,username,password,applicationName,loclfile);
		if (retValue >= 0) {
			bool busy = true;
			while(busy) {
				Sleep(100);			
				checkPendingMessages(m_pWorker);
				busy = checkStatusBusy(m_pWorker);				
			}
		}
		sshfirmwareupdate_destroy(&m_pWorker);
	}
	return 0;
}


#else

static void callbackInfo(bool errorMsg, const char* msg) {
	setMsg(errorMsg,msg);
}

static int execute(const char* hostname, 
				   const char* username,
				   const char* password,
				   const char* applicationName,
				   const char* loclfile) 
{
	return sshfirmwareUpdate_update(hostname,username,password,applicationName,loclfile,&callbackInfo);
}
#endif



/***************************************************************************************************/
// Parameters:
// 0 = hostname.				(ip-address)
// 1 = username.
// 2 = password.
// 3 = applicationName			
// 4 = local filename			
//
// Om te debuggen: Commandline arguments: 192.168.0.153 root du0t3cn4% qttouchscreenapp c:/temp/cm4s_051508_qttouchscreenapp_v66.10.00.ipk
//
int main(int argc, char *argv[])
{
    const char *hostname = NULL;
    const char *username = NULL;
    const char *password = NULL;
	const char *loclfile = NULL;   
	const char *applicationName = NULL;
    int exitcode = 0;


	////////////////////////////////////////////////////////////////
	// Commanline arguments needed.
	////////////////////////////////////////////////////////////////
	if (argc < 6) {
		dumpErrorArguments();
		return -1;
	}       
    hostname = argv[1];		 /* must be ip address only */
    username = argv[2];
    password = argv[3];	
	applicationName = argv[4];		
	loclfile = argv[5];
	
	if (NULL == hostname || NULL == username || NULL == password || NULL == applicationName || NULL == loclfile) {
		dumpErrorArguments();
		return -1;
	}

	// Execute...
	return execute(hostname,username,password,applicationName,loclfile); // ,&callbackInfo);
}

