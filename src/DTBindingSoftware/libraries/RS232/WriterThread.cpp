// WriterThread.cpp: implementation of the CWriterThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WriterThread.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////

#include "Comport.h"


//////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::RS232;
//////////////////////////////////////////////////////////////////////

DEFINE_THIS_FILE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DUOTECNO::RS232::CWriterThread::
CWriterThread(CComPort& parent) :
CThread2( ), 
m_Parent(parent)
{	
	m_WaitForDispatch = CreateEvent(NULL,FALSE,FALSE,NULL);		// AUTO 
}

DUOTECNO::RS232::CWriterThread::
~CWriterThread()
{
	CloseHandle(m_WaitForDispatch);
}

// ---------------------------------------------------------
void DUOTECNO::RS232::CWriterThread::
ThreadCtor() 
{
	TRACE("CWriterThread Constructor ...\n");

	osWrite.Offset = 0;
	osWrite.OffsetHigh = 0;
	osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	this->CThread2::ThreadCtor();
}

void DUOTECNO::RS232::CWriterThread::
ThreadDtor() 
{
	TRACE("\nWriterThread Destructor ...");
	this->CThread2::ThreadDtor();
	CloseHandle(osWrite.hEvent);
}

// ---------------------------------------------------------

BOOL DUOTECNO::RS232::CWriterThread::
MsgDispatch(void) 
{
	return (SetEvent(m_WaitForDispatch));
}

// ---------------------------------------------------------


DWORD DUOTECNO::RS232::CWriterThread::
Run(LPVOID arg)
{	
	HANDLE handles[2] = { m_WaitForDispatch, m_ThreadStop };


	HANDLE hFile = m_Parent.get_Status().get_Handle();

	TRACE("CWriterThread::Run(LPVOID arg)\n");

	osWrite.Offset = 0;
	osWrite.OffsetHigh = 0;

	while(1) 
	{		
		DWORD dwResult = ::WaitForMultipleObjects(2,handles,FALSE,INFINITE);
		switch (dwResult) 
		{
			case WAIT_OBJECT_0:							
			{
				m_Parent.get_Resources().clear_TxBuffer( );
				BYTE *pbData = m_Parent.get_Resources().get_TxBuffer();
				REQUIRE(pbData);
				
				CTXMessageQueue* const pQueue = &m_Parent.get_Resources().get_TxQueue();
				REQUIRE( pQueue);
			
			
				DWORD nBytesToWrite = pQueue->get( pbData, 512 );
				
				if (nBytesToWrite!=0) 
				{	
					BOOL  fWaitingOnWrite = FALSE;
					DWORD nBytesWritten = 0;

					BOOL fWriteStat = WriteFile(hFile,
										   pbData,
										   nBytesToWrite,
										   &nBytesWritten,
										   &osWrite);

					if (!fWriteStat) 
					{
						DWORD dwError = GetLastError();

						if (dwError == ERROR_IO_PENDING) 
						{
							fWaitingOnWrite = TRUE;
						}
						else 
						{
							m_Parent.get_Events().OnError(dwError);	
							
							ResetEvent(osWrite.hEvent);
							osWrite.Offset = 0;
							osWrite.OffsetHigh = 0;
						}
					}
					else 
					{
						fWaitingOnWrite = FALSE;
					}

					if (fWaitingOnWrite) 
					{						
						HANDLE handles2[2] = { osWrite.hEvent, m_ThreadStop };
			
						dwResult = ::WaitForMultipleObjects(2,handles2,FALSE,INFINITE);
						switch (dwResult)
						{
							case 0:
							{
								while(!(GetOverlappedResult(hFile,&osWrite,&nBytesWritten,TRUE))) 
								{
									DWORD dwError = GetLastError();								
									if (dwError == ERROR_IO_PENDING) 
									{
										continue;
									}
									else 
									{
										m_Parent.get_Events().OnError(dwError);	
										
										ResetEvent(osWrite.hEvent);
										osWrite.Offset = 0;
										osWrite.OffsetHigh = 0;

										break;
									}								
								}
								

								m_Parent.get_ThreadSync().m_DispatchDone.Set( );							

							} break;

							case 1:
							{
								TRACE("\nWriterThread Stopped");								
								return m_ThreadCtx.m_dwExitCode;

							} break;
						}										
					}					
				}					
				::Sleep( 1 );		// TM 20080207		(zie ook MODEM_USB_DELAYS)
			}break;	
			
			case 1:
			{
				// Stop !!!
				TRACE("\nWriterThread Stopped");				
				return m_ThreadCtx.m_dwExitCode;

			} break;
				
		}		
	}
}
///////////////////////////////////////////////////////////////////

#endif

