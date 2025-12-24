// DlgChildDaliControlDeviceOverview.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliControlDeviceOverview.h"
#include "appdlgdaliworker.h"
#include "app/appdali/appdali_def.h"
#include "app/appdali/appdalidevices.h"

// CDlgChildDaliControlDeviceOverview dialog
/****************************************************************************************************/
static const CString STRING_FOOTER_OFFLINE("Connect online to enable or disable device instances.");
static const CString STRING_FOOTER_ONLINE("Enable or disable device instances. the changes will be applied immediately.");
static const CString STRING_FOOTER_ONLINE_BUSY("Busy configuration, please wait...");
/****************************************************************************************************/
using namespace DUOTECNO::DALI;
/****************************************************************************************************/
#define TRACER(x)			TRACE(x)
/****************************************************************************************************/
IMPLEMENT_DYNAMIC(CDlgChildDaliControlDeviceOverview, CDialog)

CDlgChildDaliControlDeviceOverview::CDlgChildDaliControlDeviceOverview(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker,CDlgChildDaliControlDeviceOverview::IDD, pParent), m_busyEditingName(false), m_showWarning(true) { 
	this->setCurrDeviceShortAddress(-1);
	this->setBusyGuiControlsUpdate(false);
}

CDlgChildDaliControlDeviceOverview::~CDlgChildDaliControlDeviceOverview()
{
}

void CDlgChildDaliControlDeviceOverview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
}


BEGIN_MESSAGE_MAP(CDlgChildDaliControlDeviceOverview, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_INSTANCE0, IDC_CHECK_INSTANCE31, OnButtonClicked)	
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT_INSTANCE_0, IDC_EDIT_INSTANCE_31, OnEnSetfocusEditBox)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_INSTANCE_0, IDC_EDIT_INSTANCE_31, OnEnKillfocusEditBox)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_INSTANCE_0, IDC_EDIT_INSTANCE_31, OnEnChangeEditControls)
	ON_WM_TIMER()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliControlDeviceOverview::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgChildDaliControlDeviceOverview message handlers
void CDlgChildDaliControlDeviceOverview::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {		
	if (NULL == pDevice) {
		this->setDevice(NULL);
		this->setCurrDeviceShortAddress(-1);		
	}
	else {
		this->setCurrDeviceShortAddress(pDevice->getShortAddress());
		this->setDevice(pDevice);
	}

	// lokale var....
	this->m_busyEditingName  = false;	
	this->m_currentInstanceName = -1;
    this->m_strPreviousName.Empty();
	this->setBusyGuiControlsUpdate(false);

	this->updateViewMode();
	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlDeviceOverview::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {
	if (activeDialog) {
		this->init(pDevice);
	}
}

BOOL CDlgChildDaliControlDeviceOverview::OnParentPropertyPageLeave() {
	this->KillTimer(TIMER_ID_EDITBOX_INSTANCENAME);
	BOOL result = this->getRequestParentUpdateGUI();
	this->setRequestParentUpdateGUI(false);
	return result;												// Indien er namen veranderd werden is er een update nodig...
}

void CDlgChildDaliControlDeviceOverview::onTabDialogEnterDialog( ) {
	this->updateViewMode();
	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// IAppDlgDaliGui_Host	
void CDlgChildDaliControlDeviceOverview::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (task == APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF) {
		// absorbe...	
	}
	this->setViewMode(VIEWMODE_ONLINE_BUSY);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlDeviceOverview::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (task == APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF) {
		this->updateModel();	
	}
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlDeviceOverview::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	if (task == APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF) {
		this->updateModel();	
	}
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlDeviceOverview::updateModel( ) {
	this->m_mapNames.RemoveAll();

	CAppDaliDeviceProperties_InputControlDevice* properties = NULL;
	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL != pDevice) {		
		properties = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
	}
	
	for (int i=DUOTECNO::DALI::CAppDaliInputControlDeviceInstances::FIRST_INSTANCE_NUMBER;
		 i<=DUOTECNO::DALI::CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER;
		 ++i) 	
	{
		this->m_statesCheckBox[i] = STATE_CHECKBOX_DISABLED;
		if (NULL != properties) {	
			CAppDaliInputControlDeviceInstance* pInstance = NULL;
			if (properties->getInstances().find(i,&pInstance)) {
				if (pInstance->getInstanceActive()) {					
					this->m_statesCheckBox[i] = STATE_CHECKBOX_ON;					
				}
				else {
					this->m_statesCheckBox[i] = STATE_CHECKBOX_OFF;
				}
				this->m_mapNames.SetAt(i, pInstance->getName());
			}
		}		
	}
}

LONG CDlgChildDaliControlDeviceOverview::OnUpdateControls(WPARAM wParam,LPARAM lParam) {	

	this->setBusyGuiControlsUpdate(true);		// Nodig voor de onChange handler van de edit controls.

	if (!this->getInitDone()) {
		this->EnableToolTips(TRUE);
		this->setInitDone(TRUE);
	}

	for (int i=0;i<MAX_INSTANCES;++i) {
		CWnd* const pWndCheckBox = this->GetDlgItem(IDC_CHECK_INSTANCE0 + i);
		CWnd* const pWndEditBox = this->GetDlgItem(IDC_EDIT_INSTANCE_0 + i);
		if (NULL != pWndCheckBox) {
			if (this->getViewMode() == VIEWMODE_ONLINE_BUSY) {
				pWndCheckBox->EnableWindow(FALSE);
			}
			else {
				CButton* const pBtn = (CButton*)(pWndCheckBox);
				if (NULL != pBtn) {
					switch(this->m_statesCheckBox[i]) {
						case STATE_CHECKBOX_DISABLED: {
							pBtn->EnableWindow(FALSE);
							pBtn->SetCheck(0);
						} break;
						case STATE_CHECKBOX_ON: {						
							pBtn->EnableWindow(this->getViewMode() == VIEWMODE_OFFLINE ? FALSE : TRUE);
							pBtn->SetCheck(1);
						} break;
						case STATE_CHECKBOX_OFF: {
							pBtn->EnableWindow(this->getViewMode() == VIEWMODE_OFFLINE ? FALSE : TRUE);
							pBtn->SetCheck(0);
						} break;
					}
				}	
			}
		}	
		if (NULL != pWndEditBox) {
			if (this->getViewMode() == VIEWMODE_ONLINE_BUSY) {
				pWndEditBox->EnableWindow(FALSE);
			}
			else {
				switch(this->m_statesCheckBox[i]) {
					case STATE_CHECKBOX_DISABLED: {
						pWndEditBox->EnableWindow(FALSE);
					} break;
					case STATE_CHECKBOX_ON: 
					case STATE_CHECKBOX_OFF: {									
						pWndEditBox->EnableWindow(TRUE);				
					} break;				
				}
			}

			// Namen van de instances
			CString strName;	
			if (!this->m_mapNames.Lookup(i, strName)) {
				strName.Empty();
			}	

			if (this->GetFocus() == pWndEditBox) {
				TRACE("--> Do not update - Possible busy editing the current window");
			}
			else{
				pWndEditBox->SetWindowText(strName);
			}
		}
	}

	const char* strText = STRING_FOOTER_OFFLINE;
	switch(this->getViewMode()) {
		case VIEWMODE_OFFLINE: {
			strText = STRING_FOOTER_OFFLINE;		   
		} break;
		case VIEWMODE_ONLINE: {
			strText = STRING_FOOTER_ONLINE;		   
		} break;
		case VIEWMODE_ONLINE_BUSY: {
			strText = STRING_FOOTER_ONLINE_BUSY;		   
		} break;
	}	
	ASSERT(NULL != strText);
	this->m_staticFooterInfoText.SetWindowText(strText);

	this->setBusyGuiControlsUpdate(false);
	return TRUE;
}

void CDlgChildDaliControlDeviceOverview::OnButtonClicked(UINT nID) {
	const int instanceNumber = nID - IDC_CHECK_INSTANCE0;
	if (instanceNumber < CAppDaliInputControlDeviceInstances::FIRST_INSTANCE_NUMBER ||
		instanceNumber > CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER) {
		ASSERT(0);
		return;
	}
	CWnd* const pWndCheckBox = this->GetDlgItem(nID);
	if (NULL == pWndCheckBox) {
		ASSERT(0);
		return;
	}
	CButton* const pBtn = (CButton*)(pWndCheckBox);
	if (NULL == pBtn) {
		INVARIANT(0);
		return;
	}
	const int state = pBtn->GetCheck() == BST_CHECKED ? 1 : 0;
	if ((0 == state) /* && (this->m_showWarning) */) {	
		CString strTitle;
		strTitle.Format("Disable instance number %d?",instanceNumber);
		CString strMessage("Disabling an unused input simplifies the Duotecno DALI bridge setup. "
						   "However, once disabled, the input will stop sending event messages on the DALI bus.\r\n\r\n"		
					       "Because it won't send event messages anymore, you won’t be able to locate it. "
						   "(If needed, you can re-enable the instance by checking the box.)\r\n\r\n"
						   "Beware if the input was previously connected to a Duotecno input unit, it will no longer work.\r\n\r\n"
						   "Are you sure you want to disable this instance number?");

		if (IDNO == this->MessageBox(strMessage, strTitle, MB_ICONQUESTION | MB_YESNO)) {
		    this->PostMessage(WM_UPDATE_CONTROLS);
			return;
		}		
		this->m_showWarning = false;
	}

	if (this->getWorker().startControlDeviceInstanceEnableOnOff(this->getCurrDeviceShortAddress(), instanceNumber, (state == 0 ? false : true))) {
		this->PostMessage(WM_UPDATE_CONTROLS);
	}
}

void CDlgChildDaliControlDeviceOverview::OnTimer(UINT nIDEvent) {
	TRACER("OnTimer - enter\r\n");
	switch(nIDEvent) {	
		case TIMER_ID_EDITBOX_INSTANCENAME: {
			if (this->updateInstanceNameFromEditBox( )) {

				// Sedert V16.46: Onderstaande zorgt ervoor dat de focus verdwijnt naar een volgende edit box.
				// Dit zorgt ervoor dat de edit box upgedate wordt met de ingegeven naam.
				// Deze is max 2 karakters lang.
				CWnd* pWnd = GetFocus();
				if (pWnd) {
					const DWORD identifier = pWnd->GetDlgCtrlID();			
					if (identifier >= IDC_EDIT_INSTANCE_0 && identifier <= IDC_EDIT_INSTANCE_31) {
						this->helperKillFocusEditBox(identifier);
					}
				}
			}
		} break;	
	}
	this->KillTimer(nIDEvent);
	TRACER("OnTimer - exit\r\n");
}

void CDlgChildDaliControlDeviceOverview::OnEnKillfocusEditBox(UINT nID) {
	TRACER("OnEnKillfocusEditBox - enter\r\n");
	if (this->getBusyGuiControlsUpdate()) {
		ASSERT(this->getBusyGuiControlsUpdate() == false);
		return;
	}
	const int instanceNumber = nID - IDC_EDIT_INSTANCE_0;
	if (instanceNumber < CAppDaliInputControlDeviceInstances::FIRST_INSTANCE_NUMBER ||
		instanceNumber > CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER) {
		ASSERT(0);
		return;
	}

	this->KillTimer(TIMER_ID_EDITBOX_INSTANCENAME);	
	if (this->m_busyEditingName) {
		ASSERT(this->m_currentInstanceName >= 0);		
		this->updateInstanceNameFromEditBox( );
	}
	TRACER("OnEnKillfocusEditBox - exit\r\n");
}

void CDlgChildDaliControlDeviceOverview::OnEnSetfocusEditBox(UINT nID) {
	TRACER("OnEnSetfocusEditBox - enter\r\n");
	if (this->getBusyGuiControlsUpdate()) {
		ASSERT(this->getBusyGuiControlsUpdate() == false);
		return;
	}
	const int instanceNumber = nID - IDC_EDIT_INSTANCE_0;
	if (instanceNumber < CAppDaliInputControlDeviceInstances::FIRST_INSTANCE_NUMBER ||
		instanceNumber > CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER) {
		ASSERT(0);
		return;
	}
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (NULL == pEdit) {
		ASSERT(0);
		return;
	}   

	this->m_currentInstanceName = instanceNumber;
	ASSERT((this->m_currentInstanceName >= CAppDaliInputControlDeviceInstances::FIRST_INSTANCE_NUMBER)&&
		   (this->m_currentInstanceName <= CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER));
    pEdit->GetWindowText(this->m_strPreviousName);
	this->SetTimer(TIMER_ID_EDITBOX_INSTANCENAME, 2000, (TIMERPROC) NULL);
	TRACER("OnEnSetfocusEditBox - exit\r\n");
}

void CDlgChildDaliControlDeviceOverview::OnEnChangeEditControls(UINT nID) {
	TRACER("OnEnChangeEditControls - enter\r\n");
	if (this->getBusyGuiControlsUpdate()) {		// Nodig voor de onChange handler van de edit controls.
		return;
	}
	const int instanceNumber = nID - IDC_EDIT_INSTANCE_0;
	if (instanceNumber < CAppDaliInputControlDeviceInstances::FIRST_INSTANCE_NUMBER ||
		instanceNumber > CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER) {
		ASSERT(0);
		return;
	}
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (NULL == pEdit) {
		ASSERT(0);
		return;
	}

	CString strTemp;
	pEdit->GetWindowText(strTemp);
	if (0 == this->m_strPreviousName.Compare(strTemp)) {
		this->m_busyEditingName = false;
		return;
	}

	this->m_busyEditingName = true;
	this->KillTimer(TIMER_ID_EDITBOX_INSTANCENAME);
	this->SetTimer(TIMER_ID_EDITBOX_INSTANCENAME, 2000, (TIMERPROC) NULL);
	TRACER("OnEnChangeEditControls - exit\r\n");
}

BOOL CDlgChildDaliControlDeviceOverview::updateInstanceNameFromEditBox( ) {
	TRACER("updateInstanceNameFromEditBox - enter\r\n");
	if (!this->m_busyEditingName) {
		return FALSE;
	}
	if (this->m_currentInstanceName < 0) {
		ASSERT(this->m_currentInstanceName >= 0);
		return FALSE;
	}
	CWnd* const pWnd = this->GetDlgItem(IDC_EDIT_INSTANCE_0 + this->m_currentInstanceName);
	if (NULL == pWnd) {
		ASSERT(0);
		return FALSE;
	}

	const int instanceNumber = this->m_currentInstanceName;
	this->m_currentInstanceName = -1;						// Oppassen: Moeten deze wissen ook als de naam niet gewijzijd wordt.
	this->m_busyEditingName = false;

	// Ophalen van de naam in de edit box.
	CString strTemp;				
	pWnd->GetWindowText(strTemp);

	CString strNormalizedName;
	if (false == this->getWorker().setControlDeviceInstanceName(this->getCurrDeviceShortAddress(),instanceNumber,strTemp,&strNormalizedName)) {
		return FALSE;
	}	
	this->setRequestParentUpdateGUI(true);
	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
	TRACER("updateInstanceNameFromEditBox - exit\r\n");
	return TRUE;
}


// Helper functie voor het verplaatsen van de focus van een editbox naar een andere control.
// Probleem: Kunnen de focus enkel verplaatsen naar een andere control wanneer deze control enabled is...
//
// Stappen:
// 1) Proberen de focus te verplaatsen naar een volgende edit conrol.
// 2) Proberen de focus te verplaatsen naar een checkbox.
BOOL CDlgChildDaliControlDeviceOverview::helperKillFocusEditBox(UINT nID) {
	if ((nID < IDC_EDIT_INSTANCE_0) || (nID > IDC_EDIT_INSTANCE_31)) {
		return FALSE;
	}
	
	// Proberen de focus te plaatsen op een andere edit control.
	int tempControlID = nID;
	for (int i=0;i<MAX_INSTANCES;++i) {
		if (++tempControlID >= IDC_EDIT_INSTANCE_31) {
			tempControlID = IDC_EDIT_INSTANCE_0;
		}		
		if (tempControlID != nID) {
			CWnd* const pWnd = this->GetDlgItem(tempControlID);
			if (NULL != pWnd) {
				if (pWnd->IsWindowEnabled()) {
					pWnd->SetFocus();
					return TRUE;
				}
			}
		}
	}

	// Proberen de Focus te plaatsen op de checkbox.
	tempControlID = IDC_CHECK_INSTANCE0;
	for (int i=0;i<MAX_INSTANCES;++i) {
		if (++tempControlID >= IDC_CHECK_INSTANCE31) {
			tempControlID = IDC_CHECK_INSTANCE0;
		}		
		CWnd* const pWnd = this->GetDlgItem(tempControlID);
		if (NULL != pWnd) {
			if (pWnd->IsWindowEnabled()) {
				pWnd->SetFocus();
				return TRUE;
			}
		}
	}
	return FALSE;
}


// Nodig voor de 'Want Return' van de edit box voor het editeren van de naam.
BOOL CDlgChildDaliControlDeviceOverview::PreTranslateMessage(MSG* pMsg) {	
	if (NULL != pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {	
		CWnd* pWnd = GetFocus();
		if (pWnd) {
			const DWORD identifier = pWnd->GetDlgCtrlID();			
			if (identifier >= IDC_EDIT_INSTANCE_0 && identifier <= IDC_EDIT_INSTANCE_31) {
				const int instanceNumber = identifier - IDC_EDIT_INSTANCE_0;		
				ASSERT(instanceNumber < MAX_INSTANCES);
			
				// Focus veplaatsen naar een andere edit box.
				// Indien er maar 1 aanwezig is dan proberen de tekst op te halen.
				if (FALSE == this->helperKillFocusEditBox(identifier)) {				
					if ((this->m_currentInstanceName) >= 0 && (this->m_busyEditingName)) {
						this->updateInstanceNameFromEditBox( );
						KillTimer(TIMER_ID_EDITBOX_INSTANCENAME);
					}
				}
				return TRUE;
			}
		}
	}
    return CDialog::PreTranslateMessage(pMsg);    
}
// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliControlDeviceOverview::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	if ((identifier>=IDC_CHECK_INSTANCE0) && (identifier<= IDC_CHECK_INSTANCE31)) {
		const int instanceNumber = identifier - IDC_CHECK_INSTANCE0;
		if (instanceNumber < MAX_INSTANCES) {
			if (this->m_statesCheckBox[instanceNumber] == STATE_CHECKBOX_ON) {
				this->m_strToolTipText.Format("Disable instance %d (changes are applied immediately)",instanceNumber);
				handled = TRUE;
			}
			else if (this->m_statesCheckBox[instanceNumber] == STATE_CHECKBOX_OFF) {
				this->m_strToolTipText.Format("Enable instance %d (changes are applied immediately)",instanceNumber);
				handled = TRUE;
			}
		}
	}
	else if ((identifier >= IDC_EDIT_INSTANCE_0) && (identifier<=IDC_EDIT_INSTANCE_31)) {
		const int instanceNumber = identifier - IDC_EDIT_INSTANCE_0;
		if (instanceNumber < MAX_INSTANCES) {
			if (this->m_statesCheckBox[instanceNumber] != STATE_CHECKBOX_DISABLED) {			
				this->m_strToolTipText.Format("Name for instance %d (max. 2 characters)", instanceNumber);
				handled = TRUE;
			}
		}
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
