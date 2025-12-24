// BindingOperatorsOperations.h: interface for the CBindingOperatorsOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGOPERATORSOPERATIONS_H__3BAE7738_D944_424A_A150_E71FDE2C73BB__INCLUDED_)
#define AFX_BINDINGOPERATORSOPERATIONS_H__3BAE7738_D944_424A_A150_E71FDE2C73BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingOperators.h"

class CBindingOperatorsOperations  
{
protected:
	CBindingOperators* const m_pBindingOperators;

public:
	CBindingOperatorsOperations(CBindingOperators* pBindingOperators = NULL);
	virtual ~CBindingOperatorsOperations();

public:
	BOOL Import(const CBindingOperators* const pSource);
	BOOL Find(const CBindingOperator* const pIn,CBindingOperator** pOut);
};

#endif // !defined(AFX_BINDINGOPERATORSOPERATIONS_H__3BAE7738_D944_424A_A150_E71FDE2C73BB__INCLUDED_)
