// DlgChildDaliControlgearProperties.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliControlgearProperties.h"

#include "appdlgdaliworker.h"
#include "app/appdali/appdaliprotocol_def.h"

/****************************************************************************************************/
// NOTE_DALI2_FRAMERATE
// DALI-2 ondersteund een extended frame rate.
// Momenteel wordt deze configuratie niet ondersteund door de configuratie software.
// - De var 'm_supportExtendedProperties' blijft op false.
// - De minimum waarde van de slider voor de fade rate staat op 1 (ipv 0).
// 
// Bij DALI cockpit wordt de configuratie ook niet ondersteund (wel in Dali-master configurator.)

/****************************************************************************************************/
using namespace DUOTECNO::DALI;
/****************************************************************************************************/
// CDlgChildDaliControlgearProperties dialog
IMPLEMENT_DYNAMIC(CDlgChildDaliControlgearProperties, CDialog)

CDlgChildDaliControlgearProperties::CDlgChildDaliControlgearProperties(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker,CDlgChildDaliControlgearProperties::IDD, pParent),m_properties(NULL), m_supportExtendedProperties(false)
{

}

CDlgChildDaliControlgearProperties::~CDlgChildDaliControlgearProperties() {
	this->deallocProperties( );
}

void CDlgChildDaliControlgearProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_FADE_TIME, m_sliderFadeTime);
	DDX_Control(pDX, IDC_SLIDER_FADE_RATE, m_sliderFadeRate);
	DDX_Control(pDX, IDC_SLIDER_MINIMUM, m_sliderMinimum);
	DDX_Control(pDX, IDC_SLIDER_MAXIMUM, m_sliderMaximum);
	DDX_Control(pDX, IDC_SLIDER_POWERON, m_sliderPowerOn);
	DDX_Control(pDX, IDC_SLIDER_SYSTEMFAILURE, m_sliderSystemFailure);
	DDX_Control(pDX, IDC_EDIT_FADE_TIME, m_editFadeTime);
	DDX_Control(pDX, IDC_EDIT_FADE_RATE, m_editFadeRate);
	DDX_Control(pDX, IDC_EDIT_MINIMUM, m_editMinimum);
	DDX_Control(pDX, IDC_EDIT_MAXIMUM, m_editMaximum);
	DDX_Control(pDX, IDC_EDIT_POWERON, m_editPowerOn);
	DDX_Control(pDX, IDC_EDIT_SYSTEMFAILURE, m_editSystemFailure);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON_READ, m_btnReadConfig);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
	DDX_Control(pDX, IDC_BUTTON_UNDO, m_btnUndoChanges);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_btnRestoreDefaults);
}

BEGIN_MESSAGE_MAP(CDlgChildDaliControlgearProperties, CDialog)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_FADE_TIME, &CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderFadeTime)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_FADE_RATE, &CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderFadeRate)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_MINIMUM, &CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderMinimum)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_MAXIMUM, &CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderMaximum)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_POWERON, &CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderPowerOn)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SYSTEMFAILURE, &CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderSystemFailure)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CDlgChildDaliControlgearProperties::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CDlgChildDaliControlgearProperties::OnBnClickedButtonReadConfig)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CDlgChildDaliControlgearProperties::OnBnClickedButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, &CDlgChildDaliControlgearProperties::OnBnClickedButtonUndo)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliControlgearProperties::OnToolTipNeedText)
END_MESSAGE_MAP()


void CDlgChildDaliControlgearProperties::deallocProperties() {
	if (NULL != this->m_properties) {
		delete m_properties;
		m_properties = NULL;
	}
	this->setCurrDeviceShortAddress(-1);
}


// CDlgChildDaliControlgearProperties message handlers
void CDlgChildDaliControlgearProperties::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {	
	this->setDevice(pDevice);
	this->setPropertyChanged(false);
	this->setUploadDone(false);
	if (NULL == pDevice) {		
		this->deallocProperties( );	
	}
	else if (this->getCurrDeviceShortAddress() != pDevice->getShortAddress()) {
		this->deallocProperties( );	
		this->m_supportExtendedProperties = false;		// Momenteel altijd op false. (zie NOTE_DALI2_FRAMERATE)
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

BOOL CDlgChildDaliControlgearProperties::helperUpdateProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
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


void CDlgChildDaliControlgearProperties::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) { 
	if (activeDialog) {
		// this->setPropertyChanged(false);	
	}
	this->init(pDevice);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// Indien er properties werden geupload wordt gevraagd aan de parent om alle andere dialogen op de hoogte te brengen.
// Dit is nodig wanneer er properties van leden van een groep aangepast werden.
// de GUI van de groep properties toont de eigenschappen van het eerste device in de groep.
BOOL CDlgChildDaliControlgearProperties::OnParentPropertyPageLeave() {
	BOOL result = this->getRequestParentUpdateGUI();
	this->setRequestParentUpdateGUI(false);
	return result;	
}

BOOL CDlgChildDaliControlgearProperties::onTabDialogCanLeaveDialog( ) {	
	if (this->getUploadBusy()) {
		promptUserUploadBusy(*this);
		return FALSE;
	}
	if (this->getDownloadBusy()) {
		promptUserDownloadBusy(*this);
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

// zie NOTE_DALI2_FRAMERATE
// Indien de fade rate op 0 staat wordt de extended fade rate gebruikt...
// Momenteel niet ondersteund. De slider wordt altijd op minimum waarde 1 geplaatst.
void CDlgChildDaliControlgearProperties::initGUIControls( ) {

	// DALI-2 Support.
	// Wanneer ondersteund min rate 0, wanneer niet ondersteund komt op 1
	const int minimFadeRetate = (this->m_supportExtendedProperties ? DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MIN : DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MIN+1);

	m_sliderFadeRate.SetRange(minimFadeRetate,DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MAX);
	m_sliderFadeRate.SetTicFreq(4);

	m_sliderFadeTime.SetRange(DALI_SYSTEM_CONTROLGEAR_DIM_FADETIME_MIN,DALI_SYSTEM_CONTROLGEAR_DIM_FADETIME_MAX);
	m_sliderFadeTime.SetTicFreq(4);

	m_sliderMinimum.SetRange(DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
	m_sliderMinimum.SetTicFreq(32);

	m_sliderMaximum.SetRange(DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
	m_sliderMaximum.SetTicFreq(32);

	m_sliderPowerOn.SetRange(DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
	m_sliderPowerOn.SetTicFreq(32);

	m_sliderSystemFailure.SetRange(DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
	m_sliderSystemFailure.SetTicFreq(32);

	this->EnableToolTips(TRUE);
}


static int NormalizeValue(int value,int minValue,int maxValue) {
	if (value < minValue) {
		value = minValue;
	}
	else if (value > maxValue) {
		value = maxValue;
	}
	return value;
}

void CDlgChildDaliControlgearProperties::updateMsgProgressInfo( ) {
	const CString strMessage = this->getStrMsgInfoFooter(*this);
	m_staticFooterInfoText.SetWindowText(strMessage);
}

LRESULT CDlgChildDaliControlgearProperties::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	CAppDaliDeviceProperties_ControlGear* properties = this->m_properties;
	if (NULL == properties) {
		ASSERT(NULL != properties);
		return TRUE;
	}

	const BOOL controlsEnabledViewmodeOnline = (this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE); 

	this->setBusyGuiControlsUpdate(true);	
	{
		m_btnUpload.EnableWindow((controlsEnabledViewmodeOnline) && this->getPropertyChanged() ? TRUE : FALSE);
		m_btnReadConfig.EnableWindow(controlsEnabledViewmodeOnline);
		m_btnRestoreDefaults.EnableWindow(controlsEnabledViewmodeOnline); 
		m_btnUndoChanges.EnableWindow((controlsEnabledViewmodeOnline) && this->getPropertyChanged() ? TRUE : FALSE);
	}

	m_sliderFadeTime.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderFadeRate.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderMinimum.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderMaximum.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderPowerOn.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderSystemFailure.EnableWindow(controlsEnabledViewmodeOnline);

	// Tekst in de progress info (m_staticProgressInfo) control.
	this->updateMsgProgressInfo();

	int physicalMinimum = properties->getPhysicalMinValue();
	if (physicalMinimum < DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN) {
		physicalMinimum = DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN;	
	}

	{	
		const int value = NormalizeValue(properties->getLevelMin(),physicalMinimum,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
		m_sliderMinimum.SetPos(value);		

		CString strValue;
		strValue.Format("[%d]",value);
		m_editMinimum.SetWindowText(strValue);		
	}

	{
		const int value = NormalizeValue(properties->getLevelMax(),physicalMinimum,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
		m_sliderMaximum.SetPos(value);		

		CString strValue;
		strValue.Format("[%d]",value);
		m_editMaximum.SetWindowText(strValue);
	}

	{
		const int value = NormalizeValue(properties->getLevelPowerOn(),physicalMinimum,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
		m_sliderPowerOn.SetPos(value);		

		CString strValue;
		strValue.Format("[%d]",value);
		m_editPowerOn.SetWindowText(strValue);
	}

	{
		const int value = NormalizeValue(properties->getLevelSystemFailure(),physicalMinimum,DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX);
		m_sliderSystemFailure.SetPos(value);	

		CString strValue;
		strValue.Format("[%d]",value);
		m_editSystemFailure.SetWindowText(strValue);
	}

	{

		// DALI-2 Support. (zie NOTE_DALI2_FRAMERATE)
		// Wanneer ondersteund min rate 0, wanneer niet ondersteund komt op 1
		const int minimumFadeRate = (this->m_supportExtendedProperties ? DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MIN : DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MIN + 1);
		const int tempValueRate = CAppDaliDeviceProperties_ControlGear::getFadeRate(*properties);
		const int value = NormalizeValue(tempValueRate,minimumFadeRate,DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MAX);
		m_sliderFadeRate.SetPos(value);

		CString strValue;
		CAppDlgDaliWorker::getStringFadeRate(&strValue,value);
		m_editFadeRate.SetWindowText(strValue);
	}

	{
		const int tempValueTime = CAppDaliDeviceProperties_ControlGear::getFadeTime(*properties);
		const int value = NormalizeValue(tempValueTime,DALI_SYSTEM_CONTROLGEAR_DIM_FADETIME_MIN,DALI_SYSTEM_CONTROLGEAR_DIM_FADETIME_MAX);
		m_sliderFadeTime.SetPos(value);

		CString strValue;
		CAppDlgDaliWorker::getStringFadeTime(&strValue,value);
		m_editFadeTime.SetWindowText(strValue);
	}

	this->setBusyGuiControlsUpdate(false);
	return TRUE;
}


void CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderFadeTime(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_properties) {
		const int sliderPos = m_sliderFadeTime.GetPos();
		if (sliderPos >= DALI_SYSTEM_CONTROLGEAR_DIM_FADETIME_MIN && sliderPos<=DALI_SYSTEM_CONTROLGEAR_DIM_FADETIME_MAX) {	
			const int prevValue = CAppDaliDeviceProperties_ControlGear::getFadeTime(*this->m_properties);
			if (sliderPos != prevValue) {			
				CAppDaliDeviceProperties_ControlGear::setFadeTime(this->m_properties,sliderPos);
				this->setPropertyChanged(true);
			}
		}
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

// DALI-2 Support.  (zie NOTE_DALI2_FRAMERATE)
// Wanneer ondersteund min rate 0, wanneer niet ondersteund komt op 1
void CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderFadeRate(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_properties) {
		const int minimumFadeRate = (this->m_supportExtendedProperties ? DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MIN : DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MIN + 1);
		int sliderPos = m_sliderFadeRate.GetPos();
		if (sliderPos < minimumFadeRate) {
			sliderPos = minimumFadeRate;	
		}
		if (sliderPos<=DALI_SYSTEM_CONTROLGEAR_DIM_FADERATE_MAX) {	
			const int prevValue = CAppDaliDeviceProperties_ControlGear::getFadeRate(*this->m_properties);
			if (sliderPos != prevValue) {			
				CAppDaliDeviceProperties_ControlGear::setFadeRate(this->m_properties,sliderPos);
				this->setPropertyChanged(true);
			}
		}
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderMinimum(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_properties) {
		const int sliderPos = m_sliderMinimum.GetPos();
		if (sliderPos >= DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN && sliderPos<=DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX) {	
			const int prevValue = this->m_properties->getLevelMin();
			if (sliderPos != prevValue) {			
				this->m_properties->setLevelMin(sliderPos);
				this->setPropertyChanged(true);
			}
		}
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderMaximum(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_properties) {
		const int sliderPos = m_sliderMaximum.GetPos();
		if (sliderPos >= DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN && sliderPos<=DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX) {	
			const int prevValue = this->m_properties->getLevelMax();
			if (sliderPos != prevValue) {			
				this->m_properties->setLevelMax(sliderPos);
				this->setPropertyChanged(true);
			}
		}
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderPowerOn(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_properties) {
		const int sliderPos = m_sliderPowerOn.GetPos();
		if (sliderPos >= DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN && sliderPos<=DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX) {	
			const int prevValue = this->m_properties->getLevelPowerOn();
			if (sliderPos != prevValue) {			
				this->m_properties->setLevelPowerOn(sliderPos);
				this->setPropertyChanged(true);
			}
		}
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgChildDaliControlgearProperties::OnNMReleasedcaptureSliderSystemFailure(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_properties) {
		const int sliderPos = m_sliderSystemFailure.GetPos();
		if (sliderPos >= DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN && sliderPos<=DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX) {	
			const int prevValue = this->m_properties->getLevelSystemFailure();
			if (sliderPos != prevValue) {			
				this->m_properties->setLevelSystemFailure(sliderPos);
				this->setPropertyChanged(true);
			}
		}
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgChildDaliControlgearProperties::uploadData( ) {
	if ((this->getCurrDeviceShortAddress() < 0) || (NULL == this->m_properties)) {
		ASSERT(this->getCurrDeviceShortAddress()>=0);
		ASSERT(0 != this->m_properties);
		return;
	}
	const bool result = this->getWorker().startUploadControlGearDeviceProperties(this->getCurrDeviceShortAddress(), *this->m_properties);
	if (result) {

	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildDaliControlgearProperties::restoreToDefaults( ) {
	if (NULL == this->m_properties) {
		ASSERT(NULL != this->m_properties);
		return FALSE;
	}
	CAppDaliDevice_Utils::setDefaults(this->m_properties);
	return TRUE;
}

BOOL CDlgChildDaliControlgearProperties::undoChanges( ) {
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
	return TRUE;
}

void CDlgChildDaliControlgearProperties::OnBnClickedButtonUpload() {
	this->uploadData( );
}

void CDlgChildDaliControlgearProperties::OnBnClickedButtonReadConfig() {
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

// IAppDlgDaliGui_Host	
void CDlgChildDaliControlgearProperties::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_PROPERTIES == task) {		
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
	
void CDlgChildDaliControlgearProperties::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_PROPERTIES == task) {	
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
	}	
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliControlgearProperties::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());	
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_PROPERTIES == task) {
		// this->m_strErrorMessage.Format("Error happened!\r\n%s");
		this->setUploadBusy(false);	
	}
	else if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_DOWNLOAD_PROPERTIES == task) {	
		this->setDownloadBusy(false);	
	}
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearProperties::OnBnClickedButtonRestore() {
	if (FALSE == promptUserRestoreDefaults(*this)) {
		return;
	}
	this->restoreToDefaults();							
	this->setPropertyChanged(true);
	this->setUploadDone(false);
	this->setDownloadDone(false);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearProperties::OnBnClickedButtonUndo() {
	if (FALSE == promptUserUndoChanges(*this)) {
		return;
	}
	this->undoChanges();
	this->setPropertyChanged(true);
	this->setUploadDone(false);
	this->setDownloadDone(false);
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
BOOL CDlgChildDaliControlgearProperties::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}

	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_EDIT_FADE_TIME:
		case IDC_SLIDER_FADE_TIME: {
			this->m_strToolTipText = _T("Set the fade time to reach the target value. CAUTION: Use a short time (less than 1.4 seconds).");
			handled = TRUE;
		} break;
		case IDC_EDIT_FADE_RATE:
		case IDC_SLIDER_FADE_RATE: {
			this->m_strToolTipText = _T("Set the number of steps to reach the target value.");
			handled = TRUE;
		} break;
		case IDC_EDIT_MINIMUM:
		case IDC_SLIDER_MINIMUM: {
			this->m_strToolTipText = _T("Set the minimum dimming value (range: 1–254).");
			handled = TRUE;
		} break;
		case IDC_EDIT_MAXIMUM:
		case IDC_SLIDER_MAXIMUM: {
			this->m_strToolTipText = _T("Set the maximum dimming value (range: 1–254).");
			handled = TRUE;
		} break;
		case IDC_EDIT_POWERON:
		case IDC_SLIDER_POWERON: {
			this->m_strToolTipText = _T("Set the light level after bus power-on (range: 0–254).");
			handled = TRUE;
		} break;
		case IDC_EDIT_SYSTEMFAILURE:
		case IDC_SLIDER_SYSTEMFAILURE: {
			this->m_strToolTipText = _T("Set the fallback value for bus or device failure (range: 0–254).");
			handled = TRUE;
		} break;
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
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
