// FSMNicErrors.cpp: implementation of the CFSMNicErrors class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FSMNicErrors.h"

#include "FSMNicErrorsInfo.h"
#include "IComApplication.h"
#include "MyWindowTimers.h"
#include "CanNode.h"

#pragma warning( disable : 4355 )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFSMNicErrors::CFSMNicErrors(IComApplication* const pComApplication) :
m_pComApplication(pComApplication),
m_FSMNodeNicErrors(this)
{
	ASSERT( m_pComApplication );
	m_CurrentState = FSM_STATE_IDLE;
	m_CurrentIndex = 0;
}

CFSMNicErrors::~CFSMNicErrors()
{	
}

void CFSMNicErrors::StartTimer() {
	m_pComApplication->StartTimer(TIMERID_NICERRORSFSM_TIMEOUT, TIMER_NICERRORSFSM_VALUE);
}

void CFSMNicErrors::StopTimer() {
	m_pComApplication->StopTimer(TIMERID_NICERRORSFSM_TIMEOUT);
}

/**
 *
 */
BOOL CFSMNicErrors::EvOperationStart()
{
	switch (m_CurrentState)
	{
		case FSM_STATE_IDLE: {
			m_CurrentIndex = 0;			
			CCanNode* const pNode = GetNext();
			if (pNode != 0) {
				if (m_FSMNodeNicErrors.EvOperationStart(pNode)) {				
					m_CurrentState = FSM_STATE_BUSY;
					return TRUE;
				}
				else {
					/* Take actions ... */
					return FALSE;
				}		
			}
		} break;
	}
	return FALSE;
}

BOOL CFSMNicErrors::EvOperationCancel()
{
	switch (m_CurrentState) {
		case FSM_STATE_BUSY: {
			if (m_FSMNodeNicErrors.EvOperationCancel() == TRUE) {
				m_CurrentState = FSM_STATE_IDLE;
				NotifyTaskCancelled();			
			}
		} break;
	}
	return FALSE;
}
	
void CFSMNicErrors::EvSignalTimeout() {
	m_FSMNodeNicErrors.EvSignalTimeout();
}

void CFSMNicErrors::EvSignalNicErrors0(BYTE bNodeAddress) {
	m_FSMNodeNicErrors.EvSignalNicErrors0(bNodeAddress);
}

void CFSMNicErrors::EvSignalNicErrors1(BYTE bNodeAddress) {
	m_FSMNodeNicErrors.EvSignalNicErrors1(bNodeAddress);
}

void CFSMNicErrors::NotifyDone(CCanNode* pCurrentNode) {
	if (m_CurrentState == FSM_STATE_BUSY) {
		ASSERT(pCurrentNode);
		ASSERT(m_pComApplication);

		/** Notify parent: Node done : pCurrentNode */
		m_pComApplication->NotifyNicErrorsNodeFsmDone(pCurrentNode->bLogicalAddress);
		
		CCanNode* const pNode = GetNext();
		if (pNode != 0) {	
			if (m_FSMNodeNicErrors.EvOperationStart(pNode) == FALSE) {				
				m_CurrentState = FSM_STATE_IDLE;
				NotifyTaskError();
			}	
		}
		else {		
			m_CurrentState = FSM_STATE_IDLE;
			NotifyTaskDone();
		}
	}
}

void CFSMNicErrors::NotifyError(CCanNode* pCurrentNode) {
	if (m_CurrentState == FSM_STATE_BUSY) {
		ASSERT(pCurrentNode);
		ASSERT(m_pComApplication);

		/** Notify parent: Node error : pCurrentNode */
		m_pComApplication->NotifyNicErrorsNodeFsmError(pCurrentNode->bLogicalAddress);

		CCanNode* const pNode = GetNext();
		if (pNode != 0) {
			if (m_FSMNodeNicErrors.EvOperationStart(pNode) == FALSE) {			
				m_CurrentState = FSM_STATE_IDLE;			
				NotifyTaskError();
			}	
		}
		else {		
			m_CurrentState = FSM_STATE_IDLE;
			NotifyTaskDone();
		}
	}
}

void CFSMNicErrors::NotifyRetry(CCanNode* pCurrentNode) {
	if (m_CurrentState == FSM_STATE_BUSY) {
		ASSERT(pCurrentNode);
		ASSERT(m_pComApplication);
		m_pComApplication->NotifyNicErrorsNodeFsmRetry(pCurrentNode->bLogicalAddress);
	}
}

CCanNode* CFSMNicErrors::GetNext() {
	if (m_CurrentIndex < CFSMNicErrorsInfo::GetInstance()->Count()) {
		CCanNode* const pNode = CFSMNicErrorsInfo::GetInstance()->Get(m_CurrentIndex);
		m_CurrentIndex++;
		return pNode;
	}
	return 0;
}

inline void CFSMNicErrors::NotifyTaskDone() {
	if (m_pComApplication != 0) {	
		m_pComApplication->NotifyOnTaskDone(COMAPPLICATION_TASK_ID_NICERRORSFSM_ENGINE);
	}	
}

inline void CFSMNicErrors::NotifyTaskError() {
	if (m_pComApplication != 0) {		
		m_pComApplication->NotifyOnTaskError(COMAPPLICATION_TASK_ID_NICERRORSFSM_ENGINE);
	}
}

inline void CFSMNicErrors::NotifyTaskCancelled() {
	if (m_pComApplication != 0) {	
		m_pComApplication->NotifyOnTaskCancelled(COMAPPLICATION_TASK_ID_NICERRORSFSM_ENGINE);
	}
}