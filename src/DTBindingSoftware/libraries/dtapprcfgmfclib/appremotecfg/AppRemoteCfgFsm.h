#ifndef INCLUDED_APPREMOTECFGFSM_H
#define INCLUDED_APPREMOTECFGFSM_H
/**************************************************************/
#ifndef INCLUDED_HTTPRCFG_CLASSES_H
	#include "httprcfg/httprcfg_classes.h"
#endif 
#ifndef INCLUDED_APPREMOTECFGCMN_H
	#include "AppRemoteCfgCmn.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgCtx;
class AppRemoteCfgFsm;
/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask 
{
public:
	AppRCfgFsmTask(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask( );

	virtual bool startTask(AppRemoteCfgFsm& parent,AppRemoteCfgTasks_t task, void* pvUserData) {
		return false;
	}
	virtual bool poll(AppRemoteCfgFsm& parent, int msec) {
		return false;
	}

	virtual void onEnter(AppRemoteCfgFsm& parent) { }
	virtual void onExit(AppRemoteCfgFsm& parent) { }

protected:
	void setTaskStarted(AppRemoteCfgFsm& parent);
	void setTaskDone(AppRemoteCfgFsm& parent);
	void setTaskFailed(AppRemoteCfgFsm& parent, HttpRCfgResult_t errorCode, const char* msg);

private:
	AppRemoteCfgTasks_t m_taskID;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_IDLE : public AppRCfgFsmTask
{
public:
	AppRCfgFsmTask_IDLE(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_IDLE();

	virtual bool startTask(AppRemoteCfgFsm& parent, AppRemoteCfgTasks_t task, void* pvUserData);
	virtual bool poll(AppRemoteCfgFsm& parent, int msec);

protected:
	bool startTaskConnect(AppRemoteCfgFsm& parent);
	bool startTaskRefreshAccTokens(AppRemoteCfgFsm& parent);
	bool startTaskBindings(AppRemoteCfgFsm& parent, void* pvUserData);
	bool startTaskUploadBackup(AppRemoteCfgFsm& parent);
	bool startTaskUploadNames(AppRemoteCfgFsm& parent, void* pvUserData);
	bool startTaskRequestFirmwareVersions(AppRemoteCfgFsm& parent, void* pvUserData);
	bool startTaskDownloadFile(AppRemoteCfgFsm& parent, void* pvUserData);		// pvUserData = CStringLIst [0]=url, [1]=destfilename

private:
	typedef AppRCfgFsmTask Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_BASE : public AppRCfgFsmTask
{
public:
	AppRCfgFsmTask_BUSY_BASE(AppRemoteCfgTasks_t taskID, const char* taskMessage = 0);
	virtual ~AppRCfgFsmTask_BUSY_BASE();

	virtual void onExit(AppRemoteCfgFsm& parent);
	virtual bool poll(AppRemoteCfgFsm& parent, int msec);

protected:
	const char* getTaskMessage() const {
		return m_szTaskMessage;
	}
	// Opvragen van de logberichten en deze doorgeven aan de host (=client.)
	void updateLogMessages(AppRemoteCfgFsm& parent);

private:
	bool updateLogErrors(AppRemoteCfgFsm& parent);
	bool checkTaskStatusDone(AppRemoteCfgFsm& parent);
	bool checkTaskStatusFailed(AppRemoteCfgFsm& parent);

private:
	const char* const m_szTaskMessage;

	typedef AppRCfgFsmTask Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_CONNECT_BASE : public AppRCfgFsmTask_BUSY_BASE
{
public:
	AppRCfgFsmTask_BUSY_CONNECT_BASE(AppRemoteCfgTasks_t taskID,const char* taskMessage);
	virtual ~AppRCfgFsmTask_BUSY_CONNECT_BASE();

	virtual void onEnter(AppRemoteCfgFsm& parent);
	// virtual void onExit(AppRemoteCfgFsm& parent);

protected:
	bool getNodeInfo(AppRemoteCfgFsm& parent, AppCfgNodeInfo* pDest);
	bool getCredentials(AppRemoteCfgFsm& parent, AppCfgClientCredentials* pDest);

	bool updateNodeInfo(AppRemoteCfgFsm& parent);
	bool updateCredentials(AppRemoteCfgFsm& parent);

private:
	typedef AppRCfgFsmTask_BUSY_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_CONNECT: public AppRCfgFsmTask_BUSY_CONNECT_BASE
{
public:
	AppRCfgFsmTask_BUSY_CONNECT(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_BUSY_CONNECT();

	virtual bool poll(AppRemoteCfgFsm& parent, int msec);	

private:
	typedef AppRCfgFsmTask_BUSY_CONNECT_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS : public AppRCfgFsmTask_BUSY_BASE
{
public:
	AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_BUSY_REQUEST_FIRMWAREVERSIONS();

	virtual bool poll(AppRemoteCfgFsm& parent, int msec);

protected:
	bool updateFirmwareVersionInfo(AppRemoteCfgFsm& parent);

private:
	typedef AppRCfgFsmTask_BUSY_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_DOWNLOAD_FILE : public AppRCfgFsmTask_BUSY_BASE
{
public:
	AppRCfgFsmTask_BUSY_DOWNLOAD_FILE(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_BUSY_DOWNLOAD_FILE();

	virtual bool poll(AppRemoteCfgFsm& parent, int msec);

private:
	typedef AppRCfgFsmTask_BUSY_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_REFRESHACCTOKENS : public AppRCfgFsmTask_BUSY_CONNECT_BASE
{
public:
	AppRCfgFsmTask_BUSY_REFRESHACCTOKENS(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_BUSY_REFRESHACCTOKENS();

	virtual bool poll(AppRemoteCfgFsm& parent, int msec);

private:
	typedef AppRCfgFsmTask_BUSY_CONNECT_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_BINDINGS : public AppRCfgFsmTask_BUSY_BASE
{
public:
	AppRCfgFsmTask_BUSY_BINDINGS(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_BUSY_BINDINGS();

	virtual bool poll(AppRemoteCfgFsm& parent, int msec);
	virtual void onEnter(AppRemoteCfgFsm& parent);
	// virtual void onExit(AppRemoteCfgFsm& parent);
private:
	bool requestProgressInfo(AppRemoteCfgFsm& parent);

private:
	typedef AppRCfgFsmTask_BUSY_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_UPLOADBACKUP : public AppRCfgFsmTask_BUSY_BASE
{
public:
	AppRCfgFsmTask_BUSY_UPLOADBACKUP(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_BUSY_UPLOADBACKUP();

	virtual bool poll(AppRemoteCfgFsm& parent, int msec);
	virtual void onEnter(AppRemoteCfgFsm& parent);
	// virtual void onExit(AppRemoteCfgFsm& parent);

private:
	typedef AppRCfgFsmTask_BUSY_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgFsmTask_BUSY_UPLOADNAMES : public AppRCfgFsmTask_BUSY_BASE
{
public:
	AppRCfgFsmTask_BUSY_UPLOADNAMES(AppRemoteCfgTasks_t taskID);
	virtual ~AppRCfgFsmTask_BUSY_UPLOADNAMES();

	virtual bool poll(AppRemoteCfgFsm& parent, int msec);
	virtual void onEnter(AppRemoteCfgFsm& parent);

private:
	bool requestProgressInfo(AppRemoteCfgFsm& parent);

private:
	typedef AppRCfgFsmTask_BUSY_BASE Base;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgFsm 
{
public:
	AppRemoteCfgFsm(AppRemoteCfgCtx& m_ctx, IAppRemoteCfgFsm_Host& refHost);
	virtual ~AppRemoteCfgFsm( );

	// Deze doorgeven aan de taken...
	bool startTask(AppRemoteCfgTasks_t task,void* pvUserData);
	bool poll(int msec);


	// ...

	AppRemoteCfgCtx& getCtx() {
		return m_refCtx;
	}

	// Voor de fsm
	void gotoTask(AppRemoteCfgTasks_t task);

	IAppRemoteCfgFsm_Host& getHost() {
		return m_refHost;
	}

private:
	IAppRemoteCfgFsm_Host& m_refHost;
	AppRemoteCfgCtx& m_refCtx;
	AppRCfgFsmTask* m_pCurrentTask;
	AppRCfgFsmTask* m_pTaskIdle;
	AppRCfgFsmTask* m_pTaskBusyConnect;
	AppRCfgFsmTask* m_pTaskBusyRefreshAccTokens;
	AppRCfgFsmTask* m_pTaskBusyBindings;
	AppRCfgFsmTask* m_pTaskBusyUploadBackup;
	AppRCfgFsmTask* m_pTaskBusyUploadNames;
	AppRCfgFsmTask* m_pTaskRequestFirmwareInfo;
	AppRCfgFsmTask* m_pTaskDownloadFile;
};
/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/
