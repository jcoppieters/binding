#ifndef INCLUDED_APPREMOTECFGAPI_H
#define INCLUDED_APPREMOTECFGAPI_H
/**************************************************************/
#ifndef INCLUDED_IAPPREMOTECFGAPI_H
	#include "IAppRemoteCfgApi.h"
#endif 
#ifndef INCLUDED_HTTPRCFG_CLASSES_H
	#include "httprcfg/httprcfg_classes.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgCtx;
class AppRemoteCfgFsm;
class AppRemoteCfgMediator;
/**************************************************************/

/**************************************************************/
class AppRemoteCfgApi : public IAppRemoteCfgApi
{
public:
	virtual ~AppRemoteCfgApi( );

	// Opvragen van de software versie.
	virtual void getVersion(int* major, int* minor);

	// De GUI koppelen en ontkoppelen (nullptr)
	virtual void setSubscriberGui(IAppRemoteCfgGUI* ptrGui);
	virtual void setSubscriberSystemEvents(IAppRemoteCfgSystemEvents* ptr);

	// Connect info van de host.
	// Indien een secure zal probereren de credentials in te lezen, indien mislukt moet er een paswoord meegegeven worden...
	virtual Result_t setHostInfo(const char* hostName, int port, bool useHttpsSecure = false, const char* password = 0);

	// Dit is een disconnect.
	// Wist alle info ivm de server...
	virtual Result_t disConnect();

	// ProjectInfo doorgeven.
	// Projectdir & bestandsnaam van het project...
	virtual void setProjectInfo(const char* projectDir, const char* fileName);		// Bij het instellen van de projectdir worden de servers ingeladen.
	virtual const char* getProjectDir();
	virtual const char* getProjectFileName();

	// Een taak starten
	virtual Result_t startTask(AppRemoteCfgTasks_t task, void* pvUserData);

	// Pollen 
	virtual bool poll(int msec);

	// task Idle
	virtual bool taskIdle( );

	// Opvragen van de info van de server.
	virtual Result_t getServerInfo(const AppRemoteCfgServerInfo** ppInfo);

	// Opvragen van de firmware info.
	virtual Result_t getFirmwareVersionsInfo(const AppRemoteCfgFirmwareVersionInfo** ppInfo);
		
	// Connected ?
	virtual bool connected();


public:
	// Opvragen van de servers info.
	Result_t getInfoServers(AppRemoteCfgServers** ppServers);
	int addServer(const AppRemoteCfgServer& newServer);				// -1 indien mislukt, anders het aantal...
	int removeServer(const CString& hostName, int port);			// -1 indien mislukt, anders het aantal...
	Result_t getServer(int index,AppRemoteCfgServer** ppServer);	
	bool getLogErrors(const CStringList** pStringList) const;
	AppRemoteCfgMediator& getMediator();

protected:
	AppRemoteCfgApi();

private:
	AppRemoteCfgCtx* m_pAppRemoteCfgCtx;
	AppRemoteCfgFsm* m_pAppRemoteCfgFsm;
	AppRemoteCfgMediator* m_pAppRemoteCfgMediator;

	// Utilities...
	friend IAppRemoteCfgApi* appremotecfg_create();
};
/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/
