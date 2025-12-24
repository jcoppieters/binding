#include "stdafx.h"
#include "sshwrapper.h"

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include <sys/types.h>
//#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
/***************************************************************************************************/
#define DEGUG_LOCAL_ON			0
/***************************************************************************************************/
// namespace DUOTECNO {
/***************************************************************************************************/
struct STRUCT_SSH_CONTECT {
	LIBSSH2_SESSION* m_session;
	int m_fpSocket;
};

//#if(0)
static bool socketClosed(STRUCT_SSH_CONTECT_PTR session) {
	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(session->m_fpSocket, &rfd);
	timeval tv = { 0 };
	select(session->m_fpSocket + 1, &rfd, 0, 0, &tv);
	if (!FD_ISSET(session->m_fpSocket, &rfd))
		return false;
	u_long n = 0;
	int iResult = ioctlsocket(session->m_fpSocket, FIONREAD, &n);
	if (SOCKET_ERROR == iResult) {
		return true;
	}
	if (n == 0) {
		return true;
	}
	return false;
}
//#endif

#if(0)
static bool socketClosed(STRUCT_SSH_CONTECT_PTR session) {
	u_long iMode = 0;
	int iResult = ioctlsocket(session->m_fpSocket, FIONREAD, &iMode);
	if (SOCKET_ERROR == iResult) {
		return true;
	}
	return false;
}
#endif

// Private...
static int ssh_waitsocket(STRUCT_SSH_CONTECT_PTR session, int seconds = 10)			
{
    struct timeval timeout;
    int rc;
    fd_set fd;	
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = seconds;	// 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);
    FD_SET(session->m_fpSocket, &fd);

    /* now make sure we wait in the correct direction */
	dir = libssh2_session_block_directions(session->m_session);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

	rc = select(session->m_fpSocket + 1, readfd, writefd, NULL, &timeout);
    return rc;
}

static int ssh_readChannelData(STRUCT_SSH_CONTECT* pContextSession, LIBSSH2_CHANNEL* channel, int channelID,
							   char* pbBuffer, int maxLengthBuffer, void (*pfCallback)(bool error, const char* msg) = NULL) 
{
	int totalLength = 0;
	int rc = 0;
	int retValue = ERROR_READ_NODATA;
	char msg[128];
	// ASSERT(channelID == 0 || channelID == SSH_EXTENDED_DATA_STDERR);
	
	for(;;) {	
		do {
			char buffer[0x4000];	
			memset(buffer,0,sizeof(buffer));
			rc = libssh2_channel_read_ex(channel, channelID,  buffer, sizeof(buffer) );
			if(rc > 0) {
				int i;			
				for(i = 0; i < rc; ++i) {
					if (totalLength < maxLengthBuffer) {
						pbBuffer[totalLength++] = buffer[i];
						retValue = totalLength;
					}
					else {
						retValue = ERROR_BUFFER;
						break;
					}
				}
			}
			else {
				if((rc != LIBSSH2_ERROR_EAGAIN) && (rc < 0)) {	
					if (NULL != pfCallback) {
						sprintf(msg, "libssh2_channel_read returned %d\n", rc);		
						pfCallback(true,msg);
					}
				}
			}
		} while(rc > 0);

		/* this is due to blocking that would occur otherwise so we loop on
		   this condition */ 
		if(rc == LIBSSH2_ERROR_EAGAIN) {
			ssh_waitsocket(pContextSession);
		}
		else if (rc == 0) {			
			break;
		}	
		else {
			if (NULL != pfCallback) {
				sprintf(msg, "libssh2_channel_read returned %d\n", rc);		
				pfCallback(true,msg);
			}
			retValue = ERROR_READ;
			break;
		}
	}		
	return retValue;
}


// public...

STRUCT_SSH_CONTECT_PTR ssh_init(void (*pfCallback)(bool error, const char* msg)) 
{
#ifdef WIN32
    WSADATA wsadata;
    int err = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if(err != 0) {
		if (NULL != pfCallback) {
			char msg[128];
			sprintf(msg, "WSAStartup failed with error: %d", err);
			pfCallback(true, msg);
		}
        return NULL;
    }
#endif

	STRUCT_SSH_CONTECT_PTR instance = new STRUCT_SSH_CONTECT();
	if (NULL == instance) {
		return NULL;
	}

	instance->m_fpSocket  = -1;
	instance->m_session = NULL;

	err = libssh2_init(0);
    if(err != 0) {
		if (NULL != pfCallback) {
			char msg[128];
			sprintf(msg, "libssh2 initialization failed (%d)", err);
			pfCallback(true, msg);
		}		
		delete instance;
		return NULL;
    }

#if(DEGUG_LOCAL_ON == 1)
    libssh2_trace(instance->m_session, LIBSSH2_TRACE_SOCKET); //  + LIBSSH2_TRACE_CONN + LIBSSH2_TRACE_TRANS + LIBSSH2_TRACE_ERROR);
#endif 

	return instance;
}

void ssh_shutdown(STRUCT_SSH_CONTECT_PTR pContextSession, bool closeSSHSession)
{
	if (NULL != pContextSession) {
		// Cleanup SSH
		if (NULL != pContextSession->m_session) {					
			if (closeSSHSession) {
				//	libssh2_session_set_blocking(pContextSession->m_session, 1);
				libssh2_session_disconnect(pContextSession->m_session,"Normal Shutdown, Thank you for playing");
			}
			libssh2_session_free(pContextSession->m_session);
			pContextSession->m_session = NULL;
		}

		// Cleanup Socket.
		if (pContextSession->m_fpSocket != -1) {		
			closesocket(pContextSession->m_fpSocket);			
			pContextSession->m_fpSocket = -1;
		}					

		// Cleanup lib ssh library
		libssh2_exit();

		// delete memory.
		delete pContextSession;
		pContextSession = NULL;
	}
}

void ssh_cleanupOnError(STRUCT_SSH_CONTECT_PTR pContextSession, bool connected) {
	if (NULL != pContextSession) {
		
		libssh2_session_disconnect(pContextSession->m_session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(pContextSession->m_session);

		if (pContextSession->m_fpSocket != -1) {
			closesocket(pContextSession->m_fpSocket);
		}

		libssh2_exit();

		delete pContextSession;
		pContextSession = NULL;
	}
}

int ssh_connectSocket(STRUCT_SSH_CONTECT_PTR pContextSession, const char* hostname, int port, void (*pfCallback)(bool error, const char* msg)) {
			
	pContextSession->m_fpSocket = socket(AF_INET, SOCK_STREAM, 0);	
	if (NULL ==  pContextSession->m_fpSocket) {
		if (NULL != pfCallback) {
			pfCallback(true,"failed to create the socket!");
		}	
		return ERROR_EC_GENERAL;
	}

	unsigned long hostaddr;
    struct sockaddr_in sin;
	hostaddr = inet_addr(hostname);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = hostaddr;

    if(connect(pContextSession->m_fpSocket, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != 0) {
		if (NULL != pfCallback) {
			pfCallback(true,"failed to connect");
		}	
        return ERROR_EC_GENERAL;
    }


	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	int rc = setsockopt(pContextSession->m_fpSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&bOptVal, bOptLen);
	if (rc == SOCKET_ERROR) {
		if (NULL != pfCallback) {
			pfCallback(true, "failed to set socket options.");
		}
	}
	return ERROR_NONE;
}

int ssh_connectSSHServer(STRUCT_SSH_CONTECT_PTR pContextSession, const char* username, const char* password, void (*pfCallback)(bool error, const char* msg)) {

	int rc = -1;
	char msg[128];
	
    /* Create a session instance */
    pContextSession->m_session = libssh2_session_init();
    if(!pContextSession->m_session)
        return ERROR_EC_GENERAL;

    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(pContextSession->m_session, 0);


    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while((rc = libssh2_session_handshake(pContextSession->m_session, pContextSession->m_fpSocket)) == LIBSSH2_ERROR_EAGAIN);
    if(rc) {
		if (NULL != pfCallback) {
			sprintf(msg, "Failure establishing SSH session: %d\n", rc);
			pfCallback(true, msg);
		}
        return ERROR_EC_GENERAL;
    }

    if(strlen(password) != 0) {
        /* We could authenticate via password */
        while((rc = libssh2_userauth_password(pContextSession->m_session, username, password)) == LIBSSH2_ERROR_EAGAIN);
        if(rc) {
			if (NULL != pfCallback) {
				sprintf(msg, "Authentication by password failed.\n");
				pfCallback(true, msg);
			}
			return ERROR_EC_AUTHENTICATION;
        }
    }
	return 0;
}

// Returnwaarde:
// >=0 : aantal bytes verstuurd.
// <0: foutmelding.
int ssh_sendFile(STRUCT_SSH_CONTECT* pContextSession,const char* fileName,const char* destPath,void (*pfCallback)(bool error, const char* msg)) 
{
	FILE *fp = NULL;
	LIBSSH2_SFTP *sftp_session = NULL;	
	LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
	int rc;
	char mem[1024 * 100];
    size_t nread;
    char *ptr;    
    long total = 0;
	char msg[256];

	// Openen van het bestand...
	fp = fopen(fileName, "rb");
    if(!fp) {
		if (NULL != pfCallback) {			
			sprintf(msg,"Can't open local file '%s'",fileName);
			pfCallback(true, msg);
		}  
        return ERROR_FILE_OPEN;
    }

	// Bepalen van de groote van het bestand...
	fseek(fp, 0L, SEEK_END);
	const long fileSize = ftell(fp);
	if (fileSize <= 0) {
		if (NULL != pfCallback) {			
			sprintf(msg,"Empty local file '%s' cannot continue",fileName);
			pfCallback(true, msg);
		} 
		fclose(fp);
		return ERROR_FILE_OPEN;
	}	
	// terug in het begin zetten...
	fseek(fp, 0L, SEEK_SET);

	// Openen van een ftp sessie...
	do {
        sftp_session = libssh2_sftp_init(pContextSession->m_session);
		if(!sftp_session && (libssh2_session_last_errno(pContextSession->m_session) != LIBSSH2_ERROR_EAGAIN)) {
			if (NULL != pfCallback) {			
				sprintf(msg,"Unable to init SFTP session");
				pfCallback(true, msg);
			}   
			fclose(fp);
            return ERROR_SFTP_INIT;
        }
    } while(!sftp_session);

	// Aanmaken van hte bestand op de target. 
	// Bestand properties als executable zetten !!!
    do {
        sftp_handle = libssh2_sftp_open(sftp_session, destPath,
                              LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|
                              LIBSSH2_FXF_TRUNC,
                              LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|LIBSSH2_SFTP_S_IXUSR |
                              LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH|LIBSSH2_SFTP_S_IXGRP);

        if(!sftp_handle && (libssh2_session_last_errno(pContextSession->m_session) != LIBSSH2_ERROR_EAGAIN)) {
			if (NULL != pfCallback) {			
				sprintf(msg,"Unable to open file with SFTP");
				pfCallback(true, msg);
			}           
			libssh2_sftp_shutdown(sftp_session);
            fclose(fp);
			return ERROR_SFTP_ERROR;
        }
    } while(!sftp_handle);

	
	// Data overzetten naar het remote bestand...
	bool success = true;
	do {
        nread = fread(mem, 1, sizeof(mem), fp);
        if(nread <= 0) {
            /* end of file */ 
            break;
        }
        ptr = mem;
        total += nread;
 
        do {
            /* write data in a loop until we block */ 
            while((rc = libssh2_sftp_write(sftp_handle, ptr, nread)) == LIBSSH2_ERROR_EAGAIN) {
				ssh_waitsocket(pContextSession);
            }
			if(rc < 0) {
				success = false;
                break;
			}
            ptr += rc;
            nread -= rc;
        } while(nread);

		if (NULL != pfCallback) {			
			static long prevProgress = 101;
			long progress = total / (fileSize/100);		
			if (prevProgress != progress) {
				sprintf(msg,"Uploading file. Progress info %d/100",(int)(progress));
				pfCallback(false, msg);
				prevProgress = progress;
			}
		}
    } while(rc > 0);

	// Cleanup...
	fclose(fp);
    libssh2_sftp_close(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);

	if (success) {
		return total;
	}
	return -1;
}

// NOTE_SSH_CLOSE:
// *) boolean: readResponse: bepaalt of we een antwoord moeten lezen.
//			   Bij de CM3: Bij het versturen van een reboot dan blijft de ssh hier hangen wanneer we data proberen te lezen.
// *) libssh2_channel_close(): 
//			   Bij de CM3 blijft hier anders oneindig hangen...

int ssh_executeCommand(STRUCT_SSH_CONTECT* pContextSession, const char* strCommandLine, bool readResponse, char* pbBuffer, int maxLengthBuffer, bool* errorMsgResponse,
					   void (*pfCallback)(bool error, const char* msg)) 
{
	LIBSSH2_CHANNEL *channel = NULL;
	int rc = -1;
	int totalLength = 0;
	int retValue = ERROR_NONE;
	char *exitsignal = (char *)"none";
	char msg[256];

	/* Exec non-blocking on the remove host */ 
	while((channel = libssh2_channel_open_session(pContextSession->m_session)) == NULL && 
		   libssh2_session_last_error(pContextSession->m_session, NULL, NULL, 0) == LIBSSH2_ERROR_EAGAIN) 
	{
		ssh_waitsocket(pContextSession);
    }
    if(channel == NULL) {
		if (NULL != pfCallback) {
			sprintf(msg, "ssh session error");
			pfCallback(true,msg);
		}
		return ERROR_EC_GENERAL;
    }

	// Uitvoeren van de commandline.
	while((rc = libssh2_channel_exec(channel, strCommandLine)) == LIBSSH2_ERROR_EAGAIN) {
        ssh_waitsocket(pContextSession);
    }
    if(rc != 0) {
       	if (NULL != pfCallback) {
			sprintf(msg, "ssh session error: %d",rc);
			pfCallback(true,msg);
		}
		libssh2_channel_free(channel);
        return ERROR_EC_GENERAL;
    }

	if (readResponse) {
		// Oppassen:
		// Is een verschillende channel om de gewone ssh output & de fout ssh output uit te lezen...
		// eerst de fouten uitlezen, indien geen fouten dan de gewone...
		totalLength = ssh_readChannelData(pContextSession, channel, SSH_EXTENDED_DATA_STDERR, pbBuffer, maxLengthBuffer,pfCallback);
		if (totalLength > 0) {
			*errorMsgResponse = true;
		}
		else {
			totalLength = ssh_readChannelData(pContextSession, channel, 0, pbBuffer, maxLengthBuffer,pfCallback); 
			if (totalLength > 0) {
				*errorMsgResponse = false;
			}
		}
	}

	int waitValue = 0;		// NOTE_SSH_CLOSE
	int exitcode = 127;
	while((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN) {
		ssh_waitsocket(pContextSession,1);		
		if (waitValue++ >= 15) {				// NOTE_SSH_CLOSE: Na 15 seconden een break - Blijft hier soms hangen.
			break;
		}
		if (socketClosed(pContextSession)) {
			break;
		}
	}
	if(rc == 0) {
		exitcode = libssh2_channel_get_exit_status(channel);
		libssh2_channel_get_exit_signal(channel, &exitsignal,NULL, NULL, NULL, NULL, NULL);
	}

#if (DEBUG_LOCAL_ON == 1)
	if(exitsignal) {
        fprintf(stderr, "\nGot signal: %s\n", exitsignal);
	}
	else {
        fprintf(stderr, "\nEXIT: %d bytecount: %d\n", exitcode, totalLength);
	}
#endif 

	libssh2_channel_free(channel);
	return totalLength;
}

/***************************************************************************************************/
// }; /* namespace DUOTECNO { */
/***************************************************************************************************/