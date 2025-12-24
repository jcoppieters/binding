// LogFileConversionFromNodesErrors.cpp: implementation of the CLogFileConversionFromNodesErrors class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "LogFileConversionFromNodesErrors.h"

#include "LogFileConversionConstruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFileConversionFromNodesErrors::CLogFileConversionFromNodesErrors()
{
	m_fp = 0;
}

CLogFileConversionFromNodesErrors::~CLogFileConversionFromNodesErrors()
{

}

BOOL CLogFileConversionFromNodesErrors::_Start()
{
	BOOL fResult = FALSE;
	
	CString FileName((LPCSTR) IDS_FILENAME_LOGCONVERSIONERROR );
	if (_CreateFile( FileName ) == FALSE) {
		return FALSE;
	}

	GetConversionConstruct()->Init( m_fp );

	return TRUE;
}

BOOL CLogFileConversionFromNodesErrors::_Stop()
{
	BOOL fResult = FALSE;
	
	CloseFile();

	return fResult;
}


