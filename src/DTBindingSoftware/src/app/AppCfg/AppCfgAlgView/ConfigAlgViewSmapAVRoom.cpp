#include "stdafx.h"
#include "ConfigAlgViewsmapavroom.h"
#include "nodescreenmapinfo.h"	

#include "App\AppCfg\AppCfgsmap\ConfigSmapVRoomInfo.h"

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

CConfigAlgViewSmapAVRoom::CConfigAlgViewSmapAVRoom(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView), m_useOnlyDuotecnoProApp(FALSE)
{
	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
	if (CNodeScreenmapInfo_Util::isOnlyProAppConfig(pNode)) {
		m_useOnlyDuotecnoProApp = TRUE;
	}
}

CConfigAlgViewSmapAVRoom::~CConfigAlgViewSmapAVRoom(void) { }

BOOL CConfigAlgViewSmapAVRoom::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapAVRoom::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadSmapAVRoom(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewSmapAVRoom::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewSmapAVRoom::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_SMAP_AVROOM;
}

BOOL CConfigAlgViewSmapAVRoom::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_SMAP_AVROOM);

	return fShowWindow;
}

BOOL CConfigAlgViewSmapAVRoom::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapAVRoom::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigSmapAVRoomInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewSmapAVRoom::IsUploadCanSupported(void)
{
	return !m_useOnlyDuotecnoProApp;		// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

BOOL CConfigAlgViewSmapAVRoom::IsDownloadCanSupported(void)
{
	return FALSE;
}

// Sedert V16.21: Upload to all niet geimplementeerd.
BOOL CConfigAlgViewSmapAVRoom::IsUploadCanAllSupported(void)
{
	return FALSE;
	//return !m_useOnlyDuotecnoProApp;		// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

const char* CConfigAlgViewSmapAVRoom::getName(void)
{
	return "Screenmap virtual rooms";
}

BOOL CConfigAlgViewSmapAVRoom::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewSmapAVRoom::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewSmapAVRoom::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}

// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
BOOL CConfigAlgViewSmapAVRoom::getInfoMessages(CStringArray* messages) {
	if (!m_useOnlyDuotecnoProApp) {
		ASSERT(0 != messages);
		return FALSE;
	}
	return CConfigAlgView_Util::getInfoStrings(messages, CConfigAlgView_Util::STRINGS_ID_SCREENMAP_PROAPP);
}