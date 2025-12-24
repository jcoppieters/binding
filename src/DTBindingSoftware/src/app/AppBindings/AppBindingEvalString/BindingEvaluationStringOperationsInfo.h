// BindingEvaluationStringOperationsInfo.h: interface for the CBindingEvaluationStringOperationsInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGOPERATIONSINFO_H__CF850D84_2CC2_4CA8_B48E_0A8B4F8B7838__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGOPERATIONSINFO_H__CF850D84_2CC2_4CA8_B48E_0A8B4F8B7838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "IBindingEvaluationStringParserListener.h"
class CBindingEvaluationString;
class CBindingEvaluationStringParser;


class CBindingEvaluationStringOperationsInfo : public IBindingEvaluationStringParserListener  
{
protected:
	BOOL m_fParseError;
	CBindingEvaluationString* const m_pBindingEvaluationString;
	CBindingEvaluationStringParser* m_pParser;

public:
	CBindingEvaluationStringOperationsInfo(CBindingEvaluationString* p);
	virtual ~CBindingEvaluationStringOperationsInfo();

	BOOL Parse();

public:
	unsigned short GetFirstId() const{
		return m_usFirst;
	}
	unsigned short GetLastId() const {
		return m_usLast;
	}

protected:
	unsigned short m_usFirst;
	unsigned short m_usLast;
	unsigned short m_usCntUnits;
	unsigned short m_usCntTimers;

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

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGOPERATIONSINFO_H__CF850D84_2CC2_4CA8_B48E_0A8B4F8B7838__INCLUDED_)
