#include "stdafx.h"
#include "ConfigAlgViewRoomCtrlAv.h"

#include "app\AppCfg\AppCfgRoomCtrlAV\configRoomCtrlAVInfo.h"

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

CConfigAlgViewRoomCtrlAV::CConfigAlgViewRoomCtrlAV(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{

}

CConfigAlgViewRoomCtrlAV::~CConfigAlgViewRoomCtrlAV(void)
{

}

BOOL CConfigAlgViewRoomCtrlAV::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewRoomCtrlAV::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadRoomCtrlAudioCfg(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewRoomCtrlAV::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewRoomCtrlAV::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_ROOMCTRL_AUDIOCFG;
}

BOOL CConfigAlgViewRoomCtrlAV::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_ROOMCTRL_AUDIO);

	return fShowWindow;
}

BOOL CConfigAlgViewRoomCtrlAV::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewRoomCtrlAV::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigRoomCtrlAVInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewRoomCtrlAV::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewRoomCtrlAV::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewRoomCtrlAV::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewRoomCtrlAV::getName(void)
{
	return "Room A/V config";
}

BOOL CConfigAlgViewRoomCtrlAV::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewRoomCtrlAV::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewRoomCtrlAV::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}