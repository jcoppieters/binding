// DlgChildUDPDynDns.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPDynDns.h"

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device
#include "iappdlgtasks.h"

// CDlgChildUDPDynDns dialog

IMPLEMENT_DYNAMIC(CDlgChildUDPDynDns, CDlgChildUDPBase)

CDlgChildUDPDynDns::CDlgChildUDPDynDns(CWnd* pParent, CDlgUDPConfigCtx& refCtx)
	: CDlgChildUDPBase(CDlgChildUDPDynDns::IDD, pParent, refCtx), m_state(STATE_INIT), m_proxyProvider(PROVIDER_NONE),
	m_proxyHostName(), m_proxyUserName(), m_proxyPassword(), m_proxyStatus(), m_retryCounter(0) { }

CDlgChildUDPDynDns::~CDlgChildUDPDynDns() { }

void CDlgChildUDPDynDns::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PROVIDER, m_cmbDDNSProvider);
	DDX_Control(pDX, IDC_EDIT_DYNDNS_HOSTNAME, m_editHostname);
	DDX_Control(pDX, IDC_EDIT_DYNDNS_USERNAME, m_editUsername);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_BUTTON_REQUEST, m_btnRequest);
	DDX_Control(pDX, IDC_EDIT_DYNDNS_STATUS, m_editStatus);
	DDX_Control(pDX, IDC_STATIC_HOSTNAME, m_staticHostName);
	DDX_Control(pDX, IDC_STATIC_USERNAME, m_staticUserName);
	DDX_Control(pDX, IDC_STATIC_PASSWORD, m_staticPassword);
}


BEGIN_MESSAGE_MAP(CDlgChildUDPDynDns, CDlgChildUDPBase)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CDlgChildUDPDynDns::OnBnClickedButtonConfigure)
	ON_BN_CLICKED(IDC_BUTTON_REQUEST, &CDlgChildUDPDynDns::OnBnClickedButtonRequest)
	ON_CBN_SELCHANGE(IDC_COMBO_PROVIDER, &CDlgChildUDPDynDns::OnCbnSelchangeComboDdnsProvider)
END_MESSAGE_MAP()

void CDlgChildUDPDynDns::initData(AppUDPDevice& refDevice) {
	
	// Parameters ophalen van het UDP device.
	AppUDPDeviceDDNSCfg& refCfg = refDevice.getAppDDNSCfg();
	const AppUDPDeviceDDNSCfg::Provider_t provider = refCfg.getProvider();
	if ((AppUDPDeviceDDNSCfg::PROVIDER_NONE == provider) || (AppUDPDeviceDDNSCfg::PROVIDER_NOIP == provider)) {
		m_proxyProvider = provider;
	}
	else {
		m_proxyProvider = AppUDPDeviceDDNSCfg::PROVIDER_NONE;
	}

	m_proxyHostName = refCfg.getHostname();
	m_proxyUserName = refCfg.getUsername();
	m_proxyPassword = refCfg.getPassword();
	AppUDPDeviceDDNSCfg::convertToString(&m_proxyStatus, refCfg.getStatus());
	m_retryCounter = 0;
	m_state = STATE_IDLE;
}
	
// CDlgChildUDPDynDns message handlers
void CDlgChildUDPDynDns::initStaticControls(AppUDPDevice& refDevice) {

	int idx = 0;
	idx=ComboBoxAddEntry(&m_cmbDDNSProvider,idx,"DDNS disabled",PROVIDER_NONE);
	idx=ComboBoxAddEntry(&m_cmbDDNSProvider,idx,"DDNS Provider NO-IP",PROVIDER_NOIP);
	updateComboBoxSel(&m_cmbDDNSProvider,m_proxyProvider);

	m_editHostname.SetWindowText(m_proxyHostName);
	m_editUsername.SetWindowText(m_proxyUserName);
	m_editPassword.SetWindowText(m_proxyPassword);
	m_editStatus.SetWindowText(m_proxyStatus);
}

void CDlgChildUDPDynDns::updateDynamicControls(AppUDPDevice& refDevice) {

	BOOL enableControls = FALSE;
	BOOL showControls = FALSE;

	int provider = -1;
	getItemData(m_cmbDDNSProvider,&provider);
	if (PROVIDER_NOIP == provider) {
		showControls = TRUE;
	}

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

	m_cmbDDNSProvider.EnableWindow(enableControls);
	m_editHostname.EnableWindow(enableControls);
	m_editUsername.EnableWindow(enableControls);
	m_editPassword.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);
	m_btnRequest.EnableWindow(enableControls);

	// visible.
	m_editHostname.ShowWindow(showControls);
	m_editUsername.ShowWindow(showControls);
	m_editPassword.ShowWindow(showControls);
	m_btnRequest.ShowWindow(showControls);
	m_staticHostName.ShowWindow(showControls);
	m_staticUserName.ShowWindow(showControls);
	m_staticPassword.ShowWindow(showControls);
	
	// Status text updaten.
	CString strStatus;
	AppUDPDeviceDDNSCfg::convertToString(&strStatus, refDevice.getAppDDNSCfg().getStatus());
	m_editStatus.SetWindowText(strStatus);
}

// Sedert V16.28: Bij een correct config de gegevens door geven aan de host.
// Deze worden gebruikt door de applicatie om automatisch gegevens aan te vullen en opgeslagen in json export bestand.
void CDlgChildUDPDynDns::onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent) {
	switch(nIDEvent) {
		case TIMER_ID_CONFIG_SEND: {			
			AppUDPServerCfgWrapper::ErrorResponseCode_t responseCode = AppUDPServerCfgWrapper::RESPONSE_EC_GENERAL;
			if (sendConfig(refDevice,&responseCode)) {
				this->getUdpCfgCtx().getHost().updateNodeHomeserverService(refDevice);	// Sedert V16.28
				this->setStatusMsg(ID_MSG_STATUS_CONFIG_DONE);
				this->setProgressBarDone();
				if (AppUDPDevice::IsHardwareNxp(refDevice)) {
					this->notifyUserHardwareReset();
				}
				m_state = STATE_IDLE;
			}
			else {		
				// Sedert V16.31
				// Controleren van de resultcode.			
				// Wanneer de lengte van de gegevens te lang is kan het bericht niet meer in een UDP data 
				// bericht. Vroeger werd er geen foutmelding gegeven.
				if (AppUDPServerCfgWrapper::RESPONSE_EC_FORMATTER == responseCode) {
					this->setStatusMsg(ID_MSG_STATUS_CONFIG_FORMATTER);
					this->setProgressBarError();
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

BOOL CDlgChildUDPDynDns::dataChanged( ) {

	int provider = -1;
	getItemData(m_cmbDDNSProvider,&provider);
	if (m_proxyProvider != provider) {
		return TRUE;
	}

	const CString strHostName = getString(m_editHostname);
	if (!strHostName.IsEmpty() && 0 != strHostName.Compare(m_proxyHostName)) {
		return TRUE;
	}

	const CString strUserName = getString(m_editUsername);
	if (!strUserName.IsEmpty() && 0 != strUserName.Compare(m_proxyUserName)) {
		return TRUE;
	}

	const CString strPassword = getString(m_editPassword);
	if (!strPassword.IsEmpty() && 0 != strPassword.Compare(m_proxyPassword)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CDlgChildUDPDynDns::busy() {
	return ((STATE_BUSY_SEND == m_state) || (STATE_BUSY_REQUEST == m_state));
}

void CDlgChildUDPDynDns::OnBnClickedButtonConfigure()
{
	// Ophalen van de data...
	int provider = -1;
	getItemData(m_cmbDDNSProvider,&provider);
	CString strHostName = getString(m_editHostname);
	CString strUserName = getString(m_editUsername);
	CString strPassword = getString(m_editPassword);

	if (PROVIDER_NOIP == provider) {		
		if (strHostName.IsEmpty()) {
			const CString str("hostname");
			this->showMsgBoxInvalidFields(str);
			return;
		}
		if (strUserName.IsEmpty()) {
			const CString str("username");
			this->showMsgBoxInvalidFields(str);
			return;
		}
		if (strPassword.IsEmpty()) {
			const CString str("password");
			this->showMsgBoxInvalidFields(str);
			return;
		}	

		// Sedert V16.31: Extra controle op de lengte van de velden.
		const CString strInfoDescription("The length of the data input is too long.");
		if (strHostName.GetLength() >= 64) {
			const CString str("hostname");			
			this->showMsgBoxInvalidFields(str, strInfoDescription);
			return;
		}
		if (strUserName.GetLength() >= 32) {
			const CString str("username");		
			this->showMsgBoxInvalidFields(str, strInfoDescription);
			return;
		}
		if (strPassword.GetLength() >= 32) {
			const CString str("password");		
			this->showMsgBoxInvalidFields(str, strInfoDescription);
			return;
		}		

		// V17.05: Spaties verwijderen.
		strHostName = strHostName.Trim();
		strUserName = strUserName.Trim();
		strPassword = strPassword.Trim();
	}

	// Updaten van UDP device...
	{
		AppUDPDevice& refDevice = this->getUdpCfgCtx().getUDPDevice();
		AppUDPDeviceDDNSCfg& refCfg = refDevice.getAppDDNSCfg();
		refCfg.setProvider((AppUDPDeviceDDNSCfg::Provider_t) provider);
		refCfg.setStateOnOff(PROVIDER_NOIP == provider ? 1 : 0);
		if (PROVIDER_NOIP == provider) {
			refCfg.setHostName(strHostName);
			refCfg.setPassword(strPassword);
			refCfg.setUserName(strUserName);
		}
	}

	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_CONFIG_SEND, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);

	// proxy waarden updaten.
	m_proxyProvider = provider;
	if (PROVIDER_NOIP == provider) {
		m_proxyHostName = strHostName;
		m_proxyUserName = strUserName;
		m_proxyPassword = strPassword;
	}

	// De toestand + gui updaten.
	m_state = STATE_BUSY_SEND;
	m_retryCounter = 0;
	this->setStatusMsg(ID_MSG_STATUS_CONFIG_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);	
}

void CDlgChildUDPDynDns::OnBnClickedButtonRequest()
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

void CDlgChildUDPDynDns::OnCbnSelchangeComboDdnsProvider() {
	this->PostMessage(WM_UPDATE_CONTROLS);
}

bool CDlgChildUDPDynDns::sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendCfgDynDnsClient(refDevice, pResponseCode);
}

bool CDlgChildUDPDynDns::sendRequestStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendRequestDynDnsStatus(refDevice, pResponseCode);
}

const CString& CDlgChildUDPDynDns::getStrDescription( ) const {
	static const CString str("Dynamic DNS client");
	return str;
}

void CDlgChildUDPDynDns::onActive(BOOL flag) {
	if (flag) {
		this->setMsgDescription();
	}
	else {
		this->KillTimer(TIMER_ID_CONFIG_SEND);
		this->KillTimer(TIMER_ID_STATUS_REQUEST);
	}
}
