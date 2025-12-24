// ModemFrame.cpp: implementation of the CModemFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ModemFrame.h"

#include "NodeMess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CModemFrame::CModemFrame(BYTE *pbKar) :
pbReceivedFrame(pbKar)
{
	ASSERT( pbReceivedFrame != 0);
}

CModemFrame::CModemFrame() :
pbReceivedFrame(0)
{

}



CModemFrame::~CModemFrame()
{
	pbReceivedFrame = NULL;	
}


//////////////////////////////////////////////////////////////////////
void CModemFrame::Assign(char *pData)
{
	pbReceivedFrame = reinterpret_cast<BYTE*>(pData);
	ASSERT(pbReceivedFrame);
}

//////////////////////////////////////////////////////////////////////
BYTE CModemFrame::ConvertToBYTE(BYTE *pbData, BYTE bPos) const
{
   BYTE bRetVal = 0;		// CHANGES_MVS_2008_WARNINGS

   if (bPos<0) {
	   return bRetVal;
   }

   // Decode message and build event...
	if ((pbData[bPos] >= '0') && (pbData[bPos] <= '9'))
	     bRetVal = (pbData[bPos] - 48)*16;
	else if ((pbData[bPos] >= 'A') && (pbData[bPos] <= 'F'))
		  bRetVal = (pbData[bPos] - 55)*16;
	else if ((pbData[bPos] >= 'a') && (pbData[bPos] <= 'f'))
		  bRetVal = (pbData[bPos] - 87)*16;

	if ((pbData[bPos+1] >= '0') && (pbData[bPos+1] <= '9'))
		  bRetVal += (pbData[bPos+1]- 48);
	else if ((pbData[bPos+1] >= 'A') && (pbData[bPos+1] <= 'F'))
		  bRetVal += (pbData[bPos+1]- 55);
	else if ((pbData[bPos+1] >= 'a') && (pbData[bPos+1] <= 'f'))
		  bRetVal += (pbData[bPos+1]- 87);

   return(bRetVal);

} 

unsigned short CModemFrame::ConvertToWORD(BYTE *pbData, BYTE bPos) const
{
   unsigned short usRetVal = 0;	// CHANGES_MVS_2008_WARNINGS

	  // Decode message and build event...
	  if ((pbData[bPos] >= '0') && (pbData[bPos] <= '9'))
		  usRetVal = (pbData[bPos] - 48)*4096;
	  else if ((pbData[bPos] >= 'A') && (pbData[bPos] <= 'F'))
		  usRetVal = (pbData[bPos] - 55)*4096;
	  else if ((pbData[bPos] >= 'a') && (pbData[bPos] <= 'f'))
		  usRetVal = (pbData[bPos] - 87)*4096;

	  if ((pbData[bPos+1] >= '0') && (pbData[bPos+1] <= '9'))
		  usRetVal += (pbData[bPos+1]- 48)*256;
	  else if ((pbData[bPos+1] >= 'A') && (pbData[bPos+1] <= 'F'))
		  usRetVal += (pbData[bPos+1]- 55)*256;
	  else if ((pbData[bPos+1] >= 'a') && (pbData[bPos+1] <= 'f'))
		  usRetVal += (pbData[bPos+1]- 87)*256;

	  if ((pbData[bPos+2] >= '0') && (pbData[bPos+2] <= '9'))
		  usRetVal += (pbData[bPos+2]- 48)*16;
	  else if ((pbData[bPos+2] >= 'A') && (pbData[bPos+2] <= 'F'))
		  usRetVal += (pbData[bPos+2]- 55)*16;
	  else if ((pbData[bPos+2] >= 'a') && (pbData[bPos+2] <= 'f'))
		  usRetVal += (pbData[bPos+2]- 87)*16;

	  if ((pbData[bPos+3] >= '0') && (pbData[bPos+3] <= '9'))
		  usRetVal += (pbData[bPos+3]- 48);
	  else if ((pbData[bPos+3] >= 'A') && (pbData[bPos+3] <= 'F'))
		  usRetVal += (pbData[bPos+3]- 55);
	  else if ((pbData[bPos+3] >= 'a') && (pbData[bPos+3] <= 'f'))
		  usRetVal += (pbData[bPos+3]- 87);

   return(usRetVal);

} // unsigned short CPMModemFrame::ConvertToWORD(BYTE *pbData, BYTE bPos)


unsigned long CModemFrame::ConvertTo24BIT(BYTE *pbData, BYTE bPos) const
{
   unsigned long ulRetVal=0;

	  // Decode message and build event...
	  if ((pbData[bPos] >= '0') && (pbData[bPos] <= '9'))
		  ulRetVal = (pbData[bPos] - 48)*1048576;
	  else if ((pbData[bPos] >= 'A') && (pbData[bPos] <= 'F'))
		  ulRetVal = (pbData[bPos] - 55)*1048576;
	  else if ((pbData[bPos] >= 'a') && (pbData[bPos] <= 'f'))
		  ulRetVal = (pbData[bPos] - 87)*1048576;

	  if ((pbData[bPos+1] >= '0') && (pbData[bPos+1] <= '9'))
		  ulRetVal += (pbData[bPos+1]- 48)*65536;
	  else if ((pbData[bPos+1] >= 'A') && (pbData[bPos+1] <= 'F'))
		  ulRetVal += (pbData[bPos+1]- 55)*65536;
	  else if ((pbData[bPos+1] >= 'a') && (pbData[bPos+1] <= 'f'))
		  ulRetVal += (pbData[bPos+1]- 87)*65536;

	  if ((pbData[bPos+2] >= '0') && (pbData[bPos+2] <= '9'))
		  ulRetVal += (pbData[bPos+2]- 48)*4096;
	  else if ((pbData[bPos+2] >= 'A') && (pbData[bPos+2] <= 'F'))
		  ulRetVal += (pbData[bPos+2]- 55)*4096;
	  else if ((pbData[bPos+2] >= 'a') && (pbData[bPos+2] <= 'f'))
		  ulRetVal += (pbData[bPos+2]- 87)*4096;

	  if ((pbData[bPos+3] >= '0') && (pbData[bPos+3] <= '9'))
		  ulRetVal += (pbData[bPos+3]- 48)*256;
	  else if ((pbData[bPos+3] >= 'A') && (pbData[bPos+3] <= 'F'))
		  ulRetVal += (pbData[bPos+3]- 55)*256;
	  else if ((pbData[bPos+3] >= 'a') && (pbData[bPos+3] <= 'f'))
		  ulRetVal += (pbData[bPos+3]- 87)*256;

	  if ((pbData[bPos+4] >= '0') && (pbData[bPos+4] <= '9'))
		  ulRetVal += (pbData[bPos+4]- 48)*16;
	  else if ((pbData[bPos+4] >= 'A') && (pbData[bPos+4] <= 'F'))
		  ulRetVal += (pbData[bPos+4]- 55)*16;
	  else if ((pbData[bPos+4] >= 'a') && (pbData[bPos+4] <= 'f'))
		  ulRetVal += (pbData[bPos+4]- 87)*16;

	  if ((pbData[bPos+5] >= '0') && (pbData[bPos+5] <= '9'))
		  ulRetVal += (pbData[bPos+5]- 48);
	  else if ((pbData[bPos+5] >= 'A') && (pbData[bPos+5] <= 'F'))
		  ulRetVal += (pbData[bPos+5]- 55);
	  else if ((pbData[bPos+5] >= 'a') && (pbData[bPos+5] <= 'f'))
		  ulRetVal += (pbData[bPos+5]- 87);

   return(ulRetVal);
} 

BOOL CModemFrame::CheckXOR(void) 
{
      BYTE bCalcCheckSum=0;
      const BYTE bLength=ConvertToBYTE(pbReceivedFrame,OFFSET_MESSAGE_LENGTH);
      const BYTE bReceivedCheckSum=ConvertToBYTE(pbReceivedFrame,OFFSET_MESSAGE_LENGTH+bLength+2);

      for(BYTE i=0;i<bLength+2;i++) {
		  bCalcCheckSum^=pbReceivedFrame[i];        
	  }
       
      if (bCalcCheckSum==bReceivedCheckSum) {
		  return(true);
	  }
      else {
		  return(false); 
	  }
}


