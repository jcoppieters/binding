#include "stdafx.h"
#include "BindingSoftware.h"					// Nodig voor de resource ID's.
#include "PropertyPageNewApp.h"
#include "iappdlgtasks.h"
#include "app/appudpcfgserver/appudpservercfg.h"
#include "uicontrols/utils/dlghelper.h"				// GUI fonts
/*****************************************************/
using namespace DUOTECNO;
/*****************************************************/
CPropertyPageNewApp::CPropertyPageNewApp(CPropertySheet* parent, unsigned short nID,CPropertyPageNewAppContext& refCtx) : 
	CPropertyPage(nID), m_parent(parent), m_dialogID(nID), m_pDlgFontHelper(NULL), m_refCtx(refCtx) { }

CPropertyPageNewApp::~CPropertyPageNewApp(void) {
	if (NULL != m_pDlgFontHelper) {
		delete m_pDlgFontHelper;
		m_pDlgFontHelper = NULL;
	}
}

BOOL CPropertyPageNewApp::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	m_dwWizardButtons = 0xFFFFFF;

	if (NULL == m_pDlgFontHelper) {
		m_pDlgFontHelper = new CDlgHelperFont(*this);	
		ASSERT(NULL != m_pDlgFontHelper);
		if (NULL != m_pDlgFontHelper) {
			m_pDlgFontHelper->initDialog();	
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CPropertyPageNewApp::getPageResourceID(DialogPage_t page) {
	switch(page) {
		case DIALOG_START_PROJECTTYPE:
			return IDD_DIALOG_NEWAPP_PAGE1;		
		case DIALOG_SELECT_COMMUNICATION:
			return IDD_DIALOG_NEWAPP_PAGE2;
		case DIALOG_TCIP_PROPERTIES:
			return IDD_DIALOG_NEWAPP_PAGE3;
		case DIALOG_SEARCH_DEVICES:
			return IDD_DIALOG_NEWAPP_PAGE4;
		case DIALOG_CONFIG_CANBUS_DEVICE:
			return IDD_DIALOG_NEWAPP_PAGE8;
	}
	ASSERT(0);
	return -1;
}

// Bepalen of we de active pagina zijn.
BOOL CPropertyPageNewApp::isActivePage() {
	if (NULL == m_parent) {
		return FALSE;
	}
	return (this == m_parent->GetActivePage());
}

IAppDlgStartDialog_Host& CPropertyPageNewApp::getHost() {
	return m_refCtx.getHost();
}

void CPropertyPageNewApp::showWindow(CWnd* pDialog,int id,BOOL fState) {
	CWnd* const pControl = pDialog->GetDlgItem(id);
	if (0 != pControl) {
		pControl->ShowWindow( fState );
	}
}

void CPropertyPageNewApp::enableWindow(CWnd* pDialog,int id,BOOL fState) {
	CWnd* const pControl = pDialog->GetDlgItem(id);
	if (0 != pControl) {
		pControl->EnableWindow( fState );
	}
}

void CPropertyPageNewApp::setWindowText(CWnd* pDialog,int id,const CString& text) {
	CWnd* const pControl = pDialog->GetDlgItem(id);
	if (0 != pControl) {
		pControl->SetWindowText(text);
	}
}

// Sedert V16.26 bijhouden van de toestand van de wizardbuttons.
// NOTE_PROPERTYPAGE_WIZARDBUTTONS
// Moeten de vorige toestand van de wizard buttons bijhouden.
// Wanneer we de wizard buttons veranderen in een timer update GUI dan reageren deze niet normaal
// wanneer deze constant aangepast worden.
// Door het bijhouden van de vorige toestand worden deze zo min mogelijk aangeroepen...
void CPropertyPageNewApp::setWizardButtons(DWORD dwButtons) {
	if (m_dwWizardButtons != dwButtons) {
		m_parent->SetWizardButtons(dwButtons);
		m_dwWizardButtons = dwButtons;
	}
}

BOOL CPropertyPageNewApp::OnSetActive() {
	m_dwWizardButtons = 0xFFFFFF;
	PostMessage(WM_UPDATE_CONTROLS);
	return CPropertyPage::OnSetActive();
}

BOOL CPropertyPageNewApp::OnKillActive() {
	m_dwWizardButtons = 0xFFFFFF;
	return CPropertyPage::OnKillActive();
}


void CPropertyPageNewApp::updateContext( ) {

	IAppDlgStartDialog_Host& refHost = m_refCtx.getHost();
	CPropertyPageNewAppContext_Util::updatePassword(&m_refCtx,refHost);
	CPropertyPageNewAppContext_Util::updateLocalIPsettings(&m_refCtx,refHost);
	CPropertyPageNewAppContext_Util::updateRemoteIPsettings(&m_refCtx,refHost);
	CPropertyPageNewAppContext_Util::updatePreferedTCPIPMethod(&m_refCtx,refHost);
	CPropertyPageNewAppContext_Util::updateDefaultConnectMode(&m_refCtx,refHost);
}

void CPropertyPageNewApp::clearContext( ) {	
	// Context opnieuw wissen --->>> TODO !!!!
}

