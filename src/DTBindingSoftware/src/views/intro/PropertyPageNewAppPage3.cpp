// PropertyPageNewAppPage3.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage3.h"
#include "iappdlgtasks.h"
#include "utils/mfc/controls_util.h"

#include "app/appState/AppState.h"								// CLOUD SERVER POC

using namespace DUOTECNO::MFC_HELPER;

// CPropertyPageNewAppPage3 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage3, CPropertyPage)

CPropertyPageNewAppPage3::CPropertyPageNewAppPage3(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage3::IDD, refCtx)
	, m_radioLocalRemote(0), m_state(STATE_INIT), m_msgInfoBox() { }

CPropertyPageNewAppPage3::~CPropertyPageNewAppPage3() { }

void CPropertyPageNewAppPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_LOCAL, m_radioLocalRemote);
	DDX_Control(pDX, IDC_IPADDRESS1, m_CtrlLocalIPAddress);
	DDX_Control(pDX, IDC_EDIT_LOCAL_PORT, m_CtrlLocalIPPort);
	DDX_Control(pDX, IDC_EDIT_REMOTE_DNS, m_CtrlRemoteDns);
	DDX_Control(pDX, IDC_EDIT_REMOTE_PORT, m_CtrlRemotePort);
	DDX_Control(pDX, IDC_EDIT_TCPIP_PASSWORD, m_CtrlPassword);
	DDX_Control(pDX, IDC_CHECK_CONNECT, m_CtrlBtnConnect);
	DDX_Control(pDX, IDC_STATIC_FOOTER, m_staticFooter);
}


BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage3, CPropertyPage)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_LOCAL, IDC_RADIO_REMOTE, OnRadioButtons)
	ON_MESSAGE(WM_UPDATE_CONTROLS,&CPropertyPageNewAppPage3::OnUpdateControls)
	ON_BN_CLICKED(IDC_CHECK_CONNECT, &CPropertyPageNewAppPage3::OnBnClickedCheckConnect)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPropertyPageNewAppPage3 message handlers
void CPropertyPageNewAppPage3::UpdateCommunicationStatus() 
{
	if (!this->isActivePage()) {
		return;
	}
	CPropertyPageNewAppContext& refCtx = this->getCtx();
	if ( this->getHost().isConnected()) {			
		// Sedert V16.26: Controle op het node address.
		// Indien de module nog een factory init adres heeft wordt er een pop-up melding gegeven.
		if (STATE_CONNECTED != m_state) {
			if (this->getHost().getServerNodeAddressFactory()) {
				m_msgInfoBox = CString("The TCP/IP device holds a factory init address.\r\n"
									   "The next page will help you to install the TCP/IP device in the CAN-bus network.");
			}
		} 
		refCtx.setStateConnected(CPropertyPageNewAppContext::STATE_CONNECTED_TCPIP);
		refCtx.setTcpIpConnectLocal((0 == m_radioLocalRemote ? TRUE : FALSE));
		m_state = STATE_CONNECTED;
	}
	else {
		refCtx.setStateConnected(CPropertyPageNewAppContext::STATE_DISCONNECTED);
		m_state = STATE_IDLE;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageNewAppPage3::OnTimer(UINT nIDEvent) {
	if (WM_TIMER_UPDATE_BTN_CONNECT == nIDEvent) {
		updateCtrlBtnConnect();
		int interVal = 2000;		
		switch(m_state) {
			case STATE_INIT:
			case STATE_IDLE: {
				interVal = 50;
			} break;
			case STATE_CONNECTED: {
				interVal = 2000;								
			} break;
			case STATE_CONNECT_BUSY: {
				interVal = 2000;									 
			} break;
		}
		SetTimer(WM_TIMER_UPDATE_BTN_CONNECT,interVal,NULL);
	}
}

// Sedert V16.26:
// Indien de tcp/ip server nog een factory address heeft dan naar een volgende pagina gaan 
// om het device te configureren als master / slave node address.
//
// NOTE_STOREDATA: Dit gebeurd normaal in de basis klasse.
// Omdat we deze functie overschrijven moeten we deze manueel aanroepen.
// StoreData() zorgt ervoor dat de tcp/ip gegegevens opgeslagen worden.
//
LRESULT CPropertyPageNewAppPage3::OnWizardNext() {
	if (this->getHost().getServerNodeAddressFactory()) {	
		this->StoreData();	// NOTE_STOREDATA
		return getPageResourceID(DIALOG_CONFIG_CANBUS_DEVICE);
	}
	return this->CPropertyPageNewApp::OnWizardNext();
}

BOOL CPropertyPageNewAppPage3::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	m_state = STATE_IDLE;
	m_msgInfoBox.Empty();

	{		
		CPropertyPageNewAppContext& refCtx = this->getCtx();	
		m_radioLocalRemote = (refCtx.getTcpIpConnectLocal() == TRUE ? 0 : 1);

		{ 
			// Lokaal ip-address...
			int values[4] = { -1, -1, -1, -1 };
			CString strIpAddress = refCtx.getLocalHostAddress();
			if (!strIpAddress.IsEmpty()) {
				if (4 != sscanf(strIpAddress.GetBuffer(0),"%d.%d.%d.%d",&values[0],&values[1],&values[2],&values[3])) {
					values[0] = -1;
					values[1] = -1;
					values[2] = -1;
					values[3] = -1;
				}
			}
			
			if ((values[0]>=0 && values[0]<=255) && 
				(values[1]>=0 && values[1]<=255) &&
				(values[2]>=0 && values[2]<=255) &&
				(values[3]>=0 && values[3]<=255)) 
			{
				m_CtrlLocalIPAddress.SetAddress(values[0],values[1],values[2],values[3]);	
			}
			else {
				m_CtrlLocalIPAddress.ClearAddress();
			}
		}

		{
			// Lokale IP-Port
			int port = refCtx.getLocalIPPort();
			if (port>0 && port<=65536) {
				CString strPort;
				strPort.Format("%d",port);
				m_CtrlLocalIPPort.SetWindowText(strPort);
			}
		
		}
	
		{
			// Remote dns.
			CString strRemoteAddress = refCtx.getRemoteHostAddress();
			if (strRemoteAddress.IsEmpty()) {
				// Sedert V16.28:
				// Indien er nog geen remote DNS Address ingevuld is dan zoeken in de lijst (van andere nodes.)
				// Meestal wordt er 1 DynDns client geconfigureerd voor het ganse project.
				IAppDlgStartDialog_Host& refHostApp = this->getHost();
				const int nodeAddress = refHostApp.getServerNodeAddress();
				if (!refHostApp.searchRemoteDns(nodeAddress,&strRemoteAddress)) {
					strRemoteAddress.Empty();
				}			
			}
			m_CtrlRemoteDns.SetWindowText(strRemoteAddress);
		}

		{
			// Remote IP-Port
			int port = refCtx.getRemoteIPPort();
			if (port>0 && port<=65536) {
				CString strPort;
				strPort.Format("%d",port);
				m_CtrlRemotePort.SetWindowText(strPort);
			}
		}

		{
			// Password..
			CString strPassword = refCtx.getPassword();
			m_CtrlPassword.SetWindowText(strPassword);
		
		}
		UpdateData(FALSE);
	}

	SetTimer(WM_TIMER_UPDATE_BTN_CONNECT,1000,NULL);
	PostMessage(WM_UPDATE_CONTROLS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// Updaten van de Connect button.
// - Bezig met connecteren altijd disabled.
// - Geconnecteerd altijd enabled.
// - anders: Controle of alle nodige velden ingevuld zijn.
void CPropertyPageNewAppPage3::updateCtrlBtnConnect( ) {

	if (!this->isActivePage()) {
		return;
	}

	bool enableConnectBtn = FALSE;
	if (STATE_CONNECT_BUSY == m_state) {
		enableConnectBtn = FALSE;		
	}
	else if (STATE_CONNECTED == m_state) {
		enableConnectBtn = TRUE;	
	}
	else { 
		CString password;			
		if (CControls_Utils::getPassword(m_CtrlPassword,&password)) {
			int port = -1;
			CString address;
			if (m_radioLocalRemote == 0) {
				if (CControls_Utils::getNwkIPPort(m_CtrlLocalIPPort,&port) && CControls_Utils::getNwkHostAddress(m_CtrlLocalIPAddress,&address)) {	
					if (port>=0 && port<=USHRT_MAX) {
						enableConnectBtn = TRUE;	
					}
				}
			}
			else {
				if (CControls_Utils::getNwkIPPort(m_CtrlRemotePort,&port) && CControls_Utils::getNwkHostAddress(m_CtrlRemoteDns,&address)) {
					if (port>=0 && port<=USHRT_MAX) {
						enableConnectBtn = TRUE;	
					}
				}
			}
		}				
	}
	m_CtrlBtnConnect.EnableWindow(enableConnectBtn);
	m_CtrlBtnConnect.SetCheck(STATE_CONNECTED == m_state ? 1 : 0);
}

LONG CPropertyPageNewAppPage3::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	BOOL enableBtnNext = FALSE;
	BOOL enableBtnFinish = FALSE;
	BOOL enableBtnPrevious = TRUE;
	CString msgInfoFooter;

	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	const BOOL connectLocal = refCtx.getTcpIpConnectLocal();
	const CPropertyPageNewAppContext::StateConnected_t state = refCtx.getStateConnected();

	BOOL fConnected = FALSE;
	BOOL fBusyConnecting = FALSE;
	switch(m_state) {
		case STATE_INIT:
		case STATE_IDLE:  {
			msgInfoFooter = CString("Press the connect button to connect to the device");
			fConnected = FALSE;
			fBusyConnecting = FALSE;
			enableBtnPrevious = TRUE;
		} break;
		case STATE_CONNECT_BUSY: {
			msgInfoFooter = CString("Connecting to the device. Please wait...");
			fBusyConnecting = TRUE;		
			enableBtnPrevious = FALSE;
		} break;
		case STATE_CONNECTED: {

			fConnected = TRUE;		
			enableBtnPrevious = FALSE;

			// Sedert V16.26:
			// Indien de tcp/ip server een 0xFE adres heeft dan een extra pagina tonen om deze te 
			// configureren.
			if (this->getHost().getServerNodeAddressFactory()) {
				msgInfoFooter = CString("Connected. Press 'Next' to configure the CAN-bus device.");
				enableBtnNext = TRUE;	
			}
			else {
				msgInfoFooter = CString("Connected to the TCP/IP device. Press finish to continue");
				enableBtnFinish = TRUE;
			}
		} break;
	}

	m_staticFooter.SetWindowText(msgInfoFooter);
		
	m_CtrlLocalIPAddress.EnableWindow((connectLocal) && (!fConnected) && (!fBusyConnecting));
	m_CtrlLocalIPPort.EnableWindow((connectLocal) && (!fConnected) && (!fBusyConnecting));
	m_CtrlRemoteDns.EnableWindow((!connectLocal) && (!fConnected) && (!fBusyConnecting));
	m_CtrlRemotePort.EnableWindow((!connectLocal) && (!fConnected) && (!fBusyConnecting));	
	m_CtrlPassword.EnableWindow((!fConnected) && (!fBusyConnecting));
	
	updateCtrlBtnConnect();

	if (fConnected || fBusyConnecting) {
		if (m_radioLocalRemote == 0) {
			this->enableWindow(this,IDC_RADIO_LOCAL,TRUE);
			this->enableWindow(this,IDC_RADIO_REMOTE,FALSE);
		}
		else {
			this->enableWindow(this,IDC_RADIO_LOCAL,FALSE);
			this->enableWindow(this,IDC_RADIO_REMOTE,TRUE);
		}		
	}
	else {
		this->enableWindow(this,IDC_RADIO_LOCAL,TRUE);
		this->enableWindow(this,IDC_RADIO_REMOTE,TRUE);
	}
	
	DWORD dwButtons = 0;
	if (enableBtnNext) {
		dwButtons += PSWIZB_NEXT;
	}
	if (enableBtnPrevious) {
		dwButtons += PSWIZB_BACK;
	}
	if (enableBtnFinish) {
		dwButtons += PSWIZB_FINISH;
	}
	this->setWizardButtons(dwButtons);

	if (!m_msgInfoBox.IsEmpty()) {
		MessageBox(m_msgInfoBox,NULL,MB_OK | MB_ICONASTERISK);
		m_msgInfoBox.Empty();
	}
	return TRUE;
}
	
void CPropertyPageNewAppPage3::OnRadioButtons(UINT nID) {

	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	switch(nID) {
		case IDC_RADIO_LOCAL: {
			refCtx.setTcpIpConnectLocal(TRUE);			  
		} break; 
			
		case IDC_RADIO_REMOTE: {
			refCtx.setTcpIpConnectLocal(FALSE);				   
		} break;
	}
	UpdateData(TRUE);
	PostMessage(WM_UPDATE_CONTROLS);
}


BOOL CPropertyPageNewAppPage3::helper_getSettings(ConnectionMethod_t method, CString* hostAddress, int* port,
	CString* password, CString* cloudServerLoginToken)  {
	switch(method) {
		case CONNECTION_METHOD_LOCAL: {
			if (FALSE == CControls_Utils::getNwkIPPort(m_CtrlLocalIPPort, port)) {
				return FALSE;
			}			
			if (FALSE == CControls_Utils::getNwkHostAddress(m_CtrlLocalIPAddress, hostAddress)) {
				return FALSE;
			}				
			if (FALSE == CControls_Utils::getPassword(m_CtrlPassword, password)) {
				return FALSE;
			}
			cloudServerLoginToken->Empty();
		} break;

		case CONNECTION_METHOD_REMOTE_DIRECT: {
			if (FALSE == CControls_Utils::getNwkIPPort(m_CtrlRemotePort, port)) {
				return FALSE;
			}
			if (FALSE == CControls_Utils::getNwkHostAddress(m_CtrlRemoteDns, hostAddress)) {
				return FALSE;
			}
			if (FALSE == CControls_Utils::getPassword(m_CtrlPassword, password)) {
				return FALSE;
			}
			cloudServerLoginToken->Empty();
		} break;	

		case CONNECTION_METHOD_REMOTE_CLOUD: {
			int tempPort = 0;			
			if (FALSE == CControls_Utils::getNwkIPPort(m_CtrlRemotePort, &tempPort)) {
				return FALSE;
			}
			CString tempHostAddress;
			if (FALSE == CControls_Utils::getNwkHostAddress(m_CtrlRemoteDns, &tempHostAddress)) {
				return FALSE;
			}
			if (FALSE == CControls_Utils::getPassword(m_CtrlPassword, password)) {
				return FALSE;
			}

			ASSERT(!tempHostAddress.IsEmpty());
			ASSERT(tempPort>=0);

			cloudServerLoginToken->Format("[%s:%i]", tempHostAddress, tempPort);
			*hostAddress =  CLOUD_SERVER_ADDRESS;
			*port = CLOUD_SERVER_TCPPORT;
		} break;
	}
	return TRUE;
}


void CPropertyPageNewAppPage3::OnBnClickedCheckConnect() {
	IAppDlgStartDialog_Host& refHostApp = this->getHost();

	m_CtrlBtnConnect.SetCheck(FALSE);

	if (STATE_CONNECTED == m_state) {						
		refHostApp.setAppDisconnect();
		m_state = STATE_IDLE;
	}
	else {
		CString hostAddress;
		CString password;
		CString cloudServerLoginToken;
		int port = -1;
		
		ConnectionMethod_t method = CONNECTION_METHOD_LOCAL;
		if (0 == m_radioLocalRemote) {
			method = CONNECTION_METHOD_LOCAL;
		}
		else {
			// CLOUD SERVER POC
			if (CAppState::getInstance().getEnableUseRemoteTCPCloudServer()) {
				method = CONNECTION_METHOD_REMOTE_CLOUD;
			}
			else {
				method = CONNECTION_METHOD_REMOTE_DIRECT;
			}
		}
		const BOOL tryConnect = this->helper_getSettings(method, &hostAddress, &port, &password, &cloudServerLoginToken);
		if (tryConnect) {
			m_state = STATE_CONNECT_BUSY;				
			refHostApp.setAppTcpConnect(method, hostAddress, port, password, cloudServerLoginToken);
		}
		else {
			AfxMessageBox("Cannot connect because some fields are not valid.");
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);
}


// Moeten hier de data doorgeven aan de context.
// Wanneer het project opgeslagen wordt dan worden deze gegevens gebruikt als communicatie settings.
void CPropertyPageNewAppPage3::StoreData()
{
	CPropertyPageNewAppContext& refCtx = this->getCtx();	

	// Password
	{
		CString password;
		if (FALSE == CControls_Utils::getPassword(m_CtrlPassword,&password)) {
			password = CString();		
		}
		refCtx.setPassword(password);
	}

	// Local ip port
	{		
		int port = 0;
		if (!CControls_Utils::getNwkIPPort(m_CtrlLocalIPPort,&port)) {
			port = 0;
		}
		refCtx.setLocalIPPort(port);	
	}

	// Local hostname
	{
		CString hostName;
		if (!CControls_Utils::getNwkHostAddress(m_CtrlLocalIPAddress,&hostName)) {
			hostName = CString();
		}
		refCtx.setLocalHostAddress(hostName);
	}

	// remote ip port
	{
		int port = 0;
		if (!CControls_Utils::getNwkIPPort(m_CtrlRemotePort,&port)) {
			port = 0;
		}
		refCtx.setRemoteIPPort(port);
	}

	// remote hostname
	{
		CString hostName;
		if (!CControls_Utils::getNwkHostAddress(m_CtrlRemoteDns,&hostName)) {
			hostName = CString();
		}
		refCtx.setRemoteHostAddress(hostName);
	}

	// Login local/remote.
	{		
		refCtx.setTcpIpConnectLocal(0 == m_radioLocalRemote ? TRUE : FALSE);
	}
}