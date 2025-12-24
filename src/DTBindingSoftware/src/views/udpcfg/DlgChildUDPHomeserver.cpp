// DlgChildUDPHomeserver.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPHomeserver.h"

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device
#include "iappdlgtasks.h"

#include "utils/mfc/controls_util.h"
using namespace DUOTECNO::MFC_HELPER;							// Helper functies - ophalen van de data van de controls.

// CDlgChildUDPHomeserver dialog

IMPLEMENT_DYNAMIC(CDlgChildUDPHomeserver, CDlgChildUDPBase)

CDlgChildUDPHomeserver::CDlgChildUDPHomeserver(CWnd* pParent,CDlgUDPConfigCtx& refCtx,Mode_t mode)
	: CDlgChildUDPBase(CDlgChildUDPHomeserver::IDD, pParent,refCtx), m_mode(mode), m_state(STATE_INIT),
	  m_proxyPassword(), m_proxyPort(-1), m_retryCounter(0) { }

CDlgChildUDPHomeserver::~CDlgChildUDPHomeserver() { }

void CDlgChildUDPHomeserver::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IPPORT, m_editIPPort);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_STATIC_IPPORT, m_staticIPPort);
	DDX_Control(pDX, IDC_STATIC_GROUP, m_staticGroup);
}


BEGIN_MESSAGE_MAP(CDlgChildUDPHomeserver, CDlgChildUDPBase)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CDlgChildUDPHomeserver::OnBnClickedButtonConfigure)
END_MESSAGE_MAP()


// CDlgChildUDPHomeserver message handlers

void CDlgChildUDPHomeserver::initData(AppUDPDevice& refDevice) {

	// Parameters ophalen van het UDP device
	AppUDPDeviceHomeServerCfg& refCfg = refDevice.getAppHomeServerCfg();	
	m_proxyPassword = refCfg.getPassword();

	if (MODE_HOMESERVER == m_mode) {
		m_proxyPort = refCfg.getIPPort();
	}
	else {
		m_proxyPort = DEFAULT_IPPORT;
	}
	m_state = STATE_IDLE;
}

void CDlgChildUDPHomeserver::initStaticControls(AppUDPDevice& refDevice) {

	CString strPort;
	strPort.Format("%d",m_proxyPort);
	m_editIPPort.SetWindowText(strPort);

	m_editPassword.SetWindowText(m_proxyPassword);
	m_editPassword.ShowWindow(TRUE);

	switch(m_mode) {
		case MODE_HOMESERVER: {
			m_editIPPort.ShowWindow(TRUE);	
			m_staticIPPort.ShowWindow(TRUE);	
			m_staticGroup.SetWindowText("Homeserver settings");
		} break;
		case MODE_API: {
			m_editIPPort.ShowWindow(FALSE);	
			m_staticIPPort.ShowWindow(FALSE);
			m_staticGroup.SetWindowText("Secured API password");
		} break;
	}
}

void CDlgChildUDPHomeserver::updateDynamicControls(AppUDPDevice& refDevice) {
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
	m_editIPPort.EnableWindow(enableControls);
	m_editPassword.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);
}

// Sedert V16.28: Bij een correct config de gegevens door geven aan de host.
// Deze worden gebruikt door de applicatie om automatisch gegevens aan te vullen en opgeslagen in json export bestand.
void CDlgChildUDPHomeserver::onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent) {
	switch(nIDEvent) {
		case TIMER_ID_CONFIG_SEND: {		
			// Versturen van de config...		
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
				if (++m_retryCounter < UDPCFG_SEND_NR_OF_RETRIES) {
					this->SetTimer(TIMER_ID_CONFIG_SEND, 100, (TIMERPROC)NULL);
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

void CDlgChildUDPHomeserver::OnBnClickedButtonConfigure()
{
	// Ophalen van de properties.
	int port = DEFAULT_IPPORT;		
	if (MODE_HOMESERVER == m_mode) {
		port = getCtrlIPPort();
		if (port < 0) {
			const CString str("IP-Port");
			this->showMsgBoxInvalidFields(str);
			return;
		}
	}

	CString strPassword;
	if (!getCtrlPassword(&strPassword)) {
		const CString str("password");
		const CString strInfo("The password cannot be empty and should not exceed the length of 15 characters.");
		this->showMsgBoxInvalidFields(str,strInfo);
		return;
	}

	// UDP device Updaten van de properties.
	{
		AppUDPDevice& refDevice = this->getUdpCfgCtx().getUDPDevice();
		AppUDPDeviceHomeServerCfg& refCfg = refDevice.getAppHomeServerCfg();
		refCfg.setPassword(strPassword);
		refCfg.setIPPort(port);	
	}

	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_CONFIG_SEND, 100, (TIMERPROC)NULL);

	// De proxy waarden updaten	
	m_proxyPassword = strPassword;
	m_proxyPort = port;

	// De toestand + gui updaten.
	m_state = STATE_BUSY;
	m_retryCounter = 0;
	this->setStatusMsg(ID_MSG_STATUS_CONFIG_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);	
}

int CDlgChildUDPHomeserver::getCtrlIPPort( ) {
	
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


// Sedert V17.05: Spaties vooraan/achteraan verwijderen.
BOOL CDlgChildUDPHomeserver::getCtrlPassword(CString* pStr) {
	CString strPassword;
	if (FALSE == CControls_Utils::getPassword(m_editPassword,&strPassword)) {
		return FALSE;
	}
	if (strPassword.GetLength() >= MAX_LENGTH_PASSWORD) {
		return FALSE;
	}
	*pStr = strPassword;
	return TRUE;
}

BOOL CDlgChildUDPHomeserver::dataChanged( ) {
	BOOL differencesFound = FALSE;

	{
		CString str;
		if (getCtrlPassword(&str)) {
			if (0 != str.Compare(m_proxyPassword)) {
				differencesFound = TRUE;
			}
		}			
	}

	if (MODE_HOMESERVER == m_mode) {
		int port = getCtrlIPPort();
		if ((port >=0) && (port != m_proxyPort)) {
			differencesFound = TRUE;
		}
	}
	return differencesFound;
}

BOOL CDlgChildUDPHomeserver::busy() {
	return (STATE_BUSY == m_state);
}

bool CDlgChildUDPHomeserver::sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendCfgHomeserver(refDevice, pResponseCode);
}

const CString& CDlgChildUDPHomeserver::getStrDescription( ) const {
	static const CString str0("Homeserver settings");
	static const CString str1("Secured API settings");
	return (MODE_HOMESERVER == m_mode ? str0 : str1);
}


void CDlgChildUDPHomeserver::onActive(BOOL flag) {
	if (flag) {	
		this->setMsgDescription();
	}
	else {
		this->KillTimer(TIMER_ID_CONFIG_SEND);
	}
}
