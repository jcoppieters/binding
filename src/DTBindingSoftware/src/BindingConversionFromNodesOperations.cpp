// BindingConversionFromNodesOperations.cpp: implementation of the CBindingConversionFromNodesOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversionFromNodesOperations.h"

#include "BindingFiles.h"
#include "BindingEntries.h"
#include "BindingFilesOperations.h"
#include "BindingFileOperations.h"
#include "BindingUnitsOperations.h"
#include "BindingEntriesOperations.h"

#include "App\Appbindings\AppBindingFileConstruct\BindingFileConstructParserP.h"
#include "App\Appbindings\AppBindingFileConstruct\BindingFileConstruction.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperationsVerify.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"

#include "LogFileConversionFromNodes.h"
#include "NodeDatabaseOperations.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnitOperations.h"


// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingConversionFromNodesOperations::CBindingConversionFromNodesOperations(CBindingFiles* const pBindingFiles,
																			 CBindingEntries* const pBindingEntries) :
m_pBindingFiles( pBindingFiles ),
m_pBindingEntries( pBindingEntries),
m_pBindingFilesOperations( new CBindingFilesOperations( m_pBindingFiles )),
m_pBindingPropertyUnitOperations( new CBindingPropertyUnitOperations() ),
m_pBindingUnits( new CBindingUnits() ),
m_pLogFile( CLogFile::GetConversionFromNodes() )
{
	ASSERT( m_pBindingFiles );
	ASSERT( m_pBindingEntries );
	ASSERT( m_pBindingFilesOperations );
	ASSERT( m_pBindingPropertyUnitOperations );
	ASSERT( m_pBindingUnits );
	ASSERT( m_pLogFile );

	m_pBindingFileOperation = 0;

	m_BindingStrings.RemoveAll();

}

CBindingConversionFromNodesOperations::~CBindingConversionFromNodesOperations()
{
	if (m_pBindingFilesOperations != 0)
	{
		delete m_pBindingFilesOperations;
	}

	if (m_pBindingPropertyUnitOperations != 0)
	{
		delete m_pBindingPropertyUnitOperations;
	}

	if (m_pBindingFileOperation != 0)
	{
		delete m_pBindingFileOperation;
		m_pBindingFileOperation = 0;
	}
		
	if (m_pBindingUnits != 0)
	{
		delete m_pBindingUnits;
		m_pBindingUnits = 0;
	}

	m_BindingStrings.RemoveAll();
}

void CBindingConversionFromNodesOperations::Clear()
{
	ASSERT( m_pBindingPropertyUnitOperations );
	ASSERT( m_pBindingEntries );

	m_pBindingEntries->RemoveAll();
	m_pBindingPropertyUnitOperations->Clear();
	
}

void CBindingConversionFromNodesOperations::Init()
{
	ASSERT( m_pBindingFilesOperations );

	Clear();

	m_usManualBindingNr = m_pBindingFilesOperations->GetMax(0x9000,0xFFFF);
	m_usManualBindingNr++;

	m_usFirstAvailableBindingNr = m_pBindingFilesOperations->GetMax();
	m_usFirstAvailableBindingNr++;
}

int CBindingConversionFromNodesOperations::GetMaxBindingNr() const
{
	return m_usFirstAvailableBindingNr;
}

int CBindingConversionFromNodesOperations::GetMaxBindingFiles() const
{
	return (m_pBindingFiles->Count());
}

BOOL CBindingConversionFromNodesOperations::GetBindings(unsigned short usBindingNr)
{
	m_BindingStrings.RemoveAll();

	return (m_pBindingFilesOperations->FindStrings(usBindingNr,m_BindingStrings));												
}

BOOL CBindingConversionFromNodesOperations::Process()
{
	BOOL fResult = FALSE;

	if (m_pLogFile != 0) 
	{
		m_pLogFile->Process( &m_BindingStrings );
	}

	CBindingFileConstruction BindingFileConstruction(&m_BindingStrings);
	if (BindingFileConstruction.Construct() == TRUE)
	{
		TRACE("\nCBindingFileConstruction::Construct Success");

		CBindingEntryInfo *pBindingEntryInfo = 
			BindingFileConstruction.GetBindingEntryInfo();

		if (pBindingEntryInfo != NULL)
		{
			pBindingEntryInfo->GetBindingEntry()->SetID( 0 );	// !!!!!!!

			CBindingUnitsOperations BindingUnitsOperations(pBindingEntryInfo->GetBindingEntry()
						->GetColUnit());
					
			if (BindingUnitsOperations.UpgradeBindingUnitTypes() == TRUE)
			{
				if (Add(pBindingEntryInfo->GetBindingEntry()) == TRUE)
				{
					fResult = TRUE;

					if (m_pLogFile != 0) 
					{
						m_pLogFile->Success( pBindingEntryInfo->GetBindingEntry() );
					}
				}			
						
				ASSERT( fResult);
			}
		}
	}
	else
	{
		TRACE("\nCBindingFileConstruction::Construct Failed");	
	}

	if (fResult == FALSE)
	{
		if (m_pLogFile != 0) 
		{
			m_pLogFile->Failed( &m_BindingStrings );
		}

		// Must Replace Binding Nr to >9000 

		CStringArray StringsToReplace;
		
		if (ConvertToManual(m_BindingStrings, StringsToReplace) == TRUE)
		{
			const BOOL Result = m_pBindingFilesOperations->Replace( m_BindingStrings, StringsToReplace);
			ASSERT( Result );
		}	
	}

	return fResult;
}

BOOL CBindingConversionFromNodesOperations::Add(CBindingEntry* pBindingEntry)
{
	CBindingEntriesOperations BindingEntriesOperations(m_pBindingEntries);
	const unsigned short nID = BindingEntriesOperations.Add(pBindingEntry,
															pBindingEntry->GetBindingNr());

	ASSERT(nID);
	if (nID != 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBindingConversionFromNodesOperations::GetPropertyBindings(int iEntryNr)
{
	BOOL fResult = FALSE;

	ASSERT( m_pBindingUnits );

	m_BindingStrings.RemoveAll();
	m_pBindingUnits->RemoveAll();

	ASSERT( iEntryNr < m_pBindingFiles->Count() );

	if ( iEntryNr < m_pBindingFiles->Count())
	{	
		CBindingFile* const pBindingFile = 
			m_pBindingFiles->GetEntry(static_cast<BYTE>(iEntryNr));

		if (pBindingFile != 0)
		{								
			if (m_pBindingFileOperation != 0)
			{
				delete m_pBindingFileOperation;
			}
				
			try
			{
				m_pBindingFileOperation = new CBindingFileOperations(pBindingFile);
				if (m_pBindingFileOperation != 0)
				{
					fResult = m_pBindingFileOperation->FindPropertyStrings(m_BindingStrings);				
				}		

				if (fResult == TRUE)
				{
					if (m_pLogFile != 0)
					{
						CString string("P-Bindings found");
						m_pLogFile->Found(string, pBindingFile, &m_BindingStrings );
					}
				}

			}
			catch(CMemoryException *e)
			{
				if (m_pBindingFileOperation != 0) 
				{
					delete m_pBindingFileOperation;
					m_pBindingFileOperation = 0;
				}

				e->ReportError();
				e->Delete();
				return FALSE;
			}
		}
	}
	return fResult;
}

/*
BOOL CBindingConversionFromNodesOperations::VerifyUnitsUpgrade(int iEntryNr)
{
	BOOL fResult = TRUE;
	
	ASSERT( m_pBindingFiles );

	ASSERT( iEntryNr < m_pBindingFiles->Count() );

	CBindingFile* const pBindingFile = m_pBindingFiles->GetEntry(iEntryNr);
	if (pBindingFile != 0)
	{
		CStringArray strArray;
		CBindingFileOperations Operation(pBindingFile);
		const unsigned short usEntries = Operation.CheckMessageCode(strArray);
		if (usEntries != 0)
		{
			if (m_pLogFile != 0) 
			{
				m_pLogFile->ReportErrors("One or more bindingunits have wrong messagecode",
										 pBindingFile,
										 &strArray); 
			}
		}
		return TRUE;
	}
	return FALSE;
}
*/


BOOL CBindingConversionFromNodesOperations::VerifySyntaxsis(int iEntryNr)
{
	//BOOL fResult = TRUE;
	
	ASSERT( m_pBindingFiles );

	ASSERT( iEntryNr < m_pBindingFiles->Count() );

	CBindingFile* const pBindingFile = m_pBindingFiles->GetEntry(iEntryNr);
	if (pBindingFile != 0)
	{
		CStringArray strArray;
		CBindingFileOperations Operation(pBindingFile);
		const unsigned short usEntries = Operation.CheckSyntaxsis(strArray);
		if (usEntries != 0)
		{
			if (m_pLogFile != 0) 
			{
				m_pLogFile->ReportErrors("Syntaxsis errors",
										 pBindingFile,
										 &strArray); 
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingConversionFromNodesOperations::VerifyAddresses(int iEntryNr)
{
//	BOOL fResult = TRUE;
	
	ASSERT( m_pBindingFiles );

	ASSERT( iEntryNr < m_pBindingFiles->Count() );

	CBindingFile* const pBindingFile = m_pBindingFiles->GetEntry(iEntryNr);
	if (pBindingFile != 0)
	{
		CStringArray strArray;
		CBindingFileOperations Operation(pBindingFile);
		const unsigned short usEntries = Operation.CheckAddresses(strArray);
		if (usEntries != 0)
		{
			if (m_pLogFile != 0) 
			{
				m_pLogFile->ReportErrors("Nodes/units cannot be found",
										 pBindingFile,
										 &strArray); 
			}
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CBindingConversionFromNodesOperations::AdjustPBindings()
{
	const int nMaxEntries = m_BindingStrings.GetSize();
//	BOOL fResult = TRUE;
		
	if (nMaxEntries > 0)
	{
		TRACE("\nQHsmBindingConversionFromNodes : %d property Bindings",
			  nMaxEntries);
		
		ASSERT(m_pBindingFileOperation != 0);

		if (m_pBindingFileOperation != 0) 
		{
			const BOOL fResult = m_pBindingFileOperation->AdjustPBindings(m_BindingStrings);					

			if (m_pLogFile != 0)
			{
				if (fResult == TRUE)
				{			
					CString s("Replaced P-Bindings with bindingnr not equal to 0x0000 and < 0x9000\n");																	
					m_pLogFile->AddWarning(s);
				}
				else
				{
					CString s("Ok: No P-Bindings have been replaced\n");
					m_pLogFile->Add(s);
				}
			}		

			return TRUE;
		}

		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}	

BOOL CBindingConversionFromNodesOperations::GetInstructionBindings(int iEntryNr)
{
	BOOL fResult = FALSE;

	m_BindingStrings.RemoveAll();

	ASSERT( iEntryNr < m_pBindingFiles->Count() );

	if ( iEntryNr < m_pBindingFiles->Count())
	{	
		CBindingFile* const pBindingFile = 
			m_pBindingFiles->GetEntry(static_cast<BYTE>(iEntryNr));

		if (pBindingFile != 0)
		{								
			if (m_pBindingFileOperation != 0)
			{
				delete m_pBindingFileOperation;
			}
				
			try
			{
				m_pBindingFileOperation = new CBindingFileOperations(pBindingFile);
				if (m_pBindingFileOperation != 0)
				{
					fResult = m_pBindingFileOperation->FindInstructionStrings(m_BindingStrings);				
				}		

				if (fResult == TRUE)
				{
					if (m_pLogFile != 0)
					{
						CString string("I-Bindings found");
						m_pLogFile->Found(string, pBindingFile, &m_BindingStrings );
					}
				}

			}
			catch(CMemoryException *e)
			{
				if (m_pBindingFileOperation != 0) 
				{
					delete m_pBindingFileOperation;
					m_pBindingFileOperation = 0;
				}

				e->ReportError();
				e->Delete();
				return FALSE;
			}
		}
	}
	return fResult;

}

BOOL CBindingConversionFromNodesOperations::AdjustIBindings()
{
	const int nMaxEntries = m_BindingStrings.GetSize();
//	BOOL fResult = TRUE;
		
	if (nMaxEntries > 0)
	{
		TRACE("\nQHsmBindingConversionFromNodes : %d Instruction Bindings",
			  nMaxEntries);
		
		ASSERT(m_pBindingFileOperation != 0);

		if (m_pBindingFileOperation != 0) 
		{
			const unsigned short usCount = m_pBindingFileOperation->AdjustIBindings(m_BindingStrings,
																					m_usFirstAvailableBindingNr);

			if (m_pLogFile != 0)
			{
				if (usCount != 0)
				{			
					
					CString s;
					s.Format("Replaced I-Bindings with bindingnr 0x0000 from 0x%04x - 0x%04x\n",
							 m_usFirstAvailableBindingNr,
							 m_usFirstAvailableBindingNr+usCount);							
					
					m_pLogFile->AddWarning(s);
					

					//CString s("Warning: Replaced I-Bindings with bindingnr 0x0000!");
					//m_pLogFile->Replaced(s, m_usFirstAvailableBindingNr,
					//					 m_usFirstAvailableBindingNr + usCount);
				}
				else
				{
					CString s("Ok: No I-Bindings have been replaced\n");
					m_pLogFile->Add(s);
				}
			}		

			m_usFirstAvailableBindingNr += usCount;					
			return TRUE;
		}

		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

/**
 * BOOL CBindingConversionFromNodesOperations::ProcessPropertyBindings()
 *
 */
BOOL CBindingConversionFromNodesOperations::ProcessPropertyBindings()
{
	BOOL fResult = TRUE;
	const int nMaxEntries = m_BindingStrings.GetSize();
		
	if (nMaxEntries > 0)
	{
		TRACE("\nQHsmBindingConversionFromNodes : %d property Bindings",
			  nMaxEntries);

		if (GetBindingUnits(m_BindingStrings,m_pBindingUnits) == TRUE)
		{				
			if (m_pBindingUnits != 0)
			{
				CStringArray StringsToReplace;
				StringsToReplace.SetSize( nMaxEntries );
											
				CBindingUnitsOperations BindingUnitsOperations( m_pBindingUnits );
				if (BindingUnitsOperations.UpgradeBindingUnitTypes() == TRUE)
				{
					CBindingPropertyUnitOperations BindingPropertyUnitOperations;					

					const int nMaxBindingUnits = m_pBindingUnits->Count();	
					int nCountManual = 0;
					for (int i=0;i<nMaxBindingUnits;i++)
					{
						CString s(m_BindingStrings.GetAt(i));

						fResult = Add( StringsToReplace, i, s);
						ASSERT( fResult );						

						/** 						 
						 * Property bindings : Binding nummers met manueel binding adres 
						 * niet verwerken, Waarschuwing in Error log file...
						 */
						CBindingFileEntryOperations BindingFileEntryOperations(s);
						const unsigned short usBindingNr = BindingFileEntryOperations.GetBindingNr();
						if (usBindingNr >= 0x9000)
						{
							nCountManual++;	// P-Bindings with manual binding nr's !!!					
						}
						else
						{
							const BOOL fProcessed = UpgradePropertyUnit(m_pBindingUnits->Get(i));
							if (fProcessed == FALSE) 
							{
								if (m_pLogFile != 0) 
								{
									m_pLogFile->Failed(m_pBindingUnits->Get(i),
													   m_BindingStrings.GetAt(i));
								}
							}

							CString szOut;		
							if (ConvertPropertyString(s,szOut,fProcessed) == TRUE)
							{
								fResult = Add(StringsToReplace, i, szOut);
								ASSERT( fResult );						
							}					
						}
					} // for (int i=0;i<pBindingUnits->Count();i++)

					if (nCountManual > 0)
					{
						CString szMessage;
						if (m_pLogFile != 0)
						{
							szMessage.Format("%d P-Bindings have been skipped because they have a manual "
											 "assigned binding number",nCountManual);
							
							m_pLogFile->AddWarning(szMessage);
						}
					}

					ASSERT( m_pBindingUnits->Count() == StringsToReplace.GetSize() );

					fResult = Replace( m_BindingStrings, StringsToReplace );
					ASSERT( fResult );

				} // if (BindingPropertyUnitsOperations.Upgrade() == TRUE)										
		
			} // if (pBindingUnits != 0)

		}// if (BindingFileConstructParserP.Construct() == TRUE)					
	}
	return fResult;
}

BOOL CBindingConversionFromNodesOperations::Add(CStringArray& strArray,int index, CString newString)
{
	CBindingFileEntryOperationsVerify OperationsVerify(newString);
	if (OperationsVerify.IsValid())
	{
		strArray.SetAt(index,newString);
		return TRUE;
	}
	return FALSE;					
}

BOOL CBindingConversionFromNodesOperations::Replace(CStringArray &OldStrings,CStringArray &newStrings)
{
	ASSERT( OldStrings.GetSize() == newStrings.GetSize() );

	ASSERT( m_pBindingFileOperation );

	if (m_pBindingFileOperation != 0)
	{
		m_pBindingFileOperation->Replace( OldStrings, newStrings);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingConversionFromNodesOperations::ConvertPropertyString(CString szIn,CString &szOut,BOOL fProcessed)
{		
	CBindingFileEntryOperations BindingFileEntryOperations(szIn);					
	return (BindingFileEntryOperations.ConvertPropertyString(szOut,fProcessed));											
}


BOOL CBindingConversionFromNodesOperations::GetBindingUnits(CStringArray& strArray,CBindingUnits* _pBindingUnits)
{
	if (_pBindingUnits == 0) {
		ASSERT( _pBindingUnits );
		return FALSE;
	}

	_pBindingUnits->RemoveAll();

	CBindingFileConstructParserP BindingFileConstructParserP( &strArray );
	if (BindingFileConstructParserP.Construct() == TRUE)
	{	
		CBindingUnits* const pBindingUnits = 
			const_cast<CBindingUnits*>(BindingFileConstructParserP.GetBindingUnits());	
		
		if (pBindingUnits != 0)
		{
			*_pBindingUnits = *pBindingUnits;
			return TRUE;
		}
	}
	return FALSE;
}


/**
 * BOOL CBindingConversionFromNodesOperations::UpgradePropertyBindings()
 *
 */
BOOL CBindingConversionFromNodesOperations::UpgradePropertyBindings()
{
	BOOL fResult = FALSE;

	const int nMaxEntries = m_BindingStrings.GetSize();
		
	if (nMaxEntries > 0)
	{		
		CStringArray StringsToReplace;
		StringsToReplace.SetSize( nMaxEntries );

		for (int i=0;i<nMaxEntries;i++)
		{
			CString szOut;
			CString szIn(m_BindingStrings.GetAt(i));

			CBindingFileEntryOperationsVerify OperationsVerify(szIn);
			
			Add( StringsToReplace, i, szIn);

			if (OperationsVerify.Verify() == FALSE)
			{			
				if (OperationsVerify.Correct( szOut ) == TRUE)
				{
					Add( StringsToReplace, i, szOut);
				}
			}		
		}

		ASSERT( m_BindingStrings.GetSize() == StringsToReplace.GetSize() );

		fResult = Replace( m_BindingStrings, StringsToReplace );
		if (fResult == TRUE)
		{
			try
			{
				m_BindingStrings.RemoveAll();			
				m_BindingStrings.Copy(StringsToReplace);

				ASSERT( m_BindingStrings.GetSize() == StringsToReplace.GetSize() );

				fResult = TRUE;

				#ifdef _DEBUG

					for (int i=0;i<m_BindingStrings.GetSize();i++)
					{
						TRACE1("\n%s",m_BindingStrings.GetAt( i ));
					}

				#endif
			}
			catch(CMemoryException* e)
			{
				ASSERT( 0 );

				e->ReportError();
				e->Delete();			
				fResult = FALSE;
			}		
		}
		else
		{
			fResult = TRUE;	// ???
		}
	}

	return fResult;
}


BOOL CBindingConversionFromNodesOperations::UpgradePropertyUnit(const CBindingUnit* const pBindingUnit)
{
	CBindingPropertyUnitOperations BindingPropertyUnitOperations;
			
	const BOOL fProcessed = BindingPropertyUnitOperations.Update(pBindingUnit);
					
	if (fProcessed == TRUE)
	{				
		TRACE("\npBindingUnit Processed successfully ...");
	}	
	else
	{				
		TRACE("\npBindingUnit Processed failed ...");
	}							

	return fProcessed;
}

BOOL CBindingConversionFromNodesOperations::ConvertToManual(CStringArray& strArray,CStringArray& strManualArray)
{
	const int MaxStrings = strArray.GetSize();
	
	strManualArray.RemoveAll();

	for (int i=0;i<MaxStrings;i++)
	{
		CString szEntry(strArray.GetAt(i));
		CString szReturn;

		CBindingFileEntryOperations Operations(szEntry);
		if (Operations.ReplaceBindingNr(m_usManualBindingNr,szReturn) == FALSE)
		{
			//ASSERT( 0);
			return FALSE;
		}

		if (szReturn.IsEmpty() == TRUE)
		{
			//ASSERT( 0);
			return FALSE;
		}

		strManualArray.InsertAt(i, szReturn);
		
	} // for (int i=0;i<MaxStrings;i++)

	if ((m_pLogFile != 0) && (strManualArray.GetSize() != 0))
	{		
		m_pLogFile->Replaced(&strArray, &strManualArray);
	}

	m_usManualBindingNr++;

	return TRUE;
}

BOOL CBindingConversionFromNodesOperations::SetDefaultNames()
{	
	if (1)
	{
		/**
		 * Set Default names for binding entries ...
		 */
		CBindingEntriesOperations Operations(m_pBindingEntries);
		Operations.SetDefaultNames();
	}

	if (1)
	{
		/**
		 * Mark all names for units as overridden ...
		 */
		CNodeDatabaseOperations Operations(TMGetDocumentNodes());
		Operations.MarkAllNamesOveridden();
	}

	return TRUE;
}


