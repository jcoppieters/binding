// BindingEvaluationStringLevel.h: interface for the BindingEvaluationStringLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGLEVEL_H__D70A31D3_FD37_4842_98E3_8A30AF392FC5__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGLEVEL_H__D70A31D3_FD37_4842_98E3_8A30AF392FC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingConvertHelper.h"

class CBindingEvaluationStringLevel  
{
protected:
	CBindingConvertHelper Convert;

	CString m_szInputString;
	CString m_szLevelString;
	int m_nDeepestLevel;

protected:
	char convertLevelToChar(int i);
	int convertCharToLevel(char c);

public:
	CBindingEvaluationStringLevel();
	virtual ~CBindingEvaluationStringLevel();

public:
	BOOL Format(CString szInputString);
	CString GetOutputString();

public:
	int GetDeepestLevel() const {
		return m_nDeepestLevel;
	}
	CString GetLevelString() const {
		return m_szLevelString;
	}
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGLEVEL_H__D70A31D3_FD37_4842_98E3_8A30AF392FC5__INCLUDED_)
