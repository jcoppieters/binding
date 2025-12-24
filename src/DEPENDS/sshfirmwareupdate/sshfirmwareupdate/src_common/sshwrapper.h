#pragma once
/***************************************************************************************************/
// documentatie:
// ref: https://www.libssh2.org/docs.html
/***************************************************************************************************/
// namespace DUOTECNO {
/***************************************************************************************************/
typedef struct STRUCT_SSH_CONTECT STRUCT_SSH_CONTECT;
typedef struct STRUCT_SSH_CONTECT* STRUCT_SSH_CONTECT_PTR;
/***************************************************************************************************/
enum {
	ERROR_NONE = 0,
	ERROR_EC_GENERAL = -1,
	ERROR_EC_AUTHENTICATION = -2,
	ERROR_FILE_OPEN = -3,
	ERROR_SFTP_INIT = -4,
	ERROR_SFTP_ERROR = -5,
	ERROR_BUFFER = -6,
	ERROR_READ = -7,
	ERROR_READ_NODATA = -8
};


// init & shutdown...
extern STRUCT_SSH_CONTECT_PTR ssh_init(void (*pfCallback)(bool error, const char* msg) = NULL);
extern void ssh_shutdown(STRUCT_SSH_CONTECT_PTR pContextSession, bool closeSSHSession); 

// fuincties...
extern int ssh_connectSocket(STRUCT_SSH_CONTECT_PTR pContextSession, const char* hostname, int port, void (*pfCallback)(bool error, const char* msg) = NULL);
extern int ssh_connectSSHServer(STRUCT_SSH_CONTECT_PTR pContextSession, const char* username, const char* password, void (*pfCallback)(bool error, const char* msg) = NULL);
extern int ssh_sendFile(STRUCT_SSH_CONTECT* pContextSession,const char* fileName,const char* destPath,void (*pfCallback)(bool error, const char* msg) = NULL);
extern int ssh_executeCommand(STRUCT_SSH_CONTECT* pContextSession, const char* strCommandLine, bool readResponse,
					char* pbBuffer, int maxLengthBuffer, bool* errorMsgResponse,
					   void (*pfCallback)(bool error, const char* msg) = NULL);

/***************************************************************************************************/
// }; /* namespace DUOTECNO { */
/***************************************************************************************************/