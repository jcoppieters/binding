// BindingFileFSM.h: interface for the CBindingFileFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEFSM_H__553D8762_9CE1_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGFILEFSM_H__553D8762_9CE1_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/////////////////////////////////////////////////////////////////////////////////
class CBindingFile;
#include "BindingsReadWriteFSM.h"

/////////////////////////////// INTERFACE ///////////////////////////////////////

class IBindingFileFSM 
{

public:
	// Interfaces
	virtual BOOL StartReadBindings(void) = 0;	
	virtual BOOL StartWriteBindings(void) =0;		
	virtual BOOL StartEraseBindings(void) =0;   

	// Interface for Global State Machine
	virtual void Subscribe_GlobalFSM(CBindingsReadWriteFSM *ptr) = 0;	
	virtual void UnSubscribe_GlobalFSM(void) = 0;					

	// Read FSM
	virtual void SetBindingFlags(BYTE bFlags)			= 0;					
	virtual void SetBindingNr(unsigned short BindingNr) = 0;	
	virtual void SetTempBindingData(CString szData)		= 0;							
	virtual void SetBindingsToRead(int numberOfBindings)= 0;				

	// EVENT_SIGNAL_STATUS_XXX Source 
	virtual void EvSignalStatusHeaderRead(void)			= 0;
	virtual void EvSignalStatusHeaderWritten(void)		= 0;
	virtual void EvSignalStatusDataRead(void)			= 0;
	virtual void EvSignalStatusDataWritten(void)		= 0;
	virtual void EvSignalStatusEntryCleared(void)		= 0;
	virtual void EvSignalStatusWriteDone(void)			= 0;
	virtual void EvSignalStatusWriteReadCancel(void)	= 0;
	virtual void EvSignalStatusReadDone(void)			= 0;
	virtual void EvSignalStatusError(void)				= 0;

	// EVENT_SIGNAL_STATUS_XXX Source
	virtual void EvSignalStatusFileClosed(void)			= 0;
	virtual void EvSignalStatusFileOpened(void)			= 0;
	virtual void EvSignalStatusFileInfo(void)			= 0;
	virtual void EvSignalStatusFileErased(void)			= 0;
	virtual void EvSignalStatusFileError(void)			= 0;
};

/////////////////////////////// IMPLEMENTATION //////////////////////////////////

class CBindingFileFSM : public IBindingFileFSM
{
private:
	typedef enum {
		FSM_EV_OPERATION_BINDINGTASK_START = 0,
		FSM_EV_SIGNAL_BINDINGTASK_ACK,
		FSM_EV_SIGNAL_BINDINGTASK_ERROR
	} FSM_BINDINGTASK_EVENTS;

	// Afhandelen van de events afkomstig van EV_NODEBINDINGSFILECONTROL
	typedef enum {
		FSM_EV_SIGNAL_BINDINGFILE_CLOSED =0, 
		FSM_EV_SIGNAL_BINDINGFILE_OPENED, 
		FSM_EV_SIGNAL_BINDINGFILE_INFO,		
		FSM_EV_SIGNAL_BINDINGFILE_ERASED,	
		FSM_EV_SIGNAL_BINDINGFILE_ERROR
	} FSM_BINDINGFILE_EVENTS;

	typedef enum  {
		FSM_EV_SIGNAL_STATUS_HEADER_READ = 0,
		FSM_EV_SIGNAL_STATUS_HEADER_WRITTEN,
		FSM_EV_SIGNAL_STATUS_DATA_READ,
		FSM_EV_SIGNAL_STATUS_DATA_WRITTEN,
		FSM_EV_SIGNAL_STATUS_ENTRY_CLEARED,
		FSM_EV_SIGNAL_STATUS_WRITE_DONE,
		FSM_EV_SIGNAL_STATUS_WRITE_CANCEL,
		FSM_EV_SIGNAL_STATUS_DONE_READ,
		FSM_EV_SIGNAL_STATUS_BINDING_ERROR
	} FSM_BINDINGENTRY_EVENTS;

	typedef enum  {
		FSM_TASK_BINDING_READ = 0,
		FSM_TASK_BINDING_WRITE,	
		FSM_TASK_BINDING_ERASE		
	}FSM_BINDINGTASKS;

	typedef enum  {
		FSM_STATE_BINDING_TASK_IDLE = 0,
		FSM_STATE_BINDING_TASK_BUSY	
	}FSM_BINDINGTASK_STATE;

	typedef enum {
		FSM_STATE_BINDINGFILE_CLOSE=0,
		FSM_STATE_BINDINGFILE_OPEN	
	}FSM_BINDINGFILE_STATE;

	typedef enum  {
		FSM_STATE_BINDINGENTRY_IDLE=0,
		FSM_STATE_BINDINGENTRY_READ_BINDING,
		FSM_STATE_BINDINGENTRY_WRITE_BINDING	
	}FSM_BINDINGENTRY_HANDLE_STATE;


private:
	int m_stringPos;
	int m_stringLength;
	FSM_BINDINGTASKS m_currentTask;								// Read, write, erase
	FSM_BINDINGTASK_STATE m_currentTaskState;					// Idle, busy
	FSM_BINDINGFILE_STATE m_currentFileState;					// open, closed
	FSM_BINDINGENTRY_HANDLE_STATE m_currentBindingEntryState;	// Idle, read, write
	BYTE m_bNodeAddress;
	BYTE m_bMaxPacketLength;
	unsigned short m_usBindingNR;
	BYTE m_bBindingFlags;
	CString m_strBindingString;						// holds current string to Send
	CString m_strTempBindingData;					// Temp data read
	unsigned short m_currentBindingEntryNR;			// Sedert V17.00: Uitbreiding van het aantal bindings.
	unsigned short m_NROfBindingsToWrite;			// Sedert V17.00: Uitbreiding van het aantal bindings.
	unsigned short m_NROfBindingsToRead;			// Sedert V17.00: Uitbreiding van het aantal bindings.
	IBindingsReadWriteFSMEvents* m_ptrGlobalFSM; 
	CBindingFile* m_pBindingFile;

protected:
	BOOL StartReadBindings(void);
	BOOL StartWriteBindings(void);
	BOOL StartEraseBindings(void);
	
	void onNotifyParentFSM_NodeBindingsWriteDone(void) {	
		if (this->m_ptrGlobalFSM != NULL) {
			this->m_ptrGlobalFSM->NotifyOnEventWriteDone();
		}
	}

	void onNotifyParentFSM_NodeBindingsEraseDone(void){ 
		if (this->m_ptrGlobalFSM != NULL) {
			this->m_ptrGlobalFSM->NotifyOnEventEraseDone();
		}
	}

	void onNotifyParentFSM_NodeBindingsWriteError(void) { 
		if (this->m_ptrGlobalFSM != NULL) {
			this->m_ptrGlobalFSM->NotifyOnEventWriteError();
		}
	}

	void onNotifyParentFSM_NodeBindingsEraseError(void) { 
		if (this->m_ptrGlobalFSM != NULL) {
			this->m_ptrGlobalFSM->NotifyOnEventEraseError();
		}
	}
	
	void onNotifyParentFSM_NodeBindingsRefreshTimer(void) { 
		if (this->m_ptrGlobalFSM != NULL) {
			this->m_ptrGlobalFSM->NotifyOnRefreshTimer();
		}
	}

	void onNotifyParentFSM_NodeBindingsReadDone(void) { 
		if (this->m_ptrGlobalFSM != NULL) {
			this->m_ptrGlobalFSM->NotifyOnEventReadDone();
		}
	}

	void onNotifyParentFSM_NodeBindingsReadError(void) { 
		if (this->m_ptrGlobalFSM != NULL) {
			this->m_ptrGlobalFSM->NotifyOnEventReadError();
		}
	}

	// Helper functie voor display van de events.
	typedef enum {
		INFOMSG_BINDINGFILE_OPEN = 0,
		INFOMSG_BINDINGFILE_CLOSE,
		INFOMSG_BINDINGFILE_WRITE_BINDING,			// param.
		INFOMSG_BINDINGFILE_READ_BINDING,			// param
		INFOMSG_BINDINGFILE_WRITE_ERROR_CLOSE,
		INFOMSG_BINDINGFILE_READ_ERROR_CLOSE,
		INFOMSG_BINDINGFILE_ERASE_START,
		INFOMSG_BINDINGFILE_ERASE_DONE,
		INFOMSG_BINDINGFILE_ERASE_ERROR
	} InfoMsg_t;
	void displayMsg(int nodeAddress,InfoMsg_t id,int param = -1);


public:
	CBindingFileFSM();
	CBindingFileFSM(CBindingFile *pParent);	
	virtual ~CBindingFileFSM();

public:
	void Subscribe_GlobalFSM(CBindingsReadWriteFSM *ptr) {
		this->m_ptrGlobalFSM = ptr; 
	}	
	void UnSubscribe_GlobalFSM(void) {
		this->m_ptrGlobalFSM = NULL; 
	}
	void SetBindingFlags(BYTE bFlags){
		this->m_bBindingFlags = bFlags; 
	}
	void SetBindingNr(unsigned short BindingNr) {
		this->m_usBindingNR = BindingNr; 
	}
	void SetTempBindingData(CString szData)	{
		this->m_strTempBindingData.Format("%s",szData); 
	}
	// Sedert V17.00: Uitbreiding van het aantal bindings in een node.
	void SetBindingsToRead(int numberOfBindings) {	
		this->m_NROfBindingsToRead = numberOfBindings;
	}	
private:	
	// Afhandelen van de events afkomstig van EV_NODEBINDINGSFILECONTROL
	void FSM_handleBindingFileEvent(FSM_BINDINGFILE_EVENTS FileEvent);
		BOOL FSM_handleBindingFileEvent_StateBase(FSM_BINDINGFILE_EVENTS FileEvent);
			BOOL FSM_handleBindingFileEvent_StateFileOpened(FSM_BINDINGFILE_EVENTS FileEvent);
			BOOL FSM_handleBindingFileEvent_StateFileClosed(FSM_BINDINGFILE_EVENTS FileEvent);		

	// Afhandelen van de events van EV_NODEBINDINGENTRY	
	void FSM_handleBindingEntryEvent(FSM_BINDINGENTRY_EVENTS EntryEvent);
		BOOL FSM_handleBindingEntryEvent_StateBase(FSM_BINDINGENTRY_EVENTS EntryEvent);
			BOOL FSM_handleBindingEntryEvent_StateIdle(FSM_BINDINGENTRY_EVENTS EntryEvent);
			BOOL FSM_handleBindingEntryEvent_StateReadBinding(FSM_BINDINGENTRY_EVENTS EntryEvent);
			BOOL FSM_handleBindingEntryEvent_StateWriteBinding(FSM_BINDINGENTRY_EVENTS EntryEvent);		

	// Afhandelen van een globaal event (mogelijk afkomstig van bovenliggende fsm)
	void FSM_handleEvent(FSM_BINDINGTASK_EVENTS TaskEvent);			
		void FSM_handleEvent_TaskBindingsRead(FSM_BINDINGTASK_EVENTS TaskEvent);		
			void FSM_handleEvent_TaskBindingsRead_Idle(FSM_BINDINGTASK_EVENTS TaskEvent);		
			void FSM_handleEvent_TaskBindingsRead_Busy(FSM_BINDINGTASK_EVENTS TaskEvent);		
		void FSM_handleEvent_TaskBindingsWrite(FSM_BINDINGTASK_EVENTS TaskEvent);	
			void FSM_handleEvent_TaskBindingsWrite_Idle(FSM_BINDINGTASK_EVENTS TaskEvent);	
			void FSM_handleEvent_TaskBindingsWrite_Busy(FSM_BINDINGTASK_EVENTS TaskEvent);	
		void FSM_handleEvent_TaskBindingsErase(FSM_BINDINGTASK_EVENTS TaskEvent);	
			void FSM_handleEvent_TaskBindingsErase_Idle(FSM_BINDINGTASK_EVENTS TaskEvent);	
			void FSM_handleEvent_TaskBindingsErase_Busy(FSM_BINDINGTASK_EVENTS TaskEvent);	

	void Init(void);

public:			
	
	// Afhandelen van de events van EV_NODEBINDINGENTRY	
	void EvSignalStatusHeaderRead(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_HEADER_READ); 
	}
	void EvSignalStatusHeaderWritten(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_HEADER_WRITTEN); 
	}
	void EvSignalStatusDataRead(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_DATA_READ); 
	}
	void EvSignalStatusDataWritten(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_DATA_WRITTEN); 
	}
	void EvSignalStatusEntryCleared(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_ENTRY_CLEARED); 
	}
	void EvSignalStatusWriteDone(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_WRITE_DONE); 
	}
	void EvSignalStatusWriteReadCancel(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_WRITE_CANCEL); 
	}
	void EvSignalStatusReadDone(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_DONE_READ); 
	}
	void EvSignalStatusError(void) {
		FSM_handleBindingEntryEvent(FSM_EV_SIGNAL_STATUS_BINDING_ERROR); 
	}

	// Afhandelen van de events van EV_NODEBINDINGSFILECONTROL	
	void EvSignalStatusFileClosed(void)	{
		this->FSM_handleBindingFileEvent(FSM_EV_SIGNAL_BINDINGFILE_CLOSED);
	}
	void EvSignalStatusFileOpened(void) {
		this->FSM_handleBindingFileEvent(FSM_EV_SIGNAL_BINDINGFILE_OPENED);
	}
	void EvSignalStatusFileInfo(void) {
		this->FSM_handleBindingFileEvent(FSM_EV_SIGNAL_BINDINGFILE_INFO);	
	}
	void EvSignalStatusFileErased(void) {
		this->FSM_handleBindingFileEvent(FSM_EV_SIGNAL_BINDINGFILE_ERASED);
	}
	void EvSignalStatusFileError(void) {
		this->FSM_handleBindingFileEvent(FSM_EV_SIGNAL_BINDINGFILE_ERROR);	
	}
};

#endif // !defined(AFX_BINDINGFILEFSM_H__553D8762_9CE1_11D8_B865_0050BAC412B1__INCLUDED_)
