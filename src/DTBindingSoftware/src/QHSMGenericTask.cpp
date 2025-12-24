// QHSMGenericTask.cpp: implementation of the CQHSMGenericTask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "QHSMGenericTask.h"
#include "ApplicationDlgAppearance.h"

#include "views/apptasks/DlgMainApplicationTask.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQHSMGenericTask::CQHSMGenericTask()
{
	Clear();
}

CQHSMGenericTask::~CQHSMGenericTask()
{

}

//////////////////////////////////////////////////////////////////////

void CQHSMGenericTask::Set_Callback_Start(CQHSMTaskAction action) 
{
	Task_Start = action;
}

void CQHSMGenericTask::Set_Callback_Stop(CQHSMTaskAction action) 
{
	Task_Stop = action;
}

void CQHSMGenericTask::Set_Callback_Cancel(CQHSMTaskAction action)
{
	Task_Cancel = action;
}

void CQHSMGenericTask::Set_Callback_NotifyDone(CQHSMTaskNotify notify)
{
	Notify_TaskDone = notify;
}

void CQHSMGenericTask::Set_Callback_NotifyError(CQHSMTaskNotify notify)
{
	Notify_TaskError = notify;
}

void CQHSMGenericTask::Set_Callback_NotifyCancelled(CQHSMTaskNotify notify)
{
	Notify_TaskCancelled = notify;
}

//////////////////////////////////////////////////////////////////////


void CQHSMGenericTask::SetHeader(CString s)
{
	szHeader = s;
}

void CQHSMGenericTask::SetMessage(CString s)
{
	szTask = s;
}

void CQHSMGenericTask::SetEnableCancel()
{
	fBtnCancelEnable = TRUE;
}

BOOL CQHSMGenericTask::IsCancelEnabled()
{
	return (fBtnCancelEnable == TRUE);
}
//////////////////////////////////////////////////////////////////////
void CQHSMGenericTask::Clear()
{
	Task_Start = NULL;
	Task_Stop = NULL;
	Task_Cancel = NULL;

	Notify_TaskDone = NULL;
	Notify_TaskError = NULL;
	Notify_TaskCancelled = NULL;

	szHeader.Empty();
	szTask.Empty();

	fBtnCancelEnable = FALSE;
}

BOOL CQHSMGenericTask::Start(CQHSMApplicationGlobal* pInvoke)
{
	if (Task_Start != NULL) 
	{
		(pInvoke->*Task_Start) ();		// Invoke Action Start Handler !!!!

		CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
		if (pDialog != 0)
		{
			CString szTemp = szTask + ": Busy";

			if (fBtnCancelEnable == TRUE) 
			{
				CApplicationDlgAppearance Appearance(szHeader, 
													 szTemp, 
													 TASK_PROGRESSBAR_NORMAL,
													 TASK_BUTTON_CANCEL);
				pDialog->SetModeTaskStatus(&Appearance);

			}
			else
			{
				CApplicationDlgAppearance Appearance(szHeader, 
													 szTemp, 
													 TASK_PROGRESSBAR_NORMAL,
													 TASK_BUTTON_INVISIBLE);	
				pDialog->SetModeTaskStatus(&Appearance);
			}
			
			return TRUE;	
		}	
	}
	return FALSE;
}

BOOL CQHSMGenericTask::Cancel(CQHSMApplicationGlobal* pInvoke)
{
	if (Task_Cancel != NULL)
	{
		(pInvoke->*Task_Cancel) ();		// Invoke Action Cancel Handler !!!!
	}	
	
	CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
	if (pDialog != 0)
	{
		CString szTemp = szTask + ": Cancelled by user";

		CApplicationDlgAppearance Appearance(szHeader, 
											 szTemp, 
											 TASK_PROGRESSBAR_ERROR,
											 TASK_BUTTON_CANCEL);
		
		pDialog->SetModeTaskStatus(&Appearance);
	
		return TRUE;	
	}
	return FALSE;
}

BOOL CQHSMGenericTask::NotifyOnCancel(CQHSMApplicationGlobal *pInvoke)
{
	if (Notify_TaskCancelled != NULL)
	{
		(pInvoke->*Notify_TaskCancelled) ();
	}

	CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
	if (pDialog != 0)
	{
		CApplicationDlgAppearance Appearance("Cancelled by user", 
											 "Action cancelled by user", 
											 TASK_PROGRESSBAR_ERROR,
											 TASK_BUTTON_DONE);
		
		pDialog->SetModeTaskStatus(&Appearance);
		return TRUE;

	}
	return FALSE;
}

BOOL CQHSMGenericTask::NotifyOnError(CQHSMApplicationGlobal* pInvoke)
{
	if (Notify_TaskError != NULL)
	{
		(pInvoke->*Notify_TaskError) ();
	}

	CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
	if (pDialog != 0)
	{
		CString szTemp = szTask + ": Error";

		CApplicationDlgAppearance Appearance(szHeader, 
											 szTemp, 
											 TASK_PROGRESSBAR_ERROR,
											 TASK_BUTTON_ERROR);
		
		pDialog->SetModeTaskStatus(&Appearance);
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMGenericTask::NotifyOnDone(CQHSMApplicationGlobal* pInvoke)
{
	if (Notify_TaskDone != NULL)
	{
		(pInvoke->*Notify_TaskDone) ();
	}

	CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
	if (pDialog != 0)
	{
		CString szTemp = szTask + ": Done";

		CApplicationDlgAppearance Appearance(szHeader, 
											 szTemp, 
											 TASK_PROGRESSBAR_DONE,
											 TASK_BUTTON_INVISIBLE);
		
		pDialog->SetModeTaskStatus(&Appearance);
		return TRUE;
	}
	return FALSE;
}

void CQHSMGenericTask::Copy(CQHSMGenericTask* pDest,CQHSMGenericTask* const pSrc)
{
	if (pDest != NULL && pSrc != NULL)
	{
		pDest->szHeader = pSrc->szHeader;
		pDest->szTask = pSrc->szTask;
		pDest->fBtnCancelEnable = pSrc->fBtnCancelEnable;

		pDest->Task_Cancel = pSrc->Task_Cancel;
		pDest->Task_Start = pSrc->Task_Start;
		pDest->Task_Stop = pSrc->Task_Stop;	

		pDest->Notify_TaskDone = pSrc->Notify_TaskDone;
		pDest->Notify_TaskError = pSrc->Notify_TaskError;
		pDest->Notify_TaskCancelled = pSrc->Notify_TaskCancelled;
	}
}


