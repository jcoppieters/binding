#pragma once
#include "appdali_fwd.h"
#include "appdali_common.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliTransmitter;
class CAppDaliInputControlDeviceInstances;
class CAppDaliInputControlDeviceInstance;
class CAppDaliDeviceProperties_ControlGear;
class CAppDaliFsmErrorEntry;
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Communicatie van de helper fsm naar de parent fsm.
struct IAppDaliFsm_HelperFsmEvents {

	typedef enum {		
		ALGORITME_NAME_SEARCH_DEVICES = 0,									// Control gear.
		ALGORITME_NAME_SEARCH_DEVICE,										// Control gear + Control input devices.		
		ALGORITME_NAME_REQUEST_PROPERTIES,									// Control gear + Control input devices.	
		ALGORITME_NAME_REQUEST_MEMORYMAP,									// Control gear + Control input devices.							
		ALGORITME_NAME_CONTROLGEAR_REQUEST_GROUPS,							// Control gear
		ALGORITME_NAME_CONTROLGEAR_REQUEST_SCENES,							// Control gear													
		ALGORITME_NAME_CONTROLDEVICE_REQUEST_PROPERTIES_INSTANCE,			// Control input device instance	
		ALGORITME_NAME_COMMISSION_ERASEDATA,								// Control gear + Control input devices.
		ALGORITME_NAME_COMMISSION_SEARCH_UNADDRESSED_DEVICES,				// Control gear + Control input devices.					
		ALGORITME_NAME_COMMISSION_ADDRESS_DEVICES,							// Control gear + Control input devices.
		ALGORITME_NAME_COMMISSION_CONTROLDEVICE_INSTANCE_AUTOPARAMS,		// Control input devices.
		ALGORITME_NAME_CONFIG_CONTROLDEVICE_INSTANCE_PARAMS,				// Control input devices.
		ALGORITME_NAME_SHORTADDRESS_ASSIGN,									// Control gear + Control input devices.	
		ALGORITME_NAME_SHORTADDRESS_ERASE,									// Control gear + Control input devices.
		ALGORITME_NAME_CONTROLGEAR_CFG_PROPERTIES,							// ControlGear properties 
		ALGORITME_NAME_CONTROLGEAR_CFG_SCENES,								// ControlGear properties 
		ALGORITME_NAME_CONTROLGEAR_CFG_SINGLE_SCENE,						// ControlGear properties 
		ALGORITME_NAME_CONTROLGEAR_QUERY_ACTUALLEVEL,						// Controlgear properties.	
		ALGORITME_NAME_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF,					// control input device instance on/off
		ALGORITME_NAME_SHORTADDRESSES_SWAP,									// Control gear + Control input devices: Verwisselen van twee short addresses
		ALGORITME_NAME_CONTROLGEAR_SELECT_DIMMINGCURVE						// Control gear.
												
	} AlgoritmeName_t;
	
	virtual ~IAppDaliFsm_HelperFsmEvents() = 0;

	// Gestart met het lezen van data
	virtual void onNotifyFsmAlgoritmeStarted(DaliDeviceType_t daliDeviceType, int shortAddress, AlgoritmeName_t type,int param1,int param2) = 0;

	// Data ontvangen.
	virtual void onNotifyFsmAlgoritmeDone(DaliDeviceType_t daliDeviceType, int shortAddress, AlgoritmeName_t type,int param1,int param2) = 0;

	// Fout gebeurd...
	virtual void onNotifyFsmAlgoritmeError(DaliDeviceType_t daliDeviceType, int shortAddress, AlgoritmeName_t type, int param1, int param2) = 0;

	// Er werd een adres toegekend.
	virtual void onNotifyFsmAlgoritmeShortAddressAssigned(DaliDeviceType_t daliDeviceType, int shortAddress) = 0;

	// Er werd een control input device instantie geconfigureerd.
	virtual void onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(int shortAddress, int instanceNumber,int instanceType) = 0;

	// Weergave in listbox.
	virtual void onNotifyFsmAlgoritmeMsgInfo(const CString& str) = 0;
};
/***********************************************************************************/


/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
class CAppDaliFsmHelper_Base;
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Deze klasse wordt gebruikt voor het bijhouden van de info ivm een fatale fout in de FSM.
class ErrorInfo {
public:
	ErrorInfo() : m_errorCode(ERROR_CODE_NONE), m_shortAddress(-1) { }

	void clear() {
		this->m_errorCode = ERROR_CODE_NONE;
		this->m_shortAddress = -1;
		this->m_errorMsg.Empty();
	} 

	DaliErrorCode_t getErrorCode() const {
		return this->m_errorCode;
	}
	int getShortAddress() const {
		return this->m_shortAddress;
	}
	CString getErrorMsg( ) const {
		return this->m_errorMsg;
	}

	void set(DaliErrorCode_t errorCode, int shortAddress, const CString& strErrorMsg) {
		this->m_errorCode = errorCode;
		this->m_shortAddress = shortAddress;
		this->m_errorMsg = strErrorMsg;
	}
private:
	DaliErrorCode_t m_errorCode;
	CString m_errorMsg;
	int m_shortAddress;
};
/************************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmHelper_Base {
public:
	CAppDaliFsmHelper_Base(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelper_Base( ) = 0;

	// virtual int getState() const = 0;
	virtual bool cancel( ) = 0;	
	// Acties...
	virtual bool next( ) = 0;

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext, int data) = 0;
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext) = 0;
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext) = 0;
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext) = 0;
	virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext);

	// Opvragen van de toestand.
	int getState( ) const {
		return this->m_state;
	}

	// Opvragen toestand algoritme.
	bool isIdle();
	bool isDone();
	bool isError();
	bool isCancelled();
		
	////////////////////////////////////////////////////////////////////////////////////////////
	// Fatal error info	: Deze info wordt gebruikt wanneer een algoritme niet kan verder doen
	// door een fatale fout.
	// - Bij de addressering: Wanneer een fatale fout optreedt in de FSM
	// - Bij een standalone algoritme waar er direct een resultaat van verwacht wordt.	
	ErrorInfo getFatalErrorInfo() const {
		return this->m_fatalErrorInfo;
	}

protected:
	// Het shortAddress
	void setShortAddress(int shortAddress) {
		this->m_shortAddress = shortAddress;
	}
	int getShortAddress() {
		return this->m_shortAddress;
	}

	// Type device
	DaliDeviceType_t getDaliDeviceType( ) const {
		return this->m_daliDeviceType;
	}

	// de transmitter.
	CAppDaliTransmitter& getTransmitter( ) {
		return this->m_refTransmitter;
	}
	IAppDaliFsm_HelperFsmEvents& getParent() {
		return this->m_refParent;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Fatal error info	: Deze info wordt gebruikt wanneer een algoritme niet kan verder doen
	// door een fatale fout.
	void setFatalErrorInfo(DaliErrorCode_t errorCode, int shortAddress,const CString& strError);
	void clearFatalErrorInfo();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Error gebeurd tijdens het addresseren. De FSM wordt door deze fout niet gestopt.
	//
	// Error info doorgeven aan de context via een entry.
	// Bij het einde van de algoritmes kan de info weergegeven worden.
	// Deze wordt gebruikt om tijdens de addressering fouten te melden. De Algoritmes/FSM wordt niet gestopt.
	void setErrorEntry(CAppDaliFsmContext& refContext, const CAppDaliFsmErrorEntry& refErrorEntry);


	////////////////////////////////////////////////////////////////////////////////////////////
	// Algemene counter waarde voor de algoritmes.
	int getCounterValue() const;
	void clearCounterValue();
	void incrementCounterValue( );
	void decrementCounterValue( );

	void setDaliDeviceType(DaliDeviceType_t type) {
		this->m_daliDeviceType = type;
	}

	enum {
		// Short addressen
		DALI_SHORTADDRESS_FIRST					= 0,		
		DALI_SHORTADDRESS_MAX					= 64,
		DALI_SHORTADDRESS_LAST					= (DALI_SHORTADDRESS_MAX-1),

		// Control gear groups.
		DALI_CONTROLGEAR_GROUP_FIRST			= 0,		
		DALI_CONTROLGEAR_GROUP_MAX				= 16,
		DALI_CONTROLGEAR_GROUP_LAST				= (DALI_CONTROLGEAR_GROUP_MAX-1),

		// Control device instantie addressing scheme.
		// Gebruiken standaard altijd Device/Instance address = 2
		DALI_CONTROLDEVICE_INSTANCE_ADDRESSSCHEME_DEFAULT = 2,
		
		// Search Address
		DALI_SEARCHADDRESS_LAST					= 0xFFFFFF,
		DALI_SEARCHADDRESS_DEFAULT				= 0xFFFFFF,
	};

protected:
	// Transmit functies ifv het type DALI device.
	bool sendBroadcastInitialiseAll(DaliDeviceType_t daliDeviceType,int address);
	bool sendBroadcastRandomize(DaliDeviceType_t daliDeviceType);
	bool sendQueryDeviceStatus(DaliDeviceType_t daliDeviceType,int address);
	bool sendBroadcastSearchAddrL(DaliDeviceType_t daliDeviceType,int data);
	bool sendBroadcastSearchAddrM(DaliDeviceType_t daliDeviceType,int data);
	bool sendBroadcastSearchAddrH(DaliDeviceType_t daliDeviceType,int data);
	bool sendBroadcastTerminate(DaliDeviceType_t daliDeviceType);
	bool sendBroadcastCompare(DaliDeviceType_t daliDeviceType);
	bool sendProgramShortAddress(DaliDeviceType_t daliDeviceType,int address);
	bool sendQueryShortAddress(DaliDeviceType_t daliDeviceType);
	bool sendBroadcastWithdraw(DaliDeviceType_t daliDeviceType);

protected:
	bool cancelTask( );

	enum {
		FSM_BASE_STATE_READY = 0,
		FSM_BASE_STATE_DONE,
		FSM_BASE_STATE_ERROR,
		FSM_BASE_STATE_CANCEL,
		FSM_BASE_STATE_ALGORITME
	};

	void _setState(int state) {
		this->m_state = state;
	}

private:
	IAppDaliFsm_HelperFsmEvents& m_refParent;
	CAppDaliTransmitter& m_refTransmitter;
	int m_state;									// De toestand (zie enum + enums in de afgeleide klassen.)
	int m_shortAddress;								// dali short address
	DaliDeviceType_t m_daliDeviceType;				// Het type device (control gear / control inout devices).
	int m_counterValue;								// general purpose counter waarde.
	ErrorInfo m_fatalErrorInfo;						// Reden waarom het algoritme gestopt is met een fatale fout.
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
/* Algoritme voor het starten met zoeken.
 *
 * 1) startQuiescentMode( ).
 * 2) Initialse all( ).
 *
 * Gebruik:
 *		- FSM start zoeken naar devices (control gear / input devices)
 ***********************************************************************************/
class CAppDaliFsmHelperSearchDevices : public CAppDaliFsmHelper_Base {
public:
	CAppDaliFsmHelperSearchDevices(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperSearchDevices();

	bool start( );
	virtual bool cancel();

	// Acties...
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext);

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_QUIESCENT_MODE,
		STATE_INITIALISE,
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************
 * Algoritme voor het zoeken naar Control devices 
 *	- Broadcast op de bus om te controleern of er control gear aanwezig is.
 *  - Indidividueel zoeken fv het shortAddress.
 * 
 * Input: ShortAddress.
 * Output: boolean found device
 *
 * Acties:
 *		- Transmit: Query control gear present naar short address.
 *	    - Transmit: Query device status.
 *
 * Het opvragen van de device status is een extra controle.
 * Wanneer we een geldig antwoord krijgen hebben we een device gevonden.
 *
 * Gebruik:
 *		- FSM zoeken naar een control gear device.
 ***********************************************************************************
 * Sedert V17.04: 
 * Oudere DALI control gear reageert niet op de broadcast 'DALI_CMD_QUERY_CONTROL_GEAR_PRESENT'.
 * Waneer er geen antwoord komt op deze broadcast queury dan nog eens alle control gear adressen
 * overlopen van adres 0 tem adres 64.
 * Van zodra er een adres gevonden werd wordt de vlag 'm_foundDevices' op true geplaatst.
 ***********************************************************************************/
class CAppDaliFsmHelperSearchControlGearDevice : public CAppDaliFsmHelper_Base {
public:
	CAppDaliFsmHelperSearchControlGearDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperSearchControlGearDevice();

	// Individueel zoeken naar een device ifv het short address.
	bool start(int shortAddress);

	// Broadcast zoeken op de bus of er DALI gear devices aanwezig zijn.
	bool start( );

	virtual bool cancel();

	// Acties...
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

	bool foundDevices() const {
		return this->m_foundDevices;
	}
	// bool foundDevice(int shortAddress) const;

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_QUERY_DEVICES_BROADCAST_PRESENT,		// Broadcast: control gear devices aanwezig op de bus?
		STATE_QUERY_DEVICES_DEVICE_PRESENT,			// Broadcast mode: Individueel device present query sturen. (Sedert V17.04)
		STATE_QUERY_DEVICE_PRESENT,					// Zoeken naar een bepaald control gear device f(shortAddress).
		STATE_QUERY_DEVICE_STATUS,					// Zoeken naar een bepaald control gear device f(shortAddress).
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	bool m_foundDevices;						// Device is goed gevonden.	
	bool m_modeSearchBroadcast;					// Broadcast of individuel.
	int m_retryCounter;							// Wordt gebruikt bij het versturen van een broadcast.

	typedef CAppDaliFsmHelper_Base Base;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmHelperSearchControlInputDevice : public CAppDaliFsmHelper_Base {
public:
	CAppDaliFsmHelperSearchControlInputDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperSearchControlInputDevice();

	// Zoeken naar 1 device.
	// Wanneer het algoritme gedaan is kunnen we via 'foundDevices()' opvragen of het device gevonden werd.
	// Bij een frame error hebben we mogelijk dubbele addressen.
	bool start(int shortAddress);

	// Versturen van een broadcast request status naar alle devices.
	// Wanneer we een antwoord krijgen dan zijn er devices aanwezig. (Ontvangen van een frame error is wanneer er meerdere antwoorden.)
	// Starten met het versturen van een initialiseAll( ).
	// Wanneer het algoritme gedaan is kunnen we via 'foundDevices()' opvragen of er devices gevonden werd.
	//
	// Sedert V17.04: Wanneer er op de broadcast query geen antwoord komt. Nog eens alle control input devices afzonderlijk
	// proberen te vinden.
	bool startBroadcast();

	// Zoeken op de bus of er DALI input control devices aanwezig zijn.
	// Iteratie van de short address 0 tem 63
	// Wanneer het algoritme gedaan is kunnen we via 'foundDevices()' opvragen of er devices gevonden werd.
	// Het aantal devices kan opgrevraagd worden via 'getCountDevices'.
	// Er is ook een lijst van alle gevonden devices.
	// DEZE WORDT MOMENTEEL NIET GEBRUIKT...
	bool startIterate();

	virtual bool cancel();

	// Acties...
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

	bool foundDevices() const {
		return (this->m_CounterfoundDevices > 0 ? true : false);
	}
	int getCountDevices() const {
		return this->m_CounterfoundDevices;
	}
	bool foundDevice(int shortAddress) const;

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,	
		STATE_INITIALSE,									// Initialse (0xff)
		STATE_QUERY_DEVICE_STATUS,							// Zoeken naar een bepaald control gear device f(shortAddress).
	} State_t;

	typedef enum {
		MODE_BROADCAST_ALL = 0,								// Broadcast versturen en wachten tot er minimum 1 antwoord.
		MODE_REQUEST_SINGLE_DEVICE,							// Opvragen ifv het short address.
		MODE_ITERATE_ALL_DEVICES							// Alle devices opvragen 0 tem 63
	} Mode_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	Mode_t m_mode;											// De mode van het algoritme.
	int m_CounterfoundDevices;								// Aantal devices die gevonden werden.
	bool m_listFoundDevices[DALI_SHORTADDRESS_MAX];			// Lijst van de devices die gevonden werden.
	bool m_busySearchAllDevices;							// Sedert V17.04: Na de broadcast nog eens alle devices afzonderlijk proberen te vinden.

	typedef CAppDaliFsmHelper_Base Base;
};

/***********************************************************************************
 * Algoritme voor het opvragen van de properties van de control gear
 * 
 * Input: ShortAddress.
 * Output: Opgevraagde properties.
 *
 * Acties:
 *		- Transmit: Query van de verschillende properties.
 *
 * Gebruik:
 *		- FSM zoeken naar een control gear device.
 *
 ***********************************************************************************
 * TODO_DALI_FSM_ALGORITMES
 *  Wanneer we niet alle properties kunnen opvragen moeten we dan in fout toestand gaan?
 *	Sommige van de properties zijn bij het zoeken / addresseren minder belangrijk.
 *	Evt in de Parent FSM bij een fout een melding/warning geven maar toch verder doen 
 *  met de FSM???
 *
 ***********************************************************************************
 * Oppassen:
 *	In de specificatie 'IEC 62386-102-2022' 3rd edition zijn er queries/commando's toegevoegd.
 *	Niet alle control gear is conform deze specificatie.
 *	Moeten hier rekening meehouden in de FSM.
 * 
 *	- DALI_CMD_QUERY_OPERATING_MODE		= 0x9E,			// Oppassen: 'IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
 *	- DALI_CMD_QUERY_LIGHT_SOURCE_TYPE	= 0x9F,			// Oppassen: 'IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
 *	D.w.z. Dat we deze een NoResponse kunnen terugkrijgen.
 ***********************************************************************************/
class CAppDaliFsmHelperRequestPropertiesControlGear : public CAppDaliFsmHelper_Base  {
public:
	CAppDaliFsmHelperRequestPropertiesControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperRequestPropertiesControlGear( );

	bool start(int shortAddress,bool busyCommisioning = true);
	virtual bool cancel( );

	// Acties...
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */

	// Properties...
	int getDeviceType() {
		return this->m_deviceType;
	}
	int getDeviceVersion() {
		return this->m_deviceVersion;
	}
	int getDeviceStatus() {
		return this->m_deviceStatus;
	}
	int getDeviceOperatingMode() {
		return this->m_deviceOperatingMode;
	}
	int getDeviceActualLevel() {
		return this->m_deviceActualLevel;
	}
	int getDeviceMinimumLevel() {
		return this->m_deviceMinLevel;
	}
	int getDeviceMaximumLevel() {
		return this->m_deviceMaxLevel;
	}
	int getDeviceFadeTimeRate() {
		return this->m_deviceFadeTimeRate;
	}
	int getDevicePhysicalMinimum() {
		return this->m_devicePhysicalMinimum;
	}
	int getDevicePowerOnLevel( ) {
		return this->m_devicePoweronLevel;
	}
	int getDeviceFailureLevel( ) {
		return this->m_deviceFailureLevel;
	}
	int getDeviceExtendedFadeTime( ) {
		return this->m_deviceExtFadeTime;
	}
	int getDimmingCurve() {
		return this->m_dimmingCurve;
	}
	CList<int>& getDeviceTypes( ) {
		return this->m_deviceTypes;		
	}
	int getColorFeatures() {
		return this->m_colorFeatures;
	}

	// Working mode flags: Bepaald of het algoritme standaone aangeroepen wordt of tijdens de commisioning.
	bool getWorkingmodeStandalone() const;
	bool getWorkingmodeCommisioning() const;


private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_CFG_MODE_INITIALISE,						// Standalone mode.
		STATE_CFG_MODE_ENTER_QUIESCENT_MODE,			// Standalone mode.
		STATE_QUERY_DEVICE_TYPE,
		STATE_QUERY_NEXT_DEVICE_TYPE,					// Sedert V16.48: Meerdere device types (dt6/dt8)
		STATE_ENABLE_DEVICETYPE_DT6_DIMMINGCURVE,		// Sedert V16.46: Device type 6 dimming curve opvragen.
		STATE_QUERY_DEVICETYPE_DT6_DIMMING_CURVE,		// Sedert V16.46: Device type 6 dimming curve opvragen.
		STATE_ENABLE_DEVICETYPE_DT8_COLORFEATURES,		// Sedert V16.48: Device type 8 color features opvragen.
		STATE_QUERY_DEVICETYPE_DT8_COLORFEATURES,		// Sedert V16.48: Device type 8 color features opvragen.
		STATE_QUERY_DEVICE_VERSION,		
		STATE_QUERY_DEVICE_STATUS,	
		STATE_QUERY_DEVICE_OPERATING_MODE,				// Oppassen: 'IEC 62386-102-2022' 3rd edition - Niet altijd aanwezig in de ControlGear
		STATE_QUERY_DEVICE_ACTUAL_LEVEL,
		STATE_QUERY_DEVICE_MIN_LEVEL,
		STATE_QUERY_DEVICE_MAX_LEVEL,
		STATE_QUERY_DEVICE_FADE_TIME_RATE,
		STATE_QUERY_DEVICE_EXTENDED_FADETIME,
		STATE_QUERY_DEVICE_PHYSICAL_MINIMUM,
		STATE_QUERY_DEVICE_POWERON_LEVEL,
		STATE_QUERY_DEVICE_FAILURE_LEVEL,				// Oppassen: Dit is de laatste toestand - Moeten bij fouten in deze toestand ook het algoritme stoppen.	
		STATE_CFG_MODE_TERMINATE,						// Standalone mode.
		STATE_CFG_MODE_EXIT_QUIESCENT_MODE,				// Standalone mode.
	} State_t;

	enum { MAX_RETRIES = 3 };

	void setState(State_t state) {
		this->_setState(state);
	}

	// Bepalen of het algoritme de initialise/terminate , enter/exit quiescent mode moet versturen.
	bool useAlgEnterConfigMode( ) const;
	bool useAlgExitConfigMode( ) const;

private:
	enum {
		FLAG_CONFIG_MODE_ENTER		= (1<<0),			// Enter quiescent mode + initialise
		FLAG_CONFIG_MODE_EXIT		= (1<<1),			// Exit quiescent mode + terminate		
		FLAG_USE_STANDALONE			= (1<<2)			// Algoritme afzonderlijk gebruiken voor opvragen van de properties.
	};

	unsigned int m_workingFlags;						// Zie enum
	bool m_requestNextDeviceType;						// Nodig voor het opvragen van meerdere device types.
	
	int m_deviceType;
	int m_deviceVersion;
	int m_deviceStatus;
	int m_deviceOperatingMode;
	int m_deviceActualLevel;
	int m_deviceMinLevel;
	int m_deviceMaxLevel;
	int m_deviceFadeTimeRate;
	int m_devicePhysicalMinimum;
	int m_devicePoweronLevel;
	int m_deviceFailureLevel;
	int m_deviceExtFadeTime;
	int m_dimmingCurve;									// DT6 dimming curve.
	int m_colorFeatures;								// DT8 Color features
	CList<int> m_deviceTypes;							// Mogelijk meerdere device types dt6/dt8 (m_deviceType is 255)

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmHelperRequestPropertiesControlInputDevice : public CAppDaliFsmHelper_Base  {
public:
	CAppDaliFsmHelperRequestPropertiesControlInputDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperRequestPropertiesControlInputDevice( );

	bool start(int shortAddress);
	virtual bool cancel( );

	// Acties...
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */


	// Alle properties...
	uint32_t getRandomAddress( ) {
		return this->m_randomAddress;
	}
	int getDeviceVersion() {
		return this->m_deviceVersion;
	}
	bool getAppControllerEnabled( ) {
		return this->m_appControllerEnabled;
	}
	bool getPowerCycleNotification( ) {
		return this->m_powerCycleNotification;
	}
	int getDeviceCapabilities( ) {
		return this->m_deviceCapabilities;
	}
	int getDeviceStatus( ) {
		return this->m_deviceStatus;
	}
	int getDeviceNumberOfInstances( ) {
		return this->m_deviceNumberInstances;
	}
	int getDeviceOperatingMode() {
		return this->m_deviceOperatingMode;
	}
	uint32_t getBitfieldGroups( ) {
		return this->m_bitfieldGroups;
	}

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_QUERY_RANDOM_ADDRESS_H,
		STATE_QUERY_RANDOM_ADDRESS_M,	
		STATE_QUERY_RANDOM_ADDRESS_L,	
		STATE_QUERY_VESRION_NUMBER,
		STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES,
		STATE_QUERY_APPLICATIONCRTL_ENABLED,
		STATE_QUERY_DEVICE_POWERCYCLE_NOTIFICATION,		
		STATE_QUERY_DEVICE_STATUS,		
		STATE_QUERY_DEVICE_OPERATINGMODE,
		STATE_QUERY_DEVICE_GROUPS_0_7,
		STATE_QUERY_DEVICE_GROUPS_8_15,
		STATE_QUERY_DEVICE_GROUPS_16_23,
		STATE_QUERY_DEVICE_GROUPS_24_31,
		STATE_QUERY_DEVICE_CAPABILITIES,	// Oppassen: Dit is de laatste toestand - Moeten bij fouten in deze toestand ook het algoritme stoppen.
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	uint32_t m_randomAddress;				// data.
	int m_deviceVersion;					// data.
	bool m_appControllerEnabled;			// data.
	bool m_powerCycleNotification;			// data.
	int m_deviceCapabilities;				// data.
	int m_deviceStatus;						// data.
	int m_deviceNumberInstances;			// data.
	int m_deviceOperatingMode;				// data.
	uint32_t m_bitfieldGroups;				// data.

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmHelperRequestControlInputDeviceInstances : public CAppDaliFsmHelper_Base  {
public:
	CAppDaliFsmHelperRequestControlInputDeviceInstances(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperRequestControlInputDeviceInstances( );

	bool start(int shortAddress);
	virtual bool cancel( );

	// Acties...
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */
	
	// Alle properties...
	int getNumberOfInstances()  {
		return this->m_numberOfInstances;
	}
	int getCurrentInstanceID( ) {
		return this->m_currentInstance;
	}
	int getCurrInstanceType( ) {
		return this->m_instanceType;
	}
	bool getCurrInstanceEnabled( ) {
		return this->m_instanceEnabled;
	}
	int getCurrInstancePrimaryGroup() {
		return this->m_instanceGroupInfo[0];
	}
	int getCurrInstanceGroup1() {
		return this->m_instanceGroupInfo[1];
	}
	int getCurrInstanceGroup2() {
		return this->m_instanceGroupInfo[2];
	}
	int getCurrInstanceEventScheme() {
		return this->m_instanceEventScheme;
	}
	int getCurrInstanceEventPrio( ) {
		return this->m_instanceEventPrio;
	}
	int getCurrInstanceEventResolution() {
		return this->m_instanceEventResolution;
	}
	int getCurrInstanceStatus() {
		return this->m_instanceStatus;
	}
	int getCurrInstanceEventFilter() {
		return this->m_instanceEventFilter;
	}

protected:
	void initDataInstance( );

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES,		// voor alle instances.
		STATE_QUERY_DEVICE_INSTANCE_TYPE,			// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_ENABLED,		// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_GROUP_PRIMARY,	// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_GROUP_1,		// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_GROUP_2,		// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_EVENTSCHEME,	// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_EVENTPRIO,		// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_RESOLUTION,		// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_STATUS,			// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_FILTER_0_7,		// voor iedere instantie...		 Belangrijk: Afhandelen Error en No-response.
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	// NOTE
	// Instantie properties: deze properties worden herbruikt voor alle instanties.
	// Telkens wanneer de data ontvangen wordt voor een instantie wordt er een event naar
	// de parent fsm gestuurd. Deze vraagt de properties op van de instantie en gaat de database 
	// updaten.
	// De properties worden bij de start opvragen van een nieuwe instantie gewist.
	int m_numberOfInstances;				// het aantal instanties.
	int m_currentInstance;					// Huidige instantie...
	// onderstaande wordt bijgehouden per instantie - zie NOTE
	int m_instanceType;						// Instantie - type
	bool m_instanceEnabled;					// Instantie - enabled on/off
	int m_instanceGroupInfo[3];				// Instantie - 0=primary, 1=group1 ,2=group2
	int m_instanceEventScheme;				// Instantie	
	int m_instanceEventPrio;				// Instantie
	int m_instanceEventResolution;			// Instantie
	int m_instanceStatus;					// Instantie
	int m_instanceEventFilter;				// Instantie

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

/***********************************************************************************
 * Algoritme voor het opvragen in welke groepen het device aanwezig is.
 * 
 * Input: ShortAddress.
 * Output: Unsigned short (bitfield) bit0=group0
 *
 * Acties:
 *		- Transmit: Query van de group0-7 en group8-15
 *
 * Gebruik:
 *		- FSM zoeken naar een control gear device.
 *
 **********************************************************************************/
class CAppDaliFsmHelperQueryGroupsControlGear : public CAppDaliFsmHelper_Base {
public:
	CAppDaliFsmHelperQueryGroupsControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperQueryGroupsControlGear( );

	bool start(int shortAddress,bool busyCommisioning = true);
	virtual bool cancel( );

	// Acties...
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */

	// properties
	unsigned short getGroupsinfo( ) {
		return this->m_bitfieldGroups;
	}

	// Working mode flags: Bepaald of het algoritme standaone aangeroepen wordt of tijdens de commisioning.
	bool getWorkingmodeStandalone() const;
	bool getWorkingmodeCommisioning() const;

protected:
	// Bepalen of het algoritme de initialise/terminate , enter/exit quiescent mode moet versturen.
	bool useAlgEnterConfigMode( ) const;
	bool useAlgExitConfigMode( ) const;

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_CFG_MODE_INITIALISE,				// Standalone mode.
		STATE_CFG_MODE_ENTER_QUIESCENT_MODE,	// Standalone mode.
		STATE_QUERY_0_7,		
		STATE_QUERY_8_15,				
		STATE_CFG_MODE_TERMINATE,				// Standalone mode.
		STATE_CFG_MODE_EXIT_QUIESCENT_MODE,		// Standalone mode.
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	enum {
		FLAG_CONFIG_MODE_ENTER		= (1<<0),			// Enter quiescent mode + initialise
		FLAG_CONFIG_MODE_EXIT		= (1<<1),			// Exit quiescent mode + terminate		
		FLAG_USE_STANDALONE			= (1<<2)			// Algoritme afzonderlijk gebruiken voor opvragen van de properties.
	};			
										
	uint16_t m_workingFlags;			// Zie enum
	uint16_t m_bitfieldGroups;			// data.

	typedef CAppDaliFsmHelper_Base Base;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

/***********************************************************************************
 * Algoritme voor het opvragen in welke DALI scenee het device aanwezig is.
 * 
 * Input: ShortAddress.
 * Output: Array van 16 waarden. index0 = scene 0
 *		   0-0xFE: Dimmer waarde in de sfeer.	
 *		   MASK(0xff) is niet aanwezig in de sfeer, 
 *
 * Acties:
 *		- Transmit: Query van de sfeer info
 *
 * Gebruik:
 *		- FSM zoeken naar een control gear device.
 *
 **********************************************************************************/

class CAppDaliFsmHelperQueryScenesControlGear : public CAppDaliFsmHelper_Base {
public:
	CAppDaliFsmHelperQueryScenesControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperQueryScenesControlGear( );

	bool start(int shortAddress,bool supportDT8Color, bool busyCommisioning = true);
	virtual bool cancel( );

	// Acties...
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */

	// Working mode flags: Bepaald of het algoritme standaone aangeroepen wordt of tijdens de commisioning.
	bool getWorkingmodeStandalone() const;
	bool getWorkingmodeCommisioning() const;

	enum { MAX_SCENES = 16 };

	// properties
	BYTE getSceneActualLevel(int sceneNumber);
	const CAppDaliObjectDT8ColorData& getSceneDT8ColorData(int sceneNumber);

protected:
	void clear( );

	// Bepalen of het algoritme de initialise/terminate , enter/exit quiescent mode moet versturen.
	bool useAlgEnterConfigMode( ) const;
	bool useAlgExitConfigMode( ) const;

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_CFG_MODE_INITIALISE,				// Standalone mode.
		STATE_CFG_MODE_ENTER_QUIESCENT_MODE,	// Standalone mode.
		STATE_QUERY_SCENE_LEVEL,
		STATE_DT8_LOAD_DTR,						// DT8 Object: DTR laden met een waarde voor QUERY_COLOR_VALUE
		STATE_DT8_ENABLE_DT8_DEVICE,			// Enable Device type 8. Nodig voor de QUERY_COLOR_VALUE
		STATE_DT8_QUERY_COLOR_VALUE, 			// Opvragen van de QUERY_COLOR_VALUE waarde.
		STATE_DT8_QUERY_CONTENT_DTR,			// Opvragen van de LSB waarde.
		STATE_CFG_MODE_TERMINATE,				// Standalone mode.
		STATE_CFG_MODE_EXIT_QUIESCENT_MODE,		// Standalone mode.
	} State_t;
	
	CString getStrState(State_t state);
	void dumpState(State_t state);
	void setState(State_t state) {
		this->dumpState(state);
		this->_setState(state);
	}

protected:

	// Controleren of alle scenes opgevraagd zijn.
	// Wanneer alle scenes opgevvraagd zijn gaan afhankelijk van de mode naar de STATE_DONE toestand of wordt de
	// terminate vlag gezet.
	// return waarde true: Algoritme gedaan.
	bool isAlgoritmeDone( );

	bool sendControlGearQuerySceneLevel( );
	bool startRequestDT8ColorObject(int colorType);
	bool startRequestDT8ColorObjectTemperatureTC(int colorType);
	bool startRequestDT8ColorObjectXCoordinate(int colorType);
	bool startRequestDT8ColorObjectYCoordinate( );
	bool startRequestDT8ColorObjectRGBWAF(int colorType);
	bool startRequestDT8ColorObjectPrimaryNChannels(int colorType);

	typedef enum {
		SUB_STATE_DT8_NONE = -1,
		SUB_STATE_DT8_REQUEST_COLORTYPE = 0,		
		SUB_STATE_DT8_REQUEST_COLORVALUE_TEMP_TC,
		SUB_STATE_DT8_REQUEST_COLORVALUE_X_COORDINATE,
		SUB_STATE_DT8_REQUEST_COLORVALUE_Y_COORDINATE,
		SUB_STATE_DT8_REQUEST_COLORVALUE_RED,
		SUB_STATE_DT8_REQUEST_COLORVALUE_GREEN,
		SUB_STATE_DT8_REQUEST_COLORVALUE_BLUE,
		SUB_STATE_DT8_REQUEST_COLORVALUE_WHITE,
		SUB_STATE_DT8_REQUEST_COLORVALUE_AMBER,
		SUB_STATE_DT8_REQUEST_COLORVALUE_FREECOLOR,
		SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL0,
		SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL1,
		SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL2,
		SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL3,
		SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL4,
		SUB_STATE_DT8_REQUEST_COLORVALUE_CHANNEL5,
	} StateDT8ColorObject;

	void setStateDT8ColorObject(StateDT8ColorObject state);
	StateDT8ColorObject getStateDT8ColorObject() const;

private:
	enum {
		FLAG_CONFIG_MODE_ENTER		= (1<<0),			// Enter quiescent mode + initialise
		FLAG_CONFIG_MODE_EXIT		= (1<<1),			// Exit quiescent mode + terminate		
		FLAG_USE_STANDALONE			= (1<<2)			// Algoritme afzonderlijk gebruiken voor opvragen van de properties.
	};	

	unsigned int m_workingFlags;						// Zie enum
	BYTE m_scenesInfo[MAX_SCENES];
	int m_sceneNumber;
	StateDT8ColorObject m_stateDT8ColorObject;
	bool m_supportDT8ColorObject;						// Bepaalt of er DT8 kleur info moet opgevraagd worden. (Deze moet meegeven worden met de start).
	bool m_requestDT8ColorInfo;
	bool m_flagTerminate;
	CAppDaliObjectDT8ColorData m_colorData[MAX_SCENES];

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

/***********************************************************************************
 * Basisklasse voor algoritme voor het uitlezen van een geheugen map.
 * 
 * Input: ShortAddress, startAddress, aantal Bytes.
 * Output: Uitgelezen properties.
 *
 *
 * Gebruik:
 *		- FSM zoeken naar devices.
 *
 **********************************************************************************
 * TODO_DALI_FSM_ALGORITMES
 *	- Start Address wordt momenteel niet bijgehouden. Starten altijd vanaf 0x00
 **********************************************************************************/
class CAppDaliFsmHelperReadMemory : public CAppDaliFsmHelper_Base { 
public:
	CAppDaliFsmHelperReadMemory(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	// Opvragen van de gelezen properties...
	unsigned long long getGTIN() {
		return this->m_GTIN;
	}
	unsigned long long getSerialNumber() {
		return this->m_serialNumber;
	}

protected:
	// Context variablen init.
	void initContext(int shortAddress,uint16_t address,int numberOfBytes);

	// Bij het uitlezen van een memory bank is op adreslocatie 0x00 het aantal bytes aanwezig
	// die kunnen uitgelezen worden.
	// Soms zijn niet alle bytes beschikbaar die nodig zijn.
	// Via deze waarde kunnen we het aantal bepalen.
	void setLastMemoryLocation(int value);

	// Wachten op een query read memory location...
	bool busyWaitingForResponse( );

	// Wachten op een verify query read memory location...
	bool busyVerifyMemoryRead( );

	// Wanneer er een error frame ontvangen wordt op een backward response.
	// Dit wijst op dubbele addressen.
	void setFlagErrorActive( );
	bool getFlagErrorActive( ) const;

	int getCurrMemoryLocation( ) const {
		return this->m_currentAddress;
	}

protected:
	int m_currentAddress;					// Huidige geheugen locatie.
	int m_nrofBytesToRead;					// Aantal bytes die er moeten gelezen worden.
	int m_currentBytesRead;					// Huidige aantal bytes uitgelezen.
	bool m_waitForResponse;					// Werd er een bericht verstuurd waar we een backward response verwachten.
	bool m_waitForVerify;					// Werd DTR0 uitgelezen om de laatste memory locatie uit te lezen.
	bool m_stateErrorActive;				// Bijhouden of er dubbele addressen gedecteerd werden.
	unsigned long long m_GTIN;				// de product code	- toekennen aan het device.
	unsigned long long m_serialNumber;		// het serie nummer toekennen aan het device.

private:
	typedef CAppDaliFsmHelper_Base Base;
};
/**********************************************************************************/


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
class CAppDaliFsmHelperReadMemory_Bank0 : public CAppDaliFsmHelperReadMemory { 
public:
	CAppDaliFsmHelperReadMemory_Bank0(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	// Opvragen van de gelezen properties...
	int getVersionFirmware( ) const {
		return this->m_versionFirmware;
	}
	int getVersionHardware( ) const {
		return this->m_versionHardware;
	}
	int getVersionNumber( ) const {
		return this->m_versionNumber;
	}
	int getVersionNumberControlGear( ) const {
		return this->m_versionNumberControlGear;
	}
	int getVersionNumberInputDevices( ) const {
		return this->m_versionNumberInputDevices;
	}
	int getNumberOfLogicalUnits( ) const {
		return this->m_numberOfLogicalUnits;
	}
	int getNumberOfLogicalUnitsControlGear( ) const {
		return this->m_numberOfLogicalUnitsControlGear;
	}
	int getNumberOfLogicalUnitsInputDevices( ) const {
		return this->m_numberOfLogicalUnitsControlDevices;
	}

protected:
	void initData( );

	void updateRecievedData(int data);

protected:
	int m_versionFirmware;							// Bruikbaar
	int m_versionHardware;							// Bruikbaar
	int m_versionNumber;							// Bruikbaar.
	int m_versionNumberControlGear;					// 0xff wnr geen controlgear
	int m_versionNumberInputDevices;				// meestal dezelfde als de globale.
	int m_numberOfLogicalUnits;						// Geen bruikbare info staat meestal op 1
	int m_numberOfLogicalUnitsControlDevices;		// Geen bruikbare info staat meestal op 0
	int m_numberOfLogicalUnitsControlGear;			// Geen bruikbare info staat meestal op 0

	typedef CAppDaliFsmHelperReadMemory Base;
};
/**********************************************************************************/

/***********************************************************************************
 * Algoritme voor het uitlezen van een geheugen map.
 * 
 * Input: ShortAddress, startAddress, aantal Bytes.
 * Output: Uitgelezen properties.
 *
 * Acties:
 *	1) send DTR1,
 *	2) send DTR0,
 *  3) Read Memory location.
 *  4) Verify DTR0 laatste geheugen locatie.
 *
 * Gebruik:
 *		- FSM zoeken naar devices.
 **********************************************************************************/
class CAppDaliFsmHelperReadMemoryControlGear : public CAppDaliFsmHelperReadMemory_Bank0 {
public:
	CAppDaliFsmHelperReadMemoryControlGear(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	// Start lezen van de geheugen map.
	bool start(int shortAddress, uint16_t address,int nrofBytes);	
	virtual bool cancel( );

	// acties.
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */

	bool busyReadingMemory( ) {
		return (STATE_MEMORY_READ == this->getState());
	}
private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_MSB,
		STATE_LSB,
		STATE_MEMORY_READ,			
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	typedef CAppDaliFsmHelperReadMemory_Bank0 Base;
};
/***********************************************************************************/

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/***********************************************************************************
 * Algoritme voor het uitlezen van een geheugen map.
 * 
 * Input: ShortAddress, startAddress, aantal Bytes.
 * Output: Uitgelezen properties.
 *
 * Acties:
 *	1) send DTR1,DTR0
 *  2) Read Memory location.
 *  3) Verify DTR0 laatste geheugen locatie.
 *
 * Gebruik:
 *		- FSM zoeken naar devices.
 *
 **********************************************************************************/
class CAppDaliFsmHelperReadMemoryInputDevice : public CAppDaliFsmHelperReadMemory_Bank0 
{
public:
	CAppDaliFsmHelperReadMemoryInputDevice(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	// Start lezen van de geheugen map.
	bool start(int shortAddress, uint16_t address,int nrofBytes);
	virtual bool cancel( );

	// acties.
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);// { return false;	}	/* TODO */
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */

protected:
	bool busyReadingMemory( ) {
		return (STATE_MEMORY_READ == this->getState());
	}

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_LOAD_ADDRESS,		
		STATE_MEMORY_READ
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	typedef CAppDaliFsmHelperReadMemory_Bank0 Base;
};
/***********************************************************************************/

/***********************************************************************************
 * Zoeken naar devices die geen short address hebben.
 * 
 * Stappen:
 *	- initialise unaddresses. (address 0xff)				
 *	- randomize (devices krijgen een random address).
 *	- searchaddr 0xffffff
 *	- compare:
 *		- response -> devices gevonden die geen address hebben.
 *		- no response -> geen devices 
 *	- terminate:
 *
 *
 **********************************************************************************/
class CAppDaliFsmHelperDaliDevicesSearchUnaddressed : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperDaliDevicesSearchUnaddressed(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	bool start(DaliDeviceType_t deviceTypes);
	virtual bool cancel( );

	bool foundDevice() {
		return this->m_foundDevices;
	}

	// acties.
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_INITIALISE,
		STATE_RANDOMIZE,
		STATE_SEARCHADDR_L,
		STATE_SEARCHADDR_M,
		STATE_SEARCHADDR_H,
		STATE_COMPARE,
		STATE_TERMINATE,
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}
private:
	bool m_foundDevices;							// Devices gevonden die geen address hebben.

	typedef CAppDaliFsmHelper_Base Base;
};
/*********************************************************************************/

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/********************************************************************************** 
 * Initialise(all) unaddressed.
 *
 *	Wanneer er devices aanwezig zijn die geen short adres hebben.
 *	
 *	Principe:
 *  Via binary search zoeken naar het laagste adres waarop er geen modules meer
 *  antwoorden.
 * 
 *  Na het toekennen van het address een withdraw versturen zodat dit device niet
 *  meer antwoord op een initialise.
 *  
 *	Na het toekennen opnieuw starten...
 *********************************************************************************
 * Stappen:
 *	*) Initialise all (0xff)
 *  *) Randomize.
 *	*) Devices 0-63: Request device status 
 *	   -> Array aanmalen met alle short addresses die bezet zijn.
 *	*) While (searchAddress != 0xffffff)
 *		-> send address (LSB, MSB, HSB)
 *		-> Compare
 *			-> Yes:
 *				Program short address.
 *			    Request device status
 *			    withdraw.
 *				Recall max level. 
 *	*) Terminate
 *
 *********************************************************************************/
class CAppDaliFsmHelperDaliDevicesAddress : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperDaliDevicesAddress(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	bool start(DaliDeviceType_t daliDeviceType,bool assignOnlyUnAddressed);
	virtual bool cancel( );

	// acties.
	virtual bool next( );

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false;	}	/* TODO */

	// Opvragen hoeveel devices er geaddresseerd werden.
	// Het algoritme gebruikt intern de basis var counterValue.
	int getNumberOfDevices( ) {
		return this->getCounterValue();
	}

protected:
	bool diff_lsb();
	bool diff_msb();
	bool diff_hsb();
	int getNextAvailableAddress( );
	bool handleResponseCompareState( );
	void dbgDumpContext(const CString& strHeader);
	CString getStrState( ) const;

	// Helper functie voor in de toestand STATE_DEVICES_SEARCH
	// Wanneer alle short addresses overlopen zijn in de toestand STATE_DEVICES_SEARCH 
	// gaan we naar de toestand STATE_SEARCHADDRESS
	// De functie return waarde true = gaan naar de toestand STATE_SEARCHADDRESS
	bool goToNextSearchDeviceOrStateSearchAddress( );

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_INITIALISE,
		STATE_RANDOMIZE,
		STATE_DEVICES_SEARCH,			
		STATE_SEARCHADDRESS,
		STATE_SEARCHADDR_L,
		STATE_SEARCHADDR_M,
		STATE_SEARCHADDR_H,
		STATE_COMPARE,
		STATE_ASSIGN,
		STATE_VERIFY_SHORTADDRESS,								
		STATE_WITHDRAWN,
		STATE_DEVICE_RECALL_MAX_LEVEL,
		STATE_TERMINATE,
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:	
	uint8_t m_assignedAddr[DALI_SHORTADDRESS_MAX];	// Array die bijhoudt welke addressen beschikbaar zijn...
	uint8_t m_currShortAddress;						// Het huidige short address. OPPASSEN gebruiken deze ipv de var in de basisklasse!!!	
	uint32_t m_searchAddress;						// huidig search address
	uint32_t m_searchAddrAddSub;					// address voor de sub / add
	uint32_t m_searchAddressPrevious;				// vorige address.
	bool m_searchBusy;								// Bezig met een binary search...
	bool m_assignShortAddress;						// address toekennen.
	bool m_terminate;	
	bool m_assignOnlyUnAddressed;					// sedert V16.39: Nodig voor extend installation.

	typedef CAppDaliFsmHelper_Base Base;
};
/*********************************************************************************/


/**********************************************************************************
* Wissen van de addressering van de control gear devices.
*
* Stappen:
*	*) Initialise all 
*	*) load DTR0 MASk 0xFF)
*	*) Program DTR0 as shortAddress.
*	*) Broadcast remove from groups (groups 0-15)
*	*) Broadcast recall minimum level.
*	*) Terminate.
***********************************************************************************/
class CAppDaliFsmHelperControlGearErase : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperControlGearErase(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	bool start();
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext, int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */


private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_INITIALISE,
		STATE_SHORTADDRESS_DRT0,
		STATE_SHORTADDRESS,
		STATE_REMOVE_FROM_GROUP,
		STATE_REMOVE_FROM_GROUP_DONE,
		STATE_DEVICE_RECALL_MIN_LEVEL,
		STATE_TERMINATE		
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	int m_currGroup;

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************
 * Wissen data van een input control device.
 *
 * Stappen:
 *	*) Broadcast: Load DTR0 met 0xff
 *	*) Broadcast: Set short address DTR0
 *  *) Broadcast:	NIET OK !!!!!!!!
 *	*) Broadcast: Remove from device groups 0-15
 *	*) Broadcast: Remove from device groups 16-31
 ***********************************************************************************
 * Logging:
 *
 * RX :2024-08-09 13:08:11.234:  c130ff - DTR0=255 (0xff) (address=N/A)
 * RX :2024-08-09 13:08:11.270:  fffe14 - Set short address (D=BROADCAST,Device)
 * RX :2024-08-09 13:08:11.303:  fffe14 - Set short address (D=BROADCAST,Device)
 * RX :2024-08-09 13:08:11.481:  c9ffff - Dtr2:Dtr1: data2=255 (0xff), data1=255 (0xff) (address=N/A)					
 * RX :2024-08-09 13:08:11.520:  fffe1b - Remove from device groups 0-15 (D=BROADCAST,Device)
 * RX :2024-08-09 13:08:11.550:  fffe1b - Remove from device groups 0-15 (D=BROADCAST,Device)
 * RX :2024-08-09 13:08:11.588:  fffe1c - Remove from device groups 16-31 (D=BROADCAST,Device)
 * RX :2024-08-09 13:08:11.620:  fffe1c - Remove from device groups 16-31 (D=BROADCAST,Device)
 ************************************************************************************/
class CAppDaliFsmHelperControlInputDeviceErase : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperControlInputDeviceErase(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	bool start();
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */


private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_SHORTADDRESS_DRT0,
		STATE_SHORTADDRESS,
		STATE_DTR1_DTR2,
		STATE_REMOVE_FROM_GROUP_0_15,
		STATE_REMOVE_FROM_GROUP_16_31
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
//
// - Voor alle input devices:
//	 - Wanneer er instances aanwezig zijn altijd de applicatie controller disablen.
//
// - Pushbutton (Type=1)
//	 - DALI-2 events instance Enable.
//	 - Address schema: Device/Instance addressing.
//	 - Event filters: Short press, long on, long off.
//
//
// - Occupancy detector (Type=3)
//	 - DALI-2 events instance Enable.
//	 - Address schema: Device/Instance addressing.
//	 - Event filters: Alle Event messages aan.
//	 - Report timer interval: Waarde tss 50 en 33 sec. (Telkens een andere waarde.)
//
// 
// - Light sensor (Type=4)
//	 - DALI-2 events instance Disable.
//	 
// 
// TODO: Input button 

class CAppDaliFsmHelperControlInputDeviceInstanceAutoParams : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperControlInputDeviceInstanceAutoParams(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	bool start(CList<int>& listDevices);
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_SEARCH_DEVICE,
		STATE_QUERY_DEVICE_NUMBER_OF_INSTANCES,
		STATE_SET_DEVICE_APPLICATION_CONTROLLER_DISABLE,	// Voor een device (Enkel wanneer deze instances heeft...)
		STATE_QUERY_DEVICE_INSTANCE_TYPE,					// voor iedere instantie...
		STATE_QUERY_DEVICE_INSTANCE_ENABLED,				// voor iedere instantie...
		STATE_SET_DEVICE_INSTANCE_ENABLE,					// voor iedere instantie...
		STATE_SET_DEVICE_INSTANCE_DISABLE,					// voor iedere instantie...
		STATE_SET_DEVICE_INSTANCE_SCHEME_DTR0,				// voor iedere instantie...
		STATE_SET_DEVICE_INSTANCE_SCHEME,					// voor iedere instantie...
		STATE_SET_DEVICE_INSTANCE_EVENTFILTER_DTR0,			// voor iedere instantie...
		// STATE_SET_DEVICE_INSTANCE_EVENTFILTER_DTR1,		// voor iedere instantie...
		// STATE_SET_DEVICE_INSTANCE_EVENTFILTER_DTR2,		// voor iedere instantie...
		STATE_SET_DEVICE_INSTANCE_EVENTFILTER,				// voor iedere instantie...
		STATE_SET_DEVICE_INSTANCE_EVENTFILTER_WAIT,

		// Speciale toestanden voor de DALI Occupancy detector.
		STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER_DTR0,
		STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER,
		STATE_SET_DEVICE_INSTANCE_OCCUPANCY_REPORTTIMER_WAIT

	}State_t;

protected:	
	bool setListShortAddresses(CList<int>& src);

	// helper functie om naar de volgende control device instantie te gaan.
	// Naar de volgende instantie/ short address gaan + vlaggen zetten.
	// return waarde is de toestand.
	State_t helper_gotoNextInstance();

	// helper functie om naar de volgende device (shortaddress) instantie te gaan.
	State_t helper_gotoNextDevice();

	// Verify functies...
	bool verifyShortAddress( );
	bool verifyControlDeviceInstanceNumber( );
	bool verifyControlDeviceInstanceType( );

	int getCfgEventFilter(int deviceInstanceType);
	int getCfgEventAddressingScheme(int deviceInstanceType);

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	CList<int> m_listDevices;						// De lijst van devices die moet uitgevoerd worden door het algoritme.
	POSITION m_currListPosition;					// De huidige index in de lijst.
	int m_numberOfInstances;						// Het aantal instanties van het control device (max32)
	int m_currentInstance;							// Het huidige instance nummer
	int m_instanceType;								// Het huidige instantie type (momenteel enkel pushbutton)
	bool m_instanceEnabled;							// Opvragen of de instantie enabled is.
	bool m_foundDevice;								// Device shortaddress gevonden. Wortd gezet bij het ontvangen van een response op een search device.
	bool m_cfgAutoParams;							// Config Auto voor de deze instantie. Wordt gezet bij het ontvangen van de instantie type.
	bool m_cfgDisableInstance;						// Config disable instance events
	bool m_deviceDetectedLightSensor;				// Sedert V16.42

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************************/


/*

	************************************** CONTROL GEAR *****************************
	A15: Control gear short address wissen.
		RX :2024-10-16 14:41:24.891:  a500 - Initialise (all) (address=not used)
		RX :2024-10-16 14:41:24.917:  a500 - Initialise (all) (address=not used)
		RX :2024-10-16 14:41:24.955:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-16 14:41:24.987:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-16 14:41:25.016:  a3ff - DTR0=255 (0xff) (address=not used)
		RX :2024-10-16 14:41:25.043:  1f80 - Set shortaddress (DTR0) D=15
		RX :2024-10-16 14:41:25.070:  1f80 - Set shortaddress (DTR0) D=15
		RX :2024-10-16 14:41:25.200:  a100 - Terminate (address=not used)
		RX :2024-10-16 14:41:25.237:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)
		RX :2024-10-16 14:41:25.270:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)

	A15: Control gear new short address A16 toekennen
		RX :2024-10-16 14:54:32.421:  a500 - Initialise (all) (address=not used)
		RX :2024-10-16 14:54:32.446:  a500 - Initialise (all) (address=not used)
		RX :2024-10-16 14:54:32.483:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-16 14:54:32.516:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-16 14:54:32.546:  a321 - DTR0=33 (0x21) (address=not used)					(NOTE_DTR0_SHORTADDRESS)
		RX :2024-10-16 14:54:32.571:  1f80 - Set shortaddress (DTR0) D=15
		RX :2024-10-16 14:54:32.598:  1f80 - Set shortaddress (DTR0) D=15
		RX :2024-10-16 14:54:32.733:  2191 - Query control gear present D=16
		RX :2024-10-16 14:54:32.747:  ff
		RX :2024-10-16 14:54:32.832:  a100 - Terminate (address=not used)
		RX :2024-10-16 14:54:32.868:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)
		RX :2024-10-16 14:54:32.902:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)

	/************************************* CONTROL INPUT DEVICE ***********************

	A62: Control input device wissen:		
		RX :2024-10-17 16:20:26.729:  a500 - Initialise (all) (address=not used)
		RX :2024-10-17 16:20:26.755:  a500 - Initialise (all) (address=not used)
		RX :2024-10-17 16:20:26.790:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-17 16:20:26.824:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-17 16:20:26.860:  c130ff - DTR0=255 (0xff) (address=N/A)
		RX :2024-10-17 16:20:26.897:  7dfe14 - Set short address (D=62,Device)
		RX :2024-10-17 16:20:26.929:  7dfe14 - Set short address (D=62,Device)
		RX :2024-10-17 16:20:27.064:  a100 - Terminate (address=not used)
		RX :2024-10-17 16:20:27.102:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)
		RX :2024-10-17 16:20:27.134:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)


	A61 Control input device veranderen naar A11		
		RX :2024-10-17 16:23:01.069:  a500 - Initialise (all) (address=not used)
		RX :2024-10-17 16:23:01.095:  a500 - Initialise (all) (address=not used)
		RX :2024-10-17 16:23:01.132:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-17 16:23:01.165:  fffe1d - Start quiescent mode (D=BROADCAST,Device)
		RX :2024-10-17 16:23:02.263:  c1300b - DTR0=11 (0x0b) (address=N/A)
		RX :2024-10-17 16:23:02.300:  7bfe14 - Set short address (D=61,Device)
		RX :2024-10-17 16:23:02.333:  7bfe14 - Set short address (D=61,Device)
		RX :2024-10-17 16:23:02.481:  17fe34 - Query version number (D=11,Device)
		RX :2024-10-17 16:23:02.496:  08		
		RX :2024-10-17 16:23:03.616:  a100 - Terminate (address=not used)
		RX :2024-10-17 16:23:03.653:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)
		RX :2024-10-17 16:23:03.686:  fffe1e - Stop quiescent mode (D=BROADCAST,Device)
*/


// NOTE_CONTROLGEAR_ADDRESSING_PROBLEM:
// Wanneer we een nieuw adres toekennen aan een control gear device dan is er bij sommige devices een 
// extra tijd nodig om het adres toe kennen.
// Hierdoor is er een probleem dat we niet altijd detecteren of we een dubbel adres toegekend hebben.
//
// Aanpassingen:
//	- De Verify queries uitvoeren na het versturen van de terminate.
//	- De Verify queries een aantal keren na elkaar versturen. (Verhogen is geen oplossing.)
//
class CAppDaliFsmHelperAssingShortAddress : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperAssingShortAddress(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	
	// Om een device te wissen: newShortAddress 0xff
	bool start(DaliDeviceType_t deviceTypes,int shortAddress,int newShortAddress = 0xff);
	
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

	// Opvragen of het algoritme geslaagd is...
	bool success() {
		return this->m_stateSuccess;
	}

protected:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_INITIALISE,
		STATE_ENTER_QUIESCENT_MODE,
		STATE_DTR0_LOAD_ADDRESS,
		STATE_SHORTADDRESS_SET,
		STATE_TERMINATE,
		STATE_VERIFY_ANSWER,
		STATE_EXIT_QUIESCENT_MODE,		
	}State_t;

private:
	typedef enum {
		TASK_UNKNOWN = 0,
		TASK_ASSIGN_NEW_ADDRESS,
		TASK_ERASE_ADDRESS
	} Task_t;

	enum { COUNTER_MAX_VERIFY = 5 };				// zie Probleem: NOTE_CONTROLGEAR_ADDRESSING_PROBLEM

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	Task_t m_task;
	int m_newShortAddress;							// Het nieuwe short address die moet toegekend worden.
	int m_verifyCounter;
	bool m_stateSuccess;							// Status van het algoritme (afh van de verify.)

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Algoritme voor het configureren van de device instance parameters.
class CAppDaliFsmHelperControlInputDeviceInstancesCfg : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperControlInputDeviceInstancesCfg(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlInputDeviceInstancesCfg();

	// Oppassen:
	// de config data wordt doorgegeven via de clone().
	// deze klasse houdt een eigen instantie bij van de cfg data.
	bool start(int shortAddress, CAppDaliInputControlDeviceInstances* data);
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

	// Nodig voor het updaten van de database wanneer het algoritme gedaan is.
	const CAppDaliInputControlDeviceInstances& getcfgData( ) const;
	CAppDaliInputControlDeviceInstances& getcfgData( );

protected:
	void deallocateCfgData();

	// Helper functies.
	bool getInstance(int instanceNumber, CAppDaliInputControlDeviceInstance** ppInstance);
	bool getInstanceEnabled(int instanceNumber);
	int getInstanceEventFilters(int instanceNumber);
	int getInstanceEventAddressScheme(int instanceNumber);
	int getInstanceEventPrio(int instanceNumber);
	int getInstanceType(int instanceNumber);
		// todo andere...

	// Zoeken naar de volgende instantie nummer.
	// Indien de laatste wordt -1 terug gegeven.
	int getNextInstanceNumber(int instanceNumber);

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,	
		STATE_CFG_ENABLE,
		STATE_CFG_CLR_DTR_REGISTERS,
		STATE_CFG_SCHEMA_DTR0,
		STATE_CFG_SCHEMA,
		STATE_CFG_FILTERS_DTR0,
		STATE_CFG_FILTERS,
		STATE_CFG_PRIO_DTR0,
		STATE_CFG_PRIO
	}State_t;

	void setState(State_t state) {
		this->_setState(state);
	}
private:

	CAppDaliInputControlDeviceInstances* m_pDataCfg;		// De config die moet doorgestuurd worden.
	int m_currentInstance;									// de huidige instantie.

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmHelperControlGearProperties_Base : public CAppDaliFsmHelper_Base {
public:
	CAppDaliFsmHelperControlGearProperties_Base(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlGearProperties_Base();

	// Nodig voor het updaten van de database wanneer het algoritme gedaan is.
	const CAppDaliDeviceProperties_ControlGear& getcfgData( ) const;
	CAppDaliDeviceProperties_ControlGear& getcfgData( );
	
protected:	
	void clearCfgData( );		
	void deallocateCfgData();
	void setCfgData(CAppDaliDeviceProperties_ControlGear* data);


private:
	CAppDaliDeviceProperties_ControlGear* m_pDataCfg;
	
	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Klasse voor het doorsturen van de control gear properties.
class CAppDaliFsmHelperControlGearDeviceCfg : public CAppDaliFsmHelperControlGearProperties_Base
{
public:
	CAppDaliFsmHelperControlGearDeviceCfg(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlGearDeviceCfg();

	// Oppassen:
	// de config data wordt doorgegeven via de clone().
	// deze klasse houdt een eigen instantie bij van de cfg data.
	bool start(int shortAddress, CAppDaliDeviceProperties_ControlGear* data);
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext, int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,	
		STATE_QUERY_DEVICE_STATUS,
		STATE_QUERY_DEVICE_VERSION,
		STATE_INITIALISE,
		STATE_ENTER_QUIESCENT_MODE,		
		STATE_FADETIME_DTR0,
		STATE_FADETIME,		
		STATE_FADETIME_EXT_DTR0,			// Momenteel niet gebruikt.
		STATE_FADETIME_EXT,					// Momenteel niet gebruikt.
		STATE_FADERATE_DTR0,
		STATE_FADERATE,		
		STATE_LEVEL_MIN_DTR0,
		STATE_LEVEL_MIN,		
		STATE_LEVEL_MAX_DTR0,
		STATE_LEVEL_MAX,		
		STATE_LEVEL_POWERON_DTR0,
		STATE_LEVEL_POWERON,		
		STATE_LEVEL_SYSTEM_FAILURE_DTR0,
		STATE_LEVEL_SYSTEM_FAILURE,							
		STATE_EXIT_QUIESCENT_MODE,
		STATE_TERMINATE			
	}State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	bool m_supportedExtFadeTime;						// Momenteel altijd op false.

	typedef CAppDaliFsmHelperControlGearProperties_Base Base;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Klasse voor het doorsturen van de control gear properties.
class CAppDaliFsmHelperControlGearActualValueRequest : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperControlGearActualValueRequest(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlGearActualValueRequest();

	bool start(int shortAddress);
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

	int getActualLevel() const {
		return this->m_actualLevel;
	}

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_QUERY_ACTUAL_LEVEL,
	} State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	int m_actualLevel;

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmHelperControlGearCfgScenes_Base : public CAppDaliFsmHelperControlGearProperties_Base
{
public:
	CAppDaliFsmHelperControlGearCfgScenes_Base(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlGearCfgScenes_Base();


protected:
	enum { DT8_DALI_MAX_PRIMARY_CHANNELS = 6 };

	typedef enum {
		SUB_STATE_DT8_NONE = -1,					
		SUB_STATE_DT8_SET_COLORVALUE_TEMP_TC,			// Moeten 16bit waarde versturen.
		SUB_STATE_DT8_SET_COLORVALUE_X,					// Moeten 16bit waarde versturen.
		SUB_STATE_DT8_SET_COLORVALUE_Y,					// Moeten 16bit waarde versturen.
		SUB_STATE_DT8_SET_COLORVALUE_RGB,				// Moeten 3 x 8 bit waarde versturen.
		SUB_STATE_DT8_SET_COLORVALUE_WAF,				// Moeten 3 x 8 bit waarde versturen.
		SUB_STATE_DT8_SET_COLORVALUE_CHANNEL,			// Voor ierdere channel een uint16_t sturen (DTR2=channelNR)
	} StateDT8ColorObject;

	// property: m_sceneNumber
	void setSceneNumber(int sceneNumber);
	int getSceneNumber() const;

	// property: m_channelNumber
	void setChannelNumber(int channelNumber);
	int getChannelNumber() const;

	// property: m_supportDT8ColorObject
	void setSupportDT8ColorObject(bool flag);
	bool getSupportDT8ColorObject() const;

	// property m_stateDT8ColorObject
	void setStateDT8ColorObject(StateDT8ColorObject state);
	StateDT8ColorObject getStateDT8ColorObject() const;

	// Utility functie.
	// Bepalen of een sfeer moet verwijderd worden.
	// Controleren hiervoor actual level en DT8 ColorData
	bool getRemoveScene(int sceneNumber);

protected:
	bool sendDT8ColorTemperatureValueLSB( );				// load DTR0 LSB
	bool sendDT8ColorTemperatureValueMSB( );				// load DTR1 MSB
	bool sendDT8ColorCoordinateXValueLSB( );				// load DTR0 LSB
	bool sendDT8ColorCoordinateXValueMSB( );				// load DTR1 MSB
	bool sendDT8ColorCoordinateYValueLSB( );				// load DTR0 LSB
	bool sendDT8ColorCoordinateYValueMSB( );				// load DTR1 MSB
	bool sendDT8ColorDimlevelRed( );						// DTR0
	bool sendDT8ColorDimlevelGreen( );						// DTR1
	bool sendDT8ColorDimlevelBlue( );						// DTR2
	bool sendDT8ColorDimlevelWhite( );						// DTR0
	bool sendDT8ColorDimlevelAmber( );						// DTR1
	bool sendDT8ColorDimlevelFreeColor( );					// DTR2
	bool sendDT8ColorChannelDimLevelLSB(int channel);		// load DTR0 LSB	
	bool sendDT8ColorChannelDimLevelMSB(int channel);		// load DTR1 MSB
	bool sendDT8ColorChannelNumber(int channel);			 // DTR2

private:
	int m_sceneNumber;										// De scenes 0 tem 15
	int m_channelNumber;									// Primary channels 0-5
	bool m_supportDT8ColorObject;							// Bepaalt of er DT8 color info moet doorgestuurd worden.
	StateDT8ColorObject m_stateDT8ColorObject;

	typedef CAppDaliFsmHelperControlGearProperties_Base Base;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Klasse voor het doorsturen van de control gear sferen.
class CAppDaliFsmHelperControlGearCfgScenes : public CAppDaliFsmHelperControlGearCfgScenes_Base
{
public:
	CAppDaliFsmHelperControlGearCfgScenes(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlGearCfgScenes();

	// Oppassen:
	// de config data wordt doorgegeven via de clone().
	// deze klasse houdt een eigen instantie bij van de cfg data.
	bool start(int shortAddress, CAppDaliDeviceProperties_ControlGear* data);
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

protected:
	bool sendSceneLevelValue(int value);
	bool sendStopQuiescentMode();

	// Helper utility functie.
	bool startUploadScene(int sceneNumber);

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,	
		STATE_QUERY_DEVICE_STATUS,
		STATE_QUERY_DEVICE_VERSION,
		STATE_INITIALISE,
		STATE_ENTER_QUIESCENT_MODE,		
		STATE_SCENE_REMOVE,									// Verwijderen een scene
		// Color data versturen.
		STATE_SCENE_COLORVALUE_LOAD_DTR0,					// *:	DATA TRANSFER REGISTER = MSB
		STATE_SCENE_COLORVALUE_LOAD_DTR1,					// *:	DATA TRANSFER REGISTER1 = LSB (of RGBWAF waarde)
		STATE_SCENE_COLORVALUE_LOAD_DTR2,					// *:	DATA TRANSFER REGISTER2 = (RGBWAF waarde)
		STATE_SCENE_ENABLE_DT8,								// *: 	ENABLE DEVICE TYPE 8 	
		STATE_SCENE_COLORVALUE_SET,							// A:	SET TEMPORARY COLOUR TEMPERATURE TC
			
		STATE_SCENE_LOAD_DTR,								// *: 	DATA TRANSFER REGISTER = level
		STATE_SCENE_LEVEL_STORE,							// A:	STORE THE DTR AS SCENEX
		STATE_EXIT_QUIESCENT_MODE,
		STATE_TERMINATE			
	}State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	typedef CAppDaliFsmHelperControlGearCfgScenes_Base Base;
};
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Principe doorsturen van een sfeer:
// - Remove scene:
//		- Wanneer er geen kleurdata aanwezig is (of masked)
// - Scene:
//		- Eerst alle color data.
//		- Actual level (ook 255).
class CAppDaliFsmHelperControlGearCfgSingleScene : public CAppDaliFsmHelperControlGearCfgScenes_Base
{
public:
	CAppDaliFsmHelperControlGearCfgSingleScene(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlGearCfgSingleScene();

	// Oppassen:
	// de config data wordt doorgegeven via de clone().
	// deze klasse houdt een eigen instantie bij van de cfg data.
	bool start(int shortAddress, int sceneNumber, CAppDaliDeviceProperties_ControlGear* data);
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);	
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

protected:
	bool sendSceneLevelValue(int value);					// load DTR0 value	
	bool sendTerminate();

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_INITIALISE,
		STATE_ENTER_QUIESCENT_MODE,

		// Indien de colorData masked is --> Remove from scene zorgt ervoor dat alle colorData gewist wordt.
		STATE_SCENE_REMOVE,									// A:	Versturen van een REMOVE_FROM_SCENEX (x=scene)	
		
		// Color data versturen.
		STATE_SCENE_COLORVALUE_LOAD_DTR0,					// *:	DATA TRANSFER REGISTER = MSB
		STATE_SCENE_COLORVALUE_LOAD_DTR1,					// *:	DATA TRANSFER REGISTER1 = LSB (of RGBWAF waarde)
		STATE_SCENE_COLORVALUE_LOAD_DTR2,					// *:	DATA TRANSFER REGISTER2 = (RGBWAF waarde)
		STATE_SCENE_ENABLE_DT8,								// *: 	ENABLE DEVICE TYPE 8 	
		STATE_SCENE_COLORVALUE_SET,							// A:	SET TEMPORARY COLOUR TEMPERATURE TC
					
		// Scene level versturen. Kan 0xFF zijn.
		STATE_SCENE_LOAD_DTR,								// *: 	DATA TRANSFER REGISTER = level
		STATE_SCENE_LEVEL_STORE,							// A:	STORE THE DTR AS SCENEX

		STATE_EXIT_QUIESCENT_MODE,						
		STATE_TERMINATE,		
	}State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	typedef CAppDaliFsmHelperControlGearCfgScenes_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Klasse voor het enablen/disablen van een control input device instance
class CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	virtual ~CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff();


	bool start(int shortAddress, int instanceNumber, bool stateOnOff);
	virtual bool cancel();

	// Opvragen van het resultaat 
	bool getInstanceStateOnOff( ) const {
		return this->m_resultVerifyStateOnOff;
	}

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,	
		STATE_INSTANCE_QUERY_TYPE,
		STATE_INITIALISE,
		STATE_ENTER_QUIESCENT_MODE,		
		STATE_INSTANCE_ENABLE,
		STATE_INSTANCE_DISABLE,
		STATE_INSTANCE_VERIFY,
		STATE_EXIT_QUIESCENT_MODE,
		STATE_TERMINATE			
	}State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	int m_instanceAddress;					// instance address 
	bool m_stateOnOff;						// gevraagde toestand on/off
	bool m_resultVerifyStateOnOff;			// Opgevraagde toestand na het instellen (=verify)

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
/*********************************************************************************** 
	Swappen van de addressen.

		A0,A1,AT 
			
		Initialise (all) (address=not used)
		Start quiescent mode (D=BROADCAST,Device)
		
			
		Check AT free
		Check A0 used 
		Check A1 used
			Broadcast		load DTR0					AT
			A0				set short address
		
		verify AT
		
			Broadcast		load DTR0 					A0 
			A1 				set short address 			
		
		
			Broadcast		load DTR0					A1 
			AT				set short address 
					
		Terminate (address=not used)
		
		verify A0
		verify A1
		
		Stop quiescent mode (D=BROADCAST,Device)	
*******************************************************************************************/			
class CAppDaliFsmHelperSwapShortAddresses : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperSwapShortAddresses(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);
	
	// Om een device te wissen: newShortAddress 0xff
	bool start(DaliDeviceType_t deviceTypes,int a0,int a1); // , int atemp);
	
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext,int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

	// Opvragen of het algoritme geslaagd is...
	bool success() {
		return this->m_stateSuccess;
	}

protected:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,		
		STATE_INITIALISE,
		STATE_ENTER_QUIESCENT_MODE,

		STATE_SEARCH_FREE_ADDRESS,					// Zoeken naar een tijdelijk vrij adres.

		STATE_VERIFY_SHORTADDRESS_ATEMP_FREE,		// extra Controle atemp vrij?		
		STATE_VERIFY_SHORTADDRESS_A0_USED,			// Controle a0 gebruikt
		STATE_VERIFY_SHORTADDRESS_A1_USED,			// Controle a1 gebruikt		

		// A0 wordt adres temp adress
		STATE_DTR0_LOAD_ADDRESS_ATEMP,				// dtr0 = address temp
		STATE_A0_SHORTADDRESS_DTR0,					// a0 toekennen temp adres		
		STATE_VERIFY_ADDRESS_ATEMP,					// controle atemp toegekend.

		// A1 wordt adres 0
		STATE_DTR0_LOAD_ADDRESS_A0,					// dtr0 = adress a0
		STATE_A1_SHORTADDRESS_DTR0,					// a1 toekennen address a0 

		// Address temp wordt A1
		STATE_DTR0_LOAD_ADDRESS_A1,					// dtr0 = adress a1 
		STATE_ATEMP_SHORTADDRESS_DTR0,				// atemp toekennen address a1


		STATE_TERMINATE,							// terminate.
		STATE_VERIFY_ADDRESS_A0,					// Controle a0 
		STATE_VERIFY_ADDRESS_A1,					// Controle a1
		STATE_EXIT_QUIESCENT_MODE,			
	}State_t;

	bool helper_sendQueryDeviceStatus(DaliDeviceType_t daliDeviceType, int address);
	bool helper_sendBroadcastDTR0ShortAddress(DaliDeviceType_t daliDeviceType, int address);
	bool helper_sendAssignShortAddressDTR0(DaliDeviceType_t daliDeviceType, int address);


private:
	enum { COUNTER_MAX_VERIFY = 5 };				// zie Probleem: NOTE_CONTROLGEAR_ADDRESSING_PROBLEM

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	int m_address_atemp;							// Tijdelijk adres 
	int m_address_a0;
	int m_address_a1;
	// int m_verifyCounter;
	bool m_verifyError;								// controle op adres in gebruik
	bool m_stateSuccess;							// Status van het algoritme (afh van de verify.)
	bool m_errorNoTempAddressFound;					// Tijdens het zoeken naar een vrij adres werden er geen gevonden.
	
	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// TODO ALGORITME_NAME_CONTROLGEAR_SELECT_DIMMINGCURVE implementeren in de main fsm.
class CAppDaliFsmHelperSelectDimmingCurve : public CAppDaliFsmHelper_Base
{
public:
	CAppDaliFsmHelperSelectDimmingCurve(IAppDaliFsm_HelperFsmEvents& refParent, CAppDaliTransmitter& refTransmitter);

	// Dimmer curve instellen voor een deviceType.
	// Oppassen: Waarde van de dimmer curve is afhankelijk van het device type.
	bool start(int shortAddress,  int deviceType, int dimmerCurve);
	virtual bool cancel();

	// acties.
	virtual bool next();

	// Events van de FSM...
	virtual bool onFsmEventResponse(CAppDaliFsmContext& refContext, int data);
	virtual bool onFsmEventNoResponse(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventTxDone(CAppDaliFsmContext& refContext);
	virtual bool onFsmEventResponseError(CAppDaliFsmContext& refContext);
	// virtual bool onFsmEventTimerExpiry(CAppDaliFsmContext& refContext) { return false; }	/* TODO */

	// Opvragen of het algoritme geslaagd is...
	bool success() {
		return this->m_stateSuccess;
	}
	int getDimmerCurve() const {
		return this->m_dimmerCurve;
	}
private:
	typedef enum {
		STATE_READY = FSM_BASE_STATE_READY,
		STATE_DONE = FSM_BASE_STATE_DONE,
		STATE_ERROR_FATAL = FSM_BASE_STATE_ERROR,
		STATE_CANCEL = FSM_BASE_STATE_CANCEL,
		STATE_INITIALISE,
		STATE_ENTER_QUIESCENT_MODE,
		STATE_DIMMING_CURVE_LOAD_DTR0,
		STATE_ENABLE_DEVICE_TYPE,
		STATE_DIMMING_CURVE_SELECT,
		STATE_TERMINATE,							// terminate.
		STATE_EXIT_QUIESCENT_MODE,
	}State_t;

	void setState(State_t state) {
		this->_setState(state);
	}

private:
	bool m_stateSuccess;							// toestand 
	int m_deviceType;
	int m_dimmerCurve;

	typedef CAppDaliFsmHelper_Base Base;
};
/***********************************************************************************/

/***********************************************************************************/
};
};
/***********************************************************************************/