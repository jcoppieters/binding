// Mutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CriticalSectionObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::KERNEL::WIN_32;
//////////////////////////////////////////////////////////////////////
DEFINE_THIS_FILE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DUOTECNO::KERNEL::WIN_32::CCriticalSectionObj::
CCriticalSectionObj() //:
//m_hCriticalSection( 0 )
{
	InitializeCriticalSection( &m_hCriticalSection );
}

DUOTECNO::KERNEL::WIN_32::CCriticalSectionObj::
~CCriticalSectionObj()
{
	DeleteCriticalSection( &m_hCriticalSection );
}

void DUOTECNO::KERNEL::WIN_32::CCriticalSectionObj::
Enter( )
{
	EnterCriticalSection( &m_hCriticalSection ); 	
}

void DUOTECNO::KERNEL::WIN_32::CCriticalSectionObj::
Leave( )
{
	LeaveCriticalSection( &m_hCriticalSection ); 
}

/*
BOOL DUOTECNO::KERNEL::WIN_32::CCriticalSectionObj::
TryEnter( )
{
	const BOOL fResult = TryEnterCriticalSection( &m_hCriticalSection );

	return (fResult); 	
}
*/



#endif




