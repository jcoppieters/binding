#pragma once
/***********************************************************************************/
#include "appdali_fwd.h"

#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
	#include "communication/tcpsocket/homeclient/itcpsockethomeclient.h"	
#endif 
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
class CAppDaliTransmitter;
class CAppDaliFsm;
class CAppDaliFsmContext;
class CAppDaliDevices;
class CAppDaliDevice;
class CAppDaliFsmErrorEntries;
class CAppDaliDeviceProperties_ControlGear;
class CAppDaliDeviceProperties_InputControlDevice;
class CAppDaliInputControlDeviceInstances;
class CAppDaliInputControlDeviceInstance;
class CAppDaliCollectionNames;
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDali : public ITcpSocketHomeClient_Host, public IAppDaliFsm_Parent {

public:
	CAppDali(IAppDaliHost_App* parent, BYTE bNodeAddress, ITcpSocketHomeClient* pSocket);
	~CAppDali();

	void subscribeGUI(IAppDlgDaliGui_Host* pObserver);

	///////////////////////////////////////
	// Uitvoeren van taken.
	bool startSearchDevices();
	bool startAddressing(AddressingInstallation_t type);
	bool startAddressingSingleDevice(DaliDeviceType_t type, int shortAddress, int newShortAddress);	// indien newShortAddress=0xff een device wissen.
	bool startAddressingSwapDevices(DaliDeviceType_t type, int a0, int a1);
	bool startUploadControlGearDeviceProperties(int shortAddress, CAppDaliDeviceProperties_ControlGear& refProperties);
	bool startUploadControlGearDeviceScenes(int shortAddress, CAppDaliDeviceProperties_ControlGear& refProperties);
	bool startUploadControlGearDeviceSingleScene(int shortAddress, int sceneNumber, CAppDaliDeviceProperties_ControlGear& refProperties);
	bool startUploadControlGearDimmerCurve(int shortAddress, int deviceType,int dimmerCurve);
	bool startControlGearQueryActualLevel(int shortAddress);
	bool startUploadControlInputDeviceInstanceEnableOnOff(int shortAddress, int instanceNumber, bool stateOnOff);
	bool startUploadControlInputDeviceInstances(int shortAddress, CAppDaliInputControlDeviceInstances& refProperties);
	bool startReadControlGearDeviceProperties(int shortAddress);
	bool startReadControlGearDeviceGroups(int shortAddress);
	bool startReadControlGearDeviceScenes(int shortAddress);
	bool cancelTask();

	//////////////////////////////////////////
	void pollSystem(int intervalSec);					

	//////////////////////////////////////////
	// Opvragen van de huidige taak van de FSM.
	Task_t getFsmCurrentTask();

	// Opslaan van de database van de devices.
	bool saveDatabaseDevices(bool saveOnlyWhenChanges = false);
	bool loadDatabaseDevices(int nodeAddress);

	// Na het inladen van de devices de error status van de devices nakijken.
	// Indien er devices in error status staan dan moeten deze in de lijst van pending errors komen...
	void checkDatabaseDevicesErrorStatus();

	// Duotecno DALi Bridge configuratie.
	void setChangesCfgDuotecnoBridge(bool flag);
	bool getChangesCfgDuotecnoBridgeNeeded();

	// Zijn er momenteel fouten aanwezig in de DALI systeem bus configuratie.
	bool getStatePendingErrors( );

	/////////////////////////////////////////
	// Opvragen van de lijst van fouten.
	// Oppassen dit kan een NULL ptr zijn.
	// De client moet zelf een copy van deze lijst bijhouden dmv de clone() aan te roepen...
	CAppDaliFsmErrorEntries* getPendingErrors();

	///////////////////////////////////////
	// Properties...
	CAppDaliTransmitter& getTransmitter();
	CAppDaliFsm& getFsm();
	CAppDaliFsmContext& getContext();
	CAppDaliDevices& getDevices();
	CAppDaliDevices& getNwkDevices();
	CAppDaliCollectionNames& getNamesGroups( );
	CAppDaliCollectionNames& getNamesScenes( );

	// public ITcpSocketHomeClient_Host
	virtual void onStateConnectChanged(int id, bool connected);
	virtual void onStateChanged(int id, TcpSocketHomeClient_Tasks::State_t state);
	virtual void onSocketHomeClientMsgInfo(int id, CategoryMsg_t category, const CString& msg);
	virtual void onSocketHomeClientMsgError(int id, CategoryMsg_t category, const CString& msg);
	virtual void onTaskStatus(int id, TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status);
	virtual void onTaskCancelStatus(int id, TcpSocketHomeClient_Tasks::Status_t status);
	virtual void onNodeDbaseStatus(int id, TcpSocketHomeClient_NodeDbase::Status_t status);

	// Integratie DALI in TCP/IP protocol.
	virtual void onStatusDALIRxMsgSubscribe(int id, bool status);
	virtual void onStatusDALIResponse(int id, int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData);
	virtual void onRxDALIMessage(int id, BYTE lengthBits, uint32_t recievedFrame);

	typedef enum {
		PROPERTY_CHANGED_NAME = 0,
		PROPERTY_CHANGED_DEVICE_PROPERTY,
		PROPERTY_CHANGED_DEVICE_DATABASE						// TODO: Aanroepen vanuit de applicatie...
	} PropertyChangedType_t;
	void setPropertyChanged(PropertyChangedType_t type);		// Vlag zetten 


	enum {
		CONTROLGEAR_DEVICE_MAX_NAME_LENGTH = 15,			// 15 karakters + NULL char.
		CONTROLINPUT_DEVICE_MAX_NAME_LENGTH = 12,			// 12 karakters + NULL char.
		CONTROLINPUT_DEVICE_INSTANCE_MAX_NAME_LENGTH = 2	// 2 karakters.
	};

	///////////////////////////////////////////////
	// Interface voor de worker...
	BOOL getDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,CString* pStrName);	
	BOOL setDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,const CString& strName,
		CString* pStrNormalized);
	BOOL getControlDeviceInstanceName(int shortAddress,int instanceNumber, CString* pStrName);
	BOOL setControlDeviceInstanceName(int shortAddress,int instanceNumber,const CString& strInstanceName,
		CString* pStrNormalized);

	///////////////////////////////////////////////
	// Interface voor de FSM...
	// Deze events zijn afkomstig van de FSM.
	// Worden doorgegeven naar de observer (=User interface)
	virtual void notifyDaliCfgFsmStarted(Task_t task);
	virtual void notifyDaliCfgFsmDone(Task_t task);	
	virtual void notifyDaliCfgFsmError(Task_t task, DaliErrorCode_t error,int shortAddress,const CString& strErrorDetail);

	// Database functies.
	virtual void fsmAssignedNewShortAddress(DaliDeviceType_t type, int shortAddress);
	virtual bool fsmDbaseSetDeviceError(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress,DaliErrorCode_t errorCode);
	virtual bool fsmDbaseAddNewDevice(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress);
	virtual bool fsmDbaseSetDeviceGTIN(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, unsigned long long GTIN);
	virtual bool fsmDbaseSetDeviceSerialNumber(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int shortAddress, unsigned long long serialNumber);
	virtual bool fsmDbaseSetDeviceFirmwareVersion(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, int v);
	virtual bool fsmDbaseSetDeviceHardwareVersion(DeviceDatabaseType_t databaseType, DaliDeviceType_t type, int shortAddress, int v);
	// Interface voor de FSM: control gear properties.
	virtual bool fsmDbaseSetControlGearGroupMemberInfo(DeviceDatabaseType_t databaseType, int shortAddress, unsigned short value);
	virtual bool fsmDbaseSetControlGearSceneLevel(DeviceDatabaseType_t databaseType, int shortAddress, int sceneNumber, int level);
	// Control gear properties.
	virtual bool fsmDbaseSetControlGearDeviceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearDeviceVersion(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearDeviceExtendedFadeTime(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearDevicePowerOnLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearDeviceFailureLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearDeviceType(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearMinimumLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearMaximumLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearFadeTimeRate(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearPhysicalMinValue(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearPowerOnLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearSystemFailureLevel(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlGearDimmingCurve(DeviceDatabaseType_t databaseType, int shortAddress, int dimmingCurve);
	virtual bool fsmDbaseSetControlGearDeviceTypes(DeviceDatabaseType_t databaseType, int shortAddress, CList<int>& list);
	virtual bool fsmDbaseSetControlGearColorFeatures(DeviceDatabaseType_t databaseType, int shortAddress, int colorFeatures);

	// ControlInputDevice
	virtual bool fsmDbaseSetControlInputDeviceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlInputDeviceVersion(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlInputDeviceOperatingMode(DeviceDatabaseType_t databaseType, int shortAddress, int value);
	virtual bool fsmDbaseSetControlInputDeviceGroupInfo(DeviceDatabaseType_t databaseType, int shortAddress, uint32_t groupInfo);
	virtual bool fsmDbaseSetControlInputDeviceAppControllerEnabled(DeviceDatabaseType_t databaseType, int shortAddress, bool v);
	virtual bool fsmDbaseSetControlInputDevicePowerCycleNotification(DeviceDatabaseType_t databaseType, int shortAddress, bool v);
	virtual bool fsmDbaseSetControlInputDeviceDeviceCapabilities(DeviceDatabaseType_t databaseType, int shortAddress, int v);
	virtual bool fsmDbaseSetControlInputDeviceRandomAddress(DeviceDatabaseType_t databaseType, int shortAddress, uint32_t v);
	// ControlInputDevice Instances.
	virtual bool fsmDbaseSetControlInputDeviceInstanceType(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value);
	virtual bool fsmDbaseSetControlInputDeviceInstanceEnabledOnOff(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,bool value);
	virtual bool fsmDbaseSetControlInputDeviceInstanceEventScheme(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value);
	virtual bool fsmDbaseSetControlInputDeviceInstanceEventPrio(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int value);
	virtual bool fsmDbaseSetControlInputDeviceInstanceEventFilters(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,uint32_t value);
	virtual bool fsmDbaseSetControlInputDeviceInstanceGroupInfo(DeviceDatabaseType_t databaseType, int shortAddress,int instanceID,int idxGroup, int value);
	virtual bool fsmDbaseSetControlInputDeviceInstanceStatus(DeviceDatabaseType_t databaseType, int shortAddress, int instanceID, int value);
	virtual bool fsmDbaseSetControlInputDeviceInstanceResolution(DeviceDatabaseType_t databaseType, int shortAddress, int instanceID, int value);

	///////////////////////////////////////////////////
	// Document database acties.
	virtual bool fsmDbaseTaskRemovedDaliDevice(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int shortAddress);
	virtual bool fsmDbaseTaskChangedDaliDeviceShortAddress(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int shortAddress,int newAddress);
	virtual bool fsmDbaseTaskSwappedDaliDeviceShortAddresses(DeviceDatabaseType_t databaseType, DaliDeviceType_t type,int a0,int a1);
	virtual bool fsmDbaseControlGearPropertiesCfgUpdate(DeviceDatabaseType_t databaseType, int shortAddress,const CAppDaliDeviceProperties_ControlGear& properties);
	virtual bool fsmDbaseControlGearUpdateActualLevel(DeviceDatabaseType_t databaseType, int shortAddress, int actualLevel);
	virtual bool fsmDbaseControlInputDeviceInstancesCfgUpdate(DeviceDatabaseType_t databaseType, int shortAddress,const CAppDaliInputControlDeviceInstances& properties);

	// Functie nodig voor de FSM: Bij het opvragen van de scene info bepalen of we de DT8 Color Scene info moeten opvragen.
	// Wordt gebruikt tijdens de commisioning.
	virtual bool fsmDbaseControlGearIsDT8ColorObjectSupported(DeviceDatabaseType_t databaseType, int shortAddress);
	virtual bool fsmDbaseControlGearDT8SetSceneColorObjectInfo(DeviceDatabaseType_t databaseType, int shortAddress, int sceneNumber, const CAppDaliObjectDT8ColorData& refData);

	// Alle controlgear devices updaten met deze sfeer info.
	// Deze functie wordt gebruikt wanneer sfeer info geconfigureerd wordt dmv het broadcast adres.
	virtual bool fsmDbaseControlGearSetBroadcastSceneInfo(DeviceDatabaseType_t databaseType, int sceneNumber, const CAppDaliDeviceProperties_ControlGear& r);

	///////////////////////////////////////////////////
	// Mogelijkheid om progressinfo door te geven vanuit de fsm...
	virtual void notifySetProgessInfo(int shortAddress, const char* strInfo);

	// Aanpassen van de properties door de configuratie.
	bool setControlGearDevicePropertiesAddToGroup(CAppDaliDevice& refDevice,int groupAddress);
	bool setControlGearDevicePropertiesRemoveFromGroup(CAppDaliDevice& refDevice,int groupAddress);
	bool setControlGearDevicePropertiesSetSceneInfo(CAppDaliDevice& refDevice,int sceneAddress,int data);


	// node address.
	BYTE getNodeAddress( ) const {
		return this->m_bNodeAddress;
	}

protected:
	// Wanneer er een nieuwe FSM addresseringstaak gestart wordt moeten de huidige fouten verwijderd worden.
	// Wanneer de taak gedaan is worden de fouten opnieuw ingevuld.
	void clearPendingErrors( );
	bool addPendingError(CAppDaliDevice* pDevice);

protected:
	// Zoeken naar de controlgear properties ifv het shortAddress.
	bool getDeviceProperties(CAppDaliDevices& refDevices,int shortAddress, CAppDaliDeviceProperties_ControlGear** ppProperties);

	// Zoeken naar de control input device properties ifv het shortAddress.
	bool getDeviceProperties(CAppDaliDevices& refDevices, int shortAddress, CAppDaliDeviceProperties_InputControlDevice** ppProperties);

	// Zoeken naar de control device instance properties ifv het shortAddress.
	bool getDeviceInstanceProperties(CAppDaliDevices& refDevices, int shortAddress, int instance, CAppDaliInputControlDeviceInstance** ppProperties,bool create);

protected:
	// Control gear property via een functie pointer
	bool helper_setControlGearProperty(CAppDaliDevices& refDevices, int shortAddress, int value, void (CAppDaliDeviceProperties_ControlGear::*pf)(int));

	// Control input device property via een functie pointer.
	bool helper_setControlDeviceProperty(CAppDaliDevices& refDevices, int shortAddress, int value, void (CAppDaliDeviceProperties_InputControlDevice::*pf)(int));
	bool helper_setControlDeviceProperty(CAppDaliDevices& refDevices, int shortAddress, bool value, void (CAppDaliDeviceProperties_InputControlDevice::*pf)(bool));
	bool helper_setControlDeviceProperty(CAppDaliDevices& refDevices, int shortAddress, uint32_t value, void (CAppDaliDeviceProperties_InputControlDevice::*pf)(uint32_t));

	// Control input device instance property via een functie pointer.
	bool helper_setControlDeviceInstanceProperty(CAppDaliDevices& refDevices, int shortAddress, int instane, int value, void (CAppDaliInputControlDeviceInstance::*pf)(int));
	bool helper_setControlDeviceInstanceProperty(CAppDaliDevices& refDevices, int shortAddress, int instane, bool value, void (CAppDaliInputControlDeviceInstance::*pf)(bool));

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Database functies

	// Na het zoeken op het netwerk de gevonden devices copieren naar de document database...
	bool copyDatabaseNetwork2DocumentDevices(bool mergeData);

	// Zoeken naar verschillen in de database.
	// Wanneer er verschillen gedecteerd worden dan wordt indirect de PROPERTY_CHANGED_DEVICE_DATABASE vlag gezet.
	bool databaseDetectChanges();

	// Zoeken naar fouten.
	// Wanneer er fouten aanwezig zijn wordt de FLAG_DATABASE_PENDING_ERRORS vlag gezet.
	// Wannner er geen fouten zijn dan wordt de vlag gewist.
	bool databaseDectectErrors( );

	// Na het verwijderen van de devices controleren of er nog devices aanwezig zijn die in fout toestand staan.
	// Oppassen: Past zelf de m_flags member aan.
	bool databaseCheckErrorState();

	//////////////////////////////////////// end database functies ////////////////////////////////
private:
	enum {
		// Wanneer er configuratie aanpassingen gebeuren dan moet de database opnieuw 
		// ogeslagen worden.
		// Deze vlag wordt gepolled door de GUI.
		// Het opslaan gebeurd dmv saveDatabaseDevices()
		FLAG_DATABASE_REQUEST_SAVE			=	(1<<0),		// Database moet opgeslagen worden...

		// Deze vlag wordt gezet wanneer er een property aangepast werd.
		FLAG_DATABASE_PROPERTY_CHANGED		=	(1<<1),

		// Bij het mergen gaan we controleren of er errors aanwezig zijn.
		// Wanneer er errors aanwezig zijn wordt deze globale error vlag gezet.
		FLAG_DATABASE_PENDING_ERRORS		 = (1<<2),

		// Deze vlag wordt gezet wanneer er properties gewijzigd worden.
		// Deze vlag wordt gewist na het openen van de DALI Duotecno Bridge configuratie.
		// Bij het afsluiten van de configuratie/GUI wordt deze valg gecontroleerd.
		// Bij veranderingen wordt de vraag gesteld aan de gebruiken om de configuratie aan te passen.
		FLAG_CHANGES_CFG_DUOTECNOBRIDGE_NEEDED = (1<<3)
	};
	
	IAppDaliHost_App* const m_parent;	
	const BYTE m_bNodeAddress;
	ITcpSocketHomeClient* m_pSocket;			// Moeten een instantie bijhouden voor de subscribe/unsubscribe...
	CAppDaliTransmitter* m_pTransmitter;
	CAppDaliFsm* m_pFsm;
	CAppDaliFsmContext* m_pFsmContext;
	CAppDaliFsmErrorEntries* m_pPendingErrors;
	CAppDaliDevices* m_pDaliDevices;				// Document Databases...
	CAppDaliDevices* m_pNwkDaliDevices;				// Database van devices gevonden in de laatste browse...
	CAppDaliCollectionNames* m_pDaliNamesGroups;
	CAppDaliCollectionNames* m_pDaliNamesScenes;
	IAppDlgDaliGui_Host* m_pObserver;
	uint16_t m_flags;
};
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
struct CAppDali_Utils {
	static const char* getStrResponseCode(BYTE type);
};

/***********************************************************************************/
};
};
/***********************************************************************************/