// BindingOperatorOperations.h: interface for the CBindingOperatorOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGOPERATOROPERATIONS_H__B3523410_1CD7_4C01_A5EA_7CCF7FEAE789__INCLUDED_)
#define AFX_BINDINGOPERATOROPERATIONS_H__B3523410_1CD7_4C01_A5EA_7CCF7FEAE789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingOperator;


class CBindingOperatorOperations  
{
protected:
	CBindingOperator* const m_pBindingOperator;

public:
	CBindingOperatorOperations(CBindingOperator* const pBindingOperator);
	virtual ~CBindingOperatorOperations();
	CString GetFunction();
};

#endif // !defined(AFX_BINDINGOPERATOROPERATIONS_H__B3523410_1CD7_4C01_A5EA_7CCF7FEAE789__INCLUDED_)
