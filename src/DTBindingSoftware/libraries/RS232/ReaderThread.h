// ReaderThread.h: interface for the CReaderThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READERTHREAD_H__60B6A016_60F4_4177_AC59_8BF0330857F3__INCLUDED_)
#define AFX_READERTHREAD_H__60B6A016_60F4_4177_AC59_8BF0330857F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////
#include "Thread2.h"
#include "comport_fwd.h"
//////////////////////////////////////////////////////////////////////
namespace DUOTECNO {
namespace RS232 {
//////////////////////////////////////////////////////////////////////
class CReaderThread : public CThread2
{
protected:
	// TM 20081223:
	// De max groote van een USB frame is 64 Bytes.
	// De RS232 ATMEGA modem verstuurd Byte per Byte.
	enum 
	{ 
		MAX_BUFFER_LENGTH = 64			// 128
	};
	
protected:
	CComPort& m_Parent;
	OVERLAPPED osRead;

public:
	CReaderThread(CComPort& parent);
	virtual ~CReaderThread();

protected:
	virtual DWORD Run(LPVOID arg);
	virtual void ThreadCtor();
	virtual void ThreadDtor();
};
/////////////////////////////////////////////////////////////////////
};
};
/////////////////////////////////////////////////////////////////////



#endif
#endif // !defined(AFX_READERTHREAD_H__60B6A016_60F4_4177_AC59_8BF0330857F3__INCLUDED_)
