// BindingEvaluationStringParser.h: interface for the CBindingEvaluationStringParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGPARSER_H__F41CCDC8_6EBE_4084_852B_67331A5E114E__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGPARSER_H__F41CCDC8_6EBE_4084_852B_67331A5E114E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingEvaluationString.h"
#include "IBindingEvaluationStringParserListener.h"


/**
 *
 */
class CBindingEvaluationStringParser  
{
protected:
	CBindingEvaluationString* const m_pBindingEvaluationString;
	IBindingEvaluationStringParserListener* const m_pListener;
	int m_CurrentIndex;

public:
	CBindingEvaluationStringParser(CBindingEvaluationString* p,
								   IBindingEvaluationStringParserListener* l);

	virtual ~CBindingEvaluationStringParser();

protected:
	/**
	 * FSM ...
	 */
	
	typedef void (CBindingEvaluationStringParser::*ActionState) (const char c);

	ActionState CurrentState;

	void state_input(const char c);
	void state_output(const char c);
	void state_next(const char c);


public:
	BOOL Start();

};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGPARSER_H__F41CCDC8_6EBE_4084_852B_67331A5E114E__INCLUDED_)
