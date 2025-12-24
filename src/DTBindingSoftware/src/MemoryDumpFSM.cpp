// MemoryDumpFSM.cpp: implementation of the CMemoryDumpFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MemoryDumpFSM.h"

#include "TXNodeMessage.h"
#include "MemoryDumpMasterFSM.h"
#include "MemoryDumpInfo.h"


#include "utils/win32/NotifyError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


enum { SEGMENTED_BLOCK_LENGTH = 110 };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryDumpFSM::CMemoryDumpFSM(IMemoryDumpMasterFSM *ptr)
{
	CString szError;

	ptrMemoryDump = NULL;
	ptrMemoryVerify = NULL;
	ptrToMemoryDump = NULL;

	ulMemoryPointer = 0;
	ulDataArrayIndex = 0;

	bSelectedNodeAddress = 0xFF;

	pMemoryDumpMasterFSM = ptr;
	CurrentState = FSM_STATE_MEMORYDUMP_IDLE;

	pMemoryDumpInfo = NULL;
}

CMemoryDumpFSM::~CMemoryDumpFSM()
{
	Free_AllocatedMemory();
}


void CMemoryDumpFSM::InitNode(STRUCT_ADDRESSES *ptrMemoryAddress,CMemoryDumpInfo *ptrMemoryDumpInfo)
{	
	pMemoryDumpInfo = ptrMemoryDumpInfo;
	memcpy(&MemoryAddress,ptrMemoryAddress,sizeof(STRUCT_ADDRESSES));
}

BOOL CMemoryDumpFSM::CheckNodeAddress(BYTE bNodeAddress)
{
	CString szError;	
	if (bSelectedNodeAddress == bNodeAddress) return TRUE;		
	return FALSE;
}

void CMemoryDumpFSM::EvAfterMemoryDumpTimeout(void) 
{
	FSM(FSM_EV_AFTER_MEMORYDUMP_TIMEOUT);	
}

void CMemoryDumpFSM::EvSignalStatusMemoryDumpStart(BYTE bNodeAddress)
{
	if (CheckNodeAddress(bNodeAddress)) { 
		FSM(FSM_EV_OPERATION_MEMORYDUMP_START);
	}
}

void CMemoryDumpFSM::EvSignalStatusMemoryDumpWriteOk(BYTE bNodeAddress) 
{
	if (CheckNodeAddress(bNodeAddress)) {
		FSM(FSM_EV_SIGNAL_STATUS_MEMORYDUMP_WRITEOK);
	}
}

/****************************************************************************
 * TM 20071025: Na melding van Mario & Louis: Error boodschap dat de
 * pointer ptrToMemoryDump op 0 staat, de boodschap verduidelijkt +
 * wordt enkel getoond indien de FSM niet in de toestand IDLE staat.
 *
 * Komt voor bij ode verschakeling van Taken Upload Backup.
 * (Static --> Dynamic : Eerste taak is gedaan, tweede taak wordt gestart).
 * 
 ****************************************************************************/
void CMemoryDumpFSM::EvSignalStatusMemoryDumpReadOk(BYTE bNodeAddress,
													BYTE *pbData,
													BYTE bLength,
													BYTE bErrors)
{
	if (CheckNodeAddress(bNodeAddress)) 
	{
		CString szError;

		if (bErrors >= 0x80) 
		{
			szError.Format("Error: Frames filled with 0xFF.\nPossible the backup is corrupt !");			
		}
		
		if (ptrToMemoryDump != 0) 
		{
			memcpy(&(ptrToMemoryDump->pbData[ulDataArrayIndex]),&pbData[0],bLength);						
		}
		else 
		{
			if (CurrentState != FSM_STATE_MEMORYDUMP_IDLE)
			{
				szError.Format("Warning:Pointer to Memorydump FSM points to 0 && Currentstate != IDLE");			
			}
		}
		
		FSM(FSM_EV_SIGNAL_STATUS_MEMORYDUMP_READOK);

		if (!szError.IsEmpty()) 
		{
			AfxMessageBox(szError);	
		}
	}
}
///////////////////////////////////////////////////////////////////////////:
void CMemoryDumpFSM::FSM(FSM_EVENTS e)
{
	BOOL fResult;
	unsigned long ulIndex;
	unsigned short usBlockNr;
	unsigned short usMaxBlockNr;	
	CString szInfo,szTemp;
	int iProcentValue;
		
	switch (CurrentState)
	{
		case FSM_STATE_MEMORYDUMP_IDLE :
			
		break;

		case FSM_STATE_MEMORYDUMP_START_DOWNLOAD :
		{
			switch (e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_START:
				{
					Stop_TimeoutTimer();

					Fire_OnMemoryDumpStarted();
										
					fResult = FALSE;								

					szInfo.Format(_T("NodeAddress: 0x%02x - Start Downloading Memory"),
									 bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);		
					
					fFirstBlock = TRUE;					
					// Todo Notify parent ...

					usBlockLength = (unsigned short)SEGMENTED_BLOCK_LENGTH;
					ulMemoryPointer = ulMemoryStartAddress; 
					ulDataArrayIndex = 0;
									
					Start_TimeoutTimer();

					if (CTXNodeMessage::getInstance()->ReadMemoryData(bSelectedNodeAddress,
																	  ulMemoryPointer,
																	  usBlockLength)) 
					{
						CurrentState = FSM_STATE_MEMORYDUMP_DOWNLOAD;										
					}
					else 
					{	
						CurrentState = FSM_STATE_MEMORYDUMP_IDLE;											
						Fire_OnMemoryDumpError();
					}			

				} break;

				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL :				
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT :	
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();
				
				} break;

				default:
				{
					
				} break;
			}
		}
		break;

		case FSM_STATE_MEMORYDUMP_START_UPLOAD :
		{
			switch (e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL :				
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT :					
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_OPERATION_MEMORYDUMP_START:
				{
					Stop_TimeoutTimer();
					Fire_OnMemoryDumpStarted();
					
					fResult = FALSE;									

					ulDataArrayIndex = 0;
					usBlockLength = (unsigned short) SEGMENTED_BLOCK_LENGTH;

					Start_TimeoutTimer();

					CTXNodeMessage::getInstance()->WriteMemoryData(bSelectedNodeAddress,
																   ulMemoryPointer,
																   usBlockLength,
																   &(ptrToMemoryDump->pbData[ulDataArrayIndex]));


					CurrentState = FSM_STATE_MEMORYDUMP_UPLOAD;

				} break;

				default:

				break;			
			}
		}break;

		case FSM_STATE_MEMORYDUMP_UPLOAD_DONE:
		{
			switch(e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL:
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT:
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_OPERATION_MEMORYDUMP_START:
				{
					Stop_TimeoutTimer();
					
					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpDone();

				} break;
			}
		} break;

		case FSM_STATE_MEMORYDUMP_UPLOAD :
		{
			switch(e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL:
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT:
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;
			
			
				case FSM_EV_SIGNAL_STATUS_MEMORYDUMP_WRITEOK :
				{
					Stop_TimeoutTimer();
				
					ulMemoryPointer += usBlockLength;
					ulDataArrayIndex += usBlockLength;

					if (ulMemoryPointer >= (ptrToMemoryDump->ulSourceAddress+ptrToMemoryDump->ulLength))
					{		
						Start_TimeoutTimer();
					
						CTXNodeMessage::getInstance()->SendLcdLock(bSelectedNodeAddress,
																   FALSE);	

						CString szInfo;
						szInfo.Format(_T("NodeAddress: 0x%02x - Uploading of Memory done"),
									  bSelectedNodeAddress);
						Fire_OnMemoryDumpInfo(szInfo);

						// TM 25/07/2006
						szInfo.Format(_T("Downloading of memory done ..."));						
						CTXNodeMessage::getInstance()->SendLcdInfo(bSelectedNodeAddress,
																   szInfo);
						
						CurrentState = FSM_STATE_MEMORYDUMP_UPLOAD_DONE;

					}
					else if (((ptrToMemoryDump->ulSourceAddress+ptrToMemoryDump->ulLength)-ulMemoryPointer) >= SEGMENTED_BLOCK_LENGTH)
					{
						usMaxBlockNr = (unsigned short)(((ulMemoryEndAddress-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);
						usBlockNr = (unsigned short)(((ulMemoryPointer-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);					
				
						Start_TimeoutTimer();

						usBlockLength = (unsigned short)SEGMENTED_BLOCK_LENGTH;
						
						CTXNodeMessage::getInstance()->WriteMemoryData(bSelectedNodeAddress,
																	   ulMemoryPointer,
																	   usBlockLength,
																	   &(ptrToMemoryDump->pbData[ulDataArrayIndex]));					

						if (CalculateProcentValue(usBlockNr,usMaxBlockNr,&iProcentValue)) 
						{							
							szInfo.Format(_T("NodeAddress: 0x%02x - Write Memory block %d%%"),
											 bSelectedNodeAddress,
											 iProcentValue);

							Fire_OnMemoryDumpInfo(szInfo);												
						}
					}
					else 
					{
						usMaxBlockNr = (unsigned short)(((ulMemoryEndAddress-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);
					
						Start_TimeoutTimer();

						usBlockLength = (unsigned short)((ptrToMemoryDump->ulSourceAddress+ptrToMemoryDump->ulLength)-ulMemoryPointer);
						
						CTXNodeMessage::getInstance()->WriteMemoryData(bSelectedNodeAddress,
																	   ulMemoryPointer,
																	   usBlockLength,
																	   &(ptrToMemoryDump->pbData[ulDataArrayIndex]));
						
						if (CalculateProcentValue(usMaxBlockNr,usMaxBlockNr,&iProcentValue)) 
						{							
							szInfo.Format(_T("NodeAddress: 0x%02x - Write Memory block %d%%"),
										  bSelectedNodeAddress,
										  iProcentValue);

							Fire_OnMemoryDumpInfo(szInfo);	
						}
					}

				}break;
			}
		}break;

		case FSM_STATE_MEMORYDUMP_DOWNLOAD_DONE:
		{
			switch (e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL :		
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT :					
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();
				} break;

				case FSM_EV_OPERATION_MEMORYDUMP_START:
				{
					Stop_TimeoutTimer();
					
					if (ptrMemoryVerify != NULL) 
					{
						memcpy(ptrMemoryVerify,
							   ptrMemoryDump,
							   sizeof(STRUCT_MEMORY_DUMP));

						for (ulIndex=0;ulIndex<MAX_MEMORY_DUMP_LENGTH;ulIndex++) 
						{
							ptrToMemoryDump->pbData[ulIndex] = 0xFF;
						}

						ulMemoryPointer = ptrMemoryVerify->ulSourceAddress;
						ulDataArrayIndex = 0;
						ulMemoryStartAddress = ptrMemoryVerify->ulSourceAddress;
						ulMemoryEndAddress = (ptrMemoryVerify->ulSourceAddress+ptrMemoryVerify->ulLength)-1;
								

						Start_TimeoutTimer();

						szInfo.Format(_T("Verifying of Memory, please wait"));
						CTXNodeMessage::getInstance()->SendLcdInfo(bSelectedNodeAddress,
																   szInfo);

						Fire_OnMemoryDumpInfo(szInfo);

						CurrentState = FSM_STATE_MEMORYDUMP_START_VERIFY;

						if (CurrentTask == FSM_TASK_DOWNLOAD_DYNRAM) 
						{
							CurrentTask = FSM_TASK_VERIFY_DYNRAM;
						}
						else if (CurrentTask == FSM_TASK_DOWNLOAD_STATRAM) 
						{
							CurrentTask = FSM_TASK_VERIFY_STATRAM;
						}
						else if (CurrentTask == FSM_TASK_DOWNLOAD_AUDIOBITMAPS) 
						{ 
							CurrentTask = FSM_TASK_VERIFY_AUDIOBITMAPS;
						}
						else if (CurrentTask == FSM_TASK_DOWNLOAD_ALLBITMAPS) 
						{
							CurrentTask = FSM_TASK_VERIFY_ALLBITMAPS;
						}
						else 
						{
							ASSERT(0);
						}
					}
					else
					{					
						ASSERT(0);
					}       								

				} break;
			}
		} break;

		case FSM_STATE_MEMORYDUMP_DOWNLOAD :
		{
			switch (e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL :		
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT :					
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();
				} break;

				case FSM_EV_SIGNAL_STATUS_MEMORYDUMP_READOK :
				{				
					Stop_TimeoutTimer();

					fResult = FALSE;
					ulMemoryPointer += usBlockLength;
					ulDataArrayIndex += usBlockLength;

					if ((fFirstBlock == TRUE) && (CurrentTask == FSM_TASK_DOWNLOAD_DYNRAM)) 
					{ 
						// only do first block dynamic Ram ...
					
						fFirstBlock = FALSE;
										
						ulMemoryEndAddress = ((((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER])<<24) |
											 (((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER+1])<<16) |
											 (((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER+2])<<8) |
											 (((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER+3])));

						if (ulMemoryEndAddress > MemoryAddress.ulDynamicMemoryEndAddress)
							ulMemoryEndAddress = MemoryAddress.ulDynamicMemoryEndAddress;
														
					}
					
					if (ulMemoryPointer >= ulMemoryEndAddress) 
					{
						
						// Downloading done ...						
						if (ptrToMemoryDump != NULL) 
						{
							//CurrentState = FSM_STATE_MEMORYDUMP_IDLE;											
							
							ptrToMemoryDump->ulLength = ulDataArrayIndex;
							ptrToMemoryDump->ulSourceAddress = ulMemoryStartAddress;						
						
							szInfo.Format(_T("Downloading of memory done"));
							CTXNodeMessage::getInstance()->SendLcdInfo(bSelectedNodeAddress,
																	   szInfo);
							Fire_OnMemoryDumpInfo(szInfo);	
							
							Start_TimeoutTimer();
							CurrentState = FSM_STATE_MEMORYDUMP_DOWNLOAD_DONE;
												
						}
						else 
						{
							ASSERT(0);
						}
					}
					else if ((((ulMemoryEndAddress - ulMemoryPointer)+1) < (unsigned long) SEGMENTED_BLOCK_LENGTH) ) 
					{						
						// Downloading if BlockLength is smaller then Segmented Data block ...
						usBlockLength = (unsigned short) (ulMemoryEndAddress - ulMemoryPointer)+1;

						Start_TimeoutTimer();
										
						fResult = CTXNodeMessage::getInstance()->ReadMemoryData(bSelectedNodeAddress,
																				ulMemoryPointer,
																				usBlockLength);					

						if (fResult == FALSE) 
						{							
							ASSERT(0);						
							CurrentState = FSM_STATE_MEMORYDUMP_IDLE;													
							Fire_OnMemoryDumpError();
						}
						else 
						{	
							usMaxBlockNr = (unsigned short)(((ulMemoryEndAddress-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);													

							if (CalculateProcentValue(usMaxBlockNr,usMaxBlockNr,&iProcentValue)) 
							{								
								szInfo.Format(_T("NodeAddress: 0x%02x - Read Memory block %d%%"),bSelectedNodeAddress,iProcentValue);
								Fire_OnMemoryDumpInfo(szInfo);	
							}
						}					
					}
					else 
					{					
						Start_TimeoutTimer();

						usBlockLength = SEGMENTED_BLOCK_LENGTH;

						// Calculate Stats 
						usMaxBlockNr = (unsigned short)(((ulMemoryEndAddress-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);
						usBlockNr = (unsigned short)(((ulMemoryPointer-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);
									
						if (CTXNodeMessage::getInstance()->ReadMemoryData(bSelectedNodeAddress,ulMemoryPointer,usBlockLength)) 
						{
							if (CalculateProcentValue(usBlockNr,usMaxBlockNr,&iProcentValue)) 
							{
								szInfo.Format(_T("NodeAddress: 0x%02x - Read Memory block %d%%"),
											  bSelectedNodeAddress,
											  iProcentValue);

								Fire_OnMemoryDumpInfo(szInfo);	
							}
						}
						else 
						{
							ASSERT(0);						
							CurrentState = FSM_STATE_MEMORYDUMP_IDLE;

							CalculateProcentValue(usBlockNr,usMaxBlockNr,&iProcentValue);

							szInfo.Format(_T("NodeAddress: 0x%02x - Read Memory block %d%% - FAILED"),
										  bSelectedNodeAddress,
										  iProcentValue);

							Fire_OnMemoryDumpInfo(szInfo);															
							Fire_OnMemoryDumpError();
						}											
					}
				}break;

				default:

				break;
			}
		}break;

		case FSM_STATE_MEMORYDUMP_VERIFY_DONE:
		{
			switch (e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL :				
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;
/*				
				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT :					
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;
*/

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT:
				case FSM_EV_OPERATION_MEMORYDUMP_START:
				{
					Stop_TimeoutTimer();
				
					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;	
					Fire_OnMemoryDumpDone();

				} break;
			}

		}break;

		case FSM_STATE_MEMORYDUMP_VERIFY :
		{
			switch (e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL :				
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;
				
				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT :					
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_SIGNAL_STATUS_MEMORYDUMP_READOK :
				{
					Stop_TimeoutTimer();

					fResult = FALSE;
					ulMemoryPointer += usBlockLength;
					ulDataArrayIndex += usBlockLength;

					if ((fFirstBlock == TRUE) && (CurrentTask == FSM_TASK_VERIFY_DYNRAM)) {
					
						fFirstBlock = FALSE;
										
						ulMemoryEndAddress = ((((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER])<<24) |
											 (((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER+1])<<16) |
											 (((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER+2])<<8) |
											 (((unsigned long)ptrToMemoryDump->pbData[DYNAMIC_MEMORY_END_ADDRESS_POINTER+3])));

						if (ulMemoryEndAddress > MemoryAddress.ulDynamicMemoryEndAddress)
							ulMemoryEndAddress = MemoryAddress.ulDynamicMemoryEndAddress;														
					}
					
					if (ulMemoryPointer >= ulMemoryEndAddress) 
					{						
						// Downloading done ...						
						if (ptrToMemoryDump != NULL) 
						{																								
							ptrToMemoryDump->ulLength = ulDataArrayIndex;
							ptrToMemoryDump->ulSourceAddress = ulMemoryStartAddress;
																	
							szInfo.Format(_T("Verifying of memory done"));
							CTXNodeMessage::getInstance()->SendLcdInfo(bSelectedNodeAddress,
																	   szInfo);
							Fire_OnMemoryDumpInfo(szInfo);

						
							if (VerifyAndSave()) 
							{
								szInfo.Format(_T("Logical node 0x%02x file has been Saved"),bSelectedNodeAddress);
								Fire_OnMemoryDumpInfo(szInfo);	
							}
							else 
							{
								szInfo.Format(_T("Logical node 0x%02x file has NOT been Saved"),bSelectedNodeAddress);
								Fire_OnMemoryDumpInfo(szInfo);	
							}

							Start_TimeoutTimer();

							Free_AllocatedMemory();
							CurrentState = FSM_STATE_MEMORYDUMP_VERIFY_DONE;															
						}
						else 
						{
							ASSERT(0);
						}
					}
					else if ((((ulMemoryEndAddress - ulMemoryPointer)+1) < (unsigned long) SEGMENTED_BLOCK_LENGTH) ) {
						
						Start_TimeoutTimer();

						// Downloading if BlockLength is smaller then Segmented Data block ...
						usBlockLength = (unsigned short) (ulMemoryEndAddress - ulMemoryPointer)+1;
									
						fResult = CTXNodeMessage::getInstance()->
									ReadMemoryData(bSelectedNodeAddress,ulMemoryPointer,usBlockLength);											

						if (fResult == FALSE) {
							
							ASSERT(0);
						
							CurrentState = FSM_STATE_MEMORYDUMP_IDLE;						
							
							Fire_OnMemoryDumpError();
						}
						else 
						{	
							usMaxBlockNr = (unsigned short)(((ulMemoryEndAddress-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);
							
							if (CalculateProcentValue(usMaxBlockNr,usMaxBlockNr,&iProcentValue)) 
							{								
								szInfo.Format(_T("NodeAddress: 0x%02x - Read Memory block %d%% (Verify Memory)"),
											  bSelectedNodeAddress,
											  iProcentValue);

								Fire_OnMemoryDumpInfo(szInfo);	
							}
						}					
					}
					else 
					{
						Start_TimeoutTimer();
					
						usBlockLength = SEGMENTED_BLOCK_LENGTH;

						// Calculate Stats 
						usMaxBlockNr = (unsigned short)(((ulMemoryEndAddress-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);
						usBlockNr = (unsigned short)(((ulMemoryPointer-ulMemoryStartAddress)+1)/SEGMENTED_BLOCK_LENGTH);
				
						if (CTXNodeMessage::getInstance()->ReadMemoryData(bSelectedNodeAddress,
																		  ulMemoryPointer,
																		  usBlockLength)) 
						{													
							if (CalculateProcentValue(usBlockNr,usMaxBlockNr,&iProcentValue)) 
							{								
								szInfo.Format(_T("NodeAddress: 0x%02x - Read Memory block %d%% (Verify Memory)"),
											  bSelectedNodeAddress,
											  iProcentValue);

								Fire_OnMemoryDumpInfo(szInfo);	
							}
						}
						else 
						{
							ASSERT(0);						
							CurrentState = FSM_STATE_MEMORYDUMP_IDLE;

							CalculateProcentValue(usBlockNr,usMaxBlockNr,&iProcentValue);
							
							szInfo.Format(_T("NodeAddress: 0x%02x - Read Memory block %d%% (Verify Memory) FAILED"),
										  bSelectedNodeAddress,
										  iProcentValue);

							Fire_OnMemoryDumpInfo(szInfo);						
							Fire_OnMemoryDumpError();
						}											
					}
				}break;
			}
		}break;

		case FSM_STATE_MEMORYDUMP_START_VERIFY :
		{		
			switch (e)
			{
				case FSM_EV_OPERATION_MEMORYDUMP_CANCEL :
				{
					Stop_TimeoutTimer();

					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump was cancelled by user"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_AFTER_MEMORYDUMP_TIMEOUT :					
				{
					szInfo.Format(_T("NodeAddress: 0x%02x - MemoryDump failed caused by timeout"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					CurrentState = FSM_STATE_MEMORYDUMP_IDLE;
					Fire_OnMemoryDumpError();

				} break;

				case FSM_EV_OPERATION_MEMORYDUMP_START :
				{
					Stop_TimeoutTimer();				
					
					if (CurrentTask == FSM_TASK_VERIFY_DYNRAM) 
					{
						fFirstBlock = TRUE;
					}

					szInfo.Format(_T("NodeAddress: 0x%02x Verify of memory busy"),
								  bSelectedNodeAddress);
					Fire_OnMemoryDumpInfo(szInfo);

					usBlockLength = (unsigned short)SEGMENTED_BLOCK_LENGTH;
					ulMemoryPointer = ulMemoryStartAddress; 
					ulDataArrayIndex = 0;

					Start_TimeoutTimer();

					CTXNodeMessage::getInstance()->ReadMemoryData(bSelectedNodeAddress,
																  ulMemoryPointer,
																  usBlockLength);

					CurrentState = FSM_STATE_MEMORYDUMP_VERIFY;
					
				}break;
			}
		}break;

		default:

		break;
	}
}

//--------------------------------
BOOL CMemoryDumpFSM::Cancel(void) 
{
	FSM(FSM_EV_OPERATION_MEMORYDUMP_CANCEL);

	return TRUE;
}


BOOL CMemoryDumpFSM::StartDownload(void)
{
	//unsigned long ulIndex;
	CString szInfo;

	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{
		ulDataArrayIndex = 0;
		ulMemoryPointer = ulMemoryStartAddress;

		if (Init_AllocateMemory()) 
		{
			ptrToMemoryDump = ptrMemoryDump;

			for (unsigned long ulIndex=0;
				 ulIndex<MAX_MEMORY_DUMP_LENGTH;
				 ulIndex++) 
			{
				ptrToMemoryDump->pbData[ulIndex] = 0xFF;
			}

			Start_TimeoutTimer();	
			
			szInfo.Format(_T("Downloading memory,please wait..."));
			CTXNodeMessage::getInstance()->SendLcdInfo(bSelectedNodeAddress,
													   szInfo);
		
			CurrentState = FSM_STATE_MEMORYDUMP_START_DOWNLOAD;	
			
			// FSM(FSM_EV_OPERATION_MEMORYDUMP_START); (TM 26/07/2006 !! )

			return TRUE;				
		}
	}
	return FALSE;
}

// ------------------------------------------------------------------
BOOL CMemoryDumpFSM::Start_DownloadAllBitmaps(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{	
		CurrentTask = FSM_TASK_DOWNLOAD_ALLBITMAPS;	

		bSelectedNodeAddress = bNodeAddress;
		ulMemoryStartAddress = MemoryAddress.ulBitmapMemoryStartAddress;
		ulMemoryEndAddress = MemoryAddress.ulBitmapMemoryEndAddress;

		return (StartDownload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_DownloadDynRam(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{	
		CurrentTask = FSM_TASK_DOWNLOAD_DYNRAM;		

		bSelectedNodeAddress = bNodeAddress;
		ulMemoryStartAddress = MemoryAddress.ulDynamicMemoryStartAddress;
		ulMemoryEndAddress = MemoryAddress.ulDynamicMemoryEndAddress;

		return (StartDownload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_DownloadStatRam(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{	
		CurrentTask = FSM_TASK_DOWNLOAD_STATRAM;

		bSelectedNodeAddress = bNodeAddress;
		ulMemoryStartAddress = MemoryAddress.ulStaticMemoryStartAddress;
		ulMemoryEndAddress = MemoryAddress.ulStaticMemoryEndAddress;

		return (StartDownload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_DownloadAudioBitmaps(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{	
		CurrentTask = FSM_TASK_DOWNLOAD_AUDIOBITMAPS;	

		bSelectedNodeAddress = bNodeAddress;
		ulMemoryStartAddress = MemoryAddress.ulAudioBitmapMemoryStartAddress;
		ulMemoryEndAddress = MemoryAddress.ulAudioBitmapMemoryEndAddress;

		return (StartDownload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::StartUpload(void)
{
	unsigned long ulIndex;
	CString szInfo;

	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{	
		ptrToMemoryDump = NULL;

		if (Init_AllocateMemory()) 
		{
			ptrToMemoryDump = ptrMemoryDump;
			
			for (ulIndex=0;ulIndex<MAX_MEMORY_DUMP_LENGTH;ulIndex++)
			{
				ptrToMemoryDump->pbData[ulIndex] = 0xFF;
			}

			if (Load()) 
			{
				CTXNodeMessage::getInstance()->SendLcdLock(bSelectedNodeAddress,
														   TRUE);

				ulMemoryStartAddress = ptrToMemoryDump->ulSourceAddress;
				ulMemoryEndAddress = (ptrToMemoryDump->ulSourceAddress + ptrToMemoryDump->ulLength)+1;
				ulDataArrayIndex = 0;
				ulMemoryPointer = ulMemoryStartAddress;				
				
				Start_TimeoutTimer();
				
				szInfo.Format(_T("Downloading of memory Please Wait"));			
				CTXNodeMessage::getInstance()->SendLcdInfo(bSelectedNodeAddress,
														   szInfo);

				CurrentState = FSM_STATE_MEMORYDUMP_START_UPLOAD;				

				return TRUE;
			}
		}		
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_UploadBitmap(BYTE bNodeAddress,unsigned long ulMemoryStart,unsigned short usLength,BYTE *pbBitmapData)
{
	//unsigned long ulIndex;
	CString szInfo;

	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{
		bSelectedNodeAddress = bNodeAddress;
		ptrToMemoryDump = NULL;
		
		if (Init_AllocateMemory()) 
		{
			ptrToMemoryDump = ptrMemoryDump;

			for (unsigned long ulIndex=0;
				 ulIndex<MAX_MEMORY_DUMP_LENGTH;
				 ulIndex++)	
			{
				ptrToMemoryDump->pbData[ulIndex] = 0xFF;
			}

			// Instead of loading the memory struct from disc the Structure is
			// filled with the arguments ...
			ptrToMemoryDump->ulSourceAddress = ulMemoryStart;
			ptrToMemoryDump->ulLength = usLength;

			memcpy(&ptrToMemoryDump->pbData[0],
				   &pbBitmapData[0],
				   usLength);

			// This is the same is uploading memory ...
			CTXNodeMessage::getInstance()->SendLcdLock(bSelectedNodeAddress,
													   TRUE);

			ulMemoryStartAddress = ptrToMemoryDump->ulSourceAddress;
			ulMemoryEndAddress = (ptrToMemoryDump->ulSourceAddress + ptrToMemoryDump->ulLength);// -1;
			ulDataArrayIndex = 0;
			ulMemoryPointer = ulMemoryStartAddress;				

			Start_TimeoutTimer();

			szInfo.Format(_T("Downloading of memory Please Wait"));
			CTXNodeMessage::getInstance()->SendLcdInfo(bSelectedNodeAddress,
													   szInfo);

			CurrentState = FSM_STATE_MEMORYDUMP_START_UPLOAD;				
			CurrentTask = FSM_TASK_UPLOAD_BITMAP;
			return TRUE;
		}
	}

	return FALSE;
}

// ----------------------------------------------------------------
/*
BOOL CMemoryDumpFSM::Start_UploadAllBitmaps(void)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) {
		CurrentTask = FSM_TASK_UPLOAD_ALLBITMAPS;
		return (StartUpload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_UploadStatRam(void)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) {	
		CurrentTask = FSM_TASK_UPLOAD_STATRAM;
		return (StartUpload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_UploadDynRam(void)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) {		
		CurrentTask = FSM_TASK_UPLOAD_DYNRAM;
		return (StartUpload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_UploadAudioBitmaps(void)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) {	
		CurrentTask = FSM_TASK_UPLOAD_AUDIOBITMAPS;
		return (StartUpload());
	}
	return FALSE;
}
*/
	
BOOL CMemoryDumpFSM::Start_UploadAllBitmaps(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{
		CurrentTask = FSM_TASK_UPLOAD_ALLBITMAPS;
		bSelectedNodeAddress = bNodeAddress;
		return (StartUpload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_UploadStatRam(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{	
		CurrentTask = FSM_TASK_UPLOAD_STATRAM;
		bSelectedNodeAddress = bNodeAddress;
		return (StartUpload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_UploadDynRam(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE) 
	{		
		CurrentTask = FSM_TASK_UPLOAD_DYNRAM;
		bSelectedNodeAddress = bNodeAddress;
		return (StartUpload());
	}
	return FALSE;
}

BOOL CMemoryDumpFSM::Start_UploadAudioBitmaps(BYTE bNodeAddress)
{
	if (CurrentState == FSM_STATE_MEMORYDUMP_IDLE)
	{	
		CurrentTask = FSM_TASK_UPLOAD_AUDIOBITMAPS;
		bSelectedNodeAddress = bNodeAddress;
		return (StartUpload());
	}
	return FALSE;
}
//-----------------------------------------------------------------------
void CMemoryDumpFSM::Start_TimeoutTimer(void) 
{
	if (pMemoryDumpMasterFSM != NULL)
	{
		pMemoryDumpMasterFSM->Start_AckTimeoutTimer(5000);
	}
}

void CMemoryDumpFSM::Stop_TimeoutTimer(void) 
{
	if (pMemoryDumpMasterFSM != NULL)
	{
		pMemoryDumpMasterFSM->Stop_AckTimeoutTimer();
	}
}

//-----------------------------------------------------------------------
inline void CMemoryDumpFSM::Fire_OnMemoryDumpError(void) 
{
	if (pMemoryDumpMasterFSM != NULL)
	{
		pMemoryDumpMasterFSM->NotifyOnEvTaskError();
	}
}

inline void CMemoryDumpFSM::Fire_OnMemoryDumpDone(void) 
{
	if (pMemoryDumpMasterFSM != NULL) 
	{
		pMemoryDumpMasterFSM->NotifyOnEvTaskDone();
	}
}

inline void CMemoryDumpFSM::Fire_OnMemoryDumpStarted(void) 
{	
	if (pMemoryDumpMasterFSM != NULL)
	{
		pMemoryDumpMasterFSM->NotifyOnEvTaskStarted();
	}
}

inline void CMemoryDumpFSM::Fire_OnMemoryDumpInfo(CString s) 
{
	if (pMemoryDumpMasterFSM != NULL)
	{
		pMemoryDumpMasterFSM->NotifyOnEvMemoryDumpFSMInfo(s);
	}
}
//-----------------------------------------------------------------------

BOOL CMemoryDumpFSM::CalculateProcentValue(unsigned short usBlockNR,unsigned short usMaxBlockNR,int *iProcentValue)
{
	static int iPrevProcentValue = 100;

	if (iPrevProcentValue == 100) iPrevProcentValue = 0;
	
	if (usMaxBlockNR != 0) 
	{	
		*iProcentValue = ((usBlockNR*100)/usMaxBlockNR);	

		if ((*iProcentValue) != iPrevProcentValue) 
		{
			iPrevProcentValue = (*iProcentValue);
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------------------------------

BOOL CMemoryDumpFSM::Load(void)
{
	CMemoryDumpFileInfo *pFileInfo;
	CString szFileName;
	FILE *fp;

	if (pMemoryDumpInfo == NULL) 
	{
		AfxMessageBox("CMemoryDumpFSM::Load,pMemoryDump = NULL");
		return FALSE;
	}

	pFileInfo = NULL;
	
	switch (CurrentTask) 
	{
		case FSM_TASK_UPLOAD_DYNRAM :
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_DYNRAM);
		break;			

		case FSM_TASK_UPLOAD_STATRAM :
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_STATRAM);
		break;

		case FSM_TASK_UPLOAD_AUDIOBITMAPS :	
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_AUDIOBMP);	
		break;

		case FSM_TASK_UPLOAD_ALLBITMAPS :	
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_ALLBMP);
		break;

		default :								
			ASSERT(0);	
		break;
	}

	if (pFileInfo != NULL) 
	{
		szFileName = pFileInfo->GetFileName();
		
		if ((fp = fopen(szFileName,"rb")) != NULL) 
		{
			fread(ptrToMemoryDump,sizeof(STRUCT_MEMORY_DUMP),1,fp);
			fclose(fp);

			return TRUE;
		}

	}
	else 
	{
		AfxMessageBox("CMemoryDumpFSM::Load,pFileInfo = NULL");
		return FALSE;
	}
	
	// TM 21/01/2008
	CNotifyError_Simple notify;
	notify(GetLastError());
	return FALSE;	
}

BOOL CMemoryDumpFSM::Save(void)
{
	CString szFileName;
	//LPVOID lpMsgBuf;
	FILE *fp;
	CMemoryDumpFileInfo *pFileInfo;

	if (pMemoryDumpInfo == NULL) 
	{
		AfxMessageBox("CMemoryDumpFSM::Save,pMemoryDump = NULL");
		return FALSE;
	}

	pFileInfo = NULL;
	
	switch (CurrentTask) 
	{	
		case FSM_TASK_DOWNLOAD_DYNRAM :		
		case FSM_TASK_VERIFY_DYNRAM :				
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_DYNRAM);
		break;
		
		case FSM_TASK_DOWNLOAD_STATRAM :
		case FSM_TASK_VERIFY_STATRAM :	
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_STATRAM);
		break;

		case FSM_TASK_DOWNLOAD_AUDIOBITMAPS :	
		case FSM_TASK_VERIFY_AUDIOBITMAPS :
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_AUDIOBMP);
		break;
		
		case FSM_TASK_DOWNLOAD_ALLBITMAPS :	
		case FSM_TASK_VERIFY_ALLBITMAPS :	
			pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_ALLBMP);
		break;
		
		default :
			ASSERT(0);
		break;
	}

	if (pFileInfo != NULL) 
	{
		szFileName = pFileInfo->GetFileName();
		
		if ((fp = fopen(szFileName,"wb")) != NULL) 
		{
			fwrite(ptrToMemoryDump,sizeof(STRUCT_MEMORY_DUMP),1,fp);
			fclose(fp);

			pFileInfo->SetFileSaved(TRUE);
			return TRUE;
		}
	}
	else 
	{
		AfxMessageBox("CMemoryDumpFSM::Save,pFileInfo = NULL");
		return FALSE;
	}

	// TM 21/01/2008
	CNotifyError_Simple notify;
	notify(GetLastError());

	return FALSE;	
}

//-----------------------------------------------------------------------


void CMemoryDumpFSM::Free_AllocatedMemory(void)
{
	if (ptrMemoryDump != NULL) 
	{
		delete ptrMemoryDump;
		ptrMemoryDump = NULL;
	}
	
	if (ptrMemoryVerify != NULL) 
	{
		delete ptrMemoryVerify;
		ptrMemoryVerify = NULL;
	}	
	
	ptrToMemoryDump = NULL;
}

BOOL CMemoryDumpFSM::Init_AllocateMemory(void)
{
	if (ptrMemoryDump != NULL) delete ptrMemoryDump;

	#pragma warning(disable: 4345)					// CHANGES_MVS_2008	
	ptrMemoryDump = new STRUCT_MEMORY_DUMP();		// #pragma warning(disable: 4996)		// CHANGES_MVS_2008	
	if (ptrMemoryDump == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}

	if (ptrMemoryVerify != NULL) delete ptrMemoryVerify;

	ptrMemoryVerify = new STRUCT_MEMORY_DUMP();		// #pragma warning(disable: 4996)		// CHANGES_MVS_2008	
	if (ptrMemoryDump == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CMemoryDumpFSM::VerifyAndSave(void)
{
	BYTE bVerifyResult;
	int iVerifyErrors;
	int iFFErrors;
	CString szInfo;
	ENUM_MEMORYDUMPINFO_TYPE MemoryDumpInfoType;		

	//CMemoryDumpFileInfo		*pFileInfo = NULL;
	CMemoryDumpInfoVerify	*pVerifyInfoRT = NULL;
	CMemoryDumpInfoVerify	*pVerifyInfoConfig = NULL;	
	CMemoryDumpTaskInfo		*pMemoryTaskInfo = NULL;

	switch (CurrentTask) {
	
		case FSM_TASK_DOWNLOAD_DYNRAM :		
		case FSM_TASK_VERIFY_DYNRAM :		
			MemoryDumpInfoType = MEMORYDUMPINFO_TYPE_DYNRAM;
			//pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_DYNRAM);
			//pVerifyInfoRT = pMemoryDumpInfo->GetRTVerifyInfo(MEMORYDUMPINFO_TYPE_DYNRAM);
			//pVerifyInfoConfig = pMemoryDumpInfo->GetConfigVerifyInfo(MEMORYDUMPINFO_TYPE_DYNRAM);
		break;
		
		case FSM_TASK_DOWNLOAD_STATRAM :
		case FSM_TASK_VERIFY_STATRAM :	
			MemoryDumpInfoType = MEMORYDUMPINFO_TYPE_STATRAM;
			//pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_STATRAM);
			//pVerifyInfoRT = pMemoryDumpInfo->GetRTVerifyInfo(MEMORYDUMPINFO_TYPE_STATRAM);
			//pVerifyInfoConfig = pMemoryDumpInfo->GetConfigVerifyInfo(MEMORYDUMPINFO_TYPE_STATRAM);
		break;

		case FSM_TASK_DOWNLOAD_AUDIOBITMAPS :	
		case FSM_TASK_VERIFY_AUDIOBITMAPS :
			MemoryDumpInfoType = MEMORYDUMPINFO_TYPE_AUDIOBMP;
			//pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_AUDIOBMP);			
			//pVerifyInfoRT = pMemoryDumpInfo->GetRTVerifyInfo(MEMORYDUMPINFO_TYPE_AUDIOBMP);
			//pVerifyInfoConfig = pMemoryDumpInfo->GetConfigVerifyInfo(MEMORYDUMPINFO_TYPE_AUDIOBMP);
		break;
		
		case FSM_TASK_DOWNLOAD_ALLBITMAPS :	
		case FSM_TASK_VERIFY_ALLBITMAPS :	
			MemoryDumpInfoType = MEMORYDUMPINFO_TYPE_ALLBMP;

			//pFileInfo = pMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_ALLBMP);			
			//pVerifyInfoRT = pMemoryDumpInfo->GetRTVerifyInfo(MEMORYDUMPINFO_TYPE_ALLBMP);
			//pVerifyInfoConfig = pMemoryDumpInfo->GetConfigVerifyInfo(MEMORYDUMPINFO_TYPE_ALLBMP);
		break;

		default:
			MemoryDumpInfoType = MEMORYDUMPINFO_TYPE_MAX;
		break;
	}

	if (MemoryDumpInfoType != MEMORYDUMPINFO_TYPE_MAX) 
	{
		pVerifyInfoRT = pMemoryDumpInfo->GetRTVerifyInfo(MemoryDumpInfoType);
		pVerifyInfoConfig = pMemoryDumpInfo->GetConfigVerifyInfo(MemoryDumpInfoType);
		pMemoryTaskInfo = pMemoryDumpInfo->GetTaskInfo(MEMORYDUMPINFO_TASK_DOWNLOAD,MemoryDumpInfoType);
	}

	if (pVerifyInfoRT == NULL || pVerifyInfoConfig == NULL || pMemoryTaskInfo == NULL) {
		
		szInfo.Format(_T("Error : File not saved caused by pointers to NULL"));
		AfxMessageBox(szInfo);	
		return FALSE;
	}


	// Update Runtime Verify config Class
	CheckVerify(&bVerifyResult,&iVerifyErrors,&iFFErrors);

	if ((bVerifyResult & VERIFY_ERROR_FLAG) == VERIFY_ERROR_FLAG)
	{
		if ((bVerifyResult & VERIFY_ERROR_DIFFERENT_SOURCEADDRESS) == VERIFY_ERROR_DIFFERENT_SOURCEADDRESS) {		
			pVerifyInfoRT->Set_DifferentSourceAddress(TRUE);
		}

		if ((bVerifyResult & VERIFY_ERROR_DIFFERENT_LENGTH) == VERIFY_ERROR_DIFFERENT_LENGTH) {
			pVerifyInfoRT->Set_DifferentMemoryLength(TRUE);
		}

		pVerifyInfoRT->Set_VerifyErrors(iVerifyErrors);
	}

	if ((bVerifyResult&CHECK_MEMORY_FLAG) == CHECK_MEMORY_FLAG)
	{		
		pVerifyInfoRT->Set_EmptyMemoryBlocks(iFFErrors);
	}
	else {
		pVerifyInfoRT->Set_EmptyMemoryBlocks(0);
	}

	// Compare RT and Config Verify Classes to see if we should save the file ...
	if ((pVerifyInfoRT->Get_DifferentMemoryLength() == TRUE) &&
		(pVerifyInfoConfig->Get_DifferentMemoryLength() == FALSE)) {
		
		//szInfo.Format(_T("Logical node 0x%02x - memorybackup not saved - verify returned different memorylength"),bSelectedNodeAddress);		
		//Fire_OnMemoryDumpInfo(szInfo);	

		if (pMemoryTaskInfo!=NULL)
			pMemoryTaskInfo->FormatVerifyString(MEMORYDUMPTASKINFO_VERIFY_ERROR,pVerifyInfoRT);
		return FALSE;
	}
	
	if ((pVerifyInfoRT->Get_DifferentSourceAddress() == TRUE) &&
		(pVerifyInfoConfig->Get_DifferentSourceAddress() == FALSE)) {
		//szInfo.Format(_T("Logical node 0x%02x - memorybackup not saved - verify returned different SourceAddress"),bSelectedNodeAddress);		
		//Fire_OnMemoryDumpInfo(szInfo);	
		if (pMemoryTaskInfo!=NULL)
			pMemoryTaskInfo->FormatVerifyString(MEMORYDUMPTASKINFO_VERIFY_ERROR,pVerifyInfoRT);
		return FALSE;
	}

	if (pVerifyInfoRT->Get_EmptyMemoryBlocks()>pVerifyInfoConfig->Get_EmptyMemoryBlocks()) {
		//szInfo.Format(_T("Logical node 0x%02x - memorybackup not saved - verify returned too many empty memoryblocks"),bSelectedNodeAddress);		
		//Fire_OnMemoryDumpInfo(szInfo);	
		if (pMemoryTaskInfo!=NULL)
			pMemoryTaskInfo->FormatVerifyString(MEMORYDUMPTASKINFO_VERIFY_ERROR,pVerifyInfoRT);
		return FALSE;
	}

	if (pVerifyInfoRT->Get_VerifyErrors()>pVerifyInfoConfig->Get_VerifyErrors()) {
		//szInfo.Format(_T("Logical node 0x%02x - memorybackup not saved - verify returned too many errors"),bSelectedNodeAddress);		
		//Fire_OnMemoryDumpInfo(szInfo);	
		if (pMemoryTaskInfo!=NULL)
			pMemoryTaskInfo->FormatVerifyString(MEMORYDUMPTASKINFO_VERIFY_ERROR,pVerifyInfoRT);
		return FALSE;
	}

	//szInfo.Format("Logical node 0x%02x - Verify ok ...",bSelectedNodeAddress);
	//Fire_OnMemoryDumpInfo(szInfo);	


	if (!Save()) {	
		//szInfo.Format(_T("Logical node 0x%02x - memorybackup not saved - File I/O error"),bSelectedNodeAddress);		
		//Fire_OnMemoryDumpInfo(szInfo);			

		if (pMemoryTaskInfo!=NULL)
			pMemoryTaskInfo->FormatVerifyString(MEMORYDUMPTASKINFO_VERIFY_ERROR,pVerifyInfoRT);
		
		return FALSE;
	}

	// if reached this point --> Memory is Downloaded + Verified + Saved !!!
	pMemoryDumpInfo->SetTaskMemoryTypeOk(MemoryDumpInfoType,TRUE);

	if (pMemoryTaskInfo!=NULL)
			pMemoryTaskInfo->FormatVerifyString(MEMORYDUMPTASKINFO_VERIFY_OK,pVerifyInfoRT);

	return TRUE;
}

void CMemoryDumpFSM::CheckVerify(BYTE *bStatus,int *iVerifyErrors,int *iMemoryErrors)
{
	unsigned long ulIndex = 0;	// CHANGES_MVS_2008_WARNINGS
	int iCount = 0;				// CHANGES_MVS_2008_WARNINGS

	if ((ptrMemoryDump != NULL) && (ptrMemoryVerify != NULL)) 
	{		
		*bStatus = 0;
		*iVerifyErrors = 0;
		*iMemoryErrors = 0;
		
		if (ptrMemoryDump->ulLength != ptrMemoryVerify->ulLength) 
		{
			*bStatus |= VERIFY_ERROR_FLAG;
			*bStatus |= VERIFY_ERROR_DIFFERENT_LENGTH;
			(*iVerifyErrors)++;
		}
		if (ptrMemoryDump->ulSourceAddress != ptrMemoryVerify->ulSourceAddress) 
		{
			*bStatus |= VERIFY_ERROR_FLAG;
			*bStatus |= VERIFY_ERROR_DIFFERENT_SOURCEADDRESS;
			(*iVerifyErrors)++;
		}
		for (ulIndex=0;ulIndex < MAX_MEMORY_DUMP_LENGTH;ulIndex++) 
		{		
			if (ptrMemoryDump->pbData[ulIndex] != ptrMemoryVerify->pbData[ulIndex]) 
			{			
				*bStatus |= VERIFY_ERROR_FLAG;
				(*iVerifyErrors)++;
			}				

// 80 opeenvolgende bytes van 0xFF geeft een Memory Error ...
			
			if (ulIndex < ptrMemoryDump->ulLength) 
			{			
				if (ptrMemoryDump->pbData[ulIndex] == 0xFF) 
				{
					iCount++;
				}
				else 
				{
					iCount = 0;
				}

				if (iCount >= 255 ) 
				{
					*bStatus |= CHECK_MEMORY_FLAG;
					(*iMemoryErrors)++;
					iCount = 0;
				}
			}
		}
	}
	else 
	{
		ASSERT(0);
	}
}
