#include "stdafx.h"
#include "dtthreadwin32.h"
/**************************************************************/
#define ASSERT			assert			// <assert.h>
#define ALLEGE			ASSERT
#define REQUIRE			ASSERT
#define	ENSURE			ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
CThreadEventWin::CThreadEventWin(BOOL manualReset, BOOL initialState) :
	m_hEvent(0) 
{
	create(manualReset,initialState);
}

CThreadEventWin::~CThreadEventWin() { }

void CThreadEventWin::create(BOOL manualReset, BOOL initialState) {
	REQUIRE( m_hEvent == 0);
	m_hEvent = CreateEvent( 0, manualReset, initialState, 0);
	if (m_hEvent == 0) {
		const DWORD dwResult = GetLastError( );	
		// TRACE1("SetEvent failed: %i", dwResult);
		ALLEGE( m_hEvent == 0);
	}
}

void CThreadEventWin::Set() {
	REQUIRE( m_hEvent != 0);
	DWORD dwResult = SetEvent( m_hEvent );
	if (dwResult == 0) {
		const DWORD dwResult = GetLastError( );
		// TRACE1("SetEvent failed: %i", dwResult);
		ALLEGE( m_hEvent == 0);
	}
}

void CThreadEventWin::Reset() {
	REQUIRE( m_hEvent != 0);
	const DWORD dwResult = ResetEvent( m_hEvent );
	if (dwResult == 0) {
		const DWORD dwResult = GetLastError( );
		// TRACE1("SetEvent failed: %i", dwResult);
		ALLEGE( m_hEvent == 0);
	}
}

BOOL CThreadEventWin::Wait(long dwTimeout) {
	BOOL fResult = TRUE;
	REQUIRE( m_hEvent != 0);
	const DWORD dwResult = ::WaitForSingleObject( m_hEvent, dwTimeout);
	if (dwResult == WAIT_TIMEOUT) {
		return FALSE;
	}
	return TRUE;							   
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
CThreadMutexWin::CThreadMutexWin() :
	m_hMutex((HANDLE) -1) {
	create( 0 );
}

CThreadMutexWin::CThreadMutexWin(const char* szName) :
	m_hMutex((HANDLE) -1) {
	create( szName );		
}

CThreadMutexWin::~CThreadMutexWin() {
	if (m_hMutex != (HANDLE)-1)
		CloseHandle( m_hMutex );
}



void CThreadMutexWin::create(const char* szName ) {
	m_hMutex = CreateMutex( 0, FALSE, szName);
	if (m_hMutex == (HANDLE) -1) {
		const DWORD dwError = GetLastError();
		// TRACE1("Createmutex Failed : %i",dwError);
		ALLEGE( m_hMutex != (HANDLE) -1);	
	}
}

/********************************************************************************
 * Return values of WaitForSingleObject
 * WAIT_OBJECT_0: Ok 
 * WAIT_TIMEOUT: Kan geen toegang krijgen binnen de opgegeven tijd.
 * WAIT_ABANDONED: Bij het afsluiten van de thread werd de mutex werd niet vrijgegeven 
 *  door de thread die hem in zijn bezit had. 
 *  Hij is nu in bezit door  de thread die de functie aanroept.
 ********************************************************************************/
BOOL CThreadMutexWin::Reserve(long timeout) {
	BOOL fResult = TRUE;
	REQUIRE( m_hMutex != (HANDLE) -1 );
	const DWORD dwResult = WaitForSingleObject( m_hMutex,  timeout);       
	if (dwResult == WAIT_TIMEOUT) {
		fResult = FALSE;
	}
	return fResult;		
}

void CThreadMutexWin::Release() {
	REQUIRE( m_hMutex != (HANDLE) -1 );
	const BOOL fResult = ReleaseMutex( m_hMutex );
	if (fResult == FALSE) {
		const DWORD dwError = GetLastError();
		// TRACE1("Createmutex Failed : %i",dwError);
		ASSERT( 0 );		
	}
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
CThreadCSWin::CThreadCSWin() /* : m_hCriticalSection( 0 ) */
{
	InitializeCriticalSection( &m_hCriticalSection );
}

CThreadCSWin::~CThreadCSWin()
{
	DeleteCriticalSection( &m_hCriticalSection );
}

void CThreadCSWin::Enter( )
{
	EnterCriticalSection( &m_hCriticalSection ); 	
}

void CThreadCSWin::Leave( )
{
	LeaveCriticalSection( &m_hCriticalSection ); 
}

#if(0)
BOOL CThreadCSWin::TryEnter( )
{
	const BOOL fResult = TryEnterCriticalSection( &m_hCriticalSection );
	return (fResult); 	
}
#endif 
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
CThreadWin::CThreadWin( ) : CThreadBase( ), m_ThreadStop(0) , m_threadRunning() {
	m_ThreadStop = CreateEvent(NULL,FALSE,FALSE,NULL);
}

CThreadWin::~CThreadWin() {
	CloseHandle(m_ThreadStop);
}

DWORD CThreadWin::Start(void* arg) {	
	const DWORD result = this->CThreadBase::Start(arg);
	m_threadRunning.Wait(-1);	// Wachten tot de thread gestart is.
	return result;
}

void CThreadWin::Kill(void) {
	SetEvent(m_ThreadStop);
}

BOOL CThreadWin::requestThreadStop( ) {
	return (0 == ::WaitForSingleObject(m_ThreadStop,0) ? TRUE : FALSE);
}

void CThreadWin::ThreadCtor()  {
	m_threadRunning.Set();
}

void CThreadWin::ThreadDtor()  {
	if (m_ThreadCtx.m_hThread) {
		CloseHandle(m_ThreadCtx.m_hThread);
		m_ThreadCtx.m_hThread = NULL;
	} 
	m_ThreadCtx.m_dwExitCode = 0;
	m_threadRunning.Reset();
}

BOOL CThreadWin::Running( ) {
	return m_threadRunning.Wait(1);	// ;0);
}

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/