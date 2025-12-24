// DlgUDPLanConfig.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgUDPLanConfig.h"

// Child dialogen.
#include "DlgChildUDPNwk.h"
#include "DlgChildUDPHomeserver.h"
#include "DlgChildUDPDynDns.h"
#include "DlgChildUDPDns.h"
#include "DlgChildUDPNTP.h"
#include "DlgChildUDPSystemMode.h"
#include "DlgChildUDPTcpIp.h"

#include "app/appudpcfgserver/appudpservercfg.h"				// UDP device
#include "app/appudpcfgserver/appudpservercfgwrapper.h"			// UDP Device wrapper

#include "iappdlgtasks.h"			// worker
#include "uicontrols/utils/dlghelper.h"				// GUI fonts
/******************************************************************************/
using namespace DUOTECNO;
/******************************************************************************/
CDlgUDPLanConfig* CDlgUDPLanConfig::m_pInstance = NULL;
/******************************************************************************/

// NOTE:
// Indien de dialoog reeds active is moeten we een (nieuw) device koppelen en opnieuw de properties opvragen.
BOOL CDlgUDPLanConfig::Show(CWnd* pWndParent,IAppDlgUDPLanCfg_Host& refHost,const AppUDPDevice& refDevice) {
	if (NULL != m_pInstance) {
		if (NULL != m_pInstance->GetSafeHwnd()) { 
			m_pInstance->show(refDevice);	// NOTE.
			return TRUE;
		}
		delete m_pInstance;
		m_pInstance = NULL;
	}

	m_pInstance = new CDlgUDPLanConfig(refHost,refDevice,pWndParent);	
	if (NULL == m_pInstance) {
		ASSERT(NULL != m_pInstance);
		return FALSE;
	}
	if (m_pInstance->Create() == TRUE) {
		m_pInstance->ShowWindow(TRUE);
		return TRUE;
	}
	return FALSE;
}

CDlgUDPLanConfig* CDlgUDPLanConfig::getInstance() {
	return m_pInstance;
}

BOOL CDlgUDPLanConfig::IsActive() {
	if (NULL == m_pInstance) {
		return FALSE;
	}
	return (NULL != m_pInstance->GetSafeHwnd());
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
// CDlgUDPLanConfig dialog
IMPLEMENT_DYNAMIC(CDlgUDPLanConfig, CDialog)

CDlgUDPLanConfig::CDlgUDPLanConfig(IAppDlgUDPLanCfg_Host& refHost,const AppUDPDevice& refDevice, CWnd* pParent)
	: CDialog(CDlgUDPLanConfig::IDD, pParent), m_pCtx(new CDlgUDPConfigCtx(this, refHost, refDevice)), 
	  m_pFsm(new CDlgUDPConfigFsm(*m_pCtx)), m_pDlgFontHelper(NULL), m_state(STATE_INIT)
{
	ASSERT(NULL != m_pCtx);
	ASSERT(NULL != m_pFsm);

	for (int i=0;i<TAB_SELECTION_MAX;++i) {
		m_pChildDialogs[i] = NULL;
	}
}

CDlgUDPLanConfig::~CDlgUDPLanConfig()
{
	deallocSubPages( );

	if (NULL != m_pCtx) {
		delete m_pCtx;
	}
	if (NULL != m_pFsm) {
		delete m_pFsm;
	}
	if (NULL != m_pDlgFontHelper) {
		delete m_pDlgFontHelper;
		m_pDlgFontHelper = NULL;
	}
}

void CDlgUDPLanConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SELECT, m_tabCtrlSelection);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfoMsg);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editDeviceName);
}


BEGIN_MESSAGE_MAP(CDlgUDPLanConfig, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_BN_CLICKED(IDOK, &CDlgUDPLanConfig::OnBnClickedOk)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SELECT, &CDlgUDPLanConfig::OnTcnSelchangeTabSelect)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_SELECT, &CDlgUDPLanConfig::OnTcnSelchangingTabSelect)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Wanneer de dialoog reeds bestaat.
// Doen geen controle op het device - Deze wordt telkens opnieuw aan de context toegevoegd en 
// de eigenschappen worden opnieuw opgevraagd.
void CDlgUDPLanConfig::show(const AppUDPDevice& refDevice) {

	ASSERT(NULL != this->GetSafeHwnd());
	ASSERT(NULL != m_pCtx);
	ASSERT(NULL != m_pFsm);

	// Gui tabs & subdialogen verwijderen.
	removeTabCtrlItems( );	
	deallocSubPages( );

	// Device aan de context toekennen.
	m_pCtx->setUDPDevice(refDevice);

	// De FSM resetten...
	m_pFsm->reset( );
	
	// Opvragen van de properties.
	m_state = STATE_BUSY;	

	// Static GUI controls initialiseren.
	this->initStaticControls( );
	
	m_pCtx->setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_BUSY);
	m_pCtx->setStatusMsg("Requesting device properties. Please wait...");

	// Actief venster zetten.
	this->SetActiveWindow();

	this->SetTimer(TIMER_ID_POLL, 1, (TIMERPROC)NULL);
	PostMessage(WM_UPDATE_CONTROLS);
}

// CDlgUDPLanConfig message handlers
BOOL CDlgUDPLanConfig::Create(){ 
	return CDialog::Create(CDlgUDPLanConfig::IDD,AfxGetMainWnd()); 
}

void CDlgUDPLanConfig::PostNcDestroy() {
	ASSERT(NULL != m_pInstance);
	
	for (int i=0;i<TAB_SELECTION_MAX;++i) {
		if (NULL != m_pChildDialogs[i]) {
			m_pChildDialogs[i]->DestroyWindow();		
		}
	}
	delete this;
	m_pInstance = NULL;
}

void CDlgUDPLanConfig::createTabs( ) {

	ASSERT(NULL != m_pCtx);

	int indexTabControl = 0;	
	CAppSupportedFlags& refAppSupportedFlags = m_pCtx->getUDPDevice().getAppSupportedFlags();
	// Nwk settings is altijd de eerste tab...
	{
		this->addTab(indexTabControl, TAB_SELECTION_NWKSETTINGS);
		this->addDialog(TAB_SELECTION_NWKSETTINGS);
	}

	// Ofwel homeserver cfg ofwel API cfg.
	// Wanneer de homeserver cfg wordt het paswoord automatisch overgenomen...
	if (refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_SERVER)) {
		this->addTab(++indexTabControl, TAB_SELECTION_HOMESERVER);
		this->addDialog(TAB_SELECTION_HOMESERVER);
	}	
	else if (refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_API)) {
		this->addTab(++indexTabControl, TAB_SELECTION_APICFG);
		this->addDialog(TAB_SELECTION_APICFG);
	}

	if (refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_DNSPROVIDER)) {
		this->addTab(++indexTabControl, TAB_SELECTION_DNS);
		this->addDialog(TAB_SELECTION_DNS);	
	}
	if (refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_DDNS)) {
		this->addTab(++indexTabControl, TAB_SELECTION_DYNDNS);
		this->addDialog(TAB_SELECTION_DYNDNS);
	}
	if (refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_SNTP) || refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_TIME)) {
		this->addTab(++indexTabControl, TAB_SELECTION_NTP);
		this->addDialog(TAB_SELECTION_NTP);
	}
	if (refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_SYSMODE)) {
		this->addTab(++indexTabControl, TAB_SELECTION_SYSTEMMODE);
		this->addDialog(TAB_SELECTION_SYSTEMMODE);
	}	
	if (refAppSupportedFlags.IsSupported(CAppSupportedFlags::CFG_TCPIPCLIENT)) {	
		this->addTab(++indexTabControl, TAB_SELECTION_TCPIP);
		this->addDialog(TAB_SELECTION_TCPIP);
	}	
	this->showActiveDialog(TAB_SELECTION_NWKSETTINGS);
}

BOOL CDlgUDPLanConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (NULL == m_pDlgFontHelper) {
		m_pDlgFontHelper = new CDlgHelperFont(*this);	
		ASSERT(NULL != m_pDlgFontHelper);
		if (NULL != m_pDlgFontHelper) {
			m_pDlgFontHelper->initDialog();	
		}
	}

	this->CenterWindow();

	// Opvragen van de properties.
	m_state = STATE_BUSY;	

	// Static GUI controls initialiseren.
	this->initStaticControls( );
	
	m_pCtx->setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_BUSY);
	m_pCtx->setStatusMsg("Requesting device properties. Please wait...");

	this->SetTimer(TIMER_ID_POLL, 1, (TIMERPROC)NULL);
	PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUDPLanConfig::initStaticControls( ) {

	// Progress control
	// create My progress control.
	if (NULL == m_myProgressCtrl.GetSafeHwnd()) {
		CRect rcRectProgressCtrl(40,105,340,120);
		m_myProgressCtrl.SetRange(0,30);
		m_myProgressCtrl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcRectProgressCtrl,this,0);
		m_myProgressCtrl.StepUp();
		m_myProgressCtrl.ShowWindow(FALSE);
	}

	if (NULL != m_tabCtrlSelection.GetSafeHwnd()) {
		m_tabCtrlSelection.ShowWindow(FALSE);
	}

	// Updaten van de naam van het UDP device.
	CString szDeviceName;
	ASSERT(NULL != m_pCtx);
	if (NULL != m_pCtx) {
		szDeviceName = m_pCtx->getUDPDevice().getName();	
	}
	// edit box + status msg.
	m_editDeviceName.SetWindowText(szDeviceName);
	m_staticInfoMsg.SetWindowText("");
}


void CDlgUDPLanConfig::OnBnClickedOk()
{
	m_tabCtrlSelection.DeleteAllItems();
	DestroyWindow();
}

void CDlgUDPLanConfig::OnCancel()
{
	m_tabCtrlSelection.DeleteAllItems();
	DestroyWindow();
}

LONG CDlgUDPLanConfig::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	if (NULL == m_pCtx) {
		ASSERT(NULL != m_pCtx);
		return TRUE;
	}

	if (m_pCtx->statusMsgChanged()) {
		m_staticInfoMsg.SetWindowText(m_pCtx->getStatusMsg());
	}

	BOOL showTabCtrl = FALSE;
	switch(m_state) {
		case STATE_INIT: {
			showTabCtrl = FALSE;
		} break;
		case STATE_IDLE: {
			showTabCtrl = TRUE;
		} break;
		case STATE_BUSY: {
			showTabCtrl = FALSE;
		} break;	
	}

	m_tabCtrlSelection.ShowWindow(showTabCtrl);

	// Updaten van de progressbar...
	if (m_pCtx->stateProgressBarChanged()) {		
		if (m_myProgressCtrl.GetSafeHwnd()) {
			const CDlgUDPConfigCtx::ProgressBarState_t progressBarState = m_pCtx->getProgressBarState();
			switch(progressBarState) {
				case CDlgUDPConfigCtx::PROGRESSBAR_STATE_INVISIBLE: {
					m_myProgressCtrl.ShowWindow(FALSE);	
					m_myProgressCtrl.Clear();
				} break;
				case CDlgUDPConfigCtx::PROGRESSBAR_STATE_BUSY: {
					m_myProgressCtrl.ShowWindow(TRUE);		
					m_myProgressCtrl.StepUp();
					this->SetTimer(TIMER_ID_PROGRESSBAR, 100, (TIMERPROC) NULL);	// Timer hertrigger...
				} break;
				case CDlgUDPConfigCtx::PROGRESSBAR_STATE_DONE: {
					m_myProgressCtrl.ShowWindow(TRUE);		
					m_myProgressCtrl.SetComplete();		   				
				} break;
				case CDlgUDPConfigCtx::PROGRESSBAR_STATE_ERROR: {
					m_myProgressCtrl.ShowWindow(TRUE);		
					m_myProgressCtrl.SetError();								
				} break;
			}
		}
	}

	m_pCtx->clearGuiChangesFlags();
	return TRUE;
}

void CDlgUDPLanConfig::addTab(int index, TabSelection_t tab) 
{
	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT | TCIF_PARAM;
	tcItem.lParam = tab;
	tcItem.pszText = NULL;

	switch(tab) {
		case TAB_SELECTION_NWKSETTINGS: {				
			tcItem.pszText = _T("Network");
		} break;	
		case TAB_SELECTION_HOMESERVER: {								   			
			tcItem.pszText = _T("Homeserver");				   
		} break;
		case TAB_SELECTION_APICFG: {
			tcItem.pszText = _T("Secured API");								   
		} break;
		case TAB_SELECTION_DNS: {
			tcItem.pszText = _T("DNS");							
		} break;
		case TAB_SELECTION_DYNDNS: {
			tcItem.pszText = _T("DynDns");									   
		} break;
		case TAB_SELECTION_NTP: {
			tcItem.pszText = _T("NTP");											
		} break;
		case TAB_SELECTION_SYSTEMMODE: {
			tcItem.pszText = _T("System");						   
		} break;
		case TAB_SELECTION_TCPIP: {
			tcItem.pszText = _T("TCP/IP");									  
		} break;
	}
	if (NULL != tcItem.pszText) {
		m_tabCtrlSelection.InsertItem(index, &tcItem);
	}
}

// NOTE-PARENT: De parent wordt meegegeven in de create()
void CDlgUDPLanConfig::addDialog(TabSelection_t tab) {
	if (NULL != m_pChildDialogs[tab]) {
		ASSERT(NULL == m_pChildDialogs[tab]);
		return;			
	}
	
	ASSERT(NULL != m_pCtx);

	int templateResourceID = -1;
	switch(tab) {
		case TAB_SELECTION_NWKSETTINGS: {	
			m_pChildDialogs[tab] = new CDlgChildUDPNwk(NULL, *m_pCtx);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPNwk::IDD;
		} break;
		case TAB_SELECTION_HOMESERVER: {	
			m_pChildDialogs[tab] = new CDlgChildUDPHomeserver(NULL, *m_pCtx,CDlgChildUDPHomeserver::MODE_HOMESERVER);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPHomeserver::IDD;						
		} break;
		case TAB_SELECTION_APICFG: {	
			m_pChildDialogs[tab] = new CDlgChildUDPHomeserver(NULL, *m_pCtx,CDlgChildUDPHomeserver::MODE_API);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPHomeserver::IDD;						
		} break;
		case TAB_SELECTION_DNS: {
			m_pChildDialogs[tab] = new CDlgChildUDPDns(NULL, *m_pCtx);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPDns::IDD;												
		} break;
		case TAB_SELECTION_DYNDNS: {
			m_pChildDialogs[tab] = new CDlgChildUDPDynDns(NULL, *m_pCtx);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPDynDns::IDD;								   
		} break;
		case TAB_SELECTION_NTP: {
			m_pChildDialogs[tab] = new CDlgChildUDPNTP(NULL, *m_pCtx);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPNTP::IDD;										
		} break;
		case TAB_SELECTION_SYSTEMMODE: {
			m_pChildDialogs[tab] = new CDlgChildUDPSystemMode(NULL, *m_pCtx);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPSystemMode::IDD;						   
		} break;
		case TAB_SELECTION_TCPIP: {
			m_pChildDialogs[tab] = new CDlgChildUDPTcpIp(NULL, *m_pCtx);	// NOTE-PARENT
			templateResourceID = CDlgChildUDPTcpIp::IDD;			  
		} break;
	}
	
	if (NULL != m_pChildDialogs[tab]) {
		m_pChildDialogs[tab]->Create(templateResourceID, &m_tabCtrlSelection);
		m_pChildDialogs[tab]->ShowWindow(FALSE);
	}
	else {
		ASSERT(NULL != m_pChildDialogs[tab]); 
	}
}

void CDlgUDPLanConfig::showActiveDialog(TabSelection_t tab) {
	if (NULL == m_pChildDialogs[tab]) {
		ASSERT(NULL != m_pChildDialogs[tab]);
		return;			
	}

	RECT rc;
	m_tabCtrlSelection.GetItemRect(0,&rc);

	m_pChildDialogs[tab]->SetWindowPos(NULL, 
									   rc.left + 1, 
									   rc.bottom + 1, 
									   0, 
									   0, 
									   SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	m_pChildDialogs[tab]->SetFocus();
	m_pChildDialogs[tab]->onActive(TRUE);
}

BOOL CDlgUDPLanConfig::getCurrentSelected(TabSelection_t *tab) {
	const int currSel = m_tabCtrlSelection.GetCurSel();
	if (currSel < 0) {
		ASSERT(currSel >= 0);
		return FALSE;
	}
	TCITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	if (!m_tabCtrlSelection.GetItem(currSel,&tcItem)) {
		ASSERT(0);
		return FALSE;
	}
	if ((tcItem.lParam < 0) || (tcItem.lParam >= TAB_SELECTION_MAX)) {
		ASSERT(tcItem.lParam >= 0);
		ASSERT(tcItem.lParam < TAB_SELECTION_MAX);
		return FALSE;
	}
	*tab = (TabSelection_t) tcItem.lParam;
	return TRUE;
}

// Bij het verlaten van een tab altijd de progressbar verwijderern.
void CDlgUDPLanConfig::OnTcnSelchangeTabSelect(NMHDR *pNMHDR, LRESULT *pResult)
{
	TabSelection_t currSelected = TAB_SELECTION_NWKSETTINGS;
	if (!getCurrentSelected(&currSelected)) {
		*pResult = 0;		/* TODO: Andere code ?? */
		return;
	}

	// Bij het verlaten van een tab altijd de progressbar verwijderen.
	m_pCtx->setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_INVISIBLE);
	m_pCtx->setStatusMsg("");

	showActiveDialog(currSelected);
	*pResult = 0;

	this->PostMessage(WM_UPDATE_CONTROLS);
}


void CDlgUDPLanConfig::OnTcnSelchangingTabSelect(NMHDR *pNMHDR, LRESULT *pResult)
{
	TabSelection_t currSelected = TAB_SELECTION_NWKSETTINGS;
	if (!getCurrentSelected(&currSelected)) {
		*pResult = 0;		/* TODO: Andere code ?? */
		return;
	}

	CDlgChildUDPBase* const pDlg = m_pChildDialogs[currSelected];
	if (pDlg->onLeave()) {
		pDlg->ShowWindow(SW_HIDE);
		pDlg->onActive(FALSE);
		*pResult = 0;
	}
	else {
		*pResult = -1;
	}		
}

void CDlgUDPLanConfig::OnTimer(UINT_PTR nIDEvent) {
	this->KillTimer(nIDEvent);

	if (NULL == m_pCtx) {
		ASSERT(NULL != m_pCtx);
		return;
	}
	
	switch(nIDEvent) {
		case TIMER_ID_PROGRESSBAR: {				
			if (CDlgUDPConfigCtx::PROGRESSBAR_STATE_BUSY == m_pCtx->getProgressBarState()) {
				m_myProgressCtrl.StepUp();				
				this->SetTimer(nIDEvent, 100, (TIMERPROC) NULL);
			}			
		} break;	
		case TIMER_ID_POLL: {
			ASSERT(NULL != m_pFsm);
			CDlgUDPConfigFsm::GlobalFsmState_t fsmState = m_pFsm->pollFsm(CDlgUDPConfigFsm::EV_POLL);
			if (CDlgUDPConfigFsm::FSM_STATE_IDLE == fsmState) {
				// absorbe.
			}
			else if (CDlgUDPConfigFsm::FSM_STATE_BUSY == fsmState) {
				this->SetTimer(TIMER_ID_POLL, 10, (TIMERPROC)NULL);
			}
			else if (CDlgUDPConfigFsm::FSM_STATE_DONE == fsmState) {
				m_pCtx->setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_INVISIBLE);
				this->createTabs();
				m_state = STATE_IDLE;
				this->PostMessage(WM_UPDATE_CONTROLS);
			}
			else if (CDlgUDPConfigFsm::FSM_STATE_ERROR == fsmState) {
				m_pCtx->setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_ERROR);
				m_pCtx->setStatusMsg("Requesting device properties failed. Device not responding.");
				this->PostMessage(WM_UPDATE_CONTROLS);
			}
		} break;
	}
}

void CDlgUDPLanConfig::removeTabCtrlItems( ) {
	if (NULL != m_tabCtrlSelection.GetSafeHwnd()) {
		m_tabCtrlSelection.DeselectAll(FALSE);
		m_tabCtrlSelection.DeleteAllItems();
		ASSERT(0 == m_tabCtrlSelection.GetItemCount());
	}	
}

void CDlgUDPLanConfig::deallocSubPages( ) {
	for (int i=0;i<TAB_SELECTION_MAX;++i) {
		if (NULL != m_pChildDialogs[i]) {			
			delete m_pChildDialogs[i];
			m_pChildDialogs[i] = NULL;
		}
	}
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CDlgUDPConfigFsm::CDlgUDPConfigFsm(CDlgUDPConfigCtx &refCtx) : 
m_refCtx(refCtx), m_state(STATE_IDLE) { }


CDlgUDPConfigFsm::GlobalFsmState_t CDlgUDPConfigFsm::getState() const {
	if (STATE_IDLE == m_state) {
		return FSM_STATE_IDLE;
	}
	else if (STATE_REQUEST_ERROR == m_state) {
		return FSM_STATE_ERROR;
	}
	else if (STATE_REQUEST_DONE == m_state) {
		return FSM_STATE_DONE;
	}
	return FSM_STATE_BUSY;
}

void CDlgUDPConfigFsm::reset( ) {
	m_state = STATE_IDLE;
}

CDlgUDPConfigFsm::GlobalFsmState_t CDlgUDPConfigFsm::pollFsm(Event_t ev) {	
	switch(m_state) {		
		case STATE_IDLE: {
			switch(ev) {
				case EV_POLL: {
					m_retryCounter = 0;
					m_state = STATE_REQUEST_FLAGS;				
				} break;
			}						 						 
		} break;
		case STATE_REQUEST_FLAGS: {
			switch(ev) {
				case EV_POLL: {
					if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {					
						AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
						AppUDPServerCfgWrapper worker;
						if (worker.sendRequestCfgSupported(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {		
							m_retryCounter = 0;
							m_state = STATE_REQUEST_HS;
						}					
					}
					else {
						m_state = STATE_REQUEST_ERROR;
					}					
				} break;
			}
		} break;
		case STATE_REQUEST_HS: {
			switch(ev) {
				case EV_POLL: {	
					AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
					CAppSupportedFlags& refSupportedFlags = refDevice.getAppSupportedFlags();
					if (refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_SERVER) ||
						refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_API)) {
						if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {	
							AppUDPServerCfgWrapper worker;
							if (worker.sendRequestCfgAppHomeSrv(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {		
								m_refCtx.getHost().updateNodeHomeserverService(refDevice);	// Sedert V16.28
								m_retryCounter = 0;
								m_state = STATE_REQUEST_DDNS;
							}
						}
						else {
							m_state = STATE_REQUEST_ERROR;
						}					
					}
					else {
						m_state = STATE_REQUEST_DDNS;
					}					
				} break;						
			}			   
		} break;
		case STATE_REQUEST_DDNS: {
			switch(ev) {
				case EV_POLL: {				
					AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
					CAppSupportedFlags& refSupportedFlags = refDevice.getAppSupportedFlags();
					if (refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_DDNS)) {
						if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {	
							AppUDPServerCfgWrapper worker;
							if (worker.sendRequestDynDNSSettings(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {								
								m_refCtx.getHost().updateNodeDynDnHostname(refDevice);	// Sedert V16.28
								m_retryCounter = 0;
								m_state = STATE_REQUEST_SYSMODE;
							}
						}
						else {
							m_state = STATE_REQUEST_ERROR;
						}					
					}
					else {
						m_state = STATE_REQUEST_SYSMODE;
					}					
				} break;											 								 
			}
		} break;
		case STATE_REQUEST_SYSMODE: {
			switch(ev) {
				case EV_POLL: {				
					AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
					CAppSupportedFlags& refSupportedFlags = refDevice.getAppSupportedFlags();
					if (refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_SYSMODE)) {
						if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {	
							AppUDPServerCfgWrapper worker;
							if (worker.sendRequestCfgSystemMode(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {		
								m_retryCounter = 0;
								m_state = STATE_REQUEST_TCPIPCLIENT;
							}
						}
						else {
							m_state = STATE_REQUEST_ERROR;
						}					
					}
					else {
						m_state = STATE_REQUEST_TCPIPCLIENT;
					}
				} break;											 								 
			}									
		} break;
		case STATE_REQUEST_TCPIPCLIENT: {
			switch(ev) {
				case EV_POLL: {				
					AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
					CAppSupportedFlags& refSupportedFlags = refDevice.getAppSupportedFlags();
					if (refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_TCPIPCLIENT)) {
						if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {	
							AppUDPServerCfgWrapper worker;
							if (worker.sendRequestCfgTCPIPClient(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {		
								m_retryCounter = 0;
								m_state = STATE_REQUEST_DNSPROVIDER;
							}
						}
						else {
							m_state = STATE_REQUEST_ERROR;
						}					
					}
					else {
						m_state = STATE_REQUEST_DNSPROVIDER;
					}
				} break;											 								 
			}														
		} break;
		case STATE_REQUEST_DNSPROVIDER: {
			switch(ev) {
				case EV_POLL: {				
					AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
					CAppSupportedFlags& refSupportedFlags = refDevice.getAppSupportedFlags();
					if (refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_DNSPROVIDER)) {
						if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {	
							AppUDPServerCfgWrapper worker;
							if (worker.sendRequestCfgDNSProvider(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {		
								m_retryCounter = 0;
								m_state = STATE_REQUEST_SNTP;
							}
						}
						else {
							m_state = STATE_REQUEST_ERROR;
						}					
					}
					else {
						m_state = STATE_REQUEST_SNTP;
					}
				} break;											 								 
			}														
		} break;
		case STATE_REQUEST_SNTP: {
			switch(ev) {
				case EV_POLL: {				
					AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
					CAppSupportedFlags& refSupportedFlags = refDevice.getAppSupportedFlags();
					if (refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_SNTP)) {
						if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {	
							AppUDPServerCfgWrapper worker;
							if (worker.sendRequestCfgNTP(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {		
								m_retryCounter = 0;
								m_state = STATE_REQUEST_TIME;
							}
						}
						else {
							m_state = STATE_REQUEST_ERROR;
						}					
					}
					else {
						m_state = STATE_REQUEST_TIME;
					}
				} break;											 								 
			}						 
		} break;
		case STATE_REQUEST_TIME: {
			switch(ev) {
				case EV_POLL: {				
					AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
					CAppSupportedFlags& refSupportedFlags = refDevice.getAppSupportedFlags();
					if (refSupportedFlags.IsSupported(CAppSupportedFlags::CFG_TIME)) {
						if (++m_retryCounter < UDPCFG_REQUEST_NR_OF_RETRIES) {	
							AppUDPServerCfgWrapper worker;
							if (worker.sendRequestCfgTimeDate(&refDevice, UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC)) {		
								m_retryCounter = 0;
								m_state = STATE_REQUEST_DONE;
							}
						}
						else {
							m_state = STATE_REQUEST_ERROR;
						}					
					}
					else {
						m_state = STATE_REQUEST_DONE;
					}
				} break;											 								 
			}												 
		} break;
		case STATE_REQUEST_DONE: {
								 
		} break;
		case STATE_REQUEST_ERROR: {
								  
		} break;		
	}
	return getState();
}

