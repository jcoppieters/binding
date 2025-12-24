#include "stdafx.h"
#include "ConfigAlgViewUnits.h"

#include "App\AppCfg\AppCfgUnits\configunitcreateinfo.h"
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

CConfigAlgViewUnits::CConfigAlgViewUnits(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView) { }

CConfigAlgViewUnits::~CConfigAlgViewUnits(void) { }

BOOL CConfigAlgViewUnits::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewUnits::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadUnitCreate(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewUnits::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewUnits::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_UNITS;
}

BOOL CConfigAlgViewUnits::IsSupported(void) {
	const BOOL fShowWindow = CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_UNITS_CREATE);
	return fShowWindow;
}

BOOL CConfigAlgViewUnits::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewUnits::IsFileFound(int nodeAddress) {
	return CFileFindFunctions::find(CConfigUnitCreateInfo::getInstance( ).GetFullPathName(nodeAddress));
}

BOOL CConfigAlgViewUnits::IsFileFound(void) {
	return IsFileFound(getCanNode().bLogicalAddress);
}

BOOL CConfigAlgViewUnits::IsUploadCanSupported(void)
{
	BOOL result=TRUE;
	// CHANGES_AV_UNIVERSAL 
	if (UNITSCREATE_TYPE_UNITCONFIG_IP == CNodeCreateDynUnits_Util::getNodeType(&getCanNode())) {
		result=FALSE;
	}
	return result;
}

BOOL CConfigAlgViewUnits::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewUnits::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewUnits::getName(void)
{
	return "Unit configuration";
}

BOOL CConfigAlgViewUnits::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewUnits::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewUnits::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}