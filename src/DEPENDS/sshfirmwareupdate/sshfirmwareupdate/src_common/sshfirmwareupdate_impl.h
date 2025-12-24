#pragma once
/***************************************************************************************************/
#include "sshfirmwareupdate_api.h"
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/
class AppTaskSSH;
/***************************************************************************************************/
class CSSHFirmwareUpdate_Impl : public ISSHFirmwareUpdate 
{
public:
	CSSHFirmwareUpdate_Impl();
	virtual ~CSSHFirmwareUpdate_Impl( );

	int startTaskUpdate(
		const char* hostName, 
		const char* userName,
		const char* password,
		const char* applicationName,
		const char* localFileName);

	SshTaskStatus_t poll(int milliseconds);
	bool getMessage(SshMessage* pMessage);
	virtual void getVersion(int* major, int* minor);

private:
	AppTaskSSH* m_pTask;
};
/***************************************************************************************************/
};
