// NodeLCDScreenmapFSM.cpp: implementation of the CNodeLCDScreenmapFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeLCDScreenmapFSM.h"
#include "screenmapid.h"
#include "screenmaps.h"
#include "NodeLCDScreenmapFSMInfo.h"
#include "app\appnodedbase\appnodedbase.h"
#include "NodeLCDScreenmapFSMMaster.h"
#include "NodeLCDScreenmaps.h"
#include "TXNodeMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static void sendSmapInit(BYTE bNodeAddress,const CScreenmapID& screenmapID, const IScreenmap& refSmap) {
	CTXLCDScreenmapMessages::getInstance()->SendInit( bNodeAddress, screenmapID.get(), refSmap.getNrOfEntries(), refSmap.getStartPage());
}

static void sendSmapEntry(BYTE bNodeAddress,const CScreenmapID& screenmapID, const CScreenmap::ENTRY& refEntry) {
	CTXLCDScreenmapMessages::getInstance()->SendEntry(bNodeAddress,screenmapID.get(), refEntry.bPageNr, refEntry.bButtonNr,
				refEntry.bNodeAddress, refEntry.bUnitAddress);
}

static void sendSmapDone(BYTE bNodeAddress,const CScreenmapID& screenmapID) {
	CTXLCDScreenmapMessages::getInstance()->SendDone(bNodeAddress, screenmapID.get());
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CNodeLCDScreenmapFSM::CNodeLCDScreenmapFSM(INodeLCDScreenmapFSMMaster* pParent, CNodeLcdScreenmaps *pCurrent,const CScreenmapIDListIterator& refSmapIDIterator) :
m_pParent(pParent), 
m_pCurrNodeLCDScreenmaps(pCurrent),
m_ptrScreenmap( 0 ),
m_SmapIDIterator( refSmapIDIterator ),
m_iEntryNr( 0 ), 
m_retryCounter( 0 )
{
#ifdef _DEBUG
	typedef CScreenmapIDListIterator Iterator;
	for (Iterator it(refSmapIDIterator);!it.IsDone( );it.Next()) {
		 TRACE1("Screenmap ID=%d\n",it.CurrentItem().get( ));		
	}
#endif
}

CNodeLCDScreenmapFSM::~CNodeLCDScreenmapFSM() { }

//////////////////////////////////////////////////////////////////////

BOOL CNodeLCDScreenmapFSM::UpdateLCD() {	
	m_state = FSM_STATE_IDLE;	
	FSM(FSM_EVENT_OPERATION_START);				
	return TRUE;
}

BOOL CNodeLCDScreenmapFSM::CancelUpdateLCD() {
	FSM(FSM_EVENT_OPERATION_CANCEL);
	return TRUE;
}

void CNodeLCDScreenmapFSM::sendNextSmap() {
	m_SmapIDIterator.Next();	
	if (!m_SmapIDIterator.IsDone()) {															
		const CScreenmapID& nextScreenmapID = m_SmapIDIterator.CurrentItem( );																						
		if ((m_ptrScreenmap = m_pCurrNodeLCDScreenmaps->FindScreenmap(nextScreenmapID)) != NULL) {					
			const BYTE bNodeAddress = m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress;
			m_iEntryNr = 0;	
			m_retryCounter = 0;
			m_pParent->Notify_OnStartScreenmap(nextScreenmapID.get());		
			sendSmapInit(bNodeAddress,nextScreenmapID,*m_ptrScreenmap);
			m_pParent->Start_TimeoutTimer();
			m_state = FSM_STATE_WAIT_FOR_INIT_OK;									
		}
		else  {											
			m_pParent->Event_ScreenmapDoneError();
		}									
	}											
	else {									
		m_pParent->Event_ScreenmapDoneOk();								
	}												
}
// ----------------------------------------------------------------------
void CNodeLCDScreenmapFSM::FSM(FSM_EVENTS e) {	
	if ((m_pParent != NULL) && (m_pCurrNodeLCDScreenmaps != NULL)) {
		switch (m_state) {
			case FSM_STATE_IDLE: {
				switch (e) {
					case FSM_EVENT_OPERATION_START : {								
						for (m_SmapIDIterator.Begin();!m_SmapIDIterator.IsDone();m_SmapIDIterator.Next()) {
							const CScreenmapID& screenmapID = m_SmapIDIterator.CurrentItem( );
							m_ptrScreenmap = m_pCurrNodeLCDScreenmaps->FindScreenmap(screenmapID);
							if (m_ptrScreenmap != NULL) {	
								const BYTE bNodeAddress = m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress;										
								m_iEntryNr = 0;		
								m_retryCounter = 0;
								m_pParent->Notify_OnStartScreenmap(screenmapID.get());		
								sendSmapInit(bNodeAddress,screenmapID,*m_ptrScreenmap);								
								m_pParent->Start_TimeoutTimer();
								m_state = FSM_STATE_WAIT_FOR_INIT_OK;
								break;
							}
							else  {
								m_pParent->Event_ScreenmapDoneError();
							}
						}												
					} break;
					case FSM_EVENT_SIGNAL_STATUS_INIT_OK :
					case FSM_EVENT_SIGNAL_STATUS_INIT_ERROR :
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_OK :
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_ERROR:
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_UNIT_NOT_EXCISTING :
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_NODE_NOT_EXCISTING :
					case FSM_EVENT_AFTER_TIMEOUT :
					case FSM_EVENT_SIGNAL_DONE_OK:	
					case FSM_EVENT_SIGNAL_DONE_ERROR: {										
						m_pParent->Stop_TimeoutTimer();								
						m_pParent->Event_ScreenmapDoneError();					
					} break;
					case FSM_EVENT_OPERATION_CANCEL: {
						m_pParent->Stop_TimeoutTimer();								
						m_pParent->Event_ScreenmapDoneCancel();					
						m_state = FSM_STATE_IDLE;
					} break;
				}
			} break;
		
			case FSM_STATE_WAIT_FOR_INIT_OK: {
				switch (e) {
					case FSM_EVENT_OPERATION_START: {
						m_pParent->Stop_TimeoutTimer();									
						m_pParent->Event_ScreenmapDoneError();					
					} break;
					case FSM_EVENT_SIGNAL_STATUS_INIT_OK : {
						const CScreenmapID& screenmapID = m_SmapIDIterator.CurrentItem( );
						m_pParent->Stop_TimeoutTimer();				
						m_pParent->Notify_OnInitScreenmapDone(screenmapID.get( ));										
						if (m_ptrScreenmap != NULL)  {					
							if (m_iEntryNr < m_ptrScreenmap->getNrOfEntries()) {
								const BYTE bNodeAddress = m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress;														
								const CScreenmap::ENTRY& screenmapEntry = m_ptrScreenmap->getEntry(m_iEntryNr);
								m_pParent->Notify_OnWriteScreenmapEntry(screenmapID.get( ),m_iEntryNr,m_ptrScreenmap->getNrOfEntries());																							
								sendSmapEntry(bNodeAddress,screenmapID,screenmapEntry);																	
								m_pParent->Start_TimeoutTimer();
								this->m_retryCounter = RETRYCOUNTER_RELOADVALUE_ENTRY;		// Sedert V16.30.
								m_state = FSM_STATE_WAIT_FOR_ENTRY_OK;
							}	
							else  {											
								if (m_pCurrNodeLCDScreenmaps->GetAssignedNode()->GetSoftwareVersion()==0x66) {
									// Indien Nieuwe touchscreen (= Software version 0x66)
									const BYTE bNodeAddress = m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress;	
									sendSmapDone(bNodeAddress,screenmapID);
									m_pParent->Start_TimeoutTimer();
									m_state = FSM_STATE_WAIT_FOR_DONE_OK;									
								}
								else {
									sendNextSmap();
								}								
							}
						}
					} break;
					case FSM_EVENT_SIGNAL_STATUS_INIT_ERROR: {
						m_pParent->Stop_TimeoutTimer();				
						if (m_ptrScreenmap != NULL) {				
							const CScreenmapID& screenmapID = m_SmapIDIterator.CurrentItem( );
							m_pParent->Notify_OnInitScreenmapError(screenmapID.get());											
						}					
						m_pParent->Event_ScreenmapDoneError();					
					} break;
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_OK :
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_ERROR:
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_UNIT_NOT_EXCISTING :
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_NODE_NOT_EXCISTING : {
						m_pParent->Stop_TimeoutTimer();				
						m_pParent->Event_ScreenmapDoneError();					
					} break;				
					case FSM_EVENT_SIGNAL_DONE_OK:		
					case FSM_EVENT_SIGNAL_DONE_ERROR: {
						m_pParent->Stop_TimeoutTimer();				
						m_pParent->Event_ScreenmapDoneError();	
					} break;
					case FSM_EVENT_AFTER_TIMEOUT: {
						m_pParent->Stop_TimeoutTimer();									
						m_pParent->Event_ScreenmapDoneError();					
					} break;
					case FSM_EVENT_OPERATION_CANCEL: {
						m_pParent->Stop_TimeoutTimer();								
						m_pParent->Event_ScreenmapDoneCancel();					
						m_state = FSM_STATE_IDLE;
					} break;
				}
			} break;		

			case FSM_STATE_WAIT_FOR_ENTRY_OK: {
				switch (e) {
					case FSM_EVENT_OPERATION_START :
					case FSM_EVENT_SIGNAL_STATUS_INIT_OK :
					case FSM_EVENT_SIGNAL_STATUS_INIT_ERROR: {
						m_pParent->Stop_TimeoutTimer();										
						m_pParent->Event_ScreenmapDoneError();										
					} break;
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_OK: {
						m_pParent->Stop_TimeoutTimer();												
						if (m_ptrScreenmap != NULL) {		
							const CScreenmapID& screenmapID = m_SmapIDIterator.CurrentItem( );
							const int nrOfEntries = m_ptrScreenmap->getNrOfEntries();
							m_pParent->Notify_OnWriteScreenmapEntryDone(screenmapID.get( ),m_iEntryNr,nrOfEntries);												
							if (++m_iEntryNr < nrOfEntries)  {														
								const BYTE bNodeAddress = m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress;								
								const CScreenmap::ENTRY& screenmapEntry = m_ptrScreenmap->getEntry(m_iEntryNr);
								m_pParent->Notify_OnWriteScreenmapEntry(screenmapID.get(),m_iEntryNr,nrOfEntries);	
								this->m_retryCounter = RETRYCOUNTER_RELOADVALUE_ENTRY;		// Sedert V16.30.
								sendSmapEntry(bNodeAddress,screenmapID,screenmapEntry);																		
								m_pParent->Start_TimeoutTimer();		
							}
							else  {									
								if (m_pCurrNodeLCDScreenmaps->GetAssignedNode()->GetSoftwareVersion()==0x66) {
									// Indien Nieuwe touchscreen (= Software version 0x66)
									const BYTE bNodeAddress = m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress;	
									sendSmapDone(bNodeAddress,screenmapID);
									m_pParent->Start_TimeoutTimer();
									m_state = FSM_STATE_WAIT_FOR_DONE_OK;									
								}
								else {
									sendNextSmap();
								}	
							}
						}
						else  {						
							m_pParent->Event_ScreenmapDoneError();	
						}					
					} break;
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_ERROR: {
						m_pParent->Stop_TimeoutTimer();	
						if (m_ptrScreenmap != NULL) {		
							const CScreenmapID& screenmapID = m_SmapIDIterator.CurrentItem( );
							m_pParent->Notify_OnWriteScreenmapEntryError(screenmapID.get( ),m_iEntryNr,m_ptrScreenmap->getNrOfEntries());						
						}				
						m_pParent->Event_ScreenmapDoneError();				
					} break;
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_UNIT_NOT_EXCISTING: {
						m_pParent->Stop_TimeoutTimer();	
						if (m_ptrScreenmap != NULL) {						
							const CScreenmap::ENTRY& screenmapEntry = m_ptrScreenmap->getEntry(m_iEntryNr);
							const CScreenmapID& screenmapID = m_SmapIDIterator.CurrentItem( );
							m_pParent->Notify_OnScreenmapErrorNode(screenmapID.get( ), screenmapEntry.bNodeAddress);						
						}					
						m_pParent->Event_ScreenmapDoneError();					
					} break;
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_NODE_NOT_EXCISTING: {
						m_pParent->Stop_TimeoutTimer();											
						if (m_ptrScreenmap != NULL) {	
							const CScreenmap::ENTRY& screenmapEntry = m_ptrScreenmap->getEntry(m_iEntryNr);
							const CScreenmapID& screenmapID = m_SmapIDIterator.CurrentItem( );
							m_pParent->Notify_OnScreenmapErrorUnit(screenmapID.get( ), screenmapEntry.bNodeAddress,screenmapEntry.bUnitAddress);						
						}				
						m_pParent->Event_ScreenmapDoneError();					
					} break;				
					case FSM_EVENT_AFTER_TIMEOUT: {
						m_pParent->Stop_TimeoutTimer();											
						if (--this->m_retryCounter > 0) {
							m_pParent->Start_TimeoutTimer();
						}
						else {
							m_pParent->Event_ScreenmapDoneError();
						}
					} break;
					case FSM_EVENT_SIGNAL_DONE_OK:		
					case FSM_EVENT_SIGNAL_DONE_ERROR: {
						m_pParent->Stop_TimeoutTimer();				
						m_pParent->Event_ScreenmapDoneError();	
					} break;
					case FSM_EVENT_OPERATION_CANCEL: {
						m_pParent->Stop_TimeoutTimer();									
						m_pParent->Event_ScreenmapDoneCancel();					
						m_state = FSM_STATE_IDLE;	
					} break;
				}
			} break; /* FSM_STATE_WAIT_FOR_ENTRY_OK */

			case FSM_STATE_WAIT_FOR_DONE_OK: {
				// TOESTAND VOOR DE NIEUW TOUCHSCREEN - V66 nodes
				switch (e) {
					case FSM_EVENT_SIGNAL_DONE_OK: {
						// Sedert 0x0E07: Kregen een assert failure omdat de tmr expired
						m_pParent->Stop_TimeoutTimer();	 
						sendNextSmap();
					} break;
					case FSM_EVENT_AFTER_TIMEOUT: {
						// Bij een timeout verder doen met de smap...
						m_pParent->Stop_TimeoutTimer();		
						sendNextSmap();
					} break;
					case FSM_EVENT_SIGNAL_DONE_ERROR:		
					case FSM_EVENT_SIGNAL_STATUS_INIT_OK:
					case FSM_EVENT_SIGNAL_STATUS_INIT_ERROR:
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_OK:
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_ERROR:
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_UNIT_NOT_EXCISTING:
					case FSM_EVENT_SIGNAL_STATUS_ENTRY_NODE_NOT_EXCISTING: {
						m_pParent->Stop_TimeoutTimer();				
						m_pParent->Event_ScreenmapDoneError();	
					} break;	
					case FSM_EVENT_OPERATION_CANCEL: {
						m_pParent->Stop_TimeoutTimer();									
						m_pParent->Event_ScreenmapDoneCancel();					
						m_state = FSM_STATE_IDLE;	
					} break;
				}
			} break; /* FSM_STATE_WAIT_FOR_DONE_OK */
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
















