// BindingEvaluationStringSeperatorAlg.cpp: implementation of the CBindingEvaluationStringSeperatorAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingEvaluationStringSeperatorAlg.h"

#include "BindingEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringSeperatorAlg::CBindingEvaluationStringSeperatorAlg(CBindingEntry* pBindingEntry) :
m_pBindingOperators(pBindingEntry->GetColOperator())
{
	ASSERT( m_pBindingOperators );
}

CBindingEvaluationStringSeperatorAlg::~CBindingEvaluationStringSeperatorAlg()
{

}

CString CBindingEvaluationStringSeperatorAlg::Format(const CBindingOperator* const pBindingOperator,BOOL fInverse)
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
