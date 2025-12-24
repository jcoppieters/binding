#ifndef INCLUDED_JSONLOGININFO_H
#define INCLUDED_JSONLOGININFO_H
/**************************************************************/
#ifndef INCLUDED_JSONBASE_H
	#include "json/jsonbase.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgLoginInfo;

/************************************************************** 
 *	{
 *		"type" : string,
 *		"connect" : string,
 *		"local" : {			
 *			"address" : string,
 *			"port" : int
 *		},
 *		"remote" : {
 *			"address" : string,
 *			"port" : int
 *		},
 *		"password" : string,
 *	    "physicalAddress" : string,				// sedert v2.3.
 *		"logicalAddress" : string				// sedert v2.3
 *	}
 **************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerLoginInfoBase : public JsonBaseClass
{
public:
	JsonServerLoginInfoBase();
	virtual ~JsonServerLoginInfoBase();

	bool helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppRemoteCfgLoginInfo::*pf)(const CString&));	
	bool helper_setValueInteger(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppRemoteCfgLoginInfo::*pf)(int));
};

/**************************************************************/
/**************************************************************/
/**************************************************************/

// Sedert V2.3: Opslaan / laden en export van het logisch en fysich adres van de server.
class JsonServerLoginInfo : public JsonServerLoginInfoBase
{
public:
	JsonServerLoginInfo();
	~JsonServerLoginInfo();

	bool format(std::string* dest, const AppRemoteCfgLoginInfo& src);
	HttpRCfgResult_t parse(AppRemoteCfgLoginInfo* dest, const std::string& str);

	bool parseType(char* key, struct json_object *val, void* pvData);
	bool parseLocal(char* key, struct json_object *val, void* pvData);
	bool parseRemote(char* key, struct json_object *val, void* pvData);
	bool parsePassword(char* key, struct json_object *val, void* pvData);
	bool parseConnect(char* key, struct json_object *val, void* pvData);
	bool parsePhysicalAddress(char* key, struct json_object *val, void* pvData);	// sedert V2.3
	bool parseLogicalAddress(char* key, struct json_object *val, void* pvData);		// sedert V2.3

	// Sedert V2.1:
	// Hier gaan we deze niet encrypteren.
	virtual bool formatPassword(struct json_object* jsonObject, const AppRemoteCfgLoginInfo& src);

protected:
	static const char* const KEY_TYPE;
	static const char* const KEY_SERVER_LOCAL;
	static const char* const KEY_SERVER_REMOTE;	
	static const char* const KEY_SERVER_PASSWORD;
	static const char* const KEY_SERVER_CONNECT;
	static const char* const KEY_SERVER_PHYSICALADDRESS;	// sedert V2.3
	static const char* const KEY_SERVER_LOGICALADDRESS;		// sedert V2.3

	static const char* const VALUE_TYPE_OFFLINE;
	static const char* const VALUE_TYPE_USB;
	static const char* const VALUE_TYPE_TCP;

	static const char* const VALUE_CONNECT_LOCAL;
	static const char* const VALUE_CONNECT_REMOTE;
};


/**************************************************************/
/**************************************************************/
/**************************************************************/
// Sedert V2.1: Klasse is bedoeld voor een export van de connect info met de server.
// We gaan hier het paswoord encrypteren + alle andere data wordt gewoon getoond.
class JsonServerLoginInfoExport : public JsonServerLoginInfo
{
public:
	JsonServerLoginInfoExport();
	~JsonServerLoginInfoExport();

	HttpRCfgResult_t parse(AppRemoteCfgLoginInfo* dest, const std::string& str) {
		return RESULT_EC_JSON_NOT_IMPLEMENTED;	// Enkel export.
	}
	
	// Hier gaan we deze niet encrypteren.
	virtual bool formatPassword(struct json_object* jsonObject, const AppRemoteCfgLoginInfo& src);
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerAddress : public JsonServerLoginInfoBase
{
public:
	JsonServerAddress();
	~JsonServerAddress();
	
	HttpRCfgResult_t parse(AppRemoteCfgLoginInfo* dest, struct json_object *val);

protected:
	static const char* const KEY_SERVER_HOSTADDRESS;
	static const char* const KEY_SERVER_PORT;	
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerAddressLocal : public JsonServerAddress
{
public:
	JsonServerAddressLocal();
	~JsonServerAddressLocal();

	struct json_object* format(const AppRemoteCfgLoginInfo& src);

private:
	bool parseHostName(char* key, struct json_object *val, void* pvData);
	bool parseHostPort(char* key, struct json_object *val, void* pvData);
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerAddressRemote : public JsonServerAddress
{
public:
	JsonServerAddressRemote();
	~JsonServerAddressRemote();

	struct json_object* format(const AppRemoteCfgLoginInfo& src);
private:
	bool parseHostName(char* key, struct json_object *val, void* pvData);
	bool parseHostPort(char* key, struct json_object *val, void* pvData);
};
/**************************************************************/
}; /* end namespace */
/**************************************************************/
#endif 
/**************************************************************/