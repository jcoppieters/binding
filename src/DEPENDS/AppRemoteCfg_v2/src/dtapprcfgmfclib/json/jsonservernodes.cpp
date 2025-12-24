#include "stdafx.h"
#include "jsonservernodes.h"

#include "appremotecfg/AppRemoteCfgNodeService.h"
#include "httprcfg/httprcfg_http.h"							//AuthHelper
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

// Deze template functies als friend toevoegen.
template <class TYPE>
static bool helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(const CString&)) {
	CString str;
	if (!JsonBaseClass::helper_getValueString(key, val, myKey, &str)) {
		return false;
	}

	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(str);
	return true;
}

template <class TYPE>
static bool helper_setValueInteger(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(int)) {
	int value = -1;
	if (!JsonBaseClass::helper_getValueInt(key, val, myKey, &value)) {
		return false;
	}
	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}

template <class TYPE>
static bool helper_setValueHexInt(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(int)) {
	int value = -1;
	if (!JsonBaseClass::helper_getValueHexInt(key, val, myKey, &value)) {
		return false;
	}
	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonServerNodes::KEY = "serverNodes";
/**************************************************************/
JsonServerNodes::JsonServerNodes() {
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNodes::parseNodes));
}
JsonServerNodes::~JsonServerNodes() { }

bool JsonServerNodes::format(std::string* dest, const AppRemoteCfgNodes& src) {

	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}

	const int count = src.getCount();
	for (int i=0;i<count;++i) {
		AppRemoteCfgNode* pNode = 0;
		if (src.getNode(i,&pNode)) {
			ASSERT(0 != pNode);
			JsonServerNode jsonFormatter;
			struct json_object* const object = jsonFormatter.format(*pNode);
			if (0 != object) {
				json_object_array_add(array_items, object);
			}					
		}
	}

	struct json_object* jsonObject = json_object_new_object();
	if (0 != jsonObject) {
		json_object_object_add(jsonObject, KEY, array_items);
		std::string temp = json_object_to_json_string(jsonObject);
		ASSERT(temp.length() > 0);
		*dest = temp;
		json_object_put(jsonObject);
	}
	return true;
}

HttpRCfgResult_t JsonServerNodes::parse(AppRemoteCfgNodes* dest, const std::string& str) {
	return helper_parse(str, dest);
}

bool JsonServerNodes::parseNodes(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	AppRemoteCfgNodes* const dest = reinterpret_cast<AppRemoteCfgNodes*>(pvData);
	if (0 == dest) {
		return false;
	}
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonObjectNode = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonObjectNode)) {
			ASSERT(json_type_object != json_object_get_type(jsonObjectNode));
			return false;
		}

		JsonServerNode parser;
		AppRemoteCfgNode* const pNew = new AppRemoteCfgNode();
		ASSERT(0 != pNew);
		if (RESULT_SUCCESS != parser.parse(pNew, jsonObjectNode)) {
			ASSERT(0);
			delete pNew;
			return false;
		}
		dest->addNode(pNew);
	}
	return true;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonServerNode::KEY_ADDRESS_NODE_LA = "logicalAddress";
const char* const JsonServerNode::KEY_ADDRESS_NODE_PH = "physicalAddress";
const char* const JsonServerNode::KEY_ADDRESS_MAC = "MAC";
const char* const JsonServerNode::KEY_ADDRESS_HOST_LOCAL = "localAddress";
const char* const JsonServerNode::KEY_ADDRESS_HOST_REMOTE = "remoteAddress";
const char* const JsonServerNode::KEY_PASSWORD = "password";
const char* const JsonServerNode::KEY_SERVICES = "services";
/**************************************************************/
JsonServerNode::JsonServerNode() {
	m_arrParser.SetSize(7);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNode::parseNodeAddressLogical));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNode::parseNodeAddressPhysical));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNode::parseNodeAddressMAC));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNode::parseNodeAddressHostLocal));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNode::parseNodeAddressHostRemote));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNode::parseNodePassword));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNode::parseNodeServices));
}

JsonServerNode::~JsonServerNode() { }

struct json_object* JsonServerNode::format(const AppRemoteCfgNode& src) {
	json_object* const jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		return 0;
	}
	
	CString strNodeAddress;
	const int nodeAddress = src.getNodeAddress();
	if (nodeAddress<0 || nodeAddress>0xff) {
		json_object_put(jsonObject);		// Object ref count--
		return 0;
	}
	strNodeAddress.Format("0x%02x",nodeAddress);
	json_object_object_add(jsonObject, KEY_ADDRESS_NODE_LA, json_object_new_string((LPCSTR)strNodeAddress));

	CString strPhysical;
	const unsigned long address = src.getPhysicalAddress();
	strPhysical.Format("0x%06x",address);
	json_object_object_add(jsonObject, KEY_ADDRESS_NODE_PH, json_object_new_string((LPCSTR)strPhysical));

	const CString& strMAC = src.getMacAddress();
	if (!strMAC.IsEmpty()) {
		json_object_object_add(jsonObject, KEY_ADDRESS_MAC, json_object_new_string((LPCSTR)strMAC));
	}

	const CString& strAddressLocal = src.getHostnameLocal();
	if (!strAddressLocal.IsEmpty()) {
		json_object_object_add(jsonObject, KEY_ADDRESS_HOST_LOCAL, json_object_new_string((LPCSTR)strAddressLocal));
	}

	const CString& strAddressRemote = src.getHostnameRemote();
	if (!strAddressRemote.IsEmpty()) {
		json_object_object_add(jsonObject, KEY_ADDRESS_HOST_REMOTE, json_object_new_string((LPCSTR)strAddressRemote));
	}

	// Password encoded. !!!
	const CString& pwd = src.getPassword();
	if (!pwd.IsEmpty()) {
		try {
			const char* const strPrivateKey = json_getAuthPrivateKey();
			const std::string strDataEncoded = AuthHelper::encode((LPCSTR)pwd, strPrivateKey);
			json_object_object_add(jsonObject, KEY_PASSWORD, json_object_new_string(strDataEncoded.c_str()));		
		}
		catch(std::runtime_error& /* e */) {	
			// Suppress error: base 64 encode/decode - Deze smijt een exceptie wanner de data geen base64 is...		
		}
	}

	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		json_object_put(jsonObject);		// Object ref count--
		return 0;
	}

	const int count = src.getCount();
	for (int i=0;i<count;++i) {
		AppRemoteCfgNodeService* pService = 0;
		if (src.getService(i,&pService)) {
			ASSERT(0 != pService);
			JsonServerNodeService jsonFormatter;
			struct json_object* const object = jsonFormatter.format(*pService);
			if (0 != object) {
				json_object_array_add(array_items, object);
			}					
		}
	}

	json_object_object_add(jsonObject, KEY_SERVICES, array_items);
	return jsonObject;
}

HttpRCfgResult_t JsonServerNode::parse(AppRemoteCfgNode* dest, struct json_object *val) {
	return helper_parse(val, dest);
}

// protected:
bool JsonServerNode::parseNodeAddressLogical(char* key, struct json_object *val, void* pvData) {
	return helper_setValueHexInt(key, val, pvData, KEY_ADDRESS_NODE_LA, &AppRemoteCfgNode::setNodeAddress);
}
bool JsonServerNode::parseNodeAddressPhysical(char* key, struct json_object *val, void* pvData) {
	return helper_setValueHexInt(key, val, pvData, KEY_ADDRESS_NODE_PH, &AppRemoteCfgNode::setPhysicalAddress);
}
bool JsonServerNode::parseNodeAddressMAC(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_ADDRESS_MAC, &AppRemoteCfgNode::setMacAddress);
}
bool JsonServerNode::parseNodeAddressHostLocal(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_ADDRESS_HOST_LOCAL, &AppRemoteCfgNode::setHostnameLocal);
}
bool JsonServerNode::parseNodeAddressHostRemote(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_ADDRESS_HOST_REMOTE, &AppRemoteCfgNode::setHostnameRemote);
}
bool JsonServerNode::parseNodePassword(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_PASSWORD;
	CString str;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}
	
	// password zetten.
	AppRemoteCfgNode* const dest = reinterpret_cast<AppRemoteCfgNode*>(pvData);
	if (0 == dest) {
		return false;
	}

	// Decoderen...
	bool result = false;
	try {
		const char* const strPrivateKey = json_getAuthPrivateKey();
		const std::string strDataDecoded = AuthHelper::decode((LPCSTR)(str), strPrivateKey);	
		dest->setPassword(strDataDecoded.c_str());
		result = true;
	}
	catch (std::runtime_error& /* e */) {	
		// Suppress error: base 64 encode/decode - Deze smijt een exceptie wanner de data geen base64 is...
		result = false;
	}
	return result;
}

bool JsonServerNode::parseNodeServices(char* key, struct json_object *val, void* pvData) {
	CString str;
	static const char* const myKey = KEY_SERVICES;
	if (0 != strcmp(myKey,key)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	AppRemoteCfgNode* const dest = reinterpret_cast<AppRemoteCfgNode*>(pvData);
	if (0 == dest) {
		return false;
	}
	
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonObjectService = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonObjectService)) {
			ASSERT(json_type_object != json_object_get_type(jsonObjectService));
			return false;
		}

		JsonServerNodeService parser;
		AppRemoteCfgNodeService* const pNewService = new AppRemoteCfgNodeService();
		ASSERT(0 != pNewService);
		if (RESULT_SUCCESS != parser.parse(pNewService, jsonObjectService)) {
			ASSERT(0);
			delete pNewService;
			return false;
		}
		dest->addService(pNewService);
	}
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonServerNodeService::KEY_TYPE = "type";
const char* const JsonServerNodeService::KEY_PORT_LOCAL = "localPort";
const char* const JsonServerNodeService::KEY_PORT_REMOTE = "remotePort";
// value types...
const char* const JsonServerNodeService::VALUE_TYPE_TCPIP = "dt_tcpip";
const char* const JsonServerNodeService::VALUE_TYPE_HTTP = "dt_http";
const char* const JsonServerNodeService::VALUE_TYPE_HTTP_S = "dt_http_s";
/**************************************************************/
JsonServerNodeService::JsonServerNodeService( ) {
	m_arrParser.SetSize(3);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNodeService::parseServiceType));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNodeService::parsePortLocal));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonServerNodeService::parsePortRemote));
}
JsonServerNodeService::~JsonServerNodeService() { }

struct json_object* JsonServerNodeService::format(const AppRemoteCfgNodeService& src) {
	json_object* const jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		return 0;
	}

	const char* strType = 0;
	const AppRemoteCfgService_t type = src.getType();
	if (SERVICE_TCPIP == type) {
		strType = VALUE_TYPE_TCPIP;
	}
	else if (SERVICE_HTTP == type) {
		strType = VALUE_TYPE_HTTP;
	}
	else if (SERVICE_HTTP_S == type) {
		strType = VALUE_TYPE_HTTP_S;
	}
	else {
		ASSERT(0);	
		json_object_put(jsonObject);		// Object ref count--
		return 0;
	}
	if (0 != strType) {
		json_object_object_add(jsonObject, KEY_TYPE, json_object_new_string(strType));
	}
	json_object_object_add(jsonObject, KEY_PORT_LOCAL, json_object_new_int(src.getPortLocal()));
	json_object_object_add(jsonObject, KEY_PORT_REMOTE, json_object_new_int(src.getPortRemote()));
	return jsonObject;
}

HttpRCfgResult_t JsonServerNodeService::parse(AppRemoteCfgNodeService* dest, struct json_object *val) {
	return helper_parse(val, dest);
}

// protected
bool JsonServerNodeService::parseServiceType(char* key, struct json_object *val, void* pvData) {

	CString str;
	static const char* const myKey = KEY_TYPE;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}
	AppRemoteCfgService_t service = SERVICE_UNKNOWN;
	if (0 == strcmp(str, VALUE_TYPE_TCPIP)) {
		service = SERVICE_TCPIP;
	}
	else if (0 == strcmp(str, VALUE_TYPE_HTTP)) {
		service = SERVICE_HTTP;
	}
	else if (0 == strcmp(str, VALUE_TYPE_HTTP_S)) {
		service = SERVICE_HTTP_S;
	}

	AppRemoteCfgNodeService* dest = reinterpret_cast<AppRemoteCfgNodeService*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setType(service);
	return true;
}
bool JsonServerNodeService::parsePortLocal(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_PORT_LOCAL, &AppRemoteCfgNodeService::setPortLocal);
}
bool JsonServerNodeService::parsePortRemote(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_PORT_REMOTE, &AppRemoteCfgNodeService::setPortRemote);
}

/**************************************************************/
}; /* namespace DUOTECNO  */
/**************************************************************/