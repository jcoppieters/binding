// BindingEvaluationStringSeperatorAlg1.h: interface for the CBindingEvaluationStringSeperatorAlg1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATORALG1_H__17A5C97C_A3EB_4438_9F0D_BD7AC4EC79E9__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGSEPERATORALG1_H__17A5C97C_A3EB_4438_9F0D_BD7AC4EC79E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingConvertHelper.h"
#include "BindingEvaluationStringSeperatorAlg.h"

class CBindingOperators;
class CBindingLinks;
class CBindingEntry;
class CBindingOperator;

class CBindingEvaluationStringSeperatorAlg1 : public CBindingEvaluationStringSeperatorAlg
{
protected:
	//CBindingOperators* const m_pBindingOperators;
	CBindingLinks* const m_pBindingLinks;
	//CBindingConvertHelper Convert;

public:
	CBindingEvaluationStringSeperatorAlg1(CBindingEntry* pBindingEntry);
	virtual ~CBindingEvaluationStringSeperatorAlg1();

protected:
	CBindingOperator* Find(char cOperator,int nIndex,int nTimerId = 0);
	BOOL CorrectLinked(CString szDataIn,const CBindingOperator* const pBindingOperator);
	//CString Format(const CBindingOperator* const pBindingOperator,BOOL fInverse);

public:
	virtual CString FindOperator(CString szDataIn);
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATORALG1_H__17A5C97C_A3EB_4438_9F0D_BD7AC4EC79E9__INCLUDED_)
