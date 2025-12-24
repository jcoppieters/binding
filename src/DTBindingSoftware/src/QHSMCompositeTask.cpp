// QHSMCompositeTask.cpp: implementation of the CQHSMCompositeTask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "QHSMCompositeTask.h"

#include "QHSMGenericTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQHSMCompositeTask::CQHSMCompositeTask()
{
	Tasks.RemoveAll();
}

CQHSMCompositeTask::~CQHSMCompositeTask()
{
	Clear();
}
//////////////////////////////////////////////////////////////////////
void CQHSMCompositeTask::Clear()
{
	const int nCnt = Tasks.GetSize();
	CQHSMGenericTask *pDelete = NULL;
	for (int i=0;i<nCnt;i++) 
	{
		if ((pDelete = static_cast<CQHSMGenericTask*>(Tasks.GetAt(i))) != NULL)
		{
			delete pDelete;
			pDelete = NULL;
		}
	}
	Tasks.RemoveAll();
}
BOOL CQHSMCompositeTask::AllocateNrOfTasks(int i)
{
	Clear();
	Tasks.SetSize(i);
	nCurrentTask = 0;
	return TRUE;
}
BOOL CQHSMCompositeTask::AddTask(CQHSMGenericTask *pTask)
{
	CQHSMGenericTask* pNew = NULL;
	try
	{
		pNew = new CQHSMGenericTask();
		CQHSMGenericTask::Copy(pNew,pTask);
		Tasks.Add(pNew);
		return TRUE;
	}
	catch (CMemoryException *e) 
	{
		e->ReportError();
		e->Delete();	
	}
	return FALSE;
}
int CQHSMCompositeTask::GetNrOfTasks()
{
	return (Tasks.GetSize());
}
CQHSMGenericTask* CQHSMCompositeTask::Begin()
{
	nCurrentTask = 0;
	return (GetTask(nCurrentTask));
}
CQHSMGenericTask* CQHSMCompositeTask::Next()
{
	nCurrentTask++;
	return (GetTask(nCurrentTask));
}
CQHSMGenericTask* CQHSMCompositeTask::GetTask(int index)
{
	if (index <= Tasks.GetUpperBound()) {
		return (static_cast<CQHSMGenericTask*>(Tasks.GetAt(index)));
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////