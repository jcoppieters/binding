// BindingFileConstructParser.cpp: implementation of the CBindingFileConstructParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParser.h"

#include "BindingLinksOperations.h"
#include "BindingUnitsOperations.h"
#include "BindingUnitFactory.h"

#include "BindingFileConstructParserN_H.h"
#include "BindingEntryOperations.h"
#include "LogFile.h"

#include "DebugMacros.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructParser::CBindingFileConstructParser(CStringArray* pArrayBindingStrings) :
CBindingFileConstruct( pArrayBindingStrings),
m_pLogFile( CLogFile::GetConversionConstruct())
{
	ASSERT( m_pLogFile );
}

CBindingFileConstructParser::~CBindingFileConstructParser()
{

}

int CBindingFileConstructParser::GetBindingTypeEntry(BOOL (CBindingFileEntryParser::*Action)() const,
													 int index)
{
	int indexFound = -1;

	if (Action != NULL)
	{	
		const int nMaxEntries = m_pArrayBindingStrings->GetSize();
		int nCount = 0;	
			
		for (int i=0;i<nMaxEntries;i++)
		{	
			CBindingFileEntry BindingFileEntry(m_pArrayBindingStrings->GetAt(i));

			CBindingFileEntryParser* const pBindingFileEntryType = BindingFileEntry.Parse();
			if (pBindingFileEntryType != NULL)
			{
				if ((pBindingFileEntryType->*Action)() == TRUE)
				{			
					if (nCount++ == index)
					{
						indexFound = i;
						break;
					}
				}
			}
		}

		#ifdef _DEBUG_BINDINGFILE_CONSTRUCT_PARSER
	
		if(1)
		{
			CString szDebug;
			szDebug.Format("index = %d,nCount = %d, indexFound = %d",index,nCount,indexFound);
			TRACE1("\nGetBindingTypeEntry:%s",szDebug);
		}

		#endif
	}
	return indexFound;
}

BOOL CBindingFileConstructParser::Construct(CBindingEntryInfo* pBindingEntryInfo)
{
	ASSERT(pBindingEntryInfo);

	#define MAX_CONSTRUCT_ACTIONS	10
	typedef BOOL (CBindingFileConstructParser::*ConstructActions)(void);
	ConstructActions Actions[MAX_CONSTRUCT_ACTIONS];

	Actions[0] = &CBindingFileConstructParser::IsTypeOf;			// CHANGES_MVS_2008
	Actions[1] = &CBindingFileConstructParser::Init;				// CHANGES_MVS_2008
	Actions[2] = &CBindingFileConstructParser::CheckPreConditions;	// CHANGES_MVS_2008
	Actions[3] = &CBindingFileConstructParser::AddUnits;			// CHANGES_MVS_2008
	Actions[4] = &CBindingFileConstructParser::BuildEvalString;		// CHANGES_MVS_2008
	Actions[5] = &CBindingFileConstructParser::AddOperators;		// CHANGES_MVS_2008
	Actions[6] = &CBindingFileConstructParser::AddLinks;			// CHANGES_MVS_2008
	Actions[7] = &CBindingFileConstructParser::BuildNextConstruct;	// CHANGES_MVS_2008
	Actions[8] = &CBindingFileConstructParser::AppendNextConstruct;	// CHANGES_MVS_2008
	Actions[9] = &CBindingFileConstructParser::BuildBindingEntryInfo;	// CHANGES_MVS_2008

	m_pBindingEntryInfo = pBindingEntryInfo;

	BOOL fResult;

	for (int i=0;i<MAX_CONSTRUCT_ACTIONS;i++)
	{
		 fResult = (this->*Actions[i])();
		 
		 if (fResult == FALSE) 
		 {
			 //ASSERT(fResult);
			 break;
		 }
	}

	Destroy();

	return fResult;
}

BOOL CBindingFileConstructParser::AddBindingLink(CBindingUnit* pIn,CBindingUnit* pOut,BOOL fInverse)
{
	ASSERT(m_pBindingEntryInfo);

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		CBindingLinks* const pBindingLinks = pBindingEntry->GetColLink();
		CBindingLinksOperations BindingLinksOperations(pBindingLinks);
		return (BindingLinksOperations.Add(pIn,pOut,fInverse));		
	}
	return FALSE;
}


BOOL CBindingFileConstructParser::AddBindingUnit(CBindingUnit* pNewUnit)
{
	if (pNewUnit != NULL)
	{
		ASSERT(m_pBindingEntryInfo);

		CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
		if (pBindingEntry != NULL)
		{
			CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();
			if (pBindingUnits != NULL)
			{
				/**
				 * Base Class CBindingUnit --> Base class CBindingUnit.
				 */			
				ASSERT(pNewUnit->GetBindingUnitType() == 0);

				CBindingUnit* const pNew = CBindingUnitFactory::Clone(pNewUnit);
					
				if (pNew != NULL)
				{			
					ASSERT(pNew->GetBindingUnitType() == 0);

					pNew->SetID(0); // !!!!!!				
					const unsigned short nID = pBindingUnits->Add(pNew);
					
					ASSERT(nID);
					ASSERT(nID == pNew->GetID());
					
					if (nID != 0)
					{
						return TRUE;
					}
				}		
			}
		}
	}
	return FALSE;
}

BOOL CBindingFileConstructParser::FindBindingUnit(CBindingUnit** pFindUnit)
{
	if (pFindUnit != NULL)
	{
		ASSERT(m_pBindingEntryInfo);

		CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
		if (pBindingEntry != NULL)
		{
			CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();
			ASSERT(pBindingUnits);

			CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);

			CBindingUnit* pIn = *pFindUnit;
			*pFindUnit = NULL;

			return (BindingUnitsOperations.FindUnit(pIn,pFindUnit));		
		}
	}
	return FALSE;
}

BOOL CBindingFileConstructParser::IsTypeOf()
{
	return FALSE;
}

BOOL CBindingFileConstructParser::Init()
{
	m_pForwardUnit = NULL;
	m_pReturnUnit = NULL;

	return TRUE;
}
BOOL CBindingFileConstructParser::CheckPreConditions()
{
	return FALSE;
}
BOOL CBindingFileConstructParser::AddUnits()
{
	return FALSE;
}

BOOL CBindingFileConstructParser::AddOperators()
{
	return TRUE;
}
BOOL CBindingFileConstructParser::AddLinks()
{
	return FALSE;
}
BOOL CBindingFileConstructParser::BuildEvalString()
{
	return FALSE;
}
BOOL CBindingFileConstructParser::BuildNextConstruct()
{
	return TRUE;
}
BOOL CBindingFileConstructParser::AppendNextConstruct()
{
	return TRUE;
}
BOOL CBindingFileConstructParser::BuildBindingEntryInfo()
{
	return FALSE;
}
BOOL CBindingFileConstructParser::Destroy()
{
	return TRUE;
}

BOOL CBindingFileConstructParser::GetBindingFlags(CBindingFileEntryParser* const pBindingFileEntryParser,
												  BYTE *bBindingFlags)
{
	if (pBindingFileEntryParser == 0)
	{
		ASSERT( pBindingFileEntryParser );
		return FALSE;
	}

	*bBindingFlags = pBindingFileEntryParser->GetBindingFlags();
	return TRUE;
}


BOOL CBindingFileConstructParser::GetBindingNr(CBindingFileEntryParser* const pBindingFileEntryParser,
											   unsigned short* usBindingNr)
{
	if (pBindingFileEntryParser == 0)
	{
		ASSERT( pBindingFileEntryParser );
		return FALSE;
	}

	*usBindingNr = pBindingFileEntryParser->GetBindingNr();
	return TRUE;
}


BOOL CBindingFileConstructParser::GetForwardUnit(CBindingFileEntryParser* const pBindingFileEntryParser,
												 CBindingUnit* pForwardUnit)
{
	if (pBindingFileEntryParser == 0) 
	{
		ASSERT( pBindingFileEntryParser );
		return FALSE;
	}
	
	const CBindingUnit* const pUnit = 
		pBindingFileEntryParser->GetForwardUnit();

	if (pUnit != 0)
	{	
		*pForwardUnit = *pUnit;
		return TRUE;		
	}			
	return FALSE;
}

BOOL CBindingFileConstructParser::_BuildNextConstruct(CBindingFileConstructParserN_H* const pBindingFileConstructParser)
{
	BOOL fResult = FALSE;

	if (pBindingFileConstructParser == 0)
	{
		ASSERT( pBindingFileConstructParser );
		return FALSE;
	}

	if (pBindingFileConstructParser->Construct() == TRUE)
	{
		const CBindingEntry* const pBindingEntryNext = 
			pBindingFileConstructParser->GetBindingEntry();

		ASSERT(pBindingEntryNext);

		CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
		if (pBindingEntry != NULL)
		{
			fResult = TRUE;

			#ifdef _DEBUG
			if(1)	// All units must be type io Output !!!
			{
				const int nMax = pBindingEntryNext->GetColUnit()->Count();

				CString t;
				t.Format("CBindingEntry 'pBindingEntryNext' holds:%d BindingUnits",nMax);
				TRACE1("\n%s",t);

				for (int i=0;i<nMax;i++)
				{
					const CBindingUnit *const pBindingUnit = 
						pBindingEntryNext->GetColUnit()->Get(i);

					ASSERT(pBindingUnit);

					if (pBindingUnit != NULL)
					{
						CString s;
						s.Format("BindingUnit ID:%d,NodeAddress:0x%02x,UnitAddress:0x%02x,bIOType:%d",
								 pBindingUnit->GetID(),
								 pBindingUnit->GetNodeAddress(),
								 pBindingUnit->GetUnitAddress(),
								 pBindingUnit->GetInOutType());
						
						TRACE("\n%s",s);
					}
				}
			}
			#endif
		}
	}
	return fResult;
}

BOOL CBindingFileConstructParser::_AppendNextConstruct(CBindingFileConstructParserN_H *const pBindingFileConstructParser)
{
	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{		
		CBindingEntry* const pBindingEntryNext = pBindingFileConstructParser->GetBindingEntry();
		if (pBindingEntryNext != NULL)
		{
			CBindingEntryOperations BindingEntryOperations(pBindingEntry);
			if (BindingEntryOperations.Import(pBindingEntryNext) == TRUE)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}