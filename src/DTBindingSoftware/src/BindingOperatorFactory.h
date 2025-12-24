// BindingOperatorFactory.h: interface for the CBindingOperatorFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGOPERATORFACTORY_H__A11AA5D5_601B_49A0_83AC_5626B0BE5D08__INCLUDED_)
#define AFX_BINDINGOPERATORFACTORY_H__A11AA5D5_601B_49A0_83AC_5626B0BE5D08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingOperator;
#include "BindingConstants.h"


class CBindingOperatorFactory  
{
public:
	static CBindingOperator* Create(OperatorType type);
	static CBindingOperator* Create(const char c);
	static CBindingOperator* Clone(CBindingOperator* pBindingOperator);

public:
	CBindingOperatorFactory();
	virtual ~CBindingOperatorFactory();

};

#endif // !defined(AFX_BINDINGOPERATORFACTORY_H__A11AA5D5_601B_49A0_83AC_5626B0BE5D08__INCLUDED_)
