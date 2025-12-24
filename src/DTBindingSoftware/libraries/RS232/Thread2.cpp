#include "stdafx.h"
#include "Thread2.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DEFINE_THIS_FILE;
//////////////////////////////////////////////////////////////////////

CThread2::CThread2( ) :
CThread( )
{
	m_ThreadStop = CreateEvent(NULL,FALSE,FALSE,NULL);
}

CThread2::~CThread2()
{
	CloseHandle(m_ThreadStop);
}

BOOL CThread2::Running()
{
	const BOOL fResult = m_Running.Wait(1000);
	m_Running.Reset();
	return fResult;
}
BOOL CThread2::Stopped()
{
	const BOOL fResult = m_Stopped.Wait(1000);
	// ASSERT( fResult );									// <------ Deze komt af en toe voor...
	m_Stopped.Reset();
	return fResult;
}

// ---------------------------------------------------------
void CThread2::ThreadCtor() 
{
	TRACE("CWriterThread Constructor ...\n");
	m_Running.Set();
}

void CThread2::ThreadDtor() 
{
	if (m_ThreadCtx.m_hThread) 
	{
		CloseHandle(m_ThreadCtx.m_hThread);
		m_ThreadCtx.m_hThread = NULL;
	} 
	m_ThreadCtx.m_dwExitCode = 0;
	m_Stopped.Set();
}

#endif