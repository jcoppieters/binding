// Mutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mutex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////

#include "CriticalSectionObj.h"




//////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::KERNEL::WIN_32;
//////////////////////////////////////////////////////////////////////
DEFINE_THIS_FILE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DUOTECNO::KERNEL::WIN_32::CMutex::
CMutex() :
m_hMutex( (HANDLE) -1)
{
	create( 0 );
}

DUOTECNO::KERNEL::WIN_32::CMutex::
CMutex(const char* szName) :
m_hMutex( (HANDLE) -1)
{
	create( szName );		
}

DUOTECNO::KERNEL::WIN_32::CMutex::
~CMutex()
{
	if (m_hMutex != (HANDLE) -1)
		CloseHandle( m_hMutex );
}

/******************************************************************************/

void DUOTECNO::KERNEL::WIN_32::CMutex::
create(const char* szName )
{
	m_hMutex = CreateMutex( 0,
							FALSE,
							szName);

	if (m_hMutex == (HANDLE) -1)
	{
		const DWORD dwError = GetLastError();
		TRACE1("Createmutex Failed : %i",dwError);
		ALLEGE( m_hMutex != (HANDLE) -1);	
	}
}

/********************************************************************************
 *
 *
 * Return values of WaitForSingleObject
 * WAIT_OBJECT_0: Ok 
 * WAIT_TIMEOUT: Kan geen toegang krijgen binnen de opgegeven tijd.
 * WAIT_ABANDONED: Bij het afsluiten van de thread werd de mutex werd niet vrijgegeven 
 *  door de thread die hem in zijn bezit had. 
 *  Hij is nu in bezit door  de thread die de functie aanroept.
 ********************************************************************************/
 
BOOL DUOTECNO::KERNEL::WIN_32::CMutex::
Reserve(long timeout)
{
	BOOL fResult = TRUE;

	REQUIRE( m_hMutex != (HANDLE) -1 );

	const DWORD dwResult = WaitForSingleObject( m_hMutex,  timeout);   
    
	if (dwResult == WAIT_TIMEOUT)
	{
		fResult = FALSE;
	}

	return fResult;		
}

void DUOTECNO::KERNEL::WIN_32::CMutex::
Release()
{
	REQUIRE( m_hMutex != (HANDLE) -1 );

	const BOOL fResult = ReleaseMutex( m_hMutex );

	if (fResult == FALSE)
	{
		const DWORD dwError = GetLastError();
		TRACE1("Createmutex Failed : %i",dwError);
		ASSERT( 0 );
		
	}
}

#endif


