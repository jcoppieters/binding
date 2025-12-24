// DlgToolsRemoteCfg.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgToolsRemoteCfg.h"
#include "DlgEnterPassword.h"
#include "DlgManageServers.h"

#if (USE_TOOLS_APPREMOTECFG == 1)

#include "app/AppRemoteCfg/AppRemoteCfg.h"

// CDlgToolsRemoteCfg dialog
//////////////////////////////////////////////////////////////////////
using namespace DUOTECNO;
//////////////////////////////////////////////////////////////////////
CDlgToolsRemoteCfg* CDlgToolsRemoteCfg::m_pInstance = NULL;
//////////////////////////////////////////////////////////////////////
enum {
	// De timers...
	ID_TIMER_UPDATE_GUI	 = 1,		// Updaten van de GUI buttons.
	ID_TIMER_LISTBOX = 3,			// Updaten van de listbox.
};

enum {
	WM_TASK_ERROR = WM_USER + 1,
	WM_TASK_CHECK_ERRORSLOG
};
//////////////////////////////////////////////////////////////////////


static void dumpStrings(CListBox* pDest, CStringList& list) {
	if (list.IsEmpty()) {
		pDest->ResetContent();
	}
	else {
		int idx = 0;
		//const int prevCount = pDest->GetCount();
		for (POSITION pos = list.GetHeadPosition(); pos != NULL; ) {
			bool add = false;
			CString& src = list.GetNext(pos);

			bool addString = false;
			if (idx < pDest->GetCount()) {
				CString listBoxString;
				pDest->GetText(idx, listBoxString);
				if (0 != listBoxString.Compare(src)) {
					addString = true;
				}
			}
			else {
				addString = true;
			}
		
			if (addString) {
				const int resValue = pDest->InsertString(idx++,(LPCTSTR)src);
				if (LB_ERRSPACE == resValue) {
					pDest->ResetContent();
					idx = 0;
				}
			}			
		}
		const int count = pDest->GetCount();
		if (count > 30) { // && (count != prevCount)) {
			pDest->SetTopIndex(count - 30);
		}
	}
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgToolsRemoteCfg, CDialog)

CDlgToolsRemoteCfg::CDlgToolsRemoteCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgToolsRemoteCfg::IDD, pParent), m_fileLoaded(TRUE)
{
	m_nID = CDlgToolsRemoteCfg::IDD;
}

CDlgToolsRemoteCfg::~CDlgToolsRemoteCfg() {
	m_arrTimerHandlers.RemoveAll();	
}

void CDlgToolsRemoteCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_BINDINGS_ERASE, m_BtnBindingsErase);
	DDX_Control(pDX, IDC_BUTTON_BINDINGS_WRITE, m_BtnBindingsWrite);
	DDX_Control(pDX, IDC_BUTTON_BINDINGS_WRITE_MODIFIED, m_BtnBindingsWriteModified);	
	DDX_Control(pDX, IDC_LISTBOX_TRACER, m_listboxTracer);
	DDX_Control(pDX, IDC_BUTTON_BACKUP_UPLOAD, m_btnBackupUpload);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD_NAMES, m_BtnUpoadNames);
	DDX_Control(pDX, IDC_COMBO_SERVERS, m_comboBoxServers);
	DDX_Control(pDX, IDC_BTN_SERVERS, m_btnServers);		
}


BEGIN_MESSAGE_MAP(CDlgToolsRemoteCfg, CDialog)
	ON_BN_CLICKED(ID_BTN_EXIT, &CDlgToolsRemoteCfg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CDlgToolsRemoteCfg::OnBnClickedButtonConnect)	
	ON_BN_CLICKED(IDC_BUTTON_BINDINGS_ERASE, &CDlgToolsRemoteCfg::OnBnClickedButtonBindingsErase)
	ON_BN_CLICKED(IDC_BUTTON_BINDINGS_WRITE, &CDlgToolsRemoteCfg::OnBnClickedButtonBindingsWrite)
	ON_BN_CLICKED(IDC_BUTTON_BINDINGS_WRITE_MODIFIED, &CDlgToolsRemoteCfg::OnBnClickedButtonBindingsWriteModified)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP_UPLOAD, &CDlgToolsRemoteCfg::OnBnClickedButtonBackupUpload)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_NAMES, &CDlgToolsRemoteCfg::OnBnClickedButtonUploadNames)
	ON_WM_TIMER()
	ON_MESSAGE(WM_TASK_ERROR, onWmTaskError)
	ON_MESSAGE(WM_TASK_CHECK_ERRORSLOG, onWmTaskCheckErrorLog)
	ON_BN_CLICKED(IDC_BTN_SERVERS, &CDlgToolsRemoteCfg::OnBnClickedBtnServers)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVERS, &CDlgToolsRemoteCfg::OnCbnSelchangeComboServers)
END_MESSAGE_MAP()


// CDlgToolsRemoteCfg message handlers

CDlgToolsRemoteCfg* CDlgToolsRemoteCfg::getInstance( ) {
	if (NULL == m_pInstance) {
		m_pInstance = new CDlgToolsRemoteCfg();
		if (NULL != m_pInstance) {
			if (m_pInstance->Create() == TRUE) {				
				m_pInstance->ShowWindow(TRUE);				
			}
			else {
				delete m_pInstance;
				m_pInstance = 0;
			}
		}
	}
	ASSERT( m_pInstance );
	return m_pInstance;
}

BOOL CDlgToolsRemoteCfg::OnInitDialog() {

	const BOOL res = CDialog::OnInitDialog();
	if (res) {

		AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
		if (NULL!=pAppCfg) {

			pAppCfg->subscribeGui(this);

			m_currentSelServer = -1;

			// Updaten van de project info...
			AppRemoteCfgUtils::updateProjectFileInfo(*pAppCfg);

			// Friendly names: ComboBox.
			loadComboBoxServers();

			// De huidige selectie in de combobox zetten...
			setComboBoxSelectedserver();
		}

		m_stringList.RemoveAll();
		m_listboxTracer.ResetContent();

		// timer handlers.
		m_arrTimerHandlers.SetSize(2);
		m_arrTimerHandlers.Add(reinterpret_cast<PF_TIMER_HANDLERS>(&CDlgToolsRemoteCfg::timerhandler_UpdateGUI));
		m_arrTimerHandlers.Add(reinterpret_cast<PF_TIMER_HANDLERS>(&CDlgToolsRemoteCfg::timerhandler_UpdateListBox));

		// Updaten van de GUI.
		this->SetTimer(ID_TIMER_UPDATE_GUI,100,NULL);	
	}
	return res;
}

bool CDlgToolsRemoteCfg::isActive( ) {
	return (m_pInstance != 0 ? true : false);
}

void CDlgToolsRemoteCfg::Show(void) {
	if (NULL != m_pInstance) {		
		m_pInstance->SetActiveWindow();
	}
}

void CDlgToolsRemoteCfg::PostNcDestroy() {
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL != pAppCfg) {
		pAppCfg->subscribeGui(NULL);
	}	
	delete this;
	m_pInstance = NULL;
}

void CDlgToolsRemoteCfg::OnBnClickedBtnExit() {
	DestroyWindow( );
}
void CDlgToolsRemoteCfg::OnCancel() {
	DestroyWindow( );
}

BOOL CDlgToolsRemoteCfg::Create() { 
	return CDialog::Create(m_nID,AfxGetMainWnd());
}

CString CDlgToolsRemoteCfg::getDlgPassword(const CString& oldPassword) {
	CString password;
	CDlgEnterPassword dlg(this,oldPassword);
	const int ret = dlg.DoModal();
	if (IDOK == ret) {
		password = dlg.password();				
	}
	return password;
}


void CDlgToolsRemoteCfg::loadComboBoxServers() 
{
	m_comboBoxServers.ResetContent();

	CStringList stringList;
	if (appremotecfg_serversGetFriendlyNames(AppRemoteCfg::getInstance()->get(),&stringList)) {
		int index = 0;
		if (stringList.GetSize()>0) {
			POSITION pos = stringList.GetHeadPosition();
			while(pos!=NULL) {
				CString& name = stringList.GetNext(pos);
				m_comboBoxServers.InsertString(index,name);
				++index;
			}
		}
	}
}

void CDlgToolsRemoteCfg::setComboBoxSelectedserver() {
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}

	const int nrOfEntries = m_comboBoxServers.GetCount();
	if (nrOfEntries <= 0) {
		m_currentSelServer = -1;		
	}
	else {
		if (!appremoteCfg_serversGetIndexConnectedServer(AppRemoteCfg::getInstance()->get(),&m_currentSelServer)) {
			// Indien niet geconnecteerd moeten altij de eerste tonen...
			m_currentSelServer = 0;
		}
	}
	m_comboBoxServers.SetCurSel(m_currentSelServer);	
}

// Handlers...
void CDlgToolsRemoteCfg::OnBnClickedButtonConnect()
{
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}
	
	if (pAppCfg->connected()) {
		pAppCfg->setDisconect();
	}
	else {

		AppRemoteCfgServer* pServer = NULL;
		if (appremoteCfg_serversGetIndex(pAppCfg->get(),m_currentSelServer,&pServer)) {		
			const CString strHostName = pServer->getHostName();
			const int port = pServer->getHostPort();
			const bool secured = pServer->getSecured();
			const CString pwd = pServer->getPassword();
					
			IAppRemoteCfgApi::Result_t res = IAppRemoteCfgApi::RESULT_EC_FAILED;
			res = pAppCfg->setConnect((LPCSTR)strHostName, port, secured);
			if (IAppRemoteCfgApi::RESULT_EC_AUTH_PASSWORD == res) {		
				const CString newPassword = getDlgPassword(pwd);
				if (newPassword.IsEmpty()) {
					return;
				}

				// Updaten server info: password...
				appremotecfg_serversUpdatePassword(pAppCfg->get(),m_currentSelServer,newPassword);
				res = pAppCfg->setConnect((LPCSTR)strHostName, port, secured, (LPCSTR) newPassword);
			}		
		}	
	}

	// Updaten van de GUI.
	this->SetTimer(ID_TIMER_UPDATE_GUI,100,NULL);		
}

void CDlgToolsRemoteCfg::OnBnClickedButtonBindingsErase()
{
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}

	if (!pAppCfg->startBindingsErase()) {
		AfxMessageBox("Failed to start task");
	}
}

void CDlgToolsRemoteCfg::OnBnClickedButtonBindingsWrite()
{
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}

	if (!pAppCfg->startBindingsWrite(false)) {
		AfxMessageBox("Failed to start task");
	}
}

void CDlgToolsRemoteCfg::OnBnClickedButtonBindingsWriteModified() {
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}
	pAppCfg->startBindingsWrite(true);
}


void CDlgToolsRemoteCfg::OnBnClickedButtonBackupUpload()
{
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}
	if (!pAppCfg->startBackupUpload()) {
		AfxMessageBox("Failed to start task");
	}
}

void CDlgToolsRemoteCfg::OnBnClickedButtonUploadNames() {
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}
	if (!pAppCfg->startUploadNames()) {
		AfxMessageBox("Failed to start task");
	}
}

void CDlgToolsRemoteCfg::OnTimer(UINT_PTR nIDEvent)
{
	const int length = m_arrTimerHandlers.GetSize();
	for (int i = 0; i<length; ++i) {
		PF_TIMER_HANDLERS pf = m_arrTimerHandlers[i];
		if (0 != pf) {
			if ((this->*pf)(nIDEvent)) {
				break; // break for loop...
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgToolsRemoteCfg::OnCbnSelchangeComboServers()
{
	m_currentSelServer = m_comboBoxServers.GetCurSel();
}


// IAppRemoteCfgGUI
void CDlgToolsRemoteCfg::addMessage(const char* msg, bool eraseOld) {
	if (eraseOld) {
		m_stringList.RemoveAll();
		m_listboxTracer.ResetContent();
	}
	m_stringList.AddTail(msg);
	this->SetTimer(ID_TIMER_LISTBOX, 1, NULL);
}

void CDlgToolsRemoteCfg::addMessages(CStringList& messages, bool eraseOld) {
	if (eraseOld) {
		m_stringList.RemoveAll();
		m_listboxTracer.ResetContent();
	}
	m_stringList.AddTail(const_cast<CStringList*>(&messages));
	this->SetTimer(ID_TIMER_LISTBOX, 1, NULL);
}


void CDlgToolsRemoteCfg::taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
		HttpError_t errorCode, const char* msgError)
{
	this->SetTimer(ID_TIMER_UPDATE_GUI, 100, NULL);

	if (HTTP_EC_NONE != errorCode) {
		this->PostMessage(WM_TASK_ERROR, errorCode, 0);
	}

	if (status == APPREMOTE_CFG_TASK_STATUS_STARTED) {
	
	} 
	else if (status == APPREMOTE_CFG_TASK_STATUS_DONE ||
			 status == APPREMOTE_CFG_TASK_STATUS_FAILED) 
	{
		this->PostMessage(WM_TASK_CHECK_ERRORSLOG, task, 0);
	}
}

// IAppRemoteCfgGUI end...

LONG CDlgToolsRemoteCfg::onWmTaskError(WPARAM wParam, LPARAM lParam)
{
	const HttpError_t error = static_cast<HttpError_t>(wParam);
	switch (error) {
		case HTTP_EC_CONNECT: {
			AfxMessageBox("Could not connect. Server offline ?", MB_ICONEXCLAMATION);
		} break;
		case HTTP_EC_AUTHORIZATION: {
			AfxMessageBox("Could not connect. Authorization error: Wrong password or incorrect refresh token.Retry login via password.", MB_ICONEXCLAMATION);
		} break;
	}
	return TRUE;
}

LONG CDlgToolsRemoteCfg::onWmTaskCheckErrorLog(WPARAM wParam, LPARAM lParam) {
	
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return true;
	}

	AppRemoteCfgTasks_t task = static_cast<AppRemoteCfgTasks_t>(wParam);

	const CStringList* pErrorList = 0;
	if (pAppCfg->getLogErrors(&pErrorList)) {
		if (0 != pErrorList && pErrorList->GetCount() > 0) {
			addMessages(const_cast<CStringList&>(*pErrorList), true);
			AfxMessageBox("errors happened while executing the task, check listbox.");
		}
	}
	return TRUE;
}

	// Timer Handlers...
bool CDlgToolsRemoteCfg::timerhandler_UpdateGUI(UINT_PTR nIDEvent) {
	if (ID_TIMER_UPDATE_GUI != nIDEvent) {
		return false;
	}

	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return true;
	}

	const bool connected = pAppCfg->connected();
	bool enableBtnBindingsErase = false;
	bool enableBtnBindingsWrite = false;
	bool enableBtnUpload = false;
	bool enableBtnUploadNames = false;

	if (m_fileLoaded && connected) {
		
		const AppRemoteCfgServerInfo* serverInfo = 0;
		if (pAppCfg->getServerInfo(&serverInfo)) {
			ASSERT(0 != serverInfo);
			const AppRcfgSupportedFlags flags = AppRemoteCfgServerInfo_Utils::getSupportedFlags(*serverInfo);
			enableBtnBindingsErase = AppRemoteCfgServerInfo_Utils::supportedCfgBindingsErase(flags);
			enableBtnBindingsWrite = AppRemoteCfgServerInfo_Utils::supportedCfgBindingsWrite(flags);
			enableBtnUpload = AppRemoteCfgServerInfo_Utils::supportedCfgUploadBackup(flags);
			enableBtnUploadNames = AppRemoteCfgServerInfo_Utils::supportedCfgNamesWrite(flags);
		}

		m_btnConnect.SetWindowText("Disconnect");
		
	}
	else {
		m_btnConnect.SetWindowText("Connect");
	}

	m_BtnBindingsErase.EnableWindow(enableBtnBindingsErase);
	m_BtnBindingsWrite.EnableWindow(enableBtnBindingsWrite);
	m_BtnBindingsWriteModified.EnableWindow(enableBtnBindingsWrite);
	m_btnBackupUpload.EnableWindow(enableBtnUpload);
	m_btnConnect.EnableWindow(m_fileLoaded && m_currentSelServer >= 0);
	m_BtnUpoadNames.EnableWindow(enableBtnUploadNames);
	m_comboBoxServers.EnableWindow(!connected);
	m_btnServers.EnableWindow(!connected);
	
	this->KillTimer(ID_TIMER_UPDATE_GUI);
	return true;
}

bool CDlgToolsRemoteCfg::timerhandler_UpdateListBox(UINT_PTR nIDEvent) {
	if (ID_TIMER_LISTBOX != nIDEvent) {
		return false;
	}
	// Updaten van de strings in de listBox...
	dumpStrings(&m_listboxTracer, m_stringList);
	this->KillTimer(ID_TIMER_LISTBOX);
	return true;
}
void CDlgToolsRemoteCfg::OnBnClickedBtnServers()
{
	AppRemoteCfg* const pAppCfg = AppRemoteCfg::getInstance();
	if (NULL == pAppCfg) {
		ASSERT(NULL!=pAppCfg);
		return;
	}
	CDlgManageServers dlg(this,*pAppCfg->get());
	dlg.DoModal();

	// ComboBox opnieuw updaten...
	loadComboBoxServers();
	setComboBoxSelectedserver();

	// Updaten van de GUI: 
	// Nodig om de connect te enablen wanneer er een server aangemaakt werd.
	this->SetTimer(ID_TIMER_UPDATE_GUI,100,NULL);		
}

#endif