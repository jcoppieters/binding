// BindingOperators.h: interface for the CBindingOperators class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGOPERATORS_H__C9953FA5_765C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGOPERATORS_H__C9953FA5_765C_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////
#include "BindingConstants.h"
#include "BindingOperator.h"

////////////////////////////////////////
class CBindingOperators  
{
public:
	CBindingOperators();
	virtual ~CBindingOperators();

	CBindingOperators(const CBindingOperators& BindingOperators);
	CBindingOperators& operator=(const CBindingOperators& BindingOperators);

	unsigned short Count(void) const;
	OperatorType GetType(unsigned short nID) const;

	CBindingOperator* Find(unsigned short nID) const;
	CBindingOperator* Get(unsigned short nEntry) const;

	unsigned short Add(CBindingOperator *pUnit);

	BOOL Remove(unsigned short nID);
	void RemoveAll(void);

private:
	CPtrList List;

protected:
	void Copy(const CBindingOperators* const pSource);

private:
	unsigned short GetNextAvailableID(void);
};

#endif // !defined(AFX_BINDINGOPERATORS_H__C9953FA5_765C_11D8_B865_0050BAC412B1__INCLUDED_)
