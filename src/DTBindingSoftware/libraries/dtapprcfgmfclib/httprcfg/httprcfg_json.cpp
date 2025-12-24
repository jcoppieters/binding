#include "stdafx.h"
#include "httprcfg_json.h"
#include "httprcfg_classes.h"

#include "json.h"
/**************************************************************/
#define ASSERT(x)
/**************************************************************/
namespace DUOTECNO {
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
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
JsonBaseClass::JsonBaseClass( ) : m_arrParser( ) { }
JsonBaseClass::~JsonBaseClass( ) { }

HttpRCfgResult_t JsonBaseClass::parse(AppCfgResult* dest,const std::string& str) {
	return RESULT_EC_JSON_NOT_IMPLEMENTED;
};

HttpRCfgResult_t JsonBaseClass::helper_parse(const std::string& str,void* pvData) {
	// TODO: CONTROLE DOEN OF HET FORMAAT JSON IS...
	if (str.empty()) {
		return RESULT_EC_JSON_PARSE_DATA_NULL;
	}
	json_object* jobj = json_tokener_parse(str.c_str()); 
	if (0 == jobj) {
		return RESULT_EC_JSON_PARSE_OBJECT_NULL;
	}
	if (json_type_null == json_object_get_type(jobj)) {
		json_object_put(jobj);
		return RESULT_EC_JSON_PARSE_OBJECT_NULL;
	}

	const HttpRCfgResult_t result = this->helper_parse(jobj,pvData);
	json_object_put(jobj);
	return result;
}

HttpRCfgResult_t JsonBaseClass::helper_parse(struct json_object* jobj,void* pvData) {
	const int length = m_arrParser.GetSize();
	json_object_object_foreach(jobj, key, val) {		
		for (int i=0;i<length;++i) {
			PF_PARSER pf = m_arrParser[i];
			if (0 != pf) {
				if ((this->*pf)(key,val,pvData)) {
					break; // break for loop...
				}
			}
		}
	}
	return RESULT_SUCCESS;
}

// integer of een string integer...
bool JsonBaseClass::helper_getValueInt(char* key,struct json_object *val,const char* myKey,int* value) {
	if (0 != strcmp(key,myKey)) {
		return false;
	}

	const json_type type = json_object_get_type(val);
	if (json_type_int == type) {
		*value = json_object_get_int(val);
		return true;
	}
	else if (json_type_string == type) {
		const char* const s = json_object_get_string(val);
		if (0 == s) {
			return false;
		}
		int temp=-1;
		if (sscanf(s,"%d",&temp)>=1) {
			*value=temp;
			return true;
		}
	}
	else {
		ASSERT(0);	
	}
	return false;	
}

bool JsonBaseClass::helper_getValueHexInt(char* key,struct json_object *val,const char* myKey,int* value) {
	if (0 != strcmp(key,myKey)) {
		return false;
	}
	if (json_type_string!= json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}

	const char* const s = json_object_get_string(val);
	if (0 == s && strlen(s)<3) {
		return false;
	}

	int temp = -1;
	if ((s[0] != '0') && (s[1] != 'x' || s[1] != 'X')) {
		return false;
	}
	if (sscanf(s,"0x%x",&temp)<=0) {
		return false;
	}
	*value = temp;
	return true;
}

bool JsonBaseClass::helper_getValueString(char* key,struct json_object *val,const char* myKey,CString* value) {
	if (0 != strcmp(key,myKey)) {
		return false;
	}
	if (json_type_string!= json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}
	*value = CString(json_object_get_string(val));
	return true;
}

bool JsonBaseClass::helper_getValueBoolean(char* key,struct json_object *val,const char* myKey,bool* value) {
	if (0 != strcmp(key,myKey)) {
		return false;
	}

	const json_type type = json_object_get_type(val);
	if (json_type_boolean != type) {
		ASSERT(json_type_boolean != type);
		return false;
	}
	const json_bool v = json_object_get_boolean(val);
	*value = (v == 0 ? false : true);
	return true;
}
/**************************************************************/

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

HttpRCfgResult_t JsonAppCfgResult::parse(AppCfgResult* dest,const std::string& str) {	
	return helper_parse(str,dest);
}

HttpRCfgResult_t JsonAppCfgResult::testParser( ) {
	std::string strData( 
		"{ \"status\" : \"failed\"," 
	    "\"error\" : \"test error message\" ,"
		"\"errorMsg\" : \"extended test error message\" }"
		);

	AppCfgResult dest;
	return parse(&dest,strData);
}

bool JsonAppCfgResult::parseStatus(char* key,struct json_object *val,void* pvData) {	
	if (0 != strcmp(key,KEY_STATUS)) {
		return false;
	}
	AppCfgResult* dest = reinterpret_cast<AppCfgResult*>(pvData);
	if (0 == dest) {
		return false;
	}
	const char* strStatus = json_object_get_string(val);
	if (0 == strcmp(VALUE_STATUS_FAILED,strStatus)) {
		dest->setResultCode(AppCfgResult::RESULT_ERROR);			
	}
	else if (0 == strcmp(VALUE_STATUS_OK,strStatus)) {
		dest->setResultCode(AppCfgResult::RESULT_SUCCESS);			
	}		
	return true;
}

bool JsonAppCfgResult::parseError(char* key,struct json_object *val,void* pvData) {
	if (0 != strcmp(key,KEY_STRING_ERROR)) {
		return false;
	}
	AppCfgResult* dest = reinterpret_cast<AppCfgResult*>(pvData);
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
	AppCfgResult* dest = reinterpret_cast<AppCfgResult*>(pvData);
	if (0 == dest) {
		return false;
	}
	dest->setErrorDaetailMsg(json_object_get_string(val));
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
/**************************************************************/
JsonAppNodeInfo::JsonAppNodeInfo( ) { 
	m_arrParser.SetSize(8);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseLogicalAddress));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseMasterAddress));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseNumberOfUnits));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parsePhysicalAddress));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseProductName));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseSoftwareVersion));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&JsonAppNodeInfo::parseSoftwareNicVersion));
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
}; /* namespace DUOTECNO */
/**************************************************************/