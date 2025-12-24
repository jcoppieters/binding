// FSMNicErrors.h: interface for the CFSMNicErrors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSMNICERRORS_H__6C7E1DCD_02DF_4259_9076_8FE0617026AA__INCLUDED_)
#define AFX_FSMNICERRORS_H__6C7E1DCD_02DF_4259_9076_8FE0617026AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "FSMNodeNicErrors.h"
class CCanNode;
class IComApplication;


class CFSMNicErrors  
{
	friend class CFSMNodeNicErrors;

protected:

	enum enum_States
	{
		FSM_STATE_IDLE = 0,
		FSM_STATE_BUSY
	
	} m_CurrentState;

	IComApplication* const m_pComApplication;
	CFSMNodeNicErrors m_FSMNodeNicErrors;
	BYTE m_CurrentIndex;


protected:
	void StartTimer();
	void StopTimer();

	void NotifyDone(CCanNode* pCurrentNode);
	void NotifyError(CCanNode* pCurrentNode);
	void NotifyRetry(CCanNode* pCurrentNode);		// sedert V16.30

	inline void NotifyTaskDone();
	inline void NotifyTaskError();
	inline void NotifyTaskCancelled();

	CCanNode* GetNext();

public:
	CFSMNicErrors(IComApplication* const pComApplication);
	virtual ~CFSMNicErrors();

public:
	BOOL EvOperationStart();
	BOOL EvOperationCancel();
	
	void EvSignalTimeout();
	void EvSignalNicErrors0(BYTE bNodeAddress);
	void EvSignalNicErrors1(BYTE bNodeAddress);
};

#endif // !defined(AFX_FSMNICERRORS_H__6C7E1DCD_02DF_4259_9076_8FE0617026AA__INCLUDED_)
