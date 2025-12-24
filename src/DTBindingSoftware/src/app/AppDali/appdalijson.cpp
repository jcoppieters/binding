#include "stdafx.h"
#include "appdalijson.h"
#include "appdalidevices.h"
#include "appdali_common.h"
#include "appdaliprotocol_def.h"
#include "json.h"
#include "utils/mfc/mfcCollections.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
// Deze template functies als friend toevoegen.
template <class TYPE>
static bool helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(const CString&)) {
	CString str;
	if (!CAppDaliJson_Base::helper_getValueString(key, val, myKey, &str)) {
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
	if (!CAppDaliJson_Base::helper_getValueInt(key, val, myKey, &value)) {
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
	if (!CAppDaliJson_Base::helper_getValueHexInt(key, val, myKey, &value)) {
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
static bool helper_setValueBoolean(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(bool)) {
	bool value = false;
	if (!CAppDaliJson_Base::helper_getValueBoolean(key, val, myKey, &value)) {
		return false;
	}
	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliJson_Base::CAppDaliJson_Base( ) : m_arrParser() { }
CAppDaliJson_Base::~CAppDaliJson_Base( ) { }

CAppDaliJson_Base::JsonParserResult_t CAppDaliJson_Base::helper_parse(const std::string& str, void* pvData) {	
	if (str.empty()) {
		return RESULT_EC_JSON_PARSE_DATA_NULL;
	}
	json_object* jobj = json_tokener_parse(str.c_str());
	if (0 == jobj) {
		return RESULT_EC_JSON_PARSE_OBJECT_NULL;
	}
	const json_type type = json_object_get_type(jobj);
	if (json_type_null == type) {
		json_object_put(jobj);
		return RESULT_EC_JSON_PARSE_OBJECT_NULL;
	}
	if (json_type_object != type) {
		json_object_put(jobj);
		return RESULT_EC_JSON_PARSE_WRONG_TYPE;		// V2.0
	}
	const JsonParserResult_t result = this->helper_parse(jobj, pvData);
	json_object_put(jobj);
	return result;
}

CAppDaliJson_Base::JsonParserResult_t CAppDaliJson_Base::helper_parse(struct json_object* jobj, void* pvData) {
	const int length = m_arrParser.GetSize();
	json_object_object_foreach(jobj, key, val) {
		for (int i = 0; i<length; ++i) {
			PF_PARSER pf = m_arrParser[i];
			if (0 != pf) {
				if ((this->*pf)(key, val, pvData)) {
					break; // break for loop...
				}
			}
		}
	}
	return RESULT_SUCCESS;
}

// integer of een string integer...
bool CAppDaliJson_Base::helper_getValueInt(char* key, struct json_object *val, const char* myKey, int* value) {
	if (0 != strcmp(key, myKey)) {
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
		int temp = -1;
		if (sscanf(s, "%d", &temp) >= 1) {
			*value = temp;
			return true;
		}
	}
	else {
		ASSERT(0);
	}
	return false;
}

bool CAppDaliJson_Base::helper_getValueHexInt(char* key, struct json_object *val, const char* myKey, int* value) {
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	if (json_type_string != json_object_get_type(val)) {
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
	if (sscanf(s, "0x%x", &temp) <= 0) {
		return false;
	}
	*value = temp;
	return true;
}

bool CAppDaliJson_Base::helper_getValueString(char* key, struct json_object *val, const char* myKey, CString* value) {
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	if (json_type_string != json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}
	*value = CString(json_object_get_string(val));
	return true;
}

bool CAppDaliJson_Base::helper_getValueBoolean(char* key, struct json_object *val, const char* myKey, bool* value) {
	if (0 != strcmp(key, myKey)) {
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

bool CAppDaliJson_Base::loadFile(const char* fileName,std::string* strDest) {
	if (NULL == fileName || NULL == strDest) {
		REQUIRE(NULL != fileName);
		REQUIRE(NULL != strDest);
		return false;
	}
	CFile f;
	if (!f.Open(fileName, CFile::modeRead)) {
		return false;
	}
	DWORD dwRead = 0;
	do {
		BYTE buffer[4096];
		memset(buffer, 0, sizeof(buffer));
		dwRead = f.Read(buffer, 4096);
		if (dwRead>0) {
			strDest->append((const char*)buffer, (int)dwRead);
		}
	} while (dwRead > 0);
	f.Close();
	return true;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const char* const CAppDaliJson::KEY_GROUPS_NAMES = "groupsNames";
const char* const CAppDaliJson::KEY_SCENES_NAMES = "scenesNames";
/***********************************************************************************/
CAppDaliJson::CAppDaliJson( ) : CAppDaliJson_Base()  { }
CAppDaliJson::~CAppDaliJson( ) { }

bool CAppDaliJson::save(const char* strFileName, const CAppDaliDevices& srcDevices,
						const CAppDaliCollectionNames& srcNamesGroup, const CAppDaliCollectionNames& srcNamesScenes) {
	CFile f;
	if (!f.Open(strFileName, CFile::modeWrite | CFile::modeCreate)) {
		return false;
	}

	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {		
		return false;
	}

	CAppDaliJsonDevices jsonFormatter;
	if (false == jsonFormatter.format(jsonObject,srcDevices)) {	
		json_object_put(jsonObject);
		return false;
	}
	CAppDaliJsonCollectionNames jsonFormatterGroupNames(KEY_GROUPS_NAMES);
	if (false == jsonFormatterGroupNames.format(jsonObject,srcNamesGroup)) {	
		json_object_put(jsonObject);
		return false;
	}
	CAppDaliJsonCollectionNames jsonFormatterScenesNames(KEY_SCENES_NAMES);
	if (false == jsonFormatterScenesNames.format(jsonObject,srcNamesScenes)) {	
		json_object_put(jsonObject);
		return false;
	}

	const std::string strData = json_object_to_json_string(jsonObject);
	ASSERT(strData.length() > 0);	
	f.Write(strData.c_str(), strData.length());
	f.Flush();
	json_object_put(jsonObject);
	return true;
}

bool CAppDaliJson::load(const char* strFileName, CAppDaliDevices* pDest, 
						CAppDaliCollectionNames* pDestNamesGroup,CAppDaliCollectionNames* pDestNamesScenes) {
	std::string strFileData;
	if (!this->loadFile(strFileName,&strFileData)) {
		return false;
	}
	if (strFileData.length() <= 0) {
		return false;
	}

	{
		CAppDaliJsonDevices jsonParserDevices;
		const JsonParserResult_t res = jsonParserDevices.parse(pDest,strFileData);
		if (RESULT_SUCCESS != res) {	
			return false;
		}
	}
	{
		CAppDaliJsonCollectionNames jsonParser(KEY_GROUPS_NAMES);
		const JsonParserResult_t res = jsonParser.parse(pDestNamesGroup,strFileData);
		if (RESULT_SUCCESS != res) {	
			return false;
		}
	}
	{
		CAppDaliJsonCollectionNames jsonParser(KEY_SCENES_NAMES);
		const JsonParserResult_t res = jsonParser.parse(pDestNamesScenes,strFileData);
		if (RESULT_SUCCESS != res) {	
			return false;
		}
	}
	return true;
}

/***********************************************************************************/
const char* const CAppDaliJsonDevices::KEY_DEVICES = "devices";	
/***********************************************************************************/
CAppDaliJsonDevices::CAppDaliJsonDevices( ) : CAppDaliJson_Base()  { 
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevices::parseDevices));
}
CAppDaliJsonDevices::~CAppDaliJsonDevices( ) { }

CAppDaliJsonDevices::JsonParserResult_t CAppDaliJsonDevices::parse(CAppDaliDevices* dest, const std::string& str) {
	return helper_parse(str, dest);
}

bool CAppDaliJsonDevices::parseDevices(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY_DEVICES)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	CAppDaliDevices* const dest = reinterpret_cast<CAppDaliDevices*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonObjectNode = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonObjectNode)) {
			ASSERT(json_type_object != json_object_get_type(jsonObjectNode));
			return false;
		}
		CAppDaliJsonDevice parser;
		CAppDaliDevice* const pDevice = parser.parseCreate(jsonObjectNode);
		if (NULL == pDevice) {
			ASSERT(NULL != pDevice); 
			return false;
		}
		dest->add(pDevice);
	}
	return true;
}

// Json doc met alle info van de DALI devices die gevonden werden.
bool CAppDaliJsonDevices::format(struct json_object *dest, const CAppDaliDevices& src) {
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}
	const int count = src.getCount();
	for (int i = 0; i<count; ++i) {
		CAppDaliDevice* pDevice = 0;
		if (src.get(i, &pDevice)) {			
			CAppDaliJsonDevice jsonFormatter;
			struct json_object* const object = jsonFormatter.format(*pDevice);
			if (NULL != object) {
				json_object_array_add(array_items, object);
			}			
		}
	}
	json_object_object_add(dest, KEY_DEVICES, array_items);	
	return true;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const char* const CAppDaliJsonDevice::KEY_TYPE = "type";											// int
const char* const CAppDaliJsonDevice::KEY_NAME = "name";											// string
const char* const CAppDaliJsonDevice::KEY_SHORTADDRESS = "shortAddress";							// int
const char* const CAppDaliJsonDevice::KEY_GTIN = "GTIN";											// string
const char* const CAppDaliJsonDevice::KEY_SERIAL_NUMBER = "serialNumber";							// string
const char* const CAppDaliJsonDevice::KEY_VERSION_FIRMWARE = "firmwareVersion";						// int
const char* const CAppDaliJsonDevice::KEY_VERSION_HARDWARE = "hardwareVersion";						// int
const char* const CAppDaliJsonDevice::KEY_PROPERTIES = "properties";								// object
const char* const CAppDaliJsonDevice::KEY_ERROR_STATUS = "errorStatus";								// int
const char* const CAppDaliJsonDevice::KEY_ERROR_CODE = "errorCode";									// int

/***********************************************************************************/
CAppDaliJsonDevice::CAppDaliJsonDevice( ) : CAppDaliJson_Base() { 
	m_arrParser.SetSize(9);		
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseName));					// 0
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseShortAddress));			// 1
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseGTIN));					// 2
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseSerialNumber));			// 3
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseVersionFirmware));		// 4
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseVersionHardware));		// 5
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseErrorStatus));			// 6
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseErrorCode));				// 7
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevice::parseProperties));				// 8		
}

CAppDaliJsonDevice::~CAppDaliJsonDevice( ) { }

CAppDaliDevice* CAppDaliJsonDevice::parseCreate(struct json_object* src) {	
	CAppDaliDevice* pDevice = NULL;
	if (!this->parseType(src,&pDevice)) {
		return NULL;
	}	
	INVARIANT(NULL != pDevice);
	this->helper_parse(src,pDevice);
	ENSURE(NULL != pDevice);
	return pDevice;
}

bool CAppDaliJsonDevice::parseType(struct json_object *val, CAppDaliDevice** ppDevice) {
	struct json_object* jsonObject = json_object_object_get(val,KEY_TYPE);
	if (NULL == jsonObject) {
		ASSERT(NULL != jsonObject);
		return false;
	}
	const json_type type = json_object_get_type(jsonObject);
	if (json_type_int != type) {
		ASSERT(0);
		return false;
	}	
	const int value = json_object_get_int(jsonObject);
	if (!((DALI_DEVICE_TYPE_CONTROLGEAR == value) || (DALI_DEVICE_TYPE_INPUTCONTROL == value))) {
		ASSERT(0);
		return false;
	}
	
	CAppDaliDevice* pDevice = new CAppDaliDevice((DaliDeviceType_t) value);
	if (NULL == pDevice) {
		ASSERT(NULL!=pDevice);
		return false;
	}
	*ppDevice = pDevice;
	return true;
}
	
bool CAppDaliJsonDevice::parseName(char* key, struct json_object *val, void* pvData) {
	return helper_setValueString(key, val, pvData, KEY_NAME, &CAppDaliDevice::setName);
}
bool CAppDaliJsonDevice::parseShortAddress(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_SHORTADDRESS, &CAppDaliDevice::setShortAddress);	
}
bool CAppDaliJsonDevice::parseGTIN(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_GTIN;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	if (json_type_string != json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}	
	const char* str = json_object_get_string(val);
	if (NULL == str) {
		return false;
	}	
	CAppDaliDevice* const dest = reinterpret_cast<CAppDaliDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	unsigned long long value = 0;
	if (0 != strcmp(str,"")) {
		if (sscanf(str,"%llu",&value)<=0) {
			value = 0;
		}
	}	
	dest->setGTIN(value);
	return true;
}

bool CAppDaliJsonDevice::parseSerialNumber(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_SERIAL_NUMBER;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	if (json_type_string != json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}	
	const char* str = json_object_get_string(val);
	if (NULL == str) {
		return false;
	}
	CAppDaliDevice* const dest = reinterpret_cast<CAppDaliDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	unsigned long long value = 0;
	if (0 != strcmp(str,"")) {
		if (sscanf(str,"%llu",&value)<=0) {
			value = 0;
		}
	}	
	dest->setSerialcode(value);
	return true;
}

bool CAppDaliJsonDevice::parseVersionFirmware(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_VERSION_FIRMWARE, &CAppDaliDevice::setVersionFirmware);	
}

bool CAppDaliJsonDevice::parseVersionHardware(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_VERSION_HARDWARE, &CAppDaliDevice::setVersionHardware);	
}
bool CAppDaliJsonDevice::parseErrorStatus(char* key, struct json_object *val, void* pvData) {
	int value = -1;
	if (!CAppDaliJson_Base::helper_getValueInt(key, val, KEY_ERROR_STATUS, &value)) {
		return false;
	}
	CAppDaliDevice* const dest = reinterpret_cast<CAppDaliDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	dest->setErrorStatus(static_cast<DaliDeviceErrorStatus_t>(value));		// Todo: convert functie.
	return true;	
}

bool CAppDaliJsonDevice::parseErrorCode(char* key, struct json_object *val, void* pvData) {
	int value = -1;
	if (!CAppDaliJson_Base::helper_getValueInt(key, val, KEY_ERROR_CODE, &value)) {
		return false;
	}
	CAppDaliDevice* const dest = reinterpret_cast<CAppDaliDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	dest->setErrorCode(static_cast<DaliErrorCode_t>(value));		// Todo: convert functie.
	return true;
}

bool CAppDaliJsonDevice::parseProperties(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_PROPERTIES;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	if (json_type_object != json_object_get_type(val)) {
		ASSERT(json_type_object != json_object_get_type(val));
		return false;
	}	
	CAppDaliDevice* const dest = reinterpret_cast<CAppDaliDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	const DaliDeviceType_t type = dest->getType();
	if (!((DALI_DEVICE_TYPE_CONTROLGEAR == type) || (DALI_DEVICE_TYPE_INPUTCONTROL == type))) {
		ASSERT(0);
		return false;
	}
	switch(type) {
		case DALI_DEVICE_TYPE_CONTROLGEAR: {
			CAppDaliDeviceProperties_ControlGear* const p = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&dest->getProperties());
			if (NULL == p) {
				ASSERT(NULL != p);
				return false;
			}
			CAppDaliJsonDevicePropControlGear parser;
			const JsonParserResult_t res = parser.parse(p,val);
			if (RESULT_SUCCESS != res) {
				return false;
			}										   										   
		} break;
		
		case DALI_DEVICE_TYPE_INPUTCONTROL: {
			CAppDaliDeviceProperties_InputControlDevice* const p = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&dest->getProperties());
			if (NULL == p) {
				ASSERT(NULL != p);
				return false;
			}
			CAppDaliJsonDeviceInputDevice parser;
			const JsonParserResult_t res = parser.parse(p,val);
			if (RESULT_SUCCESS != res) {
				return false;
			}												
		} break;	
	}
	return true;
}

struct json_object* CAppDaliJsonDevice::format(const CAppDaliDevice& src) {
	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		ASSERT(0 != jsonObject);
		return NULL;
	}

	const DaliDeviceType_t type = src.getType();
	json_object_object_add(jsonObject, KEY_TYPE, json_object_new_int((int) type));
	json_object_object_add(jsonObject, KEY_SHORTADDRESS, json_object_new_int(src.getShortAddress()));
	json_object_object_add(jsonObject, KEY_VERSION_FIRMWARE, json_object_new_int(src.getVersionFirmware()));
	json_object_object_add(jsonObject, KEY_VERSION_HARDWARE, json_object_new_int(src.getVersionHardware()));
	json_object_object_add(jsonObject, KEY_ERROR_STATUS, json_object_new_int((int) src.getErrorStatus()));
	json_object_object_add(jsonObject, KEY_ERROR_CODE, json_object_new_int((int) src.getErrorCode()));

	{
		char str[64];
		CAppDaliDevice::getStrGTIN(src,str,sizeof(str));
		json_object_object_add(jsonObject, KEY_GTIN, json_object_new_string((LPCSTR)str));
	}
	{
		char str[64];
		CAppDaliDevice::getStrSerialCode(src,str,sizeof(str));
		json_object_object_add(jsonObject, KEY_SERIAL_NUMBER, json_object_new_string((LPCSTR)str));
	}

	{
		char str[64];
		memset(str,'\0',sizeof(str));
		const CString& srcName = src.getName();
		if (srcName.IsEmpty()) {
			strcpy(str,"");
		}
		else {
			memcpy(str,(LPCSTR)srcName,sizeof(str)-1);
		}
		json_object_object_add(jsonObject, KEY_NAME, json_object_new_string((LPCSTR)str));		
	}

	switch(type) {
		case DALI_DEVICE_TYPE_CONTROLGEAR: {
			CAppDaliJsonDevicePropControlGear formatter;
			struct json_object* jsonObjectProperties = formatter.format(src);
			if (NULL != jsonObjectProperties) {
				json_object_object_add(jsonObject, KEY_PROPERTIES, jsonObjectProperties);
			}
		} break;

		case DALI_DEVICE_TYPE_INPUTCONTROL: {
			CAppDaliJsonDeviceInputDevice formatter;
			struct json_object* jsonObjectProperties = formatter.format(src);
			if (NULL != jsonObjectProperties) {
				json_object_object_add(jsonObject, KEY_PROPERTIES, jsonObjectProperties);
			}
		} break;
	}
	return jsonObject;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
static const int CONTROLGEAR_DEVICE_MAX_GROUPS = 16;
static const int CONTROLGEAR_DEVICE_MAX_SCENES = 16;
/***********************************************************************************/
const char* const CAppDaliJsonDevicePropControlGear::KEY_GROUPS = "groups";										// array int
const char* const CAppDaliJsonDevicePropControlGear::KEY_SCENES = "scenes";										// array int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_TYPE = "deviceType";							// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_FADETIMERATE = "fadeTimeRate";					// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_MAXLEVEL = "maximumLevel";						// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_MINLEVEL = "minimumLevel";						// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_PHYSICAL_MIN_LEVEL = "physicalMinLevel";		// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_POWER_ON_LEVEL = "powerOnLevel";				// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_SYSTEM_FAILURE_LEVEL = "systemFailureLevel";	// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_EXTENDED_FADETIME = "extendedFadeTime";			// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_DIMMERCURVE = "dimmerCurve";					// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_COLORFEATURES = "colorFeatures";				// int
const char* const CAppDaliJsonDevicePropControlGear::KEY_DEVICE_TYPES = "deviceTypes";							// array int
const char* const CAppDaliJsonDevicePropControlGear::KEY_SCENES_COLOR = "scenesColor";							// array objects.
/***********************************************************************************/
CAppDaliJsonDevicePropControlGear::CAppDaliJsonDevicePropControlGear( ) : 
	CAppDaliJson_Base() { 
	m_arrParser.SetSize(14);		
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseGroups));					// 0
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseScenes));					// 1
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseDeviceType));				// 2
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseFadeTimeRate));			// 3
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseMaxLevel));				// 4
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseMinLevel));				// 5
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parsePhysicalMinLevel));		// 6
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parsePowerOnLevel));			// 7
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseSystemFailureLevel));		// 8
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseExtendedFadeTime));		// 9
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseDimmerCurve));				// 10	
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseColorFeatures));			// 11	
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parseDeviceTypes));				// 12	
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGear::parsScenesDT8Color));			// 13	
}
CAppDaliJsonDevicePropControlGear::~CAppDaliJsonDevicePropControlGear( ) { }

CAppDaliJsonDevicePropControlGear::JsonParserResult_t CAppDaliJsonDevicePropControlGear::parse(CAppDaliDeviceProperties_ControlGear* dest, struct json_object* src) {
	return helper_parse(src, dest);
}

bool CAppDaliJsonDevicePropControlGear::parseGroups(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_GROUPS;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	const json_type type = json_object_get_type(val);
	if (json_type_array != type) {
		ASSERT(json_type_array != type);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* const dest = reinterpret_cast<CAppDaliDeviceProperties_ControlGear*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}

	uint16_t groupInfo = 0;
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonArrayItemInteger = json_object_array_get_idx(val, i);
		if (json_type_int != json_object_get_type(jsonArrayItemInteger)) {
			ASSERT(json_type_int != json_object_get_type(jsonArrayItemInteger));
			return false;
		}		
		const int value = json_object_get_int(jsonArrayItemInteger);
		if (value >= 0 && value<CONTROLGEAR_DEVICE_MAX_GROUPS) {
			groupInfo += (1 << value);
		}
		else { ASSERT(0); }
	}
	INVARIANT(NULL != dest);
	dest->setGroupInfo(groupInfo);
	return true;

}
bool CAppDaliJsonDevicePropControlGear::parseScenes(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_SCENES;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	const json_type type = json_object_get_type(val);
	if (json_type_array != type) {
		ASSERT(json_type_array != type);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* const dest = reinterpret_cast<CAppDaliDeviceProperties_ControlGear*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}

	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonArrayItemInteger = json_object_array_get_idx(val, i);
		if (json_type_int != json_object_get_type(jsonArrayItemInteger)) {
			ASSERT(json_type_int != json_object_get_type(jsonArrayItemInteger));
			return false;
		}		
		if (i < CONTROLGEAR_DEVICE_MAX_SCENES) {
			INVARIANT(NULL != dest);
			const int value = json_object_get_int(jsonArrayItemInteger);
			if (value>=0 && value<=255) {			
				dest->setSceneLevel(i,value);
			}
			else {
				ASSERT(value >=0 && value<=255);
				dest->setSceneLevel(i,255);
			}
		}
		else { ASSERT(i<CONTROLGEAR_DEVICE_MAX_SCENES ); }
	}		
	return true;
}

bool CAppDaliJsonDevicePropControlGear::parseDeviceType(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_TYPE, &CAppDaliDeviceProperties_ControlGear::setDeviceType);	
}
bool CAppDaliJsonDevicePropControlGear::parseFadeTimeRate(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_FADETIMERATE, &CAppDaliDeviceProperties_ControlGear::setFadeTimeRate);	
}
bool CAppDaliJsonDevicePropControlGear::parseMaxLevel(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_MAXLEVEL, &CAppDaliDeviceProperties_ControlGear::setLevelMax);
}
bool CAppDaliJsonDevicePropControlGear::parseMinLevel(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_MINLEVEL, &CAppDaliDeviceProperties_ControlGear::setLevelMin);
}
bool CAppDaliJsonDevicePropControlGear::parsePhysicalMinLevel(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_PHYSICAL_MIN_LEVEL, &CAppDaliDeviceProperties_ControlGear::setPhysicalMinValue);
}
bool CAppDaliJsonDevicePropControlGear::parsePowerOnLevel(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_POWER_ON_LEVEL, &CAppDaliDeviceProperties_ControlGear::setLevelPowerOn);
}
bool CAppDaliJsonDevicePropControlGear::parseSystemFailureLevel(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_SYSTEM_FAILURE_LEVEL, &CAppDaliDeviceProperties_ControlGear::setLevelSystemFailure);
}
bool CAppDaliJsonDevicePropControlGear::parseExtendedFadeTime(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_EXTENDED_FADETIME, &CAppDaliDeviceProperties_ControlGear::setExtendedFadeTime);
}
bool CAppDaliJsonDevicePropControlGear::parseDimmerCurve(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_DIMMERCURVE, &CAppDaliDeviceProperties_ControlGear::setDimmingCurve);
}
bool CAppDaliJsonDevicePropControlGear::parseColorFeatures(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_DEVICE_COLORFEATURES, &CAppDaliDeviceProperties_ControlGear::setColorFeatures);
}
bool CAppDaliJsonDevicePropControlGear::parseDeviceTypes(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_DEVICE_TYPES;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	const json_type type = json_object_get_type(val);
	if (json_type_array != type) {
		ASSERT(json_type_array != type);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* const dest = reinterpret_cast<CAppDaliDeviceProperties_ControlGear*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}

	CList<int> listDeviceTypes;
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonArrayItemInteger = json_object_array_get_idx(val, i);
		if (json_type_int != json_object_get_type(jsonArrayItemInteger)) {
			ASSERT(json_type_int != json_object_get_type(jsonArrayItemInteger));
			return false;
		}			
		listDeviceTypes.AddTail(json_object_get_int(jsonArrayItemInteger));		
	}		
	dest->setDeviceTypes(listDeviceTypes);
	return true;
}

static bool addItem(CAppDaliDeviceProperties_ControlGear::CMapDT8ColorScenesInfo* pDest, int sceneNumber, CAppDaliObjectDT8ColorData& item) {
	REQUIRE(NULL != pDest);
	REQUIRE(sceneNumber>=DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST && sceneNumber<=DALI_SYSTEM_CONTROLGEAR_SCENE_LAST);

	CAppDaliObjectDT8ColorData* const pNewObject = new CAppDaliObjectDT8ColorData(item);
	if (NULL == pNewObject) {							
		return false;
	}
	CAppDaliObjectDT8ColorData* oldObject = NULL;
	if (pDest->Lookup(sceneNumber,oldObject)) {
		// Indien er een object met het zelfde scene nummer aanwezig is deze dealloceren.
		ASSERT(0);	// Kan normaal niet voorkomen...
		ASSERT(NULL != oldObject);
		delete oldObject;
		oldObject = NULL;
	}	
	pDest->SetAt(sceneNumber,pNewObject);	
	return true;
}

bool CAppDaliJsonDevicePropControlGear::parsScenesDT8Color(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_SCENES_COLOR;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	const json_type type = json_object_get_type(val);
	if (json_type_array != type) {
		ASSERT(json_type_array != type);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* const dest = reinterpret_cast<CAppDaliDeviceProperties_ControlGear*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear::CMapDT8ColorScenesInfo mapTempScenesInfo;	
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonArrayItemObject = json_object_array_get_idx(val, i);
		if (json_type_object == json_object_get_type(jsonArrayItemObject)) {		
			CAppDaliObjectDT8ColorData colorDataObject;
			CAppDaliJsonDevicePropControlGearDT8sceneColorInfo parser;
			if (RESULT_SUCCESS == parser.parse(&colorDataObject,jsonArrayItemObject)) {
				addItem(&mapTempScenesInfo,parser.getAddress(),colorDataObject);			
			}
		} else { ASSERT(0); };
	}

	// Device updaten met de DT8 Color scene info map.
	INVARIANT(NULL != dest);
	dest->setDT8SceneColorInfo(mapTempScenesInfo);

	// Dealoceren van de temp map.
	DUOTECNO::MFC_HELPER::DeallocCMapValues(mapTempScenesInfo);

	return true;
}

/*********************************************************************************** 
	{
		groups : [ 0,1,5 ],
		scenes : [ 255, 128, 255, 0 , ...]
	}
************************************************************************************/
struct json_object* CAppDaliJsonDevicePropControlGear::format(const CAppDaliDevice& src) {
	const CAppDaliDeviceProperties_ControlGear* const properties = dynamic_cast<const CAppDaliDeviceProperties_ControlGear*>(&src.getProperties());
	if (NULL == properties) {
		ASSERT(NULL != properties);
		return NULL;
	}
	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		ASSERT(0 != jsonObject);
		return NULL;
	}
	struct json_object* const jsonObjectGroups = this->formatGroups(*properties);
	if (NULL != jsonObjectGroups) {
		json_object_object_add(jsonObject, KEY_GROUPS, jsonObjectGroups);
	}
	struct json_object* const jsonObjectScenes = this->formatScenes(*properties);
	if (NULL != jsonObjectScenes) {
		json_object_object_add(jsonObject, KEY_SCENES, jsonObjectScenes);
	}
	struct json_object* const jsonObjectDeviceTypes = this->formatDeviceTypes(*properties);
	if (NULL != jsonObjectDeviceTypes) {
		json_object_object_add(jsonObject, KEY_DEVICE_TYPES, jsonObjectDeviceTypes);
	}
	json_object_object_add(jsonObject, KEY_DEVICE_COLORFEATURES, json_object_new_int(properties->getColorFeatures()));
	json_object_object_add(jsonObject, KEY_DEVICE_TYPE, json_object_new_int(properties->getDeviceType()));
	json_object_object_add(jsonObject, KEY_DEVICE_FADETIMERATE, json_object_new_int(properties->getFadeTimeRate()));
	json_object_object_add(jsonObject, KEY_DEVICE_MAXLEVEL, json_object_new_int(properties->getLevelMax()));
	json_object_object_add(jsonObject, KEY_DEVICE_MINLEVEL, json_object_new_int(properties->getLevelMin()));
	json_object_object_add(jsonObject, KEY_DEVICE_PHYSICAL_MIN_LEVEL, json_object_new_int(properties->getPhysicalMinValue()));
	json_object_object_add(jsonObject, KEY_DEVICE_POWER_ON_LEVEL, json_object_new_int(properties->getLevelPowerOn()));
	json_object_object_add(jsonObject, KEY_DEVICE_SYSTEM_FAILURE_LEVEL, json_object_new_int(properties->getLevelSystemFailure()));
	json_object_object_add(jsonObject, KEY_DEVICE_EXTENDED_FADETIME, json_object_new_int(properties->getExtendedFadeTime()));
	json_object_object_add(jsonObject, KEY_DEVICE_DIMMERCURVE, json_object_new_int(properties->getDimmingCurve()));


	struct json_object* const jsonObjectScenesDT8Color = this->formatScenesDT8Color(*properties);
	if (NULL != jsonObjectScenesDT8Color) {
		json_object_object_add(jsonObject, KEY_SCENES_COLOR, jsonObjectScenesDT8Color);
	}
	return jsonObject;
}

// TODO_DALI_JSON:		zie CAppDaliDeviceProperties_ControlGear::isGroupMember( )
// Utility functie in de klasse CAppDaliDeviceProperties_ControlGear gebruiken om te bepalen of een device aanwezig is in een groep.
struct json_object* CAppDaliJsonDevicePropControlGear::formatGroups(const CAppDaliDeviceProperties_ControlGear& refproperties) {	
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}
	const unsigned short groupInfo = refproperties.getGroupInfo();
	for (int i = 0; i < CONTROLGEAR_DEVICE_MAX_GROUPS; i++) {
		if (groupInfo & (1 << i)) {													// TODO_DALI_JSON:		zie CAppDaliDeviceProperties_ControlGear::isGroupMember( )
			json_object_array_add(array_items, json_object_new_int(i));			
		}
	}
	return array_items;
}

struct json_object* CAppDaliJsonDevicePropControlGear::formatScenes(const CAppDaliDeviceProperties_ControlGear& refproperties) {
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}	
	for (int i = 0; i < CONTROLGEAR_DEVICE_MAX_SCENES; i++) {	
		json_object_array_add(array_items, json_object_new_int(refproperties.getSceneLevel(i)));		
	}
	return array_items;
}

struct json_object* CAppDaliJsonDevicePropControlGear::formatDeviceTypes(const CAppDaliDeviceProperties_ControlGear& refproperties) {
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}	
	const CList<int>& r = refproperties.getDeviceTypes();
	POSITION pos = r.GetHeadPosition();
	while(pos!=NULL) {	
		json_object_array_add(array_items, json_object_new_int(r.GetNext(pos)));
	}
	return array_items;
}

struct json_object* CAppDaliJsonDevicePropControlGear::formatScenesDT8Color(const CAppDaliDeviceProperties_ControlGear& refproperties) {
	const CAppDaliDeviceProperties_ControlGear::CMapDT8ColorScenesInfo& refScenesColorInfo = refproperties.getT8SceneColorInfo();
	if (refScenesColorInfo.GetSize() <= 0) {
		return NULL;
	}	
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}	
	
	POSITION pos = refScenesColorInfo.GetStartPosition();  
    while (pos != NULL) {
		int key = 0;   
		CAppDaliObjectDT8ColorData* pObject = NULL;

        // Haal de sleutel en waarde op
        refScenesColorInfo.GetNextAssoc(pos, key, pObject);
		ASSERT(NULL != pObject);

		// Json object formatteren en toevoegen aan het array.
		if (NULL != pObject) {
			CAppDaliJsonDevicePropControlGearDT8sceneColorInfo formatter;
			json_object* object = formatter.format(key,*pObject);
			if (NULL != object) {
				json_object_array_add(array_items, object);
			}
		}
	}
	return array_items;
}
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_ADDRESS = "address";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_TYPE = "colorType";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_ACTIVE = "active";						// Write only.
// Color type: Temperature_tc
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_TEMPERATURE = "temperature";
// Color type: Coordinate XY
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_COORDINATE_X = "x";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_COORDINATE_Y = "y";
// Color type: Coordinate RGBWAF
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_RED = "red";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_GREEN = "green";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_BLUE = "blue";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_WHITE = "white";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_AMBER = "amber";
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_FREECOLOR = "freeColor";
// Color type: Primary Channels N
const char* const CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::KEY_COLOR_CHANNELS = "channels";
/***********************************************************************************/
CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::CAppDaliJsonDevicePropControlGearDT8sceneColorInfo( ) : 
	CAppDaliJson_Base(), m_address(-1) {
	m_arrParser.SetSize(13);		
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseAddress));					// 0
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorType));					// 1
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorTemperature));			// 2
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorCoordinateX));			// 3
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorCoordinateY));			// 4
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorRed));					// 5
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorGreen));					// 6
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorBlue));					// 7
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorWhite));					// 8
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorAmber));					// 9
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorFreeColor));				// 10
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseArrColorChannels));			// 11
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseAddress));					// 12
	// Er is geen parser voor de KEY_ACTIVE !
}
CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::~CAppDaliJsonDevicePropControlGearDT8sceneColorInfo( ) { }

struct json_object* CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::format(int sceneNumber,const CAppDaliObjectDT8ColorData& src) {

	if (!CAppDaliObjectDT8ColorData_Utils::isValidColorType(src)) {
		return NULL;
	}
	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		ASSERT(0 != jsonObject);
		return NULL;
	}

	// Adres van de sfeer.
	json_object_object_add(jsonObject, KEY_ADDRESS, json_object_new_int(sceneNumber));

	// Color type van de sfeer.
	json_object_object_add(jsonObject, KEY_COLOR_TYPE, json_object_new_int(src.getColorType()));

	// Bepaalt of er voor de configurator een sfeer moet aangemaakt worden.
	// Dit is nodig omdat er soms een geldig color type aanwezig is maar alle velden staan op MASK.
	// In dit geval moet er geen sfeer aangemaakt worden (want doet niets bij het uitvoeren...)
	json_object_object_add(jsonObject, KEY_ACTIVE, json_object_new_boolean(CAppDaliObjectDT8ColorData_Utils::isActive(src)));
	
	// Velden van de kleur types.
	// Mogelijk staan de velden op MASK waarden.
	if (CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(src)) {
		json_object_object_add(jsonObject, KEY_COLOR_COORDINATE_X, json_object_new_int(src.getCoordinateX()));
		json_object_object_add(jsonObject, KEY_COLOR_COORDINATE_Y, json_object_new_int(src.getCoordinateY()));
	}

	if (CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(src)) {
		json_object_object_add(jsonObject, KEY_COLOR_TEMPERATURE, json_object_new_int(src.getColorTemperature()));
	}

	if (CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(src)) {
		json_object_object_add(jsonObject, KEY_COLOR_RED, json_object_new_int(src.getColorDimLevelRed()));
		json_object_object_add(jsonObject, KEY_COLOR_GREEN, json_object_new_int(src.getColorDimLevelGreen()));
		json_object_object_add(jsonObject, KEY_COLOR_BLUE, json_object_new_int(src.getColorDimLevelBlue()));
		json_object_object_add(jsonObject, KEY_COLOR_WHITE, json_object_new_int(src.getColorDimLevelWhite()));
		json_object_object_add(jsonObject, KEY_COLOR_AMBER, json_object_new_int(src.getColorDimLevelAmber()));
		json_object_object_add(jsonObject, KEY_COLOR_FREECOLOR, json_object_new_int(src.getColorDimLevelFreeColor()));
	}

	if (CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(src)) {	
		struct json_object* array_items = json_object_new_array();
		if (0 != array_items) {					
			for (int i = 0; i < CAppDaliObjectDT8ColorData::MAX_PRIMARY_CHANNELS; i++) {														
				json_object_array_add(array_items, json_object_new_int(src.getColorDimLevelChannel(i)));							
			}
			json_object_object_add(jsonObject, KEY_COLOR_CHANNELS, array_items);
		}
	}
	return jsonObject;
}

CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::JsonParserResult_t CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parse(CAppDaliObjectDT8ColorData* dest, struct json_object* src) {	
	this->m_address = -1;
	return helper_parse(src, dest);
}

bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseAddress(char* key, struct json_object *val, void* pvData) {
	int value = -1;
	if (!CAppDaliJson_Base::helper_getValueInt(key, val, KEY_ADDRESS, &value)) {
		return false;
	}
	this->m_address = value;
	return true;
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorType(char* key, struct json_object *val, void* pvData) {
	int value = -1;
	if (!CAppDaliJson_Base::helper_getValueInt(key, val, KEY_COLOR_TYPE, &value)) {
		return false;
	}
	CAppDaliObjectDT8ColorData* const dest = reinterpret_cast<CAppDaliObjectDT8ColorData*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}
	dest->setColorType(value,false);	// Oppassen: false -> Object niet initialiseren...
	return true;
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorTemperature(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_TEMPERATURE, &CAppDaliObjectDT8ColorData::setColorTemperature);	
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorCoordinateX(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_COORDINATE_X, &CAppDaliObjectDT8ColorData::setCoordinateX);	
}	
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorCoordinateY(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_COORDINATE_Y, &CAppDaliObjectDT8ColorData::setCoordinateY);
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorRed(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_RED, &CAppDaliObjectDT8ColorData::setColorDimLevelRed);
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorGreen(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_GREEN, &CAppDaliObjectDT8ColorData::setColorDimLevelGreen);
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorBlue(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_BLUE, &CAppDaliObjectDT8ColorData::setColorDimLevelBlue);
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorWhite(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_WHITE, &CAppDaliObjectDT8ColorData::setColorDimLevelWhite);
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorAmber(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_AMBER, &CAppDaliObjectDT8ColorData::setColorDimLevelAmber);
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseColorFreeColor(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_COLOR_FREECOLOR, &CAppDaliObjectDT8ColorData::setColorDimLevelFreeColor);
}
bool CAppDaliJsonDevicePropControlGearDT8sceneColorInfo::parseArrColorChannels(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_COLOR_CHANNELS;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	const json_type type = json_object_get_type(val);
	if (json_type_array != type) {
		ASSERT(json_type_array != type);
		return false;
	}
	CAppDaliObjectDT8ColorData* const dest = reinterpret_cast<CAppDaliObjectDT8ColorData*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}
	
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonArrayItemInteger = json_object_array_get_idx(val, i);
		if (json_type_int != json_object_get_type(jsonArrayItemInteger)) {
			ASSERT(json_type_int != json_object_get_type(jsonArrayItemInteger));
			return false;
		}			
		dest->setColorDimLevelChannel(i,json_object_get_int(jsonArrayItemInteger));	
	}			
	return true;
}
/***********************************************************************************/

/***********************************************************************************/
/*
	"properties": { 
		"appControllerEnabled": false, 
		"powerCycleNotify": false, 
		"capabilities": 3, 
		"groups": [ ], 
		"instances": [ ] 
	}
*/
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
static const int INPUT_DEVICE_MAX_GROUPS = 32;
/***********************************************************************************/
const char* const CAppDaliJsonDeviceInputDevice::KEY_GROUPS = "groups";										// array integers
const char* const CAppDaliJsonDeviceInputDevice::KEY_APPCONTROLLER_ENABLED = "appControllerEnabled";		// bool
const char* const CAppDaliJsonDeviceInputDevice::KEY_POWERCYCLE_NOTIFY = "powerCycleNotify";				// bool
const char* const CAppDaliJsonDeviceInputDevice::KEY_CAPABILITIES = "capabilities";							// int
const char* const CAppDaliJsonDeviceInputDevice::KEY_INSTANCES = "instances";								// array of objects
/***********************************************************************************/
CAppDaliJsonDeviceInputDevice::CAppDaliJsonDeviceInputDevice() : CAppDaliJson_Base() { 
	m_arrParser.SetSize(5);		
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDevice::parseGroups));					// 0
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDevice::parseAppControllerEnabled));	// 1
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDevice::parsePowerCycleNotify));		// 2
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDevice::parseCapabilities));			// 3
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDevice::parseInstances));				// 4
}
CAppDaliJsonDeviceInputDevice::~CAppDaliJsonDeviceInputDevice() { }

CAppDaliJsonDeviceInputDevice::JsonParserResult_t CAppDaliJsonDeviceInputDevice::parse(CAppDaliDeviceProperties_InputControlDevice* dest, struct json_object* src) {
	return helper_parse(src, dest);
}

bool CAppDaliJsonDeviceInputDevice::parseGroups(char* key, struct json_object *val, void* pvData) {
	static const char* const myKey = KEY_GROUPS;
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	const json_type type = json_object_get_type(val);
	if (json_type_array != type) {
		ASSERT(json_type_array != type);
		return false;
	}
	CAppDaliDeviceProperties_InputControlDevice* const dest = reinterpret_cast<CAppDaliDeviceProperties_InputControlDevice*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}

	uint32_t groupInfo = 0;
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonArrayItemInteger = json_object_array_get_idx(val, i);
		if (json_type_int != json_object_get_type(jsonArrayItemInteger)) {
			ASSERT(json_type_int != json_object_get_type(jsonArrayItemInteger));
			return false;
		}		
		const int value = json_object_get_int(jsonArrayItemInteger);
		if (value >= 0 && value<INPUT_DEVICE_MAX_GROUPS) {
			groupInfo += (1 << value);
		}
		else { ASSERT(0); }
	}
	INVARIANT(NULL != dest);
	dest->setGroupInfo(groupInfo);
	return true;
}
	
bool CAppDaliJsonDeviceInputDevice::parseAppControllerEnabled(char* key, struct json_object *val, void* pvData) {
	return helper_setValueBoolean(key, val, pvData, KEY_APPCONTROLLER_ENABLED, &CAppDaliDeviceProperties_InputControlDevice::setAppControllerEnabled);
}

bool CAppDaliJsonDeviceInputDevice::parsePowerCycleNotify(char* key, struct json_object *val, void* pvData) {
	return helper_setValueBoolean(key, val, pvData, KEY_POWERCYCLE_NOTIFY, &CAppDaliDeviceProperties_InputControlDevice::setPowerCycleNotification);
}
	
bool CAppDaliJsonDeviceInputDevice::parseCapabilities(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_CAPABILITIES, &CAppDaliDeviceProperties_InputControlDevice::setDeviceCapabilities);	
}

bool CAppDaliJsonDeviceInputDevice::parseInstances(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY_INSTANCES)) {
		return false;
	}
	CAppDaliDeviceProperties_InputControlDevice* const dest = reinterpret_cast<CAppDaliDeviceProperties_InputControlDevice*>(pvData);
	if (0 == dest) {
		return false;
	}
	CAppDaliInputControlDeviceInstances& refInstances = dest->getInstances( );
	CAppDaliJsonDeviceInputDeviceInstances parser;
	const bool res = parser.parseArrayInstances(key,val,&refInstances);	// Oppassen: Roepen rechstreeks protected member aan, niet de parse()
	ENSURE(true == res);
	return res; 
}

struct json_object* CAppDaliJsonDeviceInputDevice::format(const CAppDaliDevice& src) {	
	const CAppDaliDeviceProperties_InputControlDevice* const properties = dynamic_cast<const CAppDaliDeviceProperties_InputControlDevice*>(&src.getProperties());
	if (NULL == properties) {
		ASSERT(NULL != properties);
		return NULL;
	}
	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		ASSERT(0 != jsonObject);
		return NULL;
	}	

	json_object_object_add(jsonObject, KEY_APPCONTROLLER_ENABLED, json_object_new_boolean(properties->getAppControllerEnabled()));
	json_object_object_add(jsonObject, KEY_POWERCYCLE_NOTIFY, json_object_new_boolean(properties->getPowerCycleNotification()));
	json_object_object_add(jsonObject, KEY_CAPABILITIES, json_object_new_int(properties->getDeviceCapabilities()));

	// groupq...
	struct json_object* const jsonObjectGroups = this->formatGroups(*properties);
	if (NULL != jsonObjectGroups) {
		json_object_object_add(jsonObject, KEY_GROUPS, jsonObjectGroups);
	}

	// Instances...	
	CAppDaliJsonDeviceInputDeviceInstances formatter;		
	struct json_object* jsonObjectInstances = formatter.format(properties->getInstances());
	if (NULL != jsonObjectInstances) {
		json_object_object_add(jsonObject, KEY_INSTANCES, jsonObjectInstances);
	}
	return jsonObject;
}

struct json_object* CAppDaliJsonDeviceInputDevice::formatGroups(const CAppDaliDeviceProperties_InputControlDevice& refproperties) {
	
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}
	const uint32_t groupInfo = refproperties.getGroupInfo();
	for (int i = 0; i < INPUT_DEVICE_MAX_GROUPS; i++) {
		if (groupInfo & (1 << i)) {													// TODO_DALI_JSON:		zie CAppDaliDeviceProperties_ControlGear::isGroupMember( )
			json_object_array_add(array_items, json_object_new_int(i));			
		}
	}
	return array_items;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

/* "instances": [ ] */

/***********************************************************************************/
CAppDaliJsonDeviceInputDeviceInstances::CAppDaliJsonDeviceInputDeviceInstances( ) : 
	CAppDaliJson_Base() { 
	m_arrParser.SetSize(1);		
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstances::parseArrayInstances));		// 0		
}
CAppDaliJsonDeviceInputDeviceInstances::~CAppDaliJsonDeviceInputDeviceInstances( ) { 
}

CAppDaliJsonDeviceInputDeviceInstances::JsonParserResult_t CAppDaliJsonDeviceInputDeviceInstances::parse(CAppDaliInputControlDeviceInstances* dest, const std::string& str) {
	return helper_parse(str, dest);
}

bool CAppDaliJsonDeviceInputDeviceInstances::parseArrayInstances(char* key, struct json_object *val, void* pvData) {
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	CAppDaliInputControlDeviceInstances* const dest = reinterpret_cast<CAppDaliInputControlDeviceInstances*>(pvData);
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
		CAppDaliJsonDeviceInputDeviceInstance parser;
		CAppDaliInputControlDeviceInstance* const pInstance = parser.parseCreateInstance(jsonObjectNode);
		if (NULL == pInstance) {
			ASSERT(0);
			return false;
		}
		const int instanceNumber = pInstance->getInstanceNumber();
		if (instanceNumber < 0 || instanceNumber > 31) {
			ASSERT((instanceNumber >= 0) && (instanceNumber <= 31));
			delete pInstance;
			return false;
		}
		dest->add(instanceNumber,pInstance);
	}
	return true;
}

struct json_object* CAppDaliJsonDeviceInputDeviceInstances::format(const CAppDaliInputControlDeviceInstances& src) {	
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		ASSERT(NULL != array_items);
		return NULL;
	}
	for (int instanceNumber=CAppDaliInputControlDeviceInstances::FIRST_INSTANCE_NUMBER;
		 instanceNumber<=CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER;
		 ++instanceNumber) {
		CAppDaliInputControlDeviceInstance* pInstance = NULL;	
		if (src.find(instanceNumber,&pInstance)) {
			ASSERT(NULL != pInstance);
			if (NULL != pInstance) {
				CAppDaliJsonDeviceInputDeviceInstance formatter;
				struct json_object* const jsonObjectInstance = formatter.format(*pInstance);
				ASSERT(NULL != jsonObjectInstance);
				if (NULL != jsonObjectInstance) {
					json_object_array_add(array_items, jsonObjectInstance);
				}			
			}		
		}		
	}
	return array_items;
}

/***********************************************************************************/
/*
	{ "name": "", 
	  "number": 0, 
	  "enabled": true, 
	  "type": 1, 
	  "scheme": 2, 
	  "filter": 84, 
	  "priority": 3, 
	  "resolution": 2, 
	  "groups": [ 255, 255, 255 ] 
	 }
*/
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_NUMBER = "number";			// Integer
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_NAME = "name";				// string
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_ENABLED = "enabled";			// boolean
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_TYPE = "type";				// Integer
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_SCHEME = "scheme";			// Integer 0-4
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_FILTER = "filter";			// integer
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_GROUPS = "groups";			// Integere Array van 3 groepen.
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_PRIORITY = "priority";		// integer
const char* const CAppDaliJsonDeviceInputDeviceInstance::KEY_INSTANCE_RESOLUTION = "resolution";	// integer
/***********************************************************************************/
CAppDaliJsonDeviceInputDeviceInstance::CAppDaliJsonDeviceInputDeviceInstance() : 
	CAppDaliJson_Base() { 
	m_arrParser.SetSize(8);			
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstanceName));		// 0
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstanceEnabled));		// 1
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstanceType));		// 2
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstanceScheme));		// 3
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstanceFilter));		// 4
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstancePriority));	// 5	
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstanceResolution));	// 6
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonDeviceInputDeviceInstance::parseInstanceGroups));		// 7	
}
CAppDaliJsonDeviceInputDeviceInstance::~CAppDaliJsonDeviceInputDeviceInstance() { } 

CAppDaliInputControlDeviceInstance* CAppDaliJsonDeviceInputDeviceInstance::parseCreateInstance(struct json_object *val) {
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (!parseInstanceNumber(val,&pInstance)) {
		ASSERT(0);
		return false;
	}
	INVARIANT(NULL != pInstance);
	this->CAppDaliJson_Base::helper_parse(val, pInstance);
	ENSURE(NULL != pInstance);
	return pInstance;
}

bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceNumber(struct json_object *val, CAppDaliInputControlDeviceInstance** ppInstance) {
	json_object* const jsonObjectVal = json_object_object_get(val,KEY_INSTANCE_NUMBER);
	if (NULL == jsonObjectVal) {
		ASSERT(0);
		return false;
	}
	const json_type type = json_object_get_type(jsonObjectVal);
	if (json_type_int != type) {
		ASSERT(0);
		return false;
	}	
	const int instanceNumber = json_object_get_int(jsonObjectVal);
	if (instanceNumber < 0 || instanceNumber > 31) {
		ASSERT(instanceNumber>=0 && instanceNumber<=31);
		return false;	
	}
	CAppDaliInputControlDeviceInstance* pInstance = new CAppDaliInputControlDeviceInstance(instanceNumber);
	if (NULL == pInstance) {
		ASSERT(NULL != pInstance);
		return false;
	}
	*ppInstance = pInstance;
	return true;
}

bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceName(char* key, struct json_object *val, void* pvData)  {
	static const char* const myKey = KEY_INSTANCE_NAME;
	CString str;
	if (!helper_getValueString(key, val, myKey, &str)) {
		return false;
	}
	CAppDaliInputControlDeviceInstance* const dest = reinterpret_cast<CAppDaliInputControlDeviceInstance*>(pvData);
	if (0 == dest) {
		INVARIANT(NULL != dest);
		return false;
	}
	dest->setName(str);
	return true;
}

bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceEnabled(char* key, struct json_object *val, void* pvData)  {
	return helper_setValueBoolean(key, val, pvData, KEY_INSTANCE_ENABLED, &CAppDaliInputControlDeviceInstance::setInstanceActive) ;
}
bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceType(char* key, struct json_object *val, void* pvData)  {
	return helper_setValueInteger(key, val, pvData, KEY_INSTANCE_TYPE, &CAppDaliInputControlDeviceInstance::setInstanceType);
}
bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceScheme(char* key, struct json_object *val, void* pvData)  {
	return helper_setValueInteger(key, val, pvData, KEY_INSTANCE_SCHEME, &CAppDaliInputControlDeviceInstance::setEventScheme);
}
bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceFilter(char* key, struct json_object *val, void* pvData)  {
	return helper_setValueInteger(key, val, pvData, KEY_INSTANCE_FILTER, &CAppDaliInputControlDeviceInstance::setEventFilter);
}
bool CAppDaliJsonDeviceInputDeviceInstance::parseInstancePriority(char* key, struct json_object *val, void* pvData)  {
	return helper_setValueInteger(key, val, pvData, KEY_INSTANCE_PRIORITY, &CAppDaliInputControlDeviceInstance::setEventPriority);
}
bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceResolution(char* key, struct json_object *val, void* pvData) {
	return helper_setValueInteger(key, val, pvData, KEY_INSTANCE_RESOLUTION, &CAppDaliInputControlDeviceInstance::setEventResolution);
}

bool CAppDaliJsonDeviceInputDeviceInstance::parseInstanceGroups(char* key, struct json_object *val, void* pvData)  {
	if (0 != strcmp(key, KEY_INSTANCE_GROUPS)) {			
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	CAppDaliInputControlDeviceInstance* const dest = reinterpret_cast<CAppDaliInputControlDeviceInstance*>(pvData);
	if (0 == dest) {
		return false;
	}

	const int arraylen = json_object_array_length(val);
	ASSERT(arraylen<=CAppDaliInputControlDeviceInstance::NR_OF_INSTANCE_GROUPS);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonObjectNode = json_object_array_get_idx(val, i);
		if (json_type_int != json_object_get_type(jsonObjectNode)) {
			ASSERT(json_type_int != json_object_get_type(jsonObjectNode));
			return false;
		}		
		if (i<CAppDaliInputControlDeviceInstance::NR_OF_INSTANCE_GROUPS) {													
			int value = json_object_get_int(jsonObjectNode);
			if (value>=0 && value<=255) {		
				dest->setInstanceGroup(i,value);		
			}
		}
		else {
			ASSERT(i<CAppDaliInputControlDeviceInstance::NR_OF_INSTANCE_GROUPS);
		}
	}
	return true;
}

struct json_object* CAppDaliJsonDeviceInputDeviceInstance::format(const CAppDaliInputControlDeviceInstance& src) {
	struct json_object* jsonObject = json_object_new_object();
	if (0 == jsonObject) {
		ASSERT(0 != jsonObject);
		return NULL;
	}
	
	{
		char str[64];
		memset(str,'\0',sizeof(str));
		const CString& srcName = src.getName();
		if (srcName.IsEmpty()) {
			strcpy(str,"");
		}
		else {
			memcpy(str,(LPCSTR)srcName,sizeof(str)-1);
		}
		json_object_object_add(jsonObject, KEY_INSTANCE_NAME, json_object_new_string((LPCSTR)str));		
	}

	json_object_object_add(jsonObject, KEY_INSTANCE_NUMBER, json_object_new_int(src.getInstanceNumber()));
	json_object_object_add(jsonObject, KEY_INSTANCE_ENABLED, json_object_new_boolean(src.getInstanceActive()));
	json_object_object_add(jsonObject, KEY_INSTANCE_TYPE, json_object_new_int(src.getInstanceType()));
	json_object_object_add(jsonObject, KEY_INSTANCE_SCHEME, json_object_new_int(src.getEventScheme()));
	json_object_object_add(jsonObject, KEY_INSTANCE_FILTER, json_object_new_int(src.getEventFilter()));
	json_object_object_add(jsonObject, KEY_INSTANCE_PRIORITY, json_object_new_int(src.getEventPriority()));
	json_object_object_add(jsonObject, KEY_INSTANCE_RESOLUTION, json_object_new_int(src.getEventResolution()));

	struct json_object* const jsonObjectGroups = this->formatGroups(src);
	if (NULL != jsonObjectGroups) {
		json_object_object_add(jsonObject, KEY_INSTANCE_GROUPS, jsonObjectGroups);
	}
	return jsonObject;
}

struct json_object* CAppDaliJsonDeviceInputDeviceInstance::formatGroups(const CAppDaliInputControlDeviceInstance& src) {
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return false;
	}
	// 3 groepen.
	json_object_array_add(array_items, json_object_new_int(src.getInstanceGroup(0)));
	json_object_array_add(array_items, json_object_new_int(src.getInstanceGroup(1)));
	json_object_array_add(array_items, json_object_new_int(src.getInstanceGroup(2)));
	return array_items;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliJsonCollectionNames::CAppDaliJsonCollectionNames(const char* key) : CAppDaliJson_Base(), m_key(key) { 
	REQUIRE(NULL != key);
	m_arrParser.SetSize(1);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&CAppDaliJsonCollectionNames::parseCollection));
}
CAppDaliJsonCollectionNames::~CAppDaliJsonCollectionNames( ) { }


bool CAppDaliJsonCollectionNames::format(struct json_object *dest, const CAppDaliCollectionNames& src) {
	struct json_object* array_items = formatArray(src);
	if (NULL == array_items) {
		ASSERT(NULL != array_items);
		return false;
	}
	json_object_object_add(dest, this->m_key, array_items);	
	return true;
}

CAppDaliJsonCollectionNames::JsonParserResult_t CAppDaliJsonCollectionNames::parse(CAppDaliCollectionNames* dest,const std::string& str) {
	return helper_parse(str, dest);
}

// Protected...
bool CAppDaliJsonCollectionNames::parseCollection(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, this->m_key)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	CAppDaliCollectionNames* const dest = reinterpret_cast<CAppDaliCollectionNames*>(pvData);
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}
	return (parseArray(dest,val));
}

bool CAppDaliJsonCollectionNames::parseArray(CAppDaliCollectionNames* dest,struct json_object *val) {
	REQUIRE(json_type_array == json_object_get_type(val));
	const int arraylen = json_object_array_length(val);
	if (arraylen<=0) {
		return false;
	}
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonArrayItem = json_object_array_get_idx(val, i);
		if (json_type_string != json_object_get_type(jsonArrayItem)) {
			ASSERT(json_type_string != json_object_get_type(jsonArrayItem));			
			return false;
		}		
		const CString str(json_object_get_string(jsonArrayItem));
		dest->setName(i, str);
	}
	return true;
}

struct json_object* CAppDaliJsonCollectionNames::formatArray(const CAppDaliCollectionNames& src) {
	struct json_object* array_items = json_object_new_array();
	if (0 == array_items) {
		return NULL;
	}
	const int max = src.getMax();
	for (int i=0;i<max;i++) {
		char str[64];
		memset(str,'\0',sizeof(str));
		const CString& srcName = src.getName(i);
		if (srcName.IsEmpty()) {
			strcpy(str,"");
		}
		else {
			memcpy(str,(LPCSTR)srcName,sizeof(str)-1);
		}
		json_object_array_add(array_items, json_object_new_string((LPCSTR)str));		
	}
	return array_items;
}
/***********************************************************************************/
};
};
/***********************************************************************************/