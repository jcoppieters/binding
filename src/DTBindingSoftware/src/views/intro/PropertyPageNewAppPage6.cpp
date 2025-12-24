// PropertyPageNewAppPage6.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage6.h"

#include "app/appudpcfgserver/appudpservercfg.h"
#include "app/appudpcfgserver/appudpservercfgwrapper.h"
#include "iappdlgtasks.h"										// de worker.

// CPropertyPageNewAppPage6 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage6, CPropertyPage)

CPropertyPageNewAppPage6::CPropertyPageNewAppPage6(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage6::IDD, refCtx),
	  m_state(STATE_INIT), m_fWaiting(FALSE), m_retryCounter(FALSE), m_strMsgBoxInfo( ), m_strMsgBoxError() { }

CPropertyPageNewAppPage6::~CPropertyPageNewAppPage6() { }

void CPropertyPageNewAppPage6::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IPPORT, m_editIPPort);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editDeviceName);
	DDX_Control(pDX, IDC_STATIC_INFO_FOOTER, m_staticFooter);
}


BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage6, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CPropertyPageNewAppPage6::OnUpdateControls)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CPropertyPageNewAppPage6::OnBnClickedButtonConfigure)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CPropertyPageNewAppPage6 message handlers
BOOL CPropertyPageNewAppPage6::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	m_strMsgBoxInfo.Empty( );
	m_strMsgBoxError.Empty();
	m_state = STATE_IDLE;
	m_fWaiting = FALSE;
	m_retryCounter = FALSE;
	return TRUE;
}

void CPropertyPageNewAppPage6::UpdateCommunicationStatus() {
	if (!this->isActivePage()) {
		return;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageNewAppPage6::StoreData( ) {
	// Moet er hier nog iets gebeuren - NOPE.
}

void CPropertyPageNewAppPage6::showSettings(BOOL enabled,AppUDPDevice* pDevice) {
	
	if (NULL != pDevice) {		
		const CString szPassword = pDevice->getAppHomeServerCfg().getPassword();
		m_editPassword.SetWindowText(szPassword);

		CString szPort;
		const int port = pDevice->getAppHomeServerCfg().getIPPort();
		szPort.Format("%d",port);
		m_editIPPort.SetWindowText(szPort);		

	}
	m_editIPPort.EnableWindow(enabled);
	m_editPassword.EnableWindow(enabled);
}

LRESULT CPropertyPageNewAppPage6::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	BOOL enableSettings = FALSE;
	BOOL enableConfigureBtn = FALSE;
	DWORD dwButtons = 0;
	CString msgInfo;
	CString msgInfoFooter;
	AppUDPDevice* const pDevice = this->getCtx().getUDPDevice();
	switch(m_state) {	
		case STATE_INIT: {		
			enableSettings = TRUE;		
			enableConfigureBtn = TRUE;
			dwButtons += PSWIZB_BACK;
			dwButtons += PSWIZB_NEXT;
		} break;
		case STATE_IDLE: {		
			msgInfo = CString("TCP/IP configuration protocol settings.\r\n\r\n"
							  "Configure the local TCP/IP port and password. "
							  "The password cannot exceed the length of 15 characters.");			  
			msgInfoFooter = CString("Configure settings or press 'Next' to continue");

			enableSettings = TRUE;
			CString szDeviceName;
			if (NULL != pDevice) {
				szDeviceName = pDevice->getName();
			}
			m_editDeviceName.SetWindowText(szDeviceName);
			
			enableConfigureBtn = TRUE;
			dwButtons += PSWIZB_BACK;
			dwButtons += PSWIZB_NEXT;
		} break;
		case STATE_CONFIGURE: {
			msgInfo = CString("TCP/IP configuration protocol settings busy. Please wait...");
			enableSettings = FALSE;	
			enableConfigureBtn = FALSE;
		} break;

		case STATE_CONFIGURE_DONE: {
			msgInfo = CString("TCP/IP configuration protocol settings done.");	
			msgInfoFooter = CString("Press 'Next' to continue");			
			enableSettings = TRUE;
			enableConfigureBtn = TRUE;
			dwButtons += PSWIZB_BACK;
			dwButtons += PSWIZB_NEXT;
		} break;

		case STATE_CONFIGURE_ERROR: {
			msgInfo = CString("TCP/IP configuration protocol settings failed.");
			msgInfoFooter = CString("Press 'Back' to search again");
			enableSettings = FALSE;	
			enableConfigureBtn = FALSE;
			dwButtons += PSWIZB_BACK;
		} break;
	}

	m_staticInfo.SetWindowText(msgInfo);
	m_staticFooter.SetWindowText(msgInfoFooter);	
	showSettings(enableSettings,pDevice);
	m_btnConfigure.EnableWindow(enableConfigureBtn);
	this->setWizardButtons(dwButtons);

	if (!m_strMsgBoxError.IsEmpty()) {
		MessageBox(m_strMsgBoxError, NULL, MB_ICONERROR);
		m_strMsgBoxError.Empty();			
	}
	if (!m_strMsgBoxInfo.IsEmpty()) {
		MessageBox(m_strMsgBoxInfo, NULL, MB_ICONINFORMATION);
		m_strMsgBoxInfo.Empty();	
	}
	return TRUE;
}


static bool checkPassword(const CString& str) {
	enum { 
		MAX_LENGTH_PASSWORD = 16			// 15 karakters + NULL char.
	};

	if (str.IsEmpty()) {
		return false;
	}
	if (str.GetLength()>=MAX_LENGTH_PASSWORD){
		return false;
	}
	return true;
}


// Sedert V17.05: Controle doen op Upper en lowercase. Gebruik functie CompareNoCase()
static bool checkDefaultPassword(AppUDPDevice* pDevice,const CString& strPassword) {
	if (NULL == pDevice) {
		ASSERT(NULL != pDevice);
		return false;
	}
	const int physicalAddress = pDevice->getPhysicalAddress( );

	CString strDefaultPassword;
	strDefaultPassword.Format("D%06X#",physicalAddress);
	return (0 == strPassword.CompareNoCase(strDefaultPassword));
}

// Controle of het standaard passwoord nog aanwezig is.
bool CPropertyPageNewAppPage6::checkDefaultPassword( ) {
	CString strPassword;
	m_editPassword.GetWindowText(strPassword);
	return ::checkDefaultPassword(this->getCtx().getUDPDevice(),strPassword);
}

void CPropertyPageNewAppPage6::OnBnClickedButtonConfigure()
{
	AppUDPDevice* pDevice = this->getCtx().getUDPDevice();
	if (NULL == pDevice) {
		ASSERT(NULL != pDevice);
		return;
	}

	int ipport = -1;
	CString strPassword;
	CString strErrorMsg;
	if (checkData(&ipport,&strPassword,&strErrorMsg)) {
		// UDP Proxy updaten met deze gegevens.
		pDevice->getAppHomeServerCfg().setIPPort(ipport);
		pDevice->getAppHomeServerCfg().setPassword(strPassword);

		// Versturen van de gegevens + wachten op antwoord.
		m_retryCounter = 0;
		m_state = STATE_CONFIGURE;
		this->SetTimer(TIMER_ID_SENDCONFIG, 1000, (TIMERPROC) NULL);
	}
	else {
		m_strMsgBoxError.Format("Cannot configure the TCP/IP device.\r\nReason:'%s'",strErrorMsg);
		m_state = STATE_IDLE;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

LRESULT CPropertyPageNewAppPage6::OnWizardNext() 
{
	int ipport = -1;
	CString strPassword;
	CString strErrorMsg;
	if (!checkData(&ipport,&strPassword,&strErrorMsg)) {
		CString msg;
		msg.Format("Cannot continue. Reason '%s'",strErrorMsg);
		m_strMsgBoxInfo = msg;					
		PostMessage(WM_UPDATE_CONTROLS);
		return -1;
	}
	return CPropertyPageNewApp::OnWizardNext();
}

bool CPropertyPageNewAppPage6::checkData(int* port,CString* pszPassword,CString* pszErrorMsg) {

	int v = -1;
	CString strPort;
	CString strPassword;

	m_editIPPort.GetWindowText(strPort);
	if (1 != sscanf((LPCSTR)strPort,"%d",&v)) {
		*pszErrorMsg = CString("Empty value for IP-Port.");
		return false;
	}
	if (v<=0) {
		*pszErrorMsg = CString("Wrong value for IP-Port.");
		return false;
	}
	m_editPassword.GetWindowText(strPassword);	

	// Sedert V17.05: Spaties verwijderen uit de instellingen van de paswoorden.
	if (!strPassword.IsEmpty()) {
		strPassword = strPassword.Trim();
	}

	if (false == checkPassword(strPassword)) {
		*pszErrorMsg = CString("Wrong value for password. "
		 					   "The password cannot be empty and should not exceed the length of 15 characters.");
		return false;
	}

	if (::checkDefaultPassword(this->getCtx().getUDPDevice(),strPassword)) {
		*pszErrorMsg = CString("Change the password. Cannot use the default password.");
		return false;
	}
	*port = v;
	*pszPassword = strPassword;
	return true;
}

// Versturen van de gegevens.
// Deze functie wacht op een antwoord van de UDP server (=tcp/ip device).
// Tijdens het wachten veranderd de cursor in een zandloper.
bool CPropertyPageNewAppPage6::sendConfig(AppUDPDevice& refDevice) {
	enum { TIMEOUT_VALUE_RESPONSE_MSEC = 8000 };
	ASSERT(FALSE == m_fWaiting);
	HCURSOR hOldCursor = NULL;
	m_fWaiting = TRUE;
	hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));	
	AppUDPServerCfgWrapper worker;
	const bool res = worker.sendCfgAppHomeSrv(refDevice,TIMEOUT_VALUE_RESPONSE_MSEC);	
	m_fWaiting = FALSE;
	SetCursor(hOldCursor);
	ASSERT(FALSE == m_fWaiting);	
	return res;
}

// Oppassen:
// Doen een controle dmv m_fWaiting.
// Indien deze op TRUE hebben we zelf de cursor veranderd naar de WAIT cursor.
// Moeten er voorzorgen dat MFC de cursor niet veranderd anders zijn we mogelijks de 
// originele cursor kwijt.
BOOL CPropertyPageNewAppPage6::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_fWaiting) 
		return TRUE;

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CPropertyPageNewAppPage6::OnTimer(UINT_PTR nIDEvent) 
{	
	this->KillTimer(nIDEvent);

	if (TIMER_ID_SENDCONFIG == nIDEvent) {	
		AppUDPDevice* pDevice = this->getCtx().getUDPDevice();
		if (NULL == pDevice) {
			m_state = STATE_CONFIGURE_ERROR;			
		}
		else {
			if (sendConfig(*pDevice)) {		
				if (STATE_CONFIGURE_DONE != m_state) {				
					m_strMsgBoxInfo = CString("Configuration done. Press 'Next' to continue.");
				}
				m_state = STATE_CONFIGURE_DONE;
			}	
			else {
				if (++m_retryCounter < NUMBER_OF_RETRIES_SENDCONFIG) {
					this->SetTimer(TIMER_ID_SENDCONFIG, 100, (TIMERPROC) NULL);
				}
				else {
					if (STATE_CONFIGURE_ERROR != m_state) {					
						m_strMsgBoxError = CString("Configuration failed.");						
					}
					m_state = STATE_CONFIGURE_ERROR;
				}
			}
		}
		PostMessage(WM_UPDATE_CONTROLS);
	}
}




