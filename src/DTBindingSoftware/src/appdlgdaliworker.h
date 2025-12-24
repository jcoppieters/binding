#ifndef INCLUDED_APPDLGDALIWORKER_H
#define INCLUDED_APPDLGDALIWORKER_H
/*****************************************************************/
#include "App/AppDali/appdali_fwd.h"
#include "App/AppDali/appdaliprotocol_def.h"

#ifndef INCLUDED_COMMUNICATION_DEF_H
	#include "communication/base/communication_def.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
	#include "communication/tcpsocket/homeclient/itcpsockethomeclient.h"	
#endif 
#ifndef INCLUDED_IAPPDLGTASKS_H
	#include "iappdlgtasks.h"
#endif 
/*****************************************************************/
namespace DUOTECNO {
namespace DALI {
	class CAppDali;
	class CAppDaliDevice;
	class CAppDaliDevices;
	class CAppDaliManager;
	class CAppDaliFsmErrorEntries;
	class CAppDaliDeviceProperties_ControlGear;
	class CAppDaliInputControlDeviceInstances;
	class CAppDaliInputControlDeviceInstance;
	struct IAppDlgDaliGui_Host;
};
};
/******************************************************************/
class CCanNode;
/******************************************************************/

/******************************************************************/
/******************************************************************/
/******************************************************************/
class CAppDlgDaliWorker : public DUOTECNO::DALI::IAppDaliHost_App{
public:
	CAppDlgDaliWorker();		
																			
	virtual ~CAppDlgDaliWorker( );

	// Initialisatie van de manager.
	// return waarde: TRUE=ok, FALSE=geen devices gevonden die DALI bus config ondersteunen.
	BOOL initDaliManager(DUOTECNO::DALI::CAppDaliManager& refAppDaliManager);

	// Opvragen van de lijst van nodes voor wie er een implementatie is.
	// Krijgen een lijst van de node addressen waarvoor er een DALI bus implementatie is.
	// return waarde: TRUE lijst aanwezig, FALSE lijst bevat geen items.
	BOOL getListDaliNodes(CList<int,int>* pListNodeAddresses);

	// Open van een dialog.
	// De Observer wordt toegekend aan alle instanties van de CAppDali.
	BOOL initOnOpenDialog(DUOTECNO::DALI::IAppDlgDaliGui_Host* pObserver);

	// Sluiten van een dialoog
	// Ontkoppelen van de GUI observers.
	BOOL closeDialog( );

	// Opslaan van de database.
	// Deze functie moet periodiek door de GUI aangeroepen worden.
	// Wanneer er veranderingen opgeslagen werden dan zal is de return waarde TRUE.
	// Deze functie wordt aangeroepen door de CPropertyPageDaliWelcome klasse.
	// Wordt aangeroepen via een timer + wanneer de selectie van de smartbox veranderd.
	BOOL pollByGuiSaveChanges( );

	// Opvragen van de huidige node die gebruikt wordt/gekoppeld is aan de GUI.
	// Indien 0xff is er geen node gekoppeld.
	BYTE getCurrentNodeAddress( );		
	BOOL setCurrentNodeAddress(int nodeAddress);

	// Opvragen van de database van de huidige CAppDaLi implmentatie.
	// Kan NULL zijn.
	DUOTECNO::DALI::CAppDaliDevices* getDaliDevices( );

	// Vertalen van een ontvangen DALI bericht naar een string.
	// Deze string kan getoond worden in de event log.
	// Oppassen:
	// Controleren op het instance type van de huidige DALI implementatie.
	// Wanneer het geen event bericht is dan is de string leeg en moet het bericht niet getoond worden.
	bool convertEventMsgToString(BYTE lengthBits,uint32_t recievedFrame,int* shortAddress,int* instanceNumber,int* pInstanceType, 
		CString* pStrEventMsg,CString* pStrInstanceType);

	// Opvragen van een lijst van short address van devices die aanwezig zijn in de database.
	void getListControlGearAddress(CList<int>* pList);
	void getListDeviceAddresses(DUOTECNO::DALI::DaliDeviceType_t deviceType,CList<int>* pList);

	// Controleren of het device reeds aanwezig is in een andere groep configuratie.
	// Indien aanwezig dan een message box tonen + vraag stellen aan de gebruiker om deze toe te voegen aan deze groep.
	// return waarde TRUE is toevoegen aan de groep.
	BOOL checkDeviceAndPromptAddToGroup(CWnd* parent,int groupAddress, int shortAddress); 	

	// Opvragen van een lijst van short addresses van devices die aanwezig zijn in een groep.
	void getListControlGearGroupMembers(int groupAddress,CList<int>* pList);

	// Opvragen in welke groepen een device aanwezig is.
	void getListOfGroups(int shortAddress, CList<int>* pList);

	// Opvragen en instellen van de naam van een device.
	// De namen zijn beperkt in lengte. Bij het instellen van de namen kan er een pointer naar een string (pStrNormalized) 
	// meegegeven worden. Deze bevat de naam die aangepast werd naar de maximum grootte.
	BOOL getDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,CString* pStrName);	
	BOOL setDeviceName(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,const CString& strName,CString* pStrNormalized);
	BOOL getControlDeviceInstanceName(int shortAddress,int instanceNumber, CString* pStrName);
	BOOL setControlDeviceInstanceName(int shortAddress,int instanceNumber,const CString& strInstanceName,CString* pStrNormalized);

	// Connectie maken met de de homeclient.
	// Dialoog tonen.
	void showCfgNodeServices( );
	bool disconnect( );
	bool isConnected();
	bool connectLocalPossible( );
	bool connectRemotePossible( );
	bool connectTCPHomeClient(ConnectionMethod_t method);

	HomeClientConnectionState_t getTypeConnected( );

	// Uitvoeren van de FSM taken.
	bool startSearchDevices( );
	bool startAddressing(DUOTECNO::DALI::AddressingInstallation_t type);
	bool startAddressingSingleDevice(DUOTECNO::DALI::DaliDeviceType_t deviceType, int shortAddress,int newShortAddress);
	bool startAddressingSwapDevices(DUOTECNO::DALI::DaliDeviceType_t deviceType, int address0,int address1);
	bool startEraseDeviceAddress(DUOTECNO::DALI::DaliDeviceType_t deviceType, int shortAddress);
	bool startUploadControlGearDeviceProperties(int shortAddress, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear& refProperties);
	bool startUploadControlGearDeviceScenes(int shortAddress, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear& refProperties);
	bool startUploadControlGearDeviceSingleScene(int shortAddress, int sceneNumber,DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear& refProperties);
	bool startUploadControlGearDimmerCurve(int shortAddress, int deviceType,int dimmerCurve);
	bool startControlGearQueryActualLevel(int shortAddress);
	bool startControlDeviceInstanceEnableOnOff(int shortAddress, int instanceNumber, bool stateOnOff);
	bool startUploadControlInputDeviceInstances(int shortAddress, DUOTECNO::DALI::CAppDaliInputControlDeviceInstances& refProperties);
	bool startReadControlGearDeviceProperties(int shortAddress);
	bool startReadControlGearDeviceGroups(int shortAddress);
	bool startReadControlGearDeviceScenes(int shortAddress);
	bool cancelTask( );

	// Generieke helperfunctie voor het starten van een leesoperatie
	template <typename Func>
	bool startReadControlGearDevice(int shortAddress, Func function);

	// Generieke helperfunctie voor het starten van een upload
	template <typename Func, typename Properties>
	bool startUploadDeviceProperties(int shortAddress, Properties& refProperties, Func function);

	// Generieke helperfunctie voor het starten van een actie
    template <typename Func>
    bool startDaliAction(Func function);

	// Overloaded helperfunctie voor 1 parameter
    template <typename Func, typename Arg1>
    bool startDaliAction(Func function, Arg1 arg1);

    // Overloaded helperfunctie voor drie parameters
    template <typename Func, typename Arg1, typename Arg2,typename Arg3>
    bool startDaliAction(Func function, Arg1 arg1, Arg2 arg2,Arg3 arg3);


	//////////////////////////////////////	
	// Sedert V16.45
 	// DALI-2 Control Input Devices: Ozuno Universele Interface
	// Een Device/instance herstellen naar het type 'push-button' instance.
	// Herstellen van het instance type naar een 'push-button' instance. 
	// Dit is momenteel een tijdelijke/snelle oplossing.
	// Reden: Het wijzigen van het instance type is alleen mogelijk in de specifieke manufacturing operating mode 
	// via de Rapix-configuratiesoftware. 
	// Het is onwaarschijnlijk dat dit per ongeluk naar een ander instance type wordt gewijzigd.
	bool sendControlInputDeviceRestoreInstanceType(int shortAddress,int instanceAddress);

	//////////////////////////////////////
	// Status van de FSM taken.
	bool getDaliCfgTaskBusy( );
	DUOTECNO::DALI::Task_t getDaliCfgCurrentTask( );

	/////////////////////////////////////////
	// Opvragen van de lijst van fouten.
	// Oppassen dit kan een NULL ptr zijn.
	// De client moet zelf een copy van deze lijst bijhouden dmv de clone() aan te roepen...
	DUOTECNO::DALI::CAppDaliFsmErrorEntries* getPendingErrors( );


	// Helper functies voor de transmit.
	typedef enum {
		CONTROLGEAR_MSG_DEVICE = 0,
		CONTROLGEAR_MSG_GROUP,
		CONTROLGEAR_MSG_BROADCAST 
	} ControlGearMsgType_t;

	bool sendDeviceOff(ControlGearMsgType_t type, int address);					// 0xff is broadcast
	bool sendDeviceMaxLevel(ControlGearMsgType_t type, int address);			// 0xff is broadcast
	bool sendDeviceMinLevel(ControlGearMsgType_t type, int address);			// 0xff is broadcast
	bool sendDeviceDimUp(ControlGearMsgType_t type, int address);	
	bool sendDeviceDimDown(ControlGearMsgType_t type, int address);	
	bool sendDeviceDimStepUp(ControlGearMsgType_t type, int address);	
	bool sendDeviceDimStepDown(ControlGearMsgType_t type, int address);	
	bool sendDeviceDimActualLevel(ControlGearMsgType_t type, int address,int level);	
	bool sendGotoScene(ControlGearMsgType_t type, int address,int level);

	bool sendDeviceQueryStatus(int shortAddress);
	bool sendInputDeviceIdentify(int shortAddress);

	// ControlGear groepen.
	bool controlGearAddToGroup(int shortAddress, int groupAddress);
	bool controlGearAddToGroup(DUOTECNO::DALI::CAppDaliDevice& refDevice, int groupAddress);
	bool controlGearRemoveFromGroup(int shortAddress, int groupAddress);
	bool controlGearRemoveFromGroup(DUOTECNO::DALI::CAppDaliDevice& refDevice, int groupAddress);

	// ControlGear groep properties instellen voor alle member van een groep.
	typedef enum {
		PROPERTY_LEVEL_MIN = 0,
		PROPERTY_LEVEL_MAX,
		PROPERTY_LEVEL_POWERON,
		PROPERTY_LEVEL_SYSTEMFAILURE,
		PROPERTY_FADE_RATE,
		PROPERTY_FADE_TIME
	} TypeProperty_t;	
	bool controlGearGroupsAdjustPropertyMembers(int groupAddress, TypeProperty_t type,int value);
	bool sendControlGearCfgProperty(int groupAddress, TypeProperty_t type,int value);

	// Control group properties updaten dmv de eerste member.
	bool copyPropertiesFromFirstGroupMember(int groupNR, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* properties, int* deviceAddress);
	// Opvragen van het eerste device van een groep.
	int getFirstDeviceInGroup(int groupNR);

	// Application controller van een input device aan/uitschakelen.
	bool controlDeviceSetApplicationControllerOnOff(int shortAddress, bool stateOnOff);

	// Openen van de Duotecno DALI Bridge configuration.
	void openDALIDuotecnoBridgeConfiguration( );

	typedef enum {
		STATE_DALIBRIDGE_CFG_DISABLED = 0,
		STATE_DALIBRIDGE_CFG_ENABLED,
		STATE_DALIBRIDGE_CFG_PENDING_CHANGES,
		STATE_DALIBRIDGE_CFG_PENDING_ERRORS,
		STATE_DALIBRIDGE_CFG_BUSY
	} StateDaliBridgeCfg_t;

	StateDaliBridgeCfg_t getDALIDuotecnoBridgeCfgState( );

	// Bijhouden veranderingen aan de Duotecno addressering.
	// Wanneer er aanpassingen gebeurd zijn moet bij het afsluiten van de DALI sub bus cfg 
	// gevraagd worden om de Duotecno DALI bridge config aan te passen.
	void setChangesCfgDuotecnoBridge(bool flag);
	bool getChangesCfgDuotecnoBridgeNeeded();

	// Zijn er momenteel fouten aanwezig in de DALI systeem bus configuratie.
	// Kunnen deze functie gebruiken wanneer de gebruiker de Duotecno DALI bridge config wil openen.
	bool getStatePendingErrors( );


	// Namen van de groepen en sferen.
	CString getGroupName(int groupNumber);
	void setGroupName(int groupNumber,const CString& str);
	bool getGroupsNames(CMap<int,int,CString,CString>* dest);	// Er wordt een copy van de map opgeslagen in dest

	CString getSceneName(int sceneNumber);
	void setSceneName(int sceneNumber,const CString& str);
	bool getSceneNames(CMap<int,int,CString,CString>* dest);	// Er wordt een copy van de map opgeslagen in dest

	// zie CHANGES_DALI_TCPIP_COMMUNICATION_SETTINGS
	// Sedert V16.48: controleren of de IP-settings voor de DALI interface ok zijn.
	// Wanneer deze niet ok zijn een melding geven en de UDP-LAN configurator tonen.
	// Na het invullen van de gegevens in de UDP-LAN configurator worden de instellingen doorgevoerd.
	bool checkDaliConnectionSettingsAndPromptUser( );

	// Utility functies...
	static CString getStringInstanceType(int instanceType);
	static bool formatStringAddress(DUOTECNO::DALI::DaliDeviceType_t deviceType,int shortAddress,CString* pStr);
	static bool formatStringProblem(DUOTECNO::DALI::DaliErrorCode_t errorCode,CString* pStr);
	static int getDeviceActualLevel(DUOTECNO::DALI::CAppDaliDevice& refDevice);

	// Utility functie:
	// Opvragen of een control input device instance geldige instellingen heeft om te kunnen werken met de 
	// domotica.
	static bool checkValidSettingForHomeAutomation(DUOTECNO::DALI::CAppDaliInputControlDeviceInstance& r);

	// Utility.
	static BOOL getStringFadeRate(CString* pstrValue, int value);
	static BOOL getStringFadeTime(CString* pstrValue, int value);


public:	// IAppDaliHost_App
	virtual CString getFullyQualifiedName(int nodeAddress);	
	virtual void notifyDataSaved( );

protected:
	// bool connect(const char* hostName,int port,const char* pwd);
	DUOTECNO::DALI::CAppDali* getCurrent( );
	CCanNode* getCurrentNode( );
	bool getStrEventMsg(int shortAddress,int instanceNumber,uint8_t eventData,int* pInstanceType,CString* pStrEventMsg,CString* pStrInstanceType);
	int helper_getFirstDeviceInGroup(int groupNR, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* properties);

private:
	DUOTECNO::DALI::CAppDaliManager& getAppDaliManager( );
	const DUOTECNO::DALI::CAppDaliManager& getAppDaliManager( ) const;

};

/******************************************************************/
#endif 
/******************************************************************/