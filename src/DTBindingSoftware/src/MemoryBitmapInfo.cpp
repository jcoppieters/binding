// MemoryBitmapInfo.cpp: implementation of the CMemoryBitmapInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MemoryBitmapInfo.h"


// ----------------------------------------
#include "CanNode.h"
#include "NodeDatabase.h"
// ----------------------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CMemoryCompatibleNodes::Clear(void)
{	
	for (BYTE i=0;i<MAX_COMPATIBLE_NODES;i++) 
		CompatibleNodes[i] = 0xFF;

	bCount = 0;
}

void CMemoryCompatibleNodes::Set(CNodeDatabase *pNodeDatabase,CCanNode *pNode)
{
	int i,iNodes;

	ASSERT(pNode);
	ASSERT(pNodeDatabase);

	Clear();

	// pNode is node in document database 
	if (pNode != NULL) 
	{
		const BYTE bNodeAddress = pNode->bLogicalAddress;
		pNode = NULL;
		
		//Find node in Runtime database ...
		if (pNodeDatabase->FindNode(bNodeAddress,&i,&pNode)) 
		{
			pNodeDatabase->NROfNodes(&iNodes);
			if (pNode != NULL) 
			{	

// Set Current Node as First Entry - Next Skip all nodes adrresses = bNodeAddress !!!

				CompatibleNodes[bCount++]=bNodeAddress;

				for (i=0;i<iNodes;i++) 
				{					
					CCanNode *p;
					if (pNodeDatabase->GetNode(i,&p)) 
					{
						if (p!=NULL) 
						{
							if (((p->bNodeType&NODE_LCD)==NODE_LCD) &&
								 (p->GetSoftwareVersion() == pNode->GetSoftwareVersion()) &&
								((p->bNodeFlags&NODE_LCD_TYPE_MASK)== (pNode->bNodeFlags&NODE_LCD_TYPE_MASK)) &&
								 (p->bLogicalAddress != pNode->bLogicalAddress))
							{							
								CompatibleNodes[bCount++]=p->bLogicalAddress;
							}						
						}		
					}				
				}			
			}
		}
	}

	ASSERT(bCount>0);
}

//////////////////////////////////////////////////////////////////////
CMemoryBitmapInfo* CMemoryBitmapInfo::GetInstance()
{
	static CMemoryBitmapInfo instance;
	return (&instance);
}

//////////////////////////////////////////////////////////////////////
CMemoryBitmapInfo::CMemoryBitmapInfo()
{
	pBitmapMemoryInfo = NULL;
	pbData = NULL;	
	FreeAllocatedMemory();		// pBitmapMemoryInfo must be NULL !!

	pAssignedNode = NULL;
	CurrentTask = MEMORYBITMAPINFO_TASK_NOTASK;
}

CMemoryBitmapInfo::~CMemoryBitmapInfo()
{
	FreeAllocatedMemory();
}

BOOL CMemoryBitmapInfo::FreeAllocatedMemory(void)
{
	if (pBitmapMemoryInfo != NULL) 
	{	
		delete [] pBitmapMemoryInfo;
		ASSERT(bMaxBitmaps);
		pBitmapMemoryInfo = 0;
	}

	/* TM 20080129: Hier crasht binding software tijdens het afsluiten :
	   Voordien was er een probleem met het nemen van een backup !
    */

	if (pbData != NULL) 
	{
		delete [] pbData;			
		pbData = 0;
	}
	
	pbData = NULL;
	pBitmapMemoryInfo = NULL;
	bMaxBitmaps = 0;
	ulDataArrayMemoryEndAddress = 0;
	usNodeSoftwareVersion = 0;
	fMemoryLock = FALSE;
	pAssignedNode = NULL;
	CurrentTask = MEMORYBITMAPINFO_TASK_NOTASK;
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////

int CMemoryBitmapInfo::GetNrOfStoredBitmaps(void)
{
	int i,iCount;

	if ((pBitmapMemoryInfo == NULL) || (pbData == NULL)) { 
		ASSERT(0);
		return 0;
	}
	iCount = 0;
	for (i=0;i<bMaxBitmaps;i++) {
		if (pBitmapMemoryInfo[i].fBitmapStoredToBeSend) iCount++;
	}
	return iCount;
}


BOOL CMemoryBitmapInfo::GetStoredBitmap(int index,unsigned long *ulMemoryStart,unsigned short *usLength,BYTE **ppbData)
{
	if (!CheckPreConditions(index)) return FALSE;

	for (int i=0,iCount=0;
		 i<bMaxBitmaps;
		 i++) 
	{
		if (pBitmapMemoryInfo[i].fBitmapStoredToBeSend) 
		{		
			if (iCount == index) 
			{			
				*ulMemoryStart = pBitmapMemoryInfo[i].ulMemoryStartAddress;
				*usLength = pBitmapMemoryInfo[i].usBitmapSize;						
				*ppbData = &(pbData[pBitmapMemoryInfo[i].ulDataArrayIndex]);
			
				return TRUE;			
			}
			iCount++;
		}	
	}
	return FALSE;
}

void CMemoryBitmapInfo::ClearAllBitmaps(void)
{
	if (fMemoryLock == FALSE) 
	{	
		for (int i=0;i<bMaxBitmaps;i++) 
		{
			if (pBitmapMemoryInfo[i].fBitmapStoredToBeSend) 
			{			
				if (!ClearBitmap(i)) ASSERT(0);
			}
		}
	}
}

BOOL CMemoryBitmapInfo::ClearBitmap(int index)
{
	unsigned long ulStartAddress,ulLength;
	BYTE *ptrToDataArray = NULL; 

	if (fMemoryLock == TRUE) {
		return FALSE;
	}

	if (!CheckPreConditions(index)) return FALSE;

	ulStartAddress = pBitmapMemoryInfo[index].ulMemoryStartAddress;
	ulLength = pBitmapMemoryInfo[index].ulMemoryEndAddress -
		       pBitmapMemoryInfo[index].ulMemoryStartAddress;

	if ((ulLength > (pBitmapMemoryInfo[index].usColorBitmapSize)) &&
		(ulLength > (pBitmapMemoryInfo[index].usMonochromeBitmapSize))) {		
		ASSERT(0);
		return FALSE;
	}

	// Fill Data in Array to 0xFF.
	ptrToDataArray = &pbData[ulStartAddress];
	for(unsigned long ulIndex = 0;ulIndex<ulLength;ulIndex++)		// CHANGES_MVS_2008
	{	
		ptrToDataArray[ulIndex] = 0xFF;
	}

	// Must adjust STRUCT_MEMORY_INFO ...
	pBitmapMemoryInfo[index].fBitmapStoredToBeSend = FALSE;
	return TRUE;
}

BOOL CMemoryBitmapInfo::CheckPreConditions(int index)
{
	CString szError;
	if (index >= bMaxBitmaps) {	
		szError.Format(_T("Fatal error : StoreBitmap index %d, only allocated memory for %d bitmaps"),
										index,bMaxBitmaps);
		AfxMessageBox(szError);
		return FALSE;
	}
	if ((pBitmapMemoryInfo == NULL) || (pbData == NULL)) { 
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

BOOL CMemoryBitmapInfo::IsBitmapStoredToSend(int index) 
{
	if (!CheckPreConditions(index)) return FALSE;
	return (pBitmapMemoryInfo[index].fBitmapStoredToBeSend);
}

BOOL CMemoryBitmapInfo::StoreBitmap(CString szFileName,int index) 
{
	CString szError;

	int iBitmapSize;
	int iPixelHeight,iPixelWidth;
	BYTE *pbDatabitmap = NULL;
	BYTE *ptrToDataArray = NULL;

	if (!CheckPreConditions(index)) return FALSE;

	if (MyBitmap.Load(szFileName)) {

		iPixelHeight = MyBitmap.GetHeight();
		iPixelWidth = MyBitmap.GetWidth();
		iBitmapSize = MyBitmap.GetBitmapSize();
		pbDatabitmap = MyBitmap.GetBuffer();

		if (iBitmapSize == 0) {		
			szError.Format("Fatal Error : Loaded bitmap has size 0");
			AfxMessageBox(szError);
			return FALSE;
		}
		if (pbDatabitmap == NULL) {
			szError.Format("Fatal Error : Loaded bitmap Data points to NULL");
			AfxMessageBox(szError);
			return FALSE;
		}

		if ((iPixelHeight == pBitmapMemoryInfo[index].usPixelHeight)  &&
			(iPixelWidth == pBitmapMemoryInfo[index].usPixelWidth)) {

			ptrToDataArray = NULL;
			ptrToDataArray = &(pbData[pBitmapMemoryInfo[index].ulDataArrayIndex]);
			ASSERT(ptrToDataArray);

			if (iBitmapSize == (pBitmapMemoryInfo[index].usColorBitmapSize )) {										
				pBitmapMemoryInfo[index].usBitmapSize = pBitmapMemoryInfo[index].usColorBitmapSize;				
				if ((pBitmapMemoryInfo[index].ulDataArrayIndex + pBitmapMemoryInfo[index].usBitmapSize) 
					> pBitmapMemoryInfo[index].ulMemoryEndAddress) {				
					ASSERT(0);
					return FALSE;
				}

				// Store 8 Bit Color Bitmap at location !!!
				if (Store_Bitmap(iBitmapSize,ptrToDataArray)) {				
					pBitmapMemoryInfo[index].fBitmapStoredToBeSend = TRUE;
					return TRUE;
				}
				return FALSE;
			}

// We Must pay special attention to the fact that the bitmapsize corresponds to the
// actual size of the loaded bitmap.
// There is always memory allocated for iBitmapSize + 5 
// Because the monochrome bitmpas are not loaded with a header, the header has to added to it ...

			else if ((iBitmapSize == (pBitmapMemoryInfo[index].usMonochromeBitmapSize - 5)) ||
					 (iBitmapSize == (pBitmapMemoryInfo[index].usMonochromeBitmapSize - 4))) {			
				// Do extra calc. to make sure bitmap fits in allocated memory ...
				pBitmapMemoryInfo[index].usBitmapSize = pBitmapMemoryInfo[index].usMonochromeBitmapSize;
				
				if ((pBitmapMemoryInfo[index].ulDataArrayIndex + pBitmapMemoryInfo[index].usBitmapSize) 
					> pBitmapMemoryInfo[index].ulMemoryEndAddress) {				
					ASSERT(0);
					return FALSE;
				}
				// Store Monochrome bitmap at location !!!
				if (Store_Bitmap(iPixelWidth,iPixelHeight,iBitmapSize,ptrToDataArray)) {
					pBitmapMemoryInfo[index].fBitmapStoredToBeSend = TRUE;
					return TRUE;
				}
				return FALSE;				
			}
			else {
				szError.Format("Cannot store the loaded bitmap because the size is wrong");
				AfxMessageBox(szError);
				return FALSE;		
			}
		}
		else {
		
			szError.Format(_T("Bitmap must be %dx%d pixels"),pBitmapMemoryInfo[index].usPixelWidth,
				pBitmapMemoryInfo[index].usPixelHeight);
			AfxMessageBox(szError);
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	return FALSE;
}


BOOL CMemoryBitmapInfo::Store_Bitmap(int iBitmapSize,BYTE *ptrToDataArray)
{
	BYTE *pbDatabitmap = NULL;
	unsigned short usIndex;
	pbDatabitmap = MyBitmap.GetBuffer();

	TRACE1("Store Bitmap at address 0x%x\n",&ptrToDataArray[0]);

	if (pbDatabitmap != NULL) {
		if (ptrToDataArray != NULL) {
			for (usIndex=0;usIndex<iBitmapSize;usIndex++) 
				ptrToDataArray[usIndex] = pbDatabitmap[usIndex];
			
			TRACE1("Last address Bitmap at address 0x%x\n",&ptrToDataArray[usIndex-1]);
			return TRUE;
		}
	}
	return FALSE;
}


// Function to store monochrome bitmaps !!!!!
// Caution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
BOOL CMemoryBitmapInfo::Store_Bitmap(int iWidth,int iHeight,int iBitmapSize,BYTE *ptrToDataArray)
{
	BYTE *pbDatabitmap = NULL;
	unsigned short usIndex;

	pbDatabitmap = MyBitmap.GetBuffer();	
	if (pbDatabitmap != NULL) {
		if (ptrToDataArray != NULL) {
			
			ptrToDataArray[0] = (BYTE) iWidth/256;
			ptrToDataArray[1] = (BYTE) iWidth%256;
			ptrToDataArray[2] = (BYTE) iHeight/256;
			ptrToDataArray[3] = (BYTE) iHeight%256;

			switch (usNodeSoftwareVersion)
			{
// We Must pay special attention to the fact that the bitmapsize corresponds to the
// actual size of the loaded bitmap.
// There is always memory allocated for iBitmapSize + 5 / iBitmapSize + 4
// Because the monochrome bitmpas are not loaded with a header, the header has to added to it ...

				case 0x62:
					for (usIndex=0;usIndex<iBitmapSize;usIndex++) {
						ptrToDataArray[4+usIndex] = pbDatabitmap[usIndex];	
					}					
				break;

				default:
					ptrToDataArray[4] = (BYTE) MONOCHROME_BITMAP;
					for (usIndex=0;usIndex<iBitmapSize;usIndex++) {
						ptrToDataArray[5+usIndex] = pbDatabitmap[usIndex];	
					}
					//ASSERT(
				break;			
			}				
			return TRUE;
		}	
		else {
		
			ASSERT(0);
			return FALSE;
		}
	}
	else {
	
		ASSERT(0);
		return FALSE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMemoryBitmapInfo::AllocateMemoryForNode(CNodeDatabase *pNodeDatabase,CCanNode *pNode)
{
	CString szError;
	BYTE bLcdType;

	if (fMemoryLock == TRUE) {
		szError.Format("Cannot access CmemoryBitmapInfo, Memory is locked by other node");
		AfxMessageBox(szError);
		return FALSE;
	}
	
	if (pNode == NULL) {	
		ASSERT(pNode);
		szError.Format(_T("AllocateMemoryforNode(CCanNode *pNode), Argument pNode refers to NULL"));
		AfxMessageBox(szError);
		return FALSE;
	}


	if ((pNode->bNodeType & NODE_LCD) != NODE_LCD) {	
		szError.Format(_T("AllocateMemoryforNode(CCanNode *pNode), Argument pNode must refer a LCD-Node"));
		AfxMessageBox(szError);
		return FALSE;
	}

	bLcdType = pNode->bNodeFlags & NODE_LCD_TYPE_MASK;
	usNodeSoftwareVersion = pNode->GetSoftwareVersion();

	pAssignedNode = pNode;
	Nodes.Set(pNodeDatabase,pNode);

	switch(usNodeSoftwareVersion) {

		case 0x62 :
		{
			if (bLcdType != NODE_LCD_TYPE_BASIC) {
				szError.Format(_T("Error : Undefined LCD type for LCD Node software version 0x62"));
				AfxMessageBox(szError);
				return FALSE;
			}
			else {
				return (AllocateMem_0x62_BasicBWLCD());
			}			
		}break;

		case 0x63 :
			if (bLcdType != NODE_LCD_TYPE_BASIC) {
				szError.Format(_T("Error : Undefined LCD type for LCD Node software version 0x63"));
				AfxMessageBox(szError);
				return FALSE;
			}
			else {
				return (AllocateMem_0x63_BasicBWLCD());
			}	
		break;

		case 0x64 :
			switch(bLcdType) 
			{
				case NODE_LCD_TYPE_BASIC: 
					return (AllocateMem_0x64_BasicColorLCD());
				break;
		
				case NODE_LCD_TYPE_HC4 : 
					return (AllocateMem_0x64_HC4ColorLCD());
				break;
				
				case NODE_LCD_TYPE_HC5 : 
					return (AllocateMem_0x64_HC5ColorLCD());
				break;

				default :
					szError.Format(_T("Error : Undefined LCD type for LCD Node software version 0x64"));
					AfxMessageBox(szError);
					return FALSE;
				break;
			}
		break;	

		default:
			szError.Format(_T("Error : LCD Node software version 0x%02x not supported"),pNode->GetSoftwareVersion());	// CHANGES_MVS_2008
			AfxMessageBox(szError);
			return FALSE;
		break;
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////
BOOL CMemoryBitmapInfo::AllocateMem_0x64_HC4ColorLCD(void) 
{
	CString szError;

	int i;
	unsigned long ulStartAddress;
	unsigned long ulBitmapSize;
	unsigned long ulDataArrayIndex;
	BYTE bArrayIndex;

	if (pBitmapMemoryInfo != NULL) {
		szError.Format(_T("Cannot allocate memory for 'pBitmapMemoryInfo' because ptr is not NULL"));
		AfxMessageBox(szError);
		return FALSE;
	}

	bMaxBitmaps = BITMAP_HC4_V64_MAX_BITMAPS;//MAX_BITMAPS_V64_HC4;

	try {

		pBitmapMemoryInfo = new STRUCT_BITMAPMEMORY_INFO[bMaxBitmaps];

		// Here we start calculating memory table for bitmaps ...
		// Start At bArrayIndex=0 --> Extra IR Bitmaps ...
		ulStartAddress = EXTRAIR_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_31X31_BITMAP_SIZE;
		ulDataArrayIndex = 0;
		bArrayIndex = 0;

		for (i=0;i<MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4;i++) {
			
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = MAIN_VXX_31X31_BITMAP_SIZE;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_31X31_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 31;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 31;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);
			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);			
		}

		// Continue with Audio Source Bitmaps ...
		// use value of ulDataArrayIndex of previous for loop ...
		ulStartAddress = SRC_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_44X34_BITMAP_SIZE;		
		bArrayIndex = MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4;

		for (i=0;i<MAX_BITMAPS_AUDIO_SOURCES_V64_HC4;i++) {
		
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = 0;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_44X34_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 44;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 34;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);
			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);	
		}

		// Continue with Audio Destination Bitmaps ...
		// use value of ulDataArrayIndex of previous for loop ...
		ulStartAddress = DEST_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_54X44_BITMAP_SIZE;		
		bArrayIndex = MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4 + MAX_BITMAPS_AUDIO_SOURCES_V64_HC4;

		for (i=0;i<MAX_BITMAPS_AUDIO_DESTINATIONS_V64_HC4;i++) {
		
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = 0;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_54X44_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 54;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 44;


			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);	
		}

		// Continue with MainScreen Moods + AudioAction Bitmaps ...
		// use value of ulDataArrayIndex of previous for loop ...
		ulStartAddress = MAIN_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_39X39_BITMAP_SIZE;		
		bArrayIndex = MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4 + MAX_BITMAPS_AUDIO_SOURCES_V64_HC4 +
					  MAX_BITMAPS_AUDIO_DESTINATIONS_V64_HC4;

		for (i=0;i<MAX_BITMAPS_MAINSCREEN_V64_HC4;i++) {
		
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = MAIN_VXX_39X39_BITMAP_SIZE;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_39X39_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 39;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 39;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);	
		}

		ulDataArrayMemoryEndAddress = (pBitmapMemoryInfo[bMaxBitmaps-1].ulMemoryEndAddress) - EXTRAIR_BITMAP_MEMORY_START_ADDRESS_HC4;

		// Here we must allocate memory for pbData ...
		// Sizof memory is ulDataArrayMemoryEndAddress + 1 !!
		pbData = new BYTE[ulDataArrayMemoryEndAddress+1];
	
		for (unsigned long ulIndex = 0;ulIndex < ulDataArrayMemoryEndAddress;ulIndex++)
			pbData[ulIndex] = 0xFF;

		for (i=0;i<bMaxBitmaps;i++) pBitmapMemoryInfo[i].fBitmapStoredToBeSend=FALSE;		
		return TRUE;

	}
	catch(CMemoryException *e) {
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return FALSE;
}

BOOL CMemoryBitmapInfo::AllocateMem_0x64_HC5ColorLCD(void) 
{
	CString szError;

	int i;
	unsigned long ulStartAddress;
	unsigned long ulBitmapSize;
	unsigned long ulDataArrayIndex;
	BYTE bArrayIndex;

	if (pBitmapMemoryInfo != NULL) {
		szError.Format(_T("Cannot allocate memory for 'pBitmapMemoryInfo' because ptr is not NULL"));
		AfxMessageBox(szError);
		return FALSE;
	}

	bMaxBitmaps = BITMAP_HC5_V64_MAX_BITMAPS;//MAX_BITMAPS_V64_HC4;

	try {

		pBitmapMemoryInfo = new STRUCT_BITMAPMEMORY_INFO[bMaxBitmaps];

		// Here we start calculating memory table for bitmaps ...
		// Start At bArrayIndex=0 --> Extra IR Bitmaps ...
		ulStartAddress = EXTRAIR_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_31X31_BITMAP_SIZE;
		ulDataArrayIndex = 0;
		bArrayIndex = 0;

		for (i=0;i<MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4;i++) {
			
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = MAIN_VXX_31X31_BITMAP_SIZE;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_31X31_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 31;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 31;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);			
		}

		// Continue with Audio Source Bitmaps ...
		// use value of ulDataArrayIndex of previous for loop ...
		ulStartAddress = SRC_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_44X34_BITMAP_SIZE;		
		bArrayIndex = MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4;

		for (i=0;i<MAX_BITMAPS_AUDIO_SOURCES_V64_HC4;i++) {
		
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = 0;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_44X34_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 44;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 34;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);	
		}

		// Continue with Audio Destination Bitmaps ...
		// use value of ulDataArrayIndex of previous for loop ...
		ulStartAddress = DEST_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_54X44_BITMAP_SIZE;		
		bArrayIndex = MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4 + MAX_BITMAPS_AUDIO_SOURCES_V64_HC4;

		for (i=0;i<MAX_BITMAPS_AUDIO_DESTINATIONS_V64_HC4;i++) {
		
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = 0;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_54X44_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 54;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 44;


			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);	
		}

		// Continue with MainScreen Moods + AudioAction Bitmaps ...
		// use value of ulDataArrayIndex of previous for loop ...
		ulStartAddress = MAIN_BITMAP_MEMORY_START_ADDRESS_HC4;
		ulBitmapSize = MAIN_COLOR_VXX_39X39_BITMAP_SIZE;		
		bArrayIndex = MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4 + MAX_BITMAPS_AUDIO_SOURCES_V64_HC4 +
					  MAX_BITMAPS_AUDIO_DESTINATIONS_V64_HC4;


		ulDataArrayMemoryEndAddress = (pBitmapMemoryInfo[bMaxBitmaps-1].ulMemoryEndAddress) - 
									   EXTRAIR_BITMAP_MEMORY_START_ADDRESS_HC4;

		// Here we must allocate memory for pbData ...
		// Sizof memory is ulDataArrayMemoryEndAddress + 1 !!
		pbData = new BYTE[ulDataArrayMemoryEndAddress+1];
	
		for (unsigned long ulIndex = 0;ulIndex < ulDataArrayMemoryEndAddress;ulIndex++)
			pbData[ulIndex] = 0xFF;

		for (i=0;i<bMaxBitmaps;i++) pBitmapMemoryInfo[i].fBitmapStoredToBeSend=FALSE;
		
		return TRUE;
	}
	catch(CMemoryException *e) {
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return FALSE;
}

BOOL CMemoryBitmapInfo::AllocateMem_0x64_BasicColorLCD(void) 
{
	CString szError;

	int i;
	unsigned long ulStartAddress;
	unsigned long ulBitmapSize;
	unsigned long ulDataArrayIndex;
	BYTE bArrayIndex;

	if (pBitmapMemoryInfo != NULL) {
		szError.Format(_T("Cannot allocate memory for 'pBitmapMemoryInfo' because ptr is not NULL"));
		AfxMessageBox(szError);
		return FALSE;
	}

	bMaxBitmaps = BITMAP_BASIC_V64_MAX_BITMAPS;

	try {

		pBitmapMemoryInfo = new STRUCT_BITMAPMEMORY_INFO[bMaxBitmaps];

		// Here we start calculating memory table for bitmaps ...

		// Start At bArrayIndex=0 --> Extra IR Bitmaps ...
		ulStartAddress = BITMAP_MEMORY_START_ADDRESS_V64_BASIC;
		ulBitmapSize = MAIN_COLOR_VXX_49x49_BITMAP_SIZE;
		ulDataArrayIndex = 0;
		bArrayIndex = 0;

		for (i=0;i<BITMAP_BASIC_V64_MAX_BITMAPS;i++) {
			
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = MAIN_VXX_49X49_BITMAP_SIZE;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = MAIN_COLOR_VXX_49x49_BITMAP_SIZE;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 49;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 49;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);			
		}

		ulDataArrayMemoryEndAddress = (pBitmapMemoryInfo[bMaxBitmaps-1].ulMemoryEndAddress) - 
									  BITMAP_MEMORY_START_ADDRESS_V64_BASIC;

		// Here we must allocate memory for pbData ...
		// Sizof memory is ulDataArrayMemoryEndAddress + 1 !!
		pbData = new BYTE[ulDataArrayMemoryEndAddress+1];
	
		for (unsigned long ulIndex = 0;ulIndex < ulDataArrayMemoryEndAddress;ulIndex++)
			pbData[ulIndex] = 0xFF;

		for (i=0;i<bMaxBitmaps;i++) pBitmapMemoryInfo[i].fBitmapStoredToBeSend=FALSE;
		
		return TRUE;
	}
	
	catch(CMemoryException *e) {
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return FALSE;
}

BOOL CMemoryBitmapInfo::AllocateMem_0x63_BasicBWLCD(void) 
{

	CString szError;

	int i;
	unsigned long ulStartAddress;
	unsigned long ulBitmapSize;
	unsigned long ulDataArrayIndex;
	BYTE bArrayIndex;

	if (pBitmapMemoryInfo != NULL) {
		szError.Format(_T("Cannot allocate memory for 'pBitmapMemoryInfo' because ptr is not NULL"));
		AfxMessageBox(szError);
		return FALSE;
	}

	bMaxBitmaps = BITMAP_BASIC_V64_MAX_BITMAPS;

	try {

		pBitmapMemoryInfo = new STRUCT_BITMAPMEMORY_INFO[bMaxBitmaps];

		// Here we start calculating memory table for bitmaps ...

		// Start At bArrayIndex=0 --> Extra IR Bitmaps ...
		ulStartAddress = BITMAP_MEMORY_START_ADDRESS_V63;
		ulBitmapSize = MAIN_VXX_49X49_BITMAP_SIZE;
		ulDataArrayIndex = 0;
		bArrayIndex = 0;

		for (i=0;i<BITMAP_BASIC_V64_MAX_BITMAPS;i++) {
			
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = MAIN_VXX_49X49_BITMAP_SIZE;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = 0;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 49;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 49;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);			
		}

		ulDataArrayMemoryEndAddress = (pBitmapMemoryInfo[bMaxBitmaps-1].ulMemoryEndAddress) - 
									  BITMAP_MEMORY_START_ADDRESS_V63;

		// Here we must allocate memory for pbData ...
		// Sizof memory is ulDataArrayMemoryEndAddress + 1 !!
		pbData = new BYTE[ulDataArrayMemoryEndAddress+1];
	
		for (unsigned long ulIndex = 0;ulIndex < ulDataArrayMemoryEndAddress;ulIndex++)
			pbData[ulIndex] = 0xFF;

		for (i=0;i<bMaxBitmaps;i++) pBitmapMemoryInfo[i].fBitmapStoredToBeSend=FALSE;
		
		return TRUE;
	}
	
	catch(CMemoryException *e) {
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return FALSE;
}

BOOL CMemoryBitmapInfo::AllocateMem_0x62_BasicBWLCD(void) 
{
	CString szError;

	int i;
	unsigned long ulStartAddress;
	unsigned long ulBitmapSize;
	unsigned long ulDataArrayIndex;
	BYTE bArrayIndex;

	if (pBitmapMemoryInfo != NULL) {
		szError.Format(_T("Cannot allocate memory for 'pBitmapMemoryInfo' because ptr is not NULL"));
		AfxMessageBox(szError);
		return FALSE;
	}

	bMaxBitmaps = BITMAP_BASIC_V64_MAX_BITMAPS;

	try {

		pBitmapMemoryInfo = new STRUCT_BITMAPMEMORY_INFO[bMaxBitmaps];

		// Here we start calculating memory table for bitmaps ...

		// Start At bArrayIndex=0 --> Extra IR Bitmaps ...
		ulStartAddress = BITMAP_MEMORY_START_ADDRESS_V63;
		ulBitmapSize = MAIN_V62_49X49_BITMAP_SIZE;
		ulDataArrayIndex = 0;
		bArrayIndex = 0;

		for (i=0;i<BITMAP_BASIC_V64_MAX_BITMAPS;i++) {
			
			pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress = ulStartAddress;
			pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress = ((ulStartAddress + ulBitmapSize ) -1);

			pBitmapMemoryInfo[bArrayIndex].ulDataArrayIndex = ulDataArrayIndex;

			pBitmapMemoryInfo[bArrayIndex].usMonochromeBitmapSize = MAIN_V62_49X49_BITMAP_SIZE;
			pBitmapMemoryInfo[bArrayIndex].usColorBitmapSize = 0;

			pBitmapMemoryInfo[bArrayIndex].usPixelWidth = 49;
			pBitmapMemoryInfo[bArrayIndex].usPixelHeight = 49;

			// Debug ...
			TRACE3("Bitmap %d : StartAddress = 0x%x, EndAddress = 0x%x\n",bArrayIndex,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryStartAddress,
																		  pBitmapMemoryInfo[bArrayIndex].ulMemoryEndAddress);

			ulDataArrayIndex += ulBitmapSize;
			ulStartAddress += ulBitmapSize,
			bArrayIndex++;
			
			ASSERT(bArrayIndex <= bMaxBitmaps);			
		}

		ulDataArrayMemoryEndAddress = (pBitmapMemoryInfo[bMaxBitmaps-1].ulMemoryEndAddress) - 
									  BITMAP_MEMORY_START_ADDRESS_V63;

		// Here we must allocate memory for pbData ...
		// Sizof memory is ulDataArrayMemoryEndAddress + 1 !!
		pbData = new BYTE[ulDataArrayMemoryEndAddress+1];
	
		for (unsigned long ulIndex = 0;ulIndex < ulDataArrayMemoryEndAddress;ulIndex++)
			pbData[ulIndex] = 0xFF;

		for (i=0;i<bMaxBitmaps;i++) pBitmapMemoryInfo[i].fBitmapStoredToBeSend=FALSE;		

		return TRUE;
	}
	
	catch(CMemoryException *e) {
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
BOOL CMemoryBitmapInfo::SetLock(BOOL fLock) {
	fMemoryLock = fLock;
	return (fMemoryLock);
}

BOOL CMemoryBitmapInfo::IsLocked(void) {
	return fMemoryLock;
}

BYTE CMemoryBitmapInfo::GetAssignedNodeAddress(void) {
	if (pAssignedNode == NULL) return 0xFF;
	else return (pAssignedNode->bLogicalAddress);
}

//////////////////////////////////////////////////////////////////////////////////