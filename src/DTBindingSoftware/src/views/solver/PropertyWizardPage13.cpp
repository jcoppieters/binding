// PropertyWizardPage13.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage13.h"
#include "PropertyWizard.h"
#include "TXNodeMessage.h"
#include "app/appnodedbase/appnodedbase.h"
#include "MyWindowMessages.h"
// CPropertyWizardPage13 dialog
/*****************************************************************************************************/
IMPLEMENT_DYNAMIC(CPropertyWizardPage13, CPropertyPage)

CPropertyWizardPage13::CPropertyWizardPage13()
	: CPropertyWizardPageBase(CPropertyWizardPage13::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE13), 
	  m_fInitDone(FALSE), m_fUserSelectionBusy(FALSE), m_state(STATE_IDLE),m_selectedPhysicalAddress(0xFFFFFF), m_showMsgFailed(FALSE) { }

CPropertyWizardPage13::~CPropertyWizardPage13() { }

void CPropertyWizardPage13::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PHYSICAL, m_cmbPhysicalAddress);
	DDX_Control(pDX, IDC_BUTTON_ASSIGN, m_btnAssign);
}

BEGIN_MESSAGE_MAP(CPropertyWizardPage13, CPropertyPage)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_CBN_CLOSEUP(IDC_COMBO_PHYSICAL, &CPropertyWizardPage13::OnCbnCloseupComboPhysicalAddress)
	ON_CBN_DROPDOWN(IDC_COMBO_PHYSICAL, &CPropertyWizardPage13::OnCbnDropdownComboPhysicalAddress)	
	ON_BN_CLICKED(IDC_BUTTON_ASSIGN, &CPropertyWizardPage13::OnBnClickedButtonAssign)
END_MESSAGE_MAP()


// CPropertyWizardPage13 message handlers
LONG CPropertyWizardPage13::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	if (!this->m_fInitDone) {
		this->initTextInfo( );
		this->initComboPhysicalAddress();
		this->m_fInitDone = TRUE;
	}
	this->updateWizardButtons();
	this->updateStateTextBottom();
	this->updateStateComboBoxPhysicalAddress();
	this->updateStateBtnAssign();	

	if (this->m_showMsgFailed) {
		const CString strTitle("Assign CAN-bus master");
		this->MessageBox("Assignment failed.",strTitle,MB_ICONERROR | MB_OK);
		this->m_showMsgFailed = FALSE;
	}

	return TRUE;
}

void CPropertyWizardPage13::initTextInfo() {	
	{
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_TOP);
		if (NULL != pWnd) {
			const CString szTextTop("No master has been detected in the Duotecno CAN-bus system.\r\n\r\n"
							"Would you like to assign a master now?\r\n"
							"The list below shows the addresses of Duotecno CAN modules that can be set as a master. "
							"Select a device from the list and click the 'Assign' button.\r\n\r\n"
							"Planning to add more modules later and set them as a master?\r\n"
			  				"Go back and select another option.");
			pWnd->SetWindowText(szTextTop);
		}
	}
	{		
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_BOTTOM);
		if (NULL != pWnd) {
			const CString szTextBottom("Press 'Finish' when done or go back to previous options");
			pWnd->SetWindowText(szTextBottom);
		}
	}
}

// Momenteel enkel de fysische adressen van de sbox ultra.
void CPropertyWizardPage13::initComboPhysicalAddress() {
	CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
	if (NULL == pAppNodeDatabase) {
		return;
	}
	CList<int> list;
	if (pAppNodeDatabase->runtimeNodeDatabase_foundSmartboxUltraNodesWithFactoryAddresses(list)) {		
		POSITION pos = list.GetHeadPosition();
		while(pos!=NULL)  {		
			const int physical_address = list.GetNext(pos);

			CString strAddress;
			strAddress.Format("0x%06x",physical_address);

			const int index = this->m_cmbPhysicalAddress.AddString(strAddress);
			if (index>=0) {
				this->m_cmbPhysicalAddress.SetItemData(index, physical_address);
			}
		}		
	}

	BOOL enableWindow = FALSE;
	if (this->m_cmbPhysicalAddress.GetCount()>=1) {
		this->m_cmbPhysicalAddress.SetCurSel(0);	
		enableWindow = TRUE;
	}
	this->m_cmbPhysicalAddress.EnableWindow(enableWindow);
}


void CPropertyWizardPage13::updateStateTextBottom() {
	CString strText;
	if (STATE_ASSIGN_BUSY == this->m_state) {
		strText = "Please wait, busy assigning the master node address";
	}
	else if (STATE_ASSIGN_DONE == this->m_state) {
		strText = "Assignment completed. Press finish to continue.";
	}
	else if (STATE_ASSIGN_FAILED == this->m_state) {
		strText = "Assignment failed. Retry or go back and choose another option.";
	}
	if (!strText.IsEmpty()) {
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_BOTTOM);
		if (NULL != pWnd) {
			pWnd->SetWindowText(strText);
		}
	}
}

void CPropertyWizardPage13::updateStateBtnAssign() {
	BOOL stateEnabled = FALSE;
	if ((STATE_IDLE == this->m_state) || (STATE_ASSIGN_FAILED == this->m_state)) {
		const int currSelected = this->m_cmbPhysicalAddress.GetCurSel();
		if (currSelected >= 0) {
			stateEnabled = TRUE;	
		}
	}
	this->m_btnAssign.EnableWindow(stateEnabled);
}

void CPropertyWizardPage13::updateStateComboBoxPhysicalAddress() {
	this->m_cmbPhysicalAddress.EnableWindow(((STATE_IDLE == this->m_state) || (STATE_ASSIGN_FAILED == this->m_state)) ? TRUE : FALSE);
}


// Er is geen volgende button.
// Sedert V17.01: Zolang het algoritme bezig is de wizard buttons disablen.
void CPropertyWizardPage13::updateWizardButtons() {
	DWORD dwStyle = 0;
	if (STATE_ASSIGN_BUSY != this->m_state) {
		dwStyle += PSWIZB_BACK;
	}
	if (STATE_ASSIGN_DONE == this->m_state) {
		dwStyle += PSWIZB_FINISH;
	}
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet == NULL) {
		return;
	}	
	pSheet->SetWizardButtons(dwStyle);
}

BOOL CPropertyWizardPage13::OnInitDialog() {
	CPropertyWizardPageBase::OnInitDialog();
	this->m_showMsgFailed = FALSE;
	this->PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;	
}

BOOL CPropertyWizardPage13::OnSetActive()  {
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();	
	if (pSheet == NULL) {
		return TRUE;
	}
	const DWORD dwStyle = PSWIZB_BACK | PSWIZB_FINISH;		
	pSheet->SetWizardButtons(dwStyle);

	PostMessage(WM_UPDATE_CONTROLS);
	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage13::OnWizardBack()  {
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet != NULL) {		
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_ASSIGNMASTER));	
	}
	return FALSE;
}

// Na het uitvoeren wordt er opnieuw een physical browse for nodes uitgevoerd...
BOOL CPropertyWizardPage13::OnWizardFinish() {
	CPropertyWizard *pSheet = NULL;	
	if ((pSheet = (CPropertyWizard*) GetOwner()) == NULL) {	
		return FALSE;
	}
	return CPropertyWizardPageBase::OnWizardFinish();
}


void CPropertyWizardPage13::OnCbnCloseupComboPhysicalAddress() {
	this->m_fUserSelectionBusy = FALSE;
}

void CPropertyWizardPage13::OnCbnDropdownComboPhysicalAddress() {
	this->m_fUserSelectionBusy = TRUE;
}

void CPropertyWizardPage13::OnBnClickedButtonAssign() {
	const int currSelected = this->m_cmbPhysicalAddress.GetCurSel();
	if (currSelected >= 0) {
		this->m_selectedPhysicalAddress = this->m_cmbPhysicalAddress.GetItemData(currSelected);
			
		// Versturen van het master node address.
		this->sendAssignMasterNodeAddress();;

		// Naar de toestand assign busy gaan.
		this->m_state = STATE_ASSIGN_BUSY;
		this->SetTimer(TIMERID_ASSIGN_ADDRESS_FSM,TIMER_VALUE_ASSIGN_ADDRESS_WAIT_MSEC,(TIMERPROC) NULL);
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyWizardPage13::sendAssignMasterNodeAddress( ) {
	static const int nodeAddress = 0xFC;
	ASSERT(this->m_selectedPhysicalAddress >=0 && this->m_selectedPhysicalAddress < 0xFFFFFF);
	CTXBrowsePnpMessages::getInstance()->AssignPhysicalAddress(this->m_selectedPhysicalAddress , nodeAddress);
}

void CPropertyWizardPage13::OnNotifyAssignedLogicalAddress(int nodeAddress) {
	if (STATE_ASSIGN_BUSY == this->m_state) {	
		if (nodeAddress == 0xFC) {
		
			// sedert V16.47: CHANGES_UPLOAD_UNITS
			// Doorgeven aan de applicatie dat de master toegekend werd.
			// Wanneer er reeds configuratie van de units aangemaakt is dan worden deze doorgestuurd na het afsluiten van de 
			// Problem solve wizard.
			CWnd* const pMainWnd = AfxGetMainWnd();
			if (NULL != pMainWnd) {
				pMainWnd->PostMessage(WM_NOTIFY_PROBLEMSOLVED_RESTORED_NODEADDRESS, nodeAddress, this->m_selectedPhysicalAddress);
			}

			// Update GUI.
			this->m_state = STATE_ASSIGN_DONE;
			this->PostMessage(WM_UPDATE_CONTROLS);		
			this->KillTimer(TIMERID_ASSIGN_ADDRESS_FSM);
		}	
	}
}

void CPropertyWizardPage13::OnTimer(UINT nIDEvent) {
	if (TIMERID_ASSIGN_ADDRESS_FSM == nIDEvent) {	
		switch(this->m_state) {
			case STATE_ASSIGN_BUSY: {				
				this->m_showMsgFailed = TRUE;
				this->KillTimer(TIMERID_ASSIGN_ADDRESS_FSM);
				this->m_state = STATE_ASSIGN_FAILED;				
				this->PostMessage(WM_UPDATE_CONTROLS);									
			} break;
			default: {
				ASSERT(0);
				this->KillTimer(TIMERID_ASSIGN_ADDRESS_FSM);
			} break;
		}	
	}
}

// V17.01: Zolang de FSM bezig is blijft de cursor in wait toestand...
BOOL CPropertyWizardPage13::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (STATE_ASSIGN_BUSY == this->m_state) {
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
		return TRUE;
	}
	return CPropertyPage::OnSetCursor(pWnd, nHitTest, message);
}