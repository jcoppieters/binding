// BindingEvaluationStringConvertAlgRBIO.h: interface for the CBindingEvaluationStringConvertAlgRBIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGCONVERTALGRBIO_H__2F669CE0_A639_41DE_B92A_9CF1576DD5D6__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGCONVERTALGRBIO_H__2F669CE0_A639_41DE_B92A_9CF1576DD5D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingEvaluationStringConvertAlg.h"

/**
 * RBIO : Remove Brackets Inverse Operator
 * (!Tx) --> !Tx
 * (!Ux) --> !Ux
 *
 */
class CBindingEvaluationStringConvertAlgRBIO : public CBindingEvaluationStringConvertAlg  
{
protected:
	unsigned short m_nTimerID;
	unsigned short m_nUnitID;

	typedef enum
	{
		STATE_IDLE = 0,
		STATE_BRACKETS_OPEN,
		STATE_PARSING_INVERSE_OPERATOR,
		STATE_PARSING_UNIT,
		STATE_PARSING_TIMER

	} ENUM_PARSING_STATE;

	ENUM_PARSING_STATE m_CurrentState;

public:
	CBindingEvaluationStringConvertAlgRBIO();
	virtual ~CBindingEvaluationStringConvertAlgRBIO();

public:
	virtual void OnInputUnit(unsigned short nID);	
	virtual void OnOutputUnit(unsigned short nID);
	virtual void OnNextUnit(unsigned short nID);
	virtual void OnTimer(unsigned short nID);
	virtual void OnCharOperator(const char c);
	virtual void OnCharSyntax(const char c);
	virtual void OnBrackets(const char c);
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGCONVERTALGRBIO_H__2F669CE0_A639_41DE_B92A_9CF1576DD5D6__INCLUDED_)
