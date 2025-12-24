#include "stdafx.h"
#include "ConfigAlgViewgateway.h"

#include "app\AppCfg\AppCfgGatewayU\ConfigGatewayUInfo.h"

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

CConfigAlgViewGateway::CConfigAlgViewGateway(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{

}

CConfigAlgViewGateway::~CConfigAlgViewGateway(void)
{

}

BOOL CConfigAlgViewGateway::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewGateway::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadGatewayCfg(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewGateway::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewGateway::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_CONFIG_GATEWAY;
}

BOOL CConfigAlgViewGateway::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_GATEWAY);

	return fShowWindow;
}

BOOL CConfigAlgViewGateway::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewGateway::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigGatewayUInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewGateway::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewGateway::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewGateway::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewGateway::getName(void)
{
	return "Gateway config";
}

BOOL CConfigAlgViewGateway::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewGateway::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewGateway::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}