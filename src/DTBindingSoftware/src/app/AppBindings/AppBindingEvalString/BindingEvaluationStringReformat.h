// BindingEvaluationStringReformat.h: interface for the CBindingEvaluationStringReformat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGREFORMAT_H__C574E918_5474_4CA0_9A73_937568EFAB25__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGREFORMAT_H__C574E918_5474_4CA0_9A73_937568EFAB25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingEvaluationString;
class CLog;

#include "BindingEvaluationStringConvertAlg.h"
#include "BindingEvaluationStringConvertAlgRBSO.h"
#include "BindingEvaluationStringConvertAlgRBIO.h"


/**
 * Uses a chain of algoritmes.
 */
class CBindingEvaluationStringReformat  
{
protected:
	#define MAX_ALGORITMEN 2
	CBindingEvaluationStringConvertAlg* Algoritmen[MAX_ALGORITMEN];	
	CBindingEvaluationStringConvertAlgRBSO m_AlgRBSO;
	CBindingEvaluationStringConvertAlgRBIO m_AlgRBIO;

	CBindingEvaluationString* const m_pBindingEvaluationString;
	CLog* const m_pErrorLog;

public:
	CBindingEvaluationStringReformat(CBindingEvaluationString* p);
	virtual ~CBindingEvaluationStringReformat();

	BOOL Start();

	CLog* GetErrorLog() {
		return m_pErrorLog;
	}
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGREFORMAT_H__C574E918_5474_4CA0_9A73_937568EFAB25__INCLUDED_)
