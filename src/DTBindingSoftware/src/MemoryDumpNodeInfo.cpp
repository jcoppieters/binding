// MemoryDumpNodeInfo.cpp: implementation of the CMemoryDumpNodeInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MemoryDumpNodeInfo.h"

//-------------------------
#include "CanNode.h"
//-------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryDumpNodeInfo::CMemoryDumpNodeInfo()
{

}

CMemoryDumpNodeInfo::~CMemoryDumpNodeInfo()
{

}

/////////////////////////////////////////////////////////////////////
BOOL CMemoryDumpNodeInfo::Init(CCanNode *pNode)
{

	if (pNode->GetSoftwareVersion() == 0x64) {
		switch (pNode->bNodeFlags & NODE_LCD_TYPE_MASK)
		{
			case NODE_LCD_TYPE_BASIC :
			{
				MemoryDumpAddress.ulBitmapMemoryStartAddress = BITMAP_MEMORY_START_ADDRESS_V64_BASIC;
 				MemoryDumpAddress.ulBitmapMemoryEndAddress = BITMAP_MEMORY_END_ADDRESS_V64_BASIC;
				MemoryDumpAddress.ulDynamicMemoryStartAddress = DYNAMIC_MEMORY_START_ADDRESS_V64_BASIC;
 				MemoryDumpAddress.ulDynamicMemoryEndAddress = DYNAMIC_MEMORY_END_ADDRESS_V64_BASIC;
				MemoryDumpAddress.ulStaticMemoryStartAddress = STATIC_MEMORY_START_ADDRESS_V64_BASIC;
				MemoryDumpAddress.ulStaticMemoryEndAddress = STATIC_MEMORY_END_ADDRESS_V64_BASIC;				
				MemoryDumpAddress.ulAudioBitmapMemoryStartAddress = 0;
				MemoryDumpAddress.ulAudioBitmapMemoryEndAddress = 0;
			}break;

			default:		// HC4 + HC5...
			{
				MemoryDumpAddress.ulBitmapMemoryStartAddress = BITMAP_MEMORY_START_ADDRESS_V64;
 				MemoryDumpAddress.ulBitmapMemoryEndAddress = BITMAP_MEMORY_END_ADDRESS_V64;
				MemoryDumpAddress.ulDynamicMemoryStartAddress = DYNAMIC_MEMORY_START_ADDRESS_V64;
 				MemoryDumpAddress.ulDynamicMemoryEndAddress = DYNAMIC_MEMORY_END_ADDRESS_V64;
				MemoryDumpAddress.ulStaticMemoryStartAddress = STATIC_MEMORY_START_ADDRESS_V64;
				MemoryDumpAddress.ulStaticMemoryEndAddress = STATIC_MEMORY_END_ADDRESS_V64;
				MemoryDumpAddress.ulAudioBitmapMemoryStartAddress = BITMAP_MEMORY_START_ADDRESS_V64;
				MemoryDumpAddress.ulAudioBitmapMemoryEndAddress = MAIN_BITMAP_MEMORY_START_ADDRESS_HC4 - 1;
			}break;
		}
	}
	else {		// LCD SWVERSION == 0x63 ...
		MemoryDumpAddress.ulBitmapMemoryStartAddress = BITMAP_MEMORY_START_ADDRESS_V63;
 		MemoryDumpAddress.ulBitmapMemoryEndAddress = BITMAP_MEMORY_END_ADDRESS_V63;
		MemoryDumpAddress.ulDynamicMemoryStartAddress = DYNAMIC_MEMORY_START_ADDRESS_V63;
 		MemoryDumpAddress.ulDynamicMemoryEndAddress = DYNAMIC_MEMORY_END_ADDRESS_V63;
		MemoryDumpAddress.ulStaticMemoryStartAddress = STATIC_MEMORY_START_ADDRESS_V63;
		MemoryDumpAddress.ulStaticMemoryEndAddress = STATIC_MEMORY_END_ADDRESS_V63;
		MemoryDumpAddress.ulAudioBitmapMemoryStartAddress = 0;
		MemoryDumpAddress.ulAudioBitmapMemoryEndAddress = 0;
	}
	return TRUE;
}
