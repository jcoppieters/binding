// ComPort.cpp: implementation of the CComPort class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ComPort.h"

			

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)	
//////////////////////////////////////////////////////////////////////
#define MODEM_USB_DELAYS	::Sleep(1);		// Sedert V16.20
//////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4355)
//////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::RS232;
//////////////////////////////////////////////////////////////////////
DEFINE_THIS_FILE;
//////////////////////////////////////////////////////////////////////
DUOTECNO::RS232::CComPortThreads::
CComPortThreads(CComPort& parent) : 
m_Parent(parent),
m_Reader(parent),
m_Writer(parent)
{
};
DUOTECNO::RS232::CComPortThreads::
~CComPortThreads( )
{
}

void DUOTECNO::RS232::CComPortThreads::
Start( )
{
	m_Reader.Start( this );
	m_Writer.Start( this );

	m_Reader.Running();
	m_Writer.Running();
}


void DUOTECNO::RS232::CComPortThreads::
Stop( )
{
	m_Reader.Kill();
	m_Writer.Kill();
	m_Reader.Stopped();
	m_Writer.Stopped();
}

void DUOTECNO::RS232::CComPortThreads::
Sleep( )
{
	// Todo ...
}


//////////////////////////////////////////////////////////////////////

void DUOTECNO::RS232::CComportSettings::
set_Comport(int port) 
{
	ASSERT(port < MAX_PORTS);
	m_Port = port;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DUOTECNO::RS232::CComPortActions::
CComPortActions(CComPort& parent) :
m_Parent( parent )
{
}
DUOTECNO::RS232::CComPortActions::
~CComPortActions( )
{
}

void DUOTECNO::RS232::CComPortActions::
ClearErrors( )
{
	const HANDLE hFile = m_Parent.get_Status().get_Handle();
	if (hFile != (HANDLE) -1) {
		DWORD dwErrorFlags;
		COMSTAT ComStat;
		const BOOL result = ClearCommError(hFile,&dwErrorFlags,&ComStat);	
		if (FALSE == result) {
			const DWORD dwError = GetLastError( );
			TRACE("ClearCommError failed: GetLastError = %i\n",dwError);
		}
	}
}

// TM 20081222: Purgecomm uitgebreid om enkel de Rx Hardware Queue te legen.
void DUOTECNO::RS232::CComPortActions::
PurgeComm(PURGE_ACTIONS flags)
{
	const HANDLE hfile = m_Parent.get_Status().get_Handle();
	DWORD dwFlags = 0;
	switch (flags)
	{
		case PURGE_RX:
		{
			dwFlags = PURGE_RXABORT | PURGE_RXCLEAR;
		} break;
		case PURGE_TX:
		{
			dwFlags = PURGE_TXABORT | PURGE_TXCLEAR;
		} break;
		default:
		{
			dwFlags = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR;
		} break;

	}

	MODEM_USB_DELAYS;
	const BOOL result = ::PurgeComm( hfile , dwFlags);
	MODEM_USB_DELAYS;
	if (FALSE == result) {
		const DWORD dwError = GetLastError( );
		TRACE("PurgeComm failed: GetLastError = %i\n",dwError);
	}	
}

void DUOTECNO::RS232::CComPortActions::
HandleErrors( )
{
	const HANDLE hFile = m_Parent.get_Status().get_Handle();
	if (hFile != (HANDLE) -1) {
		DWORD dwErrorFlags;
		COMSTAT ComStat;
		BOOL result = ClearCommError(hFile,&dwErrorFlags,&ComStat);	
		if (FALSE == result) {
			const DWORD dwError = GetLastError( );
			TRACE("ClearCommError failed: GetLastError = %i\n",dwError);
		}
		if (result) {
			if (ComStat.cbInQue != 0 || ComStat.cbOutQue != 0) {
				MODEM_USB_DELAYS;
				result = ::PurgeComm(hFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);		
				MODEM_USB_DELAYS;
				if (FALSE == result) {
					const DWORD dwError = GetLastError( );
					TRACE("PurgeComm failed: GetLastError = %i\n",dwError);
				}
			}	
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DUOTECNO::RS232::CComPort::
CComPort() :
m_Threads(*this),
m_Events(*this) { }

DUOTECNO::RS232::CComPort::
~CComPort() 
{
	m_Resources.Reset( );
}

///////////////////////////////////////////////////////////////////////
BOOL DUOTECNO::RS232::CComPort::
SetupConnection()
{
	BOOL fRetVal = FALSE;

	if (m_Status.valid_Handle())
	{
		DCB dcb;		
		dcb.DCBlength = sizeof( DCB );

		MODEM_USB_DELAYS;
		fRetVal = GetCommState (m_Status.get_Handle(), &dcb);
		MODEM_USB_DELAYS;
		if (!fRetVal) {
			const DWORD dwError = GetLastError( );
			TRACE("GetCommState failed: GetLastError = %i\n",dwError);
		}

		dcb.BaudRate = m_Settings.get_Baudrate();
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.fAbortOnError = FALSE;	//TRUE;		// Must clear commError !!!
		dcb.fNull = FALSE;	// Don't discard null char's !!!

	  
		dcb.fOutxCtsFlow  = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDtrControl = FALSE;
		dcb.fDsrSensitivity = FALSE;
		dcb.fTXContinueOnXoff = FALSE;
		dcb.fOutX = FALSE; 
		dcb.fInX = FALSE;    
		dcb.fErrorChar = FALSE;	
		dcb.fRtsControl = FALSE;

		MODEM_USB_DELAYS;
		fRetVal = SetCommState( m_Status.get_Handle() , &dcb);
		MODEM_USB_DELAYS;
		if (!fRetVal) {
			const DWORD dwError = GetLastError( );								
			TRACE("SetCommState failed: GetLastError = %i\n",dwError);
		}
	}
	return fRetVal;
}

BOOL DUOTECNO::RS232::CComPort::
CloseConnection()
{
	if (m_Status.valid_Handle())
	{
		const HANDLE hfile = m_Status.get_Handle();
		BOOL fResult = FALSE;
		
		fResult = SetCommMask(hfile , 0);	
		if (!fResult) {
			const DWORD dwError = GetLastError( );
			TRACE("SetCommMask failed: GetLastError = %i\n",dwError);
		}
		
		// Krijgen hier een foutcode dat we een verkeerde parameter meegeven.
		// 0 is geen waarde in de API.
		// Vroeger nooit geen problemen gehad dus gewoon de code laten staan.
		// De foutcode (TRACE) in commentaar zetten.
		MODEM_USB_DELAYS;
		fResult = EscapeCommFunction (hfile , 0);
		MODEM_USB_DELAYS;
		if (!fResult) {
			// const DWORD dwError = GetLastError( );
			// TRACE("EscapeCommFunction failed: GetLastError = %i\n",dwError);
		}
	
		CComPortActions actions(*this);
		actions.ClearErrors( );		
		actions.PurgeComm( );

		CloseHandle(hfile);
		m_Status.set_Handle((HANDLE) -1);
		m_Status.set_Connected(FALSE);
		m_Threads.Stop( );
	}
	else {
		ASSERT(0);	
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////


BOOL DUOTECNO::RS232::CComPort::
Connected() const
{
	return m_Status.get_Connected();
}

BOOL DUOTECNO::RS232::CComPort::
Open( )
{
	BOOL fRetVal;
	COMMTIMEOUTS CommTimeouts;

	if (m_Status.get_Connected() == TRUE) 
		return FALSE;

	ASSERT( !m_Status.valid_Handle());
	ASSERT( m_Settings.get_Comport() != -1);
	ASSERT( m_Settings.get_Comport() < CComportSettings::MAX_PORTS);

	HANDLE file = (HANDLE) -1;
	
	/**
	 * TM 20080917: Q115831
	 * als er com poorten gebruikt worden hoger dan 9 moet deze nota gebruikt worden:	   
	 */
	char szComport[20];			
	sprintf(&szComport[0],"\\\\.\\COM%i",m_Settings.get_Comport()+1);

	file = CreateFile(szComport,
					  GENERIC_READ | GENERIC_WRITE,
					  0,
					  NULL,
					  OPEN_EXISTING,
					  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
					  NULL);

	if (file == (HANDLE)-1)
	{	
		const DWORD dwError = GetLastError( );
		TRACE("Error opening comport: GetLastError = %i\n",dwError);
		// ASSERT( 0 );
	}
	else	
	{				
		m_Status.set_Handle( file );

		SetupComm( m_Status.get_Handle() , 4096 , 4096 );

		CComPortActions actions(*this);
		actions.PurgeComm( );

		GetCommTimeouts(m_Status.get_Handle(),&CommTimeouts);
		CommTimeouts.ReadIntervalTimeout = 1;
		CommTimeouts.ReadTotalTimeoutConstant = 0;
		CommTimeouts.ReadTotalTimeoutMultiplier = 0;
		CommTimeouts.WriteTotalTimeoutConstant = 0;
		CommTimeouts.WriteTotalTimeoutMultiplier =0;

		SetCommTimeouts(m_Status.get_Handle(),&CommTimeouts);
			
		SetCommMask( m_Status.get_Handle(), EV_RXCHAR);		
		fRetVal = SetupConnection();
		if (fRetVal) {		
			m_Status.set_Connected( TRUE );			
			m_Threads.Start( );			
			return TRUE;
		}
		else {			
			m_Status.set_Connected( FALSE );
		}
	}
	return fRetVal;
}

BOOL DUOTECNO::RS232::CComPort::
Close( )
{
	CloseConnection();
	return (m_Status.get_Connected());
}

/**
 * TM 20080207:
 * Problems with USB Hardware --> TX & RX may not exceeed more than 64 Bytes ...
 */
BOOL DUOTECNO::RS232::CComPort::
Send(BYTE* pbBuffer,int length)
{	
	const int TX_BUFFER_SIZE = 63; 
	REQUIRE(pbBuffer);
	CTXMessageQueue* const pQueue = &get_Resources().get_TxQueue();
	INVARIANT( pQueue );
	for(int offset = 0; offset < length; offset += TX_BUFFER_SIZE ) {	
		const int size = min( length - offset, TX_BUFFER_SIZE );
		pQueue->Add( &pbBuffer[offset], size );
		m_ThreadSync.m_DispatchDone.Reset( );
		m_Threads.get_Writer().MsgDispatch();
		m_ThreadSync.m_DispatchDone.Wait( 1000 );
		::Sleep( 1 );		// TM 20080207 (zie ook MODEM_USB_DELAYS)
	}
	return TRUE;
}


int DUOTECNO::RS232::CComPort::
Get(BYTE** pbBuffer)
{
	int count = 0;

	// Wachten tot er data in de Queue zit.
	get_ThreadSync().m_DataReady.Wait( 1 );

	CRXMessageQueue* const pQueue = &get_Resources().get_RxQueue();
	if (pQueue->count() > 0)
	{
		REQUIRE( pQueue );
		*pbBuffer = m_Resources.get_RxBuffer();
		REQUIRE(*pbBuffer);	
		count = pQueue->get(*pbBuffer, 512 );
	}
	// Data uit de Queue gehaald, reader thread kan opnieuw verder.
	get_ThreadSync().m_ProccesingDone.Set();		
	return count;
}
//////////////////////////////////////////////////////////////////////
DUOTECNO::RS232::CComportEvents::
CComportEvents(CComPort& parent) :
m_Parent(parent),
m_hWndEventSink( 0 ),
m_MessageID( 0 )
{
}
DUOTECNO::RS232::CComportEvents::
~CComportEvents()
{
}
void DUOTECNO::RS232::CComportEvents::
subscribe(HWND hwnd, UINT Id)
{	
	m_hWndEventSink = hwnd;
	m_MessageID = Id;
}

// Deze functie wordt aangeroepen vanuit de RS232 RX-thread.
void DUOTECNO::RS232::CComportEvents::
OnMessageReceived(const BYTE* pbData, int length)
{	
	// TM 20081222: Wait timeout was 1000, dit veranderd naar 10000 sec.
	// door een software bug werkte dit vroeger niet: was vergeten het
	// processing done event te resetten.
	const DWORD PROCESSING_WAIT_TIMEOUT = 10000;

	if (m_hWndEventSink != 0)
	{	
		m_Parent.get_ThreadSync().m_DataReady.Set( );

		// TM 20081222: Moeten eerst het event Resetten !!!!!!!!
		m_Parent.get_ThreadSync().m_ProccesingDone.Reset();	

		CRXMessageQueue* const pQueue = &m_Parent.get_Resources().get_RxQueue();

		INVARIANT( pQueue );

		pQueue->Add(const_cast<BYTE*>(pbData), length );

		PostMessage( m_hWndEventSink, m_MessageID, 0, 0 );
	
		const BOOL fResult = 
			m_Parent.get_ThreadSync().m_ProccesingDone.Wait( PROCESSING_WAIT_TIMEOUT );

		if (fResult == FALSE)
		{
			// TM 20081222:
			// Hier de Rx Hardware Queue wissen. 
			// Enkel het ontvangen bericht dat zich nu in de software Queue bevindt zal 
			// verwerkt worden.
			// Alle aanwezige data in de HW-Queue moet verwijderd worden.
			CComPortActions actions(m_Parent);
			actions.PurgeComm(CComPortActions::PURGE_RX);			
		}
		//ASSERT(fResult);

		m_Parent.get_ThreadSync().m_DataReady.Reset( );		// TM 20081222
	}
}

void DUOTECNO::RS232::
CComportEvents::OnError(DWORD dwError)
{
	TRACE1("Error occured : %i\n",dwError);

	switch (dwError)
	{
		case 6:	/* Invalid Handle */
		{
			TRACE("Invalid Handle ...\n");
		} break;
	}

	CComPortActions actions(m_Parent);
	actions.HandleErrors( );
}
////////////////////////////////////////////////////////////////////////////////
DUOTECNO::RS232::CThreadSync::CThreadSync( ) { }
DUOTECNO::RS232::CThreadSync::~CThreadSync( ) { }
////////////////////////////////////////////////////////////////////////////////
#endif

