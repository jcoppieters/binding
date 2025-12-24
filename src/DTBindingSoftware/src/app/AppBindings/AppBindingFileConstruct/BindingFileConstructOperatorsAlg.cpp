// BindingFileConstructOperatorsAlg.cpp: implementation of the CBindingFileConstructOperatorsAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileConstructOperatorsAlg.h"

#include "BindingEntry.h"
#include "DebugMacros.h"

#include "ExceptionDatabase.h"
#include "BindingOperatorFactory.h"

#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringSeperatorAlg2.h"
#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringSeperator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructOperatorsAlg::CBindingFileConstructOperatorsAlg(CBindingEntry* pBindingEntry) :
m_pBindingEntry( pBindingEntry ),
m_pBindingOperators( pBindingEntry->GetColOperator() )
{
	ASSERT( m_pBindingEntry );
	ASSERT( m_pBindingOperators );

	m_StringArray.RemoveAll();
	m_BindingOperators.RemoveAll();
}

CBindingFileConstructOperatorsAlg::~CBindingFileConstructOperatorsAlg()
{	
	m_StringArray.RemoveAll();
	m_BindingOperators.RemoveAll();
}

BOOL CBindingFileConstructOperatorsAlg::Add(CString s)
{
	BOOL fResult = FALSE;
	
	if (s.IsEmpty() == TRUE) {
		ASSERT(0);
		return FALSE;
	}

	CBindingOperator* const pBindingOperator = 
		CBindingEvaluationStringSeperator::GetOperator(m_pBindingOperators, 0, s);
	
	if (pBindingOperator != 0)
	{
		CBindingOperator* const pNewBindingOperator = 
			CBindingOperatorFactory::Clone(pBindingOperator);
		
		if (pNewBindingOperator != 0)
		{
			const unsigned short nID = m_BindingOperators.Add(pNewBindingOperator);

			ASSERT(nID != 0);
			ASSERT(nID == pNewBindingOperator->GetID());
			ASSERT(pBindingOperator->GetID() == pNewBindingOperator->GetID());
		
			if (nID != 0)
			{
				fResult = TRUE;
			}	
		}
	}
	
	return fResult;
}


BOOL CBindingFileConstructOperatorsAlg::Verify()
{
	BOOL fResult = FALSE;
	const int nMaxEntries = m_StringArray.GetSize();

	ASSERT( nMaxEntries > 1 );

	for (int i=0;i<nMaxEntries-1;i++)
	{
		CString s(m_StringArray.GetAt(i));

		fResult = Add(s);
		
		if (fResult == FALSE) 
		{
			ASSERT( fResult );
			return FALSE;
		}	

	}

	return fResult;
}

BOOL CBindingFileConstructOperatorsAlg::Build()
{
/*
#ifdef _DEBUG_BINDINGFILE_CONSTRUCT_PARSER

	if (1)
	{
		for (int i=0;i<m_pBindingEntry->GetColUnit()->Count();i++)
		{
			const CBindingUnit* const pBindingUnit = m_pBindingEntry->GetColUnit()->Get(i);
			CString szDebug;
			szDebug.Format("index:%d : Id: %d, Name %s",i,
				pBindingUnit->GetID(),pBindingUnit->GetName());
			TRACE1("\n%s",szDebug);
		}	
	}

#endif
*/
	BOOL fResult = FALSE;

	CBindingEvaluationStringSeperatorAlg2 Algoritme(m_pBindingEntry);
	CBindingEvaluationStringSeperator StringSeperator(m_pBindingEntry,
													  m_StringArray,
													  &Algoritme);

	try
	{
		if (StringSeperator.Build() == TRUE)
		{

#ifdef _DEBUG_BINDINGFILE_CONSTRUCT_PARSER

			TRACE("\n--CBindingFileConstructOperatorsAlg::Build()--");

			if(1)
			{
				for (int i=0;i<m_StringArray.GetSize();i++)
				{
					CString szDebug(m_StringArray.GetAt(i));
					TRACE1("\n%s",szDebug);
				}	
			}
			if(1)
			{
				CString szDebug(m_pBindingEntry->GetEvaluationString()->Get());
				TRACE1("\nString seperator: %s",szDebug);					
			}
#endif

			fResult = Verify();
			ASSERT(fResult);

			if (fResult == TRUE)
			{
				*m_pBindingOperators = m_BindingOperators;				
			}
		}
	}

	catch ( CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		fResult = FALSE;
	}

	catch ( CExceptionDatabase* e)
	{
		e->Display();
		e->Delete();
		fResult = FALSE;
	}

	return fResult;
}
