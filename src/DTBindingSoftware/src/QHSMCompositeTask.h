// QHSMCompositeTask.h: interface for the CQHSMCompositeTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QHSMCOMPOSITETASK_H__EAF01D09_0E1B_4B98_8838_447118749613__INCLUDED_)
#define AFX_QHSMCOMPOSITETASK_H__EAF01D09_0E1B_4B98_8838_447118749613__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////
class CQHSMGenericTask;
/////////////////////////////////////////////////
class CQHSMCompositeTask  
{
protected:	
	CPtrArray Tasks;
	int nCurrentTask;
protected:
	CQHSMGenericTask* GetTask(int index);

public:
	CQHSMCompositeTask();
	virtual ~CQHSMCompositeTask();
	void Clear();
	BOOL AllocateNrOfTasks(int i);
	BOOL AddTask(CQHSMGenericTask *pTask);
	int GetNrOfTasks();	
	CQHSMGenericTask* Begin();
	CQHSMGenericTask* Next();
};
///////////////////////////////////////////////////

#endif // !defined(AFX_QHSMCOMPOSITETASK_H__EAF01D09_0E1B_4B98_8838_447118749613__INCLUDED_)
