// BindingFileOperations.cpp: implementation of the CBindingFileOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileOperations.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntry.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParser.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParserFactory.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"

#include "BindingFile1.h"

#include "NodeDatabaseOperations.h"
#include "BindingUnitsOperations.h"

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

CBindingFileOperations::CBindingFileOperations(CBindingFile* pBindingFile) :
m_pBindingFile(pBindingFile)
{
	ASSERT(m_pBindingFile);
}

CBindingFileOperations::~CBindingFileOperations()
{
}


BOOL CBindingFileOperations::GetEntry(
	int EntryNr,
	CBindingFileEntryParser** pBindingEntryType)
{
	*pBindingEntryType = 0;

	if (EntryNr < m_pBindingFile->Count())
	{
		*pBindingEntryType = CBindingFileEntryParserFactory::Create(
			m_pBindingFile->GetString(EntryNr));		
			
		if (*pBindingEntryType != 0) 
		{
			return TRUE;							
		}
	}	
	return FALSE;
}

/***************************************************************//**
 * @changes	TM,v0562: Solved Bug : Memory leak.\n
 *			retval breekt de for-loop; vroeger werd rechtstreeks
 *			een return statement aangeroepen waardoor 
 *			'pBindingFileEntryType' niet werd gedelete.
 ******************************************************************/
BOOL CBindingFileOperations::HasStrings(unsigned short usBindingNr)
{
	BOOL retval = FALSE;

	const int nMax = m_pBindingFile->Count();
	for (int i=0; i<nMax; i++)
	{
		CBindingFileEntryParser* pBindingFileEntryType = 0;
		if (GetEntry(i,&pBindingFileEntryType) == TRUE)
		{		
			ASSERT( pBindingFileEntryType );

			if (pBindingFileEntryType->GetBindingNr() == usBindingNr) 
			{		
				retval = TRUE;				
			}

			delete pBindingFileEntryType;	// !!
			pBindingFileEntryType = 0;		

			if (retval == TRUE)	// since v0562.
			{
				break;	// break for loop.
			}
		}
	}
	return retval;
}


BOOL CBindingFileOperations::FindStrings(
	unsigned short usBindingNr,
	CStringArray& strArray)
{	
	try
	{
		const int nMax = m_pBindingFile->Count();
		int nCount = 0;

		for (int i=0;i<nMax;i++)
		{
			CBindingFileEntryParser* pBindingFileEntryType = 0;		
			if (GetEntry(i,&pBindingFileEntryType) == TRUE)
			{
				ASSERT( pBindingFileEntryType );

				// CHANGES_MVS_2008
/*
				if (pBindingFileEntryType == 0) 
				{
					throw new CException();
				}
*/		
				if (pBindingFileEntryType->GetBindingNr() == usBindingNr)
				{		
					strArray.Add(m_pBindingFile->GetString(i));
					nCount++;
				}		

				delete pBindingFileEntryType;	// !!
				pBindingFileEntryType = 0;			
			}
		}
	
		if (nCount != 0) 
		{
			return TRUE;
		}
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
	}

	return FALSE;
}

int CBindingFileOperations::find(
	BYTE bNodeAddress,			
	BYTE bUnitAddress,
	CStringArray& stringArray)
{
	int result = 0;

	stringArray.RemoveAll( );

	const int nMax = m_pBindingFile->Count( );
	for (int i=0;i<nMax;i++)
	{	
		CString szBindingString(m_pBindingFile->GetString(i));

		CBindingFileEntryParser* pBindingFileEntryType = 0;		
		if (GetEntry(i,&pBindingFileEntryType) == TRUE)
		{
			if (pBindingFileEntryType->IsTypeP( ))
			{			
				// Property bindings worden op een andere manier gecontroleerd.

				CBindingFileEntryParserP* const pBindingFileEntryTypeP =
					static_cast<CBindingFileEntryParserP*> (pBindingFileEntryType);

				const CBindingUnit* const pBindingUnit = pBindingFileEntryTypeP->GetForwardUnit();			
				if (pBindingUnit != 0 &&
					pBindingUnit->GetNodeAddress( ) == bNodeAddress &&
					pBindingUnit->GetUnitAddress( ) == bUnitAddress)
				{
					stringArray.Add(szBindingString);
					result++;	
				}				
			}
			else
			{
				// Alle andere binding types.
							
				CBindingUnits BindingUnits;
				CBindingFileEntryOperations BindingFileEntryOperations(szBindingString);

				if (BindingFileEntryOperations.GetBindingUnits(BindingUnits) == TRUE)
				{			
					CBindingUnits::Iterator it2(BindingUnits.CreateIterator());
					for (it2.Begin();!it2.IsDone();it2.Next())
					{
						CBindingUnit* const pBindingUnit = it2.CurrentItem();
						
						if (pBindingUnit->GetNodeAddress( ) == bNodeAddress &&
							pBindingUnit->GetUnitAddress( ) == bUnitAddress)
						{
							stringArray.Add(szBindingString);
							result++;
							break;
						}
					}
				}
			}
			delete pBindingFileEntryType;		// attention !!
			pBindingFileEntryType = 0;
		}
	}
	return result;
}

BOOL CBindingFileOperations::remove(
	CStringArray& stringArray)
{
	BOOL result = FALSE;
	const int nStringsToRemove = stringArray.GetSize( );
	for (int i=0;i<nStringsToRemove;i++)
	{
		result = m_pBindingFile->Remove( stringArray.GetAt( i ));			
		ASSERT( result == TRUE );
	}
	return result;
}


/**
 * Get Maximum binding nr between min & max
 * usMin = 1, usMax = 9000.
 */
unsigned short CBindingFileOperations::GetMax(
	unsigned short usMin,
	unsigned short usMax)
{
	unsigned short usEntry = usMin;
	const int nMax = m_pBindingFile->Count();

	for (int i=0;i<nMax;i++)
	{
		CBindingFileEntryParser* pBindingFileEntryType = 0;
		
		if (GetEntry(i,&pBindingFileEntryType) == TRUE)
		{
			ASSERT( pBindingFileEntryType );

			if ((pBindingFileEntryType->GetBindingNr() <= usMax) &&
				(pBindingFileEntryType->GetBindingNr() > usEntry))
			{
				usEntry = pBindingFileEntryType->GetBindingNr();
			}

			delete pBindingFileEntryType;
			pBindingFileEntryType = 0;			
		}
	}
	return usEntry;
}

BOOL CBindingFileOperations::FindInstructionStrings(
	CStringArray& strArray)
{
	BOOL fResult = FALSE;

	strArray.RemoveAll();

	const int nMax = m_pBindingFile->Count();
	int nCount = 0;

	for (int i=0;i<nMax;i++)
	{
		CBindingFileEntryParser* pBindingFileEntryType = 0;
		
		if (GetEntry(i,&pBindingFileEntryType) == TRUE)
		{		
			ASSERT( pBindingFileEntryType );

			TRACE1("\n%s",m_pBindingFile->GetString(i));

			if (pBindingFileEntryType->IsTypeI() == TRUE) 
			{
				strArray.Add(m_pBindingFile->GetString(i));
				
				nCount++;
				fResult = TRUE;
			}

			delete pBindingFileEntryType;
			pBindingFileEntryType = 0;			
		}
	}
	return fResult;
}

BOOL CBindingFileOperations::FindPropertyStrings(
	CStringArray& strArray)
{
	BOOL fResult = FALSE;

	strArray.RemoveAll();

	const int nMax = m_pBindingFile->Count();
	int nCount = 0;

	for (int i=0;i<nMax;i++)
	{
		CBindingFileEntryParser* pBindingFileEntryType = 0;
		
		if (GetEntry(i,&pBindingFileEntryType) == TRUE)
		{		
			ASSERT( pBindingFileEntryType );

			TRACE1("\n%s",m_pBindingFile->GetString(i));

			if (pBindingFileEntryType->IsTypeP() == TRUE) 
			{
				strArray.Add(m_pBindingFile->GetString(i));
				
				nCount++;
				fResult = TRUE;
			}

			delete pBindingFileEntryType;
			pBindingFileEntryType = 0;			
		}
	}
	return fResult;
}


BOOL CBindingFileOperations::Replace(
	const CStringArray &m_StringArray,
	const CStringArray& StringsToReplace)
{
	for (int i=0;i<m_StringArray.GetSize();i++)
	{	
		CString szOld(m_StringArray.GetAt(i));
		CString szNew(StringsToReplace.GetAt(i));

		if (szOld.Compare(szNew) != 0)
		{
			const int nMax = m_pBindingFile->Count();		
			for (int j=0;j<nMax;j++)
			{
				CString s(m_pBindingFile->GetString(j));
				if (s.IsEmpty() == FALSE)
				{				
					if (s.Compare(szOld) == 0)
					{
						if (m_pBindingFile->Replace(j,szNew) == FALSE)
						{
							ASSERT( 0 );
							return FALSE;
						}						
					}
				}
			}
		}
	}
	return TRUE;
}

unsigned short CBindingFileOperations::CheckSyntaxsis(
	CStringArray& strArray)
{
	const int nMax = m_pBindingFile->Count();	
	int nCount = 0;

	strArray.RemoveAll();

	for (int i=0;i<nMax;i++)
	{		
		CString s(m_pBindingFile->GetString(i));

		CBindingFileEntry BindingFileEntry(s);
		CBindingFileEntryParser* pBindingFileEntryType = BindingFileEntry.Parse();
		if (pBindingFileEntryType == 0)
		{	
			strArray.InsertAt(nCount, s);
			nCount++;
		}
	}	
	return nCount;
}

unsigned short CBindingFileOperations::CheckAddresses(
	CStringArray& strArray)
{
	const int nMax = m_pBindingFile->Count();	
	CNodeDatabaseOperations DatabaseOperations(TMGetDocumentNodes());

	int nCount = 0;

	strArray.RemoveAll();

	for (int i=0;i<nMax;i++)
	{		
		CString s(m_pBindingFile->GetString(i));

		CBindingUnits BindingUnits;
		CBindingFileEntryOperations BindingFileEntryOperations(s);
		if (BindingFileEntryOperations.GetBindingUnits(BindingUnits) == TRUE)
		{			
			CBindingUnits::Iterator it2(BindingUnits.CreateIterator());
			for (it2.Begin();!it2.IsDone();it2.Next())
			{
				CBindingUnit* const pBindingUnit = it2.CurrentItem();			
				CCanNode* pNode=0;
				STRUCT_UNIT* pUnit=0;

				ASSERT( pBindingUnit );

				const BOOL fResult = DatabaseOperations.Find(pBindingUnit,&pNode,&pUnit);
				if (fResult == FALSE)
				{
					strArray.InsertAt(nCount,s);
					nCount++;
				}
			}		
		}
	}
	return nCount;
}

BOOL CBindingFileOperations::AdjustPBindings(const CStringArray &m_StringArray)
{
	CStringArray StringsToReplace;
	unsigned short usCount = 0;

	const int nMaxEntries = m_StringArray.GetSize();
	for (int i=0;i<nMaxEntries;i++)
	{	
		CString s(m_StringArray.GetAt(i));
		CString szOut(s);

		CBindingFileEntry BindingFileEntry(s);
		CBindingFileEntryParser* pBindingFileEntryType = BindingFileEntry.Parse();
		if (pBindingFileEntryType != 0)
		{
			if (pBindingFileEntryType->IsTypeP() == TRUE)
			{
				const unsigned short usBindingNr = pBindingFileEntryType->GetBindingNr();
				if (usBindingNr != 0x0000 && usBindingNr < 0x9000) 
				{
					CBindingFileEntryOperations BindingFileEntryOperations(s);
					const BOOL fResult = BindingFileEntryOperations.ReplaceBindingNr(0x0000,szOut);
					ASSERT(fResult);
					
					usCount++;
				}
			}
			else
			{
				ASSERT(0);
			}
		}	

		StringsToReplace.InsertAt(i,szOut);
	}

	if (nMaxEntries > 0 && usCount > 0)
	{
		ASSERT(nMaxEntries == StringsToReplace.GetSize());

		const BOOL fResult = Replace(m_StringArray,StringsToReplace);	
		return fResult;		
	}
	return FALSE;
}

unsigned short CBindingFileOperations::AdjustIBindings(const CStringArray &m_StringArray,unsigned short usNextBindingNr)
{
	CStringArray StringsToReplace;
	unsigned short usCount = 0;
	const int nMaxEntries = m_StringArray.GetSize();

	for (int i=0;i<nMaxEntries;i++)
	{	
		CString s(m_StringArray.GetAt(i));
		CString szOut(s);

		CBindingFileEntry BindingFileEntry(s);
		CBindingFileEntryParser* pBindingFileEntryType = BindingFileEntry.Parse();
		if (pBindingFileEntryType != 0)
		{
			if (pBindingFileEntryType->IsTypeI() == TRUE)
			{
				const unsigned short usBindingNr = pBindingFileEntryType->GetBindingNr();
				if (usBindingNr == 0x0000)
				{
					CBindingFileEntryOperations BindingFileEntryOperations(s);
					const BOOL fResult = BindingFileEntryOperations.ReplaceBindingNr(usNextBindingNr + usCount,
																					 szOut);
					ASSERT(fResult);
					usCount++;
				}
			}
			else
			{
				ASSERT(0);
			}
		}	

		StringsToReplace.InsertAt(i,szOut);
	}

	if (nMaxEntries > 0)
	{
		ASSERT(nMaxEntries == StringsToReplace.GetSize());
		
		const BOOL fResult = Replace(m_StringArray,StringsToReplace);
		ASSERT(fResult);
	}
	return usCount;
}

// Sedert 0x0902:
// Oppassen: Beide strings worden genormaliseerd zodat ze hetzelfde formaat hebben...

BOOL CBindingFileOperations::findString(const CString& refString)
{
	BOOL fResult=FALSE;
	const int count = m_pBindingFile->Count();
	int i=0;

	for(i=0;i<count;++i)
	{
		if(TRUE==CBindingFileEntryOperations::compare(
				m_pBindingFile->GetString(i),
				refString))
		{
			fResult=TRUE;
			break;			
		}
	}

	if(FALSE==fResult)
	{
		TRACE("%s",refString);
	}
	return fResult;
}