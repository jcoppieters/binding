// QHSMGenericTask.h: interface for the CQHSMGenericTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QHSMGENERICTASK_H__9E6871B8_3218_4A4B_BFF0_DA8075A064A4__INCLUDED_)
#define AFX_QHSMGENERICTASK_H__9E6871B8_3218_4A4B_BFF0_DA8075A064A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//class CQHSMApplicationGlobal;


#include "QHSMApplicationGlobal.h"

typedef BOOL (CQHSMApplicationGlobal::*CQHSMTaskAction) ();
typedef void (CQHSMApplicationGlobal::*CQHSMTaskNotify) ();


class CQHSMGenericTask  
{
protected:

	CQHSMTaskAction Task_Start;
	CQHSMTaskAction Task_Stop;
	CQHSMTaskAction Task_Cancel;

	CQHSMTaskNotify Notify_TaskDone;
	CQHSMTaskNotify Notify_TaskError;
	CQHSMTaskNotify Notify_TaskCancelled;

	CString szHeader;
	CString szTask;
	BOOL fBtnCancelEnable;

public:
	CQHSMGenericTask();
	virtual ~CQHSMGenericTask();

public:

	static void Copy(CQHSMGenericTask* pDest,CQHSMGenericTask* const pSrc);
	
	void Set_Callback_Start(CQHSMTaskAction action);
	void Set_Callback_Stop(CQHSMTaskAction action);
	void Set_Callback_Cancel(CQHSMTaskAction action);
	
	void Set_Callback_NotifyDone(CQHSMTaskNotify notify);
	void Set_Callback_NotifyError(CQHSMTaskNotify notify);
	void Set_Callback_NotifyCancelled(CQHSMTaskNotify notify);


	void SetHeader(CString s);
	void SetMessage(CString s);
	void SetEnableCancel();
	BOOL IsCancelEnabled();

	void Clear();

	BOOL Start(CQHSMApplicationGlobal *pInvoke);
	BOOL Cancel(CQHSMApplicationGlobal *pInvoke);

	BOOL NotifyOnError(CQHSMApplicationGlobal *pInvoke);
	BOOL NotifyOnDone(CQHSMApplicationGlobal *pInvoke);
	BOOL NotifyOnCancel(CQHSMApplicationGlobal *pInvoke);
};

#endif // !defined(AFX_QHSMGENERICTASK_H__9E6871B8_3218_4A4B_BFF0_DA8075A064A4__INCLUDED_)
