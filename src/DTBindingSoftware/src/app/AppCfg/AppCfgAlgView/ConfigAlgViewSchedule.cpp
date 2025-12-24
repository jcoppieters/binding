#include "stdafx.h"
#include "ConfigAlgViewschedule.h"

#include "App\AppCfg\AppCfgschedule\ConfigScheduleInfo.h"

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

CConfigAlgViewShedule::CConfigAlgViewShedule(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView), m_useHTTPUpload(FALSE) // , m_httpInfoMessage() 
{
	init(pNode);
}

CConfigAlgViewShedule::~CConfigAlgViewShedule(void) { }

BOOL CConfigAlgViewShedule::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewShedule::Upload(void)
{
	CNotifier notifier(*this);
	TMAlgoritmesCfg::TMUploadScheduling(&getCanNode(),notifier);	

	return TRUE;
}

BOOL CConfigAlgViewShedule::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewShedule::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_SCHEDULE;
}

BOOL CConfigAlgViewShedule::IsSupported(void)
{
	const BOOL fShowWindow = 
		CNodeExtConfigInfo_Util::isSupported(&getCanNode(),
		EXTERNAL_CONFIG_SCHEDULE);

	return fShowWindow;
}

BOOL CConfigAlgViewShedule::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewShedule::IsFileFound(void)
{
	const BYTE bNodeAddress = getCanNode().bLogicalAddress;
	const BOOL result = CFileFindFunctions::find(
		CConfigScheduleInfo::getInstance( ).GetFullPathName(bNodeAddress));

	return result;
}

BOOL CConfigAlgViewShedule::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewShedule::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewShedule::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewShedule::getName(void)
{
	return "Scheduling config";
}

BOOL CConfigAlgViewShedule::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewShedule::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewShedule::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}

BOOL CConfigAlgViewShedule::IsReady(void)
{
	return CConfigScheduleInfo::getInstance().isReady();
}

BOOL CConfigAlgViewShedule::IsSupportedHTTPProtocol(void) {
	BOOL result = FALSE;
	if (CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_MOODS)) {
		if (CNodeConfigSupportHttpInfo_Util::isSupported(&getCanNode(),CONFIG_SUPPORT_HTTP_MOODS)) {
			// m_httpInfoMessage = CString("Only use this option when the node is not connected to the Ethernet LAN network.");
			result = TRUE;
		}
	}
	return result;
}

BOOL CConfigAlgViewShedule::IsUseUploadHTTPProtocol(void) {
	return m_useHTTPUpload;
}

void CConfigAlgViewShedule::SetUseUploadHTTPProtocol(BOOL flag) {
	m_useHTTPUpload = flag;
}

/*
BOOL CConfigAlgViewShedule::getHttpInfoMessage(CString* pszMsg) const {
	if (m_httpInfoMessage.IsEmpty()) {
		return FALSE;
	}
	*pszMsg = m_httpInfoMessage;
	return TRUE;
}
*/

// Protected...
void CConfigAlgViewShedule::init(CCanNode* pNode) {
	m_useHTTPUpload = IsSupportedHTTPProtocol();	
}