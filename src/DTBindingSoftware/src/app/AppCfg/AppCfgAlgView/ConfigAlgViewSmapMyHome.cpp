#include "stdafx.h"
#include "ConfigAlgViewsmapmyhome.h"
#include "nodescreenmapinfo.h"	
#include "App\AppCfg\AppCfgsmap\ConfigSmapPageInfoInfo.h"
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

CConfigAlgViewSmapMyHome::CConfigAlgViewSmapMyHome(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView), m_useOnlyDuotecnoProApp(FALSE)
{
	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
	if (CNodeScreenmapInfo_Util::isOnlyProAppConfig(pNode)) {
		m_useOnlyDuotecnoProApp = TRUE;
	}
}

CConfigAlgViewSmapMyHome::~CConfigAlgViewSmapMyHome(void) { }

BOOL CConfigAlgViewSmapMyHome::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapMyHome::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadSmapMyHome(&getCanNode(),notifier);	
	return TRUE;
}

BOOL CConfigAlgViewSmapMyHome::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewSmapMyHome::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_SMAP_MYHOME;
}

BOOL CConfigAlgViewSmapMyHome::IsSupported(void)
{
	const BOOL fShowWindow = CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_SMAP_MYHOME);
	return fShowWindow;
}

BOOL CConfigAlgViewSmapMyHome::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapMyHome::IsFileFound(void)
{
	// Doet enkel controle op "srcmap_%02x_names.txt" bestand.
	// Dit bestand wordt aangemaakt via de configurator.

	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(CConfigSmapPageInfoInfo::getInstance( ).GetFullPathName(bNodeAddress));
	return result;
}

BOOL CConfigAlgViewSmapMyHome::IsUploadCanSupported(void)
{
	return !m_useOnlyDuotecnoProApp;	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

BOOL CConfigAlgViewSmapMyHome::IsDownloadCanSupported(void)
{
	return FALSE;
}

// Sedert V16.21: Upload to all niet geimplementeerd.
BOOL CConfigAlgViewSmapMyHome::IsUploadCanAllSupported(void)
{
	return FALSE;
	//return !m_useOnlyDuotecnoProApp;	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

const char* CConfigAlgViewSmapMyHome::getName(void)
{
	return "Screenmap my home";
}

BOOL CConfigAlgViewSmapMyHome::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewSmapMyHome::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewSmapMyHome::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}

BOOL CConfigAlgViewSmapMyHome::IsReady(void)
{
	return CConfigSmapPageInfoInfo::getInstance().isReady();
}


// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
BOOL CConfigAlgViewSmapMyHome::getInfoMessages(CStringArray* messages) {
	if (!m_useOnlyDuotecnoProApp) {
		ASSERT(0 != messages);
		return FALSE;
	}
	return CConfigAlgView_Util::getInfoStrings(messages, CConfigAlgView_Util::STRINGS_ID_SCREENMAP_PROAPP);
}