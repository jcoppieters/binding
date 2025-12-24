// DlgChildUDPNwk.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPNwk.h"
#include "iappdlgtasks.h"										// worker.

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device

// CDlgChildUDPNwk dialog

IMPLEMENT_DYNAMIC(CDlgChildUDPNwk, CDlgChildUDPBase)

CDlgChildUDPNwk::CDlgChildUDPNwk(CWnd* pParent,CDlgUDPConfigCtx& refCtx)
	: CDlgChildUDPBase(CDlgChildUDPNwk::IDD, pParent,refCtx), m_state(STATE_INIT), m_fControlStateDHCP(FALSE),
	  m_proxyStateDHCP(FALSE), m_fScanModeFast(FALSE)
{ 
	for (int i=0;i<4;++i) {
		m_proxyIpV4[i] = 0;
		m_proxyNetmask[i] = 0;
		m_proxyGateway[i] = 0;
	}
}

CDlgChildUDPNwk::~CDlgChildUDPNwk() { 
	this->restoreUDPScanModeNormal();
}

void CDlgChildUDPNwk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_IP, m_ipAddressCtrlIPAddress);
	DDX_Control(pDX, IDC_IPADDRESS_NETMASK, m_ipAddressCtrlNetmask);
	DDX_Control(pDX, IDC_IPADDRESS_GATEWAY, m_ipAddressCtrlGateway);
	DDX_Control(pDX, IDC_CHECK_DHCP, m_btnCheckDhcp);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
}


BEGIN_MESSAGE_MAP(CDlgChildUDPNwk, CDlgChildUDPBase)
	ON_BN_CLICKED(IDC_CHECK_DHCP, &CDlgChildUDPNwk::OnBnClickedCheckDhcp)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CDlgChildUDPNwk::OnBnClickedButtonConfigure)
END_MESSAGE_MAP()


// CDlgChildUDPNwk message handlers

void CDlgChildUDPNwk::initData(AppUDPDevice& refDevice) {

	const CString& strIPAddress = refDevice.getIpAddress();
	const CString& strGateway = refDevice.getGateway();
	const CString& strNetmask = refDevice.getNetmask();

	convertIPAddress(m_proxyIpV4,strIPAddress);
	convertIPAddress(m_proxyNetmask,strNetmask);
	convertIPAddress(m_proxyGateway,strGateway);

	m_proxyStateDHCP = (true == refDevice.getDhcpOnOff() ? TRUE : FALSE);

	m_state = STATE_IDLE;
}

void CDlgChildUDPNwk::initStaticControls(AppUDPDevice& refDevice) {
	m_fControlStateDHCP = m_proxyStateDHCP;
	m_btnCheckDhcp.SetCheck(m_fControlStateDHCP);
	m_ipAddressCtrlIPAddress.SetAddress(m_proxyIpV4[0],m_proxyIpV4[1],m_proxyIpV4[2],m_proxyIpV4[3]);
	m_ipAddressCtrlNetmask.SetAddress(m_proxyNetmask[0],m_proxyNetmask[1],m_proxyNetmask[2],m_proxyNetmask[3]);
	m_ipAddressCtrlGateway.SetAddress(m_proxyGateway[0],m_proxyGateway[1],m_proxyGateway[2],m_proxyGateway[3]);
}

void CDlgChildUDPNwk::updateDynamicControls(AppUDPDevice& refDevice) {
	BOOL enableControls = FALSE;
	switch(m_state) {
		case STATE_INIT: {
			enableControls = TRUE;			 
		} break;
		case STATE_IDLE: {
			enableControls = TRUE;	
		} break;
		case STATE_BUSY: {
			enableControls = FALSE;							 
		} break;
	}

	m_ipAddressCtrlIPAddress.EnableWindow(enableControls && !m_fControlStateDHCP);
	m_ipAddressCtrlNetmask.EnableWindow(enableControls && !m_fControlStateDHCP);
	m_ipAddressCtrlGateway.EnableWindow(enableControls && !m_fControlStateDHCP);
	m_btnCheckDhcp.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);
}

void CDlgChildUDPNwk::OnBnClickedCheckDhcp() {
	m_fControlStateDHCP = m_btnCheckDhcp.GetCheck();
	PostMessage(WM_UPDATE_CONTROLS);
}

// Configure button. Via een timer + GUI updaten...
void CDlgChildUDPNwk::OnBnClickedButtonConfigure()
{	
	BYTE ipAddress[4] = { 0,0,0,0};
	BYTE netmask[4] = { 0,0,0,0 };
	BYTE gateway[4] = { 0,0,0,0 };

	ASSERT(STATE_IDLE == m_state);

	// Ophalen van de data...
	if (!m_fControlStateDHCP) {	
		if (4 != m_ipAddressCtrlIPAddress.GetAddress(ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3])) {
			const CString str("IP-Address");
			this->showMsgBoxInvalidFields(str);
			return;
		}
		if (4 != m_ipAddressCtrlNetmask.GetAddress(netmask[0],netmask[1],netmask[2],netmask[3])) {
			const CString str("netmask");
			this->showMsgBoxInvalidFields(str);
			return;
		}
		if (4 != m_ipAddressCtrlGateway.GetAddress(gateway[0],gateway[1],gateway[2],gateway[3])) {
			const CString str("gateway");
			this->showMsgBoxInvalidFields(str);
			return;
		}
	}

	// Updaten van het UDP device...
	{
		AppUDPDevice& refDevice = this->getUdpCfgCtx().getUDPDevice();	
		refDevice.setDHCPOnOff(TRUE == m_fControlStateDHCP);
		if (!m_fControlStateDHCP) {
			CString strIpAddress;
			CString strNetmask;
			CString strGateway;
			strIpAddress.Format("%d.%d.%d.%d",ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3]);
			strNetmask.Format("%d.%d.%d.%d",netmask[0],netmask[1],netmask[2],netmask[3]);
			strGateway.Format("%d.%d.%d.%d",gateway[0],gateway[1],gateway[2],gateway[3]);
			refDevice.setIpAddress(strIpAddress);
			refDevice.setGateway(strGateway);
			refDevice.setNetmask(strNetmask);
		}
	}
	
	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_CONFIG_SEND,UDPCFG_SEND_INTERVAL_MSEC,(TIMERPROC) NULL);

	// proxy waarden updaten.
	if (!m_fControlStateDHCP) {
		for (int i=0;i<4;++i) {
			m_proxyIpV4[i] = ipAddress[i];
			m_proxyNetmask[i] = netmask[i];
			m_proxyGateway[i] = gateway[i];
		}
	}
	m_proxyStateDHCP = m_fControlStateDHCP;

	// Toestand + gui updaten.
	m_state = STATE_BUSY;
	m_retryCounter = 0;
	this->setStatusMsg(ID_MSG_STATUS_CONFIG_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildUDPNwk::dataChanged( ) {

	if (m_fControlStateDHCP != m_proxyStateDHCP) {
		return FALSE;
	}

	BYTE ipv4[4];
	if (4 == m_ipAddressCtrlIPAddress.GetAddress(ipv4[0],ipv4[1],ipv4[2],ipv4[3])) {
		if ((m_proxyIpV4[0] != ipv4[0]) || (m_proxyIpV4[1] != ipv4[1]) ||
			(m_proxyIpV4[2] != ipv4[2]) || (m_proxyIpV4[3] != ipv4[3])) 
		{
			return TRUE;
		}
	}
	if (4 == m_ipAddressCtrlNetmask.GetAddress(ipv4[0],ipv4[1],ipv4[2],ipv4[3])) {
		if ((m_proxyNetmask[0] != ipv4[0]) || (m_proxyNetmask[1] != ipv4[1]) ||
			(m_proxyNetmask[2] != ipv4[2]) || (m_proxyNetmask[3] != ipv4[3])) 
		{
			return TRUE;
		}
	}
	if (4 == m_ipAddressCtrlGateway.GetAddress(ipv4[0],ipv4[1],ipv4[2],ipv4[3])) {
		if ((m_proxyGateway[0] != ipv4[0]) || (m_proxyGateway[1] != ipv4[1]) ||
			(m_proxyGateway[2] != ipv4[2]) || (m_proxyGateway[3] != ipv4[3])) 
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDlgChildUDPNwk::busy() {
	return (STATE_BUSY == m_state);
}

const CString& CDlgChildUDPNwk::getStrDescription( ) const {
	static const CString str("Network settings");
	return str;
}

void CDlgChildUDPNwk::onActive(BOOL flag) {
	if (flag){
		this->setMsgDescription();	
	}
	else {
		this->restoreUDPScanModeNormal();
		this->KillTimer(TIMER_ID_CONFIG_SEND);
		this->KillTimer(TIMER_ID_CONFIGURE_WAIT);
	}
}

bool CDlgChildUDPNwk::sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	enum { VALUE_WAIT_FOR_RESPONSE_MSEC = 5000 };		// OVERRIDE TIME !!!
	return this->sendCfgNetwork(refDevice,pResponseCode, VALUE_WAIT_FOR_RESPONSE_MSEC);
}

void CDlgChildUDPNwk::setUDPScanModeFast( ) {
	this->getUdpCfgCtx().getHost().setUDPScanModeFast();
	m_fScanModeFast = TRUE;
}
void CDlgChildUDPNwk::restoreUDPScanModeNormal( ) {
	if (m_fScanModeFast) {	
		this->getUdpCfgCtx().getHost().setUDPScanModeNormal();
		m_fScanModeFast = FALSE;
	}
}

// NOTE_REMOVE_FROM_LIST
// Na het configuren wordt het device uit de bestaande lijst verwijderd.
// Er wordt na een timeout gecontroleerd of het device opnieuw terug te vinden is in de lijst met geldige 
// netwerk instellingen.
// Het device moet FOUND_COUNTER_THRESSHOLD keren terug gevonden worden in de lijst.
void CDlgChildUDPNwk::onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent) {
	switch(nIDEvent) {
		case TIMER_ID_CONFIG_SEND: {						
			bool retry = false;		
			AppUDPServerCfgWrapper::ErrorResponseCode_t responseCode = AppUDPServerCfgWrapper::RESPONSE_EC_GENERAL;
			if (!sendConfig(refDevice,&responseCode)) {
				retry = true;							
			}
			if (retry && (++m_retryCounter < NUMBER_OF_SEND_RETRIES)) {		
				this->setStatusMsg(ID_MSG_STATUS_CONFIG_RETRY);
				this->SetTimer(TIMER_ID_CONFIG_SEND, 250, (TIMERPROC) NULL);
			}
			else {
				m_intervalCounter = 0;		
				m_retryCounter = 0;
				m_foundCounter = 0;							
				this->getUdpCfgCtx().getHost().removeDevice(refDevice.getMACAddress());	// NOTE_REMOVE_FROM_LIST
				this->setUDPScanModeFast();
				this->setStatusMsg(ID_MSG_STATUS_CONFIG_VERIFY);
				this->SetTimer(TIMER_ID_CONFIGURE_WAIT, TIMER_VALUE_CONFIGURE_WAIT_MSEC, (TIMERPROC)NULL);
			}			
			PostMessage(WM_UPDATE_CONTROLS);								   
		} break;

		case TIMER_ID_CONFIGURE_WAIT: {					
			if (++m_intervalCounter >= TIMER_COUNTER_CONFIGURE) {
				this->restoreUDPScanModeNormal();
				this->setProgressBarError();
				this->setStatusMsg(ID_MSG_STATUS_CONFIG_FAILED);
				m_foundCounter = 0;
				m_intervalCounter = 0;						
				m_state = STATE_IDLE;
			}
			else 
			{
				IAppDlgUDPLanCfg_Host& refHost = this->getUdpCfgCtx().getHost();
				const bool found = refHost.updateNetworkSettings(&refDevice);
				if (found) {
					m_foundCounter++;	
				}
		
				// Het device moet een aantal keren in de lijst (=netwerk) terugevonden worden
				// om terug naar IDLE te gaan.
				if (m_foundCounter >= FOUND_COUNTER_THRESSHOLD) {
					if (STATE_IDLE != m_state) {
						this->restoreUDPScanModeNormal();
						this->setProgressBarDone();	
						this->setStatusMsg(ID_MSG_STATUS_CONFIG_DONE);

						// Oppassen:
						// Updaten van de proxy & gui waarden.
						this->initData(refDevice);
						this->initStaticControls(refDevice);
					}
					m_state = STATE_IDLE;
				}
				else {	
					if (found) {
						refHost.removeDevice(refDevice.getMACAddress());		// NOTE_REMOVE_FROM_LIST
					}
					this->SetTimer(TIMER_ID_CONFIGURE_WAIT, TIMER_VALUE_CONFIGURE_WAIT_MSEC, (TIMERPROC)NULL);
				}																	
			}		
			PostMessage(WM_UPDATE_CONTROLS);
		} break;
	}	
}

