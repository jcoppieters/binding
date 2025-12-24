#include "stdafx.h"
#include "AppUDPServerCfgWrapper.h"
#include "dtudpconfiglib/dtudpconfig.h"					// udp lan config library
#include "dtudpconfiglib/dtdevicelist.h" 
/************************************************************************************/
using namespace DUOTECNO;
/************************************************************************************/

static void translateResponseCode(AppUDPServerCfgWrapper::ErrorResponseCode_t* dest,
								  ResultCode src) {
	switch(src) {
		case RESULTCODE_SUCCESS: {		
			*dest = AppUDPServerCfgWrapper::RESPONSE_SUCCESS;			
		} break;
		case RESULTCODE_FAILED: {
			*dest = AppUDPServerCfgWrapper::RESPONSE_EC_NAK;							
		} break;
		case RESULTCODE_TIMEOUT: {
			*dest = AppUDPServerCfgWrapper::RESPONSE_EC_TIMEOUT;						
		} break;
		case RESULTCODE_EC_FORMATTER: {
			*dest = AppUDPServerCfgWrapper::RESPONSE_EC_FORMATTER;
		} break;
		default: {
			*dest = AppUDPServerCfgWrapper::RESPONSE_EC_GENERAL;							
		} break;	
	}
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
AppUDPServerCfgWrapper::AppUDPServerCfgWrapper( ) : 
	m_responseCode(RESPONSE_EC_GENERAL) { }	

AppUDPServerCfgWrapper::~AppUDPServerCfgWrapper() { }


// Instellen van de App homesever cfg instellingen.
bool AppUDPServerCfgWrapper::sendCfgAppHomeSrv(AppUDPDevice& refDevice, int milliseconds)
{	
	if (!checkDevice(&refDevice)) {
		return false;
	}
	const CString& strIPAddress = refDevice.getIpAddress();
	const AppUDPDeviceHomeServerCfg& ref = refDevice.getAppHomeServerCfg();
	const char* const szPassword = (LPCSTR) ref.getPassword();
	const int IPPort = ref.getIPPort();
	const int protocolFlags = ref.getProtocolFlags();
	const ResultCode res = dtudpcfg_setCfgAppHomeSrv((LPCSTR) strIPAddress,szPassword,IPPort,protocolFlags,milliseconds);
	translateResponseCode(&this->m_responseCode,res);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

bool AppUDPServerCfgWrapper::sendRequestCfgAppHomeSrv(AppUDPDevice* pDevice,int milliseconds) 
{	
	if (!checkDevice(pDevice)) {
		return false;
	}
	
	AppUDPDeviceHomeServerCfg& ref = pDevice->getAppHomeServerCfg();
	ref.clear();

	const CString& strIPAddress = pDevice->getIpAddress();
	int ipport = -1;
	int protocolFlags = 0;
	char szPassword[32];
	memset(szPassword,0,sizeof(szPassword));
	const ResultCode res = dtudpcfg_getCfgAppHomeSrv((LPCSTR)strIPAddress,szPassword,&ipport,&protocolFlags,milliseconds);
	translateResponseCode(&this->m_responseCode,res);
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
		ref.setPassword(CString(szPassword));
		ref.setIPPort(ipport);
		ref.setProtocolFlags(protocolFlags);
	}
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

bool AppUDPServerCfgWrapper::sendNwkConfig(AppUDPDevice& refDevice,int milliseconds) {
	const CString& macAddress = refDevice.getMACAddress();
	if (macAddress.IsEmpty()) {
		ASSERT(!macAddress.IsEmpty());
		return false;
	}
	BYTE pbMACAddress[6] = { 0,0,0,0,0,0 };
	if (!AppUDPDevice::convertMacAddress(pbMACAddress, macAddress)) {
		ASSERT(0);
		this->m_responseCode = RESPONSE_EC_GENERAL;
		return false;
	}
	
	const char* pszIPAddress = NULL;
	const char* pszNetmask = NULL;
	const char* pszGateway = NULL;
	char szIPAddress[32];
	char szNetmask[32];
	char szGateway[32];

	if (refDevice.getDhcpOnOff()) {
		sprintf(szIPAddress, "%d.%d.%d.%d", 0, 0, 0, 0);
		sprintf(szNetmask, "%d.%d.%d.%d", 0, 0, 0, 0);
		sprintf(szGateway, "%d.%d.%d.%d", 0, 0, 0, 0);

		pszIPAddress = szIPAddress;
		pszNetmask = szNetmask;
		pszGateway = szGateway;
	}
	else {
		const CString& strIpAddress = refDevice.getIpAddress();
		const CString& strNetmask = refDevice.getNetmask();
		const CString& strGateway = refDevice.getGateway();

		pszIPAddress = (LPCSTR) strIpAddress;
		pszNetmask = (LPCSTR) strNetmask;
		pszGateway = (LPCSTR) strGateway;
	}

	ASSERT(NULL != pszIPAddress);
	ASSERT(NULL != pszNetmask);
	ASSERT(NULL != pszGateway);

	const ResultCode resultCode = dtudpcfg_configureDevice(pbMACAddress, refDevice.getDhcpOnOff(), pszIPAddress, pszNetmask, pszGateway, milliseconds);
	translateResponseCode(&this->m_responseCode,resultCode);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// DynDns enable/disable.
bool AppUDPServerCfgWrapper::sendDynDNSSettings(AppUDPDevice& refDevice,int milliseconds) {
	if (!checkDevice(&refDevice)) {
		return false;
	}
	const CString& strIPAddress = refDevice.getIpAddress();
	ResultCode resultCode = RESULTCODE_FAILED;
	AppUDPDeviceDDNSCfg& refCfg = refDevice.getAppDDNSCfg();
	if (refCfg.getStateOnOff()) {	
		const AppUDPDeviceDDNSCfg::Provider_t provider = refCfg.getProvider();
		const CString& strHostName = refCfg.getHostname();
		const CString& strPassword = refCfg.getPassword();
		const CString& strUserName = refCfg.getUsername();
		if (strHostName.IsEmpty() || strPassword.IsEmpty() || strUserName.IsEmpty()) {
			ASSERT(!strHostName.IsEmpty());
			ASSERT(!strPassword.IsEmpty());
			ASSERT(!strUserName.IsEmpty());
			this->m_responseCode = RESPONSE_EC_GENERAL;
			return false;
		}

		resultCode = dtudpcfg_setCfgAppDDNSEnable((LPCSTR) strIPAddress, (int)provider, 
			(LPCSTR) strHostName, (LPCSTR) strUserName, (LPCSTR) strPassword, milliseconds);	
	}
	else {
		resultCode = dtudpcfg_setCfgAppDDNSDisable((LPCSTR) strIPAddress,milliseconds);	
	}

	translateResponseCode(&this->m_responseCode,resultCode);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// Opvragen van de DynDns instellingen.
bool AppUDPServerCfgWrapper::sendRequestDynDNSSettings(AppUDPDevice* pDevice,int milliseconds) {
	if (!checkDevice(pDevice)) {
		return false;
	}

	AppUDPDeviceDDNSCfg& refCfg = pDevice->getAppDDNSCfg();
	refCfg.clear();

	const CString& strIPAddress = pDevice->getIpAddress();
	int providerID = DDNS_SERVICE_PROVIDER_NONE;
	int status = DDNS_STATUS_DISABLED;
	char szHostName[64];
	char szUserName[32];
	char szPassword[32];
	memset(szHostName,0,sizeof(szHostName));
	memset(szUserName,0,sizeof(szUserName));
	memset(szPassword,0,sizeof(szPassword));

	const ResultCode res = dtudpcfg_getCfgAppDDNS((LPCSTR)strIPAddress,&providerID,&status,
		szHostName, szUserName, szPassword, milliseconds);

	translateResponseCode(&this->m_responseCode,res);	
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
				
		AppUDPDeviceDDNSCfg::Provider_t dnsProvider = AppUDPDeviceDDNSCfg::PROVIDER_NONE;
		AppUDPDeviceDDNSCfg::Status_t dnsStatus = AppUDPDeviceDDNSCfg::STATUS_DISABLED;

		if (DDNS_SERVICE_PROVIDER_NOIP == providerID) {
			dnsProvider = AppUDPDeviceDDNSCfg::PROVIDER_NOIP;
		}

		switch(status) {
			case DDNS_STATUS_DISABLED: {
				dnsStatus = AppUDPDeviceDDNSCfg::STATUS_DISABLED;										   
			} break;
			case DDNS_STATUS_GOOD: {
				dnsStatus = AppUDPDeviceDDNSCfg::STATUS_GOOD;				   
			} break;
			case DDNS_STATUS_ERROR: {
				dnsStatus = AppUDPDeviceDDNSCfg::STATUS_ERROR;				   
			} break;
			case DDNS_STATUS_WAIT: {
				dnsStatus = AppUDPDeviceDDNSCfg::STATUS_WAIT;				   
			} break;
			default: {
				ASSERT(0); 
			} break;
		}

		refCfg.setStateOnOff(DDNS_SERVICE_PROVIDER_NOIP == providerID);
		refCfg.setProvider(dnsProvider);
		refCfg.setHostName(CString(szHostName));
		refCfg.setUserName(CString(szUserName));
		refCfg.setPassword(CString(szPassword));
		refCfg.setStatus(dnsStatus);
	}
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// Instellen van de DNS provider/server.
bool AppUDPServerCfgWrapper::sendCfgDNSProvider(AppUDPDevice& refDevice,int milliseconds) {
	if (!checkDevice(&refDevice)) {
		return false;
	}
	const CString& strIPAddress = refDevice.getIpAddress();	
	AppUDPDeviceDnsServerCfg& refCfg = refDevice.getAppDnsServerCfg();
	const CString& strDnsServer = refCfg.getDNSServerAddress();
	const ResultCode res = dtudpcfg_setCfgAppDNSProvider((LPCSTR)strIPAddress,(LPCSTR)strDnsServer,milliseconds);
	translateResponseCode(&this->m_responseCode,res);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// Opvragen van de DNS provider/server.
bool AppUDPServerCfgWrapper::sendRequestCfgDNSProvider(AppUDPDevice* pDevice,int milliseconds) 
{
	if (!checkDevice(pDevice)) {
		return false;
	}

	AppUDPDeviceDnsServerCfg& refCfg = pDevice->getAppDnsServerCfg();
	refCfg.clear();

	char dnsIPAddress[32];
	memset(dnsIPAddress,0,sizeof(dnsIPAddress));
	const CString& strIPAddress = pDevice->getIpAddress();
	const ResultCode res = dtudpcfg_getCfgAppDNSProvider((LPCSTR)strIPAddress, dnsIPAddress, milliseconds);
	translateResponseCode(&this->m_responseCode,res);
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
		const CString serverIpAddress(dnsIPAddress);
		refCfg.setDNSServerAddress(serverIpAddress);
	}
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// TCP/IP client...
bool AppUDPServerCfgWrapper::sendCfgTCPIPClient(AppUDPDevice& refDevice,int milliseconds) {
	if (!checkDevice(&refDevice)) {
		return false;
	}
	const CString& strIPAddress = refDevice.getIpAddress();
	AppUDPDeviceTCPIPClientCfg& refCfg = refDevice.getAppTCPIPClientCfg();
	const CString& strServerAddress = refCfg.getServerAddress();
	const int port = refCfg.getServerPort();

	const ResultCode res = dtudpcfg_setCfgAppTCPIPClient((LPCSTR)strIPAddress, (LPCSTR)strServerAddress, port, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

bool AppUDPServerCfgWrapper::sendRequestCfgTCPIPClient(AppUDPDevice* pDevice,int milliseconds) {
	if (!checkDevice(pDevice)) {
		return false;
	}

	AppUDPDeviceTCPIPClientCfg& refCfg = pDevice->getAppTCPIPClientCfg();
	refCfg.clear();

	char serverAddress[32];
	int serverIPPort = -1;
	int status = 0;
	memset(serverAddress, 0, sizeof(serverAddress));
	const CString& strIPAddress = pDevice->getIpAddress();
	const ResultCode res = dtudpcfg_getCfgAppTCPIPClient((LPCSTR)strIPAddress, serverAddress, &serverIPPort, &status, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
		const CString strServerIPAddress(serverAddress);
		refCfg.setServerAddress(strServerIPAddress);
	    refCfg.setServerPort(serverIPPort);
		refCfg.setStatusConnected(status);
	}
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// Time and date.
bool AppUDPServerCfgWrapper::sendCfgTimeDate(AppUDPDevice& refDevice, int milliseconds) {
	if (!checkDevice(&refDevice)) {
		return false;
	}
	const CString& strIPAddress = refDevice.getIpAddress();
	AppUDPDeviceNtpTimeCfg& refCfg = refDevice.getAppNtpTimeCfg();

	const int area = refCfg.getPoolID();
	const int zone = refCfg.getUTCZone();
	const int dstEnable = refCfg.getDSTEnable();
	const ResultCode res = dtudpcfg_setCfgAppTime((LPCSTR)strIPAddress, area, zone, dstEnable, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

bool AppUDPServerCfgWrapper::sendRequestCfgTimeDate(AppUDPDevice* pDevice, int milliseconds) {
	if (!checkDevice(pDevice)) {
		return false;
	}

	AppUDPDeviceNtpTimeCfg& refCfg = pDevice->getAppNtpTimeCfg();
	// refCfg.clear();	<-------- Mogen geen clear( ) uitvoeren.

	int area = -1;
	int zone = -1;
	int dstEnable = -1;
	const CString& strIPAddress = pDevice->getIpAddress();
	const ResultCode res = dtudpcfg_getCfgAppTime((LPCSTR)strIPAddress, &area, &zone, &dstEnable, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
		// refCfg.setPoolID(area);				// Wordt upgedate door opvragen van de NTP server poolid.
		refCfg.setDSTEnable(dstEnable);
		refCfg.setUTCZone(zone);
	}	
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// NTP
bool AppUDPServerCfgWrapper::sendCfgNTP(AppUDPDevice& refDevice, int milliseconds) {
	if (!checkDevice(&refDevice)) {
		return false;
	}
	const CString& strIPAddress = refDevice.getIpAddress();
	AppUDPDeviceNtpTimeCfg& refCfg = refDevice.getAppNtpTimeCfg();

	const int poolID = refCfg.getPoolID();
	const ResultCode res = dtudpcfg_setCfgAppSNTP((LPCSTR)strIPAddress, poolID, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

bool AppUDPServerCfgWrapper::sendRequestCfgNTP(AppUDPDevice* pDevice, int milliseconds) {
	if (!checkDevice(pDevice)) {
		return false;
	}

	AppUDPDeviceNtpTimeCfg& refCfg = pDevice->getAppNtpTimeCfg();
	// refCfg.clear();	<-------- Mogen geen clear( ) uitvoeren.

	int poolID = -1;
	const CString& strIPAddress = pDevice->getIpAddress();
	const ResultCode res = dtudpcfg_getCfgAppSNTP((LPCSTR)strIPAddress, &poolID, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
		refCfg.setPoolID(poolID);
	}
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

bool AppUDPServerCfgWrapper::sendCfgSystemMode(AppUDPDevice& refDevice, int milliseconds) {
	if (!checkDevice(&refDevice)) {
		return false;
	}

	const CString& strIPAddress = refDevice.getIpAddress();
	AppUDPDeviceSystemModeCfg& refCfg = refDevice.getAppSystemMode();
	const int systemMode = refCfg.getSystemMode();
	const ResultCode res = dtudpcfg_setCfgAppSystemMode((LPCSTR)strIPAddress, systemMode, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

bool AppUDPServerCfgWrapper::sendRequestCfgSystemMode(AppUDPDevice* pDevice, int milliseconds) {
	if (!checkDevice(pDevice)) {
		return false;
	}

	const CString& strIPAddress = pDevice->getIpAddress();
	AppUDPDeviceSystemModeCfg& refCfg = pDevice->getAppSystemMode();
	int systemMode = -1;
	const ResultCode res = dtudpcfg_getCfgAppSystemMode((LPCSTR)strIPAddress, &systemMode, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
		ASSERT((AppUDPDeviceSystemModeCfg::BASIC == systemMode) || (AppUDPDeviceSystemModeCfg::EXTENDED == systemMode));
		refCfg.setSystemMode((AppUDPDeviceSystemModeCfg::SystemMode_t) systemMode);
	}
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

// Opvragen van de applicatie cfg mogelijkheden.
bool AppUDPServerCfgWrapper::sendRequestCfgSupported(AppUDPDevice* pDevice, int milliseconds) {
	if (!checkDevice(pDevice)) {
		return false;
	}
	unsigned long supportedFlags = 0;
	const CString& strIPAddress = pDevice->getIpAddress();
	const ResultCode res = dtudpcfg_getCfgAppSupported((LPCSTR)strIPAddress, &supportedFlags, milliseconds);
	translateResponseCode(&this->m_responseCode, res);
	if (RESULTCODE_SUCCESS == this->m_responseCode) {
		pDevice->getAppSupportedFlags().setFlags(supportedFlags);
	}
	return (RESULTCODE_SUCCESS == this->m_responseCode);
}

AppUDPServerCfgWrapper::ErrorResponseCode_t AppUDPServerCfgWrapper::getResponseCode() const {
	return m_responseCode;
}

// Controle op de geldigheid van pDevice en de IP-Instellingen.
bool AppUDPServerCfgWrapper::checkDevice(AppUDPDevice* pDevice){	
	if (NULL == pDevice) {
		ASSERT(NULL != pDevice);
		this->m_responseCode = RESPONSE_EC_GENERAL;
		return false;
	}
	const CString& strIPAddress = pDevice->getIpAddress();
	if (strIPAddress.IsEmpty()) {
		ASSERT(0);
		this->m_responseCode = RESPONSE_EC_GENERAL;
		return false;
	}
	return true;
}
/************************************************************************************/

