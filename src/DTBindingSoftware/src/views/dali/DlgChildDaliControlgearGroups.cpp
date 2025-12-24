// DlgChildDaliControlgearGroups.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliControlgearGroups.h"
#include "appdlgdaliworker.h"

/****************************************************************************************************/
using namespace DUOTECNO::DALI;
/****************************************************************************************************/
// CDlgChildDaliControlgearGroups dialog
/****************************************************************************************************/
static const CString STRING_FOOTER_OFFLINE("Connect online to read or change the device group configuration.");
static const CString STRING_FOOTER_ONLINE("Read or change the device group configuration. the changes will be applied immediately.");
/****************************************************************************************************/
IMPLEMENT_DYNAMIC(CDlgChildDaliControlgearGroups, CDialog)

CDlgChildDaliControlgearGroups::CDlgChildDaliControlgearGroups(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker, CDlgChildDaliControlgearGroups::IDD, pParent) { }

CDlgChildDaliControlgearGroups::~CDlgChildDaliControlgearGroups() { }

void CDlgChildDaliControlgearGroups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
	DDX_Control(pDX, IDC_BUTTON_READ, m_btnReadConfig);
}


BEGIN_MESSAGE_MAP(CDlgChildDaliControlgearGroups, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_GROUP0, IDC_CHECK_GROUP15, OnButtonClickedGroup)	
	ON_BN_CLICKED(IDC_BUTTON_READ, &CDlgChildDaliControlgearGroups::OnBnClickedButtonReadConfig)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliControlgearGroups::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgChildDaliControlgearGroups message handlers
void CDlgChildDaliControlgearGroups::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	this->setDevice(pDevice);
	if (NULL != pDevice) {
		this->setCurrDeviceShortAddress(pDevice->getShortAddress());
	}
	else {
		this->setCurrDeviceShortAddress(-1);	
	}
	this->updateModel();
	this->updateViewMode();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearGroups::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {
	this->setDevice(pDevice);
	if (NULL != pDevice) {
		this->setCurrDeviceShortAddress(pDevice->getShortAddress());
	}
	else {
		this->setCurrDeviceShortAddress(-1);	
	}
	if (activeDialog) {	
		this->updateModel();
	}
	this->updateViewMode();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearGroups::OnButtonClickedGroup(UINT nID) {		
	CAppDaliDevice* const pThisDevice = this->getDevice();
	if (NULL == pThisDevice) {
		return;
	}

	ASSERT(pThisDevice->getShortAddress() == this->getCurrDeviceShortAddress());

	CButton* const pButton = (CButton*)(this->GetDlgItem(nID));
	if (NULL == pButton) {
		return;
	}

	const int groupID = nID - IDC_CHECK_GROUP0;
	ASSERT(groupID >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupID <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);

	// Controleren van de toestand van de checkbox + doorgeven aan de worker.
	const int state = pButton->GetCheck();
	if (BST_CHECKED == state) {
		if (this->getWorker().checkDeviceAndPromptAddToGroup(this, groupID, pThisDevice->getShortAddress())) {
			this->getWorker().controlGearAddToGroup(*pThisDevice, groupID);
		}
	}
	else {	
		this->getWorker().controlGearRemoveFromGroup(*pThisDevice, groupID);
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}	

LONG CDlgChildDaliControlgearGroups::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	uint16_t flagsGroupInfo = 0;
	CAppDaliDevice* const pDevice = this->getDevice();
	this->setBusyGuiControlsUpdate(true);

	if (!this->getInitDone()) {
		this->EnableToolTips(TRUE);
		this->setInitDone(true);
	}

	if (NULL != pDevice) {
		CAppDaliDeviceProperties_ControlGear* const properties = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
		if (NULL != properties) {
			flagsGroupInfo = properties->getGroupInfo();
		}
	}
	for (int i=0;i<DALI_SYSTEM_CONTROLGEAR_GROUP_MAX;++i) {		
		CWnd* pWnd = this->GetDlgItem(IDC_CHECK_GROUP0 + i);
		if (NULL != pWnd) {

			{
				// Namen updaten.
				CString str;
				CString strGroupName;
				str.Format("G%d",i);

				if (this->m_mapGroupNames.Lookup(i,strGroupName)) {
					if (!strGroupName.IsEmpty()) {
						str += ": ";
						str += strGroupName;
					}
				}
				pWnd->SetWindowText(str);
			}

			// Toestand:
			CButton* const pBtn = (CButton*)(pWnd);
			if (NULL != pBtn) {
				const int mask = (1<<i);
				pBtn->EnableWindow(this->getViewMode() == VIEWMODE_ONLINE);
				if (flagsGroupInfo & mask) {
					pBtn->SetCheck(1);				
				}
				else {
					pBtn->SetCheck(0);
				}			
			}
		}		
	}

	this->m_staticFooterInfoText.SetWindowText((VIEWMODE_OFFLINE == this->getViewMode() ? STRING_FOOTER_OFFLINE : STRING_FOOTER_ONLINE));
	this->m_btnReadConfig.EnableWindow(VIEWMODE_ONLINE == this->getViewMode() ? TRUE : FALSE);

	this->setBusyGuiControlsUpdate(false);
	return TRUE;
}

void CDlgChildDaliControlgearGroups::updateModel() {
	this->getWorker().getGroupsNames(&this->m_mapGroupNames);
}

void CDlgChildDaliControlgearGroups::OnBnClickedButtonReadConfig() {
	const int shortAddress = this->getCurrDeviceShortAddress();
	if (shortAddress < 0) {
		ASSERT(shortAddress>=0);	
		return;
	}
	const bool result = this->getWorker().startReadControlGearDeviceGroups(shortAddress);
	if (result) {

	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// IAppDlgDaliGui_Host	
void CDlgChildDaliControlgearGroups::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_GROUPS == task) {
		this->setDownloadBusy(true);
		this->setDownloadDone(false);
	}
	this->setViewMode(VIEWMODE_ONLINE_BUSY);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliControlgearGroups::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_GROUPS == task) {			
		this->setRequestParentUpdateGUI(true);	// Vragen aan de parent om de andere dialogen up te daten. (nodig voor de groepen.)
		this->setDownloadBusy(false);
		this->setDownloadDone(true);		
	}	
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliControlgearGroups::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());	
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_GROUPS == task) {	
		this->setDownloadBusy(false);	
	}
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliControlgearGroups::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	if (IDC_BUTTON_READ == identifier) {
		this->m_strToolTipText = _T("Read group configuration for this device");
		handled = TRUE;
	}
	else if (identifier>=IDC_CHECK_GROUP0 && identifier<=IDC_CHECK_GROUP15) {
		const int groupNumber = identifier - IDC_CHECK_GROUP0;

		bool member = false;
		CButton* const pBtn = (CButton*)(this->GetDlgItem(identifier));
		if (NULL != pBtn) {
			if (BST_CHECKED == pBtn->GetCheck()) {
				member = true;
			}
		}
		CString strGroupName;
		if (m_mapGroupNames.Lookup(groupNumber,strGroupName)) {
			if (member) {
				this->m_strToolTipText.Format("Remove from the group '%s' (changes are applied immediately)", strGroupName);			
			}
			else {
				this->m_strToolTipText.Format("Add to the group '%s' (changes are applied immediately)", strGroupName);
			}
			handled = TRUE;
		}
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
