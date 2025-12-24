// MemoryDumpMasterFSM.cpp: implementation of the CMemoryDumpMasterFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MemoryDumpMasterFSM.h"

// --------------------------
#include "CanNode.h"
#include "ComApplication.h"
#include "MemoryBitmapInfo.h"
#include "MemoryDumpInfo.h"

#include "utils/app/DisplayDevice.h"
// --------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryDumpMasterFSM::CMemoryDumpMasterFSM(IComApplication *pCom) :
pComApplication(pCom)
{	
	m_pMemoryDumpFSM = new CMemoryDumpFSM((IMemoryDumpMasterFSM*) this);
	ASSERT(m_pMemoryDumpFSM);

	MemoryDumpActions = NULL;
	pMemoryBitmapInfo = NULL;
	pMemoryDumpInfo = NULL;
	pMemoryDumpTaskInfo = NULL;

	//pSelNode = NULL;
	CurrentState = FSM_STATE_IDLE;
}

CMemoryDumpMasterFSM::~CMemoryDumpMasterFSM() 
{
	if (m_pMemoryDumpFSM != NULL) 
	{
		delete m_pMemoryDumpFSM;
		m_pMemoryDumpFSM = NULL;
	}

	ClearTasks();
}

// --------------------------------------------------------------------------

void CMemoryDumpMasterFSM::ClearTasks(void)
{
	if (MemoryDumpActions != NULL) 
	{	
		delete [] MemoryDumpActions;
		MemoryDumpActions = NULL;
	}

	iNrOfTasks = 0;
}


void CMemoryDumpMasterFSM::InitTasks()
{
	int iNrOfTasksToAllocate;
	BYTE bNodeAddress;

	ClearTasks();

	// First we must find out how much tasks ...
	iNrOfTasksToAllocate = 0;

	if (pMemoryDumpInfo!= NULL) 
	{
		bNodeAddress = pMemoryDumpInfo->GetAssignedNodeAddress();

		if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_DYNRAM)) iNrOfTasksToAllocate++;
		if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_STATRAM)) iNrOfTasksToAllocate++;
		if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_ALLBMP)) iNrOfTasksToAllocate++;		
		if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_AUDIOBMP)) iNrOfTasksToAllocate++;
		if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_DYNRAM)) iNrOfTasksToAllocate++;
		if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_STATRAM)) iNrOfTasksToAllocate++;
		if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_ALLBMP)) iNrOfTasksToAllocate++;		
		if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_AUDIOBMP)) iNrOfTasksToAllocate++;

		if (iNrOfTasksToAllocate != 0) {
	
			try {
		
				// Allocate memory for nr of Task Required ...
				MemoryDumpActions = new MEMORYDUMP_ACTIONS[iNrOfTasksToAllocate];

				// Init Tasks ...
				if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_STATRAM)) 
				{
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Uploading Static Ram"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_UploadStatRam;	// CHANGES_MVS_2008
				}

				if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_DYNRAM)) 
				{
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Uploading Dynamic Ram"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_UploadDynRam;	// CHANGES_MVS_2008
				}

				if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_ALLBMP)) 
				{
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Uploading All Bitmaps"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_UploadAllBitmaps;	// CHANGES_MVS_2008
				}
				
				if (pMemoryDumpInfo->IsUpload(MEMORYDUMPINFO_TYPE_AUDIOBMP)) 
				{
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Uploading Audio Bitmaps"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_UploadAudioBitmaps;	// CHANGES_MVS_2008
				}

				if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_STATRAM)) 
				{
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Downloading Static Ram"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_DownloadStatRam;		// CHANGES_MVS_2008
				}
				
				if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_DYNRAM)) 
				{
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Downloading Dynamic Ram"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_DownloadDynRam;		// CHANGES_MVS_2008
				}

				if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_ALLBMP)) 
				{ 
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Downloading All Bitmaps"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_DownloadAllBitmaps;	// CHANGES_MVS_2008
				}

				if (pMemoryDumpInfo->IsDownload(MEMORYDUMPINFO_TYPE_AUDIOBMP)) 
				{
					MemoryDumpActions[iNrOfTasks].szInfo.Format(_T("Downloading Audio Bitmaps"));
					MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
					MemoryDumpActions[iNrOfTasks].bBitmapIndex = 0xFF;
					MemoryDumpActions[iNrOfTasks++].Actions = (Action) &CMemoryDumpMasterFSM::Task_DownloadAudioBitmaps;	// CHANGES_MVS_2008
				}

				ASSERT(iNrOfTasks == iNrOfTasksToAllocate);
			}		
			catch (CMemoryException *e) 
			{		
				e->ReportError();
				e->Delete();
			}
		}
		else
		{
			AfxMessageBox("Error - Memory for tasks allocated = NULL");

			// TM 20060504 : Fire Error ??
		}
	}
	else
	{
		AfxMessageBox("Error - Memory for tasks allocated = NULL");

		// TM 20060504 : Fire Error ??
	}
}

// ------------------------------------------------------------

BOOL CMemoryDumpMasterFSM::Cancel(void) 
{
	FSM(FSM_EV_OPERATION_CANCEL);

	return TRUE;
}



BOOL CMemoryDumpMasterFSM::StartMemoryDump(CCanNode *pNode,CMemoryDumpInfo *pInfo)
{
	if (CurrentState != FSM_STATE_IDLE) return FALSE;

	iTaskIndex = 0;
	pMemoryDumpInfo = pInfo;
	MemoryNodeinfo.Init(pNode);
	InitTasks();

	if (iNrOfTasks == 0) 
	{
		Fire_OnEvTasksError();	// TM 20060504 !!!
		return FALSE;
	}

	// Clear TaskInfo ....
	pMemoryDumpInfo->ClearTaskInfo();

	if (m_pMemoryDumpFSM != NULL) 
	{
		m_pMemoryDumpFSM->InitNode(MemoryNodeinfo.GetNodeAddressInfo(),
								   pMemoryDumpInfo);
	}

	FSM(FSM_EV_OPERATION_START);
	return TRUE;
}

BOOL CMemoryDumpMasterFSM::StartUploadBitmaps(CCanNode *pNode,CMemoryBitmapInfo *pInfo)
{
	CString szAction;
	BYTE bNrOfBitmaps;
	int i,j;
	BYTE bNodeAddress,bNodeCount;
	int iNrOfTasksToAllocate;

	if (pInfo == NULL) return FALSE;
	if (pNode == NULL) return FALSE;
	if (CurrentState != FSM_STATE_IDLE) return FALSE;

	pMemoryBitmapInfo = pInfo;
	iTaskIndex = 0;
	MemoryNodeinfo.Init(pNode);

	// Here we must init the nr of Tasks ...
	bNodeCount = pInfo->Nodes.Count();	
	if (bNodeCount == 0) return FALSE;

	if (!pInfo->isSendToAll()) bNodeCount = 1;

	ClearTasks();
	bNrOfBitmaps = iNrOfTasksToAllocate = 0;
	bNrOfBitmaps = pInfo->GetNrOfStoredBitmaps();
	iNrOfTasksToAllocate = (int) (bNrOfBitmaps * bNodeCount);

	try 
	{
		szAction.Format("Uploading Bitmap");

		MemoryDumpActions = new MEMORYDUMP_ACTIONS[iNrOfTasksToAllocate];

		for (i=0,iNrOfTasks=0;i<bNodeCount;i++) 
		{
		
			//bNodeAddress = pNodeAddresses[i];
			bNodeAddress = pInfo->Nodes.Get(i);

			for (j=0;j<bNrOfBitmaps;j++) 
			{			
				MemoryDumpActions[iNrOfTasks].szInfo = szAction;
				MemoryDumpActions[iNrOfTasks].bNodeAddress = bNodeAddress;
				MemoryDumpActions[iNrOfTasks].bBitmapIndex = j;
				MemoryDumpActions[iNrOfTasks].Actions = (Action) &CMemoryDumpMasterFSM::Task_UploadBitmaps;			// CHANGES_MVS_2008	

				iNrOfTasks++;

			}
		}

		//ASSERT(t==NrOfTaskToAllocate);

		if (iNrOfTasks == 0) return FALSE;

		//if (NrOfTaskToAllocate ==0) return FALSE;

		if (m_pMemoryDumpFSM != NULL) 
			m_pMemoryDumpFSM->InitNode(MemoryNodeinfo.GetNodeAddressInfo(),NULL);

		FSM(FSM_EV_OPERATION_START);

		return TRUE;
	}
	
	catch (CMemoryException *e) {
	
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////

void CMemoryDumpMasterFSM::FSM(FSM_EVENTS e)
{
	BYTE bNodeAddress = 0xFF;

	switch (CurrentState)
	{
		case FSM_STATE_IDLE :
		{
			switch (e)
			{
				case FSM_EV_OPERATION_START:

					CDisplayDevice::Clear();

					//TRACE("FSM_STATE_IDLE - FSM_EV_OPERATION_START\n");
				
					if (MemoryDumpActions[iTaskIndex].Actions != NULL) 
					{
						bNodeAddress = MemoryDumpActions[iTaskIndex].bNodeAddress; 
						CurrentState = FSM_STATE_WAIT_STARTED;

						if (!(this ->*MemoryDumpActions[iTaskIndex].Actions) (bNodeAddress)) 
						{
							CurrentState = FSM_STATE_IDLE;
							Fire_OnEvTasksError();
						}
					}
					else
					{
						ASSERT(0);
						Fire_OnEvTasksError();
					}
					
				break;

				default:
				break;
			}			
		}break;

			
		case FSM_STATE_WAIT_STARTED :
		{
			switch (e)
			{
				case FSM_EV_SIGNAL_START_OK:
				{					
					//TRACE("FSM_STATE_WAIT_STARTED - FSM_EV_SIGNAL_START_OK\n");
					CurrentState = FSM_STATE_BUSY;
				} break;

				case FSM_EV_SIGNAL_DONE_ERROR:

					//TRACE("FSM_STATE_WAIT_STARTED - FSM_EV_SIGNAL_DONE_ERROR\n");
					
					if (++iTaskIndex < iNrOfTasks) 
					{		
						CDisplayDevice::Clear();

						if (MemoryDumpActions[iTaskIndex].Actions != NULL) 
						{
							bNodeAddress = MemoryDumpActions[iTaskIndex].bNodeAddress; 
							if ((this ->*MemoryDumpActions[iTaskIndex].Actions)(bNodeAddress)) 
							{
								CurrentState = FSM_STATE_WAIT_STARTED;
							}
							else 
							{
								CurrentState = FSM_STATE_IDLE;
								Fire_OnEvTasksError();
							}						
						}
						else
						{
							ASSERT(0);
							CurrentState = FSM_STATE_IDLE;
							Fire_OnEvTasksError();
						}
					}
					else
					{						
						CurrentState = FSM_STATE_IDLE;
						Fire_OnEvTasksError();
					}
				break;

				case FSM_EV_OPERATION_CANCEL :
				{
					CurrentState = FSM_STATE_IDLE;

					if (m_pMemoryDumpFSM != NULL) 
					{
						m_pMemoryDumpFSM->Cancel();
					}

					Fire_OnEvTasksCancelled();

				} break;

				default:
					//TRACE("FSM_STATE_WAIT_STARTED - DEFAULT\n");
				break;
			}
		}break;
	

		case FSM_STATE_BUSY :
		{
			switch (e)
			{
				case FSM_EV_SIGNAL_DONE_OK:
				
					//TRACE("FSM_STATE_BUSY - FSM_EV_SIGNAL_DONE_OK\n");
					
					if (++iTaskIndex < iNrOfTasks) 
					{

						CDisplayDevice::Clear();
						
						if (MemoryDumpActions[iTaskIndex].Actions != NULL) 
						{
							
							CurrentState = FSM_STATE_WAIT_STARTED;
							
							bNodeAddress = MemoryDumpActions[iTaskIndex].bNodeAddress; 

							if (!(this ->*MemoryDumpActions[iTaskIndex].Actions) (bNodeAddress))
							{
								CurrentState = FSM_STATE_IDLE;
								Fire_OnEvTasksError();
							}
						}
						else 
						{
							ASSERT(0);
							CurrentState = FSM_STATE_IDLE;
							Fire_OnEvTasksError();
						}
					}
					else
					{						
						CurrentState = FSM_STATE_IDLE;
						Fire_OnEvTasksDone();
					}

				break;

				case FSM_EV_SIGNAL_DONE_ERROR:

					//TRACE("FSM_STATE_BUSY - FSM_EV_SIGNAL_DONE_ERROR\n");
		
					if (++iTaskIndex < iNrOfTasks) 
					{			
						if (MemoryDumpActions[iTaskIndex].Actions != NULL) 
						{							

							CDisplayDevice::Clear();
							CurrentState = FSM_STATE_WAIT_STARTED;

							bNodeAddress = MemoryDumpActions[iTaskIndex].bNodeAddress; 							
							if (!(this ->*MemoryDumpActions[iTaskIndex].Actions) (bNodeAddress)) 
							{
								CurrentState = FSM_STATE_IDLE;
								Fire_OnEvTasksDone();
							}
						}
						else
						{
							ASSERT(0);
							CurrentState = FSM_STATE_IDLE;						
							Fire_OnEvTasksError();
						}
					}
					else
					{						
						CurrentState = FSM_STATE_IDLE;
						Fire_OnEvTasksError();
					}
				break;

				case FSM_EV_OPERATION_CANCEL :
				{
					CurrentState = FSM_STATE_IDLE;
					
					if (m_pMemoryDumpFSM != NULL) 
					{
						m_pMemoryDumpFSM->Cancel();
					}

					Fire_OnEvTasksCancelled();

				} break;

				default:				
				break;
			}		
		}break;
	}
}


// ----------------------------------------------------------------------
inline void CMemoryDumpMasterFSM::Start_AckTimeoutTimer(unsigned short usValue)
{
	if (pComApplication != NULL) 
	{
		pComApplication->StartTimer(TIMERID_MEMORYDUMP_ACKTIMEOUT,
									TIMER_MEMORYDUMP_ACKTIMEOUT_VALUE);							
	}
}

inline void CMemoryDumpMasterFSM::Stop_AckTimeoutTimer(void) 
{
	if (pComApplication != NULL) 
	{
		pComApplication->StopTimer(TIMERID_MEMORYDUMP_ACKTIMEOUT);	
	}
}

// -----------------------------------------------------------------------

inline void CMemoryDumpMasterFSM::NotifyOnEvTaskDone(void) 
{
	if (pMemoryDumpTaskInfo != NULL) 
	{
		pMemoryDumpTaskInfo->SetTaskState(MEMORYDUMPTASKINFO_STATE_DONE);
	}
	
	FSM(FSM_EV_SIGNAL_DONE_OK);
}

inline void CMemoryDumpMasterFSM::NotifyOnEvTaskError(void) 
{
	if (pMemoryDumpTaskInfo != NULL) 
	{
		pMemoryDumpTaskInfo->SetTaskState(MEMORYDUMPTASKINFO_STATE_ERROR);
	}
	
	FSM(FSM_EV_SIGNAL_DONE_ERROR);
}

inline void CMemoryDumpMasterFSM::NotifyOnEvTaskStarted(void) 
{
	if (pMemoryDumpTaskInfo != NULL) 
	{
		pMemoryDumpTaskInfo->SetTaskState(MEMORYDUMPTASKINFO_STATE_START);
	}

	FSM(FSM_EV_SIGNAL_START_OK);
}

inline void CMemoryDumpMasterFSM::NotifyOnEvTaskCancelled(void) 
{
	if (pMemoryDumpTaskInfo != NULL) 
	{
		pMemoryDumpTaskInfo->SetTaskState(MEMORYDUMPTASKINFO_STATE_CANCEL);
	}
	
	FSM(FSM_EV_SIGNAL_DONE_ERROR);
}

void CMemoryDumpMasterFSM::NotifyOnEvMemoryDumpFSMInfo(CString s) 
{	
	CString szInfo;

	if (iTaskIndex < iNrOfTasks) 
	{
		CString szTemp = MemoryDumpActions[iTaskIndex].szInfo;
		szInfo.Format(_T("Task %d/%d : %s : %s"),iTaskIndex+1,iNrOfTasks,szTemp,s);
	}
	else 
	{
		szInfo.Format(_T("Task %d/%d : %s"),iTaskIndex+1,iNrOfTasks,s);
	}

	CDisplayDevice::DisplayInfoMsg(szInfo);
}

// ----------------------------------------------------------------------
/*
void CMemoryDumpMasterFSM::Init(IComApplication *pCom) 
{
	ASSERT(pCom);
	pComApplication = pCom;
}
*/

// -----------------------------------------------------------------------
BOOL CMemoryDumpMasterFSM::Task_DownloadDynRam(BYTE bNodeAddress) 
{
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_DOWNLOAD,
													   MEMORYDUMPINFO_TYPE_DYNRAM);
	return (m_pMemoryDumpFSM->Start_DownloadDynRam(bNodeAddress));
}
BOOL CMemoryDumpMasterFSM::Task_DownloadStatRam(BYTE bNodeAddress){
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_DOWNLOAD,
													   MEMORYDUMPINFO_TYPE_STATRAM);
	return (m_pMemoryDumpFSM->Start_DownloadStatRam(bNodeAddress));
}
BOOL CMemoryDumpMasterFSM::Task_DownloadAllBitmaps(BYTE bNodeAddress){
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_DOWNLOAD,
													   MEMORYDUMPINFO_TYPE_ALLBMP);
	return (m_pMemoryDumpFSM->Start_DownloadAllBitmaps(bNodeAddress));
}
BOOL CMemoryDumpMasterFSM::Task_DownloadAudioBitmaps(BYTE bNodeAddress){
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_DOWNLOAD,
													   MEMORYDUMPINFO_TYPE_AUDIOBMP);
	return (m_pMemoryDumpFSM->Start_DownloadAudioBitmaps(bNodeAddress));
}
// ------
BOOL CMemoryDumpMasterFSM::Task_UploadDynRam(BYTE bNodeAddress){
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_UPLOAD,
													   MEMORYDUMPINFO_TYPE_DYNRAM);
	return (m_pMemoryDumpFSM->Start_UploadDynRam(bNodeAddress));
}
BOOL CMemoryDumpMasterFSM::Task_UploadStatRam(BYTE bNodeAddress){
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_UPLOAD,
													   MEMORYDUMPINFO_TYPE_STATRAM);
	return (m_pMemoryDumpFSM->Start_UploadStatRam(bNodeAddress));
}

BOOL CMemoryDumpMasterFSM::Task_UploadAllBitmaps(BYTE bNodeAddress)
{
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_UPLOAD,
													   MEMORYDUMPINFO_TYPE_ALLBMP);
	return (m_pMemoryDumpFSM->Start_UploadAllBitmaps(bNodeAddress));
}

BOOL CMemoryDumpMasterFSM::Task_UploadAudioBitmaps(BYTE bNodeAddress)
{
	pMemoryDumpTaskInfo = NULL;
	pMemoryDumpTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_UPLOAD,
													   MEMORYDUMPINFO_TYPE_AUDIOBMP);
	return (m_pMemoryDumpFSM->Start_UploadAudioBitmaps(bNodeAddress));
}

BOOL CMemoryDumpMasterFSM::Task_UploadBitmaps(BYTE bNodeAddress) 
{
	BOOL fResult;
	unsigned long ulMemoryStart;
	unsigned short usLength;
	BYTE *pbBitmapData;
	BYTE bitmapIndex;

	if (pMemoryBitmapInfo == NULL) return FALSE;

	pMemoryDumpTaskInfo = NULL;

	bitmapIndex = MemoryDumpActions[iTaskIndex].bBitmapIndex;

	fResult = pMemoryBitmapInfo->GetStoredBitmap(bitmapIndex,&ulMemoryStart,
												 &usLength,&pbBitmapData);
	if (fResult == FALSE) return (FALSE);
	
	return(m_pMemoryDumpFSM->Start_UploadBitmap(bNodeAddress,ulMemoryStart,usLength,pbBitmapData));
}

// ----------------------------------------------------------------------

inline void CMemoryDumpMasterFSM::Fire_OnEvTasksDone(void) 
{
	if (pComApplication != NULL) 
	{
		//pComApplication->NotifyMemoryDumpFSMDoneOk();
		pComApplication->NotifyOnTaskDone(COMAPPLICATION_TASK_ID_MEMORYDUMP_ENGINE);
	}
}

inline void CMemoryDumpMasterFSM::Fire_OnEvTasksError(void) 
{
	if (pComApplication != NULL) 
	{
		//pComApplication->NotifyMemoryDumpFSMDoneError();
		pComApplication->NotifyOnTaskError(COMAPPLICATION_TASK_ID_MEMORYDUMP_ENGINE);
	}
}

inline void CMemoryDumpMasterFSM::Fire_OnEvTasksCancelled(void) 
{
	if (pComApplication != NULL) 
	{
		//pComApplication->NotifyMemoryDumpFSMUserCancel();
		pComApplication->NotifyOnTaskCancelled(COMAPPLICATION_TASK_ID_MEMORYDUMP_ENGINE);
	}
}
// -----------------------------------------------------------------------
// Events that come from CComApplication ...

void CMemoryDumpMasterFSM::EvSignalStatusMemoryDumpStart(BYTE bNodeAddress) 
{
	m_pMemoryDumpFSM->EvSignalStatusMemoryDumpStart(bNodeAddress);
}

void CMemoryDumpMasterFSM::EvSignalStatusMemoryDumpWriteOk(BYTE bNodeAddress) 
{
	m_pMemoryDumpFSM->EvSignalStatusMemoryDumpWriteOk(bNodeAddress);
}

void CMemoryDumpMasterFSM::EvAfterMemoryDumpTimeout(void) 
{
	m_pMemoryDumpFSM->EvAfterMemoryDumpTimeout();
}

void CMemoryDumpMasterFSM::EvSignalStatusMemoryDumpReadOk(BYTE bNodeAddress,BYTE *pbData,BYTE bLength,BYTE bErrors) 
{
	m_pMemoryDumpFSM->EvSignalStatusMemoryDumpReadOk(bNodeAddress,pbData,bLength,bErrors);
}
// -----------------------------------------------------------------------
