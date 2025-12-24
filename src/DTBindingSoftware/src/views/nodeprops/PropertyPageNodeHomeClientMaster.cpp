// PropertyPageNodeHomeClientMaster.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeHomeClientMaster.h"
#include "app\AppState\AppState.h"										// CHANGES_CLOUDBRIDGE_POC - Tijdelijk?

// CPropertyPageNodeHomeClientMaster dialog

IMPLEMENT_DYNAMIC(CPropertyPageNodeHomeClientMaster, CPropertyPage)

CPropertyPageNodeHomeClientMaster::CPropertyPageNodeHomeClientMaster()
	: CPropertyPageNode(CPropertyPageNodeHomeClientMaster::IDD) , m_pDlgWorker(NULL), m_pDlgHelperEvents(NULL), 
	m_stateIdle(true), m_showControlsConnect(true), m_initDone(false), m_busyConnect(false), m_stateConnected(false)
	, m_selectionSettings(0)
{ 
	m_pDlgHelperEvents = new CPropertyPageNodeHomeClientMaster_WorkerEvents(*this);
	ASSERT(0 != m_pDlgHelperEvents);
}


CPropertyPageNodeHomeClientMaster::CPropertyPageNodeHomeClientMaster(CCanNode* pNode,IAppDlgHomeClientMasterTasks_Host* pDlgWorker)
	: CPropertyPageNode(CPropertyPageNodeHomeClientMaster::IDD, pNode), m_pDlgWorker(pDlgWorker), m_pDlgHelperEvents(NULL), 
		m_stateIdle(true), m_showControlsConnect(true), m_initDone(false), m_busyConnect(false), m_stateConnected(false)
{
	m_pDlgHelperEvents = new CPropertyPageNodeHomeClientMaster_WorkerEvents(*this);
	ASSERT(0 != m_pDlgHelperEvents);
}

CPropertyPageNodeHomeClientMaster::~CPropertyPageNodeHomeClientMaster( )	
{
	if (NULL != m_pDlgWorker) {	
		m_pDlgWorker->unsubscribe(m_pDlgHelperEvents);
		m_pDlgWorker->close();
	}
	if (NULL != m_pDlgHelperEvents) {
		delete m_pDlgHelperEvents;
		m_pDlgHelperEvents = NULL;
	}
}

void CPropertyPageNodeHomeClientMaster::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_MASTER_PNP, m_btnStartPnp);
	DDX_Control(pDX, IDC_BTN_MASTER_BROWSE, m_btnStartBrowse);
	DDX_Control(pDX, IDC_BTN_MASTER_BROWSECLEAR2, m_btnStartBrowseClear);
	DDX_Control(pDX, IDC_BTN_MASTER_UPDATE, m_btnStartUpdate);
	DDX_Control(pDX, IDC_BTN_MASTER_CANCEL, m_btnCancelTask);
	DDX_Control(pDX, IDC_LISTBOX_INFO, m_listboxInfo);
	DDX_Control(pDX, IDC_STATIC_CONNECT, m_groupConnect);
	DDX_Control(pDX, IDC_CHECK_CONNECT2, m_checkConnect);
	DDX_Control(pDX, IDC_EDIT_TASKAPPSTATE, m_EditTaskAppState);
	DDX_Control(pDX, IDC_EDIT_NODEDATABASE, m_EditNodeDatabase);	
	DDX_Control(pDX, IDC_RADIO_LOCAL, m_radioLocal);
	DDX_Control(pDX, IDC_RADIO_REMOTE, m_radioRemote);
	DDX_Radio(pDX, IDC_RADIO_LOCAL, m_selectionSettings);
	DDX_Control(pDX, IDC_BUTTON_EDITCFG, m_btnEdit);
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeHomeClientMaster, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS, OnUpdateControls)
	ON_MESSAGE(WM_DISPLAY_ERRORMESSAGE,OnDisplayErrorMessage)
	ON_BN_CLICKED(IDC_BTN_MASTER_PNP, &CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterPnp)
	ON_BN_CLICKED(IDC_BTN_MASTER_BROWSE, &CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterBrowse)
	ON_BN_CLICKED(IDC_BTN_MASTER_BROWSECLEAR2, &CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterBrowseclear2)
	ON_BN_CLICKED(IDC_BTN_MASTER_UPDATE, &CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterUpdate)
	ON_BN_CLICKED(IDC_BTN_MASTER_CANCEL, &CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterCancel)
	ON_BN_CLICKED(IDC_CHECK_CONNECT2, &CPropertyPageNodeHomeClientMaster::OnBnClickedCheckConnect2)
	ON_BN_CLICKED(IDC_BUTTON_EDITCFG, &CPropertyPageNodeHomeClientMaster::OnBnClickedButtonEditcfg)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_LOCAL, IDC_RADIO_REMOTE, OnRadioButtonsType)
END_MESSAGE_MAP()


// CPropertyPageNodeHomeClientMaster message handlers
void CPropertyPageNodeHomeClientMaster::UpdateRunTimeControls() {
	// absorbe
}

BOOL CPropertyPageNodeHomeClientMaster::OnInitDialog() {
	CPropertyPage::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

	m_stateConnected = FALSE;
	m_stateIdle = TRUE;
	if ((NULL != m_pDlgWorker) && (NULL != m_pDlgHelperEvents)) {	

		ASSERT(NULL != this->m_pSelNode);
		m_pDlgWorker->init(this->m_pSelNode);
		m_pDlgWorker->subscribe(m_pDlgHelperEvents);
		m_stateIdle = m_pDlgWorker->isStateIdle() == TRUE ? true : false;
		m_stateConnected = m_pDlgWorker->isConnected();
	}

	m_selectionSettings = 0;
	m_listboxInfo.ResetContent();
	m_initDone = false;	
	UpdateData(FALSE);
	PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

LONG CPropertyPageNodeHomeClientMaster::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	m_showControlsConnect = true;

	bool connected = true;	
	if (0 != m_pDlgWorker) {
		connected = m_pDlgWorker->isConnected();
	}

	if (m_stateConnected != connected) {		
		if (!connected) {			
			AddMessage("Disconnected");
		}	
		else {		
			AddMessage("Connected",TRUE);
		}
		m_stateConnected = connected;
	}

	if (!m_initDone) {	
		if (m_showControlsConnect) {			
			if (connected) {
				// Wanneer we geconnecteerd zijn de juiste radio button selectie tonen.
				// f(connected) : lokaal of remote.
				const HomeClientConnectionState_t type = m_pDlgWorker->getTypeConnected( );
				if (HOMECLIENT_STATE_CONNECTED_LOCAL_IP == type) {
					m_selectionSettings = 0;
				}
				else {
					m_selectionSettings = 1;
				}
				UpdateData(FALSE);
			}			
		}	
		m_initDone = true;
	}


	if (m_stateIdle) {
		m_btnStartPnp.EnableWindow(connected && TRUE);
		m_btnStartBrowse.EnableWindow(connected && TRUE);
		m_btnStartBrowseClear.EnableWindow(connected && TRUE);
		m_btnStartUpdate.EnableWindow(connected && TRUE);
		m_btnCancelTask.EnableWindow(FALSE);	
	}
	else {
		m_btnStartPnp.EnableWindow(FALSE);
		m_btnStartBrowse.EnableWindow(FALSE);
		m_btnStartBrowseClear.EnableWindow(FALSE);
		m_btnStartUpdate.EnableWindow(FALSE);
		m_btnCancelTask.EnableWindow(connected && TRUE);	
	}
	
	CString textAppTaskState("Offline");
	m_EditTaskAppState.ShowWindow(true);
	if (connected && m_pDlgWorker) {
		const CString textNodeDatabase = m_pDlgWorker->getStrNodeDbaseState();
		m_EditNodeDatabase.ShowWindow(true);
		textAppTaskState = m_pDlgWorker->getStrAppTaskState();
		m_EditNodeDatabase.SetWindowText(textNodeDatabase);
	}
	else {
		m_EditNodeDatabase.ShowWindow(false);
	}
	m_EditTaskAppState.SetWindowText(textAppTaskState);


	m_groupConnect.ShowWindow(m_showControlsConnect);
	m_checkConnect.ShowWindow(m_showControlsConnect);
	m_radioLocal.ShowWindow(m_showControlsConnect);
	m_radioRemote.ShowWindow(m_showControlsConnect);
	m_btnEdit.ShowWindow(m_showControlsConnect);

	if (m_showControlsConnect) {

		if (connected || m_busyConnect) {
			m_radioLocal.EnableWindow(FALSE);
			m_radioRemote.EnableWindow(FALSE);			
		}
		else {
			// Radio button: select local/remote...
			const BOOL connectLocalPossible = (m_pDlgWorker->connectLocalPossible( ) == true ? TRUE : FALSE);
			const BOOL connectRemotePossible = (m_pDlgWorker->connectRemotePossible( ) == true ? TRUE : FALSE);

			m_radioLocal.EnableWindow(connectLocalPossible);
			m_radioRemote.EnableWindow(connectRemotePossible);

			if (FALSE == connectLocalPossible && FALSE == connectRemotePossible) {
				if (m_selectionSettings>=0) {
					m_selectionSettings = -1;
					UpdateData(FALSE);
				}
			}
			else if (FALSE == connectLocalPossible) {
				if (m_selectionSettings==0) {
					m_selectionSettings = 1;
					UpdateData(FALSE);
				}
			}
			else if (FALSE == connectRemotePossible) {
				if (m_selectionSettings==1) {
					m_selectionSettings = 0;
					UpdateData(FALSE);
				}
			}
		}

		m_btnEdit.EnableWindow(!m_busyConnect && !connected);
		m_checkConnect.EnableWindow(!m_busyConnect && (m_selectionSettings>=0));
		m_checkConnect.SetCheck(connected == true ? 1 : 0);
	}	
	return TRUE;
}

// Oppassen: 
// Moeten de errors in een messagebox via assynchrone manier doen.
LONG CPropertyPageNodeHomeClientMaster::OnDisplayErrorMessage(WPARAM wParam,LPARAM lParam) {
	if (!this->m_errorString.IsEmpty()) {
		AfxMessageBox(this->m_errorString,MB_ICONERROR);
		this->m_errorString.Empty();
	}

	return TRUE;
}

// Display error msg via assync call in messagebox.
void CPropertyPageNodeHomeClientMaster::showErrorMessage(const CString& msg) {
	if (!msg.IsEmpty()) {
		this->m_errorString = msg;
		this->PostMessage(CPropertyPageNodeHomeClientMaster::WM_DISPLAY_ERRORMESSAGE);	
	}
}

void CPropertyPageNodeHomeClientMaster::AddMessage(const CString& msg,BOOL clearMessages) {
	if (NULL == m_listboxInfo.GetSafeHwnd()) {
		ASSERT(NULL != m_listboxInfo.GetSafeHwnd());
		return;
	}

	int count = 0;
	if (clearMessages) {
		m_listboxInfo.ResetContent();	
	}
	else {
		count = m_listboxInfo.GetCount();
		if (count >= 10000) {
			m_listboxInfo.ResetContent();
			count=0;
		}
	}

	CString msgNormalized = msg;
	msgNormalized.Replace(static_cast<char>('\r'), static_cast<char>(' '));
	msgNormalized.Replace(static_cast<char>('\n'), static_cast<char>(' '));
	
	const int resValue = m_listboxInfo.InsertString(count,msgNormalized);
	if (LB_ERRSPACE==resValue) {
		// Niet genoeg plaats in de listbox om string toe te voegen...
		m_listboxInfo.ResetContent();   
	}
	else if (LB_ERR != resValue) {
		count++;	
		m_listboxInfo.SetTopIndex(count-10);	
	}
}

void CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterPnp() {
	if (NULL == m_pDlgWorker) {
		return;
	}
	m_listboxInfo.ResetContent();
	if (!m_pDlgWorker->startTaskPnp()) {
		const CString msg("Plug and play could not start");
		AddMessage(msg);
	}
}

void CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterBrowse() {
	if (NULL == m_pDlgWorker) {
		return;
	}
	m_listboxInfo.ResetContent();
	if (!m_pDlgWorker->startTaskBrowse()) {
		const CString msg("Browse for nodes could not start");
		AddMessage(msg);
	}
}

void CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterBrowseclear2() {
	if (NULL == m_pDlgWorker) {
		return;
	}
	m_listboxInfo.ResetContent();
	if (!m_pDlgWorker->startTaskBrowseClear()) {
		const CString msg("Browse for nodes could not start");
		AddMessage(msg);
	}
}

void CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterUpdate() {
	if (NULL == m_pDlgWorker) {
		return;
	}
	m_listboxInfo.ResetContent();
	if (!m_pDlgWorker->startTaskUpdateDbase()) {
		const CString msg("Update database could not start");
		AddMessage(msg);
	}
}

void CPropertyPageNodeHomeClientMaster::OnBnClickedBtnMasterCancel()
{
	if (NULL == m_pDlgWorker) {
		return;
	}
	m_listboxInfo.ResetContent();
	if (!m_pDlgWorker->cancelTask()) {
		const CString msg("Failed to cancel current task");
		AddMessage(msg);
	}
}

void CPropertyPageNodeHomeClientMaster::OnBnClickedCheckConnect2() {
	if (NULL == m_pDlgWorker) {
		return;
	}
	if (m_pDlgWorker->isConnected()) {
		m_pDlgWorker->disconnect();	
		PostMessage(WM_UPDATE_CONTROLS);
		return;
	}
	ConnectionMethod_t method = CONNECTION_METHOD_UNKNOWN;
	if (0 == m_selectionSettings) {
		method = CONNECTION_METHOD_LOCAL;
	}
	else if (1 == m_selectionSettings) {
		// CHANGES_CLOUDBRIDGE_POC:
		// Commandline - optie: `-enable_tcp_cloud`
		// Bij inschakeling verloopt remote TCP / IP - communicatie via de cloudserver i.p.v.direct via DNS.
		if (CAppState::getInstance().getEnableUseRemoteTCPCloudServer()) {
			method = CONNECTION_METHOD_REMOTE_CLOUD;
		}
		else {
			method = CONNECTION_METHOD_REMOTE_DIRECT;
		}
	}
	ASSERT(CONNECTION_METHOD_UNKNOWN != method);
	if (CONNECTION_METHOD_UNKNOWN != method) {
		if (m_pDlgWorker->connectTCPHomeClient(method)) {
			AddMessage("Trying to connect to the server...", TRUE);
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);
}


void CPropertyPageNodeHomeClientMaster::OnBnClickedButtonEditcfg() {
	if (0 != m_pDlgWorker) {
		m_pDlgWorker->showCfgNodeServices( );
		PostMessage(WM_UPDATE_CONTROLS);
	}
}

void CPropertyPageNodeHomeClientMaster::OnRadioButtonsType(UINT nID) {

	UpdateData(TRUE);

	// Update GUI.
	PostMessage(WM_UPDATE_CONTROLS);	
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
CPropertyPageNodeHomeClientMaster_WorkerEvents::CPropertyPageNodeHomeClientMaster_WorkerEvents(CPropertyPageNodeHomeClientMaster& parent) :
	m_parent(parent) { }

void CPropertyPageNodeHomeClientMaster_WorkerEvents::setStateIdle() {
	m_parent.m_stateIdle = true;
	this->update();
}

void CPropertyPageNodeHomeClientMaster_WorkerEvents::setStateBusy() {	
	m_parent.m_stateIdle = false;
	this->update();
}

void CPropertyPageNodeHomeClientMaster_WorkerEvents::addInfoMsg(const CString& msg) {
	m_parent.AddMessage(msg);
}

void CPropertyPageNodeHomeClientMaster_WorkerEvents::addErrorMsg(const CString& msg) {
	m_parent.AddMessage(msg);
	m_parent.showErrorMessage(msg);	// Display error msg via assync call in messagebox.	
}

void CPropertyPageNodeHomeClientMaster_WorkerEvents::setStateConnected(bool connected) {
	m_parent.m_busyConnect = false;
	this->update();
}

void CPropertyPageNodeHomeClientMaster_WorkerEvents::update( ) {
	m_parent.PostMessage(CPropertyPageNodeHomeClientMaster::WM_UPDATE_CONTROLS);
}
/****************************************************************************/

