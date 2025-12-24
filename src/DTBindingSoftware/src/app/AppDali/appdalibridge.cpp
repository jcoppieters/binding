#include "StdAfx.h"
#include "BindingSoftware.h"
#include "appdalibridge.h"

/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
AppDaliBridgeDevice::AppDaliBridgeDevice() :
	m_nodeAddress(-1), m_unitAddress(-1), m_daliAddress(-1), m_type(TYPE_DALI_DEVICE_UNKNOWN)  { }
AppDaliBridgeDevice::~AppDaliBridgeDevice() { }
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
AppDaliBridgeDevices::AppDaliBridgeDevices() : m_list( )  { }
AppDaliBridgeDevices::~AppDaliBridgeDevices() { }

bool AppDaliBridgeDevices::add(AppDaliBridgeDevice* device) {
	if (0 == device) {
		REQUIRE(NULL != device);
		return false;
	}	
	m_list.AddTail(device);
	return true;
}

int AppDaliBridgeDevices::getCount() const {
	return this->m_list.GetCount();
}

bool AppDaliBridgeDevices::get(int index, AppDaliBridgeDevice** ppDevice) {
	if (0 == ppDevice) {
		return false;
	}
	if (index>=m_list.GetCount()) {
		return false;
	}
	const POSITION pos = m_list.FindIndex( index );
	if (pos == NULL) {
		return false;
	}
	*ppDevice = m_list.GetAt(pos);
	return true;
}

bool AppDaliBridgeDevices::find(int nodeAddress,int unitAddress, AppDaliBridgeDevice** ppDevice) {
	POSITION pos = m_list.GetHeadPosition();
	while(pos!=NULL)  {
		AppDaliBridgeDevice* pEntry = 0;
		if ((pEntry=m_list.GetNext(pos)) != NULL) {	
			if ((nodeAddress == pEntry->getNodeAddress()) && (unitAddress == pEntry->getUnitAddress())) {
				if (0 != ppDevice) {
					*ppDevice = pEntry;
				}
				return true;
			}			
		}
	}
	return false;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const char* const AppDaliJsonBridgeDevices::KEY_COLLECTION_DEVICES = "devicesDali";
const char* const AppDaliJsonBridgeDevices::KEY_COLLECTION_GROUPS = "groupsDali";
const char* const AppDaliJsonBridgeDevices::KEY_COLLECTION_SCENES = "scenesDali";
/***********************************************************************************/
AppDaliJsonBridgeDevices::AppDaliJsonBridgeDevices(int nodeAddress) : Base(), m_nodeAddress(nodeAddress) {
	REQUIRE(this->m_nodeAddress>=0);
	m_arrParser.SetSize(3);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&AppDaliJsonBridgeDevices::parseDevices));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&AppDaliJsonBridgeDevices::parseGroups));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&AppDaliJsonBridgeDevices::parseScenes));
}

AppDaliJsonBridgeDevices::~AppDaliJsonBridgeDevices() { }


// Inladen van de bridge config ifv het node adres
bool AppDaliJsonBridgeDevices::load(const char* strFileName, AppDaliBridgeDevices* pDest) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}
	std::string strFileData;
	if (!this->loadFile(strFileName,&strFileData)) {
		return false;
	}
	if (strFileData.length() <= 0) {
		return false;
	}
	{		
		const JsonParserResult_t res = this->parse(pDest,this->removeBOM(strFileData));
		if (RESULT_SUCCESS != res) {	
			return false;
		}
	}
	return false;
}

AppDaliJsonBridgeDevices::JsonParserResult_t AppDaliJsonBridgeDevices::parse(AppDaliBridgeDevices* dest, const std::string& str) {
	return this->helper_parse(str, dest);
}

bool AppDaliJsonBridgeDevices::parseDevices(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY_COLLECTION_DEVICES)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	// type = TYPE_DALI_DEVICE_UNKNOWN: Oppassen: Parsen van type bepaalt control device / control gear
	return helper_parseArray(reinterpret_cast<AppDaliBridgeDevices*>(pvData), val,AppDaliBridgeDevice::TYPE_DALI_DEVICE_UNKNOWN);
}

bool AppDaliJsonBridgeDevices::parseGroups(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY_COLLECTION_GROUPS)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	// type = TYPE_DALI_GROUP_CONTROLGEAR: Geen type om te parsen dus altijd group control gear
	return helper_parseArray(reinterpret_cast<AppDaliBridgeDevices*>(pvData), val,AppDaliBridgeDevice::TYPE_DALI_GROUP_CONTROLGEAR);
}

bool AppDaliJsonBridgeDevices::parseScenes(char* key, struct json_object *val, void* pvData) {
	if (0 != strcmp(key, KEY_COLLECTION_SCENES)) {
		return false;
	}
	if (json_type_array != json_object_get_type(val)) {
		ASSERT(json_type_array != json_object_get_type(val));
		return false;
	}
	// type = TYPE_DALI_SCENE: Geen type om te parsen dus altijd scene
	return helper_parseArray(reinterpret_cast<AppDaliBridgeDevices*>(pvData), val,AppDaliBridgeDevice::TYPE_DALI_SCENE);
}

bool AppDaliJsonBridgeDevices::helper_parseArray(AppDaliBridgeDevices* dest,struct json_object *val,AppDaliBridgeDevice::Type_t type) {
	if (0 == dest) {
		REQUIRE(0 != dest);
		return false;
	}
	const int arraylen = json_object_array_length(val);
	for (int i = 0; i < arraylen; i++) {
		struct json_object* jsonObjectNode = json_object_array_get_idx(val, i);
		if (json_type_object != json_object_get_type(jsonObjectNode)) {
			ASSERT(json_type_object != json_object_get_type(jsonObjectNode));
			return false;
		}
		AppDaliJsonBridgeDevice parser(this->m_nodeAddress);
		AppDaliBridgeDevice* const pDevice = parser.parseCreate(jsonObjectNode,type);
		if (NULL == pDevice) {
			ASSERT(NULL != pDevice); 
			return false;
		}		
		dest->add(pDevice);
	}
	return true;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const char* const AppDaliJsonBridgeDevice::KEY_ITEM_ADDRESS_DALI = "daliAddress";
const char* const AppDaliJsonBridgeDevice::KEY_ITEM_ADDRESS_UNIT = "unitAddress";
const char* const AppDaliJsonBridgeDevice::KEY_ITEM_TYPE = "type";		// Enkel aanwezig bij het parsen van een device.
/***********************************************************************************/
AppDaliJsonBridgeDevice::AppDaliJsonBridgeDevice(int nodeAddress) : Base(), m_nodeAddress(nodeAddress) {
	REQUIRE(this->m_nodeAddress>=0);
	m_arrParser.SetSize(2);
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&AppDaliJsonBridgeDevice::parseAddressDali));
	m_arrParser.Add(reinterpret_cast<PF_PARSER>(&AppDaliJsonBridgeDevice::parseAddressUnit));
}

AppDaliJsonBridgeDevice::~AppDaliJsonBridgeDevice( ) { }

AppDaliBridgeDevice* AppDaliJsonBridgeDevice::parseCreate(struct json_object* src,AppDaliBridgeDevice::Type_t type) {		
	AppDaliBridgeDevice* const pDevice = new AppDaliBridgeDevice( );
	if (NULL == pDevice) {
		ASSERT(NULL != pDevice);
		return false;
	}
	if (type == AppDaliBridgeDevice::TYPE_DALI_DEVICE_UNKNOWN) {
		if (false == parseType(src, pDevice)) {
			pDevice->setType(AppDaliBridgeDevice::TYPE_DALI_DEVICE_CONTROLGEAR);
		}
	}
	else {
		pDevice->setType(type);
	}

	INVARIANT(this->m_nodeAddress >= 0);
	pDevice->setNodeAddress(this->m_nodeAddress);

	INVARIANT(NULL != pDevice);
	INVARIANT(AppDaliBridgeDevice::TYPE_DALI_DEVICE_UNKNOWN != pDevice->getType());

	this->helper_parse(src,pDevice);
	ENSURE(NULL != pDevice);
	return pDevice;
}

bool AppDaliJsonBridgeDevice::parseAddressDali(char* key, struct json_object *val, void* pvData) {
	int value = -1;
	if (!CAppJsonParser::helper_getValueInt(key, val, KEY_ITEM_ADDRESS_UNIT, &value)) {
		return false;
	}
	AppDaliBridgeDevice* const dest = reinterpret_cast<AppDaliBridgeDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	dest->setUnitAddress(value);
	return true;
}

bool AppDaliJsonBridgeDevice::parseAddressUnit(char* key, struct json_object *val, void* pvData) {
	int value = -1;
	if (!CAppJsonParser::helper_getValueInt(key, val, KEY_ITEM_ADDRESS_DALI, &value)) {
		return false;
	}
	AppDaliBridgeDevice* const dest = reinterpret_cast<AppDaliBridgeDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	dest->setDaliAddress(value);
	return true;
}

bool AppDaliJsonBridgeDevice::parseType(struct json_object *val, void* pvData) {
	struct json_object* jsonObject = json_object_object_get(val,KEY_ITEM_TYPE);
	if (NULL == jsonObject) {
		ASSERT(NULL != jsonObject);
		return false;
	}
	AppDaliBridgeDevice* const dest = reinterpret_cast<AppDaliBridgeDevice*>(pvData);
	if (0 == dest) {
		ASSERT(NULL != dest);
		return false;
	}
	const json_type type = json_object_get_type(jsonObject);
	if (json_type_int != type) {
		ASSERT(0);
		return false;
	}	
	const int value = json_object_get_int(jsonObject);
	switch(value) {
		case DALI_DEVICE_TYPE_CONTROLGEAR: {
			dest->setType(AppDaliBridgeDevice::TYPE_DALI_DEVICE_CONTROLGEAR);						   
		} break;
		case DALI_DEVICE_TYPE_INPUTCONTROL: {
			dest->setType(AppDaliBridgeDevice::TYPE_DALI_DEVICE_INPUTCONTROLDEVICE);			
		} break;
		default: {
			dest->setType(AppDaliBridgeDevice::TYPE_DALI_DEVICE_CONTROLGEAR);
			ASSERT(0); 
			return false;
		};
	}
	return true;
}
/***********************************************************************************/
}; /*  namespace DUOTECNO */
}; /*  namespace DALI */
/***********************************************************************************/