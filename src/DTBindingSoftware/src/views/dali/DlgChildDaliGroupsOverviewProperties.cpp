// DlgChildDaliGroupsOverviewProperties.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliGroupsOverviewProperties.h"
#include "appdlgdaliworker.h"


// CDlgChildDaliGroupsOverviewProperties dialog
using namespace DUOTECNO::DALI;


static int NormalizeValue(int value,int minValue,int maxValue) {
	if (value < minValue) {
		value = minValue;
	}
	else if (value > maxValue) {
		value = maxValue;
	}
	return value;
}

IMPLEMENT_DYNAMIC(CDlgChildDaliGroupsOverviewProperties, CDialog)

CDlgChildDaliGroupsOverviewProperties::CDlgChildDaliGroupsOverviewProperties(CAppDlgDaliWorker& refWorker, CWnd* pParent) : 
	CDlgChildDaliGroupsOverviewBase(refWorker, CDlgChildDaliGroupsOverviewProperties::IDD, pParent), m_supportExtendedProperties(false),
	    m_deviceAddressProxyProperties(-1), m_properties(new CAppDaliDeviceProperties_ControlGear()) {
	ASSERT(NULL != this->m_properties);
}

CDlgChildDaliGroupsOverviewProperties::~CDlgChildDaliGroupsOverviewProperties() {
	if (NULL != this->m_properties) {
		delete this->m_properties;
		this->m_properties=NULL;
	}
}

void CDlgChildDaliGroupsOverviewProperties::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_BUTTON_FADE_TIME, m_btnSetFadeTime);
	DDX_Control(pDX, IDC_BUTTON_FADE_RATE, m_btnSetFadeRate);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnSetLevelMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnSetLevelMax);
	DDX_Control(pDX, IDC_BUTTON_POWERON, m_btnSetLevelPowerOn);
	DDX_Control(pDX, IDC_BUTTON_FAILURE, m_btnSetLevelSystemFailure);
}

BEGIN_MESSAGE_MAP(CDlgChildDaliGroupsOverviewProperties, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_FADE_TIME, &CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderFadeTime)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_FADE_RATE, &CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderFadeRate)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_MINIMUM, &CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderMinimum)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_MAXIMUM, &CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderMaximum)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_POWERON, &CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderPowerOn)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SYSTEMFAILURE, &CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderSystemFailure)
	ON_BN_CLICKED(IDC_BUTTON_FADE_TIME,OnButtonClickedSetFadeTime)
	ON_BN_CLICKED(IDC_BUTTON_FADE_RATE,OnButtonClickedSetFadeRate)
	ON_BN_CLICKED(IDC_BUTTON_MIN,OnButtonClickedSetLevelMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX,OnButtonClickedSetLevelMax)
	ON_BN_CLICKED(IDC_BUTTON_POWERON,OnButtonClickedSetLevelPowerOn)
	ON_BN_CLICKED(IDC_BUTTON_FAILURE,OnButtonClickedSetLevelFailure)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliGroupsOverviewProperties::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgChildDaliGroupsOverviewTest message handlers
void CDlgChildDaliGroupsOverviewProperties::init(int groupNumber) {	
	this->updateViewMode();
	this->setSelectedGroup(groupNumber);
	this->initModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliGroupsOverviewProperties::OnParentPropertyPageActive(int groupNumber,bool activeDialog) {
}

BOOL CDlgChildDaliGroupsOverviewProperties::OnParentPropertyPageLeave() {
	return FALSE;
}

// Bij het openen van het tabblad controleren of het eerst device van de groep niet veranderd is.
// Bij verandering van het device moeten we de properties opnieuw updaten. 
void CDlgChildDaliGroupsOverviewProperties::onTabDialogEnterDialog() {
	const int shortAddress = this->getWorker().getFirstDeviceInGroup(this->getSelectedGroup());
	if (shortAddress != this->m_deviceAddressProxyProperties) {
		// het eerste device in de groep werd gewijzigd aanpassen weergave van de default properties.
		this->initModel();
		this->PostMessage(WM_UPDATE_CONTROLS);
	}
}

void CDlgChildDaliGroupsOverviewProperties::initModel() {
	CAppDaliDeviceProperties_ControlGear* properties = this->m_properties;
	if (NULL == properties) {
		ASSERT(NULL != properties);
		return;
	}
	
	// Ophalen van de properties van de eerste groep member + bijhouden van het adres van de eerste member in de groep.
	int shortAddressFirstDevice = -1;
	if (this->getWorker().copyPropertiesFromFirstGroupMember(this->getSelectedGroup(),this->m_properties,&shortAddressFirstDevice)) {
		ASSERT((shortAddressFirstDevice >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST) && (shortAddressFirstDevice <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST));
		this->m_deviceAddressProxyProperties = shortAddressFirstDevice;
	}	
	else {
		int physicalMinimum = DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN;
		int levelMin = DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MIN;
		int levelMax = DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX;
		int levelPowerOn = DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX;
		int levelSystemFailure = DALI_SYSTEM_CONTROLGEAR_DIM_LEVEL_VALUE_MAX;
		int fadeRate = 1;	// TODO: DEFAULTS bepalen
		int fadeTime = 1;	// TODO: DEFAULTS bepalen
	
		properties->setLevelMin(levelMin);
		properties->setLevelMax(levelMax);
		properties->setPhysicalMinValue(physicalMinimum);
		properties->setLevelSystemFailure(levelSystemFailure);
		properties->setLevelPowerOn(levelPowerOn);
		CAppDaliDeviceProperties_ControlGear::setFadeTime(properties, fadeTime);
		CAppDaliDeviceProperties_ControlGear::setFadeRate(properties, fadeRate);

		this->m_deviceAddressProxyProperties = -1;
	}
}

// zie NOTE_DALI2_FRAMERATE
// Indien de fade rate op 0 staat wordt de extended fade rate gebruikt...
// Momenteel niet ondersteund. De slider wordt altijd op minimum waarde 1 geplaatst.
void CDlgChildDaliGroupsOverviewProperties::initGUIControls( ) {

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


LONG CDlgChildDaliGroupsOverviewProperties::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	if (!this->getInitDone()) {			
		this->initGUIControls( );
		this->setInitDone(true);
	}

	CAppDaliDeviceProperties_ControlGear* properties = this->m_properties;
	if (NULL == properties) {
		ASSERT(NULL != properties);
		return TRUE;
	}

	const BOOL controlsEnabledViewmodeOnline = (this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE); 

	this->setBusyGuiControlsUpdate(true);	

	// In functie van de view mode de controls enablen/disablen.
	m_sliderFadeTime.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderFadeRate.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderMinimum.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderMaximum.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderPowerOn.EnableWindow(controlsEnabledViewmodeOnline);
	m_sliderSystemFailure.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnSetFadeTime.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnSetFadeRate.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnSetLevelMin.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnSetLevelMax.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnSetLevelPowerOn.EnableWindow(controlsEnabledViewmodeOnline);
	m_btnSetLevelSystemFailure.EnableWindow(controlsEnabledViewmodeOnline);

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


void CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderFadeTime(NMHDR *pNMHDR, LRESULT *pResult) {
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
void CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderFadeRate(NMHDR *pNMHDR, LRESULT *pResult) {
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

void CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderMinimum(NMHDR *pNMHDR, LRESULT *pResult) {
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

void CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderMaximum(NMHDR *pNMHDR, LRESULT *pResult) {
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

void CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderPowerOn(NMHDR *pNMHDR, LRESULT *pResult) {
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

void CDlgChildDaliGroupsOverviewProperties::OnNMReleasedcaptureSliderSystemFailure(NMHDR *pNMHDR, LRESULT *pResult) {
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

void CDlgChildDaliGroupsOverviewProperties::OnButtonClickedSetFadeTime() {
	if (NULL == this->m_properties) {
		INVARIANT(NULL != this->m_properties);
		return;
	}	

	const int selectedGroup = this->getSelectedGroup();
	ASSERT((selectedGroup>=DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) && (selectedGroup<=DALI_SYSTEM_CONTROLGEAR_GROUP_LAST));

	const int value = CAppDaliDeviceProperties_ControlGear::getFadeTime(*this->m_properties);

	// Versturen group configuratie param.
	if (false == this->getWorker().sendControlGearCfgProperty(selectedGroup,CAppDlgDaliWorker::PROPERTY_FADE_TIME,value)) {
		return;
	}

	// Alle members van de groep updaten met deze waarde.
	this->getWorker().controlGearGroupsAdjustPropertyMembers(selectedGroup,CAppDlgDaliWorker::PROPERTY_FADE_TIME,value);
}


void CDlgChildDaliGroupsOverviewProperties::OnButtonClickedSetFadeRate() {
	if (NULL == this->m_properties) {
		INVARIANT(NULL != this->m_properties);
		return;
	}	

	const int selectedGroup = this->getSelectedGroup();
	ASSERT((selectedGroup>=DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) && (selectedGroup<=DALI_SYSTEM_CONTROLGEAR_GROUP_LAST));

	const int value = CAppDaliDeviceProperties_ControlGear::getFadeRate(*this->m_properties);

	// Versturen group configuratie param.
	if (false == this->getWorker().sendControlGearCfgProperty(selectedGroup,CAppDlgDaliWorker::PROPERTY_FADE_RATE,value)) {
		return;
	}

	// Alle members van de groep updaten met deze waarde.
	this->getWorker().controlGearGroupsAdjustPropertyMembers(selectedGroup,CAppDlgDaliWorker::PROPERTY_FADE_RATE,value);
}

void CDlgChildDaliGroupsOverviewProperties::OnButtonClickedSetLevelMin() {
	if (NULL == this->m_properties) {
		INVARIANT(NULL != this->m_properties);
		return;
	}	

	const int selectedGroup = this->getSelectedGroup();
	ASSERT((selectedGroup>=DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) && (selectedGroup<=DALI_SYSTEM_CONTROLGEAR_GROUP_LAST));

	const int value = this->m_properties->getLevelMin();

	// Versturen group configuratie param.
	if (false == this->getWorker().sendControlGearCfgProperty(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_MIN,value)) {
		return;
	}

	// Alle members van de groep updaten met deze waarde.
	this->getWorker().controlGearGroupsAdjustPropertyMembers(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_MIN,value);
}

void CDlgChildDaliGroupsOverviewProperties::OnButtonClickedSetLevelMax() {
	if (NULL == this->m_properties) {
		INVARIANT(NULL != this->m_properties);
		return;
	}	

	const int selectedGroup = this->getSelectedGroup();
	ASSERT((selectedGroup>=DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) && (selectedGroup<=DALI_SYSTEM_CONTROLGEAR_GROUP_LAST));

	const int value = this->m_properties->getLevelMax();

	// Versturen group configuratie param.
	if (false == this->getWorker().sendControlGearCfgProperty(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_MAX,value)) {
		return;
	}

	// Alle members van de groep updaten met deze waarde.
	this->getWorker().controlGearGroupsAdjustPropertyMembers(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_MAX,value);
}

void CDlgChildDaliGroupsOverviewProperties::OnButtonClickedSetLevelPowerOn() {
	if (NULL == this->m_properties) {
		INVARIANT(NULL != this->m_properties);
		return;
	}	

	const int selectedGroup = this->getSelectedGroup();
	ASSERT((selectedGroup>=DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) && (selectedGroup<=DALI_SYSTEM_CONTROLGEAR_GROUP_LAST));

	const int value = this->m_properties->getLevelPowerOn();

	// Versturen group configuratie param.
	if (false == this->getWorker().sendControlGearCfgProperty(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_POWERON,value)) {
		return;
	}

	// Alle members van de groep updaten met deze waarde.
	this->getWorker().controlGearGroupsAdjustPropertyMembers(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_POWERON,value);
}


void CDlgChildDaliGroupsOverviewProperties::OnButtonClickedSetLevelFailure() {
	if (NULL == this->m_properties) {
		INVARIANT(NULL != this->m_properties);
		return;
	}	

	const int selectedGroup = this->getSelectedGroup();
	ASSERT((selectedGroup>=DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST) && (selectedGroup<=DALI_SYSTEM_CONTROLGEAR_GROUP_LAST));

	const int value = this->m_properties->getLevelSystemFailure();

	// Versturen group configuratie param.
	if (false == this->getWorker().sendControlGearCfgProperty(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_SYSTEMFAILURE,value)) {
		return;
	}

	// Alle members van de groep updaten met deze waarde.
	this->getWorker().controlGearGroupsAdjustPropertyMembers(selectedGroup,CAppDlgDaliWorker::PROPERTY_LEVEL_SYSTEMFAILURE,value);
}


// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliGroupsOverviewProperties::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}

	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_BUTTON_FADE_TIME: {
			this->m_strToolTipText = _T("Send the fade time property for the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_FADE_RATE: {
			this->m_strToolTipText = _T("Send the fade rate property for the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_MIN: {
			this->m_strToolTipText = _T("Send the minimum value property for the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_MAX: {
			this->m_strToolTipText = _T("Send the maximum value property for the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_POWERON: {
			this->m_strToolTipText = _T("Send the power-on value property for the group");
			handled = TRUE;
		} break;
		case IDC_BUTTON_FAILURE: {
			this->m_strToolTipText = _T("Send the power failure value property for the group");
			handled = TRUE;
		} break;
		case IDC_EDIT_FADE_TIME:
		case IDC_SLIDER_FADE_TIME: {
			this->m_strToolTipText = _T("Set the fade time to the requested value. CAUTION: Use a short time (less than 1.4 sec).");
			handled = TRUE;
		} break;
		case IDC_EDIT_FADE_RATE:
		case IDC_SLIDER_FADE_RATE: {
			this->m_strToolTipText = _T("Set the number of steps to reach the requested value.");
			handled = TRUE;
		} break;
		case IDC_EDIT_MINIMUM:
		case IDC_SLIDER_MINIMUM: {
			this->m_strToolTipText = _T("Set the minimum dim value (range: 1–254)");
			handled = TRUE;
		} break;
		case IDC_EDIT_MAXIMUM:
		case IDC_SLIDER_MAXIMUM: {
			this->m_strToolTipText = _T("Set the maximum dim value (range: 1–254)");
			handled = TRUE;
		} break;
		case IDC_EDIT_POWERON:
		case IDC_SLIDER_POWERON: {
			this->m_strToolTipText = _T("Set the value for bus power on (range: 0–254)");
			handled = TRUE;
		} break;
		case IDC_EDIT_SYSTEMFAILURE:
		case IDC_SLIDER_SYSTEMFAILURE: {
			this->m_strToolTipText = _T("Set the value for bus or device failure (range: 0–254)");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
