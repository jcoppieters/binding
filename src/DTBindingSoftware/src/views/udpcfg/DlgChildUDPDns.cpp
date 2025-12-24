// DlgChildUDPDns.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPDns.h"

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device

// CDlgChildUDPDns dialog

IMPLEMENT_DYNAMIC(CDlgChildUDPDns, CDlgChildUDPBase)

CDlgChildUDPDns::CDlgChildUDPDns(CWnd* pParent,CDlgUDPConfigCtx& refCtx)
	: CDlgChildUDPBase(CDlgChildUDPDns::IDD, pParent,refCtx), m_state(STATE_INIT), m_retryCounter(0)
{
	for (int i=0;i<4;++i) {
		m_proxyIpaddress[i] = 0;
	}
}

CDlgChildUDPDns::~CDlgChildUDPDns()
{
}

void CDlgChildUDPDns::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_IPADDRESS_IP, m_ctrlIpAddress);
}


BEGIN_MESSAGE_MAP(CDlgChildUDPDns, CDlgChildUDPBase)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CDlgChildUDPDns::OnBnClickedButtonConfigure)
END_MESSAGE_MAP()


// CDlgChildUDPDns message handlers
void CDlgChildUDPDns::initData(AppUDPDevice& refDevice) {
	const CString& str = refDevice.getAppDnsServerCfg().getDNSServerAddress();
	convertIPAddress(m_proxyIpaddress, str);
	m_retryCounter = 0;
	m_state = STATE_IDLE;
}
	
void CDlgChildUDPDns::initStaticControls(AppUDPDevice& refDevice) {
	m_ctrlIpAddress.SetAddress(m_proxyIpaddress[0],m_proxyIpaddress[1],m_proxyIpaddress[2],m_proxyIpaddress[3]);
}

void CDlgChildUDPDns::updateDynamicControls(AppUDPDevice& refDevice)
{
	BOOL enableControls = FALSE;
	switch(m_state) {
		case STATE_INIT:
		case STATE_IDLE: 
		{
			enableControls = TRUE;
		} break;
		case STATE_BUSY:
		{
			enableControls = FALSE;
		} break;
	}
	m_ctrlIpAddress.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);
}
	
void CDlgChildUDPDns::onTimerExpiry(AppUDPDevice& refDevice,UINT_PTR nIDEvent) {
	switch(nIDEvent) {
		case TIMER_ID_CONFIG_SEND: {	
			AppUDPServerCfgWrapper::ErrorResponseCode_t responseCode = AppUDPServerCfgWrapper::RESPONSE_EC_GENERAL;
			if (sendConfig(refDevice,&responseCode)) {
				this->setStatusMsg(ID_MSG_STATUS_CONFIG_DONE);
				this->setProgressBarDone();
				if (AppUDPDevice::IsHardwareNxp(refDevice)) {
					this->notifyUserHardwareReset();
				}
				m_state = STATE_IDLE;
			}
			else {			
				if (++m_retryCounter < UDPCFG_SEND_NR_OF_RETRIES) {
					this->SetTimer(TIMER_ID_CONFIG_SEND, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);
					this->setStatusMsg(ID_MSG_STATUS_CONFIG_RETRY);
				}
				else {					
					this->setStatusMsg(ID_MSG_STATUS_CONFIG_FAILED);
					this->setProgressBarError();
					m_state = STATE_IDLE;
				}
			}
			PostMessage(WM_UPDATE_CONTROLS);
		} break;
	}
}

bool CDlgChildUDPDns::sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendCfgDNSProvider(refDevice, pResponseCode);
}

BOOL CDlgChildUDPDns::dataChanged( ) {
	BYTE ipv4[4];
	if (4 == m_ctrlIpAddress.GetAddress(ipv4[0],ipv4[1],ipv4[2],ipv4[3])) {
		if ((m_proxyIpaddress[0] != ipv4[0]) || (m_proxyIpaddress[1] != ipv4[1]) ||
			(m_proxyIpaddress[2] != ipv4[2]) || (m_proxyIpaddress[3] != ipv4[3])) 
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDlgChildUDPDns::busy() {
	return (STATE_BUSY == m_state);
}

void CDlgChildUDPDns::OnBnClickedButtonConfigure()
{
	BYTE ipAddress[4] = { 0,0,0,0};
	if (4 != m_ctrlIpAddress.GetAddress(ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3])) {
		const CString str("IP-address");
		this->showMsgBoxInvalidFields(str);
		return;
	}

	// Updaten van het UDP device...
	{
		AppUDPDevice& refDevice = this->getUdpCfgCtx().getUDPDevice();
		AppUDPDeviceDnsServerCfg& refCfg = refDevice.getAppDnsServerCfg();
		CString strIpAddress;
		strIpAddress.Format("%d.%d.%d.%d", ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
		refCfg.setDNSServerAddress(strIpAddress);
	}

	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_CONFIG_SEND, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);

	// proxy waarden updaten.
	for (int i=0;i<4;++i) {
		m_proxyIpaddress[i] = ipAddress[i];
	}

	// De toestand + gui updaten.
	m_retryCounter = 0;
	m_state = STATE_BUSY;
	this->setStatusMsg(ID_MSG_STATUS_CONFIG_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);	
}


const CString& CDlgChildUDPDns::getStrDescription( ) const {
	static const CString str("DNS provider");
	return str;
}

void CDlgChildUDPDns::onActive(BOOL flag) {
	if (flag) {
		this->setMsgDescription();
	}
	else {
		this->KillTimer(TIMER_ID_CONFIG_SEND);		
	}
}

