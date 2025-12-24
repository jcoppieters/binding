#include "stdafx.h"
#include "ConfigAlgViewSonosSrc.h"

#include "App\AppCfg\AppCfgSonosSrc\configsonossrcitemsfileinfo.h"
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

CConfigAlgViewSonosSrc::CConfigAlgViewSonosSrc(
	CCanNode* pNode,
	IViewDisplay* pNotifyView) :
	Base(pNode,pNotifyView)
{

}

CConfigAlgViewSonosSrc::~CConfigAlgViewSonosSrc(void)
{

}

BOOL CConfigAlgViewSonosSrc::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSonosSrc::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadSonosSrcCfg(&getCanNode(),notifier);	
	return TRUE;
}

BOOL CConfigAlgViewSonosSrc::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewSonosSrc::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_SONOSSRCCFG;
}

BOOL CConfigAlgViewSonosSrc::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_SONOSSRC);

	return fShowWindow;
}

BOOL CConfigAlgViewSonosSrc::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSonosSrc::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const CString szFullPathName(
		CConfigSonosSrcItemsFileInfo::getInstance( ).
			GetFullPathName(bNodeAddress));

	return CFileFindFunctions::find(szFullPathName);
}

BOOL CConfigAlgViewSonosSrc::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewSonosSrc::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSonosSrc::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewSonosSrc::getName(void)
{
	return "Sonos src config";
}

BOOL CConfigAlgViewSonosSrc::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewSonosSrc::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewSonosSrc::IsUseFTPProtocol(void)
{
	// Gebruikt FTP commandos.
	return TRUE;
}