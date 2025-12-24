#include "stdafx.h"
#include "httprcfg_json.h"
#include "httprcfg_classes.h"

#include "json.h"
/**************************************************************/
#define ASSERT(x)
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

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

/**************************************************************/
void httprcfg_testJsonParser( ) {	
	{
		JsonAppCfgResult jsonParser;
		const HttpRCfgResult_t result = jsonParser.testParser( );
		ASSERT(RESULT_SUCCESS == result);
	}
	{
		JsonAppCfgLogInfo jsonParser;
		const HttpRCfgResult_t result = jsonParser.testParser( );
		ASSERT(RESULT_SUCCESS == result);
	}
	{
		JsonAppBindInfoNodes jsonParser;
		const HttpRCfgResult_t result = jsonParser.testParser( );
		ASSERT(RESULT_SUCCESS == result);
	}
	{
		JsonAppNodeInfo jsonParser;
		const HttpRCfgResult_t result = jsonParser.testParser( );
		ASSERT(RESULT_SUCCESS == result);
	}
	{
		JsonFirmwareVersionInfo jsonParser;
		const HttpRCfgResult_t result = jsonParser.testParser();
		ASSERT(RESULT_SUCCESS == result);
	}
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonAppCfgBindAction::KEY_ACTION = "action";
const char* const JsonAppCfgBindAction::KEY_NODES = "nodes";

const char* const JsonAppCfgBindAction::VALUE_ACTION_WRITE = "write";
const char* const JsonAppCfgBindAction::VALUE_ACTION_READ = "read";
const char* const JsonAppCfgBindAction::VALUE_ACTION_ERASE = "erase";
/**************************************************************/
JsonAppCfgBindAction::JsonAppCfgBindAction( ) { }
JsonAppCfgBindAction::~JsonAppCfgBindAction( ) { }


bool JsonAppCfgBindAction::format(std::string* dest, const AppCfgBindAction& src) {
	const int numberOfNodes = src.getCount();
	if (numberOfNodes<=0) {
		return false;
	}

	struct json_object* jsonObject = json_object_new_object();

	struct json_object* array_nodes = json_object_new_array();
	for (int i=0;i<numberOfNodes;++i) {		
		json_object_array_add(array_nodes, json_object_new_int(src.get(i)));		
	}
	
	const AppCfgBindAction::Action_t action = src.getAction();
	const char* strAction = VALUE_ACTION_WRITE;
	switch(action) {
		case AppCfgBindAction::ERASE:
			strAction = VALUE_ACTION_ERASE;
		break;
		case AppCfgBindAction::READ:
			strAction = VALUE_ACTION_READ;
		break;
		case AppCfgBindAction::WRITE:
			strAction = VALUE_ACTION_WRITE;
		break;
	}
	json_object_object_add(jsonObject, KEY_NODES , array_nodes);
	json_object_object_add(jsonObject, KEY_ACTION, json_object_new_string(strAction));

	std::string temp = json_object_to_json_string(jsonObject);
	ASSERT(temp.length()>0);
	*dest = temp;
	json_object_put(jsonObject);
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonAppCfgResult::KEY_STRING_ERROR = "error";
const char* const JsonAppCfgResult::KEY_STRING_ERROR_DETAIL = "errorMsg";
const char* const JsonAppCfgResult::KEY_STATUS = "status";

const char* const JsonAppCfgResult::VALUE_STATUS_FAILED = "failed";
const char* const JsonAppCfgResult::VALUE_STATUS_OK = "ok";
/**************************************************************/
JsonAppCfgResult::JsonAppCfgResult( ) 
{ 
	m_arrParser.SetSize(3);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppCfgResult::parseStatus));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppCfgResult::parseError));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppCfgResult::parseErrorDetail));
}

HttpRCfgResult_t JsonAppCfgResult::parse(AppRCfgResult* dest,const std::string& str) {
	return helper_parse(str,dest);
}

HttpRCfgResult_t JsonAppCfgResult::testParser( ) {
	std::string strData( 
		"{ \"status\" : \"failed\"," 
	    "\"error\" : \"test error message\" ,"
		"\"errorMsg\" : \"extended test error message\" }"
		);

	AppRCfgResult dest;
	return parse(&dest,strData);
}

bool JsonAppCfgResult::parseStatus(char* key,struct json_object *val,void* pvData) {	
	if (0 != strcmp(key,KEY_STATUS)) {
		return false;
	}
	AppRCfgResult* dest = reinterpret_cast<AppRCfgResult*>(pvData);
	if (0 == dest) {
		return false;
	}
	const char* strStatus = json_object_get_string(val);
	if (0 == strcmp(VALUE_STATUS_FAILED,strStatus)) {
		dest->setResultCode(AppRCfgResult::RESULT_ERROR);
	}
	else if (0 == strcmp(VALUE_STATUS_OK,strStatus)) {
		dest->setResultCode(AppRCfgResult::RESULT_SUCCESS);
	}		
	return true;
}

bool JsonAppCfgResult::parseError(char* key,struct json_object *val,void* pvData) {
	if (0 != strcmp(key,KEY_STRING_ERROR)) {
		return false;
	}
	AppRCfgResult* dest = reinterpret_cast<AppRCfgResult*>(pvData);
	if (0 == dest) {
		return false;
	}	
	dest->setErrorMsg(json_object_get_string(val));
	return true;
}

bool JsonAppCfgResult::parseErrorDetail(char* key,struct json_object *val,void* pvData) {
if (0 != strcmp(key,KEY_STRING_ERROR_DETAIL)) {
		return false;
	}
	AppRCfgResult* dest = reinterpret_cast<AppRCfgResult*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setErrorDetailMsg(json_object_get_string(val));
	return true;
}
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonAppCfgLogInfo::KEY_MESSAGES = "messages";
/**************************************************************/
JsonAppCfgLogInfo::JsonAppCfgLogInfo( ) { 
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppCfgLogInfo::parseMessages));
} 

HttpRCfgResult_t JsonAppCfgLogInfo::parse(AppCfgLogInfo* dest,const std::string& str) {
	return helper_parse(str,dest);
}

HttpRCfgResult_t JsonAppCfgLogInfo::testParser( ) {
	std::string strData( 
		"{\"messages\": [\"node 0xfc:Write binding 26/27\","
			"\"node 0xfc:Write binding 27/27\","
			"\"node 0xfc:Binding file close\","
			"\"node 0xfc:Binding file closed\","
			"\"node 0x20:Binding file erase\","
			"\"node 0x20:Binding file erase error, error='Error_response_timeout'\","
			"\"node 0x20:Binding file erase error, error='Error_response_timeout'\","
			"\"node 0x20:Binding file erase error, error='Error_response_timeout'\","
			"\"Write bindings failed by error\" ] }");

	AppCfgLogInfo dest;
	return parse(&dest,strData);
}

bool JsonAppCfgLogInfo::parseMessages(char* key,struct json_object *val,void* pvData) {
	if (0 != strcmp(key,KEY_MESSAGES)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	AppCfgLogInfo* dest = reinterpret_cast<AppCfgLogInfo*>(pvData);
	if (0 == dest) {
		return false;
	}

	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonMsg = json_object_array_get_idx(val, i);
		if (json_type_string != json_object_get_type(jsonMsg)) {
			ASSERT(json_type_string != json_object_get_type(jsonMsg));
			return false;
		}
		dest->add(json_object_get_string(jsonMsg));
	}
	return true;
}

/**************************************************************/
static BindingFileFsmStatus_t fromString(const char* strValue) {
	BindingFileFsmStatus_t retValue = BINDINGFILE_FSM_STATUS_IDLE;
	if (0 == strcmp(strValue,"idle")) {
		retValue = BINDINGFILE_FSM_STATUS_IDLE;
	}
	else if (0 == strcmp(strValue,"busy")) {
		retValue = BINDINGFILE_FSM_STATUS_BUSY;
	}
	else if (0 == strcmp(strValue,"done")) {
		retValue = BINDINGFILE_FSM_STATUS_DONE;
	}
	else if (0 == strcmp(strValue,"cancelled")) {
		retValue = BINDINGFILE_FSM_STATUS_CANCELLED;
	}
	else if (0 == strcmp(strValue,"error")) {
		retValue = BINDINGFILE_FSM_STATUS_ERROR;
	}
	else {
		ASSERT(0);
	}
	return retValue;   
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonAppBindInfoNodes::KEY_NODES = "nodes";
const char* const JsonAppBindInfoNodes::KEY_STATUS = "status";
const char* const JsonAppBindInfoNodes::KEY_NODES_CURRENT = "nodesCurrent";
const char* const JsonAppBindInfoNodes::KEY_NODES_MAX = "nodesMax";
/**************************************************************/
JsonAppBindInfoNodes::JsonAppBindInfoNodes( ) { 
	m_arrParser.SetSize(4);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNodes::parseNodes));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNodes::parseStatus));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNodes::parseNodesCurrent));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNodes::parseNodesMax));
}

HttpRCfgResult_t JsonAppBindInfoNodes::parse(AppCfgBindInfoNodes* dest,const std::string& str) {
	return helper_parse(str,dest);
}

HttpRCfgResult_t JsonAppBindInfoNodes::testParser( ) {
	std::string strData("{\"nodes\": [{ \"entriesDone\": 10, \"entriesMax\": 10, \"fileChecksum\" : 124,"
				"\"fileEntries\": 10, \"nodeAddress\": 3, \"status\": \"done\" } ],"
				"\"nodesCurrent\": 6, \"nodesMax\": 7, \"status\": \"done\"}");

	AppCfgBindInfoNodes dest;
	return parse(&dest,strData);	
}

bool JsonAppBindInfoNodes::parseNodes(char* key,struct json_object *val,void* pvData) {
	if (0 != strcmp(key,KEY_NODES)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	AppCfgBindInfoNodes* dest = reinterpret_cast<AppCfgBindInfoNodes*>(pvData);
	if (0 == dest) {
		return false;
	}

	JsonAppBindInfoNode jsonAppBindInfoNodeParser;
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonNodeInfo = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonNodeInfo)) {
			ASSERT(json_type_object != json_object_get_type(jsonNodeInfo));
			return false;
		}

		AppCfgBindInfoNode* const pNewNode = new AppCfgBindInfoNode( );
		ASSERT(0 != pNewNode);
		if (RESULT_SUCCESS != jsonAppBindInfoNodeParser.parse(pNewNode,jsonNodeInfo)) {
			ASSERT(0);
			delete pNewNode;
			return false;
		}
		dest->addNode(pNewNode);
	}
	return true;
}

bool JsonAppBindInfoNodes::parseStatus(char* key,struct json_object *val,void* pvData) {
	if (0 != strcmp(key,KEY_STATUS)) {
		return false;
	}
	if (json_type_string!= json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}
	AppCfgBindInfoNodes* dest = reinterpret_cast<AppCfgBindInfoNodes*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setStatus(fromString(json_object_get_string(val)));	
	return true;
}

bool JsonAppBindInfoNodes::parseNodesCurrent(char* key,struct json_object *val,void* pvData) {
	return helper_setValueInt(key,val,pvData,KEY_NODES_CURRENT,&AppCfgBindInfoNodes::setNodesProgressCurrent);
}

bool JsonAppBindInfoNodes::parseNodesMax(char* key,struct json_object *val,void* pvData) {
	return helper_setValueInt(key,val,pvData,KEY_NODES_MAX,&AppCfgBindInfoNodes::setNodesProgressMax);
}

bool JsonAppBindInfoNodes::helper_setValueInt(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgBindInfoNodes::*pf)(int)) {
	int value = -1;
	if (!helper_getValueInt(key,val,myKey,&value)) {
		return false;
	}	
	AppCfgBindInfoNodes* dest = reinterpret_cast<AppCfgBindInfoNodes*>(pvData);
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
const char* const JsonAppBindInfoNode::KEY_STATUS = "status";
const char* const JsonAppBindInfoNode::KEY_NODES_BINDINGFILE_ENTRIES = "fileEntries";
const char* const JsonAppBindInfoNode::KEY_NODES_BINDINGFILE_CHECKSUM = "fileChecksum";
const char* const JsonAppBindInfoNode::KEY_NODES_ENTRIES_DONE = "entriesDone";
const char* const JsonAppBindInfoNode::KEY_NODES_ENTRIES_MAX = "entriesMax";
const char* const JsonAppBindInfoNode::KEY_NODES_NODEADDRESS = "nodeAddress";
/**************************************************************/
JsonAppBindInfoNode::JsonAppBindInfoNode( ) { 

	m_arrParser.SetSize(6);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNode::parseStatus));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNode::parseFileEntries));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNode::parseFileChecksum));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNode::parseEntriesDone));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNode::parseEntriesMax));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppBindInfoNode::parseNodeAddress));	
}

HttpRCfgResult_t JsonAppBindInfoNode::parse(AppCfgBindInfoNode* dest,const std::string& str) {
	return helper_parse(str,dest);
}

HttpRCfgResult_t JsonAppBindInfoNode::parse(AppCfgBindInfoNode* dest,struct json_object *val) {
	return helper_parse(val,dest);
}

bool JsonAppBindInfoNode::parseStatus(char* key,struct json_object *val,void* pvData) {
	if (0 != strcmp(key,KEY_STATUS)) {
		return false;
	}
	if (json_type_string!= json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}
	AppCfgBindInfoNode* dest = reinterpret_cast<AppCfgBindInfoNode*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setStatus(fromString(json_object_get_string(val)));	
	return true;
}
bool JsonAppBindInfoNode::parseFileEntries(char* key,struct json_object *val,void* pvData) {
	return helper_setValueInt(key,val,pvData,KEY_NODES_BINDINGFILE_ENTRIES,&AppCfgBindInfoNode::setBindFileNrOfEntries);
}
bool JsonAppBindInfoNode::parseFileChecksum(char* key,struct json_object *val,void* pvData) {
	return helper_setValueInt(key,val,pvData,KEY_NODES_BINDINGFILE_CHECKSUM,&AppCfgBindInfoNode::setBindFileChecksum);
}
bool JsonAppBindInfoNode::parseEntriesDone(char* key,struct json_object *val,void* pvData) {
	return helper_setValueInt(key,val,pvData,KEY_NODES_ENTRIES_DONE,&AppCfgBindInfoNode::setBindingsProgressCurrent);			
}
bool JsonAppBindInfoNode::parseEntriesMax(char* key,struct json_object *val,void* pvData) {
	return helper_setValueInt(key,val,pvData,KEY_NODES_ENTRIES_MAX,&AppCfgBindInfoNode::setBindingsProgressMax);	
}
bool JsonAppBindInfoNode::parseNodeAddress(char* key,struct json_object *val,void* pvData) {
	return helper_setValueInt(key,val,pvData,KEY_NODES_NODEADDRESS,&AppCfgBindInfoNode::setNodeAddress);
}

bool JsonAppBindInfoNode::helper_setValueInt(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgBindInfoNode::*pf)(int)) {
	int value = -1;
	if (!helper_getValueInt(key,val,myKey,&value)) {
		return false;
	}	
	AppCfgBindInfoNode* dest = reinterpret_cast<AppCfgBindInfoNode*>(pvData);
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
const char* const JsonAppNodeInfo::KEY_LOGICALADDRESS = "logicalAddress";
const char* const JsonAppNodeInfo::KEY_MASTERADDRESS = "masterAddress";
const char* const JsonAppNodeInfo::KEY_NAME = "name";
const char* const JsonAppNodeInfo::KEY_NUMBEROFUNITS = "numberOfUnits";
const char* const JsonAppNodeInfo::KEY_PHYSICALADDRESS = "physicalAddress";
const char* const JsonAppNodeInfo::KEY_PRODUCTNAME = "productName";
const char* const JsonAppNodeInfo::KEY_SWVERSION = "softwareVersion";
const char* const JsonAppNodeInfo::KEY_SWVERSION_NIC = "softwareVersionNic";
const char* const JsonAppNodeInfo::KEY_HARDWARE = "hardware";
const char* const JsonAppNodeInfo::KEY_QTVERSION = "qtVersion";
/**************************************************************/
JsonAppNodeInfo::JsonAppNodeInfo( ) { 
	m_arrParser.SetSize(10);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseLogicalAddress));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseMasterAddress));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseNumberOfUnits));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parsePhysicalAddress));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseProductName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseSoftwareVersion));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseSoftwareNicVersion));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseHardware));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseQtVersion));
}

HttpRCfgResult_t JsonAppNodeInfo::parse(AppCfgNodeInfo* dest,const std::string& str) {
	return helper_parse(str,dest);
}

HttpRCfgResult_t JsonAppNodeInfo::testParser( ) {

	std::string strData("{\"logicalAddress\": \"0x08\","
						"\"masterAddress\": \"0xfc\","
						"\"name\": \"tcp/ip rpi emb\","
						"\"numberOfUnits\": \"52\","
						"\"physicalAddress\": \"0x1c5461\","
						"\"productName\": \"TCP/IP homeserver\","
						"\"softwareVersion\": \"V66.05.14\","
						"\"softwareVersionNic\": \"V90.23.10\"}");

	AppCfgNodeInfo dest;
	return parse(&dest,strData);
}

bool JsonAppNodeInfo::parseLogicalAddress(char* key,struct json_object *val,void* pvData) {
	return helper_setValueHexInt(key,val,pvData,KEY_LOGICALADDRESS,&AppCfgNodeInfo::setLogicalNodeAddress);
}

bool JsonAppNodeInfo::parseMasterAddress(char* key,struct json_object *val,void* pvData) {
	return helper_setValueHexInt(key,val,pvData,KEY_MASTERADDRESS,&AppCfgNodeInfo::setMasterAddress);
}

bool JsonAppNodeInfo::parseName(char* key,struct json_object *val,void* pvData)  {
	return helper_setValueString(key,val,pvData,KEY_NAME,&AppCfgNodeInfo::setName);
}

bool JsonAppNodeInfo::parseNumberOfUnits(char* key,struct json_object *val,void* pvData)  {
	int numberOfUnits = -1;
	if (!helper_getValueInt(key,val,KEY_NUMBEROFUNITS,&numberOfUnits)) {
		return false;
	}
	AppCfgNodeInfo* dest = reinterpret_cast<AppCfgNodeInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setNumberOfUnits(numberOfUnits);
	return true;
}

bool JsonAppNodeInfo::parsePhysicalAddress(char* key,struct json_object *val,void* pvData)  {
	return helper_setValueHexInt(key,val,pvData,KEY_PHYSICALADDRESS,&AppCfgNodeInfo::setPhysicalNodeAddress);
}

bool JsonAppNodeInfo::parseProductName(char* key,struct json_object *val,void* pvData)  {
	return helper_setValueString(key,val,pvData,KEY_PRODUCTNAME,&AppCfgNodeInfo::setProductName);
}
	
bool JsonAppNodeInfo::parseSoftwareVersion(char* key,struct json_object *val,void* pvData)  {
	return helper_setValueString(key,val,pvData,KEY_SWVERSION,&AppCfgNodeInfo::setSoftwareVersion);
}

bool JsonAppNodeInfo::parseSoftwareNicVersion(char* key,struct json_object *val,void* pvData)  {
	return helper_setValueString(key,val,pvData,KEY_SWVERSION_NIC,&AppCfgNodeInfo::setNicSoftwareVersion);
}

bool JsonAppNodeInfo::parseHardware(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_HARDWARE, &AppCfgNodeInfo::setHardware);
}

bool JsonAppNodeInfo::parseQtVersion(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_QTVERSION, &AppCfgNodeInfo::setQtVersion);
}

bool JsonAppNodeInfo::helper_setValueHexInt(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgNodeInfo::*pf)(int)) {
	int value = -1;
	if (!helper_getValueHexInt(key,val,myKey,&value)) {
		return false;
	}	
	AppCfgNodeInfo* dest = reinterpret_cast<AppCfgNodeInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}

bool JsonAppNodeInfo::helper_setValueString(char* key,struct json_object *val,void* pvData,const char* myKey,void (AppCfgNodeInfo::*pf)(const CString& )) {
	CString str;
	if (!helper_getValueString(key,val,myKey,&str)) {
		return false;
	}	
	AppCfgNodeInfo* dest = reinterpret_cast<AppCfgNodeInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(str);
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonCfgFileServerFile::KEY_TYPE_DIR = "dir";
const char* const JsonCfgFileServerFile::KEY_TYPE_FILE = "file";
const char* const JsonCfgFileServerFile::KEY_NAME = "name";
const char* const JsonCfgFileServerFile::KEY_SIZE = "size";
const char* const JsonCfgFileServerFile::KEY_TIME = "time";
/**************************************************************/
JsonCfgFileServerFile::JsonCfgFileServerFile( ) { 

	m_arrParser.SetSize(4);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgFileServerFile::parseTypeDir));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgFileServerFile::parseTypeFile));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgFileServerFile::parseName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgFileServerFile::parseSize));
}
/*
HttpRCfgResult_t JsonCfgFileServerFile::parse(AppCfgFileServerFile* dest,const std::string& str) {
	return helper_parse(str,dest);
}
*/
HttpRCfgResult_t JsonCfgFileServerFile::parse(AppCfgFileServerFile* dest,struct json_object *val) {
	return helper_parse(val,dest);
}
bool JsonCfgFileServerFile::parseTypeDir(char* key,struct json_object *val,void* pvData) {
	bool value = false;
	if (!helper_getValueBoolean(key,val,KEY_TYPE_DIR,&value)) {
		return false;
	}	
	if (value == true) {
		AppCfgFileServerFile* dest = reinterpret_cast<AppCfgFileServerFile*>(pvData);
		if (0 == dest) {
			return false;
		}
		dest->setType(AppCfgFileServerFile::TYPE_DIR);
	}
	return true;
}
bool JsonCfgFileServerFile::parseTypeFile(char* key,struct json_object *val,void* pvData) {
	bool value = false;
	if (!helper_getValueBoolean(key,val,KEY_TYPE_FILE,&value)) {
		return false;
	}	
	if (value == true) {
		AppCfgFileServerFile* dest = reinterpret_cast<AppCfgFileServerFile*>(pvData);
		if (0 == dest) {
			return false;
		}
		dest->setType(AppCfgFileServerFile::TYPE_FILE);
	}
	return true;
}
bool JsonCfgFileServerFile::parseName(char* key,struct json_object *val,void* pvData) {
	CString str;
	if (!helper_getValueString(key,val,KEY_NAME,&str)) {
		return false;
	}	
	AppCfgFileServerFile* dest = reinterpret_cast<AppCfgFileServerFile*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setFileName(str);
	return true;
}

bool JsonCfgFileServerFile::parseSize(char* key,struct json_object *val,void* pvData) {
	int value = 0;
	if (!helper_getValueInt(key,val,KEY_SIZE,&value)) {
		return false;
	}
	AppCfgFileServerFile* dest = reinterpret_cast<AppCfgFileServerFile*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setSize(value);
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonCfgFileServerDir::KEY_DIR = "dir";
/**************************************************************/
JsonCfgFileServerDir::JsonCfgFileServerDir( ) { 
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgFileServerDir::parseArrayDir));
}

HttpRCfgResult_t JsonCfgFileServerDir::parse(AppCfgFileServerDir* dest,const std::string& str) {
	return helper_parse(str,dest);
}

bool JsonCfgFileServerDir::parseArrayDir(char* key,struct json_object *val,void* pvData) {
	if (0 != strcmp(key,KEY_DIR)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	AppCfgFileServerDir* dest = reinterpret_cast<AppCfgFileServerDir*>(pvData);
	if (0 == dest) {
		return false;
	}

	JsonCfgFileServerFile jsonCfgFileServerFileParser;
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonFileInfo = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonFileInfo)) {
			ASSERT(json_type_object != json_object_get_type(jsonFileInfo));
			return false;
		}

		AppCfgFileServerFile* const pNewItem = new AppCfgFileServerFile( );
		ASSERT(0 != pNewItem);
		if (RESULT_SUCCESS != jsonCfgFileServerFileParser.parse(pNewItem,jsonFileInfo)) {
			ASSERT(0);
			return false;
		}
		dest->addFile(pNewItem);
	}
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonCfgClientCredentials::KEY_CLIENTID = "client_id";
const char* const JsonCfgClientCredentials::KEY_ACCESS_TOKEN = "access_token";
const char* const JsonCfgClientCredentials::KEY_REFRESH_TOKEN = "refresh_token";
const char* const JsonCfgClientCredentials::KEY_EXPIRYTIME = "expiry_time";
const char* const JsonCfgClientCredentials::KEY_TOKENTYPE = "token_type";
/**************************************************************/
JsonCfgClientCredentials::JsonCfgClientCredentials() { 
	m_arrParser.SetSize(5);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgClientCredentials::parseClientID));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgClientCredentials::parseAccessToken));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgClientCredentials::parseRefreshToken));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgClientCredentials::parseExpiryTime));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonCfgClientCredentials::parseTokenType));
}

HttpRCfgResult_t JsonCfgClientCredentials::parse(AppCfgClientCredentials* dest, const std::string& str) {
	return helper_parse(str, dest);
}

bool JsonCfgClientCredentials::format(std::string* dest, const AppCfgClientCredentials& src) {
	json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		return false;
	}

	json_object_object_add(jsonObject, KEY_CLIENTID, json_object_new_string((LPCSTR)src.getClientID()));
	json_object_object_add(jsonObject, KEY_ACCESS_TOKEN, json_object_new_string((LPCSTR)src.getAccessToken()));
	json_object_object_add(jsonObject, KEY_REFRESH_TOKEN, json_object_new_string((LPCSTR)src.getRefreshToken()));
	json_object_object_add(jsonObject, KEY_TOKENTYPE, json_object_new_string((LPCSTR)src.getType()));
	json_object_object_add(jsonObject, KEY_EXPIRYTIME, json_object_new_int(src.getExpiryTimeSec()));

	std::string temp = json_object_to_json_string(jsonObject);
	ASSERT(temp.length()>0);
	*dest = temp;
	while (1 != json_object_put(jsonObject));
	return true;
}

bool JsonCfgClientCredentials::parseClientID(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_CLIENTID, &AppCfgClientCredentials::setClientID);
}
bool JsonCfgClientCredentials::parseAccessToken(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_ACCESS_TOKEN, &AppCfgClientCredentials::setAccessToken);
}
bool JsonCfgClientCredentials::parseRefreshToken(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_REFRESH_TOKEN, &AppCfgClientCredentials::setRefreshToken);
}
bool JsonCfgClientCredentials::parseExpiryTime(char* key, struct json_object *val, void* pvData) {
	int value = 0;
	if (!helper_getValueInt(key, val, KEY_EXPIRYTIME, &value)) {
		return false;
	}
	AppCfgClientCredentials* dest = reinterpret_cast<AppCfgClientCredentials*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setExpiryTimeSec(value);
	return true;
}
bool JsonCfgClientCredentials::parseTokenType(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_TOKENTYPE, &AppCfgClientCredentials::setType);
}

bool JsonCfgClientCredentials::helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (AppCfgClientCredentials::*pf)(const CString&)) {
	CString str;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}
	AppCfgClientCredentials* dest = reinterpret_cast<AppCfgClientCredentials*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(str);
	return true;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonRCfgActionName::KEY_NAMES_NODEADDRESS = "nodeAddress";
const char* const JsonRCfgActionName::KEY_NAMES_UNITADDRESS = "unitAddress";
const char* const JsonRCfgActionName::KEY_NAMES_NAME = "name";
/**************************************************************/
JsonRCfgActionName::JsonRCfgActionName() { }

struct json_object* JsonRCfgActionName::format(const AppCfgRemoteActionWriteName& src) {
	const int nodeAddress = src.getNodeAddress();
	const int unitAddress = src.getUnitAddress();
	const char* const name = src.getName();
	if (0 == name) {
		return 0;
	}
	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		return 0;
	}
	json_object_object_add(jsonObject, KEY_NAMES_NODEADDRESS, json_object_new_int(nodeAddress));
	if (unitAddress >= 0 && unitAddress <= 255) {
		json_object_object_add(jsonObject, KEY_NAMES_UNITADDRESS, json_object_new_int(unitAddress));
	}
	json_object_object_add(jsonObject, KEY_NAMES_NAME, json_object_new_string(name));
	return jsonObject;
}

bool JsonRCfgActionName::format(std::string* dest, const AppCfgRemoteActionWriteName& src) {	
	struct json_object* jsonObject = format(src);
	if (0 != jsonObject) {
		std::string temp = json_object_to_json_string(jsonObject);
		ASSERT(temp.length() > 0);
		*dest = temp;
		json_object_put(jsonObject);
		return true;
	}
	return false;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonRCfgActionList::KEY_NAMES = "names";
/**************************************************************/
JsonRCfgActionList::JsonRCfgActionList() { }

bool JsonRCfgActionList::format(std::string* dest, const AppCfgRemoteActionList& src) {
	if (src.getCount() <= 0) {
		return false;
	}
	AppCfgRemoteAction* pFirstItem = 0;
	if (!src.get(0, &pFirstItem)) {
		return false;
	}

	if (0 != dynamic_cast<AppCfgRemoteActionWriteName*>(pFirstItem)) {
		return formatActionListNames(dest, src);
	}
	return false;
}

bool JsonRCfgActionList::formatActionListNames(std::string* dest, const AppCfgRemoteActionList& src) {
	const int numberOfItems = src.getCount();
	if (numberOfItems <= 0) {
		return false;
	}
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}
	for (int i = 0; i<numberOfItems; ++i) {
		AppCfgRemoteAction* pItem = 0;
		if (src.get(i, &pItem)) {
			AppCfgRemoteActionWriteName* const pActionWriteName = dynamic_cast<AppCfgRemoteActionWriteName*>(pItem);
			if (0 != pActionWriteName) {
				JsonRCfgActionName jsonFormatter;
				struct json_object* const object = jsonFormatter.format(*pActionWriteName);
				if (0 != object) {
					json_object_array_add(array_items, object);
				}
			}
		}
	}

	struct json_object* jsonObject = json_object_new_object();
	if (0 != jsonObject) {
		json_object_object_add(jsonObject, KEY_NAMES, array_items);
		std::string temp = json_object_to_json_string(jsonObject);
		ASSERT(temp.length() > 0);
		*dest = temp;
		json_object_put(jsonObject);
	}
	return true;
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* const JsonRCfgCmnProgressInfo::KEY_PROGRESS_STATUS = "status";

const char* const JsonRCfgCmnProgressInfo::VALUE_PROGRESS_STATE_IDLE = "idle";
const char* const JsonRCfgCmnProgressInfo::VALUE_PROGRESS_STATE_BUSY = "busy";
const char* const JsonRCfgCmnProgressInfo::VALUE_PROGRESS_STATE_DONE_SUCCESS = "done";
const char* const JsonRCfgCmnProgressInfo::VALUE_PROGRESS_STATE_DONE_ERROR = "failed";
const char* const JsonRCfgCmnProgressInfo::VALUE_PROGRESS_STATE_DONE_CANCELLED = "cancelled";
/*************************************************************/
JsonRCfgCmnProgressInfo::JsonRCfgCmnProgressInfo() { 
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonRCfgCmnProgressInfo::parseStatus));
}

HttpRCfgResult_t JsonRCfgCmnProgressInfo::parse(AppCfgCmnProgressInfo* dest, const std::string& str) {
	return helper_parse(str, dest);
}

bool JsonRCfgCmnProgressInfo::parseStatus(char* key, struct json_object *val, void* pvData) {
	CString strValue;
	if (!helper_getValueString(key, val, KEY_PROGRESS_STATUS, &strValue)) {
		return false;
	}
	AppCfgCmnProgressInfo* dest = reinterpret_cast<AppCfgCmnProgressInfo*>(pvData);
	if (0 == dest) {
		return false;
	}

	AppCfgCmnProgressInfo::State_t state = AppCfgCmnProgressInfo::STATE_IDLE;
	if (0 == strValue.Compare(VALUE_PROGRESS_STATE_IDLE)) {
		state = AppCfgCmnProgressInfo::STATE_IDLE;	
	}
	else if (0 == strValue.Compare(VALUE_PROGRESS_STATE_BUSY)) {
		state = AppCfgCmnProgressInfo::STATE_BUSY;
	}
	else if (0 == strValue.Compare(VALUE_PROGRESS_STATE_DONE_SUCCESS)) {
		state = AppCfgCmnProgressInfo::STATE_DONE_SUCCESS;
	}
	else if (0 == strValue.Compare(VALUE_PROGRESS_STATE_DONE_ERROR)) {
		state = AppCfgCmnProgressInfo::STATE_DONE_ERROR;
	}
	else if (0 == strValue.Compare(VALUE_PROGRESS_STATE_DONE_CANCELLED)) {
		state = AppCfgCmnProgressInfo::STATE_DONE_CANCELLED;
	}
	dest->setState(state);
	return true;	
}
/**************************************************************/

/**************************************************************/
const char* const JsonFirmwareVersionInfo::KEY_VERSIONS = "versions";
/**************************************************************/
JsonFirmwareVersionInfo::JsonFirmwareVersionInfo() {
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonFirmwareVersionInfo::parseVersions));
}
JsonFirmwareVersionInfo::~JsonFirmwareVersionInfo() { }
/**************************************************************/
HttpRCfgResult_t JsonFirmwareVersionInfo::parse(AppRemoteCfgFirmwareVersionInfo* dest, const std::string& str) {
	return helper_parse(str, dest);
}

HttpRCfgResult_t JsonFirmwareVersionInfo::testParser()
{
	std::string strData("{\"versions\" : [ {"
		"\"softwareVersion\": \"V66.08.04\","
		"\"release\" : true,"
		"\"productName\" : \"TCP/IP homeserver\","
		"\"platform\" : \"rpi1\","
		"\"ipk\" : \"http://www.duotecno-cloud.be/data/firmware/rpi1_051508_qtnodegatewayapp_v66.08.04.ipk\","
		"\"releaseNotes\" : \"http://www.duotecno-cloud.be/data/varia/nodegateway_releaseNotes.txt\""
		"},"
		"{"
		"\"softwareVersion\": \"V66.10.00\","
		"\"release\" : false,"
		"\"productName\" : \"TCP/IP homeserver\","
		"\"platform\" : \"rpi1\","
		"\"ipk\" : \"http://www.duotecno-cloud.be/data/firmware/beta/rpi1_051508_qtnodegatewayapp_v66.10.00.ipk\","
		"\"releaseNotes\" : \"http://www.duotecno-cloud.be/data/varia/nodegateway_releaseNotes.txt\""
		"}]}");

	AppRemoteCfgFirmwareVersionInfo dest;
	return parse(&dest, strData);
}

bool JsonFirmwareVersionInfo::parseVersions(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY_VERSIONS)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	AppRemoteCfgFirmwareVersionInfo* const dest = reinterpret_cast<AppRemoteCfgFirmwareVersionInfo*>(pvData);
	if (0 == dest) {
		return false;
	}
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonObjectItem = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonObjectItem)) {
			ASSERT(json_type_object != json_object_get_type(jsonObjectItem));
			return false;
		}

		JsonFirmwareVersion parser;
		AppRemoteCfgFirmwareVersion* const pNew = new AppRemoteCfgFirmwareVersion();
		ASSERT(0 != pNew);
		if (RESULT_SUCCESS != parser.parse(pNew, jsonObjectItem)) {
			ASSERT(0);
			delete pNew;
			return false;
		}
		dest->add(pNew);
	}
	return true;
}


/**************************************************************/
const char* const JsonFirmwareVersion::KEY_VERSION = "softwareVersion";
const char* const JsonFirmwareVersion::KEY_RELEASE = "release";
const char* const JsonFirmwareVersion::KEY_PRODUCTNAME = "productName";
const char* const JsonFirmwareVersion::KEY_PLATFORM = "platform";
const char* const JsonFirmwareVersion::KEY_IPK = "ipk";
const char* const JsonFirmwareVersion::KEY_RELEASENOTES = "releaseNotes";
/**************************************************************/
JsonFirmwareVersion::JsonFirmwareVersion() {
	m_arrParser.SetSize(5);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonFirmwareVersion::parseVersion));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonFirmwareVersion::parseRelease));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonFirmwareVersion::parseProductName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonFirmwareVersion::parsePlatform));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonFirmwareVersion::parseIpk));
}
JsonFirmwareVersion::~JsonFirmwareVersion() { }
/**************************************************************/

HttpRCfgResult_t JsonFirmwareVersion::parse(AppRemoteCfgFirmwareVersion* dest, struct json_object *val) {
	return helper_parse(val, dest);
}
bool JsonFirmwareVersion::parseVersion(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_VERSION, &AppRemoteCfgFirmwareVersion::setVersion);
}
bool JsonFirmwareVersion::parseRelease(char* key, struct json_object *val, void* pvData) {
	bool value = false;
	if (!helper_getValueBoolean(key, val, KEY_RELEASE, &value)) {
		return false;
	}
	AppRemoteCfgFirmwareVersion* dest = reinterpret_cast<AppRemoteCfgFirmwareVersion*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setFlagReleaseVersion(value);
	return true;
}
bool JsonFirmwareVersion::parseProductName(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_PRODUCTNAME, &AppRemoteCfgFirmwareVersion::setProductName);
}
bool JsonFirmwareVersion::parsePlatform(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_PLATFORM, &AppRemoteCfgFirmwareVersion::setPlatform);
}
bool JsonFirmwareVersion::parseIpk(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_IPK, &AppRemoteCfgFirmwareVersion::setIpk);
}

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/