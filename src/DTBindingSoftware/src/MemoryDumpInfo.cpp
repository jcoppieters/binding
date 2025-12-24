// MemoryDumpInfo.cpp: implementation of the CMemoryDumpInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MemoryDumpInfo.h"

//---------------------------------
#include "NodeDatabase.h"
#include "CanNode.h"
#include "MainFrm.h"
#include "NodeDatabaseOperations.h"

#include "utils/win32/ShellFunctions.h"	
#include "utils/win32/ShellCommands.h"	
#include "utils/win32/NotifyError.h"

// -----------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalSettings;
//------------------------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMemoryDumpFileInfo::CMemoryDumpFileInfo()
{
	Clear();
}

void CMemoryDumpFileInfo::Clear(void)
{
	szFileName.Empty();
	szCreationDate.Empty();
	fAvailable = FALSE;
	fFileSaved = FALSE;
}

///////////////////////////////////////////////////////////////////////

void CMemoryDumpInfoVerify::Clear(void)
{
	nVerifyErrors = 0;
	nEmptyMemoryBlocks = 0xFF;
	fDiffSourceAddress = FALSE;
	fDiffMemoryLength = FALSE;
}

///////////////////////////////////////////////////////////////////////

void CMemoryDumpTaskInfo::Clear(void) 
{
	szVerifyString.Empty();

	TaskState = MEMORYDUMPTASKINFO_STATE_IDLE;
	VerifyState = MEMORYDUMPTASKINFO_VERIFY_IDLE;

	szSubString.Empty();
}

void CMemoryDumpTaskInfo::FormatVerifyString(ENUM_MEMORYDUMPTASKINFO_VERIFY state,CMemoryDumpInfoVerify *pVerifyInfo)
{
	szVerifyString.Empty();
	
	if (pVerifyInfo != NULL) 
	{		
		VerifyState = state;

		switch (state)
		{
			case MEMORYDUMPTASKINFO_VERIFY_IDLE:
				szVerifyString.Format("No verify done");
			break;

			case MEMORYDUMPTASKINFO_VERIFY_OK:
				szVerifyString.Format(_T("Verify ok: %d verify errors, %d empty blocks"),pVerifyInfo->Get_VerifyErrors(),
									  pVerifyInfo->Get_EmptyMemoryBlocks());
			break;

			case MEMORYDUMPTASKINFO_VERIFY_ERROR:

				if (pVerifyInfo->Get_DifferentSourceAddress()) 
				{
					szVerifyString.Format(_T("VERIFY FAILED-FILE NOT SAVED:different source address"));
				}
				else if (pVerifyInfo->Get_DifferentMemoryLength()) 
				{
					szVerifyString.Format(_T("VERIFY FAILED-FILE NOT SAVED:different memory length"));
				}
				else 
				{
					szVerifyString.Format(_T("VERIFY FAILED-FILE NOT SAVED: %d verify errors, %d empty blocks"),pVerifyInfo->Get_VerifyErrors(),
								  		  pVerifyInfo->Get_EmptyMemoryBlocks());
				}

			break;
		}
	}
}

CString CMemoryDumpTaskInfo::GetTaskString(void)
{
	CString szTask,szReturn;
	
	szTask.Empty();
	szReturn.Empty();

	switch (TaskState)
	{
		case MEMORYDUMPTASKINFO_STATE_IDLE:
			szTask.Format("Task not started");
		break;

		case MEMORYDUMPTASKINFO_STATE_START :
			szTask.Format("Task started");
		break;

		case MEMORYDUMPTASKINFO_STATE_DONE :
			szTask.Format("Task done");
		break;

		case MEMORYDUMPTASKINFO_STATE_ERROR :
			szTask.Format("Task Error");
		break;
		
		case MEMORYDUMPTASKINFO_STATE_CANCEL :
			szTask.Format("Task Cancelled by user");
		break;
	}
	szReturn.Format(_T("%s:%s"),szSubString,szTask);
	return szReturn;
}

CString CMemoryDumpTaskInfo::GetVerifyString(void)
{
	CString szReturn;

	szReturn.Format(_T("%s:%s"),szSubString,szVerifyString);
	return szReturn;
}

///////////////////////////////////////////////////////////////////////
void CMemoryDumpLogFile::Clear(void) 
{
	int i;
	for (i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) MemoryTypeLog[i].Clear();

	MemoryTypeLog[MEMORYDUMPINFO_TYPE_DYNRAM].Set_Header("DYNRAM");
	MemoryTypeLog[MEMORYDUMPINFO_TYPE_STATRAM].Set_Header("STATRAM");
	MemoryTypeLog[MEMORYDUMPINFO_TYPE_ALLBMP].Set_Header("ALLBMPRAM");
	MemoryTypeLog[MEMORYDUMPINFO_TYPE_AUDIOBMP].Set_Header("AUDIOBMPRAM");

}

BOOL CMemoryDumpLogFile::Load(void)
{
	CString szStr;
	int i;


	#define MAX_BUFFER_LENGTH	255

	char szbuffer[MAX_BUFFER_LENGTH];
	char szAttribute[MAX_BUFFER_LENGTH];
	char szData[MAX_BUFFER_LENGTH];

	//BYTE bTemp;
	unsigned long ulTemp;
	unsigned int nTemp;
	CMemoryDumpLog *pMemoryType = NULL;

	Clear();

	FILE* fs = fopen((LPCTSTR) szFileName,"r");							// CHANGES_MVS_2008_CSTRING
	if (fs==NULL) return FALSE;
		
	while(fgets(szbuffer,MAX_BUFFER_LENGTH,fs) != NULL) 
	{
		szData[0] = '\0';		
		sscanf(szbuffer,"%[^'=']=%[^\n]",szAttribute,szData);			// CHECKED_SSCANF_OK

		if ((szAttribute[0] != 0x0a) && (szAttribute[0] != 0x20) && (szAttribute[0] != ';')) {
			
			TRACE2("Attribute:%s,Data:%s\n",szAttribute,szData);

			if (!strcmp(szAttribute,"Software")) 
			{
				if (pMemoryType!= NULL) 
				{				
					if (sscanf(szData,"%x;",&nTemp) == 1)				// CHANGES_MVS_2008_SSCANF			
					{
						pMemoryType->Set_SoftwareVersion(nTemp);
					}
					else
					{
						ASSERT( 0 );
					}
				}
			}
			else if (!strcmp(szAttribute,"LcdType")) 
			{
				if (pMemoryType!= NULL) 
				{
					if (sscanf(szData,"%x;",&nTemp) == 1)				// CHANGES_MVS_2008_SSCANF			
					{
						pMemoryType->Set_LcdType(nTemp);
					}
					else
					{
						ASSERT( 0 );
					}
				}
			}
			else if (!strcmp(szAttribute,"Address"))					// CHANGES_MVS_2008_SSCANF		
			{
				if (pMemoryType!= NULL) 
				{
					if (sscanf(szData,"%x;",&ulTemp) == 1)			
					{
						pMemoryType->Set_PhysicalAddress(ulTemp);
					}
					else
					{
						ASSERT( 0 );
					}
				}
			}
			else if (!strcmp(szAttribute,"VerifyE")) 
			{
				if (pMemoryType!= NULL) 
				{
					if (sscanf(szData,"%d;\n",&nTemp) == 1)			// CHECKED_SSCANF_OK
					{
						pMemoryType->Set_nVerifyErrors(nTemp);
					}
					else
					{
						ASSERT( 0 );
					}
				}
			}
			else if (!strcmp(szAttribute,"MemoryE")) 
			{
				if (pMemoryType!= NULL) 
				{
					if (sscanf(szData,"%d;\n",&nTemp) == 1)			// CHECKED_SSCANF_OK
					{
						pMemoryType->Set_nMemoryErrors(nTemp);
					}
					else
					{
						ASSERT( 0 );
					}
				}
			}
			else 
			{
				for (i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) 
				{
					szStr = MemoryTypeLog[i].Get_Header();
					if (!strcmp(szAttribute,szStr)) 
					{
						pMemoryType = &MemoryTypeLog[i];
						if (sscanf(szData,"%d",&nTemp))			// CHANGES_MVS_2008_SSCANF
						{
							pMemoryType->Set_Available((BOOL)nTemp);
						}
						else
						{
							ASSERT( 0 );
						}
						
						break;
					}
				}
			}
		}
			
	}	

	fclose(fs);
	return TRUE;
}

BOOL CMemoryDumpLogFile::Save(void)
{
	CString szStr;
	BOOL fTemp; 
	int i;

	FILE* fd = fopen((LPCTSTR) szFileName,"w");		// CHANGES_MVS_2008_CSTRING
	if (fd == NULL) 
	{
		return FALSE;
	}
	
	for (i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) 
	{	
		// First header of memory ...
		fTemp = MemoryTypeLog[i].IsInfoAvailable();
		szStr.Format(_T("%s=%d;\n"),MemoryTypeLog[i].Get_Header(),fTemp);
		fputs((LPCTSTR)szStr,fd);				// CHANGES_0X0A01_USE_FPUTS	
		
		if (fTemp) 
		{
			// Software Version
			szStr.Format(_T("Software=0x%02x;\n"),MemoryTypeLog[i].Get_SoftwareVersion());
			fputs((LPCTSTR)szStr,fd);			// CHANGES_0X0A01_USE_FPUTS	

			// Lcd Type 
			szStr.Format(_T("LcdType=0x%02x;\n"),MemoryTypeLog[i].Get_LcdType());
			fputs((LPCTSTR)szStr,fd);			// CHANGES_0X0A01_USE_FPUTS	

			// Physical Address
			szStr.Format(_T("Address=0x%06x;\n"),MemoryTypeLog[i].Get_PhysicalAddress());
			fputs((LPCTSTR) szStr,fd);			// CHANGES_0X0A01_USE_FPUTS	

			// VerifyErrors
			szStr.Format(_T("VerifyE=%d;\n"),MemoryTypeLog[i].Get_nVerifyErrors());
			fputs((LPCTSTR) szStr,fd);			// CHANGES_0X0A01_USE_FPUTS	

			// Empty Memory Blocks 
			szStr.Format(_T("MemoryE=%d;\n"),MemoryTypeLog[i].Get_nMemoryErrors());
			fputs((LPCTSTR) szStr,fd);			// CHANGES_0X0A01_USE_FPUTS	
		}		
	}

	fclose(fd);
	return TRUE;
}

BOOL CMemoryDumpLogFile::UpdateData(CMemoryDumpInfo *pInfo)
{
	int i;

	if (pAssignedNode==NULL) return FALSE;
	if (pInfo==NULL) return FALSE;

	CMemoryDumpInfoVerify *pVerifyInfo = NULL;

	if (pInfo->IsDownload()) {
		// Check all Tasks ...
		for (i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) {
		
			if(pInfo->IsTaskMemoryTypeOk((ENUM_MEMORYDUMPINFO_TYPE) i)) {	//File saved and verified?
				pVerifyInfo = NULL;
				pVerifyInfo = pInfo->GetRTVerifyInfo((ENUM_MEMORYDUMPINFO_TYPE) i);
				if (pVerifyInfo != NULL) {
					
					MemoryTypeLog[i].Set_Available(TRUE);

					MemoryTypeLog[i].Set_LcdType(pAssignedNode->bNodeFlags&NODE_LCD_TYPE_MASK);				
					MemoryTypeLog[i].Set_PhysicalAddress(pAssignedNode->ulPhysicalAddress);
					MemoryTypeLog[i].Set_SoftwareVersion(pAssignedNode->GetSoftwareVersion());

					MemoryTypeLog[i].Set_nMemoryErrors(pVerifyInfo->Get_VerifyErrors());
					MemoryTypeLog[i].Set_nMemoryErrors(pVerifyInfo->Get_EmptyMemoryBlocks());					
				}				
			}
		}
	}
	return TRUE;
}

BOOL CMemoryDumpLogFile::CheckNode(ENUM_MEMORYDUMPINFO_TYPE type)
{
	szError.Empty();

	if (pAssignedNode == NULL) return FALSE;

	if (MemoryTypeLog[type].IsInfoAvailable() == TRUE) {
		
		if (pAssignedNode->GetSoftwareVersion() != MemoryTypeLog[type].Get_SoftwareVersion()) {
			
			szError.Format(_T("MemoryBackup of node 0x%06x not compatible because of different software version"),
				MemoryTypeLog[type].Get_PhysicalAddress());
			return FALSE;
		}

		if ((pAssignedNode->bNodeFlags&NODE_LCD_TYPE_MASK) != MemoryTypeLog[type].Get_LcdType()) {
			
			szError.Format(_T("MemoryBackup of node 0x%06x not compatible because of different LCD type"),
				MemoryTypeLog[type].Get_PhysicalAddress());
			return FALSE;			
		}
	}
	return TRUE;
}
CString CMemoryDumpLogFile::GetErrorMessage(void)
{
	return szError;
}

///////////////////////////////////////////////////////////////////////




CMemoryDumpInfo::CMemoryDumpInfo()
{
	Clear();
}

CMemoryDumpInfo::~CMemoryDumpInfo()
{

}

void CMemoryDumpInfo::Clear(void)
{
	fMemoryLock = FALSE;
	bNodeAddress = 0xFF;
	usMemoryTypeFlags=0;
	usMemoryTaskFlags=0;
	usMemoryTypeSaved=0;
}


void CMemoryDumpInfo::SetTask(ENUM_MEMORYDUMPINFO_TASK task,BOOL flag)
{
	switch (task)
	{
		case MEMORYDUMPINFO_TASK_DOWNLOAD :
			if (flag) usMemoryTaskFlags |= MEMORYDUMPINFO_FLAG_DOWNLOAD;
			else usMemoryTaskFlags &= (MEMORYDUMPINFO_FLAG_DOWNLOAD^0xFFFF);
		break;

		case MEMORYDUMPINFO_TASK_UPLOAD :
			if (flag) usMemoryTaskFlags |= MEMORYDUMPINFO_FLAG_UPLOAD;
			else usMemoryTaskFlags &= (MEMORYDUMPINFO_FLAG_UPLOAD^0xFFFF);
		break;

		case MEMORYDUMPINFO_TASK_VERIFY :
			if (flag) usMemoryTaskFlags |= MEMORYDUMPINFO_FLAG_VERIFY;
			else usMemoryTaskFlags &= (MEMORYDUMPINFO_FLAG_VERIFY^0xFFFF);
		break;

		default:
		case MEMORYDUMPINFO_TASK_NOTASK :
			ASSERT(0);
		break;
	}
}

void CMemoryDumpInfo::SetTaskMemoryType(ENUM_MEMORYDUMPINFO_TYPE type,BOOL flag)
{
	switch(type)
	{
		case MEMORYDUMPINFO_TYPE_DYNRAM :
			if (flag) usMemoryTypeFlags |= MEMORYDUMPINFO_FLAG_DYNRAM;
			else usMemoryTypeFlags &= (MEMORYDUMPINFO_FLAG_DYNRAM^0xFFFF);
		break;

		case MEMORYDUMPINFO_TYPE_STATRAM :
			if (flag) usMemoryTypeFlags |= MEMORYDUMPINFO_FLAG_STATRAM;
			else usMemoryTypeFlags &= (MEMORYDUMPINFO_FLAG_STATRAM^0xFFFF);
		break;

		case MEMORYDUMPINFO_TYPE_ALLBMP :
			if (flag) usMemoryTypeFlags |= MEMORYDUMPINFO_FLAG_ALLBITMAPS;
			else usMemoryTypeFlags &= (MEMORYDUMPINFO_FLAG_ALLBITMAPS^0xFFFF);
		break;

		case MEMORYDUMPINFO_TYPE_AUDIOBMP :
			if (flag) usMemoryTypeFlags |= MEMORYDUMPINFO_FLAG_AUDIOBITMAPS;
			else usMemoryTypeFlags &= (MEMORYDUMPINFO_FLAG_AUDIOBITMAPS^0xFFFF);
		break;

		default:
			ASSERT(0);
		break;
	}
}

BOOL CMemoryDumpInfo::IsUpload(void)
{
	if ((usMemoryTaskFlags&MEMORYDUMPINFO_FLAG_UPLOAD)==MEMORYDUMPINFO_FLAG_UPLOAD) 
		return TRUE;
	return FALSE;
}

BOOL CMemoryDumpInfo::IsDownload(void)
{
	if ((usMemoryTaskFlags&MEMORYDUMPINFO_FLAG_DOWNLOAD)==MEMORYDUMPINFO_FLAG_DOWNLOAD) 
		return TRUE;
	return FALSE;
}

BOOL CMemoryDumpInfo::IsVerify(void)
{
	if ((usMemoryTaskFlags&MEMORYDUMPINFO_FLAG_VERIFY)==MEMORYDUMPINFO_FLAG_VERIFY) 
		return TRUE;
	return FALSE;
}

BOOL CMemoryDumpInfo::IsUpload(ENUM_MEMORYDUMPINFO_TYPE type)
{

	if (IsUpload())
	{
		switch(type)
		{
			case MEMORYDUMPINFO_TYPE_DYNRAM :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_DYNRAM) == MEMORYDUMPINFO_FLAG_DYNRAM)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_STATRAM :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_STATRAM) == MEMORYDUMPINFO_FLAG_STATRAM)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_ALLBMP :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_ALLBITMAPS) == MEMORYDUMPINFO_FLAG_ALLBITMAPS)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_AUDIOBMP :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_AUDIOBITMAPS) == MEMORYDUMPINFO_FLAG_AUDIOBITMAPS)
					return TRUE;		
			break;

			default:
				ASSERT(0);
			break;
		}	
	}
	return FALSE;
}

BOOL CMemoryDumpInfo::IsDownload(ENUM_MEMORYDUMPINFO_TYPE type)
{
	if (IsDownload())
	{
		switch(type)
		{
			case MEMORYDUMPINFO_TYPE_DYNRAM :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_DYNRAM) == MEMORYDUMPINFO_FLAG_DYNRAM)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_STATRAM :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_STATRAM) == MEMORYDUMPINFO_FLAG_STATRAM)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_ALLBMP :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_ALLBITMAPS) == MEMORYDUMPINFO_FLAG_ALLBITMAPS)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_AUDIOBMP :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_AUDIOBITMAPS) == MEMORYDUMPINFO_FLAG_AUDIOBITMAPS)
					return TRUE;		
			break;

			default:
				ASSERT(0);
			break;
		}	
	}
	return FALSE;
}

BOOL CMemoryDumpInfo::IsVerify(ENUM_MEMORYDUMPINFO_TYPE type)
{
	if (IsVerify())
	{
		switch(type)
		{
			case MEMORYDUMPINFO_TYPE_DYNRAM :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_DYNRAM) == MEMORYDUMPINFO_FLAG_DYNRAM)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_STATRAM :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_STATRAM) == MEMORYDUMPINFO_FLAG_STATRAM)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_ALLBMP :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_ALLBITMAPS) == MEMORYDUMPINFO_FLAG_ALLBITMAPS)
					return TRUE;		
			break;

			case MEMORYDUMPINFO_TYPE_AUDIOBMP :
				if ((usMemoryTypeFlags&MEMORYDUMPINFO_FLAG_AUDIOBITMAPS) == MEMORYDUMPINFO_FLAG_AUDIOBITMAPS)
					return TRUE;		
			break;

			default:
				ASSERT(0);
			break;
		}	
	}
	return FALSE;
}

BOOL CMemoryDumpInfo::AssignNode(CNodeDatabase *pNodeDatabase,CCanNode *pNode)
{
	CString szTemp;
	CString szFileName;
	int i;

	if (fMemoryLock==TRUE) return FALSE;
	if (pNodeDatabase == NULL) return FALSE;

	Clear();

	bNodeAddress = pNode->bLogicalAddress;

	for (i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) {
		MemoryFileInfo[i].Clear();	
	}	
		

	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());
	CString szSubDir((LPCSTR) IDS_FILENAME_SUBDIRBACKUP);

	szTemp.LoadString(IDS_FILENAME_STATICMEMBACKUP);
	szFileName.Format(_T("%s%s//%s%02x.bin"),szPathName,szSubDir,szTemp,bNodeAddress);
	MemoryFileInfo[MEMORYDUMPINFO_TYPE_STATRAM].SetFileName(szFileName);
	
	if (FindFileDate(szFileName,&szTemp))
	{
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_STATRAM].SetFileCreationDate(szTemp);
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_STATRAM].SetAvailable(TRUE);
	}

	szTemp.LoadString(IDS_FILENAME_DYNMEMBACKUP);
	szFileName.Format(_T("%s%s//%s%02x.bin"),szPathName,szSubDir,szTemp,bNodeAddress);
	MemoryFileInfo[MEMORYDUMPINFO_TYPE_DYNRAM].SetFileName(szFileName);
	if (FindFileDate(szFileName,&szTemp)) 
	{
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_DYNRAM].SetFileCreationDate(szTemp);
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_DYNRAM].SetAvailable(TRUE);
	}

	szTemp.LoadString(IDS_FILENAME_AUDIOBMPMEMBACKUP);
	szFileName.Format(_T("%s%s//%s%02x.bin"),szPathName,szSubDir,szTemp,bNodeAddress);
	MemoryFileInfo[MEMORYDUMPINFO_TYPE_AUDIOBMP].SetFileName(szFileName);		
	if (FindFileDate(szFileName,&szTemp)) 
	{
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_AUDIOBMP].SetFileCreationDate(szTemp);
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_AUDIOBMP].SetAvailable(TRUE);
	}

	szTemp.LoadString(IDS_FILENAME_ALLBMPMEMBACKUP);
	szFileName.Format(_T("%s%s//%s%02x.bin"),szPathName,szSubDir,szTemp,bNodeAddress);
	MemoryFileInfo[MEMORYDUMPINFO_TYPE_ALLBMP].SetFileName(szFileName);
	if (FindFileDate(szFileName,&szTemp)) 
	{
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_ALLBMP].SetFileCreationDate(szTemp);
		MemoryFileInfo[MEMORYDUMPINFO_TYPE_ALLBMP].SetAvailable(TRUE);
	}

	// Format verify classes ...
	InitVerifyErrors(pNodeDatabase,pNode);

	// Log File Classe ...
	szFileName.Format(_T("%s%s//Log_%02x.bin"),szPathName,szSubDir,bNodeAddress);
	MemoryDumpLogFile.SetFileName(szFileName);
	MemoryDumpLogFile.AssignNode(pNode);
	MemoryDumpLogFile.Load();
	return TRUE;
}

CMemoryDumpFileInfo* CMemoryDumpInfo::GetFileInfo(ENUM_MEMORYDUMPINFO_TYPE type) {
	if (type < MEMORYDUMPINFO_TYPE_MAX ) return (&MemoryFileInfo[type]);
	return NULL;
}

///////////////////////////////////////////////////////////////////////
CMemoryDumpInfoVerify* CMemoryDumpInfo::GetConfigVerifyInfo(ENUM_MEMORYDUMPINFO_TYPE type) {
	if (type < MEMORYDUMPINFO_TYPE_MAX ) return (&ConfigVerifyInfo[type]);
	return NULL;
}

CMemoryDumpInfoVerify* CMemoryDumpInfo::GetRTVerifyInfo(ENUM_MEMORYDUMPINFO_TYPE type) {
	if (type < MEMORYDUMPINFO_TYPE_MAX ) return (&RuntimeVerifyInfo[type]);
	return NULL;
}
CMemoryDumpTaskInfo* CMemoryDumpInfo::GetTaskInfo(ENUM_MEMORYDUMPINFO_TASK task,ENUM_MEMORYDUMPINFO_TYPE type)
{
	//int i,j;
	if (task >= MEMORYDUMPINFO_TASK_NOTASK) return NULL;
	if (type >= MEMORYDUMPINFO_TYPE_MAX) return NULL;
	return (&TaskInfo[task][type]);
}




//////////////////////////////////////////////////////////////////////
CMemoryDumpInfo* CMemoryDumpInfo::GetInstance() 
{
	static CMemoryDumpInfo instance;
	return (&instance);
}
///////////////////////////////////////////////////////////////////////
void CMemoryDumpInfo::InitVerifyErrors(CNodeDatabase *pNodeDatabase,CCanNode *pNode)
{
	int i,j;
	CMemoryDumpInfoVerify *pMemoryDumpInfoVerify;

	// Init Both ...
	for (i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) 
	{
		RuntimeVerifyInfo[i].Clear();
		ConfigVerifyInfo[i].Clear();
	}

	// For each sensor in CNodeDatabase set 2 Verify errors ...
	pMemoryDumpInfoVerify = &ConfigVerifyInfo[MEMORYDUMPINFO_TYPE_DYNRAM];
	if (pMemoryDumpInfoVerify != NULL) 
	{	
		i = 0;
	
		CNodeDatabaseOperations NodeDatabaseOperations(pNodeDatabase);
		NodeDatabaseOperations.GetNROfUnitsType(UNIT_TYPE_SENS,&i);
	
		j = i*2;
		pMemoryDumpInfoVerify->Set_VerifyErrors(j);
	}
}

// --------------------------------------------------------------------------------

void CMemoryDumpInfo::SetTaskMemoryTypeOk(ENUM_MEMORYDUMPINFO_TYPE type,BOOL flag)
{
	switch(type)
	{
		case MEMORYDUMPINFO_TYPE_DYNRAM :
			if (flag) usMemoryTypeSaved |= MEMORYDUMPINFO_FLAG_DYNRAM;
			else usMemoryTypeSaved &= (MEMORYDUMPINFO_FLAG_DYNRAM^0xFFFF);
		break;

		case MEMORYDUMPINFO_TYPE_STATRAM :
			if (flag) usMemoryTypeSaved |= MEMORYDUMPINFO_FLAG_STATRAM;
			else usMemoryTypeSaved &= (MEMORYDUMPINFO_FLAG_STATRAM^0xFFFF);
		break;

		case MEMORYDUMPINFO_TYPE_ALLBMP :
			if (flag) usMemoryTypeSaved |= MEMORYDUMPINFO_FLAG_ALLBITMAPS;
			else usMemoryTypeSaved &= (MEMORYDUMPINFO_FLAG_ALLBITMAPS^0xFFFF);
		break;

		case MEMORYDUMPINFO_TYPE_AUDIOBMP :
			if (flag) usMemoryTypeSaved |= MEMORYDUMPINFO_FLAG_AUDIOBITMAPS;
			else usMemoryTypeSaved &= (MEMORYDUMPINFO_FLAG_AUDIOBITMAPS^0xFFFF);
		break;

		default:
			ASSERT(0);
		break;
	}
}

BOOL CMemoryDumpInfo::IsTaskMemoryTypeOk(ENUM_MEMORYDUMPINFO_TYPE type)
{
	switch(type)
	{
		case MEMORYDUMPINFO_TYPE_DYNRAM :
			if ((usMemoryTypeSaved&MEMORYDUMPINFO_FLAG_DYNRAM) == MEMORYDUMPINFO_FLAG_DYNRAM)
				return TRUE;		
		break;

		case MEMORYDUMPINFO_TYPE_STATRAM :
			if ((usMemoryTypeSaved&MEMORYDUMPINFO_FLAG_STATRAM) == MEMORYDUMPINFO_FLAG_STATRAM)
				return TRUE;		
		break;

		case MEMORYDUMPINFO_TYPE_ALLBMP :
			if ((usMemoryTypeSaved&MEMORYDUMPINFO_FLAG_ALLBITMAPS) == MEMORYDUMPINFO_FLAG_ALLBITMAPS)
				return TRUE;		
		break;

		case MEMORYDUMPINFO_TYPE_AUDIOBMP :
			if ((usMemoryTypeSaved&MEMORYDUMPINFO_FLAG_AUDIOBITMAPS) == MEMORYDUMPINFO_FLAG_AUDIOBITMAPS)
				return TRUE;		
		break;

		default:
			ASSERT(0);
		break;
	}	
	return FALSE;
}
/*
BOOL CMemoryDumpInfo::AllMemoryDownloadedAndSaved(void)
{
	int i;
	ENUM_MEMORYDUMPINFO_TYPE MemoryDumpInfoType;

	BOOL fResult = TRUE;
	for (i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) {		
		MemoryDumpInfoType = (ENUM_MEMORYDUMPINFO_TYPE) i;

		if (((IsDownload(MemoryDumpInfoType) == TRUE) || (IsUpload(MemoryDumpInfoType) == TRUE)) &&
			(IsTaskMemoryTypeOk(MemoryDumpInfoType) == FALSE)) {
			fResult = FALSE;		
		}
	}
	return fResult;
}
*/

void CMemoryDumpInfo::ClearTaskInfo(void)
{
	int i,j;

	for (i=0;i<MEMORYDUMPINFO_TASK_NOTASK;i++) 
	{
		for (j=0;j<MEMORYDUMPINFO_TYPE_MAX;j++) 
		{			
			TaskInfo[i][j].Clear();
		}
	}

	TaskInfo[MEMORYDUMPINFO_TASK_DOWNLOAD][MEMORYDUMPINFO_TYPE_DYNRAM].SetTaskIDString("Download & Verify  Dynamic RAM");
	TaskInfo[MEMORYDUMPINFO_TASK_DOWNLOAD][MEMORYDUMPINFO_TYPE_STATRAM].SetTaskIDString("Download & Verify Static RAM");
	TaskInfo[MEMORYDUMPINFO_TASK_DOWNLOAD][MEMORYDUMPINFO_TYPE_ALLBMP].SetTaskIDString("Download & Verify Bitmap RAM");
	TaskInfo[MEMORYDUMPINFO_TASK_DOWNLOAD][MEMORYDUMPINFO_TYPE_AUDIOBMP].SetTaskIDString("Download & Verify Audio Bitmap RAM");

	TaskInfo[MEMORYDUMPINFO_TASK_UPLOAD][MEMORYDUMPINFO_TYPE_DYNRAM].SetTaskIDString("Upload Dynamic RAM");
	TaskInfo[MEMORYDUMPINFO_TASK_UPLOAD][MEMORYDUMPINFO_TYPE_STATRAM].SetTaskIDString("Upload Static RAM");
	TaskInfo[MEMORYDUMPINFO_TASK_UPLOAD][MEMORYDUMPINFO_TYPE_ALLBMP].SetTaskIDString("Upload Bitmap RAM");
	TaskInfo[MEMORYDUMPINFO_TASK_UPLOAD][MEMORYDUMPINFO_TYPE_AUDIOBMP].SetTaskIDString("Upload Audio Bitmap RAM");
}

BOOL CMemoryDumpInfo::StoreLogFile(void)
{
	MemoryDumpLogFile.UpdateData(this);
	return (MemoryDumpLogFile.Save());
}

CMemoryDumpLog* CMemoryDumpInfo::GetLogFile(ENUM_MEMORYDUMPINFO_TYPE type) 
{
	return (MemoryDumpLogFile.GetLogFile(type));
}

