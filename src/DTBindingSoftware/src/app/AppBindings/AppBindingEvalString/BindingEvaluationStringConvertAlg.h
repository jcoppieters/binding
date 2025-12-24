// BindingEvaluationStringConvertAlg.h: interface for the CBindingEvaluationStringConvertAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGCONVERTALG_H__4B8D29D1_881A_4BAE_9F9E_CAB08C9E4B13__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGCONVERTALG_H__4B8D29D1_881A_4BAE_9F9E_CAB08C9E4B13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingEvaluationString;
class CBindingEvaluationStringParser;

#include "IBindingEvaluationStringParserListener.h"

/**
 * Abstract base class 
 */
class CBindingEvaluationStringConvertAlg : public IBindingEvaluationStringParserListener  
{
protected:
	CBindingEvaluationString* m_pBindingEvaluationString;
	CBindingEvaluationStringParser* m_pParser;
	CBindingEvaluationString* m_pBindingNewEvaluationString;
	BOOL m_fParseFatalError;

protected:
	void Init();
	void Destroy();

	BOOL Replace(CString szOld,CString szNew);

public:
	CBindingEvaluationStringConvertAlg();
	virtual ~CBindingEvaluationStringConvertAlg();

	BOOL Start(CBindingEvaluationString* p);
	BOOL Result(CBindingEvaluationString* pResult);

public:
	virtual void OnInputUnit(unsigned short nID);	
	virtual void OnOutputUnit(unsigned short nID);
	virtual void OnNextUnit(unsigned short nID);
	virtual void OnTimer(unsigned short nID);
	virtual void OnCharOperator(const char c);
	virtual void OnCharSyntax(const char c);
	virtual void OnBrackets(const char c);
	virtual void OnError();
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGCONVERTALG_H__4B8D29D1_881A_4BAE_9F9E_CAB08C9E4B13__INCLUDED_)
