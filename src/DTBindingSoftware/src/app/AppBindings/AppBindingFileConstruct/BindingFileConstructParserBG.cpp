// BindingFileConstructParserBG.cpp: implementation of the CBindingFileConstructParserBG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParserBG.h"

#include "BindingFileConstructParserB_H.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParserFactory.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructParserBG::CBindingFileConstructParserBG(CStringArray* pArrayBindingStrings) :
CBindingFileConstructParser(pArrayBindingStrings)
{
	m_pBindingFileEntryParserG = 0;
}

CBindingFileConstructParserBG::~CBindingFileConstructParserBG()
{
	Destroy();
}

/**
 *
 */
CBindingUnit* CBindingFileConstructParserBG::AddForwardUnit(CBindingFileEntryParser* pBindingFileEntryParser)
{
	//BOOL fResult = FALSE;

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

/**
 *
 */
CBindingUnit* CBindingFileConstructParserBG::AddReturnUnit()
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

//////////////////////////////////////////////////////////////////
/**
 *	Array has only B & G Strings !!!
 */
BOOL CBindingFileConstructParserBG::IsTypeOf()
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
					pBindingFileEntryType->IsTypeG() == TRUE)
				{
					//TRACE1("\n%s",pBindingFileEntryType->ToString());

					fResult = TRUE;				
				}
				else
				{
					//TRACE1("\n%s",pBindingFileEntryType->ToString());

					fResult = FALSE;
					break;
				}
			}
		}
	}
	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBG::Init()
 *
 */
BOOL CBindingFileConstructParserBG::Init()
{
	ASSERT(m_pBindingFileEntryParserG == 0);

	BOOL fResult = FALSE;

	m_pForwardUnit = NULL;
	m_pReturnUnit = NULL;

	ASSERT( m_pBindingFileEntryParserG == 0);

	if (m_pBindingFileEntryParserG == 0)
	{
		const int indexGBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeG);
		if (indexGBinding != -1)
		{
			if ((m_pBindingFileEntryParserG = CBindingFileEntryParserFactory::Get_TypeG(
					m_pArrayBindingStrings->GetAt(indexGBinding))) != 0)
			{
				fResult = TRUE;
			}
		}
	}

	ASSERT( m_pBindingFileEntryParserG != 0);

	return fResult;
}

/**
 *
 */
BOOL CBindingFileConstructParserBG::Destroy()
{
	if (m_pBindingFileEntryParserG !=0)
	{
		delete m_pBindingFileEntryParserG;
		m_pBindingFileEntryParserG = 0;
	}

	return TRUE;
}

/**
 * BOOL CBindingFileConstructParserBG::CheckPreConditions()
 *
 */
BOOL CBindingFileConstructParserBG::CheckPreConditions()
{
	BOOL fResult = TRUE;

	ASSERT(IsTypeOf() == TRUE);
	ASSERT(m_pArrayBindingStrings);

	const int nMaxEntries = m_pArrayBindingStrings->GetSize();

	if (nMaxEntries != 2) 
	{	
		TRACE("\nB/G Construction may have only 2 entries in array...");
		ASSERT(0);

		fResult = FALSE;
	}

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBG::AddUnits()
 *
 */
BOOL CBindingFileConstructParserBG::AddUnits()
{
	BOOL fResult = FALSE;

	m_pReturnUnit = AddReturnUnit();
	m_pForwardUnit = AddForwardUnit( m_pBindingFileEntryParserG );

	if (m_pForwardUnit != NULL && m_pReturnUnit != NULL)
	{	
		fResult = TRUE;	
	}

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBG::AddLinks()
 *
 */
BOOL CBindingFileConstructParserBG::AddLinks()
{
	BOOL fResult = FALSE;

	ASSERT(m_pReturnUnit);	
	ASSERT(m_pForwardUnit);

	fResult = AddBindingLink(m_pReturnUnit,m_pForwardUnit);		

	return fResult;
}

/**
 * BOOL CBindingFileConstructParserBG::AddLinks()
 *
 */
BOOL CBindingFileConstructParserBG::BuildEvalString()
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
 * BOOL CBindingFileConstructParserBG::BuildBindingEntryInfo()
 *
 */
BOOL CBindingFileConstructParserBG::BuildBindingEntryInfo()
{
	ASSERT(m_pBindingEntryInfo);

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry != NULL)
	{
		m_pBindingEntryInfo->Set_NInputs(1);
		m_pBindingEntryInfo->Set_NOutputs(1);
		m_pBindingEntryInfo->Set_Type(TYPE_BG_BINDING);

		unsigned short usBindingNr = 0;
		if (GetBindingNr(m_pBindingFileEntryParserG,
						 &usBindingNr) 
						 == TRUE)
		{
			CString szName;
			szName.Format("B/G - 0x%04x",usBindingNr);

			pBindingEntry->SetName(szName);
			pBindingEntry->SetBindingNr(usBindingNr);

			BYTE bBindingFlags;
			if (GetBindingFlags(m_pBindingFileEntryParserG,
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

	return FALSE;
}

/**
 *
 */
BOOL CBindingFileConstructParserBG::ConstructBindingEvaluation(CBindingUnit* pForwardUnit,
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
			pBindingEvaluationString->AddUnit(pReturnUnit->GetID());
			pBindingEvaluationString->Add('=');
			pBindingEvaluationString->AddUnit(pForwardUnit->GetID());

			return TRUE;
		}
	}
	return FALSE;
}




/**
 *
 */
/*
BOOL CBindingFileConstructParserBG::GetBindingNr(unsigned short* usBindingNr)
{
	CBindingFileEntry BindingFileEntryB(m_pArrayBindingStrings->GetAt(0));
	CBindingFileEntryParser* const pBindingFileEntry = BindingFileEntryB.Parse();
	if (pBindingFileEntry != NULL)
	{
		*usBindingNr = pBindingFileEntry->GetBindingNr();
		return TRUE;
	}
	return FALSE;
}
*/

/**
 *
 */
/*
BOOL CBindingFileConstructParserBG::GetBindingFlags(BYTE* bBindingFlags)
{
	if (m_pBindingFileEntryParserG == 0) {
		ASSERT(m_pBindingFileEntryParserG);
		return FALSE;
	}

	*bBindingFlags = m_pBindingFileEntryParserG->GetBindingFlags();
	return TRUE;
}
*/

/**
 * 
 */
/*
BOOL CBindingFileConstructParserBG::GetForwardUnit(CBindingUnit* pForwardUnit)
{
	if (m_pBindingFileEntryParserG == 0) {
		ASSERT(m_pBindingFileEntryParserG);
		return FALSE;
	}

	CBindingUnit* const pUnit = m_pBindingFileEntryParserG->GetForwardUnit();
	if (pUnit != 0)
	{
		*pForwardUnit = *pUnit;	
		return TRUE;			
	}
	return FALSE;
}
*/

/**
 *
 */
/*
CBindingUnit* CBindingFileConstructParserBG::AddForwardUnit()
{
	BOOL fResult = FALSE;

	CBindingUnit  ForwardUnit;
	CBindingUnit* pForwardUnit = NULL;

	if (GetForwardUnit(m_pBindingFileEntryParserG,
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
CBindingUnit* CBindingFileConstructParserBG::AddForwardUnit()
{
	BOOL fResult = FALSE;

	CBindingUnit  ForwardUnit;
	CBindingUnit* pForwardUnit = NULL;

	if (GetForwardUnit(m_pBindingFileEntryParserG,
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