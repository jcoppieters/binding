// DlgChildUDPTcpIp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPTcpIp.h"

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device

// CDlgChildUDPTcpIp dialog

IMPLEMENT_DYNAMIC(CDlgChildUDPTcpIp, CDlgChildUDPBase)

CDlgChildUDPTcpIp::CDlgChildUDPTcpIp(CWnd* pParent,CDlgUDPConfigCtx& refCtx)
	: CDlgChildUDPBase(CDlgChildUDPTcpIp::IDD, pParent, refCtx), 
	  m_state(STATE_INIT), m_proxyPort(-1), m_proxyStatus(0), m_retryCounter(0)
{
	for (int i=0;i<4;++i) {
		m_proxyIpaddress[i] = 0;
	}
}

CDlgChildUDPTcpIp::~CDlgChildUDPTcpIp()
{
}

void CDlgChildUDPTcpIp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_IP, m_ctrlIPAddress);
	DDX_Control(pDX, IDC_EDIT_IPPORT, m_editIPPort);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_EDIT_DYNDNS_STATUS, m_editStatus);
	DDX_Control(pDX, IDC_BUTTON_REQUEST, m_btnRequest);
}


BEGIN_MESSAGE_MAP(CDlgChildUDPTcpIp, CDlgChildUDPBase)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CDlgChildUDPTcpIp::OnBnClickedButtonConfigure)
	ON_BN_CLICKED(IDC_BUTTON_REQUEST, &CDlgChildUDPTcpIp::OnBnClickedButtonRequest)
END_MESSAGE_MAP()


// CDlgChildUDPTcpIp message handlers

void CDlgChildUDPTcpIp::initData(AppUDPDevice& refDevice) {	
	AppUDPDeviceTCPIPClientCfg& ref = refDevice.getAppTCPIPClientCfg();
	const CString& serverAddress = ref.getServerAddress();
	convertIPAddress(m_proxyIpaddress,serverAddress);	
	m_proxyPort = ref.getServerPort();
	m_proxyStatus = ref.getStatusConnected();
	m_retryCounter = 0;
	m_state = STATE_IDLE;
}

void CDlgChildUDPTcpIp::initStaticControls(AppUDPDevice& refDevice)
{
	CString strPort;
	if (m_proxyPort >= 0) {		
		strPort.Format("%d",m_proxyPort);		
	}		

	m_editIPPort.SetWindowText(strPort);
	m_ctrlIPAddress.SetAddress(m_proxyIpaddress[0],m_proxyIpaddress[1],m_proxyIpaddress[2],m_proxyIpaddress[3]);
	m_editStatus.SetWindowText((m_proxyStatus == 0 ? "Disconnected" : "Connected"));
}

void CDlgChildUDPTcpIp::updateDynamicControls(AppUDPDevice& refDevice)
{
	BOOL enableControls = FALSE;
	switch(m_state) {
		case STATE_INIT:
		case STATE_IDLE: {
			enableControls = TRUE;
		} break;
		case STATE_BUSY_SEND:
		case STATE_BUSY_REQUEST: {
			enableControls = FALSE;
		} break;
	}

	m_editStatus.SetWindowText((m_proxyStatus == 0 ? "Disconnected" : "Connected"));

	m_ctrlIPAddress.EnableWindow(enableControls);
	m_editIPPort.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);
	m_btnRequest.EnableWindow(enableControls);
}

void CDlgChildUDPTcpIp::onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent) {
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

		case TIMER_ID_STATUS_REQUEST: {			
			AppUDPServerCfgWrapper::ErrorResponseCode_t responseCode = AppUDPServerCfgWrapper::RESPONSE_EC_GENERAL;
			if (sendRequestStatus(refDevice,&responseCode)) {
				this->setStatusMsg(ID_MSG_STATUS_REQUEST_DONE);
				this->setProgressBarDone();
				m_state = STATE_IDLE;
			}
			else {			
				if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {
					this->SetTimer(TIMER_ID_STATUS_REQUEST, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);
					this->setStatusMsg(ID_MSG_STATUS_REQUEST_RETRY);
				}
				else {
					this->setStatusMsg(ID_MSG_STATUS_REQUEST_FAILED);
					this->setProgressBarError();
					m_state = STATE_IDLE;
				}
			}
			PostMessage(WM_UPDATE_CONTROLS);										  
		} break;
	}
}

BOOL CDlgChildUDPTcpIp::dataChanged( ) {
	const int port = getCtrlIPPort();
	if (port != m_proxyPort) {
		return TRUE;
	}

	BYTE ipv4[4];
	if (4 == m_ctrlIPAddress.GetAddress(ipv4[0],ipv4[1],ipv4[2],ipv4[3])) {
		if ((m_proxyIpaddress[0] != ipv4[0]) || (m_proxyIpaddress[1] != ipv4[1]) ||
			(m_proxyIpaddress[2] != ipv4[2]) || (m_proxyIpaddress[3] != ipv4[3])) 
		{
			return TRUE;
		}
	}
	return FALSE;
}
	
BOOL CDlgChildUDPTcpIp::busy() {
	return (STATE_BUSY_SEND == m_state);
}

void CDlgChildUDPTcpIp::OnBnClickedButtonConfigure()
{
	const int port = getCtrlIPPort();
	if (port < 0) {
		const CString str("IP-Port");
		this->showMsgBoxInvalidFields(str);
		return;
	}

	BYTE ipAddress[4] = { 0,0,0,0 };
	if (4 != m_ctrlIPAddress.GetAddress(ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3])) {
		const CString str("IP-Address");
		this->showMsgBoxInvalidFields(str);
		return;
	}

	{
		// Updaten van UDP device...
		AppUDPDevice& refDevice = this->getUdpCfgCtx().getUDPDevice();
		AppUDPDeviceTCPIPClientCfg& refCfg = refDevice.getAppTCPIPClientCfg();
		CString serverAddress;
		serverAddress.Format("%d.%d.%d.%d",ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3]);
		refCfg.setServerAddress(serverAddress);
		refCfg.setServerPort(port);
	}


	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_CONFIG_SEND, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);

	// proxy waarden updaten.
	for (int i=0;i<4;++i) {
		m_proxyIpaddress[i] = ipAddress[i];
	}
	m_proxyPort = port;

	// De toestand + gui updaten.
	m_state = STATE_BUSY_SEND;
	m_retryCounter = 0;
	this->setStatusMsg(ID_MSG_STATUS_CONFIG_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);	
}

void CDlgChildUDPTcpIp::OnBnClickedButtonRequest()
{
	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_STATUS_REQUEST, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);

	// De toestand + gui updaten.
	m_state = STATE_BUSY_REQUEST;
	m_retryCounter = 0;
	this->setStatusMsg(ID_MSG_STATUS_REQUEST_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);	
}

int CDlgChildUDPTcpIp::getCtrlIPPort( ) {
	
	CString str;
	m_editIPPort.GetWindowText(str);
	if (str.IsEmpty()) {
		return -1;	
	}	

	int temp = -1;
	if (1 == sscanf((LPCSTR)str,"%d",&temp)) {
		if (temp >= 0) {
			return temp;		
		}	
	}
	return -1;
}

const CString& CDlgChildUDPTcpIp::getStrDescription( ) const {
	static const CString str("TCP/IP client");
	return str;
}

void CDlgChildUDPTcpIp::onActive(BOOL flag) {
	if (flag) {
		this->setMsgDescription();
	}
	else {
		this->KillTimer(TIMER_ID_CONFIG_SEND);
		this->KillTimer(TIMER_ID_STATUS_REQUEST);
	}
}

bool CDlgChildUDPTcpIp::sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendCfgTCPIPClient(refDevice, pResponseCode);
}

bool CDlgChildUDPTcpIp::sendRequestStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendRequestTCPIPClientStatus(refDevice, pResponseCode);
}