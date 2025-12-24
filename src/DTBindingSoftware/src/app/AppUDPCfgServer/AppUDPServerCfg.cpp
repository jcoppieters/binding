#include "stdafx.h"
#include "AppUDPServerCfg.h"
#include "dtudpconfiglib/dtudpconfig.h"					// udp lan config library
#include "dtudpconfiglib/dtdevicelist.h"
/************************************************************************************/
using namespace DUOTECNO;
/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
AppUDPServerCfg::AppUDPServerCfg(void) :
	m_pList(new AppUDPDevices()),
	m_scanEnabled(true),
	m_intervalScanMsec(INTERVAL_SCAN_TIME_MSEC), 
	m_intervalUpdateWaitMsec(INTERVAL_SCAN_TIME_WAIT)
{
	dtudpcfg_initApplication( );
}

AppUDPServerCfg::~AppUDPServerCfg(void)
{
	dtudpcfg_shutdownApplication();

	if (0 != m_pList) {
		delete m_pList;
		m_pList = 0;
	}
}
/************************************************************************************/
void AppUDPServerCfg::setTimingsScanMode(ScanMode_t mode) {
	switch(mode) {
		case SCANMODE_NORMAL: {
			m_intervalScanMsec = INTERVAL_SCAN_TIME_MSEC; 
			m_intervalUpdateWaitMsec = INTERVAL_SCAN_TIME_WAIT;
		} break;
		case SCANMODE_FAST: {
			m_intervalScanMsec = INTERVAL_SCAN_FASTMODE_TIME_MSEC; 
			m_intervalUpdateWaitMsec = INTERVAL_SCAN_FASTMODE_TIME_WAIT;
		} break;
	}
}

void AppUDPServerCfg::selectNetworkAdapter(const CString& strNwkAdapter) {
	const char* szNetworkAdapter = NULL;
	if (!strNwkAdapter.IsEmpty()) {
		szNetworkAdapter = (LPCSTR) strNwkAdapter;
	}
	dtudpcfg_selectInterfaceAdapter(szNetworkAdapter);
}

void AppUDPServerCfg::eraseData( ) {
	this->clearDeviceList( );
	getDeviceList().erase( );
}

void AppUDPServerCfg::clearDeviceList( ) {
	dtudpcfg_eraseDeviceList();
}

void AppUDPServerCfg::requestScan( ) 
{
	dtudpcfg_scanNetwork();
}

// Updaten van de device list...
bool AppUDPServerCfg::updateDeviceList(CList<AppUDPDevice*>* pDevicesModified) // CList<int>* pListModified) 
{
	bool result = false;
	if (TRUE == dtupcfg_checkDeviceListModified()) {
		IDTDeviceList* pDeviceList = 0;	
		if (0 == dtudpcfg_getDeviceList(&pDeviceList)) {
			// Updaten van de nodes.		
			updateNodes(pDeviceList);
			// Lijst last found vlag.
			updateNodesFound(pDeviceList);
			// Opvragen van de lijst van de aangepaste devices.
			getNodeListChanged(pDevicesModified);
			// Return waarde...
			result = (0 == pDevicesModified->GetCount() ? false : true);
		}	
	}
	return result;
}

AppUDPDevices& AppUDPServerCfg::getDeviceList()  {
	ASSERT(0 != m_pList);
	return *m_pList;
}
/************************************************************************************/

bool AppUDPServerCfg::updateNodes(IDTDeviceList* pList) {
	bool result = false;
	if (0 == pList) {
		return false;
	}
	IDTDeviceIterator* const it = pList->createIterator( );
	if (0 != it) {
		for (it->begin();!it->end();it->next()) {
			int physicalNodeAddress = 0xffffff;
			IDTDevice* p = it->currentItem();					
			if (updateNode(p)) { 
				result = true;
			}
		}	
		pList->destroyIterator(it);
	}	
	return result;
}


// Zoeken in de lijst: Indien een nieuwe node (mac-address) dan bijvoegen, anders  de node updaten...
bool AppUDPServerCfg::updateNode(IDTDevice* pDevice) {
	bool result = false;
	if (0 == pDevice) {
		return false;
	}
	const BYTE* const pbMacAddress = pDevice->getMACAddress();
	if (0 == pbMacAddress) {
		return false;
	}
	CString strMacAddress;
	if (!AppUDPDevice::formatMacAddress(&strMacAddress,pbMacAddress)) {
		return false;
	}

	// Indien een device nog niet in de lijst aanwezig is dan deze toevoegen...	
	AppUDPDevice* pAppCfgDevice = 0;
	AppUDPDevices& refDevices = this->getDeviceList();
	if (!refDevices.find(strMacAddress,&pAppCfgDevice)) {
		pAppCfgDevice = new AppUDPDevice(strMacAddress);
		ASSERT(0 != pAppCfgDevice);
		refDevices.add(pAppCfgDevice);		
	}
	if (0 == pAppCfgDevice) {
		return false;
	}

	//
	// Hieronder alle properties...
	// 
	int devicePhysicalNodeAddress = pDevice->getPhysicalNodeAddress();
	if (devicePhysicalNodeAddress < 0 || devicePhysicalNodeAddress>=0xffffff) {
		// NXP Modules antwoorden niet met hun physical node address maar het physical node address kan gevonden 
		// worden in het het mac-Address.
		// MAC-Address van de nxp nodes starten met 1E:30:6C gevolgd door hun physical node address.
		if ((pbMacAddress[0] == 0x1E) && (pbMacAddress[1] == 0x30) && (pbMacAddress[2]==0x6C)) { 
			devicePhysicalNodeAddress = (pbMacAddress[3] << 16) + (pbMacAddress[4] << 8) + pbMacAddress[5];
		}
	}
	pAppCfgDevice->setName(pDevice->getDeviceName());
	pAppCfgDevice->setIpAddress(pDevice->getIPAddress());
	pAppCfgDevice->setGateway(pDevice->getGateway());
	pAppCfgDevice->setNetmask(pDevice->getNetmask());
	pAppCfgDevice->setPhysicalAddress(devicePhysicalNodeAddress);
	pAppCfgDevice->setDHCPOnOff(pDevice->getDHCPOnOff() == 0 ? false : true);
	pAppCfgDevice->setNodeAddress(pDevice->getNodeLogicalAddress());
	pAppCfgDevice->setSoftwareVersion(pDevice->getSoftwareVersion());
	pAppCfgDevice->setProductCode(pDevice->getProductID());
	
	result = pAppCfgDevice->getChanged();
	return result;
}

static bool findMAC(DUOTECNO::IDTDeviceList* pList,const CString& strMACAddress) {
	IDTDeviceIterator* const it = pList->createIterator( );
	if (0 != it) {
		for (it->begin();!it->end();it->next()) {			
			IDTDevice* p = it->currentItem();	
			CString strTemp;
			if (AppUDPDevice::formatMacAddress(&strTemp,p->getMACAddress())) {
				if (0 == strMACAddress.Compare(strTemp)) {
					return true;	// Gevonden...
				}
			}
		}
	}
	return false;
}

bool AppUDPServerCfg::updateNodesFound(DUOTECNO::IDTDeviceList* pList) { 
	if (0 == pList) {
		return false;
	}
	const int countDevices = getDeviceList().getCount();
	for (int i=0;i<countDevices;++i) {
		AppUDPDevice* p = NULL;
		if (getDeviceList().getAt(i,&p)) {
			const bool found = findMAC(pList,p->getMACAddress());
			p->setLastFound(found);
		}		
	}
	return true;
}

/*
static bool findValue(CList<int>* pListModified, int value) {
	return (NULL == pListModifled->Find(value) ? false : true);
}
*/

bool AppUDPServerCfg::getNodeListChanged(CList<AppUDPDevice*>* pDevicesModified) {
	if (NULL == pDevicesModified) {
		return false;
	}

	CList<int> m_listAddressesModified;
	bool result = false;
	const int countDevices = getDeviceList().getCount();
	for (int i=0;i<countDevices;++i) {
		AppUDPDevice* p = NULL;
		if (getDeviceList().getAt(i,&p)) {
			if (p->getChanged()) {
				const int address = p->getPhysicalAddress();
				if (NULL == m_listAddressesModified.Find(address)) {				
					m_listAddressesModified.AddTail(address);
					pDevicesModified->AddTail(p);
				}		
				result = true;
				p->setChanged(false);
			}
		}
	}
	return result;
}


/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor DynDns instellingen.
AppUDPDeviceDDNSCfg::AppUDPDeviceDDNSCfg( ) :  
	m_changedFlags(0), m_stateOnOff(0), m_provider(PROVIDER_NONE), m_status(STATUS_DISABLED),
	m_hostName(), m_userName(), m_szPassword() { }

AppUDPDeviceDDNSCfg::~AppUDPDeviceDDNSCfg( ) { 
	clear();
}

void AppUDPDeviceDDNSCfg::clear() {
	m_changedFlags = 0;
	m_stateOnOff = 0;
	m_provider = PROVIDER_NONE;		
	m_status = STATUS_DISABLED;
	m_hostName.Empty();
	m_userName.Empty();
	m_szPassword.Empty();
}
bool AppUDPDeviceDDNSCfg::changed() const {
	return (0 != m_changedFlags);
}

void AppUDPDeviceDDNSCfg::resetChanged( ) {
	m_changedFlags = 0;
}

// properties.
void AppUDPDeviceDDNSCfg::setStateOnOff(int state) {
	if (state != m_stateOnOff) {
		m_changedFlags |= FLAG_CHANGED_ONOFF;
		m_stateOnOff = state;
	}
}

void AppUDPDeviceDDNSCfg::setProvider(Provider_t provider) {
	if (provider != m_provider) {
		m_changedFlags |= FLAG_CHANGED_PROVIDER;
		m_provider = provider;
	}
}

void AppUDPDeviceDDNSCfg::setHostName(const CString& hostName) {
	if (0 != hostName.Compare(m_hostName)) {
		m_changedFlags |= FLAG_CHANGED_HOSTNAME;
		m_hostName = hostName;
	}
}

void AppUDPDeviceDDNSCfg::setUserName(const CString& userName) {
	if (0 != userName.Compare(m_userName)) {
		m_changedFlags |= FLAG_CHANGED_USERNAME;
		m_userName = userName;
	}
}
	
void AppUDPDeviceDDNSCfg::setPassword(const CString& password) {
	if (0 != password.Compare(m_szPassword)) {
		m_changedFlags |= FLAG_CHANGED_PASSWORD;
		m_szPassword = password;
	}
}

void AppUDPDeviceDDNSCfg::setStatus(Status_t status) {
	if (status != m_status) {
		m_changedFlags |= FLAG_CHANGED_STATUS;
		m_status = status;
	}
}

void AppUDPDeviceDDNSCfg::convertToString(CString* pStr, Status_t status) {
	switch (status) {
		case DDNS_STATUS_DISABLED: {
			*pStr = "disabled";
		} break;
		case DDNS_STATUS_GOOD: {
			*pStr = "good";
		} break;
		case DDNS_STATUS_ERROR: {
			*pStr = "error";
		} break;
		case DDNS_STATUS_WAIT: {
			*pStr = "waiting";
		} break;
		default: {
			*pStr = "disabled";
		} break;
	}
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
const int CAppSupportedFlags::CFG_SERVER = DUOTECNO::SUPPORTED_FLAG_SERVER;
const int CAppSupportedFlags::CFG_DDNS = DUOTECNO::SUPPORTED_FLAG_DDNSCLIENT;
const int CAppSupportedFlags::CFG_SYSMODE = DUOTECNO::SUPPORTED_FLAG_SYSMODE;
const int CAppSupportedFlags::CFG_TCPIPCLIENT = DUOTECNO::SUPPORTED_FLAG_TCPIPCLIENT;
const int CAppSupportedFlags::CFG_DNSPROVIDER = DUOTECNO::SUPPORTED_FLAG_DNSPROVIDER;
const int CAppSupportedFlags::CFG_SNTP = DUOTECNO::SUPPORTED_FLAG_SNTP;
const int CAppSupportedFlags::CFG_TIME = DUOTECNO::SUPPORTED_FLAG_TIME;
const int CAppSupportedFlags::CFG_API = DUOTECNO::SUPPORTED_FLAG_API;			

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
AppUDPDeviceHomeServerCfg::AppUDPDeviceHomeServerCfg( ) :
	m_changedFlags(0), m_szPassword(), m_ipPort(-1), m_protocolFlags(0x03)
{ 
}
AppUDPDeviceHomeServerCfg::~AppUDPDeviceHomeServerCfg( ) { 
	clear();
}

bool AppUDPDeviceHomeServerCfg::changed() const {
	return (m_changedFlags != 0);
}
void AppUDPDeviceHomeServerCfg::resetChanged( ) {
	m_changedFlags = 0;
}

void AppUDPDeviceHomeServerCfg::clear() {
	m_changedFlags = 0; 
	m_szPassword.Empty();
	m_ipPort = -1;
	m_protocolFlags = 0;
}

void AppUDPDeviceHomeServerCfg::setPassword(const CString& password) {
	if (0 != m_szPassword.Compare(password)) {
		m_szPassword = password;
		m_changedFlags |= FLAG_CHANGED_PASSWORD;
	}
}	

void AppUDPDeviceHomeServerCfg::setIPPort(int port) {
	if (m_ipPort != port) {
		m_ipPort = port;
		m_changedFlags |= FLAG_CHANGED_IPPORT;
	}
}

void AppUDPDeviceHomeServerCfg::setProtocolFlags(int flags) {
	if (m_protocolFlags != flags) {
		m_protocolFlags = flags;
		m_changedFlags |= FLAG_CHANGED_PROTOCOLFLAGS;
	}
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor DynDns instellingen.
AppUDPDeviceSystemModeCfg::AppUDPDeviceSystemModeCfg( ) :
	m_changedFlags(0), m_systemMode(EXTENDED) { }
AppUDPDeviceSystemModeCfg::~AppUDPDeviceSystemModeCfg( ) { }

void AppUDPDeviceSystemModeCfg::clear() {
	m_changedFlags = 0;
	m_systemMode = EXTENDED;
}

bool AppUDPDeviceSystemModeCfg::changed() const {
	return (m_changedFlags != 0);
}	

void AppUDPDeviceSystemModeCfg::resetChanged( ) {
	m_changedFlags = 0;
}

void AppUDPDeviceSystemModeCfg::setSystemMode(SystemMode_t mode) {
	if (mode != m_systemMode) {
		m_changedFlags |= FLAG_CHANGED;
		m_systemMode = mode;
	}
}
/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor de DNS.
AppUDPDeviceDnsServerCfg::AppUDPDeviceDnsServerCfg( ) : 
	m_changedFlags(0), m_dnsServerAddress() { }
AppUDPDeviceDnsServerCfg::~AppUDPDeviceDnsServerCfg( ) {		
}

void AppUDPDeviceDnsServerCfg::clear() {
	m_changedFlags = 0;
	m_dnsServerAddress.Empty();
}

bool AppUDPDeviceDnsServerCfg::changed() const {
	return (0 != m_changedFlags);
}

void AppUDPDeviceDnsServerCfg::resetChanged( ) {
	m_changedFlags = 0;	
}

void AppUDPDeviceDnsServerCfg::setDNSServerAddress(const CString& serverAddress) {
	if (0 != m_dnsServerAddress.Compare(serverAddress)) {
		m_changedFlags |= FLAG_CHANGED;
		m_dnsServerAddress = serverAddress;
	}
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
AppUDPDeviceTCPIPClientCfg::AppUDPDeviceTCPIPClientCfg() : 
	m_changedFlags(0), m_serverAddress(), m_serverPort(0), m_statusConnected(0) { }

AppUDPDeviceTCPIPClientCfg::~AppUDPDeviceTCPIPClientCfg() { }

void AppUDPDeviceTCPIPClientCfg::clear() {
	m_changedFlags = 0;
	m_serverAddress.Empty();
	m_serverPort = 0;
	m_statusConnected = 0;
}

bool AppUDPDeviceTCPIPClientCfg::changed() const {
	return (0 != m_changedFlags);
}	

void AppUDPDeviceTCPIPClientCfg::resetChanged( ) {
	m_changedFlags = 0;
}

// Properties.
void AppUDPDeviceTCPIPClientCfg::setServerAddress(const CString& str) {
	if (0 != str.Compare(m_serverAddress)) {
		m_serverAddress = str;
		m_changedFlags |= FLAG_CHANGED_ADDRESS;
	}
}

void AppUDPDeviceTCPIPClientCfg::setServerPort(int port) {
	if (port != m_serverPort) {
		m_changedFlags |= FLAG_CHANGED_PORT;
		m_serverPort = port;
	}
}

void AppUDPDeviceTCPIPClientCfg::setStatusConnected(int status) {
	if (status != m_statusConnected) {
		m_changedFlags |= FLAG_CHANGED_STATUS;
		m_statusConnected = status;
	}
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor de SNTP Time server
AppUDPDeviceNtpTimeCfg::AppUDPDeviceNtpTimeCfg() : 
	m_changedFlags(0), m_poolID(0), m_utcZone(1), m_dstEnabled(0) { }

AppUDPDeviceNtpTimeCfg::~AppUDPDeviceNtpTimeCfg() {}

void AppUDPDeviceNtpTimeCfg::clear() {
	m_changedFlags = 0;
	m_poolID = 0;
	m_utcZone = 0;
	m_dstEnabled = 0;
}

bool AppUDPDeviceNtpTimeCfg::changed() const {
	return (0 != m_changedFlags);
}

void AppUDPDeviceNtpTimeCfg::resetChanged( ) {
	m_changedFlags = 0;
}

void AppUDPDeviceNtpTimeCfg::setPoolID(int pool) {
	if (pool != m_poolID) {
		m_changedFlags |= FLAG_CHANGED_POOL;
		m_poolID = pool;
	}
}

void AppUDPDeviceNtpTimeCfg::setUTCZone(int zone) {
	if (zone != m_utcZone) {
		m_changedFlags |= FLAG_CHANGED_UTCZONE;
		m_utcZone = zone;
	}
}

void AppUDPDeviceNtpTimeCfg::setDSTEnable(int value) {
	if (value != m_dstEnabled) {
		m_changedFlags |= FLAG_CHANGED_DST;
		m_dstEnabled = value;
	}
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
AppUDPDevice::AppUDPDevice(const CString& macAddress) :
	m_macAddress(macAddress), m_deviceName(), m_ipAddress(), m_gateway( ), m_netmask( ),
	m_physicalNodeAddress(0xffffff), m_nodeAddress(0xff), m_productCode(0xff), m_softwareVersion(0xffff),
	m_flags(0), m_pAppSupportedFlags(NULL), m_pAppHomeServerCfg(NULL), m_pAppDDNSConfig(NULL), m_pAppSystemModeCfg(NULL), 
	m_pAppDnsServerCfg(NULL), m_pAppNtpTimeCfg(NULL), m_pAppTCPIPClientCfg(NULL)
{ 	
	m_flags += FLAG_CHANGED;
	m_flags += FLAG_DHCP;
}

AppUDPDevice::AppUDPDevice(const AppUDPDevice& r) :
	m_macAddress(r.m_macAddress), m_deviceName(r.m_deviceName), m_ipAddress(r.m_ipAddress), m_gateway(r.m_gateway ), 
	m_netmask(r.m_netmask), m_physicalNodeAddress(r.m_physicalNodeAddress), m_nodeAddress(r.m_nodeAddress), 
	m_productCode(r.m_productCode), m_softwareVersion(r.m_softwareVersion), m_flags(r.m_flags), m_pAppSupportedFlags(NULL),
	m_pAppHomeServerCfg(NULL), m_pAppDDNSConfig(NULL), m_pAppSystemModeCfg(NULL), m_pAppDnsServerCfg(NULL),
	m_pAppNtpTimeCfg(NULL), m_pAppTCPIPClientCfg(NULL)
{	
	if (NULL != r.m_pAppSupportedFlags) {
		m_pAppSupportedFlags = new CAppSupportedFlags(*r.m_pAppSupportedFlags);
		ASSERT(NULL != m_pAppSupportedFlags);
	}
	if (NULL != r.m_pAppHomeServerCfg) {
		m_pAppHomeServerCfg = new AppUDPDeviceHomeServerCfg(*r.m_pAppHomeServerCfg);
		ASSERT(NULL != m_pAppHomeServerCfg);
	}
	if (NULL != r.m_pAppDDNSConfig) {
		m_pAppDDNSConfig = new AppUDPDeviceDDNSCfg(*r.m_pAppDDNSConfig);
		ASSERT(NULL != m_pAppDDNSConfig);
	}
	if (NULL != r.m_pAppSystemModeCfg) {
		m_pAppSystemModeCfg = new AppUDPDeviceSystemModeCfg(*r.m_pAppSystemModeCfg);
		ASSERT(NULL != m_pAppSystemModeCfg);
	}
	if (NULL != r.m_pAppDnsServerCfg) {
		m_pAppDnsServerCfg = new AppUDPDeviceDnsServerCfg(*r.m_pAppDnsServerCfg);
		ASSERT(NULL != m_pAppDnsServerCfg);
	}
	if (NULL != r.m_pAppNtpTimeCfg) {
		m_pAppNtpTimeCfg = new AppUDPDeviceNtpTimeCfg(*r.m_pAppNtpTimeCfg);
		ASSERT(NULL != m_pAppNtpTimeCfg);
	}
	if (NULL != r.m_pAppTCPIPClientCfg) {
		m_pAppTCPIPClientCfg = new AppUDPDeviceTCPIPClientCfg(*r.m_pAppTCPIPClientCfg);
		ASSERT(NULL != m_pAppTCPIPClientCfg);
	}
}

AppUDPDevice::~AppUDPDevice( ) { 
	if (NULL != m_pAppSupportedFlags) {
		delete m_pAppSupportedFlags;
		m_pAppSupportedFlags = NULL;
	}
	if (NULL != m_pAppHomeServerCfg) {
		delete m_pAppHomeServerCfg;
		m_pAppHomeServerCfg = NULL;
	}
	if (NULL != m_pAppDDNSConfig) {
		delete m_pAppDDNSConfig;
		m_pAppDDNSConfig = NULL;
	}
	if (NULL != m_pAppSystemModeCfg) {
		delete m_pAppSystemModeCfg;
		m_pAppSystemModeCfg = NULL;
	}
	if (NULL != m_pAppDnsServerCfg) {
		delete m_pAppDnsServerCfg;
		m_pAppDnsServerCfg = NULL;
	}
	if (NULL != m_pAppNtpTimeCfg) {
		delete m_pAppNtpTimeCfg;
		m_pAppNtpTimeCfg = NULL;
	}
	if (NULL != m_pAppTCPIPClientCfg) {
		delete m_pAppTCPIPClientCfg;
		m_pAppTCPIPClientCfg = NULL;
	}
}

AppUDPDevice& AppUDPDevice::operator=(const AppUDPDevice& r) {
	if (this == &r) {
		return *this;
	}

	// m_macAddress = r.m_macAddress;
	m_deviceName = r.m_deviceName; 
	m_ipAddress = r.m_ipAddress; 
	m_gateway = r.m_gateway;
	m_netmask = r.m_netmask;
	m_physicalNodeAddress = r.m_physicalNodeAddress; 
	m_nodeAddress = r.m_nodeAddress;
	m_productCode = r.m_productCode; 
	m_softwareVersion = r.m_softwareVersion;
	m_flags = r.m_flags;

	// Dealloc.
	if (NULL != m_pAppSupportedFlags) {
		delete m_pAppSupportedFlags;
		m_pAppSupportedFlags = NULL;
	}
	if (NULL != m_pAppHomeServerCfg) {		
		delete m_pAppHomeServerCfg;
		m_pAppHomeServerCfg = NULL;
	}
	if (NULL != m_pAppDDNSConfig) {
		delete m_pAppDDNSConfig;
		m_pAppDDNSConfig = NULL;
	}
	if (NULL != m_pAppSystemModeCfg) {
		delete m_pAppSystemModeCfg;
		m_pAppSystemModeCfg = NULL;
	}
	if (NULL != m_pAppDnsServerCfg) {
		delete m_pAppDnsServerCfg;
		m_pAppDnsServerCfg = NULL;
	}
	if (NULL != m_pAppNtpTimeCfg) {
		delete m_pAppNtpTimeCfg;
		m_pAppNtpTimeCfg = NULL;
	}
	if (NULL != m_pAppTCPIPClientCfg) {
		delete m_pAppTCPIPClientCfg;
		m_pAppTCPIPClientCfg = NULL;
	}

	// Create new.
	if (NULL != r.m_pAppSupportedFlags) {	
		m_pAppSupportedFlags = new CAppSupportedFlags(*r.m_pAppSupportedFlags);
		ASSERT(NULL != m_pAppSupportedFlags);
	}
	if (NULL != r.m_pAppHomeServerCfg) {
		m_pAppHomeServerCfg = new AppUDPDeviceHomeServerCfg(*r.m_pAppHomeServerCfg);
		ASSERT(NULL != m_pAppHomeServerCfg);
	}
	if (NULL != r.m_pAppDDNSConfig) {
		m_pAppDDNSConfig = new AppUDPDeviceDDNSCfg(*r.m_pAppDDNSConfig);
		ASSERT(NULL != m_pAppDDNSConfig);
	}
	if (NULL != r.m_pAppSystemModeCfg) {
		m_pAppSystemModeCfg = new AppUDPDeviceSystemModeCfg(*r.m_pAppSystemModeCfg);
		ASSERT(NULL != m_pAppSystemModeCfg);
	}
	if (NULL != r.m_pAppDnsServerCfg) {
		m_pAppDnsServerCfg = new AppUDPDeviceDnsServerCfg(*r.m_pAppDnsServerCfg);
		ASSERT(NULL != m_pAppDnsServerCfg);
	}
	if (NULL != r.m_pAppNtpTimeCfg) {
		m_pAppNtpTimeCfg = new AppUDPDeviceNtpTimeCfg(*r.m_pAppNtpTimeCfg);
		ASSERT(NULL != m_pAppNtpTimeCfg);
	}
	if (NULL != r.m_pAppTCPIPClientCfg) {
		m_pAppTCPIPClientCfg = new AppUDPDeviceTCPIPClientCfg(*r.m_pAppTCPIPClientCfg);
		ASSERT(NULL != m_pAppTCPIPClientCfg);
	}
	return *this;
}


bool AppUDPDevice::formatMacAddress(CString* dest,const BYTE* pbMacAddress) {
		
	if (0 == dest || 0 == pbMacAddress) {
		ASSERT(0 != dest);
		ASSERT(0 != pbMacAddress);
		return false;
	}

	dest->Format("%02X:%02X:%02X:%02X:%02X:%02X",
		pbMacAddress[0], pbMacAddress[1], pbMacAddress[2], 
		pbMacAddress[3], pbMacAddress[4], pbMacAddress[5]);

	return true;
}

bool AppUDPDevice::convertMacAddress(BYTE* pbMacAddress, const CString& s) {
	if (NULL == pbMacAddress) {
		ASSERT(0 != pbMacAddress);
		return false;
	}

	int v[6] = { 0, 0, 0, 0, 0, 0 };
	if (6 != sscanf((LPCSTR) s, "%02X:%02X:%02X:%02X:%02X:%02X", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5])) {
		return false;
	}

	for (int i = 0; i < 6; ++i) {
		pbMacAddress[i] = v[i];
	}
	return true;
}

void AppUDPDevice::updateNetworkSettings(AppUDPDevice* pDest,const AppUDPDevice& r) {
	if (NULL == pDest) {
		return;
	}

	pDest->setIpAddress((LPCSTR)r.getIpAddress());
	pDest->setNetmask((LPCSTR)r.getNetmask());
	pDest->setGateway((LPCSTR)r.getGateway());
	pDest->setDHCPOnOff(r.getDhcpOnOff());
}

// Bepalen of het device een NXP hardware device is.
// Het MAC address van een NXP begint altijd met 1E:30:6C (gevolgd door het physical address...)
bool AppUDPDevice::IsHardwareNxp(const AppUDPDevice& r) {
	BYTE pbMACAddress[6] = { 0,0,0,0,0,0 };
	if (AppUDPDevice::convertMacAddress(pbMACAddress, r.m_macAddress)) {		
		if ((pbMACAddress[0] == 0x1E) && (pbMACAddress[1] == 0x30) && (pbMACAddress[2]==0x6C)) {		
			return true;
		}
	}
	return false;
}

void AppUDPDevice::setName(const char* s) {
	if (0 == s) {
		return;
	}
	const CString newString(s);
	if (0 != m_deviceName.Compare(newString)) {
		setChanged(true);
		m_deviceName = newString;
	}
}
void AppUDPDevice::setIpAddress(const char* s) {
	if (0 == s) {
		return;
	}
	const CString newString(s);
	if (0 != m_ipAddress.Compare(newString)) {
		setChanged(true);
		m_ipAddress = newString;
	}
}
void AppUDPDevice::setGateway(const char* s) {
	if (0 == s) {
		return;
	}
	const CString newString(s);
	if (0 != m_gateway.Compare(newString)) {
		setChanged(true);
		m_gateway = newString;
	}
}
void AppUDPDevice::setNetmask(const char* s) {
	if (0 == s) {
		return;
	}
	const CString newString(s);
	if (0 != m_netmask.Compare(newString)) {
		setChanged(true);
		m_netmask = newString;
	}
}
void AppUDPDevice::setPhysicalAddress(int v) {
	if (m_physicalNodeAddress != v) {
		setChanged(true);
		m_physicalNodeAddress = v;
	}
}

void AppUDPDevice::setNodeAddress(int v) {
	if (m_nodeAddress != v) {
		setChanged(true);
		m_nodeAddress = v;
	}
}

void AppUDPDevice::setSoftwareVersion(int v) {
	if (m_softwareVersion != v) {
		setChanged(true);
		m_softwareVersion = v;
	}
}

void AppUDPDevice::setProductCode(int v) {
	if (m_productCode != v) {
		setChanged(true);
		m_productCode = v;
	}
}

void AppUDPDevice::setDHCPOnOff(bool v) {
	const bool old = ((m_flags & FLAG_DHCP) == FLAG_DHCP);
	if (old != v) {
		setChanged(true);
		if (v) {
			m_flags |= FLAG_DHCP;
		}
		else {
			m_flags &= ~FLAG_DHCP;
		}
	}
}

void AppUDPDevice::setLastFound(bool flag) {

	const bool foundPrev = ((m_flags & FLAG_FOUND_LASTSCAN) == FLAG_FOUND_LASTSCAN);	
	if (flag != foundPrev) {
		setChanged(true);
						
		// De huidige toestand updaten.
		if (flag) {
			m_flags |= FLAG_FOUND_LASTSCAN;
		}
		else {
			m_flags &= ~FLAG_FOUND_LASTSCAN;
		}
	}
}

void AppUDPDevice::setChanged(bool v) {
	if (v) {
		m_flags |= FLAG_CHANGED;
	}
	else {
		m_flags &= ~FLAG_CHANGED;
	}
}

/**************************************************************************************/

CAppSupportedFlags& AppUDPDevice::getAppSupportedFlags() {
	if (NULL == m_pAppSupportedFlags) {
		m_pAppSupportedFlags = new CAppSupportedFlags();
	}
	ASSERT(NULL != m_pAppSupportedFlags);
	return *m_pAppSupportedFlags;
}

// Opvragen van de Specifieke App instellingen.
AppUDPDeviceHomeServerCfg& AppUDPDevice::getAppHomeServerCfg() {
	if (NULL == m_pAppHomeServerCfg) {
		m_pAppHomeServerCfg = new AppUDPDeviceHomeServerCfg( );		
	}
	ASSERT(NULL != m_pAppHomeServerCfg);
	return *m_pAppHomeServerCfg;	
}

AppUDPDeviceDDNSCfg& AppUDPDevice::getAppDDNSCfg() {
	if (NULL == m_pAppDDNSConfig) {
		m_pAppDDNSConfig = new AppUDPDeviceDDNSCfg( );		
	}
	ASSERT(NULL != m_pAppDDNSConfig);
	return *m_pAppDDNSConfig;	
}

AppUDPDeviceSystemModeCfg& AppUDPDevice::getAppSystemMode() {
	if (NULL == m_pAppSystemModeCfg) {
		m_pAppSystemModeCfg = new AppUDPDeviceSystemModeCfg( );		
	}
	ASSERT(NULL != m_pAppSystemModeCfg);
	return *m_pAppSystemModeCfg;	
}

AppUDPDeviceDnsServerCfg& AppUDPDevice::getAppDnsServerCfg() {
	if (NULL == m_pAppDnsServerCfg) {
		m_pAppDnsServerCfg = new AppUDPDeviceDnsServerCfg( );		
	}
	ASSERT(NULL != m_pAppDnsServerCfg);
	return *m_pAppDnsServerCfg;	
}

AppUDPDeviceNtpTimeCfg& AppUDPDevice::getAppNtpTimeCfg() {
	if (NULL == m_pAppNtpTimeCfg) {
		m_pAppNtpTimeCfg = new AppUDPDeviceNtpTimeCfg( );		
	}
	ASSERT(NULL != m_pAppNtpTimeCfg);
	return *m_pAppNtpTimeCfg;	
}

AppUDPDeviceTCPIPClientCfg& AppUDPDevice::getAppTCPIPClientCfg() {
	if (NULL == m_pAppTCPIPClientCfg) {
		m_pAppTCPIPClientCfg = new AppUDPDeviceTCPIPClientCfg();
	}
	ASSERT(NULL != m_pAppTCPIPClientCfg);
	return *m_pAppTCPIPClientCfg;
}
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
AppUDPDevices::AppUDPDevices( ) : 
	m_list( ) { }

AppUDPDevices::~AppUDPDevices( ) 
{ 
	this->erase();
}

// Wissen van de lijst + deallocatie.
void AppUDPDevices::erase( ) {
	while (!m_list.IsEmpty()) {
		AppUDPDevice* const p = m_list.RemoveHead( );
		ASSERT( p );
		delete p;
	}
	ASSERT(m_list.GetCount( )==0 );
}

bool AppUDPDevices::find(const CString& strMacAddress,AppUDPDevice **ppDevice) const {
	POSITION pos = m_list.GetHeadPosition();
	while(pos!=NULL)  {
		AppUDPDevice* pEntry = 0;
		if ((pEntry=m_list.GetNext(pos)) != NULL) {	
			if (0 == pEntry->getMACAddress().Compare(strMacAddress)) {
				if (0 != ppDevice) {
					*ppDevice = pEntry;
				}
				return true;
			}			
		}
	}
	return false;
}
bool AppUDPDevices::find(int physicalNodeAddress, const AppUDPDevice **ppDevice) const {
	POSITION pos = m_list.GetHeadPosition();
	while(pos!=NULL)  {
		AppUDPDevice* pEntry = 0;
		if ((pEntry=m_list.GetNext(pos)) != NULL) {	
			if (physicalNodeAddress == pEntry->getPhysicalAddress()) {
				if (0 != ppDevice) {
					*ppDevice = pEntry;
				}
				return true;
			}			
		}
	}
	return false;
}

bool AppUDPDevices::add(AppUDPDevice* pNewDevice) {
	if (0 == pNewDevice) {
		return false;
	}
	ASSERT(false == find(pNewDevice->getMACAddress(),0));
	m_list.AddTail(pNewDevice);
	return true;
}

int AppUDPDevices::getCount() {
	return m_list.GetCount();
}

bool AppUDPDevices::getAt(int index,AppUDPDevice **ppDevice) {
	if (0 == ppDevice) {
		return false;
	}
	if (index>=m_list.GetCount()) {
		return false;
	}
	const POSITION pos = m_list.FindIndex( index );
	if (pos == NULL)
	{
		return false;
	}
	*ppDevice = m_list.GetAt(pos);
	return true;
}

bool AppUDPDevices::remove(const CString& strMacAddress) {
	int index = 0;
	int indexInList = -1;

	{
		POSITION pos = m_list.GetHeadPosition();
		while(pos!=NULL)  {		
			AppUDPDevice* pEntry = NULL;
			if ((pEntry=m_list.GetNext(pos)) != NULL) {
				if (0 == strMacAddress.Compare(pEntry->getMACAddress())) {
					indexInList = index;	
					break;
				}
				index++;
			}
		}
	}

	if (indexInList >=0) {
		ASSERT(indexInList < m_list.GetCount());
		const POSITION tempPos = m_list.FindIndex(indexInList);
		if (NULL != tempPos) {
			AppUDPDevice* const p = m_list.GetAt(tempPos);
			if (NULL != p) {
				ASSERT(0 == strMacAddress.Compare(p->getMACAddress()));
				delete p;
			}		
			m_list.RemoveAt(tempPos);
			return true;
		}
	}
	return false;
}
/************************************************************************************/


/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
bool AppUDPDevices_Utils::updateNetworkSettings(const AppUDPDevices& refDevices,AppUDPDevice* pDest) {
	AppUDPDevice* pDeviceSource = NULL;
	if (!refDevices.find(pDest->getMACAddress(), &pDeviceSource)) {
		return false;
	}
	ASSERT(NULL != pDeviceSource);
	if (!pDeviceSource->getLastFound()) {
		return false;
	}

	// Controle op geldige ip-instellingen.
	const CString& strIpAddress = pDeviceSource->getIpAddress();
	if (strIpAddress.IsEmpty() || (0 == strIpAddress.Compare("0.0.0.0")) || (0 == strIpAddress.Compare("255.255.255.255")))	{
		return false;
	}

	bool retValue = false;
	const bool source_dhcp_on = pDeviceSource->getDhcpOnOff();
	const bool dest_dhcp_on = pDest->getDhcpOnOff();

	// Wanneer beide op dhcp staan = ok.
	if (source_dhcp_on == dest_dhcp_on) {
		retValue = true;
	}
	else{
		if (!source_dhcp_on) {
			// Wanneer het gevonden device niet op DHCP staat dan moeten de ip-addressen gelijk zijn.
			if (0 == pDest->getIpAddress().Compare(strIpAddress)) {
				retValue = true;
			}
			else { retValue = false; }
		}
		else { retValue = false; }
	}

	// Enkel de networksettings updaten...
	if (retValue) {
		AppUDPDevice::updateNetworkSettings(pDest,*pDeviceSource);
	}
	return retValue;
}

/************************************************************************************/
uint32_t AppUDPDevice_Utils::convertMACAddress2Uint32(BYTE* pbMACAddress) {
	return (pbMACAddress[2]<<24) + (pbMACAddress[3]<<16) + (pbMACAddress[4]<<8) + pbMACAddress[5];
}

uint32_t AppUDPDevice_Utils::convertMACAddress2Uint32(const AppUDPDevice& refDevice) {
	BYTE pbMACAddress[6] = { 0,0,0,0,0,0 };	
	const CString& strMacAddress = refDevice.getMACAddress();
	AppUDPDevice::convertMacAddress(pbMACAddress, strMacAddress);
	return convertMACAddress2Uint32(pbMACAddress); 
}

/************************************************************************************/