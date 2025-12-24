
// AppRemoteCfgMfcWinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppRemoteCfgMfcWin.h"
#include "AppRemoteCfgMfcWinDlg.h"
#include "DlgEnterPassword.h"
#include "DlgManageServers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace DUOTECNO;

enum {
	// De timers...
	ID_TIMER_UPDATE_GUI	 = 1,		// Updaten van de GUI buttons.
	ID_TIMER_POLL = 2,				// Pollen van de taken...
	ID_TIMER_LISTBOX = 3,

	// De timer interval waarden.
	TMR_INTERVAL_POLL_MSEC = 50,
};

enum {
	WM_TASK_ERROR = WM_USER + 1
};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAppRemoteCfgMfcWinDlg dialog



CAppRemoteCfgMfcWinDlg::CAppRemoteCfgMfcWinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppRemoteCfgMfcWinDlg::IDD, pParent), m_pAppRemoteCfg(0), m_fileLoaded(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	

	m_pAppRemoteCfg = appremotecfg_create();
	if (0 != m_pAppRemoteCfg) {
		m_pAppRemoteCfg->setSubscriberGui(this);
	}
}

CAppRemoteCfgMfcWinDlg::~CAppRemoteCfgMfcWinDlg( ) {	
	m_arrTimerHandlers.RemoveAll();	

	appremotecfg_destroy(&m_pAppRemoteCfg);
}

void CAppRemoteCfgMfcWinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_BINDINGS_ERASE, m_BtnBindingsErase);
	DDX_Control(pDX, IDC_BUTTON_BINDINGS_WRITE, m_BtnBindingsWrite);
	DDX_Control(pDX, IDC_BUTTON_TEST01, m_btnTest1);
	DDX_Control(pDX, IDC_BUTTON_TEST02, m_btnTest2);
	DDX_Control(pDX, IDC_LISTBOX_TRACER, m_listboxTracer);
	DDX_Control(pDX, IDC_BUTTON_BACKUP_UPLOAD, m_btnBackupUpload);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_btnOpenProject);
	DDX_Control(pDX, IDC_COMBO_SERVERS, m_comboBoxServers);
	DDX_Control(pDX, IDC_BUTTON_SERVERS, m_btnServers);
}

BEGIN_MESSAGE_MAP(CAppRemoteCfgMfcWinDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_BINDINGS_ERASE, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonBindingsErase)
	ON_BN_CLICKED(IDC_BUTTON_BINDINGS_WRITE, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonBindingsWrite)
	ON_BN_CLICKED(IDC_BUTTON_TEST01, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonTest01)
	ON_BN_CLICKED(IDC_BUTTON_TEST02, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonTest02)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP_UPLOAD, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonBackupUpload)
	ON_WM_TIMER()
	ON_MESSAGE(WM_TASK_ERROR, onWmTaskError)
	ON_BN_CLICKED(IDC_BUTTON_SERVERS, &CAppRemoteCfgMfcWinDlg::OnBnClickedButtonServers)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVERS, &CAppRemoteCfgMfcWinDlg::OnCbnSelchangeComboServers)
END_MESSAGE_MAP()


// CAppRemoteCfgMfcWinDlg message handlers

BOOL CAppRemoteCfgMfcWinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	m_currentSelServer = -1;

	m_stringList.RemoveAll();
	m_listboxTracer.ResetContent();

	// timer handlers.
	m_arrTimerHandlers.SetSize(3);
	m_arrTimerHandlers.Add(reinterpret_cast<PF_TIMER_HANDLERS>(&CAppRemoteCfgMfcWinDlg::timerhandler_UpdateGUI));
	m_arrTimerHandlers.Add(reinterpret_cast<PF_TIMER_HANDLERS>(&CAppRemoteCfgMfcWinDlg::timerhandler_UpdateListBox));
	m_arrTimerHandlers.Add(reinterpret_cast<PF_TIMER_HANDLERS>(&CAppRemoteCfgMfcWinDlg::timerhandler_pollFsm));

	// Updaten van de GUI.
	this->SetTimer(ID_TIMER_UPDATE_GUI,100,NULL);	
	this->SetTimer(ID_TIMER_POLL, TMR_INTERVAL_POLL_MSEC, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAppRemoteCfgMfcWinDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAppRemoteCfgMfcWinDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppRemoteCfgMfcWinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// IAppRemoteCfgGUI
void CAppRemoteCfgMfcWinDlg::addMessage(const char* msg, bool eraseOld) {
	if (eraseOld) {
		m_stringList.RemoveAll();
		m_listboxTracer.ResetContent();
	}
	m_stringList.AddTail(msg);
	this->SetTimer(ID_TIMER_LISTBOX, 1, NULL);
}
void CAppRemoteCfgMfcWinDlg::addMessages(CStringList& messages, bool eraseOld) {
	if (eraseOld) {
		m_stringList.RemoveAll();
		m_listboxTracer.ResetContent();
	}
	m_stringList.AddTail(const_cast<CStringList*>(&messages));
	this->SetTimer(ID_TIMER_LISTBOX, 1, NULL);
}


void CAppRemoteCfgMfcWinDlg::taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
	HttpError_t errorCode, const char* msgError) 
{
	// Verandering van toestand altijd de gui updaten...
	this->SetTimer(ID_TIMER_UPDATE_GUI, 100, NULL);

	// Fout doorgeven...
	if (HTTP_EC_NONE != errorCode) {
		this->PostMessage(WM_TASK_ERROR, errorCode, 0);
	}
}

// IAppRemoteCfgGUI tot hier...

LONG CAppRemoteCfgMfcWinDlg::onWmTaskError(WPARAM wParam, LPARAM lParam)
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

CString CAppRemoteCfgMfcWinDlg::getDlgPassword(const CString& oldPassword) {
	CString password;
	CDlgEnterPassword dlg(this, oldPassword);
	const int ret = dlg.DoModal();
	if (IDOK == ret) {
		password = dlg.password();				
	}
	return password;
}


void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonConnect()
{
	ASSERT(0 != m_pAppRemoteCfg);
	if (m_pAppRemoteCfg->connected()) {
		m_pAppRemoteCfg->disConnect();
	}
	else {
		ASSERT(0 != m_pAppRemoteCfg);
		AppRemoteCfgServer* pServer = NULL;
		if (appremoteCfg_serversGetIndex(m_pAppRemoteCfg, m_currentSelServer, &pServer)) {
			const CString strHostName = pServer->getHostName();
			const int port = pServer->getHostPort();
			const bool secured = pServer->getSecured();
			const CString pwd = pServer->getPassword();

			IAppRemoteCfgApi::Result_t res = IAppRemoteCfgApi::RESULT_EC_FAILED;
			res = m_pAppRemoteCfg->setHostInfo((LPCSTR)strHostName, port, secured, (LPCSTR)pwd);
			if (IAppRemoteCfgApi::RESULT_EC_AUTH_PASSWORD == res) {
				const CString newPassword = getDlgPassword(pwd);
				if (newPassword.IsEmpty()) {
					return;
				}
				// Updaten server info: password...
				appremotecfg_serversUpdatePassword(m_pAppRemoteCfg, m_currentSelServer, newPassword);		

				// Er zijn geen client credentials gevonden voor deze server.
				// Opnieuw proberen en een paswoord meegegeven.
				res = m_pAppRemoteCfg->setHostInfo((LPCSTR)strHostName, port, secured, (LPCSTR)pwd);
			}

			if (IAppRemoteCfgApi::RESULT_SUCCESS == res) {
				m_pAppRemoteCfg->startTask(APPREMOTE_CFG_TASK_CONNECT, 0);
			}
		}
	}
	// Updaten van de GUI.
	this->SetTimer(ID_TIMER_UPDATE_GUI,100,NULL);		
}


void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonOpen()
{
	CFileDialog dlgFileOpen(TRUE, _T("duo"), _T("*.duo"),
							OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
							_T("Duotecno Files (*.duo)|*.duo||"),
							NULL); 

	if (dlgFileOpen.DoModal() == IDOK) 
	{		
		
		CString PathName = dlgFileOpen.GetPathName();		// Volledige naam 		
		CString fileName = dlgFileOpen.GetFileName();		// Naam van het bestand.
		CString folderName = dlgFileOpen.GetFolderPath();	// Naam van de folder.
	
		// Working dir zetten: Hier zijn de binding bestanden aanwezig.		
		m_pAppRemoteCfg->setProjectInfo((LPCSTR)folderName, (LPCSTR) fileName);

		m_fileLoaded = TRUE;

		m_currentSelServer = -1;

		// Friendly names: ComboBox.
		loadComboBoxServers();

		// De huidige selectie in de combobox zetten...
		setComboBoxSelectedserver();

		// De title aanpassen.
		this->SetWindowText(fileName);

		// Updaten van de GUI.
		this->SetTimer(ID_TIMER_UPDATE_GUI,100,NULL);	
	}
}

void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonBindingsErase()
{
	ASSERT(0 != m_pAppRemoteCfg);
	const char* const projectDir = m_pAppRemoteCfg->getProjectDir();
	if (0 != projectDir) {
		if (!appremotecfg_startTaskPrjBindingsErase(m_pAppRemoteCfg, projectDir)) {
			AfxMessageBox("Failed to start task");
		}
	}
}

void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonBindingsWrite()
{
	ASSERT(0 != m_pAppRemoteCfg);
	const char* const projectDir = m_pAppRemoteCfg->getProjectDir();
	if (0 != projectDir) {
		if (!appremotecfg_startTaskPrjBindingsWrite(m_pAppRemoteCfg, projectDir)) {
			AfxMessageBox("Failed to start task");
		}
	}
}

void CAppRemoteCfgMfcWinDlg::OnCbnSelchangeComboServers()
{
	m_currentSelServer = m_comboBoxServers.GetCurSel();
}

void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonTest01()
{
	ASSERT(0 != m_pAppRemoteCfg);
	if (!appremotecfg_test_startTaskNamesWrite(m_pAppRemoteCfg)) {
		AfxMessageBox("Failed to start task: Test write names.");
	}
}

void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonTest02()
{
	// TODO: Add your control notification handler code here
}

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
				const int resValue = pDest->InsertString(idx++, src);
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

void CAppRemoteCfgMfcWinDlg::OnTimer(UINT_PTR nIDEvent)
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

// timer Handlers...
bool CAppRemoteCfgMfcWinDlg::timerhandler_UpdateGUI(UINT_PTR nIDEvent) {
	if (ID_TIMER_UPDATE_GUI != nIDEvent) {
		return false;
	}

	ASSERT(0 != m_pAppRemoteCfg);

	const bool connected = m_pAppRemoteCfg->connected();
	bool enableBtnBindingsErase = false;
	bool enableBtnBindingsWrite = false;
	bool enableBtnTests = false;
	bool enableBtnUpload = false;

	if (m_fileLoaded && connected) {
		
		const AppRemoteCfgServerInfo* serverInfo = 0;
		if (m_pAppRemoteCfg->getServerInfo(&serverInfo)) {
			ASSERT(0 != serverInfo);
			const AppRcfgSupportedFlags flags = AppRemoteCfgServerInfo_Utils::getSupportedFlags(*serverInfo);
			enableBtnBindingsErase = AppRemoteCfgServerInfo_Utils::supportedCfgBindingsErase(flags);
			enableBtnBindingsWrite = AppRemoteCfgServerInfo_Utils::supportedCfgBindingsWrite(flags);
			enableBtnUpload = AppRemoteCfgServerInfo_Utils::supportedCfgUploadBackup(flags);
		}
		enableBtnTests = true;

		m_btnConnect.SetWindowText("Disconnect");
	}
	else {
		m_btnConnect.SetWindowText("Connect");
	}

	m_comboBoxServers.EnableWindow(!connected);
	m_btnServers.EnableWindow(!connected && m_fileLoaded);
	m_btnConnect.EnableWindow(m_currentSelServer >= 0 ? 1 : 0);

	m_BtnBindingsErase.EnableWindow(enableBtnBindingsErase);
	m_BtnBindingsWrite.EnableWindow(enableBtnBindingsWrite);
	m_btnBackupUpload.EnableWindow(enableBtnUpload);
	m_btnConnect.EnableWindow(m_fileLoaded);
	m_btnOpenProject.EnableWindow(!m_fileLoaded || !connected);
	m_btnTest1.EnableWindow(enableBtnTests);
	m_btnTest2.EnableWindow(enableBtnTests);

	this->KillTimer(ID_TIMER_UPDATE_GUI);
	return true;
}

bool CAppRemoteCfgMfcWinDlg::timerhandler_UpdateListBox(UINT_PTR nIDEvent) {
	if (ID_TIMER_LISTBOX != nIDEvent) {
		return false;
	}

	// Updaten van de strings in de listBox...
	dumpStrings(&m_listboxTracer, m_stringList);
	this->KillTimer(ID_TIMER_LISTBOX);
	return true;
}

bool CAppRemoteCfgMfcWinDlg::timerhandler_pollFsm(UINT_PTR nIDEvent) {
	if (ID_TIMER_POLL != nIDEvent) {
		return false;
	}

	ASSERT(0!= m_pAppRemoteCfg);
	m_pAppRemoteCfg->poll(TMR_INTERVAL_POLL_MSEC);
	return true;
}

void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonBackupUpload()
{
	ASSERT(0 != m_pAppRemoteCfg);
	m_pAppRemoteCfg->startTask(APPREMOTE_CFG_TASK_UPLOADBACKUP,0);	
}

void CAppRemoteCfgMfcWinDlg::OnBnClickedButtonServers()
{
	CDlgManageServers dlg(this,*m_pAppRemoteCfg);
	dlg.DoModal();
}

void CAppRemoteCfgMfcWinDlg::loadComboBoxServers()
{
	m_comboBoxServers.ResetContent();

	CStringList stringList;
	if (appremotecfg_serversGetFriendlyNames(m_pAppRemoteCfg, &stringList)) {
		int index = 0;
		if (stringList.GetSize()>0) {
			POSITION pos = stringList.GetHeadPosition();
			while (pos != NULL) {
				CString& name = stringList.GetNext(pos);
				m_comboBoxServers.InsertString(index, name);
				++index;
			}
		}
	}
}

void CAppRemoteCfgMfcWinDlg::setComboBoxSelectedserver() {

	const int nrOfEntries = m_comboBoxServers.GetCount();
	if (nrOfEntries <= 0) {
		m_currentSelServer = -1;
	}
	else {
		if (!appremoteCfg_serversGetIndexConnectedServer(m_pAppRemoteCfg, &m_currentSelServer)) {
			// Indien niet geconnecteerd moeten altij de eerste tonen...
			m_currentSelServer = 0;
		}
	}
	m_comboBoxServers.SetCurSel(m_currentSelServer);
}
