// WriterThread.h: interface for the CWriterThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRITERTHREAD_H__D7E7F722_D9BA_483C_874D_DE404971D2F2__INCLUDED_)
#define AFX_WRITERTHREAD_H__D7E7F722_D9BA_483C_874D_DE404971D2F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////

#include "Thread2.h"
#include "comport_fwd.h"


/////////////////////////////////////////////////////////////////////

namespace DUOTECNO {
namespace RS232 {

/////////////////////////////////////////////////////////////////////

class CWriterThread : public CThread2  
{
protected:
	CComPort& m_Parent;	

	HANDLE m_WaitForDispatch;
	OVERLAPPED osWrite;

public:
	CWriterThread(CComPort& parent);
	virtual ~CWriterThread();
	BOOL MsgDispatch(void);				// Thread of Creation

protected:
	virtual DWORD Run(LPVOID arg);		// Newly Created Thread
	virtual void ThreadCtor();
	virtual void ThreadDtor();
};

/////////////////////////////////////////////////////////////////////

};
};

/////////////////////////////////////////////////////////////////////

#endif
#endif // !defined(AFX_WRITERTHREAD_H__D7E7F722_D9BA_483C_874D_DE404971D2F2__INCLUDED_)
