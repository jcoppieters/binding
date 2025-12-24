#include "stdafx.h"
#include "ConfigAlgViewirrx.h"

#include "App\AppCfg\AppCfgIRRX\configirrxinfo.h"

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

CConfigAlgViewIRRX::CConfigAlgViewIRRX(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{

}

CConfigAlgViewIRRX::~CConfigAlgViewIRRX(void)
{

}

BOOL CConfigAlgViewIRRX::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewIRRX::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadIRRX(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewIRRX::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewIRRX::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_IRRX;
}

BOOL CConfigAlgViewIRRX::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_IRRX);

	return fShowWindow;
}

BOOL CConfigAlgViewIRRX::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewIRRX::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigIRRXInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewIRRX::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewIRRX::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewIRRX::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewIRRX::getName(void)
{
	return "IR RX config";
}

BOOL CConfigAlgViewIRRX::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewIRRX::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewIRRX::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}