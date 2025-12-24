// BindingEvaluationStringSeperatorAlg.h: interface for the CBindingEvaluationStringSeperatorAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATORALG_H__46E712B8_5860_488B_B270_E74F6398C5E1__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGSEPERATORALG_H__46E712B8_5860_488B_B270_E74F6398C5E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingConvertHelper.h"


class CBindingEntry;
class CBindingOperator;
class CBindingOperators;


class CBindingEvaluationStringSeperatorAlg  
{
protected:
	CBindingOperators* const m_pBindingOperators;
	CBindingConvertHelper Convert;

public:
	CBindingEvaluationStringSeperatorAlg(CBindingEntry* pBindingEntry = 0);
	virtual ~CBindingEvaluationStringSeperatorAlg();

protected:
	CString Format(const CBindingOperator* const pBindingOperator,BOOL fInverse);

public:
	virtual CString FindOperator(CString szDataIn) 
	{
		return CString("");
	}
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATORALG_H__46E712B8_5860_488B_B270_E74F6398C5E1__INCLUDED_)
