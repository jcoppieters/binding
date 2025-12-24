// BindingEvaluationStringSeperatorAlg2.cpp: implementation of the CBindingEvaluationStringSeperatorAlg2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingEvaluationStringSeperatorAlg2.h"

#include "BindingEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringSeperatorAlg2::CBindingEvaluationStringSeperatorAlg2(CBindingEntry* pBindingEntry) :
CBindingEvaluationStringSeperatorAlg(pBindingEntry)
//m_pBindingOperators(pBindingEntry->GetColOperator())
{
	//ASSERT( m_pBindingOperators );

	m_MaxOperatorsTypeAnd = 0;
	m_MaxOperatorsTypeOr = 0;
	m_MaxOperatorsTypeXor = 0;
	
	nCurrentTypeAnd = 0;
	nCurrentTypeOr = 0;
	nCurrentTypeXor = 0;

	m_fInit = FALSE;
}

CBindingEvaluationStringSeperatorAlg2::~CBindingEvaluationStringSeperatorAlg2()
{

}

/**
 *
 */
CString CBindingEvaluationStringSeperatorAlg2::FindOperator(CString szDataIn)
{
	if (szDataIn.IsEmpty() == TRUE) {
		return CString("");
	}

	CBindingOperator BindingOperator;
	CString szOperator;

	Init();

	/**
	 * if ParseLogOperators returns TRUE, cOperator holds: *,+,^
	 */
	char cOperator = '\0';
	unsigned short nTimerId = 0;
	BOOL fInverse = FALSE;

	if (Convert.ParseLogOperators(szDataIn,&cOperator) == FALSE) 
	{
		if (Convert.ParseTOperators(szDataIn,&cOperator,&nTimerId,&fInverse) == FALSE)
		{
			return CString("");
		}
	}	

	ASSERT(cOperator!='\0');

	const int nMaxEntries = m_pBindingOperators->Count();

	nCountTypeAnd = 0;
	nCountTypeOr = 0;
	nCountTypeXor = 0;

	for (int i=0;i<nMaxEntries;i++)
	{
		CBindingOperator* const pBindingOperator = 
			Find(cOperator,i,nTimerId);

		if (pBindingOperator != 0)
		{			
			return(Format(pBindingOperator,fInverse));					
		}
	}	

	return CString("");
}

void CBindingEvaluationStringSeperatorAlg2::Init()
{
	if (m_fInit == FALSE)
	{
		for (int i=0;i<m_pBindingOperators->Count();i++)
		{
			CBindingOperator* const pBindingOperator = m_pBindingOperators->Get(i);
			if (pBindingOperator != 0)
			{
				switch(pBindingOperator->GetType())
				{
					case PM_OPERATOR_AND:
						m_MaxOperatorsTypeAnd++;
					break;

					case PM_OPERATOR_OR:
						m_MaxOperatorsTypeOr++;
					break;

					case PM_OPERATOR_XOR:
						m_MaxOperatorsTypeXor++;
					break;				
				}
			}		
		}
		m_fInit = TRUE;
	}
}

CBindingOperator* CBindingEvaluationStringSeperatorAlg2::Find(char cOperator,int index,int nTimerId)
{
	const OperatorType Type = CBindingOperator::Convert(cOperator);

	if (Type == PM_OPERATOR_UNDEFINED) 
	{
		ASSERT(0);
		return 0;
	}

	for (index;index<m_pBindingOperators->Count();index++)
	{
		if (Type == PM_OPERATOR_TIMER) 
		{
			CBindingOperator* const pBindingOperator = Find(index,nTimerId);
			if (pBindingOperator != 0) 
			{
				return (pBindingOperator);
			}
		}
		else
		{
			CBindingOperator* const pBindingOperator = Find(index,Type);
			if (pBindingOperator != 0) 
			{
				return (pBindingOperator);
			}
		}
	}	
	return 0;
}

CBindingOperator* CBindingEvaluationStringSeperatorAlg2::Find(int index,int nTimerId)
{
	CBindingOperator* pBindingOperator = 0;

	if ((pBindingOperator = m_pBindingOperators->Get(index)) == 0) 
	{
		ASSERT(pBindingOperator);
		return 0;
	}

	
	if ((pBindingOperator->GetType() == PM_OPERATOR_TIMER) &&
		(pBindingOperator->GetID() == nTimerId))
	{
		return pBindingOperator;
	}	
	return 0;
}

CBindingOperator* CBindingEvaluationStringSeperatorAlg2::Find(int index,OperatorType Type)
{
	CBindingOperator* pBindingOperator = 0;

	BOOL fResult = FALSE;

	if ((pBindingOperator = m_pBindingOperators->Get(index)) == 0) 
	{
		ASSERT(pBindingOperator);
		return 0;
	}

	if (pBindingOperator->GetType() != Type) {
		return 0;
	}

	switch(Type)
	{
		case PM_OPERATOR_AND:
		{
			if (nCurrentTypeAnd < m_MaxOperatorsTypeAnd)
			{										
				if (nCountTypeAnd == nCurrentTypeAnd)
				{					
					fResult = TRUE;
					nCurrentTypeAnd++;
				}
			}

			nCountTypeAnd++;

		} break;

		case PM_OPERATOR_OR:
		{
			if (nCurrentTypeOr < m_MaxOperatorsTypeOr)
			{				
				if (nCountTypeOr == nCurrentTypeOr)
				{					
					fResult = TRUE;
					nCurrentTypeOr++;
				}
			}		
			
			nCountTypeOr++;

		} break;

		case PM_OPERATOR_XOR:
		{
			if (nCurrentTypeXor < m_MaxOperatorsTypeXor)
			{				
				if (nCountTypeXor == nCurrentTypeXor)
				{					
					fResult = TRUE;
					nCurrentTypeXor++;
				}
			}	

			nCountTypeXor++;

		} break;
					
		default:
		{
			ASSERT(0);
			return 0;

		} break;
	}

	if (fResult == TRUE) 
	{		
		return pBindingOperator;
	}

	return 0;
}