#include "stdafx.h"
#include "ConfigCanFtp_FSM.h"
///////////////////////////////////////////////////////////////////////
#include "Nodemess.h"
#include "TXNodeMessage.h"
#include "IComApplication.h"			// temp
///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON		0
///////////////////////////////////////////////////////////////////////

namespace
{
	template <class PARAM>
		inline void TRACER(PARAM x)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE(x);
		#endif
	};

	template <class PARAM, class PARAM2>
		inline void TRACER(PARAM x, PARAM2 y)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE( x, y );
		#endif
	};


	CString _szFileName;
	CString _szInfoMessage;
};

///////////////////////////////////////////////////////////////////////
///////////////////////// CConfigCanFTP_State /////////////////////////
///////////////////////////////////////////////////////////////////////
unsigned long CConfigCanFTP_State::m_fileSize = 0;
BYTE CConfigCanFTP_State::m_waitFlags = 0;
BYTE CConfigCanFTP_State::m_actionFlags = 0;
BYTE CConfigCanFTP_State::m_ctxFlags=0;
///////////////////////////////////////////////////////////////////////
CConfigCanFTP_State::CConfigCanFTP_State( ) { }
CConfigCanFTP_State::~CConfigCanFTP_State( ) { }
///////////////////////////////////////////////////////////////////////
void CConfigCanFTP_State::enter(CConfigCanFTP_FSM* parent)
{
	TRACER("CConfigCanFTP_State::enter\n");
}

void  CConfigCanFTP_State::leave(CConfigCanFTP_FSM* parent)
{
	TRACER("CConfigCanFTP_State::leave\n");
}

BOOL CConfigCanFTP_State::openFileWrite(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress, 
	const char* szFileName,
	const char* szInfo)
{
	TRACER("CConfigCanFTP_State::openFileWrite\n");

	return FALSE;
}

BOOL CConfigCanFTP_State::openFileRead(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress, 
	const char* szFileName,
	const char* szInfo,
	BOOL flagFileNotFoundSuppressError)
{
	TRACER("CConfigCanFTP_State::openFileRead\n");

	return FALSE;
}

BOOL CConfigCanFTP_State::appendData(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	BYTE* pbData, 
	int length)
{
	TRACER("CConfigCanFTP_State::appendData\n");

	return FALSE;
}

BOOL CConfigCanFTP_State::closeFile(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)	
{
	TRACER("CConfigCanFTP_State::closeFile\n");

	return FALSE;
}

BOOL CConfigCanFTP_State::abortFile(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)	
{
	TRACER("CConfigCanFTP_State::abortFile\n");

	return FALSE;
}

void CConfigCanFTP_State::signalFileWriteOpened(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_State::signalFileWriteOpened\n");
}

void CConfigCanFTP_State::signalFileReadOpened(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_State::signalFileReadOpened\n");
}

void CConfigCanFTP_State::signalDataAppended(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)
{
	TRACER("CConfigCanFTP_State::signalDataAppended\n");
}

void CConfigCanFTP_State::signalFileClosed(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_State::signalFileClosed\n");
}

void CConfigCanFTP_State::signalFileAborted(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_State::signalFileAborted\n");
}

void CConfigCanFTP_State::signalDataReceived(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	const BYTE* pbData,
	int numBytes)
{
	TRACER("CConfigCanFTP_State::signalDataReceived\n");
}

void CConfigCanFTP_State::signalError(CConfigCanFTP_FSM* parent,BYTE bNodeAddress,FtpErrorCode errorCode)
{
	TRACER("CConfigCanFTP_State::signalError\n");
	
	switch (errorCode)
	{
		case FTP_ERROR_WRONG_STATE:
		{		
			CTXNodeFTPFileControl::getInstance()->SendAbort( bNodeAddress );		
			parent->notifyTaskError( );		
		} break;

		case FTP_ERROR_GET_INFO:
		case FTP_ERROR_WRITE_DATA:
		case FTP_ERROR_READ_DATA:
		case FTP_ERROR_NOT_IMPLEMENTED:
		case FTP_ERROR_GENERAL:
		{
			CTXNodeFTPFileControl::getInstance()->SendAbort( bNodeAddress );		
			parent->notifyTaskError( );			
		} break;

		case FTP_ERROR_FILE_OPEN:
		{	
			// Sedert versie 0x0B01:
			// Indien een bestand niet aanwezig is dan geen foutmelding geven...

			BOOL fSignalError=TRUE;
			if (getActionFlags( ) == ACTION_FLAG_READ) {
				if (this->getCtxFlags()&CTXFLAG_READ_FILENOTFOUND_SUPPRESS_ERROR) {
					fSignalError = FALSE;
				}
			}
			if (TRUE==fSignalError) {
				parent->notifyTaskError( );
			}
			else {
				parent->notifyTaskDone( );
			}

		} break;

		case FTP_ERROR_FILE_CLOSE:
		{			
			parent->notifyTaskError( );
		} break;
	}

	// zou kunnen naar een error toestand.
	parent->setState_IDLE( );	
}

void CConfigCanFTP_State::signalInfoDialogACK(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)
{
	TRACER("CConfigCanFTP_State::signalInfoDialogACK\n");
}
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
////////////////// CConfigCanFTP_StateIDLE ////////////////////////
///////////////////////////////////////////////////////////////////
CConfigCanFTP_StateIDLE::CConfigCanFTP_StateIDLE( ) :
Base( )
{
	// Moeten altijd de vlaggen wissen !!
	// De vlaggen zijn static data en hun status blijft behouden
	// nadat de FSM gestopt is (en uit geheugen verwijderd is.)

	clearWaitFlags( ); 
	clearActionFlags( );
	clearCtxFlags();
}
CConfigCanFTP_StateIDLE::~CConfigCanFTP_StateIDLE( )
{
}
///////////////////////////////////////////////////////////////////////
void CConfigCanFTP_StateIDLE::enter(
	CConfigCanFTP_FSM* parent)
{
	TRACER("CConfigCanFTP_StateIDLE::enter\n");

	// Bij de initial transitie wordt dit niet uitgevoerd, 
	// daarom ook in de constructor.

	clearWaitFlags( ); 
	clearActionFlags( );
}

BOOL CConfigCanFTP_StateIDLE::openFileWrite(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress, 
	const char* szFileName,
	const char* szInfo)
{
	TRACER("CConfigCanFTP_StateIDLE::openFile\n");

	BOOL retVal = FALSE;
	
	if (szInfo == 0)
	{
		_szInfoMessage = "Please wait, writing file";
	}	
	else
	{
		_szInfoMessage.Format("Please wait, write %s",
							  szInfo);					  
	}

	if (CTXNodeMessage::getInstance()->SendLcdInfo(
			bNodeAddress,
			_szInfoMessage))
	{
		setActionFlag(ACTION_FLAG_WRITE);		// NEW !!!!!
		_szFileName = szFileName;
		parent->setState_WAITOPEN( );
		retVal = TRUE;
	}	
	
	return retVal;
}

BOOL CConfigCanFTP_StateIDLE::openFileRead(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress, 
	const char* szFileName,
	const char* szInfo,
	BOOL flagFileNotFoundSuppressError)
{
	TRACER("CConfigCanFTP_StateIDLE::openFileRead\n");

	BOOL retVal = FALSE;
	
	if (szInfo == 0)
	{
		_szInfoMessage = "Please wait, reading file";
	}	
	else
	{
		_szInfoMessage.Format("Please wait, read %s",
							  szInfo);					  
	}

	if (CTXNodeMessage::getInstance()->SendLcdInfo(
			bNodeAddress,
			_szInfoMessage))
	{
		setActionFlag(ACTION_FLAG_READ);		// NEW !!!!!
		
		// Sedert 0x0B01
		if (TRUE==flagFileNotFoundSuppressError)
		{
			setCtxFlag(CTXFLAG_READ_FILENOTFOUND_SUPPRESS_ERROR);
		}

		_szFileName = szFileName;
		parent->setState_WAITOPEN( );
		retVal = TRUE;
	}
	
	return retVal;
}

void CConfigCanFTP_StateIDLE::signalError(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	FtpErrorCode errorCode)
{
	TRACER("CConfigCanFTP_StateIDLE::signalError\n");

	clearWaitFlags( );  
	this->Base::signalError(parent, bNodeAddress, errorCode);
}
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//////////////// CConfigCanFTP_StateWAITOPEN //////////////////////
///////////////////////////////////////////////////////////////////
CConfigCanFTP_StateWAITOPEN::CConfigCanFTP_StateWAITOPEN( ) :
Base( )
{
}
CConfigCanFTP_StateWAITOPEN::~CConfigCanFTP_StateWAITOPEN( )
{
}
///////////////////////////////////////////////////////////////////////
void CConfigCanFTP_StateWAITOPEN::enter(
	CConfigCanFTP_FSM* parent)
{
	TRACER("CConfigCanFTP_StateWAITOPEN::enter\n");

	clearWaitFlags( ); 
}
void CConfigCanFTP_StateWAITOPEN::signalFileWriteOpened(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_StateWAITOPEN::signalFileWriteOpened\n");

	if (getWaitFlags() == WAIT_FLAG_OPEN_WRITE)
	{
		TRACER("FileSize = %d\n",ulFileSize);

		clearWaitFlag( WAIT_FLAG_OPEN_WRITE );
		this->Base::setFileSize(ulFileSize);
		parent->setState_OPENWRITE( );
		parent->notifyTaskStarted( );
	}
}

void CConfigCanFTP_StateWAITOPEN::signalFileReadOpened(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_StateWAITOPEN::signalFileReadOpened\n");

	if (getWaitFlags() == WAIT_FLAG_OPEN_READ)
	{
		TRACER("FileSize = %d\n",ulFileSize);

		clearWaitFlag( WAIT_FLAG_OPEN_READ );
		this->Base::setFileSize(ulFileSize);

		BOOL retVal = CTXNodeFTPFileControl::getInstance()->
			SendReadData(
				bNodeAddress,
				0,
				110);					// Tijdelijk, moet via een macro.

		if (retVal == TRUE)
		{
			parent->setState_OPENREAD( );
			parent->notifyTaskStarted( );
		}
		else
		{
			/** Wat doen ??? */
		}		
	}
}

BOOL CConfigCanFTP_StateWAITOPEN::abortFile(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)
{
	// Oppassen:
	// Als de abort file door een CANCEL operatie werd uitgevoerd
	// dan zal de FSM stoppen.
	// De parent klasse verwijderd deze FSM uit het geheugen.
	
	TRACER("CConfigCanFTP_StateWAITOPEN::abortFile\n");

	BOOL retVal = CTXNodeFTPFileControl::getInstance()->
					SendAbort( bNodeAddress );

	if (retVal == TRUE)
	{
		parent->setState_CLOSE( );
	}
	return retVal;
}

void CConfigCanFTP_StateWAITOPEN::signalError(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	FtpErrorCode errorCode)
{
	TRACER("CConfigCanFTP_StateWAITOPEN::signalError\n");

	clearWaitFlags( );  
	this->Base::signalError(parent, bNodeAddress, errorCode);
}

void CConfigCanFTP_StateWAITOPEN::signalInfoDialogACK(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)
{
	TRACER("CConfigCanFTP_StateWAITOPEN::signalInfoDialogACK\n");

	if (getWaitFlags( ) == WAIT_FLAG_IDLE)
	{
		BOOL retVal = FALSE;
		if (getActionFlags( ) == ACTION_FLAG_READ)
		{
			retVal = CTXNodeFTPFileControl::getInstance()->
						SendReadOpen(
							bNodeAddress,
							_szFileName);

			if (retVal == TRUE)
			{
				setWaitFlag( WAIT_FLAG_OPEN_READ );
			}	
		}
		else if (getActionFlags( ) == ACTION_FLAG_WRITE)
		{
			retVal = CTXNodeFTPFileControl::getInstance()->
						SendWriteOpen(
							bNodeAddress,
							_szFileName);

			if (retVal == TRUE)
			{
				setWaitFlag( WAIT_FLAG_OPEN_WRITE );
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}
}
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
////////////////// CConfigCanFTP_StateOPEN ////////////////////////
///////////////////////////////////////////////////////////////////
CConfigCanFTP_StateOPEN::CConfigCanFTP_StateOPEN( ) :
Base( )
{
}
CConfigCanFTP_StateOPEN::~CConfigCanFTP_StateOPEN( )
{
	// pure virtual destructor...
}
///////////////////////////////////////////////////////////////////////
void CConfigCanFTP_StateOPEN::enter(
	CConfigCanFTP_FSM* parent)
{
	TRACER("CConfigCanFTP_StateOPEN::enter\n");
}

BOOL CConfigCanFTP_StateOPEN::abortFile(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)
{
	// Oppassen:
	// Als de abort file door een CANCEL operatie werd uitgevoerd
	// dan zal de FSM stoppen.
	// De parent klasse verwijderd deze FSM uit het geheugen.
	
	TRACER("CConfigCanFTP_StateOPEN::abortFile\n");

	BOOL retVal = CTXNodeFTPFileControl::getInstance()->
					SendAbort( bNodeAddress );

	if (retVal == TRUE)
	{
		parent->setState_CLOSE( );
	}
	return retVal;
}


BOOL CConfigCanFTP_StateOPEN::closeFile(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress)
{
	TRACER("CConfigCanFTP_StateOPEN::closeFile\n");

	BOOL retVal = FALSE;

	retVal = CTXNodeFTPFileControl::getInstance()->
		SendClose( bNodeAddress );

	if (retVal == TRUE)
	{
		parent->setState_CLOSE( );
	}
	return retVal;
}

void CConfigCanFTP_StateOPEN::signalError(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	FtpErrorCode errorCode)
{
	TRACER("CConfigCanFTP_StateOPEN::signalError\n");

	clearWaitFlags( );
	this->Base::signalError(parent, bNodeAddress, errorCode);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//////////////// CConfigCanFTP_StateOPENWRITE /////////////////////
///////////////////////////////////////////////////////////////////
CConfigCanFTP_StateOPENWRITE::CConfigCanFTP_StateOPENWRITE( ) :
Base( )
{
}
CConfigCanFTP_StateOPENWRITE::~CConfigCanFTP_StateOPENWRITE( )
{
}
///////////////////////////////////////////////////////////////////////
void CConfigCanFTP_StateOPENWRITE::enter(
	CConfigCanFTP_FSM* /*parent*/)			// CHANGES_MVS_2008_WARNINGS
{
	TRACER("CConfigCanFTP_StateOPENWRITE::enter\n");

	m_ulPosition = 0;
	clearWaitFlags( ); 
}

BOOL CConfigCanFTP_StateOPENWRITE::appendData(
	CConfigCanFTP_FSM* /*parent*/,		// CHANGES_MVS_2008_WARNINGS
	BYTE bNodeAddress,
	BYTE* pbData, 
	int length)
{
	TRACER("CConfigCanFTP_StateOPENWRITE::appendData\n");

	BOOL retVal = FALSE;

	if (getWaitFlags() != WAIT_FLAG_ACK)
	{
		retVal = CTXNodeFTPFileControl::getInstance()->
				SendAppendData(
						bNodeAddress,
						m_ulPosition,		// Is dit ok ???
						pbData,
						length);

		if (retVal == TRUE)
		{
			m_ulPosition += length;			// Is dit ok ???
			setWaitFlag( WAIT_FLAG_ACK );	
		}
	}
	return retVal;
}

void CConfigCanFTP_StateOPENWRITE::signalDataAppended(
	CConfigCanFTP_FSM* /*parent*/,				// CHANGES_MVS_2008_WARNINGS
	BYTE /*bNodeAddress*/)						// CHANGES_MVS_2008_WARNINGS
{
	TRACER("CConfigCanFTP_StateOPENWRITE::signalDataAppended\n");

	if (getWaitFlags() == WAIT_FLAG_ACK)
	{
		clearWaitFlag( WAIT_FLAG_ACK );
	}
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
///////////////// CConfigCanFTP_StateOPENREAD /////////////////////
///////////////////////////////////////////////////////////////////
CConfigCanFTP_StateOPENREAD::CConfigCanFTP_StateOPENREAD( ) :
Base( ), m_ulBytesReceived( 0 ) { }
CConfigCanFTP_StateOPENREAD::~CConfigCanFTP_StateOPENREAD( ) { }
///////////////////////////////////////////////////////////////////////
void CConfigCanFTP_StateOPENREAD::enter(CConfigCanFTP_FSM*)			
{
	TRACER("CConfigCanFTP_StateOPENREAD::enter\n");
	clearWaitFlags( ); 
	m_ulBytesReceived = 0;
}

void CConfigCanFTP_StateOPENREAD::signalDataReceived(CConfigCanFTP_FSM* parent, BYTE bNodeAddress,const BYTE*,int numBytes)
{
	TRACER("CConfigCanFTP_StateOPENREAD::signalDataReceived\n");

	clearWaitFlag(WAIT_FLAG_ACK);
	m_ulBytesReceived += numBytes;

	TRACE("signalDataReceived(): numBytes = %d",numBytes);
	TRACE(",m_ulBytesReceived = %d",m_ulBytesReceived);

	if (m_ulBytesReceived < getFileSize()) {
		BOOL retVal = CTXNodeFTPFileControl::getInstance()->SendReadData(bNodeAddress, m_ulBytesReceived, 110);				
		if (retVal == TRUE) {
			setWaitFlag( WAIT_FLAG_ACK );	
		}
		else {
			// Fout, wat doen ???
		}
	}
	else
	{
		BOOL retVal = CTXNodeFTPFileControl::getInstance()->SendClose( bNodeAddress );
		if (retVal == TRUE) {
			parent->setState_CLOSE( );
		}
	}
}

/////////////////////////////////////////////////////////////////////
/////////////////// CConfigCanFTP_StateCLOSE ////////////////////////
/////////////////////////////////////////////////////////////////////
CConfigCanFTP_StateCLOSE::CConfigCanFTP_StateCLOSE( ) :
Base( )
{
}
CConfigCanFTP_StateCLOSE::~CConfigCanFTP_StateCLOSE( )
{
}
/////////////////////////////////////////////////////////////////////
void CConfigCanFTP_StateCLOSE::enter(
	CConfigCanFTP_FSM* /*parent*/)		// CHANGES_MVS_2008_WARNINGS
{
	TRACER("CConfigCanFTP_StateCLOSE::enter\n");
}

void CConfigCanFTP_StateCLOSE::signalFileClosed(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_StateCLOSE::signalFileClosed\n");
	TRACER("FileSize = %d\n",ulFileSize);

	if (getActionFlags( ) == ACTION_FLAG_READ || 
		getActionFlags( ) == ACTION_FLAG_WRITE)
	{
		CTXNodeMessage::getInstance()->SendLcdInfo(
			bNodeAddress,
			"Configuration done ...");
	}

	parent->setState_IDLE( );
	parent->notifyTaskDone( );
}

void CConfigCanFTP_StateCLOSE::signalError(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress,
	FtpErrorCode errorCode)
{
	TRACER("CConfigCanFTP_StateCLOSE::signalError\n");

	this->Base::signalError(parent, bNodeAddress, errorCode);
}

void CConfigCanFTP_StateCLOSE::signalFileAborted(
	CConfigCanFTP_FSM* parent,
	BYTE bNodeAddress, 
	unsigned long ulFileSize)
{
	TRACER("CConfigCanFTP_StateCLOSE::signalFileAborted\n");
	TRACER("FileSize = %d\n",ulFileSize);

	if (getActionFlags( ) == ACTION_FLAG_READ)
	{
		CTXNodeMessage::getInstance()->SendLcdInfo(
			bNodeAddress,
			"Read cconfiguration cancelled");
	}
	else if (getActionFlags( ) == ACTION_FLAG_WRITE)
	{
		CTXNodeMessage::getInstance()->SendLcdInfo(
			bNodeAddress,
			"Write configuration cancelled");
	}

	clearWaitFlags( );
	parent->setState_IDLE( );
	parent->notifyTaskCancelled( );
}
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
///////////////////////// CConfigCanFTP_FSM //////////////////////////
///////////////////////////////////////////////////////////////////////
CConfigCanFTP_FSM::CConfigCanFTP_FSM(IConfigCanFTP_Notifier& notifier) :
m_notifier( notifier ),
m_pCurrentState( &m_stateIDLE )
{
}
CConfigCanFTP_FSM::~CConfigCanFTP_FSM( )
{
}
////////////////////////////////////////////////////////////////////////
void CConfigCanFTP_FSM::reset( )
{
	m_pCurrentState = &m_stateIDLE;
}

BOOL CConfigCanFTP_FSM::openFileWrite(
	BYTE bNodeAddress, 
	const char* szFileName,
	const char* szInfo)
{
	return m_pCurrentState->openFileWrite(
			this,
			bNodeAddress,
			szFileName,
			szInfo);
}

BOOL CConfigCanFTP_FSM::openFileRead(
	BYTE bNodeAddress, 
	const char* szFileName,
	const char* szInfo,
	BOOL flagFileNotFoundSuppressError)
{
	return m_pCurrentState->openFileRead(
			this,
			bNodeAddress,
			szFileName,
			szInfo,
			flagFileNotFoundSuppressError);
}

BOOL CConfigCanFTP_FSM::abortFile(
	BYTE bNodeAddress)
{
	return m_pCurrentState->abortFile(
			this,
			bNodeAddress);
}

BOOL CConfigCanFTP_FSM::closeFile(
	BYTE bNodeAddress)
{
	return m_pCurrentState->closeFile(
			this,
			bNodeAddress);
}

BOOL CConfigCanFTP_FSM::appendData(
	BYTE bNodeAddress,
	BYTE* pbData, 
	int length)
{
	return m_pCurrentState->appendData(
		this,
		bNodeAddress,
		pbData,
		length);
}

void CConfigCanFTP_FSM::signalFileWriteOpened(
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	m_pCurrentState->signalFileWriteOpened(
		this, 
		bNodeAddress,
		ulFileSize);
}

void CConfigCanFTP_FSM::signalFileReadOpened(
	BYTE bNodeAddress, 
	unsigned long ulFileSize)
{
	m_pCurrentState->signalFileReadOpened(
		this, 
		bNodeAddress,
		ulFileSize);
}

void CConfigCanFTP_FSM::signalFileAborted(
	BYTE bNodeAddress, 
	unsigned long ulFileSize)
{
	m_pCurrentState->signalFileAborted(
		this, 
		bNodeAddress,
		ulFileSize);
}

void CConfigCanFTP_FSM::signalDataAppended(
	BYTE bNodeAddress)
{
	m_pCurrentState->signalDataAppended(
		this,
		bNodeAddress);
}

void CConfigCanFTP_FSM::signalDataReceived(	
	BYTE bNodeAddress,											
	const BYTE* pbData,
	int numBytes)
{
	m_pCurrentState->signalDataReceived(
		this,
		bNodeAddress,
		pbData,
		numBytes);
}

void CConfigCanFTP_FSM::signalFileClosed(
	BYTE bNodeAddress,
	unsigned long ulFileSize)
{
	m_pCurrentState->signalFileClosed(
		this,
		bNodeAddress,
		ulFileSize);
}

void CConfigCanFTP_FSM::signalError(
	BYTE bNodeAddress,
	FtpErrorCode errorCode)
{
	m_pCurrentState->signalError(
		this,
		bNodeAddress,
		errorCode);
}

void CConfigCanFTP_FSM::signalInfoDialogACK(
	BYTE bNodeAddress)
{
	m_pCurrentState->signalInfoDialogACK(
		this,
		bNodeAddress);
}

void CConfigCanFTP_FSM::tran(
	CConfigCanFTP_State& newState)
{
	m_pCurrentState->leave( this );
	m_pCurrentState = &newState;
	m_pCurrentState->enter( this );
}


