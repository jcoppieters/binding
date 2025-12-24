// ReaderThread.cpp: implementation of the CReaderThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReaderThread.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////

#include "comport.h"

//////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::RS232;
//////////////////////////////////////////////////////////////////////

DEFINE_THIS_FILE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DUOTECNO::RS232::CReaderThread::
CReaderThread(CComPort& parent) :
CThread2( ), 
m_Parent(parent)
{
}

DUOTECNO::RS232::CReaderThread::
~CReaderThread()
{
}

// ---------------------------------------------------------
void DUOTECNO::RS232::CReaderThread::
ThreadCtor() 
{
	TRACE("ReaderThread Constructor ...\n");

	osRead.Offset = 0;
	osRead.OffsetHigh = 0;
	osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	this->CThread2::ThreadCtor();
}

void DUOTECNO::RS232::CReaderThread::
ThreadDtor() 
{
	// BUGJE : THREADDTOR IS NEVER INVOKED ...

	TRACE("\nReaderThread Destructor ...");

	CloseHandle(osRead.hEvent);

	this->CThread2::ThreadDtor();
}

// ---------------------------------------------------------

DWORD DUOTECNO::RS232::CReaderThread::
Run(LPVOID arg)
{
	BYTE bData[MAX_BUFFER_LENGTH];

	DWORD nBytesRead;
	
	HANDLE hFile = m_Parent.get_Status().get_Handle();

	HANDLE handles[2] = {osRead.hEvent,
						 m_ThreadStop };

	//BOOL fWaitingOnRead = FALSE;		

	TRACE("CReaderThread::Run\n");

	CComPortActions actions(m_Parent);
	actions.ClearErrors( );

	osRead.Offset = 0;
	osRead.OffsetHigh = 0;

	while(1) 
	{			
		DWORD dwRes = ::WaitForSingleObject(handles[1],0);
		if (dwRes == 0) 
		{	
			TRACE("\nReaderThread Stopped...");		
			return m_ThreadCtx.m_dwExitCode;
		}		

		BOOL fWaitingOnRead = FALSE;
		int nBytesToRead = m_Parent.get_Resources().get_RxQueue().spaceFree();

		if (nBytesToRead > MAX_BUFFER_LENGTH)
			nBytesToRead = MAX_BUFFER_LENGTH;

	//	INVARIANT( nBytesToRead != 0);

		if (nBytesToRead > 0)
		{
			const BOOL fReadStat = ReadFile(hFile,&bData[0],nBytesToRead,&nBytesRead, &osRead);			
			if (!fReadStat) 
			{
				DWORD dwError = GetLastError();
				if (dwError == ERROR_IO_PENDING) 
				{
					fWaitingOnRead = TRUE;
				}
				else 
				{
					m_Parent.get_Events().OnError(dwError);						
					osRead.Offset = 0;
					osRead.OffsetHigh = 0;
					ResetEvent(osRead.hEvent );
				}
			}
			else 
			{
				fWaitingOnRead = FALSE;		
				dwRes=::WaitForSingleObject(handles[1],0);				
				if (dwRes == WAIT_OBJECT_0) 
				{
					TRACE("\nReaderThread Stopped...");				
					return m_ThreadCtx.m_dwExitCode;
				}
			}		
		}
		else
		{
			TRACE("No More Space in Rx Qeueu\n");
			m_Parent.get_Resources().get_RxQueue().spaceFree();
		}

		if (fWaitingOnRead) 
		{			
			dwRes = WaitForMultipleObjects(2,handles,FALSE,INFINITE);
			switch (dwRes)
			{
				case WAIT_OBJECT_0 :
				{				
					while (!(GetOverlappedResult(hFile,&osRead,&nBytesRead,TRUE))) 
					{				
						DWORD dwError = GetLastError();
						if (dwError != ERROR_IO_PENDING)
						{						
							m_Parent.get_Events().OnError(dwError);
							
							osRead.Offset = 0;
							osRead.OffsetHigh = 0;
							ResetEvent(osRead.hEvent );

							break;
						}
					}				
					fWaitingOnRead = FALSE;
				} break;

				case 1:
				{
					TRACE("\nReaderThread Stopped...");					
					return m_ThreadCtx.m_dwExitCode;

				} break;
			}
		}

		if (nBytesRead > 0)
		{		
			m_Parent.get_Events().OnMessageReceived(&bData[0],nBytesRead);	
			nBytesRead = 0;
		}

		//ClearCommError(hFile,&dwErrorFlags,&ComStat);	// ???????
	}
}


#endif