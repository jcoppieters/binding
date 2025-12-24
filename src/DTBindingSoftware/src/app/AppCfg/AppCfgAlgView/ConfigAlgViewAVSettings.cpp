#include "stdafx.h"
#include "ConfigAlgViewavsettings.h"

#include "App\AppCfg\AppCfgAVSettings\ConfigAVSettings.h"

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

CConfigAlgViewAVSettings::CConfigAlgViewAVSettings(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView) { }

CConfigAlgViewAVSettings::~CConfigAlgViewAVSettings(void) { }

BOOL CConfigAlgViewAVSettings::Load(void) {
	return FALSE;
}

BOOL CConfigAlgViewAVSettings::Upload(void) {
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadAVSettings(&getCanNode(),notifier);	
	return TRUE;
}

BOOL CConfigAlgViewAVSettings::UploadToAll(void) {
	return FALSE;
}

int CConfigAlgViewAVSettings::GetViewID(void) const {
	using namespace DUOTECNO::PLUGIN;
	return VIEW_AVSETTINGS;
}

BOOL CConfigAlgViewAVSettings::IsSupported(void) {
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_AVSETTINGS);
	return fShowWindow;
}

BOOL CConfigAlgViewAVSettings::IsLoadSupported(void) {
	return FALSE;
}

BOOL CConfigAlgViewAVSettings::IsFileFound(void) 
{
	enum { MAX_FILENAME_LENGTH = 255 };
	BOOL result=FALSE;
	char szFileName[MAX_FILENAME_LENGTH];
	CCanNode& refNode = getCanNode( );
	if (true==CNodeAVSettingsConfig_Util::getFileNameLocal(&refNode,szFileName,sizeof(szFileName))) {
		CString szFullPathName;
		//CString szSubDir;
		CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
		//szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
		szFullPathName.Format("%s%s",szPathName,szFileName);	
		if (true==CFileFindFunctions::find((LPCSTR)szFullPathName)) {
			result=TRUE;
		}
	}
	return result;
}

BOOL CConfigAlgViewAVSettings::IsUploadCanSupported(void) {
	return TRUE;
}

BOOL CConfigAlgViewAVSettings::IsDownloadCanSupported(void) {
	return FALSE;
}

BOOL CConfigAlgViewAVSettings::IsUploadCanAllSupported(void) {
	return FALSE;
}

const char* CConfigAlgViewAVSettings::getName(void) {
	return "AV settings config";
}

BOOL CConfigAlgViewAVSettings::IsUseConfigurator(void) {
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewAVSettings::getFullPathName(int idx,CString* pszFullPathName) {
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewAVSettings::IsUseFTPProtocol(void) {
	// Gebruikt FTP commandos.
	return TRUE;
}