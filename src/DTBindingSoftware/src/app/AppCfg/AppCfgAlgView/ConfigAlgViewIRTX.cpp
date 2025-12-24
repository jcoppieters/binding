#include "stdafx.h"
#include "ConfigAlgViewirtx.h"

#include "App\AppCfg\AppCfgIRTX\ConfigIRTXFileInfo.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"				
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"					
#include "TMAlgoritmesCfg.h"
#include "cannode.h"

#include "utils/win32/win32filefind.h"
/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
using namespace TMCommunication;
/////////////////////////////////////////////////////////////////////////////

CConfigAlgViewIRTX::CConfigAlgViewIRTX(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{

}

CConfigAlgViewIRTX::~CConfigAlgViewIRTX(void)
{

}

BOOL CConfigAlgViewIRTX::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewIRTX::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadIRTX(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewIRTX::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewIRTX::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_IRTX;
}

BOOL CConfigAlgViewIRTX::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_IRTX);

	return fShowWindow;
}

BOOL CConfigAlgViewIRTX::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewIRTX::IsFileFound(void)
{
	const BOOL result = CFileFindFunctions::find(
		CConfigIRTXFileInfo::getInstance( ).GetFullPathName());

	return result;
}

BOOL CConfigAlgViewIRTX::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewIRTX::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewIRTX::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewIRTX::getName(void)
{
	return "IR TX config";
}

BOOL CConfigAlgViewIRTX::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewIRTX::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewIRTX::IsUseFTPProtocol(void)
{
	// Gebruikt FTP commandos.
	return TRUE;
}