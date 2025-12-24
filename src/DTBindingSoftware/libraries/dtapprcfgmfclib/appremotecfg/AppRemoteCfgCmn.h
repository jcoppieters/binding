#ifndef INCLUDED_APPREMOTECFGCMN_H
#define INCLUDED_APPREMOTECFGCMN_H
/**************************************************************/
#ifndef INCLUDED_HTTPRCFG_CMN_H
	#include "httprcfg/httprcfg_cmn.h"
#endif 
#ifndef INCLUDED_HTTPRCFG_CLASSES_H
	#include "httprcfg/httprcfg_classes.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
/**************************************************************/
/**************************************************************/
typedef enum {
	APPREMOTE_CFG_TASK_IDLE = 0,
	APPREMOTE_CFG_TASK_CONNECT,
	APPREMOTE_CFG_TASK_REFRESHACCTOKENS,
	APPREMOTE_CFG_TASK_BINDINGS,
	APPREMOTE_CFG_TASK_UPLOADBACKUP,
	APPREMOTE_CFG_TASK_WRITENAMES,
	APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS,
	APPREMOTE_CFG_TASK_DOWNLOADFILE

} AppRemoteCfgTasks_t;
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
typedef enum {
	APPREMOTE_CFG_TASK_STATUS_STARTED,
	APPREMOTE_CFG_TASK_STATUS_DONE,
	APPREMOTE_CFG_TASK_STATUS_FAILED,

} AppRemoteCfgTaskStatus_t;

/**************************************************************/
/**************************************************************/
/**************************************************************/
typedef unsigned int AppRcfgSupportedFlags;

enum {
	APPRCFG_SUPPORTED_CFG_BINDINGS_ERASE = (1 << 0),
	APPRCFG_SUPPORTED_CFG_BINDINGS_WRITE = (1 << 1),
	APPRCFG_SUPPORTED_CFG_BINDINGS_READ = (1 << 2),		// nog niet ok 
	APPRCFG_SUPPORTED_CFG_BACKUP_UPLOAD = (1 << 3),
	APPRCFG_SUPPORTED_CFG_NAMES_WRITE = (1 << 4)
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgServerInfo
{
public:
	AppRemoteCfgServerInfo();

	void clear();

	void setHostName(const CString& name);
	const CString& getHostName() const;

	void setSoftwareVersion(const CString& name);
	const CString& getSoftwareVersion( ) const;

	void setHostPort(int v);
	int getHostPort() const;

	void setSecureLogin(bool value);
	bool getSecureLogin() const;

	void setProductName(const CString& name);
	const CString& getProductName() const;

	void setPhysicalAddress(unsigned long v);
	unsigned long getPhysicalAddress() const;

	void setNodeAddress(int nodeAddress);
	int getNodeAddress() const;

	void setQtVersion(const CString& version);
	const CString& getQtVersion() const;

	void setHardware(const CString& v);
	const CString& getHardware() const;

private:
	CString m_hostName;			// ip-address of dyndns
	int m_hostPort;				// ip-poort
	bool m_secureLogin;			// Secure protocol login.
	CString m_productName;
	CString m_softwareVersion;
	unsigned long m_physicalAddress;
	int m_nodeAddress;
	CString m_qtVersion;
	CString m_hardware;

	friend struct AppRemoteCfgServerInfo_Utils;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
struct AppRemoteCfgServerInfo_Utils {
public:
	static AppRcfgSupportedFlags getSupportedFlags(const AppRemoteCfgServerInfo& r);
	static bool supportedCfgBindingsWrite(const AppRcfgSupportedFlags& flags);
	static bool supportedCfgBindingsErase(const AppRcfgSupportedFlags& flags);
	static bool supportedCfgBindingsRead(const AppRcfgSupportedFlags& flags);
	static bool supportedCfgUploadBackup(const AppRcfgSupportedFlags& flags);
	static bool supportedCfgNamesWrite(const AppRcfgSupportedFlags& flags);

protected:
	typedef enum {
		UNKNOWN = -1,
		TOUCHSCREEN = 0,
		HOMESERVER,
		AV
	} type_t;

	static type_t getType(const AppRemoteCfgServerInfo& r);
	static bool getSoftwareVersion(const AppRemoteCfgServerInfo& r, int* major, int* minor, int* micro);
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/

// Implementeren door de client GUI.
struct IAppRemoteCfgGUI
{
	typedef enum {
		HTTP_EC_NONE = -1,
		HTTP_EC_CONNECT = 0,
		HTTP_EC_AUTHORIZATION = 1,
	} HttpError_t;

	virtual ~IAppRemoteCfgGUI() = 0;

	virtual void addMessage(const char* msg, bool eraseOld) = 0;
	virtual void addMessages(CStringList& messages, bool eraseOld) = 0;
	virtual void taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
		HttpError_t errorCode = HTTP_EC_NONE, const char* msgError = 0) = 0;
#if(0)
	virtual void statusChanged() = 0;									// ???
	virtual void onHttpTaskError(HttpError_t error) = 0;
#endif 
};

// Implementeren door het client systeem.
struct IAppRemoteCfgSystemEvents
{
	virtual ~IAppRemoteCfgSystemEvents() = 0;

	virtual void taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
		HttpRCfgResult_t errorCode = RESULT_SUCCESS, const char* msgError = 0) = 0;
	virtual void taskBindingsProgressInfo(const AppCfgBindInfoNodes& refInfo) = 0;
	virtual void taskUploadNamesProgressInfo(const AppCfgCmnProgressInfo& refInfo) = 0;
};
/**************************************************************/
/**************************************************************/
/**************************************************************/
// Private voor de library.
struct IAppRemoteCfgFsm_Host
{
public:
	virtual ~IAppRemoteCfgFsm_Host() = 0;

	virtual void addMessage(const char* msg, bool eraseOld = false) = 0;
	virtual void addMessages(CStringList& messages, bool eraseOld = false) = 0;
	virtual void taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status, 
		HttpRCfgResult_t errorCode = RESULT_SUCCESS, const char* msgError = 0) = 0;
	virtual void taskBindingsProgressInfo(const AppCfgBindInfoNodes& refInfo) = 0;
	virtual void taskUploadNamesProgressInfo(const AppCfgCmnProgressInfo& refInfo) = 0;
};
/**************************************************************/

/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/
