// BindingEvaluationStringOperationsInfo.cpp: implementation of the CBindingEvaluationStringOperationsInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingEvaluationStringOperationsInfo.h"

#include "BindingEvaluationStringParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringOperationsInfo::CBindingEvaluationStringOperationsInfo(CBindingEvaluationString* p) :
m_pBindingEvaluationString( p )
{
	ASSERT( m_pBindingEvaluationString );

	m_pParser = 0;
}

CBindingEvaluationStringOperationsInfo::~CBindingEvaluationStringOperationsInfo()
{
	if (m_pParser != 0)
	{
		delete m_pParser;
		m_pParser = 0;
	}
}

BOOL CBindingEvaluationStringOperationsInfo::Parse()
{
	BOOL fResult = FALSE;
	m_fParseError = FALSE;

	m_usFirst = 0;
	m_usLast = 0;
	m_usCntUnits = 0;
	m_usCntTimers = 0;

	try
	{
		if (m_pParser == 0)
		{
			m_pParser = new CBindingEvaluationStringParser(m_pBindingEvaluationString,
														   this);
		}

		if ((fResult = m_pParser->Start()) == TRUE)
		{
			if (m_fParseError == FALSE) 
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}			
	}
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();				
		fResult = FALSE;
	}

	return fResult;
}

void CBindingEvaluationStringOperationsInfo::OnInputUnit(unsigned short nID)
{
	if (m_usFirst == 0) 
	{
		m_usFirst = nID;	
	}

	m_usCntUnits++;
}

void CBindingEvaluationStringOperationsInfo::OnOutputUnit(unsigned short nID)
{
	m_usLast = nID;
	m_usCntUnits++;
}

void CBindingEvaluationStringOperationsInfo::OnNextUnit(unsigned short nID)
{
	m_usLast = nID;
	m_usCntUnits++;
}

void CBindingEvaluationStringOperationsInfo::OnTimer(unsigned short /*nID*/)		//CHANGES_MVS_2008_WARNINGS
{
	m_usCntTimers++;
}
void CBindingEvaluationStringOperationsInfo::OnCharOperator(const char c)			
{

}
void CBindingEvaluationStringOperationsInfo::OnCharSyntax(const char c)
{

}
void CBindingEvaluationStringOperationsInfo::OnBrackets(const char c)
{

}
void CBindingEvaluationStringOperationsInfo::OnError()
{
	m_fParseError = TRUE;
}
