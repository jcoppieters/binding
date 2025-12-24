#ifndef INCLUDED_APPREMOTECFG_H
#define INCLUDED_APPREMOTECFG_H
//////////////////////////////////////////////////////////////////////
#if (USE_TOOLS_APPREMOTECFG == 1)
//////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_IAPPREMOTECFGAPI_H
	#include "appremotecfg/IAppRemoteCfgApi.h"
#endif 
//////////////////////////////////////////////////////////////////////
namespace DUOTECNO {
//////////////////////////////////////////////////////////////////////
class AppRemoteCfg : public IAppRemoteCfgSystemEvents
{
public:
	static AppRemoteCfg* getInstance();
	~AppRemoteCfg( );

	// De dialoog die de events implementeert.
	// Subscribe bij openen / unsubscribe bij sluiten.
	void subscribeGui(IAppRemoteCfgGUI* pGui);

	// Connectie maken met de server...
	IAppRemoteCfgApi::Result_t setConnect(const char* hostName,int port,bool useHttpsSecure = false,const char* password = 0);

	// Disconnect met de server...
	IAppRemoteCfgApi::Result_t setDisconect( );

	// Geconnecteerd met een server ?
	bool connected();

	// ...
	IAppRemoteCfgApi::Result_t getServerInfo(const DUOTECNO::AppRemoteCfgServerInfo** ppInfo);

	// Taken...
	bool startBindingsWrite(bool onlyModified);
	bool startBindingsErase( );
	bool startBackupUpload( );
	bool startUploadNames( );

	// Opvragen of er fouten gebeurd zijn bij het uitvoeren van een taak...
	bool getLogErrors(const CStringList** pStringList) const;
	
	// Internal pointer to impl...
	IAppRemoteCfgApi* get() { return m_pAppRemoteCfg; }

protected:
	AppRemoteCfg( );

	// Timer callback voor het pollen van de fsm.
	static void timerCallback(HWND unnamedParam1,UINT unnamedParam2,UINT_PTR unnamedParam3,DWORD unnamedParam4);

protected:
	// DUOTECNO::IAppRemoteCfgSystemEvents events
	virtual void taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
		HttpRCfgResult_t errorCode = RESULT_SUCCESS, const char* msgError = 0); 
	virtual void taskBindingsProgressInfo(const AppCfgBindInfoNodes& refInfo);
	virtual void taskUploadNamesProgressInfo(const AppCfgCmnProgressInfo& refInfo);

private:
	typedef enum {
		TASK_NONE = 0,
		TASK_BINDINGS_ERASE,
		TASK_BINDINGS_WRITE,
		TASK_BACKUP_UPLOAD,
		TASK_NAMES_WRITE
	} Task_t;	

	IAppRemoteCfgApi* m_pAppRemoteCfg;		// De implementatie.
	UINT_PTR m_timerID;						// Id van de polling timer
	Task_t m_currentTask;					// De huidige taak wordt hier bijgehouden nodig voor binding erase/write.
	AppCfgBindInfoNodes m_bindInfoNodes;	// De info van de taak...

	friend struct AppRemoteCfgUtils;
};
//////////////////////////////////////////////////////////////////////

struct AppRemoteCfgUtils
{
	// Updaten van de Ctx: projectdir + filename
	// Deze aanroepen voor het starten van een taak.
	static bool updateProjectFileInfo(AppRemoteCfg& refParent);

	// Voor het versturen van de bindings moeten de binding bestanden gesaved worden.
	static bool saveModifiedProject(AppRemoteCfg& refParent);

	// update bindings write.
	static bool updateBindingsWritten(AppRemoteCfg& refParent);

	// update bindings erased.
	static bool updateBindingsErased(AppRemoteCfg& refParent);

	// update bindings write.
	static bool updateBindingsWrittenFailed(AppRemoteCfg& refParent);

	// Opvragen van de node lijst.
	static bool getNodeBindingFiles(AppRemoteCfg& refParent,std::list<int>* nodes, bool onlyModified=false);

	// TODO: Status wanneer de bindings niet allemaal konden doorgestuurd worden !!!
	// ...

	// Opvragen van alle namen van de nodes & units.
	// Vult de lijst op met alle namen van de nodes/units aanwezig in de document nodeDatabase.
	static bool getListNames(AppRemoteCfg& refParent,AppCfgRemoteActionList* pListDest);
};

//////////////////////////////////////////////////////////////////////
}; /* end namespace DUOTECNO */
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#endif /* INCLUDED_APPREMOTECFG_H */
#endif 