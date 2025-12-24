#ifndef INCLUDED_APPREMOTECFMEDIATOR_H
#define INCLUDED_APPREMOTECFMEDIATOR_H
/**************************************************************/
#ifndef INCLUDED_APPREMOTECFGCMN_H
	#include "AppRemoteCfgCmn.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgCtx;

/**************************************************************/
/**************************************************************/
/**************************************************************/
// TODO: Referentie naar de FSM ???
// TODO: API interface van maken ??

class AppRemoteCfgMediator : public IAppRemoteCfgFsm_Host
{
public:
	AppRemoteCfgMediator(AppRemoteCfgCtx& refCtx,IAppRemoteCfgGUI* refGui, IAppRemoteCfgSystemEvents* ptrSystemEvents);
	virtual ~AppRemoteCfgMediator();

	void setSubscriberGui(IAppRemoteCfgGUI* ptrGui);
	void setSubscriberSystemEvents(IAppRemoteCfgSystemEvents* ptr);

	// IAppRemoteCfgFsm_Host
	virtual void addMessage(const char* msg, bool eraseOld = false);
	virtual void addMessages(CStringList& messages, bool eraseOld = false);
	virtual void taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
		HttpRCfgResult_t errorCode = RESULT_SUCCESS, const char* msgError = 0);
	virtual void taskBindingsProgressInfo(const AppCfgBindInfoNodes& refInfo);
	virtual void taskUploadNamesProgressInfo(const AppCfgCmnProgressInfo& refInfo);

public:	
	bool loadClientCredentials(const char* hostName,int port);
	bool loadServersInfo( );		
	bool saveServersInfo( );


protected:
	CString getPathClientCredentials( );
	bool deleteClientCredentials(const char* hostName,int port);
	bool clearClientCredentials();
	bool saveClientCredentials();

private:
	AppRemoteCfgCtx& m_refCtx;
	IAppRemoteCfgGUI* m_ptrGui;	
	IAppRemoteCfgSystemEvents* m_ptrSystemEvents;
};
/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/
