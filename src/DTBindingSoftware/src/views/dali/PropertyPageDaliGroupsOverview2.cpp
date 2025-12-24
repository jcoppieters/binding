// PropertyPageDaliGroupsOverview2.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliGroupsOverview2.h"
#include "DlgChildDaliGroupsOverviewGroups.h"
#include "DlgChildDaliGroupsOverviewProperties.h"
#include "appdlgdaliworker.h"	

using namespace DUOTECNO::DALI;		// Nodig voor de enums.

enum { WM_UPDATE_CONTROLS = WM_USER + 105 };

// CPropertyPageDaliGroupsOverview2 dialog

IMPLEMENT_DYNAMIC(CPropertyPageDaliGroupsOverview2, CPropertyPage)

CPropertyPageDaliGroupsOverview2::CPropertyPageDaliGroupsOverview2(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext) :
	CPropertyPageDaliBaseClass(CPropertyPageDaliGroupsOverview2::IDD,  refWorker, refContext), 
	m_mode(VIEWMODE_OFFLINE), m_previousChildDialog(-1), m_fInitDone(FALSE),
	m_busyUpdateGUIEditBox(FALSE), m_selectedGroup(0), m_busyEditingName(FALSE), m_strGroupName(), m_pendingUpdateControls(FALSE) {
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;++i) {
		this->m_pArrChildDialogs[i] = NULL;
	}	
}

CPropertyPageDaliGroupsOverview2::~CPropertyPageDaliGroupsOverview2() {
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;++i) {
		if (NULL != this->m_pArrChildDialogs[i]) {		
			this->m_pArrChildDialogs[i]->DestroyWindow();
			delete this->m_pArrChildDialogs[i];
			this->m_pArrChildDialogs[i] = NULL;
		}
	}
}

void CPropertyPageDaliGroupsOverview2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DALI_GROUPS, m_cmbGroups);	
	DDX_Control(pDX, IDC_STATIC_INFO_HEADER, m_staticInfoHeader);
	DDX_Control(pDX, IDC_EDIT_GROUP_NAME, m_editGroupName);	
	DDX_Control(pDX, IDC_TAB_DIALOGS, m_tabDialogs);
	DDX_Control(pDX, IDC_BUTTON_OFF, m_btnOff);
	DDX_Control(pDX, IDC_BUTTON_MAX_LEVEL, m_btnMaxLevel);
	DDX_Control(pDX, IDC_BUTTON_MIN_LEVEL, m_btnMinLevel);
	DDX_Control(pDX, IDC_BUTTON_DIM_DOWN, m_btnDimDown);
	DDX_Control(pDX, IDC_BUTTON_DIM_UP, m_btnDimUp);
	DDX_Control(pDX, IDC_BUTTON_STEP_DOWN, m_btnStepDown);
	DDX_Control(pDX, IDC_BUTTON_STEP_UP, m_btnStepUp);
	DDX_Control(pDX, IDC_EDIT_ACTUAL_VALUE, m_editActualValue);
	DDX_Control(pDX, IDC_BUTTON_VALUE_SET, m_btnSetActualValue);
	DDX_Control(pDX, IDC_SPIN_ACTUAL_VALUE, m_spinCtrlActualValue);
}

BEGIN_MESSAGE_MAP(CPropertyPageDaliGroupsOverview2, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_CBN_SELCHANGE(IDC_COMBO_DALI_GROUPS,OnCbnSelchangeGroup)		
	ON_EN_CHANGE(IDC_EDIT_GROUP_NAME, &CPropertyPageDaliGroupsOverview2::OnEnChangeEditGroupName)
	ON_EN_KILLFOCUS(IDC_EDIT_GROUP_NAME, &CPropertyPageDaliGroupsOverview2::OnEnKillfocusEditGroupName)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DIALOGS, &CPropertyPageDaliGroupsOverview2::OnTabChanged)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_DIALOGS, &CPropertyPageDaliGroupsOverview2::OnTabChanging)
	ON_BN_CLICKED(IDC_BUTTON_OFF, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonOff)
	ON_BN_CLICKED(IDC_BUTTON_MAX_LEVEL, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonMaxLevel)
	ON_BN_CLICKED(IDC_BUTTON_MIN_LEVEL, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonMinLevel)	
	ON_BN_CLICKED(IDC_BUTTON_DIM_DOWN, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonDimDown)	
	ON_BN_CLICKED(IDC_BUTTON_DIM_UP, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonDimUp)	
	ON_BN_CLICKED(IDC_BUTTON_STEP_DOWN, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonStepDown)	
	ON_BN_CLICKED(IDC_BUTTON_STEP_UP, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonStepUp)	
	ON_BN_CLICKED(IDC_BUTTON_VALUE_SET, &CPropertyPageDaliGroupsOverview2::OnBnClickedButtonValueSet)	
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ACTUAL_VALUE, &CPropertyPageDaliGroupsOverview2::OnDeltaposSpinCtrlActualValue)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CPropertyPageDaliGroupsOverview2::OnToolTipNeedText)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPropertyPageDaliGroupsOverview message handlers
BOOL CPropertyPageDaliGroupsOverview2::OnInitDialog() {
	CPropertyPage::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	this->initDialog();
	this->updateModel();
	this->requestUpdateGuiControlsUpdate();
	return TRUE;
}

BOOL CPropertyPageDaliGroupsOverview2::OnSetActive() {		
	if (this->getContext().getFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_GROUPS)) {
		this->initDialog();	
		this->getContext().clearFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_GROUPS);
	}
	if (this->updateModel()) {
		this->requestUpdateGuiControlsUpdate();
	}

	// Doorgeven aan de child dialogen dat de parent actief geworden is.
	// Zoeken naar het tabblad die momenteel aktief en melden...
	const int currentActiveDialog = tabDialogsGetCurrSelectedDialog(m_tabDialogs);	
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; i++) {
		if (NULL != this->m_pArrChildDialogs[i]) {
			const bool activeDialog = (currentActiveDialog == i ? true : false);		
			this->m_pArrChildDialogs[i]->OnParentPropertyPageActive(this->m_selectedGroup,activeDialog);
		}
	}		

	return CPropertyPage::OnSetActive();
}

BOOL CPropertyPageDaliGroupsOverview2::OnKillActive() {
	if (this->m_busyEditingName) {
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
		this->GetEditBoxName();	
	}
	return CPropertyPage::OnKillActive();
}

void CPropertyPageDaliGroupsOverview2::initDialog() {
	this->m_selectedGroup = 0;
	this->m_strGroupName.Empty();
	this->m_busyEditingName = FALSE;
	this->initTabDialogs( );
	this->setChildDialogGroupNumber(this->m_selectedGroup);
	this->EnableToolTips(TRUE);
}

BOOL CPropertyPageDaliGroupsOverview2::PreTranslateMessage(MSG* pMsg)
{
#if(0)
    if (m_ToolTip.m_hWnd != NULL)
        m_ToolTip.RelayEvent(pMsg);
#endif
    return CPropertyPage::PreTranslateMessage(pMsg);
}

void CPropertyPageDaliGroupsOverview2::initTabDialogs() {
	if (FALSE == this->m_fInitDone) {
		if (NULL == this->m_pArrChildDialogs[DIALOG_CHILD_DALI_GROUPSOVERVIEW_GROUPS]) {
			this->m_pArrChildDialogs[DIALOG_CHILD_DALI_GROUPSOVERVIEW_GROUPS] =  new CDlgChildDaliGroupsOverviewGroups(this->getWorker(),this);
			this->m_pArrChildDialogs[DIALOG_CHILD_DALI_GROUPSOVERVIEW_GROUPS]->Create(CDlgChildDaliGroupsOverviewGroups::IDD,&m_tabDialogs);
		}	
		if (NULL == this->m_pArrChildDialogs[DIALOG_CHILD_DALI_GROUPSOVERVIEW_PROPERTIES]) {
			this->m_pArrChildDialogs[DIALOG_CHILD_DALI_GROUPSOVERVIEW_PROPERTIES] =  new CDlgChildDaliGroupsOverviewProperties(this->getWorker(),this);
			this->m_pArrChildDialogs[DIALOG_CHILD_DALI_GROUPSOVERVIEW_PROPERTIES]->Create(CDlgChildDaliGroupsOverviewProperties::IDD,&m_tabDialogs);
		}	

		m_tabDialogs.DeleteAllItems();	
		m_tabDialogs.ShowWindow(TRUE);		
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,0,"Members",0,DIALOG_CHILD_DALI_GROUPSOVERVIEW_GROUPS,0,0);			
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,1,"Properties",0,DIALOG_CHILD_DALI_GROUPSOVERVIEW_PROPERTIES,0,0);		
		this->OnTabChanged(NULL, NULL);
		m_tabDialogs.SetCurSel(0);
	}
}

BOOL CPropertyPageDaliGroupsOverview2::updateModel() {
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
	return TRUE;
}

LONG CPropertyPageDaliGroupsOverview2::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	if (FALSE == this->m_fInitDone) {
		this->initComboBoxGroups( );
		this->initSpinCtrlActualValue( );
		this->initTextFooter( );
		this->m_fInitDone = TRUE;
	}
	m_btnOff.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnMaxLevel.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnMinLevel.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_cmbGroups.EnableWindow(VIEWMODE_ONLINE_BUSY == this->m_mode ? FALSE : TRUE); 
	m_btnDimDown.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnDimUp.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnStepDown.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnStepUp.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_btnSetActualValue.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_editActualValue.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);
	m_spinCtrlActualValue.EnableWindow(VIEWMODE_ONLINE == this->m_mode ? TRUE : FALSE);

	this->m_cmbGroups.SetCurSel(this->m_selectedGroup);		// selectie combobox.
	this->UpdateEditBoxGroupName();							// De Naam van de groep.	

	// Vlaggen.	
	this->m_pendingUpdateControls = FALSE;
	return true;
}

void CPropertyPageDaliGroupsOverview2::initComboBoxGroups( ) {
	for (int i=0;i<DALI_SYSTEM_CONTROLGEAR_GROUP_MAX;i++) {
		CString strGroup;
		strGroup.Format("Group %d",i);
		const int index = m_cmbGroups.AddString(strGroup);
		if (index >=0) {
			m_cmbGroups.SetItemData(index,i);	
		}
	}		
}
void CPropertyPageDaliGroupsOverview2::initSpinCtrlActualValue( ) {

	// Spin ctrl and edit box.
	m_spinCtrlActualValue.SetRange(ACTUAL_LEVEL_MIN, ACTUAL_LEVEL_MAX);

	// Startwaarde instellen
	m_editActualValue.SetWindowText("128");
	m_spinCtrlActualValue.SetPos(128);
}

void CPropertyPageDaliGroupsOverview2::initTextFooter( ) {

	CString strInfo("Group Configuration of control gear devices.\r\n"
				    "Changes are applied immediately in the DALI system. On the right, you can test the current group setup.\r\n"
					"You need to be online to use this configuration.");
	m_staticInfoHeader.SetWindowText(strInfo);
}

void CPropertyPageDaliGroupsOverview2::UpdateEditBoxGroupName() {
	ASSERT(FALSE == this->m_busyEditingName);
	this->m_busyUpdateGUIEditBox = TRUE;
	this->m_editGroupName.SetWindowText(this->m_strGroupName);
	this->m_busyUpdateGUIEditBox = FALSE;
}

void CPropertyPageDaliGroupsOverview2::OnCbnSelchangeGroup() {
	int index = this->m_cmbGroups.GetCurSel();
	if (index < 0) {		
		ASSERT(index >= 0);
		return;
	}		

	const int previousSelectedGroup = this->m_selectedGroup;
	this->m_selectedGroup = this->m_cmbGroups.GetItemData(index);		

	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(this->m_selectedGroup >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && this->m_selectedGroup <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
		return;
	}
	if (previousSelectedGroup != this->m_selectedGroup) {
		this->setChildDialogGroupNumber(this->m_selectedGroup);
	}
	if (this->updateModel()) {		
		this->requestUpdateGuiControlsUpdate();		
	}	
}

void CPropertyPageDaliGroupsOverview2::requestUpdateGuiControlsUpdate() {
	if (!this->m_pendingUpdateControls) {	
		this->PostMessage(WM_UPDATE_CONTROLS);
		this->m_pendingUpdateControls = TRUE;
	}
}

void CPropertyPageDaliGroupsOverview2::GetEditBoxName() {
	enum { MAX_NAME_LENGTH = 15 };
	if (!this->m_busyEditingName) {
		return;
	}
	this->m_busyEditingName = FALSE;
					
	CString strNewName;
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

void CPropertyPageDaliGroupsOverview2::setChildDialogGroupNumber(int groupNumber) {
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;i++) {
		if (NULL != this->m_pArrChildDialogs[i]) {
			this->m_pArrChildDialogs[i]->init(groupNumber);
		}
	}
}

void CPropertyPageDaliGroupsOverview2::OnEnChangeEditGroupName() {
	if (this->m_busyUpdateGUIEditBox) {
		return;
	}
	this->m_busyEditingName = TRUE;
	this->KillTimer(TIMER_ID_EDITBOX_NAME);
	this->SetTimer(TIMER_ID_EDITBOX_NAME, 2000, (TIMERPROC) NULL);
}

void CPropertyPageDaliGroupsOverview2::OnEnKillfocusEditGroupName() {
	if (this->m_busyEditingName) {
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
		this->GetEditBoxName();		
	}
}

void CPropertyPageDaliGroupsOverview2::OnTimer(UINT nIDEvent) {
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

// Result 1 wil zeggen dat er niet mag verlaten worden.
void CPropertyPageDaliGroupsOverview2::OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult) {

	DialogID_t dialogID = DIALOG_CHILD_NONE;
	const int nIndex = m_tabDialogs.GetCurSel();	

	if (0 == nIndex) {	
		dialogID = DIALOG_CHILD_DALI_GROUPSOVERVIEW_GROUPS; 
	}
	else if (1 == nIndex) {
		dialogID = DIALOG_CHILD_DALI_GROUPSOVERVIEW_PROPERTIES;
	}
	
		// Vorige actieve dialog: On Leave
	if (this->m_previousChildDialog >= 0 && this->m_previousChildDialog < DIALOG_CHILD_MAX_DIALOGS) {			
		if (NULL != this->m_pArrChildDialogs[this->m_previousChildDialog]) {
			this->m_pArrChildDialogs[this->m_previousChildDialog]->onTabDialogLeaveDialog();
		}	
	}

	// Nieuwe dialog actief zetten + OnEnter()
	this->showActiveDaliog(dialogID);

	// Bewaren van de vorige dialoog.
	this->m_previousChildDialog = (int) dialogID;
}

void CPropertyPageDaliGroupsOverview2::OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult) {
	*pResult = 0;		
}

void CPropertyPageDaliGroupsOverview2::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) {
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;i++) {
		if (NULL != this->m_pArrChildDialogs[i]) {
			this->m_pArrChildDialogs[i]->onNotifyGuiStatusConnected(bNodeAddress,statusConnected);
		}
	}
	this->updateModel();
	this->requestUpdateGuiControlsUpdate();
}

void CPropertyPageDaliGroupsOverview2::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) 
	{			
		this->updateModel();
		this->requestUpdateGuiControlsUpdate();
	}
}

void CPropertyPageDaliGroupsOverview2::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) 
	{			
		this->updateModel();
		this->requestUpdateGuiControlsUpdate();
	}
}
void CPropertyPageDaliGroupsOverview2::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) 
	{			
		this->updateModel();
		this->requestUpdateGuiControlsUpdate();
	}
}

void CPropertyPageDaliGroupsOverview2::showActiveDaliog(DialogID_t dialog) {
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;i++) {
		if ((i != dialog) && (NULL != this->m_pArrChildDialogs[i])) {
			this->m_pArrChildDialogs[i]->ShowWindow(SW_HIDE);		
		}
	}

	if (dialog >= DIALOG_CHILD_MAX_DIALOGS) {
		return;
	}

	CDlgChildDaliGroupsOverviewBase* const pDlg = this->m_pArrChildDialogs[(int)dialog];
	ASSERT(NULL != pDlg);
	if (NULL == pDlg) {
		return;
	}
	
	RECT rc;
	m_tabDialogs.GetItemRect(0,&rc);
	pDlg->SetWindowPos( NULL, rc.left + 1, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	pDlg->SetFocus();			
	pDlg->onTabDialogEnterDialog();
}

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonOff() {
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

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonMaxLevel() {
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

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonMinLevel() {
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

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonDimDown() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	this->getWorker().sendDeviceDimDown(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup);
}

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonDimUp() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	this->getWorker().sendDeviceDimUp(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup);
}

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonStepDown() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	this->getWorker().sendDeviceDimStepDown(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup);
}

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonStepUp() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	this->getWorker().sendDeviceDimStepUp(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup);
}

void CPropertyPageDaliGroupsOverview2::OnBnClickedButtonValueSet() {
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->m_selectedGroup < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->m_selectedGroup > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}

	int iValue;
    CString strValue;

    // Haal huidige waarde uit de Edit Control
    m_editActualValue.GetWindowText(strValue);
	iValue = atoi((LPCTSTR)strValue);

	// Limieten afdwingen
	if (iValue < ACTUAL_LEVEL_MIN || iValue > ACTUAL_LEVEL_MAX) {
		this->MessageBox("Wrong value for actual level");	
		return;
	}  
	this->getWorker().sendDeviceDimActualLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_GROUP, this->m_selectedGroup,iValue);
}

void CPropertyPageDaliGroupsOverview2::OnDeltaposSpinCtrlActualValue(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	int iValue;
    CString strValue;

    // Haal huidige waarde uit de Edit Control
    m_editActualValue.GetWindowText(strValue);
	iValue = atoi((LPCTSTR)strValue);

    // Verhoog of verlaag de waarde
    iValue += pNMUpDown->iDelta;

    // Limieten afdwingen
    if (iValue < ACTUAL_LEVEL_MIN) iValue = ACTUAL_LEVEL_MIN;
    if (iValue > ACTUAL_LEVEL_MAX) iValue = ACTUAL_LEVEL_MAX;

    // Zet de nieuwe waarde
    strValue.Format("%d", iValue);
    m_editActualValue.SetWindowText(strValue);
    m_spinCtrlActualValue.SetPos(iValue);

    *pResult = 0;
}

int CPropertyPageDaliGroupsOverview2::tabDialogsGetCurrSelectedDialog(CTabCtrl& r) {
	const int nNewSel = r.GetCurSel();
	if (nNewSel <  0) {
		return -1;
	}	
	TCITEM item;
	item.lParam = 0;
	item.mask = TCIF_PARAM;
	if (FALSE == r.GetItem(nNewSel, &item)) {  		
		return -1;
	}	
	const LPARAM lParamValue = item.lParam;
	if ((lParamValue >=0) && (lParamValue > DIALOG_CHILD_MAX_DIALOGS)) {
		ASSERT(lParamValue >= 0);
		ASSERT(lParamValue < DIALOG_CHILD_MAX_DIALOGS);
		return -1;
	}	
	return (int) lParamValue;				
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CPropertyPageDaliGroupsOverview2::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_COMBO_DALI_GROUPS: {
			this->m_strToolTipText = _T("Select DALI group (Range: 0-15)");
			handled = TRUE;
		} break;
		case IDC_EDIT_GROUP_NAME: {
			this->m_strToolTipText = _T("Enter the DALI group name");
			handled = TRUE;				  
		} break;
		case IDC_BUTTON_OFF: {
			this->m_strToolTipText = _T("Send off command to the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_MIN_LEVEL: {
			this->m_strToolTipText = _T("Send minimum level command to the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_MAX_LEVEL: {
			this->m_strToolTipText = _T("Send maximum level command to the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_STEP_DOWN: {
			this->m_strToolTipText = _T("Send step down command to decrease the group value");
			handled = TRUE;
		} break;
		case IDC_BUTTON_STEP_UP: {
			this->m_strToolTipText = _T("Send step up command to increase the group value");
			handled = TRUE;
		} break;		
		case IDC_EDIT_ACTUAL_VALUE: {
			this->m_strToolTipText = _T("Change the current group value (Range: 0-254)");
			handled = TRUE;
		} break;
		case IDC_BUTTON_VALUE_SET: {
			this->m_strToolTipText = _T("Send the actual value to the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_DIM_DOWN: {
			this->m_strToolTipText = _T("Send dim down command to the group");
			handled = TRUE;			  
		} break;
		case IDC_BUTTON_DIM_UP: {
			this->m_strToolTipText = _T("Send dim up command to the group");
			handled = TRUE;			  
		} break;

#if(0)
		case IDC_TAB_DIALOGS: {						
			 // 1. Haal de muispositie op in schermcoördinaten
			POINT ptScreen;
			::GetCursorPos(&ptScreen);

			// 2. Zet om naar *clientcoördinaten van de TabCtrl*
			POINT ptClient = ptScreen;
			::ScreenToClient(m_tabDialogs.GetSafeHwnd(), &ptClient); // belangrijk!

			// Itereer over de tabbladen
			int tabCount = m_tabDialogs.GetItemCount();
			for (int i = 0; i < tabCount; ++i) {
				CRect rect;
				m_tabDialogs.GetItemRect(i, &rect);
				if (rect.PtInRect(ptClient)) {				
					switch (i) {
						case 0: 
							this->m_strToolTipText = _T("Add or remove devices"); 
							handled = TRUE;								
						break;
						case 1: 
							this->m_strToolTipText =_T("Set control gear group properties"); 
							handled = TRUE;		
						break;
					}
				}

				if (handled) break;
			}							  
		} break;
#endif 
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
