#include "stdafx.h"
#include "ConfigAlgViewMM.h"

#include "App/AppCfg/AppCfgMM/ConfigMultiMediaInfo.h"
#include "App/AppCfg/AppCfgMM/ConfigMultiMediafile.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"						
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	

#include "utils/win32/ShellFunctions.h"					
#include "utils/win32/ShellCommands.h"	

#include "cannode.h"

#include "utils/win32/win32filefind.h"

/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
using namespace TMCommunication;
/////////////////////////////////////////////////////////////////////////////

CConfigAlgViewMM::CConfigAlgViewMM(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{

}

CConfigAlgViewMM::~CConfigAlgViewMM(void)
{

}

BOOL CConfigAlgViewMM::Load(void)
{
	BOOL result = FALSE;

	CConfigMultiMediaInfo& configInfo =
			*CConfigMultiMediaInfo::GetInstance( );

	if (configInfo.Load(&getCanNode()))
	{
		DisplayMessage("Multimedia config file load done");
		result = TRUE;		
	}
	else
	{			
		DisplayMessage("Multimedia config file load failed");
		DisplayErrors(*configInfo.getErrors());
	}		
	return result;
}

BOOL CConfigAlgViewMM::Upload(void)
{
	BOOL result = FALSE;

	CConfigMultiMediaInfo& configInfo =
			*CConfigMultiMediaInfo::GetInstance( );
	
	if (configInfo.Load(&getCanNode()))
	{
		ASSERT( configInfo.IsFileLoaded( ));
		
		const BYTE bNodeAddress = getCanNode().bLogicalAddress;
		if (CConfigMultiMediaInfo::GetInstance()->InitNodeEntries(FALSE,
													   TMGetDocumentNodes(),
													   bNodeAddress))
		{
			TMGetGlobalHSM()->EvOperationUploadMMConfig();	
			DisplayMessage("multimedia config file send...");
			result = TRUE;
		}
		else
		{
			DisplayMessage("Error: No nodes found.");
			result = FALSE;
		}
	}
	else
	{
		DisplayMessage("Multimedia config file load failed");	
		DisplayErrors(*configInfo.getErrors());
	}	
	return result;
}

BOOL CConfigAlgViewMM::UploadToAll(void)
{
	BOOL result = FALSE;

	CConfigMultiMediaInfo& configInfo =
			*CConfigMultiMediaInfo::GetInstance( );

	if (configInfo.Load(&getCanNode()))
	{
		ASSERT( configInfo.IsFileLoaded( ));

		configInfo.SendToAllNodes(TRUE);			
		DisplayMessage("Multimedia config file load done");

		if (configInfo.InitNodeEntries(TRUE,
						    TMGetDocumentNodes(),
							getCanNode().bLogicalAddress))
		{
			TMGetGlobalHSM()->EvOperationUploadMMConfig();			
			DisplayMessage("multimedia config file send to all ...");
			result = TRUE;
		}
		else
		{
			DisplayMessage("Error: No nodes found.");
		}
	}
	else
	{
		DisplayMessage("Multimedia config file load failed");
		DisplayErrors(*configInfo.getErrors());
	}
	return result;
}

int CConfigAlgViewMM::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;

	return VIEW_AVMM;
}

BOOL CConfigAlgViewMM::IsSupported(void)
{
	BOOL result = 
		CNodeExtConfigInfo_Util::isSupported(
			&getCanNode(),
			EXTERNAL_CONFIG_MULTIMEDIA);
	
	return result;
}

BOOL CConfigAlgViewMM::IsLoadSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewMM::IsFileFound(void)
{
	BOOL result = FALSE;

	result = CFileFindFunctions::find(
		CConfigMultiMediaInfo::GetInstance( )->GetFullPathName( ));

	return result;
}

BOOL CConfigAlgViewMM::IsUploadCanSupported(void)
{
	return TRUE;
}

BOOL CConfigAlgViewMM::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewMM::IsUploadCanAllSupported(void)
{
	return TRUE;
}

const char* CConfigAlgViewMM::getName(void)
{
	return "Multimedia config";
}

BOOL CConfigAlgViewMM::IsUseConfigurator(void)
{
	BOOL result = TRUE;
	const NodeMultimediaConfigType type = 
		CNodeMultimediaConfig_Util::getNodeType(&getCanNode());

	if (NODEMULTIMEDIA_CONFIG_TYPE_V64 == type)
	{
		result = FALSE;
	}

	return result;
}

BOOL CConfigAlgViewMM::getFullPathName(int idx,CString* pszFullPathName)
{
	BOOL result = FALSE;

	if (idx == 0)
	{
		*pszFullPathName = CConfigMultiMediaInfo::GetInstance()->GetFullPathName();
		result = TRUE;
	}
	return result;
}

BOOL CConfigAlgViewMM::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}

// zie BUG_SOLVE_0158
BOOL CConfigAlgViewMM::IsReady(void)
{
	BOOL result = TRUE;
	CConfigMultiMediaInfo* const pInfo = CConfigMultiMediaInfo::GetInstance();
	if (pInfo) {
		result = pInfo->isReady();
	}
	return result;
}