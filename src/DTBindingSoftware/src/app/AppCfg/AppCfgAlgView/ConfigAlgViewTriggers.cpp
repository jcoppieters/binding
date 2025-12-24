#include "stdafx.h"
#include "ConfigAlgViewtriggers.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"						
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	

#include "cannode.h"
/////////////////////////////////////////////////////////////////////////////

// CHANGES_AV_UNIVERSAL
CConfigAlgViewTriggers::CConfigAlgViewTriggers(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{
}

CConfigAlgViewTriggers::~CConfigAlgViewTriggers(void)
{
}

BOOL CConfigAlgViewTriggers::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewTriggers::Upload(void)
{	
	return FALSE;
}

BOOL CConfigAlgViewTriggers::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewTriggers::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_TRIGGERS;
}

BOOL CConfigAlgViewTriggers::IsSupported(void)
{
	return CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_TRIGGERS);
}

BOOL CConfigAlgViewTriggers::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewTriggers::IsFileFound(void)
{
	return FALSE;
}

BOOL CConfigAlgViewTriggers::IsUploadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewTriggers::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewTriggers::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewTriggers::getName(void)
{
	return "Triggers and actions config";
}

BOOL CConfigAlgViewTriggers::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewTriggers::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewTriggers::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}