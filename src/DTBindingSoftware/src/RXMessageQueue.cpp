// RXMessageQueue.cpp: implementation of the CRXMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "RXMessageQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define DEBUG_SERIAL_RXMESSAGEBUFFERS

#ifdef DEBUG_SERIAL_RXMESSAGEBUFFERS
	#define SERIAL_TRACE(x)			TRACE(x)
	#define SERIAL_TRACE1(x,y)		TRACE1(x,y)
	#define SERIAL_TRACE2(x,y,z)	TRACE2(x,y,z)
#else
	#define SERIAL_TRACE(x)			
	#define SERIAL_TRACE1(x,y)		
	#define SERIAL_TRACE2(x,y,z)		
#endif

CRXMessageQueue::CRXMessageQueue()
{
	Flush();
}

CRXMessageQueue::~CRXMessageQueue()
{

}

const char* CRXMessageQueue::GetMessage(void)
{
	if (m_nCount == 0) return NULL;

	m_pCurrentMessage = m_szMessageQueue[m_nMessageReadPtr].GetBuffer(0);
	
	if (m_nMessageReadPtr++>=(MAX_NR_RX_MESSAGES-1))
		m_nMessageReadPtr=0;
	
	m_nCount--;	
	return(m_pCurrentMessage);
}

int CRXMessageQueue::Count()
{
	return m_nCount;
}

BOOL CRXMessageQueue::InsertMessage(CString szString)
{
	if (m_nCount < MAX_NR_RX_MESSAGES)
	{    
		m_szMessageQueue[m_nMessageWritePtr] = szString;

		if (m_nMessageWritePtr++>=(MAX_NR_RX_MESSAGES-1))
			m_nMessageWritePtr=0;

		m_nCount++;
		if (m_nCount > m_nMaxCount) {
			m_nMaxCount = m_nCount;
		}
		SERIAL_TRACE2("\nRX-BUFFERS \t\tInsertString: \t%s , \t\tNrOfMessages :%d",szString,m_nCount);	
		return TRUE;
	}      
	ASSERT( 0 );		// Dit mag nooit voorkomen !!
	SERIAL_TRACE("\n---- NR OF RX-MESSAGE BUFFERS FULL ----\n");   
	return FALSE;
}

void CRXMessageQueue::Flush()
{
	int i;
	m_nCount = 0;
	m_nMaxCount = 0;
	m_nMessageReadPtr = 0;
	m_nMessageWritePtr = 0;
	for(i=0;i<MAX_NR_RX_MESSAGES;i++) {
		m_szMessageQueue[i].Empty();	
	}
}

