// MessageQueue.cpp: implementation of the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Timer.h"
#include "MessageQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageQueue::CMessageQueue()
{

}

CMessageQueue::~CMessageQueue()
{

}

////////////////////////////////////////////////////////
CString CMessageQueue::GetMessage()
{
	CString szString;

	if (nCount == 0) 
		return szString;

	szString = szMessageQueue[nMessageReadPtr];
	szMessageQueue[nMessageReadPtr].Empty();

	if (nMessageReadPtr++>=(MAX_NR_TX_MESSAGES-1))
		nMessageReadPtr=0;
	nCount--;

	return(szString);
}

int CMessageQueue::Count()
{
	return (nCount);
}

BOOL CMessageQueue::InsertMessage(CString szString)
{
   if (nCount < MAX_NR_TX_MESSAGES)
   {
      szMessageQueue[nMessageWritePtr] = szString;
      if (nMessageWritePtr++>=(MAX_NR_TX_MESSAGES-1))
         nMessageWritePtr=0;

      nCount++;
	  return TRUE;
   }
      
   //m_pParent->MessageBox("FATAL ERROR, out of Send resources...");
   
   AfxMessageBox("FATAL ERROR, out of Send resources...",MB_ICONSTOP);

   return FALSE;
}

void CMessageQueue::Flush()
{
	int i;
	nCount = 0;
	nMessageReadPtr = 0;
	nMessageWritePtr = 0;

	for(i=0;i<MAX_NR_TX_MESSAGES;i++)
		szMessageQueue[i].Empty();
}

