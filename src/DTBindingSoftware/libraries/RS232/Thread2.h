#ifndef THREAD2_20080125_H
#define THREAD2_20080125_H




//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////


#include "..\RS232\Kernel_Include.h"
#include "Thread.h"



class CThread2 : public CThread
{
protected:
	HANDLE m_ThreadStop;
	DUOTECNO::KERNEL::WIN_32::CEvent m_Running;
	DUOTECNO::KERNEL::WIN_32::CEvent m_Stopped;

public:
	CThread2();
	virtual ~CThread2();

	BOOL Running();
	BOOL Stopped();

	
	void Kill(void) 
	{
		SetEvent(m_ThreadStop);
	}

protected:
	virtual DWORD Run(LPVOID arg) = 0;		// Newly Created Thread

	virtual void ThreadCtor();
	virtual void ThreadDtor();
};


#endif

#endif