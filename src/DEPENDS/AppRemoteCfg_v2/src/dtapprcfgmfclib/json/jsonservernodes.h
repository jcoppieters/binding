#ifndef INCLUDED_JSONSERVERNODES_H
#define INCLUDED_JSONSERVERNODES_H
/**************************************************************/
#ifndef INCLUDED_JSONBASE_H
	#include "json/jsonbase.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgNodes;
class AppRemoteCfgNode;
class AppRemoteCfgNodeService;
/**************************************************************/

/**************************************************************

{
	"serverNodes" : [ {
		"logicalAddress" : "0xfc", 					// Key !!!!
		"physicalAddress" : "0x1c2345", 					
		"MAC" : "xx:xx:xx:xx:xx:xx",				// Info via de udp tool.
		"localAddress" : "10.0.10.56",
		"remoteAddress" : "dns.org",
		"password": "AD+POP", 						// encrypted
		"services" : [
			{ "type": "dt_tcpip", 																						
			  "localPort": 5001,								
			  "remotePort": 19000,
			},
			{
			  "type" : "dt_http_s",						  
			  "localPort": 8081,								
			  "remotePort": 19001,
			},
			{
			  "type" : "dt_http",						 
			  "localPort": 8080,	
			  "remotePort": -1,						  
			}							
		]								
	},						
	]						
}
**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerNodes : public JsonBaseClass 
{
public:
	JsonServerNodes();
	~JsonServerNodes();

	bool format(std::string* dest, const AppRemoteCfgNodes& src);
	HttpRCfgResult_t parse(AppRemoteCfgNodes* dest, const std::string& str);

private:
	bool parseNodes(char* key, struct json_object *val, void* pvData);

private:	
	static const char* const KEY;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerNode : public JsonBaseClass 
{
public:
	JsonServerNode();
	~JsonServerNode();

	struct json_object* format(const AppRemoteCfgNode& src);
	HttpRCfgResult_t parse(AppRemoteCfgNode* dest, struct json_object *val);

private:
	bool parseNodeAddressLogical(char* key, struct json_object *val, void* pvData);
	bool parseNodeAddressPhysical(char* key, struct json_object *val, void* pvData);
	bool parseNodeAddressMAC(char* key, struct json_object *val, void* pvData);
	bool parseNodeAddressHostLocal(char* key, struct json_object *val, void* pvData);
	bool parseNodeAddressHostRemote(char* key, struct json_object *val, void* pvData);
	bool parseNodePassword(char* key, struct json_object *val, void* pvData);
	bool parseNodeServices(char* key, struct json_object *val, void* pvData);

private:	
	static const char* const KEY_ADDRESS_NODE_LA;
	static const char* const KEY_ADDRESS_NODE_PH;
	static const char* const KEY_ADDRESS_MAC;
	static const char* const KEY_ADDRESS_HOST_LOCAL;
	static const char* const KEY_ADDRESS_HOST_REMOTE;
	static const char* const KEY_PASSWORD;
	static const char* const KEY_SERVICES;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonServerNodeService : public JsonBaseClass 
{
public:
	JsonServerNodeService();
	~JsonServerNodeService();

	struct json_object* format(const AppRemoteCfgNodeService& src);
	HttpRCfgResult_t parse(AppRemoteCfgNodeService* dest, struct json_object *val);

private:
	bool parseServiceType(char* key, struct json_object *val, void* pvData);
	bool parsePortLocal(char* key, struct json_object *val, void* pvData);
	bool parsePortRemote(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_TYPE;
	static const char* const KEY_PORT_LOCAL;
	static const char* const KEY_PORT_REMOTE;
	// Value types...
	static const char* const VALUE_TYPE_TCPIP;
	static const char* const VALUE_TYPE_HTTP;
	static const char* const VALUE_TYPE_HTTP_S;
};
/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/
#endif /* INCLUDED_JSONSERVERNODES_H */
/**************************************************************/