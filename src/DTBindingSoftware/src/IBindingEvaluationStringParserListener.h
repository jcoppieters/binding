// IBindingEvaluationStringParserListener.h: interface for the IBindingEvaluationStringParserListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBINDINGEVALUATIONSTRINGPARSERLISTENER_H__01ECC520_72A8_4BC1_BDF2_3DBFABE2B9ED__INCLUDED_)
#define AFX_IBINDINGEVALUATIONSTRINGPARSERLISTENER_H__01ECC520_72A8_4BC1_BDF2_3DBFABE2B9ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IBindingEvaluationStringParserListener  
{
public:
	IBindingEvaluationStringParserListener();
	virtual ~IBindingEvaluationStringParserListener();

public:

	virtual void OnInputUnit(unsigned short ) 
	{
	
	};
	
	virtual void OnOutputUnit(unsigned short )
	{
	
	};

	virtual void OnNextUnit(unsigned short )
	{
	
	};

	virtual void OnTimer(unsigned short )
	{
	
	};

	/**
	 * '+','^','*','!',
	 */
	virtual void OnCharOperator(const char )
	{
	
	};

	/**
	 * '=','>'
	 */
	virtual void OnCharSyntax(const char )
	{
	
	};

	/**
	 * '(', ')'
	 */
	virtual void OnBrackets(const char )
	{
	
	};

	virtual void OnError()
	{
	
	};
};

#endif // !defined(AFX_IBINDINGEVALUATIONSTRINGPARSERLISTENER_H__01ECC520_72A8_4BC1_BDF2_3DBFABE2B9ED__INCLUDED_)
