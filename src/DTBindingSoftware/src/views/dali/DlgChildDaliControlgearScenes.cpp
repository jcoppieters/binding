// DlgChildDaliControlgearScenes.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliControlgearScenes.h"
#include "DlgDaliControlGearDT8Scene.h"
#include "appdlgdaliworker.h"

using namespace DUOTECNO::DALI;

// CDlgChildDaliControlgearScenes dialog

IMPLEMENT_DYNAMIC(CDlgChildDaliControlgearScenes, CDialog)

CDlgChildDaliControlgearScenes::CDlgChildDaliControlgearScenes(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker, CDlgChildDaliControlgearScenes::IDD, pParent), m_pDialogDT8Scenes(NULL),
	  m_properties(NULL), m_strPreviousValue(), m_mapNames(), m_strSceneName( ), m_selectedScene(0), m_busyEditingName(FALSE), 
	  m_supportDT8ColorObjects(false)
{
	this->setInitDone(false);
}

CDlgChildDaliControlgearScenes::~CDlgChildDaliControlgearScenes()
{	
	this->deallocateDlgDT8Scenes();
}

void CDlgChildDaliControlgearScenes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON_READ, m_btnReadConfig);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
	DDX_Control(pDX, IDC_COMBO_DALI_SCENES, m_cmbSceneSelect);
	DDX_Control(pDX, IDC_GROUP_DALI_SCENE_TEST, m_groupSceneTest);
	DDX_Control(pDX, IDC_BUTTON_DALI_SCENE_EXECUTE, m_btnTestScene);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_btnRestoreDefaults);
	DDX_Control(pDX, IDC_BUTTON_UNDO, m_btnUndoChanges);
	DDX_Control(pDX, IDC_EDIT_SCENE_NAME, m_editSceneName);
}

BEGIN_MESSAGE_MAP(CDlgChildDaliControlgearScenes, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_SCENE0, IDC_CHECK_SCENE15, OnButtonClickedScene)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT_SCENE0, IDC_EDIT_SCENE15, OnEnSetfocusEditBox)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_SCENE0, IDC_EDIT_SCENE15, OnEnKillfocusEditBox)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_SCENE0, IDC_EDIT_SCENE15, OnEnChangeEditControls)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_COLOR_SCENE0, IDC_BUTTON_COLOR_SCENE15, OnButtonClickedColor)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CDlgChildDaliControlgearScenes::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CDlgChildDaliControlgearScenes::OnBnClickedButtonReadConfig)
	ON_BN_CLICKED(IDC_BUTTON_DALI_SCENE_EXECUTE, &CDlgChildDaliControlgearScenes::OnBnClickedButtonDaliSceneExecute)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CDlgChildDaliControlgearScenes::OnBnClickedButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, &CDlgChildDaliControlgearScenes::OnBnClickedButtonUndo)
	ON_CBN_SELCHANGE(IDC_COMBO_DALI_SCENES,OnCbnSelchangeSceneNumber)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SCENE_NAME, &CDlgChildDaliControlgearScenes::OnEnChangeEditSceneName)
	ON_EN_KILLFOCUS(IDC_EDIT_SCENE_NAME, &CDlgChildDaliControlgearScenes::OnEnKillfocusEditSceneName)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliControlgearScenes::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgChildDaliControlgearScenes message handlers
void CDlgChildDaliControlgearScenes::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	this->setDevice(pDevice);
	if (NULL == pDevice) {
		this->setPropertyChanged(false);
		this->setUploadDone(false);
		this->deallocProperties( );
	}
	else if (this->getCurrDeviceShortAddress() != pDevice->getShortAddress()) {
		this->setPropertyChanged(false);
		this->setUploadDone(false);
		this->deallocProperties( );
		CAppDaliDeviceProperties_ControlGear* prop = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
		ASSERT(0 != prop);
		if (NULL != prop) {
			this->m_properties = prop->clone();
			ASSERT(NULL != this->m_properties);
		}
		this->setCurrDeviceShortAddress(pDevice->getShortAddress());
	}
	this->initModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearScenes::deallocateDlgDT8Scenes() {
	if (NULL != m_pDialogDT8Scenes) {
		delete m_pDialogDT8Scenes;
		m_pDialogDT8Scenes = NULL;
	}
}

void CDlgChildDaliControlgearScenes::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {
	if (activeDialog) {
		this->setPropertyChanged(false);
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// TODO_DT8_SCENES:
// - m_sceneUsed -> Afhankelijk maken dt8 color object indien aanwezig...
void CDlgChildDaliControlgearScenes::initModel( ) {
	int i;
	for (i=0;i<DUOTECNO::DALI::DALI_SYSTEM_CONTROLGEAR_SCENE_MAX;++i) {
		this->m_sceneLevel[i] = 0xff;
		this->m_sceneUsed[i] = false;
	}

	ASSERT(NULL != this->m_properties);
	if (NULL == this->m_properties) {
		return;
	}
	
	for (i=0;i<DUOTECNO::DALI::DALI_SYSTEM_CONTROLGEAR_SCENE_MAX;++i) {
		const int level = this->m_properties->getSceneLevel(i);				
		this->m_sceneUsed[i] = CAppDaliDeviceProperties_ControlGear::isSceneActive(*this->m_properties,i);		
		this->m_sceneLevel[i] = ((level >=0 && level< 255) ? level : 255);
	}	
	this->m_supportDT8ColorObjects = CAppDaliDeviceProperties_ControlGear::isDT8ColorObjectSupported(*this->m_properties);
	this->m_busyEditingName = FALSE;
	this->m_strSceneName.Empty();
	this->getWorker().getSceneNames(&m_mapNames);
}

void CDlgChildDaliControlgearScenes::updateModel( ) {
	this->getWorker().getSceneNames(&m_mapNames);
}

BOOL CDlgChildDaliControlgearScenes::updateDeviceProperties( ) {
	ASSERT(NULL != this->m_properties);
	if (NULL == this->m_properties) {
		return FALSE;
	}
	for (int i=0;i<DUOTECNO::DALI::DALI_SYSTEM_CONTROLGEAR_SCENE_MAX;++i) {
		int level = 255;
		if (this->m_sceneUsed[i]) {
			level = this->m_sceneLevel[i];
		}
		this->m_properties->setSceneLevel(i,level);
	}
	return TRUE;
}

LONG CDlgChildDaliControlgearScenes::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	
	const BOOL controlsEnabledViewmodeOnline = (this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE); 

	this->setBusyGuiControlsUpdate(true);

	if (!this->getInitDone()) {	
		for (int i=0;i<DUOTECNO::DALI::DALI_SYSTEM_CONTROLGEAR_SCENE_MAX;++i) {
			CString str;
			str.Format("%d",i);
			const int index = this->m_cmbSceneSelect.AddString(str);
			if (index >= 0) {
				this->m_cmbSceneSelect.SetItemData(index,i);	
			}
		}		
		this->m_selectedScene = 0;
		this->m_cmbSceneSelect.SetCurSel(this->m_selectedScene);
		this->updateEditBoxSceneName(this->m_selectedScene);		
		this->EnableToolTips(TRUE);
		this->setInitDone(true);
	}

	// Scene test controls...
	m_cmbSceneSelect.EnableWindow(TRUE);
	m_groupSceneTest.EnableWindow(TRUE);
	m_btnTestScene.EnableWindow(controlsEnabledViewmodeOnline);

	// Btn upload.
	m_btnUpload.EnableWindow((controlsEnabledViewmodeOnline && this->getPropertyChanged()) ? TRUE : FALSE);
	m_btnReadConfig.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnRestoreDefaults.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnUndoChanges.EnableWindow(controlsEnabledViewmodeOnline && this->getPropertyChanged() ? TRUE : FALSE);

	// Tekst in de progress info (m_staticFooterInfoText) control.
	this->updateMsgProgressInfo();

	for (int i=0;i<DUOTECNO::DALI::DALI_SYSTEM_CONTROLGEAR_SCENE_MAX;++i) {
		CWnd* pWnd = NULL;		
		pWnd = this->GetDlgItem(IDC_CHECK_SCENE0 + i);
		if (NULL != pWnd) {
						
			{
				// Namen van de sferen.
				CString str;
				CString strName;
				str.Format("%d",i);			
				if (m_mapNames.Lookup(i,strName)) {
					if (!strName.IsEmpty()) {
						str += ": ";
						str += strName;
					}
				}			
				pWnd->SetWindowText(str);
			}

			CButton* const pBtn = (CButton*)(pWnd);
			if (NULL != pBtn) {		
				pBtn->SetCheck(this->m_sceneUsed[i] == true ? TRUE : FALSE);
				pBtn->EnableWindow(controlsEnabledViewmodeOnline);
			}
		}	

		pWnd = this->GetDlgItem(IDC_EDIT_SCENE0 + i);
		if (NULL != pWnd) {
			CEdit* const pEdit = (CEdit*)(pWnd);
			if (NULL != pEdit) {
				CString strValue("Mask");
				if (this->m_sceneLevel[i]  < 255) {
					strValue.Format("%d",m_sceneLevel[i]);		// Wordt momenteel weergegeven in abs waarden.
				}
				pEdit->SetWindowText(strValue);
				pEdit->EnableWindow((this->m_sceneUsed[i] == true ?  TRUE : FALSE) && (controlsEnabledViewmodeOnline));
			}
		}

		// DT8 Color buttons enkel weergeven wanneer een control gear device DT8 ondersteund.
		pWnd = this->GetDlgItem(IDC_BUTTON_COLOR_SCENE0 + i);
		if (NULL != pWnd) {
			pWnd->ShowWindow(this->m_supportDT8ColorObjects ? 1 : 0);
			pWnd->EnableWindow((this->m_sceneUsed[i] == true ?  TRUE : FALSE));		// && (controlsEnabledViewmodeOnline));
		}		
	}

	this->setBusyGuiControlsUpdate(false);
	return TRUE;
}

void CDlgChildDaliControlgearScenes::OnButtonClickedScene(UINT nID) {
	const int sceneNumber = nID - IDC_CHECK_SCENE0;
	if (sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST || sceneNumber > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST) {
		return;
	}	
	CButton* const pButton = (CButton*)(this->GetDlgItem(nID));
	if (NULL == pButton) {
		return;
	}
	ASSERT(sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_MAX);
	const bool dt8ColorObjectSupported = CAppDaliDeviceProperties_ControlGear::isDT8ColorObjectSupported(*this->m_properties);
	const bool state = (pButton->GetCheck() == BST_CHECKED ? true : false);

	if (!state) {
		if (dt8ColorObjectSupported) {		
			if (CAppDaliDeviceProperties_ControlGear::isSceneActive(*this->m_properties,sceneNumber)) {
				const CString strHeader(_T("Clear scene"));
				const CString strMessage(_T("Clear scene intensity and DT8 colordata?"));
				if (IDNO == this->MessageBox(strMessage,strHeader, MB_ICONQUESTION | MB_YESNO)) {
					this->PostMessage(WM_UPDATE_CONTROLS);
					return;
				}		
			}
			CAppDaliDeviceProperties_ControlGear::clearDT8SceneColorData(this->m_properties,sceneNumber);
		}
		this->m_sceneLevel[sceneNumber] = 255;	
	}
	
	this->m_sceneUsed[sceneNumber] = state;
	if ((!dt8ColorObjectSupported) && (this->m_sceneUsed[sceneNumber]) && (this->m_sceneLevel[sceneNumber] >= 255)) {		
		this->m_sceneLevel[sceneNumber] = 254;		
	}	
	this->setPropertyChanged(true);
	this->PostMessage(WM_UPDATE_CONTROLS);
}	

void CDlgChildDaliControlgearScenes::OnButtonClickedColor(UINT nID) {
	const int sceneNumber = nID - IDC_BUTTON_COLOR_SCENE0;
	if (sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST || sceneNumber > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST) {
		return;
	}	

	const int sceneLevel = this->m_sceneLevel[sceneNumber];		// Doorgeven aan de modale dialoog.
	ASSERT(NULL != this->m_properties);							// Doorgeven aan de modale dialoog.

	// Modaal dialoog openenen met de properties van het DT8 kleur object.
	this->deallocateDlgDT8Scenes();
	m_pDialogDT8Scenes = new CDlgDaliControlGearDT8Scene(this->getWorker());
	ASSERT(NULL != m_pDialogDT8Scenes);

	CString strSceneName;
	m_mapNames.Lookup(sceneNumber, strSceneName);
	if (this->m_pDialogDT8Scenes->showDialog(this->getCurrDeviceShortAddress(), sceneNumber, this->m_properties, sceneLevel, strSceneName)) {

		// Updaten van de properties.
		ASSERT(sceneNumber == this->m_pDialogDT8Scenes->getCurrentScene());
		this->m_sceneLevel[sceneNumber] = this->m_pDialogDT8Scenes->getCurrentLevel();
	
		// NOTE
		// Properties/ Color data wordt upgedate wanneer:
		// - De data doorgestuurd wordt voor een single sfeer.
		// - Wanneer er op de 'Ok'-knop gedrukt wordt voor het afsluiten.
		// Moeten hier niks anders meer doen...

		if (this->m_pDialogDT8Scenes->getPropertyChanged()) {
			this->setPropertyChanged(true);
		}
	
		// Controle of er in de dialoog de naam van de sfeer aangepast werd.
		const CString& strNewSceneName = this->m_pDialogDT8Scenes->getSceneName();
		if (0 != strNewSceneName.Compare(strSceneName)) {
			// Database aanpasseen.
			this->getWorker().setSceneName(sceneNumber,strNewSceneName);
			// Namen updaten in het model.
			this->updateModel();
			// Updaten van de scene naam indien deze momenteel getoond wordt in de combo box.
			if ((sceneNumber == this->m_selectedScene) && (0 != strNewSceneName.Compare(this->m_strSceneName))) {
				this->SetTimer(TIMER_ID_EDITBOX_NAME_UPDATE,100,(TIMERPROC)NULL);
			}
			// this->setPropertyChanged(true);
		}
		//	this->setPropertyChanged(true);
		//}
	}

	this->deallocateDlgDT8Scenes();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearScenes::OnEnSetfocusEditBox(UINT nID) {
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (NULL == pEdit) {
		return;
	}   
    pEdit->GetWindowText(this->m_strPreviousValue);
}

void CDlgChildDaliControlgearScenes::OnEnKillfocusEditBox(UINT nID) {
	const int sceneNumber = nID - IDC_EDIT_SCENE0;
	if (sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST || sceneNumber > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST) {
		return;
	}	
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (NULL == pEdit) {
		return;
	}
    CString strValue;
    pEdit->GetWindowText(strValue);
	if (0 == strValue.Compare(this->m_strPreviousValue)) {
		return;
	}

    // Converteer de waarde naar een integer.
    const int value = _ttoi(strValue);
    // Controleer of de waarde binnen het bereik ligt.
    if (value < 0 || value > 255) {
        AfxMessageBox(_T("Invalid value. The scene level should be between 0 and 255."));        
        // Optioneel: zet de focus terug naar de Edit control met de verkeerde waarde.
        pEdit->SetFocus();
		return;
    }

	ASSERT(sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_MAX);
	this->m_sceneLevel[sceneNumber] = value;
#if(0)
	if (value == 255) {	
		this->m_sceneUsed[sceneNumber] = false;	
	}
#endif
	this->setPropertyChanged(true);
	this->PostMessage(WM_UPDATE_CONTROLS);
}


// Oppassen:
// Gaan hier een deel van de GUI gaan updaten...
void CDlgChildDaliControlgearScenes::OnEnChangeEditControls(UINT nID) {
	if (this->getBusyGuiControlsUpdate()) {
		return;
	}
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (NULL == pEdit) {
		return;
	}
	CString strValue;
    pEdit->GetWindowText(strValue);
	if (0 == strValue.Compare(this->m_strPreviousValue)) {
		return;
	}

	// Updaten van de GUI.
	// Tekst in de progress info (m_staticProgressInfo) control.
	// Upload button
	if (!this->getPropertyChanged()) {
		this->setBusyGuiControlsUpdate(true);	
		this->updateMsgProgressInfo();		
		if (VIEWMODE_ONLINE == this->getViewMode()) {
			m_btnUpload.EnableWindow(TRUE);			
		}
		this->setBusyGuiControlsUpdate(false);
		this->setPropertyChanged(true);
	}
}

// Nodig voor de 'Want Return' van de edit box voor het editeren van de naam.
BOOL CDlgChildDaliControlgearScenes::PreTranslateMessage(MSG* pMsg) {	
	if (NULL != pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {	
		CWnd* pWnd = GetFocus();
		if (pWnd) {
			const DWORD identifier = pWnd->GetDlgCtrlID();			
			if (identifier >= IDC_EDIT_SCENE0 && identifier <= IDC_EDIT_SCENE15) {
				const int sceneNumber = identifier - IDC_EDIT_SCENE0;			
				ASSERT(sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_MAX);
				CWnd* const pWnd = this->GetDlgItem(IDC_CHECK_SCENE0 + sceneNumber);
				if (NULL != pWnd) {
					pWnd->SetFocus();
				}
				return TRUE;
			}
			else if (IDC_EDIT_SCENE_NAME == identifier) {
				this->m_editSceneName.SetFocus();
				return TRUE;
			}
		}
	}
    return CDialog::PreTranslateMessage(pMsg);    
}

void CDlgChildDaliControlgearScenes::deallocProperties() {
	if (NULL != this->m_properties) {
		delete m_properties;
		m_properties = NULL;
	}	
	this->setCurrDeviceShortAddress(-1);
}

BOOL CDlgChildDaliControlgearScenes::onTabDialogCanLeaveDialog( ) {
	if (this->getUploadBusy()) {
		promptUserUploadBusy(*this);
		return FALSE;
	}
	if (VIEWMODE_ONLINE != this->getViewMode()) {
		return TRUE;
	}	
	if (!this->getPropertyChanged()) {
		return TRUE;
	}
	if (FALSE == promptUserUploadChangesBeforeLeavingDialog(*this)) {
		if (this->undoChanges()) {
			this->setPropertyChanged(false);
		}
		return TRUE;
	}
	this->uploadData( );
	return FALSE;
}

void CDlgChildDaliControlgearScenes::uploadData( ) {
	if (this->getCurrDeviceShortAddress() < 0 || NULL == this->m_properties) {
		ASSERT(this->getCurrDeviceShortAddress()>=0);
		ASSERT(0 != this->m_properties);
		return;
	}
	if (!this->updateDeviceProperties()) {
		ASSERT(0);
		return;
	}
	INVARIANT(NULL != this->m_properties);
	const bool result = this->getWorker().startUploadControlGearDeviceScenes(this->getCurrDeviceShortAddress(), *this->m_properties);	
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildDaliControlgearScenes::restoreToDefaults( ) {
	if (NULL == this->m_properties) {
		ASSERT(NULL != this->m_properties);
		return FALSE;
	}
	CAppDaliDevice_Utils::setScenesDefaults(this->m_properties);
	return TRUE;
}

BOOL CDlgChildDaliControlgearScenes::undoChanges( ) {
	CAppDaliDevice* const pDevice = this->getDevice();
	if ((NULL == pDevice) || (NULL == this->m_properties)) {
		ASSERT(NULL != pDevice);
		ASSERT(NULL != this->m_properties);
		return FALSE;
	}
	CAppDaliDeviceProperties_ControlGear* const pPropertiesOriginal = 
		dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
	if (NULL == pPropertiesOriginal) {
		return FALSE;
	}
	CAppDaliDeviceProperties_ControlGear::copy(this->m_properties,*pPropertiesOriginal,CAppDaliDeviceProperties_ControlGear::FLAG_COPY_SCENES);
	return TRUE;
}

void CDlgChildDaliControlgearScenes::OnBnClickedButtonUpload() {
	this->uploadData( );
}

void CDlgChildDaliControlgearScenes::OnBnClickedButtonReadConfig() {
	const int shortAddress = this->getCurrDeviceShortAddress();
	if (shortAddress < 0) {
		ASSERT(shortAddress>=0);	
		return;
	}
	const bool result = this->getWorker().startReadControlGearDeviceScenes(shortAddress);
	if (result) {

	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearScenes::updateMsgProgressInfo( ) {
	const CString strMessage = this->getStrMsgInfoFooter(*this);
	m_staticFooterInfoText.SetWindowText(strMessage);
}


// IAppDlgDaliGui_Host	
void CDlgChildDaliControlgearScenes::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_SCENES == task) {
		this->setUploadBusy(true);	
		this->setUploadDone(false);
	}
	else if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_SCENES == task) {
		this->setDownloadBusy(true);
		this->setDownloadDone(false);
	}

	if (NULL != m_pDialogDT8Scenes) {
		m_pDialogDT8Scenes->onNotifyGuiTaskStarted(task);
	}

	this->setViewMode(VIEWMODE_ONLINE_BUSY);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearScenes::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_SCENES == task) {	
		this->setUploadDone(true);
		this->setUploadBusy(false);
		this->setPropertyChanged(false);
	}
	else if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_SCENES == task) {
		helperUpdateProperties(this->getDevice());
		this->initModel();	// Nodig om de properties met de gui controsl te syncen.
		this->setDownloadBusy(false);
		this->setDownloadDone(true);
		this->setPropertyChanged(false);
	}

	if (NULL != m_pDialogDT8Scenes) {
		m_pDialogDT8Scenes->onNotifyGuiTaskDone(task);
	}
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliControlgearScenes::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	this->setViewMode(VIEWMODE_ONLINE);
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_SCENES == task) {
		// this->m_strErrorMessage.Format("Error happened!\r\n%s");
		this->setUploadBusy(false);		
	}
	else if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_SCENES == task) {
		this->setDownloadBusy(false);	
	}

	if (NULL != m_pDialogDT8Scenes) {
		m_pDialogDT8Scenes->onNotifyGuiTaskError(task,strMessage);
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearScenes::OnBnClickedButtonDaliSceneExecute() {
	const int currSelected = this->m_cmbSceneSelect.GetCurSel();
	if (currSelected < 0) {
		ASSERT(0);
		return;
	}
	const int sceneNumber = this->m_cmbSceneSelect.GetItemData(currSelected);
	if (sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST || sceneNumber > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST) {
		ASSERT(0);
		return;
	}	
	if (this->getPropertyChanged()) {
		const CString strMessage("The latest changes have not been uploaded to the device.\r\n"
								 "Executing the test will not show the latest changes.\r\n"
								 "Continue and execute test?");
		if (IDOK != this->MessageBox(strMessage,"Execute broadcast scene",MB_OKCANCEL | MB_ICONQUESTION)) {
			return;
		}								
	}
	this->getWorker().sendGotoScene(CAppDlgDaliWorker::CONTROLGEAR_MSG_BROADCAST,0xff,sceneNumber);
}

void CDlgChildDaliControlgearScenes::OnBnClickedButtonRestore() {
	if (FALSE == promptUserRestoreDefaults(*this)) {
		return;
	}

	this->restoreToDefaults();
	this->initModel();		// Oppassen: Nodig om de properties opnieuw naar de lokale var te copieren.
							
	this->setPropertyChanged(true);
	this->setUploadDone(false);

	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearScenes::OnBnClickedButtonUndo() {
	if (FALSE == promptUserUndoChanges(*this)) {
		return;
	}

	this->undoChanges();
	this->initModel();		// Oppassen: Nodig om de properties opnieuw naar de lokale var te copieren.

	this->setPropertyChanged(true);
	this->setUploadDone(false);

	this->PostMessage(WM_UPDATE_CONTROLS);
}

// Todo: Onderstaande uitwerken met itemData.
void CDlgChildDaliControlgearScenes::OnCbnSelchangeSceneNumber() {
	const int currSelection = m_cmbSceneSelect.GetCurSel();
	if (currSelection >= 0) {		
		this->m_selectedScene = currSelection;
		this->setBusyGuiControlsUpdate(true);
		this->updateEditBoxSceneName(currSelection);
		this->setBusyGuiControlsUpdate(false);
	}
}

void CDlgChildDaliControlgearScenes::updateEditBoxSceneName(int sceneNumber) {
	CString str;
	ASSERT(this->getBusyGuiControlsUpdate());
	m_mapNames.Lookup(sceneNumber,str);
	this->m_strSceneName = str;	
	this->m_editSceneName.SetWindowText(m_strSceneName);	
}

void CDlgChildDaliControlgearScenes::OnEnChangeEditSceneName() {
	if (this->getBusyGuiControlsUpdate()) {
		return;
	}
	this->m_busyEditingName = TRUE;
	this->KillTimer(TIMER_ID_EDITBOX_NAME);
	this->SetTimer(TIMER_ID_EDITBOX_NAME, 2000, (TIMERPROC) NULL);
}

void CDlgChildDaliControlgearScenes::OnEnKillfocusEditSceneName() {
	if (this->m_busyEditingName) {
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
		this->GetEditBoxName();		
	}
}

void CDlgChildDaliControlgearScenes::OnTimer(UINT nIDEvent) {
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

void CDlgChildDaliControlgearScenes::GetEditBoxName() {
	enum { MAX_NAME_LENGTH = 15 };
	if (!this->m_busyEditingName) {
		return;
	}
	this->m_busyEditingName = FALSE;
					
	CString strNewName;		
	this->m_editSceneName.GetWindowText(strNewName);
	if (0 == strNewName.Compare(this->m_strSceneName)) {
		return;
	}	
	this->m_strSceneName = strNewName.Left(MAX_NAME_LENGTH);	
	this->getWorker().setSceneName(this->m_selectedScene,this->m_strSceneName);
	this->updateModel();
	if (0 != strNewName.Compare(this->m_strSceneName)) {
		this->SetTimer(TIMER_ID_EDITBOX_NAME_UPDATE,100,(TIMERPROC)NULL);
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearScenes::UpdateEditBoxGroupName() {
	ASSERT(FALSE == this->m_busyEditingName);
	this->setBusyGuiControlsUpdate(TRUE);
	this->updateEditBoxSceneName(this->m_selectedScene);	
	this->setBusyGuiControlsUpdate(FALSE);
}

BOOL CDlgChildDaliControlgearScenes::helperUpdateProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	if (NULL == pDevice) {
		REQUIRE(NULL != pDevice);
		return FALSE;
	}
	CAppDaliDeviceProperties_ControlGear* prop = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
	if (NULL == prop) {
		ASSERT(NULL != prop);
		return FALSE;
	}	
	if (NULL == this->m_properties) {
		ASSERT(NULL != this->m_properties);	// Kan normaal niet voorkomen.
		this->m_properties = prop->clone();
	}
	else{
		CAppDaliDeviceProperties_ControlGear::copy(this->m_properties,*prop,CAppDaliDeviceProperties_ControlGear::FLAG_COPY_SCENES);
	}
	ENSURE(NULL != this->m_properties);
	return TRUE;
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliControlgearScenes::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR toolbarItemIdentifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		toolbarItemIdentifier = ::GetDlgCtrlID((HWND)toolbarItemIdentifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	if ((toolbarItemIdentifier >= IDC_EDIT_SCENE0) && (toolbarItemIdentifier <= IDC_EDIT_SCENE15)) {
		this->m_strToolTipText = _T("Set intensity value or mask (range: 0–254; 255 = mask/ignore)");
		handled = TRUE;
	}
	else if ((toolbarItemIdentifier >= IDC_BUTTON_COLOR_SCENE0) && (toolbarItemIdentifier <= IDC_BUTTON_COLOR_SCENE15)) {
		this->m_strToolTipText = _T("Configure DT8 scene color objects");
		handled = TRUE;
	}
	else if ((toolbarItemIdentifier >= IDC_CHECK_SCENE0) && (toolbarItemIdentifier <= IDC_CHECK_SCENE15)) {
		this->m_strToolTipText = _T("Enable or disable this DALI scene");
		handled = TRUE;
	}
	else if (IDC_BUTTON_UPLOAD == toolbarItemIdentifier) {
		this->m_strToolTipText = _T("Upload all scenes to the device");
		handled = TRUE;
	}
	else if (IDC_BUTTON_RESTORE == toolbarItemIdentifier) {
		this->m_strToolTipText = _T("Clear all scene data from the device");
		handled = TRUE;
	}
	else if (IDC_BUTTON_READ == toolbarItemIdentifier) {
		this->m_strToolTipText = _T("Read all scenes from the device");
		handled = TRUE;
	} 
	else if (IDC_BUTTON_UNDO == toolbarItemIdentifier) {
		this->m_strToolTipText = _T("Undo the last changes");
		handled = TRUE;
	}
	else if (IDC_BUTTON_DALI_SCENE_EXECUTE == toolbarItemIdentifier) {
		this->m_strToolTipText = _T("Test the selected scene (broadcast to all devices)");
		handled = TRUE;
	}
	else if (IDC_COMBO_DALI_SCENES == toolbarItemIdentifier) {
		this->m_strToolTipText = _T("Select a scene (range: 0–15)");
		handled = TRUE;
	}
	else if (IDC_EDIT_SCENE_NAME == toolbarItemIdentifier) {
		this->m_strToolTipText = _T("Enter scene name (max. 15 characters)");
		handled = TRUE;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}