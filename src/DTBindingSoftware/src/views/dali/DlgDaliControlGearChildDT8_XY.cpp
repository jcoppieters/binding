// DlgDaliControlGearChildDT8_XY.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDaliControlGearChildDT8_XY.h"
#include "app/appdali/appdali_common.h"
#include "app/appdali/appdaliprotocol_def.h"
#include "utils/mfc/combobox_util.h"

#include <cmath>  // Voor std::fabs
#include <algorithm> // Voor std::max

using namespace DUOTECNO::DALI;

// CDlgDaliControlGearChildDT8_XY dialog

IMPLEMENT_DYNAMIC(CDlgDaliControlGearChildDT8_XY, CDialog)


CDlgDaliControlGearChildDT8_XY::CDlgDaliControlGearChildDT8_XY(CWnd* pParent,CAppDaliObjectDT8ColorData* pSceneDT8ColorData)
	: Base(CDlgDaliControlGearChildDT8_XY::IDD, pParent,pSceneDT8ColorData), m_valueX(-1.0), m_valueY(-1.0), 
	  m_busyEditingValue(FALSE), m_busyUpdatingControls(FALSE)  { }


CDlgDaliControlGearChildDT8_XY::~CDlgDaliControlGearChildDT8_XY() { }

void CDlgDaliControlGearChildDT8_XY::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRESETS, m_comboPresets);
	DDX_Control(pDX, IDC_SLIDER_VALUE_X, m_sliderValueX);
	DDX_Control(pDX, IDC_EDIT_VALUE_X, m_editValueX);
	DDX_Control(pDX, IDC_SLIDER_VALUE_Y, m_sliderValueY);
	DDX_Control(pDX, IDC_EDIT_VALUE_Y, m_editValueY);
	DDX_Control(pDX, IDC_STATIC_INFO, m_infoText);
}


BEGIN_MESSAGE_MAP(CDlgDaliControlGearChildDT8_XY, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CDlgDaliControlGearChildDT8_XY::OnUpdateControls)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_X, &CDlgDaliControlGearChildDT8_XY::OnNMReleasedcaptureSliderValueX)
	ON_EN_CHANGE(IDC_EDIT_VALUE_X, &CDlgDaliControlGearChildDT8_XY::OnEnChangeValueX)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_X, &CDlgDaliControlGearChildDT8_XY::OnEnKillfocusValueX)	
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE_Y, &CDlgDaliControlGearChildDT8_XY::OnNMReleasedcaptureSliderValueY)
	ON_EN_CHANGE(IDC_EDIT_VALUE_Y, &CDlgDaliControlGearChildDT8_XY::OnEnChangeValueY)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_Y, &CDlgDaliControlGearChildDT8_XY::OnEnKillfocusValueY)	
	ON_CBN_SELCHANGE(IDC_COMBO_PRESETS,&CDlgDaliControlGearChildDT8_XY::OnCbnSelchangePresets)	
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgDaliControlGearChildDT8_XY::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgDaliControlGearChildDT8_XY message handlers
BOOL CDlgDaliControlGearChildDT8_XY::OnInitDialog() {
	CDialog::OnInitDialog();
	this->initModel();
	this->initControls();
	this->PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

void CDlgDaliControlGearChildDT8_XY::initControls() {

	this->initControlComboColorPresets();

	// Slider value.
	this->m_sliderValueX.SetRange(0, 0xFFFF);
	this->m_sliderValueX.SetTicFreq(1000);

	this->m_sliderValueY.SetRange(0, 0xFFFF);
	this->m_sliderValueY.SetTicFreq(1000);

	// Preview color.
	m_staticColorPreview.SubclassDlgItem(IDC_STATIC_PREVIEW, this);

	// Tooltips.
	this->EnableToolTips(TRUE);

	// Info text
	this->m_infoText.SetWindowText("Color select by XY Coordinates.\r\n"
								   "Values range 0 - 1.0, Mask=Ignore values\r\n"
								   "See CIE 1931 color space for more info.");
}

void CDlgDaliControlGearChildDT8_XY::initControlComboColorPresets() {
	// ComboBox items.
	using namespace DUOTECNO::MFC_HELPER;
	const int count = CAppDaliObjectDT8ColorSpace_Utils::getColorTableNrOfEntries();
	for (int i=0;i<count;++i) {
		const CAppDaliObjectDT8ColorSpace_Utils::ColorTableEntry* entry = &CAppDaliObjectDT8ColorSpace_Utils::m_colorTable[i];
		CComboxbox_util::setComboBoxItem(this->m_comboPresets,entry->name, i);
	}
	CComboxbox_util::setComboBoxSetHeight(&this->m_comboPresets);
}

void CDlgDaliControlGearChildDT8_XY::initModel() {
	this->m_busyEditingValue = FALSE;
	this->m_busyUpdatingControls = FALSE;
	this->updateColorData(FALSE);
}

static double clampXYValue(double v) {
	if (v >=1.0) {
		v = 1.0;
	}
	else if (v<0.0) {
		v = -1.0;
	}
	return v;
}

void CDlgDaliControlGearChildDT8_XY::updateControlEditBox(CEdit* pCtrl,double value) {
	REQUIRE(NULL != pCtrl);
	CString strValue;
	if (value < 0) {
		strValue = STRING_MASK_VALUE;
	}
	else {
		strValue.Format("%1.4f",value);
	}
	pCtrl->SetWindowText(strValue);
}

static bool checkAlmostEqual(double a, double b, double toleranceProcent = 5.0) {
    // Als beide exact nul zijn, zijn ze gelijk
    if (a == 0.0 && b == 0.0) return true;

    // Grootste absolute waarde bepaalt de referentie voor tolerantie
	double maxWaarde = max(std::fabs(a), std::fabs(b));

    // Bereken de maximale toegestane afwijking
    double maxDifference = maxWaarde * (toleranceProcent / 100.0);

    // Vergelijk het absolute verschil met de toegestane afwijking
    return std::fabs(a - b) <= maxDifference;
}

LONG CDlgDaliControlGearChildDT8_XY::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	const BOOL enableControls = this->getViewMode() == VIEWMODE_ONLINE ? TRUE : FALSE;

	this->m_valueX = clampXYValue(this->m_valueX);
	this->m_valueY = clampXYValue(this->m_valueY);

	this->m_busyUpdatingControls = TRUE;

	// Update slider value
	this->m_sliderValueX.SetPos(CAppDaliObjectDT8ColorSpace_Utils::convertFromXYValue(this->m_valueX));
	this->m_sliderValueY.SetPos(CAppDaliObjectDT8ColorSpace_Utils::convertFromXYValue(this->m_valueY));
	this->m_sliderValueX.EnableWindow(enableControls);
	this->m_sliderValueY.EnableWindow(enableControls);

	// Update edit box.
	this->updateControlEditBox(&this->m_editValueX,this->m_valueX);
	this->updateControlEditBox(&this->m_editValueY,this->m_valueY);
	this->m_editValueX.EnableWindow(enableControls);
	this->m_editValueY.EnableWindow(enableControls);

	// ComboBox presets.
	int selectedItem = -1;
	const int count = CAppDaliObjectDT8ColorSpace_Utils::getColorTableNrOfEntries();
	for (int i=0;i<count;++i) {
		const CAppDaliObjectDT8ColorSpace_Utils::ColorTableEntry* entry = &CAppDaliObjectDT8ColorSpace_Utils::m_colorTable[i];
		if (checkAlmostEqual(entry->x,this->m_valueX) && checkAlmostEqual(entry->y,this->m_valueY)) {	
			selectedItem = i;
			break;
		}
	}
	if (selectedItem >= 0) { 
		this->m_comboPresets.SetCurSel(selectedItem);
	}
	else {
		this->m_comboPresets.SetWindowText("Custom");
	}

	this->m_comboPresets.EnableWindow(enableControls);

	// Bitmap met preview van de kleur.	
	CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB rgb = CAppDaliObjectDT8ColorSpace_Utils::convert_xyToRGB(this->m_valueX,this->m_valueY);			
	m_staticColorPreview.SetColor(RGB(rgb.r, rgb.g, rgb.b));	

	this->m_busyUpdatingControls = FALSE;
	return TRUE;
}

void CDlgDaliControlGearChildDT8_XY::OnNMReleasedcaptureSliderValueX(NMHDR *pNMHDR, LRESULT *pResult) {
	const int sliderPos = this->m_sliderValueX.GetPos();
	const double value = CAppDaliObjectDT8ColorSpace_Utils::convertToXYValue(sliderPos);
	if (value != this->m_valueX) {
		this->m_valueX = value;
		this->setFlagPropertyChanged();
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgDaliControlGearChildDT8_XY::OnNMReleasedcaptureSliderValueY(NMHDR *pNMHDR, LRESULT *pResult) {
	const int sliderPos = this->m_sliderValueY.GetPos();
	const double value = CAppDaliObjectDT8ColorSpace_Utils::convertToXYValue(sliderPos);
	if (value != this->m_valueY) {
		this->m_valueY = value;
		this->setFlagPropertyChanged();
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgDaliControlGearChildDT8_XY::OnEnChangeValueX() {
	if (this->m_busyUpdatingControls) {
		return;
	}
	this->m_busyEditingValue = TRUE;
	this->KillTimer(TIMER_ID_EDITBOX_VALUE_X);
	this->SetTimer(TIMER_ID_EDITBOX_VALUE_X, 2000, (TIMERPROC)NULL);
}

void CDlgDaliControlGearChildDT8_XY::OnEnChangeValueY() {
	if (this->m_busyUpdatingControls) {
		return;
	}
	this->m_busyEditingValue = TRUE;
	this->KillTimer(TIMER_ID_EDITBOX_VALUE_Y);
	this->SetTimer(TIMER_ID_EDITBOX_VALUE_Y, 2000, (TIMERPROC)NULL);
}

void CDlgDaliControlGearChildDT8_XY::OnEnKillfocusValueX() {
	if (this->m_busyEditingValue) {
		this->KillTimer(TIMER_ID_EDITBOX_VALUE_X);
		this->GetEditBoxValueX();
	}
}
void CDlgDaliControlGearChildDT8_XY::OnEnKillfocusValueY() {
	if (this->m_busyEditingValue) {
		this->KillTimer(TIMER_ID_EDITBOX_VALUE_Y);
		this->GetEditBoxValueY();
	}
}

void CDlgDaliControlGearChildDT8_XY::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_EDITBOX_VALUE_X == nIDEvent) {
		this->GetEditBoxValueX();
		this->KillTimer(TIMER_ID_EDITBOX_VALUE_X);
	}
	else if (TIMER_ID_EDITBOX_VALUE_Y == nIDEvent) {
		this->GetEditBoxValueY();
		this->KillTimer(TIMER_ID_EDITBOX_VALUE_Y);
	}
}

double CDlgDaliControlGearChildDT8_XY::GetEditBoxValue(CEdit* pCtrl) {
	CString strValue;
    pCtrl->GetWindowText(strValue);

	double tempValue = 0.0;
	if (_stscanf_s(strValue, _T("%lf"), &tempValue) == 1) {
		return tempValue;
	}
	return -1.0;
}

void CDlgDaliControlGearChildDT8_XY::GetEditBoxValueX() {
	if (!this->m_busyEditingValue) {
		return;
	}
	this->m_busyEditingValue = FALSE;

	const double value = clampXYValue(this->GetEditBoxValue(&this->m_editValueX));	
	if (value != this->m_valueX) {
		this->setFlagPropertyChanged();
		this->m_valueX = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearChildDT8_XY::GetEditBoxValueY() {
		if (!this->m_busyEditingValue) {
		return;
	}
	this->m_busyEditingValue = FALSE;

	const double value = clampXYValue(this->GetEditBoxValue(&this->m_editValueY));	
	if (value != this->m_valueY) {
		this->setFlagPropertyChanged();
		this->m_valueY = value;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearChildDT8_XY::OnCbnSelchangePresets() {
	int selected = this->m_comboPresets.GetCurSel();
	if (selected < 0) {		
		ASSERT(selected >= 0);
		return;
	}	
	if (selected < CAppDaliObjectDT8ColorSpace_Utils::getColorTableNrOfEntries()) {	
		const CAppDaliObjectDT8ColorSpace_Utils::ColorTableEntry* entry = &CAppDaliObjectDT8ColorSpace_Utils::m_colorTable[selected];
		if (this->m_valueX != entry->x) {
			this->setFlagPropertyChanged();
			this->m_valueX = entry->x;
		}
		if (this->m_valueY != entry->y) {
			this->setFlagPropertyChanged();
			this->m_valueY = entry->y;
		}		
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgDaliControlGearChildDT8_XY::PreTranslateMessage(MSG* pMsg) {
	if (NULL != pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
		CWnd* pWnd = GetFocus();
		if (pWnd) {
			const DWORD identifier = pWnd->GetDlgCtrlID();
			if (IDC_EDIT_VALUE_X == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValueX.SetFocus();
				return TRUE;
			}			
			else if (IDC_EDIT_VALUE_Y == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_sliderValueY.SetFocus();
				return TRUE;
			}		
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgDaliControlGearChildDT8_XY::updateColorData(BOOL saveToProperties) {
	if (saveToProperties) {
		DUOTECNO::DALI::CAppDaliObjectDT8ColorData& refData = this->getData();
		const int valueX = CAppDaliObjectDT8ColorSpace_Utils::convertFromXYValue(this->m_valueX);
		const int valueY = CAppDaliObjectDT8ColorSpace_Utils::convertFromXYValue(this->m_valueY);
		refData.setColorType(DALI_DT8_COLOUR_TYPE_XY_COORDINATE);
		refData.setCoordinateX(valueX);
		refData.setCoordinateY(valueY);
	}
	else {
		const CAppDaliObjectDT8ColorData& refData = this->getData();
		this->m_valueX = CAppDaliObjectDT8ColorSpace_Utils::convertToXYValue(refData.getCoordinateX());
		this->m_valueY = CAppDaliObjectDT8ColorSpace_Utils::convertToXYValue(refData.getCoordinateY());
		this->clearFlagPropertyChanged();
	}
}

void CDlgDaliControlGearChildDT8_XY::clearData(BOOL saveToProperties) {
	if (-1 != this->m_valueX) {
		this->m_valueX = -1;
		this->setFlagPropertyChanged();
	}

	if (-1 != this->m_valueY) {
		this->m_valueY = -1;
		this->setFlagPropertyChanged();
	}

	if(saveToProperties) {		
		this->CDlgDaliControlGearChildDT8_XY::updateColorData(TRUE);
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
BOOL CDlgDaliControlGearChildDT8_XY::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR toolbarItemIdentifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		toolbarItemIdentifier = ::GetDlgCtrlID((HWND)toolbarItemIdentifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(toolbarItemIdentifier) {
		case IDC_SLIDER_VALUE_X: 
		case IDC_SLIDER_VALUE_Y:
		case IDC_EDIT_VALUE_Y:
		case IDC_EDIT_VALUE_X: {
			this->m_strToolTipText = _T("Adjust color XY values (Range: 0.0 - 1.0) or use Mask/Ignore");			   
			handled = TRUE;				   
		} break;
		case IDC_COMBO_PRESETS: {
			this->m_strToolTipText = _T("Select a color preset value");
			handled = TRUE;	
		} break;
		case IDC_STATIC_PREVIEW: {
			this->m_strToolTipText = _T("Preview the selected color");		
			handled = TRUE;					 
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
