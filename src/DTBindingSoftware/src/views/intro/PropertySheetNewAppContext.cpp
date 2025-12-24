#include "stdafx.h"
#include "BindingSoftware.h"					// Nodig voor de resource ID's.
#include "PropertySheetNewAppContext.h"
#include "app/appudpcfgserver/appudpservercfg.h"
#include "iappdlgtasks.h"			// Nodig voor CPropertyPageNewAppContext_Util
/*****************************************************/
using namespace DUOTECNO;
/*****************************************************/


/*****************************************************/
CPropertyPageNewAppContext::CPropertyPageNewAppContext(PropertyPageNewAppMode_t mode,IAppDlgStartDialog_Host& refHost) : 
	m_mode(mode), m_refHost(refHost), m_projectType(PROJECT_TYPE_EXISTING), m_projectLoaded(FALSE), m_projectName(""), 
	m_modeConnect(MODE_CONNECT_OFFLINE), m_fConnectLocal(TRUE), m_Password(), m_stateConnected(STATE_DISCONNECTED), 
	m_pProxyUDPDevice(NULL), m_flags(0)
{ 
	m_hostAddrSettings[0].m_hostAddress = CString("");
	m_hostAddrSettings[0].m_ipPort = -1;

	m_hostAddrSettings[1].m_hostAddress = CString("");
	m_hostAddrSettings[1].m_ipPort = -1;
}

CPropertyPageNewAppContext::~CPropertyPageNewAppContext() { 
	dealocate(&m_pProxyUDPDevice);
}

IAppDlgStartDialog_Host& CPropertyPageNewAppContext::getHost() {
	return m_refHost;
}

// Opvragen van de proxy - oppassen deze kan 0 zijn...
AppUDPDevice* CPropertyPageNewAppContext::getUDPDevice() {
	return m_pProxyUDPDevice;
}

const AppUDPDevice* CPropertyPageNewAppContext::getUDPDevice() const {
	return m_pProxyUDPDevice;
}

// Instellen van de proxy - indien NULL dan wordt de huidige proxy verwijderd.
// Maakt een copy van een UDP Device...
void CPropertyPageNewAppContext::setUDPDevice(AppUDPDevice* pDevice) {

	dealocate(&m_pProxyUDPDevice);

	if (NULL == pDevice) {		
		return;
	}

	m_pProxyUDPDevice = new AppUDPDevice(*pDevice);
	ASSERT(NULL != m_pProxyUDPDevice);
}

// Wizard
void CPropertyPageNewAppContext::setFlagSearchAgain(BOOL value) {
	if (value) {
		m_flags |= FLAG_WIZARD_SEARCH_AGAIN;
	}
	else{
		m_flags &= ~FLAG_WIZARD_SEARCH_AGAIN;
	}
}
BOOL CPropertyPageNewAppContext::getFlagSearchAgain() const {
	return (FLAG_WIZARD_SEARCH_AGAIN == (m_flags & FLAG_WIZARD_SEARCH_AGAIN));
}

void CPropertyPageNewAppContext::dealocate(AppUDPDevice** ppDevice) {
	if (NULL != *ppDevice) {
		delete *ppDevice;
		*ppDevice = NULL;
	}
}
/*****************************************************/


void CPropertyPageNewAppContext_Util::updatePassword(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;
	}
	CString password;
	if (refSrc.getServerPassword(&password)) {
		pDest->setPassword(password);			
	}	
}

void CPropertyPageNewAppContext_Util::updateLocalIPsettings(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;
	}
	CString hostAddress;
	int port = -1;
	if (refSrc.getServerInfoLocal(&hostAddress,&port)) {
		pDest->setLocalHostAddress(hostAddress);
		pDest->setLocalIPPort(port);
	}	
}

void CPropertyPageNewAppContext_Util::updateRemoteIPsettings(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;
	}

	CString hostAddress;
	int port = -1;
	if (refSrc.getServerInfoRemote(&hostAddress,&port)) {
		pDest->setRemoteHostAddress(hostAddress);
		pDest->setRemoteIPPort(port);
	}	
}

void CPropertyPageNewAppContext_Util::updatePreferedTCPIPMethod(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;
	}

	pDest->setTcpIpConnectLocal(refSrc.getPreferedServerLocal());
}

void CPropertyPageNewAppContext_Util::updateDefaultConnectMode(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;
	}

	const IAppDlgStartDialog_Host::ConnectMode_t mode = refSrc.getDefaultConnectMode();
	switch(mode) {
		case IAppDlgStartDialog_Host::OFFLINE: {
			pDest->setModeConnect(CPropertyPageNewAppContext::MODE_CONNECT_OFFLINE);							   
		} break;
		case IAppDlgStartDialog_Host::USB: {
			pDest->setModeConnect(CPropertyPageNewAppContext::MODE_CONNECT_USB);								   
		} break;
		case IAppDlgStartDialog_Host::TCP: {
			pDest->setModeConnect(CPropertyPageNewAppContext::MODE_CONNECT_TCPIP);									   
		} break;
	}
}