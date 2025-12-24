// MemoryBitmapInfo.h: interface for the CMemoryBitmapInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYBITMAPINFO_H__6D7D974D_7462_4E82_A7C1_7CECE4C606BD__INCLUDED_)
#define AFX_MEMORYBITMAPINFO_H__6D7D974D_7462_4E82_A7C1_7CECE4C606BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



// ----------------------------------------------------------
class CCanNode;
class CNodeDatabase;

#include "MemoryMyBitmap.h"

// ----------------------------------------------------------

// --> We May only use these macro's : Bitmapsize + 5 
#define MAIN_V62_49X49_BITMAP_SIZE						347	 //SPECIAL HEADER ONLY 4 BYTES LONG !!

#define MAIN_VXX_49X49_BITMAP_SIZE						348	 //MAIN_49X49_BITMAP_SIZE+5
#define MAIN_VXX_39X39_BITMAP_SIZE						200  //MAIN_39X39_BITMAP_SIZE+5
#define MAIN_VXX_31X31_BITMAP_SIZE						129  //MAIN_31X31_BITMAP_SIZE+5
#define MAIN_COLOR_VXX_39X39_BITMAP_SIZE				1526 //MAIN_COLOR_39X39_BITMAP_SIZE+5 // 8 Color bitmaps
#define MAIN_COLOR_VXX_44X34_BITMAP_SIZE				1501 //MAIN_COLOR_44X34_BITMAP_SIZE+5 // 8 Color bitmaps
#define MAIN_COLOR_VXX_54X44_BITMAP_SIZE				2381 //MAIN_COLOR_54X44_BITMAP_SIZE+5 // 8 Color bitmaps
#define MAIN_COLOR_VXX_31X31_BITMAP_SIZE				966  //MAIN_COLOR_31X31_BITMAP_SIZE+5
#define MAIN_COLOR_VXX_49x49_BITMAP_SIZE				2406 //MAIN_COLOR_49x49_BITMAP_SIZE+5

//-------------------- HC4 Color LCD (0x64) -----------------

#define MAIN_BITMAP_MEMORY_START_ADDRESS_HC4    0x0032D696	
#define DEST_BITMAP_MEMORY_START_ADDRESS_HC4    0x003241C6
#define SRC_BITMAP_MEMORY_START_ADDRESS_HC4     0x003212DE
#define EXTRAIR_BITMAP_MEMORY_START_ADDRESS_HC4 0x00320000


#define MAX_BITMAPS_MAINSCREEN_V64_HC4					28
#define MAX_BITMAPS_AUDIO_SOURCES_V64_HC4				8
#define MAX_BITMAPS_AUDIO_DESTINATIONS_V64_HC4			16
#define MAX_BITMAPS_AUDIO_EXTRAIR_V64_HC4				5


// ------------------- BASIC Color LCD (0x64) ------------------

#define BITMAP_MEMORY_START_ADDRESS_V64_BASIC   0x00320000
#define BITMAP_MEMORY_END_ADDRESS_V64_BASIC     0x0032FFFF	/*0x0037FFFF*/



// --------------------- BASIC B/W LCD (0x63) -------------------

#define BITMAP_MEMORY_START_ADDRESS_V63			0x001CE000
#define BITMAP_MEMORY_END_ADDRESS_V63			0x001CFFFF

// ----------------------------------------------------------


typedef enum {

	BITMAP_HC4_V64_EXTRAIR_0				= 0,
	BITMAP_HC4_V64_EXTRAIR_1				= 1,
	BITMAP_HC4_V64_EXTRAIR_2				= 2,
	BITMAP_HC4_V64_EXTRAIR_3				= 3,
	BITMAP_HC4_V64_EXTRAIR_4				= 4,
	BITMAP_HC4_V64_AUDIOSOURCE0				= 5,
	BITMAP_HC4_V64_AUDIOSOURCE1				= 6,
	BITMAP_HC4_V64_AUDIOSOURCE2				= 7,
	BITMAP_HC4_V64_AUDIOSOURCE3				= 8,
	BITMAP_HC4_V64_AUDIOSOURCE4				= 9,
	BITMAP_HC4_V64_AUDIOSOURCE5				= 10,
	BITMAP_HC4_V64_AUDIOSOURCE6				= 11,
	BITMAP_HC4_V64_AUDIOSOURCE7				= 12,
	BITMAP_HC4_V64_AUDIODEST0				= 13,
	BITMAP_HC4_V64_AUDIODEST1				= 14,
	BITMAP_HC4_V64_AUDIODEST2				= 15,
	BITMAP_HC4_V64_AUDIODEST3				= 16,
	BITMAP_HC4_V64_AUDIODEST4				= 17,
	BITMAP_HC4_V64_AUDIODEST5				= 18,
	BITMAP_HC4_V64_AUDIODEST6				= 19,
	BITMAP_HC4_V64_AUDIODEST7				= 20,
	BITMAP_HC4_V64_AUDIODEST8				= 21,
	BITMAP_HC4_V64_AUDIODEST9				= 22,
	BITMAP_HC4_V64_AUDIODEST10				= 23,
	BITMAP_HC4_V64_AUDIODEST11				= 24,
	BITMAP_HC4_V64_AUDIODEST12				= 25,
	BITMAP_HC4_V64_AUDIODEST13				= 26,
	BITMAP_HC4_V64_AUDIODEST14				= 27,
	BITMAP_HC4_V64_AUDIODEST15				= 28,
	BITMAP_HC4_V64_MOOD0					= 29,
	BITMAP_HC4_V64_MOOD1					= 30,
	BITMAP_HC4_V64_MOOD2					= 31,
	BITMAP_HC4_V64_MOOD3					= 32,
	BITMAP_HC4_V64_MOOD4					= 33,
	BITMAP_HC4_V64_MOOD5					= 34,
	BITMAP_HC4_V64_MOOD6					= 35,
	BITMAP_HC4_V64_MOOD7					= 36,
	BITMAP_HC4_V64_MOOD8					= 37,
	BITMAP_HC4_V64_MOOD9					= 38,
	BITMAP_HC4_V64_MOOD10					= 39,
	BITMAP_HC4_V64_MOOD11					= 40,
	BITMAP_HC4_V64_MOOD12					= 41,
	BITMAP_HC4_V64_MOOD13					= 42,
	BITMAP_HC4_V64_MOOD14					= 43,
	BITMAP_HC4_V64_MOOD15					= 44,
	BITMAP_HC4_V64_MOOD16					= 45,
	BITMAP_HC4_V64_MAINSCREEN_SOURCE0		= 46,
	BITMAP_HC4_V64_MAINSCREEN_SOURCE1		= 47,
	BITMAP_HC4_V64_MAINSCREEN_SOURCE2		= 48,
	BITMAP_HC4_V64_MAINSCREEN_SOURCE3		= 49,
	BITMAP_HC4_V64_MAINSCREEN_ACTION0		= 50,
	BITMAP_HC4_V64_MAINSCREEN_ACTION1		= 51,
	BITMAP_HC4_V64_MAINSCREEN_ACTION2		= 52,
	BITMAP_HC4_V64_MAINSCREEN_ACTION3		= 53,
	BITMAP_HC4_V64_MAINSCREEN_ACTION4		= 54,
	BITMAP_HC4_V64_MAINSCREEN_ACTION5		= 55,
	BITMAP_HC4_V64_MAINSCREEN_ACTION6		= 56,
	BITMAP_HC4_V64_MAX_BITMAPS				

} ENUM_HC4BITMAP_ALLOCATION;


typedef enum {

	BITMAP_BASIC_V64_MOOD0					= 0,	//Tuin
	BITMAP_BASIC_V64_MOOD1					= 1,	//zon
	BITMAP_BASIC_V64_MOOD2					= 2,	//film
	BITMAP_BASIC_V64_MOOD3					= 3,	//weggaan
	BITMAP_BASIC_V64_MOOD4					= 4,	//maan
	BITMAP_BASIC_V64_MOOD5					= 5,	//Cocoon
	BITMAP_BASIC_V64_MOOD6					= 6,	//'party'
	BITMAP_BASIC_V64_MOOD7					= 7,	//eten
	BITMAP_BASIC_V64_MOOD8					= 8,	//animatie
	BITMAP_BASIC_V64_MOOD9					= 9,	//boek lezen
	BITMAP_BASIC_V64_SENS					= 10,	//Sensor-Temp
	BITMAP_BASIC_V64_CONTINUE				= 11,	//Continue
	BITMAP_BASIC_V64_SOURCE0				= 12,	//Source CD
	BITMAP_BASIC_V64_SOURCE1				= 13,	//Source TAPE
	BITMAP_BASIC_V64_SOURCE2				= 14,	//Source TUNER
	BITMAP_BASIC_V64_SOURCE3				= 15,	//Source TV
	BITMAP_BASIC_V64_VOLUP					= 16,	//Volume Up
	BITMAP_BASIC_V64_VOLDN					= 17,	//Volume Dn
	BITMAP_BASIC_V64_MAX_BITMAPS

} ENUM_BASICBITMAP_ALLOCATION;

typedef enum {

	BITMAP_HC5_V64_EXTRAIR_0				= 0,
	BITMAP_HC5_V64_EXTRAIR_1				= 1,
	BITMAP_HC5_V64_EXTRAIR_2				= 2,
	BITMAP_HC5_V64_EXTRAIR_3				= 3,
	BITMAP_HC5_V64_EXTRAIR_4				= 4,
	BITMAP_HC5_V64_AUDIOSOURCE0				= 5,
	BITMAP_HC5_V64_AUDIOSOURCE1				= 6,
	BITMAP_HC5_V64_AUDIOSOURCE2				= 7,
	BITMAP_HC5_V64_AUDIOSOURCE3				= 8,
	BITMAP_HC5_V64_AUDIOSOURCE4				= 9,
	BITMAP_HC5_V64_AUDIOSOURCE5				= 10,
	BITMAP_HC5_V64_AUDIOSOURCE6				= 11,
	BITMAP_HC5_V64_AUDIOSOURCE7				= 12,
	BITMAP_HC5_V64_AUDIODEST0				= 13,
	BITMAP_HC5_V64_AUDIODEST1				= 14,
	BITMAP_HC5_V64_AUDIODEST2				= 15,
	BITMAP_HC5_V64_AUDIODEST3				= 16,
	BITMAP_HC5_V64_AUDIODEST4				= 17,
	BITMAP_HC5_V64_AUDIODEST5				= 18,
	BITMAP_HC5_V64_AUDIODEST6				= 19,
	BITMAP_HC5_V64_AUDIODEST7				= 20,
	BITMAP_HC5_V64_AUDIODEST8				= 21,
	BITMAP_HC5_V64_AUDIODEST9				= 22,
	BITMAP_HC5_V64_AUDIODEST10				= 23,
	BITMAP_HC5_V64_AUDIODEST11				= 24,
	BITMAP_HC5_V64_AUDIODEST12				= 25,
	BITMAP_HC5_V64_AUDIODEST13				= 26,
	BITMAP_HC5_V64_AUDIODEST14				= 27,
	BITMAP_HC5_V64_AUDIODEST15				= 28,
	BITMAP_HC5_V64_MAX_BITMAPS				

} ENUM_HC5BITMAP_ALLOCATION;

//------------------------------------------------------------


// 1) this structure is used to allocate memory for pbData array ...
//    the memory allocated per bitmap is always biggest memory needed (= Color Bitmap)
// 2) Structure is used to check the loaded bitmap with the allocated memory
//    for this bitmap ...

typedef struct {

	// these values are initialised ifv type node ...

	unsigned long ulMemoryStartAddress;		//refers to address location in LCD.
	unsigned long ulMemoryEndAddress;		//refers to absolute end address in LCD.	
	unsigned long ulDataArrayIndex;			//refers to pbData Array location.

	unsigned short usPixelWidth;
	unsigned short usPixelHeight;
	unsigned short usMonochromeBitmapSize;
	unsigned short usColorBitmapSize;

	//Bitmap Actual size it takes in the DataArray - 
	//this is in function of (Color/Monochrome) bitmap ...
	unsigned short usBitmapSize;			

	BOOL fBitmapStoredToBeSend;


} STRUCT_BITMAPMEMORY_INFO;

// ----------------------------------------------------------

class CMemoryCompatibleNodes
{
	enum { MAX_COMPATIBLE_NODES = 254 };

protected:
	BYTE CompatibleNodes[MAX_COMPATIBLE_NODES];	
	BYTE bCount;

protected:
	void Clear(void);

public:
	void Set(CNodeDatabase *pNodeDatabase,CCanNode *pNode);
	
	BYTE Count(void) {
		return bCount;
	}

	BYTE Get(BYTE i) {
		if (i<bCount) return CompatibleNodes[i];
		return 0xFF;
	}
};

// ----------------------------------------------------------
typedef enum {

	MEMORYBITMAPINFO_TASK_UPLOAD,
	MEMORYBITMAPINFO_TASK_UPLOADTOALL,
	MEMORYBITMAPINFO_TASK_NOTASK

}ENUM_MEMORYBITMAPINFO_TASK;


class CMemoryBitmapInfo  
{
public:

	static CMemoryBitmapInfo* GetInstance();

protected:

	ENUM_MEMORYBITMAPINFO_TASK CurrentTask;

	CCanNode *pAssignedNode;

	unsigned short usNodeSoftwareVersion;
	BOOL fMemoryLock;

	BYTE bMaxBitmaps;
	unsigned long ulDataArrayMemoryEndAddress;
	
	CMemoryMyBitmap	MyBitmap;

	STRUCT_BITMAPMEMORY_INFO *pBitmapMemoryInfo;
	BYTE *pbData;	

	BOOL CheckPreConditions(int index);

public:
	CMemoryBitmapInfo();
	virtual ~CMemoryBitmapInfo();

	CMemoryCompatibleNodes Nodes;

protected:
	BOOL AllocateMem_0x64_HC5ColorLCD(void);
	BOOL AllocateMem_0x64_HC4ColorLCD(void);
	BOOL AllocateMem_0x64_BasicColorLCD(void);

	BOOL AllocateMem_0x63_BasicBWLCD(void);
	BOOL AllocateMem_0x62_BasicBWLCD(void);


	BOOL Store_Bitmap(int iWidth,int iHeight,int iBitmapSize,BYTE *);
	BOOL Store_Bitmap(int iBitmapSize,BYTE *);

public:
	BOOL AllocateMemoryForNode(CNodeDatabase *pNodeDatabase,CCanNode *pNode);
	BOOL FreeAllocatedMemory(void);

	BOOL StoreBitmap(CString szFileName,int index);
	BOOL IsBitmapStoredToSend(int index);

	int GetNrOfStoredBitmaps(void);
	BOOL GetStoredBitmap(int index,unsigned long *ulMemoryStart,unsigned short *usLength,
						 BYTE **ppbData);		
	
	BOOL ClearBitmap(int index);
	void ClearAllBitmaps(void);

	BOOL SetLock(BOOL fLock);
	BOOL IsLocked(void);

//-----
	BOOL SetTask(ENUM_MEMORYBITMAPINFO_TASK task) {
		if (fMemoryLock == TRUE) return FALSE;
		CurrentTask = task;
		return TRUE;
	}

	BOOL isSendToAll(void) {
		return (CurrentTask == MEMORYBITMAPINFO_TASK_UPLOADTOALL);
	}

	BYTE GetAssignedNodeAddress(void); 


/*
	void SetArrayCompatibleNodes(CCanNode *pNode);
	void ClearCompatibleNodes(void);
	void CountCompatibleNodes
*/
};

#endif // !defined(AFX_MEMORYBITMAPINFO_H__6D7D974D_7462_4E82_A7C1_7CECE4C606BD__INCLUDED_)
