#pragma once
#include "json_object.h"	// 3rd party json library.
#include <string>			// std::string	
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliDevices;		
class CAppDaliDevice;		
class CAppDaliDeviceProperties_ControlGear;
class CAppDaliDeviceProperties_InputControlDevice;
class CAppDaliInputControlDeviceInstance;
class CAppDaliInputControlDeviceInstances;
class CAppDaliCollectionNames;
class CAppDaliObjectDT8ColorData;
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliJson_Base {
public:
	CAppDaliJson_Base( );
	virtual ~CAppDaliJson_Base( );

	typedef enum {
		RESULT_SUCCESS = 0,
		RESULT_EC_JSON_PARSE_DATA_NULL = -1,
		RESULT_EC_JSON_PARSE_OBJECT_NULL = -2,
		RESULT_EC_JSON_PARSE_WRONG_TYPE = -3,
		RESULT_EC_JSON_PARSE_NOT_IMPLEMENTED = -4
	} JsonParserResult_t;

public:
	// Dit zijn dezelfde functies als in de 'JsonBaseClass' klasse.
	static bool helper_getValueInt(char* key, struct json_object *val, const char* myKey, int* value);
	static bool helper_getValueHexInt(char* key, struct json_object *val, const char* myKey, int* value);
	static bool helper_getValueString(char* key, struct json_object *val, const char* myKey, CString* value);
	static bool helper_getValueBoolean(char* key, struct json_object *val, const char* myKey, bool* value);

protected:
	JsonParserResult_t helper_parse(const std::string& str, void* pvData);
	JsonParserResult_t helper_parse(struct json_object* object, void* pvData);
	bool loadFile(const char* fileName,std::string* strDest);

protected:
	typedef bool (CAppDaliJson_Base::*PF_PARSER)(char*, struct json_object*, void*);
	CArray<PF_PARSER> m_arrParser;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Export van de DALI devices + andere info...
class CAppDaliJson : public CAppDaliJson_Base {
public:
	CAppDaliJson( );
	virtual ~CAppDaliJson( );

	bool save(const char* strFileName, const CAppDaliDevices& src,
		const CAppDaliCollectionNames& namesGroup,const CAppDaliCollectionNames& namesScenes);
	bool load(const char* strFileName, CAppDaliDevices* ppDest,
		CAppDaliCollectionNames* pDestNamesGroup,CAppDaliCollectionNames* pDestNamesScenes);

private:
	static const char* const KEY_GROUPS_NAMES;
	static const char* const KEY_SCENES_NAMES;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliJsonDevices : public CAppDaliJson_Base {
public:
	CAppDaliJsonDevices( );
	virtual ~CAppDaliJsonDevices( );

	bool format(struct json_object *dest,const CAppDaliDevices& src);
	JsonParserResult_t parse(CAppDaliDevices* dest, const std::string& str);

protected:
	bool parseDevices(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_DEVICES;	
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliJsonDevice : public CAppDaliJson_Base {
public:
	CAppDaliJsonDevice( );
	virtual ~CAppDaliJsonDevice( );

	struct json_object* format(const CAppDaliDevice& src);
	CAppDaliDevice* parseCreate(struct json_object* src);

protected:
	bool parseType(struct json_object *val, CAppDaliDevice** ppDevice);
	bool parseName(char* key, struct json_object *val, void* pvData);
	bool parseShortAddress(char* key, struct json_object *val, void* pvData);
	bool parseGTIN(char* key, struct json_object *val, void* pvData);
	bool parseSerialNumber(char* key, struct json_object *val, void* pvData);
	bool parseVersionFirmware(char* key, struct json_object *val, void* pvData);
	bool parseVersionHardware(char* key, struct json_object *val, void* pvData);
	bool parseErrorStatus(char* key, struct json_object *val, void* pvData);
	bool parseErrorCode(char* key, struct json_object *val, void* pvData);
	bool parseProperties(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_TYPE;				// 0 = Control gear, 1= Input device.
	static const char* const KEY_NAME;				// Naam wordt gegeven in de binding software (niet in DALI device)
	static const char* const KEY_SHORTADDRESS;	
	static const char* const KEY_GTIN;	
	static const char* const KEY_SERIAL_NUMBER;	
	static const char* const KEY_VERSION_FIRMWARE;	
	static const char* const KEY_VERSION_HARDWARE;	
	static const char* const KEY_PROPERTIES;	
	static const char* const KEY_ERROR_STATUS;		// waarde zie enum DaliDeviceErrorStatus_t
	static const char* const KEY_ERROR_CODE;		// waarde zie enum DaliErrorCode_t
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// sedert V16.48: NOTE_DALI_DEVICE_TYPES:
// Een DALI device kan meer dan 1 type ondersteunen.
// Wanneer deze maar 1 type ondersteund:
//	- deviceType : int type.
//	- deviceTypes: [ ]
// Wanneer er meerdere types ondersteund worden.
//	- deviceType : 255
//	- deviceTypes: [ int type, int type]
class CAppDaliJsonDevicePropControlGear : public CAppDaliJson_Base { 
public:
	CAppDaliJsonDevicePropControlGear( );
	~CAppDaliJsonDevicePropControlGear( );

	struct json_object* format(const CAppDaliDevice& src);
	JsonParserResult_t parse(CAppDaliDeviceProperties_ControlGear* dest, struct json_object* src);

protected:
	struct json_object* formatGroups(const CAppDaliDeviceProperties_ControlGear& refproperties);
	struct json_object* formatScenes(const CAppDaliDeviceProperties_ControlGear& refproperties);
	struct json_object* formatDeviceTypes(const CAppDaliDeviceProperties_ControlGear& refproperties);
	struct json_object* formatScenesDT8Color(const CAppDaliDeviceProperties_ControlGear& refproperties);		
		
	bool parseGroups(char* key, struct json_object *val, void* pvData);
	bool parseScenes(char* key, struct json_object *val, void* pvData);
	bool parseDeviceType(char* key, struct json_object *val, void* pvData);
	bool parseFadeTimeRate(char* key, struct json_object *val, void* pvData);
	bool parseMaxLevel(char* key, struct json_object *val, void* pvData);
	bool parseMinLevel(char* key, struct json_object *val, void* pvData);
	bool parsePhysicalMinLevel(char* key, struct json_object *val, void* pvData);
	bool parsePowerOnLevel(char* key, struct json_object *val, void* pvData);
	bool parseSystemFailureLevel(char* key, struct json_object *val, void* pvData);
	bool parseExtendedFadeTime(char* key, struct json_object *val, void* pvData);
	bool parseDimmerCurve(char* key, struct json_object *val, void* pvData);
	bool parseColorFeatures(char* key, struct json_object *val, void* pvData);
	bool parseDeviceTypes(char* key, struct json_object *val, void* pvData);
	bool parsScenesDT8Color(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_GROUPS;	
	static const char* const KEY_SCENES;	
	static const char* const KEY_DEVICE_TYPE;
	static const char* const KEY_DEVICE_FADETIMERATE;
	static const char* const KEY_DEVICE_MAXLEVEL;
	static const char* const KEY_DEVICE_MINLEVEL;
	static const char* const KEY_DEVICE_PHYSICAL_MIN_LEVEL;
	static const char* const KEY_DEVICE_POWER_ON_LEVEL;
	static const char* const KEY_DEVICE_SYSTEM_FAILURE_LEVEL;
	static const char* const KEY_DEVICE_EXTENDED_FADETIME;
	static const char* const KEY_DEVICE_DIMMERCURVE;
	static const char* const KEY_DEVICE_COLORFEATURES;
	static const char* const KEY_DEVICE_TYPES;						// zie NOTE_DALI_DEVICE_TYPES
	static const char* const KEY_SCENES_COLOR;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliJsonDevicePropControlGearDT8sceneColorInfo : public CAppDaliJson_Base { 
public:
	CAppDaliJsonDevicePropControlGearDT8sceneColorInfo( );
	~CAppDaliJsonDevicePropControlGearDT8sceneColorInfo( );

	struct json_object* format(int sceneNumber,const CAppDaliObjectDT8ColorData& src);
	JsonParserResult_t parse(CAppDaliObjectDT8ColorData* dest, struct json_object* src);

	int getAddress( ) const {
		return this->m_address;
	}

protected:
	bool parseAddress(char* key, struct json_object *val, void* pvData);
	bool parseColorType(char* key, struct json_object *val, void* pvData);
	bool parseColorTemperature(char* key, struct json_object *val, void* pvData);
	bool parseColorCoordinateX(char* key, struct json_object *val, void* pvData);
	bool parseColorCoordinateY(char* key, struct json_object *val, void* pvData);
	bool parseColorRed(char* key, struct json_object *val, void* pvData);
	bool parseColorGreen(char* key, struct json_object *val, void* pvData);
	bool parseColorBlue(char* key, struct json_object *val, void* pvData);
	bool parseColorWhite(char* key, struct json_object *val, void* pvData);
	bool parseColorAmber(char* key, struct json_object *val, void* pvData);
	bool parseColorFreeColor(char* key, struct json_object *val, void* pvData);
	bool parseArrColorChannels(char* key, struct json_object *val, void* pvData);

private:
	int m_address;

	static const char* const KEY_ADDRESS;
	static const char* const KEY_COLOR_TYPE;
	static const char* const KEY_ACTIVE;					// Bepaalt of er door de cfg'er een sfeer moet aangemaakt worden - WORDT NIER GEPARSED!!
	// Color type: Temperature_tc
	static const char* const KEY_COLOR_TEMPERATURE;			// Color type: Temperature_tc
	// Color type: Coordinate XY
	static const char* const KEY_COLOR_COORDINATE_X;		// Color type: Coordinate XY
	static const char* const KEY_COLOR_COORDINATE_Y;		// Color type: Coordinate XY
	// Color type: Coordinate RGBWAF
	static const char* const KEY_COLOR_RED;					// Color type: Coordinate RGBWAF
	static const char* const KEY_COLOR_GREEN;				// Color type: Coordinate RGBWAF
	static const char* const KEY_COLOR_BLUE;				// Color type: Coordinate RGBWAF
	static const char* const KEY_COLOR_WHITE;				// Color type: Coordinate RGBWAF
	static const char* const KEY_COLOR_AMBER;				// Color type: Coordinate RGBWAF
	static const char* const KEY_COLOR_FREECOLOR;			// Color type: Coordinate RGBWAF
	// Color type: Primary Channels N
	static const char* const KEY_COLOR_CHANNELS;			// Color type: Primary Channels N
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliJsonDeviceInputDevice : public CAppDaliJson_Base {
public:
	CAppDaliJsonDeviceInputDevice();
	~CAppDaliJsonDeviceInputDevice();

	struct json_object* format(const CAppDaliDevice& src);
	JsonParserResult_t parse(CAppDaliDeviceProperties_InputControlDevice* dest, struct json_object* src);

protected:
	struct json_object* formatGroups(const CAppDaliDeviceProperties_InputControlDevice& refproperties);

	bool parseGroups(char* key, struct json_object *val, void* pvData);
	bool parseAppControllerEnabled(char* key, struct json_object *val, void* pvData);
	bool parsePowerCycleNotify(char* key, struct json_object *val, void* pvData);
	bool parseCapabilities(char* key, struct json_object *val, void* pvData);
	bool parseInstances(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_GROUPS;
	static const char* const KEY_APPCONTROLLER_ENABLED;
	static const char* const KEY_POWERCYCLE_NOTIFY;
	static const char* const KEY_CAPABILITIES;
	static const char* const KEY_INSTANCES;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliJsonDeviceInputDeviceInstances : public CAppDaliJson_Base { 
public:
	CAppDaliJsonDeviceInputDeviceInstances();
	~CAppDaliJsonDeviceInputDeviceInstances();

	struct json_object* format(const CAppDaliInputControlDeviceInstances& src);	
	JsonParserResult_t parse(CAppDaliInputControlDeviceInstances* dest, const std::string& str);

protected:
	bool CAppDaliJsonDeviceInputDeviceInstances::parseArrayInstances(char* key, struct json_object *val, void* pvData);


private:
	friend class CAppDaliJsonDeviceInputDevice;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

class CAppDaliJsonDeviceInputDeviceInstance : public CAppDaliJson_Base {
public:
	CAppDaliJsonDeviceInputDeviceInstance();
	~CAppDaliJsonDeviceInputDeviceInstance();

	struct json_object* format(const CAppDaliInputControlDeviceInstance& src);
	CAppDaliInputControlDeviceInstance* parseCreateInstance(struct json_object *val);

protected:
	struct json_object* formatGroups(const CAppDaliInputControlDeviceInstance& src);
	bool parseInstanceNumber(struct json_object *val, CAppDaliInputControlDeviceInstance** ppProperties);
	// bool parseInstanceNumber(char* key, struct json_object *val, void* pvData);
	bool parseInstanceName(char* key, struct json_object *val, void* pvData);
	bool parseInstanceEnabled(char* key, struct json_object *val, void* pvData);
	bool parseInstanceType(char* key, struct json_object *val, void* pvData);
	bool parseInstanceScheme(char* key, struct json_object *val, void* pvData);
	bool parseInstanceFilter(char* key, struct json_object *val, void* pvData);
	bool parseInstanceGroups(char* key, struct json_object *val, void* pvData);
	bool parseInstancePriority(char* key, struct json_object *val, void* pvData);
	bool parseInstanceResolution(char* key, struct json_object *val, void* pvData);

private:
	static const char* const KEY_INSTANCE_NUMBER;
	static const char* const KEY_INSTANCE_NAME;				// Naam wordt gegeven in de binding software (niet in DALI device)
	static const char* const KEY_INSTANCE_ENABLED;
	static const char* const KEY_INSTANCE_TYPE;
	static const char* const KEY_INSTANCE_SCHEME;
	static const char* const KEY_INSTANCE_FILTER;
	static const char* const KEY_INSTANCE_GROUPS;
	static const char* const KEY_INSTANCE_PRIORITY;
	static const char* const KEY_INSTANCE_RESOLUTION;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliJsonCollectionNames : public CAppDaliJson_Base {
public:
	CAppDaliJsonCollectionNames(const char* key);
	~CAppDaliJsonCollectionNames( );

	bool format(struct json_object *dest,const CAppDaliCollectionNames& src);
	JsonParserResult_t parse(CAppDaliCollectionNames* dest, const std::string& str);
	
protected:
	struct json_object* formatArray(const CAppDaliCollectionNames& src);
	bool parseCollection(char* key, struct json_object *val, void* pvData);
	bool parseArray(CAppDaliCollectionNames* dest,struct json_object *val);

private:
	const char* m_key;
};
/***********************************************************************************/

/***********************************************************************************/
};
};
/***********************************************************************************/