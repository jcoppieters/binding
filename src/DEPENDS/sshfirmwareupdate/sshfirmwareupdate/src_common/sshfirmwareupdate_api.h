#pragma once
#include "sshfirmwareupdate_cmn.h"
/***************************************************************************************************
 * - Author: Mitchell Tom.
 * - Date: 20230109
 ***************************************************************************************************
 * - Version: V1.0.
 * - Description: First version.
 * - Date: 20230110
 ***************************************************************************************************
 * - Version: V1.1
 * - Description: Varia
 * - Date: 20230112
 * - Changes:
 *		- shutdown ssl + session on error: memory leaks.
 *		- socket library: use winsock 2.2.
 ***************************************************************************************************
 * KNOWN-PROBLEMS/TODO:
 * 1) NOTE_SSH_CLOSE. 
 *	  Aanpassingen nodig omdat de CM3/CM3+ anders reageert dan de CM4S en RPI1 distributies.
 *    Bij de CM3 blijft de applicatie soms hangen na het versturen van het reboot commando.
 *	  Het lijkt erop dat de SSH library niet detecteerd dat de connectie verbroken werd.
 *	  Nodige aanpassingen/workarounds:
 *		- Bij het versturen van het shutdown / reboot commando wordt er geen response data gelezen.
 *		- Bij het sluiten van het ssh kanaal wordt er 15 seconden gewacht. Na deze tijd wordt de 
 *		  sessie gewoon afgesloten.
 *	
 ***************************************************************************************************
 * - Version Info: VERSION_MAJOR en VERSION_MINOR.
 **************************************************************************************************/

/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
#if(0)
// Wordt uitgevoerd in dezelfde thread --> Gebruik als console.
// NIET MEER GEBRUIKEN.
extern int sshfirmwareUpdate_update(const char* hostName, 
								    const char* userName,
									const char* password,
									const char* applicationName,
									const char* localFileName,
									void (*PF_CALLBACK)(bool errorMsg, const char* msg) = NULL);
#endif
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
// De taken Worden uitgevoerd in een andere thread.
struct ISSHFirmwareUpdate 
{
	virtual ~ISSHFirmwareUpdate( ) = 0;

	// Starten van de updater.
	// Functie is Threadsafe 
	virtual int startTaskUpdate(
		const char* hostName, 
		const char* userName,
		const char* password,
		const char* applicationName,
		const char* localFileName) = 0;

	// Periodiek aanroepen door de client...
	// Functie is Threadsafe 
	virtual SshTaskStatus_t poll(int milliseconds) = 0;

	// Periodiek aanroepen door de client.
	// Bevat status info, error en toestand berichten.
	// Functie is Threadsafe 
	virtual bool getMessage(SshMessage* pMessage) = 0;

	// Opvragen van de versie van de biblitheek.
	// Functie is Threadsafe 
	virtual void getVersion(int* major, int* minor) = 0;
};
/***************************************************************************************************/
extern ISSHFirmwareUpdate* sshfirmwareupdate_create();
extern void sshfirmwareupdate_destroy(ISSHFirmwareUpdate** pp);
/***************************************************************************************************/
}; /* end namespace */
/***************************************************************************************************/
