// BindingEvaluationStringParser.cpp: implementation of the CBindingEvaluationStringParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEvaluationStringParser.h"

#include "BindingEvaluationString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringParser::CBindingEvaluationStringParser
( 
  CBindingEvaluationString* p,
  IBindingEvaluationStringParserListener *l
) :
m_pBindingEvaluationString(p),
m_pListener(l)
{
	ASSERT(m_pBindingEvaluationString);
	ASSERT(m_pListener);

	m_CurrentIndex = 0;
}

CBindingEvaluationStringParser::~CBindingEvaluationStringParser()
{

}

BOOL CBindingEvaluationStringParser::Start()
{
	if (m_pBindingEvaluationString != NULL)
	{
		CString szEvaluationString(m_pBindingEvaluationString->Get());

		CurrentState = &CBindingEvaluationStringParser::state_input;	// CHANGES_MVS_2008

		const int nLen = szEvaluationString.GetLength();

		for (m_CurrentIndex=0;
			 m_CurrentIndex<nLen;
			 m_CurrentIndex++)
		{
			(this->*CurrentState)(szEvaluationString.GetAt(m_CurrentIndex));				
		}

		return TRUE;
	}
	return FALSE;
}


void CBindingEvaluationStringParser::state_input(const char c)
{
	switch(c)
	{
		case 'U':
		{
			unsigned short nID;
			if (m_pBindingEvaluationString->ParseUnit(m_CurrentIndex,&nID) == TRUE)
			{			
				m_pListener->OnInputUnit(nID);
			}
			else
			{
				m_pListener->OnError();
			}

		} break;

		case 'T':
		{
			unsigned short nID;
			if (m_pBindingEvaluationString->ParseTimer(m_CurrentIndex,&nID) == TRUE)
			{
				m_pListener->OnTimer(nID);
			}
			else
			{
				m_pListener->OnError();
			}
		} break;

		case '!':
		case '*':
		case '+':
		case '^':
		{
			m_pListener->OnCharOperator(c);

		} break;

		case ')':
		case '(':
		{
			m_pListener->OnBrackets(c);

		} break;
		
		case '=':
		{
			m_pListener->OnCharSyntax(c);

			CurrentState = &CBindingEvaluationStringParser::state_output;	// CHANGES_MVS_2008

		} break;

		case '>':
		{
			m_pListener->OnCharSyntax(c);

			m_pListener->OnError();

		} break;
	}
}
void CBindingEvaluationStringParser::state_output(const char c)
{
	switch(c)
	{
		case 'U':
		{
			unsigned short nID;
			if (m_pBindingEvaluationString->ParseUnit(m_CurrentIndex,&nID) == TRUE)
			{
				m_pListener->OnOutputUnit(nID);
			}
			else
			{
				m_pListener->OnError();
			}

		} break;

		case '>':
		{
			m_pListener->OnCharSyntax(c);

			CurrentState = &CBindingEvaluationStringParser::state_next;		// CHANGES_MVS_2008

		} break;

		case '(':
		case ')':
		case 'T' :
		case '=':
		{
			ASSERT(0);

		} break;
	}
}

void CBindingEvaluationStringParser::state_next(const char c)
{
	switch(c)
	{
		case 'U':
		{
			unsigned short nID;
			if (m_pBindingEvaluationString->ParseUnit(m_CurrentIndex,&nID) == TRUE)
			{
				m_pListener->OnNextUnit(nID);
			}
			else
			{
				m_pListener->OnError();
			}

		} break;

		case '>':
		{
			m_pListener->OnCharSyntax(c);

		} break;
	
		case '(':
		case ')':
		case 'T' :
		case '=':
		{
			ASSERT(0);

		} break;
	}
}
