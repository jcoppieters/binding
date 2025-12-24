// DlgDaliControlGearChildDT8_RGBWAF.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDaliControlGearChildDT8_RGBWAF.h"
#include "app/appdali/appdali_common.h"
#include "app/appdali/appdaliprotocol_def.h"
#include "utils/mfc/combobox_util.h"

// CDlgDaliControlGearChildDT8_RGBWAF dialog
using namespace DUOTECNO::DALI;

IMPLEMENT_DYNAMIC(CDlgDaliControlGearChildDT8_RGBWAF, CDialog)

CDlgDaliControlGearChildDT8_RGBWAF::CDlgDaliControlGearChildDT8_RGBWAF(CWnd* pParent,CAppDaliObjectDT8ColorData* pSceneDT8ColorData,
																	   ModeColorType_t mode,int nrOfChannels)
	: Base(CDlgDaliControlGearChildDT8_RGBWAF::IDD, pParent,pSceneDT8ColorData), m_modeColorType(mode), m_nrOfChannels(nrOfChannels),
	  m_busyEditingEditBoxNR(-1), m_busyEditingValue(FALSE), m_busyUpdatingControls(FALSE)
{ 	
	for (int i=0;i<MAX_NR_CHANNELS;i++) {
		this->m_values[i] = -1;
	}

	if (this->m_nrOfChannels < 0 || this->m_nrOfChannels > MAX_NR_CHANNELS) {
		ASSERT(this->m_nrOfChannels >= 0);
		ASSERT(this->m_nrOfChannels < MAX_NR_CHANNELS);
		this->m_nrOfChannels = MAX_NR_CHANNELS;
	}
}


CDlgDaliControlGearChildDT8_RGBWAF::~CDlgDaliControlGearChildDT8_RGBWAF()
{
}

void CDlgDaliControlGearChildDT8_RGBWAF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRESETS, m_comboPresets);
	DDX_Control(pDX, IDC_SLIDER_VALUE_R, m_sliderValue0);
	DDX_Control(pDX, IDC_SLIDER_VALUE_G, m_sliderValue1);
	DDX_Control(pDX, IDC_SLIDER_VALUE_B, m_sliderValue2);
	DDX_Control(pDX, IDC_SLIDER_VALUE_W, m_sliderValue3);
	DDX_Control(pDX, IDC_SLIDER_VALUE_A, m_sliderValue4);
	DDX_Control(pDX, IDC_SLIDER_VALUE_F, m_sliderValue5);
	DDX_Control(pDX, IDC_EDIT_VALUE_R, m_editValue0);
	DDX_Control(pDX, IDC_EDIT_VALUE_G, m_editValue1);
	DDX_Control(pDX, IDC_EDIT_VALUE_B, m_editValue2);
	DDX_Control(pDX, IDC_EDIT_VALUE_W, m_editValue3);
	DDX_Control(pDX, IDC_EDIT_VALUE_A, m_editValue4);
	DDX_Control(pDX, IDC_EDIT_VALUE_F, m_editValue5);
	DDX_Control(pDX, IDC_STATIC_INFO, m_infoText);
}

BEGIN_MESSAGE_MAP(CDlgDaliControlGearChildDT8_RGBWAF, CDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CDlgDaliControlGearChildDT8_RGBWAF::OnUpdateControls)	
	ON_CBN_SELCHANGE(IDC_COMBO_PRESETS,&CDlgDaliControlGearChildDT8_RGBWAF::OnCbnSelchangePresets)	
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_R, &CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue0)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_G, &CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_B, &CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_W, &CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue3)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_A, &CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue4)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_F, &CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue5)
	ON_EN_CHANGE(IDC_EDIT_VALUE_R, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue0)
	ON_EN_CHANGE(IDC_EDIT_VALUE_G, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue1)
	ON_EN_CHANGE(IDC_EDIT_VALUE_B, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue2)
	ON_EN_CHANGE(IDC_EDIT_VALUE_W, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue3)
	ON_EN_CHANGE(IDC_EDIT_VALUE_A, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue4)
	ON_EN_CHANGE(IDC_EDIT_VALUE_F, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue5)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_R, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue0)	
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_G, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue1)	
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_B, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue2)	
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_W, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue3)	
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_A, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue4)	
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_F, &CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue5)	
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgDaliControlGearChildDT8_RGBWAF::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgDaliControlGearChildDT8_RGBWAF message handlers
BOOL CDlgDaliControlGearChildDT8_RGBWAF::OnInitDialog() {
	CDialog::OnInitDialog();
	this->initModel();
	this->initControls();
	this->PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

static void initSlider(CSliderCtrl* pCtrl,int maxValue,int ticFreq) {
	pCtrl->SetRange(0, maxValue);
	pCtrl->SetTicFreq(ticFreq);
}

void CDlgDaliControlGearChildDT8_RGBWAF::initControlsStaticChannels() {
	if (MODE_COLOR_TYPE_PRIMARY_N == this->m_modeColorType) {		
		CWnd* pWnd = NULL;
		pWnd = this->GetDlgItem(IDC_STATIC_RED);
		if (NULL != pWnd) {
			pWnd->SetWindowText("0:");
		}
		pWnd = this->GetDlgItem(IDC_STATIC_GREEN);
		if (NULL != pWnd) {
			pWnd->SetWindowText("1:");
		}
		pWnd = this->GetDlgItem(IDC_STATIC_BLUE);
		if (NULL != pWnd) {
			pWnd->SetWindowText("2:");
		}
		pWnd = this->GetDlgItem(IDC_STATIC_WHITE);
		if (NULL != pWnd) {
			pWnd->SetWindowText("3:");
		}
		pWnd = this->GetDlgItem(IDC_STATIC_AMBER);
		if (NULL != pWnd) {
			pWnd->SetWindowText("4:");
		}
		pWnd = this->GetDlgItem(IDC_STATIC_FREECOLOR);
		if (NULL != pWnd) {
			pWnd->SetWindowText("5:");
		}
	}
}

void CDlgDaliControlGearChildDT8_RGBWAF::initControls() {

	if (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) {	
		this->initControlComboColorPresets();
	}

	// Weergave controls f(mode)
	CWnd* const pWnd = this->GetDlgItem(IDC_STATIC_PRESET);
	if (NULL != pWnd) {
		pWnd->ShowWindow(MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType);
	}
	this->m_comboPresets.ShowWindow(MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType);

	// CColorPreviewStatic -> toont een preview van het geselecteerd kleur. 
	m_staticColorPreview.SubclassDlgItem(IDC_STATIC_PREVIEW, this);
	m_staticColorPreview.ShowWindow(MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? TRUE : FALSE);

/*
	// Weergave van het aantal channels f(ColorFeatures).
	this->initControlsEnableChannels();
*/

	int sliderValueMax = RGBWAF_MASK_VALUE;
	int sliderValueTick = 5;

	if (MODE_COLOR_TYPE_PRIMARY_N == this->m_modeColorType) {
		int sliderValueMax = PRIMARY_CHANNEL_N_MASK_VALUE;
		int sliderValueTick = 100;
	}

	// Slider value.
	initSlider(&this->m_sliderValue0, sliderValueMax, sliderValueTick);
	initSlider(&this->m_sliderValue1, sliderValueMax, sliderValueTick);
	initSlider(&this->m_sliderValue2, sliderValueMax, sliderValueTick);
	initSlider(&this->m_sliderValue3, sliderValueMax, sliderValueTick);
	initSlider(&this->m_sliderValue4, sliderValueMax, sliderValueTick);
	initSlider(&this->m_sliderValue5, sliderValueMax, sliderValueTick);

	this->initControlsStaticChannels();

	// Tooltips
	this->EnableToolTips(TRUE);

	CString strInfo;
	if (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) {	
		strInfo = _T("RGBWAF color channels.\r\n"
					 "Values range 0-254, 255=Mask/Ignore values");
	}
	else if (MODE_COLOR_TYPE_PRIMARY_N == this->m_modeColorType) {
		strInfo = _T("Primary N color channels.\r\n"
					 "Values range 0-65536, Mask/Ignore values");
	}
	this->m_infoText.SetWindowText(strInfo);
}

void CDlgDaliControlGearChildDT8_RGBWAF::updateControlsEnableChannels() {

	const BOOL enableControls = this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE;

	this->m_sliderValue5.EnableWindow(enableControls && (this->m_nrOfChannels >= 6));
	this->m_editValue5.EnableWindow(enableControls && (this->m_nrOfChannels >= 6));

	this->m_sliderValue4.EnableWindow(enableControls && (this->m_nrOfChannels >= 5));
	this->m_editValue4.EnableWindow(enableControls && (this->m_nrOfChannels >= 5));

	this->m_sliderValue3.EnableWindow(enableControls && (this->m_nrOfChannels >= 4));
	this->m_editValue3.EnableWindow(enableControls && (this->m_nrOfChannels >= 4));

	if (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) {
		this->m_sliderValue2.EnableWindow(enableControls);
		this->m_editValue2.EnableWindow(enableControls);

		this->m_sliderValue1.EnableWindow(enableControls);
		this->m_editValue1.EnableWindow(enableControls);	
	}
	else {
		this->m_sliderValue2.EnableWindow(enableControls && (this->m_nrOfChannels >= 3));
		this->m_editValue2.EnableWindow(enableControls && (this->m_nrOfChannels >= 3));

		this->m_sliderValue1.EnableWindow(enableControls && (this->m_nrOfChannels >= 2));
		this->m_editValue1.EnableWindow(enableControls && (this->m_nrOfChannels >= 2));
	}

	this->m_sliderValue0.EnableWindow(enableControls);
	this->m_editValue0.EnableWindow(enableControls);
}

void CDlgDaliControlGearChildDT8_RGBWAF::initControlComboColorPresets() {
	using namespace DUOTECNO::MFC_HELPER;
	const int count = CAppDaliObjectDT8ColorSpace_Utils::getColorTableNrOfEntries();
	for (int i=0;i<count;++i) {
		const CAppDaliObjectDT8ColorSpace_Utils::ColorTableEntry* entry = &CAppDaliObjectDT8ColorSpace_Utils::m_colorTable[i];
		CComboxbox_util::setComboBoxItem(this->m_comboPresets,entry->name, i);
	}
	CComboxbox_util::setComboBoxSetHeight(&this->m_comboPresets);
}

void CDlgDaliControlGearChildDT8_RGBWAF::initModel() {
	this->m_busyEditingValue = FALSE;
	this->m_busyUpdatingControls = FALSE;
	this->updateColorData(FALSE);
}

void CDlgDaliControlGearChildDT8_RGBWAF::updateControlEditBox(CEdit* pCtrl,int value) {
	REQUIRE(NULL != pCtrl);
	CString strValue;

	if (value < 0) {
		strValue = STRING_MASK_VALUE;
	}
	else if ((MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) && (value>= RGBWAF_MASK_VALUE)) {
		strValue = STRING_MASK_VALUE;		
	}
	else if (value >= PRIMARY_CHANNEL_N_MASK_VALUE) {
			strValue = STRING_MASK_VALUE;
	}		
	else {
		strValue.Format("%d", value);
	}
	pCtrl->SetWindowText(strValue);
}

static int clampValue(int v,int maxValue) {
	if ((v < 0) || (v > maxValue)) {
		v = maxValue;
	}
	return v;
}

LONG CDlgDaliControlGearChildDT8_RGBWAF::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	int maxValue = RGBWAF_MASK_VALUE;
	if (MODE_COLOR_TYPE_PRIMARY_N == this->m_modeColorType) {
		maxValue = PRIMARY_CHANNEL_N_MASK_VALUE;
	}
	for (int i=0;i<MAX_NR_CHANNELS;++i) {
		this->m_values[i] = clampValue(this->m_values[i], maxValue);
	}

	const BOOL enableControls = this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE;

	this->m_busyUpdatingControls = TRUE;

	// Update slider value
	this->m_sliderValue0.SetPos(this->m_values[0]);
	this->m_sliderValue1.SetPos(this->m_values[1]);
	this->m_sliderValue2.SetPos(this->m_values[2]);
	this->m_sliderValue3.SetPos(this->m_values[3]);
	this->m_sliderValue4.SetPos(this->m_values[4]);
	this->m_sliderValue5.SetPos(this->m_values[5]);

	// Update edit box.
	this->updateControlEditBox(&this->m_editValue0,this->m_values[0]);
	this->updateControlEditBox(&this->m_editValue1,this->m_values[1]);
	this->updateControlEditBox(&this->m_editValue2,this->m_values[2]);
	this->updateControlEditBox(&this->m_editValue3,this->m_values[3]);
	this->updateControlEditBox(&this->m_editValue4,this->m_values[4]);
	this->updateControlEditBox(&this->m_editValue5,this->m_values[5]);

	// f(viewmode + nr of channels)
	this->updateControlsEnableChannels();

	// ComboBox presets.
	if (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) {
		int selectedItem = -1;
		const int count = CAppDaliObjectDT8ColorSpace_Utils::getColorTableNrOfEntries();
		for (int i=0;i<count;++i) {
			const CAppDaliObjectDT8ColorSpace_Utils::ColorTableEntry* entry = &CAppDaliObjectDT8ColorSpace_Utils::m_colorTable[i];
			if ((entry->r == this->m_values[ID_CHANNEL_RED]) && 
				(entry->g == this->m_values[ID_CHANNEL_GREEN]) && 
				(entry->b == this->m_values[ID_CHANNEL_BLUE])) {
				// Extra controle op de WAF: Wanneer deze ingesteld is op een andere waarde dan 0 of 255
				// dan is er een kleurverschil.			
				if ((this->m_values[ID_CHANNEL_WHITE] == 0 || this->m_values[ID_CHANNEL_WHITE] == 255) && 
					(this->m_values[ID_CHANNEL_AMBER] == 0 || this->m_values[ID_CHANNEL_AMBER] == 255) &&
					(this->m_values[ID_CHANNEL_FREECOLOR] == 0 || this->m_values[ID_CHANNEL_FREECOLOR] == 255)) 
				{
					selectedItem = i;
					break;
				}
			}
		}
		if (selectedItem >= 0) { 
			this->m_comboPresets.SetCurSel(selectedItem);
		}
		else {
			this->m_comboPresets.SetWindowText("Custom");
		}
		this->m_comboPresets.EnableWindow(enableControls);
	}

	// Bitmap met preview van de kleur.	
	this->updatePreviewImage();

	this->m_busyUpdatingControls = FALSE;
	return TRUE;
}

void CDlgDaliControlGearChildDT8_RGBWAF::updatePreviewImage( ) {

	// m_staticColorPreview.SetColor(RGB(this->m_values[ID_CHANNEL_RED], this->m_values[ID_CHANNEL_GREEN], this->m_values[ID_CHANNEL_BLUE]));	

	CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB rgbValues;
	rgbValues.r = this->m_values[ID_CHANNEL_RED];
	rgbValues.g = this->m_values[ID_CHANNEL_GREEN];
	rgbValues.b = this->m_values[ID_CHANNEL_BLUE];

	if (this->m_nrOfChannels == 4) {
		// RGBW			
		rgbValues = CAppDaliObjectDT8ColorSpace_Utils::applyWhite(rgbValues, this->m_values[ID_CHANNEL_WHITE]);			
	}
	else if (this->m_nrOfChannels >= 5) {
		// RGBWA(F)			
		rgbValues = CAppDaliObjectDT8ColorSpace_Utils::applyWhiteAmber(rgbValues, this->m_values[ID_CHANNEL_WHITE],this->m_values[ID_CHANNEL_AMBER]);			
	}
	else {	
		rgbValues = CAppDaliObjectDT8ColorSpace_Utils::normalize(rgbValues);
	}
	m_staticColorPreview.SetColor(RGB(rgbValues.r, rgbValues.g, rgbValues.b));
}

void CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue0(NMHDR *pNMHDR, LRESULT *pResult) {
	const int value = this->m_sliderValue0.GetPos();
	if (this->m_values[0] != value) {
		this->setFlagPropertyChanged();
		this->m_values[0] = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue1(NMHDR *pNMHDR, LRESULT *pResult) {
	const int value = this->m_sliderValue1.GetPos();
	if (this->m_values[1] != value) {
		this->setFlagPropertyChanged();
		this->m_values[1] = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue2(NMHDR *pNMHDR, LRESULT *pResult) {
	const int value = this->m_sliderValue2.GetPos();
	if (this->m_values[2] != value) {
		this->setFlagPropertyChanged();
		this->m_values[2] = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue3(NMHDR *pNMHDR, LRESULT *pResult) {
	const int value = this->m_sliderValue3.GetPos();
	if (this->m_values[3] != value) {
		this->setFlagPropertyChanged();
		this->m_values[3] = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue4(NMHDR *pNMHDR, LRESULT *pResult) {
	const int value = this->m_sliderValue4.GetPos();
	if (this->m_values[4] != value) {
		this->setFlagPropertyChanged();
		this->m_values[4] = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnNMReleasedcaptureSliderValue5(NMHDR *pNMHDR, LRESULT *pResult) {
	const int value = this->m_sliderValue5.GetPos();
	if (this->m_values[5] != value) {
		this->setFlagPropertyChanged();
		this->m_values[5] = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgDaliControlGearChildDT8_RGBWAF::onEnChangeValue(int channelNR) {
	if (this->m_busyUpdatingControls) {
		return;
	}
	this->m_busyEditingEditBoxNR = channelNR;
	this->m_busyEditingValue = TRUE;
	this->KillTimer(TIMER_ID_EDITBOX_VALUE);
	this->SetTimer(TIMER_ID_EDITBOX_VALUE, 2000, (TIMERPROC)NULL);
}

void CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue0() {
	this->onEnChangeValue(0);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue1() {
	this->onEnChangeValue(1);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue2() {
	this->onEnChangeValue(2);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue3() {
	this->onEnChangeValue(3);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue4() {
	this->onEnChangeValue(4);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnChangeValue5() {
	this->onEnChangeValue(5);
}

void CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue(int channelNR) {
	if (this->m_busyEditingValue) {
		this->KillTimer(TIMER_ID_EDITBOX_VALUE);
		this->GetEditBoxValue(channelNR);
	}
}

void CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue0() {
	this->OnEnKillfocusValue(0);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue1() {
	this->OnEnKillfocusValue(1);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue2() {
	this->OnEnKillfocusValue(2);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue3() {
	this->OnEnKillfocusValue(3);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue4() {
	this->OnEnKillfocusValue(4);
}
void CDlgDaliControlGearChildDT8_RGBWAF::OnEnKillfocusValue5() {
	this->OnEnKillfocusValue(5);
}

void CDlgDaliControlGearChildDT8_RGBWAF::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_EDITBOX_VALUE == nIDEvent) {
		ASSERT(this->m_busyEditingEditBoxNR>=0 && this->m_busyEditingEditBoxNR<MAX_NR_CHANNELS);
		this->GetEditBoxValue(this->m_busyEditingEditBoxNR);
		this->KillTimer(TIMER_ID_EDITBOX_VALUE);
	}	
}

int CDlgDaliControlGearChildDT8_RGBWAF::GetEditBoxValue(CEdit* pCtrl) {
	CString strValue;
    pCtrl->GetWindowText(strValue);

	int tempValue = 0;
	if (_stscanf_s(strValue, _T("%d"), &tempValue) == 1) {
		return tempValue;
	}
	return (this->MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? 255 : 0xffff);
}

void CDlgDaliControlGearChildDT8_RGBWAF::GetEditBoxValue(int channelNR) {	
	if (!this->m_busyEditingValue) {
		return;
	}
	this->m_busyEditingValue = FALSE;
	ASSERT(channelNR>=0 && channelNR<MAX_NR_CHANNELS);
	CEdit* pCtrl = NULL;
	if (0 == channelNR) {
		pCtrl = &this->m_editValue0;
	}
	else if (1 == channelNR) {
		pCtrl = &this->m_editValue1;
	}
	else if (2 == channelNR) {
		pCtrl = &this->m_editValue2;
	}
	else if (3 == channelNR) {
		pCtrl = &this->m_editValue3;
	}
	else if (4 == channelNR) {
		pCtrl = &this->m_editValue4;
	}
	else if (5 == channelNR) {
		pCtrl = &this->m_editValue5;
	}
	else {
		ASSERT(0);
		return;
	}

	ASSERT(NULL != pCtrl);
	const int value = this->GetEditBoxValue(pCtrl);
	if (value != this->m_values[channelNR]) {
		this->setFlagPropertyChanged();
		this->m_values[channelNR] = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearChildDT8_RGBWAF::OnCbnSelchangePresets() {
	if (MODE_COLOR_TYPE_PRIMARY_N == this->m_modeColorType) { 
		ASSERT(0);
		return;
	}

	const int selected = this->m_comboPresets.GetCurSel();
	if (selected < 0) {		
		ASSERT(selected >= 0);
		return;
	}	
	if (selected < CAppDaliObjectDT8ColorSpace_Utils::getColorTableNrOfEntries()) {	
		const CAppDaliObjectDT8ColorSpace_Utils::ColorTableEntry* entry = &CAppDaliObjectDT8ColorSpace_Utils::m_colorTable[selected];
		if (NULL != entry) {
			if (this->m_values[ID_CHANNEL_RED] != entry->r) {
				this->setFlagPropertyChanged();
				this->m_values[ID_CHANNEL_RED] = entry->r;
			}
			if (this->m_values[ID_CHANNEL_GREEN] != entry->g) {
				this->setFlagPropertyChanged();
				this->m_values[ID_CHANNEL_GREEN] = entry->g;
			}
			if (this->m_values[ID_CHANNEL_BLUE] != entry->b) {
				this->setFlagPropertyChanged();
				this->m_values[ID_CHANNEL_BLUE] = entry->b;
			}

			// Oppassen: Bij de presets moeten de WAF waarden allemaal op 0 plaatsen.
			// Anders zijn de RGB kleuren van de presets niet ok.
			if (0 != this->m_values[ID_CHANNEL_WHITE] || 0 != this->m_values[ID_CHANNEL_AMBER] || 0 != this->m_values[ID_CHANNEL_FREECOLOR]) {
				this->setFlagPropertyChanged();
				this->m_values[ID_CHANNEL_WHITE] = 0;
				this->m_values[ID_CHANNEL_AMBER] = 0;
				this->m_values[ID_CHANNEL_FREECOLOR] = 0;
			}
		}
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgDaliControlGearChildDT8_RGBWAF::PreTranslateMessage(MSG* pMsg) {
	if (NULL != pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
		CWnd* pWnd = GetFocus();
		if (pWnd) {
			const DWORD identifier = pWnd->GetDlgCtrlID();
			if (IDC_EDIT_VALUE_R == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValue0.SetFocus();
				return TRUE;
			}			
			else if (IDC_EDIT_VALUE_G == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValue1.SetFocus();
				return TRUE;
			}	
			else if (IDC_EDIT_VALUE_B == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValue2.SetFocus();
				return TRUE;
			}	
			else if (IDC_EDIT_VALUE_W == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValue3.SetFocus();
				return TRUE;
			}	
			else if (IDC_EDIT_VALUE_A == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValue4.SetFocus();
				return TRUE;
			}	
			else if (IDC_EDIT_VALUE_F == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValue5.SetFocus();
				return TRUE;
			}	
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgDaliControlGearChildDT8_RGBWAF::updateColorData(BOOL saveToProperties) {
	if (saveToProperties) {
		DUOTECNO::DALI::CAppDaliObjectDT8ColorData& refData = this->getData();

		if (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) {		
			refData.setColorType(DALI_DT8_COLOUR_TYPE_RGBWAF);
			refData.setColorDimLevelRed(this->m_values[ID_CHANNEL_RED]);
			refData.setColorDimLevelGreen(this->m_values[ID_CHANNEL_GREEN]);
			refData.setColorDimLevelBlue(this->m_values[ID_CHANNEL_BLUE]);
			refData.setColorDimLevelWhite(this->m_values[ID_CHANNEL_WHITE]);
			refData.setColorDimLevelAmber(this->m_values[ID_CHANNEL_AMBER]);
			refData.setColorDimLevelFreeColor(this->m_values[ID_CHANNEL_FREECOLOR]);
		}
		else {
			refData.setColorType(DALI_DT8_COLOUR_TYPE_PRIMARY_N);
			ASSERT(CAppDaliObjectDT8ColorData::MAX_PRIMARY_CHANNELS == MAX_NR_CHANNELS);
			for (int i=0;i<CAppDaliObjectDT8ColorData::MAX_PRIMARY_CHANNELS;++i) {
				refData.setColorDimLevelChannel(i,this->m_values[i]);	
			}
		}
	}
	else {
		const CAppDaliObjectDT8ColorData& refData = this->getData();
		this->clearFlagPropertyChanged();
		if (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) {		
			this->m_values[ID_CHANNEL_RED] = refData.getColorDimLevelRed();
			this->m_values[ID_CHANNEL_GREEN] = refData.getColorDimLevelGreen();
			this->m_values[ID_CHANNEL_BLUE] = refData.getColorDimLevelBlue();
			this->m_values[ID_CHANNEL_WHITE] = refData.getColorDimLevelWhite();
			this->m_values[ID_CHANNEL_AMBER] = refData.getColorDimLevelAmber();
			this->m_values[ID_CHANNEL_FREECOLOR] = refData.getColorDimLevelFreeColor();
		}
		else {
			ASSERT(CAppDaliObjectDT8ColorData::MAX_PRIMARY_CHANNELS == MAX_NR_CHANNELS);
			for (int i=0;i<MAX_NR_CHANNELS;++i) {
				this->m_values[i] = refData.getColorDimLevelChannel(i);
			}
		}		
	}
}

void CDlgDaliControlGearChildDT8_RGBWAF::clearData(BOOL saveToProperties) {
	int maskedValue = RGBWAF_MASK_VALUE;
	if (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType) {
		maskedValue = RGBWAF_MASK_VALUE;
	}
	else {
		maskedValue = PRIMARY_CHANNEL_N_MASK_VALUE;
	}
	for (int i=0;i<MAX_NR_CHANNELS;++i) {
		if (maskedValue != this->m_values[i]) {
			this->setFlagPropertyChanged();
			this->m_values[i] = maskedValue;
		}
	}
	if(saveToProperties) {		
		this->CDlgDaliControlGearChildDT8_RGBWAF::updateColorData(TRUE);
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
BOOL CDlgDaliControlGearChildDT8_RGBWAF::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR toolbarItemIdentifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		toolbarItemIdentifier = ::GetDlgCtrlID((HWND)toolbarItemIdentifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	const int maxValue = (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? 254 : 65535);
	switch(toolbarItemIdentifier) {
		case IDC_SLIDER_VALUE_R:
		case IDC_EDIT_VALUE_R: {
			const CString strChannel = (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? _T("Red") : _T("0"));
			this->m_strToolTipText.Format("Channel %s: Value range 0-%d, %d = Mask/Ignore", strChannel, maxValue, maxValue + 1);
			handled = TRUE;
		} break;
		case IDC_SLIDER_VALUE_G:
		case IDC_EDIT_VALUE_G: {
			const CString strChannel = (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? _T("Green") : _T("1"));
			this->m_strToolTipText.Format("Channel %s: Value range 0-%d, %d = Mask/Ignore", strChannel, maxValue, maxValue + 1);
			handled = TRUE;
		} break;
		case IDC_SLIDER_VALUE_B:
		case IDC_EDIT_VALUE_B: {
			const CString strChannel = (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? _T("Blue") : _T("2"));
			this->m_strToolTipText.Format("Channel %s: Value range 0-%d, %d = Mask/Ignore", strChannel, maxValue, maxValue + 1);
			handled = TRUE;
		} break;
		case IDC_SLIDER_VALUE_W:
		case IDC_EDIT_VALUE_W: {
			const CString strChannel = (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? _T("White") : _T("3"));
			this->m_strToolTipText.Format("Channel %s: Value range 0-%d, %d = Mask/Ignore", strChannel, maxValue, maxValue + 1);
			handled = TRUE;
		} break;
		case IDC_SLIDER_VALUE_A:
		case IDC_EDIT_VALUE_A: {
			const CString strChannel = (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? _T("Amber") : _T("4"));
			this->m_strToolTipText.Format("Channel %s: Value range 0-%d, %d = Mask/Ignore", strChannel, maxValue, maxValue + 1);
			handled = TRUE;
		} break;
		case IDC_SLIDER_VALUE_F:
		case IDC_EDIT_VALUE_F: {
			const CString strChannel = (MODE_COLOR_TYPE_RGBWAF == this->m_modeColorType ? _T("Freecolor") : _T("5"));
			this->m_strToolTipText.Format("Channel %s: Value range 0-%d, %d = Mask/Ignore", strChannel, maxValue, maxValue + 1);
			handled = TRUE;
		} break;
		case IDC_COMBO_PRESETS: {
			this->m_strToolTipText = _T("Select a color preset");
			handled = TRUE;
		} break;
		case IDC_STATIC_PREVIEW: {
			this->m_strToolTipText = _T("Color preview display");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
