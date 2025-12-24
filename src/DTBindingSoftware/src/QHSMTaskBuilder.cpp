// QHSMTaskBuilder.cpp: implementation of the CQHSMTaskBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "QHSMTaskBuilder.h"
#include "QHSMGenericTask.h"
#include "QHSMCompositeTask.h"
#include "TMAlgoritmesCfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQHSMTaskBuilder::CQHSMTaskBuilder() { }
CQHSMTaskBuilder::~CQHSMTaskBuilder() { }
//////////////////////////////////////////////////////////////////////
BOOL CQHSMTaskBuilder::Build_DownloadBindings(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_DownloadBindings);			// CHANGES_MVS_2008	
		pTask->SetHeader("Downloading all bindings from the network");
		pTask->SetMessage("Downloading all bindings");
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMTaskBuilder::Build_UploadAllBindings(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{	
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_WriteAllBindings);			// CHANGES_MVS_2008
		pTask->SetHeader("Uploading all bindings to the network");
		pTask->SetMessage("Uploading all bindings");		
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskWriteBindingsDone);	// CHANGES_MVS_2008
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadModifiedBindings(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_WriteModifiedBindings);		// CHANGES_MVS_2008
		pTask->SetHeader("Uploading modified bindings to the network");
		pTask->SetMessage("Uploading modified bindings");

		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskWriteBindingsDone);	// CHANGES_MVS_2008
		
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_BrowseSingleNode(CQHSMGenericTask* pTask) {
	if (pTask != NULL) 
	{
		pTask->Clear();
	
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_BrowseSingleNode);		
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_BrowseSingleNode);		
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskBrowseSingleNodeFailed);
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskBrowseSingleNodeFailed);
		pTask->SetEnableCancel();
		pTask->SetHeader("Browse single node");
		pTask->SetMessage("Browse node and search for new units");		
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_BrowseAllPhysicalNodes(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_BrowseAllPhysicalNodes);		// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_BrowseAllPhysicalNodes);		// CHANGES_MVS_2008
		pTask->SetEnableCancel();

		pTask->SetHeader("Physical browse for nodes");
		pTask->SetMessage("Physical browse for nodes");	

		return TRUE;
	}
	return FALSE;
}


BOOL CQHSMTaskBuilder::Build_RequestForStates(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_RequestForStates);		// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_RequestForStates);	// CHANGES_MVS_2008

		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskRequestForStatesDone);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskRequestForStatesCancelled); // CHANGES_MVS_2008

		pTask->SetEnableCancel();		

		pTask->SetHeader("Request for states");
		pTask->SetMessage("Request for states");

		return TRUE;
	}	
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_RequestForStatesSingleNode(CQHSMGenericTask* pTask)
{
	if (pTask != NULL)  {
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_RequestForStatesSingleNode);		
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_RequestForStates);	
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskBrowseSingleNodeFailed);
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskBrowseSingleNodeFailed);

		// pTask->SetEnableCancel();	WORDT NIET OPGEVANGEN.

		pTask->SetHeader("Browse single node");
		pTask->SetMessage("Request for states");

		return TRUE;
	}	
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_AutomaticPlugAndPlay(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_AutomaticPnp);				// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_AutomaticPnp);			// CHANGES_MVS_2008
		pTask->SetEnableCancel();

		pTask->SetHeader("Automatic Plug and Play");
		pTask->SetMessage("Automatic Plug and Play");	
		
		return TRUE;
	}
	return FALSE;
}


BOOL CQHSMTaskBuilder::Build_UploadAllNames(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();
				
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadAllNames);		// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadAllNames);		// CHANGES_MVS_2008
		pTask->SetEnableCancel();
	
		pTask->SetHeader("Uploading names to the network");
		pTask->SetMessage("Uploading names to the network");

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadScreenmaps(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadLcdScreenmaps);		// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadLcdScreenmaps);	// CHANGES_MVS_2008

		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskUploadScreenmapsDone);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskUploadScreenmapsError);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskUploadScreenmapsCancelled);	// CHANGES_MVS_2008
		
		pTask->SetEnableCancel();

		pTask->SetHeader("Uploading LCD screenmaps to the network");
		pTask->SetMessage("Uploading LCD screenmaps");
		
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadBitmaps(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadBitmaps);			// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadBitmaps);			// CHANGES_MVS_2008

		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskUploadBitmapsDone);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskUploadBitmapsError);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskUploadBitmapsCancelled);	// CHANGES_MVS_2008

		pTask->SetEnableCancel();

		pTask->SetHeader("Uploading bitmaps to the network");
		pTask->SetMessage("Uploading bitmaps");

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_NicErrorsFsm(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_NicErrorsFsm);					// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_NicErrorsFsm);				// CHANGES_MVS_2008

		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskNicErrorsFsmDone);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskNicErrorsFsmError);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskNicErrorsFsmCancelled);	// CHANGES_MVS_2008

		pTask->SetEnableCancel();

		pTask->SetHeader("Requesting NIC-medium status from the network");
		pTask->SetMessage("Requesting NIC-medium status");

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadMultimediaConfig(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadMultimMediaConfig);		// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadMultimMediaConfig);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);		// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);		// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	// CHANGES_MVS_2008
		pTask->SetEnableCancel();

		pTask->SetHeader("Uploading multimedia configuration to the network");
		pTask->SetMessage("Uploading multimedia configuration");

		return TRUE;
	}
	return FALSE;
}

// sedert V16.43: Wanneer algoritme gedaan is een specifieke handler aanroepen.
BOOL CQHSMTaskBuilder::Build_UploadUnitsCreateConfig(CQHSMGenericTask* pTask) {
	if (pTask != NULL) {
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadUnitsConfig);				// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadUnitsConfig);				// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskUploadUnitsDone);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	// CHANGES_MVS_2008
		pTask->SetEnableCancel();

		pTask->SetHeader("Uploading units configuration to the network");
		pTask->SetMessage("Uploading units configuration");

		return TRUE;
	}
	return FALSE;
}

// sedert V16.47: CHANGES_UPLOAD_UNITS
// Unit configuratie taak aanmaken die uitgevoerd wordt voor de browse for nodes.
// Deze taak wordt uitgevoerd wanneer er een node vervangen werd waarvoor er unitconfiguratie moet doorgestuurd worden.
//
// Deze taak wordt iedere keer aangemaakt per node adres.
BOOL CQHSMTaskBuilder::Build_UploadUnitsCreateConfigReplaceNode(CQHSMGenericTask* pTask,int nodeAddress) {
	using namespace TMAlgoritmesCfg;
	if (pTask == NULL) {
		return FALSE;
	}	

	// Inladen van de configuratie voor deze node. Het ingeladen bestand wordt later gebruikt in de master FSM.
	if (FALSE == TMLoadFileUnitCreate(nodeAddress)) {
		return FALSE;
	}

	pTask->Clear();

	pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadUnitsConfig);				
	pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadUnitsConfig);				
	pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			
	pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			
	pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	
	pTask->SetEnableCancel();

	pTask->SetHeader("Uploading units configuration to the network");
	pTask->SetMessage("Uploading units configuration");

	return TRUE;	
}

BOOL CQHSMTaskBuilder::Build_UploadMoodConfig(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadMoodConfig);					// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadMoodConfig);				// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	// CHANGES_MVS_2008
		pTask->SetEnableCancel();

		pTask->SetHeader("Uploading moods configuration to the network");
		pTask->SetMessage("Uploading moods configuration");

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadIRRXConfig(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadIRRXConfig);					// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadIRRXConfig);				// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled); // CHANGES_MVS_2008
		pTask->SetEnableCancel();

		pTask->SetHeader("Uploading irrx configuration to the network");
		pTask->SetMessage("Uploading irrx configuration");

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadScheduleConfig(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadScheduleConfig);				// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadScheduleConfig);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	// CHANGES_MVS_2008
		pTask->SetEnableCancel();
		pTask->SetHeader("Uploading schedule configuration to the network");
		pTask->SetMessage("Uploading schedule configuration");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadMemory(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadMemory);				// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadMemory);			// CHANGES_MVS_2008
	
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpDone);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpError);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpCancelled);	// CHANGES_MVS_2008

		pTask->SetEnableCancel();

		pTask->SetHeader("Uploading memory to LCD's in network");
		pTask->SetMessage("Uploading memory to LCD's");

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_DownloadMemory(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_DownloadMemory);			// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_DownloadMemory);			// CHANGES_MVS_2008

		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpDone);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpError);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpCancelled);	// CHANGES_MVS_2008

		pTask->SetEnableCancel();

		pTask->SetHeader("Downloading memory from LCD's in network");
		pTask->SetMessage("Downloading memory from LCD's");

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_ftpConfig(CQHSMGenericTask* pTask)// since v0562
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_ftpConfig);				// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_ftpConfig);				// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);	// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	// CHANGES_MVS_2008
		pTask->SetEnableCancel();
		pTask->SetHeader("ftp configuration");
		pTask->SetMessage("ftp configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadSmapPageInfo(CQHSMGenericTask* pTask)	// since v0571
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadSmapPageInfo);				// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadSmapPageInfo);				// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	// CHANGES_MVS_2008
		pTask->SetEnableCancel();
		pTask->SetHeader("Screenmap configuration");
		pTask->SetMessage("Screenmap configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadSmapMenu(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadSmapMenu);					// CHANGES_MVS_2008
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadSmapMenu);					// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			// CHANGES_MVS_2008
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	// CHANGES_MVS_2008
		pTask->SetEnableCancel();
		pTask->SetHeader("Screenmap menu configuration");
		pTask->SetMessage("Screenmap menu configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadSmapAVRoom(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadSmapAVRoom);					
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadSmapAVRoom);					
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	
		pTask->SetEnableCancel();
		pTask->SetHeader("Screenmap AV-Room configuration");
		pTask->SetMessage("Screenmap AV-Room configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadGatewayCfg(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadGatewayCfg);					
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadGatewayCfg);					
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	
		pTask->SetEnableCancel();
		pTask->SetHeader("Gateway configuration");
		pTask->SetMessage("Gateway configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadRoomCtrlAVCfg(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();
		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadRoomCtrlAudioCfg);					
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadRoomCtrlAudioCfg);	
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskUploadRoomCtrlAudioCfg);		
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	
		pTask->SetEnableCancel();
		pTask->SetHeader("RoomController audio configuration");
		pTask->SetMessage("RoomController audio configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadBoseSTCfg(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadBoseSTCfg);					
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadBoseSTCfg);					
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	
		pTask->SetEnableCancel();
		pTask->SetHeader("Bose soundtouch audio configuration");
		pTask->SetMessage("Bose soundtouch audio configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadLedsCfg(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadLedsCfg);					
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadLedsCfg);					
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskConfigDone);			
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	
		pTask->SetEnableCancel();
		pTask->SetHeader("LEDS configuration");
		pTask->SetMessage("LEDS configuration busy");
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadSmapLite(CQHSMGenericTask* pTask)
{
	if (pTask != NULL) 
	{
		pTask->Clear();

		pTask->Set_Callback_Start(&CQHSMApplicationGlobal::Start_UploadSmapLiteCfg);					
		pTask->Set_Callback_Cancel(&CQHSMApplicationGlobal::Cancel_UploadSmapLiteCfg);					
		pTask->Set_Callback_NotifyDone(&CQHSMApplicationGlobal::Notify_OnTaskSmapLiteCfgDone);			
		pTask->Set_Callback_NotifyError(&CQHSMApplicationGlobal::Notify_OnTaskConfigError);			
		pTask->Set_Callback_NotifyCancelled(&CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled);	
		pTask->SetEnableCancel();
		pTask->SetHeader("Screenmapping");
		pTask->SetMessage("Screenmapping configuration busy");
		return TRUE;
	}
	return FALSE;	
}


// composite tasks

BOOL CQHSMTaskBuilder::Build_UploadProgrammationFile(CQHSMCompositeTask *pTasks)
{
	if (pTasks != NULL)
	{
		CQHSMGenericTask TaskWriteBindings;
		CQHSMGenericTask TaskWriteNames;	
		
		Build_UploadAllNames(&TaskWriteNames);
		Build_UploadAllBindings(&TaskWriteBindings);

		pTasks->Clear();
		pTasks->AddTask(&TaskWriteNames);
		pTasks->AddTask(&TaskWriteBindings);

		return TRUE;
	}

	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_ExtendedBrowseAllPhNodes(CQHSMCompositeTask *pTasks)
{
	if (pTasks != NULL)
	{
		CQHSMGenericTask TaskBrowsePhNodes;
		CQHSMGenericTask TaskRequestStates;	
		
		Build_BrowseAllPhysicalNodes(&TaskBrowsePhNodes);
		Build_RequestForStates(&TaskRequestStates);

		pTasks->Clear();
		pTasks->AddTask(&TaskBrowsePhNodes);
		pTasks->AddTask(&TaskRequestStates);

		return TRUE;
	}

	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadSmapMyHome(CQHSMCompositeTask *pTasks)
{
	if (pTasks != NULL)
	{
		CQHSMGenericTask TaskUploadSmapPageInfo;
		CQHSMGenericTask TaskUploadSmap;
		
		Build_UploadSmapPageInfo(&TaskUploadSmapPageInfo);
		Build_UploadScreenmaps(&TaskUploadSmap);

		pTasks->Clear();
		pTasks->AddTask(&TaskUploadSmapPageInfo);
		pTasks->AddTask(&TaskUploadSmap);

		return TRUE;
	}

	return FALSE;
}

BOOL CQHSMTaskBuilder::Build_UploadSmapMainPage(CQHSMCompositeTask *pTasks)
{
	if (pTasks != NULL)
	{
		CQHSMGenericTask TaskUploadSmapMenu;
		CQHSMGenericTask TaskUploadSmap;
		
		Build_UploadSmapMenu(&TaskUploadSmapMenu);
		Build_UploadScreenmaps(&TaskUploadSmap);

		pTasks->Clear();
		pTasks->AddTask(&TaskUploadSmapMenu);
		pTasks->AddTask(&TaskUploadSmap);

		return TRUE;
	}

	return FALSE;
}