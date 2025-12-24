// PropertyPageNewAppPage2.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage2.h"
#include "iappdlgtasks.h"
#include "MyWindowMessages.h"					// window messages...
#include "utils/app/DisplayDevice.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalSettings;
// ----------------------------------------

namespace {
	// Items worden aan de combobox toegevoegd.
	// Item data is het compoort id - Deze start vanaf 0.
	static void updateComboBoxUsbPorts(CComboBox* pCtrl,BYTE* pbComPorts,int count) {
		if (NULL != pCtrl) {
			pCtrl->ResetContent();
			for (int i=0;i<count;i++) {
				CString s;
				s.Format("COM%d",pbComPorts[i]+1);
				const int index = pCtrl->AddString(s);
				if (CB_ERR != index) {			
					pCtrl->SetItemData(index,pbComPorts[i]);
				}
			}			
		}
	}

	static int getSelectedItemData(CComboBox* pCtrl)
	{
		int selected = -1;
		const int index = pCtrl->GetCurSel();		
		if (index != CB_ERR) {
			selected = pCtrl->GetItemData(index);
		}
		return selected;
	}

	static void setSelected(CComboBox* pCtrl,int selectedItem)
	{
		BOOL fResult = FALSE;
		const int count = pCtrl->GetCount();	
		for (int i=0;i<count;i++) {
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
}; /* end namespace */


// CPropertyPageNewAppPage2 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage2, CPropertyPage)

CPropertyPageNewAppPage2::CPropertyPageNewAppPage2(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage2::IDD, refCtx)
	, m_radioMode(0),m_fUserSelectionBusy(FALSE), m_fBusyConnecting(FALSE), m_fConnected(FALSE), m_fLocalTCPIPSettingsAvailable(FALSE), m_settingsUsbPort(0),
	  m_displayMsg( )
{

}

CPropertyPageNewAppPage2::~CPropertyPageNewAppPage2()
{
}

void CPropertyPageNewAppPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_OFFLINE, m_radioMode);	
	DDX_Control(pDX, IDC_STATIC_CONNECT_USB, m_staticGroupUSBModem);
	DDX_Control(pDX, IDC_STATIC_USBPORT, m_staticUSBPort);
	DDX_Control(pDX, IDC_COMBO_USBPORT, m_ComboUsbPort);
	DDX_Control(pDX, IDC_CHECK_USBCONNECT, m_BtnCheckConnect);
	DDX_Control(pDX, IDC_GROUP_LAN, m_staticGroupSearchLAN);
	DDX_Control(pDX, IDC_CHECK_LAN_SEARCH, m_btnCheckSearchLAN);
	DDX_Control(pDX, IDC_STATIC_INFO_FOOTER, m_staticInfoFooter);
	DDX_Control(pDX, IDC_STATIC_INFO_HEADER, m_staticInfoHeader);
}



BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage2, CPropertyPage)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_OFFLINE, IDC_RADIO_TCPIP, OnRadioButtonsMode)
	ON_MESSAGE(WM_UPDATE_CONTROLS,&CPropertyPageNewAppPage2::OnUpdateControls)
	ON_BN_CLICKED(IDC_CHECK_USBCONNECT, &CPropertyPageNewAppPage2::OnBnClickedCheckUsbconnect)
	ON_CBN_CLOSEUP(IDC_COMBO_USBPORT, &CPropertyPageNewAppPage2::OnCbnCloseupComboSettingsPort)
	ON_CBN_DROPDOWN(IDC_COMBO_USBPORT, &CPropertyPageNewAppPage2::OnCbnDropdownComboSettingsPort)
	ON_BN_CLICKED(IDC_CHECK_LAN_SEARCH, &CPropertyPageNewAppPage2::OnBnClickedCheckLANSearch)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPropertyPageNewAppPage2 message handlers

// Sedert V16.26:
// Controle doen via isActivePage() om te controleren of deze pagina actief is.
// Dit is nodig omdat de instantie van de dialoog blijft bestaan en het bericht 
// UpdateCommunicationStatus() bij alle property pages terecht komt.
void CPropertyPageNewAppPage2::UpdateCommunicationStatus() 
{
	if (!this->isActivePage()) {
		return;
	}

	IAppDlgStartDialog_Host& refHostApp = this->getHost();
	m_fBusyConnecting = FALSE;
	m_fConnected = refHostApp.isConnected();
	if (m_fConnected) {		
		// Wanneer we geconnecteerd zijn dan wordt dit de standaard usb poort.
		// Deze wordt opgeslagen in het register. waarde 1-255
		const int defaultComport = m_settingsUsbPort+1;
		if (defaultComport >0 && defaultComport<=255) {
			refHostApp.setDefaultUSBPort(defaultComport);
		}		
	}

	// Updaten van de ctx
	CPropertyPageNewAppContext& refCtx = this->getCtx();
	refCtx.setStateConnected((TRUE == m_fConnected ? CPropertyPageNewAppContext::STATE_CONNECTED_USB : CPropertyPageNewAppContext::STATE_DISCONNECTED));

	PostMessage( WM_UPDATE_CONTROLS );
}

void CPropertyPageNewAppPage2::displayMsgDefaultUsbPort(int port) {		
	if (port > 0 && port<=255) {					
		m_displayMsg.Format("You normally use communication port: COM%d to connect to the duotecno modem ...",port);								
	}				
}

BOOL CPropertyPageNewAppPage2::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	m_settingsUsbPort = 0;
	m_displayMsg.Empty();

	m_fUserSelectionBusy = FALSE;	
	m_fBusyConnecting = FALSE;
	m_fConnected = FALSE;

	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	const CPropertyPageNewAppContext::ProjectType_t projectType = refCtx.getProjectType();
	CPropertyPageNewAppContext::ModeConnect_t mode = refCtx.getModeConnect();

	{		
		if (CPropertyPageNewAppContext::PROJECT_TYPE_NEW == projectType) {
			// Wanneer het een nieuw project is kunnen we het project niet offline gaan openen.
			// Aanpassen naar USB.			
			if (CPropertyPageNewAppContext::MODE_CONNECT_OFFLINE == mode) {
				mode = CPropertyPageNewAppContext::MODE_CONNECT_USB;		
				refCtx.setModeConnect(mode);
			}		
		}
		else if (CPropertyPageNewAppContext::PROJECT_TYPE_EXISTING == projectType) {
			//... 
		}

		// sedert V16.26:
		// Wanneer de dialoog geopend werd in offline mode om te connecteren is de
		// offline mode knop niet enabled.
		// Indien de selectie offline is dan overschrijven naar USB mode.
		if (DIALOGMODE_APPLICATION_CONNECT == this->getCtx().getDlgMode()) {
			if (CPropertyPageNewAppContext::MODE_CONNECT_OFFLINE == mode) {
				mode = CPropertyPageNewAppContext::MODE_CONNECT_USB;
			}		
		}

		m_radioMode = (int) mode;	
		UpdateData(FALSE);
	}

	// Sedert V16.26: Controle doen of er al tcp/ip locale parameters aanwezig zijn.
	// Indien er nog geen tcp/ip instellingen voor het locale netwerk aanwezig zijn dan is de optie 
	// "Search for tcp/ip devices in LAN network" enabled.	
	m_fLocalTCPIPSettingsAvailable = this->getHost().getServerInfoLocalValid( );
	m_btnCheckSearchLAN.SetCheck((TRUE == m_fLocalTCPIPSettingsAvailable ? 0 : 1));

	// Initialisatie van de usb-poort selectie.
	// De standaard poort wordt opgevraagd aan de registry instellingen.
	const int defaultUsbPort = getDefaultUSBPort();
	
	// Default USB poort als huidige selectie zetten.
	if (defaultUsbPort > 0) {	
		m_settingsUsbPort = defaultUsbPort-1;		
	}

	// Indien mode usb: Default usb poort tonen.
	if (CPropertyPageNewAppContext::MODE_CONNECT_USB == mode) {
		displayMsgDefaultUsbPort(defaultUsbPort);
	}

	updateListComports();

	if (DIALOGMODE_APPLICATION_CONNECT == this->getCtx().getDlgMode()) {
		m_staticInfoHeader.SetWindowText("Select connection method.");
	}

	PostMessage(WM_UPDATE_CONTROLS);
	SetTimer(WM_TIMER_UPDATE_USB_PORTS,5,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// // waarde tss 1 en 255, -1 is ongeldig
int CPropertyPageNewAppPage2::getDefaultUSBPort( ) {
	int retValue = -1;
	IAppDlgStartDialog_Host& refHostApp = this->getHost();
	m_fConnected = refHostApp.isConnected();	
	if (!m_fConnected) {			
		const int defaultUsbPort = refHostApp.getDefaultUSBPort();
		if (defaultUsbPort > 0 && defaultUsbPort<=255) {
			retValue = defaultUsbPort;
		}
	}	
	return retValue;
}

LONG CPropertyPageNewAppPage2::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	BOOL enableBtnNext = FALSE;
	BOOL enableBtnFinish = FALSE;
	BOOL enableBtnPrevious = FALSE;
	BOOL enableControlsSearchLAN = FALSE;		// sedert V16.26
	CString strMsgInfo;

	// Als er een bericht is dan de scrollview wissen en het bericht onderaan weergeven.
	if (!m_displayMsg.IsEmpty()) {		
		CDisplayDevice::Clear();
		CDisplayDevice::DisplayInfoMsg(m_displayMsg);
		m_displayMsg.Empty();
	}

	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	const CPropertyPageNewAppContext::ProjectType_t projectType = refCtx.getProjectType();
	switch(projectType) {
		case CPropertyPageNewAppContext::PROJECT_TYPE_NEW: {
			this->enableWindow(this,IDC_RADIO_OFFLINE,FALSE);	
			this->enableWindow(this,IDC_RADIO_USB,!m_fBusyConnecting);
			this->enableWindow(this,IDC_RADIO_TCPIP,!m_fBusyConnecting);
			enableBtnPrevious = FALSE;
			enableBtnNext = TRUE;			
		} break;
		case CPropertyPageNewAppContext::PROJECT_TYPE_EXISTING: {
			// Sedert V16.26.
			// Indien de dialoog geopend werd om te connecteren in offline mode 
			// dan de knop offline mode disablen.
			BOOL enableRadioBtnOffline = (m_fBusyConnecting == TRUE ? FALSE : TRUE);
			if (DIALOGMODE_APPLICATION_CONNECT == this->getCtx().getDlgMode()) {
				enableRadioBtnOffline = FALSE;
			}

			this->enableWindow(this,IDC_RADIO_OFFLINE,enableRadioBtnOffline);
			this->enableWindow(this,IDC_RADIO_USB,!m_fBusyConnecting);
			this->enableWindow(this,IDC_RADIO_TCPIP,!m_fBusyConnecting);

			if (CPropertyPageNewAppContext::MODE_CONNECT_OFFLINE == ((CPropertyPageNewAppContext::ModeConnect_t) m_radioMode)) {
				enableBtnNext = FALSE;
				enableBtnFinish = TRUE;				
			}
			else {
				enableBtnNext = TRUE;
				enableBtnFinish = FALSE;				
			}		
		} break;	
	}

	m_staticGroupUSBModem.ShowWindow(CPropertyPageNewAppContext::MODE_CONNECT_USB == ((CPropertyPageNewAppContext::ModeConnect_t) m_radioMode));
	m_staticUSBPort.ShowWindow(CPropertyPageNewAppContext::MODE_CONNECT_USB == ((CPropertyPageNewAppContext::ModeConnect_t) m_radioMode));
	m_ComboUsbPort.ShowWindow(CPropertyPageNewAppContext::MODE_CONNECT_USB == ((CPropertyPageNewAppContext::ModeConnect_t) m_radioMode));
	m_BtnCheckConnect.ShowWindow(CPropertyPageNewAppContext::MODE_CONNECT_USB == ((CPropertyPageNewAppContext::ModeConnect_t) m_radioMode));
	
	m_ComboUsbPort.EnableWindow(!m_fBusyConnecting && !m_fConnected);
	m_BtnCheckConnect.EnableWindow(!m_fBusyConnecting);
	
	if (CPropertyPageNewAppContext::MODE_CONNECT_USB == ((CPropertyPageNewAppContext::ModeConnect_t) m_radioMode)) {
		enableBtnNext = FALSE;

		if (CPropertyPageNewAppContext::STATE_CONNECTED_USB == refCtx.getStateConnected()) {
			enableBtnFinish = TRUE;
		}
		else {
			enableBtnFinish = FALSE;
		}
	}

	if (m_fBusyConnecting) {
		enableBtnPrevious = FALSE;
	}
	
	{
		if (m_fConnected) {
			enableBtnNext = FALSE;
			enableBtnPrevious = TRUE;
			enableBtnFinish = TRUE;
			this->enableWindow(this,IDC_RADIO_OFFLINE,FALSE);	
			this->enableWindow(this,IDC_RADIO_USB,TRUE);
			this->enableWindow(this,IDC_RADIO_TCPIP,FALSE);
		}
		m_BtnCheckConnect.SetCheck(m_fConnected == TRUE ? 1 : 0);
	}
	
	enableControlsSearchLAN = (CPropertyPageNewAppContext::MODE_CONNECT_TCPIP == ((CPropertyPageNewAppContext::ModeConnect_t) m_radioMode));

	m_staticGroupSearchLAN.ShowWindow( enableControlsSearchLAN );
	m_btnCheckSearchLAN.ShowWindow( enableControlsSearchLAN );

	DWORD dwButtons = 0;
	if (enableBtnNext) {
		strMsgInfo = CString("Press 'Next' to continue");
		dwButtons += PSWIZB_NEXT;
	}
	if (enableBtnPrevious) {
		dwButtons += PSWIZB_BACK;
	}
	if (enableBtnFinish) {
		strMsgInfo = CString("Press 'Finish' to continue");
		dwButtons += PSWIZB_FINISH;
	}
	this->setWizardButtons(dwButtons);

	m_staticInfoFooter.SetWindowText(strMsgInfo);
	return TRUE;
}

void CPropertyPageNewAppPage2::OnRadioButtonsMode(UINT nID) {

	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	if (IDC_RADIO_OFFLINE == nID) {
		// m_displayMsg = CString("Click finish to open the project in offline mode.");
		refCtx.setModeConnect(CPropertyPageNewAppContext::MODE_CONNECT_OFFLINE);
	}
	else if (IDC_RADIO_USB == nID) {
		refCtx.setModeConnect(CPropertyPageNewAppContext::MODE_CONNECT_USB);
		displayMsgDefaultUsbPort(this->getDefaultUSBPort());
		SetTimer(WM_TIMER_UPDATE_USB_PORTS,100,NULL);
	}
	else if (IDC_RADIO_TCPIP == nID) {
		// m_displayMsg = CString("Go to next page to select the tcp/ip settings.");
		refCtx.setModeConnect(CPropertyPageNewAppContext::MODE_CONNECT_TCPIP);
	}

	UpdateData(TRUE);
	PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageNewAppPage2::OnBnClickedCheckUsbconnect()
{
	IAppDlgStartDialog_Host& refHostApp = this->getHost();	
	m_BtnCheckConnect.SetCheck(FALSE);
	if (refHostApp.isConnected()) {
		refHostApp.setAppDisconnect();
		m_fConnected = FALSE;
	}
	else {		
		const int comport = getSelectedItemData(&m_ComboUsbPort);
		if (comport >= 0 && comport<255) {
			m_fBusyConnecting = TRUE;	
			m_settingsUsbPort = comport;
			refHostApp.setAppUsbConnect(m_settingsUsbPort+1);
		}
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

// Optie Search for TCP/IP devices in local LAN network.
// Wanneer deze optie aanligt en er is nog geen geldige tcp/îp ingesteld (m_fLocalTCPIPSettingsAvailable) 
// wordt er een messagebox getoond die vraagt aan de gebruiker of de optie moet uitgeschakeld worden.
void CPropertyPageNewAppPage2::OnBnClickedCheckLANSearch() {
	const int stateChecked = m_btnCheckSearchLAN.GetCheck();
	if ((!m_fLocalTCPIPSettingsAvailable) && (0 == stateChecked)) {
		CString msg("This option will search for TCP/IP devices in the local LAN network and will help you "
					"to configure the found TCP/IP device.\r\n"
					"Are you sure you want to disable this option?");
		if (IDNO == MessageBox(msg,"Disable automatic search for TCP/IP devices in the LAN network?",MB_YESNO | MB_ICONQUESTION)) {
			m_btnCheckSearchLAN.SetCheck(1);
		}	
	}
}

void CPropertyPageNewAppPage2::OnCbnCloseupComboSettingsPort()
{
	m_fUserSelectionBusy = FALSE;
}

void CPropertyPageNewAppPage2::OnCbnDropdownComboSettingsPort()
{
	m_fUserSelectionBusy = TRUE;
}

void CPropertyPageNewAppPage2::OnTimer(UINT_PTR nIDEvent)
{
	if (WM_TIMER_UPDATE_USB_PORTS == nIDEvent) {
		updateListComports();
	}
	CDialog::OnTimer(nIDEvent);
}

void CPropertyPageNewAppPage2::updateListComports() {
	
	if (!m_fUserSelectionBusy) {
		BOOL init = FALSE;
		BYTE arrByteComports[255];
		int count = -1;
		memset(arrByteComports,0,sizeof(arrByteComports));
		if (TMGetCommunicationSettings() != 0) {
			TMGetCommunicationSettings()->Load();
			if (TMGetCommunicationSettings()->GetAvailableComPorts(&arrByteComports[0],&count,255)) {
				init = TRUE;
			}
		}
		if ((FALSE == init) || (count<0)) {
			count = 10;
			for (int i=0;i<count;++i) {
				arrByteComports[i] = i;	
			}
		}
			
		// Updaten van de listbox control...			
		const int sel = getSelectedItemData(&m_ComboUsbPort);
		if (sel>=0) {
			m_settingsUsbPort = sel;
		}
		updateComboBoxUsbPorts(&m_ComboUsbPort,arrByteComports,count);
		setSelected(&m_ComboUsbPort,m_settingsUsbPort);
	}
	SetTimer(WM_TIMER_UPDATE_USB_PORTS,1500,NULL);	
}

void CPropertyPageNewAppPage2::StoreData()
{
	// Hier de context updaten ...
	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	// Moeten hier niks doen. Ctx wordt upgedate bij het maken van de radiobutton selectie..	
}

// Sedert V16.26:
// Mogelijkheid om te zoeken in het LAN netwerk.
LRESULT CPropertyPageNewAppPage2::OnWizardNext() {
	if ((CPropertyPageNewAppContext::MODE_CONNECT_TCPIP == this->getCtx().getModeConnect()) && (m_btnCheckSearchLAN.GetCheck())) {
		this->StoreData();
		return this->getPageResourceID(DIALOG_SEARCH_DEVICES);	// Resource id teruggeven...
	}
	return this->CPropertyPageNewApp::OnWizardNext();
}

// Sedert V16.26:
// Indien de dialoog geopend werd in offline mode om te connecteren dan is er geen eerste pagina.
LRESULT CPropertyPageNewAppPage2::OnWizardBack() {
	if (DIALOGMODE_APPLICATION_CONNECT == this->getCtx().getDlgMode()) {
		return -1;	// geen actie.
	}
	return this->CPropertyPageNewApp::OnWizardBack();
}