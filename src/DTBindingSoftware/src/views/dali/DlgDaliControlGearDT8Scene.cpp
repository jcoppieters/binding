// DlgDaliControlGearDT8Scene.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDaliControlGearDT8Scene.h"
#include "app/AppDali/appdalidevices.h"
#include "app/AppDali/appdali_common.h"
#include "appdlgdaliworker.h"
#include "utils/mfc/combobox_util.h"

#include "DlgDaliControlGearChildDT8_TC.h"
#include "DlgDaliControlGearChildDT8_XY.h"
#include "DlgDaliControlGearChildDT8_RGBWAF.h"
using namespace DUOTECNO::DALI;
/*********************************************************************************************/
// DEBUG Macro om de diverse kleur combinaties te testen.
#undef DEBUG_SIMULATE
/*********************************************************************************************/
#if (_DEBUG) 
#define DEBUG_SIMULATE	0	// OPPASSEN MOET OP 0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#else
#define DEBUG_SIMULATE	0
#endif
/*********************************************************************************************/
#if (DEBUG_SIMULATE == 1)
	#include "app/AppDali/appdaliprotocol_def.h"
#endif
/*********************************************************************************************/
#if (DEBUG_SIMULATE == 1)
static CAppDaliDeviceProperties_ControlGear* dbgSimulateCreateProperties(int selectedScene) {
	static CAppDaliDeviceProperties_ControlGear dbgProperties;


	enum { 
		NR_OF_RGBWA_CHANNELS = 4, 
		NR_OF_PRIMARY_CHANNELS_N = 3,
		COLOR_RED = 128,
		COLOR_GREEN = 128,
		COLOR_BLUE = 128,
		COLOR_WHITE = 192,
		COLOR_AMBER = 250,
		COLOR_FREECOLOR = 255
	};

	const DaliDT8ColorTypes colorType = // DALI_DT8_COLOUR_TYPE_XY_COORDINATE; 
										 // DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC; 
										DALI_DT8_COLOUR_TYPE_RGBWAF;	
										// DALI_DT8_COLOUR_TYPE_PRIMARY_N;

	const int colorFeatures = DALI_DT8_COLOR_FEATURES_XY_COORDINATE + 
							  DALI_DT8_COLOR_FEATURES_COLOR_TEMPERATURE + 
							  (NR_OF_RGBWA_CHANNELS << 5) +
							  (NR_OF_PRIMARY_CHANNELS_N << 2);
	dbgProperties.setColorFeatures(colorFeatures);

	CAppDaliObjectDT8ColorData colorData;
	colorData.setColorType(colorType);
	switch(colorType) {	
		case DALI_DT8_COLOUR_TYPE_PRIMARY_N: {
			colorData.setColorDimLevelChannel(0,COLOR_RED);
			colorData.setColorDimLevelChannel(1,COLOR_GREEN);
			colorData.setColorDimLevelChannel(2,COLOR_BLUE);			
		} break;
		case DALI_DT8_COLOUR_TYPE_RGBWAF: {
			colorData.setColorDimLevelRed(COLOR_RED);
			colorData.setColorDimLevelGreen(COLOR_GREEN);
			colorData.setColorDimLevelBlue(COLOR_BLUE);
			colorData.setColorDimLevelWhite(COLOR_WHITE);
			colorData.setColorDimLevelAmber(COLOR_AMBER);
			colorData.setColorDimLevelFreeColor(COLOR_FREECOLOR);
		} break;
		case DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC: {
			colorData.setColorTemperature(5000);		// Is MirekWaarde.							  
		} break;
		case DALI_DT8_COLOUR_TYPE_XY_COORDINATE: {
			colorData.setCoordinateX(65535/2);									 
			colorData.setCoordinateY(65535/2);	
		} break;
	}			
	dbgProperties.setDT8SceneColorInfo(selectedScene,colorData);
	return &dbgProperties;
}
#endif	/* (DEBUG_SIMULATE == 1) */
/*********************************************************************************************/


// CDlgDaliControlGearDT8Scene dialog
IMPLEMENT_DYNAMIC(CDlgDaliControlGearDT8Scene, CDialog)

const CString CDlgDaliControlGearDT8Scene::STRING_MASK_VALUE = "MASK";

CDlgDaliControlGearDT8Scene::CDlgDaliControlGearDT8Scene(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDaliControlGearDT8Scene::IDD, pParent), m_refWorker(refWorker), m_viewMode(VIEWMODE_OFFLINE),
	m_viewModeColorType(VIEW_MODE_COLOR_TYPE_UNKNOWN),
	m_properties(NULL), m_pSceneDT8ColorData(NULL), m_shortAddress(-1),  m_selectedScene(-1), m_currentLevel(-1), 
	m_strSceneName(), m_propertyChanged(FALSE), m_busyEditingIntensity(FALSE), m_busyUpdatingControls(FALSE)
{
	for (int i=0;i<NR_OF_CHILD_DIALOGS;++i) {
		m_pChildDialogs[i] = NULL;
	}
}

CDlgDaliControlGearDT8Scene::~CDlgDaliControlGearDT8Scene() {
	if (NULL != m_pSceneDT8ColorData) {
		delete m_pSceneDT8ColorData;
		m_pSceneDT8ColorData = NULL;
	}
	this->deallocSubDialogs();
}

void CDlgDaliControlGearDT8Scene::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD_ALL, m_btnUploadAll);
	DDX_Control(pDX, IDC_BUTTON_TEST, m_btnTest);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_BUTTON_UNDO, m_btnUndo);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editSceneName);
	DDX_Control(pDX, IDC_EDIT_SCENE_NUMBER, m_editSceneNumber);
	DDX_Control(pDX, IDC_EDIT_SCENE_INTENSITY, m_editSceneIntensity);
	DDX_Control(pDX, IDC_SLIDER_INTENSITY, m_sliderIntensity);
	DDX_Control(pDX, IDC_TAB_SUBDIALOGS, m_tabSubDialogs);
	DDX_Control(pDX, IDC_STATIC_FOOTER, m_staticTextFooter);
	DDX_Control(pDX, IDC_STATIC_HEADER, m_staticTextHeader);
}

BEGIN_MESSAGE_MAP(CDlgDaliControlGearDT8Scene, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS, OnUpdateControls)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_INTENSITY, &CDlgDaliControlGearDT8Scene::OnNMReleasedcaptureSliderItensity)
	ON_EN_CHANGE(IDC_EDIT_SCENE_INTENSITY, &CDlgDaliControlGearDT8Scene::OnEnChangeSceneIntensity)
	ON_EN_KILLFOCUS(IDC_EDIT_SCENE_INTENSITY, &CDlgDaliControlGearDT8Scene::OnEnKillfocusSceneIntensity)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, &CDlgDaliControlGearDT8Scene::OnEnKillfocusSceneName)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CDlgDaliControlGearDT8Scene::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_ALL, &CDlgDaliControlGearDT8Scene::OnBnClickedButtonUploadAll)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CDlgDaliControlGearDT8Scene::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDlgDaliControlGearDT8Scene::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, &CDlgDaliControlGearDT8Scene::OnBnClickedButtonUndo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SUBDIALOGS, &CDlgDaliControlGearDT8Scene::OnTabChanged)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_SUBDIALOGS, &CDlgDaliControlGearDT8Scene::OnTabChanging)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgDaliControlGearDT8Scene::OnToolTipNeedText)
	ON_BN_CLICKED(IDOK, &CDlgDaliControlGearDT8Scene::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgDaliControlGearDT8Scene message handlers
BOOL CDlgDaliControlGearDT8Scene::showDialog(int shortAddress, int selectedScene, CAppDaliDeviceProperties_ControlGear* properties, int currentLevel, const CString& name) {
	this->m_shortAddress = shortAddress;
	this->m_currentLevel = currentLevel;
	this->m_selectedScene = selectedScene;

#if (DEBUG_SIMULATE == 0)
	this->m_properties = properties;
#else
	this->m_properties = dbgSimulateCreateProperties(this->m_selectedScene);
#endif

	this->m_strSceneName = name;
	this->m_propertyChanged = FALSE;	
	
	if (IDOK == this->DoModal()) {	
		ASSERT(this->m_viewModeColorType != VIEW_MODE_COLOR_TYPE_UNKNOWN);
		if (this->m_viewModeColorType != VIEW_MODE_COLOR_TYPE_UNKNOWN) {
			if ((NULL != m_pChildDialogs[this->m_viewModeColorType])) {
				// Color data updaten.
				m_pChildDialogs[this->m_viewModeColorType]->updateColorData(TRUE);

				// Dialoog data copieren naar de properties.
				m_pChildDialogs[this->m_viewModeColorType]->updateProperties(this->m_selectedScene, this->m_properties);
			}
		}
		this->updateIntensityLevelData(TRUE);
		return TRUE;
	}
	return FALSE;	// this->m_propertyChanged;
}

BOOL CDlgDaliControlGearDT8Scene::OnInitDialog() {
	CDialog::OnInitDialog();
	this->initModel();
	this->initControls();
	this->PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

BOOL CDlgDaliControlGearDT8Scene::getPropertyChanged() const {
	if (this->m_propertyChanged) {
		return TRUE;
	}
	
	for (int i = 0; i < NR_OF_CHILD_DIALOGS; ++i) {
		if (NULL != m_pChildDialogs[i]) {
			if (m_pChildDialogs[i]->getPropertyChanged()) {
				return TRUE;			
			}
		}
	}
	return FALSE;
}

void CDlgDaliControlGearDT8Scene::onNotifyGuiTaskStarted(DUOTECNO::DALI::Task_t task) {
	this->setViewMode(VIEWMODE_ONLINE_BUSY);	
}

// 
void CDlgDaliControlGearDT8Scene::onNotifyGuiTaskDone(DUOTECNO::DALI::Task_t task) {
	if (APPDALI_FSM_TASK_CONTROLGEAR_DEVICE_UPLOAD_SCENES == task) {
		for (int i = 0; i < NR_OF_CHILD_DIALOGS; ++i) {
			if (NULL != m_pChildDialogs[i]) {
				m_pChildDialogs[i]->clearFlagPropertyChanged();
			}
		}	
		this->m_propertyChanged = FALSE;
	}
	this->setViewMode(VIEWMODE_ONLINE);	
}

void CDlgDaliControlGearDT8Scene::onNotifyGuiTaskError(DUOTECNO::DALI::Task_t task, const CString& strMessage) {
	this->setViewMode(VIEWMODE_ONLINE);
}

BOOL CDlgDaliControlGearDT8Scene::PreTranslateMessage(MSG* pMsg) {
	if (NULL != pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
		CWnd* pWnd = GetFocus();
		if (pWnd) {
			const DWORD identifier = pWnd->GetDlgCtrlID();
			if (IDC_EDIT_SCENE_INTENSITY == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderIntensity.SetFocus();
				return TRUE;
			}			
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgDaliControlGearDT8Scene::initControls() {

	const CString msg("Extended configuration for DT8 color scenes.\r\n"
					  "Values can be ignored using 'MASK' values'.\r\n" 
					  "When the scene is executed only values which are non-masked are applied."); 
	m_staticTextHeader.SetWindowText(msg);

	// Scene number & Scene Name: Read-Only.
	// this->m_editSceneName.EnableWindow(1);
	this->m_editSceneNumber.EnableWindow(0);

	// Slider intensity.
	this->m_sliderIntensity.SetRange(0, 255);
	this->m_sliderIntensity.SetTicFreq(5);

	// De verschillende mogelijkheden ifv de colorfeatures
	this->initControlTabDialogs( );

	// ifv de viewmode de tabcontrol updaten.
	if (this->setTabSelected(this->m_viewModeColorType)) {
		this->showActiveDialog(this->m_viewModeColorType);
	}

	// Tooltips 
	this->EnableToolTips(TRUE);
}

void CDlgDaliControlGearDT8Scene::initControlTabDialogs() {
	int tabCtrlIndex = 0;
	this->deallocSubDialogs();
	m_tabSubDialogs.DeleteAllItems();		
	if (CAppDaliDeviceProperties_ControlGear::getColorFeaturesSupportColorTemperatureTC(*this->m_properties)) {
		m_tabSubDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,tabCtrlIndex,"Color temperature TC",0,VIEW_MODE_COLOR_TYPE_TEMPERATURE,0,0);
		this->m_pChildDialogs[VIEW_MODE_COLOR_TYPE_TEMPERATURE] = new CDlgDaliControlGearChildDT8_TC(this,this->m_pSceneDT8ColorData);
		this->m_pChildDialogs[VIEW_MODE_COLOR_TYPE_TEMPERATURE]->Create(CDlgDaliControlGearChildDT8_TC::IDD,&m_tabSubDialogs);
		tabCtrlIndex++;
	}
	if (CAppDaliDeviceProperties_ControlGear::getColorFeaturesSupportColorCoordinatesXY(*this->m_properties)) {
		m_tabSubDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,tabCtrlIndex,"Color XY-Coordinates",0,VIEW_MODE_COLOR_TYPE_COORDINATE_XY,0,0);
		this->m_pChildDialogs[DALI_DT8_COLOR_FEATURES_XY_COORDINATE] = new CDlgDaliControlGearChildDT8_XY(this,this->m_pSceneDT8ColorData);
		this->m_pChildDialogs[DALI_DT8_COLOR_FEATURES_XY_COORDINATE]->Create(CDlgDaliControlGearChildDT8_XY::IDD,&m_tabSubDialogs);
		tabCtrlIndex++;
	}
	const int nrOfRGBWafChannels = CAppDaliDeviceProperties_ControlGear::getColorFeaturesNrOfRGBWAFChannels(*this->m_properties);
	if (nrOfRGBWafChannels > 0) {	
		m_tabSubDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,tabCtrlIndex,"Color RGBWAF",0,VIEW_MODE_COLOR_TYPE_RGBWAF,0,0);
		this->m_pChildDialogs[VIEW_MODE_COLOR_TYPE_RGBWAF] = new CDlgDaliControlGearChildDT8_RGBWAF(this,this->m_pSceneDT8ColorData,CDlgDaliControlGearChildDT8_RGBWAF::MODE_COLOR_TYPE_RGBWAF, nrOfRGBWafChannels);
		this->m_pChildDialogs[VIEW_MODE_COLOR_TYPE_RGBWAF]->Create(CDlgDaliControlGearChildDT8_RGBWAF::IDD,&m_tabSubDialogs);
		tabCtrlIndex++;
	}
	const int nrOfPrimaryNChannels = CAppDaliDeviceProperties_ControlGear::getColorFeaturesNrOfPrimaryNChannels(*this->m_properties);
	if (nrOfPrimaryNChannels > 0) {	
		m_tabSubDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,tabCtrlIndex,"Color Primary N Channels",0,VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS,0,0);
		this->m_pChildDialogs[VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS] = new CDlgDaliControlGearChildDT8_RGBWAF(this,this->m_pSceneDT8ColorData,CDlgDaliControlGearChildDT8_RGBWAF::MODE_COLOR_TYPE_PRIMARY_N, nrOfPrimaryNChannels);
		this->m_pChildDialogs[VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS]->Create(CDlgDaliControlGearChildDT8_RGBWAF::IDD,&m_tabSubDialogs);
		tabCtrlIndex++;
	}
	if (tabCtrlIndex>0) {	
		m_tabSubDialogs.ShowWindow(TRUE);	
		this->OnTabChanged(NULL, NULL);
		m_tabSubDialogs.SetCurSel(0);
	}

	m_staticTextFooter.ShowWindow(tabCtrlIndex>1 ? TRUE : FALSE);
	if (tabCtrlIndex>1) {
		const CString strMsg("Attention: This devices supports more then one color mode.\r\n" 
							 "A DT8 color scene can only have settings for one color mode.\r\n");	
		m_staticTextFooter.SetWindowText(strMsg);
	}
}

void CDlgDaliControlGearDT8Scene::initModel() {

	this->m_busyEditingIntensity = FALSE;
	this->m_busyUpdatingControls = FALSE;
	this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_UNKNOWN;
	this->m_viewMode = VIEWMODE_OFFLINE;

	if (this->m_refWorker.isConnected()) {
		if (this->m_refWorker.getDaliCfgTaskBusy()) {
			this->m_viewMode = VIEWMODE_ONLINE_BUSY;
		}
		else {
			this->m_viewMode = VIEWMODE_ONLINE;
		}
	}
	else {
		this->m_viewMode = VIEWMODE_OFFLINE;
	}

	if (this->m_currentLevel < 0 || this->m_currentLevel > 255)
		this->m_currentLevel = 255;

	this->initModelViewModeColorType();
}

void CDlgDaliControlGearDT8Scene::initModelViewModeColorType() {
	ASSERT(NULL == this->m_pSceneDT8ColorData);

	if (NULL == this->m_properties) {
		ASSERT(NULL != this->m_properties);
		return;
	}

	const CAppDaliObjectDT8ColorData& refSource = this->m_properties->getDT8SceneColorInfo(this->m_selectedScene);
	this->m_pSceneDT8ColorData = new CAppDaliObjectDT8ColorData(refSource);
	ENSURE(this->m_pSceneDT8ColorData);

	// View mode aanpassen aan de gegevens van de DT8 Color scene.
	const int colorType = this->m_pSceneDT8ColorData->getColorType();
	if (colorType < 0 || colorType >= 255) {
		this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_UNKNOWN;
	}
	else if (CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(*this->m_pSceneDT8ColorData)) {
		this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_TEMPERATURE;
	}
	else if (CAppDaliObjectDT8ColorData_Utils::isActiveTypeColorCoordinateXY(*this->m_pSceneDT8ColorData)) {
		this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_COORDINATE_XY;
	}
	else if (CAppDaliObjectDT8ColorData_Utils::isActiveTypeColorRGBWAF(*this->m_pSceneDT8ColorData)) {
		this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_RGBWAF;
	}
	else if (CAppDaliObjectDT8ColorData_Utils::isActiveTypeColorPrimaryChannelN(*this->m_pSceneDT8ColorData)) {
		this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS;
	}

	// Indien de color data leeg is dan de view mode aanpassen aan de mogelijkheden via de combo box.
	if (VIEW_MODE_COLOR_TYPE_UNKNOWN == this->m_viewModeColorType) {			
		if (CAppDaliDeviceProperties_ControlGear::getColorFeaturesSupportColorCoordinatesXY(*this->m_properties)) {
			this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_COORDINATE_XY;
		}
		else if (CAppDaliDeviceProperties_ControlGear::getColorFeaturesSupportColorTemperatureTC(*this->m_properties)) {	
			this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_TEMPERATURE;
		}	
		else if (CAppDaliDeviceProperties_ControlGear::getColorFeaturesNrOfRGBWAFChannels(*this->m_properties) > 0) {
			this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_RGBWAF;
		}
		else if (CAppDaliDeviceProperties_ControlGear::getColorFeaturesNrOfPrimaryNChannels(*this->m_properties) > 0) {
			this->m_viewModeColorType = VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS;
		}
	}
}
int CDlgDaliControlGearDT8Scene::getCurrentLevel() const {
	return this->m_currentLevel;
}

int CDlgDaliControlGearDT8Scene::getCurrentScene() const {
	return this->m_selectedScene;
}

const CString& CDlgDaliControlGearDT8Scene::getSceneName() const {
	return this->m_strSceneName;
}

LONG CDlgDaliControlGearDT8Scene::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	this->m_busyUpdatingControls = TRUE;
	this->updateControlsActualLevel();
	this->updateControlsSceneName();
	this->updateControlsSceneNumber();
	this->updateControlsTaskButtons();
	this->m_busyUpdatingControls = FALSE;
	return true;
}

void CDlgDaliControlGearDT8Scene::updateControlsActualLevel() {	
	m_sliderIntensity.SetPos(this->m_currentLevel);

	m_editSceneIntensity.EnableWindow(this->m_viewMode == VIEWMODE_ONLINE ? 1 : 0);
	if (!this->m_busyEditingIntensity) {
		CString strIntensity;
		if (this->m_currentLevel >= 255) {
			strIntensity = STRING_MASK_VALUE;
		}
		else {
			strIntensity.Format("%d", this->m_currentLevel);
		}
		m_editSceneIntensity.SetWindowText(strIntensity);
	}
}

void CDlgDaliControlGearDT8Scene::updateControlsSceneName() {
	this->m_editSceneName.SetWindowText(this->m_strSceneName);
}

void CDlgDaliControlGearDT8Scene::updateControlsSceneNumber() {
	CString str;
	str.Format("%d", this->m_selectedScene);
	this->m_editSceneNumber.SetWindowText(str);
}

void CDlgDaliControlGearDT8Scene::updateControlsTaskButtons() {
	switch (this->m_viewMode) {
		case VIEWMODE_OFFLINE: {	
			m_btnUpload.EnableWindow(0);
			m_btnUploadAll.EnableWindow(0);
			m_btnTest.EnableWindow(0);	
			m_btnClear.EnableWindow(0);		// om te testen in offline mode op 1 zetten.
			m_btnUndo.EnableWindow(0);		// om te testen in offline mode op 1 zetten.
		} break;
		case VIEWMODE_ONLINE: {
			m_btnUpload.EnableWindow(1);
			m_btnUploadAll.EnableWindow(1);
			m_btnTest.EnableWindow(1);
			m_btnClear.EnableWindow(1);
			m_btnUndo.EnableWindow(1);		
		} break;
		case VIEWMODE_ONLINE_BUSY: {		
			m_btnUpload.EnableWindow(0);
			m_btnUploadAll.EnableWindow(0);
			m_btnTest.EnableWindow(0);
			m_btnClear.EnableWindow(0);
			m_btnUndo.EnableWindow(0);				
		} break;
	}
}

void CDlgDaliControlGearDT8Scene::OnNMReleasedcaptureSliderItensity(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_viewMode == VIEWMODE_ONLINE) {
		const int sliderPos = m_sliderIntensity.GetPos();
		ASSERT(sliderPos >= 0 && sliderPos <= 0xFF);
		if (this->m_currentLevel != sliderPos) {
			this->m_currentLevel = sliderPos;
			this->m_propertyChanged = TRUE;
		}			
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}


void CDlgDaliControlGearDT8Scene::OnEnChangeSceneIntensity() {
	if (this->m_busyUpdatingControls) {
		return;
	}
	this->m_busyEditingIntensity = true;
	this->KillTimer(TIMER_ID_EDITBOX_INTENSITY);
	this->SetTimer(TIMER_ID_EDITBOX_INTENSITY, 2000, (TIMERPROC)NULL);
}

void CDlgDaliControlGearDT8Scene::OnEnKillfocusSceneIntensity() {
	if (this->m_busyEditingIntensity) {
		this->KillTimer(TIMER_ID_EDITBOX_INTENSITY);
		this->GetEditBoxValueIntensity();
	}
}

void CDlgDaliControlGearDT8Scene::OnEnKillfocusSceneName() {
	if (this->m_busyUpdatingControls) {
		return;
	}
	enum { MAX_NAME_LENGTH = 15 };
	CString strTemp;
	m_editSceneName.GetWindowText(strTemp);
	if (0 != m_strSceneName.Compare(strTemp)) {
		this->m_strSceneName = strTemp.Left(MAX_NAME_LENGTH);
		this->m_propertyChanged = TRUE;
	}
}

void CDlgDaliControlGearDT8Scene::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_EDITBOX_INTENSITY == nIDEvent) {
		this->GetEditBoxValueIntensity();
		this->KillTimer(TIMER_ID_EDITBOX_INTENSITY);
	}
}

void CDlgDaliControlGearDT8Scene::GetEditBoxValueIntensity() {
	if (!this->m_busyEditingIntensity) {
		return;
	}
	this->m_busyEditingIntensity = FALSE;

	CString strValue;
	int newValue = 255;
	m_editSceneIntensity.GetWindowText(strValue);
	if (0 == strValue.Compare(STRING_MASK_VALUE)) {
		newValue = 255;
	}
	else {
		const int iValue = atoi((LPCTSTR)strValue);
		if (iValue < 0) {
			newValue = 255;
		}
		else if (iValue > 255) {
			newValue = 255;
		}
		else {
			newValue = iValue;
		}
	}
	if (newValue != this->m_currentLevel) {
		this->m_propertyChanged = TRUE;
		this->m_currentLevel = newValue;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearDT8Scene::deallocSubDialogs() {
	for (int i=0;i<NR_OF_CHILD_DIALOGS;++i) {
		if (NULL != m_pChildDialogs[i]) {			
			delete m_pChildDialogs[i];
			m_pChildDialogs[i] = NULL;
		}
	}
}

void CDlgDaliControlGearDT8Scene::updateIntensityLevelData(BOOL saveToProperties) {
	if (NULL == this->m_properties) {
		ASSERT(NULL != this->m_properties);
		return;
	}

	if (saveToProperties) {
		this->m_properties->setSceneLevel(this->m_selectedScene,this->m_currentLevel);
	}
	else {
		this->m_currentLevel = this->m_properties->getSceneLevel(this->m_selectedScene);
	}
}



void CDlgDaliControlGearDT8Scene::handleClickedButtonUpload(int shortAddress) {
	const ViewModeColorType_t type = this->getTabCurrentSelected();
	ASSERT(VIEW_MODE_COLOR_TYPE_UNKNOWN != type);
	if (VIEW_MODE_COLOR_TYPE_UNKNOWN != type) {
		ASSERT(type >= VIEW_MODE_COLOR_TYPE_TEMPERATURE && type <= VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS);
		ASSERT(type< NR_OF_CHILD_DIALOGS);
		CDlgDaliControlGearChildDT8* const pDlg = this->m_pChildDialogs[type];
		ASSERT(NULL != pDlg);
		if (NULL != pDlg) {
			// Dialoog data copieren naar de properties.
			pDlg->updateProperties(this->m_selectedScene, this->m_properties);
		}
	}
	// CurrentLevel opslaan in de properties.
	this->updateIntensityLevelData(TRUE);

	INVARIANT(NULL != this->m_properties);
	const bool result = this->m_refWorker.startUploadControlGearDeviceSingleScene(0xFF, this->m_selectedScene, *this->m_properties);
	ASSERT(result);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearDT8Scene::OnBnClickedButtonUpload() {
	this->handleClickedButtonUpload(this->m_shortAddress);
}

void CDlgDaliControlGearDT8Scene::OnBnClickedButtonUploadAll() {
	const CString strMsg("Uploading this scene to all devices will update the scene configuration on each device.\r\n"
				         "Previous configurations may be lost.\r\n\r\n"
					     "Do you want to send this scene to all devices ?");
	const CString strHeader("Upload scene to all the devices.");
	if (IDNO == this->MessageBox(strMsg, strHeader, MB_YESNO | MB_ICONQUESTION)) {
		return;
	}
	this->handleClickedButtonUpload(0xFF);
}

void CDlgDaliControlGearDT8Scene::OnBnClickedButtonTest() {
	if (this->m_selectedScene < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST || this->m_selectedScene > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST) {
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
	this->m_refWorker.sendGotoScene(CAppDlgDaliWorker::CONTROLGEAR_MSG_BROADCAST, 0xff, this->m_selectedScene);
}

void CDlgDaliControlGearDT8Scene::OnBnClickedButtonClear() {
	for (int i=0;i<NR_OF_CHILD_DIALOGS;++i) {
		if (NULL != this->m_pChildDialogs[i]) {
			this->m_pChildDialogs[i]->clearData(FALSE);
		}
	}
	this->m_currentLevel = 255;
	this->m_propertyChanged = TRUE;
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearDT8Scene::OnBnClickedButtonUndo() {
	for (int i=0;i<NR_OF_CHILD_DIALOGS;++i) {
		if (NULL != this->m_pChildDialogs[i]) {
			this->m_pChildDialogs[i]->updateColorData(FALSE);
			this->m_pChildDialogs[i]->updateDialog();
		}
	}
	// CurrentLevel ophalen van de properties.
	this->updateIntensityLevelData(FALSE);
	this->m_propertyChanged = FALSE;
	this->PostMessage(WM_UPDATE_CONTROLS);
}

CDlgDaliControlGearDT8Scene::ViewModeColorType_t CDlgDaliControlGearDT8Scene::getTabCurrentSelected() {
	const int nIndex = m_tabSubDialogs.GetCurSel();	
	if (nIndex < 0) {
		ASSERT(0);
		return VIEW_MODE_COLOR_TYPE_UNKNOWN;
	}
	TCITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	tcItem.lParam = VIEW_MODE_COLOR_TYPE_UNKNOWN;
	m_tabSubDialogs.GetItem(nIndex,&tcItem);
	
	int viewMode = tcItem.lParam;
	if (viewMode < VIEW_MODE_COLOR_TYPE_TEMPERATURE || viewMode > VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS) {
		ASSERT(0);
		viewMode = VIEW_MODE_COLOR_TYPE_UNKNOWN;
	}
	return (ViewModeColorType_t) viewMode;
}

BOOL CDlgDaliControlGearDT8Scene::setTabSelected(ViewModeColorType_t mode) {
	int selectItem = -1;
	const int count = m_tabSubDialogs.GetItemCount();
	for (int i=0;i<count;++i) {
		TCITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		tcItem.lParam = VIEW_MODE_COLOR_TYPE_UNKNOWN;
		m_tabSubDialogs.GetItem(i,&tcItem);
		if (tcItem.lParam == mode) {
			selectItem = i;
			break;
		}	
	}
	if ((selectItem>=0) && (m_tabSubDialogs.SetCurSel(selectItem) >= 0)) {
		return TRUE;	
	}	
	return (FALSE);
}

void CDlgDaliControlGearDT8Scene::setViewMode(ViewMode_t mode) {	
	if (this->m_viewMode != mode) {
		this->m_viewMode = mode;
		for (int i=0;i<NR_OF_CHILD_DIALOGS;i++) {
			if (NULL != this->m_pChildDialogs[i]) {
				this->m_pChildDialogs[i]->setViewMode((CDlgDaliControlGearChildDT8::ViewMode_t)mode);		// CAUTION
			}
		}
		this->PostMessage(WM_UPDATE_CONTROLS);
	}
}

// Result 1 wil zeggen dat er niet mag verlaten worden.
void CDlgDaliControlGearDT8Scene::OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult) {
	const ViewModeColorType_t type = this->getTabCurrentSelected();
	this->showActiveDialog(type);
}


void CDlgDaliControlGearDT8Scene::OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult) {
	*pResult = 0;		
}

void CDlgDaliControlGearDT8Scene::showActiveDialog(ViewModeColorType_t dialog) {
	for (int i=0;i<NR_OF_CHILD_DIALOGS;i++) {
		if ((i != dialog) && (NULL != this->m_pChildDialogs[i])) {
			this->m_pChildDialogs[i]->ShowWindow(SW_HIDE);		
		}
	}

	if (dialog >= NR_OF_CHILD_DIALOGS) {
		return;
	}

	CDlgDaliControlGearChildDT8* pDlg = this->m_pChildDialogs[(int)dialog];
	ASSERT(NULL != pDlg);
	if (NULL == pDlg) {
		return;
	}
	
	RECT rc;
	m_tabSubDialogs.GetItemRect(0,&rc);
	pDlg->SetWindowPos( NULL, rc.left + 1, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	pDlg->SetFocus();	
	pDlg->setViewMode((CDlgDaliControlGearChildDT8::ViewMode_t) this->m_viewMode);		// Opppassen!!
	// pDlg->onTabDialogEnterDialog();
}


// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgDaliControlGearDT8Scene::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR toolbarItemIdentifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		toolbarItemIdentifier = ::GetDlgCtrlID((HWND)toolbarItemIdentifier);  // Haal ID van de control op
	}

	BOOL handled = FALSE;
	switch(toolbarItemIdentifier) {
		case IDC_SLIDER_INTENSITY:
		case IDC_EDIT_SCENE_INTENSITY: {						
			this->m_strToolTipText = _T("Set intensity (Range: 0-254, 255=Mask/ignore)");
			handled = TRUE;
		} break;
		case IDC_BUTTON_UPLOAD: {
			this->m_strToolTipText = _T("Upload the current scene to the device");
			handled = TRUE;
		} break;
		case IDC_BUTTON_UPLOAD_ALL: {
			this->m_strToolTipText = _T("Upload the current scene to all control gear devices");
			handled = TRUE;
		} break;
		case IDC_BUTTON_TEST: {
			this->m_strToolTipText = _T("Test the uploaded scene (Broadcast to all devices)");
			handled = TRUE;
		} break;
		case IDC_EDIT_NAME: {
			this->m_strToolTipText = _T("Enter scene name (Max 15 characters)");
			handled = TRUE;		
		} break;
		case IDC_EDIT_SCENE_NUMBER: {
			this->m_strToolTipText = _T("Scene number");
			handled = TRUE;							
		} break;
		case IDOK: {
			this->m_strToolTipText = _T("Store the scene and upload later");
			handled = TRUE;							
		} break;
		case IDC_BUTTON_CLEAR: {
			this->m_strToolTipText = _T("Clear all scene information (Set all values to Mask/Ignore)");
			handled = TRUE;							
		} break;
		case IDC_BUTTON_UNDO: {
			this->m_strToolTipText = _T("Undo the last changes made");
			handled = TRUE;							
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
void CDlgDaliControlGearDT8Scene::OnBnClickedOk() {
	OnOK();
}
