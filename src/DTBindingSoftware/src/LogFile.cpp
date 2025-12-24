// LogFile.cpp: implementation of the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "LogFile.h"

#include "MainFrm.h"
#include "LogFileConversionFromNodes.h"
#include "LogFileConversionFromNodesErrors.h"
#include "LogFileConversionConstruct.h"
#include "utils/win32/ShellFunctions.h"	

// -------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalSettings;
// -------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile()
{
	m_fp = 0;
}

CLogFile::~CLogFile()
{

}

//////////////////////////////////////////////////////////////////////

/**
 *
 */
CLogFileConversionFromNodes* CLogFile::GetConversionFromNodes()
{
	static CLogFileConversionFromNodes instance;
	return &instance;
}

/**
 *
 */
CLogFileConversionFromNodesErrors* CLogFile::GetConversionFromNodesErrors()
{
	static CLogFileConversionFromNodesErrors instance;
	return &instance;
}

/**
 *
 */

CLogFileConversionConstruct* CLogFile::GetConversionConstruct()
{
	static CLogFileConversionConstruct instance;
	return &instance;
}

////////////////////////////////////////////////////////////////////////

/**
 *
 */
BOOL CLogFile::CreateFile(CString szFilename)
{
	if ((m_fp = fopen(szFilename,"w")) != 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CLogFile::CloseFile()
{
	if (m_fp != 0)
	{
		const int result = fclose( m_fp );
		ASSERT( result == 0);

		m_fp = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL CLogFile::AddHeader()
{
	return Write( m_szStartTime );
}

BOOL CLogFile::AddSpace()
{
	return (Write(CString("\n")));
}

BOOL CLogFile::AddSeperator()
{
	CString szSeperator;

	CString szSpace("\n");
	
	for (int i=0;i<50;i++) {
		szSeperator += '-';
	}

	Write( szSpace );
	Write( szSeperator );
	Write( szSpace );

	return TRUE;
}

BOOL CLogFile::Write(CString s)
{
	if (m_fp != 0)
	{
		const int written = fputs((LPCTSTR)s,m_fp);	// CHANGES_0X0A01_USE_FPUTS	
		ASSERT( written>=0 );
		return TRUE;
	}
	return FALSE;
}

BOOL CLogFile::Add(CString s)
{
	return (Write(s));
}

/**
 *
 */
BOOL CLogFile::Start()
{
	if (TMGetFileSettings() != 0)
	{
		CString SubDir((LPCSTR) IDS_FILENAME_SUBDIRLOGFILE );
		CString PathName(TMGetFileSettings()->GetFullyQualifiedPathName());

		ASSERT( SubDir.IsEmpty() == FALSE );
		ASSERT( PathName.IsEmpty() == FALSE );

		const  BOOL fResult = CreateSubDirectory(PathName, SubDir);
		if (fResult == TRUE)
		{
			szPathName.Format("%s\\%s\\",PathName,SubDir);
		
			m_szStartTime = "Start Time :" + 
							CTime::GetCurrentTime().Format( "%d %b, %H:%M:%S " );

			return ( _Start() );
		}
	}
	return FALSE;
}

/**
 *
 */
BOOL CLogFile::Stop()
{
	return ( _Stop() );
}
