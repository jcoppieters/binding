#ifndef INCLUDED_HTTPRCFG_JSON_H
#define INCLUDED_HTTPRCFG_JSON_H
/**************************************************************/
#ifndef INCLUDED_HTTPRCFG_CLASSES_H
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
/**************************************************************/
/**************************************************************/
class JsonAppCfgBindAction : public JsonBaseClass
{
public:
	JsonAppCfgBindAction( );
	~JsonAppCfgBindAction( );
	bool format(std::string* dest, const AppCfgBindAction& src);

private:
	static const char* const KEY_ACTION;
	static const char* const KEY_NODES;

	static const char* const VALUE_ACTION_WRITE;
	static const char* const VALUE_ACTION_READ;
	static const char* const VALUE_ACTION_ERASE;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonAppCfgResult : public JsonBaseClass
{
public:
	JsonAppCfgResult( );
	HttpRCfgResult_t parse(AppRCfgResult* dest,const std::string& str);
	HttpRCfgResult_t testParser( );

protected:	
	bool parseStatus(char* key,struct json_object *val,void* pvData);
	bool parseError(char* key,struct json_object *val,void* pvData);
	bool parseErrorDetail(char* key,struct json_object *val,void* pvData);

private:
	static const char* const KEY_STRING_ERROR;
	static const char* const KEY_STRING_ERROR_DETAIL;
	static const char* const KEY_STATUS;

	static const char* const VALUE_STATUS_FAILED;
	static const char* const VALUE_STATUS_OK;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonAppCfgLogInfo : public JsonBaseClass
{
public:
	JsonAppCfgLogInfo( );

	HttpRCfgResult_t parse(AppCfgLogInfo* dest,const std::string& str);
	HttpRCfgResult_t testParser( );

protected:
	bool parseMessages(char* key,struct json_object *val,void* pvData);

private:
	static const char* const KEY_MESSAGES;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonAppBindInfoNodes : public JsonBaseClass
{
public:
	JsonAppBindInfoNodes( );

	HttpRCfgResult_t parse(AppCfgBindInfoNodes* dest,const std::string& str);
	HttpRCfgResult_t testParser( );

protected:
	bool parseNodes(char* key,struct json_object *val,void* pvData);
	bool parseStatus(char* key,struct json_object *val,void* pvData);
	bool parseNodesCurrent(char* key,struct json_object *val,void* pvData);
	bool parseNodesMax(char* key,struct json_object *val,void* pvData);
	// helper...
	bool helper_setValueInt(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgBindInfoNodes::*pf)(int));

private:
	static const char* const KEY_NODES;
	static const char* const KEY_STATUS;
	static const char* const KEY_NODES_CURRENT;
	static const char* const KEY_NODES_MAX;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonAppBindInfoNode : public JsonBaseClass
{
public:
	JsonAppBindInfoNode( );

	HttpRCfgResult_t parse(AppCfgBindInfoNode* dest,const std::string& str);
	HttpRCfgResult_t parse(AppCfgBindInfoNode* dest,struct json_object *val);

protected:
	bool parseStatus(char* key,struct json_object *val,void* pvData);
	bool parseFileEntries(char* key,struct json_object *val,void* pvData);
	bool parseFileChecksum(char* key,struct json_object *val,void* pvData);
	bool parseEntriesDone(char* key,struct json_object *val,void* pvData);
	bool parseEntriesMax(char* key,struct json_object *val,void* pvData);
	bool parseNodeAddress(char* key,struct json_object *val,void* pvData);
	// helper...
	bool helper_setValueInt(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgBindInfoNode::*pf)(int));
private:
	static const char* const KEY_STATUS;
	static const char* const KEY_NODES_BINDINGFILE_ENTRIES;
	static const char* const KEY_NODES_BINDINGFILE_CHECKSUM;
	static const char* const KEY_NODES_ENTRIES_DONE;
	static const char* const KEY_NODES_ENTRIES_MAX;
	static const char* const KEY_NODES_NODEADDRESS;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonAppNodeInfo : public JsonBaseClass
{
public:
	JsonAppNodeInfo( );

	HttpRCfgResult_t parse(AppCfgNodeInfo* dest,const std::string& str);
	HttpRCfgResult_t testParser( );

protected:
	bool parseLogicalAddress(char* key,struct json_object *val,void* pvData);
	bool parseMasterAddress(char* key,struct json_object *val,void* pvData);
	bool parseName(char* key,struct json_object *val,void* pvData);
	bool parseNumberOfUnits(char* key,struct json_object *val,void* pvData);
	bool parsePhysicalAddress(char* key,struct json_object *val,void* pvData);
	bool parseProductName(char* key,struct json_object *val,void* pvData);
	bool parseSoftwareVersion(char* key,struct json_object *val,void* pvData);
	bool parseSoftwareNicVersion(char* key,struct json_object *val,void* pvData);
	bool parseHardware(char* key, struct json_object *val, void* pvData);
	bool parseQtVersion(char* key, struct json_object *val, void* pvData);

	bool helper_setValueHexInt(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgNodeInfo::*pf)(int));
	bool helper_setValueString(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgNodeInfo::*pf)(const CString& ));

private:
	static const char* const KEY_LOGICALADDRESS;
	static const char* const KEY_MASTERADDRESS;
	static const char* const KEY_NAME;
	static const char* const KEY_NUMBEROFUNITS;
	static const char* const KEY_PHYSICALADDRESS;
	static const char* const KEY_PRODUCTNAME;
	static const char* const KEY_SWVERSION;
	static const char* const KEY_SWVERSION_NIC;
	static const char* const KEY_HARDWARE;
	static const char* const KEY_QTVERSION;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonCfgFileServerFile : public JsonBaseClass
{
public:
	JsonCfgFileServerFile( );

	// HttpRCfgResult_t parse(AppCfgFileServerFile* dest,const std::string& str);
	HttpRCfgResult_t parse(AppCfgFileServerFile* dest,struct json_object *val);

protected:
	bool parseTypeDir(char* key,struct json_object *val,void* pvData);
	bool parseTypeFile(char* key,struct json_object *val,void* pvData);
	bool parseName(char* key,struct json_object *val,void* pvData);
	bool parseSize(char* key,struct json_object *val,void* pvData);
	// time wordt nog niet gedaan...

private:
	static const char* const KEY_TYPE_DIR;
	static const char* const KEY_TYPE_FILE;
	static const char* const KEY_NAME;
	static const char* const KEY_SIZE;
	static const char* const KEY_TIME;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonCfgFileServerDir : public JsonBaseClass
{
public:
	JsonCfgFileServerDir( );

	HttpRCfgResult_t parse(AppCfgFileServerDir* dest,const std::string& str);

protected:
	bool parseArrayDir(char* key,struct json_object *val,void* pvData);

private:
	static const char* const KEY_DIR;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonCfgClientCredentials : public JsonBaseClass
{
public:
	JsonCfgClientCredentials();

	HttpRCfgResult_t parse(AppCfgClientCredentials* dest, const std::string& str);
	bool format(std::string* dest, const AppCfgClientCredentials& src);

protected:
	bool parseClientID(char* key, struct json_object *val, void* pvData);
	bool parseAccessToken(char* key, struct json_object *val, void* pvData);
	bool parseRefreshToken(char* key, struct json_object *val, void* pvData);
	bool parseExpiryTime(char* key, struct json_object *val, void* pvData);
	bool parseTokenType(char* key, struct json_object *val, void* pvData);

	bool helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppCfgClientCredentials::*pf)(const CString&));

private:
	static const char* const KEY_CLIENTID;
	static const char* const KEY_ACCESS_TOKEN;
	static const char* const KEY_REFRESH_TOKEN;
	static const char* const KEY_EXPIRYTIME;
	static const char* const KEY_TOKENTYPE;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonRCfgActionName : public JsonBaseClass
{
public:
	JsonRCfgActionName();
	bool format(std::string* dest, const AppCfgRemoteActionWriteName& src);
	struct json_object* format(const AppCfgRemoteActionWriteName& src);

private:
	static const char* const KEY_NAMES_NODEADDRESS;
	static const char* const KEY_NAMES_UNITADDRESS;
	static const char* const KEY_NAMES_NAME;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonRCfgActionList : public JsonBaseClass
{
public:
	JsonRCfgActionList();
	bool format(std::string* dest, const AppCfgRemoteActionList& src);

protected:
	bool formatActionListNames(std::string* dest, const AppCfgRemoteActionList& src);

private:
	static const char* const KEY_NAMES;	
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonRCfgCmnProgressInfo : public JsonBaseClass
{
public:
	JsonRCfgCmnProgressInfo();

	HttpRCfgResult_t parse(AppCfgCmnProgressInfo* dest, const std::string& str);

protected:
	bool parseStatus(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_PROGRESS_STATUS;

	static const char* const VALUE_PROGRESS_STATE_IDLE;
	static const char* const VALUE_PROGRESS_STATE_BUSY;
	static const char* const VALUE_PROGRESS_STATE_DONE_SUCCESS;
	static const char* const VALUE_PROGRESS_STATE_DONE_ERROR;
	static const char* const VALUE_PROGRESS_STATE_DONE_CANCELLED;
};
/**************************************************************/

/**************************************************************
{
	"versions" : [
	{
	"softwareVersion": "V66.08.04",
	"release" : true,
	"productName": "TCP/IP homeserver",
	"platform" : "rpi1",
	"ipk" : "http://www.duotecno-cloud.be/data/firmware/rpi1_051508_qtnodegatewayapp_v66.08.04.ipk",
	"releaseNotes" : "http://www.duotecno-cloud.be/data/varia/nodegateway_releaseNotes.txt"
	},
	{
	"softwareVersion": "V66.10.00",
	"release" : false,
	"productName": "TCP/IP homeserver",
	"platform" : "rpi1",
	"ipk" : "http://www.duotecno-cloud.be/data/firmware/beta/rpi1_051508_qtnodegatewayapp_v66.10.00.ipk",
	"releaseNotes" : "http://www.duotecno-cloud.be/data/varia/nodegateway_releaseNotes.txt"
	}
]
**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonFirmwareVersionInfo : public JsonBaseClass
{
public:
	JsonFirmwareVersionInfo();
	virtual ~JsonFirmwareVersionInfo();

	HttpRCfgResult_t parse(AppRemoteCfgFirmwareVersionInfo* dest, const std::string& str);
	HttpRCfgResult_t testParser();

private:
	bool parseVersions(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_VERSIONS;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonFirmwareVersion : public JsonBaseClass
{
public:
	JsonFirmwareVersion();
	virtual ~JsonFirmwareVersion();

	HttpRCfgResult_t parse(AppRemoteCfgFirmwareVersion* dest, struct json_object *val);

	bool parseVersion(char* key, struct json_object *val, void* pvData);
	bool parseRelease(char* key, struct json_object *val, void* pvData);
	bool parseProductName(char* key, struct json_object *val, void* pvData);
	bool parsePlatform(char* key, struct json_object *val, void* pvData);
	bool parseIpk(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_VERSION;
	static const char* const KEY_RELEASE;
	static const char* const KEY_PRODUCTNAME;
	static const char* const KEY_PLATFORM;
	static const char* const KEY_IPK;
	static const char* const KEY_RELEASENOTES;
};

/**************************************************************/
extern void httprcfg_testJsonParser( );
/**************************************************************/
}; /* namespace DUOTECNO*/
/**************************************************************/
#endif 
/**************************************************************/