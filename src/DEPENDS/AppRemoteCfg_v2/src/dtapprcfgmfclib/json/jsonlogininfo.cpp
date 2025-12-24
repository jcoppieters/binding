#include "stdafx.h"
#include "jsonlogininfo.h"
#include "appremotecfg/AppRemoteCfgLoginInfo.h"
#include "httprcfg/httprcfg_http.h"							//AuthHelper
#include "json.h"
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
JsonServerLoginInfoBase::JsonServerLoginInfoBase() { }
JsonServerLoginInfoBase::~JsonServerLoginInfoBase() { }
/**************************************************************/
bool JsonServerLoginInfoBase::helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppRemoteCfgLoginInfo::*pf)(const CString&)) {
	CString str;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}

	AppRemoteCfgLoginInfo* dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(str);
	return true;
}


bool JsonServerLoginInfoBase::helper_setValueInteger(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppRemoteCfgLoginInfo::*pf)(int)) {
	int value = -1;
	if (!helper_getValueInt(key,val,myKey,&value)) {
		return false;
	}
	AppRemoteCfgLoginInfo* dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonServerLoginInfo::KEY_TYPE = "type";						// USB/TCP/OFFLINE
const char* const JsonServerLoginInfo::KEY_SERVER_LOCAL = "localAddress";
const char* const JsonServerLoginInfo::KEY_SERVER_REMOTE = "remoteAddress";
const char* const JsonServerLoginInfo::KEY_SERVER_PASSWORD = "password";
const char* const JsonServerLoginInfo::KEY_SERVER_CONNECT = "connect";			// local,remote
const char* const JsonServerLoginInfo::KEY_SERVER_PHYSICALADDRESS = "physicalAddress";	// sedert V2.3: physical address van de server (tcp/ip)
const char* const JsonServerLoginInfo::KEY_SERVER_LOGICALADDRESS = "logicalAddress";	// sedert V2.3: logical address van de server (tcp/ip)

// value types 
const char* const JsonServerLoginInfo::VALUE_TYPE_OFFLINE = "OFFLINE";
const char* const JsonServerLoginInfo::VALUE_TYPE_USB = "USB";
const char* const JsonServerLoginInfo::VALUE_TYPE_TCP = "TCP";
// value types.
const char* const JsonServerLoginInfo::VALUE_CONNECT_LOCAL = "local";
const char* const JsonServerLoginInfo::VALUE_CONNECT_REMOTE = "remote";
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
JsonServerLoginInfo::JsonServerLoginInfo() : JsonServerLoginInfoBase() 
{
	m_arrParser.SetSize(5);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerLoginInfo::parseType));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerLoginInfo::parseLocal));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerLoginInfo::parseRemote));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerLoginInfo::parsePassword));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerLoginInfo::parseConnect));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerLoginInfo::parsePhysicalAddress));	// sedert V2.3
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerLoginInfo::parseLogicalAddress));	// sedert V2.3			
}
JsonServerLoginInfo::~JsonServerLoginInfo() { }

bool JsonServerLoginInfo::format(std::string* dest, const AppRemoteCfgLoginInfo& src) {

	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		ASSERT(0 != jsonObject);
		return false;
	}

	// Sedert V2.1
	formatPassword(jsonObject,src);

	const char* strType = 0;	
	const AppRemoteCfgLoginInfo::LoginMethodType_t type = src.getMethod();
	if (AppRemoteCfgLoginInfo::LOGIN_METHOD_USB == type) {
		strType = VALUE_TYPE_USB;
	}
	else if (AppRemoteCfgLoginInfo::LOGIN_METHOD_TCPIP == type) {
		strType = VALUE_TYPE_TCP;
	}
	else if (AppRemoteCfgLoginInfo::LOGIN_METHOD_OFFLINE == type) {
		strType = VALUE_TYPE_OFFLINE;
	}
	if (0 != strType) {
		json_object_object_add(jsonObject, KEY_TYPE, json_object_new_string((LPCSTR)strType));
	}

	const char* strConnect = 0;
	const AppRemoteCfgHostAddress_t addressType = src.getPreferedLogin();
	if (HOSTADDRESS_TYPE_LOCAL == addressType) {
		strConnect = VALUE_CONNECT_LOCAL;
	}
	else if (HOSTADDRESS_TYPE_REMOTE == addressType) {
		strConnect = VALUE_CONNECT_REMOTE;
	}
	if (0 != strConnect) {
		json_object_object_add(jsonObject, KEY_SERVER_CONNECT, json_object_new_string((LPCSTR)strConnect));
	}

	// Sedert V2.3: logical node adres van de server.
	const int nodeAddress = src.getNodeAddress();
	if (nodeAddress >= 0x03 && nodeAddress <= 0xFD) {
		CString strNodeAddress;
		strNodeAddress.Format("0x%02x",nodeAddress);
		json_object_object_add(jsonObject, KEY_SERVER_LOGICALADDRESS, json_object_new_string((LPCSTR)strNodeAddress));
	}

	// Sedert V2.3: physical node adres van de server.	
	const int address = src.getPhysicalAddress();
	if ((address >=0) && (address < 0xFFFFFF)) {
		CString strPhysical;
		strPhysical.Format("0x%06x",address);
		json_object_object_add(jsonObject, KEY_SERVER_PHYSICALADDRESS, json_object_new_string((LPCSTR)strPhysical));
	}

	// Local.	
	JsonServerAddressLocal formatterLocal;
	struct json_object* const jsonObjectLocal = formatterLocal.format(src);
	if (0 != jsonObjectLocal) {
		json_object_object_add(jsonObject, KEY_SERVER_LOCAL, jsonObjectLocal);
	}

	// Remote.
	JsonServerAddressRemote formatterRemote;
	struct json_object* const jsonObjectRemote = formatterRemote.format(src);
	if (0 != jsonObjectLocal) {
		json_object_object_add(jsonObject, KEY_SERVER_REMOTE, jsonObjectRemote);
	}
	*dest = json_object_to_json_string(jsonObject);
	json_object_put(jsonObject);	
	return true;
}

bool JsonServerLoginInfo::formatPassword(struct json_object* jsonObject, const AppRemoteCfgLoginInfo& src) {
	const char* const strPassword = src.getPassword();
	if (0 != strPassword) {
		json_object_object_add(jsonObject, KEY_SERVER_PASSWORD, json_object_new_string((LPCSTR)strPassword));
	}
	return true;
}

HttpRCfgResult_t JsonServerLoginInfo::parse(AppRemoteCfgLoginInfo* dest, const std::string& str) {
	return helper_parse(str, dest);
}

bool JsonServerLoginInfo::parseType(char* key, struct json_object *val, void* pvData) {
	CString str;
	static const char* const myKey = KEY_TYPE;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}

	AppRemoteCfgLoginInfo::LoginMethodType_t method = AppRemoteCfgLoginInfo::LOGIN_METHOD_OFFLINE;
	if (0 == strcmp(str, VALUE_TYPE_USB)) {
		method = AppRemoteCfgLoginInfo::LOGIN_METHOD_USB;
	}
	else if (0 == strcmp(str, VALUE_TYPE_TCP)) {
		method = AppRemoteCfgLoginInfo::LOGIN_METHOD_TCPIP;
	}
	AppRemoteCfgLoginInfo* dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setMethod(method);
	return true;
}

bool JsonServerLoginInfo::parseLocal(char* key, struct json_object *val, void* pvData) {
	CString str;
	static const char* const myKey = KEY_SERVER_LOCAL;

	if (0 != strcmp(myKey,key)) {
		return false;
	}
	if (json_type_object != json_object_get_type(val)) {
		ASSERT(json_type_object != json_object_get_type(val));
		return false;
	}
	AppRemoteCfgLoginInfo* const dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	JsonServerAddressLocal parser;
	if (RESULT_SUCCESS != parser.parse(dest, val)) {
		ASSERT(0);
		return false;
	}
	return true;
}

bool JsonServerLoginInfo::parseRemote(char* key, struct json_object *val, void* pvData) {
	CString str;
	static const char* const myKey = KEY_SERVER_REMOTE;
	if (0 != strcmp(myKey,key)) {
		return false;
	}
	if (json_type_object != json_object_get_type(val)) {
		ASSERT(json_type_object != json_object_get_type(val));
		return false;
	}
	AppRemoteCfgLoginInfo* const dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	JsonServerAddressRemote parser;
	if (RESULT_SUCCESS != parser.parse(dest, val)) {
		ASSERT(0);
		return false;
	}
	return true;
}

bool JsonServerLoginInfo::parsePassword(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_SERVER_PASSWORD, &AppRemoteCfgLoginInfo::setPassword);	
}

bool JsonServerLoginInfo::parseConnect(char* key, struct json_object *val, void* pvData) {
	CString str;
	static const char* const myKey = KEY_SERVER_CONNECT;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}

	AppRemoteCfgHostAddress_t addressType = HOSTADDRESS_TYPE_LOCAL;
	if (0 == strcmp(str, VALUE_CONNECT_LOCAL)) {
		addressType = HOSTADDRESS_TYPE_LOCAL;
	}
	else if (0 == strcmp(str, VALUE_CONNECT_REMOTE)) {
		addressType = HOSTADDRESS_TYPE_REMOTE;
	}
	AppRemoteCfgLoginInfo* dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setPreferedLogin(addressType);
	return true;
}

bool JsonServerLoginInfo::parsePhysicalAddress(char* key, struct json_object *val, void* pvData) {

	static const char* const myKey = KEY_SERVER_PHYSICALADDRESS;
	int value = -1;
	if (!helper_getValueHexInt(key,val,myKey,&value)) {
		return false;
	}	
	AppRemoteCfgLoginInfo* dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setPhysicalAddress(value);
	return true;
}
	
bool JsonServerLoginInfo::parseLogicalAddress(char* key, struct json_object *val, void* pvData) {

	static const char* const myKey = KEY_SERVER_LOGICALADDRESS;
	int value = -1;
	if (!helper_getValueHexInt(key,val,myKey,&value)) {
		return false;
	}	
	AppRemoteCfgLoginInfo* dest = reinterpret_cast<AppRemoteCfgLoginInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setNodeAddress(value);
	return true;

}
/**************************************************************/

#if(0)
static const char* createRandomString(char* strOut,int size) {      
	static const char* const charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	const int lengthcharSet = strlen(charset);
	srand(static_cast<unsigned int>(time(NULL)));
    for(int i=0; i<size; ++i) {		   	
        strOut[i] = charset[rand() % lengthcharSet];
	}    
	return strOut;
}
#endif

/**************************************************************/
/**************************************************************/
/**************************************************************/
JsonServerLoginInfoExport::JsonServerLoginInfoExport() { }
JsonServerLoginInfoExport::~JsonServerLoginInfoExport() { }

bool JsonServerLoginInfoExport::formatPassword(struct json_object* jsonObject, const AppRemoteCfgLoginInfo& src) {	
	const char* const strPassword = src.getPassword();
	if (0 == strPassword) {
		return false;
	}
	
	const int length = strlen(strPassword);
	if (length<=0) {
		return false;
	}

	static const char* const secretKey = "Configurator";

	std::string strTemp = AuthHelper::encode(strPassword,secretKey);
	if (strTemp.empty()) {
		return false;
	}
	json_object_object_add(jsonObject, KEY_SERVER_PASSWORD, json_object_new_string(strTemp.c_str()));
	return true;		
}
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonServerAddress::KEY_SERVER_HOSTADDRESS = "address";
const char* const JsonServerAddress::KEY_SERVER_PORT = "port";
/**************************************************************/
JsonServerAddress::JsonServerAddress() { }
JsonServerAddress::~JsonServerAddress() { }

HttpRCfgResult_t JsonServerAddress::parse(AppRemoteCfgLoginInfo* dest, struct json_object *val) {
	return helper_parse(val, dest);
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
JsonServerAddressLocal::JsonServerAddressLocal() { 
	m_arrParser.SetSize(2);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerAddressLocal::parseHostPort));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerAddressLocal::parseHostName));
}
JsonServerAddressLocal::~JsonServerAddressLocal() { }

bool JsonServerAddressLocal::parseHostName(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_SERVER_HOSTADDRESS, &AppRemoteCfgLoginInfo::setLocalHostAddress);
}
bool JsonServerAddressLocal::parseHostPort(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_SERVER_PORT, &AppRemoteCfgLoginInfo::setLocalPort);
}

struct json_object* JsonServerAddressLocal::format(const AppRemoteCfgLoginInfo& src) {
	json_object* const jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		return 0;
	}
	const char* const hostName = src.getLocalHostAddress();
	if (0 != hostName) {
		json_object_object_add(jsonObject, KEY_SERVER_HOSTADDRESS, json_object_new_string(hostName));
	}
	json_object_object_add(jsonObject, KEY_SERVER_PORT, json_object_new_int(src.getLocalPort()));
	return jsonObject;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
JsonServerAddressRemote::JsonServerAddressRemote() { 
	m_arrParser.SetSize(2);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerAddressRemote::parseHostPort));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerAddressRemote::parseHostName));
}
JsonServerAddressRemote::~JsonServerAddressRemote() { }

bool JsonServerAddressRemote::parseHostName(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_SERVER_HOSTADDRESS, &AppRemoteCfgLoginInfo::setRemoteHostAddress);
}
bool JsonServerAddressRemote::parseHostPort(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_SERVER_PORT, &AppRemoteCfgLoginInfo::setRemotePort);
}

struct json_object* JsonServerAddressRemote::format(const AppRemoteCfgLoginInfo& src) {
	json_object* const jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		return 0;
	}
	const char* const hostName = src.getRemoteHostAddress();
	if (0 != hostName) {
		json_object_object_add(jsonObject, KEY_SERVER_HOSTADDRESS, json_object_new_string(hostName));
	}
	json_object_object_add(jsonObject, KEY_SERVER_PORT, json_object_new_int(src.getRemotePort()));
	return jsonObject;
}
/**************************************************************/
}; 
/**************************************************************/

