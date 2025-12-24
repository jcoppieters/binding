// BindingEvaluationStringSeperatorAlg1.cpp: implementation of the CBindingEvaluationStringSeperatorAlg1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingEvaluationStringSeperatorAlg1.h"

#include "BindingEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringSeperatorAlg1::CBindingEvaluationStringSeperatorAlg1(CBindingEntry* pBindingEntry) :
CBindingEvaluationStringSeperatorAlg(pBindingEntry),
m_pBindingLinks(pBindingEntry->GetColLink())
//m_pBindingOperators(pBindingEntry->GetColOperator())
{
	ASSERT(m_pBindingLinks);
	ASSERT(m_pBindingOperators);
}

CBindingEvaluationStringSeperatorAlg1::~CBindingEvaluationStringSeperatorAlg1()
{

}

/**
 *
 */
CString CBindingEvaluationStringSeperatorAlg1::FindOperator(CString szDataIn)
{
	if (szDataIn.IsEmpty() == TRUE) {
		return CString("");
	}

	CBindingOperator BindingOperator;
	CString szOperator;

	/**
	 * if ParseLogOperators returns TRUE, cOperator holds : *,+,^
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

	for (int i=0;i<nMaxEntries;i++)
	{
		CBindingOperator* const pBindingOperator = 
			Find(cOperator,i,nTimerId);

		if (pBindingOperator != 0)
		{
			if (CorrectLinked(szDataIn,pBindingOperator) == TRUE)
			{
				return(Format(pBindingOperator,fInverse));		
			}
		}
	}	

	return CString("");
}

/*
CString CBindingEvaluationStringSeperatorAlg1::Format(const CBindingOperator* const pBindingOperator,BOOL fInverse)
{
	CString szReturn;

	char cOperatorType = '\0';
	Convert.ConvertOperatorType2Char(pBindingOperator->GetType(),
									 &cOperatorType);
	switch(cOperatorType)
	{
		case 'Y':		
		{
			if (fInverse == TRUE) 
			{
				szReturn.Format("!Y%i",pBindingOperator->GetID());	// Must add Inverse '!'				
			}
			else 
			{
				szReturn.Format("Y%i",pBindingOperator->GetID());	// Must add Inverse '!'						
			}		
		} break;

		case 'X':
		{
			szReturn.Format("%c%i",cOperatorType,pBindingOperator->GetID());		

		} break;
	}

	return szReturn;
}
*/

BOOL CBindingEvaluationStringSeperatorAlg1::CorrectLinked(CString szDataIn,const CBindingOperator* const pBindingOperator)
{
	// pOperator holds possible Operator
	const int nInputs = Convert.ParseNInputs(szDataIn);

	// if 1 of the inputs is connected to this operator then 100% it is ok
	// all controls can have only 1 output link !!!			

	char c;
	unsigned short nInputId;
	if (Convert.ParseInputs(szDataIn,&c,&nInputId,0) == FALSE) 
	{
		return FALSE;			
	}

	// nInputId  + c == Controltype
	ControlType InType;
	if ((InType = CBinding2Controls::Convert(c)) == UNDEFINED)
	{
		return FALSE;
	}		

	for (int i=0;i<m_pBindingLinks->Count();i++) 
	{
		// Search for plink with input == nInputId + c
		CBindingLink* const pBindingLink = m_pBindingLinks->Get(i);
		if (pBindingLink != 0)
		{							
			if ((pBindingLink->GetInLinkType() == InType) && 
				(pBindingLink->GetInLinkID() == nInputId)) 
			{
				if (pBindingLink->GetOutLinkID() == 
					pBindingOperator->GetID())
				{
					return TRUE;
				}				
			}
		}
	}				
	return FALSE;
}

CBindingOperator* CBindingEvaluationStringSeperatorAlg1::Find(char cOperator,int index,int nTimerId)
{
	CBindingOperator* pBindingOperator = 0;
	const OperatorType Type = CBindingOperator::Convert(cOperator);

	if (Type == PM_OPERATOR_UNDEFINED) {
		ASSERT(0);
		return 0;
	}

	for (index;index<m_pBindingOperators->Count();index++)
	{
		if ((pBindingOperator = m_pBindingOperators->Get(index)) == 0) {
			ASSERT(pBindingOperator);
			return 0;
		}

		if (Type == PM_OPERATOR_TIMER) 
		{
			if ((pBindingOperator->GetType() == PM_OPERATOR_TIMER) &&
				(pBindingOperator->GetID() == nTimerId))
			{
				return pBindingOperator;
			}
		}
		else
		{
			if (pBindingOperator->GetType() == Type)
			{
				return pBindingOperator;
			}
		}
	}
	return 0;
}