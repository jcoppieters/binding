#ifndef INCLUDED_APPRCFGTASKS_H
#define INCLUDED_APPRCFGTASKS_H
/**************************************************************/
#ifndef INCLUDED_DTTHREADWIN32_H
	#include "win32/dtthreadwin32.h"
#endif 
#ifndef INCLUDED_HTTPRCFG_CLASSES_H
	#include "httprcfg/httprcfg_classes.h"
#endif 
#ifndef INCLUDED_HTTPRCFG_CMN_H
	#include "httprcfg/httprcfg_cmn.h"
#endif 
#ifndef INCLUDED_JSONBASE_H
	#include "json/jsonbase.h"
#endif 

#include <string>		// std::string
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTask : public CThreadWin
{
public:
	typedef enum {
		STATUS_IDLE = 0,
		STATUS_BUSY,		
		STATUS_DONE,
		STATUS_ERROR,
		STATUS_CANCEL
	} Status_t;

	typedef enum {
		TYPE_LOGGING_INFO,			// Info berichten 
		TYPE_LOGGING_ERRORS			// Fout berichten enkel van de API.
	} LogType_t;


	AppRcfgTask(const char* hostAddress,int port);

	Status_t getStatus( );				
	const char* getMsgError( );
	HttpRCfgResult_t getErrorCode( );

	BOOL logChanged(LogType_t type,CStringList* pList);

protected:
	void setStatus(Status_t state);	
	void setError(HttpRCfgResult_t ec,const char* msg);
	const char* getHostAddress( ) const;
	int getPort() const;

	virtual void ThreadCtor();
	virtual void ThreadDtor();

	void addLogMessage(const char* str);
	void addLogMessages(const CStringList& list,bool removeOld=true);
	void addErrorMessage(const CStringList& list);	// Dit is de error log die opgevraagd wordt via de API !!

	struct DataInfo {
		DataInfo();
		CThreadMutexWin m_mutexLock;
		CThreadEventWin m_eventChanged;
	};

private:
	volatile Status_t m_status;
	const int m_port;
	std::string m_hostAddress;
	std::string m_msgError;
	HttpRCfgResult_t m_errorCode;
	CThreadMutexWin m_mutexStatus;

	//typedef struct LogInfo LogInfo_t;

	struct LogInfo : public DataInfo {	
	public:
		LogInfo();
		BOOL Changed(CStringList* pStringList);
		void addMessage(const char* str);
		void addMessages(const CStringList& list, bool removeOld);
	private:
		CStringList m_data;	
	};

	LogInfo m_logInfo;
	LogInfo m_logErrors;

	typedef CThreadWin Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskConnect_Base : public AppRcfgTask
{
public:
	AppRcfgTaskConnect_Base(const char* hostAddress, int port);
	virtual ~AppRcfgTaskConnect_Base();
	BOOL getNodeInfo(AppCfgNodeInfo* pNodeInfo);

protected:
	void setNodeInfo(const AppCfgNodeInfo& info);

private:
	AppCfgNodeInfo m_nodeInfo;
	CThreadMutexWin m_mutexLock;

	typedef AppRcfgTask Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskConnect : public AppRcfgTaskConnect_Base
{
public:
	AppRcfgTaskConnect(const char* hostAddress,int port);
	virtual ~AppRcfgTaskConnect( );

protected:
	virtual DWORD Run(LPVOID arg);
	bool taskRequestNodeInfo();

private:
	typedef AppRcfgTaskConnect_Base Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskRequestCloudFirmwareInfo : public AppRcfgTask
{
public:
	AppRcfgTaskRequestCloudFirmwareInfo(const char* hostAddress, int port);
	virtual ~AppRcfgTaskRequestCloudFirmwareInfo();

	BOOL getInfo(AppRemoteCfgFirmwareVersionInfo* pNodeInfo);

protected:	
	virtual DWORD Run(LPVOID arg);
	bool taskRequestInfo();
	void setInfo(const AppRemoteCfgFirmwareVersionInfo& info);

private:	
	AppRemoteCfgFirmwareVersionInfo m_info;
	CThreadMutexWin m_mutexLock;

	typedef AppRcfgTask Base;
};


/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskDownloadFile : public AppRcfgTask
{
public:
	AppRcfgTaskDownloadFile(const char* url,const char* destFileName);
	virtual ~AppRcfgTaskDownloadFile();

protected:
	virtual DWORD Run(LPVOID arg);
	bool taskDownloadFile();

private:
	typedef AppRcfgTask Base;

	CString m_url;
	CString m_destFileName;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTask_https 
{
public:
	AppRcfgTask_https(AppCfgClientCredentials credentials);
	virtual ~AppRcfgTask_https();
	const AppCfgClientCredentials getCredentials() const;	

protected:
	void updateCredentials(const AppCfgClientCredentials& r);
	const char* getClientID( );
	const char* getAccessToken( );
	const char* getRefreshToken( );

private:
	AppCfgClientCredentials m_credentials;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskConnectHttps : public AppRcfgTaskConnect_Base, public AppRcfgTask_https
{
public:
	AppRcfgTaskConnectHttps(const char* hostAddress, int port, const char* password, AppCfgClientCredentials credentials);
	AppRcfgTaskConnectHttps(const char* hostAddress, int port, AppCfgClientCredentials credentials);
	virtual ~AppRcfgTaskConnectHttps();

protected:
	virtual DWORD Run(LPVOID arg);
	bool taskCreateClient();
	bool taskRefreshToken();
	bool taskRequestNodeInfo();

private:
	typedef enum {
		MODE_CONNECT = 0,		// Connectie via password of via refresh access token...
		MODE_REFRESH			// Refreshen van het token.
	} Mode_t;

	const Mode_t m_mode;
	CString m_password;

	typedef AppRcfgTaskConnect_Base Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskBindings_Base : public AppRcfgTask
{
public:
	AppRcfgTaskBindings_Base(const char* hostAddress, int port, const char* path, const AppCfgBindAction& refAction);
	virtual ~AppRcfgTaskBindings_Base() = 0;

	BOOL progressChanged(AppCfgBindInfoNodes* pInfoNodes);

protected:
	virtual DWORD Run(LPVOID arg);
	bool taskUploadBindingFiles();
	bool taskStartTask();
	bool taskRequestLogInfo();
	bool taskRequestProgressInfo();

protected:
	void setProgressInfo(const AppCfgBindInfoNodes& r);
	const AppCfgBindAction& getAction() const {
		return m_action;
	}
	const char* getPath() const;
	void handleResponseBindingsProgress(BindingFileFsmStatus_t);

protected:
	virtual HttpRCfgResult_t sendUploadBindFile(int nodeAddress, const char* fileName, AppRCfgResult* response) = 0;
	virtual HttpRCfgResult_t sendStartBindingsAction(const AppCfgBindAction& action, AppRCfgResult* response) = 0;
	virtual HttpRCfgResult_t sendBindingsGetLog(AppCfgLogInfo* progressLogInfo) = 0;
	virtual HttpRCfgResult_t sendBindingsGetProgress(AppCfgBindInfoNodes* progessInfoNodes) = 0;

private:
	const AppCfgBindAction m_action;
	const std::string m_path;

	struct ProgressInfo : public DataInfo {
		ProgressInfo();
		AppCfgBindInfoNodes m_data;
	} m_progressInfo;

	typedef AppRcfgTask Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskBindings : public AppRcfgTaskBindings_Base
{
public:
	AppRcfgTaskBindings(const char* hostAddress,int port,const char* path, const AppCfgBindAction& refAction);
	virtual ~AppRcfgTaskBindings( );

protected:
	virtual HttpRCfgResult_t sendUploadBindFile(int nodeAddress, const char* fileName, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendStartBindingsAction(const AppCfgBindAction& action, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendBindingsGetLog(AppCfgLogInfo* progressLogInfo);
	virtual HttpRCfgResult_t sendBindingsGetProgress(AppCfgBindInfoNodes* progessInfoNodes);

private:
	typedef AppRcfgTaskBindings_Base Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskBindingsHttps : public AppRcfgTaskBindings_Base, public AppRcfgTask_https
{
public:
	AppRcfgTaskBindingsHttps(const char* hostAddress, int port, const char* path, const AppCfgBindAction& refAction, AppCfgClientCredentials credentials);
	virtual ~AppRcfgTaskBindingsHttps();

protected:
	virtual HttpRCfgResult_t sendUploadBindFile(int nodeAddress, const char* fileName, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendStartBindingsAction(const AppCfgBindAction& action, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendBindingsGetLog(AppCfgLogInfo* progressLogInfo);
	virtual HttpRCfgResult_t sendBindingsGetProgress(AppCfgBindInfoNodes* progessInfoNodes);

private:
	typedef AppRcfgTaskBindings_Base Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskUploadBackupBindPrj_Base : public AppRcfgTask
{
public:
	AppRcfgTaskUploadBackupBindPrj_Base(const char* hostAddress,int port,const char* path, const char* fileName);
	virtual ~AppRcfgTaskUploadBackupBindPrj_Base( );

protected:
	virtual DWORD Run(LPVOID arg);

	bool taskRequestNodeInfo( );
	bool taskRequestDirInfo( );
	bool taskDeleteOldFiles( );
	bool taskArchiveFiles( );
	bool taskUploadBackup( );	
	bool taskRemoveArchive( );

protected:
	virtual HttpRCfgResult_t sendGetNodeInfo(AppCfgNodeInfo* nodeInfo) = 0;
	virtual HttpRCfgResult_t sendDeleteFile(const char* fileName, AppRCfgResult* response) = 0;
	virtual HttpRCfgResult_t sendUploadFile(const char* archiveName, const char* destPath, AppRCfgResult* response) = 0;

private:
	std::string m_archiveName;
	std::string m_path;
	CStringList m_oldFiles;

	static const char* const DESTINATION_PATH;

	typedef AppRcfgTask Base;
};
/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskUploadBackupBindPrj : public AppRcfgTaskUploadBackupBindPrj_Base
{
public:
	AppRcfgTaskUploadBackupBindPrj(const char* hostAddress, int port, const char* path, const char* fileName);
	virtual ~AppRcfgTaskUploadBackupBindPrj();

protected:
	virtual HttpRCfgResult_t sendGetNodeInfo(AppCfgNodeInfo* nodeInfo);
	virtual HttpRCfgResult_t sendDeleteFile(const char* fileName, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendUploadFile(const char* archiveName, const char* destPath, AppRCfgResult* response);

private:
	typedef AppRcfgTaskUploadBackupBindPrj_Base Base;
};
/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskUploadBackupBindPrjHttps : public AppRcfgTaskUploadBackupBindPrj_Base, public AppRcfgTask_https
{
public:
	AppRcfgTaskUploadBackupBindPrjHttps(const char* hostAddress, int port, const char* path, const char* fileName, AppCfgClientCredentials credentials);
	virtual ~AppRcfgTaskUploadBackupBindPrjHttps();

protected:
	virtual HttpRCfgResult_t sendGetNodeInfo(AppCfgNodeInfo* nodeInfo);
	virtual HttpRCfgResult_t sendDeleteFile(const char* fileName, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendUploadFile(const char* archiveName, const char* destPath, AppRCfgResult* response);

private:
	typedef AppRcfgTaskUploadBackupBindPrj_Base Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskNames_Base : public AppRcfgTask
{
public:
	AppRcfgTaskNames_Base(const char* hostAddress, int port, const char* path, const AppCfgRemoteActionList& refActionList);
	virtual ~AppRcfgTaskNames_Base() = 0;

	BOOL progressChanged(AppCfgCmnProgressInfo* pInfoProgress);

protected:
	virtual DWORD Run(LPVOID arg);
	bool taskStartTask();
	bool taskRequestLogInfo();
	bool taskRequestErrorsInfo();
	bool taskRequestProgressInfo();
	void handleProgressInfo(const AppCfgCmnProgressInfo& info);

protected:
	void setProgressInfo(const AppCfgCmnProgressInfo& r);

protected: // Virtuele functies voor de afgeleidde klassen...
	virtual HttpRCfgResult_t sendStartWriteNames(const AppCfgRemoteActionList& listActions, AppRCfgResult* response) = 0;
	virtual HttpRCfgResult_t sendBindingsGetLogInfo(AppCfgLogInfo* progressLogInfo) = 0;
	virtual HttpRCfgResult_t sendBindingsGetLogErrors(AppCfgLogInfo* progressLogInfo) = 0;
	virtual HttpRCfgResult_t sendBindingsGetProgress(AppCfgCmnProgressInfo* progressInfo) = 0;
	const AppCfgRemoteActionList& getListActions( ) { return m_listActions; }

private:
	AppCfgRemoteActionList m_listActions;

	struct ProgressInfo : public DataInfo {
		ProgressInfo();
		AppCfgCmnProgressInfo m_data;
	} m_progressInfo;

	typedef AppRcfgTask Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskNames: public AppRcfgTaskNames_Base
{
public:
	AppRcfgTaskNames(const char* hostAddress, int port, const char* path, const AppCfgRemoteActionList& refActionList);
	virtual ~AppRcfgTaskNames();

protected: // Virtuele functies voor de afgeleidde klassen...
	virtual HttpRCfgResult_t sendStartWriteNames(const AppCfgRemoteActionList& listActions, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendBindingsGetLogInfo(AppCfgLogInfo* progressLogInfo);
	virtual HttpRCfgResult_t sendBindingsGetLogErrors(AppCfgLogInfo* progressLogInfo);
	virtual HttpRCfgResult_t sendBindingsGetProgress(AppCfgCmnProgressInfo* progressInfo);

private:
	typedef AppRcfgTaskNames_Base Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRcfgTaskNamesHttps : public AppRcfgTaskNames_Base, public AppRcfgTask_https
{
public:
	AppRcfgTaskNamesHttps(const char* hostAddress, int port, const char* path, const AppCfgRemoteActionList& refActionList, AppCfgClientCredentials credentials);
	virtual ~AppRcfgTaskNamesHttps();

protected: // Virtuele functies voor de afgeleidde klassen...
	virtual HttpRCfgResult_t sendStartWriteNames(const AppCfgRemoteActionList& listActions, AppRCfgResult* response);
	virtual HttpRCfgResult_t sendBindingsGetLogInfo(AppCfgLogInfo* progressLogInfo);
	virtual HttpRCfgResult_t sendBindingsGetLogErrors(AppCfgLogInfo* progressLogInfo);
	virtual HttpRCfgResult_t sendBindingsGetProgress(AppCfgCmnProgressInfo* progressInfo);

private:
	typedef AppRcfgTaskNames_Base Base;
};
/**************************************************************/
};
/**************************************************************/
#endif
/**************************************************************/