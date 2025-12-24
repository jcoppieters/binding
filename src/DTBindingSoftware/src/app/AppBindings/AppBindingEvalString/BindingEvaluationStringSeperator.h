// BindingEvaluationStringSeperator.h: interface for the CBindingEvaluationStringSeperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATOR_H__70330D61_568D_4BCF_AEC9_E1D0A0F4B087__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGSEPERATOR_H__70330D61_568D_4BCF_AEC9_E1D0A0F4B087__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingConvertHelper.h"
#include "BindingEvaluationStringSeperatorAlg.h"
#include "BindingEvaluationStringSeperatorAlg1.h"


class CBindingEntry;
class CBindingEvaluationString;
class CBindingEvaluationStringSeperatorAlg;
class CBindingOperators;
class CBindingOperator;
class CBindingUnit;
class CBindingUnits;


class CBindingEvaluationStringSeperator  
{
protected:
	CBindingConvertHelper Convert;
	CBindingEvaluationStringSeperatorAlg* m_pSeperatorAlgoritme;

	CStringArray& m_StringArray;
	CBindingEvaluationString* const m_pBindingEvaluationString;
	CString m_szEvaluationString;

protected:
	BOOL BuildInput();
	BOOL BuildOutput();
	BOOL GenerateDebugInfo();

protected:
	void Debug(CString szMessage);
	BOOL AddOutputToString(CString szOutput,int nIndex);

public:
	CBindingEvaluationStringSeperator(CBindingEntry* pBindingEntry,
									  CStringArray& strArray,
									  CBindingEvaluationStringSeperatorAlg* pAlgoritme);

	virtual ~CBindingEvaluationStringSeperator();

public:
	BOOL Build();

public:
	static CBindingOperator* GetOperator(const CBindingOperators* const pBindingOperators,
									     int index,
										 CString s);

	static CBindingUnit* GetBindingUnit(const CBindingUnits* const pBindingUnits,
										int index,
										CString s);
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGSEPERATOR_H__70330D61_568D_4BCF_AEC9_E1D0A0F4B087__INCLUDED_)
