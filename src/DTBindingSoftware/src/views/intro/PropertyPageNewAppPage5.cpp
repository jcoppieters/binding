// PropertyPageNewAppPage5.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage5.h"
#include "app/appudpcfgserver/appudpservercfg.h"
#include "app/appudpcfgserver/appudpservercfgwrapper.h"
#include "iappdlgtasks.h"										// de worker.

// CPropertyPageNewAppPage5 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage5, CPropertyPage)

CPropertyPageNewAppPage5::CPropertyPageNewAppPage5(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage5::IDD, refCtx), m_state(STATE_INIT), m_fStateDHCP(FALSE),
	  m_fWaiting(FALSE), m_intervalCounter(0), m_foundCounter(0), m_retryCounter(0), m_strMsgBoxInfo( ), 
	  m_strMsgBoxError( ) { }

CPropertyPageNewAppPage5::~CPropertyPageNewAppPage5() { }

void CPropertyPageNewAppPage5::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_IPADDRESS_IP, m_ipAddressCtrlIPAddress);
	DDX_Control(pDX, IDC_IPADDRESS_NETMASK, m_ipAddressCtrlNetmask);
	DDX_Control(pDX, IDC_IPADDRESS_GATEWAY, m_ipAddressCtrlGateway);
	DDX_Control(pDX, IDC_CHECK_DHCP, m_btnCheckDhcp);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editDeviceName);
	DDX_Control(pDX, IDC_STATIC_INFO_FOOTER, m_staticFooter);
}


BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage5, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CPropertyPageNewAppPage5::OnUpdateControls)
	ON_BN_CLICKED(IDC_CHECK_DHCP, &CPropertyPageNewAppPage5::OnBnClickedCheckDhcp)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CPropertyPageNewAppPage5::OnBnClickedButtonConfigure)
	ON_WM_TIMER()
END_MESSAGE_MAP()


static bool convertIPAddress(int* ipAddrV4, const CString& refAddress) {
	if (refAddress.IsEmpty()) {
		return false;
	}
	int v[4] = { 0,0,0,0 };
	if(4 == sscanf((LPCSTR)refAddress,"%d.%d.%d.%d",&v[0],&v[1],&v[2],&v[3])) {
		ipAddrV4[0] = v[0];
		ipAddrV4[1] = v[1];
		ipAddrV4[2] = v[2];
		ipAddrV4[3] = v[3];
		return true;
	}
	return false;
}

static bool getIpv4(CIPAddressCtrl& refCtrl, CString* pszIpAddress,bool checkValid = true) {
	if (0 == pszIpAddress) {
		return false;
	}
	BYTE v[4] = { 0,0,0,0 };
	if (4 != refCtrl.GetAddress(v[0], v[1], v[2], v[3])) {
		return false;
	}
	if (checkValid) {
		if (((0 == v[0]) && (0 == v[1]) && (0 == v[2]) && (0  == v[3])) ||
			((255 == v[0] && (255 == v[1]) && (255 == v[2]) && (255 == v[3])))) {
			return false;
		}
	}
	pszIpAddress->Format("%d.%d.%d.%d", v[0], v[1], v[2], v[3]);
	return true;
}

// CPropertyPageNewAppPage5 message handlers
BOOL CPropertyPageNewAppPage5::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	// create My progress control.
	CRect rcRectProgressCtrl(20, 50, 320, 65);
	m_myProgressCtrl.SetRange(0, 30);
	m_myProgressCtrl.Create(NULL, NULL, WS_VISIBLE, rcRectProgressCtrl, this, 0);
	m_myProgressCtrl.Clear();

	m_strMsgBoxError.Empty();
	m_strMsgBoxInfo.Empty();
	
	init();

	if (m_fStateDHCP) {
		m_strMsgBoxInfo = CString("The network configuration for the TCP/IP device is configured as DHCP. "
								  "We suggest you configure the device with a fix ip-address instead of DHCP. "
								  "You can change the network settings on this page.");
	}
	//	PostMessage(WM_UPDATE_CONTROLS);		<-- OnSetActive()
	return TRUE;
}

BOOL CPropertyPageNewAppPage5::OnSetActive() 
{
	init( );
	return this->CPropertyPageNewApp::OnSetActive();
}

BOOL CPropertyPageNewAppPage5::OnKillActive() {
	this->KillTimer(TIMER_ID_CONFIGURE_WAIT);
	this->KillTimer(TIMER_ID_CONFIG_SEND);
	return CPropertyPage::OnKillActive();
}

void CPropertyPageNewAppPage5::init() {

	m_fStateDHCP = TRUE;
	m_intervalCounter = 0;
	m_foundCounter = 0;
	m_retryCounter = 0;

	const AppUDPDevice* const pDevice = this->getCtx().getUDPDevice();
	if (NULL != pDevice) {
		m_fStateDHCP = pDevice->getDhcpOnOff();
	}
	if (m_btnCheckDhcp.GetSafeHwnd()) {
		m_btnCheckDhcp.SetCheck(m_fStateDHCP);	
	}
	m_state = STATE_IDLE;
}

void CPropertyPageNewAppPage5::UpdateCommunicationStatus() {
	if (!this->isActivePage()) {
		return;
	}
	PostMessage(WM_UPDATE_CONTROLS);	
}

void CPropertyPageNewAppPage5::StoreData( ) {
	// Moet er hier nog iets gebeuren - NOPE.
}

void CPropertyPageNewAppPage5::showIPSettings(BOOL enabled,const AppUDPDevice* pDevice) {

	int ipaddress[4] = { 0,0,0,0 };
	int netmask[4] = { 0,0,0,0 };
	int gateway[4] = { 0,0,0,0 };

	if (NULL != pDevice) {
		convertIPAddress(ipaddress,pDevice->getIpAddress());
		convertIPAddress(netmask,pDevice->getNetmask());
		convertIPAddress(gateway,pDevice->getGateway());		
	}

	m_ipAddressCtrlIPAddress.SetAddress(ipaddress[0],ipaddress[1],ipaddress[2],ipaddress[3]);
	m_ipAddressCtrlNetmask.SetAddress(netmask[0],netmask[1],netmask[2],netmask[3]);
	m_ipAddressCtrlGateway.SetAddress(gateway[0],gateway[1],gateway[2],gateway[3]);

	m_ipAddressCtrlIPAddress.EnableWindow(enabled);
	m_ipAddressCtrlNetmask.EnableWindow(enabled);
	m_ipAddressCtrlGateway.EnableWindow(enabled);
}

LRESULT CPropertyPageNewAppPage5::OnUpdateControls(WPARAM wParam, LPARAM lParam) {

	BOOL enableIPSettings = FALSE;
	BOOL enableCfgButton = TRUE;
	BOOL enableDHCPButton = FALSE;
	BOOL showProgressControl = FALSE;
	DWORD dwButtons = 0;
	CString msg;
	CString msgInfoFooter;
	
	const AppUDPDevice* const pDevice = this->getCtx().getUDPDevice();
	ASSERT(NULL != pDevice);
	switch(m_state) {	
		case STATE_INIT: {
			enableIPSettings = FALSE;
			enableCfgButton = FALSE;
			enableDHCPButton = FALSE;
			dwButtons += PSWIZB_BACK;
			dwButtons += PSWIZB_NEXT;
		} break;
		case STATE_IDLE: {
			msg = CString("TCP/IP device LAN network configuration.");
			msgInfoFooter = CString("Configure settings or press 'Next' to continue");
			enableIPSettings = (TRUE == m_fStateDHCP ? FALSE : TRUE);
			enableCfgButton = TRUE;
			enableDHCPButton = TRUE;
			{
				CString szDeviceName;
				if (NULL != pDevice) {
					szDeviceName = pDevice->getName();
				}
				m_editDeviceName.SetWindowText(szDeviceName);
			}		
			dwButtons += PSWIZB_BACK;
			dwButtons += PSWIZB_NEXT;
		} break;
		case STATE_CONFIGURE: { 
			msg = CString("TCP/IP device LAN network configuration busy, please wait...");
			if (m_retryCounter > 1) {
				msg += CString("\r\nRetry sending configuration.");
			}
			enableIPSettings = FALSE;	
			enableCfgButton = FALSE;
			enableDHCPButton = FALSE;		
			showProgressControl = TRUE;
		} break;

		case STATE_CONFIGURE_ERROR: {
			msg = "TCP/IP device LAN network configuration busy, failed.\r\n";
			msgInfoFooter = CString("Press 'Back' to search again");
			enableIPSettings = FALSE;									
			enableCfgButton = FALSE;
			enableDHCPButton = FALSE;
			showProgressControl = TRUE;			
			dwButtons += PSWIZB_BACK;			
		} break;
	}

	showIPSettings(enableIPSettings,pDevice);
	m_myProgressCtrl.ShowWindow(showProgressControl);
	m_btnConfigure.EnableWindow(enableCfgButton);
	m_btnCheckDhcp.EnableWindow(enableDHCPButton);
	m_staticInfo.SetWindowText(msg);
	m_staticFooter.SetWindowText(msgInfoFooter);
	this->setWizardButtons(dwButtons);

	if (!m_strMsgBoxError.IsEmpty()) {
		MessageBox(m_strMsgBoxError,NULL,MB_ICONERROR | MB_OK);		
		m_strMsgBoxError.Empty();
	}
	if (!m_strMsgBoxInfo.IsEmpty()) {
		MessageBox(m_strMsgBoxInfo,NULL,MB_ICONINFORMATION | MB_OK);		
		m_strMsgBoxInfo.Empty();
	}
	return TRUE;
}


void CPropertyPageNewAppPage5::OnBnClickedCheckDhcp() {
	m_fStateDHCP = m_btnCheckDhcp.GetCheck();
	PostMessage(WM_UPDATE_CONTROLS);
}

// Oppassen:
// Doen een controle dmv m_fWaiting.
// Indien deze op TRUE hebben we zelf de cursor veranderd naar de WAIT cursor.
// Moeten er voorzorgen dat MFC de cursor niet veranderd anders zijn we mogelijks de 
// originele cursor kwijt.
BOOL CPropertyPageNewAppPage5::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (m_fWaiting)
		return TRUE;

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CPropertyPageNewAppPage5::OnTimer(UINT_PTR nIDEvent) {
	this->KillTimer(nIDEvent);

	switch (nIDEvent) {

		case TIMER_ID_CONFIG_SEND: {
			AppUDPDevice* pDevice = this->getCtx().getUDPDevice();
			ASSERT(NULL != pDevice);
			if (NULL == pDevice) {
				m_myProgressCtrl.SetError();
				m_foundCounter = 0;
				m_intervalCounter = 0;
				this->getCtx().setFlagSearchAgain(TRUE);
				if (STATE_CONFIGURE_ERROR != m_state) {
					m_strMsgBoxError = CString("Failed to configure the TCP/IP device.");
				}
				m_state = STATE_CONFIGURE_ERROR;
			}
			else {		
				bool retry = false;				
				if (!sendConfig(*pDevice)) {
					retry = true;							
				}
				if (retry && (++m_retryCounter < NUMBER_OF_SEND_RETRIES)) {
					m_myProgressCtrl.StepUp();
					this->SetTimer(TIMER_ID_CONFIG_SEND, 250, (TIMERPROC) NULL);
				}
				else {
					m_retryCounter = 0;
					this->SetTimer(TIMER_ID_CONFIGURE_WAIT, TIMER_VALUE_CONFIGURE_WAIT_MSEC, (TIMERPROC)NULL);
				}
			}
			PostMessage(WM_UPDATE_CONTROLS);
		} break;

		case TIMER_ID_CONFIGURE_WAIT: {					
			if (++m_intervalCounter >= TIMER_COUNTER_CONFIGURE) {
				m_myProgressCtrl.SetError();
				m_foundCounter = 0;
				m_intervalCounter = 0;
				this->getCtx().setFlagSearchAgain(TRUE);
				if (STATE_CONFIGURE_ERROR != m_state) {
					m_strMsgBoxError = CString("Failed to configure the TCP/IP device.");
				}
				m_state = STATE_CONFIGURE_ERROR;
			}
			else 
			{
				bool found = false;

				ASSERT(STATE_CONFIGURE == m_state); 
				m_myProgressCtrl.StepUp();			

				// controleren of de aanpassingen doorgevoerd werden...
				IAppDlgStartDialog_Host& refHostApp = this->getHost();					
				AppUDPDevice* const pDevice = this->getCtx().getUDPDevice();
				ASSERT(NULL != pDevice);
								
				if (NULL != pDevice) {
					found = refHostApp.updateNetworkSettings(pDevice);
					if (found) {
						m_foundCounter++;
					}
				}

				// Het device moet een aantal keren in de lijst (=netwerk) terugevonden worden
				// om terug naar IDLE te gaan.
				if (m_foundCounter >= FOUND_COUNTER_THRESSHOLD) {
					if (STATE_IDLE != m_state) {
						m_strMsgBoxInfo = CString("Configuration done.");
					}
					m_state = STATE_IDLE;
				}
				else {		
					if (found) {
						ASSERT(NULL != pDevice);
						refHostApp.removeDevice(pDevice->getMACAddress());
					}
					this->SetTimer(TIMER_ID_CONFIGURE_WAIT, TIMER_VALUE_CONFIGURE_WAIT_MSEC, (TIMERPROC)NULL);
				}																	
			}		
			PostMessage(WM_UPDATE_CONTROLS);
		} break;
	}
}

void CPropertyPageNewAppPage5::OnBnClickedButtonConfigure()
{
	bool fContinue = false;
	AppUDPDevice* pDevice = this->getCtx().getUDPDevice();
	if (NULL == pDevice) {
		ASSERT(NULL != pDevice);
		return;
	}

	if (m_fStateDHCP) {
		pDevice->setDHCPOnOff(true);
		fContinue = true;	
	}
	else {	
		CString szIpAddress;
		CString szNetmask;
		CString szGateway;
	
		bool v[3] = { false, false, false };
		v[0] = getIpv4(m_ipAddressCtrlIPAddress, &szIpAddress);
		v[1] = getIpv4(m_ipAddressCtrlNetmask, &szNetmask);
		v[2] = getIpv4(m_ipAddressCtrlGateway, &szGateway);

		if (!v[0] || !v[1] || !v[2]) {	
			m_strMsgBoxError = CString("Cannot configure.\r\nReason: Not all addresses are valid.");
			fContinue = false;	
		}
		else  {
			// Proxy UDP device updaten met deze instellingen...	
			pDevice->setIpAddress(szIpAddress.GetBuffer(0));
			pDevice->setNetmask(szNetmask.GetBuffer(0));
			pDevice->setGateway(szGateway.GetBuffer(0));
			pDevice->setDHCPOnOff(false);
			fContinue = true;		
		}
	}

	if (fContinue) {			
		// Wissen van de UDP List...
		this->getHost().clearUDPDeviceList();
		m_foundCounter = 0;
		m_retryCounter = 0;
		m_intervalCounter = 0;
		m_myProgressCtrl.Clear();
		m_state = STATE_CONFIGURE;
		// Timer starten om de config door te sturen...
		this->SetTimer(TIMER_ID_CONFIG_SEND, 100, (TIMERPROC) NULL);
	}
	PostMessage(WM_UPDATE_CONTROLS);
}


bool CPropertyPageNewAppPage5::sendConfig(AppUDPDevice& refDevice) {
	enum { VALUE_WAIT_FOR_RESPONSE_MSEC = 5000 };
	ASSERT(FALSE == m_fWaiting);
	HCURSOR hOldCursor = NULL;
	m_fWaiting = TRUE;
	hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));		
	AppUDPServerCfgWrapper worker;
	const bool res = worker.sendNwkConfig(refDevice,VALUE_WAIT_FOR_RESPONSE_MSEC);
	if (!res) {
		// Gebruiken 'm_retryCounter' om opnieuw te versturen wanneer we geen response krijgen.
		// Controle gebeurd in de onTimer( )
	}	
	m_fWaiting = FALSE;
	SetCursor(hOldCursor);
	ASSERT(FALSE == m_fWaiting);		
	return res;
}

