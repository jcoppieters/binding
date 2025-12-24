// BindingFileConstructLinksAlg.cpp: implementation of the CBindingFileConstructLinksAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileConstructLinksAlg.h"

#include "BindingEntry.h"
#include "BindingLinksOperations.h"

#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringSeperatorAlg2.h"
#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringSeperator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructLinksAlg::CBindingFileConstructLinksAlg(CBindingEntry* pBindingEntry) :
m_pBindingEntry( pBindingEntry ),
m_pBindingLinks( pBindingEntry->GetColLink() ),
m_pBindingOperators( pBindingEntry->GetColOperator() ),
m_pBindingUnits( pBindingEntry->GetColUnit() )
{
	ASSERT( m_pBindingEntry );
	ASSERT( m_pBindingLinks );
	ASSERT( m_pBindingOperators );
	ASSERT( m_pBindingUnits );

	m_StringArray.RemoveAll();
}

CBindingFileConstructLinksAlg::~CBindingFileConstructLinksAlg()
{
	m_StringArray.RemoveAll();
}

BOOL CBindingFileConstructLinksAlg::ConstructOutputs(CString s)
{
	BOOL fResult = FALSE;
	BOOL fInverse = FALSE;
	BOOL fInput = FALSE;
	
	CBindingUnit* pFirstBindingUnit = 0;
	CBindingUnit* pNextBindingUnit = 0;
	CBindingOperator* pBindingOperator = 0;

	const int nLength = s.GetLength();
	for (int i=0;i<nLength;i++)
	{
		const char c = s.GetAt(i);
		switch(c)
		{
			case 'U':
			{				
				CBindingUnit* const pBindingUnit = 
					CBindingEvaluationStringSeperator::GetBindingUnit(m_pBindingUnits, i, s);

				if (fInput == TRUE)
				{			
					CBindingLinksOperations BindingLinksOperations(m_pBindingLinks);
					fResult = BindingLinksOperations.Add(pBindingUnit,
														 pFirstBindingUnit,
														 fInverse);				
				}
				else
				{
					if (pFirstBindingUnit == 0) 
					{
						pFirstBindingUnit = pBindingUnit;
					}
					else
					{
						pFirstBindingUnit = pNextBindingUnit;
						pNextBindingUnit = pBindingUnit;

						CBindingLinksOperations BindingLinksOperations(m_pBindingLinks);
						fResult = BindingLinksOperations.Add(pFirstBindingUnit,pNextBindingUnit);

						if (fResult == FALSE) 
						{
							ASSERT( fResult );
							return fResult;
						}
					}
				}

			} break;

			case 'Y':			
			case 'X':
			{
				if (fInput == TRUE)
				{
					CBindingOperator* const pBindingOperator = 
						CBindingEvaluationStringSeperator::GetOperator(m_pBindingOperators, i, s);
					
					CBindingLinksOperations BindingLinksOperations(m_pBindingLinks);
					fResult = BindingLinksOperations.Add(pBindingOperator,
														 pFirstBindingUnit,
														 fInverse);		
				}
				else
				{
					ASSERT(0);
					return FALSE;
				}
				
			} break;			

			case '>':
			{
				if (fInput == TRUE)
				{
					ASSERT(0);
					return FALSE;
				}

			} break;

			case '=':
			{
				if (fInput == TRUE)
				{
					ASSERT(0);
					return FALSE;
				}

				fInput = TRUE;

			} break;

			case '!':
			{
				fInverse = TRUE;

			} break;
		}
	
	}

	return fResult;
}

BOOL CBindingFileConstructLinksAlg::ConstructInputs(CString s)
{
	BOOL fResult = FALSE;

	CBindingOperator* const pBindingOperatorOut = 
		CBindingEvaluationStringSeperator::GetOperator(m_pBindingOperators, 0, s);

	if (pBindingOperatorOut == 0) {
		return FALSE;
	}

	const int nLength = s.GetLength();
	int i = s.Find('=');

	if (i == -1) 
	{
		ASSERT(0);
		return FALSE;
	}

	BOOL fInverse = FALSE;

	for (;i<nLength;i++) 
	{
		const char c = s.GetAt(i);
		switch(c)
		{
			case '!':
			{
				fInverse = TRUE;

			} break;

			case 'Y':
			case 'X':
			{
				CBindingOperator* const pBindingOperator = 
					CBindingEvaluationStringSeperator::GetOperator(m_pBindingOperators, i, s);

				if (pBindingOperator != 0)
				{
					CBindingLinksOperations BindingLinksOperations(m_pBindingLinks);
					fResult = BindingLinksOperations.Add(pBindingOperator,
														 pBindingOperatorOut,
														 fInverse);

					fInverse = FALSE;

					if (fResult == FALSE) 
					{
						ASSERT( fResult );
						return fResult;
					}
				}
			} break;
					
			case 'U':
			{
				CBindingUnit* const pBindingUnit = 
					CBindingEvaluationStringSeperator::GetBindingUnit(m_pBindingUnits, i, s);

				if (pBindingUnit != 0)
				{
					CBindingLinksOperations BindingLinksOperations(m_pBindingLinks);
					fResult = BindingLinksOperations.Add(pBindingUnit,
														 pBindingOperatorOut,
														 fInverse);

					fInverse = FALSE;

					if (fResult == FALSE) 
					{
						ASSERT( fResult );
						return fResult;
					}
				}
			} break;

			case '+':
			case '^':
			case '*':
			{
				fInverse = FALSE;

			} break;
		
		}	
	}
	
	return fResult;
}

BOOL CBindingFileConstructLinksAlg::Build()
{
	BOOL fResult = FALSE;

	CBindingEvaluationStringSeperatorAlg2 Algoritme(m_pBindingEntry);
	CBindingEvaluationStringSeperator StringSeperator(m_pBindingEntry,
													  m_StringArray,
													  &Algoritme);	
	try
	{
		if (StringSeperator.Build() == TRUE)
		{

#ifdef _DEBUG
			if(1)
			{
				for (int i=0;i<m_StringArray.GetSize();i++)
				{
					CString szDebug(m_StringArray.GetAt(i));
					TRACE1("\n%s",szDebug);
				}	
			}

			TRACE1("\nString seperator: %s",
				m_pBindingEntry->GetEvaluationString()->Get());
#endif
		
			const int nMaxEntries = m_StringArray.GetSize();
			ASSERT( nMaxEntries > 1);

			if (1)
			{
				CString s(m_StringArray.GetAt(nMaxEntries-1));
				fResult = ConstructOutputs(s);
				if (fResult == FALSE) 
				{
					return fResult;				
				}
			}
		
			for (int i=0;i<nMaxEntries-1;i++)
			{
				CString s(m_StringArray.GetAt(i));
				if (ConstructInputs(s) == FALSE)
				{
					if (!(i == 0 && nMaxEntries == 2))
					{
						return FALSE;
					}
				}
				else
				{
					fResult = TRUE;
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
	return fResult;
}
