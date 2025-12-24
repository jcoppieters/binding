
// AppFirmwareUpdateMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppFirmwareUpdateMFC.h"
#include "AppFirmwareUpdateMFCDlg.h"
#include "afxdialogex.h"
#include "AppFirmwareUpdateWorker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAppFirmwareUpdateMFCDlg dialog
enum { WM_UPDATE_CONTROLS  = WM_USER + 1};

CAppFirmwareUpdateMFCDlg::CAppFirmwareUpdateMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_APPFIRMWAREUPDATEMFC_DIALOG, pParent), m_pWorker(NULL), m_updateFirmwareInfo(false), m_showExpertMode(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAppFirmwareUpdateMFCDlg::~CAppFirmwareUpdateMFCDlg() {
}



void CAppFirmwareUpdateMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_NODE_REFRESH, m_btnRefreshNodeInfo);
	DDX_Control(pDX, IDC_STATIC_FIRMWARE_CURRENT, m_staticCurrentVersion);
	DDX_Control(pDX, IDC_EDIT_FIRMWARE_CURRENT, m_editCurrentVersion);
	DDX_Control(pDX, IDC_NODE_IPADDRESS, m_ctrlHostAddress);
	DDX_Control(pDX, IDC_STATIC_FIRMWARE_RELEASE, m_staticRelease);
	DDX_Control(pDX, IDC_EDIT_RELEASE, m_editRelease);
	DDX_Control(pDX, IDC_STATIC_FIRMWARE_BETA, m_staticBeta);
	DDX_Control(pDX, IDC_EDIT_BETA, m_editBeta);
	DDX_Control(pDX, IDC_BUTTON_RELEASE, m_btnRelease);
	DDX_Control(pDX, IDC_BUTTON_BETA, m_btnBeta);
	DDX_Control(pDX, IDC_LISTBOX_PROGRESS, m_listboxProgress);
	DDX_Control(pDX, IDC_STATIC_VERSIONS, m_staticGroupFirmware);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_editStatus);
	DDX_Control(pDX, IDC_STATIC_PRODUCT, m_staticAdvOptionsProduct);
	DDX_Control(pDX, IDC_STATIC_PLATFORM, m_staticAdvOptionsPlatform);
	DDX_Control(pDX, IDC_COMBO_PRODUCT, m_comboAdvOptionsProduct);
	DDX_Control(pDX, IDC_COMBO_PLATFORM, m_comboAdvOptionsPlatform);
	DDX_Control(pDX, IDC_CHECK_SHOW_ADVANCED, m_btnCheckShowAdvOptions);
	DDX_Control(pDX, IDC_CHECK_SHOW_BETA, m_btnCheckShowBetaVersions);
	DDX_Control(pDX, IDC_CHECK_SHOW_DOWNGRADE, m_btnCheckShowDowngradeOpt);
	DDX_Control(pDX, IDC_EDIT_INFO_FIRMWARE, m_editInfoVersions);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PRODUCT, m_staticCurrProduct);
	DDX_Control(pDX, IDC_EDIT_CURRENT_PRODUCT, m_editCurrProduct);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PLATFORM, m_staticCurrPlatform);
	DDX_Control(pDX, IDC_EDIT_CURRENT_PLATFORM, m_editCurrPlatform);
}

BEGIN_MESSAGE_MAP(CAppFirmwareUpdateMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_NODE_REFRESH, &CAppFirmwareUpdateMFCDlg::OnBnClickedButtonNodeRefresh)
	ON_MESSAGE(WM_UPDATE_CONTROLS, onWmUpdateControls)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RELEASE, &CAppFirmwareUpdateMFCDlg::OnBnClickedButtonRelease)
	ON_BN_CLICKED(IDC_BUTTON_BETA, &CAppFirmwareUpdateMFCDlg::OnBnClickedButtonBeta)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_CHECK_SHOW_ADVANCED, &CAppFirmwareUpdateMFCDlg::OnBnClickedCheckShowAdvanced)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT, &CAppFirmwareUpdateMFCDlg::OnCbnSelchangeComboProduct)
	ON_CBN_SELCHANGE(IDC_COMBO_PLATFORM, &CAppFirmwareUpdateMFCDlg::OnCbnSelchangeComboPlatform)
	ON_CBN_DROPDOWN(IDC_COMBO_PRODUCT, &CAppFirmwareUpdateMFCDlg::OnCbnDropdownComboProduct)
	ON_CBN_CLOSEUP(IDC_COMBO_PRODUCT, &CAppFirmwareUpdateMFCDlg::OnCbnCloseupComboProduct)
	ON_CBN_DROPDOWN(IDC_COMBO_PLATFORM, &CAppFirmwareUpdateMFCDlg::OnCbnDropdownComboPlatform)
	ON_CBN_CLOSEUP(IDC_COMBO_PLATFORM, &CAppFirmwareUpdateMFCDlg::OnCbnCloseupComboPlatform)
	ON_BN_CLICKED(IDC_CHECK_SHOW_BETA, &CAppFirmwareUpdateMFCDlg::OnBnClickedCheckShowBeta)
	ON_BN_CLICKED(IDC_CHECK_SHOW_DOWNGRADE, &CAppFirmwareUpdateMFCDlg::OnBnClickedCheckShowDowngrade)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_NODE_IPADDRESS, &CAppFirmwareUpdateMFCDlg::OnIpnFieldchangedIpaddress1)
END_MESSAGE_MAP()


static AppFirmwareUpdaterWorker::HardwarePlatform_t getHardwarePlatform(CComboBox& refComboHardware) {
	AppFirmwareUpdaterWorker::HardwarePlatform_t platform = AppFirmwareUpdaterWorker::HARDWARE_UNKNOWN;
	const int sel = refComboHardware.GetCurSel();
	if (sel >= 0) {
		platform = (AppFirmwareUpdaterWorker::HardwarePlatform_t) refComboHardware.GetItemData(sel);
	}
	return platform;
}

static AppFirmwareUpdaterWorker::Product_t getProduct(CComboBox& refComboProduct) {
	AppFirmwareUpdaterWorker::Product_t productType = AppFirmwareUpdaterWorker::PRODUCT_UNKNOWN;
	const int sel = refComboProduct.GetCurSel();
	if (sel >= 0) {
		productType = (AppFirmwareUpdaterWorker::Product_t) refComboProduct.GetItemData(sel);
	}
	return productType;
}

// CAppFirmwareUpdateMFCDlg message handlers

void CAppFirmwareUpdateMFCDlg::init(AppFirmwareUpdaterWorker& refWorker, const CString& strIpAddress) {
	m_pWorker = &refWorker;
	m_hostAddress = strIpAddress;
}

BOOL CAppFirmwareUpdateMFCDlg::OnInitDialog()
{
	ASSERT(NULL != m_pWorker);

	CDialogEx::OnInitDialog();

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


	// Initialisatie...
	m_guiFlags = 0;
	m_updateFirmwareInfo = true;
	this->SetTimer(TIMER_ID_POLL, TIMER_POLL_VALUE_MSEC, (TIMERPROC)NULL);

	if (!m_hostAddress.IsEmpty()) {
		int v[4] = { 127, 0, 0, 1};
		if (4 == sscanf((LPCSTR)m_hostAddress,"%d.%d.%d.%d",&v[0],&v[1],&v[2],&v[3])) {
			m_ctrlHostAddress.SetAddress(v[0], v[1], v[2], v[3]);
		}
	}

	m_listboxProgress.ResetContent();
	m_errorMsg.Empty();
	m_infoMsg.Empty();
	m_statusMsg.Empty();

	initComboPlatform(AppFirmwareUpdaterWorker::HARDWARE_UNKNOWN);
	initComboProduct(AppFirmwareUpdaterWorker::PRODUCT_UNKNOWN);

	m_showExpertMode = true;		// Advanced opties - Momenteel op 1	
	m_pWorker->init(m_hostAddress);
	PostMessage(WM_UPDATE_CONTROLS);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAppFirmwareUpdateMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAppFirmwareUpdateMFCDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppFirmwareUpdateMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAppFirmwareUpdateMFCDlg::updateGuiListboxMessage(const CString& msg, bool eraseContent) {
	if (eraseContent) {
		m_listboxProgress.ResetContent();
	}
	const int count = m_listboxProgress.GetCount();
	if (!msg.IsEmpty()) {
		m_listboxProgress.InsertString(count,msg);
		if (count > 30) { 
			m_listboxProgress.SetTopIndex(count - 30);
		}
	}
}

void CAppFirmwareUpdateMFCDlg::initComboPlatform(AppFirmwareUpdaterWorker::HardwarePlatform_t platform) {
	m_comboAdvOptionsPlatform.InsertString(AppFirmwareUpdaterWorker::HARDWARE_RPI3, "CM3/CM3+");
	m_comboAdvOptionsPlatform.SetItemData(AppFirmwareUpdaterWorker::HARDWARE_RPI3, AppFirmwareUpdaterWorker::HARDWARE_RPI3 );

	m_comboAdvOptionsPlatform.InsertString(AppFirmwareUpdaterWorker::HARDWARE_RPI1, "RPI1");
	m_comboAdvOptionsPlatform.SetItemData(AppFirmwareUpdaterWorker::HARDWARE_RPI1, AppFirmwareUpdaterWorker::HARDWARE_RPI1 );

	m_comboAdvOptionsPlatform.InsertString(AppFirmwareUpdaterWorker::HARDWARE_CM4S, "CM4S");
	m_comboAdvOptionsPlatform.SetItemData(AppFirmwareUpdaterWorker::HARDWARE_CM4S, AppFirmwareUpdaterWorker::HARDWARE_CM4S);

	if (AppFirmwareUpdaterWorker::HARDWARE_UNKNOWN == platform) {
		m_comboAdvOptionsPlatform.SetCurSel(0);
	}
	else {
		m_comboAdvOptionsPlatform.SetCurSel((int)platform);
	}
}

void CAppFirmwareUpdateMFCDlg::initComboProduct(AppFirmwareUpdaterWorker::Product_t product) {
	m_comboAdvOptionsProduct.InsertString(AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN, "Touchscreen 100 Virtuals");
	m_comboAdvOptionsProduct.SetItemData(AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN, AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN);

	m_comboAdvOptionsProduct.InsertString(AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL, "AVMatrix IP + 8Channel IRTX");
	m_comboAdvOptionsProduct.SetItemData(AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL, AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL);

	m_comboAdvOptionsProduct.InsertString(AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO, "TCP/IP homeserver");
	m_comboAdvOptionsProduct.SetItemData(AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO, AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO);

	if (AppFirmwareUpdaterWorker::PRODUCT_UNKNOWN == product) {
		m_comboAdvOptionsProduct.SetCurSel(0);
	}
	else {
		m_comboAdvOptionsProduct.SetCurSel((int)product);
	}
}


static bool getHostAddress(CIPAddressCtrl& refCtrl,CString* pDestString) {
	BYTE v[4];
	if (4 == refCtrl.GetAddress(v[0], v[1], v[2], v[3])) {
		pDestString->Format("%d.%d.%d.%d", v[0], v[1], v[2], v[3]);
		return true;
	}
	return false;
}


void CAppFirmwareUpdateMFCDlg::OnBnClickedButtonNodeRefresh()
{
	CString strHostAddress;
	if (getHostAddress(m_ctrlHostAddress,&strHostAddress)) {
		if (m_pWorker) {
			updateGuiListboxMessage("Checking node firmware version.", true);
			m_pWorker->startRequestNodeInfo(strHostAddress);
			m_updateFirmwareInfo = true;
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

void CAppFirmwareUpdateMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent) {
		case TIMER_ID_POLL: {							
			if (NULL != m_pWorker) {
				m_pWorker->poll(TIMER_POLL_VALUE_MSEC);																
			}			
			this->SetTimer(TIMER_ID_POLL, TIMER_POLL_VALUE_MSEC,(TIMERPROC)NULL);
			PostMessage(WM_UPDATE_CONTROLS);
		} break;
		case TIMER_ID_TASK_START: {
			if (NULL != m_pWorker) {
				m_pWorker->startAssyncFsmTask();
			}
			this->KillTimer(TIMER_ID_TASK_START);
		} break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

static CString getBtnText(int currentVersion,int cloudVersion) {
	CString retValue;
	if (cloudVersion > currentVersion) {
		retValue = "Upgrade";
	}
	else if (cloudVersion < currentVersion) {
		retValue = "Downgrade";
	}
	return retValue;
}

void CAppFirmwareUpdateMFCDlg::updateGUIFlags() {

	m_guiFlags = 0;

	BYTE v[4];
	if (4 == m_ctrlHostAddress.GetAddress(v[0], v[1], v[2], v[3])) {
		m_guiFlags |= GUI_UPDATE_VALIDHOST_ADDRESS;
	}

	if (m_btnCheckShowAdvOptions.GetCheck()) {
		m_guiFlags |= GUI_UPDATE_ADVANCED_OPTIONS_SELECTED;
	}

	if (NULL != m_pWorker) {
		if (m_pWorker->getReceivedNodeInfo()) {
			m_guiFlags |= GUI_UPDATE_NODE_CONNECTED;
		}
		if (m_pWorker->recievedCloudFirmwareVersions()) {
			m_guiFlags |= GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS;
		}
		if (m_pWorker->busy())
			m_guiFlags |= GUI_UPDATE_WORKERFSM_BUSY;
	}
	if (m_pWorker->getReceivedNodeInfo() && m_pWorker->recievedCloudFirmwareVersions()) {
		m_updateFirmwareInfo = true;
	}
}

// Updaten van de firmware versies info + de advanced opties.
// Wanneer de optie advanced aangevinkt is dan worden de versies getoond ifv de combobox selecties.
// Anders worden de versies getoond ifv de (geconnecteerde) node info.
void CAppFirmwareUpdateMFCDlg::updateGuiFirmwareAvailableInfo() {	
	const bool visible = (GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS == (m_guiFlags & GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS));
	const bool fsmBusy = (GUI_UPDATE_WORKERFSM_BUSY == (m_guiFlags & GUI_UPDATE_WORKERFSM_BUSY));
	const bool fRecievedFirmwareVersions = (GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS == (m_guiFlags & GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS));
	const bool fAdvancedOptions = (GUI_UPDATE_ADVANCED_OPTIONS_SELECTED == (m_guiFlags & GUI_UPDATE_ADVANCED_OPTIONS_SELECTED));

	m_staticGroupFirmware.ShowWindow(visible);
	m_staticRelease.ShowWindow(visible);
	m_editRelease.ShowWindow(visible);
	m_staticBeta.ShowWindow(visible);
	m_editBeta.ShowWindow(visible);
	m_editRelease.EnableWindow(visible);		
	m_editBeta.EnableWindow(visible);		

	m_btnCheckShowAdvOptions.EnableWindow(m_showExpertMode && !fsmBusy);		// Enkel in expert view mode ???
	m_comboAdvOptionsPlatform.EnableWindow(fAdvancedOptions);
	m_comboAdvOptionsProduct.EnableWindow(fAdvancedOptions);

	m_btnCheckShowBetaVersions.ShowWindow(visible);
	m_btnCheckShowBetaVersions.EnableWindow(!fsmBusy);
	m_btnCheckShowDowngradeOpt.ShowWindow(visible);
	m_btnCheckShowDowngradeOpt.EnableWindow(!fAdvancedOptions && !fsmBusy);
	m_editInfoVersions.ShowWindow(visible);

	m_btnCheckShowAdvOptions.ShowWindow(visible);
	m_staticAdvOptionsProduct.ShowWindow(visible && fAdvancedOptions);
	m_staticAdvOptionsPlatform.ShowWindow(visible && fAdvancedOptions);
	m_comboAdvOptionsProduct.ShowWindow(visible && fAdvancedOptions);
	m_comboAdvOptionsPlatform.ShowWindow(visible && fAdvancedOptions);

	if (!visible) { 
		m_btnRelease.ShowWindow(0);
		m_btnBeta.ShowWindow(0);
	}

	if (fRecievedFirmwareVersions) {
		if (m_updateFirmwareInfo) {

			CString strRelease;
			CString strBeta;

			if (fAdvancedOptions) {			
				// De Optie advanced is geselecteerd.
				// De GUI wordt upgedate voklgens de huidige selectie van de combo boxes.			
				int nrOfVersionsAvailable = 0;
				const AppFirmwareUpdaterWorker::HardwarePlatform_t platform = getHardwarePlatform(m_comboAdvOptionsPlatform);
				const AppFirmwareUpdaterWorker::Product_t productType = getProduct(m_comboAdvOptionsProduct);
				const AppRemoteCfgFirmwareVersion* pVersionRelease = NULL;
				const AppRemoteCfgFirmwareVersion* pVersionBeta = NULL;
				if (!m_pWorker->getFirmwareVersion(&pVersionRelease, platform, productType, AppFirmwareUpdaterWorker::VERSION_RELEASE)) {
					pVersionRelease = NULL;
				}
				if (!m_pWorker->getFirmwareVersion(&pVersionBeta, platform, productType, AppFirmwareUpdaterWorker::VERSION_BETA)) {
					pVersionBeta = NULL;
				}

				if (NULL == pVersionRelease) {
					strRelease = "N/A";
					m_btnRelease.ShowWindow(0);
				}
				else {
					strRelease = pVersionRelease->getVersion();
					m_btnRelease.SetWindowText("update");
					nrOfVersionsAvailable++;
					m_btnRelease.ShowWindow(1);
				}

				if (NULL == pVersionBeta) {
					strBeta = "N/A";
					m_btnBeta.ShowWindow(0);
				}
				else {
					strBeta = pVersionBeta->getVersion();
					m_btnBeta.SetWindowText("update");
					m_btnBeta.ShowWindow(m_btnCheckShowBetaVersions.GetCheck());
					if (m_btnCheckShowBetaVersions.GetCheck()) {
						nrOfVersionsAvailable++;
					}
				}

				if (nrOfVersionsAvailable==0) {
					m_editInfoVersions.SetWindowText("No firmware available.");
				}
				else {
					m_editInfoVersions.SetWindowText("Select firmware version:");
				}
				m_editBeta.SetWindowText(strBeta);
				m_editRelease.SetWindowText(strRelease);	

				m_btnRelease.EnableWindow(!fsmBusy && (GUI_UPDATE_VALIDHOST_ADDRESS == (m_guiFlags & GUI_UPDATE_VALIDHOST_ADDRESS)));
				m_btnBeta.EnableWindow(!fsmBusy && (GUI_UPDATE_VALIDHOST_ADDRESS == (m_guiFlags & GUI_UPDATE_VALIDHOST_ADDRESS)));
			}
			else {			
				// Normale mode
				// De GUI wordt upgedate volgens de gegevens van de (server) node.
				int serverCurrentVersion = -1;
				const AppRemoteCfgFirmwareVersion* pVersionRelease = NULL;
				const AppRemoteCfgFirmwareVersion* pVersionBeta = NULL;

				if (!AppFirmwareUpdaterWorker::getNodeInfoFirmwareUpdateVersion(*m_pWorker, AppFirmwareUpdaterWorker::VERSION_RELEASE, &pVersionRelease)) {
					pVersionRelease = NULL;
				}
				if (!AppFirmwareUpdaterWorker::getNodeInfoFirmwareUpdateVersion(*m_pWorker, AppFirmwareUpdaterWorker::VERSION_BETA, &pVersionBeta)) {
					pVersionBeta = NULL;
				}

				if (NULL == pVersionRelease) {
					strRelease = "N/A";
				}
				else {
					strRelease = pVersionRelease->getVersion();
				}

				if (NULL == pVersionBeta) {
					strBeta = "N/A";
				}
				else {
					strBeta = pVersionBeta->getVersion();
				}
				m_editBeta.SetWindowText(strBeta);
				m_editRelease.SetWindowText(strRelease);

			
				if (!m_pWorker->getNodeInfoVersionInfo(&serverCurrentVersion)) {
					serverCurrentVersion = -1;
				}

				int nrOfVersionsAvailable = 0;
				if (serverCurrentVersion<0) {
					nrOfVersionsAvailable = 0;
					m_btnRelease.ShowWindow(0);
					m_btnBeta.ShowWindow(0);
				}
				else {				
					{
						bool downgrade = false;
						CString strBtn;
						if (NULL != pVersionRelease) {						
							const int version = AppFirmwareUpdaterWorker::getSoftwareVersion(strRelease);
							if (version>0) {
								strBtn = getBtnText(serverCurrentVersion, version);
								if (serverCurrentVersion > version) {
									downgrade = true;
								}
							}
						}
						if (!strBtn.IsEmpty()) {
							m_btnRelease.SetWindowText(strBtn);
							if (downgrade && !m_btnCheckShowDowngradeOpt.GetCheck()) {
								m_btnRelease.ShowWindow(0);
							}
							else {
								nrOfVersionsAvailable++;
								m_btnRelease.ShowWindow(1);
							}
						}
						else {
							m_btnRelease.ShowWindow(0);
						}
					}
					
					{
						bool downgrade = false;
						CString strBtn;
						if (NULL != pVersionBeta) {
							const int version = AppFirmwareUpdaterWorker::getSoftwareVersion(strBeta);
							if (version>0) {
								strBtn = getBtnText(serverCurrentVersion, version);
								if (serverCurrentVersion > version) {
									downgrade = true;
								}
							}
						}
						if (!strBtn.IsEmpty()) {
							m_btnBeta.SetWindowText(strBtn);
							m_btnBeta.ShowWindow(m_btnCheckShowBetaVersions.GetCheck());
							if (m_btnCheckShowBetaVersions.GetCheck()) {
								nrOfVersionsAvailable++;
							}
						}
						else {
							m_btnBeta.ShowWindow(0);
						}
					}
				}

				if (nrOfVersionsAvailable == 0) {
					m_editInfoVersions.SetWindowText("No firmware available.");
				}
				else {
					m_editInfoVersions.SetWindowText("Select firmware version:");
				}
				m_btnRelease.EnableWindow(fRecievedFirmwareVersions && !fsmBusy);
				m_btnBeta.EnableWindow(fRecievedFirmwareVersions && !fsmBusy);
			}
			m_updateFirmwareInfo = false;
		}
	}
}

void CAppFirmwareUpdateMFCDlg::updateGuiNodeInfo() {

	const bool fConnectedHost = (GUI_UPDATE_NODE_CONNECTED == (m_guiFlags & GUI_UPDATE_NODE_CONNECTED));
	CString strVersion("Unknown");
	CString strPlatform("Unknown");
	CString strProduct("Unknown");
	if (fConnectedHost) {			
		CString s;
		m_pWorker->getNodeInfoStrVersionInfo(&strVersion);
		m_pWorker->getNodeInfoStrHardwarePlatform(&strPlatform);
		m_pWorker->getNodeInfoStrProductName(&strProduct);
	}
	m_editCurrentVersion.SetWindowText(strVersion);
	m_editCurrProduct.SetWindowText(strProduct);
	m_editCurrPlatform.SetWindowText(strPlatform);
}

void CAppFirmwareUpdateMFCDlg::updateGuiStatusInfoMsg() {
	m_editStatus.ShowWindow(!m_statusMsg.IsEmpty());
	m_editStatus.SetWindowText(m_statusMsg);
}

void CAppFirmwareUpdateMFCDlg::updateGuiMessageBox()
{
	if (!m_errorMsg.IsEmpty()) {
		CString msg = m_errorMsg;
		m_errorMsg.Empty();
		MessageBox(msg, "Error happened.", MB_ICONERROR);
	}
	if (!m_infoMsg.IsEmpty()) {
		CString msg = m_infoMsg;
		m_infoMsg.Empty();
		MessageBox(msg, "Status info message.", MB_ICONINFORMATION);
	}
}



LONG CAppFirmwareUpdateMFCDlg::onWmUpdateControls(WPARAM wParam, LPARAM lParam) {
		
	updateGUIFlags();		// Oppassen: Als eerste !

	const bool fsmBusy = (GUI_UPDATE_WORKERFSM_BUSY == (m_guiFlags & GUI_UPDATE_WORKERFSM_BUSY));
	m_ctrlHostAddress.EnableWindow(!fsmBusy);	
	m_btnRefreshNodeInfo.EnableWindow((GUI_UPDATE_VALIDHOST_ADDRESS == (m_guiFlags & GUI_UPDATE_VALIDHOST_ADDRESS)) &&
									  (GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS == (m_guiFlags & GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS)) &&
									  !fsmBusy);
	updateGuiNodeInfo( );
	updateGuiFirmwareAvailableInfo( );
	updateGuiStatusInfoMsg();
	updateGuiMessageBox();

	// this->UpdateWindow();		// Alles eens refeshen...
	return TRUE;
}


void CAppFirmwareUpdateMFCDlg::OnBnClickedButtonRelease() {

	CString strHostAddress;
	if (!getHostAddress(m_ctrlHostAddress, &strHostAddress)) {
		strHostAddress.Empty();
	}
	if (strHostAddress.IsEmpty()) {
		ASSERT(!strHostAddress.IsEmpty());
		return;
	}

	bool continueUpdate = true;
	CString strBtnText;
	m_btnRelease.GetWindowText(strBtnText);
	if (0 == strBtnText.Compare("Downgrade")) {
		if (IDNO == MessageBox("Are you sure you want to downgrade the firmware to a previous version? "
							   "Possible some features will no longer work in a previous firmware version.",
							   "Downgrade the firmware to a previous version?",
							   MB_ICONQUESTION | MB_YESNO)) {
							   continueUpdate = false;
		}	
	}

	if (continueUpdate) {
		if (NULL != m_pWorker) {
			if (!(GUI_UPDATE_ADVANCED_OPTIONS_SELECTED == (m_guiFlags & GUI_UPDATE_ADVANCED_OPTIONS_SELECTED))) {
				const AppRemoteCfgFirmwareVersion* pVersion = NULL;
				if (AppFirmwareUpdaterWorker::getNodeInfoFirmwareUpdateVersion(*m_pWorker, AppFirmwareUpdaterWorker::VERSION_RELEASE, &pVersion)) {
					updateGuiListboxMessage("Start target firmware update", true);
					m_pWorker->startTargetFirmwareUpdate(strHostAddress,*pVersion);
				}					
			}
			else {				
				const AppFirmwareUpdaterWorker::HardwarePlatform_t platform = getHardwarePlatform(m_comboAdvOptionsPlatform);
				const AppFirmwareUpdaterWorker::Product_t productType = getProduct(m_comboAdvOptionsProduct);				
				const AppRemoteCfgFirmwareVersion* pVersion = NULL;				
				if (!m_pWorker->getFirmwareVersion(&pVersion, platform, productType, AppFirmwareUpdaterWorker::VERSION_RELEASE)) {
					pVersion = NULL;
				}
				if (NULL != pVersion) {
					updateGuiListboxMessage("Start target firmware update", true);
					m_pWorker->startTargetFirmwareUpdate(strHostAddress,*pVersion);
				}				
			}
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);
}


void CAppFirmwareUpdateMFCDlg::OnBnClickedButtonBeta() {

	CString strHostAddress;
	if (!getHostAddress(m_ctrlHostAddress, &strHostAddress)) {
		strHostAddress.Empty();
	}
	if (strHostAddress.IsEmpty()) {
		ASSERT(!strHostAddress.IsEmpty());
		return;
	}

	if (IDYES == MessageBox("The beta firmware version holds the newest features but has NOT BEEN FULLY TESTED. "
							"Are you sure you want to upgrade to the beta test version?",						
							"Upgrade the firmware to the beta version?",
							MB_ICONQUESTION | MB_YESNO)) 
	{
		if (NULL != m_pWorker) {
			if (!(GUI_UPDATE_ADVANCED_OPTIONS_SELECTED == (m_guiFlags & GUI_UPDATE_ADVANCED_OPTIONS_SELECTED))) {
				const AppRemoteCfgFirmwareVersion* pVersion = NULL;
				if (AppFirmwareUpdaterWorker::getNodeInfoFirmwareUpdateVersion(*m_pWorker, AppFirmwareUpdaterWorker::VERSION_BETA, &pVersion)) {
					updateGuiListboxMessage("Downloading firmware from the cloud.", true);
					m_pWorker->startTargetFirmwareUpdate(strHostAddress,*pVersion);
				}				
			}
			else {				
				const AppFirmwareUpdaterWorker::HardwarePlatform_t platform = getHardwarePlatform(m_comboAdvOptionsPlatform);
				const AppFirmwareUpdaterWorker::Product_t productType = getProduct(m_comboAdvOptionsProduct);
				const AppRemoteCfgFirmwareVersion* pVersion = NULL;
				if (!m_pWorker->getFirmwareVersion(&pVersion, platform, productType, AppFirmwareUpdaterWorker::VERSION_BETA)) {
					pVersion = NULL;
				}
				if (NULL != pVersion) {			
					updateGuiListboxMessage("Start target firmware update", true);
					m_pWorker->startTargetFirmwareUpdate(strHostAddress, *pVersion);
				}				
			}
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

// Wanneer er een taak bezig is dan de cursor aanpassen.
// TODO: Wanneer de muis niet beweegt dan blijft de muis cursor soms in de toestand wachten.
BOOL CAppFirmwareUpdateMFCDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (GUI_UPDATE_WORKERFSM_BUSY == (m_guiFlags & GUI_UPDATE_WORKERFSM_BUSY)) {
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		return TRUE;
	}
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

void CAppFirmwareUpdateMFCDlg::OnBnClickedCheckShowAdvanced()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CAppFirmwareUpdateMFCDlg::OnCbnSelchangeComboProduct()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CAppFirmwareUpdateMFCDlg::OnCbnSelchangeComboPlatform()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CAppFirmwareUpdateMFCDlg::OnCbnDropdownComboProduct()
{
	m_updateFirmwareInfo = false;
}
void CAppFirmwareUpdateMFCDlg::OnCbnCloseupComboProduct()
{
	m_updateFirmwareInfo = true;
}
void CAppFirmwareUpdateMFCDlg::OnCbnDropdownComboPlatform()
{
	m_updateFirmwareInfo = false;
}
void CAppFirmwareUpdateMFCDlg::OnCbnCloseupComboPlatform()
{
	m_updateFirmwareInfo = true;
}

void CAppFirmwareUpdateMFCDlg::OnBnClickedCheckShowBeta()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CAppFirmwareUpdateMFCDlg::OnBnClickedCheckShowDowngrade()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}

// Bij verandering van een veld in het ip-address veld dan een disconnect uitvoeren.
// Dit zorgt ervoor dat de gegevens van de vorige node (ip-address) verdwijnen.
void CAppFirmwareUpdateMFCDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);

	CString strHostAddress;
	if (getHostAddress(m_ctrlHostAddress, &strHostAddress)) {
		if (m_pWorker) {
			if (m_pWorker->getReceivedNodeInfo()) {
				m_pWorker->disconnect();
			}
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);

	*pResult = 0;
}
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
IAppFirmwareUpdaterWorkerWindowHost::~IAppFirmwareUpdaterWorkerWindowHost() { }
/****************************************************************************/
CAppFirmwareUpdateMFCDlg_Host::CAppFirmwareUpdateMFCDlg_Host(CAppFirmwareUpdateMFCDlg& refDlg) :
	m_refParentDlg(refDlg) { } 
CAppFirmwareUpdateMFCDlg_Host::~CAppFirmwareUpdateMFCDlg_Host() { }

void CAppFirmwareUpdateMFCDlg_Host::guiAddListboxMsg(const char* msg, bool eraseContent) {
	if (NULL != msg) {
		m_refParentDlg.updateGuiListboxMessage(msg, eraseContent);
	}
}
void CAppFirmwareUpdateMFCDlg_Host::guiSetMsgError(const char* msg) {
	if (NULL != msg) {
		m_refParentDlg.m_errorMsg = msg;
	}
}
void CAppFirmwareUpdateMFCDlg_Host::guiSetMsgStatus(const char* msg) {
	if (NULL != msg) {
		m_refParentDlg.m_statusMsg = msg;
	}
	else {
		m_refParentDlg.m_statusMsg.Empty();
	}
}
void CAppFirmwareUpdateMFCDlg_Host::guiUpdate() {
	m_refParentDlg.PostMessage(WM_UPDATE_CONTROLS);
}
void CAppFirmwareUpdateMFCDlg_Host::startAssyncTask() {
	m_refParentDlg.SetTimer(CAppFirmwareUpdateMFCDlg::TIMER_ID_TASK_START, 1, (TIMERPROC)NULL);
}
void CAppFirmwareUpdateMFCDlg_Host::startTimerPoll() {
	m_refParentDlg.SetTimer(CAppFirmwareUpdateMFCDlg::TIMER_ID_POLL, CAppFirmwareUpdateMFCDlg::TIMER_POLL_VALUE_MSEC, (TIMERPROC)NULL);
}
void CAppFirmwareUpdateMFCDlg_Host::setStatusFirmwareUpdata(bool statusSuccess) {
	m_refParentDlg.m_updateFirmwareInfo = true;
	if (statusSuccess) {
		m_refParentDlg.m_infoMsg = "Updating done. Busy rebooting the target device...\n"
								   "Rebooting can take 30 seconds.";		
	}
	guiUpdate();
}
/****************************************************************************/









