#pragma once
#include "appdali_fwd.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliDevices;
class CAppDaliDevice;
class CAppDaliDeviceProperties;
class CAppDaliDeviceProperties_ControlGear;
class CAppDaliInputControlDeviceInstance;
class CAppDaliObjectDT8ColorData;
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
struct CAppDaliDevices_Utils {

public:
	// Utility functies om een device aan te passen.
	// return waarde true wanneer aanpassing gebeurd is.
	static bool addDeviceToGroup(CAppDaliDevices* pDatabase, DaliDeviceType_t type, int shortAddress,
		int groupAddress);
	static bool removeDeviceFromGroup(CAppDaliDevices* pDatabase,DaliDeviceType_t type, int shortAddress,
		int groupAddress);

	static bool isDeviceGroupMember(CAppDaliDevices* pDatabase,DaliDeviceType_t type, int shortAddress, int groupAddress);

	// Utility functies om een device aan te passen.
	// return waarde true wanneer aanpassing gebeurd is.
	static bool setDeviceSceneInfo(CAppDaliDevices* pDatabase,DaliDeviceType_t type, int shortAddress,
		int sceneAddress,uint8_t data);

	// Utility functie om de sceneinfo in alle devices te wijzigen.
	// Nodig wanneer er een scene geconfigureerd wordt via een broadcast bericht.
	// Kleur data wordt aangepast indien het DT8 device het kleur object ondersteund.
	static bool setAllDevicesSceneInfo(CAppDaliDevices* pDatabase,  int sceneAddress, uint8_t intensity, const CAppDaliObjectDT8ColorData& refColorData);

	// Namen van de devices en van de device input instances copieren naar de dest database...
	static bool mergeNames(CAppDaliDevices* pDest,CAppDaliDevices& src);
	
protected:
	static bool mergeNames(CAppDaliDevices* pDest,CAppDaliDevices& src,DaliDeviceType_t type);
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
struct CAppDaliDevice_Utils {
	// Utility functies om een device aan te passen.
	// return waarde true wanneer aanpassing gebeurd is.
	static bool addDeviceToGroup(CAppDaliDevice& refDevice,int groupAddress);
	static bool removeDeviceFromGroup(CAppDaliDevice& refDevice,int groupAddress);
	static bool isDeviceGroupMember(CAppDaliDevice& refDevice,int groupAddress);
	static bool setDeviceSceneInfo(CAppDaliDevice& refDevice,int sceneAddress,uint8_t data);
	static bool getControlGearProperties(CAppDaliDevice& refDevice,CAppDaliDeviceProperties_ControlGear** ppProperties);
	static bool mergeNames(CAppDaliDevice* pDest,CAppDaliDevice& src);

	// Om te bepalen of een device het DALI-2 protocol ondersteund zijn er geen queries in het
	// protocol die deze info kunnen opvragen.
	// Dit kan enkel bepaald worden of een device geantwoord heeft op nieuwe queries die enkel aanwezig
	// zijn in het DALI-2 protocol.
	static bool supportDALIVersion2(CAppDaliDevice& refDevice);

	// Bepalen of het DALI control gear device DT8 type is.
	static bool isControlGearDeviceTypeDT8(CAppDaliDevice& refDevice);

	// Default waarden voor een device.
	static void setDefaults(CAppDaliInputControlDeviceInstance* pDest);
	static void setDefaults(CAppDaliDeviceProperties_ControlGear* pDest);
	static void setScenesDefaults(CAppDaliDeviceProperties_ControlGear* pDest);

	// Scene aanpassen.
	// Nodig wanneer er een scene geconfigureerd wordt via een broadcast bericht.
	// Kleur data wordt aangepast indien het DT8 device het kleur object ondersteund.
	static bool setDeviceSceneInfo(CAppDaliDevice* pDevice, int sceneAddress, uint8_t intensity,const CAppDaliObjectDT8ColorData& refColorData);

protected:
	static bool mergeNamesInputDeviceInstances(CAppDaliDevice* pDest,CAppDaliDevice& src);
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliDevices {
public:
	CAppDaliDevices( );
	virtual ~CAppDaliDevices( );

	void erase( );
	bool find(DaliDeviceType_t type, int shortAddress, CAppDaliDevice **ppDevice) const;
	bool add(CAppDaliDevice* pNewDevice);
	int getCount() const;
	bool get(int index,CAppDaliDevice **ppDevice) const;
	bool remove(DaliDeviceType_t type, int shortAddress);

	CAppDaliDevices* clone( );
	
private:
	// OPPASSEN: Nieuwe properties bijvoegen in de clone()
	CList<CAppDaliDevice*> m_list;
};
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliDevice {
public:
	CAppDaliDevice(DaliDeviceType_t type);
	virtual ~CAppDaliDevice( );

	// properties...
	DaliDeviceType_t getType() const;

	void setName(const CString& str);
	const CString& getName( ) const;

	int getShortAddress( ) const;
	void setShortAddress(int v);

	unsigned long long getGTIN( ) const;
	void setGTIN(unsigned long long v);

	unsigned long long getSerialcode( ) const;
	void setSerialcode(unsigned long long v);

	int getVersionFirmware( ) const;
	void setVersionFirmware(int v);

	int getVersionHardware( ) const;
	void setVersionHardware(int v);

	void setErrorStatus(DaliDeviceErrorStatus_t status);
	DaliDeviceErrorStatus_t getErrorStatus( ) const;

	void setErrorCode(DaliErrorCode_t errorCode);
	DaliErrorCode_t getErrorCode( ) const;

	// Opvragen van de device input/output properties...
	CAppDaliDeviceProperties& getProperties();
	const CAppDaliDeviceProperties& getProperties() const;

	CAppDaliDevice* clone( );

	// Utilities...
	static bool getStrGTIN(const CAppDaliDevice& ref,char* pszDest,int maxLength);
	static bool getStrSerialCode(const CAppDaliDevice& ref,char* pszDest,int maxLength);

private:
	// OPPASSEN: Nieuwe properties bijvoegen in de clone()
	const DaliDeviceType_t m_type;
	CString m_strName;											// Naam wordt gegeven in de Duotecno software.
	CAppDaliDeviceProperties* m_properties;		 
	int m_shortAddress;
	unsigned long long m_GTIN;
	unsigned long long m_serialCode;
	int m_versionFirmware;							
	int m_versionHardware;
	DaliDeviceErrorStatus_t m_errorStatus;						// Globale error code.
	DaliErrorCode_t m_errorCode;								// extended error code.
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliDeviceProperties {
public:
	CAppDaliDeviceProperties( );
	virtual ~CAppDaliDeviceProperties( );

	int getDeviceStatus( );
	void setDeviceStatus(int v);

	int getOperatingmode( );
	void setOperatingMode(int v);

	int getVersionNumber( );
	void setVersionNumber(int v);

	static bool copy(CAppDaliDeviceProperties* pDest,const CAppDaliDeviceProperties& src);

private:	
	// OPPASSEN: Nieuwe properties bijvoegen in de clone()
	int m_deviceStatus;						// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_operatingMode;					// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_versionNumber;					// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliDeviceProperties_ControlGear : public CAppDaliDeviceProperties {
public:
	CAppDaliDeviceProperties_ControlGear( );
	virtual ~CAppDaliDeviceProperties_ControlGear( );

	enum { MAX_NR_SCENES = 16};

	// Bitfield group info: 16 groepen.
	void setGroupInfo(uint16_t groupInfo);		
	uint16_t getGroupInfo( ) const;

	int getDeviceType( ) const;
	void setDeviceType(int v);

	int getLevelMin( ) const;
	void setLevelMin(int v);

	int getLevelMax( )const;
	void setLevelMax(int v);

	int getFadeTimeRate( ) const;
	void setFadeTimeRate(int v);

	static void setFadeTime(CAppDaliDeviceProperties_ControlGear* p,int v);
	static void setFadeRate(CAppDaliDeviceProperties_ControlGear* p,int v);
	static int getFadeTime(const CAppDaliDeviceProperties_ControlGear& r);
	static int getFadeRate(const CAppDaliDeviceProperties_ControlGear& r);

	int getPhysicalMinValue( ) const;
	void setPhysicalMinValue(int v);

	int getLevelPowerOn( ) const;
	void setLevelPowerOn(int v);

	int getLevelSystemFailure( ) const;
	void setLevelSystemFailure(int v);

	int getExtendedFadeTime( ) const;
	void setExtendedFadeTime(int v);

	// scene info
	void setSceneLevel(int sceneNumber,int value);
	int getSceneLevel(int sceneNumber) const;

	void setActualLevel(int actualLevel);
	int getActualLevel() const;

	void setDimmingCurve(int value);
	int getDimmingCurve() const;

	const CList<int>& getDeviceTypes() const;
	void setDeviceTypes(CList<int>& list);

	void setColorFeatures(int v);
	int getColorFeatures( ) const;

	/// Utility functies....

	// Utiltity functie om te bepalen of een sfeer actief is.
	// Houdt rekening met actual level en DT8 kleur objecten.
	static bool isSceneActive(const CAppDaliDeviceProperties_ControlGear& r, int sceneNumber);

	// Utiltity functie om te bepalen of DT8 color objecten ondersteund zijn.
	static bool isDT8ColorObjectSupported(const CAppDaliDeviceProperties_ControlGear& r);

	// Utiltity functie om te bepalen of DT8 color objecten ondersteund zijn.
	// Controleert de control gear color features.
	static bool isDT8ColorObjectSupported(const CAppDaliDeviceProperties_ControlGear& r, int sceneColorType);

	// Opvragen of color data type ondersteund en aantal channels.
	// Nakijken color features
	static int getColorFeaturesNrOfRGBWAFChannels(const CAppDaliDeviceProperties_ControlGear& r);
	static int getColorFeaturesNrOfPrimaryNChannels(const CAppDaliDeviceProperties_ControlGear& r);
	static bool getColorFeaturesSupportColorTemperatureTC(const CAppDaliDeviceProperties_ControlGear& r);
	static bool getColorFeaturesSupportColorCoordinatesXY(const CAppDaliDeviceProperties_ControlGear& r);

	// Utility functie om de DT8 Color scene info te wissen.
	static void clearDT8SceneColorData(CAppDaliDeviceProperties_ControlGear* pDest);
	static void clearDT8SceneColorData(CAppDaliDeviceProperties_ControlGear* pDest,int sceneNumber);

	// DT8-8 Scene color info.
	typedef CMap<int,int, CAppDaliObjectDT8ColorData*,CAppDaliObjectDT8ColorData*> CMapDT8ColorScenesInfo;

	// Volledige map.
	void setDT8SceneColorInfo(const CMapDT8ColorScenesInfo& src);
	const CMapDT8ColorScenesInfo& getT8SceneColorInfo() const;

	// Individueel item.
	bool setDT8SceneColorInfo(int sceneNumber, const CAppDaliObjectDT8ColorData& src);
	const CAppDaliObjectDT8ColorData& getDT8SceneColorInfo(int sceneNumber) const;

	// Utility functie om op te vragen of een device aanwezig is in een group...
	static bool isGroupMember(CAppDaliDeviceProperties_ControlGear& refDevice,int groupNumber);

	// Utility functie om te vragen of een device een bepaald type ondersteund.
	// Control gear devices ondersteunen soms meerdere types DT6,DT8.
	static bool isSupported(CAppDaliDeviceProperties_ControlGear& refDevice,int deviceType);

	enum {
		FLAG_COPY_PROPERTIES = (1<<0),
		FLAG_COPY_SCENES = (1<<1),
	};

	CAppDaliDeviceProperties_ControlGear* clone( );
	static bool copy(CAppDaliDeviceProperties_ControlGear* pDest,const CAppDaliDeviceProperties_ControlGear& src,
		uint8_t flags = (FLAG_COPY_PROPERTIES +  FLAG_COPY_SCENES));

protected:	
	static void eraseColorDT8SceneInfo(CMapDT8ColorScenesInfo* pDest);
	static void copyColorDT8SceneInfo(CMapDT8ColorScenesInfo* pDest,const CMapDT8ColorScenesInfo& src);

private:
	// OPPASSEN: Nieuwe properties bijvoegen in de clone()
	uint16_t m_bitfieldGroupInfo;			// Bit field die bepaalt in welke groepen het device aanwezig is.		
	int m_deviceType;						// -1 is onbekend, waarde 0-0xff zie DALI specificatie.	
	int m_levelMin;							// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_levelMax;							// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_fadeTimeRate;						// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_physicalMinValue;					// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_levelPowerOn;						// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_levelSystemFailure;				// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_extendedFadeTime;					// -1 is onbekend, waarde 0-0xff zie DALI specificatie.
	int m_dimmingCurve;						// zie enum DaliControlGearDimCurve_t: 
	int m_actualLevel;						// Runtime property - wordt niet opgeslagen - default 0
	int m_colorFeatures;					// -1 is onbekend, dt8 specifief
	CList<int> m_deviceTypes;				// Wanneer een device meerdere device types ondersteund.
	uint8_t m_sceneInfo[MAX_NR_SCENES];				// 255 MASK is niet gebruikt anders een actual level 0-254
	CMapDT8ColorScenesInfo m_mapColorScenes;		// DT8 Object Color: Key = scene number, Data is ColorObjectData.
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliInputControlDeviceInstances;
/***********************************************************************************/
// Alle properties van de control input devices.
//	- Initialiseren op '-1'.
//	- Indien onbekend dan op '-1' plaatsen.
class CAppDaliDeviceProperties_InputControlDevice : public CAppDaliDeviceProperties {
public:
	CAppDaliDeviceProperties_InputControlDevice( );
	CAppDaliDeviceProperties_InputControlDevice(CAppDaliInputControlDeviceInstances* pInstances);
	virtual ~CAppDaliDeviceProperties_InputControlDevice( );

	// Bitfield group info: 32 groepen.
	void setGroupInfo(uint32_t groupInfo);		
	uint32_t getGroupInfo( ) const;
	// zie isGroupMember( ) gebruiken...

	bool getAppControllerEnabled( ) const;
	void setAppControllerEnabled(bool v);

	bool getPowerCycleNotification() const;
	void setPowerCycleNotification(bool v);

	int getDeviceCapabilities( ) const;
	void setDeviceCapabilities(int v);

	uint32_t getRandomAddress() const;
	void setRandomAddress(uint32_t v);

	CAppDaliInputControlDeviceInstances& getInstances();
	const CAppDaliInputControlDeviceInstances& getInstances() const;

	// Utility functie om op te vragen of een device aanwezig is in een group...
	static bool isGroupMember(CAppDaliDeviceProperties_InputControlDevice& refDevice,int groupNumber);

	CAppDaliDeviceProperties_InputControlDevice* clone( );
	static bool copy(CAppDaliDeviceProperties_InputControlDevice* pDest,const CAppDaliDeviceProperties_InputControlDevice& src);

private:
	// OPPASSEN: Nieuwe properties bijvoegen in de clone()
	CAppDaliInputControlDeviceInstances* const m_pInstances;
	uint32_t m_bitfieldGroupInfo;			// Bit field die bepaalt in welke groepen het device aanwezig is.	
	bool m_appControllerEnabled;			// default value: false
	bool m_powerCycleNotification;			// deafult value: false
	int m_deviceCapabilities;				// -1 is onbekend, waarde 0-0xff zie DALI specificatie.	
	uint32_t m_randomAddress;				// data.
	// Firmware & hardware versie in de CAppDaliDevice klasse.
};


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliInputControlDeviceInstance;
/***********************************************************************************/
class CAppDaliInputControlDeviceInstances {
public:
	CAppDaliInputControlDeviceInstances( );
	~CAppDaliInputControlDeviceInstances( );

	enum {
		FIRST_INSTANCE_NUMBER = 0,
		LAST_INSTANCE_NUMBER = 31
	};

	// Properties...
	int getNumberOfInstances() const;
	void setNumberOfInstances(int v);

	// het aantal instances in de map.
	int getCountNumberOfInstances() const;

	// Alle items wissen en het geheugen de-alloceren.
	void erase( );

	// Zoeken naar een item.
	// bool find(int instanceNumber,CAppDaliInputControlDeviceInstance** pp);		
	bool find(int instanceNumber,CAppDaliInputControlDeviceInstance** pp) const;		

	// Client alloceerd de instantie, wanneer result is false moet de client deze dealloceren.
	// Wanneer er reeds een instantie aanwezihg is voor een instance number dan return false.
	bool add(int instanceNumber,CAppDaliInputControlDeviceInstance* p);			


	CAppDaliInputControlDeviceInstances* clone( );
	static bool copy(CAppDaliInputControlDeviceInstances* pDest,const CAppDaliInputControlDeviceInstances& src);

private:
	// OPPASSEN: Nieuwe properties bijvoegen in de clone()
	int m_numberOfInstance;					// -1 is onbekend, waarde 0 - 31
	CMap<int,int,CAppDaliInputControlDeviceInstance*, CAppDaliInputControlDeviceInstance*> m_map;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/	
class CAppDaliInputControlDeviceInstance {
public:
	CAppDaliInputControlDeviceInstance(int instanceNumber);
	~CAppDaliInputControlDeviceInstance( );

	enum { NR_OF_INSTANCE_GROUPS = 3 };

	// void setInstanceNumber(int number);
	int getInstanceNumber() const;

	bool getInstanceActive( ) const;
	void setInstanceActive(bool v);

	int getInstanceType( ) const;
	void setInstanceType(int v);

	int getEventScheme( ) const;
	void setEventScheme(int v);

	int getEventFilter( ) const;
	void setEventFilter(int v);

	int getInstanceGroup(int idx) const;
	void setInstanceGroup(int idx,int v);

	int getEventPriority( ) const;
	void setEventPriority(int v);

	int getEventResolution()  const;
	void setEventResolution(int v);

	int getStatus() const;
	void setStatus(int v);

	const CString& getName( ) const;
	void setName(const CString& strName);

	CAppDaliInputControlDeviceInstance* clone( );
	static bool copy(CAppDaliInputControlDeviceInstance* pDest,const CAppDaliInputControlDeviceInstance& src);

private:
	// OPPASSEN: Nieuwe properties bijvoegen in de clone()
	const int m_instanceNumber;									// Number 0-31
	CString m_strName;										// Naam wordt gegeven in de Duotecno software.
	bool m_instanceActive;									// defualt true
	int m_instanceType;										// -1 is onbekend,
	int m_eventScheme;										// 0 is default.	0 tem 4
	int m_eventFilter;										// -1 is onbekend, anders een 24 bit waarde met vlaggen.
	int m_instanceGroup[NR_OF_INSTANCE_GROUPS];				// 3 instance groups, primary group=0, -1 is onbekend,
	int m_eventPriority;									// -1 is onbekend,
	int m_instanceEventResolution;							// -1 is onbekend,
	int m_instanceStatus;									// -1 is onbekend,
};		
/***********************************************************************************/


/***********************************************************************************/
// Sedert V16.32: Namen van de groepen / sferen.
class CAppDaliCollectionNames {
public:
	CAppDaliCollectionNames(int maxItems);		// MaxItems bepaalt de laatste identifier in de map.
	~CAppDaliCollectionNames( );
	
	void setName(int identifier,const CString& str);
	const CString getName(int identifier) const;
	
	int getMax( ) const {
		return this->m_maxItems;
	}
/*
	CMap<int,int,CString,CString>& get() {
		return this->m_mapNames;
	}
*/
	bool copy(CMap<int,int,CString,CString>* dest);

private:
	const int m_maxItems;
	CMap<int,int,CString,CString> m_mapNames;		// Key = identifier van de groep/sfeer.
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
};
};
/***********************************************************************************/