// NotifyError.cpp: implementation of the CNotifyError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NotifyError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotifyError::CNotifyError()
{

}

CNotifyError::~CNotifyError()
{

}


//////////////////////////////////////////////////////////////////////

CNotifyError_None::CNotifyError_None()
{

}

CNotifyError_None::~CNotifyError_None()
{

}

void CNotifyError_None::operator()(int error)
{
	TRACE1("CNotifyError_None : %i\n",error);
}

void CNotifyError_None::operator()(CString szError)
{
	TRACE1("CNotifyError_None : %s\n",szError);
}

//////////////////////////////////////////////////////////////////////

CNotifyError_Simple::CNotifyError_Simple()
{

}

CNotifyError_Simple::~CNotifyError_Simple()
{

}

void CNotifyError_Simple::operator()(int error)
{
	LPVOID lpMsgBuf = 0;

	const int flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS;

	try
	{
		FormatMessage( flags,
					   NULL,
					   error,
					   0, 
					   (LPTSTR) &lpMsgBuf,
					   0,
					   NULL );
	
		AfxMessageBox((LPCTSTR) lpMsgBuf, MB_ICONERROR);				

		LocalFree(lpMsgBuf);
	}
	catch(CException* e)
	{
		if (lpMsgBuf != 0)
			LocalFree(lpMsgBuf);				

		e->Delete( );
	}
}

void CNotifyError_Simple::operator()(CString szError)
{
	AfxMessageBox( szError, MB_ICONERROR );		
}



//////////////////////////////////////////////////////////////////////

CNotifyError_FileOpener::CNotifyError_FileOpener()
{

	m_ErrorMessages.SetAt( 31, "There is no application associated with "
							   "the given filename extension" );

	// De andere berichten zijn standaard duidelijk genoeg.

}

CNotifyError_FileOpener::~CNotifyError_FileOpener()
{

}

void CNotifyError_FileOpener::operator()(int error)
{
	CString szError;
	if (m_ErrorMessages.Lookup(error,szError))
	{
		AfxMessageBox(szError, MB_ICONERROR);
	}
	else
	{
		this->Base::operator()(error);
	}
}

//////////////////////////////////////////////////////////////////////

CNotifyError_PdfOpener::CNotifyError_PdfOpener() :
Base()
{
	m_ErrorMessages.SetAt( 31, "Unable to open the *.pdf file\n"			
		"To open a *.pdf file you must have Adobe Reader installed on your computer.");
}

CNotifyError_PdfOpener::~CNotifyError_PdfOpener()
{

}
