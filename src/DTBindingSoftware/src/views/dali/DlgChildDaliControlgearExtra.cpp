// DlgChildDaliControlgearExtra.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliControlgearExtra.h"
#include "appdlgdaliworker.h"
#include "app/appdali/appdaliprotocol_def.h"

/****************************************************************************************************/
using namespace DUOTECNO::DALI;
/****************************************************************************************************/

// CDlgChildDaliControlgearExtra dialog

IMPLEMENT_DYNAMIC(CDlgChildDaliControlgearExtra, CDialog)

CDlgChildDaliControlgearExtra::CDlgChildDaliControlgearExtra(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker,CDlgChildDaliControlgearExtra::IDD, pParent),m_properties(NULL),m_selectDimmerCurve(0) {
}

CDlgChildDaliControlgearExtra::~CDlgChildDaliControlgearExtra() {
	this->deallocProperties( );
}

void CDlgChildDaliControlgearExtra::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON_READ, m_btnReadConfig);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
	DDX_Control(pDX, IDC_BUTTON_UNDO, m_btnUndoChanges);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_btnRestoreDefaults);
	DDX_Control(pDX, IDC_STATIC_TEXT_HEADER, m_staticHeaderInfoText);
	DDX_Control(pDX, IDC_GROUP_DIMMING_CURVE, m_staticGroupSelectDimmingCurve);
	DDX_Control(pDX, IDC_RADIO_DIMMERCURVE_DALI, m_btnRadioDimmingCurveDali);
	DDX_Control(pDX, IDC_RADIO_DIMMERCURVE_LINEAR, m_btnRadioDimmingCurveLinear);
	DDX_Radio(pDX, IDC_RADIO_DIMMERCURVE_DALI, m_selectDimmerCurve);
}

BEGIN_MESSAGE_MAP(CDlgChildDaliControlgearExtra, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CDlgChildDaliControlgearExtra::OnUpdateControls)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CDlgChildDaliControlgearExtra::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CDlgChildDaliControlgearExtra::OnBnClickedButtonReadConfig)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CDlgChildDaliControlgearExtra::OnBnClickedButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, &CDlgChildDaliControlgearExtra::OnBnClickedButtonUndo)
	ON_BN_CLICKED(IDC_RADIO_DIMMERCURVE_DALI, &CDlgChildDaliControlgearExtra::OnBnClickedRadioDimmerCurveDali)
	ON_BN_CLICKED(IDC_RADIO_DIMMERCURVE_LINEAR, &CDlgChildDaliControlgearExtra::OnBnClickedRadioDimmerCurveLinear)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliControlgearExtra::OnToolTipNeedText)
END_MESSAGE_MAP()


void CDlgChildDaliControlgearExtra::deallocProperties() {
	if (NULL != this->m_properties) {
		delete m_properties;
		m_properties = NULL;
	}
	this->setCurrDeviceShortAddress(-1);
}

void CDlgChildDaliControlgearExtra::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {	
	this->setDevice(pDevice);
	this->setPropertyChanged(false);
	this->setUploadDone(false);
	if (NULL == pDevice) {		
		this->deallocProperties( );	
	}
	else if (this->getCurrDeviceShortAddress() != pDevice->getShortAddress()) {
		this->deallocProperties( );	
		// this->m_supportExtendedProperties = false;		// Momenteel altijd op false. (zie NOTE_DALI2_FRAMERATE)
		CAppDaliDeviceProperties_ControlGear* prop = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
		ASSERT(0 != prop);
		if (NULL != prop) {
			this->m_properties = prop->clone();
			ASSERT(NULL != this->m_properties);
		}
		this->setCurrDeviceShortAddress(pDevice->getShortAddress());
	}
	else {
		// Sedert V16.45: wanneer de groep properties updaten.
		ASSERT(this->getCurrDeviceShortAddress() == pDevice->getShortAddress());
		if (FALSE == this->helperUpdateProperties(pDevice)) {
			ASSERT(0);
		}	
	}
	this->updateViewMode();
	this->initGUIControls( );
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildDaliControlgearExtra::helperUpdateProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
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
		CAppDaliDeviceProperties_ControlGear::copy(this->m_properties,*prop);
	}
	ENSURE(NULL != this->m_properties);
	return TRUE;
}

void CDlgChildDaliControlgearExtra::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) { 
	if (activeDialog) {
		// this->setPropertyChanged(false);	
	}
	this->init(pDevice);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildDaliControlgearExtra::OnParentPropertyPageLeave() {
//	BOOL result = this->getRequestParentUpdateGUI();
// this->setRequestParentUpdateGUI(false);
	return FALSE;
}

BOOL CDlgChildDaliControlgearExtra::onTabDialogCanLeaveDialog( ) {	
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

void CDlgChildDaliControlgearExtra::initGUIControls( ) {
	CAppDaliDeviceProperties_ControlGear* properties = this->m_properties;
	if (NULL == properties) {
		ASSERT(NULL != properties);
		return;
	}

	BOOL supportDimmingCurve = FALSE;
	const int dimmerCurve = this->m_properties->getDimmingCurve();
	switch(dimmerCurve) {
		case DALI_CONTROLGEAR_DIMCURVE_UNSUPPORTED: {
			supportDimmingCurve = FALSE;			
			this->m_selectDimmerCurve = 0;			// Updaten van de radiobutton selection.
		} break;
		case DALI_CONTROLGEAR_DIMCURVE_DALI: {
			supportDimmingCurve = TRUE;									
			this->m_selectDimmerCurve = 0;			// Updaten van de radiobutton selection.
		} break;
		case DALI_CONTROLGEAR_DIMCURVE_LINEAIR: {
			supportDimmingCurve = TRUE;		
			this->m_selectDimmerCurve = 1;			// Updaten van de radiobutton selection.
												
		} break;
	}

	// Updaten van de header info text.
	CString strText;
	if (supportDimmingCurve) {
		strText.Format("After selecting the dimming curve press the upload button and\r\n"
					   "when done do not forget to sync the DALI bridge configuration");
	}
	else {
		strText = CString("This device does not support extra configuration features");
	}
	m_staticHeaderInfoText.SetWindowText(strText);

	this->UpdateData(FALSE);	// Updaten van de Radiobutton.
}

void CDlgChildDaliControlgearExtra::updateMsgProgressInfo( ) {
	const CString strMessage = this->getStrMsgInfoFooter(*this);
	m_staticFooterInfoText.SetWindowText(strMessage);
}

LRESULT CDlgChildDaliControlgearExtra::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	CAppDaliDeviceProperties_ControlGear* properties = this->m_properties;
	if (NULL == properties) {
		ASSERT(NULL != properties);
		return TRUE;
	}

	if (!this->getInitDone()) {
		this->EnableToolTips(TRUE);
		this->setInitDone(TRUE);
	}

	const BOOL controlsEnabledViewmodeOnline = (this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE); 
	const BOOL supportDimmingCurve = properties->getDimmingCurve( ) == DALI_CONTROLGEAR_DIMCURVE_UNSUPPORTED ? FALSE : TRUE;

	this->setBusyGuiControlsUpdate(true);	
	{
		m_btnUpload.EnableWindow((controlsEnabledViewmodeOnline) && (supportDimmingCurve) && this->getPropertyChanged() ? TRUE : FALSE);
	    m_btnReadConfig.EnableWindow(controlsEnabledViewmodeOnline && supportDimmingCurve);
		m_btnRestoreDefaults.EnableWindow(controlsEnabledViewmodeOnline && supportDimmingCurve); 
		m_btnUndoChanges.EnableWindow((controlsEnabledViewmodeOnline) && (supportDimmingCurve) && this->getPropertyChanged() ? TRUE : FALSE);
	}
	
	m_staticGroupSelectDimmingCurve.EnableWindow((controlsEnabledViewmodeOnline) && (supportDimmingCurve));
	m_btnRadioDimmingCurveDali.EnableWindow((controlsEnabledViewmodeOnline) && (supportDimmingCurve));
	m_btnRadioDimmingCurveLinear.EnableWindow((controlsEnabledViewmodeOnline) && (supportDimmingCurve));

	this->updateMsgProgressInfo();
	this->setBusyGuiControlsUpdate(false);
	return TRUE;
}


void CDlgChildDaliControlgearExtra::uploadData( ) {
	if ((this->getCurrDeviceShortAddress() < 0) || (NULL == this->m_properties)) {
		ASSERT(this->getCurrDeviceShortAddress()>=0);
		ASSERT(0 != this->m_properties);
		return;
	}

	// Sedert V16.48: Een dali device kan meerdere device types ondersteunen.
	// Wanneer een device Type 0xFF moeten we in de devices lijst kijken.
	// Kunnen de lijst overlopen via isSupported()
	int deviceType = this->m_properties->getDeviceType();
	if (deviceType < 0 || deviceType >= 0xff) {
		if (CAppDaliDeviceProperties_ControlGear::isSupported(*this->m_properties, 6)) {
			deviceType = 6;
		}
		else {	
			ASSERT(0);		// Andere device types... TODO
		}
	}
	const int dimmerCurve = this->m_properties->getDimmingCurve( );
	const bool result = this->getWorker().startUploadControlGearDimmerCurve(this->getCurrDeviceShortAddress(), deviceType, dimmerCurve);
	if (result) {

	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildDaliControlgearExtra::restoreToDefaults( ) {
	if (NULL == this->m_properties) {
		ASSERT(NULL != this->m_properties);
		return FALSE;
	}	
	this->m_properties->setDimmingCurve(DALI_CONTROLGEAR_DIMCURVE_DALI);
	this->initGUIControls();
	return TRUE;
}

BOOL CDlgChildDaliControlgearExtra::undoChanges( ) {
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
	CAppDaliDeviceProperties_ControlGear::copy(this->m_properties,*pPropertiesOriginal,CAppDaliDeviceProperties_ControlGear::FLAG_COPY_PROPERTIES);
	this->initGUIControls();
	return TRUE;
}

void CDlgChildDaliControlgearExtra::OnBnClickedButtonUpload() {
	this->uploadData( );
}


// IAppDlgDaliGui_Host	
void CDlgChildDaliControlgearExtra::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_DIMMERCURVE == task) {		
		this->setUploadBusy(true);
		this->setUploadDone(false);
	}
	else if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_PROPERTIES == task) {
		this->setDownloadBusy(true);
		this->setDownloadDone(false);
	}
	this->setViewMode(VIEWMODE_ONLINE_BUSY);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliControlgearExtra::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_DIMMERCURVE == task) {	
		this->setRequestParentUpdateGUI(true);	// Vragen aan de parent om de andere dialogen up te daten. (nodig voor de groepen.)
		this->setUploadDone(true);
		this->setUploadBusy(false);
		this->setPropertyChanged(false);
	}
	else if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_PROPERTIES == task) {
		CAppDaliDevice* const pDevice = this->getDevice();
		ASSERT(NULL != pDevice);
		if (FALSE == this->helperUpdateProperties(pDevice)) {
			ASSERT(0);
		}
		this->setRequestParentUpdateGUI(true);	// Vragen aan de parent om de andere dialogen up te daten. (nodig voor de groepen.)
		this->setDownloadBusy(false);
		this->setDownloadDone(true);
		this->setPropertyChanged(false);
		this->initGUIControls();
	}
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliControlgearExtra::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	this->setViewMode(VIEWMODE_ONLINE);
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_DIMMERCURVE == task) {
		// this->m_strErrorMessage.Format("Error happened!\r\n%s");
		this->setUploadBusy(false);	
	}
	else if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_PROPERTIES == task) {
		this->setDownloadBusy(false);	
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearExtra::OnBnClickedButtonRestore() {
	if (FALSE == promptUserRestoreDefaults(*this)) {
		return;
	}
	this->restoreToDefaults();							
	this->setPropertyChanged(true);
	this->setUploadDone(false);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearExtra::OnBnClickedButtonUndo() {
	if (FALSE == promptUserUndoChanges(*this)) {
		return;
	}
	this->undoChanges();
	this->setPropertyChanged(true);
	this->setUploadDone(false);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearExtra::updateModel( ) {
	if (NULL == this->m_properties) {
		ASSERT(NULL != this->m_properties);
		return;
	}
	const int prev = this->m_properties->getDimmingCurve();
	if (prev != this->m_selectDimmerCurve) {
		ASSERT(this->m_selectDimmerCurve == DALI_CONTROLGEAR_DIMCURVE_DALI ||
			   this->m_selectDimmerCurve == DALI_CONTROLGEAR_DIMCURVE_LINEAIR);
		this->m_properties->setDimmingCurve(this->m_selectDimmerCurve);
		this->setPropertyChanged(true);
	}
}

void CDlgChildDaliControlgearExtra::OnBnClickedRadioDimmerCurveDali() {
	this->UpdateData(TRUE);		// Update m_selectDimmerCurve
	this->updateModel( );
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearExtra::OnBnClickedRadioDimmerCurveLinear() {
	this->UpdateData(TRUE);		// Update m_selectDimmerCurve
	this->updateModel( );
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearExtra::OnBnClickedButtonReadConfig() {
	const int shortAddress = this->getCurrDeviceShortAddress();
	if ((shortAddress < 0) || (NULL == this->m_properties)) {
		ASSERT(shortAddress>=0);
		ASSERT(0 != this->m_properties);
		return;
	}
	const bool result = this->getWorker().startReadControlGearDeviceProperties(shortAddress);
	if (result) {

	}
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
BOOL CDlgChildDaliControlgearExtra::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_BUTTON_UPLOAD: {
			this->m_strToolTipText = _T("Upload properties to the DALI device");
			handled = TRUE;
		} break;
		case IDC_BUTTON_RESTORE: {
			this->m_strToolTipText = _T("Restore default values");
			handled = TRUE;
		} break;
		case IDC_BUTTON_UNDO: {
			this->m_strToolTipText = _T("Undo the last changes");
			handled = TRUE;
		} break;
		case IDC_BUTTON_READ: {
			this->m_strToolTipText = _T("Read properties from the DALI device");
			handled = TRUE;
		} break;
		case IDC_RADIO_DIMMERCURVE_DALI: {
			this->m_strToolTipText = _T("Select DALI dimming curve (DT6 control gear)");
			handled = TRUE;
		} break;
		case IDC_RADIO_DIMMERCURVE_LINEAR: {
			this->m_strToolTipText = _T("Select linear dimming curve (DT6 control gear)");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
