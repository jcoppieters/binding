// DlgUDPSystemMode.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPSystemMode.h"

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device
// CDlgUDPSystemMode dialog

IMPLEMENT_DYNAMIC(CDlgChildUDPSystemMode, CDlgChildUDPBase)

CDlgChildUDPSystemMode::CDlgChildUDPSystemMode(CWnd* pParent,CDlgUDPConfigCtx& refCtx)
	: CDlgChildUDPBase(CDlgChildUDPSystemMode::IDD, pParent,refCtx),
	  m_state(STATE_IDLE),m_proxySystemMode(AppUDPDeviceSystemModeCfg::EXTENDED), m_retryCounter(0) { }

CDlgChildUDPSystemMode::~CDlgChildUDPSystemMode()
{
}

void CDlgChildUDPSystemMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MODE, m_cmbSystemMode);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
}

BEGIN_MESSAGE_MAP(CDlgChildUDPSystemMode, CDlgChildUDPBase)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CDlgChildUDPSystemMode::OnBnClickedButtonConfigure)
	ON_CBN_SELCHANGE(IDC_COMBO_MODE, &CDlgChildUDPSystemMode::OnCbnSelchangeComboSystemMode)
END_MESSAGE_MAP()


// CDlgUDPSystemMode message handlers

void CDlgChildUDPSystemMode::initData(AppUDPDevice& refDevice) {
	AppUDPDeviceSystemModeCfg& ref = refDevice.getAppSystemMode();
	m_proxySystemMode = ref.getSystemMode();
	m_retryCounter = 0;
	m_state = STATE_IDLE;
}
void CDlgChildUDPSystemMode::initStaticControls(AppUDPDevice& refDevice) {
	int idx = 0;	
	idx=ComboBoxAddEntry(&m_cmbSystemMode,idx,"Basic system",AppUDPDeviceSystemModeCfg::BASIC);
	idx=ComboBoxAddEntry(&m_cmbSystemMode,idx,"Extended system",AppUDPDeviceSystemModeCfg::EXTENDED);
	updateComboBoxSel(&m_cmbSystemMode,m_proxySystemMode);
}

void CDlgChildUDPSystemMode::updateDynamicControls(AppUDPDevice& refDevice) {
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
	m_cmbSystemMode.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);
}

void CDlgChildUDPSystemMode::onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent) {
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

BOOL CDlgChildUDPSystemMode::dataChanged( ) {
	int systemMode = -1;
	if (getItemData(m_cmbSystemMode,&systemMode)>=0) {
		if (m_proxySystemMode != systemMode) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDlgChildUDPSystemMode::busy() {
	return (STATE_BUSY == m_state);
}

void CDlgChildUDPSystemMode::OnBnClickedButtonConfigure()
{
	int systemMode = -1;
	if (getItemData(m_cmbSystemMode,&systemMode)<0) {		
		systemMode = -1;		
	}

	if (systemMode < 0) {
		const CString str("system mode");
		this->showMsgBoxInvalidFields(str);
		return;
	}

	// Updaten van UDP device...
	{
		AppUDPDevice& refDevice = this->getUdpCfgCtx().getUDPDevice();
		AppUDPDeviceSystemModeCfg& ref = refDevice.getAppSystemMode();
		ASSERT((systemMode == AppUDPDeviceSystemModeCfg::BASIC) || (systemMode == AppUDPDeviceSystemModeCfg::EXTENDED));
		ref.setSystemMode((AppUDPDeviceSystemModeCfg::SystemMode_t) systemMode);	
	}

	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_CONFIG_SEND, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);

	// proxy waarden updaten.
	m_proxySystemMode = systemMode;

	// De toestand + gui updaten.
	m_state = STATE_BUSY;
	m_retryCounter = 0;
	this->setStatusMsg(ID_MSG_STATUS_CONFIG_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);	
}

void CDlgChildUDPSystemMode::OnCbnSelchangeComboSystemMode() {
	this->PostMessage(WM_UPDATE_CONTROLS);
}

const CString& CDlgChildUDPSystemMode::getStrDescription( ) const {
	static const CString str("CAN-bus system mode");
	return str;
}

void CDlgChildUDPSystemMode::onActive(BOOL flag) {
	if (flag) {
		this->setMsgDescription();
	}
	else {
		this->KillTimer(TIMER_ID_CONFIG_SEND);
	}
}

bool CDlgChildUDPSystemMode::sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendCfgSystemMode(refDevice, pResponseCode);
}