#include "stdafx.h"
#include "AppRemoteCfgCmn.h"

/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgServerInfo::AppRemoteCfgServerInfo() :
	m_hostName(), m_hostPort(0), m_secureLogin(false),
	m_productName(), m_softwareVersion(), m_physicalAddress(0xffffff), m_nodeAddress(0xff), 
	m_qtVersion( ), m_hardware( )
{
}
void AppRemoteCfgServerInfo::clear() {
	m_hostName.Empty();
	m_hostPort = 0;
	m_secureLogin = false;
	m_productName.Empty();
	m_softwareVersion.Empty();
	m_physicalAddress = 0xFFFFF;
	m_nodeAddress = 0xFF;
	m_qtVersion.Empty();
	m_hardware.Empty();
}

void AppRemoteCfgServerInfo::setHostName(const CString& name) {
	m_hostName = name;
}
const CString& AppRemoteCfgServerInfo::getHostName() const {
	return m_hostName;
}

void AppRemoteCfgServerInfo::setSoftwareVersion(const CString& name) {
	m_softwareVersion = name;
}
const CString& AppRemoteCfgServerInfo::getSoftwareVersion() const {
	return m_softwareVersion;
}

void AppRemoteCfgServerInfo::setHostPort(int v) {
	m_hostPort = v;
}
int AppRemoteCfgServerInfo::getHostPort() const {
	return m_hostPort;
}

void AppRemoteCfgServerInfo::setSecureLogin(bool value) {
	m_secureLogin = value;
}
bool AppRemoteCfgServerInfo::getSecureLogin() const {
	return m_secureLogin;
}

void AppRemoteCfgServerInfo::setProductName(const CString& name) {
	m_productName = name;
}
const CString& AppRemoteCfgServerInfo::getProductName() const {
	return m_productName;
}

void AppRemoteCfgServerInfo::setPhysicalAddress(unsigned long v) {
	m_physicalAddress = v;
}
unsigned long AppRemoteCfgServerInfo::getPhysicalAddress() const {
	return m_physicalAddress;
}

void AppRemoteCfgServerInfo::setNodeAddress(int nodeAddress) {
	if (nodeAddress >= 0 && nodeAddress <= 0xff) {
		m_nodeAddress = nodeAddress;
	}
}
int AppRemoteCfgServerInfo::getNodeAddress() const {
	return m_nodeAddress;
}

void AppRemoteCfgServerInfo::setQtVersion(const CString& version) {
	m_qtVersion = version;
}
const CString& AppRemoteCfgServerInfo::getQtVersion() const {
	return m_qtVersion;
}

void AppRemoteCfgServerInfo::setHardware(const CString& v) {
	m_hardware = v;
}
const CString& AppRemoteCfgServerInfo::getHardware() const {
	return m_hardware;
}

/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgServerInfo_Utils::type_t AppRemoteCfgServerInfo_Utils::getType(const AppRemoteCfgServerInfo& r) {
	if (0 == strcmp(r.m_productName, "TCP/IP homeserver")) {
		return HOMESERVER;
	}
	else if (0 == strcmp(r.m_productName, "Touchscreen 100 Virtuals")) {
		return TOUCHSCREEN;
	}
	// TODO AV
	return UNKNOWN;
}

bool AppRemoteCfgServerInfo_Utils::getSoftwareVersion(const AppRemoteCfgServerInfo& r,int* major, int* minor, int* micro) {
	const char* s = (LPCSTR)r.m_softwareVersion;
	if (0 == s || 0 == strcmp(s, "")) {
		return false;
	}
	if (sscanf(s,"V%x.%x.%x",major,minor,micro)>=3) {
		return true;
	}
	*major = -1;
	*minor = -1;
	*micro = -1;
	return false;
}

AppRcfgSupportedFlags AppRemoteCfgServerInfo_Utils::getSupportedFlags(const AppRemoteCfgServerInfo& r) {	
	int versionMajor = 0;
	int versionMinor = 0;
	int versionMicro = 0;
	getSoftwareVersion(r, &versionMajor, &versionMinor, &versionMicro);

	AppRcfgSupportedFlags flags = 0;
	const type_t type = getType(r);
	switch (type) {
		case HOMESERVER:
		{						
			// Vanaf V66.05.15
			if ((versionMajor >= 0x66 && versionMinor == 0x05 && versionMicro >= 0x15) || 
	            (versionMajor >= 0x66 && versionMinor > 0x05))
			{
				flags += APPRCFG_SUPPORTED_CFG_BINDINGS_WRITE;
				flags += APPRCFG_SUPPORTED_CFG_BINDINGS_ERASE;
			}

			// Vanaf V66.06.01
			if ((versionMajor >= 0x66 && versionMinor == 0x06 && versionMicro >= 0x01) ||
				(versionMajor >= 0x66 && versionMinor > 0x06)) 
			{
				flags += APPRCFG_SUPPORTED_CFG_NAMES_WRITE;
			}

			flags += APPRCFG_SUPPORTED_CFG_BACKUP_UPLOAD;
		} break;
		case TOUCHSCREEN:
		{
			// Vanaf V66.02.29
			if ((versionMajor >= 0x66 && versionMinor == 0x02 && versionMicro >= 0x29) ||
				(versionMajor >= 0x66 && versionMinor > 0x02)) 
			{
				flags += APPRCFG_SUPPORTED_CFG_BACKUP_UPLOAD;
			}
		} break;
	}
	return flags;
}

bool AppRemoteCfgServerInfo_Utils::supportedCfgBindingsWrite(const AppRcfgSupportedFlags& flags) {
	return (APPRCFG_SUPPORTED_CFG_BINDINGS_WRITE == (flags & APPRCFG_SUPPORTED_CFG_BINDINGS_WRITE));
}
bool AppRemoteCfgServerInfo_Utils::supportedCfgBindingsErase(const AppRcfgSupportedFlags& flags) {
	return (APPRCFG_SUPPORTED_CFG_BINDINGS_ERASE == (flags & APPRCFG_SUPPORTED_CFG_BINDINGS_ERASE));
}
bool AppRemoteCfgServerInfo_Utils::supportedCfgBindingsRead(const AppRcfgSupportedFlags& flags) {
	return (APPRCFG_SUPPORTED_CFG_BINDINGS_READ == (flags & APPRCFG_SUPPORTED_CFG_BINDINGS_READ));
}
bool AppRemoteCfgServerInfo_Utils::supportedCfgUploadBackup(const AppRcfgSupportedFlags& flags) {
	return (APPRCFG_SUPPORTED_CFG_BACKUP_UPLOAD == (flags & APPRCFG_SUPPORTED_CFG_BACKUP_UPLOAD));
}
bool AppRemoteCfgServerInfo_Utils::supportedCfgNamesWrite(const AppRcfgSupportedFlags& flags) {
	return (APPRCFG_SUPPORTED_CFG_NAMES_WRITE == (flags & APPRCFG_SUPPORTED_CFG_NAMES_WRITE));
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/