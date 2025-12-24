#include "stdafx.h"
#include "jsonserverscfg.h"

#include "appremotecfg/AppRemoteCfgServers.h"
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonServersCfg::KEY_SERVERS = "servers";
/**************************************************************/
JsonServersCfg::JsonServersCfg() { 
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServersCfg::parseServers));
}
JsonServersCfg::~JsonServersCfg() { }

bool JsonServersCfg::format(std::string* dest, const AppRemoteCfgServers& src) {
	
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}

	const int count = src.getCount();
	for (int i = 0; i<count; ++i) {
		AppRemoteCfgServer* pServer = 0;
		if (src.get(i, &pServer)) {			
			JsonServerCfg jsonFormatter;
			struct json_object* const object = jsonFormatter.format(*pServer);
			if (0 != object) {
				json_object_array_add(array_items, object);
			}			
		}
	}

	struct json_object* jsonObject = json_object_new_object();
	if (0 != jsonObject) {
		json_object_object_add(jsonObject, KEY_SERVERS, array_items);
		std::string temp = json_object_to_json_string(jsonObject);
		ASSERT(temp.length() > 0);
		*dest = temp;
		json_object_put(jsonObject);
	}
	return true;
}

HttpRCfgResult_t JsonServersCfg::parse(AppRemoteCfgServers* dest, const std::string& str) {
	return helper_parse(str, dest);
}

bool JsonServersCfg::parseServers(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY_SERVERS)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	AppRemoteCfgServers* dest = reinterpret_cast<AppRemoteCfgServers*>(pvData);
	if (0 == dest) {
		return false;
	}

	JsonServerCfg jsonServerCfg;
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonObjectServer = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonObjectServer)) {
			ASSERT(json_type_object != json_object_get_type(jsonObjectServer));
			return false;
		}

		AppRemoteCfgServer* const pNewServer = new AppRemoteCfgServer();
		ASSERT(0 != pNewServer);
		if (RESULT_SUCCESS != jsonServerCfg.parse(pNewServer, jsonObjectServer)) {
			ASSERT(0);
			delete pNewServer;
			return false;
		}
		dest->add(pNewServer);
	}
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonServerCfg::KEY_NAME = "name";
const char* const JsonServerCfg::KEY_HOST_NAME = "hostName";
const char* const JsonServerCfg::KEY_HOST_PORT = "port";
const char* const JsonServerCfg::KEY_PASSWORD = "password";
const char* const JsonServerCfg::KEY_SECURED = "secured";
/**************************************************************/
JsonServerCfg::JsonServerCfg() 
{ 
	m_arrParser.SetSize(5);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerCfg::parseName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerCfg::parseHostName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerCfg::parseHostPort));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerCfg::parsePassword));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerCfg::parseSecured));
}
JsonServerCfg::~JsonServerCfg() { }

struct json_object* JsonServerCfg::format(const AppRemoteCfgServer& src) {

	json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		return 0;
	}
	const char* const friendlyName = src.getFriendlyName();
	if (0 != friendlyName) {
		json_object_object_add(jsonObject, KEY_NAME, json_object_new_string((LPCSTR)friendlyName));
	}

	const char* const hostName = src.getHostName();
	if (0 != hostName) {
		json_object_object_add(jsonObject, KEY_HOST_NAME, json_object_new_string((LPCSTR)hostName));
	}

	const char* const password = src.getPassword();
	if (0 != password) {
		json_object_object_add(jsonObject, KEY_PASSWORD, json_object_new_string((LPCSTR)password));
	}
	json_object_object_add(jsonObject, KEY_HOST_PORT, json_object_new_int(src.getHostPort()));
	json_object_object_add(jsonObject, KEY_SECURED, json_object_new_boolean(src.getSecured()));
	return jsonObject;
}
	
HttpRCfgResult_t JsonServerCfg::parse(AppRemoteCfgServer* dest, struct json_object *val) {
	return helper_parse(val, dest);
}

// protected.

bool JsonServerCfg::parseName(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_NAME, &AppRemoteCfgServer::setFriendlyName);
}
bool JsonServerCfg::parseHostName(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_HOST_NAME, &AppRemoteCfgServer::setHostName);
}
bool JsonServerCfg::parseHostPort(char* key, struct json_object *val, void* pvData) {
	int port = -1;
	if (!helper_getValueInt(key, val, KEY_HOST_PORT, &port)) {
		return false;
	}
	AppRemoteCfgServer* dest = reinterpret_cast<AppRemoteCfgServer*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setHostPort(port);
	return true;
}
bool JsonServerCfg::parsePassword(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_PASSWORD, &AppRemoteCfgServer::setPassword);
}

bool JsonServerCfg::parseSecured(char* key, struct json_object *val, void* pvData) {
	bool value = false;
	if (!helper_getValueBoolean(key, val, KEY_SECURED, &value)) {
		return false;
	}
	AppRemoteCfgServer* dest = reinterpret_cast<AppRemoteCfgServer*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setSecured(value);	
	return true;
}

bool JsonServerCfg::helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppRemoteCfgServer::*pf)(const CString&)) {
	CString str;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}
	AppRemoteCfgServer* dest = reinterpret_cast<AppRemoteCfgServer*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(str);
	return true;
}

/**************************************************************/
}; /*  namespace DUOTECNO */
/**************************************************************/