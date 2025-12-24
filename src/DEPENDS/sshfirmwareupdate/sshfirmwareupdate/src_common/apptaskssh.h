#pragma once
/***************************************************************************************************/
#include "win32/dtthreadwin32.h"
#include "sshfirmwareupdate_cmn.h"
#include <string>								// std::string
#include <queue>								// std::queue
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/
struct ISSHFirmwareUpdateHost;
/***************************************************************************************************/
class AppTaskSSH : public DUOTECNO::CThreadWin
{
public:
	AppTaskSSH(const char* hostName, 
			   const char* userName,
			   const char* password,
			   const char* applicationName,
			   const char* localFileName,
			   const char* remoteFileName); // ,
			   // ISSHFirmwareUpdateHost* pHost);
	virtual ~AppTaskSSH();

	virtual DWORD Start( void* arg = NULL);	// Main thread
	SshTaskStatus_t getStatus();
	SshMessage* pop( );				// Client moet de return waarde dealloceren !!

protected:	
	virtual DWORD Run(LPVOID arg);		// Worker thread
	virtual void ThreadCtor();			// Worker thread
	virtual void ThreadDtor();			// Worker thread
	void setStatus(SshTaskStatus_t status);	// Worker thread
	void notifyStatus(bool flagErrorMsg,const char* msg); // Worker thread
	void notifyTaskStatus(SshTaskStatus_t status, const char* msg); // Worker thread
	void push(const SshMessage& msg);
	static void callbackInfo(bool errorMsg, const char* msg); // Worker thread

private:
	static AppTaskSSH* m_pInstance;			// momenteel een singleton --> Callback...
	std::string m_hostName; 
	std::string m_userName;
	std::string m_password;
	std::string m_applicationName;
	std::string m_localFileName;
	std::string m_remoteFileName;
	volatile SshTaskStatus_t m_status;
	DUOTECNO::CThreadMutexWin m_mutexStatus;
	DUOTECNO::CThreadMutexWin m_mutexQueueMsg;
	std::queue<SshMessage*> m_queueMessages;

	typedef CThreadWin Base;
};
/***************************************************************************************************/
};
/***************************************************************************************************/