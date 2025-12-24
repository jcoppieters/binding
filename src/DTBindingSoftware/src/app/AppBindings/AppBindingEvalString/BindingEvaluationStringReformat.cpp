// BindingEvaluationStringReformat.cpp: implementation of the CBindingEvaluationStringReformat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEvaluationStringReformat.h"

#include "Log.h"
#include "BindingEvaluationString.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringReformat::CBindingEvaluationStringReformat(CBindingEvaluationString* p) :
m_pBindingEvaluationString(p),
m_pErrorLog( new CLog() )
{
	Algoritmen[0] = &m_AlgRBSO;		// Remove Brackets Single Operand (Ux),(Tx)
	Algoritmen[1] = &m_AlgRBIO;		// Remove Brackets Inverse Operator (!Ux), (!Tx)

	ASSERT(m_pErrorLog);
	ASSERT(m_pBindingEvaluationString);
}

CBindingEvaluationStringReformat::~CBindingEvaluationStringReformat()
{
	if (m_pErrorLog != 0)
	{
		delete m_pErrorLog;
	}
}


BOOL CBindingEvaluationStringReformat::Start()
{
	BOOL fResult = FALSE;

	CBindingEvaluationString* pNewBindingEvaluation = m_pBindingEvaluationString;

	CString szOld(m_pBindingEvaluationString->Get());

	for (int i=0;i<MAX_ALGORITMEN;i++)
	{
		fResult = FALSE;
		
		TRACE1("\nOld Eval string: '%s'\n",pNewBindingEvaluation->Get());

		if (Algoritmen[i]->Start(pNewBindingEvaluation) == TRUE)
		{		
			if (Algoritmen[i]->Result(pNewBindingEvaluation) == TRUE)
			{							
				TRACE1("\nNew Eval string: '%s'\n",pNewBindingEvaluation->Get());
				fResult = TRUE;
			}
		}		

		if (fResult == FALSE)
		{
			break;
		}
	}

	if (fResult == TRUE)
	{		
		CString szNew(m_pBindingEvaluationString->Get());

		if (szOld.Compare(szNew) != 0)
		{		
			if (1)
			{
				CString szMessage;
				szMessage.Format("Changed evaluationstring: '%s' to '%s'",szOld,szNew);
				m_pErrorLog->AddWarning(szMessage);
			}
		}
	}
	return fResult;
}
