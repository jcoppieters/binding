#include "stdafx.h"
#include "ConfigAlgViewBitmaps.h"

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

CConfigAlgViewBitmaps::CConfigAlgViewBitmaps(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView) { }

CConfigAlgViewBitmaps::~CConfigAlgViewBitmaps(void) { }

BOOL CConfigAlgViewBitmaps::Load(void) {
	return FALSE;
}

// Upload enkel toegelaten wnr een TFT V65 is.
BOOL CConfigAlgViewBitmaps::Upload(void)
{
	if (CNodeBitmapConfig_Util::getNodetype(&getCanNode())==NODEBITMAP_CONFIG_TYPE_V65) {
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadBitmaps(&getCanNode(),notifier);	
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigAlgViewBitmaps::UploadToAll(void) {
	return FALSE;
}

int CConfigAlgViewBitmaps::GetViewID(void) const {
	using namespace DUOTECNO::PLUGIN;
	return VIEW_BITMAPS;
}

BOOL CConfigAlgViewBitmaps::IsSupported(void) {
	BOOL fShowWindow = FALSE;
	CCanNode& refNode=getCanNode();
	if (CNodeBitmapConfig_Util::isSupported(&refNode)) {	
		const NodeBitmapConfigtype type = CNodeBitmapConfig_Util::getNodetype(&refNode);		
		if ((NODEBITMAP_CONFIG_TYPE_V65==type)||(NODEBITMAP_CONFIG_TYPE_V66==type)) {
			fShowWindow = true;
		}	
	}
	return fShowWindow;
}

BOOL CConfigAlgViewBitmaps::IsLoadSupported(void) {
	return FALSE;
}

BOOL CConfigAlgViewBitmaps::IsFileFound(void)
{
	CString szFindFileName;
	CCanNode& refNode=getCanNode();
	BOOL result = FALSE;

	const NodeBitmapConfigtype type = CNodeBitmapConfig_Util::getNodetype(&refNode);
	if (NODEBITMAP_CONFIG_TYPE_V65==type) {
		szFindFileName.Format("%s\\config\\Bitmaps_%02x\\*.bgb",
							  TMGetFileSettings()->GetFullyQualifiedPathName(),
							  refNode.getNodeAddress());

		if (CFileFindFunctions::getCount(szFindFileName)>0) {
			result = TRUE;
		}
	}
	return result;
}

// Enkel V65 nodes.
BOOL CConfigAlgViewBitmaps::IsUploadCanSupported(void)
{	
	BOOL result=FALSE;
	CCanNode& refNode=getCanNode();
	const NodeBitmapConfigtype type = CNodeBitmapConfig_Util::getNodetype(&refNode);
	if (NODEBITMAP_CONFIG_TYPE_V65==type) {
		result=TRUE;
	}
	return result;
}

BOOL CConfigAlgViewBitmaps::IsDownloadCanSupported(void) {
	return FALSE;
}

BOOL CConfigAlgViewBitmaps::IsUploadCanAllSupported(void) {
	return FALSE;
}

const char* CConfigAlgViewBitmaps::getName(void)
{
	const char* s = "";
	CCanNode& refNode=getCanNode();
	const NodeBitmapConfigtype type = CNodeBitmapConfig_Util::getNodetype(&refNode);
	if (NODEBITMAP_CONFIG_TYPE_V65==type) {
		s="Bitmaps config";
	}
	else if (NODEBITMAP_CONFIG_TYPE_V66==type) {
		s="Graphics && sounds config";
	}
	return s;
}

BOOL CConfigAlgViewBitmaps::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewBitmaps::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

// OPPASSEN: CAN FTP (is niet aan FTP via ethernet)
BOOL CConfigAlgViewBitmaps::IsUseFTPProtocol(void)
{
	BOOL result=FALSE;
	CCanNode& refNode=getCanNode();
	const NodeBitmapConfigtype type = CNodeBitmapConfig_Util::getNodetype(&refNode);
	if (NODEBITMAP_CONFIG_TYPE_V65==type) {
		result=TRUE;
	}
	return result;
}
