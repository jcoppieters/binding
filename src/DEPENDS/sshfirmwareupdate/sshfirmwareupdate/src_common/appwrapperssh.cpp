#include "stdafx.h"
#include "appwrapperssh.h"
#include <string.h>
/***************************************************************************************************/
#define DEBUG_LOCAL_ON		0
/***************************************************************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define DUMP_RX_EC(x)			dumpRxError(x)
	#define DUMP_RX_INFO(x)			dumpRxInfo(x)
#else
	#define DUMP_RX_EC(x)			
	#define DUMP_RX_INFO(x)	
#endif 
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/
static void dumpRxInfo(const char* msg) {
	fprintf(stderr, "RX: %s\r\n",msg);
}

static void dumpRxError(const char* msg) {
	fprintf(stderr, "RX-ERROR: %s\r\n", msg);
}

bool appwrapperssh_checkAppRunning(STRUCT_SSH_CONTECT_PTR pContextSession, const char* applicationName, bool* fResultRunning,
										 void (*pfCallback)(bool error, const char* msg)) 
{
	bool errorMsg = false;
	char textBuffer[1 * 1000];
	char commandLine[128];
	bool retValue = false;

	memset(textBuffer,0,sizeof(textBuffer));
	memset(commandLine,0,sizeof(commandLine));

	sprintf(commandLine,"pidof %s",applicationName);		// example: "pidof qttouchscreenapp"
				
	const int res = ssh_executeCommand(pContextSession,commandLine, true, textBuffer,sizeof(textBuffer),&errorMsg,pfCallback);
	if (res >= 0) {
		if (errorMsg) {
			DUMP_RX_EC(textBuffer);
			*fResultRunning = false;
		}
		else {	
			DUMP_RX_INFO(textBuffer);		
			// TODO <------------------------- PARSEN MOET EEN INTEGER ZIJN (=process id...)
			*fResultRunning = true;
		}
		retValue = true;
	}
	else { 
		// todo: Fouten....	<----------------------------- ???????????
		retValue = false;
	}
	return retValue;
}

bool appwrapperssh_shutdownApp(STRUCT_SSH_CONTECT_PTR pContextSession, const char* applicationName, 
									  void (*pfCallback)(bool error, const char* msg)) 
{
	bool errorMsg = false;
	char textBuffer[1 * 1000];
	char commandLine[128];
	bool retValue = false;

	memset(textBuffer,0,sizeof(textBuffer));
	memset(commandLine,0,sizeof(commandLine));

	sprintf(commandLine,"/opt/%s/bin/my-shutdown.sh",applicationName);		// example: "/opt/qttouchscreenapp/bin/my-shutdown.sh";

	const int res = ssh_executeCommand(pContextSession,commandLine,true, textBuffer,sizeof(textBuffer),&errorMsg,pfCallback);
	if (res >= 0) {
		if (errorMsg) {
			// BIJ HET AFSLUITEN IS ER ALTIJD EEN FOUTMELDING ?????????????????????????		<--- TODO ????
			DUMP_RX_EC(textBuffer);
		}
		else {			
			DUMP_RX_INFO(textBuffer);	
		}
		retValue = true;
	}
	else { 
		// todo: Fouten....	<----------------------------- ???????????
		retValue = false;
	}
	return retValue;
}

bool appwrapperssh_remove(STRUCT_SSH_CONTECT_PTR pContextSession, const char* applicationName, 
						  void (*pfCallback)(bool error, const char* msg))
{
	bool errorMsg = false;
	char textBuffer[1 * 1000];
	char commandLine[256];
	bool retValue = false;

	memset(textBuffer,0,sizeof(textBuffer));
	memset(commandLine,0,sizeof(commandLine));

	// Voorbeeld "/usr/bin/opkg remove --force-remove qttouchscreenapp";
	sprintf(commandLine,"/usr/bin/opkg remove --force-remove %s",applicationName);
	const int res = ssh_executeCommand(pContextSession,commandLine, true, textBuffer,sizeof(textBuffer),&errorMsg,pfCallback);					
	if (res >= 0) {
		if (errorMsg) {
			DUMP_RX_EC(textBuffer);
			// ?????????????????????????????????????
			// TODO ????????????????????????????????
			// ?????????????????????????????????????

			retValue = false;
		}
		else {			
			DUMP_RX_INFO(textBuffer);
			retValue = true;
		}		
	}
	else { 
		retValue = false;
	}
	return retValue;
}


bool appwrapperssh_install(STRUCT_SSH_CONTECT_PTR pContextSession, const char* fileName, 
						   void (*pfCallback)(bool error, const char* msg)) 
{
	bool errorMsg = false;
	char textBuffer[1 * 1000];
	char commandLine[256];
	bool retValue = false;

	memset(textBuffer,0,sizeof(textBuffer));
	memset(commandLine,0,sizeof(commandLine));

	// voorbeeld: "/usr/bin/opkg install --force-overwrite /tmp/cm4s_051508_qttouchscreenapp_v66.10.00.ipk";
	sprintf(commandLine,"/usr/bin/opkg install --force-overwrite %s",fileName);
	
	const int res = ssh_executeCommand(pContextSession,commandLine,true, textBuffer,sizeof(textBuffer),&errorMsg,pfCallback);					
	if (res >= 0) {
		if (errorMsg) {
			DUMP_RX_EC(textBuffer);

			// ?????????????????????????????????????
			// TODO ????????????????????????????????
			// ?????????????????????????????????????

			retValue = false;
		}
		else {			
			DUMP_RX_INFO(textBuffer);
			retValue = true;
		}				
	}
	else { 
		// todo: Fouten....
		retValue = false;
	}
	return retValue;
}


bool appwrapperssh_reboot(STRUCT_SSH_CONTECT_PTR pContextSession,void (*pfCallback)(bool error, const char* msg)) {

	bool errorMsg = false;
	char textBuffer[1 * 1000];
	bool retValue = false;

	//const char* const strCommandLine = "/sbin/reboot";			// Oppassen: MOET FULL PATH NAME ZIJN !!!
	//const char* const strCommandLine = "/bin/sleep 1 && /sbin/reboot";			// Oppassen: MOET FULL PATH NAME ZIJN !!!
	//const char* const strCommandLine = "/sbin/reboot && /bin/sleep 5";				// Oppassen: MOET FULL PATH NAME ZIJN !!!

	const char* const strCommandLine = "/sbin/shutdown -r now";			// Oppassen: MOET FULL PATH NAME ZIJN !!!
	const int res = ssh_executeCommand(pContextSession,strCommandLine,false, textBuffer,sizeof(textBuffer),&errorMsg,pfCallback);
	if (res >= 0) {
		if (errorMsg) {
			DUMP_RX_EC(textBuffer);
		}
		else {			
			DUMP_RX_INFO(textBuffer);	
		}		
		retValue = true;
	}
	else { 
		// todo: Fouten....
		retValue = false;
	}	
	return retValue;
}
/***************************************************************************************************/
}; /* namespace DUOTECNO */
/***************************************************************************************************/