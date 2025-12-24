// PropertyPageNewAppPage7.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage7.h"

#include "app/appudpcfgserver/appudpservercfg.h"
#include "iappdlgtasks.h"				// de worker.


#include "utils/mfc/controls_util.h"
using namespace DUOTECNO::MFC_HELPER;							// Helper functies - ophalen van de data van de controls.

// CPropertyPageNewAppPage7 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage7, CPropertyPage)

CPropertyPageNewAppPage7::CPropertyPageNewAppPage7(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage7::IDD, refCtx),
	  m_state(STATE_INIT), m_msgInfoBox()  { }

CPropertyPageNewAppPage7::~CPropertyPageNewAppPage7() { }

void CPropertyPageNewAppPage7::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editDeviceName);
	DDX_Control(pDX, IDC_EDIT_IPADDRESS, m_editIPAddress);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editIPPort);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_STATIC_INFO_FOOTER, m_staticFooter);
}


BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage7, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CPropertyPageNewAppPage7::OnUpdateControls)
	ON_BN_CLICKED(IDC_CHECK_CONNECT, &CPropertyPageNewAppPage7::OnBnClickedCheckConnect)
END_MESSAGE_MAP()


// CPropertyPageNewAppPage7 message handlers
BOOL CPropertyPageNewAppPage7::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	m_msgInfoBox.Empty();
	m_state = STATE_IDLE;
	PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

// Moeten controleren of de tcp/ip module nog een factory address heeft.
//
// NOTE-ASSERT: Wizard button NEXT wordt enkel weergegeven wanneer de node het address 0xFE heeft anders 
// wordt de FINISH knop getoond.
// 
// NOTE_STOREDATA: Dit gebeurd normaalin de basis klasse.
// Omdat we deze functie overschrijven moeten we deze manueel aanroepen.
// StoreData() zorgt ervoor dat de tcp/ip gegegevens opgeslagen worden.
//
LRESULT CPropertyPageNewAppPage7::OnWizardNext() {
	ASSERT(this->getHost().getServerNodeAddressFactory());	// NOTE-ASSERT
	if (this->getHost().getServerNodeAddressFactory()) {	
		this->StoreData();	// NOTE_STOREDATA
		return getPageResourceID(DIALOG_CONFIG_CANBUS_DEVICE);
	}
	return this->CPropertyPageNewApp::OnWizardNext();
}

// Opvangen van de communicatie status.
void CPropertyPageNewAppPage7::UpdateCommunicationStatus() {
	if (!this->isActivePage()) {
		return;
	}
	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	if (this->getHost().isConnected()) {					
		if (STATE_CONNECTED != m_state) {
			if (this->getHost().getServerNodeAddressFactory()) {
				m_msgInfoBox = CString("The TCP/IP device holds a factory init address.\r\n"
									   "The next page will help you to install the TCP/IP device in the CAN-bus network.");
			}
		}
		refCtx.setStateConnected(CPropertyPageNewAppContext::STATE_CONNECTED_TCPIP);
		refCtx.setTcpIpConnectLocal(TRUE);
		m_state = STATE_CONNECTED;
	}
	else {
		refCtx.setStateConnected(CPropertyPageNewAppContext::STATE_DISCONNECTED);
		m_state = STATE_IDLE;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

// Moeten hier de data doorgeven aan de context.
// Wanneer het project opgeslagen wordt dan worden deze gegevens gebruikt als communicatie settings.
void CPropertyPageNewAppPage7::StoreData( ) {

	CPropertyPageNewAppContext& refCtx = this->getCtx();	

	// Password
	{
		CString password;
		if (FALSE == CControls_Utils::getPassword(m_editPassword,&password)) {
			password = CString();		
		}
		refCtx.setPassword(password);
	}

	// Local ip port
	{		
		int port = 0;
		if (!CControls_Utils::getNwkIPPort(m_editIPPort,&port)) {
			port = 0;
		}
		refCtx.setLocalIPPort(port);	
	}

	// Local hostname
	{
		CString hostName;
		if (!CControls_Utils::getNwkHostAddress(m_editIPAddress,&hostName)) {
			hostName = CString();
		}
		refCtx.setLocalHostAddress(hostName);
	}

	// Locale LAN connectie.
	refCtx.setTcpIpConnectLocal(TRUE);
}

void CPropertyPageNewAppPage7::showProperties(AppUDPDevice* pDevice) {

	CString szDeviceName;
	CString szIpAddress;
	CString szPort;
	CString szPassword;
	if (NULL != pDevice) {
		const int port = pDevice->getAppHomeServerCfg().getIPPort();		
		szDeviceName = pDevice->getName();
		szIpAddress = pDevice->getIpAddress();		
		szPassword = pDevice->getAppHomeServerCfg().getPassword();		
		szPort.Format("%d",port);			
		ASSERT(!szPassword.IsEmpty());		// Mogen niet leeg zijn + poort moet geldig zijn.
		ASSERT(!szIpAddress.IsEmpty());		// Mogen niet leeg zijn + poort moet geldig zijn.
		ASSERT(port>0);						// Mogen niet leeg zijn + poort moet geldig zijn.
	}
	m_editDeviceName.SetWindowText(szDeviceName);
	m_editIPAddress.SetWindowText(szIpAddress);
	m_editIPPort.SetWindowText(szPort);
	m_editPassword.SetWindowText(szPassword);
}

LRESULT CPropertyPageNewAppPage7::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	CString msgInfo;
	CString msgInfoFooter;
	DWORD dwButtons = PSWIZB_BACK;
	BOOL enableBtnConnect = FALSE;
	BOOL stateBtnConnected = FALSE;
	AppUDPDevice* const pDevice = this->getCtx().getUDPDevice();
	switch(m_state) {
		case STATE_INIT: {
			 ASSERT(0);
		} break;
		case STATE_IDLE: {
			 msgInfo = "Press connect to connect to the TCP/IP device";		
			 msgInfoFooter = "Press 'Connect' to connect or 'Back' to adjust the settings";
			 enableBtnConnect = TRUE;
			 showProperties(pDevice);										  
		} break;
		case STATE_CONNECT_BUSY: {
			 msgInfo = "Trying to connect to the TCP/IP device. Please wait.";
			 enableBtnConnect = FALSE;							  
		} break;
		case STATE_CONNECTED: {
			if (this->getHost().getServerNodeAddressFactory()) {
				msgInfo = "Connected to the TCP/IP device. The device holds a factory node address.";
				msgInfoFooter = "Press 'Next' to configure the device in the CAN-bus network.";
				dwButtons = PSWIZB_BACK;
				dwButtons |= PSWIZB_NEXT;
				enableBtnConnect = TRUE;
				stateBtnConnected = TRUE;
			}
			else {
				msgInfo = "Connected to the TCP/IP device. Press finish to continue";
				msgInfoFooter = "Press 'Finish' to continue.";
				dwButtons = 0;		// Verwijderen BACk-button.
				dwButtons |= PSWIZB_FINISH;
				enableBtnConnect = TRUE;
				stateBtnConnected = TRUE;
			}
		} break;
	}
	m_staticFooter.SetWindowText(msgInfoFooter);
	m_staticInfo.SetWindowText(msgInfo);
	m_btnConnect.EnableWindow(enableBtnConnect);
	m_btnConnect.SetCheck(stateBtnConnected);

	this->setWizardButtons(dwButtons);

	if (!m_msgInfoBox.IsEmpty()) {
		MessageBox(m_msgInfoBox,NULL,MB_OK | MB_ICONASTERISK);
		m_msgInfoBox.Empty();
	}
	return TRUE;
}


bool CPropertyPageNewAppPage7::verifyConnectionSettings(AppUDPDevice& refDevice,
		CString* pStrIPAddress,CString* pStrPassword,int* ipport) 
{
	const CString& hostAddress = refDevice.getIpAddress();
	if (hostAddress.IsEmpty()) {
		ASSERT(!hostAddress.IsEmpty());
		return false;
	}

	const CString& password = refDevice.getAppHomeServerCfg().getPassword();
	if (password.IsEmpty()) {
		ASSERT(!password.IsEmpty());
		return false;
	}

	const int port =  refDevice.getAppHomeServerCfg().getIPPort();
	if (port<=0) {
		ASSERT(port>0);
		return false;	
	}

	*pStrPassword = password;
	*pStrIPAddress = hostAddress;
	*ipport = port;
	return true;
}

void CPropertyPageNewAppPage7::OnBnClickedCheckConnect()
{
	IAppDlgStartDialog_Host& refHostApp = this->getHost();
	if (STATE_CONNECTED == m_state) {						
		refHostApp.setAppDisconnect();
		m_state = STATE_IDLE;
	}
	else {		
		AppUDPDevice* const pDevice = this->getCtx().getUDPDevice();
		ASSERT(NULL != pDevice);
		if (NULL != pDevice) {		
			CString hostAddress;
			CString password;
			int port = -1;
			if (verifyConnectionSettings(*pDevice,&hostAddress,&password,&port)) {
				CString strEmpty;
				refHostApp.setAppTcpConnect(CONNECTION_METHOD_LOCAL, hostAddress, port, password, strEmpty);
				m_state = STATE_CONNECT_BUSY;
			}
			else {
				MessageBox("Cannot connect to the TCP/IP device because not all the settings are valid.",NULL,MB_ICONERROR);
			}
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);
}


