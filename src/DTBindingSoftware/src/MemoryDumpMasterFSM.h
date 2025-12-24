// MemoryDumpMasterFSM.h: interface for the CMemoryDumpMasterFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYDUMPMASTERFSM_H__72A68CB7_392E_466D_BFF2_6EBBDCA50653__INCLUDED_)
#define AFX_MEMORYDUMPMASTERFSM_H__72A68CB7_392E_466D_BFF2_6EBBDCA50653__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ---------------------------------------------------------------
class CCanNode;
class IComApplication;
class CMemoryBitmapInfo;
class CMemoryDumpInfo;
class CMemoryDumpTaskInfo;

#include "MemoryDumpFsm.h"
#include "MemoryDumpNodeInfo.h"


// --------------------------------------------------------------
// Interface for CMemoryDumpFSM

class IMemoryDumpMasterFSM
{
public:
	virtual void NotifyOnEvTaskDone(void)						=0;
	virtual void NotifyOnEvTaskError(void)						=0;
	virtual void NotifyOnEvTaskStarted(void)					=0;
	virtual void NotifyOnEvMemoryDumpFSMInfo(CString s)			=0;

	virtual void Start_AckTimeoutTimer(unsigned short usValue)	=0;
	virtual void Stop_AckTimeoutTimer(void)						=0;
};


class CMemoryDumpMasterFSM : public IMemoryDumpMasterFSM
{

protected:

	IComApplication*		const pComApplication;
	CMemoryBitmapInfo*		pMemoryBitmapInfo;
	CMemoryDumpInfo*		pMemoryDumpInfo;
	CMemoryDumpNodeInfo		MemoryNodeinfo;
	CMemoryDumpTaskInfo*	pMemoryDumpTaskInfo;
	
	typedef enum 
	{	
		FSM_STATE_IDLE = 0,
		FSM_STATE_WAIT_STARTED,
		FSM_STATE_BUSY

	}FSM_STATES;

	typedef enum 
	{		
		FSM_EV_OPERATION_START = 0,
		FSM_EV_OPERATION_CANCEL,
		FSM_EV_SIGNAL_START_OK,
		FSM_EV_SIGNAL_DONE_OK,
		FSM_EV_SIGNAL_DONE_ERROR

	}FSM_EVENTS;

	FSM_STATES CurrentState;

	void FSM(FSM_EVENTS e);


	int iTaskIndex;
	int iNrOfTasks;
		
	void InitTasks(void);
	void ClearTasks(void);

	void InitNodeAddress(CCanNode *pNode);
	
	typedef BOOL (CMemoryDumpMasterFSM::*Action) (BYTE);

	typedef struct 
	{
		Action Actions;
		CString szInfo;
		BYTE bNodeAddress;
		BYTE bBitmapIndex;

	}MEMORYDUMP_ACTIONS;
	
	MEMORYDUMP_ACTIONS *MemoryDumpActions;

	BOOL Task_DownloadDynRam(BYTE );
	BOOL Task_DownloadStatRam(BYTE );
	BOOL Task_DownloadAllBitmaps(BYTE );
	BOOL Task_DownloadAudioBitmaps(BYTE );
	BOOL Task_UploadDynRam(BYTE );
	BOOL Task_UploadStatRam(BYTE );
	BOOL Task_UploadAllBitmaps(BYTE );
	BOOL Task_UploadAudioBitmaps(BYTE );
	BOOL Task_UploadBitmaps(BYTE );

	CMemoryDumpFSM	*m_pMemoryDumpFSM;

	//CMemoryDumpTaskInfo *GetTaskInfo {return pMemoryDumpTaskInfo;};


	// ---------------------------------------------------------
	inline void Fire_OnEvTasksDone(void);
	inline void Fire_OnEvTasksError(void);
	inline void Fire_OnEvTasksCancelled(void);

public:
	CMemoryDumpMasterFSM(IComApplication *pCom);
	virtual ~CMemoryDumpMasterFSM();

	BOOL Cancel(void);

	BOOL StartMemoryDump(CCanNode *pNode,CMemoryDumpInfo *pInfo);
	BOOL StartUploadBitmaps(CCanNode *pNode,CMemoryBitmapInfo *pInfo);

	//void Init(IComApplication *pCom);

	// --------
	void NotifyOnEvTaskDone(void);
	void NotifyOnEvTaskError(void);
	void NotifyOnEvTaskStarted(void);
	void NotifyOnEvTaskCancelled(void);
	void NotifyOnEvMemoryDumpFSMInfo(CString s);
	// -------

	void Start_AckTimeoutTimer(unsigned short usValue);
	void Stop_AckTimeoutTimer(void);

	// --------- EVENTS FROM UPPER-LAYER -----------------
	void EvAfterMemoryDumpTimeout(void);
	void EvSignalStatusMemoryDumpStart(BYTE bNodeAddress);
	void EvSignalStatusMemoryDumpWriteOk(BYTE bNodeAddress);	
	void EvSignalStatusMemoryDumpReadOk(BYTE bNodeAddress,
										BYTE *pbData,
										BYTE bLength,
										BYTE bErrors);

};

#endif // !defined(AFX_MEMORYDUMPMASTERFSM_H__72A68CB7_392E_466D_BFF2_6EBBDCA50653__INCLUDED_)
