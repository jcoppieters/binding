#include "stdafx.h"
#include "ConfigAlgViewDALI.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"						
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	
#include "cannode.h"
#include "MyWindowMessages.h"
/////////////////////////////////////////////////////////////////////////////
CConfigAlgViewDALI::CConfigAlgViewDALI(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView) { }

CConfigAlgViewDALI::~CConfigAlgViewDALI(void) { }

BOOL CConfigAlgViewDALI::Load(void) {
	return FALSE;
}

BOOL CConfigAlgViewDALI::Upload(void) {	
	return FALSE;
}

BOOL CConfigAlgViewDALI::UploadToAll(void) {
	return FALSE;
}

int CConfigAlgViewDALI::GetViewID(void) const {
	using namespace DUOTECNO::PLUGIN;
	return VIEW_DALI;
}

BOOL CConfigAlgViewDALI::IsSupported(void) {
	return CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_DALI);
}

BOOL CConfigAlgViewDALI::IsLoadSupported(void) {
	return FALSE;
}

BOOL CConfigAlgViewDALI::IsFileFound(void) {
	return FALSE;
}

BOOL CConfigAlgViewDALI::IsUploadCanSupported(void) {
	return FALSE;
}

BOOL CConfigAlgViewDALI::IsDownloadCanSupported(void) {
	return FALSE;
}

BOOL CConfigAlgViewDALI::IsUploadCanAllSupported(void){
	return FALSE;
}

const char* CConfigAlgViewDALI::getName(void) {
	return "DALI-subbus config";
}

BOOL CConfigAlgViewDALI::IsUseConfigurator(void) {
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewDALI::getFullPathName(int idx,CString* pszFullPathName) {
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewDALI::IsUseFTPProtocol(void) {
	// Gebruikt External config commandos.
	return FALSE;
}

BOOL CConfigAlgViewDALI::openConfiguration(BYTE bNodeAddress) {
	CWnd* const pWnd = AfxGetMainWnd();
	if (NULL == pWnd) {
		ASSERT(NULL != pWnd);
		return FALSE;
	}
	if (!pWnd->PostMessage(WM_APPLICATION_OPEN_DALISUBBUS_CFG, bNodeAddress)) {
		return FALSE;
	}
	return TRUE;
}