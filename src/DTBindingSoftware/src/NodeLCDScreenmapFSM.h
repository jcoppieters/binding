// NodeLCDScreenmapFSM.h: interface for the CNodeLCDScreenmapFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODELCDSCREENMAPFSM_H__F58CB931_E00E_45FA_99A3_1D909D01B9DF__INCLUDED_)
#define AFX_NODELCDSCREENMAPFSM_H__F58CB931_E00E_45FA_99A3_1D909D01B9DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ---------------------------------------
class CNodeLcdScreenmaps;
class IScreenmap;
class INodeLCDScreenmapFSMMaster;
// ---------------------------------------

#ifndef INCLUDED_SCREENMAPLISTID_H
#include "screenmaplistid.h"
#endif

class CNodeLCDScreenmapFSM  
{
protected:	
	typedef enum
	{
		FSM_STATE_IDLE=0, 
		FSM_STATE_WAIT_FOR_INIT_OK, 
		FSM_STATE_WAIT_FOR_ENTRY_OK,
		FSM_STATE_WAIT_FOR_DONE_OK		// Nieuwe touchscreen..
		
	}FSM_STATES;

	typedef enum 
	{
		FSM_EVENT_OPERATION_START = 0,
		FSM_EVENT_OPERATION_CANCEL,
		FSM_EVENT_SIGNAL_STATUS_INIT_OK,
		FSM_EVENT_SIGNAL_STATUS_INIT_ERROR,
		FSM_EVENT_SIGNAL_STATUS_ENTRY_OK,
		FSM_EVENT_SIGNAL_STATUS_ENTRY_ERROR,
		FSM_EVENT_SIGNAL_STATUS_ENTRY_UNIT_NOT_EXCISTING,
		FSM_EVENT_SIGNAL_STATUS_ENTRY_NODE_NOT_EXCISTING,
		FSM_EVENT_AFTER_TIMEOUT,
		// Voor de nieuwe touchscreen - Nodes V66
		FSM_EVENT_SIGNAL_DONE_OK,
		FSM_EVENT_SIGNAL_DONE_ERROR	
	}FSM_EVENTS;

	// Sedert V16.30
	// Aantal keren de timer herstarten wannneer het antwoord op een ADD_ENTRY te laat komt.
	// Gaan er vanuit wanneer een module antwoord op een Screenmap init dat hij aanwezig is 
	// op de bus...
	enum { RETRYCOUNTER_RELOADVALUE_ENTRY  = 3 };			

	IScreenmap*	m_ptrScreenmap;
	INodeLCDScreenmapFSMMaster* const m_pParent;
	CNodeLcdScreenmaps* const m_pCurrNodeLCDScreenmaps;
	int m_iEntryNr;
	CScreenmapIDListIterator m_SmapIDIterator;
	FSM_STATES m_state;
	int m_retryCounter;			// V16.30

public:
	CNodeLCDScreenmapFSM(INodeLCDScreenmapFSMMaster* pParent,CNodeLcdScreenmaps *pCurrent,const CScreenmapIDListIterator& refSmapIDIterator);
	virtual ~CNodeLCDScreenmapFSM();

	BOOL UpdateLCD();
	BOOL CancelUpdateLCD();

	void EvSignalStatusInitDone() {
		FSM(FSM_EVENT_SIGNAL_STATUS_INIT_OK);		
	}
	void EvSignalStatusInitError() {
		FSM(FSM_EVENT_SIGNAL_STATUS_INIT_ERROR);
	}
	void EvSignalStatusEntryDone() {
		FSM(FSM_EVENT_SIGNAL_STATUS_ENTRY_OK);
	}
	void EvSignalStatusEntryError() {
		FSM(FSM_EVENT_SIGNAL_STATUS_ENTRY_ERROR);	
	}
	void EvSignalStatusNodeNotFound() {
		FSM(FSM_EVENT_SIGNAL_STATUS_ENTRY_NODE_NOT_EXCISTING);
	}
	void EvSignalStatusUnitNotFound() {
		FSM(FSM_EVENT_SIGNAL_STATUS_ENTRY_UNIT_NOT_EXCISTING);
	}
	void EvAfterTimeout(void) {
		FSM(FSM_EVENT_AFTER_TIMEOUT);
	}
	void EvSignalStatusDoneOk() {
		FSM(FSM_EVENT_SIGNAL_DONE_OK);
	}
	void EvSignalStatusDoneError() {
		FSM(FSM_EVENT_SIGNAL_DONE_ERROR);
	}

protected:
	void FSM(FSM_EVENTS e);
	void sendNextSmap();
};


#endif // !defined(AFX_NODELCDSCREENMAPFSM_H__F58CB931_E00E_45FA_99A3_1D909D01B9DF__INCLUDED_)
