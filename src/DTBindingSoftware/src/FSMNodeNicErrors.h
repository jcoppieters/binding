// FSMNodeNicErrors.h: interface for the CFSMNodeNicErrors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSMNODENICERRORS_H__E82F1DE3_0D6C_41FC_90DF_5CC0605462F1__INCLUDED_)
#define AFX_FSMNODENICERRORS_H__E82F1DE3_0D6C_41FC_90DF_5CC0605462F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCanNode;
class CFSMNicErrors;


class CFSMNodeNicErrors  
{
protected:
	
	enum enum_States
	{
		FSM_STATE_IDLE = 0,
		FSM_STATE_WAIT_FOR_NIC_ERRORS0,
		FSM_STATE_WAIT_FOR_NIC_ERRORS1	
	};


	// Sedert V16.30: 2x retry, wordt 3 keer verstuurd.
	enum { NR_OF_RETRIES = 2 };			

	enum_States m_CurrentState;
	CCanNode* m_pCurrentNode;
	CFSMNicErrors* const m_pParent;
	int m_retryCounter;					// Sedert V16.30

protected:		
	void StartTimer();
	void StopTimer();
	void NotifyDone();
	void NotifyError();
	void NotifyRetry();

public:
	CFSMNodeNicErrors(CFSMNicErrors* pParent);
	virtual ~CFSMNodeNicErrors();

public:
	/** FSM Events */
	BOOL EvOperationStart(CCanNode* pNode);
	BOOL EvOperationCancel();

	void EvSignalTimeout();
	void EvSignalNicErrors0(BYTE bNodeAddress);
	void EvSignalNicErrors1(BYTE bNodeAddress);
};

#endif // !defined(AFX_FSMNODENICERRORS_H__E82F1DE3_0D6C_41FC_90DF_5CC0605462F1__INCLUDED_)
