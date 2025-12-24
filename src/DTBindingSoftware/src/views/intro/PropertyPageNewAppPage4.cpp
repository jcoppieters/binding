// PropertyPageNewAppPage4.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage4.h"

#include "app/appudpcfgserver/appudpservercfg.h"
#include "app/appudpcfgserver/appudpservercfgwrapper.h"

#include "views/udpcfg/DlgUDPLANSettings.h"							


#include "iappdlgtasks.h"										// de worker.
// CPropertyPageNewAppPage4 dialog

static int getSelectedItemData(CComboBox* pCtrl)
{
	int selected = -1;
	const int index = pCtrl->GetCurSel();
	if (index != CB_ERR) {
		selected = pCtrl->GetItemData(index);
	}
	return selected;
}

static void setSelected(CComboBox* pCtrl, int selectedItem)
{
	BOOL fResult = FALSE;
	const int count = pCtrl->GetCount();
	for (int i = 0; i<count; i++) {
		if (pCtrl->GetItemData(i) == selectedItem) {
			pCtrl->SetCurSel(i);
			return;
		}
	}
	// geen gevonden - de eerste tonen.
	if (pCtrl->GetCount()>0) {
		pCtrl->SetCurSel(0);
	}
}

// Converteren MAC address (laatste 8 cijfers) naar een 32 bit integer
static unsigned int convertFromMACAddress(const CString& refAddress) {
	unsigned int retVal = 0;
	if (refAddress.IsEmpty()) {
		ASSERT(0);
		return 0xffffff;
	}
	int v[4] = { 0,0,0,0 };
	if (4 != sscanf((LPCSTR)refAddress /* .GetBuffer(0) */, "%02x:%02x:%02x:%02x", &v[3], &v[2], &v[1], &v[0])) {
		ASSERT(0);
		return 0xffffff;
	}
	retVal += (v[3] << 24) + (v[2] << 16) + (v[1] << 8) + v[0];
	return retVal;
}


IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage4, CPropertyPage)

CPropertyPageNewAppPage4::CPropertyPageNewAppPage4(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage4::IDD, refCtx), m_state(STATE_INIT), m_errorCode(ERROR_NONE), 
	  m_fUserSelectionBusy(FALSE), m_fWaiting(FALSE), m_retryCounter(0) { }

CPropertyPageNewAppPage4::~CPropertyPageNewAppPage4() { }

void CPropertyPageNewAppPage4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATUSBAR, m_staticStatusInfo);
	DDX_Control(pDX, IDC_GROUP_DEVICE, m_groupDeviceInfo);
	DDX_Control(pDX, IDC_STATIC_DEVICE, m_staticDevice);
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_comboDevice);
	DDX_Control(pDX, IDC_STATIC_IPADRESS, m_staticIPAddress);
	DDX_Control(pDX, IDC_STATIC_IPPORT, m_staticIPPort);
	DDX_Control(pDX, IDC_STATIC_PASSWORD, m_staticPassword);
	DDX_Control(pDX, IDC_EDIT_IPADDRESS, m_editIPAddress);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editIPPort);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_STATIC_INFO_FOOTER, m_staticFooter);
	DDX_Control(pDX, IDC_BUTTON_SETTINGS, m_btnSettings);
}


BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage4, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CPropertyPageNewAppPage4::OnUpdateControls)
	ON_CBN_CLOSEUP(IDC_COMBO_DEVICE, &CPropertyPageNewAppPage4::OnCbnCloseupComboDevice)
	ON_CBN_DROPDOWN(IDC_COMBO_DEVICE, &CPropertyPageNewAppPage4::OnCbnDropdownComboDevice)	
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CPropertyPageNewAppPage4::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_SETTINGS, &CPropertyPageNewAppPage4::OnBnClickedButtonSettings)
END_MESSAGE_MAP()


// CPropertyPageNewAppPage4 message handlers
BOOL CPropertyPageNewAppPage4::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	m_state = STATE_INIT;
	m_errorCode = ERROR_NONE; 
	m_fUserSelectionBusy = FALSE;
	m_retryCounter = 0;

	// Wissen van de proxy.
	this->getCtx().setUDPDevice(NULL);

	CString msgInfo("Searching for devices in the local network, please wait");
	m_staticStatusInfo.SetWindowText(msgInfo);
	m_staticStatusInfo.EnableWindow(1);

	// create My progress control.
	CRect rcRectProgressCtrl(20,75,320,90);
	m_myProgressCtrl.SetRange(0,30);
	m_myProgressCtrl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcRectProgressCtrl,this,0);
	m_myProgressCtrl.StepUp();

	// List box met devices updaten...
	this->SetTimer(TIMER_ID_UPDATEDEVICES, 1000, (TIMERPROC)NULL);
	PostMessage(WM_UPDATE_CONTROLS);
	m_state = STATE_SEARCH_DEVICES;
	m_fWaiting = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPropertyPageNewAppPage4::OnSetActive() {
	int intervalMSEC = -1;
	if (STATE_INIT != m_state) {
		if (this->getCtx().getFlagSearchAgain()) {
			m_comboDevice.ResetContent();			
			this->getHost().clearUDPDeviceList();			
			this->getCtx().setUDPDevice(NULL);
			m_state = STATE_SEARCH_DEVICES;
			m_retryCounter = 0;
			m_fWaiting = FALSE;
			this->getCtx().setFlagSearchAgain(FALSE);
		}
	}

	switch(m_state) {
		case STATE_INIT: 
			// absorbe...
		break;
		case STATE_SEARCH_DEVICES:
			intervalMSEC = 100;
		break;
		case STATE_SEARCH_DEVICES_TIMEOUT:
			searchAgain(TRUE);
		break;
		case STATE_DEVICE_REQUEST: 
			ASSERT(0);	// Mag niet voorkomen.
		break;
		case STATE_DEVICE_RESPONSE_OK:
			intervalMSEC = 2000;
		break;
		case STATE_DEVICE_RESPONSE_EC:
			searchAgain(TRUE);
		break;
	}
	if (intervalMSEC > 0) {
		this->SetTimer(TIMER_ID_UPDATEDEVICES,intervalMSEC,(TIMERPROC) NULL);
	}
	return this->CPropertyPageNewApp::OnSetActive();
}

void CPropertyPageNewAppPage4::UpdateCommunicationStatus() {
	if (!this->isActivePage()) {
		return;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageNewAppPage4::StoreData( ) {
	// Moet er hier nog iets gebeuren - NOPE.
}


static bool findDevice(AppUDPDevices& refDevices,int comboBoxItemData, AppUDPDevice** ppDevice) {	
	const int count = refDevices.getCount();
	for (int i = 0; i < count; ++i) {
		AppUDPDevice* pDevice = NULL;
		if (refDevices.getAt(i, &pDevice)) {
			ASSERT(NULL != pDevice);
			if (comboBoxItemData == convertFromMACAddress(pDevice->getMACAddress())) {
				*ppDevice = pDevice;
				return true;
			}
		}
	}
	return false;
}

void CPropertyPageNewAppPage4::showUDPDeviceProperties(BOOL show,AppUDPDevice* pDevice)
{
	if (show) {	
		CString szPort;
		CString szDeviceIPAddress;
		CString szDevicePassword;	
		if (NULL != pDevice) {
			szDeviceIPAddress = pDevice->getIpAddress();
			szDevicePassword = pDevice->getAppHomeServerCfg().getPassword();
			const int devicePort =  pDevice->getAppHomeServerCfg().getIPPort();
			szPort.Format("%d",devicePort);
		}
		m_editIPAddress.SetWindowText(szDeviceIPAddress);
		m_editIPPort.SetWindowText(szPort);
		m_editPassword.SetWindowText(szDevicePassword);
	}

	m_staticIPAddress.ShowWindow(show);
	m_staticIPPort.ShowWindow(show);
	m_staticPassword.ShowWindow(show);
	m_editIPAddress.ShowWindow(show);
	m_editIPPort.ShowWindow(show);
	m_editPassword.ShowWindow(show);
}


LRESULT CPropertyPageNewAppPage4::OnWizardBack() {
	return this->getPageResourceID(DIALOG_SELECT_COMMUNICATION);
}

BOOL CPropertyPageNewAppPage4::OnKillActive() {
	this->KillTimer(TIMER_ID_UPDATEDEVICES);
	this->KillTimer(TIMER_ID_REQUESTDEVICEINFO);
	return CPropertyPage::OnKillActive();
}

static void helper_formatMsgErrorNoDevicesFound(int nrOfDevicesFound, CString* pMsg) {
	CString msg;
	if (nrOfDevicesFound < 1) {
		msg = "There are no Duotecno devices found in the LAN-network.\r\n"
			  "Check if the Duotecno devices are powered and connected to the same LAN " 
			  "network as your pc.";	
	}
	else {
		msg.Format("Found %d Duotecno device(s) in the LAN-network.\r\n"
				   "The device(s) found in the LAN network do not support the configuration TCP/IP protocol.",
					nrOfDevicesFound);
	}
	*pMsg = msg;
}

static void helper_formatMsgInfoFoundDevices(int nrOfItems,CString* pMsg) {
	CString msg;
	if (nrOfItems == 1) {
		msg = "Press 'Next' to configure the TCP/IP device.";
	}
	else if (nrOfItems > 1) {
		msg.Format("Found %d TCP/IP devices in the LAN network.\r\n"
				   "Select TCP/IP device to configure and press 'Next'.", nrOfItems);
	}
	*pMsg = msg;	
}

static void helper_formatMsgDeviceNotResponding(int nrOfItems,CString* pMsg) {	
	CString msg("Failed to get info from the selected device.\r\n");
	if (nrOfItems > 1) {
		msg += "Try again or select another device.";
	}
	else {
		msg += "Try again";
	}
	*pMsg = msg;
}


LRESULT CPropertyPageNewAppPage4::OnUpdateControls(WPARAM wParam, LPARAM lParam) {

	BOOL enableControls = FALSE;
	BOOL enableComboBox = FALSE;
	BOOL showProgressCtrl = FALSE;
	BOOL showBtnSearchAgain = FALSE;
	BOOL showBtnSettings = FALSE;
	CString msgInfoFooter;
	
	AppUDPDevice* const pUDPDevice = this->getCtx().getUDPDevice();

	DWORD dwButtons = PSWIZB_BACK;
	CString msgInfo; 
	BOOL showDeviceConnectProperties = FALSE;
	switch(m_state) {	
		case STATE_INIT: {
			msgInfo = "Searching for devices in the local network.";
			showProgressCtrl = TRUE;
		} break;

		case STATE_SEARCH_DEVICES: {
			msgInfo = "Searching for devices in the local network. please wait";	
			showProgressCtrl = TRUE;			
		} break;

		case STATE_SEARCH_DEVICES_TIMEOUT: {
			// Aantal gevonden UDP devices opvragen.
			// Niet alle devices die gevonden werden zijn geschikt om mee te connecteren.
			// Momenteel enkel de DT18-PRO.
			// Indien er andere gevonden werden dan dit weergeven in de foutmelding.			
			const int nrOfDevicesFound = this->getHost().getUDPDeviceList().getCount();			
			helper_formatMsgErrorNoDevicesFound(nrOfDevicesFound,&msgInfo);			
			showProgressCtrl = TRUE;
			showBtnSearchAgain = TRUE;		
			showBtnSettings = TRUE;
		} break;

		case STATE_DEVICE_REQUEST: {

			// Updaten van de info/status tekst
			const int nrOfItems = m_comboDevice.GetCount();
			if (nrOfItems >= 1) {				
				enableControls = TRUE;
				enableComboBox = TRUE;
				if (m_retryCounter == 0) {
					msgInfo = "Requesting info from the selected device. Please wait.";					
				}
				else{
					msgInfo = "Requesting info from the selected device.\r\n"
						      "No response. Retrying. Please wait.";
				}				
				showProgressCtrl = FALSE;
			}
			else {
				showProgressCtrl = TRUE;
			}
			
		} break;

		case STATE_DEVICE_RESPONSE_OK: {

			enableControls = TRUE;
			enableComboBox = TRUE;
			showDeviceConnectProperties = TRUE;

			const int nrOfItems = m_comboDevice.GetCount();
			if (nrOfItems >= 1) {
				helper_formatMsgInfoFoundDevices(nrOfItems,&msgInfo);			
			}
			dwButtons |= PSWIZB_NEXT;
			msgInfoFooter = CString("Press 'Next' to continue");
		} break;
		
		case STATE_DEVICE_RESPONSE_EC: {
			enableControls = FALSE;
			enableComboBox = FALSE;
			showBtnSearchAgain = TRUE;
			showBtnSettings = TRUE;
			helper_formatMsgDeviceNotResponding(m_comboDevice.GetCount(),&msgInfo);		
		} break;	
	}

	// GUI updaten...
	m_comboDevice.ShowWindow(enableControls);
	m_comboDevice.EnableWindow(enableComboBox);
	m_groupDeviceInfo.ShowWindow(enableControls);
	m_staticDevice.ShowWindow(enableControls);
	showUDPDeviceProperties(showDeviceConnectProperties,pUDPDevice);
	m_btnSearch.ShowWindow(showBtnSearchAgain);
	m_btnSettings.ShowWindow(showBtnSettings);
	m_staticStatusInfo.SetWindowText(msgInfo);
	m_myProgressCtrl.ShowWindow(showProgressCtrl);
	m_staticFooter.SetWindowText(msgInfoFooter);

	this->setWizardButtons(dwButtons);
	return TRUE;
}

void CPropertyPageNewAppPage4::OnCbnCloseupComboDevice() {

	// Een andere geselecteerd - Zoeken naar het device...
	this->SetTimer(TIMER_ID_REQUESTDEVICEINFO, 100 , (TIMERPROC)NULL);

	// Het nieuw geselecteerde device als proxy zetten...
	const int selected = m_comboDevice.GetCurSel();
	ASSERT(selected >= 0);
	if (selected >= 0) {
		AppUDPDevice *pDevice = NULL;	
		findDevice(this->getHost().getUDPDeviceList(),m_comboDevice.GetItemData(selected),&pDevice);
		ASSERT(NULL != pDevice);			
		this->getCtx().setUDPDevice(pDevice);
	}

	m_retryCounter = 0;
	m_state = STATE_DEVICE_REQUEST;

	m_fUserSelectionBusy = FALSE;
	PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageNewAppPage4::OnCbnDropdownComboDevice() {
	m_fUserSelectionBusy = TRUE;
}


// Oppassen:
// Doen een controle dmv m_fWaiting.
// Indien deze op TRUE hebben we zelf de cursor veranderd naar de WAIT cursor.
// Moeten er voorzorgen dat MFC de cursor niet veranderd anders zijn we mogelijks de 
// originele cursor kwijt.
BOOL CPropertyPageNewAppPage4::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_fWaiting) 
		return TRUE;

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CPropertyPageNewAppPage4::OnTimer(UINT_PTR nIDEvent) {

	this->KillTimer(nIDEvent);

	if (TIMER_ID_UPDATEDEVICES == nIDEvent) {	
		enum {
			// Standaard tijd.
			// De tijdsinterval wordt gebruikt wanneer er al devices gevonden zijn in het netwerk.
			INTERVAL_DEFAULT_MSEC = 5000,		

			// Tijd wanneer we nog geen devices gevonden hebben in het netwerk.
			// Oppassen: Niet aanpassen zie COUNTER_VALUE_SEARCH_DEVICES_SEC	
			INTERVAL_SEARCH_DEVICES_MSEC = 100,	// Niet meer aanpassen zie COUNTER_VALUE_SEARCH_DEVICES_SEC
		};

		int timerIntervalMSEC = INTERVAL_DEFAULT_MSEC;
		updateListDevicesFound();		// In iedere toestand de combobox aanvullen met nieuwe devices.
		switch(m_state) {
			case STATE_INIT: {
				// absorbe...
			} break;
			case STATE_SEARCH_DEVICES: {				
				if (++m_retryCounter < (10 * COUNTER_VALUE_SEARCH_DEVICES_SEC)) { 
					timerIntervalMSEC = INTERVAL_SEARCH_DEVICES_MSEC;					  
					if (m_myProgressCtrl.GetSafeHwnd()) {
						m_myProgressCtrl.StepUp();
					}
				}
				else {
					if (m_myProgressCtrl.GetSafeHwnd()) {
						m_myProgressCtrl.SetError();
					}
					m_state = STATE_SEARCH_DEVICES_TIMEOUT;
				}
			} break;
			case STATE_DEVICE_REQUEST:
			case STATE_DEVICE_RESPONSE_OK:
			case STATE_DEVICE_RESPONSE_EC: {
				// absorbe...				 
			} break;
		}	
		this->SetTimer(TIMER_ID_UPDATEDEVICES, timerIntervalMSEC , (TIMERPROC)NULL);
	}

	else if (TIMER_ID_REQUESTDEVICEINFO == nIDEvent) {	
		switch(m_state) {
			case STATE_INIT: 
			case STATE_SEARCH_DEVICES: {
				ASSERT(0);
			} break;			
			case STATE_DEVICE_REQUEST: {
				// Veranderen de cursor.
				// Applicatie wacht hier op antwoord.
				// Zolang er geen antwoord is van het device blijven we hier wachten + blijft de cursor in
				// de toestand wachten...
				ASSERT(FALSE == m_fWaiting);
				HCURSOR hOldCursor = NULL;
				m_fWaiting = TRUE;
				hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));				
				requestDeviceProperties();
				PostMessage(WM_UPDATE_CONTROLS);
				m_fWaiting = FALSE;
				SetCursor(hOldCursor);
				ASSERT(FALSE == m_fWaiting);
				PostMessage(WM_UPDATE_CONTROLS);
			} break;
			case STATE_DEVICE_RESPONSE_OK:
			case STATE_DEVICE_RESPONSE_EC: {
				ASSERT(0);							   
			} break;
		}		
	}

	CDialog::OnTimer(nIDEvent);
}

// Opvragen van de gevonden LAN devices.
void CPropertyPageNewAppPage4::updateListDevicesFound() {		
	AppUDPDevices& refDevices = this->getHost().getUDPDeviceList();
	updateListDevicesFound(refDevices);		
}

// Wanneer er al een device in de lijst aanwezig is dan deze niet meer verwijderen !!!
// Nieuwe devices altijd als laatste bijvoegen...
void CPropertyPageNewAppPage4::updateListDevicesFound(AppUDPDevices& refDevices) {

	if (m_fUserSelectionBusy) {
		return;
	}

	// Updaten van de lijst van de gevonden devices.
	// Nieuw gevonden items worden toegevoegd aan het einde van de lijst.
	// Reeds aanwezige items worden upgedate.
	const int count = refDevices.getCount();
	for (int i = 0; i < count; ++i) {
		AppUDPDevice* pDevice = NULL;
		if (refDevices.getAt(i, &pDevice)) {
			ASSERT(NULL != pDevice);
			addDevice(*pDevice);
		}
	}
	
	if (STATE_SEARCH_DEVICES == m_state) {
		// Indien er geen geselekteerd zijn en er zijn devices gevonden dan de eerste tonen.
		// Een timer starten die de eigenschappen van dit device zoekt...

		// Dit device als proxy opslaan...
		if (m_comboDevice.GetCurSel() < 0) {		
			if (m_comboDevice.GetCount() > 0) {	
				static const int currentSelected = 0;
				m_comboDevice.SetCurSel(currentSelected);

				AppUDPDevice *pDevice = NULL;				
				findDevice(refDevices,m_comboDevice.GetItemData(currentSelected),&pDevice);
				ASSERT(NULL != pDevice);
				this->getCtx().setUDPDevice(pDevice);
				
				this->SetTimer(TIMER_ID_REQUESTDEVICEINFO, 100 , (TIMERPROC)NULL);
				m_retryCounter = 0;
				m_state = STATE_DEVICE_REQUEST;
			}
		}
	}

	// Updaten van de controls.
	PostMessage(WM_UPDATE_CONTROLS);
}

// Oppassen:
// Is enkel de proxy die upgedate wordt !!!
bool CPropertyPageNewAppPage4::requestDeviceProperties() {

	enum { TIMEOUT_VALUE_RESPONSE_MSEC = 10000 };

	AppUDPDevice* const pDevice = this->getCtx().getUDPDevice();
	if (NULL == pDevice) {
		ASSERT(NULL != pDevice);
		m_errorCode = ERROR_GENERAL;
		m_state = STATE_DEVICE_RESPONSE_EC;
		return false;
	}
	
	AppUDPServerCfgWrapper worker;
	if (worker.sendRequestCfgAppHomeSrv(pDevice,TIMEOUT_VALUE_RESPONSE_MSEC)) {
		m_retryCounter = 0;
		m_errorCode = ERROR_NONE;
		m_state = STATE_DEVICE_RESPONSE_OK;
		return true;
	}

	if (++m_retryCounter>=COUNTER_THRESSHOLD_REQUEST_DEVICEINFO) {		
		pDevice->getAppHomeServerCfg().clear();
		m_errorCode = ERROR_RESPONSE_TIMEOUT;
		m_state = STATE_DEVICE_RESPONSE_EC;
	}
	else {
		this->SetTimer(TIMER_ID_REQUESTDEVICEINFO,100,(TIMERPROC)NULL);
	}
	return false;
}


void CPropertyPageNewAppPage4::addDevice(AppUDPDevice& refDevice) {

	const CString& s = refDevice.getMACAddress();
	const CString& szName = refDevice.getName();
	const int identifier = convertFromMACAddress(s);

	if (!this->getHost().getTCPIPCfgSupported(refDevice)) {
		return;
	}
	
	// zoeken naar het device - indien reeds aanwezig niks doen...
	const int count = m_comboDevice.GetCount();
	for (int i = 0; i < count; i++) {
		if (m_comboDevice.GetItemData(i) == identifier) {		
			return;
		}
	}
	
	// Item toevoegen.
	const int newIndex = m_comboDevice.AddString(szName);
	if (newIndex >= 0) {
		m_comboDevice.SetItemData(newIndex, identifier);
	}
	ASSERT((count + 1) == m_comboDevice.GetCount());	// nieuw item toegevoegd.
}

void CPropertyPageNewAppPage4::OnBnClickedButtonSearch() {
	searchAgain(STATE_DEVICE_RESPONSE_EC == m_state);
	PostMessage(WM_UPDATE_CONTROLS);
}

// De voledige lijst van gevonden devices wissen (bool clearList) en opnieuw zoeken.
void CPropertyPageNewAppPage4::searchAgain(BOOL clearList) {
	if (clearList) {			
		this->getHost().clearUDPDeviceList();
		this->getCtx().setUDPDevice(NULL);	// De proxy wissen...
	}
	m_comboDevice.ResetContent();
	m_myProgressCtrl.Clear();
	this->SetTimer(TIMER_ID_UPDATEDEVICES, 100, (TIMERPROC)NULL);
	m_retryCounter = 0;		
	m_state = STATE_SEARCH_DEVICES;
}

void CPropertyPageNewAppPage4::OnBnClickedButtonSettings() {
	CDlgUDPLANSettings dlg;
	if (IDOK == dlg.DoModal()) {
		const CString strNwkAdapter = dlg.getNetworkAdapterAddress();
		this->getHost().selectNetworkAdapter(strNwkAdapter);
	}
}


