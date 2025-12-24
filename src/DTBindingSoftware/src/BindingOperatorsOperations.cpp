// BindingOperatorsOperations.cpp: implementation of the CBindingOperatorsOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingOperatorsOperations.h"

#include "BindingOperatorFactory.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingOperatorsOperations::CBindingOperatorsOperations(CBindingOperators* pBindingOperators) :
m_pBindingOperators(pBindingOperators)
{
	ASSERT( m_pBindingOperators );
}

CBindingOperatorsOperations::~CBindingOperatorsOperations()
{

}


/**
 * Destination Units are added to collection, ID's are not the same as source 
 * units.
 */
BOOL CBindingOperatorsOperations::Import(const CBindingOperators* const pSource)
{
	ASSERT(m_pBindingOperators);
	BOOL fResult = FALSE;

	if (m_pBindingOperators != 0)
	{
		const int nCount = pSource->Count();
		for (int i=0;i<nCount;i++)
		{
			fResult = FALSE;

			const CBindingOperator* const pBindingOperator = pSource->Get(i);
			if (pBindingOperator != NULL)
			{
				CBindingOperator* const pNew = 
					CBindingOperatorFactory::Clone(const_cast<CBindingOperator*>(pBindingOperator));
		
				if (pNew != 0)
				{
					pNew->SetID(0);
					const unsigned short nID = m_pBindingOperators->Add(pNew);
					if (nID == 0)
					{
						ASSERT(nID);
						return FALSE;
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


BOOL CBindingOperatorsOperations::Find(const CBindingOperator* const pIn,CBindingOperator** pOut)
{
	ASSERT(m_pBindingOperators);

	*pOut = 0;

	if (m_pBindingOperators != 0 && pIn != 0)
	{
		for (int i=0;i<m_pBindingOperators->Count();i++)
		{
			CBindingOperator* const pBindingOperator = 
				m_pBindingOperators->Get(i);		
			
			if (pBindingOperator != 0)
			{
				if (pBindingOperator->IsEqual(pIn) == TRUE)
				{
					*pOut = pBindingOperator;
					return TRUE;
				}
			}		
		}
	}
	return FALSE;
}