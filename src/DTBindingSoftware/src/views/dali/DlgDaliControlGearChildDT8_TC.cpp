// DlgDaliControlGearChildDT8_TC.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDaliControlGearChildDT8_TC.h"
#include "app/appdali/appdali_common.h"
#include "app/appdali/appdaliprotocol_def.h"
#include "utils/mfc/combobox_util.h"

using namespace DUOTECNO::DALI;

// CDlgDaliControlGearChildDT8_TC dialog
const int CDlgDaliControlGearChildDT8_TC::VALUE_KELVIN_MAX = CAppDaliObjectDT8ColorSpace_Utils::DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MAX;
const int CDlgDaliControlGearChildDT8_TC::VALUE_KELVIN_MIN = CAppDaliObjectDT8ColorSpace_Utils::DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MIN;
const int CDlgDaliControlGearChildDT8_TC::VALUE_KELVIN_MASK = CAppDaliObjectDT8ColorSpace_Utils::DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MAX+1;

IMPLEMENT_DYNAMIC(CDlgDaliControlGearChildDT8_TC, CDialog)

CDlgDaliControlGearChildDT8_TC::CDlgDaliControlGearChildDT8_TC(CWnd* pParent,CAppDaliObjectDT8ColorData* pSceneDT8ColorData)
	: Base(CDlgDaliControlGearChildDT8_TC::IDD, pParent,pSceneDT8ColorData), m_valueKelvin(-1), m_busyEditingValue(FALSE), 
	  m_busyUpdatingControls(FALSE)
{
}

CDlgDaliControlGearChildDT8_TC::~CDlgDaliControlGearChildDT8_TC()
{
}

void CDlgDaliControlGearChildDT8_TC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRESETS, m_comboPresets);
	DDX_Control(pDX, IDC_SLIDER_VALUE, m_sliderValue);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_editValue);
	DDX_Control(pDX, IDC_STATIC_INFO, m_infoText);
}

BEGIN_MESSAGE_MAP(CDlgDaliControlGearChildDT8_TC, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS, OnUpdateControls)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE, &CDlgDaliControlGearChildDT8_TC::OnNMReleasedcaptureSliderValue)
	ON_EN_CHANGE(IDC_EDIT_VALUE, &CDlgDaliControlGearChildDT8_TC::OnEnChangeValue)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE, &CDlgDaliControlGearChildDT8_TC::OnEnKillfocusValue)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_PRESETS,OnCbnSelchangePresets)	
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgDaliControlGearChildDT8_TC::OnToolTipNeedText)
END_MESSAGE_MAP()

BOOL CDlgDaliControlGearChildDT8_TC::OnInitDialog() {
	CDialog::OnInitDialog();
	this->initModel();
	this->initControls();
	this->PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

void CDlgDaliControlGearChildDT8_TC::initControls() {

	this->initControlComboColorPresets();

	// Slider value.
	this->m_sliderValue.SetRange(VALUE_KELVIN_MIN, VALUE_KELVIN_MASK);
	this->m_sliderValue.SetTicFreq(100);

	// Tooltips
	this->EnableToolTips(TRUE);

	this->m_infoText.SetWindowText("Cool/Warm light color Temperature TC.\r\n"
								   "Values range 200-20000 Kelvin, Mask=Ignore values");
}

void CDlgDaliControlGearChildDT8_TC::initControlComboColorPresets() {
	// ComboBox items.
	using namespace DUOTECNO::MFC_HELPER;
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Warm candle light (1000K)", 1000);
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Tungsten (2000K)", 2000);
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Household bulbs (3000K)", 3000);
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Natural light (4000K)", 4000);
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Daylight (5000K)", 5000);
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Cool White (6000K)", 6000);
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Cool Overcast (7000K)", 7000);
	CComboxbox_util::setComboBoxItem(this->m_comboPresets,"Cold sky blue (10000K)", 10000);	
	CComboxbox_util::setComboBoxSetHeight(&this->m_comboPresets);
}

void CDlgDaliControlGearChildDT8_TC::initModel() {
	this->m_busyEditingValue = FALSE;
	this->m_busyUpdatingControls = FALSE;
	this->updateColorData(FALSE);
}

LONG CDlgDaliControlGearChildDT8_TC::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	const BOOL enableControls = this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE;

	if (this->m_valueKelvin < 0 || this->m_valueKelvin >= VALUE_KELVIN_MASK) {
		this->m_valueKelvin = VALUE_KELVIN_MASK;
	}
	this->m_busyUpdatingControls = TRUE;

	// Update slider value
	this->m_sliderValue.SetPos(this->m_valueKelvin);
	this->m_sliderValue.EnableWindow(enableControls);

	// Update edit box.
	CString strValue;
	if (this->m_valueKelvin == VALUE_KELVIN_MASK) {
		strValue = STRING_MASK_VALUE;
	}
	else {
		strValue.Format("%d",this->m_valueKelvin);
	}
	this->m_editValue.SetWindowText(strValue);
	this->m_editValue.EnableWindow(enableControls);

	// Note: Onderstaande zorgt ervoor dat de weergave van de preset van combobox de dichtste waarde toont.
	// Dit is nodig bij HouseHold bulbs (3000K).
	// Na omrekening van de waarde naar Mirek is deze 3003K.
	// De preset wordt weergegeven wanneer de waarde max 10% afwijkt. (3000K: dus tussen 2070K en 3030K).
	using DUOTECNO::MFC_HELPER::CComboxbox_util;
	const int percentageDifference = 10;	// 10% afwijking
	if (FALSE == CComboxbox_util::setComboBoxItemSelect(this->m_comboPresets, this->m_valueKelvin, percentageDifference)) {	
		CString strValue;
		if (this->m_valueKelvin != VALUE_KELVIN_MASK) {
			strValue.Format("Custom (%dK)",this->m_valueKelvin);
		}
		else {
			strValue = "Masked";
		}
		this->m_comboPresets.SetWindowText(strValue);
	}
	this->m_comboPresets.EnableWindow(enableControls);

	this->m_busyUpdatingControls = FALSE;
	return TRUE;
}

void CDlgDaliControlGearChildDT8_TC::OnNMReleasedcaptureSliderValue(NMHDR *pNMHDR, LRESULT *pResult) {
	if (this->m_valueKelvin != this->m_sliderValue.GetPos()) {
		this->m_valueKelvin = this->m_sliderValue.GetPos();
		this->setFlagPropertyChanged();
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgDaliControlGearChildDT8_TC::OnEnChangeValue() {
	if (this->m_busyUpdatingControls) {
		return;
	}
	this->m_busyEditingValue = TRUE;
	this->KillTimer(TIMER_ID_EDITBOX_VALUE);
	this->SetTimer(TIMER_ID_EDITBOX_VALUE, 2000, (TIMERPROC)NULL);
}

void CDlgDaliControlGearChildDT8_TC::OnEnKillfocusValue() {
	if (this->m_busyEditingValue) {
		this->KillTimer(TIMER_ID_EDITBOX_VALUE);
		this->GetEditBoxValue();
	}
}

void CDlgDaliControlGearChildDT8_TC::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_EDITBOX_VALUE == nIDEvent) {
		this->GetEditBoxValue();
		this->KillTimer(TIMER_ID_EDITBOX_VALUE);
	}
}

void CDlgDaliControlGearChildDT8_TC::GetEditBoxValue() {
	if (!this->m_busyEditingValue) {
		return;
	}
	this->m_busyEditingValue = FALSE;

	CString strValue;
	int newValue = VALUE_KELVIN_MASK;
	m_editValue.GetWindowText(strValue);
	if (0 == strValue.Compare(STRING_MASK_VALUE)) {
		newValue = VALUE_KELVIN_MASK;
	}
	else {
		const int iValue = atoi((LPCTSTR)strValue);
		if (iValue < VALUE_KELVIN_MIN) {
			newValue = VALUE_KELVIN_MASK;
		}
		else if (iValue > VALUE_KELVIN_MAX) {
			newValue = VALUE_KELVIN_MASK;
		}
		else {
			newValue = iValue;
		}
	}
	if (newValue != this->m_valueKelvin) {
		this->m_valueKelvin = newValue;
		this->setFlagPropertyChanged();
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearChildDT8_TC::OnCbnSelchangePresets() {
	int selected = this->m_comboPresets.GetCurSel();
	if (selected < 0) {		
		ASSERT(selected >= 0);
		return;
	}	
	const int value = this->m_comboPresets.GetItemData(selected);
	if (value != this->m_valueKelvin) {
		this->m_valueKelvin = this->m_comboPresets.GetItemData(selected);
		this->setFlagPropertyChanged();
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgDaliControlGearChildDT8_TC::PreTranslateMessage(MSG* pMsg) {
	if (NULL != pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
		CWnd* pWnd = GetFocus();
		if (pWnd) {
			const DWORD identifier = pWnd->GetDlgCtrlID();
			if (IDC_EDIT_VALUE == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValue.SetFocus();
				return TRUE;
			}			
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgDaliControlGearChildDT8_TC::updateColorData(BOOL saveToProperties) {
	if (saveToProperties) {
		DUOTECNO::DALI::CAppDaliObjectDT8ColorData& refData = this->getData();
		const int mirekValue = CAppDaliObjectDT8ColorSpace_Utils::convertFromKelvin2Mirek(this->m_valueKelvin);
		refData.setColorType(DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC);
		refData.setColorTemperature(mirekValue);
	}
	else {
		const CAppDaliObjectDT8ColorData& refData = this->getData();
		const int mirekValue = refData.getColorTemperature();
		this->m_valueKelvin = CAppDaliObjectDT8ColorSpace_Utils::convertFromMirek2Kelvin(mirekValue);
		this->clearFlagPropertyChanged();
	}
}

void CDlgDaliControlGearChildDT8_TC::clearData(BOOL saveToProperties) {
	if (VALUE_KELVIN_MASK != this->m_valueKelvin) {
		this->setFlagPropertyChanged();
		this->m_valueKelvin = VALUE_KELVIN_MASK;
	}
	if(saveToProperties) {		
		this->CDlgDaliControlGearChildDT8_TC::updateColorData(TRUE);
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
BOOL CDlgDaliControlGearChildDT8_TC::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR toolbarItemIdentifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		toolbarItemIdentifier = ::GetDlgCtrlID((HWND)toolbarItemIdentifier);  // Haal ID van de control op
	}

	BOOL handled = FALSE;
	switch(toolbarItemIdentifier) {
		case IDC_SLIDER_VALUE:
		case IDC_EDIT_VALUE: {						
			this->m_strToolTipText = _T("Color temperature (TC) in Kelvin (Range: 200-20000K, Mask/Ignore)");
			handled = TRUE;
		} break;
		case IDC_COMBO_PRESETS: {
			this->m_strToolTipText = _T("Select a color temperature preset value");	
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
