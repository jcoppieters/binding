#ifndef INCLUDED_HTTP_RCONFIG_H
#define INCLUDED_HTTP_RCONFIG_H
/**************************************************************/
#ifndef INCLUDED_DTCFGAPI_CLASSES_H
	#include "httprcfg_classes.h"
#endif 
#ifndef INCLUDED_HTTPRCFG_CMN_H
	#include "httprcfg_cmn.h"
#endif 
#ifndef INCLUDED_JSONBASE_H
	#include "json/jsonbase.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
extern int httprcfg_init( );
extern int httprcfg_shutdown( );

/**************************** General *************************/
extern HttpRCfgResult_t httprcfg_sendGetNodeInfo(const char* remoteAddress,
	int port,AppCfgNodeInfo* response);

// https
extern HttpRCfgResult_t httpsrcfg_sendGetNodeInfo(const char* remoteAddress,
	int port, const char* clientID, const char* accessCode, AppCfgNodeInfo* response);

/**************** Cloud - Firmware versies opvragen ***********/
extern HttpRCfgResult_t httprcfg_sendGetCloudFirmwareVersions(const char* remoteAddress,
	int port, AppRemoteCfgFirmwareVersionInfo* response);

// --> TODO: Moeten we een extra header toevoegen om een bestand binair te downloaden ???
// TODO --> Protocol type bepalen adhv de			<-----------------------------
// http & https.

extern HttpRCfgResult_t httprcfg_sendDownloadFile(const char* url, const char* destFileName);

/**************************** Bindings ************************/

extern HttpRCfgResult_t httprcfg_sendUploadBindFile(const char* remoteAddress,
	int port, int nodeAddress, const char* fileName, AppRCfgResult* response);

extern HttpRCfgResult_t httprcfg_sendStartBindingsAction(const char* remoteAddress,
	int port, const AppCfgBindAction& action, AppRCfgResult* response);

extern HttpRCfgResult_t httprcfg_sendBindingsGetLog(const char* remoteAddress,
	int port,AppCfgLogInfo* response);

extern HttpRCfgResult_t httprcfg_sendBindingsGetProgress(const char* remoteAddress,
	int port,AppCfgBindInfoNodes* response);

// https 
extern HttpRCfgResult_t httpsrcfg_sendUploadBindFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, int nodeAddress, const char* fileName, 
	AppRCfgResult* response);

extern HttpRCfgResult_t httpsrcfg_sendStartBindingsAction(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const AppCfgBindAction& action, AppRCfgResult* response);

extern HttpRCfgResult_t httpsrcfg_sendBindingsGetLog(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, AppCfgLogInfo* response);

extern HttpRCfgResult_t httpsrcfg_sendBindingsGetProgress(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, AppCfgBindInfoNodes* response);

/************************** File server ***********************/

extern HttpRCfgResult_t httprcfg_sendUploadFile(const char* remoteAddress,
	int port, const char* fileName, const char* destPath, AppRCfgResult* response);

extern HttpRCfgResult_t httprcfg_sendRequestDirInfo(const char* remoteAddress,
	int port, const char* path, AppCfgFileServerDir* response);

extern HttpRCfgResult_t httprcfg_sendDeleteFile(const char* remoteAddress,
	int port, const char* fileName, AppRCfgResult* response);

// https
extern HttpRCfgResult_t httpsrcfg_sendUploadFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const char* fileName, const char* destPath, AppRCfgResult* response);

extern HttpRCfgResult_t httpsrcfg_sendRequestDirInfo(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const char* path, AppCfgFileServerDir* response);

extern HttpRCfgResult_t httpsrcfg_sendDeleteFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const char* fileName, AppRCfgResult* response);

/************************** Authentication ********************/

extern HttpRCfgResult_t httprcfg_sendAuthCreateClient(const char* remoteAddress, 
	int port, const char* clientID, const char* password, AppCfgClientCredentials* response);

extern HttpRCfgResult_t httprcfg_sendAuthRefreshAccessCode(const char* remoteAddress,
	int port, const char* clientID, const char* refreshToken, AppCfgClientCredentials* response);

/************************* Names config ***********************/
// http
extern HttpRCfgResult_t httprcfg_sendStartSendNames(const char* remoteAddress,int port, const AppCfgRemoteActionList& listAction, AppRCfgResult* response);
extern HttpRCfgResult_t httprcfg_sendCancelSendNames(const char* remoteAddress,int port, AppRCfgResult* response);
extern HttpRCfgResult_t httprcfg_sendNamesGetLogInfo(const char* remoteAddress,int port, AppCfgLogInfo* response);
extern HttpRCfgResult_t httprcfg_sendNamesGetLogErrors(const char* remoteAddress, int port, AppCfgLogInfo* response);
extern HttpRCfgResult_t httprcfg_sendNamesGetProgressInfo(const char* remoteAddress, int port, AppCfgCmnProgressInfo* response);

// progress info.

extern HttpRCfgResult_t httpsrcfg_sendStartSendNames(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const AppCfgRemoteActionList& listAction, 
	AppRCfgResult* response);

extern HttpRCfgResult_t httpsrcfg_sendCancelSendNames(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, AppRCfgResult* response);

extern HttpRCfgResult_t httpsrcfg_sendNamesGetLogInfo(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, AppCfgLogInfo* response);

extern HttpRCfgResult_t httpsrcfg_sendNamesGetLogErrors(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, AppCfgLogInfo* response);

extern HttpRCfgResult_t httpsrcfg_sendNamesGetProgressInfo(const char* remoteAddress, 
	int port, const char* clientID, const char* accessToken, AppCfgCmnProgressInfo* response);

/**************************************************************/
}; /* namespace DUOTECNO*/
/**************************************************************/
#endif 
/**************************************************************/