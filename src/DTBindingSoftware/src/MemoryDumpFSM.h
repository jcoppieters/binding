// MemoryDumpFSM.h: interface for the CMemoryDumpFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYDUMPFSM_H__13A4B804_936B_43EB_9336_BC8A08FA6C46__INCLUDED_)
#define AFX_MEMORYDUMPFSM_H__13A4B804_936B_43EB_9336_BC8A08FA6C46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// --------------------------------
#include "MemoryDumpNodeInfo.h"

class CTXNodeMessage;
class IMemoryDumpMasterFSM;
class CMemoryDumpInfo;
// ---------------------------------------------

#define DYNAMIC_MEMORY_END_ADDRESS_POINTER	0x00		// top0 address!!!
#define MAX_MEMORY_DUMP_LENGTH				0x80000


typedef struct
{
   unsigned long ulSourceAddress;
   unsigned long ulLength;
   BYTE pbData[MAX_MEMORY_DUMP_LENGTH];

} STRUCT_MEMORY_DUMP;

// ---------------------------------------------

#define VERIFY_ERROR_DIFFERENT_LENGTH			0x01
#define VERIFY_ERROR_DIFFERENT_SOURCEADDRESS	0x02
#define VERIFY_ERROR_FLAG						0x40
#define CHECK_MEMORY_FLAG						0x80

// ----------------------------------------------

class CMemoryDumpFSM 
{
protected:

	BYTE bSelectedNodeAddress;
	IMemoryDumpMasterFSM *pMemoryDumpMasterFSM;
	//CTXNodeMessage *pTXNodeMsg;

	STRUCT_MEMORY_DUMP *ptrMemoryDump;		// via new ...
	STRUCT_MEMORY_DUMP *ptrMemoryVerify;	// via new ...
	STRUCT_MEMORY_DUMP *ptrToMemoryDump;

	unsigned long ulMemoryPointer;			// used for FC_GETMEMORYDATA ...
	unsigned long ulDataArrayIndex;			// used as index for pbData ...
	unsigned long ulMemoryStartAddress;
	unsigned long ulMemoryEndAddress;
	unsigned short usBlockLength;
	BOOL fFirstBlock;

	CMemoryDumpInfo *pMemoryDumpInfo;

protected:

	typedef enum 
	{
		FSM_TASK_DOWNLOAD_DYNRAM = 0,
		FSM_TASK_DOWNLOAD_STATRAM,
		FSM_TASK_DOWNLOAD_AUDIOBITMAPS,
		FSM_TASK_DOWNLOAD_ALLBITMAPS,
		FSM_TASK_UPLOAD_DYNRAM,
		FSM_TASK_UPLOAD_STATRAM,
		FSM_TASK_UPLOAD_AUDIOBITMAPS,
		FSM_TASK_UPLOAD_ALLBITMAPS,
		FSM_TASK_UPLOAD_BITMAP,
		FSM_TASK_VERIFY_DYNRAM,
		FSM_TASK_VERIFY_STATRAM,
		FSM_TASK_VERIFY_AUDIOBITMAPS,
		FSM_TASK_VERIFY_ALLBITMAPS

	} FSM_TASK;

	typedef enum 
	{
		FSM_EV_OPERATION_MEMORYDUMP_START = 0,
		FSM_EV_OPERATION_MEMORYDUMP_CANCEL,
		FSM_EV_SIGNAL_STATUS_MEMORYDUMP_WRITEOK,
		FSM_EV_SIGNAL_STATUS_MEMORYDUMP_READOK,
		FSM_EV_AFTER_MEMORYDUMP_TIMEOUT

	}FSM_EVENTS;

	typedef enum 
	{
		FSM_STATE_MEMORYDUMP_IDLE = 0,
		FSM_STATE_MEMORYDUMP_DOWNLOAD,
		FSM_STATE_MEMORYDUMP_UPLOAD,
		FSM_STATE_MEMORYDUMP_START_DOWNLOAD,
		FSM_STATE_MEMORYDUMP_START_UPLOAD,
		FSM_STATE_MEMORYDUMP_VERIFY,
		FSM_STATE_MEMORYDUMP_START_VERIFY,
		FSM_STATE_MEMORYDUMP_UPLOAD_DONE,
		FSM_STATE_MEMORYDUMP_DOWNLOAD_DONE,
		FSM_STATE_MEMORYDUMP_VERIFY_DONE
	
	}FSM_STATES;


	FSM_STATES			CurrentState;
	FSM_TASK			CurrentTask;

	STRUCT_ADDRESSES	MemoryAddress;
	
// ------
	void FSM(FSM_EVENTS e);

	void Fire_OnMemoryDumpError(void);
	void Fire_OnMemoryDumpDone(void);
	void Fire_OnMemoryDumpStarted(void);
	void Fire_OnMemoryDumpInfo(CString s);
	void Start_TimeoutTimer(void);
	void Stop_TimeoutTimer(void);

// ------
	void Free_AllocatedMemory(void);
	BOOL Init_AllocateMemory(void);

	void CheckVerify(BYTE *bStatus,int *iVerifyErrors,int *iMemoryErrors);
	BOOL VerifyAndSave(void);


	BOOL CheckNodeAddress(BYTE bNodeAddress);
	BOOL CalculateProcentValue(unsigned short usBlockNR,
							   unsigned short usMaxBlockNR,
							   int *iPrecentValue);
// ------

	BOOL Save(void);
	BOOL Load(void);
	BOOL StartUpload(void);
	BOOL StartDownload(void);

public:
	CMemoryDumpFSM(IMemoryDumpMasterFSM *ptr);
	virtual ~CMemoryDumpFSM();

// ---
	void EvAfterMemoryDumpTimeout(void);
	void EvSignalStatusMemoryDumpStart(BYTE bNodeAddress);
	void EvSignalStatusMemoryDumpWriteOk(BYTE bNodeAddress);	
	void EvSignalStatusMemoryDumpReadOk(BYTE bNodeAddress,
									    BYTE *pbData,
								        BYTE bLength,
								        BYTE bErrors);
// ---

	BOOL Start_DownloadStatRam(BYTE bNodeAddress);
	BOOL Start_DownloadDynRam(BYTE bNodeAddress);
	BOOL Start_UploadStatRam(BYTE bNodeAddress);
	BOOL Start_UploadDynRam(BYTE bNodeAddress);
	BOOL Start_DownloadAllBitmaps(BYTE bNodeAddress);
	BOOL Start_UploadAllBitmaps(BYTE bNodeAddress);
	BOOL Start_DownloadAudioBitmaps(BYTE bNodeAddress);
	BOOL Start_UploadAudioBitmaps(BYTE bNodeAddress);
	BOOL Start_UploadBitmap(BYTE bNodeAddress,unsigned long ulMemoryStart,unsigned short usLength,BYTE *pbBitmapData);

	BOOL Cancel(void);

	void InitNode(STRUCT_ADDRESSES *ptrMemoryAddress,
				  CMemoryDumpInfo *ptrMemoryDumpInfo);
};

#endif // !defined(AFX_MEMORYDUMPFSM_H__13A4B804_936B_43EB_9336_BC8A08FA6C46__INCLUDED_)
