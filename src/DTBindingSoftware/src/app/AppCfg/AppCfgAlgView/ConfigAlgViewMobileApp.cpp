#include "stdafx.h"
#include "ConfigAlgViewMobileApp.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"						
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	

#include "cannode.h"
/////////////////////////////////////////////////////////////////////////////

CConfigAlgViewMobileApp::CConfigAlgViewMobileApp(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{
}

CConfigAlgViewMobileApp::~CConfigAlgViewMobileApp(void)
{
}

BOOL CConfigAlgViewMobileApp::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMobileApp::Upload(void)
{	
	return FALSE;
}

BOOL CConfigAlgViewMobileApp::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewMobileApp::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_MOBILE_APP;
}

BOOL CConfigAlgViewMobileApp::IsSupported(void)
{
	return CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_MOBILE_APP);
}

BOOL CConfigAlgViewMobileApp::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMobileApp::IsFileFound(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMobileApp::IsUploadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMobileApp::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMobileApp::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewMobileApp::getName(void)
{
	return "Duotecno Pro App config";
}

BOOL CConfigAlgViewMobileApp::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewMobileApp::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewMobileApp::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}