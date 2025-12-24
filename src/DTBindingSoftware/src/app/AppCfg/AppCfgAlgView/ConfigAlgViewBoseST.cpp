#include "stdafx.h"
#include "ConfigAlgViewBoseST.h"

#include "app\AppCfg\AppCfgBoseST\configBoseSTCfginfo.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"					
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	

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

CConfigAlgViewBoseST::CConfigAlgViewBoseST(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{

}

CConfigAlgViewBoseST::~CConfigAlgViewBoseST(void)
{

}

BOOL CConfigAlgViewBoseST::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewBoseST::Upload(void)
{
	CNotifier notifier(*this);
	const char* szName = "Bose devices config";
	CCanNode& refNode=getCanNode();
	const BYTE bProductID=refNode.GetProductID();

	switch(bProductID)
	{
		case PRODUCT_ID_AVMATRIX_BOSEST:
		{
			szName = "Bose devices config";
		} break;
		case PRODUCT_ID_AVMATRIX_SONOS:
		{
			szName = "Sonos devices config";
		} break;
	}		

	TMAlgoritmesCfg::TMUploadBoseSTCfg(&refNode,szName,notifier);	

	return TRUE;
}

BOOL CConfigAlgViewBoseST::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewBoseST::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_BOSESTCFG;
}

BOOL CConfigAlgViewBoseST::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_BOSEST);

	return fShowWindow;
}

BOOL CConfigAlgViewBoseST::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewBoseST::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigBoseSTCfgInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewBoseST::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewBoseST::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewBoseST::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewBoseST::getName(void)
{
	const char* szName= "Bose Soundtouch config";
	CCanNode& refNode=getCanNode();
	const BYTE bProductID=refNode.GetProductID();

	switch(bProductID)
	{
		case PRODUCT_ID_AVMATRIX_BOSEST:
		{
			szName = "Bose Soundtouch config";
		} break;
		case PRODUCT_ID_AVMATRIX_SONOS:
		{
			szName = "Sonos devices config";
		} break;
	}		
	return szName;
}

BOOL CConfigAlgViewBoseST::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewBoseST::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewBoseST::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}