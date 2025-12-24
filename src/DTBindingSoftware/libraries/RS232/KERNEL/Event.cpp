// Event.cpp: implementation of the CEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Event.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::KERNEL::WIN_32;
/////////////////////////////////////////////////////////////////////
DEFINE_THIS_FILE;
/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DUOTECNO::KERNEL::WIN_32::CEvent::
CEvent() :
m_hEvent( 0)
{
	create( );
}

DUOTECNO::KERNEL::WIN_32::CEvent::
~CEvent()
{

}

//////////////////////////////////////////////////////////////////////

void DUOTECNO::KERNEL::WIN_32::CEvent::
create( )
{
	REQUIRE( m_hEvent == 0);

	m_hEvent = CreateEvent( 0, TRUE, FALSE, 0);

	if (m_hEvent == 0)
	{
		DWORD dwResult = GetLastError( );
		TRACE1("CreateEvent failed: %i", dwResult);
		ALLEGE( m_hEvent == 0);
	}
}


void DUOTECNO::KERNEL::WIN_32::CEvent::
Set()
{
	REQUIRE( m_hEvent != 0);

	DWORD dwResult = SetEvent( m_hEvent );

	if (dwResult == 0)
	{
		DWORD dwResult = GetLastError( );
		TRACE1("SetEvent failed: %i", dwResult);
		ALLEGE( m_hEvent == 0);
	}
}

void DUOTECNO::KERNEL::WIN_32::CEvent::
Reset()
{
	REQUIRE( m_hEvent != 0);

	DWORD dwResult = ResetEvent( m_hEvent );

	if (dwResult == 0)
	{
		DWORD dwResult = GetLastError( );
		TRACE1("SetEvent failed: %i", dwResult);
		ALLEGE( m_hEvent == 0);
	}
}

BOOL DUOTECNO::KERNEL::WIN_32::CEvent::
Wait(long dwTimeout)
{
	BOOL fResult = TRUE;

	REQUIRE( m_hEvent != 0);

	DWORD dwResult = ::WaitForSingleObject( m_hEvent, dwTimeout);

	// TM 20081222 WAIT_ABANDONED
	if (dwResult == WAIT_TIMEOUT || dwResult == WAIT_ABANDONED)		
	{
		fResult = FALSE;
	}

	return fResult;								   
}



#endif
