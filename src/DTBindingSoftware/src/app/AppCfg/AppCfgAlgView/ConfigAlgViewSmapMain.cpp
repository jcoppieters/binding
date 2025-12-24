#include "stdafx.h"
#include "ConfigAlgViewsmapmain.h"
#include "nodescreenmapinfo.h"	
#include "app\appCfg\AppCfginfo\cfginfonode.h"				
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	
#include "TMAlgoritmesCfg.h"
#include "cannode.h"

/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
using namespace TMCommunication;
/////////////////////////////////////////////////////////////////////////////

CConfigAlgViewSmapMain::CConfigAlgViewSmapMain(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView), m_useOnlyDuotecnoProApp(FALSE)
{
	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
	if (CNodeScreenmapInfo_Util::isOnlyProAppConfig(pNode)) {
		m_useOnlyDuotecnoProApp = TRUE;
	}
}

CConfigAlgViewSmapMain::~CConfigAlgViewSmapMain(void) { }

BOOL CConfigAlgViewSmapMain::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapMain::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadSmapMain(&getCanNode(),notifier);	
	return TRUE;
}

BOOL CConfigAlgViewSmapMain::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewSmapMain::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_SMAP_MAIN;
}

BOOL CConfigAlgViewSmapMain::IsSupported(void)
{
	const BOOL fShowWindow = CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_SMAP_MAIN);
	return fShowWindow;
}

BOOL CConfigAlgViewSmapMain::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapMain::IsFileFound(void)
{
	// TODO - Implementeren.

	return TRUE;
}

BOOL CConfigAlgViewSmapMain::IsUploadCanSupported(void)
{
	return !m_useOnlyDuotecnoProApp;	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

BOOL CConfigAlgViewSmapMain::IsDownloadCanSupported(void)
{
	return FALSE;
}

// Sedert V16.21: Upload to all niet geimplementeerd.
BOOL CConfigAlgViewSmapMain::IsUploadCanAllSupported(void)
{
	return FALSE;
	// return !m_useOnlyDuotecnoProApp;	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

const char* CConfigAlgViewSmapMain::getName(void)
{
	return "Screenmap main page";
}

BOOL CConfigAlgViewSmapMain::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewSmapMain::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewSmapMain::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}

// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
BOOL CConfigAlgViewSmapMain::getInfoMessages(CStringArray* messages) {
	if (!m_useOnlyDuotecnoProApp) {
		ASSERT(0 != messages);
		return FALSE;
	}
	return CConfigAlgView_Util::getInfoStrings(messages, CConfigAlgView_Util::STRINGS_ID_SCREENMAP_PROAPP);
}