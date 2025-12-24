// RXMessageQueue.h: interface for the CRXMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RXMESSAGEQUEUE_H__D1E328C2_3B74_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_RXMESSAGEQUEUE_H__D1E328C2_3B74_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////
// Sedert V16.20:
// Het aantal verhoogd van 10 naar 30.
#define MAX_NR_RX_MESSAGES	30		
////////////////////////////////////////////////////////////
class CRXMessageQueue  
{
public:
	CRXMessageQueue();
	virtual ~CRXMessageQueue();

	void Flush(void);
	BOOL InsertMessage(CString szString);
	int Count();
	const char* GetMessage(void);

private:
	char *m_pCurrentMessage;
	CString m_szMessageQueue[MAX_NR_RX_MESSAGES];
	int m_nCount;
	int m_nMaxCount;
	int m_nMessageReadPtr;
	int m_nMessageWritePtr;
};

#endif // !defined(AFX_RXMESSAGEQUEUE_H__D1E328C2_3B74_11D8_B865_0050BAC412B1__INCLUDED_)
