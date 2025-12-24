// MemoryDumpInfo.h: interface for the CMemoryDumpInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYDUMPINFO_H__08EAAB7D_F1A7_4B40_B756_A4F5A770EEBF__INCLUDED_)
#define AFX_MEMORYDUMPINFO_H__08EAAB7D_F1A7_4B40_B756_A4F5A770EEBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//-------------------------------------------------
class CCanNode;
class CNodeDatabase;
//--------------------------------------------------

#define	MEMORYDUMPINFO_FLAG_STATRAM				0x0001
#define MEMORYDUMPINFO_FLAG_DYNRAM				0x0002
#define MEMORYDUMPINFO_FLAG_ALLBITMAPS			0x0004
#define MEMORYDUMPINFO_FLAG_AUDIOBITMAPS		0x0008

#define MEMORYDUMPINFO_FLAG_DOWNLOAD			0x0001
#define MEMORYDUMPINFO_FLAG_UPLOAD				0x0002
#define MEMORYDUMPINFO_FLAG_VERIFY				0x0001

//---------------------------------------------------



class CMemoryDumpFileInfo
{
public:
	CMemoryDumpFileInfo();

public:
	CString GetFileName(void)					{return szFileName;}
	CString GetFileCreationData(void)			{return szCreationDate;}
	BOOL isAvailable(void)						{return fAvailable;}
	BOOL isFileSaved(void)						{return fFileSaved;}
	
	void SetFileName(CString s)					{szFileName = s;}
	void SetFileCreationDate(CString s)			{szCreationDate = s;}
	void SetAvailable(BOOL flag)				{fAvailable = flag;}
	void SetFileSaved(BOOL flag)				{fFileSaved = flag;}
	
	void Clear(void);
	
protected:
	BOOL fAvailable;							// file in directory
	BOOL fFileSaved;							// file is saved after verify was ok
	
	CString szFileName;
	CString szCreationDate;
};


class CMemoryDumpInfoVerify
{

public:
	void Clear(void);
	//void CalculateMaxValues(CNodeDatabase *pNodeDatabase,CCanNode *pNode);

public:
	void Set_VerifyErrors(int i) {
		nVerifyErrors = i;
	}
	void Set_EmptyMemoryBlocks(int i) {
		nEmptyMemoryBlocks = i;
	}
	void Set_DifferentSourceAddress(BOOL f) {
		fDiffSourceAddress = f;
	}
	void Set_DifferentMemoryLength(BOOL f) {
		fDiffMemoryLength = f;
	}
	int Get_VerifyErrors(void) {
		return (nVerifyErrors);
	}
	int Get_EmptyMemoryBlocks(void) {
		return (nEmptyMemoryBlocks);
	}
	BOOL Get_DifferentSourceAddress(void) {
		return fDiffSourceAddress;
	}
	BOOL Get_DifferentMemoryLength(void) {
		return fDiffMemoryLength;
	}

protected:
	int nVerifyErrors;
	int nEmptyMemoryBlocks;
	BOOL fDiffSourceAddress;
	BOOL fDiffMemoryLength;
};

//----------------------------------------------------------
typedef enum {
	MEMORYDUMPTASKINFO_STATE_IDLE	= 0,	
	MEMORYDUMPTASKINFO_STATE_START,	
	MEMORYDUMPTASKINFO_STATE_DONE,
	MEMORYDUMPTASKINFO_STATE_ERROR,
	MEMORYDUMPTASKINFO_STATE_CANCEL
}ENUM_MEMORYDUMPTASKINFO_STATE;

typedef enum {
	MEMORYDUMPTASKINFO_VERIFY_IDLE = 0,
	MEMORYDUMPTASKINFO_VERIFY_OK,
	MEMORYDUMPTASKINFO_VERIFY_ERROR

}ENUM_MEMORYDUMPTASKINFO_VERIFY;

class CMemoryDumpTaskInfo
{
protected:
	
	ENUM_MEMORYDUMPTASKINFO_STATE TaskState;
	ENUM_MEMORYDUMPTASKINFO_VERIFY VerifyState;
	CString szVerifyString;
	CString szSubString;

public:

	void Clear(void);

	void SetTaskState(ENUM_MEMORYDUMPTASKINFO_STATE state) {
		TaskState = state;
	}

	BOOL IsTaskStarted(void) {
		if (TaskState!=MEMORYDUMPTASKINFO_STATE_IDLE) return TRUE;
		return FALSE;
	}

	BOOL IsTaskDone(void) {
		if (TaskState==MEMORYDUMPTASKINFO_STATE_DONE) return TRUE;
		return FALSE;
	}

	void FormatVerifyString(ENUM_MEMORYDUMPTASKINFO_VERIFY state,CMemoryDumpInfoVerify *pVerifyInfo);

	CString GetTaskString(void);
	CString GetVerifyString(void);
	void SetTaskIDString(CString s) { 
		szSubString = s; 
	}
};



typedef enum {
	MEMORYDUMPINFO_TYPE_DYNRAM = 0,
	MEMORYDUMPINFO_TYPE_STATRAM,
	MEMORYDUMPINFO_TYPE_ALLBMP,
	MEMORYDUMPINFO_TYPE_AUDIOBMP,
	MEMORYDUMPINFO_TYPE_MAX

}ENUM_MEMORYDUMPINFO_TYPE;

typedef enum {
	MEMORYDUMPINFO_TASK_DOWNLOAD = 0,
	MEMORYDUMPINFO_TASK_UPLOAD,
	MEMORYDUMPINFO_TASK_VERIFY,
	MEMORYDUMPINFO_TASK_NOTASK 

}ENUM_MEMORYDUMPINFO_TASK;

//-----------------------------------------------------------
class CMemoryDumpInfo;
//-----------------------------------------------------------

class CMemoryDumpLog
{
protected:

	CString szHeader;

	BOOL fInfoAvailable;

	BYTE bSoftwareVersion;
	unsigned long ulPhysicalAddress;
	BYTE bLCDType;
	int iVerifyErrors;
	int iEmptyMemBlocks;

public:

	void Set_Available(BOOL flag)		{fInfoAvailable = flag; };
	BOOL IsInfoAvailable(void)			{return fInfoAvailable;};

	void Set_SoftwareVersion(BYTE val)	{bSoftwareVersion = val; };
	BYTE Get_SoftwareVersion(void)		{return bSoftwareVersion; };

	void Set_PhysicalAddress(unsigned long val)	{ulPhysicalAddress=val; };
	unsigned long Get_PhysicalAddress(void)		{return ulPhysicalAddress; };

	void Set_LcdType(BYTE val)			{bLCDType =val;};
	BYTE Get_LcdType(void)				{return bLCDType;};

	void Set_nVerifyErrors(int val)		{iVerifyErrors=val;};
	int Get_nVerifyErrors(void)		{return iVerifyErrors;};

	void Set_nMemoryErrors(int val)		{iEmptyMemBlocks=val;};
	int Get_nMemoryErrors(void)		{return iEmptyMemBlocks;};

	void Set_Header(CString s)			{szHeader = s;		};
	CString Get_Header(void)			{return szHeader;	};

public:
	void Clear(void) {
		szHeader.Empty();
		fInfoAvailable = FALSE;
		bSoftwareVersion = 0;
		ulPhysicalAddress = 0;
		bLCDType = 0;
		iVerifyErrors = 0;
		iEmptyMemBlocks = 0;
	}
};


class CMemoryDumpLogFile
{

protected:
	CString szError;
	CString szFileName;
	CCanNode *pAssignedNode;

	CMemoryDumpLog MemoryTypeLog[MEMORYDUMPINFO_TYPE_MAX];

public:

	CMemoryDumpLog* GetLogFile(ENUM_MEMORYDUMPINFO_TYPE type) {
		if (type < MEMORYDUMPINFO_TYPE_MAX) return (&MemoryTypeLog[type]);
		return NULL;
	}

	void SetFileName(CString s)			{szFileName = s;};
	void AssignNode(CCanNode *pNode) {
		pAssignedNode = pNode;
	}

	void Clear(void);
	BOOL Load(void);
	BOOL Save(void);
	BOOL UpdateData(CMemoryDumpInfo *pInfo);
	BOOL CheckNode(ENUM_MEMORYDUMPINFO_TYPE type);

	CString GetErrorMessage(void);
};


//----------------------------------------------------------

class CMemoryDumpInfo  
{
public:
	static CMemoryDumpInfo* GetInstance();

protected:
	CMemoryDumpInfo();

public:
	virtual ~CMemoryDumpInfo();

protected:
	BOOL fMemoryLock;
	BYTE bNodeAddress;

	//CCanNode *pAssignedNode;

	CMemoryDumpFileInfo MemoryFileInfo[MEMORYDUMPINFO_TYPE_MAX];
	CMemoryDumpLogFile  MemoryDumpLogFile;

	unsigned short usMemoryTypeFlags;
	unsigned short usMemoryTaskFlags;
	unsigned short usMemoryTypeSaved;
	
	CMemoryDumpInfoVerify RuntimeVerifyInfo[MEMORYDUMPINFO_TYPE_MAX];
	CMemoryDumpInfoVerify ConfigVerifyInfo[MEMORYDUMPINFO_TYPE_MAX];
	CMemoryDumpTaskInfo TaskInfo[MEMORYDUMPINFO_TASK_NOTASK][MEMORYDUMPINFO_TYPE_MAX];


protected:
	void InitVerifyErrors(CNodeDatabase *pNodeDatabase,CCanNode *pNode);

public:

	void Clear(void);
	
	void SetLock(BOOL fLock){
		fMemoryLock = fLock;
	};
	
	BOOL IsLocked(void){
		return fMemoryLock;
	};

	BOOL AssignNode(CNodeDatabase *pNodeDatabase,CCanNode *pNode);
	BYTE GetAssignedNodeAddress(void) {
		return bNodeAddress;
	}

	//CCanNode* GetAssignedNode(void) {
	//	return pAssignedNode;
	//}

	void SetTask(ENUM_MEMORYDUMPINFO_TASK task,BOOL flag=TRUE);
	void SetTaskMemoryType(ENUM_MEMORYDUMPINFO_TYPE type,BOOL flag=TRUE);
	void SetTaskMemoryTypeOk(ENUM_MEMORYDUMPINFO_TYPE type,BOOL flag);
	BOOL IsTaskMemoryTypeOk(ENUM_MEMORYDUMPINFO_TYPE type);

	BOOL IsUpload(ENUM_MEMORYDUMPINFO_TYPE type);
	BOOL IsDownload(ENUM_MEMORYDUMPINFO_TYPE type);
	BOOL IsVerify(ENUM_MEMORYDUMPINFO_TYPE type);
	
	BOOL IsUpload(void);
	BOOL IsDownload(void);
	BOOL IsVerify(void);

	//BOOL AllMemoryDownloadedAndSaved(void);

	CMemoryDumpFileInfo* GetFileInfo(ENUM_MEMORYDUMPINFO_TYPE type);
	CMemoryDumpInfoVerify* GetConfigVerifyInfo(ENUM_MEMORYDUMPINFO_TYPE type);
	CMemoryDumpInfoVerify* GetRTVerifyInfo(ENUM_MEMORYDUMPINFO_TYPE type);
	CMemoryDumpTaskInfo* GetTaskInfo(ENUM_MEMORYDUMPINFO_TASK task,ENUM_MEMORYDUMPINFO_TYPE type);
	
/*
	CMemoryDumpLog* GetLogFile(ENUM_MEMORYDUMPINFO_TYPE type) {
		if (type<MEMORYDUMPINFO_TYPE_MAX) return (&MemoryDumpLogFile[type]);
		return NULL;
	}
*/
	CMemoryDumpLog* GetLogFile(ENUM_MEMORYDUMPINFO_TYPE type);
	CMemoryDumpLogFile* GetLogFile(void) {
		return (&MemoryDumpLogFile);
	}
	
	void ClearTaskInfo(void);
	BOOL StoreLogFile(void);
	
};

#endif // !defined(AFX_MEMORYDUMPINFO_H__08EAAB7D_F1A7_4B40_B756_A4F5A770EEBF__INCLUDED_)
