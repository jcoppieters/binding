// MemoryDumpNodeInfo.h: interface for the CMemoryDumpNodeInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYDUMPNODEINFO_H__9133F058_B1C4_4C24_B806_D117FDEDDA68__INCLUDED_)
#define AFX_MEMORYDUMPNODEINFO_H__9133F058_B1C4_4C24_B806_D117FDEDDA68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCanNode;

//---------------------------------------------------

#define DYNAMIC_MEMORY_START_ADDRESS_V63		0x001D0000
#define DYNAMIC_MEMORY_END_ADDRESS_V63			0x001FFBFB
#define STATIC_MEMORY_START_ADDRESS_V63			0x001C0000
#define STATIC_MEMORY_END_ADDRESS_V63			0x001CFFFF
#define BITMAP_MEMORY_START_ADDRESS_V63			0x001CE000
#define BITMAP_MEMORY_END_ADDRESS_V63			0x001CFFFF


#define DYNAMIC_MEMORY_START_ADDRESS_V64		0x00380000
#define DYNAMIC_MEMORY_END_ADDRESS_V64			0x003FFFFF
#define STATIC_MEMORY_START_ADDRESS_V64			0x00300000
#define STATIC_MEMORY_END_ADDRESS_V64			0x0031FFFF
#define BITMAP_MEMORY_START_ADDRESS_V64			0x00320000
#define BITMAP_MEMORY_END_ADDRESS_V64			0x0037FFFF

// ---------------
#define MAIN_BITMAP_MEMORY_START_ADDRESS_HC4    0x0032D696	/*TM 24/01/2002 : 0x0032CD49*/
#define DEST_BITMAP_MEMORY_START_ADDRESS_HC4    0x003241C6
#define SRC_BITMAP_MEMORY_START_ADDRESS_HC4     0x003212DE
#define EXTRAIR_BITMAP_MEMORY_START_ADDRESS_HC4 0x00320000

/*  BASIC COLOR LCD */
#define DYNAMIC_MEMORY_START_ADDRESS_V64_BASIC  0x00380000
#define DYNAMIC_MEMORY_END_ADDRESS_V64_BASIC    0x003FFFFF
#define STATIC_MEMORY_START_ADDRESS_V64_BASIC   0x00300000
#define STATIC_MEMORY_END_ADDRESS_V64_BASIC     0x0031FFFF
#define BITMAP_MEMORY_START_ADDRESS_V64_BASIC   0x00320000
#define BITMAP_MEMORY_END_ADDRESS_V64_BASIC     0x0032FFFF	/*0x0037FFFF*/

//---------------------------------------------------

typedef struct 
{
	unsigned long ulDynamicMemoryStartAddress;
	unsigned long ulDynamicMemoryEndAddress;
	unsigned long ulStaticMemoryStartAddress;
	unsigned long ulStaticMemoryEndAddress;
	unsigned long ulBitmapMemoryStartAddress;
	unsigned long ulBitmapMemoryEndAddress;
	unsigned long ulAudioBitmapMemoryStartAddress;
	unsigned long ulAudioBitmapMemoryEndAddress;

} STRUCT_ADDRESSES;


// --------------------------------------------------
class CMemoryDumpNodeInfo  
{
protected:
	STRUCT_ADDRESSES MemoryDumpAddress;

public:
	CMemoryDumpNodeInfo();
	virtual ~CMemoryDumpNodeInfo();

	BOOL Init(CCanNode *pNode);
	STRUCT_ADDRESSES *GetNodeAddressInfo(void)	{return &MemoryDumpAddress;};
};

#endif // !defined(AFX_MEMORYDUMPNODEINFO_H__9133F058_B1C4_4C24_B806_D117FDEDDA68__INCLUDED_)
