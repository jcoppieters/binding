#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppRemoteCfgNodes.h"
#include "appremotecfg/AppRemoteCfgNodeService.h"				// Library.
#include "appremotecfg/AppRemoteCfgLoginInfo.h"					// Library.
#include "app/appudpcfgserver/appudpservercfg.h"				// UDP Config.
#include "app/appcfg/appcfginfo/cfginfoproductid_def.h"			// productcodes.
/**************************************************************/
using namespace DUOTECNO;
/**************************************************************/

/************************************************************************************************************************
 * Klasse:					AppRemoteCfgNodes
 * json dat bestand:		/CONFIG/projectnodeservices.adt
 * json export bestand:		/CONFIG/projectnodeservices.json
 *
 ************************************************************************************************************************
 * Principe:
 *	   Lijst van alle gevonden devices in het netwerk die aanwezig zijn in het projectbestand.
 *	   De lijst wordt upgedate met de gegevens die beschikbaar zijn in het LAN-netwerk + de gegevens wanneer er een 
 *	   connectie gemaakt wordt met de DT18-PRO of Master tasks.
 *	   Bruikbare gegevens zijn lokale ip-adressen / remote DNS namen / paswoorden.
 *	   
 *	   De gegevens worden in json / dat bestand opgeslagen onder de project directory.
 *	   
 *	   Wanneer de configurator via de binding software geopend wordt dan wordt het json info bestand opgeslagen en kan het 
 *	   gebruikt worden door de Configurator.
 *	   
 *	   Wanneer de UDP cfg gebeurd is door de aparte UDP Configurator tool dan eens alle dialogen van de Binding software UDP config openen.
 *	   Op deze manier wordt alle data gesynchroniseerd.
 *	   
 *	
 *  Binding software:
 *		Update van de gegevens:
 *			- Volledige update van de lijst na een browse/pnp: Koppeling nodeAddress / physical address / MAC-Address / IP-adres. 
 *			  Dit is nodig wanneer er een module vervangen wordt.	   
 *			- Update van een device wanneer deze veranderd van lokale netwerk instellingen.
 *			  bvb: bij verandering van ip-adres / netmask / gateway / dhcp.	   
 *			- update na een connectie met de DT18-PRO.
 *			  De gegevens waarmee geconnecteerd werden worden upgedate voor deze node. 
 *				- Bij een lokale connectie: Lokaal IP-adres / poort / password.
 *				- Bij een remote connectie: remote dns name / poort / password.	   
 *			- update na een connectie met de master (master tasks).
 *			  De gegevens die ingevuld werden. (er wordt niet gecontroleerd of de connectie lukt?)
 *			  Lokaal IP-adres / lokale IP-poort / remote dns name / remote ip port / password.
 *			- update na hetinstellen of opvragen van de eigenschappen via UDP configuratie.
 *				- Wanneer deze geconfigureerd worden via de binding software dan worden deze automatisch bewaard.
 *				- Wanneer deze vroeger geconfigureerd werden via de UDP LAN configurator dan eens de UDP config van de node 
 *				  ophalen door de dialoog te openen.
 *		
 *		Opslaan van de gegevens:
 *			- Bij het opslaan van het project. (Save / Save as).
 *			- Bij het openen van de configurator via de binding software wordt het Json bestand opgeslagen onder de 
 *			  project dir.
 *
 *	    Gebruik van de gegevens:
 *			- Remote DNS naam.
 *			  Wanneer er geen remote DNS naam aanwezig is voor deze node (= geen dyndns geconfigureerd voor deze node) 
 *			  dan wordt in de lijst gezocht of er ergens bij een andere Duotecno node een DynDns naam gevonden wordt. 
 *			  Als er ergens anders ene gevonden wordt dan wordt deze standaard in de dialogen ingevuld.	   
 *			  Toegepast bij:	
 *				- Bij de master tasks: In de settings.
 * 			    - Configuratie van de DT18-PRO
 *		    - Connect (start dialoog) Configuratie via de DT18-PRO:
 *				- Via de "search in LAN network" worden de gegevens in het lokaal netwerk automatisch opgehaald.
 *				- Wanneer er configuratie gewijzigd/opgevraagd wordt via het UDP protocol worden de login gegevens van de DT18-PRO upgedate.
 *					- Bij verandering IP-adres wordt deze upgedate.
 *					- Bij verandering Homeserver poort / password wordt deze upgedate.
 *					- Als de DNS van de DT18-PRO veranderd wordt deze upgedate. (Niet wanneer deze ingesteld werd in de 
 *					  router / andere Duotecno module.)
 *			- Dialoog: Uitvoeren van de master task.
 *				- Lokale instellingen: Werden automatisch ingevuld wanneer ingesteld/opgevraagd via de UDP tool.
 * 				- Remote instellingen:
 *					- Remote DNS naam (zie hierboven).
 *					- IP-poort deze moet manueel 1x ingeven worden.
 *			- Dialoog Firmware update:
 *				Het IP-adres wordt automatisch ingevuld zoals deze in het LAN-netwerk gevonden werd.
 *				Indien deze ontbreekt wordt gezocht in de lijst. Bij het ontbreken wordt gezocht in de CONFIG map (=waar de 
 *				configurator het IP-adres bewaard.)
 *	   		
 * De configurator kan de gegevens op dezelfde manier gebruiken.
 *		- Bij een verbinding in een LAN netwerk bevat het bestand altijd een juiste koppeling van het node address met een lokaal ip-adres.
 *		- secure API Paswoord wordt upgedate wanneer deze ingesteld / opgevraagd wordt via de binding software UDP configuratie.
 *		- Remote DNS naam kan gezocht worden in de device lijst (op dezelfde manier als de binding software?)
 *
 *******************************************************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
static bool isValidIpAddress(const CString& str) {
	if ((0 == str.Compare("0.0.0.0")) || (0 == str.Compare("255.255.255.255"))) {
		return false;
	}
	int v[4] = {0,0,0,0};
	if (4 != sscanf((LPCSTR) str,"%d.%d.%d.%d",&v[0],&v[1],&v[2],&v[3])) {
		return false;
	}
	return ((v[0]>=0 && v[0]<=255) && (v[1]>=0 && v[1]<=255) && (v[2]>=0 && v[2]<=255) && (v[3]>=0 && v[3]<=255));
}

static bool helper_findNode(AppRemoteCfgNodes* pCfg,int nodeAddress,AppRemoteCfgNode** ppNode) {
	if (0 == pCfg) {
		return false;
	}
	AppRemoteCfgNode* pCfgNode = 0;
	if (!pCfg->findNode(nodeAddress,&pCfgNode)) {
		// Node niet aanwezig - toevoegen aan de node services...
		pCfgNode = new AppRemoteCfgNode(nodeAddress);
		ASSERT(0 != pCfgNode);
		if (!pCfg->addNode(pCfgNode)) {
			delete pCfgNode;
			return false;
		}
	}	
	ASSERT(0 != pCfgNode);
	*ppNode = pCfgNode;
	return true;
}

static bool helper_findService(AppRemoteCfgNode* pNode,AppRemoteCfgService_t type,AppRemoteCfgNodeService** ppDest) {
	AppRemoteCfgNodeService* pService = 0;
	if (!pNode->findService(type,&pService)) {		
		pService = new AppRemoteCfgNodeService(type);
		ASSERT(0 != pService);
		if (!pNode->addService(pService)) {
			delete pService;
			return false;
		}
	}		
	ASSERT(0 != pService);
	*ppDest = pService;
	return true;
}

static bool helper_getSettings(AppRemoteCfgNodes& src,int nodeAddress,AppRemoteCfgService_t service,
							   AppRemoteCfgHostAddress_t addressType, CString* hostAddress, int* port) 
{
	if (0 == hostAddress || 0 == port) {
		ASSERT(0 != hostAddress);
		ASSERT(0 != port);
		return false;
	}

	AppRemoteCfgNode* pCfgNode = 0;
	if (!src.findNode(nodeAddress,&pCfgNode)) {
		return false;
	}
	ASSERT(0 != pCfgNode);

	AppRemoteCfgNodeService* pCfgService = 0;
	if (!pCfgNode->findService(service,&pCfgService)) {
		return false;	
	}
	
	if (HOSTADDRESS_TYPE_LOCAL == addressType) {
		*hostAddress = pCfgNode->getHostnameLocal();
		*port = pCfgService->getPortLocal();
	}
	else {
		*hostAddress = pCfgNode->getHostnameRemote();
		*port = pCfgService->getPortRemote();
	}
	return true;
}	

static bool helper_getSettingsValid(AppRemoteCfgNodes& src,int nodeAddress,AppRemoteCfgService_t service,
									AppRemoteCfgHostAddress_t addressType) {

	CString hostAddress;
	int port = -1;

	if (!helper_getSettings(src,nodeAddress,service,addressType,&hostAddress,&port)) {
		return false;
	}
	if (hostAddress.IsEmpty()) {
		return false;
	}
	return (port>=0 && port<=0xffff ? true : false);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
static bool addNodeService(AppRemoteCfgNode* pRemoteCfgNode,AppRemoteCfgService_t service, int port = -1) {
	bool addNewService = false;
	AppRemoteCfgNodeService* pService = NULL;	
	if (!pRemoteCfgNode->findService(service, &pService)) {
		pService = new AppRemoteCfgNodeService();
		addNewService = true;
	}
	if (NULL == pService) {
		return false;
	}
	
	// Sedert V16.46: Voor de TCP/IP poort de gegevens overnemen van de UDP configuratie.
	if ((port >=0 ) && ((-1 == pService->getPortLocal()) || (SERVICE_TCPIP == service))) {
		pService->setPortLocal(port);
	}
	pService->setType(service);

	if (addNewService) {
		if (!pRemoteCfgNode->addService(pService)) {
			delete pService;
			return false;
		}	
	}		
	return true;
}



static bool updateNodeDevice(AppRemoteCfgNode* pRemoteCfgNode, const AppUDPDevice& refDevice) {

	const CString& strIpAddress = refDevice.getIpAddress();
	if (!isValidIpAddress(strIpAddress)) {
		return false;
	}

	pRemoteCfgNode->setHostnameLocal(strIpAddress);
	pRemoteCfgNode->setMacAddress(refDevice.getMACAddress());
	pRemoteCfgNode->setPhysicalAddress(refDevice.getPhysicalAddress());
				 
	// De beschikbare services ifv het product type.				
	// Sedert V16.32: Sbox ultra toegevoegd.
	const int productCode = refDevice.getProductCode();
	if (productCode == PRODUCT_ID_GATEWAY_HS ||
		productCode == PRODUCT_ID_GATEWAY_HS_32VIRT ||
		productCode == PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT ||
		productCode == PRODUCT_ID_HC_BOX_ULTRA)
	{
		addNodeService(pRemoteCfgNode, SERVICE_TCPIP);
		addNodeService(pRemoteCfgNode, SERVICE_HTTP, 8080);
		addNodeService(pRemoteCfgNode, SERVICE_HTTP_S, 8081);
	}
	else if (productCode == PRODUCT_ID_TFT_32VIRT) {
		addNodeService(pRemoteCfgNode, SERVICE_TCPIP);
		addNodeService(pRemoteCfgNode, SERVICE_HTTP, 8080);
		addNodeService(pRemoteCfgNode, SERVICE_HTTP_S, 8081);
	}
	else if (productCode == PRODUCT_ID_HC_BOX) {
		// OPPASSEN:
		// Deze heeft geen invloed - Het UDP protocol bevat geen product type.
		// Daarom zal bij een Sbox de services altijd leeg zijn...
		addNodeService(pRemoteCfgNode, SERVICE_TCPIP);
	}
	else if (productCode == PRODUCT_ID_8CHIRTX_AVIP) {
		addNodeService(pRemoteCfgNode, SERVICE_HTTP, 8080);
		addNodeService(pRemoteCfgNode, SERVICE_HTTP_S, 8081);
	}	
	return true;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
static bool helper_updateNodeService(AppRemoteCfgNodes* dest,int nodeAddress,AppRemoteCfgService_t service,
									AppRemoteCfgHostAddress_t addressType,const CString& password, 
									const CString& hostName,int port) 
{
	AppRemoteCfgNode* pCfgNode = 0;
	if (!helper_findNode(dest,nodeAddress,&pCfgNode)) {
		return false;
	}
	AppRemoteCfgNodeService* pService = 0;
	if (!helper_findService(pCfgNode,service,&pService)) {
		return false;
	}
	ASSERT(0 != pCfgNode);
	ASSERT(0 != pService);

	pCfgNode->setPassword(password);	

	if (HOSTADDRESS_TYPE_LOCAL == addressType) {
		pCfgNode->setHostnameLocal(hostName);	
		pService->setPortLocal(port);
	}
	else {
		pCfgNode->setHostnameRemote(hostName);	
		pService->setPortRemote(port);
	}
	return true;
}

static bool addDevice(DUOTECNO::AppRemoteCfgNodes* pRemoteCfgNodes, BYTE bNodeAddress,const AppUDPDevice& refDevice) {
	if (NULL == pRemoteCfgNodes) {
		ASSERT(NULL != pRemoteCfgNodes);
		return false;
	}

	AppRemoteCfgNode* pRemoteCfgNode = new AppRemoteCfgNode(bNodeAddress);
	if (NULL == pRemoteCfgNode) {
		ASSERT(NULL != pRemoteCfgNode);
		return false;
	}
	
	bool r = updateNodeDevice(pRemoteCfgNode, refDevice);
	if (!r) {
		delete pRemoteCfgNode;
		pRemoteCfgNode = NULL;
		return false;
	}
		
	r = pRemoteCfgNodes->addNode(pRemoteCfgNode); 
	if (!r) {
		delete pRemoteCfgNode;
		pRemoteCfgNode = NULL;
		return false;
	}										
	return true;
}

static bool updateNodeService(AppRemoteCfgNodes* dest,int nodeAddress,AppRemoteCfgService_t service, 
											       const CString& password, const CString& localIpAddress,int localPort, const CString& dns, int remotePort) 
{
	AppRemoteCfgNode* pCfgNode = 0;
	if (!helper_findNode(dest,nodeAddress,&pCfgNode)) {
		return false;
	}
	ASSERT(0 != pCfgNode);
	pCfgNode->setPassword(password);		
	pCfgNode->setHostnameLocal(localIpAddress);
	pCfgNode->setHostnameRemote(dns);

	if ((localPort >=0) || (remotePort>=0)) {	
		AppRemoteCfgNodeService* pService = 0;
		if (!helper_findService(pCfgNode,service,&pService)) {
			return false;
		}

		ASSERT(0 != pService);
		pService->setPortLocal(localPort);
		pService->setPortRemote(remotePort);
	}
	else {
		pCfgNode->deleteService(service);
	}
	return true;
}

static bool getNodeServiceSettingsValid(AppRemoteCfgNodes& src,int nodeAddress,AppRemoteCfgService_t service,
											  AppRemoteCfgHostAddress_t addressType) {
	return helper_getSettingsValid(src,nodeAddress,service,addressType);	
}

static bool getNodeServiceSettings(AppRemoteCfgNodes& src,int nodeAddress,AppRemoteCfgService_t service,
												AppRemoteCfgHostAddress_t type,CString* hostAddress,int* port) {
	return helper_getSettings(src,nodeAddress,service,type,hostAddress,port);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
bool AppRemoteCfgNodes_Util::updateDeviceByLogicalAddress(DUOTECNO::AppRemoteCfgNodes* pRemoteCfgNodes, BYTE bNodeAddress, const AppUDPDevice& refDevice) {
	if (NULL == pRemoteCfgNodes) {
		ASSERT(NULL != pRemoteCfgNodes);
		return false;
	}	
	AppRemoteCfgNode* pRemoteCfgNode = NULL;				
	if (pRemoteCfgNodes->findNode(bNodeAddress, &pRemoteCfgNode)) {
		return updateNodeDevice(pRemoteCfgNode,refDevice);
	}	
	return addDevice(pRemoteCfgNodes, bNodeAddress, refDevice);
}

bool AppRemoteCfgNodes_Util::updateDeviceByMACAddress(DUOTECNO::AppRemoteCfgNodes* pRemoteCfgNodes, AppUDPDevice& refDevice, int mask) {

	AppRemoteCfgNode* pRemoteCfgNode = NULL;	
	if (!pRemoteCfgNodes->findNode(refDevice.getMACAddress(),&pRemoteCfgNode)) {
		return false;
	}
	ASSERT(NULL != pRemoteCfgNode);

	bool result = false;
	if (MASK_CONFIG_HOMESERVER == (mask & MASK_CONFIG_HOMESERVER)) {
		AppUDPDeviceHomeServerCfg& refCfg = refDevice.getAppHomeServerCfg();
		pRemoteCfgNode->setPassword(refCfg.getPassword( ));
		::addNodeService(pRemoteCfgNode, SERVICE_TCPIP, refCfg.getIPPort());		
		result = true;
	}
	if (MASK_CONFIG_DYNDNS == (mask & MASK_CONFIG_DYNDNS)) {	
		AppUDPDeviceDDNSCfg& refCfg = refDevice.getAppDDNSCfg();
		if (refCfg.getStateOnOff()) { 
			const CString& strDns = refCfg.getHostname();
			if (!strDns.IsEmpty()) {
				pRemoteCfgNode->setHostnameRemote(strDns);
			}				
		}
	}
	return result;
}

bool AppRemoteCfgNodes_Util::getNodePassword(AppRemoteCfgNodes& src,int nodeAddress,CString* pwd) {
	AppRemoteCfgNode* pCfgNode = 0;
	if (!src.findNode(nodeAddress,&pCfgNode)) {
		return false;
	}
	ASSERT(0 != pCfgNode);
	*pwd = pCfgNode->getPassword();
	return true;
}

bool AppRemoteCfgNodes_Util::getNodeLocalHostName(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress, CString* hostAddress) {
	AppRemoteCfgNode* pCfgNode = NULL;
	if (src.findNode(nodeAddress,&pCfgNode)) {
		const CString& localHostName = pCfgNode->getHostnameLocal();
		if (!localHostName.IsEmpty()) {
			*hostAddress = localHostName;
			return true;
		}
	}
	return false;
}

bool AppRemoteCfgNodes_Util::searchNodesForRemoteDns(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress,CString* pStrDNS) {
	if(NULL == pStrDNS) {
		ASSERT(NULL != pStrDNS);
		return false;
	}

	CString dnsRemote;
	AppRemoteCfgNode* pCfgNode = NULL;

	if (nodeAddress >= 0x03 && nodeAddress <= 0xFC) {
		if (src.findNode(nodeAddress,&pCfgNode)) {	
			dnsRemote = pCfgNode->getHostnameRemote();
			if (!dnsRemote.IsEmpty()) {
				*pStrDNS = dnsRemote;
				return true;
			}
		}
	}

	const int count = src.getCount();
	for (int i=0;i<count;++i) {
		if (src.getNode(i,&pCfgNode)) {
			dnsRemote = pCfgNode->getHostnameRemote();
			if (!dnsRemote.IsEmpty()) {
				*pStrDNS = dnsRemote;
				return true;
			}
		}	
	}
	return false;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
bool AppRemoteCfgNodesTCPIPService::updateDevicesAfterLogin(AppRemoteCfgNodes* dest,const AppRemoteCfgLoginInfo& src) {
	if (0 == dest) {
		ASSERT(0 != dest);
		return false;
	}

	const CString& strPassword = src.getPassword();
	const int nodeAddress = src.getNodeAddress();	
	const AppRemoteCfgHostAddress_t addressType = src.getPreferedLogin();
	bool result = false;
	if (HOSTADDRESS_TYPE_LOCAL == addressType) {
		const CString& hostName = src.getLocalHostAddress();
		const int port = src.getLocalPort();
		result = AppRemoteCfgNodesTCPIPService::updateSettingsLocal(dest,nodeAddress,strPassword,hostName,port);
	}
	else{
		const CString& hostName = src.getRemoteHostAddress();
		const int port = src.getRemotePort();
		result = AppRemoteCfgNodesTCPIPService::updateSettingsRemote(dest,nodeAddress,strPassword,hostName,port);
	}		
	return result;
}

bool AppRemoteCfgNodesTCPIPService::updateSettingsLocal(DUOTECNO::AppRemoteCfgNodes* dest,int nodeAddress,const CString& password,const CString& hostAddress,int port) {
	return ::helper_updateNodeService(dest,nodeAddress,SERVICE_TCPIP,HOSTADDRESS_TYPE_LOCAL,password,hostAddress,port);
}

bool AppRemoteCfgNodesTCPIPService::updateSettingsRemote(DUOTECNO::AppRemoteCfgNodes* dest,int nodeAddress,const CString& password,const CString& hostAddress,int port) {
	return ::helper_updateNodeService(dest,nodeAddress,SERVICE_TCPIP,HOSTADDRESS_TYPE_REMOTE,password,hostAddress,port);
}

bool AppRemoteCfgNodesTCPIPService::updateNodeService(DUOTECNO::AppRemoteCfgNodes* dest, int nodeAddress, 
													  const CString& password, const CString& localIpAddress, int localPort, 
													  const CString& dns, int remotePort)  {
	return ::updateNodeService(dest,nodeAddress,SERVICE_TCPIP,password,localIpAddress,localPort,dns,remotePort);
}

bool AppRemoteCfgNodesTCPIPService::getSettingsLocal(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress,CString* hostAddress,int* port) {
	return ::getNodeServiceSettings(src,nodeAddress,SERVICE_TCPIP,HOSTADDRESS_TYPE_LOCAL,hostAddress,port);
}

bool AppRemoteCfgNodesTCPIPService::getSettingsRemote(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress,CString* hostAddress,int* port) {
	return ::getNodeServiceSettings(src,nodeAddress,SERVICE_TCPIP,HOSTADDRESS_TYPE_REMOTE,hostAddress,port);
}

bool AppRemoteCfgNodesTCPIPService::getSettingsLocalValid(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress) {
	return ::getNodeServiceSettingsValid(src,nodeAddress,SERVICE_TCPIP,HOSTADDRESS_TYPE_LOCAL);
}

bool AppRemoteCfgNodesTCPIPService::getSettingsRemoteValid(DUOTECNO::AppRemoteCfgNodes& src,int nodeAddress) {
	return ::getNodeServiceSettingsValid(src,nodeAddress,SERVICE_TCPIP,HOSTADDRESS_TYPE_REMOTE);
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
// updaten van de login gegevens wanneer er een UDP decvice geconfigureerd werd.
// Nodig updaten bij verandering van:
// - De netwerk config.
// - homeserver passwoord en poort.
// - dyndns login gegevens.
bool AppRemoteCfgLoginInfo_Util::updateLoginAfterUdpDeviceCfg(DUOTECNO::AppRemoteCfgLoginInfo* dest, AppUDPDevice& r,int mask) {
	if (NULL == dest) {
		ASSERT(NULL != dest);
		return false;
	}

	const unsigned long physicalAddress = dest->getPhysicalAddress();
	if (!((physicalAddress == r.getPhysicalAddress()) && (physicalAddress > 0) && (physicalAddress < 0xffffff))) {
		return false;
	}
		
	ASSERT(physicalAddress == r.getPhysicalAddress());

	if (MASK_CONFIG_NWK == (mask & MASK_CONFIG_NWK)) {
		const CString& strIpAddress = r.getIpAddress();
		if (isValidIpAddress(strIpAddress)) {			
			dest->setLocalHostAddress(strIpAddress);			
		}
	}

	// Updaten van de homeserver cfg & poort
	if (MASK_CONFIG_HOMESERVER == (mask & MASK_CONFIG_HOMESERVER)) {
		AppUDPDeviceHomeServerCfg& refCfg = r.getAppHomeServerCfg();	
		const int port = refCfg.getIPPort( );
		if (port > 0) {
			dest->setLocalPort(port);
		}			
		const CString& strPassword = refCfg.getPassword();
		if (!strPassword.IsEmpty()) {
			dest->setPassword(strPassword);
		}
	}

	// Updaten van de dynDns client --> Remote DNS veranderen.
	if (MASK_CONFIG_DYNDNS == (mask & MASK_CONFIG_DYNDNS)) {
		AppUDPDeviceDDNSCfg& refCfg = r.getAppDDNSCfg();
		if (refCfg.getStateOnOff()) {
			const CString& hostName = refCfg.getHostname();
			if (!hostName.IsEmpty()) {
				dest->setRemoteHostAddress(hostName);
			}		
		}		
	}

	return true;
}