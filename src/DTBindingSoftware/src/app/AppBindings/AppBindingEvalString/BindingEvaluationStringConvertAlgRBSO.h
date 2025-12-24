// BindingEvaluationStringConvertAlgRBSO.h: interface for the CBindingEvaluationStringConvertAlgRBSO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGCONVERTALGRBSO_H__CFD0345A_7F13_40DA_A720_064087E0FACD__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGCONVERTALGRBSO_H__CFD0345A_7F13_40DA_A720_064087E0FACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingEvaluationStringConvertAlg.h"

/**
 * RBSO : Replace Brackets Single Operand.
 * (Ux) --> Ux
 * (Tx) --> Tx
 */

class CBindingEvaluationStringConvertAlgRBSO : public CBindingEvaluationStringConvertAlg  
{
protected:
	unsigned short m_nTimerID;
	unsigned short m_nUnitID;

	typedef enum
	{
		STATE_IDLE = 0,
		STATE_BRACKETS_OPEN,
		STATE_PARSING_UNIT,
		STATE_PARSING_TIMER

	} ENUM_PARSING_STATE;

	ENUM_PARSING_STATE m_CurrentState;

public:
	CBindingEvaluationStringConvertAlgRBSO();
	virtual ~CBindingEvaluationStringConvertAlgRBSO();

public:
	virtual void OnInputUnit(unsigned short nID);	
	virtual void OnOutputUnit(unsigned short nID);
	virtual void OnNextUnit(unsigned short nID);
	virtual void OnTimer(unsigned short nID);
	virtual void OnCharOperator(const char c);
	virtual void OnCharSyntax(const char c);
	virtual void OnBrackets(const char c);
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGCONVERTALGRBSO_H__CFD0345A_7F13_40DA_A720_064087E0FACD__INCLUDED_)
