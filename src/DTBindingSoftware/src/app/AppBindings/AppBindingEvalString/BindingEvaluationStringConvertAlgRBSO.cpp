// BindingEvaluationStringConvertAlgRBSO.cpp: implementation of the CBindingEvaluationStringConvertAlgRBSO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEvaluationStringConvertAlgRBSO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringConvertAlgRBSO::CBindingEvaluationStringConvertAlgRBSO()
{
	m_CurrentState = STATE_IDLE;
}

CBindingEvaluationStringConvertAlgRBSO::~CBindingEvaluationStringConvertAlgRBSO()
{

}

void CBindingEvaluationStringConvertAlgRBSO::OnInputUnit(unsigned short nID)
{
	this->CBindingEvaluationStringConvertAlg::OnInputUnit(nID);

	if (m_CurrentState == STATE_BRACKETS_OPEN)
	{
		m_nUnitID = nID;
		m_CurrentState = STATE_PARSING_UNIT;
	}
	else
	{
		m_CurrentState = STATE_IDLE;
	}
}

void CBindingEvaluationStringConvertAlgRBSO::OnOutputUnit(unsigned short nID)
{
	this->CBindingEvaluationStringConvertAlg::OnOutputUnit(nID);
	m_CurrentState = STATE_IDLE;
}

void CBindingEvaluationStringConvertAlgRBSO::OnNextUnit(unsigned short nID)
{
	this->CBindingEvaluationStringConvertAlg::OnNextUnit(nID);
	m_CurrentState = STATE_IDLE;
}

void CBindingEvaluationStringConvertAlgRBSO::OnTimer(unsigned short nID)
{
	this->CBindingEvaluationStringConvertAlg::OnTimer(nID);

	if (m_CurrentState == STATE_BRACKETS_OPEN)
	{
		m_nTimerID = nID;
		m_CurrentState = STATE_PARSING_TIMER;
	}
	else
	{
		m_CurrentState = STATE_IDLE;
	}
}

void CBindingEvaluationStringConvertAlgRBSO::OnCharOperator(const char c)
{
	this->CBindingEvaluationStringConvertAlg::OnCharOperator(c);
	m_CurrentState = STATE_IDLE;
}

void CBindingEvaluationStringConvertAlgRBSO::OnCharSyntax(const char c)
{
	this->CBindingEvaluationStringConvertAlg::OnCharSyntax(c);
	m_CurrentState = STATE_IDLE;
}

void CBindingEvaluationStringConvertAlgRBSO::OnBrackets(const char c)
{
	this->CBindingEvaluationStringConvertAlg::OnBrackets(c);

	if (c == '(')
	{
		if (m_CurrentState == STATE_IDLE)
		{
			m_CurrentState = STATE_BRACKETS_OPEN;
		}
		else if (m_CurrentState == STATE_BRACKETS_OPEN)
		{
			// Stay in this state !!!
		}
		else
		{
			m_CurrentState = STATE_IDLE;
		}
	}
	else if (c == ')')
	{
		if (m_CurrentState == STATE_PARSING_TIMER)
		{
			CString szOld;
			szOld.Format("(T%i)",m_nTimerID);
			CString szNew;
			szNew.Format("T%i",m_nTimerID);

			if (Replace(szOld,szNew) == FALSE) {
				m_fParseFatalError = TRUE;
			}
	
			//TRACE1("\nszOld: %s\n",szOld);
			//TRACE1("szNew: %s\n",szNew);
		}
		else if (m_CurrentState == STATE_PARSING_UNIT)
		{
			CString szOld;
			szOld.Format("(U%i)",m_nUnitID);
			CString szNew;
			szNew.Format("U%i",m_nUnitID);

			if (Replace(szOld,szNew) == FALSE) {
				m_fParseFatalError = TRUE;
			}
		
			//TRACE1("\nszOld: %s\n",szOld);
			//TRACE1("szNew: %s\n",szNew);
		}

		m_CurrentState = STATE_IDLE;
	}
}
