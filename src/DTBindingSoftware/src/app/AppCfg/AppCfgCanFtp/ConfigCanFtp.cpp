#include "stdafx.h"
#include "ConfigCANFTP.h"
#include "ConfigCanFtp_Info.h"
///////////////////////////////////////////////////////////////////////
#include "Nodemess.h"
#include "IComApplication.h"			// temp

#include "utils/app/DisplayDevice.h"
///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#pragma warning( disable : 4355 )

///////////////////////////////////////////////////////////////////////
////////////////////////// CConfigCanFTP //////////////////////////////
///////////////////////////////////////////////////////////////////////
CConfigCanFTP::CConfigCanFTP(IComApplication& parent) :
m_pCommApplication( &parent ),
m_fsmEventListener( *this ),
m_eventNotifier( &parent ),
m_fsm(m_fsmEventListener),
m_pIterator( 0 )
{ 
	ASSERT( m_pCommApplication );
}
CConfigCanFTP::~CConfigCanFTP() 
{ 
	destroyIterator( );
}
///////////////////////// protected functies //////////////////////////
void CConfigCanFTP::destroyIterator( )
{
	if (m_pIterator)
	{
		delete m_pIterator;
		m_pIterator = 0;
	}
}

bool CConfigCanFTP::createIterator(CConfigCanFTP_Info& info)
{
	destroyIterator( );
	m_pIterator = new CConfigCanFTP_FileEntryIterator(info.createIterator( ));
	return (m_pIterator == 0 ? false : true);
}

bool CConfigCanFTP::startWriteOpen(CConfigCanFTP_FileEntry& entry)
{
	bool retVal = false;

	const BYTE bNodeAddress = entry.getNodeAddress( );
	if (bNodeAddress != 0xFF)
	{
		const char* szFileName = entry.getLocalFileName( );
		if (szFileName)
		{
			// Bestand openen in read mode.
			if (m_fileOpener.openFileRead(szFileName))
			{		
				// De fsm starten als open file write.
				const char* szDestFileName = entry.getRemoteFileName( );		
				if (m_fsm.openFileWrite(bNodeAddress,
										szDestFileName,
										entry.getInfo()))
				{			
					retVal = true;
				}
			}
		}
	}		
	return retVal;
}

bool CConfigCanFTP::startReadOpen(CConfigCanFTP_FileEntry& entry)
{
	bool retVal = false;

	const BYTE bNodeAddress = entry.getNodeAddress( );
	if (bNodeAddress != 0xFF)
	{
		const char* szFileName = entry.getLocalFileName( );
		if (szFileName)
		{
			/** @todo	Moeten hier een tempbestand gebruiken ??? */

			if (m_fileOpener.openFileWrite(szFileName))
			{	
				// Sedert versie 0x0B01.
				// Indien het bestand niet aanwezig is dan geen foutmelding geven.
				const char* szRemoteFileName = entry.getRemoteFileName( );	
				BOOL flagFileNotFoundSuppressError=FALSE;

				if(entry.getCtxFlags()&CConfigCanFTP_FileEntry::CTXFLAG_READ_FILENOTFOUND_SUPPRESS_ERROR)
				{
					flagFileNotFoundSuppressError = TRUE;
				}
				
				if (m_fsm.openFileRead(bNodeAddress,
									   szRemoteFileName,
									   entry.getInfo(),
									   flagFileNotFoundSuppressError))
				{				
					retVal = true;
				}
			}
		}
	}		
	return retVal;
}

bool CConfigCanFTP::startOperation(CConfigCanFTP_FileEntry& entry)
{
	typedef bool (CConfigCanFTP::*PF_ACTION)(CConfigCanFTP_FileEntry&);

	bool result = false;
	PF_ACTION pfAction = 0;

	if (entry.getAction( ) == 
		CConfigCanFTP_FileEntry::ACTION_FTP_WRITE)
	{
		pfAction = &CConfigCanFTP::startWriteOpen;
	}
	else if (entry.getAction( ) == 
		CConfigCanFTP_FileEntry::ACTION_FTP_READ)
	{
		pfAction = &CConfigCanFTP::startReadOpen;
	}

	// Callback functie activeren.
	if ((this->*pfAction)(entry))
	{
		CConfigCanFTP_FileEntry::PF_CALLBACK pf = entry.getCallbackStarted( );
		if (pf)
		{
			// Callback functie started aanroepen.
			pf(entry);
		}		
		result = true;
	}
	return result;
}

BYTE CConfigCanFTP::getCurrentNodeAddress( )
{
	BYTE bNodeAddress = 0xFF;

	if (m_pIterator != 0)
	{
		CConfigCanFTP_FileEntry* const pEntry =
			m_pIterator->CurrentItem( );

		if (pEntry)
		{
			bNodeAddress = pEntry->getNodeAddress();	
		}
	}
	return bNodeAddress;
}

void CConfigCanFTP::displayFileInfo( )
{
	const char* szFileInfo = 0;
	BYTE bNodeAddress = 0xFF;
	long procent = -1;
	CString szAction;

	if (m_pIterator)
	{
		CConfigCanFTP_FileEntry* const pEntry =
			m_pIterator->CurrentItem( );

		if (pEntry)
		{
			szFileInfo = pEntry->getInfo();
			bNodeAddress = pEntry->getNodeAddress( );

			int length = 0;
			if (pEntry->getAction( ) ==
				CConfigCanFTP_FileEntry::ACTION_FTP_WRITE)				
			{					
				// Lengte is de totale lengte van het bestand.
				length = m_fileOpener.getLength( );	
				szAction.Format("Send: ");
			}
			else
			{
				// lengte is de lengte ontvagen bij het openen
				// van het remote bestand in read mode.
				length = m_fsm.getFileSize( );	
				szAction.Format("Receive: ");
			}

			if (length > 0)
			{
				int pos = m_fileOpener.getPosition( );
				procent = ((long) (pos*100))/length;						  		
			}	
		}
	}

	if (procent != -1 && 
		szFileInfo != 0 &&
		bNodeAddress != 0xFF)
	{
		CString szMessage;
		szMessage.Format("%s NodeAddress=0x%02x:'%s' %d%%",
						 szAction,
						 bNodeAddress,
						 szFileInfo,
						 procent);

		CDisplayDevice::DisplayInfoMsg(szMessage);						 
	}
}

////////////////////////// publieke functies //////////////////////////
BOOL CConfigCanFTP::Start(CConfigCanFTP_Info& info)
{
	BOOL retVal = FALSE;
		
	if (createIterator(info))
	{
		CConfigCanFTP_FileEntry* const pEntry = 
			m_pIterator->CurrentItem( );
		
		if (pEntry)
		{
			if (startOperation(*pEntry))
			{		
				displayFileInfo( );
				retVal = TRUE;
			}
		}
	}

	if (!retVal)
	{
		m_eventNotifier.signalTaskError( );
	}
	return retVal;
}
BOOL CConfigCanFTP::Cancel( )
{
	BOOL retVal = FALSE;
	
	m_fileOpener.close( );

	retVal = m_fsm.abortFile(getCurrentNodeAddress());
	if (retVal)
	{
		m_eventNotifier.signalTaskCancelled();
	}
	return retVal;	
}

void CConfigCanFTP::signalReadOpen(
	BYTE bNodeAddress,
	unsigned long fileSize)
{
	if (getCurrentNodeAddress() == bNodeAddress)
	{		
		m_fsm.signalFileReadOpened(bNodeAddress, fileSize);	
	}
}

void CConfigCanFTP::signalDataReceived(
	BYTE bNodeAddress,
	const BYTE* pbData,
	int numBytes)
{
	if (getCurrentNodeAddress() == bNodeAddress)
	{		
		m_fileOpener.addData(pbData, numBytes);

		m_fsm.signalDataReceived(bNodeAddress,
								 pbData,
								 numBytes);

		displayFileInfo( );
	}
}

void CConfigCanFTP::signalInfoDialogACK(BYTE bNodeAddress)
{
	if (getCurrentNodeAddress() == bNodeAddress) {			
		m_fsm.signalInfoDialogACK(bNodeAddress);
	}
}

void CConfigCanFTP::signalWriteOpen(BYTE bNodeAddress,unsigned long fileSize)
{
	ASSERT( getCurrentNodeAddress() == bNodeAddress );

	BOOL result = FALSE;
	if (getCurrentNodeAddress() == bNodeAddress) {	
		m_fsm.signalFileWriteOpened(bNodeAddress,fileSize);
		if (m_fileOpener.isDone( )) {
			m_fileOpener.close( );
			result = m_fsm.closeFile(bNodeAddress);
			if (result == FALSE) {		
				m_eventNotifier.signalTaskError( );	
			}
		}
		else
		{			
			enum { SEGMENTED_BLOCK_LENGTH = 110 };
			enum { MAX_BUFFER_LENGTH = SEGMENTED_BLOCK_LENGTH - 1 };
			
			BYTE pbData[ MAX_BUFFER_LENGTH ];
			int numRead = 0;

			if (m_fileOpener.getData(&pbData[0], MAX_BUFFER_LENGTH, &numRead)) {
				result = m_fsm.appendData(bNodeAddress, pbData, numRead);
				if (!result)
				{
					m_eventNotifier.signalTaskError( );	
				}					
			}									  				
		}
	}

	if (result){
		displayFileInfo( );
	}
}
void CConfigCanFTP::signalFileClosed(
	BYTE bNodeAddress,
	unsigned long fileSize)
{
	ASSERT( getCurrentNodeAddress() == bNodeAddress );

	if (getCurrentNodeAddress() == bNodeAddress)
	{	
		// Tijdelijke oplossing ??
		// Als het bestand nog open is deze sluiten.

		// TODO:
		// Bij het lezen van een bestand moeten we een temp 
		// bestand gebruiken ...
		if (m_fileOpener.isBusy())
		{
			m_fileOpener.close( );
		}

		m_fsm.signalFileClosed( bNodeAddress, fileSize );	
	}
}

void CConfigCanFTP::signalFileAborted(
	BYTE bNodeAddress,
	unsigned long fileSize)
{
	ASSERT( getCurrentNodeAddress() == bNodeAddress );

	if (getCurrentNodeAddress() == bNodeAddress)
	{		
		m_fsm.signalFileAborted( bNodeAddress, fileSize );	
	}
}

// zie SOLVE_BUG_FTP_CONFIG
void CConfigCanFTP::signalDataAppended(BYTE bNodeAddress)
{
	ASSERT( getCurrentNodeAddress() == bNodeAddress );

	BOOL result = FALSE;
	if (getCurrentNodeAddress() == bNodeAddress)
	{
		m_fsm.signalDataAppended( bNodeAddress );

		if (m_fileOpener.isDone( ))
		{
			m_fileOpener.close( );

			result = m_fsm.closeFile(bNodeAddress);
			if (!result)
			{
				m_eventNotifier.signalTaskError( );	
			}
		}
		else
		{			
			// Oppassen:
			// Er worden maar telkens 109 bytes verstuurd ipv de 110.
			// In feite is de (-1) niet nodig ...

			enum { SEGMENTED_BLOCK_LENGTH = 110 };
			enum { MAX_BUFFER_LENGTH = SEGMENTED_BLOCK_LENGTH - 1 };
			
			BYTE pbData[ MAX_BUFFER_LENGTH ];
			int numRead = 0;

			if (m_fileOpener.getData(&pbData[0],
									 MAX_BUFFER_LENGTH,
									 &numRead))
			{
				// Controle of er karakters uitgelezen werden anders zal wellicht
				// het einde van het bestand bereikt zijn.
				if (numRead > 0)
				{
					result = m_fsm.appendData(bNodeAddress,
												   pbData,
												   numRead);
					if (result)
					{
						displayFileInfo( );
					}
					else
					{
						m_eventNotifier.signalTaskError( );	
					}	
				}
				else
				{
					ASSERT(m_fileOpener.isDone( ));		//Einde van het bestand bereikt.

					result = m_fsm.closeFile(bNodeAddress);
					if (!result)
					{
						m_eventNotifier.signalTaskError( );	
					}
				}
			}									  				
		}
	}
}

void CConfigCanFTP::signalError(
	BYTE bNodeAddress,
	FtpErrorCode ec)
{
	ASSERT( getCurrentNodeAddress() == bNodeAddress );

	if (getCurrentNodeAddress() == bNodeAddress)
	{
		m_fsm.signalError(bNodeAddress, ec);
	}
}
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/////////////// CConfigCanFTP::CFsmEventListener///////////////////////
///////////////////////////////////////////////////////////////////////
CConfigCanFTP::CFsmEventListener::CFsmEventListener(CConfigCanFTP& parent) :
m_parent( parent )
{
}
CConfigCanFTP::CFsmEventListener::~CFsmEventListener( )
{
}
////////////////////////////////////////////////////////////////////////
void CConfigCanFTP::CFsmEventListener::signalTaskDone( )
{
	bool result = false;

	if (m_parent.m_pIterator != 0)
	{
		CConfigCanFTP_FileEntry* pEntry = 
			m_parent.m_pIterator->CurrentItem( );

		if (pEntry)
		{					
			CConfigCanFTP_FileEntry::PF_CALLBACK pf =
				pEntry->getCallbackDone( );
			if (pf)
			{	
				// Callback end functie aanroepen.
				pf(*pEntry);
			}
		}

#ifdef SKIP
		// Sedert versie 0x0B01
		if (m_parent.m_fileOpener.isBusy())
		{
			m_parent.m_fileOpener.close();
		}
#endif
		

		m_parent.m_pIterator->Next( );
		if (!m_parent.m_pIterator->IsDone())
		{
			pEntry = m_parent.m_pIterator->CurrentItem( );
			if (pEntry)
			{			
				result = m_parent.startOperation(*pEntry);
				if (result)
				{
					m_parent.displayFileInfo( );
				
				}												
			}
		}	
	}

	if (!result)
	{		
		m_parent.m_eventNotifier.signalTaskDone( );	
	}	
}
void CConfigCanFTP::CFsmEventListener::signalTaskStarted( )
{
	m_parent.m_eventNotifier.signalTaskStarted( );
}
void CConfigCanFTP::CFsmEventListener::signalTaskCancelled( )
{
	m_parent.m_eventNotifier.signalTaskCancelled( );
}
void CConfigCanFTP::CFsmEventListener::signalTaskError( )
{
	m_parent.m_eventNotifier.signalTaskError( );
}
////////////////////////////////////////////////////////////////////////

