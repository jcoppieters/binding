// PropertyPageDaliGroupsOverview.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliGroupsOverview.h"
#include "appdlgdaliworker.h"	

using namespace DUOTECNO::DALI;		// Nodig voor de enums.

enum { WM_UPDATE_CONTROLS = WM_USER + 105 };

// CPropertyPageDaliGroupsOverview dialog

IMPLEMENT_DYNAMIC(CPropertyPageDaliGroupsOverview, CPropertyPage)

CPropertyPageDaliGroupsOverview::CPropertyPageDaliGroupsOverview(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext)
	: CPropertyPageDaliBaseClass(CPropertyPageDaliGroupsOverview::IDD,  refWorker, refContext), m_mode(VIEWMODE_OFFLINE), m_fInitDone(FALSE),
	  m_busyUpdateGUIEditBox(FALSE), m_selectedGroup(0), m_mapNames(), m_modelNamesChanged(FALSE), m_busyEditingName(FALSE), m_strGroupName(), m_pendingUpdateControls(FALSE) {
	for (int i=0;i<MAX_DALI_ADDRESSES;++i) {
		this->m_stateEnabled[i] = FALSE;	
		this->m_stateChecked[i] = FALSE;
	}
}

CPropertyPageDaliGroupsOverview::~CPropertyPageDaliGroupsOverview()
{
}

void CPropertyPageDaliGroupsOverview::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DALI_GROUPS, m_cmbGroups);
	DDX_Control(pDX, IDC_BUTTON_OFF, m_btnOff);
	DDX_Control(pDX, IDC_BUTTON_MAX_LEVEL, m_btnMaxLevel);
	DDX_Control(pDX, IDC_BUTTON_MIN_LEVEL, m_btnMinLevel);
	DDX_Control(pDX, IDC_STATIC_INFO_HEADER, m_staticInfoHeader);
	DDX_Control(pDX, IDC_EDIT_GROUP_NAME, m_editGroupName);	
}


BEGIN_MESSAGE_MAP(CPropertyPageDaliGroupsOverview, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_CBN_SELCHANGE(IDC_COMBO_DALI_GROUPS,OnCbnSelchangeGroup)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_A0, IDC_CHECK_A63, OnButtonClickedAddress)	
	ON_BN_CLICKED(IDC_BUTTON_OFF, &CPropertyPageDaliGroupsOverview::OnBnClickedButtonOff)
	ON_BN_CLICKED(IDC_BUTTON_MAX_LEVEL, &CPropertyPageDaliGroupsOverview::OnBnClickedButtonMaxLevel)
	ON_BN_CLICKED(IDC_BUTTON_MIN_LEVEL, &CPropertyPageDaliGroupsOverview::OnBnClickedButtonMinLevel)	
	ON_EN_CHANGE(IDC_EDIT_GROUP_NAME, &CPropertyPageDaliGroupsOverview::OnEnChangeEditGroupName)
	ON_EN_KILLFOCUS(IDC_EDIT_GROUP_NAME, &CPropertyPageDaliGroupsOverview::OnEnKillfocusEditGroupName)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPropertyPageDaliGroupsOverview message handlers
BOOL CPropertyPageDaliGroupsOverview::OnInitDialog() {
	CPropertyPage::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	this->initDialog();
	this->updateModel();
	this->requestUpdateGuiControlsUpdate();
	return TRUE;
}

BOOL CPropertyPageDaliGroupsOverview::OnSetActive() {	
	if (this->getContext().getFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_GROUPS)) {
		this->initDialog();
		this->m_modelNamesChanged = TRUE;		// Nodig voor de namen van de groupen/devices.
		this->getContext().clearFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_GROUPS);
	}

	if (this->updateModel()) {
		this->requestUpdateGuiControlsUpdate();
	}
	return CPropertyPage::OnSetActive();
}

BOOL CPropertyPageDaliGroupsOverview::OnKillActive() {
	if (this->m_busyEditingName) {
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
		this->GetEditBoxName();	
	}
	return CPropertyPage::OnKillActive();
}

void CPropertyPageDaliGroupsOverview::initDialog() {
	this->m_strGroupName.Empty();
	this->m_busyEditingName = FALSE;
	for (int i=0;i<MAX_DALI_ADDRESSES;++i) {
		this->m_stateEnabled[i] = FALSE;	
		this->m_stateChecked[i] = FALSE;
	}
}

BOOL CPropertyPageDaliGroupsOverview::updateModel() {
	BOOL r[3] = { FALSE, FALSE, FALSE };
	if (this->getWorker().isConnected()) {
		if (this->getWorker().getDaliCfgTaskBusy()) {
			this->m_mode = VIEWMODE_ONLINE_BUSY;
		}
		else {
			this->m_mode = VIEWMODE_ONLINE;
		}
	}
	else {
		this->m_mode = VIEWMODE_OFFLINE;
	}

	if (!this->m_busyEditingName) {
		this->m_strGroupName = this->getWorker().getGroupName(this->m_selectedGroup);
	}

	r[0] = this->updateModelDeviceFound();
	r[1] = this->updateModelDeviceInGroup();
	r[2] = this->updateModelDeviceNames();
	return ((r[0] == TRUE) || (r[1] == TRUE) || (r[2] == TRUE));	// Bij een verandering return true...
}

BOOL CPropertyPageDaliGroupsOverview::updateModelDeviceFound() {
	BOOL result = FALSE;
	if (VIEWMODE_OFFLINE == this->m_mode || VIEWMODE_ONLINE_BUSY == this->m_mode) {		
		for (int i=0;i<MAX_DALI_ADDRESSES;i++) {
			if (this->m_stateEnabled[i] != FALSE) {
				this->m_stateEnabled[i] = FALSE;
				result = TRUE;
			}			
		}	
	}
	else {
		CList<int> list;
		this->getWorker().getListControlGearAddress(&list);
		for (int i=0;i<MAX_DALI_ADDRESSES;i++) {
			if (list.Find(i)) {
				if (this->m_stateEnabled[i] == FALSE) {
					this->m_stateEnabled[i] = TRUE;
					result = TRUE;
				}			
			}
			else {
				if (this->m_stateEnabled[i] == TRUE) {
					this->m_stateEnabled[i] = FALSE;
					result = TRUE;
				}
			}
		}
	}
	return result;
}

BOOL CPropertyPageDaliGroupsOverview::updateModelDeviceInGroup() {
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return FALSE;
	}

	BOOL result = FALSE;
	CList<int> list;
	this->getWorker().getListControlGearGroupMembers(this->m_selectedGroup,&list);
	for (int i=0;i<MAX_DALI_ADDRESSES;i++) {
		if (list.Find(i)) {
			if (this->m_stateChecked[i] == FALSE) {
				this->m_stateChecked[i] = TRUE;
				result = TRUE;
			}
		}
		else {
			if (this->m_stateChecked[i] == TRUE) {
				this->m_stateChecked[i] = FALSE;
				result = TRUE;
			}
		}		
	}
	return result;
}

// TODO: Code optimaliseren.
// Enkel wanneer er een naam anders is de vlag m_modelNamesChanged op 1 zetten...
BOOL CPropertyPageDaliGroupsOverview::updateModelDeviceNames() {
	this->m_mapNames.RemoveAll();

	CList<int> list;
	this->getWorker().getListControlGearAddress(&list);
	for (int i = 0; i < MAX_DALI_ADDRESSES; i++) {
		if (list.Find(i)) {
			CString strName;
			if (!this->getWorker().getDeviceName(DALI_DEVICE_TYPE_CONTROLGEAR, i, &strName)) {
				strName.Empty();
			}
			this->m_mapNames.SetAt(i,strName);
			this->m_modelNamesChanged = TRUE;
		}
	}
	return TRUE;
}

LONG CPropertyPageDaliGroupsOverview::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	if (FALSE == this->m_fInitDone) {		
		for (int i=0;i<DALI_SYSTEM_CONTROLGEAR_GROUP_MAX;i++) {
			CString strGroup;
			strGroup.Format("Group %d",i);
			const int index = m_cmbGroups.AddString(strGroup);
			if (index >=0) {
				m_cmbGroups.SetItemData(index,i);	
			}
		}
		this->m_cmbGroups.SetCurSel(this->m_selectedGroup);

		// Tekst header
		const CString strInfo("Group Configuration of control gear devices.\r\n"
				   			  "Changes are applied immediately in the DALI system. On the right, you can test the current group setup.\r\n"
							  "You need to be online to use this configuration.");
		m_staticInfoHeader.SetWindowText(strInfo);

		// init done vlag.
		this->m_fInitDone = TRUE;
	}
		
	m_cmbGroups.EnableWindow(VIEWMODE_ONLINE_BUSY == this->m_mode ? FALSE : TRUE); 
	m_btnOff.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnMaxLevel.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnMinLevel.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	
	for (int i = 0;i<MAX_DALI_ADDRESSES;++i) {				
		CWnd* pWnd = this->GetDlgItem(IDC_CHECK_A0 + i);
		if (NULL != pWnd) {
			CButton* const pBtn = (CButton*)(pWnd);
			if (NULL != pBtn) {
				pBtn->EnableWindow(this->m_stateEnabled[i]);
				pBtn->SetCheck(this->m_stateChecked[i]);
				this->UpdateBtnName(pBtn, i);				
			}
		}	
	}

	this->UpdateEditBoxGroupName();		// De Naam van de groep.	

	// Vlaggen.
	this->m_modelNamesChanged = FALSE;
	this->m_pendingUpdateControls = FALSE;
	return true;
}

void CPropertyPageDaliGroupsOverview::UpdateEditBoxGroupName() {
	ASSERT(FALSE == this->m_busyEditingName);
	this->m_busyUpdateGUIEditBox = TRUE;
	this->m_editGroupName.SetWindowText(this->m_strGroupName);
	this->m_busyUpdateGUIEditBox = FALSE;
}

void CPropertyPageDaliGroupsOverview::UpdateBtnName(CButton* pButton, int shortAddress) {
	if (!this->m_modelNamesChanged) {
		return;
	}
	CString strName;	
	if (!this->m_mapNames.Lookup(shortAddress, strName)) {
		strName.Empty();
	}		

	CString strAddress;
	this->getWorker().formatStringAddress(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &strAddress);	

	CString strLabel;
	if (strName.IsEmpty()) {
		strLabel = strAddress;
	}
	else {
		strLabel = strAddress + " - " + strName;
	}
	pButton->SetWindowText(strLabel);	
}

void CPropertyPageDaliGroupsOverview::OnCbnSelchangeGroup() {
	int index = this->m_cmbGroups.GetCurSel();
	if (index < 0) {		
		ASSERT(index >= 0);
		return;
	}		
	this->m_selectedGroup = this->m_cmbGroups.GetItemData(index);		
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(this->m_selectedGroup >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && this->m_selectedGroup <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
		return;
	}
	if (this->updateModel()) {
		this->requestUpdateGuiControlsUpdate();
	}	
}

void CPropertyPageDaliGroupsOverview::requestUpdateGuiControlsUpdate() {
	if (!this->m_pendingUpdateControls) {
		this->PostMessage(WM_UPDATE_CONTROLS);
		this->m_pendingUpdateControls = TRUE;
	}
}

void CPropertyPageDaliGroupsOverview::OnButtonClickedAddress(UINT nID) {	
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	const int shortAddress = nID - IDC_CHECK_A0;
	if (shortAddress < DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST || shortAddress > DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
		ASSERT(0);
		return;
	}
	CWnd* pWnd = this->GetDlgItem(nID);
	if (NULL == pWnd) {
		return;
	}	
	CButton* const pBtn = (CButton*)(pWnd);
	if (NULL == pBtn) {
		return;
	}	

	if (pBtn->GetCheck()) {
		if (this->getWorker().checkDeviceAndPromptAddToGroup(this,this->m_selectedGroup, shortAddress)) {
			this->getWorker().controlGearAddToGroup(shortAddress, this->m_selectedGroup);
		}
	}
	else {
		this->getWorker().controlGearRemoveFromGroup(shortAddress,this->m_selectedGroup);
	}
		
	if (this->updateModel()) {
		this->requestUpdateGuiControlsUpdate();
	}	
}

void CPropertyPageDaliGroupsOverview::OnBnClickedButtonOff() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	this->getWorker().sendDeviceOff(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup);
}

void CPropertyPageDaliGroupsOverview::OnBnClickedButtonMaxLevel() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	this->getWorker().sendDeviceMaxLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup);
}

void CPropertyPageDaliGroupsOverview::OnBnClickedButtonMinLevel() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	this->getWorker().sendDeviceMinLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup);
}

void CPropertyPageDaliGroupsOverview::GetEditBoxName() {
	enum { MAX_NAME_LENGTH = 15 };
	if (!this->m_busyEditingName) {
		return;
	}
	this->m_busyEditingName = FALSE;
					
	CString strNewName;
	// CString strOldName = this->m_strGroupName;
	this->m_editGroupName.GetWindowText(strNewName);
	if (0 == strNewName.Compare(this->m_strGroupName)) {
		return;
	}	
	this->m_strGroupName = strNewName.Left(MAX_NAME_LENGTH);	
	this->getWorker().setGroupName(this->m_selectedGroup,this->m_strGroupName);

	if (0 != strNewName.Compare(this->m_strGroupName)) {
		this->SetTimer(TIMER_ID_EDITBOX_NAME_UPDATE,100,(TIMERPROC)NULL);
	}
}

void CPropertyPageDaliGroupsOverview::OnEnChangeEditGroupName() {
	if (this->m_busyUpdateGUIEditBox) {
		return;
	}
	this->m_busyEditingName = TRUE;
	this->KillTimer(TIMER_ID_EDITBOX_NAME);
	this->SetTimer(TIMER_ID_EDITBOX_NAME, 2000, (TIMERPROC) NULL);
}
void CPropertyPageDaliGroupsOverview::OnEnKillfocusEditGroupName() {
	if (this->m_busyEditingName) {
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
		this->GetEditBoxName();		
	}
}

void CPropertyPageDaliGroupsOverview::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_EDITBOX_NAME == nIDEvent) {
		this->GetEditBoxName( );
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
	}
	else if (TIMER_ID_EDITBOX_NAME_UPDATE == nIDEvent) {
		// De naam updaten van de edit box.
		this->UpdateEditBoxGroupName();		// De Naam van de groep.	
		this->KillTimer(TIMER_ID_EDITBOX_NAME_UPDATE);
	}
}

void CPropertyPageDaliGroupsOverview::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) {
	this->updateModel();
	this->requestUpdateGuiControlsUpdate();
}

void CPropertyPageDaliGroupsOverview::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) 
	{			
		this->updateModel();
		this->requestUpdateGuiControlsUpdate();
	}
}

void CPropertyPageDaliGroupsOverview::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) 
	{			
		this->updateModel();
		this->requestUpdateGuiControlsUpdate();
	}
}
void CPropertyPageDaliGroupsOverview::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) 
	{			
		this->updateModel();
		this->requestUpdateGuiControlsUpdate();
	}
}