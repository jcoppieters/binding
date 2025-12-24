// DlgFirmwareUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgFirmwareUpdate.h"


// CDlgFirmwareUpdate dialog
enum { WM_UPDATE_CONTROLS	= (WM_USER + 150) };

IMPLEMENT_DYNAMIC(CDlgFirmwareUpdate, CDialog)

CDlgFirmwareUpdate::CDlgFirmwareUpdate(CWnd* pParent, const CString& title)
	: CDialog(CDlgFirmwareUpdate::IDD, pParent), m_strDlgTitle(title)
{	
}

CDlgFirmwareUpdate::~CDlgFirmwareUpdate()
{
}

void CDlgFirmwareUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
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


BEGIN_MESSAGE_MAP(CDlgFirmwareUpdate, CDialog)	
	ON_BN_CLICKED(IDC_BUTTON_NODE_REFRESH, &CDlgFirmwareUpdate::OnBnClickedButtonNodeRefresh)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CDlgFirmwareUpdate::onWmUpdateControls)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RELEASE, &CDlgFirmwareUpdate::OnBnClickedButtonRelease)
	ON_BN_CLICKED(IDC_BUTTON_BETA, &CDlgFirmwareUpdate::OnBnClickedButtonBeta)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_CHECK_SHOW_ADVANCED, &CDlgFirmwareUpdate::OnBnClickedCheckShowAdvanced)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT, &CDlgFirmwareUpdate::OnCbnSelchangeComboProduct)
	ON_CBN_SELCHANGE(IDC_COMBO_PLATFORM, &CDlgFirmwareUpdate::OnCbnSelchangeComboPlatform)
	ON_CBN_DROPDOWN(IDC_COMBO_PRODUCT, &CDlgFirmwareUpdate::OnCbnDropdownComboProduct)
	ON_CBN_CLOSEUP(IDC_COMBO_PRODUCT, &CDlgFirmwareUpdate::OnCbnCloseupComboProduct)
	ON_CBN_DROPDOWN(IDC_COMBO_PLATFORM, &CDlgFirmwareUpdate::OnCbnDropdownComboPlatform)
	ON_CBN_CLOSEUP(IDC_COMBO_PLATFORM, &CDlgFirmwareUpdate::OnCbnCloseupComboPlatform)
	ON_BN_CLICKED(IDC_CHECK_SHOW_BETA, &CDlgFirmwareUpdate::OnBnClickedCheckShowBeta)
	ON_BN_CLICKED(IDC_CHECK_SHOW_DOWNGRADE, &CDlgFirmwareUpdate::OnBnClickedCheckShowDowngrade)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_NODE_IPADDRESS, &CDlgFirmwareUpdate::OnIpnFieldchangedIpaddress1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgFirmwareUpdate message handlers

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

void CDlgFirmwareUpdate::init(AppFirmwareUpdaterWorker& refWorker, const CString& strIpAddress) {
	m_pWorker = &refWorker;
	m_hostAddress = strIpAddress;
}

BOOL CDlgFirmwareUpdate::OnInitDialog()
{
	ASSERT(NULL != m_pWorker);

	CDialog::OnInitDialog();

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

	if (!m_strDlgTitle.IsEmpty()) {
		this->SetWindowText(m_strDlgTitle);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlgFirmwareUpdate::updateGuiListboxMessage(const CString& msg, bool eraseContent) {
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

void CDlgFirmwareUpdate::initComboPlatform(AppFirmwareUpdaterWorker::HardwarePlatform_t platform) {
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

void CDlgFirmwareUpdate::initComboProduct(AppFirmwareUpdaterWorker::Product_t product) {
	m_comboAdvOptionsProduct.InsertString(AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN, "Touchscreen 100 Virtuals");
	m_comboAdvOptionsProduct.SetItemData(AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN, AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN);

	m_comboAdvOptionsProduct.InsertString(AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL, "AVMatrix IP + 8Channel IRTX");
	m_comboAdvOptionsProduct.SetItemData(AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL, AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL);

	m_comboAdvOptionsProduct.InsertString(AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO, "TCP/IP homeserver");
	m_comboAdvOptionsProduct.SetItemData(AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO, AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO);

	m_comboAdvOptionsProduct.InsertString(AppFirmwareUpdaterWorker::PRODUCT_SMARTBOX_ULTRA, "Smartbox ultra");
	m_comboAdvOptionsProduct.SetItemData(AppFirmwareUpdaterWorker::PRODUCT_SMARTBOX_ULTRA, AppFirmwareUpdaterWorker::PRODUCT_SMARTBOX_ULTRA);

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


void CDlgFirmwareUpdate::OnBnClickedButtonNodeRefresh()
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

void CDlgFirmwareUpdate::OnTimer(UINT_PTR nIDEvent)
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
	CDialog::OnTimer(nIDEvent);
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

void CDlgFirmwareUpdate::updateGUIFlags() {

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
void CDlgFirmwareUpdate::updateGuiFirmwareAvailableInfo() {	
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

void CDlgFirmwareUpdate::updateGuiNodeInfo() {

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

void CDlgFirmwareUpdate::updateGuiStatusInfoMsg() {
	m_editStatus.ShowWindow(!m_statusMsg.IsEmpty());
	m_editStatus.SetWindowText(m_statusMsg);
}

void CDlgFirmwareUpdate::updateGuiMessageBox()
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



LONG CDlgFirmwareUpdate::onWmUpdateControls(WPARAM wParam, LPARAM lParam) {
		
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


	// Sedert V16.30 - Tijdens de update van de firmawre IDOK-button disablen.
	CWnd* const pWndBtnOk = this->GetDlgItem(IDOK);
	if (NULL != pWndBtnOk) {
		pWndBtnOk->EnableWindow(!fsmBusy);
	}

	// this->UpdateWindow();		// Alles eens refeshen...
	return TRUE;
}


void CDlgFirmwareUpdate::OnBnClickedButtonRelease() {

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


void CDlgFirmwareUpdate::OnBnClickedButtonBeta() {

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
BOOL CDlgFirmwareUpdate::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (GUI_UPDATE_WORKERFSM_BUSY == (m_guiFlags & GUI_UPDATE_WORKERFSM_BUSY)) {
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CDlgFirmwareUpdate::OnBnClickedCheckShowAdvanced()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CDlgFirmwareUpdate::OnCbnSelchangeComboProduct()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CDlgFirmwareUpdate::OnCbnSelchangeComboPlatform()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CDlgFirmwareUpdate::OnCbnDropdownComboProduct()
{
	m_updateFirmwareInfo = false;
}
void CDlgFirmwareUpdate::OnCbnCloseupComboProduct()
{
	m_updateFirmwareInfo = true;
}
void CDlgFirmwareUpdate::OnCbnDropdownComboPlatform()
{
	m_updateFirmwareInfo = false;
}
void CDlgFirmwareUpdate::OnCbnCloseupComboPlatform()
{
	m_updateFirmwareInfo = true;
}

void CDlgFirmwareUpdate::OnBnClickedCheckShowBeta()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}
void CDlgFirmwareUpdate::OnBnClickedCheckShowDowngrade()
{
	m_updateFirmwareInfo = true;
	PostMessage(WM_UPDATE_CONTROLS);
}

// Bij verandering van een veld in het ip-address veld dan een disconnect uitvoeren.
// Dit zorgt ervoor dat de gegevens van de vorige node (ip-address) verdwijnen.
void CDlgFirmwareUpdate::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
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

// Sedert V16.30:
// Tijdens het updaten van de firmware een extra melding geven.
void CDlgFirmwareUpdate::OnClose() {
	bool close = true;
	const bool fsmBusy = (GUI_UPDATE_WORKERFSM_BUSY == (m_guiFlags & GUI_UPDATE_WORKERFSM_BUSY));
	if (fsmBusy) {
		const CString msgHeader("Stop firmware update?");
		const CString msg("If you stop the firmware update process the device can be damaged. "
						  "Are you sure you want to cancel the update?");

		if (IDNO == MessageBox(msg, msgHeader, MB_ICONQUESTION | MB_YESNO)) {
			close = false;
		}
	}
	if (close) {
		this->CDialog::OnClose();
	}
}
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
IAppFirmwareUpdaterWorkerWindowHost::~IAppFirmwareUpdaterWorkerWindowHost() { }
/****************************************************************************/
CDlgFirmwareUpdate_Host::CDlgFirmwareUpdate_Host(CDlgFirmwareUpdate& refDlg) :
	m_refParentDlg(refDlg) { } 
CDlgFirmwareUpdate_Host::~CDlgFirmwareUpdate_Host() { }

void CDlgFirmwareUpdate_Host::guiAddListboxMsg(const char* msg, bool eraseContent) {
	if (NULL != msg) {
		m_refParentDlg.updateGuiListboxMessage(msg, eraseContent);
	}
}
void CDlgFirmwareUpdate_Host::guiSetMsgError(const char* msg) {
	if (NULL != msg) {
		m_refParentDlg.m_errorMsg = msg;
	}
}
void CDlgFirmwareUpdate_Host::guiSetMsgStatus(const char* msg) {
	if (NULL != msg) {
		m_refParentDlg.m_statusMsg = msg;
	}
	else {
		m_refParentDlg.m_statusMsg.Empty();
	}
}
void CDlgFirmwareUpdate_Host::guiUpdate() {
	m_refParentDlg.PostMessage(WM_UPDATE_CONTROLS);
}
void CDlgFirmwareUpdate_Host::startAssyncTask() {
	m_refParentDlg.SetTimer(CDlgFirmwareUpdate::TIMER_ID_TASK_START, 1, (TIMERPROC)NULL);
}
void CDlgFirmwareUpdate_Host::startTimerPoll() {
	m_refParentDlg.SetTimer(CDlgFirmwareUpdate::TIMER_ID_POLL, CDlgFirmwareUpdate::TIMER_POLL_VALUE_MSEC, (TIMERPROC)NULL);
}
void CDlgFirmwareUpdate_Host::setStatusFirmwareUpdata(bool statusSuccess) {
	m_refParentDlg.m_updateFirmwareInfo = true;
	if (statusSuccess) {
		m_refParentDlg.m_infoMsg = "Updating done. Busy rebooting the target device...\n"
								   "Rebooting can take 30 seconds.";		
	}
	guiUpdate();
}
/****************************************************************************/

