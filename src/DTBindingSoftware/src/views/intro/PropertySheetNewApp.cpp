// PropertySheetNewApp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertySheetNewApp.h"
#include "PropertyPageNewAppPage1.h"
#include "PropertyPageNewAppPage2.h"
#include "PropertyPageNewAppPage3.h"
#include "PropertyPageNewAppPage4.h"
#include "PropertyPageNewAppPage5.h"
#include "PropertyPageNewAppPage6.h"
#include "PropertyPageNewAppPage7.h"
#include "PropertyPageNewAppPage8.h"
#include "MyWindowMessages.h"
#include "iappdlgtasks.h"
// CPropertySheetNewApp

/////////////////////////////////////////////////////////////////////////////
CPropertySheetNewApp* CPropertySheetNewApp::m_pInstance = 0;
/////////////////////////////////////////////////////////////////////////////

CPropertySheetNewApp* CPropertySheetNewApp::get() {
	return m_pInstance;
}
	
CPropertySheetNewApp* CPropertySheetNewApp::create(CWnd* pParent,PropertyPageNewAppMode_t mode,IAppDlgStartDialog_Host& refHost) {

	if (0 != m_pInstance) {
		delete m_pInstance;
		m_pInstance = 0;
	}
	
	CString szTitle;
	if (DIALOGMODE_APPLICATION_CONNECT == mode) {
		szTitle = CString("Connect");
	}
	else {
		szTitle = CString("New application");
	}

	if (0 == m_pInstance) {
		m_pInstance = new CPropertySheetNewApp(szTitle,pParent,mode,refHost,0);
		ASSERT(0 != m_pInstance);
	}	
	if (0 == m_pInstance) { 
		return 0;
	}
	return m_pInstance;
}

void CPropertySheetNewApp::destroy() {
	if (0 != m_pInstance) {
		delete m_pInstance;
		m_pInstance = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPropertySheetNewApp, CPropertySheet)
/////////////////////////////////////////////////////////////////////////////
CPropertySheetNewApp::CPropertySheetNewApp(LPCTSTR pszCaption, CWnd* pParentWnd, PropertyPageNewAppMode_t mode, IAppDlgStartDialog_Host& refHost, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), m_pContext(0)
{
	for (int i=0;i<MAX_PAGES;++i) {
		m_pages[i] = 0;
	}

	SetTitle(pszCaption);

	m_psh.dwFlags |= PSH_WIZARD;		// WIZARD !!!

	m_pContext = new CPropertyPageNewAppContext(mode,refHost);	
	ASSERT(0 != m_pContext);
	createPages(*m_pContext);

	// Sedert V16.26.
	// Wanneer de dialoog in de mode 'Connect' geopend wordt dan wordt de eerste pagina niet getoond.
	if (DIALOGMODE_APPLICATION_CONNECT == mode) {
		this->SetActivePage(1);
	}
}

CPropertySheetNewApp::~CPropertySheetNewApp()
{
	for (int i=0;i<MAX_PAGES;++i) {
		if (0 != m_pages[i]) {
			delete m_pages[i];
			m_pages[i] = 0;
		}
	}
	if (0 != m_pContext) {
		delete m_pContext;
		m_pContext = 0;
	}
}

BEGIN_MESSAGE_MAP(CPropertySheetNewApp, CPropertySheet)
	ON_MESSAGE(WM_COMMUNICATION_MODEM_STATUS,OnCommunicationModemStatus)
END_MESSAGE_MAP()

// Sedert V16.26:
// Property pages ingevoegd voor zoeken en configureren van TCP/IP devices in het LAN-netwerk.
//
// Sedert V16.26:
// Aanroepen van de 'Construct()'
void CPropertySheetNewApp::createPages(CPropertyPageNewAppContext& ref) {
	m_pages[0] = new CPropertyPageNewAppPage1(this,ref);
	if (0 != m_pages[0]) {
		m_pages[0]->Construct(CPropertyPageNewAppPage1::IDD);
		m_pages[0]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[0]->m_psp.pszTitle = _T("Select project type.");
		AddPage(m_pages[0]);
	}
	m_pages[1] = new CPropertyPageNewAppPage2(this,ref);
	if (0 != m_pages[1]) {
		m_pages[1]->Construct(CPropertyPageNewAppPage2::IDD);
		m_pages[1]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[1]->m_psp.pszTitle = _T("Select mode offline or online.");
		AddPage(m_pages[1]);
	}
	m_pages[2] = new CPropertyPageNewAppPage3(this,ref);
	if (0 != m_pages[2]) {	
		m_pages[2]->Construct(CPropertyPageNewAppPage3::IDD);
		m_pages[2]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[2]->m_psp.pszTitle = _T("Set TCP/IP communication settings");
		AddPage(m_pages[2]);
	}
	m_pages[3] = new CPropertyPageNewAppPage4(this,ref);
	if (0 != m_pages[3]) {		
		m_pages[3]->Construct(CPropertyPageNewAppPage4::IDD);
		m_pages[3]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[3]->m_psp.pszTitle = _T("Search local TCP/IP LAN network.");
		AddPage(m_pages[3]);
	}
	m_pages[4] = new CPropertyPageNewAppPage5(this,ref);
	if (0 != m_pages[4]) {	
		m_pages[4]->Construct(CPropertyPageNewAppPage5::IDD);
		m_pages[4]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[4]->m_psp.pszTitle = _T("Configure TCP/IP device network settings.");
		AddPage(m_pages[4]);
	}
	m_pages[5] = new CPropertyPageNewAppPage6(this,ref);
	if (0 != m_pages[5]) {	
		m_pages[5]->Construct(CPropertyPageNewAppPage6::IDD);
		m_pages[5]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[5]->m_psp.pszTitle = _T("Configure TCP/IP device protocol settings.");
		AddPage(m_pages[5]);
	}
	m_pages[6] = new CPropertyPageNewAppPage7(this,ref);
	if (0 != m_pages[6]) {		
		m_pages[6]->Construct(CPropertyPageNewAppPage7::IDD);
		m_pages[6]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[6]->m_psp.pszTitle = _T("Connect to the TCP/IP device.");
		AddPage(m_pages[6]);
	}
	m_pages[7] = new CPropertyPageNewAppPage8(this, ref);
	if (0 != m_pages[7]) {
		m_pages[7]->Construct(CPropertyPageNewAppPage8::IDD);
		m_pages[7]->m_psp.dwFlags |= PSP_USETITLE;
		m_pages[7]->m_psp.pszTitle = _T("Configure CAN-bus device.");
		AddPage(m_pages[7]);
	}
}

BOOL CPropertySheetNewApp::OnInitDialog() {

	BOOL result = CPropertySheet::OnInitDialog();

	CFont *pFont = GetFont();	
	if (pFont != NULL) {	
		// Todo: Font instellen...
	}
	
	initContext();
	return result;
}


// Sedert V16.26
// Init van de context afhankelijk van de mode
void CPropertySheetNewApp::initContext() {
	if (0 == m_pContext) {
		ASSERT(0 != m_pContext);
		return;
	}

	const PropertyPageNewAppMode_t mode = m_pContext->getDlgMode();
	switch(mode) {
		case DIALOGMODE_APPLICATION_START: {
			m_pContext->getHost().setStateAppIdle();									 
			// De Context gegevens ifv project type wodren aagevuld in CPropertyPageNewAppPage1.
			// Bij het openen van een project bestand worden de connect tcp/ip gegevens ingeladen.
		} break;
		case DIALOGMODE_APPLICATION_CONNECT: {						
			IAppDlgStartDialog_Host& refHost = m_pContext->getHost();		
			CPropertyPageNewAppContext_Util::updatePassword(m_pContext,refHost);
			CPropertyPageNewAppContext_Util::updateLocalIPsettings(m_pContext,refHost);
			CPropertyPageNewAppContext_Util::updateRemoteIPsettings(m_pContext,refHost);
			CPropertyPageNewAppContext_Util::updatePreferedTCPIPMethod(m_pContext,refHost);
			CPropertyPageNewAppContext_Util::updateDefaultConnectMode(m_pContext,refHost);
		} break;
		default: {
			ASSERT(0); 
		} break;
	}
}

const PropertyPageNewAppMode_t CPropertySheetNewApp::getDlgMode() const {
	if (0 == m_pContext) {
		ASSERT(0 != m_pContext);
		return DIALOGMODE_APPLICATION_START;
	}
	return m_pContext->getDlgMode();
}

void CPropertySheetNewApp::setPage(int pageNR) {
	if (pageNR>=MAX_PAGES) {
		ASSERT(pageNR<MAX_PAGES);
		return;
	}
	this->SetActivePage(pageNR);
}

LONG CPropertySheetNewApp::OnCommunicationModemStatus(WPARAM wParam,LPARAM lParam) {

	for (int i=0;i<MAX_PAGES;++i) {
		if ((0 != m_pages[i]) && (0 != m_pages[i]->GetSafeHwnd())) {		
			m_pages[i]->UpdateCommunicationStatus();			
		}
	}
	return TRUE;
}

INT_PTR CPropertySheetNewApp::DoModal() 
{
	const INT_PTR retValue =  this->CPropertySheet::DoModal();
	if (ID_WIZFINISH == retValue) {
		if (0 != m_pContext) {
			// Toestand app & login info updaten mbv de context.
			IAppDlgStartDialog_Host& refHostApp = m_pContext->getHost();					
			updateAppState(refHostApp,*m_pContext);
			updateLoginInfo(refHostApp, *m_pContext);			
		}	
	}
	else if (IDCANCEL == retValue) {
		// Absorbe...
	}
	return retValue;
}

// Wordt aangeroepen na het sluiten van de dialoog.
// Doorgeven van de data van context naar de host: Updaten van de applicatie toestand: offline,online, new installation, ...
void CPropertySheetNewApp::updateAppState(IAppDlgStartDialog_Host& refHost,CPropertyPageNewAppContext& refCtx) {
	const CPropertyPageNewAppContext::ProjectType_t type = refCtx.getProjectType( );
	switch(type) {
		case CPropertyPageNewAppContext::PROJECT_TYPE_NEW: {
			if (CPropertyPageNewAppContext::STATE_DISCONNECTED == refCtx.getStateConnected()) {
				ASSERT(0);	// Mag nooit voorkomen
			}																   
			else {
				refHost.SetStateAppModeOnlineNew();	
			}
		} break;
		case CPropertyPageNewAppContext::PROJECT_TYPE_EXISTING: {
			if (CPropertyPageNewAppContext::STATE_DISCONNECTED == refCtx.getStateConnected()) {
				refHost.SetStateAppModeOfflineContinue();
			}																   
			else {
				refHost.SetStateAppModeOnlineContinue();		
			}																																																	
		} break;
	}	
}

// Wordt aangeroepen na het sluiten van de dialoog.
// Doorgeven van de data van context naar de host: Updaten van de login: offline,usb,tcp + serverinfo...
void CPropertySheetNewApp::updateLoginInfo(IAppDlgStartDialog_Host& refHost,CPropertyPageNewAppContext& refCtx) {
	
	// Updaten van de mode connectie.
	const CPropertyPageNewAppContext::ModeConnect_t mode = refCtx.getModeConnect();
	switch(mode) {
		case CPropertyPageNewAppContext::MODE_CONNECT_OFFLINE: {
			refHost.setDefaultConnectMode(IAppDlgStartDialog_Host::OFFLINE);											   
		} break;
		case CPropertyPageNewAppContext::MODE_CONNECT_TCPIP: {
			refHost.setDefaultConnectMode(IAppDlgStartDialog_Host::TCP);															 
		} break;
		case CPropertyPageNewAppContext::MODE_CONNECT_USB: {
			refHost.setDefaultConnectMode(IAppDlgStartDialog_Host::USB);
		} break;
	}

	// Updaten van de tcp/ip login gegevens.
	const CString localHostName = refCtx.getLocalHostAddress();
	const CString remoteHostName = refCtx.getRemoteHostAddress();
	const CString pwd = refCtx.getPassword();
	const int localPort = refCtx.getLocalIPPort();
	const int remotePort = refCtx.getRemoteIPPort();
	const BOOL connectLocal = refCtx.getTcpIpConnectLocal();
	refHost.setServerTCPIPInfo(localHostName,localPort,remoteHostName,remotePort,pwd,connectLocal);
}