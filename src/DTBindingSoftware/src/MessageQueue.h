// MessageQueue.h: interface for the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEQUEUE_H__D1E328C1_3B74_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MESSAGEQUEUE_H__D1E328C1_3B74_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMessageQueue  
{


public:
	void Flush(void);
	BOOL InsertMessage(CString szString);
	int Count();
	CString GetMessage(void);
	CMessageQueue();
	virtual ~CMessageQueue();

};

#endif // !defined(AFX_MESSAGEQUEUE_H__D1E328C1_3B74_11D8_B865_0050BAC412B1__INCLUDED_)
