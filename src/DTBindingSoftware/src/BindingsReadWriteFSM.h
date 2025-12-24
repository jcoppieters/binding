// BindingsReadWriteFSM.h: interface for the CBindingsReadWriteFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGSREADWRITEFSM_H__EB380EA9_9DC4_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGSREADWRITEFSM_H__EB380EA9_9DC4_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IComApplication;
class CBindingFile;


////////////////////////////////// INTERFACES /////////////////////////////////////////////////

/**
 * implemented by CBindingFileFSM 
 */
class IBindingsReadWriteFSMEvents 
{
public:
	virtual void NotifyOnEventWriteDone(void)		= 0;		
	virtual void NotifyOnEventEraseDone(void)		= 0;		
	virtual void NotifyOnRefreshTimer(void)			= 0;		
	virtual void NotifyOnEventReadDone(void)		= 0;		
	virtual void NotifyOnEventReadError(void)		= 0;	
	virtual void NotifyOnEventWriteError(void)		= 0;		
	virtual void NotifyOnEventEraseError(void)		= 0;		
};

///////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////

/**
 * Created by CComApplication Class 
 */
class CBindingsReadWriteFSM : public IBindingsReadWriteFSMEvents 
{
private:	
	// Pointer naar de volgende binding file 'm_pCurrBindingFile'.
	// Gebruiken voor write/write modified.
	//
	// return waarde:
	// TRUE: Binding file gevonden.
	// FALSE: Geen binding file gevonden.
	BOOL GetNextBindingFile(void);	

	// Pointer naar de volgende binding file 'm_pCurrBindingFile'.
	// Gebruiken om bindings uit te lezen.
	//
	// return waarde:
	// TRUE: Binding file gevonden.
	// FALSE: Geen binding file gevonden.
	BOOL GetNextReadBindingFile(void);	

	
	// Clean up current task.
	// - pointer 'm_pCurrBindingFile' op NULL plaatsen.
	// - Unsubscribe.
	// - m_iCurrentNodeNr=0
	BOOL CleanUpCurrentTask(void);

	typedef enum  {
		FSM_STATE_IDLE=0,
		FSM_STATE_ERASE,
		FSM_STATE_WRITE,
		FSM_STATE_READ	
	}FSM_STATES;
	
	typedef enum  {
		FSM_WRITE_MODE_ALL = 0,
		FSM_WRITE_MODE_MODIFIED
	}FSM_WRITE_MODES;	

	typedef enum  {
		FSM_EV_OPERATION_START_WRITE = 0,
		FSM_EV_OPERATION_START_READ,
		FSM_EV_SIGNAL_RW_DONE,						// Node read/write done
		FSM_EV_SIGNAL_ERASE_DONE,					// Node erase done.
		FSM_EV_SIGNAL_ERROR,						// Node erase/write/read error.	
		FSM_EV_SIGNAL_ACK_REFRESH_TIMER,
		FSM_BINDINGS_EV_AFTER_TIMEOUT
	}FSM_EVENTS;


	CWnd* m_pParent;
	int m_iCurrentNodeNr;			
	IComApplication *const pComApplication;
	CBindingFile *m_pCurrBindingFile;	
	int m_retryCounter;							// Sedert V16.30
	FSM_STATES m_currentState;
	FSM_WRITE_MODES m_writeMode;
	
private:		
	void FSMReadWrite(FSM_EVENTS event);
	void WriteBindings(void);

	// Events to be fired to IComApplication ...
	void Fire_NotifyParentOnEraseNodeStart(BYTE bNodeAddress);
	void Fire_NotifyParentOnEraseNodeError(BYTE bNodeAddress);
	void Fire_NotifyParentOnEraseNodeDone(BYTE bNodeAddress);
	void Fire_NotifyParentOnWriteNodeStart(BYTE bNodeAddress);
	void Fire_NotifyParentOnWriteNodeDone(BYTE bNodeAddress);
	void Fire_NotifyParentOnWriteNodeError(BYTE bNodeAddress);
	void Fire_NotifyParentOnReadNodeStart(BYTE bNodeAddress);
	void Fire_NotifyParentOnReadNodeDone(BYTE bNodeAddress);
	void Fire_NotifyParentOnReadNodeError(BYTE bNodeAddress);
	void Fire_NotifyParentOnWriteNodesFSMDone(void);
	void Fire_NotifyParentOnReadNodesFSMDone(void);
	void StartTimer();
	void StopTimer();

public:
	CBindingsReadWriteFSM(IComApplication* );
	virtual ~CBindingsReadWriteFSM();

	// Sedert V17.02: Opvragen van het node address.
	int getCurrentNodeAddress( ) const;


	/**
	 * Invoked by CComApplication
	 */
	void WriteAllBindings(void) 
	{
		m_writeMode = FSM_WRITE_MODE_ALL;	
		WriteBindings();
	}

	/**
	 * Invoked by CComApplication
	 */
	void WriteModifiedBindings(void) 
	{
		m_writeMode = FSM_WRITE_MODE_MODIFIED;	
		WriteBindings();
	}

	/**
	 * Invoked by CComApplication
	 */
	void ReadAllBindings(void);

	/**
	 * CCommApplication : Timer ...
	 */
	void EvAfterTimeout() {
		FSMReadWrite(FSM_BINDINGS_EV_AFTER_TIMEOUT);
	}
	
	/**
	 * Implementation of IBindingsReadWriteFSMEvents - CBindingFileFSM
	 */
	void NotifyOnEventWriteDone(void) { 
		FSMReadWrite(FSM_EV_SIGNAL_RW_DONE); 
	}
	void NotifyOnEventEraseDone(void) { 
		FSMReadWrite(FSM_EV_SIGNAL_ERASE_DONE);
	}
	void NotifyOnRefreshTimer(void) { 
		FSMReadWrite(FSM_EV_SIGNAL_ACK_REFRESH_TIMER); 
	}
	void NotifyOnEventReadDone(void) { 
		FSMReadWrite(FSM_EV_SIGNAL_RW_DONE);
	}
	void NotifyOnEventReadError(void) { 
		FSMReadWrite(FSM_EV_SIGNAL_ERROR); 
	}
	void NotifyOnEventWriteError(void) { 
		FSMReadWrite(FSM_EV_SIGNAL_ERROR); 
	}
	void NotifyOnEventEraseError(void) { 
		FSMReadWrite(FSM_EV_SIGNAL_ERROR); 
	}
};

/////////////////////////// INTERFACES /////////////////////////////////////////

#endif // !defined(AFX_BINDINGSREADWRITEFSM_H__EB380EA9_9DC4_11D8_B865_0050BAC412B1__INCLUDED_)
