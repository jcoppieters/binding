#ifndef INCLUDED_NODELCDSCREENMAPFSMMASTER_H
#define INCLUDED_NODELCDSCREENMAPFSMMASTER_H


///////////////////////////////////////////////////////////////////////
class CCanNode;
class CNodeLcdScreenmaps;
class IComApplication;
class CNodeLCDScreenmapFSM;
///////////////////////////////////////////////////////////////////////


/**
 * Interface for child FSM : CNodeLCDScreenmapFSM
 */
class INodeLCDScreenmapFSMMaster
{

public:

	/**
	 * Notifications from Child FSM ...
	 */
	virtual void Notify_OnStartScreenmap(BYTE bScreenmapID)			= 0;

	virtual void Notify_OnWriteScreenmapEntry(BYTE bScreenmapID,
											  int iEntryNR,
											  int iMaxEntries)		= 0;

	virtual void Notify_OnInitScreenmapDone(BYTE bScreenmapID)		= 0;

	virtual void Notify_OnWriteScreenmapEntryDone(BYTE bScreenmapID,
												  int iEntryNR,
												  int iMaxEntries)	= 0;

	virtual void Notify_OnInitScreenmapError(BYTE bScreenmapID)		= 0;

	virtual void Notify_OnWriteScreenmapEntryError(BYTE bScreenmapID,
												   int iEntryNR,
												   int iMaxEntries)	= 0;

	virtual void Notify_OnScreenmapErrorNode(BYTE bScreenmapID,
											 BYTE bNodeError)		= 0;

	virtual void Notify_OnScreenmapErrorUnit(BYTE bScreenmapID,
											 BYTE bNodeError,
											 BYTE bUnitError)		= 0;



	
	/**
	 * Global Events from child FSM
	 */
	virtual void Event_ScreenmapDoneCancel()						= 0;
	virtual void Event_ScreenmapDoneOk()							= 0;
	virtual	void Event_ScreenmapDoneError()							= 0;

	/**
	 *	Trigger timer from Child FSM ....
	 */
	virtual void Start_TimeoutTimer()								= 0;
	virtual	void Stop_TimeoutTimer()								= 0;
};




/**
 * @class	CNodeLCDScreenmapFSMMaster
 * @brief	Master FSM klasse.
 *
 */
class CNodeLCDScreenmapFSMMaster : public INodeLCDScreenmapFSMMaster
{
protected:

	typedef enum
	{
		STATE_SCREENMAP_NODE_IDLE = 0,
		STATE_SCREENMAP_NODE_BUSY

	} ENUM_STATE_SCREENMAP_NODE;

	typedef enum
	{
		FSM_EVENT_OPERATION_NODE_START = 0,
		FSM_EVENT_SIGNAL_NODE_DONE,
		FSM_EVENT_SIGNAL_NODE_ERROR,
		FSM_EVENT_SIGNAL_NODE_CANCEL	

	} ENUM_EVENT_SCREENMAP_NODE;

	ENUM_STATE_SCREENMAP_NODE m_currentState;
	int m_iCurrentNode;
	CNodeLcdScreenmaps* m_pCurrNodeLCDScreenmaps;
	IComApplication* const m_pComApplication;
	CNodeLCDScreenmapFSM* m_pChildFSM;

protected:

	void FSM(ENUM_EVENT_SCREENMAP_NODE e);
	void Trigger(ENUM_EVENT_SCREENMAP_NODE event);
	void DestroyChild();
	void DestroyScreenmap();

	BYTE GetNodeAddress();
	BOOL CheckNodeAddress(BYTE bNodeAddress);
	BOOL CreateNewLCDScreenmap(CCanNode *pNode,CNodeLcdScreenmaps** pReturn);	
	BOOL GetNextLCDCanNode(CCanNode **pReturnNode);

	void Fire_OnScreenmapFSMDone();
	void Fire_OnScreenmapFSMError();
	void Fire_OnScreenmapFSMCancel();

protected:
	void invoke_signalEvent(CNodeLCDScreenmapFSM* pChildFSM,BYTE bNodeAddress,void (CNodeLCDScreenmapFSM::*pf)());

public:
	CNodeLCDScreenmapFSMMaster(IComApplication *pCom);
	virtual ~CNodeLCDScreenmapFSMMaster();

	BOOL CancelUpdateLCD(void);
	BOOL Update();


	// Events from CWND to trigger Parent FSM...
	void OnEventCancel() {
		FSM(FSM_EVENT_SIGNAL_NODE_CANCEL);
	}
	void OnEventDone() {
		FSM(FSM_EVENT_SIGNAL_NODE_DONE);
	}
	void OnEventError() {
		FSM(FSM_EVENT_SIGNAL_NODE_ERROR);
	}


	// Events from parent FSM --> Forward to the child FSM
	void EvSignalStatusInitDone(BYTE bNodeAddress);
	void EvSignalStatusInitError(BYTE bNodeAddress);
	void EvSignalStatusEntryDone(BYTE bNodeAddress);
	void EvSignalStatusEntryError(BYTE bNodeAddress);
	void EvSignalStatusNodeNotFound(BYTE bNodeAddress);
	void EvSignalStatusUnitNotFound(BYTE bNodeAddress);
	void EvAfterTimeout(void);	
	// Voor de nieuwe touchscreen.
	void EvSignalStatusDoneOk(BYTE bNodeAddress);
	void EvSignalStatusDoneError(BYTE bNodeAddress);

	// Notify - Events from Child FSM...
	void Notify_OnStartScreenmap(BYTE bScreenmapID);
	void Notify_OnWriteScreenmapEntry(BYTE bScreenmapID,int iEntryNR,int iMaxEntries);
	void Notify_OnInitScreenmapDone(BYTE bScreenmapID);
	void Notify_OnWriteScreenmapEntryDone(BYTE bScreenmapID,int iEntryNR,int iMaxEntries);
	void Notify_OnInitScreenmapError(BYTE bScreenmapID);
	void Notify_OnWriteScreenmapEntryError(BYTE bScreenmapID,int iEntryNR,int iMaxEntries);	
	void Notify_OnScreenmapErrorNode(BYTE bScreenmapID,BYTE bNodeError);
	void Notify_OnScreenmapErrorUnit(BYTE bScreenmapID,BYTE bNodeError,BYTE bUnitError);
	
	// Global Events from child FSM
	void Event_ScreenmapDoneCancel();
	void Event_ScreenmapDoneOk();
	void Event_ScreenmapDoneError();

	void Start_TimeoutTimer();
	void Stop_TimeoutTimer();
};

////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SCREENMAPSUPDATELCD_H__8139EAEC_F032_48B3_88D7_C68FBD06303C__INCLUDED_)
