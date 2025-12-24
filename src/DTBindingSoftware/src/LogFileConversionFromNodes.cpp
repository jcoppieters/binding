// LogFileConversionFromNodes.cpp: implementation of the CLogFileConversionFromNodes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "LogFileConversionFromNodes.h"

#include "BindingEntryOperations.h"
#include "BindingFile1.h"
#include "LogFileConversionFromNodesErrors.h"
#include "LogFileConversionConstruct.h"
#include "BindingUnitOperations.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnitOperations.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
unsigned short CLogFileConversionFromNodes::m_usCurrentBindingNr = 0;
CStringArray* CLogFileConversionFromNodes::m_pCurrentStrings  = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFileConversionFromNodes::CLogFileConversionFromNodes()  { }

CLogFileConversionFromNodes::~CLogFileConversionFromNodes() { }

CLogFileConversionFromNodesErrors* CLogFileConversionFromNodes::getErrors() {
	return CLogFile::GetConversionFromNodesErrors();
}

BOOL CLogFileConversionFromNodes::_CreateFile(CString FileName)
{
	CString szFileName(szPathName+"\\"+FileName);

	if (CreateFile( szFileName ) == FALSE) {
		return FALSE;
	}

	if (this->CLogFile::AddHeader() == FALSE) {
		return FALSE;
	}

	if (AddSeperator() == FALSE) {
		return FALSE;
	}

	return TRUE;
}

BOOL CLogFileConversionFromNodes::_Start()
{
	BOOL fResult = FALSE;
	
	CString FileName((LPCSTR) IDS_FILENAME_LOGCONVERSION );
	if (_CreateFile( FileName ) == FALSE) {
		return FALSE;
	}

	getErrors()->Start();	// Start Error log
	return TRUE;
}

BOOL CLogFileConversionFromNodes::_Stop()
{
	BOOL fResult = TRUE;
	
	CloseFile();
	getErrors()->Stop();	// Stop Error log 

	return fResult;
}


BOOL CLogFileConversionFromNodes::Search(unsigned short usBindingNr)
{
	CString szMessage;

	m_usCurrentBindingNr = usBindingNr;

	AddSeperator( );

	szMessage.Format("Searching bindingnr : 0x%04x\n",usBindingNr);
	BOOL fResult = Write( szMessage );
	return fResult;
}

BOOL CLogFileConversionFromNodes::Process(CStringArray *pStringArray)
{
	if (pStringArray == 0) {
		return FALSE;
	}
	
	m_pCurrentStrings = pStringArray;

	Report( pStringArray );

	Write("\n");
	
	return TRUE;
}

BOOL CLogFileConversionFromNodes::Success(const CBindingEntry* const pBindingEntry)
{
	if (pBindingEntry != 0)
	{
		if (1)
		{
			CString szMessage("BindingEntry Processed Successfully");
			if ( Write( szMessage ) == FALSE) {
				return FALSE;
			}	
		}

		CBindingEntryOperations Operations(const_cast<CBindingEntry*>(pBindingEntry));
		return (Operations.Export(m_fp));

	}
	return FALSE;
}

BOOL CLogFileConversionFromNodes::Success(CString szMessage,const CBindingPropertyUnit* const pBindingPropertyUnit)
{
	if (pBindingPropertyUnit != 0)
	{
		CBindingPropertyUnitOperations Operations(const_cast<CBindingPropertyUnit*>(pBindingPropertyUnit));
		if (Operations.Export(m_fp) == FALSE)
			return FALSE;
	}
	
	CString Message;
	Message.Format(": %s",szMessage);
	if ( Write( Message ) == FALSE) {
		return FALSE;
	}
	return TRUE;
}

/*
BOOL CLogFileConversionFromNodes::Failed(const CBindingPropertyUnit* const pBindingPropertyUnit)
{
	if (pBindingPropertyUnit != 0)
	{
		CBindingPropertyUnitOperations Operations(const_cast<CBindingPropertyUnit*>(pBindingPropertyUnit));
		if (Operations.Export(m_fp) == FALSE)
			return FALSE;

		if (1)
		{
			CString szMessage(": Failed to process");
	
			if ( Write( szMessage ) == FALSE) {
				return FALSE;
			}	
		}		
	}

	if (this != m_pErrors)
	{
		m_pErrors->Failed( pBindingPropertyUnit );
	}

	return TRUE;
}
*/

BOOL CLogFileConversionFromNodes::Failed(const CBindingUnit* const pBindingUnit,CString szPropertyBinding)
{
	if (pBindingUnit != 0)
	{
		if (1)
		{
			CString szMessage;
			szMessage.Format("\nFailed to process P-Binding:'%s'",szPropertyBinding);
	
			if ( Write( szMessage ) == FALSE) {
				return FALSE;
			}	
		}
		
		CBindingUnitOperations Operations(const_cast<CBindingUnit*>(pBindingUnit));
		if (Operations.Export(m_fp) == FALSE)
			return FALSE;

		Write("\n");
		
	}

	if (this != getErrors())
	{
		getErrors()->Failed( pBindingUnit, szPropertyBinding );
	}

	return TRUE;
}


BOOL CLogFileConversionFromNodes::Found(CString string,CBindingFile* const pBindingFile,CStringArray* pStringArray)
{
	AddSeperator( );

	if (pBindingFile != 0)
	{
		CString szMessage;
		szMessage.Format("%s ,node address: 0x%02x\n",
						 string,
						 pBindingFile->GetNodeAddress());

		if ( Write( szMessage ) == FALSE) {
			return FALSE;
		}	
	}

	if (pStringArray != 0)
	{
		for (int i=0; i<pStringArray->GetSize(); i++)
		{
			CString szMessage(pStringArray->GetAt(i));
			szMessage += "\n";

			if ( Write( szMessage ) == FALSE) {
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CLogFileConversionFromNodes::Failed(CStringArray *pStringArray)
{
	if (this != getErrors())
	{
		getErrors()->Failed( pStringArray );
	}
	else
	{
		AddSeperator();

		if (1)
		{
			CString szMessage;
			szMessage.Format("Failed to process: BindingNr 0x%04x\n",
							 m_usCurrentBindingNr);
	
			if ( Write( szMessage ) == FALSE) {
				return FALSE;
			}	
		}	

		Report( pStringArray );

		AddSeperator();
	}

	return TRUE;
}

BOOL CLogFileConversionFromNodes::Replaced(CStringArray* const pStringArray,CStringArray* const pStringsReplaced)
{	
	if (1)
	{
		CString szMessage;
		szMessage.Format("Original Strings - BindingNr 0x%04x\n",m_usCurrentBindingNr);

		if ( Write( szMessage ) == FALSE) {
			return FALSE;
		}	
	}

	Report( pStringArray );

	if (1)
	{
		CString szMessage("Replaced Strings\n");
		if ( Write( szMessage ) == FALSE) {
			return FALSE;
		}	
	}

	Report( pStringsReplaced );

	AddSeperator();
	
	if (this != getErrors())
	{
		getErrors()->Replaced( pStringArray, pStringsReplaced);
	}

	return TRUE;
}

void CLogFileConversionFromNodes::Report(CStringArray* pStrings,BOOL fDisplayIndex)
{
	CString szMessage;
	for (int i=0;i<pStrings->GetSize();i++)
	{
		if (fDisplayIndex == TRUE) {
			szMessage.Format("index:%d :'%s'\n",i,pStrings->GetAt(i));
		}
		else {
			szMessage = pStrings->GetAt(i) + "\n";
		}

		if ( Write( szMessage ) == FALSE) {
			ASSERT( 0 );
		}			
	}	
}

BOOL CLogFileConversionFromNodes::ReportErrors(CString szError,CBindingFile* const pBindingFile,CStringArray* pStringArray)
{
	ASSERT( pBindingFile );
	ASSERT( pStringArray );

	if (pBindingFile != 0 && pStringArray != 0)
	{
		AddSeperator();
	
		if (1)
		{	
			const BYTE bNodeAddress = pBindingFile->GetNodeAddress();

			CString szMessage;
			szMessage.Format("%s : NodeAddress 0x%02x\n",szError,bNodeAddress);

			if ( Write( szMessage ) == FALSE) {
				return FALSE;
			}	
		}
	
		Report(pStringArray,FALSE);

		if (1)
		{
			CString szMessage("\n");
			
			if ( Write( szMessage ) == FALSE) {
				return FALSE;
		}
		}

		if (this != getErrors())
		{	
			getErrors()->ReportErrors( szError, pBindingFile, pStringArray );
			return TRUE;
		}
	}
	
	return FALSE;
}

/**
 *
 */
BOOL CLogFileConversionFromNodes::AddHeader(CString szMessage)
{
	AddSpace();
	AddSeperator();	
	if (1)
	{
		CString s(szMessage);
		if ( Write( s ) == FALSE) {
			return FALSE;
		}	
	}
	AddSeperator();
	AddSpace();
	AddSpace();

	if (this != getErrors())
	{
		getErrors()->AddHeader(szMessage);
	}

	return TRUE;
}

/**
 *
 */
/*
BOOL CLogFileConversionFromNodes::Replaced(CString string,unsigned short usStart,unsigned short usStop)
{

	CString szMessage;
	szMessage.Format("%s ,bindingnrs: 0x%04x - 0x%04x",
					 string,
					 usStart,
					 usStop);						

	if ( Write( szMessage ) == FALSE) {
		return FALSE;
	}	

	if (this != m_pErrors)
	{
		m_pErrors->AddHeader(szMessage);
	}

	return TRUE;
}
*/


/**
 * 
 */
BOOL CLogFileConversionFromNodes::AddWarning(CString s)
{
	CString szWarning("\nWARNING: " + s);

	if (Write(szWarning) == FALSE) {
		return FALSE;
	}	

	if (this != getErrors()) {
		getErrors()->AddWarning(s);
	}
	return TRUE;
}
