#include "stdafx.h"
#include "ConfigAlgViewMoods.h"

#include "App\AppCfg\AppCfgMoods\ConfigMoodInfo.h"
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

CConfigAlgViewMoods::CConfigAlgViewMoods(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView) , m_useHTTPUpload(FALSE) /*, m_httpInfoMessage() */ { 
	init(pNode);
}

CConfigAlgViewMoods::~CConfigAlgViewMoods(void) { }

BOOL CConfigAlgViewMoods::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMoods::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadMoods(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewMoods::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewMoods::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_MOODS;
}

BOOL CConfigAlgViewMoods::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_MOODS);

	return fShowWindow;
}

BOOL CConfigAlgViewMoods::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMoods::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigMoodInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewMoods::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewMoods::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMoods::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewMoods::getName(void)
{
	return "Mood configuration";
}

BOOL CConfigAlgViewMoods::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewMoods::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewMoods::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}
BOOL CConfigAlgViewMoods::IsReady(void)
{
	return CConfigMoodInfo::getInstance().isReady();
}

BOOL CConfigAlgViewMoods::IsSupportedHTTPProtocol(void) {
	BOOL result = FALSE;
	if (CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_MOODS)) {
		if (CNodeConfigSupportHttpInfo_Util::isSupported(&getCanNode(),CONFIG_SUPPORT_HTTP_MOODS)) {
			// m_httpInfoMessage = CString("Only use this option when the node is not connected to the Ethernet LAN network.");
			result = TRUE;
		}
	}
	return result;
}

BOOL CConfigAlgViewMoods::IsUseUploadHTTPProtocol(void) {
	return m_useHTTPUpload;
}

void CConfigAlgViewMoods::SetUseUploadHTTPProtocol(BOOL flag) {
	m_useHTTPUpload = flag;
}

/*
BOOL CConfigAlgViewMoods::getHttpInfoMessage(CString* pszMsg) const {
	if (m_httpInfoMessage.IsEmpty()) {
		return FALSE;
	}
	*pszMsg = m_httpInfoMessage;
	return TRUE;
}
*/

// Protected...
void CConfigAlgViewMoods::init(CCanNode* pNode) {
	m_useHTTPUpload = IsSupportedHTTPProtocol();	
}