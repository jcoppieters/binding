// DlgChildUDPNTP.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPNTP.h"

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device

namespace {

	// De Tijdzones: UTC-12 tem UTC+14
	typedef enum
	{
		UTC_TIMEZONE_MIN_1		 	= -1,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_2			= -2,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_3			= -3,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_4			= -4,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_5			= -5,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_6			= -6,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_7			= -7,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_8			= -8,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_9			= -9,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_10			= -10,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_11			= -11,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_MIN_12			= -12,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_0				= 0,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_1			= 1,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_2			= 2,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_3			= 3,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_4			= 4,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_5			= 5,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_6			= 6,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_7			= 7,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_8			= 8,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_9			= 9,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_10		= 10,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_11		= 11,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_12		= 12,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_13		= 13,	/*NIET VERANDEREN, CAST NAAR INTEGER*/
		UTC_TIMEZONE_PLUS_14		= 14,	/*NIET VERANDEREN, CAST NAAR INTEGER*/

	} UTC_TimeZones_t;

	typedef enum
	{
		UTC_AREA_TIMEZONE_UNDEFINED		= 0,	/* NIET VERANDEREN */
		UTC_AREA_TIMEZONE_AFRICA		= 1,	/* NIET VERANDEREN */
		UTC_AREA_TIMEZONE_ASIA			= 2,	/* NIET VERANDEREN */
		UTC_AREA_TIMEZONE_EUROPE		= 3,	/* NIET VERANDEREN */
		UTC_AREA_TIMEZONE_NORTH_AMERICA	= 4,	/* NIET VERANDEREN */
		UTC_AREA_TIMEZONE_OCEANIA		= 5,	/* NIET VERANDEREN */
		UTC_AREA_TIMEZONE_SOUTH_AMERICA	= 6,	/* NIET VERANDEREN */

	} UTC_TimeZoneArea;

};

// CDlgChildUDPNTP dialog

IMPLEMENT_DYNAMIC(CDlgChildUDPNTP, CDlgChildUDPBase)

CDlgChildUDPNTP::CDlgChildUDPNTP(CWnd* pParent,CDlgUDPConfigCtx& refCtx)
	: CDlgChildUDPBase(CDlgChildUDPNTP::IDD, pParent,refCtx), m_state(STATE_INIT), m_fControlStateEnable(FALSE),
	  m_fControlStateDST(FALSE), m_proxyArea(UTC_AREA_TIMEZONE_EUROPE), m_proxyZone(UTC_TIMEZONE_PLUS_1), m_proxyEnable(FALSE),
	  m_proxyAutoSummerWinter(TRUE), m_retryCounter(0)			
{
}

CDlgChildUDPNTP::~CDlgChildUDPNTP()
{
}

void CDlgChildUDPNTP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_AREA, m_cmbTimeArea);
	DDX_Control(pDX, IDC_COMBO_ZONE, m_cmbTimeUTCZone);
	DDX_Control(pDX, IDC_CHECK_ENABLE, m_btnCheckEnable);
	DDX_Control(pDX, IDC_CHECK_DST, m_btnCheckAutoSummerWinter);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_STATIC_AREA, m_staticArea);
	DDX_Control(pDX, IDC_STATIC_ZONE, m_staticZone);
}


BEGIN_MESSAGE_MAP(CDlgChildUDPNTP, CDlgChildUDPBase)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CDlgChildUDPNTP::OnBnClickedButtonConfigure)
	ON_BN_CLICKED(IDC_CHECK_ENABLE, &CDlgChildUDPNTP::OnBnClickedCheckEnable)
	ON_BN_CLICKED(IDC_CHECK_DST, &CDlgChildUDPNTP::OnBnClickedCheckDst)
END_MESSAGE_MAP()


// CDlgChildUDPNTP message handlers

void CDlgChildUDPNTP::initData(AppUDPDevice& refDevice) {

	AppUDPDeviceNtpTimeCfg& refCfg = refDevice.getAppNtpTimeCfg();
	m_proxyArea = refCfg.getPoolID();
	m_proxyZone = refCfg.getUTCZone();

	if ((m_proxyArea >= UTC_AREA_TIMEZONE_AFRICA) && (m_proxyArea <= UTC_AREA_TIMEZONE_SOUTH_AMERICA)) {
		m_proxyEnable = TRUE;
	}
	else {
		m_proxyEnable = FALSE;
	}
	m_proxyAutoSummerWinter = (1 == refCfg.getDSTEnable() ? TRUE : FALSE);

	m_fControlStateEnable = m_proxyEnable;	
	m_fControlStateDST = m_proxyAutoSummerWinter;

	m_retryCounter = 0;
	m_state = STATE_IDLE;
}

void CDlgChildUDPNTP::initStaticControls(AppUDPDevice& refDevice) {

	int idx = 0;
	// Init Controls ComboBox: m_cmbTimeArea 
	{
		idx=0;
		idx=ComboBoxAddEntry(&m_cmbTimeArea,idx,"Europe",UTC_AREA_TIMEZONE_EUROPE);
		idx=ComboBoxAddEntry(&m_cmbTimeArea,idx,"Africa",UTC_AREA_TIMEZONE_AFRICA);
		idx=ComboBoxAddEntry(&m_cmbTimeArea,idx,"North America",UTC_AREA_TIMEZONE_NORTH_AMERICA);
		idx=ComboBoxAddEntry(&m_cmbTimeArea,idx,"South America",UTC_AREA_TIMEZONE_SOUTH_AMERICA);
		idx=ComboBoxAddEntry(&m_cmbTimeArea,idx,"Asia",UTC_AREA_TIMEZONE_ASIA);
		idx=ComboBoxAddEntry(&m_cmbTimeArea,idx,"Oceania",UTC_AREA_TIMEZONE_OCEANIA);
	} 

	// Init Controls ComboBox: m_cmbTimeUTCZone 
	{
		idx=0;
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-12",UTC_TIMEZONE_MIN_12);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-11",UTC_TIMEZONE_MIN_11);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-10",UTC_TIMEZONE_MIN_10);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-09",UTC_TIMEZONE_MIN_9);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-08",UTC_TIMEZONE_MIN_8);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-07",UTC_TIMEZONE_MIN_7);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-06",UTC_TIMEZONE_MIN_6);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-05",UTC_TIMEZONE_MIN_5);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-04",UTC_TIMEZONE_MIN_4);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-03",UTC_TIMEZONE_MIN_3);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-02",UTC_TIMEZONE_MIN_2);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC-01",UTC_TIMEZONE_MIN_1);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC: WET:United Kingdom,Ireland",UTC_TIMEZONE_0);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+01: CET:Brussels,Amsterdam,Paris",UTC_TIMEZONE_PLUS_1);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+02: EET:Kalingrad time",UTC_TIMEZONE_PLUS_2);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+03: FET:Russia-Moscow time",UTC_TIMEZONE_PLUS_3);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+04",UTC_TIMEZONE_PLUS_4);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+05",UTC_TIMEZONE_PLUS_5);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+06",UTC_TIMEZONE_PLUS_6);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+07",UTC_TIMEZONE_PLUS_7);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+08",UTC_TIMEZONE_PLUS_8);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+09",UTC_TIMEZONE_PLUS_9);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+10",UTC_TIMEZONE_PLUS_10);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+11",UTC_TIMEZONE_PLUS_11);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+12",UTC_TIMEZONE_PLUS_12);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+13",UTC_TIMEZONE_PLUS_13);
		idx=ComboBoxAddEntry(&m_cmbTimeUTCZone,idx,"UTC+14",UTC_TIMEZONE_PLUS_14);
	}

	// Oppassen: NOTE
	// Wanneer disabled is m_proxyArea UTC_AREA_TIMEZONE_UNDEFINED.
	// Deze entry bestaat niet in de combobox...
	if (m_proxyEnable) {
		updateComboBoxSel(&m_cmbTimeArea, m_proxyArea);
	}
	else {
		updateComboBoxSel(&m_cmbTimeArea, UTC_AREA_TIMEZONE_EUROPE);	// NOTE
	}
	updateComboBoxSel(&m_cmbTimeUTCZone, m_proxyZone);

	// Checkbox
	m_btnCheckEnable.SetCheck(m_proxyEnable);
	m_btnCheckAutoSummerWinter.SetCheck(m_proxyAutoSummerWinter);
}

void CDlgChildUDPNTP::updateDynamicControls(AppUDPDevice& refDevice) {

	BOOL enableControls = FALSE;
	BOOL showControls = FALSE;

	switch(m_state) {
		case STATE_INIT:
		case STATE_IDLE: 
		{
			enableControls = TRUE;
		} break;
		case STATE_BUSY_CFG_NTP:
		case STATE_BUSY_CFG_TIME:
		{
			enableControls = FALSE;
		} break;
	}

	// Enable window.
	m_cmbTimeArea.EnableWindow(enableControls);
	m_cmbTimeUTCZone.EnableWindow(enableControls);
	m_btnCheckEnable.EnableWindow(enableControls);
	m_btnCheckAutoSummerWinter.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);

	// Show Window.
	m_cmbTimeArea.ShowWindow(m_fControlStateEnable);
	m_cmbTimeUTCZone.ShowWindow(m_fControlStateEnable);
	m_btnCheckAutoSummerWinter.ShowWindow(m_fControlStateEnable);
	m_staticArea.ShowWindow(m_fControlStateEnable);
	m_staticZone.ShowWindow(m_fControlStateEnable);
}

void CDlgChildUDPNTP::onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent) {
	switch(nIDEvent) {
		case TIMER_ID_CONFIG_SEND: {				
			switch(m_state) {
				case STATE_BUSY_CFG_NTP: {
					AppUDPServerCfgWrapper::ErrorResponseCode_t responseCode = AppUDPServerCfgWrapper::RESPONSE_EC_GENERAL;
					if (sendConfigNTP(refDevice,&responseCode)) {
						m_retryCounter = 0;
						this->SetTimer(TIMER_ID_CONFIG_SEND, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);
						m_state = STATE_BUSY_CFG_TIME;
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
				} break;
				case STATE_BUSY_CFG_TIME: {					
					AppUDPServerCfgWrapper::ErrorResponseCode_t responseCode = AppUDPServerCfgWrapper::RESPONSE_EC_GENERAL;
					if (sendConfigTime(refDevice,&responseCode)) {
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
				} break;				
			}
			PostMessage(WM_UPDATE_CONTROLS);
		} break;
	}
}

BOOL CDlgChildUDPNTP::dataChanged( ) {

	if (m_fControlStateEnable != m_proxyEnable) {
		return TRUE;
	}

	if (m_fControlStateEnable) {
		if (m_fControlStateDST != m_proxyAutoSummerWinter) {
			return TRUE;
		}
		int tempValueArea = -1;
		if (this->getItemData(m_cmbTimeArea,&tempValueArea)>=0) {
			if (tempValueArea != m_proxyArea) {
				return TRUE;
			}
		}
		int tempValueZone = -1;
		if (this->getItemData(m_cmbTimeUTCZone,&tempValueZone)>=0) {
			if (tempValueZone != m_proxyZone) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CDlgChildUDPNTP::busy() {
	return ((STATE_BUSY_CFG_NTP == m_state) || (STATE_BUSY_CFG_TIME == m_state));
}

void CDlgChildUDPNTP::OnBnClickedButtonConfigure()
{
	int tempValueArea = -1;
	int tempValueZone = -1;

	if (m_fControlStateEnable) {	
		if (this->getItemData(m_cmbTimeArea,&tempValueArea)<0) {
			tempValueArea = -1;
		}	
		if (this->getItemData(m_cmbTimeUTCZone,&tempValueZone)<0) {
			tempValueZone = -1;
		}
		if (tempValueArea < 0) {
			const CString str("area");
			this->showMsgBoxInvalidFields(str);
			return;			
		}
		if (tempValueZone < 0) {
			const CString str("zone");
			this->showMsgBoxInvalidFields(str);
			return;
		}
	}	

	{
		// Updaten van UDP device...
		AppUDPDevice& refDevice = this->getUdpCfgCtx().getUDPDevice();
		AppUDPDeviceNtpTimeCfg& refCfg = refDevice.getAppNtpTimeCfg();

		if (m_fControlStateEnable) {
			refCfg.setPoolID(tempValueArea);
			refCfg.setUTCZone(tempValueZone);
			refCfg.setDSTEnable(m_fControlStateDST);
		}
		else {			
			refCfg.setPoolID(UTC_AREA_TIMEZONE_UNDEFINED);
		}
	}


	// Timer starten om te configuren.
	// wanneer de timer verloopt wordt de config doorgestuurd.
	this->SetTimer(TIMER_ID_CONFIG_SEND, UDPCFG_SEND_INTERVAL_MSEC, (TIMERPROC)NULL);

	
	// Proxy updaten.
	m_proxyEnable = m_fControlStateEnable;
	if (m_fControlStateEnable) {
		m_proxyArea = tempValueArea;
		m_proxyZone = tempValueZone;
		m_proxyAutoSummerWinter = m_fControlStateDST;
	}

	// De toestand + gui updaten.
	m_state = STATE_BUSY_CFG_NTP;
	m_retryCounter = 0;
	this->setStatusMsg(ID_MSG_STATUS_CONFIG_BUSY);
	this->setProgressBarBusy();
	this->PostMessage(WM_UPDATE_CONTROLS);	
}

void CDlgChildUDPNTP::OnBnClickedCheckEnable()
{
	m_fControlStateEnable = m_btnCheckEnable.GetCheck() == 1 ? TRUE : FALSE;
	if (m_fControlStateEnable == TRUE) {
			
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildUDPNTP::OnBnClickedCheckDst()
{
	m_fControlStateDST = m_btnCheckAutoSummerWinter.GetCheck() == 1 ? TRUE : FALSE;
	this->PostMessage(WM_UPDATE_CONTROLS);
}

const CString& CDlgChildUDPNTP::getStrDescription( ) const {
	static const CString str("NTP time and date");
	return str;
}

void CDlgChildUDPNTP::onActive(BOOL flag) {
	if (flag) {
		this->setMsgDescription();
	}
	else {
		this->KillTimer(TIMER_ID_CONFIG_SEND);
	}
}

bool CDlgChildUDPNTP::sendConfigNTP(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendCfgNTP(refDevice, pResponseCode);
}

bool CDlgChildUDPNTP::sendConfigTime(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode) {
	return this->sendCfgTimeDate(refDevice, pResponseCode);
}