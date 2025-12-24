// ExceptionDatabase.cpp: implementation of the CExceptionDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ExceptionDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExceptionDatabase::CExceptionDatabase(CString szMessage)
{
	m_szErrorMessage = CString("Database Exception: \n");
	m_szErrorMessage += szMessage;
}

CExceptionDatabase::~CExceptionDatabase()
{

}


///////////////////////////////////////////////////////////////////////
void CExceptionDatabase::Display()
{
	AfxMessageBox(m_szErrorMessage,MB_ICONERROR);
}

