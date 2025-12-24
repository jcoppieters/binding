// BindingEvaluationStringConvertAlg.cpp: implementation of the CBindingEvaluationStringConvertAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEvaluationStringConvertAlg.h"

#include "BindingEvaluationString.h"
#include "BindingEvaluationStringParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringConvertAlg::CBindingEvaluationStringConvertAlg()
{
	//Init();
}

CBindingEvaluationStringConvertAlg::~CBindingEvaluationStringConvertAlg()
{
	Destroy();
}

/**
 *
 */
void CBindingEvaluationStringConvertAlg::Init()
{
	try
	{
		ASSERT( m_pBindingEvaluationString );

		m_pParser = new CBindingEvaluationStringParser(m_pBindingEvaluationString,
													   this);

		m_pBindingNewEvaluationString = new CBindingEvaluationString();

		m_fParseFatalError = FALSE;

	}
	catch ( CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
	}
}

void CBindingEvaluationStringConvertAlg::Destroy()
{
	if (m_pParser != 0)
	{
		delete m_pParser;
		m_pParser = NULL;
	}

	if (m_pBindingNewEvaluationString != 0)
	{
		delete m_pBindingNewEvaluationString;
		m_pBindingNewEvaluationString = NULL;
	}
}

BOOL CBindingEvaluationStringConvertAlg::Result(CBindingEvaluationString* pResult)
{
	*pResult = *m_pBindingNewEvaluationString;
	
	//TRACE1("\nResult : '%s'",pResult->Get());

	return (!m_fParseFatalError);
}


BOOL CBindingEvaluationStringConvertAlg::Start(CBindingEvaluationString* p)
{

	BOOL fResult = FALSE;

	m_pBindingEvaluationString = p;

	//TRACE1("\nStart : '%s'",m_pBindingEvaluationString->Get());

	Init();

	ASSERT(m_pParser);
	
	if (m_pParser != NULL)
	{
		ASSERT(m_pBindingEvaluationString);
	
		m_pBindingNewEvaluationString->Clear();
	
		m_pParser->Start();

		if (m_fParseFatalError == FALSE)
		{								
			fResult = TRUE;
		}
		else
		{
			fResult = FALSE;
		}
	}
	return fResult;
}

/**
 *
 */
void CBindingEvaluationStringConvertAlg::OnInputUnit(unsigned short nID)
{
	m_pBindingNewEvaluationString->AddUnit(nID);
}
void CBindingEvaluationStringConvertAlg::OnOutputUnit(unsigned short nID)
{
	m_pBindingNewEvaluationString->AddUnit(nID);
}
void CBindingEvaluationStringConvertAlg::OnNextUnit(unsigned short nID)
{
	m_pBindingNewEvaluationString->AddUnit(nID);
}
void CBindingEvaluationStringConvertAlg::OnTimer(unsigned short nID)
{
	m_pBindingNewEvaluationString->AddTimer(nID);
}
void CBindingEvaluationStringConvertAlg::OnCharOperator(const char c)
{
	m_pBindingNewEvaluationString->Add(c);	
}
void CBindingEvaluationStringConvertAlg::OnCharSyntax(const char c)
{
	m_pBindingNewEvaluationString->Add(c);	
}
void CBindingEvaluationStringConvertAlg::OnBrackets(const char c)
{
	m_pBindingNewEvaluationString->Add(c);
}
void CBindingEvaluationStringConvertAlg::OnError()
{
	m_fParseFatalError = TRUE;
}


/**
 *
 */
BOOL CBindingEvaluationStringConvertAlg::Replace(CString szOld,CString szNew)
{
	BOOL fResult = FALSE;

	if (szOld.IsEmpty() == FALSE && szNew.IsEmpty() == FALSE)
	{
		szOld.MakeUpper();
		szNew.MakeUpper();

		CString szEvaluationString(m_pBindingNewEvaluationString->Get());
		
		if (szEvaluationString.IsEmpty() == FALSE)
		{
			if (szEvaluationString.Replace(szOld,szNew) != 0)
			{
				m_pBindingNewEvaluationString->Set(szEvaluationString);		
				fResult = TRUE;						
			}			
		}
	}
	return fResult;
}
