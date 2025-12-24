#pragma once
#include "sshwrapper.h"
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/
extern bool appwrapperssh_checkAppRunning(STRUCT_SSH_CONTECT_PTR pContextSession, const char* applicationName, bool* fResultRunning,
										 void (*pfCallback)(bool error, const char* msg) = NULL);

extern bool appwrapperssh_shutdownApp(STRUCT_SSH_CONTECT_PTR pContextSession, const char* applicationName, 
										 void (*pfCallback)(bool error, const char* msg) = NULL);

extern bool appwrapperssh_remove(STRUCT_SSH_CONTECT_PTR pContextSession, const char* applicationName, 
								  void (*pfCallback)(bool error, const char* msg) = NULL);

extern bool appwrapperssh_install(STRUCT_SSH_CONTECT_PTR pContextSession, const char* fileName, 
								  void (*pfCallback)(bool error, const char* msg) = NULL);


extern bool appwrapperssh_reboot(STRUCT_SSH_CONTECT_PTR pContextSession,void (*pfCallback)(bool error, const char* msg) = NULL);

/***************************************************************************************************/
}; /*  namespace DUOTECNO { */
/***************************************************************************************************/
