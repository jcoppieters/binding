#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDaliControlGearChildDT8.h"
#include "app/AppDali/appdalidevices.h"

using namespace DUOTECNO::DALI;


const CString CDlgDaliControlGearChildDT8::STRING_MASK_VALUE = "MASK";

CDlgDaliControlGearChildDT8::CDlgDaliControlGearChildDT8(DWORD identifier,CWnd* pParent,CAppDaliObjectDT8ColorData* pSceneDT8ColorData) :
CDialog(identifier, pParent), m_pSceneDT8ColorData(pSceneDT8ColorData) ,m_flagPropertyChanged(FALSE)  {  }
	
CDlgDaliControlGearChildDT8::~CDlgDaliControlGearChildDT8() { }

DUOTECNO::DALI::CAppDaliObjectDT8ColorData& CDlgDaliControlGearChildDT8::getData() {
	ASSERT(NULL != this->m_pSceneDT8ColorData);
	return *this->m_pSceneDT8ColorData;
}
const DUOTECNO::DALI::CAppDaliObjectDT8ColorData& CDlgDaliControlGearChildDT8::getData() const {
	ASSERT(NULL != this->m_pSceneDT8ColorData);
	return *this->m_pSceneDT8ColorData;
}

void CDlgDaliControlGearChildDT8::updateDialog( ) {
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgDaliControlGearChildDT8::setViewMode(ViewMode_t mode) {
	if (this->m_viewMode != mode) {
		this->m_viewMode = mode;
		this->updateDialog();
	}
}

BOOL CDlgDaliControlGearChildDT8::updateProperties(int sceneNumber, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* properties) {
	if (NULL == properties) {
		REQUIRE(NULL != properties);
		return FALSE;
	}
	if (NULL == this->m_pSceneDT8ColorData) {
		INVARIANT(NULL != this->m_pSceneDT8ColorData);
		return FALSE;
	}

	// Dialoog data updaten naar m_pSceneDT8ColorData
	this->updateColorData(TRUE);

	// Properties updaten 
	properties->setDT8SceneColorInfo(sceneNumber, *this->m_pSceneDT8ColorData);
	return TRUE;
}

BOOL CDlgDaliControlGearChildDT8::getPropertyChanged() const {
	return this->m_flagPropertyChanged;
}

void CDlgDaliControlGearChildDT8::setFlagPropertyChanged() {
	this->m_flagPropertyChanged = TRUE;
}
void CDlgDaliControlGearChildDT8::clearFlagPropertyChanged() {
	this->m_flagPropertyChanged = FALSE;
}