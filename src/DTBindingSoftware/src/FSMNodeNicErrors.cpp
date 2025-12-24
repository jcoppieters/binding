// FSMNodeNicErrors.cpp: implementation of the CFSMNodeNicErrors class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "FSMNodeNicErrors.h"

#include "FSMNicErrors.h"
#include "TXNodeMessage.h"
#include "CanNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFSMNodeNicErrors::CFSMNodeNicErrors(CFSMNicErrors* pParent) :
m_pParent(pParent) {
	ASSERT(m_pParent);
	m_CurrentState = FSM_STATE_IDLE;
	m_retryCounter = 0;
}

CFSMNodeNicErrors::~CFSMNodeNicErrors() {
	StopTimer();
}

/** Public : FSM ...*/
BOOL CFSMNodeNicErrors::EvOperationStart(CCanNode* pNode)
{
	ASSERT(pNode);
	switch (m_CurrentState) {
		case FSM_STATE_IDLE: {							
			if ((m_pCurrentNode = pNode) != 0) {			
				CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pCurrentNode->bLogicalAddress, 0);
				StartTimer();			
				m_retryCounter = 0;
				m_CurrentState = FSM_STATE_WAIT_FOR_NIC_ERRORS0;
				return TRUE;
			}
		} break;

		default: {
			ASSERT(0);
		}
	}
	return FALSE;
}

BOOL CFSMNodeNicErrors::EvOperationCancel()
{
	switch (m_CurrentState) {
		case FSM_STATE_IDLE: {
			/* Absorbe */
		} break;
		case FSM_STATE_WAIT_FOR_NIC_ERRORS0: {
			StopTimer();
			m_CurrentState = FSM_STATE_IDLE;
		} break;

		case FSM_STATE_WAIT_FOR_NIC_ERRORS1: {
			StopTimer();
			m_CurrentState = FSM_STATE_IDLE;
		} break;
	}
	return TRUE;
}

void CFSMNodeNicErrors::EvSignalTimeout()
{
	switch (m_CurrentState) {
		case FSM_STATE_IDLE: {
			/* Absorbe */
		} break;

		case FSM_STATE_WAIT_FOR_NIC_ERRORS0: {
			StopTimer();
			if (++m_retryCounter <= NR_OF_RETRIES) {
				ASSERT(NULL != m_pCurrentNode);
				NotifyRetry();
				CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pCurrentNode->bLogicalAddress, 0);				
				StartTimer();
			}
			else {
				m_CurrentState = FSM_STATE_IDLE;
				NotifyError();
			}
		} break;

		case FSM_STATE_WAIT_FOR_NIC_ERRORS1: {
			StopTimer();
			if (++m_retryCounter <= NR_OF_RETRIES) {
				ASSERT(NULL != m_pCurrentNode);
				NotifyRetry();
				CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pCurrentNode->bLogicalAddress, 1);
				StartTimer();
			}
			else {
				m_CurrentState = FSM_STATE_IDLE;
				NotifyError();
			}
		} break;
	}
}
void CFSMNodeNicErrors::EvSignalNicErrors0(BYTE bNodeAddress)
{
	switch (m_CurrentState) {
		case FSM_STATE_IDLE: {
			/* Absorbe */
		} break;

		// Sedert V16.30:
		// Wanneer we een status van een andere node ontvangen de timer opnieuw starten en wachten tot we een 
		// antwoord krijgen van de node.
		// Vroeger werd dit als een fout gezien.
		case FSM_STATE_WAIT_FOR_NIC_ERRORS0: {
			StopTimer();
			if (m_pCurrentNode->bLogicalAddress == bNodeAddress) {
				CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pCurrentNode->bLogicalAddress , 1);
				m_retryCounter = 0;
				StartTimer();
				m_CurrentState = FSM_STATE_WAIT_FOR_NIC_ERRORS1;
			}
			else {
				StartTimer();
			}
		} break;

		// Sedert V16.30:
		// Absorbe: Vroeger zagen we dit als een fout.
		case FSM_STATE_WAIT_FOR_NIC_ERRORS1: {
			/* absorbe */	
		} break;
	}
}

void CFSMNodeNicErrors::EvSignalNicErrors1(BYTE bNodeAddress)
{
	switch (m_CurrentState) {
		case FSM_STATE_IDLE: {
			/* Absorbe */
		} break;

		// Sedert V16.30:
		// Absorbe: Vroeger zagen we dit als een fout.
		case FSM_STATE_WAIT_FOR_NIC_ERRORS0: {
			/* Absorbe */			
		} break;

		// Sedert V16.30:
		// Wanneer we een status van een andere node ontvangen de timer opnieuw starten en wachten tot we een 
		// antwoord krijgen van de node.
		// Vroeger werd dit als een fout gezien.
		case FSM_STATE_WAIT_FOR_NIC_ERRORS1: {
			StopTimer();
			m_CurrentState = FSM_STATE_IDLE;
			if (m_pCurrentNode->bLogicalAddress == bNodeAddress) {		
				m_retryCounter = 0;
				NotifyDone();
			}
			else {
				StartTimer();
			}
		} break;
	}
}

/** Protected */
void CFSMNodeNicErrors::StartTimer() {
	m_pParent->StartTimer();
}

void CFSMNodeNicErrors::StopTimer() {
	m_pParent->StopTimer();
}

void CFSMNodeNicErrors::NotifyDone() {
	m_pParent->NotifyDone(m_pCurrentNode);
}

void CFSMNodeNicErrors::NotifyError() {
	m_pParent->NotifyError(m_pCurrentNode);
}

void CFSMNodeNicErrors::NotifyRetry() {
	m_pParent->NotifyRetry(m_pCurrentNode);
}
