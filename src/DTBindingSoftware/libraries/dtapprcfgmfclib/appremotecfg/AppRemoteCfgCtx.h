#ifndef INCLUDED_APPREMOTECFGCTX_H
#define INCLUDED_APPREMOTECFGCTX_H
/**************************************************************/
#ifndef INCLUDED_APPRCFGTASKS_H
	#include "apprcfg/apprcfgtasks.h"
#endif 
#ifndef INCLUDED_HTTPRCFG_CLASSES_H
	#include "httprcfg/httprcfg_classes.h"
#endif 
#ifndef INCLUDED_APPREMOTECFGCMN_H
	#include "appRemoteCfgCmn.h"
#endif 
#ifndef INCLUDED_APPREMOTECFGSERVERS_H
	#include "appremoteCfgServers.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgClientCredentials : public AppCfgClientCredentials
{
public:
	AppRemoteCfgClientCredentials( );

	void updateCredentials(AppCfgClientCredentials& newCredentials);
	void clearClientCredentialsInfo( );

	// password
	void setPassword(const CString& s);
	const CString& getPassword() const;
	const CString& getPassword();

	// secure...
	void setUseSecure(bool state);
	bool getUseSecure() const;

	// tokenExpiryTime
	void setCurrTokenExpiryMsec(int value);
	int getCurrTokenExpiryMsec() const;

private:
	CString m_password;						// https password.
	bool m_useSecure;						// Use https service
	int m_currTokenExpiryMsec;

	typedef AppCfgClientCredentials Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgCtx
{
public:
	typedef enum {
		STATE_IDLE = 0,
		STATE_BUSY_CONNECT,				// Connect & https connect.
		STATE_BUSY_REFRESH_ACCTOKENS,
		STATE_BUSY_BINDINGS_ERASE,
		STATE_BUSY_BINDINGS_WRITE,
		STATE_BUSY_BACKUP_UPLOAD,
		STATE_BUSY_REQUEST_FIRMWAREVERSIONS,
		STATE_BUSY_DOWNLOAD_FILE

	} State_t;

	AppRemoteCfgCtx();
	~AppRemoteCfgCtx();

	void setWorkingDirectory(const CString& s);
	const CString& getWorkingDirectory( ) const;
	const CString& getWorkingDirectory( );

	void setFileName(const CString& s);
	const CString& getFileName( ) const;
	const CString& getFileName( );

	void setHost(const CString& hostName,int port); 
	const CString& getHostName() const;
	int getHostPort() const;
 
	void setConnected(bool state);
	bool getConnected( ) const;

	AppRcfgTask* getCurrentTask( );
	const AppRcfgTask* getCurrentTask( ) const;
	void setCurrentTask(AppRcfgTask* pTask);

	State_t getTaskState( ) const;
	void setTaskState(State_t state);

	const AppRemoteCfgClientCredentials& getCredentials() const {
		return m_credentials;
	}
	AppRemoteCfgClientCredentials& getCredentials() {
		return m_credentials;
	}

	const AppRemoteCfgServerInfo& getServerInfo() const {
		return m_serverInfo;
	}
	AppRemoteCfgServerInfo& getServerInfo() {
		return m_serverInfo;
	}
	AppRemoteCfgServers& getServers() {
		return m_servers;
	}
	AppRemoteCfgFirmwareVersionInfo& getFirmwareInfoVersions( ) {
		return m_firmwareInfo;
	}

	void clearLogErrors( );
	void setLogErrors(const CStringList& errors);
	const CStringList& getLogErrors() const {
		return m_LogErrors;
	}

protected:
	void destroyTask();

private:		
	AppRemoteCfgServers m_servers;						// Lijst van de servers.
	AppRemoteCfgClientCredentials m_credentials;		// De https client credentials.
	AppRemoteCfgServerInfo m_serverInfo;				// De Server info 
	AppRemoteCfgFirmwareVersionInfo m_firmwareInfo;		// Cloud firmware info
	CString m_directory;								// Deze bevat de working directory.
	CString m_fileName;									// De bevat de naam van het project
	CString m_hostName;									// Hostname van de gateway.
	int m_port;											// Poort van de gateway.
	bool m_connected;									// Connected to gateway.
	AppRcfgTask* m_pCurrentTask;						// De huidige taak.
	State_t m_state;									// De toestand.		
	CStringList m_LogErrors;							// error log tijdens het uitvoeren van een taak.
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
struct AppRemoteCfgCtx_Util 
{
public:
	static bool taskIdle(AppRemoteCfgCtx& refCtx);
	static bool taskDone(AppRemoteCfgCtx& refCtx);
	static bool taskFailed(AppRemoteCfgCtx& refCtx);
	static bool taskRunning(AppRemoteCfgCtx& refCtx);
	static bool taskClear(AppRemoteCfgCtx& refCtx);
	static bool taskError(AppRemoteCfgCtx& refCtx);
	static const char* taskErrorMsg(AppRemoteCfgCtx& refCtx);
};
/**************************************************************/

/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/
