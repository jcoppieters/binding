// PropertyPageNewAppPage8.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage8.h"
#include "MyWindowMessages.h"
#include "iappdlgtasks.h"										// de worker.

// CPropertyPageNewAppPage8 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage8, CPropertyPage)

CPropertyPageNewAppPage8::CPropertyPageNewAppPage8(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage8::IDD, refCtx),
	  m_state(STATE_INIT), m_selectionCfgNodeAddress(RADIOBUTTON_SELECTION_MASTER), m_msgError(), m_msgInfo(), m_physicalNodeAddress(0xffffff),
	  m_responseCounter(0), m_selectedNodeAddress(0xff), m_assignedNodeAddress(-1), m_assignMasterSupported(false), m_showPopupMsgBox(false) { }

CPropertyPageNewAppPage8::~CPropertyPageNewAppPage8()
{
}

void CPropertyPageNewAppPage8::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_COMBO_NODEADDRESS, m_cmbNodeAddress);
	DDX_Control(pDX, IDC_STATIC_NODEADDRESS, m_staticNodeAddress);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfoHeader);
	DDX_Control(pDX, IDC_STATIC_INFO_FOOTER, m_staticInfoFooter);
	DDX_Radio(pDX, IDC_RADIO_MASTER, m_selectionCfgNodeAddress);
}


BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage8, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CPropertyPageNewAppPage8::OnUpdateControls)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CPropertyPageNewAppPage8::OnBnClickedButtonConfigure)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_MASTER, IDC_RADIO_SLAVE, OnRadioButtons)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// Combobox vullen met mogelijke node addressen.
void CPropertyPageNewAppPage8::initComboBoxNodeAddresses() {
	for (int value = SLAVE_NODEADDRESS_FIRST; value <= SLAVE_NODEADDRESS_LAST; ++value) {
		CString str;
		str.Format("0x%02x", value);
		const int index = m_cmbNodeAddress.AddString(str);
		if (index >= 0) {
			m_cmbNodeAddress.SetItemData(index, value);
			if (SLAVE_NODEADDRESS_DEFAULT == value) {
				m_cmbNodeAddress.SetCurSel(index);
			}
 		}
	}
}

// CPropertyPageNewAppPage8 message handlers
BOOL CPropertyPageNewAppPage8::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	m_selectedNodeAddress = 0xff;
	this->m_assignedNodeAddress = -1;
	m_responseCounter = 0;
	m_showPopupMsgBox = false;
	m_state = STATE_IDLE;
	m_assignMasterSupported = this->getHost().getServerNodeMasterSupported();
	m_physicalNodeAddress = this->getHost().getServerPhysicalAddress();
	m_selectionCfgNodeAddress = (true == m_assignMasterSupported ? RADIOBUTTON_SELECTION_MASTER : RADIOBUTTON_SELECTION_SLAVE);
	m_msgError.Empty();
	m_msgInfo.Empty();
	initComboBoxNodeAddresses();
	PostMessage(WM_UPDATE_CONTROLS);
	UpdateData(FALSE);		// Update control data --> m_selectionCfgNodeAddress		
	return TRUE;
}

void CPropertyPageNewAppPage8::UpdateCommunicationStatus() {
	if (!this->isActivePage()) {
		return;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageNewAppPage8::StoreData( ) {
	// Moet er hier nog iets gebeuren - NOPE.
}

// NOTE - Geen BACK Button
// Er is geen BACK-button voorzien omdat afh van de context de vorige pagina verschilt.
// Dit is in feite de laatste stap. (Er is reeds geconnecteeerd met de tcp/ip module.)
LRESULT CPropertyPageNewAppPage8::OnUpdateControls(WPARAM wParam, LPARAM lParam) {

	CString msgInfoFooter;
	CString msgInfoHeader;
	BOOL showWindowNodeAddress = FALSE;
	BOOL enableControls = FALSE;
	BOOL showControls = TRUE;
	DWORD dwButtons = 0;

	// MessageBox pop-up tonen...
	if (m_showPopupMsgBox) {	
		if (!m_msgError.IsEmpty()) {
			MessageBox(m_msgError,NULL,MB_ICONERROR | MB_OK);
			m_msgError.Empty();
		}
		if (!m_msgInfo.IsEmpty()) {
			MessageBox(m_msgInfo,NULL,MB_ICONINFORMATION | MB_OK);
			m_msgInfo.Empty();
		}
		m_showPopupMsgBox = false;
		return TRUE;
	}

	switch(m_selectionCfgNodeAddress) {
		case RADIOBUTTON_SELECTION_MASTER: {
			showWindowNodeAddress = FALSE;
		} break;
		case RADIOBUTTON_SELECTION_SLAVE: {
			showWindowNodeAddress = TRUE;
		} break;
	}

	switch(m_state) {
		case STATE_INIT:
		case STATE_IDLE: {
			// NOTE - Geen BACK Button 
			showControls = TRUE;
			enableControls = TRUE;
			msgInfoHeader =  "Configure the TCP/IP device in the Duotecno CAN-bus network.";
			msgInfoFooter = "Press 'Configure' to configure the device.";		
		} break;
		case STATE_BUSY:
		case STATE_BUSY_VERIFY: {
			showControls = TRUE;
			msgInfoHeader = "Busy configuring the TCP/IP device. Please wait.";
			msgInfoFooter = "Busy configuring the TCP/IP device. Please wait.";
			enableControls = FALSE;
		} break;
		case STATE_DONE: {
			showWindowNodeAddress = FALSE;	// Override.
			showControls = FALSE;
			enableControls = FALSE;
			msgInfoHeader = CString("the TCP/IP device has been configured in the Duotecno CAN-bus network.\r\n");
			msgInfoFooter = "The TCP/IP device has been configured. Press 'Finish' to continue.";
			dwButtons += PSWIZB_FINISH;
		} break;
		case STATE_ERROR: {
			// NOTE - Geen BACK Button 
			showControls = TRUE;
			enableControls = TRUE;
			msgInfoHeader = "Failed to configure the TCP/IP device.";
			msgInfoFooter = "Press 'Configure' to retry configure the device.";		
		} break;
	}
	
	m_staticNodeAddress.EnableWindow(enableControls);
	m_cmbNodeAddress.EnableWindow(enableControls);
	m_btnConfigure.EnableWindow(enableControls);

	{
		CWnd* const pWnd = GetDlgItem(IDC_RADIO_MASTER);
		if (pWnd != NULL) {
			pWnd->EnableWindow(enableControls && m_assignMasterSupported);
			pWnd->ShowWindow(showControls);
		}
	}
	{
		CWnd* const pWnd = GetDlgItem(IDC_RADIO_SLAVE);
		if (pWnd != NULL) {
			pWnd->EnableWindow(enableControls);
			pWnd->ShowWindow(showControls);
		}
	}
	{
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_GROUP);
		if (pWnd != NULL) {
			pWnd->ShowWindow(showControls);
		}
	}

	m_btnConfigure.ShowWindow(showControls);	

	m_staticNodeAddress.ShowWindow(showWindowNodeAddress);
	m_cmbNodeAddress.ShowWindow(showWindowNodeAddress);

	m_staticInfoFooter.SetWindowText(msgInfoFooter);
	m_staticInfoHeader.SetWindowText(msgInfoHeader);
	this->setWizardButtons(dwButtons);

	// Messagebox pop-up tonen...
	if (!m_msgError.IsEmpty() || !m_msgInfo.IsEmpty()) {
		m_showPopupMsgBox = true;
		PostMessage(WM_UPDATE_CONTROLS);
	}	
	return TRUE;
}

static bool parseHexValueByte(const char* str,int* value) {
	if (NULL == str) {
		ASSERT(NULL != str);
		return false;
	}
	int temp = -1;
	if (1 != sscanf(str,"0x%02x",&temp)) {
		temp = -1;
	}
	if (-1 == temp) {
		if (1 != sscanf(str,"%x",&temp)) {
			temp = -1;
		}			
	}
	if ((temp < 0) || (temp > 255)) {
		return false;
	}
	*value = temp;
	return true;
}

void CPropertyPageNewAppPage8::OnBnClickedButtonConfigure() {
	bool continueAssign = true;
	switch(m_selectionCfgNodeAddress) {
		case RADIOBUTTON_SELECTION_MASTER: {				
			m_selectedNodeAddress = 0xff;			
			CString strNameMaster;
			int physicalNodeAddress = 0xffffff;			
			if (this->getHost().SearchDbaseNodeLogicalAddress(0xFC,&strNameMaster,&physicalNodeAddress)) {			
				if ((physicalNodeAddress != 0xffffff) && (this->m_physicalNodeAddress == physicalNodeAddress)) {
					// Wanneer we hetzelfde node adres hebben moeten we geen melding tonen en 
					// kunnen gewoon het adres toekennen als master.
					continueAssign = true;
				}
				else {
					CString msgInfo;
					msgInfo.Format("The device '%s' with physical address 0x%06x is already assigned as a master.\r\n"
								   "There can only be one master in the CAN-bus network. "
								   "Are you sure you want to assign the TCP/IP device as the master?",
								   strNameMaster, physicalNodeAddress);

					if (IDYES == MessageBox(msgInfo,NULL,MB_YESNO | MB_ICONQUESTION)) {
						continueAssign = true;
					}
					else {
						continueAssign = false;
					}
				}
			}			
			if (continueAssign) {
				this->getHost().sendServerTCPIPSetMaster();
				this->m_assignedNodeAddress = 0xFC;
			}
		} break;
		case RADIOBUTTON_SELECTION_SLAVE: {				
			CString strNodeAddress;
			m_cmbNodeAddress.GetWindowText(strNodeAddress);
			if (!strNodeAddress.IsEmpty()) {			
				int temp = -1;
				if (!parseHexValueByte((LPCSTR) strNodeAddress,&temp)) {
					MessageBox("Cannot configure. Wrong value for the logical address.",NULL,MB_ICONERROR);
					return;
				}			
				if ((temp >= SLAVE_NODEADDRESS_FIRST) && (temp <= SLAVE_NODEADDRESS_LAST)) {					
					CString strNameMaster;
					int physicalNodeAddress = 0xffffff;			
					if (this->getHost().SearchDbaseNodeLogicalAddress(temp,&strNameMaster,&physicalNodeAddress)) {		
						if ((physicalNodeAddress != 0xffffff) && (this->m_physicalNodeAddress == physicalNodeAddress)) {
							// Wanneer we hetzelfde node adres hebben moeten we geen melding tonen en 
							// kunnen gewoon het adres toekennen als slave.
							continueAssign = true;
						}
						else {
							CString msgInfo;
							msgInfo.Format("The device '%s' with physical address 0x%06x already holds this logical address.\r\n"								      
										   "Are you sure you want to assign this logical address?",
										   strNameMaster, physicalNodeAddress);

							if (IDYES == MessageBox(msgInfo,NULL,MB_YESNO | MB_ICONQUESTION)) {
								continueAssign = true;
							}
							else {
								continueAssign = false;
							}
						}
					}

					if (continueAssign) {
						m_selectedNodeAddress = static_cast<BYTE>(temp);
						this->getHost().sendServerTCPIPNodeAddress(m_selectedNodeAddress);
						this->m_assignedNodeAddress = m_selectedNodeAddress;
					}					
				}						
				else {
					MessageBox("Cannot configure. Wrong value for the logical address.",NULL,MB_ICONERROR);
					return;
				}
			}
		} break;
	}
	if (continueAssign) {
		m_responseCounter = 0;
		m_state = STATE_BUSY;
		this->SetTimer(TIMER_ID_BUSY, TIMER_VALUE_WAIT_REQUEST_MSEC, (TIMERPROC)NULL);
		PostMessage(WM_UPDATE_CONTROLS);
	}	
}


void CPropertyPageNewAppPage8::OnRadioButtons(UINT nID) {
	UpdateData(TRUE);		// Update control data --> m_selectionCfgNodeAddress		
	PostMessage(WM_UPDATE_CONTROLS);	
}


// NOTE-POLLING.
// Controle of de configuratie uitgevoerd werd.
bool CPropertyPageNewAppPage8::verifyCfg(CString* pMsgError) {
	if (this->getHost().getServerNodeAddressFactory()) {
		*pMsgError = CString("the device still holds a factory init address");
		return false;
	}
	bool result = false;					
	switch(m_selectionCfgNodeAddress) {
		case RADIOBUTTON_SELECTION_MASTER: {
			result = this->getHost().getServerNodeMaster();
			if (!result) {
				*pMsgError = CString("The device could not be configured as the master node.");
			}
		} break;
		case RADIOBUTTON_SELECTION_SLAVE: {
			const BYTE bNodeAddress = this->getHost().getServerNodeAddress();
			if (m_selectedNodeAddress == bNodeAddress) {
				result = true;
			}
			else {
				*pMsgError = CString("The device could not be configured with the correct node address.");
				result = false;
			}
		} break;
	}
	return result;
}

// NOTE-POLLING.
void CPropertyPageNewAppPage8::OnTimer(UINT_PTR nIDEvent) {
	this->KillTimer(nIDEvent);

	switch (nIDEvent) {
		case TIMER_ID_BUSY: {
			switch (m_state) {
				case STATE_BUSY: {					
					this->getHost().sendServerTCPIPGetNodeInfo();			// NOTE-POLLING.
					this->SetTimer(TIMER_ID_BUSY, TIMER_VALUE_VERIFY_MSEC, (TIMERPROC)NULL);
					m_state = STATE_BUSY_VERIFY;
				} break;
				case STATE_BUSY_VERIFY: {				
					CString msgReason;
					if (this->verifyCfg(&msgReason)) {	
						if (STATE_DONE != m_state) {
							m_msgInfo = CString("Configuration done. Press 'Finish' to continue.");							
							// Sedert V16.47
							// De applicatie op de hoogte brengen dat er een node adres toegekend werd.
							// Bij een bestaande installatie (vervangen van een module) moet er mogelijks unit configuratie 
							// doorgestuurd worden naar deze module.
							CWnd* const pMainWnd = AfxGetMainWnd();
							if (NULL != pMainWnd) {
								ASSERT(this->m_assignedNodeAddress >= SLAVE_NODEADDRESS_FIRST);
								pMainWnd->PostMessage(WM_NOTIFY_PROBLEMSOLVED_RESTORED_NODEADDRESS, this->m_assignedNodeAddress, this->m_physicalNodeAddress);
							}
							m_state = STATE_DONE;
						}
					}
					else {						
						if (++m_responseCounter >= COUNTER_WAIT_RESPONSE) {
							m_msgError.Format("Failed to configure the TCP/IP device. Reason '%s'",msgReason);
							m_state = STATE_ERROR;
						}
						else {
							this->getHost().sendServerTCPIPGetNodeInfo();	// NOTE-POLLING.
							this->SetTimer(TIMER_ID_BUSY, TIMER_VALUE_VERIFY_MSEC, (TIMERPROC)NULL);	
						}
					}
				} break;
			}		
		} break;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

// V17.01: Zolang de FSM bezig is blijft de cursor in wait toestand...
BOOL CPropertyPageNewAppPage8::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (STATE_BUSY == this->m_state || STATE_BUSY_VERIFY == this->m_state) {
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
		return TRUE;
	}
	return CPropertyPage::OnSetCursor(pWnd, nHitTest, message);
}


