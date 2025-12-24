#pragma once
#include "appdali_fwd.h"
#include "utils/app/appjson.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class AppDaliBridgeDevice {
public:
	AppDaliBridgeDevice();
	virtual ~AppDaliBridgeDevice();

	typedef enum {
		TYPE_DALI_DEVICE_UNKNOWN = -1,
		TYPE_DALI_DEVICE_CONTROLGEAR = 0,
		TYPE_DALI_DEVICE_INPUTCONTROLDEVICE,
		TYPE_DALI_GROUP_CONTROLGEAR,
		TYPE_DALI_SCENE	
	} Type_t;

	void setNodeAddress(int nodeAddress) {
		this->m_nodeAddress = nodeAddress;
	}
	int getNodeAddress() const {
		return this->m_nodeAddress;
	}

	void setUnitAddress(int value) {
		this->m_unitAddress = value;
	}
	int getUnitAddress() const {
		return this->m_unitAddress;
	}

	void setDaliAddress(int value) {
		this->m_daliAddress = value;
	}
	int getDaliAddress() const {
		return this->m_daliAddress;
	}

	void setType(Type_t type) {
		this->m_type = type;
	}
	Type_t getType() const {
		return this->m_type;
	}

private:
	int m_nodeAddress;
	int m_unitAddress;
	int m_daliAddress;
	Type_t m_type;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class AppDaliBridgeDevices {
public:
	AppDaliBridgeDevices( );
	virtual ~AppDaliBridgeDevices();
	
	bool add(AppDaliBridgeDevice* device);
	int getCount() const;
	bool get(int index, AppDaliBridgeDevice** ppdevice);
	bool find(int nodeAddress,int unitAddress, AppDaliBridgeDevice** ppDevice);

private:
	CList<AppDaliBridgeDevice*> m_list;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class AppDaliJsonBridgeDevices : public CAppJsonParser {
public:
	AppDaliJsonBridgeDevices(int nodeAddress);
	~AppDaliJsonBridgeDevices( );

	// Inladen van de bridge config ifv het node adres
	bool load(const char* fileName,AppDaliBridgeDevices* p);

protected:
	JsonParserResult_t parse(AppDaliBridgeDevices* dest, const std::string& str);

	bool parseDevices(char* key, struct json_object *val, void* pvData);
	bool parseGroups(char* key, struct json_object *val, void* pvData);
	bool parseScenes(char* key, struct json_object *val, void* pvData);
	bool helper_parseArray(AppDaliBridgeDevices* dest,struct json_object *val,AppDaliBridgeDevice::Type_t type);

private:
	int m_nodeAddress;

	static const char* const KEY_COLLECTION_DEVICES;
	static const char* const KEY_COLLECTION_GROUPS;
	static const char* const KEY_COLLECTION_SCENES;

	typedef CAppJsonParser Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class AppDaliJsonBridgeDevice : public CAppJsonParser {
public:
	AppDaliJsonBridgeDevice(int nodeAddress);
	~AppDaliJsonBridgeDevice( );

	AppDaliBridgeDevice* parseCreate(struct json_object* src,AppDaliBridgeDevice::Type_t type);

protected:
	bool parseType(struct json_object *val, void* pvData);

	bool parseAddressDali(char* key, struct json_object *val, void* pvData);
	bool parseAddressUnit(char* key, struct json_object *val, void* pvData);	

private:
	int m_nodeAddress;

	static const char* const KEY_ITEM_ADDRESS_DALI;
	static const char* const KEY_ITEM_ADDRESS_UNIT;
	static const char* const KEY_ITEM_TYPE;

	typedef CAppJsonParser Base;
};

/***********************************************************************************/
};	/* namespace DUOTECNO */
};	/* namespace DALI */
/***********************************************************************************/