#include "stdafx.h"
#include "ConfigAlgViewLedsCfg.h"

#include "app\AppCfg\AppCfgUiCtrl\configAppUiCtrlinfo.h"
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

CConfigAlgViewLedsCfg::CConfigAlgViewLedsCfg(
	CCanNode* pNode,
	IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{

}

CConfigAlgViewLedsCfg::~CConfigAlgViewLedsCfg(void)
{

}

BOOL CConfigAlgViewLedsCfg::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewLedsCfg::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadLedsCfg(&getCanNode(),notifier);	
	return TRUE;
}

BOOL CConfigAlgViewLedsCfg::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewLedsCfg::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_LEDSCFG;
}

BOOL CConfigAlgViewLedsCfg::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(
			&getCanNode(),
			EXTERNAL_CONFIG_LEDSCFG);

	return fShowWindow;
}

BOOL CConfigAlgViewLedsCfg::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewLedsCfg::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigAppUiCtrlInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewLedsCfg::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewLedsCfg::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewLedsCfg::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewLedsCfg::getName(void)
{
	return "Leds config";
}

BOOL CConfigAlgViewLedsCfg::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewLedsCfg::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewLedsCfg::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}