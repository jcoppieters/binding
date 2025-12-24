// BindingFileConstructParserBCN.cpp: implementation of the CBindingFileConstructParserBCN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParserBCN.h"

#include "BindingFileConstructParserN_H.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParserFactory.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileConstructParserBCN::CBindingFileConstructParserBCN(CStringArray* pArrayBindingStrings) :
CBindingFileConstructParserBC(pArrayBindingStrings)
{
	m_pBindingFileEntryParserCN = 0;
	m_pBindingFileConstructN = 0;
}

CBindingFileConstructParserBCN::~CBindingFileConstructParserBCN()
{
	Destroy();
}


/**
 *	Array has only B,CN,N && NN Strings !!!
 */
BOOL CBindingFileConstructParserBCN::IsTypeOf()
{
	BOOL fResult = FALSE;

	if (m_pArrayBindingStrings != NULL)
	{
		const int nMaxEntries = m_pArrayBindingStrings->GetSize();

		for (int i=0;i<nMaxEntries;i++)
		{
			CBindingFileEntry BindingFileEntry(m_pArrayBindingStrings->GetAt(i));
			CBindingFileEntryParser* const pBindingFileEntryType = BindingFileEntry.Parse();
			if (pBindingFileEntryType != NULL)
			{
				if (pBindingFileEntryType->IsTypeB() == TRUE || 
					pBindingFileEntryType->IsTypeCN() == TRUE ||
					pBindingFileEntryType->IsTypeN() == TRUE ||
					pBindingFileEntryType->IsTypeNN() == TRUE)
				{
					fResult = TRUE;				
				}
				else
				{
					fResult = FALSE;
					break;
				}
			}
		}
	}
	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBCN::Init()
 *
 */
BOOL CBindingFileConstructParserBCN::Init()
{
	BOOL fResult = FALSE;

	m_pForwardUnit = NULL;
	m_pReturnUnit = NULL;

	ASSERT( m_pBindingFileConstructN == 0);
	ASSERT( m_pBindingFileEntryParserCN == 0);

	try
	{
		if (m_pBindingFileConstructN == 0)
		{
			m_pBindingFileConstructN = new CBindingFileConstructParserN_H(m_pArrayBindingStrings);			
		
			if (m_pBindingFileEntryParserCN == 0)
			{
				const int indexCNBinding = 
					GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeCN);

				if (indexCNBinding != -1)
				{
					if ((m_pBindingFileEntryParserCN = CBindingFileEntryParserFactory::Get_TypeCN(
							m_pArrayBindingStrings->GetAt(indexCNBinding))) != 0)
					{
						fResult = TRUE;
					}
				}
			}
		}
	}
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		fResult = FALSE;
	}

	ASSERT( m_pBindingFileConstructN != 0);
	ASSERT( m_pBindingFileEntryParserCN != 0);

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBCN::CheckPreConditions()
 *
 */
BOOL CBindingFileConstructParserBCN::CheckPreConditions()
{
	ASSERT(IsTypeOf() == TRUE);
	ASSERT(m_pArrayBindingStrings);

	const int nMaxEntries = m_pArrayBindingStrings->GetSize();

	if (nMaxEntries < 3) 
	{	
		TRACE("\nB/C/N Construction must have at least 3 entries in array...");
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}



/**
 * BOOL CBindingFileConstructParserBCN::AddUnits()
 *
 */
BOOL CBindingFileConstructParserBCN::AddUnits()
{
	BOOL fResult = FALSE;

	if (AddReturnUnits() == TRUE)
	{
		m_pForwardUnit = AddForwardUnit(m_pBindingFileEntryParserCN);
		if (m_pForwardUnit != 0)
		{
			fResult = TRUE;
		}
	}

	ASSERT(fResult);
	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBCN::AddOperators()
 *
 */
BOOL CBindingFileConstructParserBCN::AddOperators()
{
	return ( _AddOperators( m_pBindingFileEntryParserCN ) );
}

/**
 * BOOL CBindingFileConstructParserBCN::AddLinks()
 *
 */
/*
BOOL CBindingFileConstructParserBCN::AddLinks()
{
	return FALSE;
}
*/

/**
 * BOOL CBindingFileConstructParserBCN::BuildEvalString()
 *
 */
BOOL CBindingFileConstructParserBCN::BuildEvalString()
{
	return ( _BuildEvalString(m_pBindingFileEntryParserCN) );
}

/**
 * BOOL CBindingFileConstructParserBCN::BuildNextConstruct()
 *
 */
BOOL CBindingFileConstructParserBCN::BuildNextConstruct()
{
	ASSERT(m_pArrayBindingStrings);
	ASSERT(m_pBindingFileConstructN);

	return ( _BuildNextConstruct(m_pBindingFileConstructN) );
}

/**
 * BOOL CBindingFileConstructParserBCN::AppendNextConstruct()
 *
 */
BOOL CBindingFileConstructParserBCN::AppendNextConstruct()
{
	ASSERT(m_pArrayBindingStrings);
	ASSERT(m_pBindingFileConstructN);

	return ( _AppendNextConstruct( m_pBindingFileConstructN ) );
}

/**
 * BOOL CBindingFileConstructParserBCN::BuildBindingEntryInfo()
 *
 */
BOOL CBindingFileConstructParserBCN::BuildBindingEntryInfo()
{
		BOOL fResult = FALSE;

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		ASSERT( pBindingEntry->GetEvaluationString() );

		if ((fResult = pBindingEntry->GetEvaluationString()
			->Parse()) == FALSE)
		{
			ASSERT(fResult);
			return fResult;
		}

		unsigned short usBindingNr = 0;
		if (GetBindingNr(m_pBindingFileEntryParserCN,
						&usBindingNr) 
						== TRUE)
		{
			CString szName;
			szName.Format("B/C/N - 0x%04x",usBindingNr);

			pBindingEntry->SetName(szName);
			pBindingEntry->SetBindingNr(usBindingNr);

			BYTE bBindingFlags;
			if (GetBindingFlags(m_pBindingFileEntryParserCN,
								&bBindingFlags) 
								== TRUE)
			{
				ASSERT(bBindingFlags <= BINDINGFLAGS_STATUS_PREPULS);

				pBindingEntry->SetBindingReturnFlags(bBindingFlags);

				return TRUE;
			}
			ASSERT(0);
		}
		ASSERT(0);
	}
	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBCN::Destroy()
 *
 */
BOOL CBindingFileConstructParserBCN::Destroy()
{
	if (m_pBindingFileConstructN != 0)
	{
		delete m_pBindingFileConstructN;
		m_pBindingFileConstructN = 0;
	}

	if (m_pBindingFileEntryParserCN != 0)
	{
		delete m_pBindingFileEntryParserCN;
		m_pBindingFileEntryParserCN = 0;
	}

	return TRUE;
}