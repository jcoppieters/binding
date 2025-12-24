/************************************************************************
 * dtudpconfigLib.
 *
 * @brief		UDP configuration of Duotecno devices.
 * @date		20131002
 * @author		Mitchell Tom - Duotecno
 * @version		v1.1 - protocol version v1.0
 *				Initial version.
 * @version		v1.2 - protocol version v1.0
 * @version		v1.3 - protocol version v1.0
 *				Interface adapter selecteren.
 * @version		v2.0 - TCP/IP Client app cfg.
 *				v2.1 - DNS provider app cfg
 *				v2.2 - BUG TerminateThread( ) mag niet aangeroepen worden.
 * @version		v3.0 - SNTP & Time configuratie.
 * @version		v3.1 - DLL compilatie.
 *				v3.1 - Oplossing name mangling probleem: extern "C"
 * @version		v4.0 - hercompileren om compatible te zijn met de binding software.
 *				v4.1 - Extended bericht voor ATTR_FIND_IPV4 - Version nummer.
 *				v4.1 - IDTDevice & CDTDevice uitgebreid met extra members
 * @version		v4.2 - Mogelijkheid instellen van het paswoord voor de beveiligde Cfg API.
 * @version		v4.3 - Advanced installer gebruiken voor setup programma. 
 * @version		v4.4 - Controle of het packet kan verstuurd worden.
 *				       Oplossing probleem DynDns Configuratie.
 *					   Data payload soms te lang waardoor het frame niet verstuurd wordt.
 *					 - Oplossing BUG inc_WritePtr( ).
 *					   index buiten het array.
 *					 - Nieuwe functie ipappframeformatter_peekLength( ).
 *				       Wordt nog niet gebruikt !
 ************************************************************************/
#pragma once

#include "dtdevicelist.h"
#include "dttypes.h"

#ifdef DTUDPCONFIGSTATIC_LINK	
	#define DTAPI extern "C"					// Absorbe static linkage/
#else
	#ifdef DTUDPCONFIGDLL_EXPORTS
		#define DTAPI extern "C" __declspec(dllexport)
	#else
		#define DTAPI extern "C" __declspec(dllimport)
	#endif
#endif

#ifdef __cplusplus
#   define EXTERN_C     extern "C"
#else
#   define EXTERN_C
#endif // __cplusplus

#define USERINT_FUNC __cdecl 

/************************************************************************/
#ifdef __cplusplus
namespace DUOTECNO {
#endif
/************************************************************************/

/**
 * Result Code
 */
typedef enum
{
	/* Success */
	RESULTCODE_SUCCESS		= 0,

	/* Failed, mogelijk remote antwoord van het device */
	RESULTCODE_FAILED		= -1,

	/* Timeout, Geen antwoord van remote device */
	RESULTCODE_TIMEOUT		= -2,

	/* Error, Formatter - Data kan niet verstuurd worden - Frame te groot */
	RESULTCODE_EC_FORMATTER = -3		// Sedert V4.4

} ResultCode;


/**
 * @brief	Aanroepen bij start applicatie.
 */
DTAPI int dtudpcfg_initApplication(void);

/**
 * @brief	Aanroepen bij afsluiten applicatie.
 */
DTAPI int dtudpcfg_shutdownApplication(void);

/**
 * @brief	Een andere interface adapter seleceteren.
 * @param	szIPAddress [IN] 0 = Don't care anders een IP-address
 */
DTAPI int dtudpcfg_selectInterfaceAdapter(
	const char* szIPAddress);

/**
 * @brief	Scannen van het netwerk opzoek naar DT devices.
 *			De device list wordt opgebouwd ifv het MAC-address.
 */
DTAPI int dtudpcfg_scanNetwork(void);

/**
 * @brief	DeviceList wissen
 * @result	0 = Success; andere is een error
 */
DTAPI int dtudpcfg_eraseDeviceList(void);

/**
 * @brief	Controle of de devicelist aangepast werd.
 * @result	TRUE = changes, FALSE = No changes to device list.
 */
DTAPI BOOL dtupcfg_checkDeviceListModified(void);

/**
 * @brief	Kan gebruikt worden om te controleren of de workerthread
 *			nog draait.
 * @result	0 = Success; andere is een error
 */
DTAPI int dtudpcfg_running(void);

/**
 * @brief	De Devicelist van de DT devices opvragen.
 * @param	ppDeviceList [OUT] Device list
 * @result	0 = Success; andere is een error
 */
DTAPI int dtudpcfg_getDeviceList(
	IDTDeviceList** ppDeviceList);

/**
 * @brief	IP configuratie van een device.
 * @param	pbMACAddress [IN] array van 6 bytes - MAC address
 * @param	dhcpOnOff [IN] 0=dhcp off, 1=dhcp on
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx"
 * @param	szNetmask [IN] string "xxx.xxx.xxx.xxx"
 * @param	szGateway [IN] string "xxx.xxx.xxx.xxx"
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_configureDevice(
	const BYTE* pbMACAddress,
	int dhcpOnOff,
	const char* szIPAddress,
	const char* szNetmask,
	const char* szGateway,
	int milliseconds);

/**
 * @brief	Opvragen welke configuratie er mogelijk is.
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	supportedFlags [OUT] Vlaggen enum AppCfgSupportedFlags
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_getCfgAppSupported(
	const char* szIPAddress,
	unsigned long* supportedFlags,	
	int milliseconds);

/**
 * @brief	App configuratie van TCP/IP homeserver
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	szPassword [IN] paswoord om aan te loggen. MAX 16 char.
 * @param	IPPort [IN] Remote IP-poort
 * @param	protocolFlags [IN] Applicatie specifieke vlaggen.
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_setCfgAppHomeSrv(
   const char* szIPAddress,
   const char* szPassword,
   int IPPort,
   int protocolFlags,
   int milliseconds);

/**
 * @brief	Opvragen van de App configuratie van TCP/IP homeserver
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	szPassword [OUT] paswoord om aan te loggen. MAX 16 char.
 * @param	IPPort [OUT] Remote IP-poort
 * @param	protocolFlags [OUT] Applicatie specifieke vlaggen.
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_getCfgAppHomeSrv(
	const char* szIPAddress,
	char* szPassword,
	int* IPPort,
	int* protocolFlags,
	int milliseconds);


/**
 * @brief	DDNS client disablen.
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_setCfgAppDDNSDisable(
	const char* szIPAddress,
	int milliseconds);

/**
 * @brief	DDNS client enablen.
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	ProviderID [IN] Zie enum DDNSProvider
 * @param	szHostname [IN] Host naam vb: "duotecno.no-ip.com"
 * @param	szUsername [IN] Gebruikersnaam vb: "tom@duotecno.be"
 * @param	szPassword [IN] paswoord vb: "abc112233"
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_setCfgAppDDNSEnable(
	const char* szIPAddress,
	int ProviderID,				
	const char* szHostname,
	const char* szUsername,
	const char* szPassword,
	int milliseconds);

/**
 * @brief	Opvragen van DDNS client gegevens.
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	ProviderID [OUT] Zie enum DDNSProvider
 * @param	Status [OUT] Zie enum DDNSStatus
 * @param	szHostname [OUT] Host naam vb: "duotecno.no-ip.com"
 * @param	szUsername [OUT] Gebruikersnaam vb: "tom@duotecno.be"
 * @param	szPassword [OUT] paswoord vb: "abc112233"
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_getCfgAppDDNS(
	const char* szIPAddress,
	int* ProviderID,			
	int* Status,				
	char* szHostname,
	char* szUsername,
	char* szPassword,
	int milliseconds);

/**
 * @brief	Instellen van de systeem mode.
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	systemMode [IN] Zie enum SystemMode
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_setCfgAppSystemMode(
	const char* szIPAddress,
	int systemMode,				
	int milliseconds);

/**
 * @brief	Opvragen van de systeem mode.
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	systemMode [OUT] Zie enum SystemMode
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_getCfgAppSystemMode(
	const char* szIPAddress,
	int* systemMode,			// Zie enum SystemMode
	int milliseconds);

/**
 * @brief	Instellen van de TCP/IP Client
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	szServerIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-Address van de server
 * @param	serverIPPort [IN] IP port van de server
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_setCfgAppTCPIPClient(
	const char* szIPAddress,
	const char* szServerIPAddress,
	int serverIPPort,
	int milliseconds);

/**
 * @brief	Opvragen van de TCP/IP Client
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	szServerIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-Address van de server
 * @param	serverIPPort [OUT] IP port van de server
 * @param	status [OUT] Connection status 0 of 1
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_getCfgAppTCPIPClient(
	const char* szIPAddress,
	char* szServerIPAddress,
	int* serverIPPort,
	int* status,
	int milliseconds);

/**
 * @brief	Instellen van het ip-address van de DNS provider
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	szIPAddressDNSProvider [IN] string "xxx.xxx.xxx.xxx" - DNS Provider IP-address
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_setCfgAppDNSProvider(
	const char* szIPAddress,
	const char* szIPAddressDNSProvider,
	int milliseconds);

/**
 * @brief	Opvragen van het ip-address van de DNS provider
 * @param	szIPAddress [IN] string "xxx.xxx.xxx.xxx" - Remote IP-address Address
 * @param	szIPAddressDNSProvider [OUT] string "xxx.xxx.xxx.xxx" - DNS Provider IP-address
 * @param	milliseconds [IN] aantal ms die er gewacht wordt op antwoord van device.
 * @return	zie enum ResultCode.
 */
DTAPI ResultCode dtudpcfg_getCfgAppDNSProvider(
	const char* szIPAddress,
	char* szIPAddressDNSProvider,
	int milliseconds);


/********************************** SNTP ********************************/

// Sedert v3.0
DTAPI ResultCode dtudpcfg_setCfgAppSNTP(
	const char* szIPAddress,
	int poolID,
	int milliseconds);

// Sedert v3.0
DTAPI ResultCode dtudpcfg_getCfgAppSNTP(
	const char* szIPAddress,
	int* poolID,
	int milliseconds);


/********************************** TIME ********************************/

// Sedert v3.0
DTAPI ResultCode dtudpcfg_setCfgAppTime(
	const char* szIPAddress,
	int area,
	int utcZone,
	int dstEnabled,
	int milliseconds);

// Sedert v3.0
DTAPI ResultCode dtudpcfg_getCfgAppTime(
	const char* szIPAddress,
	int* area,
	int* utcZone,
	int* dstEnabled,
	int milliseconds);

/************************************************************************/
#ifdef __cplusplus
};	// end namespace DUOTECNO
#endif
/************************************************************************/


/**********************************EOF***********************************/