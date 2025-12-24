// BindingsReadWriteFSM.cpp: implementation of the CBindingsReadWriteFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingsReadWriteFSM.h"

#include "ComApplication.h"
#include "BindingFile1.h"
#include "BindingFileFSM.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/**********************************************************************/
#define TRACER_FSM(x)			TRACE x				// DEBUGGEN: TRACER INSCHAKELEN.
//#define TRACER_FSM(x)								// TRACER UITSCHAKELEN.

/**********************************************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingsReadWriteFSM::CBindingsReadWriteFSM(IComApplication *pCom) :
pComApplication(pCom), m_pCurrBindingFile(NULL), m_iCurrentNodeNr(0), m_currentState(FSM_STATE_IDLE), m_retryCounter(0)
{				
}

CBindingsReadWriteFSM::~CBindingsReadWriteFSM()
{
	// CleanUpCurrentTask();		// NEW 20060508 !!!
}

int CBindingsReadWriteFSM::getCurrentNodeAddress( ) const {
	if (NULL == m_pCurrBindingFile) {
		return -1;
	}
	return m_pCurrBindingFile->GetNodeAddress();
}

void CBindingsReadWriteFSM::WriteBindings(void) {
	m_currentState = FSM_STATE_IDLE;
	FSMReadWrite(FSM_EV_OPERATION_START_WRITE);
}

void CBindingsReadWriteFSM::ReadAllBindings(void) {
	m_currentState = FSM_STATE_IDLE;
	FSMReadWrite(FSM_EV_OPERATION_START_READ);
}

////////////////////////////////////////////////////////////////////////
BOOL CBindingsReadWriteFSM::CleanUpCurrentTask(void) {
	if (m_pCurrBindingFile != NULL)  {
		TRACER_FSM(("SUBSCRIBE BINDINGFILE - 0x%02x\r\n",m_pCurrBindingFile->GetNodeAddress()));	
		m_pCurrBindingFile->GetFSM()->UnSubscribe_GlobalFSM();		
		m_pCurrBindingFile = NULL;
	}
	m_iCurrentNodeNr = 0;
	return TRUE;
}


BOOL CBindingsReadWriteFSM::GetNextReadBindingFile(void)
{
	CCanNode *pNode;
	int t=0;
	if (TMGetRuntimeNodes()->NROfNodes(&t)) {
		if (m_pCurrBindingFile != NULL) {		
			// Unsubscribe bindingfile from FSM ...
			TRACER_FSM(("UNSUBSCRIBE BINDINGFILE - 0x%02x\r\n",m_pCurrBindingFile->GetNodeAddress()));
			m_pCurrBindingFile->ClearModified();	//Clear Modified Flag ...
			m_pCurrBindingFile->GetFSM()->UnSubscribe_GlobalFSM();		
			m_pCurrBindingFile = NULL;
		}
			
		if (m_iCurrentNodeNr < t)  {					
			if (TMGetRuntimeNodes()->GetNode(m_iCurrentNodeNr,&pNode)) {						
				if ((m_pCurrBindingFile = pNode->GetBindingFilePtr()) != NULL) {
					m_pCurrBindingFile->GetFSM()->Subscribe_GlobalFSM(this);		
					TRACER_FSM(("SUBSCRIBE BINDINGFILE - 0x%02x\r\n",m_pCurrBindingFile->GetNodeAddress()));		
					m_iCurrentNodeNr++;
					return TRUE;
				}
			}
		}			
	}	
	return FALSE;
}


// f(Write only Modified , Write All ...)
BOOL CBindingsReadWriteFSM::GetNextBindingFile(void) {
	CCanNode *pNode;
	int t=0;	
	if (TMGetRuntimeNodes()->NROfNodes(&t)) {
		if (m_pCurrBindingFile != NULL) {
			TRACER_FSM(("UNSUBSCRIBE BINDINGFILE - 0x%02x\r\n",m_pCurrBindingFile->GetNodeAddress()));
			m_pCurrBindingFile->ClearModified();	//Clear Modified Flag ...
			m_pCurrBindingFile->GetFSM()->UnSubscribe_GlobalFSM();		
			m_pCurrBindingFile = NULL;
		}
	
		if (m_writeMode == FSM_WRITE_MODE_ALL) {
			if (m_iCurrentNodeNr < t) {					
				if (TMGetRuntimeNodes()->GetNode(m_iCurrentNodeNr,&pNode)) {						
					if ((m_pCurrBindingFile = pNode->GetBindingFilePtr()) != NULL) {
						m_pCurrBindingFile->GetFSM()->Subscribe_GlobalFSM(this);		
						TRACER_FSM(("SUBSCRIBE BINDINGFILE - 0x%02x\r\n",m_pCurrBindingFile->GetNodeAddress()));			
						m_iCurrentNodeNr++;
						return TRUE;
					}
				}
			}			
		}
		else // FSM_WRITE_MODE_MODIFIED
		{
			while(m_iCurrentNodeNr < t) {
				if (TMGetRuntimeNodes()->GetNode(m_iCurrentNodeNr,&pNode)) {								
					if ((m_pCurrBindingFile = pNode->GetBindingFilePtr()) != NULL) {			
						if (m_pCurrBindingFile->IsModified() == TRUE)  {
							m_pCurrBindingFile->GetFSM()->Subscribe_GlobalFSM(this);
							TRACER_FSM(("SUBSCRIBE BINDINGFILE - 0x%02x\r\n",m_pCurrBindingFile->GetNodeAddress()));	
							return TRUE;
						}
						m_iCurrentNodeNr++;
						m_pCurrBindingFile = NULL;
					}
				}
			}		
		}
	}	
	return FALSE;	
}

////////////////////////////////////////////////////////////////////////



/* void CBindingsReadWriteFSM::FSMReadWrite(FSM_EVENTS event) */
/**
 * CurrentState
 * 		- FSM_STATE_IDLE=0,
 *		- FSM_STATE_ERASE,
 *		- FSM_STATE_WRITE,
 *		- FSM_STATE_READ
 *
 * event
 *		- FSM_EV_OPERATION_START_WRITE=0,
 *		- FSM_EV_OPERATION_START_READ,
 *		- FSM_EV_SIGNAL_RW_DONE,
 *		- FSM_EV_SIGNAL_ERASE_DONE,
 *		- FSM_EV_SIGNAL_ERROR,
 *		- FSM_EV_AFTER_TIMEOUT,
 *		- FSM_EV_SIGNAL_ACK_REFRESH_TIMER
 *
 */
// Sedert V16.30
enum {
	RETRYCOUNTER_RELOADVALUE_ERASE = 2,
	RETRYCOUNTER_RELOADVALUE_WRITE = 5,
	RETRYCOUNTER_RELOADVALUE_READ = 5
};

void CBindingsReadWriteFSM::FSMReadWrite(FSM_EVENTS fsmEvent)
{
	switch (m_currentState)
	{
		case FSM_STATE_IDLE:
		{
			switch (fsmEvent)
			{
				case FSM_EV_OPERATION_START_WRITE:	
				{						
					m_iCurrentNodeNr = 0;						
					if (GetNextBindingFile() == TRUE) {						
						Fire_NotifyParentOnEraseNodeStart(m_pCurrBindingFile->GetNodeAddress());
						m_pCurrBindingFile->GetFSM()->StartEraseBindings();
						m_retryCounter = RETRYCOUNTER_RELOADVALUE_ERASE;
						StartTimer();						
						m_currentState = FSM_STATE_ERASE;
					}
#ifdef _DEBUG
					else { ASSERT(0); }
#endif
				} break;

				case FSM_EV_OPERATION_START_READ:
				{
					m_iCurrentNodeNr = 0;					
					if (GetNextReadBindingFile()) {											
						m_pCurrBindingFile->Clear();	
						m_pCurrBindingFile->GetFSM()->StartReadBindings();
						m_retryCounter = RETRYCOUNTER_RELOADVALUE_READ;
						StartTimer();				
						m_currentState = FSM_STATE_READ;
					}
#ifdef _DEBUG
					else { ASSERT(0); }
#endif
				} break;
			
				case FSM_EV_SIGNAL_RW_DONE :
				case FSM_EV_SIGNAL_ERASE_DONE:
				case FSM_EV_SIGNAL_ERROR	:
				case FSM_BINDINGS_EV_AFTER_TIMEOUT :
				case FSM_EV_SIGNAL_ACK_REFRESH_TIMER :
				break;
			}
		} break;

		case FSM_STATE_ERASE:
		{
			switch (fsmEvent)
			{
				case FSM_EV_SIGNAL_ERASE_DONE:					
				{
					StopTimer();
					Fire_NotifyParentOnEraseNodeDone(m_pCurrBindingFile->GetNodeAddress());
					ASSERT(m_pCurrBindingFile);						
					if (m_pCurrBindingFile->GetFSM()->StartWriteBindings() == TRUE) {										
						m_retryCounter = RETRYCOUNTER_RELOADVALUE_WRITE;
						StartTimer();
						Fire_NotifyParentOnWriteNodeStart(m_pCurrBindingFile->GetNodeAddress());						
						m_currentState = FSM_STATE_WRITE;
					}								
					else 
					{						
						if (GetNextBindingFile()) {					
							m_retryCounter = RETRYCOUNTER_RELOADVALUE_ERASE;
							StartTimer();
							Fire_NotifyParentOnEraseNodeStart(m_pCurrBindingFile->GetNodeAddress());
							m_pCurrBindingFile->GetFSM()->StartEraseBindings();
							m_currentState = FSM_STATE_ERASE;
						}
						else {																					
							Fire_NotifyParentOnWriteNodesFSMDone();
							m_currentState = FSM_STATE_IDLE;
						}		
					}
				} break;

				case FSM_EV_SIGNAL_RW_DONE :	
				break;

				case FSM_BINDINGS_EV_AFTER_TIMEOUT:
				{
					StopTimer();
					if (--m_retryCounter>0) {
						StartTimer();
					}
					else {
						if (m_pCurrBindingFile != NULL) {
							Fire_NotifyParentOnEraseNodeError(m_pCurrBindingFile->GetNodeAddress());
						}
						CleanUpCurrentTask();
					}
				} break;

				case FSM_EV_SIGNAL_ERROR:
				{
					StopTimer();
					if (m_pCurrBindingFile != NULL) {
						Fire_NotifyParentOnEraseNodeError(m_pCurrBindingFile->GetNodeAddress());
					}
					CleanUpCurrentTask();
				} break;

				case FSM_EV_SIGNAL_ACK_REFRESH_TIMER :
				{					
					// StopTimer();					
					StartTimer();
				} break;

				case FSM_EV_OPERATION_START_WRITE:							
				case FSM_EV_OPERATION_START_READ: {
					// Absorbe ...	
				} break;
			}
		} break;

		case FSM_STATE_WRITE:
		{
			switch (fsmEvent)
			{				
				case FSM_EV_SIGNAL_RW_DONE:	
				{						
					StopTimer();					
					Fire_NotifyParentOnWriteNodeDone(m_pCurrBindingFile->GetNodeAddress());					
					if (GetNextBindingFile() == TRUE) {													
						Fire_NotifyParentOnEraseNodeStart(m_pCurrBindingFile->GetNodeAddress());		
						m_retryCounter = RETRYCOUNTER_RELOADVALUE_ERASE;	
						StartTimer();
						m_pCurrBindingFile->GetFSM()->StartEraseBindings();
						m_currentState = FSM_STATE_ERASE;
					}
					else {
						Fire_NotifyParentOnWriteNodesFSMDone();
						m_currentState = FSM_STATE_IDLE;
					}				
				} break;

				case FSM_EV_SIGNAL_ACK_REFRESH_TIMER :
				{				
					// StopTimer();	
					m_retryCounter = RETRYCOUNTER_RELOADVALUE_WRITE;
					StartTimer();
				} break;

				case FSM_BINDINGS_EV_AFTER_TIMEOUT :
				{
					StopTimer();
					if (--m_retryCounter>0) {
						StartTimer();
					}
					else {
						if (m_pCurrBindingFile != NULL) {
							Fire_NotifyParentOnWriteNodeError(m_pCurrBindingFile->GetNodeAddress());
						}
						CleanUpCurrentTask();
					}
				} break;

				case FSM_EV_SIGNAL_ERROR:
				{					
					StopTimer();
					if (m_pCurrBindingFile != NULL) {
						Fire_NotifyParentOnWriteNodeError(m_pCurrBindingFile->GetNodeAddress());
					}
					CleanUpCurrentTask();
				} break;							
			
				case FSM_EV_SIGNAL_ERASE_DONE:
				case FSM_EV_OPERATION_START_WRITE:							
				case FSM_EV_OPERATION_START_READ:								
				break;
			}
		} break;

		case FSM_STATE_READ:
		{
			switch (fsmEvent)
			{				
				case FSM_EV_SIGNAL_RW_DONE :	
				{					
					StopTimer();					
					TRACE("\nFSM_BINDINGS_STATE_READ - FSM_EV_SIGNAL_RW_DONE");					
					if (GetNextReadBindingFile()) {											
						m_pCurrBindingFile->Clear();	
						m_retryCounter = RETRYCOUNTER_RELOADVALUE_READ;
						StartTimer();
						m_pCurrBindingFile->GetFSM()->StartReadBindings();						
						m_currentState = FSM_STATE_READ;
					}
					else {
						Fire_NotifyParentOnReadNodesFSMDone();
						m_currentState = FSM_STATE_IDLE;
					}	
				} break;

				case FSM_EV_SIGNAL_ACK_REFRESH_TIMER :
				{										
					// StopTimer();			
					m_retryCounter = RETRYCOUNTER_RELOADVALUE_READ;
					StartTimer();
				} break;

				case FSM_BINDINGS_EV_AFTER_TIMEOUT:
				{
					StopTimer();
					if (--m_retryCounter>0) {
						StartTimer();
					}
					else {
						if (m_pCurrBindingFile != NULL) {
							Fire_NotifyParentOnReadNodeError(m_pCurrBindingFile->GetNodeAddress());
						}
						CleanUpCurrentTask();
					}
				} break;
				case FSM_EV_SIGNAL_ERROR:
				{					
					StopTimer();
					if (m_pCurrBindingFile != NULL) {
						Fire_NotifyParentOnReadNodeError(m_pCurrBindingFile->GetNodeAddress());
					}
					CleanUpCurrentTask();					
				} break;			

				case FSM_EV_SIGNAL_ERASE_DONE:
				case FSM_EV_OPERATION_START_WRITE:							
				case FSM_EV_OPERATION_START_READ:	
				{					
				} break;
			}
		} break;
		default:
		{
			ASSERT(0);
		} break;
	}
}


//////////////////////////////////////////////////////////////////////////////
inline void CBindingsReadWriteFSM::StartTimer() {
	TRACER_FSM(("Bindings read write FSM: StartTimer()\r\n"));
	pComApplication->StartTimer(TIMERID_BINDINGS_TIMEOUT, TIMER_BINDINGS_TIMEOUT_VALUE);
}

inline void CBindingsReadWriteFSM::StopTimer() {
	TRACER_FSM(("Bindings read write FSM: StopTimer()\r\n"));
	pComApplication->StopTimer(TIMERID_BINDINGS_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////////////

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnEraseNodeStart(BYTE bNodeAddress)  {
	pComApplication->NotifyEraseBindingsNodeStart(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnEraseNodeError(BYTE bNodeAddress) {
	pComApplication->NotifyEraseBindingsNodeError(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnEraseNodeDone(BYTE bNodeAddress) {
	pComApplication->NotifyEraseBindingsNodeDone(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnWriteNodeStart(BYTE bNodeAddress) {
	pComApplication->NotifyWriteBindingsNodeStart(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnWriteNodeDone(BYTE bNodeAddress)  {
	pComApplication->NotifyWriteBindingsNodeDone(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnWriteNodeError(BYTE bNodeAddress)  {
	pComApplication->NotifyWriteBindingsNodeError(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnReadNodeStart(BYTE bNodeAddress) {
	pComApplication->NotifyReadBindingsNodeStart(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnReadNodeDone(BYTE bNodeAddress) {
	pComApplication->NotifyReadBindingsNodeDone(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnReadNodeError(BYTE bNodeAddress)  {
	pComApplication->NotifyReadBindingsNodeError(bNodeAddress);
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnWriteNodesFSMDone(void)  {
	pComApplication->NotifyWriteBindingsFSMDone();
}

inline void CBindingsReadWriteFSM::Fire_NotifyParentOnReadNodesFSMDone(void) {
	pComApplication->NotifyReadBindingsFSMDone();
}