// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "Log.h"

#include "LogMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	m_Messages.RemoveAll();
	Clear();
}

CLog::~CLog()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

/**
 * void CLog::Clear()
 */
void CLog::Clear()
{
	if (Count() > 0)
	{
		for (int i=0;i<Count();i++)
		{
			CLogMessage* pMessage = static_cast<CLogMessage*>(m_Messages.GetAt(i));

			if (pMessage != 0)
			{
				delete pMessage;
				pMessage = 0;
			}
		}

		m_Messages.RemoveAll();
	}

	m_ErrorCnt = 0;
	m_WarningCnt = 0;
}

/**
 * void CLog::AddError(CString s)
 */
void CLog::AddError(CString s)
{
	CLogMessage* const pMessage = CLogMessage::Create();
	if (pMessage !=0)
	{
		CString string("ERROR: " + s);

		pMessage->SetError(string);
		m_Messages.Add(pMessage);

		m_ErrorCnt++;
	}
}

/**
 * void CLog::AddWarning(CString s)
 */
void CLog::AddWarning(CString s)
{	
	CLogMessage* const pMessage = CLogMessage::Create();
	if (pMessage !=0)
	{
		CString string("WARNING: " + s);

		pMessage->SetWarning(string);
		m_Messages.Add(pMessage);

		m_WarningCnt++;
	}
}

/**
 * void CLog::AddInfo(CString s)
 */
void CLog::AddInfo(CString s)
{
	CLogMessage* const pMessage = CLogMessage::Create();
	if (pMessage !=0)
	{
		CString string("INFO: " + s);

		pMessage->SetInfo(string);
		m_Messages.Add(pMessage);	
	}
}

//////////////////////////////////////////////////////////////////////

/**
 * BOOL CLog::IsError()
 */
BOOL CLog::IsError() 
{
	return (m_ErrorCnt > 0);
}

/**
 * BOOL CLog::IsWarning()
 */
BOOL CLog::IsWarning()
{
	return (m_WarningCnt > 0);
}

/**
 * int CLog::Count() 
 */
int CLog::Count() 
{
	return m_Messages.GetSize();
}

/**
 * CString CLog::Get(int index)
 */
CString CLog::GetMessage(int index)
{	
	if (index < Count())
	{
		CLogMessage* const pMessage = static_cast<CLogMessage*> 
			(m_Messages.GetAt( index ));

		if (pMessage != 0)
		{
			if (pMessage->szMessage.IsEmpty() == FALSE)
			{
				return (pMessage->szMessage);
			}
		}
	}
	return CString("");
}

/**
 * CLogMessage* CLog::Get(int index)
 */
CLogMessage* CLog::Get(int index)
{
	if (index < Count())
	{
		CLogMessage* const pMessage = static_cast<CLogMessage*> 
			(m_Messages.GetAt( index ));

		ASSERT( pMessage);

		return pMessage;
	}

	return 0;
}

/**
 * int CLog::GetError(CString& szError,int index)
 */
int CLog::GetError(CString& szError,int index)
{
	return (Get(szError, index, &CLogMessage::IsTypeError));
}

/**
 * int CLog::GetWarning(CString& szError,int index)
 */
int CLog::GetWarning(CString& szWarning,int index)
{
	return (Get(szWarning, index, &CLogMessage::IsTypeWarning));
}

/**
 * int CLog::GetInfo(CString& szError,int index)
 */
int CLog::GetInfo(CString& szInfo,int index)
{
	return (Get(szInfo, index, &CLogMessage::IsTypeInfo));
}

/**
 * int CLog::Get(CString &szMessage,int index,BOOL (CLogMessage::*ptr) ())
 */
int CLog::Get(CString &szMessage,int index,BOOL (CLogMessage::*ptr) ())
{
	const int nMax = Count();

	do 
	{
		CLogMessage* const pMessage = Get( index );
		
		if ((pMessage != 0) &&
			((pMessage->*ptr) () == TRUE))
		{		
			szMessage = pMessage->szMessage;
			return index;			
		}

		index++;
	}
	while(index < nMax);

	return -1;
}