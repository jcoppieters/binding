#ifndef INCLUDED_JSONSERVERCFG_H
#define INCLUDED_JSONSERVERCFG_H
/**************************************************************/
#ifndef INCLUDED_JSONBASE_H
	#include "json/jsonbase.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgServer;
class AppRemoteCfgServers;
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServersCfg : public JsonBaseClass
{
public:
	JsonServersCfg();
	~JsonServersCfg();

	bool format(std::string* dest, const AppRemoteCfgServers& src);
	HttpRCfgResult_t parse(AppRemoteCfgServers* dest, const std::string& str);

protected:
	bool parseServers(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_SERVERS;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerCfg : public JsonBaseClass
{
public:
	JsonServerCfg();
	~JsonServerCfg();

	struct json_object* format(const AppRemoteCfgServer& src);
	HttpRCfgResult_t parse(AppRemoteCfgServer* dest, struct json_object *val);

protected:
	bool parseName(char* key, struct json_object *val, void* pvData);
	bool parseHostName(char* key, struct json_object *val, void* pvData);
	bool parseHostPort(char* key, struct json_object *val, void* pvData);
	bool parsePassword(char* key, struct json_object *val, void* pvData);
	bool parseSecured(char* key, struct json_object *val, void* pvData);

protected:
	// Helper 
	bool helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppRemoteCfgServer::*pf)(const CString&));

private:
	static const char* const KEY_NAME;
	static const char* const KEY_HOST_NAME;
	static const char* const KEY_HOST_PORT;
	static const char* const KEY_PASSWORD;
	static const char* const KEY_SECURED;
};
/**************************************************************/


/**************************************************************/
}; /*  namespace DUOTECNO */
/**************************************************************/
#endif 
/**************************************************************/