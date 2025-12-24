// TXMessageQueue.cpp: implementation of the CTXMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "TXMessageQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTXMessageQueue::CTXMessageQueue()
{
	Flush();
}

CTXMessageQueue::~CTXMessageQueue()
{

}

////////////////////////////////////////////////////////
CString CTXMessageQueue::GetMessage(ENUM_APDU_HEADER *Header)
{
	if (nCount == 0) 
		return CString("");

	CString szString(szMessageQueue[nMessageReadPtr]);
	*Header = APDUHeader[nMessageReadPtr];

	szMessageQueue[nMessageReadPtr].Empty();

	if (nMessageReadPtr++>=(MAX_NR_TX_MESSAGES-1))
		nMessageReadPtr=0;

	nCount--;
	return(szString);
}

int CTXMessageQueue::Count()
{
	return (nCount);
}

BOOL CTXMessageQueue::InsertMessage(const CString& szString,ENUM_APDU_HEADER Header)
{
   if (nCount < MAX_NR_TX_MESSAGES)
   {
      szMessageQueue[nMessageWritePtr] = szString;
	  APDUHeader[nMessageWritePtr] = Header;

      if (nMessageWritePtr++>=(MAX_NR_TX_MESSAGES-1))
         nMessageWritePtr=0;

      nCount++;
	  return TRUE;
   }
         
   AfxMessageBox("FATAL ERROR, out of Send resources...",MB_ICONSTOP);
   return FALSE;
}

void CTXMessageQueue::Flush()
{
	nCount = 0;
	nMessageReadPtr = 0;
	nMessageWritePtr = 0;

	for(int i=0;i<MAX_NR_TX_MESSAGES;i++)
		szMessageQueue[i].Empty();
}
