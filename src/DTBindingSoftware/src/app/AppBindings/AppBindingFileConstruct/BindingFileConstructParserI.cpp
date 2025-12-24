// BindingFileConstructParserI.cpp: implementation of the CBindingFileConstructParserI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParserI.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParserFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructParserI::CBindingFileConstructParserI(CStringArray* pArrayBindingStrings) :
CBindingFileConstructParser(pArrayBindingStrings)
{
	m_pBindingFileEntryParserI = 0;
}

CBindingFileConstructParserI::~CBindingFileConstructParserI()
{
	Destroy();
}


BOOL CBindingFileConstructParserI::GetReturnUnit(CBindingUnit* pReturnUnit)
{
	if (m_pBindingFileEntryParserI == 0) {
		ASSERT( m_pBindingFileEntryParserI );
		return FALSE;
	}

	CBindingUnit* const pUnit = m_pBindingFileEntryParserI->GetReturnUnit();
	if (pUnit != 0)
	{
		*pReturnUnit = *pUnit;
		return TRUE;	
	}
	return FALSE;
}


/**
 *
 */
CBindingUnit* CBindingFileConstructParserI::AddForwardUnit()
{
	//BOOL fResult = FALSE;

	CBindingUnit  ForwardUnit;
	CBindingUnit* pForwardUnit = NULL;

	if (GetForwardUnit(m_pBindingFileEntryParserI,
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

/**
 *
 */
CBindingUnit* CBindingFileConstructParserI::AddReturnUnit()
{
	//BOOL fResult = FALSE;

	CBindingUnit  ReturnUnit;
	CBindingUnit* pReturnUnit = NULL;

	if (GetReturnUnit(&ReturnUnit) == TRUE)
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

BOOL CBindingFileConstructParserI::ConstructBindingEvaluation(CBindingUnit* pForwardUnit,
															  CBindingUnit* pReturnUnit)
{
	ASSERT(m_pBindingEntryInfo);

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		CBindingEvaluationString* const pBindingEvaluationString = 
			pBindingEntry->GetEvaluationString();
		
		if (pBindingEvaluationString != NULL)
		{
			if (m_fInverse == TRUE)
			{
				pBindingEvaluationString->Add('!');	
			}
		
			pBindingEvaluationString->AddUnit(pReturnUnit->GetID());
			pBindingEvaluationString->Add('=');
			pBindingEvaluationString->AddUnit(pForwardUnit->GetID());

			return TRUE;			
		}
	}
	return FALSE;
}


/**
 *	Array has only I Strings !!!
 */
BOOL CBindingFileConstructParserI::IsTypeOf()
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
				if (pBindingFileEntryType->IsTypeI() == TRUE)
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
 * BOOL CBindingFileConstructParserI::Init()
 * 
 */
BOOL CBindingFileConstructParserI::Init()
{
	BOOL fResult = FALSE;

	m_pForwardUnit = NULL;
	m_pReturnUnit = NULL;
	m_fInverse = FALSE; 

	if (m_pBindingFileEntryParserI == 0)
	{
		int indexIBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeI);
		if (indexIBinding != -1)
		{
			if ((m_pBindingFileEntryParserI = CBindingFileEntryParserFactory::Get_TypeI(
					m_pArrayBindingStrings->GetAt(indexIBinding))) != 0)
			{
				BYTE bBindingFlags = 0xFF;
				if (GetBindingFlags(m_pBindingFileEntryParserI,
									&bBindingFlags) 
									== TRUE)
				{
					if ((bBindingFlags & 0x80) != 0x80)
					{
						m_fInverse = TRUE;
					}
					
					fResult = TRUE;
				}
			}
		}
	}		

	return fResult;
}


/** 
 *
 */
BOOL CBindingFileConstructParserI::Destroy()
{
	if (m_pBindingFileEntryParserI != 0)
	{
		delete m_pBindingFileEntryParserI;
		m_pBindingFileEntryParserI = 0;
	}

	return TRUE;
}

/**
 * BOOL CBindingFileConstructParserI::CheckPreConditions()
 */
BOOL CBindingFileConstructParserI::CheckPreConditions()
{
	BOOL fResult = TRUE;

	const int nMaxEntries = m_pArrayBindingStrings->GetSize();

	if (nMaxEntries != 1) 
	{	
		TRACE("\nI Construction may have only 1 entry in array...");
		ASSERT(0);

		fResult = FALSE;
	}

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserI::AddUnits()
 */
BOOL CBindingFileConstructParserI::AddUnits()
{
	BOOL fResult = FALSE;

	m_pReturnUnit = AddReturnUnit();
	m_pForwardUnit = AddForwardUnit();

	if (m_pForwardUnit != NULL && m_pReturnUnit != NULL)
	{	
		fResult = TRUE;	
	}

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserI::AddLinks()
 */
BOOL CBindingFileConstructParserI::AddLinks()
{
	BOOL fResult = FALSE;

	ASSERT(m_pReturnUnit);	
	ASSERT(m_pForwardUnit);

	fResult = AddBindingLink(m_pReturnUnit,m_pForwardUnit,m_fInverse);		
	
	return fResult;
}

/**
 * BOOL CBindingFileConstructParserI::BuildEvalString()
 */
BOOL CBindingFileConstructParserI::BuildEvalString()
{
	BOOL fResult = FALSE;

	ASSERT(m_pForwardUnit);
	ASSERT(m_pReturnUnit);

	if (m_pForwardUnit != NULL && m_pReturnUnit != NULL)
	{
		fResult = ConstructBindingEvaluation(m_pForwardUnit,
											 m_pReturnUnit);		
	}

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserI::BuildBindingEntryInfo()
 */
BOOL CBindingFileConstructParserI::BuildBindingEntryInfo()
{
ASSERT(m_pBindingEntryInfo);

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		m_pBindingEntryInfo->Set_NInputs(1);
		m_pBindingEntryInfo->Set_NOutputs(1);
		m_pBindingEntryInfo->Set_Type(TYPE_I_BINDING);
		m_pBindingEntryInfo->Set_Inverse(m_fInverse);
		//m_pBindingEntryInfo->Set

		unsigned short usBindingNr = 0;
		if (GetBindingNr(m_pBindingFileEntryParserI,
						 &usBindingNr) 
						 == TRUE)
		{
			CString szName;
			szName.Format("I - 0x%04x",usBindingNr);

			pBindingEntry->SetName(szName);
			pBindingEntry->SetBindingNr(usBindingNr);				
			pBindingEntry->SetBindingReturnFlags(BINDINGFLAGS_STATUS_NOCHANGE);
			return TRUE;
		}
		ASSERT(0);
	}

	return FALSE;
}



/*
BOOL CBindingFileConstructParserI::GetForwardUnit(CBindingUnit* pForwardUnit)
{
	int indexIBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeI);
	if (indexIBinding != -1)
	{
		CBindingFileEntry BindingFileEntryI(m_pArrayBindingStrings->GetAt(indexIBinding));
			
		CBindingFileEntryParserI* const pBindingFileEntryTypeI = 
			static_cast<CBindingFileEntryParserI*>(BindingFileEntryI.Parse());

		if (pBindingFileEntryTypeI != NULL)
		{		
			ASSERT(pBindingFileEntryTypeI->IsTypeI() == TRUE);

			CBindingUnit* const pUnit = pBindingFileEntryTypeI->GetForwardUnit();
			if (pUnit != 0)
			{
				//pForwardUnit->Copy(pUnit);

				*pForwardUnit = *pUnit;
				return TRUE;			
			}
		}		
	}
	return FALSE;
}
BOOL CBindingFileConstructParserI::GetReturnUnit(CBindingUnit* pReturnUnit)
{
	int indexIBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeI);
	if (indexIBinding != -1)
	{
		CBindingFileEntry BindingFileEntryI(m_pArrayBindingStrings->GetAt(indexIBinding));
			
		CBindingFileEntryParserI* const pBindingFileEntryTypeI = 
			static_cast<CBindingFileEntryParserI*>(BindingFileEntryI.Parse());

		if (pBindingFileEntryTypeI != NULL)
		{		
			ASSERT(pBindingFileEntryTypeI->IsTypeI() == TRUE);

			CBindingUnit* pUnit = pBindingFileEntryTypeI->GetReturnUnit();
			if (pUnit != 0)
			{
				//pReturnUnit->Copy(pUnit);
				*pReturnUnit = *pUnit;
				return TRUE;			
			}
		}		
	}
	return FALSE;
}
*/

/*
BOOL CBindingFileConstructParserI::Init()
{
	BOOL fResult = FALSE;

	m_pForwardUnit = NULL;
	m_pReturnUnit = NULL;
	m_fInverse = FALSE; 

	if (m_pBindingFileEntryParserI == 0)
	{
		int indexIBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeI);
		if (indexIBinding != -1)
		{
			CBindingFileEntry BindingFileEntryI(m_pArrayBindingStrings->GetAt(indexIBinding));

			CBindingFileEntryParserI* const pBindingFileEntryTypeI = 
				static_cast<CBindingFileEntryParserI*>(BindingFileEntryI.Parse());

			if (pBindingFileEntryTypeI != NULL)
			{
				m_pBindingFileEntryParserI = new CBindingFileEntryParserI(*pBindingFileEntryTypeI);

				BYTE bBindingFlags = 0xFF;
				if (GetBindingFlags(&bBindingFlags) == TRUE)
				{
					if ((bBindingFlags & 0x80) != 0x80)
					{
						m_fInverse = TRUE;
					}
					
					fResult = TRUE;
				}
			}
		}
	}

	return fResult;
}
*/

/**
 *
 */
/*
BOOL CBindingFileConstructParserI::GetBindingNr(unsigned short* usBindingNr)
{
	if (m_pBindingFileEntryParserI == 0) {
		ASSERT( m_pBindingFileEntryParserI );
		return FALSE;
	}

	*usBindingNr = m_pBindingFileEntryParserI->GetBindingNr();
	return TRUE;
}
*/

/**
 *
 */
/*
BOOL CBindingFileConstructParserI::GetBindingFlags(BYTE* bBindingFlags)
{
	if (m_pBindingFileEntryParserI == 0) {
		ASSERT( m_pBindingFileEntryParserI );
		return FALSE;
	}

	*bBindingFlags = m_pBindingFileEntryParserI->GetBindingFlags();
	return TRUE;
}
*/

////////////////////////////////////////////////////////////////////
/**
 *
 */
/*
BOOL CBindingFileConstructParserI::GetForwardUnit(CBindingUnit* pForwardUnit)
{
	if (m_pBindingFileEntryParserI == 0) {
		ASSERT( m_pBindingFileEntryParserI );
		return FALSE;
	}

	CBindingUnit* const pUnit = m_pBindingFileEntryParserI->GetForwardUnit();
	if (pUnit != 0)
	{
		*pForwardUnit = *pUnit;
		return TRUE;	
	}
	return FALSE;
}
*/