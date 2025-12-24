// BindingFileConstructParserBGN.cpp: implementation of the CBindingFileConstructParserBGN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParserBGN.h"

#include "BindingEntryOperations.h"
#include "BindingFileConstructParserB_H.h"
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

CBindingFileConstructParserBGN::CBindingFileConstructParserBGN(CStringArray* pArrayBindingStrings) :
CBindingFileConstructParserBG(pArrayBindingStrings)
{
	m_pBindingFileConstructN = 0;
	m_pBindingFileEntryParserGN = 0;
}

CBindingFileConstructParserBGN::~CBindingFileConstructParserBGN()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
BOOL CBindingFileConstructParserBGN::GetNextUnit(CBindingUnit* pNextUnit)
{
	BOOL fResult = FALSE;

	if (m_pBindingFileEntryParserGN == 0) {
		ASSERT(m_pBindingFileEntryParserGN);
		return FALSE;
	}

	const CBindingUnit* pUnit = NULL;	
	if ((pUnit = m_pBindingFileEntryParserGN->GetNextUnit()) != 0)
	{
		*pNextUnit = *pUnit;
		fResult = TRUE;	
	}
	return fResult;
}


/**
 *
 */
CBindingUnit* CBindingFileConstructParserBGN::AddReturnUnit()
{
	//BOOL fResult = FALSE;

	CBindingUnit  ReturnUnit;
	CBindingUnit* pReturnUnit = NULL;

	CBindingFileConstructParserB_H BindingFileConstructParserB(m_pArrayBindingStrings);
	if (BindingFileConstructParserB.GetReturnUnit(&ReturnUnit) == TRUE)
	{
		pReturnUnit = &ReturnUnit;	

		if (AddBindingUnit(pReturnUnit) == TRUE)
		{
			if (FindBindingUnit(&pReturnUnit) == TRUE)
			{
				return (pReturnUnit);
			}
		}	
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

/**
 *	Array has only B,GN,N & NN Strings !!!
 */
BOOL CBindingFileConstructParserBGN::IsTypeOf()
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
					pBindingFileEntryType->IsTypeGN() == TRUE ||
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
 * BOOL CBindingFileConstructParserBGN::Init()
 * BindingFileEntryParserFactory
 */
BOOL CBindingFileConstructParserBGN::Init()
{
	BOOL fResult = FALSE;

	m_pForwardUnit = NULL;
	m_pReturnUnit = NULL;

	ASSERT( m_pBindingFileConstructN == 0);
	ASSERT( m_pBindingFileEntryParserGN == 0);

	try
	{
		if (m_pBindingFileConstructN == 0)
		{
			m_pBindingFileConstructN = new CBindingFileConstructParserN_H(m_pArrayBindingStrings);			
		
			if (m_pBindingFileEntryParserGN == 0)
			{
				const int indexGNBinding = 
					GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeGN);

				if (indexGNBinding != -1)
				{
					if ((m_pBindingFileEntryParserGN = CBindingFileEntryParserFactory::Get_TypeGN(
						m_pArrayBindingStrings->GetAt(indexGNBinding))) != 0)
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
	ASSERT( m_pBindingFileEntryParserGN != 0);

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBGN::CheckPreConditions()
 */
BOOL CBindingFileConstructParserBGN::CheckPreConditions()
{
	ASSERT(IsTypeOf() == TRUE);
	ASSERT(m_pArrayBindingStrings);

	const int nMaxEntries = m_pArrayBindingStrings->GetSize();

	if (nMaxEntries < 3) 
	{	
		TRACE("\nB/G/N Construction must have at least 3 entries in array...");
		ASSERT(0);
		return FALSE;
	}

	return TRUE;

}

/**
 * BOOL CBindingFileConstructParserBGN::Destroy()
 */
BOOL CBindingFileConstructParserBGN::Destroy()
{
	if (m_pBindingFileConstructN != 0)
	{
		delete m_pBindingFileConstructN;
		m_pBindingFileConstructN = 0;
	}

	if (m_pBindingFileEntryParserGN != 0)
	{
		delete m_pBindingFileEntryParserGN;
		m_pBindingFileEntryParserGN = 0;
	}
	return TRUE;
}


/**
 *	BOOL CBindingFileConstructParserBGN::AddUnits()
 */
BOOL CBindingFileConstructParserBGN::AddUnits()
{
	BOOL fResult = FALSE;

	m_pReturnUnit = AddReturnUnit();
	m_pForwardUnit = AddForwardUnit( m_pBindingFileEntryParserGN );

	if (m_pForwardUnit != NULL && m_pReturnUnit != NULL)
	{	
		fResult = TRUE;	
	}

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBGN::BuildNextConstruct()
 */
BOOL CBindingFileConstructParserBGN::BuildNextConstruct()
{
	ASSERT(m_pArrayBindingStrings);
	ASSERT(m_pBindingFileConstructN);

	return ( _BuildNextConstruct(m_pBindingFileConstructN) );
}

/**
 * BOOL CBindingFileConstructBGN::AppendNextConstruct()
 */
BOOL CBindingFileConstructParserBGN::AppendNextConstruct()
{
	ASSERT(m_pArrayBindingStrings);
	ASSERT(m_pBindingFileConstructN);

	return ( _AppendNextConstruct( m_pBindingFileConstructN ) );
}

/**
 * BOOL CBindingFileConstructParserBGN::BuildBindingEntryInfo()
 */
BOOL CBindingFileConstructParserBGN::BuildBindingEntryInfo()
{
	BOOL fResult = FALSE;

	ASSERT(m_pBindingEntryInfo);

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		m_pBindingEntryInfo->Set_NInputs(1);

		CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();
		if (pBindingUnits != NULL)
		{
			const int nOutputs = pBindingUnits->Count() - 1;
			ASSERT(nOutputs>=2);

			m_pBindingEntryInfo->Set_NOutputs(nOutputs);
			m_pBindingEntryInfo->Set_Type(TYPE_BGN_BINDING);

			unsigned short usBindingNr = 0;
			if (GetBindingNr(m_pBindingFileEntryParserGN,
							 &usBindingNr) 
							 == TRUE)
			{
				CString szName;
				szName.Format("B/G/N - 0x%04x",usBindingNr);

				pBindingEntry->SetName(szName);
				pBindingEntry->SetBindingNr(usBindingNr);

				BYTE bBindingFlags;
				if (GetBindingFlags(m_pBindingFileEntryParserGN,
									&bBindingFlags) 
									== TRUE)
				{
					ASSERT(bBindingFlags <= BINDINGFLAGS_STATUS_PREPULS);

					pBindingEntry->SetBindingReturnFlags(bBindingFlags);
					fResult = TRUE;
				}			
			}		
		}	
	}
	ASSERT(fResult);
	return fResult;
}



BOOL CBindingFileConstructParserBGN::ConstructBindingInfo()
{
	ASSERT(m_pBindingEntryInfo);

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		m_pBindingEntryInfo->Set_NInputs(1);

		CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();
		if (pBindingUnits != NULL)
		{
			const int nOutputs = pBindingUnits->Count() - 1;
			ASSERT(nOutputs>=2);
			m_pBindingEntryInfo->Set_NOutputs(nOutputs);

			m_pBindingEntryInfo->Set_Type(TYPE_BGN_BINDING);

			unsigned short usBindingNr = 0;
			if (GetBindingNr(m_pBindingFileEntryParserGN,
							 &usBindingNr) 
							 == TRUE)
			{
				CString szName;
				szName.Format("B/G/N - 0x%04x",usBindingNr);

				pBindingEntry->SetName(szName);
				pBindingEntry->SetBindingNr(usBindingNr);

				BYTE bBindingFlags;
				if (GetBindingFlags(m_pBindingFileEntryParserGN,
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
		ASSERT(0);
	}
	return FALSE;
}


/**
 * Compiler Bug ????
 * Cannot use Polymorphisme therefore '_'
 */
BOOL CBindingFileConstructParserBGN::_ConstructBindingEvaluation(CBindingUnit* pForwardUnit,
														   CBindingUnit* pReturnUnit,
														   CBindingUnit* pNextUnit)
{
	ASSERT(m_pBindingEntryInfo);
	
	if (ConstructBindingEvaluation(pForwardUnit,pReturnUnit) == TRUE)
	{
		CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
		if (pBindingEntry != NULL)
		{
			CBindingEvaluationString* const pBindingEvaluationString = 
				pBindingEntry->GetEvaluationString();
		
			if (pBindingEvaluationString != NULL)
			{
				pBindingEvaluationString->Add('>');
				pBindingEvaluationString->AddUnit(pNextUnit->GetID());

				return TRUE;
			}
		}
	}
	return FALSE;
}

	


/*
BOOL CBindingFileConstructParserBGN::GetBindingFlags(BYTE* bBindingFlags)
{
	if (m_pBindingFileEntryParserGN == 0) {
		ASSERT(m_pBindingFileEntryParserGN);
		return FALSE;
	}

	*bBindingFlags = m_pBindingFileEntryParserGN->GetBindingFlags();
	return TRUE;
}
*/

/**
 * 
 */
/*
BOOL CBindingFileConstructParserBGN::GetForwardUnit(CBindingUnit* pForwardUnit)
{
	BOOL fResult = FALSE;

	if (m_pBindingFileEntryParserGN == 0) {
		ASSERT(m_pBindingFileEntryParserGN);
		return FALSE;
	}

	CBindingUnit* pUnit = NULL;			
	if ((pUnit = m_pBindingFileEntryParserGN->GetForwardUnit()) != 0)
	{
		*pForwardUnit = *pUnit;
		fResult = TRUE;
	}
	return fResult;
}
*/

/**
 *
 */
/*
CBindingUnit* CBindingFileConstructParserBGN::AddForwardUnit()
{
	BOOL fResult = FALSE;

	CBindingUnit  ForwardUnit;
	CBindingUnit* pForwardUnit = NULL;

	if (GetForwardUnit(m_pBindingFileEntryParserGN,
					   &ForwardUnit) 
					   == TRUE)
	{
		pForwardUnit = &ForwardUnit;

		if (AddBindingUnit(pForwardUnit) == TRUE)
		{
			if (FindBindingUnit(&pForwardUnit) == TRUE)
			{
				return (pForwardUnit);
			}
		}	
	}

	return 0;
}
*/

/*
BOOL CBindingFileConstructParserBGN::BuildNextConstruct()
{
	ASSERT(m_pArrayBindingStrings);
	ASSERT(m_pBindingFileConstructN);
	BOOL fResult = FALSE;

	if (m_pBindingFileConstructN->Construct() == TRUE)
	{
		CBindingEntry* const pBindingEntryNext = m_pBindingFileConstructN->GetBindingEntry();
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
					CBindingUnit *const pBindingUnit = 
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

 BOOL CBindingFileConstructParserBGN::AppendNextConstruct()
{
	ASSERT(m_pArrayBindingStrings);
	ASSERT(m_pBindingFileConstructN);

	BOOL fResult = FALSE;

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{		
		CBindingEntry* const pBindingEntryNext = m_pBindingFileConstructN->GetBindingEntry();
		if (pBindingEntryNext != NULL)
		{
			CBindingEntryOperations BindingEntryOperations(pBindingEntry);
			if (BindingEntryOperations.Import(pBindingEntryNext) == TRUE)
			{
				fResult = TRUE;
			}
		}
	}

	return fResult;
}

*/