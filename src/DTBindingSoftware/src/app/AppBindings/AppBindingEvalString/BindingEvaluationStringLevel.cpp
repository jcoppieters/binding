// BindingEvaluationStringLevel.cpp: implementation of the BindingEvaluationStringLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingEvaluationStringLevel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char CBindingEvaluationStringLevel::convertLevelToChar(int i)
{
	char c = '\0';
	if (i < 10) c = i + '0';
	else if (i < ('Z'-'A')) c = i + 'A' - 10;

	return c;
}

int CBindingEvaluationStringLevel::convertCharToLevel(char c)
{
	int i=0;
	if ((c >= '0') && (c <= '9')) i = c - '0';
	else if ((c >= 'A') && (c <= 'Z')) i = c - 'A' + 10;
	return i;
}
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringLevel::CBindingEvaluationStringLevel()
{

}

CBindingEvaluationStringLevel::~CBindingEvaluationStringLevel()
{

}

BOOL CBindingEvaluationStringLevel::Format(CString szInputString)
{
	if (szInputString.IsEmpty() == TRUE) {
		return FALSE;
	}

	m_szInputString = szInputString;
	const int nLength = m_szInputString.GetLength();

	m_szLevelString.Empty();
	m_nDeepestLevel=0;
	
	char cPrev='\0';
	for (int i=0,k=1;
		 i<nLength;
		 i++)
	{
		const char c= m_szInputString.GetAt(i);
		char cLevel = convertLevelToChar(k);

		switch(c)
		{
			case '(':
				k++;					
				cLevel = convertLevelToChar(k);
				cPrev = '(';
			break;

			case ')':			
				cLevel = convertLevelToChar(k);
				k--;
				cPrev = ')';
			break;
			
			case 'T':			
				if (cPrev == 'T') k++;
				cLevel = convertLevelToChar(k);
				cPrev = 'T';
			break;

			case 'U':						
				cLevel = convertLevelToChar(k);			
			break;

			case '+':
			case '^':
			case '*':				
				if (cPrev == 'T') k--;
				cLevel = convertLevelToChar(k);
				cPrev = c;
			break;
		}			

		m_szLevelString += cLevel;

		if (k >= m_nDeepestLevel)	
		{
			m_nDeepestLevel = k;
		}
	}
	
	
	if (1)
	{
		CString szDebugInfo;
		szDebugInfo.Format("String : '%s'",m_szInputString);
		Convert.DebugInfo(szDebugInfo);
	}

	if (1)
	{
		CString szDebugInfo;
		szDebugInfo.Format("Level String : '%s'",m_szLevelString);
		Convert.DebugInfo(szDebugInfo);
	}

	return TRUE;
}

CString CBindingEvaluationStringLevel::GetOutputString()
{
	CString szReturn;

	if (m_szInputString.IsEmpty() == TRUE) {
		return CString("");
	}

	BOOL found = FALSE;
	for (int i=0;i<m_szInputString.GetLength();i++) 
	{		
		if (m_nDeepestLevel == convertCharToLevel(m_szLevelString.GetAt(i))) 
		{
			szReturn += m_szInputString.GetAt(i);				
			found = TRUE;
		}
		else 
		{			
			if (found==TRUE) break;
		}
	}

	return szReturn;
}
