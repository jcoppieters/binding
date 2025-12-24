#pragma once
/**************************************************************/
#include "appremotecfg/IAppRemoteCfgApi.h"				// Library.
/**************************************************************/
class AppUDPDevice;
/**************************************************************/
namespace DUOTECNO {
	class AppRemoteCfgNode;
	class AppRemoteCfgNodes;
	class AppRemoteCfgLoginInfo;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
struct AppRemoteCfgNodes_Util 
{	
public:
	// RemoteCfg node updaten met de gegevens van een AppUDPDevice.
	// 
	// Wanneer een node met hetzelde node address reeds aanwezig is in de lijst dan 
	// wordt deze upgedate anders wordt er een nieuwe aangemaakt.
	//
	// pRemoteCfgNodes: De Lijst met nodes die moet upgedate worden.
	// bNodeAddress: Het node address van de module.
	// refDevice: Device die gevonden werd in het LAN netwerk.
	//
	// return waarde: true: node upgedate/toegevoegd.
	static bool updateDeviceByLogicalAddress(DUOTECNO::AppRemoteCfgNodes* pRemoteCfgNodes, BYTE bNodeAddress, const AppUDPDevice& refDevice);

	// RemoteCfg node updaten met de gegevens van een AppUDPDevice.
	//
	// De node moet aanwezig zijn in de lijst van devices. Er worden geen nieuwe devices ingevoegd.
	// Deze functie kan gebruikt worden om de App configuratie homeserver/secured API en dynDns 
	// van het LAN-device te copieren naar een node in de device list.
	// 
	enum {
		MASK_CONFIG_HOMESERVER		= (1<<0),
		MASK_CONFIG_DYNDNS			= (1<<1)
	};
	static bool updateDeviceByMACAddress(DUOTECNO::AppRemoteCfgNodes* pRemoteCfgNodes, AppUDPDevice& refDevice, int mask);
	
	// Opvragen van het password om te connecteren via secured API of TCP/IP homeserver.
	// Wordt gebruikt in de dialoog van de master Tasks.
	static bool getNodePassword(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress,CString* pwd);


	// Opvragen van de host name om een locale verbinding te maken.
	static bool getNodeLocalHostName(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress, CString* hostAddress);	

	// Zoeken naar de remote Dns.
	// Eerst zoeken of er voor dit node address een remote DNS ingesteld / gevonden werd.
	// Anders alle nodes overlopen en zoeken naar een DynDns hostName.
	static bool searchNodesForRemoteDns(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress,CString* pStrDNS);	
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
struct AppRemoteCfgNodesTCPIPService 
{
	// Wordt aangeroepen wanneer er een status tcp/ip connect ontvangen wordt.
	// Afh van de connectie lokaal / remote worden de gegevens upgedate.
	static bool updateDevicesAfterLogin(DUOTECNO::AppRemoteCfgNodes* dest,const DUOTECNO::AppRemoteCfgLoginInfo& src);

	// Updaten van de service.
	// Deze wordt aangeroepen door de Dlg voor de instellingen van de master taken.
	// Instellen van de lokale / remote parameters voor de TCP/IP master tasks.
	static bool updateNodeService(DUOTECNO::AppRemoteCfgNodes* dest, int nodeAddress, 
								  const CString& password,  const CString& localIpAddress, int localPort, 
								  const CString& dns, int remotePort);

	// Updaten van de instellingen
	static bool updateSettingsLocal(DUOTECNO::AppRemoteCfgNodes* dest,int nodeAddress,const CString& password, const CString& hostAddress,int port);
	static bool updateSettingsRemote(DUOTECNO::AppRemoteCfgNodes* dest,int nodeAddress,const CString& password, const CString& hostAddress,int port);

	// Opvragen van de instellingen.
	static bool getSettingsLocal(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress,CString* hostAddress,int* port);
	static bool getSettingsRemote(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress,CString* hostAddress,int* port);

	// Opvragen of de instellingen geldig zijn.
	static bool getSettingsLocalValid(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress);
	static bool getSettingsRemoteValid(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress);
};
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
struct AppRemoteCfgLoginInfo_Util 
{
	// updaten van de login gegevens wanneer er een UDP decvice geconfigureerd werd.
	// Nodig updaten bij verandering van:
	// - De netwerk config.
	// - homeserver passwoord en poort.
	// - dyndns login gegevens.

	enum {
		MASK_CONFIG_NWK				= (1<<0),
		MASK_CONFIG_HOMESERVER		= (1<<1),
		MASK_CONFIG_DYNDNS			= (1<<2)
	};
	static bool updateLoginAfterUdpDeviceCfg(DUOTECNO::AppRemoteCfgLoginInfo*dest,AppUDPDevice& r,int mask);
};