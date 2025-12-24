// PropertyPageDaliNwk.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliNwk.h"
#include "appdlgdaliworker.h"	
#if(0)
	#include "MyWindowMessages.h"										// TIJDELIJK OM TE TESTEN.
#endif 
#include "app\AppState\AppState.h"										// CHANGES_CLOUDBRIDGE_POC - Tijdelijk?
#include "utils/mfc/controls_util.h"
using namespace DUOTECNO::MFC_HELPER;
using namespace DUOTECNO::DALI;

// CPropertyPageDaliNwk dialog

IMPLEMENT_DYNAMIC(CPropertyPageDaliNwk, CPropertyPage)

CPropertyPageDaliNwk::CPropertyPageDaliNwk(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext)
	: CPropertyPageDaliBaseClass(CPropertyPageDaliNwk::IDD, refWorker, refContext), m_viewMode(VIEWMODE_OFFLINE),
	  m_selectionSettings(0), m_counterFoundControlInputDevices(0), m_counterFoundControlGearDevices(0)
{
}

CPropertyPageDaliNwk::~CPropertyPageDaliNwk()
{
}

void CPropertyPageDaliNwk::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_DALI_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_BTN_DALI_CONTINUE, m_btnAddrContinue);
	DDX_Control(pDX, IDC_BTN_DALI_NEW, m_btnAddrNewInstallation);
	DDX_Control(pDX, IDC_BTN_DALI_CANCEL, m_btnCancel);	
	DDX_Control(pDX, IDC_BTN_DALI_OFF, m_btnAllOff);	
	DDX_Control(pDX, IDC_BTN_DALI_MAX, m_btnAllMax);	
	DDX_Control(pDX, IDC_BTN_DALI_MIN, m_btnAllMin);	
	DDX_Control(pDX, IDC_CHECK_CONNECT, m_checkConnect);	
	DDX_Control(pDX, IDC_RADIO_LOCAL, m_radioLocal);
	DDX_Control(pDX, IDC_RADIO_REMOTE, m_radioRemote);
	DDX_Control(pDX, IDC_LISTBOX_INFO, m_listboxInfo);	
	DDX_Control(pDX, IDC_BUTTON_EDITCFG, m_btnEdit);
	DDX_Radio(pDX, IDC_RADIO_LOCAL, m_selectionSettings);
	DDX_Control(pDX, IDC_EDIT_TASKSTATE, m_editTaskState);
	DDX_Control(pDX, IDC_LIST_DEVICES, m_listDevices);
	DDX_Control(pDX, IDC_STATIC_SUMMARY, m_staticSummary);
}


BEGIN_MESSAGE_MAP(CPropertyPageDaliNwk, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_CONNECT, &CPropertyPageDaliNwk::OnBnClickedCheckConnect)
	ON_BN_CLICKED(IDC_BTN_DALI_SEARCH, &CPropertyPageDaliNwk::OnBnClickedBtnDaliSearch)
	ON_BN_CLICKED(IDC_BTN_DALI_CONTINUE, &CPropertyPageDaliNwk::OnBnClickedBtnDaliContinue)
	ON_BN_CLICKED(IDC_BTN_DALI_NEW, &CPropertyPageDaliNwk::OnBnClickedBtnDaliNew)
	ON_BN_CLICKED(IDC_BTN_DALI_CANCEL, &CPropertyPageDaliNwk::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BUTTON_EDITCFG, &CPropertyPageDaliNwk::OnBnClickedButtonEditcfg)
	ON_BN_CLICKED(IDC_BTN_DALI_OFF, &CPropertyPageDaliNwk::OnBnClickedBtnAllOff)
	ON_BN_CLICKED(IDC_BTN_DALI_MAX, &CPropertyPageDaliNwk::OnBnClickedBtnAllMax)
	ON_BN_CLICKED(IDC_BTN_DALI_MIN, &CPropertyPageDaliNwk::OnBnClickedBtnAllMin)
	ON_MESSAGE(WM_DISPLAY_ERRORMESSAGE,OnDisplayErrorMessage)
	ON_WM_TIMER()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CPropertyPageDaliNwk::OnToolTipNeedText)
END_MESSAGE_MAP()


// CPropertyPageDaliNwk message handlers
BOOL CPropertyPageDaliNwk::OnInitDialog() {
	CPropertyPage::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	
	int nCol = -1;
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_COUNT,"count",40);
	ASSERT(COLUMN_DEVICE_COUNT == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_NAME,"Address",80);
	ASSERT(COLUMN_DEVICE_NAME == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_TYPE,"Type",120);
	ASSERT(COLUMN_DEVICE_TYPE == nCol);	
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_NEW,"New",40);
	ASSERT(COLUMN_DEVICE_NEW == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_SERIALNUMBER,"Serial number",140);
	ASSERT(COLUMN_DEVICE_SERIALNUMBER == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_GTIN,"GTIN",140);
	ASSERT(COLUMN_DEVICE_GTIN == nCol);

	this->EnableToolTips(TRUE);
	this->initDialog();			
	return TRUE;
}


BOOL CPropertyPageDaliNwk::OnSetActive() {
	if (this->getContext().getFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_NWK)) {
		this->initDialog();
		this->getContext().clearFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_NWK);	
	}
	// zie CHANGES_DALI_TCPIP_COMMUNICATION_SETTINGS
	// Sedert V16.48: Updaten van de connect buttons.
	// Bij het openen van de dialoog wordt er gecontroleerd of er communicatie settings aanwezig zijn.
	// Wanneer deze afwezig zijn wordt dit gemeld aan de gebruiker en wordt de UDP configuratie geopend.
	// Via de timer 'TIMER_ID_REFRESH_CONNECTSETTINGS' kunnen we een refresh doen in deze dialoog.
	// Van zodra er connectie is met de server wordt de timer gestopt.
	if (!this->getWorker().checkDaliConnectionSettingsAndPromptUser( )) {			
		this->SetTimer(TIMER_ID_REFRESH_CONNECTSETTINGS,TIMER_RELOADVALUE_REFRESH_CONNECTSETTINGS_MSEC,(TIMERPROC) NULL);	
	}
	return CPropertyPage::OnSetActive();
}

BOOL CPropertyPageDaliNwk::OnKillActive() {
	this->KillTimer(TIMER_ID_REFRESH_CONNECTSETTINGS);
	return CPropertyPage::OnSetActive();
}

void CPropertyPageDaliNwk::initDialog() {
	if (this->getWorker().isConnected()) {
		this->m_viewMode = VIEWMODE_ONLINE;
	}
	else {
		this->m_viewMode = VIEWMODE_OFFLINE;
	}

	this->m_counterFoundControlInputDevices = 0;
	this->m_counterFoundControlGearDevices = 0;
	this->clearErrorInfo();
	this->m_selectionSettings = 0;
	this->m_listboxInfo.ResetContent();
	this->m_listDevices.DeleteAllItems();
	UpdateData(FALSE);
	this->updateControls( );
}


// IAppDlgDaliGui_Host	
void CPropertyPageDaliNwk::onNotifyGuiStatusConnected(BYTE bNodeAddress,bool statusConnected) {
	if (statusConnected) {
		//if (VIEWMODE_CONNECT_WAIT == this->m_viewMode) {
		this->AddMessage("Connected",TRUE);
		//}
		this->m_viewMode = VIEWMODE_ONLINE;
	}
	else {
		if (VIEWMODE_CONNECT_WAIT == this->m_viewMode) {
			this->AddMessage("Failed to connect.",FALSE);
		}
		else {
			this->AddMessage("Disconnected.",TRUE);
		}
		this->m_viewMode = VIEWMODE_OFFLINE;
	}
	this->updateControls();
}

// Bij een fout bericht wordt er een message box getoond.
// De message box wordt assync geopend dmv WM_DISPLAY_ERRORMESSAGE bericht.
void CPropertyPageDaliNwk::onNotifyGuiMsgCommunication(BYTE bNodeAddress,Type_t typeMessage, const CString& msg) {
	this->AddMessage(msg,FALSE);

	if (TYPE_MSG_ERROR == typeMessage) {
		this->setErrorInfo(msg);
		this->PostMessage(CPropertyPageDaliNwk::WM_DISPLAY_ERRORMESSAGE);	
	}
}

void CPropertyPageDaliNwk::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (VIEWMODE_ONLINE == this->m_viewMode) {		
		if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task ||
		    APPDALI_FSM_TASK_COMMISSIONING_NEW == task) {
			this->m_listDevices.DeleteAllItems();				
			this->m_counterFoundControlGearDevices = 0;
			this->m_counterFoundControlInputDevices = 0;
		}
		this->m_viewMode = VIEWMODE_ONLINE_BUSY;	
	}
	this->updateControls();
}

void CPropertyPageDaliNwk::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (VIEWMODE_ONLINE_BUSY == this->m_viewMode) {
		this->m_viewMode = VIEWMODE_ONLINE;	
	}
	this->updateControls();
}

void CPropertyPageDaliNwk::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {

	// Oppassen:
	// Moeten ervoor zorgen dat er niet voor ieder error bericht een messagebox getoond wordt.
	// Daarom eerst een controle doen of de huidige 'getErrorInfo()' leeg is.
	// Na het bevestigen van de messagebox wordt de string gewist.
	if ((this->getErrorInfo().IsEmpty()) && (!strMessage.IsEmpty())) {
		this->setErrorInfo(strMessage);
		this->PostMessage(CPropertyPageDaliNwk::WM_DISPLAY_ERRORMESSAGE);	
	}
	if (VIEWMODE_ONLINE_BUSY == this->m_viewMode) {
		this->m_viewMode = VIEWMODE_ONLINE;	
	}
	this->updateControls();
}

// Helper functies...
static DWORD convertToItemData(DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress) {
	DWORD dwItemData = shortAddress;
	if (DALI_DEVICE_TYPE_INPUTCONTROL == dalideviceType) {
		dwItemData += (1<<8);		
	}	
	return dwItemData;
}


void CPropertyPageDaliNwk::onNotifyGuiFoundDevice(BYTE bNodeAddress,DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice) {
	using namespace DUOTECNO::DALI;

	// Indien het item nog niet aanwezig is in de lijst de counters aanpassen.
	const DWORD dwItemData = convertToItemData(dalideviceType,shortAddress);
	const int indexItem = MFCGuiHelper_ListCtrl::listCtrlFindItemIndex(m_listDevices,dwItemData);
	if (indexItem < 0) {
		if (DALI_DEVICE_TYPE_CONTROLGEAR == dalideviceType) {
			this->m_counterFoundControlGearDevices++;
		}
		else {
			this->m_counterFoundControlInputDevices++;
		}
	}

	// Item toevoegen aan de lijst.
	this->listCtrlAddDevice(&m_listDevices,dalideviceType,shortAddress,newDevice);
	this->updateControls();
}

void CPropertyPageDaliNwk::onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress,DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber) {
	const DWORD dwItemData = convertToItemData(dalideviceType,shortAddress);
	const int indexItem = MFCGuiHelper_ListCtrl::listCtrlFindItemIndex(m_listDevices,dwItemData);
	if (indexItem>=0) {
		char strTemp[32];
		sprintf(strTemp,"%llu",serialNumber);		
		m_listDevices.SetItemText(indexItem,COLUMN_DEVICE_SERIALNUMBER, strTemp);
	}
}

void CPropertyPageDaliNwk::onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN) {
	const DWORD dwItemData = convertToItemData(dalideviceType,shortAddress);
	const int indexItem = MFCGuiHelper_ListCtrl::listCtrlFindItemIndex(m_listDevices,dwItemData);
	if (indexItem>=0) {
		char strTemp[32];
		sprintf(strTemp,"%llu",GTIN);		
		m_listDevices.SetItemText(indexItem,COLUMN_DEVICE_GTIN, strTemp);
	}
}

void CPropertyPageDaliNwk::updateControls( ) {
	switch(this->m_viewMode) {
		case VIEWMODE_OFFLINE: {
			// Connect controls.
			m_checkConnect.EnableWindow(TRUE);
			m_radioLocal.EnableWindow(TRUE);
			m_radioRemote.EnableWindow(TRUE);
			m_btnEdit.EnableWindow(TRUE);

			m_checkConnect.SetCheck(FALSE);

			// Task control
			m_btnSearch.EnableWindow(FALSE);		   
			m_btnAddrContinue.EnableWindow(FALSE);	
			m_btnAddrNewInstallation.EnableWindow(FALSE);	
			m_btnCancel.EnableWindow(FALSE);	

			// Broadcast commands
			m_btnAllOff.EnableWindow(FALSE);		   
			m_btnAllMax.EnableWindow(FALSE);
			m_btnAllMin.EnableWindow(FALSE);

			// Status
			m_editTaskState.SetWindowText("Offline");
		} break;

		case VIEWMODE_CONNECT_WAIT: {								
			// Connect controls.
			m_checkConnect.EnableWindow(FALSE);
			m_radioLocal.EnableWindow(FALSE);
			m_radioRemote.EnableWindow(FALSE);
			m_btnEdit.EnableWindow(FALSE);

			// Task control
			m_btnSearch.EnableWindow(FALSE);		   
			m_btnAddrContinue.EnableWindow(FALSE);	
			m_btnAddrNewInstallation.EnableWindow(FALSE);	
			m_btnCancel.EnableWindow(FALSE);	

			// Broadcast commands
			m_btnAllOff.EnableWindow(FALSE);		   
			m_btnAllMax.EnableWindow(FALSE);
			m_btnAllMin.EnableWindow(FALSE);

			// Status
			m_editTaskState.SetWindowText("Connect busy");
									
		} break;

		case VIEWMODE_ONLINE: {							 
			// Connect controls.
			m_checkConnect.EnableWindow(TRUE);
			m_radioLocal.EnableWindow(FALSE);
			m_radioRemote.EnableWindow(FALSE);
			m_btnEdit.EnableWindow(FALSE);

			m_checkConnect.SetCheck(TRUE);

			// Task control
			m_btnSearch.EnableWindow(TRUE);		   
			m_btnAddrContinue.EnableWindow(TRUE);				
			m_btnAddrNewInstallation.EnableWindow(TRUE);	
			m_btnCancel.EnableWindow(FALSE);	

			// Broadcast commands
			m_btnAllOff.EnableWindow(TRUE);		   
			m_btnAllMax.EnableWindow(TRUE);
			m_btnAllMin.EnableWindow(TRUE);

			// Status
			m_editTaskState.SetWindowText("Online");

		} break;
		case VIEWMODE_ONLINE_BUSY: {

			// Connect controls.
			m_checkConnect.EnableWindow(TRUE);
			m_radioLocal.EnableWindow(FALSE);
			m_radioRemote.EnableWindow(FALSE);
			m_btnEdit.EnableWindow(FALSE);

			m_checkConnect.SetCheck(TRUE);

			// Task control
			m_btnSearch.EnableWindow(FALSE);		   
			m_btnAddrContinue.EnableWindow(FALSE);				
			m_btnAddrNewInstallation.EnableWindow(FALSE);		
			m_btnCancel.EnableWindow(TRUE);	

			// Broadcast commands
			m_btnAllOff.EnableWindow(FALSE);		   
			m_btnAllMax.EnableWindow(FALSE);
			m_btnAllMin.EnableWindow(FALSE);

			// Status
			m_editTaskState.SetWindowText("Task busy");
	    } break;
	}

	// Weergave van het aantal devices die gevonden werden.
	if (this->m_counterFoundControlInputDevices > 0 || this->m_counterFoundControlGearDevices > 0) {
		m_staticSummary.ShowWindow(TRUE);
		CString strSummary;
		strSummary.Format("Found: ");
		if (this->m_counterFoundControlGearDevices > 0) {
			CString strTemp;
			strTemp.Format("Control gear devices=%d",this->m_counterFoundControlGearDevices);
			strSummary += " ";
			strSummary += strTemp;
			if (this->m_counterFoundControlInputDevices > 0) {
				strSummary += ",";
			}
		}
		if (this->m_counterFoundControlInputDevices > 0) {
			CString strTemp;
			strTemp.Format("Control input devices=%d",this->m_counterFoundControlInputDevices);
			strSummary += " ";
			strSummary += strTemp;
		}
		m_staticSummary.SetWindowText(strSummary);
	}
	else {
		m_staticSummary.ShowWindow(FALSE);
	}

	// Listbox
	m_listboxInfo.EnableWindow(TRUE);	
	this->updateControlsConnectSelLocalRemote( );
}

void CPropertyPageDaliNwk::updateControlsConnectSelLocalRemote( ) {

	// if (VIEWMODE_OFFLINE == this->m_viewMode) {
	const HomeClientConnectionState_t type = this->getWorker().getTypeConnected( );
	if (HOMECLIENT_STATE_DISCONNECTED == type) {
	
		// Radio button: select local/remote...
		const BOOL connectLocalPossible = (this->getWorker().connectLocalPossible( ) == true ? TRUE : FALSE);
		const BOOL connectRemotePossible = (this->getWorker().connectRemotePossible( ) == true ? TRUE : FALSE);

		m_radioLocal.EnableWindow(connectLocalPossible);
		m_radioRemote.EnableWindow(connectRemotePossible);

		if (FALSE == connectLocalPossible && FALSE == connectRemotePossible) {
			if (this->m_selectionSettings>=0) {
				this->m_selectionSettings = -1;
				this->UpdateData(FALSE);
			}
		}
		else if (FALSE == connectLocalPossible) {
			if ((this->m_selectionSettings==0) || (m_selectionSettings==-1)) {
				this->m_selectionSettings = 1;
				this->UpdateData(FALSE);
			}
		}
		else if (FALSE == connectRemotePossible) {
			if ((this->m_selectionSettings==1) || (m_selectionSettings==-1)) {
				this->m_selectionSettings = 0;
				this->UpdateData(FALSE);
			}
		}

		// zie CHANGES_DALI_TCPIP_COMMUNICATION_SETTINGS
		// Sedert V16.48: Overriden status: Indien er geen connect mogelijk is dan de button disablen.
		// Oppassen: Enable van deze button ook in de globale updateControls() 
		if (!connectLocalPossible && !connectRemotePossible) {
			m_checkConnect.EnableWindow(FALSE);
		}
		else {
			m_checkConnect.EnableWindow(TRUE);
		}
	}
	else {
		const HomeClientConnectionState_t type = this->getWorker().getTypeConnected( );
		if (HOMECLIENT_STATE_CONNECTED_LOCAL_IP == type) {
			this->m_selectionSettings = 0;
		}
		else {
			this->m_selectionSettings = 1;
		}
		this->UpdateData(FALSE);
	}	
}

void CPropertyPageDaliNwk::OnBnClickedCheckConnect() {
	if (this->getWorker().isConnected()) {
		this->getWorker().cancelTask();
		this->getWorker().disconnect();			
		this->m_viewMode = VIEWMODE_OFFLINE;	
		this->updateControls();
		return;
	}
	
	ConnectionMethod_t method = CONNECTION_METHOD_UNKNOWN;

	// V17.05: BUG-0185: Oplossing BUG.
	// Keuze lokaal/remote -> Nodig voor het updaten van de 'm_selectionSettings' variabel.
	UpdateData(TRUE);		

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
		if (this->getWorker().connectTCPHomeClient(method)) {
			this->m_viewMode = VIEWMODE_CONNECT_WAIT;
			this->AddMessage("Trying to connect to the server...", TRUE);
		}
	}
	this->updateControls();
}

void CPropertyPageDaliNwk::AddMessage(const CString& msg,BOOL clearMessages) {
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
void CPropertyPageDaliNwk::OnBnClickedBtnDaliSearch() {

	const CString strTitle("Search for devices on the DALI bus.");
	const CString strMessage("Search for DALI devices that already have an address.\r\n\r\n"
						     "Warning: Only devices with a short address will be found. Devices without an address will not be found with this option. "
							 "New devices (without a short address) can be added using the 'Extend installation' option.\r\n\r\n"
							 "During this action, it is possible that DALI devices may not respond. "
							 "The operation of the DALI inputs will be temporarily disabled during this process.\r\n\r\n"
							 "Do you want to proceed with searching for devices that already have an address?");

	if (IDNO == this->MessageBox(strMessage,strTitle,MB_YESNO | MB_ICONQUESTION)) {
		return;
	}

	if (true == this->getWorker().startSearchDevices()) {
		const CString msg("Search DALI-bus devices.");
		this->AddMessage(msg,TRUE);
		this->m_viewMode = VIEWMODE_ONLINE_BUSY;
	}
}

void CPropertyPageDaliNwk::OnBnClickedBtnDaliContinue() {

	const CString strTitle("Add new devices to the DALI bus system");
	const CString strMessage("Add new devices to the DALI bus system.\r\n\r\n"
							 "This option allows you to add new devices without an address. All unaddressed devices will be assigned "
							 "a new short address and the default configuration needed to work with the Duotecno system.\r\n\r\n"
							 "When a control gear device is assigned an address, it will be switched on. "
							 "This provides a visual indication to confirm that the device has received an address.\r\n\r\n"
							 "During this action, it is possible that DALI devices may not respond. "
							 "The operation of the DALI inputs will be temporarily disabled during this process.\r\n\r\n"
							 "Do you want to proceed with addressing new devices?");

	if (IDNO == this->MessageBox(strMessage,strTitle,MB_YESNO | MB_ICONQUESTION)) {
		return;
	}

	if (true == this->getWorker().startAddressing(DUOTECNO::DALI::APPDALI_ADDRESSING_INSTALLATION_EXTEND)) {
		const CString msg("Extend installation: Start addressing.");
		this->AddMessage(msg,TRUE);
		this->m_viewMode = VIEWMODE_ONLINE_BUSY;
	}
}

void CPropertyPageDaliNwk::OnBnClickedBtnDaliNew() {
	const CString strTitle("Start a new DALI bus installation.");
	const CString strMessage("Start a new DALI bus installation.\r\n\r\n"
							 "This option allows you to assign a new address to all devices in the DALI system and apply "
							 "the default configuration needed to work with the Duotecno system.\r\n\r\n"
							 "When a control gear device is assigned an address, it will be switched on. "
							 "This provides a visual indication to confirm that the device has received an address.\r\n\r\n"												
							 "Warning: Devices that already had an address or were configured will lose their address and configuration. "
							 "If you want to keep the existing configuration and only add a few devices, use the 'Extend installation' option.\r\n\r\n"
							 "Do you want to proceed with starting a new installation?");

	if (IDNO == this->MessageBox(strMessage,strTitle,MB_YESNO | MB_ICONQUESTION)) {
		return;
	}

	if (true == this->getWorker().startAddressing(DUOTECNO::DALI::APPDALI_ADDRESSING_INSTALLATION_NEW)) {
		const CString msg("New installation: Start addressing.");
		this->AddMessage(msg, TRUE);
		this->m_viewMode = VIEWMODE_ONLINE_BUSY;
	}
}


// TODO_DALI:
// Mogen we hier de vieuw mode aanpassen of moeten we wachten???
void CPropertyPageDaliNwk::OnBnClickedBtnCancel() {
	if (true == this->getWorker().cancelTask()) {
		const CString msg("Request cancel task.");
		this->AddMessage(msg,FALSE);
		if (VIEWMODE_ONLINE_BUSY == this->m_viewMode) {
			this->m_viewMode = VIEWMODE_ONLINE;			// ?????????????????????????????
		}
		this->updateControls();
	}
}

void CPropertyPageDaliNwk::OnBnClickedButtonEditcfg() {	
	this->getWorker().showCfgNodeServices( );
	this->updateControls();
}

LONG CPropertyPageDaliNwk::OnDisplayErrorMessage(WPARAM wParam,LPARAM lParam) {
	const CString strErrorMsg = this->getErrorInfo();
	if (!strErrorMsg.IsEmpty()) {
		AfxMessageBox(strErrorMsg,MB_ICONERROR);
		this->clearErrorInfo();
	}
	return TRUE;
}

void CPropertyPageDaliNwk::OnBnClickedBtnAllOff() {
	this->getWorker().sendDeviceOff(CAppDlgDaliWorker::CONTROLGEAR_MSG_BROADCAST,0xff);
}
void CPropertyPageDaliNwk::OnBnClickedBtnAllMax() {
	this->getWorker().sendDeviceMaxLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_BROADCAST,0xff);
}
void CPropertyPageDaliNwk::OnBnClickedBtnAllMin() {
	this->getWorker().sendDeviceMinLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_BROADCAST,0xff);
}

/*
int CPropertyPageDaliNwk::listCtrlInitAddColumn(CListCtrl* pCtrl,int columnNR,const char* szName,int sizeX) {
	int nCol = -1;
	if (pCtrl) {
		LVCOLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.cx = sizeX;
		lvColumn.pszText = (LPSTR) szName;
		nCol = pCtrl->InsertColumn(columnNR, &lvColumn);
	}
	return nCol;
}
*/
/*
static void helper_SetTopIndex(CListCtrl* pList) {
	const int count = pList->GetItemCount();
	if (count > 0) {
		if (pList->EnsureVisible(count-1, FALSE)) {
		}
	}
}
*/

void CPropertyPageDaliNwk::listCtrlAddDevice(CListCtrl* pList, DUOTECNO::DALI::DaliDeviceType_t dalideviceType, int shortAddress, bool newDevice) {
	using namespace DUOTECNO::DALI;

	const DWORD dwItemData = convertToItemData(dalideviceType,shortAddress);

	CString strDeviceType;
	if (DALI_DEVICE_TYPE_INPUTCONTROL == dalideviceType) {	
		strDeviceType = CString("Control device");
	}
	else {
		strDeviceType = CString("Control gear");
	}

	// reeds aanwezig.
	if (MFCGuiHelper_ListCtrl::listCtrlFindItemIndex(*pList,dwItemData) >= 0) {
		return;
	}
	
	// Indien nog niet aanwezig is dan bijvoegen...
	CString strName;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == dalideviceType) {
		strName.Format("A%d",shortAddress);
	}
	else {
		strName.Format("A%d²",shortAddress);
	}

	const int itemIndex = pList->GetItemCount();

	CString strCounter;
	strCounter.Format("%d",itemIndex+1);

	LVITEM lvItem;	
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = itemIndex;				// Op het einde van de lijst bij voegen..
	lvItem.iSubItem = 0;
	lvItem.pszText = "name"; 
	const int nItem = pList->InsertItem(&lvItem);
	ASSERT(nItem>=0);	
	pList->SetItemText(nItem, COLUMN_DEVICE_COUNT, strCounter);	
	pList->SetItemText(nItem, COLUMN_DEVICE_NAME, strName);			
	pList->SetItemText(nItem, COLUMN_DEVICE_TYPE, strDeviceType);
	pList->SetItemText(nItem, COLUMN_DEVICE_NEW, (newDevice == true ? "yes" : "no"));
	pList->SetItemText(nItem, COLUMN_DEVICE_SERIALNUMBER, "");
	pList->SetItemText(nItem, COLUMN_DEVICE_GTIN, "");
	pList->SetItemData(nItem, dwItemData);

	// Laatste altijd zichtbaar.
	MFCGuiHelper_ListCtrl::setTopIndex(pList);
}

// zie CHANGES_DALI_TCPIP_COMMUNICATION_SETTINGS
// Sedert V16.48: Updaten van de connect buttons.
// Bij het openen van de dialoog wordt er gecontroleerd of er communicatie settings aanwezig zijn.
// Wanneer deze afwezig zijn wordt dit gemeld aan de gebruiker en wordt de UDP configuratie geopend.
// Via de timer 'TIMER_ID_REFRESH_CONNECTSETTINGS' kunnen we een refresh doen in deze dialoog.
// Van zodra er connectie is met de server wordt de timer gestopt.
void CPropertyPageDaliNwk::OnTimer(UINT nIDEvent) {
	this->KillTimer(nIDEvent);
	if (TIMER_ID_REFRESH_CONNECTSETTINGS == nIDEvent) {	
		this->updateControlsConnectSelLocalRemote( );
		if (this->m_viewMode == VIEWMODE_OFFLINE) {
			this->SetTimer(TIMER_ID_REFRESH_CONNECTSETTINGS,TIMER_RELOADVALUE_REFRESH_CONNECTSETTINGS_MSEC,(TIMERPROC) NULL);	
		}
	}
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CPropertyPageDaliNwk::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_BTN_DALI_NEW: {
			this->m_strToolTipText = _T("Start a new installation. Assign DALI addresses to devices and remove all existing configuration. CAUTION: All previous configuration will be erased.");
			handled = TRUE;
		} break;
		case IDC_BTN_DALI_CONTINUE: {
			this->m_strToolTipText = _T("Extend the current installation. Add new DALI devices to the existing system.");
			handled = TRUE;
		} break;
		case IDC_BTN_DALI_SEARCH: {
			this->m_strToolTipText = _T("Read the current installation without assigning DALI addresses. Only reads device properties.");
			handled = TRUE;
		} break;
		case IDC_BTN_DALI_CANCEL: {
			this->m_strToolTipText = _T("Cancel the current operation.");
			handled = TRUE;
		} break;
		case IDC_LISTBOX_INFO: {
			this->m_strToolTipText = _T("Displays task progress information.");
			handled = TRUE;
		} break;
		case IDC_CHECK_CONNECT: {
			this->m_strToolTipText = _T("Connect to the Duotecno node via TCP/IP.");
			handled = TRUE;
		} break;
		case IDC_EDIT_TASKSTATE: {
			this->m_strToolTipText = _T("Displays the current status of the task.");
			handled = TRUE;
		} break;
		case IDC_BUTTON_EDITCFG: {
			this->m_strToolTipText = _T("Edit TCP/IP communication settings.");
			handled = TRUE;
		} break;
		case IDC_RADIO_LOCAL: {
			this->m_strToolTipText = _T("Use LAN network settings for local connection.");
			handled = TRUE;
		} break;
		case IDC_RADIO_REMOTE: {
			this->m_strToolTipText = _T("Use TCP/IP internet settings for remote connection.");
			handled = TRUE;
		} break;
		case IDC_BTN_DALI_OFF: {
			this->m_strToolTipText = _T("Turn off all devices (use after assigning DALI addresses).");
			handled = TRUE;
		} break;
		case IDC_BTN_DALI_MAX: {
			this->m_strToolTipText = _T("Turn on all devices and set them to maximum brightness.");
			handled = TRUE;
		} break;
		case IDC_BTN_DALI_MIN: {
			this->m_strToolTipText = _T("Turn on all devices and set them to minimum brightness.");
			handled = TRUE;
		} break;
		case IDC_LIST_DEVICES: {
			this->m_strToolTipText = _T("Displays all devices found in the DALI network.");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
