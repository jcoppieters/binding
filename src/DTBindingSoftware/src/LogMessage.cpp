// LogMessage.cpp: implementation of the CLogMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "LogMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////

CLogMessage* CLogMessage::Create()
{
	CLogMessage *pNew = 0;

	try
	{
		pNew = new CLogMessage();
	}
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();		
	}
	return pNew;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLogMessage::CLogMessage()
{
	Type = LOGMESSAGE_TYPE_INFO;
	szMessage.Empty();
}

CLogMessage::~CLogMessage()
{
	szMessage.Empty();
}

//////////////////////////////////////////////////////////////////////

void CLogMessage::SetInfo(CString s)
{
	Type = LOGMESSAGE_TYPE_INFO;
	szMessage = s;
}

void CLogMessage::SetError(CString s)
{
	Type = LOGMESSAGE_TYPE_ERROR;
	szMessage = s;
}

void CLogMessage::SetWarning(CString s)
{
	Type = LOGMESSAGE_TYPE_WARNING;
	szMessage = s;
}

//////////////////////////////////////////////////////////////////////

BOOL CLogMessage::IsTypeInfo()
{
	return (Type == LOGMESSAGE_TYPE_INFO);	
}

BOOL CLogMessage::IsTypeWarning()
{
	return (Type == LOGMESSAGE_TYPE_WARNING);	
}

BOOL CLogMessage::IsTypeError()
{
	return (Type == LOGMESSAGE_TYPE_ERROR);	
}

//////////////////////////////////////////////////////////////////////