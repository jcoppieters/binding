// BindingFileFSM.cpp: implementation of the CBindingFileFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileFSM.h"
#include "TXNodeMessage.h"
#include "BindingFile1.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"
#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileFSM::CBindingFileFSM() { }

CBindingFileFSM::CBindingFileFSM(CBindingFile *pParent) :
m_pBindingFile(pParent), m_ptrGlobalFSM(0) {
	ASSERT(this->m_pBindingFile);
	this->Init();
}

CBindingFileFSM::~CBindingFileFSM() { }

//////////////////////////////////////////////////////////////////////
void CBindingFileFSM::Init(void) {
	this->m_currentFileState = FSM_STATE_BINDINGFILE_CLOSE;
	this->m_currentBindingEntryState = FSM_STATE_BINDINGENTRY_IDLE;
	this->m_currentTaskState = FSM_STATE_BINDING_TASK_IDLE;
	this->m_bNodeAddress = this->m_pBindingFile->GetNodeAddress();
	this->m_bMaxPacketLength = 6;
}
//////////////////////////////////////////////////////////////////////
BOOL CBindingFileFSM::StartReadBindings(void) {
	this->Init();
	this->m_currentTask = FSM_TASK_BINDING_READ;
	this->FSM_handleEvent(FSM_EV_OPERATION_BINDINGTASK_START);
	return TRUE;
}

BOOL CBindingFileFSM::StartWriteBindings(void) {
	this->Init();
	this->m_NROfBindingsToWrite = this->m_pBindingFile->Count();	// V17.00: Uitbreiding van het aantal bindings in een node.
	this->m_currentTask = FSM_TASK_BINDING_WRITE;
	this->FSM_handleEvent(FSM_EV_OPERATION_BINDINGTASK_START);
	return TRUE;
}

BOOL CBindingFileFSM::StartEraseBindings(void) {
	this->Init();
	this->m_currentTask = FSM_TASK_BINDING_ERASE;
	this->FSM_handleEvent(FSM_EV_OPERATION_BINDINGTASK_START);
	return TRUE;
}


/*******************************************************************************************/
/**
 * void CBindingFileFSM::FSM_handleBindingFileEvent(FSM_BINDINGFILE_EVENTS FileEvent) 
 * Afhandelen van de events afkomstig van EV_NODEBINDINGSFILECONTROL
 */
void CBindingFileFSM::FSM_handleBindingFileEvent(FSM_BINDINGFILE_EVENTS FileEvent) {		
	switch (this->m_currentFileState) {
		case FSM_STATE_BINDINGFILE_CLOSE: {
			this->FSM_handleBindingFileEvent_StateFileClosed(FileEvent);
		} break;
		case FSM_STATE_BINDINGFILE_OPEN: {
			this->FSM_handleBindingFileEvent_StateFileOpened(FileEvent);			
		} break;
		default: {
			ASSERT(0);
		} break;
	}
}

BOOL CBindingFileFSM::FSM_handleBindingFileEvent_StateBase(FSM_BINDINGFILE_EVENTS FileEvent) {
	switch(FileEvent) {
		case FSM_EV_SIGNAL_BINDINGFILE_INFO:
			this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ACK);	       
			return TRUE;
        break; 
		case FSM_EV_SIGNAL_BINDINGFILE_ERROR: {									
			this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ERROR);					
			this->m_currentFileState = FSM_STATE_BINDINGFILE_CLOSE;
			this->m_currentBindingEntryState = FSM_STATE_BINDINGENTRY_IDLE;    
			return TRUE;
		} break; 
		default: {
			ASSERT(0);
		} break;
	}
	return FALSE;
}

BOOL CBindingFileFSM::FSM_handleBindingFileEvent_StateFileOpened(FSM_BINDINGFILE_EVENTS FileEvent) {
	switch(FileEvent) {
		case FSM_EV_SIGNAL_BINDINGFILE_CLOSED:
			this->m_currentFileState = FSM_STATE_BINDINGFILE_CLOSE;
			this->m_currentBindingEntryState = FSM_STATE_BINDINGENTRY_IDLE;					
			this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ACK);	
			return TRUE;
		break;	    
		case FSM_EV_SIGNAL_BINDINGFILE_OPENED: 
		case FSM_EV_SIGNAL_BINDINGFILE_ERASED:     
			return TRUE;
        break;
	} 
	return this->FSM_handleBindingFileEvent_StateBase(FileEvent);
}

BOOL CBindingFileFSM::FSM_handleBindingFileEvent_StateFileClosed(FSM_BINDINGFILE_EVENTS FileEvent) {
	switch(FileEvent) {		
		case FSM_EV_SIGNAL_BINDINGFILE_OPENED:				
			this->m_currentFileState=FSM_STATE_BINDINGFILE_OPEN;
			this->m_currentBindingEntryState = FSM_STATE_BINDINGENTRY_IDLE;				
			this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ACK);							
			return TRUE;
		break;		
		case FSM_EV_SIGNAL_BINDINGFILE_ERASED:
       		this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ACK);	
			this->m_currentFileState = FSM_STATE_BINDINGFILE_CLOSE;
       		this->m_currentBindingEntryState = FSM_STATE_BINDINGENTRY_IDLE;
			return TRUE;
		break;				
		case FSM_EV_SIGNAL_BINDINGFILE_CLOSED:    			
			return TRUE;
		break;
	}
	return this->FSM_handleBindingFileEvent_StateBase(FileEvent);
}
/*******************************************************************************************/

/**
 * void CBindingFileFSM::FSM_handleBindingEntryEvent(FSM_BINDINGENTRY_EVENTS EntryEvent)
 *
 * Afhandelen van de events van EV_NODEBINDINGENTRY	
 *
 *  + CurrentBindingEntryState:
 * 	 - FSM_STATE_BINDINGENTRY_IDLE=0,
 *   - FSM_STATE_BINDINGENTRY_READ_BINDING,
 *	 - FSM_STATE_BINDINGENTRY_WRITE_BINDING
 * 
 *  + EntryEvent : 
 *   - FSM_EV_SIGNAL_STATUS_HEADER_READ = 0,
 *	 - FSM_EV_SIGNAL_STATUS_HEADER_WRITTEN,
 *   - FSM_EV_SIGNAL_STATUS_DATA_READ,
 *   - FSM_EV_SIGNAL_STATUS_DATA_WRITTEN,
 *   - FSM_EV_SIGNAL_STATUS_ENTRY_CLEARED,
 *   - FSM_EV_SIGNAL_STATUS_WRITE_DONE,
 *   - FSM_EV_SIGNAL_STATUS_WRITE_CANCEL,
 *   - FSM_EV_SIGNAL_STATUS_DONE_READ,
 *   - FSM_EV_SIGNAL_STATUS_BINDING_ERROR
 *
 *
 */
void CBindingFileFSM::FSM_handleBindingEntryEvent(FSM_BINDINGENTRY_EVENTS EntryEvent) {		
	if (this->m_currentFileState == FSM_STATE_BINDINGFILE_CLOSE) {
		this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ERROR);
		return;
	}	
	switch (this->m_currentBindingEntryState) {
		case FSM_STATE_BINDINGENTRY_IDLE: {
			this->FSM_handleBindingEntryEvent_StateIdle(EntryEvent);
		}break;
		case FSM_STATE_BINDINGENTRY_READ_BINDING: {
			this->FSM_handleBindingEntryEvent_StateReadBinding(EntryEvent);
		}break;
		case FSM_STATE_BINDINGENTRY_WRITE_BINDING: {
			this->FSM_handleBindingEntryEvent_StateWriteBinding(EntryEvent);
		}break;
		default:
			ASSERT(0);
		break;	
	}
}

BOOL CBindingFileFSM::FSM_handleBindingEntryEvent_StateBase(FSM_BINDINGENTRY_EVENTS EntryEvent) {
	switch(EntryEvent) {
		case FSM_EV_SIGNAL_STATUS_BINDING_ERROR: {
			this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ERROR); //20040604				
			CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_CLOSE);					
			this->m_currentFileState = FSM_STATE_BINDINGFILE_CLOSE;
			this->m_currentBindingEntryState = FSM_STATE_BINDINGENTRY_IDLE;    
			return TRUE;
		}break;
		default: {
			ASSERT(0); 
		} break;
	}
	return FALSE;
}

BOOL CBindingFileFSM::FSM_handleBindingEntryEvent_StateIdle(FSM_BINDINGENTRY_EVENTS EntryEvent) {
	switch(EntryEvent) {
		case FSM_EV_SIGNAL_STATUS_HEADER_READ: {
			if (this->m_strBindingString) {																	
				this->m_strBindingString.Format(_T("%06x_%02x_%04x"),this->m_bNodeAddress,this->m_bBindingFlags,this->m_usBindingNR);
			}   											
			CTXNodeBindingEntryMessage::getInstance()->ReadData(this->m_bNodeAddress);					
			this->m_currentBindingEntryState=FSM_STATE_BINDINGENTRY_READ_BINDING;
			return TRUE;
		} break; 				

		case FSM_EV_SIGNAL_STATUS_HEADER_WRITTEN:{						
			if (this->m_strBindingString) {
				this->m_stringPos=14;
				this->m_stringLength=this->m_strBindingString.GetLength();                                              
				if ((this->m_stringLength-this->m_stringPos)>=(this->m_bMaxPacketLength-1)) {														
					CTXNodeBindingEntryMessage::getInstance()->WriteData(this->m_bNodeAddress,this->m_strBindingString.Mid(this->m_stringPos,this->m_bMaxPacketLength-1));
					this->m_stringPos += this->m_bMaxPacketLength-1;
				}
				else {							
					CTXNodeBindingEntryMessage::getInstance()->WriteData(this->m_bNodeAddress,this->m_strBindingString.Mid(this->m_stringPos,this->m_stringLength-this->m_stringPos));
					this->m_stringPos = this->m_stringLength;
				}
				this->m_currentBindingEntryState=FSM_STATE_BINDINGENTRY_WRITE_BINDING;
			}
			else {
				this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ERROR);               
				this->m_currentBindingEntryState=FSM_STATE_BINDINGENTRY_IDLE;
			}					
			return TRUE;
		} break; 
	
		case FSM_EV_SIGNAL_STATUS_DATA_READ:               
		case FSM_EV_SIGNAL_STATUS_DATA_WRITTEN:
		case FSM_EV_SIGNAL_STATUS_ENTRY_CLEARED:
		case FSM_EV_SIGNAL_STATUS_WRITE_DONE:               
		case FSM_EV_SIGNAL_STATUS_WRITE_CANCEL:               
		case FSM_EV_SIGNAL_STATUS_DONE_READ:               
			return TRUE;	// ABSORBE.
		break;		
	} 
	return this->FSM_handleBindingEntryEvent_StateBase(EntryEvent);
}

BOOL CBindingFileFSM::FSM_handleBindingEntryEvent_StateReadBinding(FSM_BINDINGENTRY_EVENTS EntryEvent) {
	switch(EntryEvent) {					           
		case FSM_EV_SIGNAL_STATUS_DATA_READ: {						
			if (this->m_strBindingString) {
				this->m_strBindingString+=this->m_strTempBindingData;                                                                  					
			}
			CTXNodeBindingEntryMessage::getInstance()->ReadData(this->m_bNodeAddress);
			return TRUE;
		} break;                
		case FSM_EV_SIGNAL_STATUS_DONE_READ: {						
			if (this->m_strBindingString) {
				this->m_strBindingString+=this->m_strTempBindingData;						             
			}
			this->m_currentBindingEntryState = FSM_STATE_BINDINGENTRY_IDLE;               
			this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ACK);
			return TRUE;
		} break; 
		case FSM_EV_SIGNAL_STATUS_HEADER_READ:
		case FSM_EV_SIGNAL_STATUS_HEADER_WRITTEN:
		case FSM_EV_SIGNAL_STATUS_DATA_WRITTEN:
		case FSM_EV_SIGNAL_STATUS_ENTRY_CLEARED:
		case FSM_EV_SIGNAL_STATUS_WRITE_DONE:
		case FSM_EV_SIGNAL_STATUS_WRITE_CANCEL: {
			return TRUE;	// ABSORBE.
		} break;
	} // end switch (bEvent);	

	return this->FSM_handleBindingEntryEvent_StateBase(EntryEvent);
}

BOOL CBindingFileFSM::FSM_handleBindingEntryEvent_StateWriteBinding(FSM_BINDINGENTRY_EVENTS EntryEvent) {
	switch(EntryEvent) {
		case FSM_EV_SIGNAL_STATUS_DATA_WRITTEN: {					
			if (this->m_strBindingString) {
				if ((this->m_stringLength-this->m_stringPos)>=(this->m_bMaxPacketLength-1)) {															
					CTXNodeBindingEntryMessage::getInstance()->WriteData(this->m_bNodeAddress,this->m_strBindingString.Mid(this->m_stringPos,this->m_bMaxPacketLength-1));
					this->m_stringPos += this->m_bMaxPacketLength-1;
				}
				else  {
					if (this->m_stringPos == this->m_stringLength) {								
						CTXNodeBindingEntryMessage::getInstance()->WriteDone(this->m_bNodeAddress);										
					}
					else  {																	
						CTXNodeBindingEntryMessage::getInstance()->WriteData(this->m_bNodeAddress,this->m_strBindingString.Mid(this->m_stringPos,this->m_stringLength-this->m_stringPos));
						this->m_stringPos = this->m_stringLength;
					}
				}             
				this->onNotifyParentFSM_NodeBindingsRefreshTimer();	// V17.02: Timer refreshen. 
				this->m_currentBindingEntryState=FSM_STATE_BINDINGENTRY_WRITE_BINDING;
			}
			else  {
				this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ERROR);               
				this->m_currentBindingEntryState=FSM_STATE_BINDINGENTRY_IDLE;
			}    
			return TRUE;
		} break; 

		case FSM_EV_SIGNAL_STATUS_WRITE_DONE: {                     	
			this->m_currentBindingEntryState=FSM_STATE_BINDINGENTRY_IDLE;               
			this->FSM_handleEvent(FSM_EV_SIGNAL_BINDINGTASK_ACK);											
			return TRUE;
		} break; 

		case FSM_EV_SIGNAL_STATUS_HEADER_READ:
		case FSM_EV_SIGNAL_STATUS_HEADER_WRITTEN:
		case FSM_EV_SIGNAL_STATUS_DATA_READ:
		case FSM_EV_SIGNAL_STATUS_ENTRY_CLEARED:
		case FSM_EV_SIGNAL_STATUS_WRITE_CANCEL:
		case FSM_EV_SIGNAL_STATUS_DONE_READ: {
			return TRUE;	// ABSORBE.
		}break;		
	} // end switch (bEvent);

	return this->FSM_handleBindingEntryEvent_StateBase(EntryEvent);
}

/****************************************************************************************************/
/**
 * void CBindingFileFSM::BindingRWFileFSM(FSM_BINDINGTASK_EVENTS TaskEvent)
 *
 * 3 Tasks : 
 *		-FSM_TASK_BINDING_READ
 *		-FSM_TASK_BINDING_WRITE
 *		-Task_binding_erase
 *
 * 2 States :
 *		-FSM_STATE_BINDING_TASK_IDLE : Triggert by EV_START, Goes to BUSY
 *								   Opens the binding file 
 *								   
 *      -FSM_STATE_BINDING_TASK_BUSY : Triggert by EV_ACK, EV_ERROR, Goes to IDLE
 *								   Stays in this state as long as Task is busy.	
 *								   When receiving ACK --> Next binding is Read/Written.
 *
 * 3 Events: TaskEvent
 *		-FSM_EV_OPERATION_BINDINGTASK_START :
 *      -FSM_EV_SIGNAL_BINDINGTASK_ACK :
 *		-FSM_EV_SIGNAL_BINDINGTASK_ERROR :
 *
 *
 */
void CBindingFileFSM::FSM_handleEvent(FSM_BINDINGTASK_EVENTS TaskEvent) {	
	ASSERT(this->m_pBindingFile);
	switch (this->m_currentTask) {
		case FSM_TASK_BINDING_READ: {
			this->FSM_handleEvent_TaskBindingsRead(TaskEvent);
		}break;
		case FSM_TASK_BINDING_WRITE: {
			this->FSM_handleEvent_TaskBindingsWrite(TaskEvent);
		}break;		
		case FSM_TASK_BINDING_ERASE: {
			this->FSM_handleEvent_TaskBindingsErase(TaskEvent);
		}break;
		default: {
			ASSERT(0); 
		} break;
	}
}

void CBindingFileFSM::FSM_handleEvent_TaskBindingsRead(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(this->m_currentTaskState) {
		case FSM_STATE_BINDING_TASK_IDLE: {
			this->FSM_handleEvent_TaskBindingsRead_Idle(TaskEvent);
		} break;				
		case FSM_STATE_BINDING_TASK_BUSY: {
			this->FSM_handleEvent_TaskBindingsRead_Busy(TaskEvent);
		} break;							
	}// end switch CurrentTaskState
}

void CBindingFileFSM::FSM_handleEvent_TaskBindingsRead_Idle(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(TaskEvent) {	
		case FSM_EV_OPERATION_BINDINGTASK_START: {							
			this->m_currentBindingEntryNR = 0;
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_OPEN);						
			CTXNodeBindingFileMessage::getInstance()->OpenBindingFile(this->m_bNodeAddress);
			this->m_currentTaskState = FSM_STATE_BINDING_TASK_BUSY;									
		} break; 
		case FSM_EV_SIGNAL_BINDINGTASK_ACK: {
			this->onNotifyParentFSM_NodeBindingsReadDone(); 
		} break; 
		case FSM_EV_SIGNAL_BINDINGTASK_ERROR: {
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_READ_ERROR_CLOSE);										
			CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);
			this->onNotifyParentFSM_NodeBindingsReadError();
		} break; 
	} // switch(TaskEvent)	
}	

void CBindingFileFSM::FSM_handleEvent_TaskBindingsRead_Busy(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(TaskEvent) {
		case FSM_EV_OPERATION_BINDINGTASK_START:
		break;
		case FSM_EV_SIGNAL_BINDINGTASK_ACK: {							
			// V17.00: Uitbreiding van het aantal bindings in een node.
			TRACE1("\nNr Of BindingsToRead = %d",this->m_NROfBindingsToRead);
			if (this->m_currentBindingEntryNR < this->m_NROfBindingsToRead) {	
				this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_READ_BINDING,this->m_currentBindingEntryNR);															
				if (this->m_currentBindingEntryNR > 0) {
					this->m_pBindingFile->AddString(0000,this->m_strBindingString);
				}																						
				CTXNodeBindingEntryMessage::getInstance()->ReadHeader(this->m_bNodeAddress,this->m_currentBindingEntryNR);
				this->m_currentBindingEntryNR++;
			}
			else {							
				if (this->m_currentBindingEntryNR > 0) {
					this->m_pBindingFile->AddString(0000,this->m_strBindingString);
				}				
				this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_CLOSE);								
				this->m_currentTaskState = FSM_STATE_BINDING_TASK_IDLE;							  
				CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);
			}
			this->onNotifyParentFSM_NodeBindingsRefreshTimer();	
		} break; 
		case FSM_EV_SIGNAL_BINDINGTASK_ERROR: {							
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_READ_ERROR_CLOSE);						 
			CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);
			this->onNotifyParentFSM_NodeBindingsReadError(); 						
		} break;
	} // switch(TaskEvent)
}

/********************************************************************************************************/
/**
 * void CBindingFileFSM::BindingRWFileFSM(FSM_BINDINGTASK_EVENTS TaskEvent)
 * 
 * ---------------------------------------------------------------------------------
 * CurrentTask = FSM_TASK_BINDING_WRITE				 
 * CurrentTaskState = FSM_STATE_BINDING_TASK_IDLE 
 *
 *
 *	- FSM_EV_OPERATION_BINDINGTASK_START : 
 *		+ Open Binding file
 *	 
 * - FSM_EV_SIGNAL_BINDINGTASK_ACK
 *		+ Fire_NotifyParentFSMWriteNodeBindingsDone() --> Trigger parent FSM to go to next binding file.
 *
 *		--> SPECIAL : See (***)
 *		    Closing the binding file FSM_STATE_BINDING_TASK_BUSY + FSM_EV_SIGNAL_BINDINGTASK_ACK
 *			Response causes FSM_EV_SIGNAL_BINDINGTASK_ACK
 *
 * 
 * - FSM_EV_SIGNAL_BINDINGTASK_ERROR
 *		+ Close binding file
 *		+ Fire_OnWriteError() --> Trigger parent FSM to go to error state:
 *								  parent FSM Stops Here !!
 *		 
 *  ---------------------------------------------------------------------------------
 * CurrentTask = FSM_TASK_BINDING_WRITE				 
 * CurrentTaskState = FSM_STATE_BINDING_TASK_BUSY
 *
 * - FSM_EV_OPERATION_BINDINGTASK_START 
 *	
 * - FSM_EV_SIGNAL_BINDINGTASK_ACK
 *		+ Get Next binding string and WriteHeader of new Binding string.
 *		+ If all binding strings have been send :	(***)
 *			+ CurrentTaskState = FSM_STATE_BINDING_TASK_IDLE;
 *			+ Close File()
 *
 * - FSM_EV_SIGNAL_BINDINGTASK_ERROR
 *		+ Close binding file
 *		+ Fire_OnWriteError() --> Trigger parent FSM to go to error state:
 *								  parent FSM Stops Here !!
 *
 *
 */
void CBindingFileFSM::FSM_handleEvent_TaskBindingsWrite(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(this->m_currentTaskState) {				
		case FSM_STATE_BINDING_TASK_IDLE: {
			this->FSM_handleEvent_TaskBindingsWrite_Idle(TaskEvent);
		} break;									
		case FSM_STATE_BINDING_TASK_BUSY: {
			this->FSM_handleEvent_TaskBindingsWrite_Busy(TaskEvent);
		} break;									
	}	
}

void CBindingFileFSM::FSM_handleEvent_TaskBindingsWrite_Idle(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(TaskEvent) {
		case FSM_EV_OPERATION_BINDINGTASK_START: {								
			this->m_currentBindingEntryNR = 0;
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_OPEN);				        							
			CTXNodeBindingFileMessage::getInstance()->OpenBindingFile(this->m_bNodeAddress);
			this->m_currentTaskState = FSM_STATE_BINDING_TASK_BUSY;							
		}break;					
		case FSM_EV_SIGNAL_BINDINGTASK_ACK: {						
			this->onNotifyParentFSM_NodeBindingsWriteDone(); 
		} break; 
		case FSM_EV_SIGNAL_BINDINGTASK_ERROR: {
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_WRITE_ERROR_CLOSE);												  
			CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);
			this->onNotifyParentFSM_NodeBindingsWriteError();
		} break; 
	}
}

void CBindingFileFSM::FSM_handleEvent_TaskBindingsWrite_Busy(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(TaskEvent) {
		case FSM_EV_OPERATION_BINDINGTASK_START:						
		break; 

		case FSM_EV_SIGNAL_BINDINGTASK_ACK:	{
			// V17.00: Uitbreiding van het aantal bindings in een node.
			if (this->m_currentBindingEntryNR < this->m_NROfBindingsToWrite) {																													
				this->m_strBindingString.Empty( );	// BUG-0100
				if (CBindingFileEntryOperations::normalize(this->m_pBindingFile->GetString(this->m_currentBindingEntryNR),&this->m_strBindingString)) {				
					this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_WRITE_BINDING,this->m_currentBindingEntryNR);
					CTXNodeBindingEntryMessage::getInstance()->WriteHeaderBindingIndex(this->m_bNodeAddress,this->m_strBindingString);
					this->m_currentBindingEntryNR++;
					this->onNotifyParentFSM_NodeBindingsRefreshTimer();
				}
				else {
					this->onNotifyParentFSM_NodeBindingsWriteError();	
				}
			}
			else  {								
				this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_CLOSE);
				this->m_currentTaskState = FSM_STATE_BINDING_TASK_IDLE;
				CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);   															
			}							
		}break;

		case FSM_EV_SIGNAL_BINDINGTASK_ERROR: {
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_WRITE_ERROR_CLOSE);						
			CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);
			this->onNotifyParentFSM_NodeBindingsWriteError();
		} break; // EV_READ_BINDING_ERROR
	}// end switch (TaskEvent)
}


/************************************************************************************************/
/**
 * void CBindingFileFSM::BindingRWFileFSM(FSM_BINDINGTASK_EVENTS TaskEvent)
 * 
 * ---------------------------------------------------------------------------------
 * CurrentTask = FSM_TASK_BINDING_ERASE				 
 * CurrentTaskState = FSM_STATE_BINDING_TASK_IDLE 
 *
 *
 *	- FSM_EV_OPERATION_BINDINGTASK_START : 
 *		+ Erase Binding file
 *	 
 * - FSM_EV_SIGNAL_BINDINGTASK_ACK
 *		+ Fire_OnEraseDone() --> Trigger parent FSM to start writing bindings.		 		
 *
 * 
 * - FSM_EV_SIGNAL_BINDINGTASK_ERROR
 *		+ Close binding file
 *		+ Fire_OnWriteError() --> Trigger parent FSM to go to error state:
 *								  parent FSM Stops Here !!
 *		 
 * Special : Always stays in the FSM_STATE_BINDING_TASK_IDLE state !!!
 *
 *  ---------------------------------------------------------------------------------
 * CurrentTask = FSM_TASK_BINDING_ERASE				 
 * CurrentTaskState = FSM_STATE_BINDING_TASK_BUSY
 *
 * - FSM_EV_OPERATION_BINDINGTASK_START 
 *	
 * - FSM_EV_SIGNAL_BINDINGTASK_ACK
 *	
 * - FSM_EV_SIGNAL_BINDINGTASK_ERROR 
 *		+ Close binding file
 *		
 */
void CBindingFileFSM::FSM_handleEvent_TaskBindingsErase(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(this->m_currentTaskState) {
		case FSM_STATE_BINDING_TASK_IDLE: {
			this->FSM_handleEvent_TaskBindingsErase_Idle(TaskEvent);
		} break;					
		case FSM_STATE_BINDING_TASK_BUSY: {
			this->FSM_handleEvent_TaskBindingsErase_Busy(TaskEvent);
		}break;
	} 
}

void CBindingFileFSM::FSM_handleEvent_TaskBindingsErase_Idle(FSM_BINDINGTASK_EVENTS TaskEvent) {
	switch(TaskEvent) {
		case FSM_EV_OPERATION_BINDINGTASK_START: {
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_ERASE_START);			
			CTXNodeBindingFileMessage::getInstance()->EraseBindingFile(this->m_bNodeAddress);
		}break;
		case FSM_EV_SIGNAL_BINDINGTASK_ACK: {
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_ERASE_DONE);								
			this->onNotifyParentFSM_NodeBindingsEraseDone();						
		} break; // EV_READ_BINDING_SUCCESS						
		case FSM_EV_SIGNAL_BINDINGTASK_ERROR: {
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_ERASE_ERROR);				
			CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);  
			this->onNotifyParentFSM_NodeBindingsEraseError();
		} break; // EV_READ_BINDING_ERROR
	}
}

void CBindingFileFSM::FSM_handleEvent_TaskBindingsErase_Busy(FSM_BINDINGTASK_EVENTS TaskEvent) {
	ASSERT(0);
	switch(TaskEvent) {
		case FSM_EV_OPERATION_BINDINGTASK_START:
		case FSM_EV_SIGNAL_BINDINGTASK_ACK:						
		break;
		case FSM_EV_SIGNAL_BINDINGTASK_ERROR: {
			this->displayMsg(this->m_bNodeAddress,INFOMSG_BINDINGFILE_ERASE_ERROR);				 
			CTXNodeBindingFileMessage::getInstance()->CloseBindingFile(this->m_bNodeAddress);   
		} break; // EV_READ_BINDING_ERROR
	}
}
/****************************************************************************************************/

// Helper functie voor display van de events.
void CBindingFileFSM::displayMsg(int nodeAddress,InfoMsg_t id,int param) {
	CString szMessage;						
	bool errorMsg = false;
	switch(id) {
		case INFOMSG_BINDINGFILE_OPEN: {
			szMessage.Format(_T("Node 0x%02x: Open Binding File ..."),nodeAddress);				   
		} break;
		case INFOMSG_BINDINGFILE_CLOSE: {
			szMessage.Format(_T("Node 0x%02x: Close Binding File ..."),nodeAddress);										
		} break;
		case INFOMSG_BINDINGFILE_WRITE_BINDING: {
			szMessage.Format(_T("Node 0x%02x: Write binding %d"),nodeAddress,param);												
		} break;
		case INFOMSG_BINDINGFILE_READ_BINDING: {
			szMessage.Format(_T("Node 0x%02x: Read binding %d"),nodeAddress,param);	
		} break;
		case INFOMSG_BINDINGFILE_WRITE_ERROR_CLOSE: {
			szMessage.Format(_T("Node 0x%02x: ERROR writing binding file, closing file ..."),nodeAddress);		
			errorMsg = true;
		} break;
		case INFOMSG_BINDINGFILE_READ_ERROR_CLOSE: {
			szMessage.Format(_T("Node 0x%02x: ERROR reading binding file, closing file ..."),nodeAddress);		
			errorMsg = true;							   
		} break;
		case INFOMSG_BINDINGFILE_ERASE_START: {
			szMessage.Format(_T("Node 0x%02x: Erase bindings in node..."),nodeAddress);					  
		} break;
		case INFOMSG_BINDINGFILE_ERASE_DONE: {
			szMessage.Format(_T("Node 0x%02x: Erase bindings in node done..."),nodeAddress);					 
		} break;
		case INFOMSG_BINDINGFILE_ERASE_ERROR: {
			errorMsg = true;
			szMessage.Format(_T("Node 0x%02x: ERROR erasing binding file, closing file ..."),nodeAddress);		  
		} break;
	}
	if (!szMessage.IsEmpty()) {
		if (errorMsg) {
			CDisplayDevice::DisplayErrorMsg(szMessage);		
		}
		else {
			CDisplayDevice::DisplayInfoMsg(szMessage);		
		}
	}
}