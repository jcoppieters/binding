// BindingEvaluationStringSeperatorAlg2.h: interface for the CBindingEvaluationStringSeperatorAlg2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATORALG2_H__414CAED3_90D0_470B_9DA2_1AED9B188BFD__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGSEPERATORALG2_H__414CAED3_90D0_470B_9DA2_1AED9B188BFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingEvaluationStringSeperatorAlg.h"
#include "BindingConvertHelper.h"

class CBindingOperators;
class CBindingOperator;

class CBindingEvaluationStringSeperatorAlg2 : public CBindingEvaluationStringSeperatorAlg  
{
protected:
	//CBindingOperators* const m_pBindingOperators;	
	//CBindingConvertHelper Convert;

	int m_MaxOperatorsTypeAnd,nCurrentTypeAnd,nCountTypeAnd;
	int m_MaxOperatorsTypeOr,nCurrentTypeOr,nCountTypeOr;
	int m_MaxOperatorsTypeXor,nCurrentTypeXor,nCountTypeXor;
	BOOL m_fInit;

public:
	CBindingEvaluationStringSeperatorAlg2(CBindingEntry* pBindingEntry);
	virtual ~CBindingEvaluationStringSeperatorAlg2();

protected:
	CBindingOperator* Find(char cOperator,int nIndex,int nTimerId = 0);
	CBindingOperator* Find(int index,int nTimerId);
	CBindingOperator* Find(int index,OperatorType Type);

	void Init();

public:
	virtual CString FindOperator(CString szDataIn);
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATORALG2_H__414CAED3_90D0_470B_9DA2_1AED9B188BFD__INCLUDED_)
