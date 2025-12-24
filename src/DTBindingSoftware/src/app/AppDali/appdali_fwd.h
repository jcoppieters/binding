#pragma once

/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
class CAppDaliDeviceProperties_ControlGear;
class CAppDaliInputControlDeviceInstances;
class CAppDaliObjectDT8ColorData;
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
typedef enum {
	APPDALI_ADDRESSING_INSTALLATION_EXTEND = 0,		// uitbreiding bestaande installatie.
	APPDALI_ADDRESSING_INSTALLATION_NEW				// Nieuwe installatie.

} AddressingInstallation_t;

typedef enum {
	APPDALI_FSM_TASK_NONE = 0,
	APPDALI_FSM_TASK_SEARCH_DEVICES,			// Zoeken in een bestaande installatie.
	APPDALI_FSM_TASK_COMMISSIONING_EXTEND,		// Installatie uitbreiden.
	APPDALI_FSM_TASK_COMMISSIONING_NEW,			// Nieuwe installatie.
	APPDALI_FSM_TASK_ADDRESSING_SINGLE_DEVICE,
	APPDALI_FSM_TASK_ADDRESSING_ERASE_DEVICE,
	APPDALI_FSM_TASK_ADDRESSING_SWAP_DEVICES,
	APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_PROPERTIES,
	APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_SCENES,
	APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_DIMMERCURVE,
	APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_PROPERTIES,
	APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_GROUPS,
	APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_SCENES,
	APPDALI_FSM_TASK_CONTROLGEAR_QUERY_ACTUAL_LEVEL,
	APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF,
	APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCES,
} Task_t;

// Oppassen: Onderstaande fouten worden ook gebruikt in de JSON export. Niet meer veranderen.
typedef enum {

	// Geen fout...
	ERROR_CODE_NONE = 0,													// DON'T CHANGE

	// Mogelijk een dubbel short address aanwezig.
	// Error code wordt toegekend aan een Dali device.
	// Error code controleren in de Duotecno/DALI bridge configuratie - Device heeft wellicht een dubbel adres.
	ERROR_CODE_SHORTADDRESS_DOUBLE = -1,									// DON'T CHANGE

	// Fout bij de commissioning.
	// Na het toekennen van een short addresse wordt er een verify uitgevoerd.
	// Wanneer het device antwoord met een ander short adres krijgen we deze fout.
	// Deze fout is een globale fout. Wordt niet toegekend aan een DALI device.
	ERROR_CODE_COMMISSIONING_SHORTADDRESS_VERIFY = -2,						// DON'T CHANGE

	// Kregen een verkeerde response op een property die nodig is voor de configuratie.
	ERROR_CODE_PROPERTY_WRONGRESPONSE = -3,									// DON'T CHANGE

	// Kregen geen response op een property die nodig is voor de configuratie.
	// Deze fout is een globale fout. Wordt niet toegekend aan een DALI device.
	ERROR_CODE_PROPERTY_NORESPONSE = -4,									// DON'T CHANGE

	// Algemene fout.
	// Deze fout is een globale fout. Wordt niet toegekend aan een DALI device.
	ERROR_CODE_WRONG_EVENT = -5,											// DON'T CHANGE

	// Tijdens de commissioning zijn alle short addressen opgebruikt.
	// Deze fout is een globale fout. Wordt niet toegekend aan een DALI device.
	ERROR_CODE_COMMISSIONING_SHORTADDRESS_EXCEED_MAX = -6,					// DON'T CHANGE

	// 
	// Het device is aanwezig in de offline database maar niet gevonden in de network database. 
	// Mogelijk is het device niet meer aanwezig in het systeem.
	// De gebruiker kan het device verwijderen uit de device database.
	// Error code wordt toegekend aan een Dali device.
	// Error code controleren in de Duotecno/DALI bridge configuratie - Device niet meer aanwezig?
	ERROR_CODE_DATABASE_DEVICE_NOT_FOUND = -8,								// DON'T CHANGE


	// Timer expiry.
	ERROR_CODE_TIMER_EXPIRY				= -9,


#if(0)	// todo	--> Onderstaande gebruiken in code.	
	// Globale system bus fout.
	ERROR_CODE_SYSTEMBUS_FAILURE	= -10,

	// Device in het nwk heeft een andere serie nummer dan in de database.
	ERROR_CODE_DATABASE_REPLACED	= -21,

#endif

} DaliErrorCode_t;

/******************************************************************/
/******************************************************************/
/******************************************************************/
// Oppassen: Onderstaande fouten worden ook gebruikt in de JSON export. Niet meer veranderen.
typedef enum {
	ERROR_STATUS_NONE = 0,			// DON'T CHANGE - Geen fout. (Default).
	ERROR_STATUS_PENDING = 1,		// DON'T CHANGE - Fout gedetecteerd op het DALI bus netwerk - Device staat in foutstatus, zal mogelijk niet correct werken.
	// ERROR_STATUS_UNKNOWN = 2		// DON'T CHANGE - Fout gedetecteerd, manueel adres gewijzigd, Probleem opgelost
} DaliDeviceErrorStatus_t;

/******************************************************************/
/******************************************************************/
/******************************************************************/
// Interface die moet geimplementeerd worden door de Applicatie.
struct IAppDaliHost_App {

	virtual ~IAppDaliHost_App() = 0;

	// Opvragen waar het bestand moet opgeslagen worden.
	// De app zal de projecct dir + "DATA" map teruggeven.	
	virtual CString getFullyQualifiedName(int nodeAddress) = 0;

	// Het bestand werd opgeslagen.
	// De App zal het bestand nog eens copieren on de "CONFIG" map.
	virtual void notifyDataSaved( ) = 0;
};

/******************************************************************/
typedef enum {
	DALI_DEVICE_TYPE_CONTROLGEAR = 0,
	DALI_DEVICE_TYPE_INPUTCONTROL = 1,	
} DaliDeviceType_t;


// DALI Control gear dimming curve
// Oppassen: Momenteel enkel ondersteund voor dt6.
//	- Bij andere devices types is dit ook mogelijk maar de waarden zijn anders.
//	  bvb: Bij een 0-10V (dt5) is de curve standaard lineair en is dit waarde 0. De DALI curve is daar waarde 1
typedef enum {
	DALI_CONTROLGEAR_DIMCURVE_UNSUPPORTED = -1,
	DALI_CONTROLGEAR_DIMCURVE_DALI = 0,
	DALI_CONTROLGEAR_DIMCURVE_LINEAIR = 1
} DaliControlGearDimCurve_t;

/******************************************************************/
/******************************************************************/
/******************************************************************/
// Deze interface wordt geimplementeerd in de GUI...
struct IAppDlgDaliGui_Host {

	typedef enum {
		TYPE_MSG_INFO = 0,
		TYPE_MSG_ERROR
	} Type_t;

	virtual ~IAppDlgDaliGui_Host( ) = 0;

	// Status socket connected.
	virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) = 0;

	// Info bericht om weer te geven in de listbox.
	virtual void onNotifyGuiMsgCommunication(BYTE bNodeAddress, Type_t typeMessage, const CString& msg) = 0;
	
	// Notify events van de taken...
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress, Task_t task) = 0;
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress, Task_t task) = 0;
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress, Task_t task,const CString& strMessage) = 0;	

	virtual void onNotifyGuiFoundDevice(BYTE bNodeAddress, DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice) = 0;
	virtual void onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress, DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber) = 0;
	virtual void onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN) = 0;

	// Er werd een DALi-24 bericht ontvangen.
	virtual void onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame) = 0;
};
/********************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
struct IAppDaliFsm_Parent {
	virtual ~IAppDaliFsm_Parent( ) = 0;

	typedef enum {
		DEVICE_DATABASE_NETWORK = 0,			// Network database tijdens het addresseren/search devices.
		DEVICE_DATABASE_DOCUMENT = 1			// Offline database
	} DeviceDatabaseType_t;

	// FSM status.
	virtual void notifyDaliCfgFsmDone(Task_t task) = 0;
	virtual void notifyDaliCfgFsmStarted(Task_t task) = 0;
	virtual void notifyDaliCfgFsmError(Task_t task, DaliErrorCode_t error, int shortAddress, const CString& strErrorDetail) = 0;

	// TODO_DALI_FSM_ALGORITMES
	// Mogelijkheid om progressinfo door te geven vanuit de fsm...
	virtual void notifySetProgessInfo(int shortAddress, const char* strInfo) = 0;

	// Er werd een nieuw address toegekend aan een device.
	virtual void fsmAssignedNewShortAddress(DaliDeviceType_t type, int shortAddress) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Database functies wanneer er fouten gebeuren tijdens het uitvoeren van de fsm taken.
	virtual bool fsmDbaseSetDeviceError(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress,DaliErrorCode_t errorCode) = 0;


	//////////////////////////////////////////////////////////////////////////
	// Database functies.

	// Toevoegen van een nieuw device.
	// return false:
	//	- ongeldig shortAddress
	//	- wanneer er al een device aanwezig is met dit short address.
	virtual bool fsmDbaseAddNewDevice(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress) = 0;
	
	// Device property GTIN.
	virtual bool fsmDbaseSetDeviceGTIN(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, unsigned long long GTIN) = 0;

	// Device property serial number.
	virtual bool fsmDbaseSetDeviceSerialNumber(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, unsigned long long serialNumber) = 0;

	virtual bool fsmDbaseSetDeviceFirmwareVersion(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, int v) = 0;
	virtual bool fsmDbaseSetDeviceHardwareVersion(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, int v) = 0;

	// Group info: info wordt doorgegeven dmv een 16bit waarde.
	// bit0 = group0, bit15=group15
	virtual bool fsmDbaseSetControlGearGroupMemberInfo(DeviceDatabaseType_t databaseType, int shortAddress, unsigned short value) = 0;

	// Scene level info.
	virtual bool fsmDbaseSetControlGearSceneLevel(DeviceDatabaseType_t databaseType, int shortAddress, int sceneNumber, int level) = 0;

	// Control gear properties.
	virtual bool fsmDbaseSetControlGearDeviceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearDeviceVersion(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearDeviceExtendedFadeTime(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearDevicePowerOnLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearDeviceFailureLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearDeviceType(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearMinimumLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearMaximumLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearFadeTimeRate(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearPhysicalMinValue(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearPowerOnLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearSystemFailureLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlGearDimmingCurve(DeviceDatabaseType_t databaseType, int shortAddress, int dimmingCurve) = 0;
	virtual bool fsmDbaseSetControlGearDeviceTypes(DeviceDatabaseType_t databaseType, int shortAddress, CList<int>& list) = 0;
	virtual bool fsmDbaseSetControlGearColorFeatures(DeviceDatabaseType_t databaseType, int shortAddress, int colorFeatures) = 0;


	// ControlInputDevice
	virtual bool fsmDbaseSetControlInputDeviceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceVersion(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceOperatingMode(DeviceDatabaseType_t databaseType, int shortAddress, int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceGroupInfo(DeviceDatabaseType_t databaseType, int shortAddress, uint32_t groupInfo) = 0;
	virtual bool fsmDbaseSetControlInputDeviceAppControllerEnabled(DeviceDatabaseType_t databaseType, int shortAddress, bool v) = 0;
	virtual bool fsmDbaseSetControlInputDevicePowerCycleNotification(DeviceDatabaseType_t databaseType, int shortAddress, bool v) = 0;
	virtual bool fsmDbaseSetControlInputDeviceDeviceCapabilities(DeviceDatabaseType_t databaseType, int shortAddress, int v) = 0;
	virtual bool fsmDbaseSetControlInputDeviceRandomAddress(DeviceDatabaseType_t databaseType, int shortAddress, uint32_t v) = 0;

	// ControlInputDevice Instances.
	virtual bool fsmDbaseSetControlInputDeviceInstanceType(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceInstanceEnabledOnOff(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,bool value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceInstanceEventScheme(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceInstanceEventPrio(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceInstanceEventFilters(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,uint32_t value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceInstanceGroupInfo(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int idxGroup, int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceInstanceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int instanceID, int value) = 0;
	virtual bool fsmDbaseSetControlInputDeviceInstanceResolution(DeviceDatabaseType_t databaseType, int shortAddress, int instanceID, int value) = 0;

	// Document database acties.
	virtual bool fsmDbaseTaskRemovedDaliDevice(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int shortAddress) = 0;
	virtual bool fsmDbaseTaskChangedDaliDeviceShortAddress(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int shortAddress,int newAddress) = 0;
		virtual bool fsmDbaseTaskSwappedDaliDeviceShortAddresses(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int a0,int a1) = 0;
	virtual bool fsmDbaseControlGearPropertiesCfgUpdate(DeviceDatabaseType_t databaseType, int shortAddress,const CAppDaliDeviceProperties_ControlGear& properties) = 0;
	virtual bool fsmDbaseControlGearUpdateActualLevel(DeviceDatabaseType_t databaseType, int shortAddress, int actualLevel) = 0;
	virtual bool fsmDbaseControlInputDeviceInstancesCfgUpdate(DeviceDatabaseType_t databaseType, int shortAddress,const CAppDaliInputControlDeviceInstances& properties) = 0;

	// Functie nodig voor de FSM: Bij het opvragen van de scene info bepalen of we de DT8 Color Scene info moeten opvragen.
	// Wordt gebruikt tijdens de commisioning.
	virtual bool fsmDbaseControlGearIsDT8ColorObjectSupported(DeviceDatabaseType_t databaseType, int shortAddress) = 0;
	virtual bool fsmDbaseControlGearDT8SetSceneColorObjectInfo(DeviceDatabaseType_t databaseType, int shortAddress, int sceneNumber, const CAppDaliObjectDT8ColorData& refData) = 0;

	// Alle controlgear devices updaten met deze sfeer info.
	// Deze functie wordt gebruikt wanneer sfeer info geconfigureerd wordt dmv het broadcast adres.
	virtual bool fsmDbaseControlGearSetBroadcastSceneInfo(DeviceDatabaseType_t databaseType, int sceneNumber, const CAppDaliDeviceProperties_ControlGear& r) = 0;
};

/***********************************************************************************/
};
};
/***********************************************************************************/