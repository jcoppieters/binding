// BindingFileConstructParserBC.cpp: implementation of the CBindingFileConstructParserBC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParserBC.h"

#include "BindingFileConstructParserB_H.h"
#include "BindingOperatorsOperations.h"
#include "BindingUnitFactory.h"
#include "BindingOperatorFactory.h"
#include "BindingFileConstructLinksAlg.h"
#include "BindingFileConstructOperatorsAlg.h"

#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringOperations.h"
#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringReformat.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParserFactory.h"

#include "LogFileConversionConstruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructParserBC::CBindingFileConstructParserBC(CStringArray* pArrayBindingStrings) :
CBindingFileConstructParser(pArrayBindingStrings)
{
	m_pBindingFileEntryTypeC = NULL;
	m_StringArray.RemoveAll();
}

CBindingFileConstructParserBC::~CBindingFileConstructParserBC()
{
	Destroy();
}

/**
 *
 */
BOOL CBindingFileConstructParserBC::AddBindingOperator(CBindingOperator* pNewOperator)
{
	BOOL fResult = FALSE;

	if (pNewOperator != NULL)
	{
		ASSERT(m_pBindingEntryInfo);

		CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
		if (pBindingEntry != 0)
		{
			CBindingOperators* const pBindingOperators = pBindingEntry->GetColOperator();
			if (pBindingOperators != 0)
			{
				CBindingOperator* const pNew = CBindingOperatorFactory::Clone(pNewOperator);
				if (pNew != 0)
				{
					pNew->SetID(0);
					const unsigned short nID = pBindingOperators->Add(pNew);
					if (nID == 0)
					{
						ASSERT(nID);

						fResult = FALSE;
						return fResult;
					}
					else
					{
						fResult = TRUE;
					}

					ASSERT(nID == pNew->GetID());
				}						
			}
		}
	}
	return fResult;
}

CBindingUnit* CBindingFileConstructParserBC::AddForwardUnit(CBindingFileEntryParser* pBindingFileEntryParser)
{
	BOOL fResult = FALSE;

	CBindingUnit  ForwardUnit;
	CBindingUnit* pForwardUnit = NULL;

	if (GetForwardUnit(pBindingFileEntryParser,
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


BOOL CBindingFileConstructParserBC::AddReturnUnits()
{
	BOOL fResult = FALSE;

	CBindingFileConstructParserB_H BindingFileConstructB(m_pArrayBindingStrings);
	if (BindingFileConstructB.Construct() == TRUE)
	{
		const CBindingUnits* const pReturnUnits = BindingFileConstructB.GetReturnUnits();
		if (pReturnUnits != 0)
		{
			/**
			 * Add All Return Units 
			 */
			const int nCount = pReturnUnits->Count();
			for (int i=0;i<nCount;i++)
			{
				CBindingUnit* pReturnUnit = pReturnUnits->Get(i);
				if (pReturnUnit != 0)
				{
					pReturnUnit->SetID(0);

					fResult = AddBindingUnit(pReturnUnit);
					if (fResult == FALSE)
					{
						ASSERT(fResult);
						return fResult;
					}			
				}
				else
				{	
					ASSERT(0);
					return FALSE;
				}
			}				
		}
	}
	ASSERT(fResult);
	return fResult;
}



/**
 *
 *
 */
BOOL CBindingFileConstructParserBC::IsTypeOf()
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
					pBindingFileEntryType->IsTypeC() == TRUE)				
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

BOOL CBindingFileConstructParserBC::Init()
{
	BOOL fResult = TRUE;	

	ASSERT(m_pBindingFileEntryTypeC == 0);

	if (m_pBindingFileEntryTypeC == 0)
	{	
		const int indexCBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeC);
		if (indexCBinding != -1)
		{			
			if ((m_pBindingFileEntryTypeC = CBindingFileEntryParserFactory::Get_TypeC(
					m_pArrayBindingStrings->GetAt(indexCBinding))) != 0)
			{				
				fResult = TRUE;
			}	
		}

		ASSERT(indexCBinding != -1);
	}

	ASSERT(m_pBindingFileEntryTypeC != 0);

	return fResult;
}
		
BOOL CBindingFileConstructParserBC::CheckPreConditions()
{
	BOOL fResult = TRUE;	//FALSE;

	return fResult;
}

BOOL CBindingFileConstructParserBC::AddUnits()
{
	BOOL fResult = FALSE;

	if (AddReturnUnits() == TRUE)
	{
		m_pForwardUnit = AddForwardUnit(m_pBindingFileEntryTypeC);
		if (m_pForwardUnit != 0)
		{
			fResult = TRUE;
		}
	}

	ASSERT(fResult);
	return fResult;
}

BOOL CBindingFileConstructParserBC::AddOperators()
{
	return ( _AddOperators( m_pBindingFileEntryTypeC ) );
}

BOOL CBindingFileConstructParserBC::_AddOperators(CBindingFileEntryParserC *pBindingFileEntryParser)
{
	BOOL fResult = FALSE;

	if (pBindingFileEntryParser == 0) {
		ASSERT( pBindingFileEntryParser );
		return FALSE;
	}

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{		
		CBindingOperators* const pBindingOperators = 
			pBindingFileEntryParser->GetBindingOperators(); 

		if (pBindingOperators != 0)
		{
			if (pBindingOperators->Count() != 0)
			{
				CBindingOperatorsOperations BindingOperatorOperations(
					pBindingEntry->GetColOperator());

				fResult = BindingOperatorOperations.Import(pBindingOperators);	
				
				CBindingFileConstructOperatorsAlg Algoritme(pBindingEntry);
				fResult = Algoritme.Build();	
				
				ASSERT(fResult);
			}
			else
			{
				fResult = TRUE;
			}
		}
	}		

	ASSERT(fResult);
	return fResult;
}

BOOL CBindingFileConstructParserBC::_BuildEvalString(CBindingFileEntryParserC *pBindingFileEntryParser)
{
	BOOL fResult = FALSE;

	if (pBindingFileEntryParser == 0) {
		ASSERT(pBindingFileEntryParser);
		return FALSE;
	}

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != 0)
	{			
		pBindingEntry->GetEvaluationString()->Set(
			pBindingFileEntryParser->GetBindingEvaluationString()->Get());

		CBindingEvaluationStringOperations BindingEvaluationStringOperations(
			pBindingEntry->GetEvaluationString(),		
			pBindingFileEntryParser->GetBindingUnits(),			// Old
			pBindingEntry->GetColUnit(),						// New
			pBindingFileEntryParser->GetBindingOperators(),		// Old
			pBindingEntry->GetColOperator());					// New

		if ((fResult = BindingEvaluationStringOperations.UpdateData()) == FALSE)	
		{
			if (m_pLogFile != 0)
			{
				m_pLogFile->Report(BindingEvaluationStringOperations.GetErrorLog());
			}

			ASSERT(fResult);
			return fResult;
		}

		if (m_pLogFile != 0)
		{
			m_pLogFile->Report(BindingEvaluationStringOperations.GetErrorLog());
		}

		CBindingEvaluationStringReformat BindingEvaluationReformat(pBindingEntry->GetEvaluationString());
		fResult = BindingEvaluationReformat.Start();

		if (m_pLogFile != 0)
		{
			m_pLogFile->Report(BindingEvaluationReformat.GetErrorLog(),FALSE);
		}

		if (fResult == FALSE) {
			return FALSE;
		}

		pBindingEntry->GetEvaluationString()->AddUnit(m_pForwardUnit->GetID());

		fResult = TRUE;
	}

	return fResult;
}

BOOL CBindingFileConstructParserBC::BuildEvalString()
{
	return ( _BuildEvalString(m_pBindingFileEntryTypeC) );

}

BOOL CBindingFileConstructParserBC::AddLinks()
{
	BOOL fResult = FALSE;

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		CBindingFileConstructLinksAlg Algoritme(pBindingEntry);
		fResult = Algoritme.Build();	
	}

	return fResult;
}

BOOL CBindingFileConstructParserBC::BuildBindingEntryInfo()
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
		if (GetBindingNr(m_pBindingFileEntryTypeC,
						&usBindingNr) 
						== TRUE)
		{
			CString szName;
			szName.Format("B/C - 0x%04x",usBindingNr);

			pBindingEntry->SetName(szName);
			pBindingEntry->SetBindingNr(usBindingNr);

			BYTE bBindingFlags;
			if (GetBindingFlags(m_pBindingFileEntryTypeC,
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

BOOL CBindingFileConstructParserBC::Destroy()
{
	BOOL fResult = TRUE;

	if (m_pBindingFileEntryTypeC != 0)
	{
		delete m_pBindingFileEntryTypeC;
		m_pBindingFileEntryTypeC = 0;
	}
	
	return fResult;
}







/*
BOOL CBindingFileConstructParserBC::GetForwardUnit(CBindingUnit* pForwardUnit)
{
	BOOL fResult = FALSE;

	if (m_pBindingFileEntryTypeC == 0) {
		ASSERT( m_pBindingFileEntryTypeC );
		return FALSE;
	}
	
	CBindingUnit* const pUnit = m_pBindingFileEntryTypeC->GetForwardUnit();
	if (pUnit != 0)
	{	
		*pForwardUnit = *pUnit;
		return TRUE;		
	}
			
	ASSERT(fResult);
	return fResult;
}
*/
/**
 *
 */
/*
BOOL CBindingFileConstructParserBC::GetBindingNr(unsigned short* usBindingNr)
{
	if (m_pBindingFileEntryTypeC == 0) {
		ASSERT( m_pBindingFileEntryTypeC );
		return FALSE;
	}

	*usBindingNr = m_pBindingFileEntryTypeC->GetBindingNr();
	return TRUE;
}
*/
/*
BOOL CBindingFileConstructParserBC::GetBindingFlags(BYTE* bBindingFlags)
{
	if (m_pBindingFileEntryTypeC == 0) {
		ASSERT( m_pBindingFileEntryTypeC );
		return FALSE;
	}
	
	*bBindingFlags = m_pBindingFileEntryTypeC->GetBindingFlags();
	return TRUE;
}
BOOL CBindingFileConstructParserBC::BuildEvalString()
{
	BOOL fResult = FALSE;

	if (m_pBindingFileEntryTypeC == 0) {
		ASSERT(m_pBindingFileEntryTypeC);
		return FALSE;
	}

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != 0)
	{			
		pBindingEntry->GetEvaluationString()->Set(
			m_pBindingFileEntryTypeC->GetBindingEvaluationString()->Get());

		CBindingEvaluationStringOperations BindingEvaluationStringOperations(
			pBindingEntry->GetEvaluationString(),		
			m_pBindingFileEntryTypeC->GetBindingUnits(),		// Old
			pBindingEntry->GetColUnit(),						// New
			m_pBindingFileEntryTypeC->GetBindingOperators(),	// Old
			pBindingEntry->GetColOperator());					// New

		if ((fResult = BindingEvaluationStringOperations.UpdateData()) == FALSE)	
		{
			ASSERT(fResult);
			return fResult;
		}

		pBindingEntry->GetEvaluationString()
			->AddUnit(m_pForwardUnit->GetID());

		fResult = TRUE;
	}

	return fResult;
}

BOOL CBindingFileConstructParserBC::AddOperators()
{
	BOOL fResult = FALSE;

	if (m_pBindingFileEntryTypeC == 0) {
		ASSERT( m_pBindingFileEntryTypeC );
		return FALSE;
	}

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{		
		CBindingOperators* const pBindingOperators = 
			m_pBindingFileEntryTypeC->GetBindingOperators(); 

		if (pBindingOperators != 0)
		{
			if (pBindingOperators->Count() != 0)
			{
				CBindingOperatorsOperations BindingOperatorOperations(
					pBindingEntry->GetColOperator());

				fResult = BindingOperatorOperations.Import(pBindingOperators);	
				
				CBindingFileConstructOperatorsAlg Algoritme(pBindingEntry);
				fResult = Algoritme.Build();	
				
				ASSERT(fResult);
			}
			else
			{
				fResult = TRUE;
			}
		}
	}		

	ASSERT(fResult);
	return fResult;
}

CBindingUnit* CBindingFileConstructParserBC::AddForwardUnit()
{
	BOOL fResult = FALSE;

	CBindingUnit  ForwardUnit;
	CBindingUnit* pForwardUnit = NULL;

	if (GetForwardUnit(m_pBindingFileEntryTypeC,
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