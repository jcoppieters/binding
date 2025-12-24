// BindingEvaluationStringSeperator.cpp: implementation of the CBindingEvaluationStringSeperator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingEvaluationStringSeperator.h"

#include "BindingEntry.h"
#include "BindingEvaluationString.h"
#include "BindingEvaluationStringLevel.h"
#include "BindingEvaluationStringSeperatorAlg1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationStringSeperator::CBindingEvaluationStringSeperator(CBindingEntry* pBindingEntry,
																	 CStringArray& strArray,
																	 CBindingEvaluationStringSeperatorAlg* pAlgoritme) :
m_StringArray(strArray),
m_pBindingEvaluationString(pBindingEntry->GetEvaluationString()),
m_szEvaluationString(pBindingEntry->GetEvaluationString()->Get()),
m_pSeperatorAlgoritme(pAlgoritme)
{
	ASSERT(m_pBindingEvaluationString);
	ASSERT(m_szEvaluationString.IsEmpty() == FALSE);
	ASSERT(m_pSeperatorAlgoritme);

	m_StringArray.RemoveAll();	
}

CBindingEvaluationStringSeperator::~CBindingEvaluationStringSeperator()
{
	//m_StringArray = reference to stringarray parent class !!!	
	//m_StringArray.RemoveAll();
}

BOOL CBindingEvaluationStringSeperator::Build()
{	
	ASSERT( m_pSeperatorAlgoritme );

	if (m_pSeperatorAlgoritme == 0) {	// check PreCondition ...
		return FALSE;
	}

	// Start Algoritmes ...
	if (BuildInput() == FALSE) {	
		return FALSE;
	}
	
	if (BuildOutput() == FALSE) {
		return FALSE;
	}
	
	GenerateDebugInfo();
	return TRUE;
}

//////////////////////////////////////////////////////////////
/**
 *
 */
BOOL CBindingEvaluationStringSeperator::BuildInput() 
{
	CString szEvalInputString = m_pBindingEvaluationString->GetInput();
	if (szEvalInputString.IsEmpty() == TRUE) {
		ASSERT(0);
		return FALSE;
	}

	if (1)
	{
		CString szDebug;
		szDebug.Format("Parse Input String - Level 1: '%s'",szEvalInputString);
		Convert.DebugInfo(szDebug);
	}

	m_StringArray.RemoveAll();	

	CString szEvalReplaceIn(szEvalInputString);

	CString szPrevReplace;
	CString szReplace;
	int nLoopCnt = 0;
	CString szTemp;

	CBindingEvaluationStringLevel BindingEvaluationLevel;

	do 
	{				
		if (BindingEvaluationLevel.Format(szEvalReplaceIn) == FALSE) {
			return FALSE;
		}

		szTemp = BindingEvaluationLevel.GetOutputString();
		CString szOperator(m_pSeperatorAlgoritme->FindOperator(szTemp));
		
		//CString szOperator = m_SeperatorAlgoritme.FindOperator(szTemp);
		if (szOperator.IsEmpty() == FALSE) 
		{			
			szEvalReplaceIn.Replace(szTemp,szOperator);
			szReplace = szOperator;
		
			if (szReplace.GetAt(0) == '!') 
			{
				szReplace.TrimLeft('!');
			}
		}
		
		m_StringArray.Add(szTemp);

		if (nLoopCnt != 0) 
		{
			AddOutputToString(szPrevReplace,nLoopCnt-1);
		}

		szPrevReplace = szReplace;
	
	}while ((nLoopCnt++ < 20) &&
		    (Convert.ParseNrOfOperators(szEvalReplaceIn) != 0));

	szTemp.Format("=%s",szEvalReplaceIn);
	m_StringArray.Add(szTemp);

	AddOutputToString(szPrevReplace,nLoopCnt-1);
	return TRUE;
}

BOOL CBindingEvaluationStringSeperator::BuildOutput() 
{
	CString szEvalOutputString(m_pBindingEvaluationString->GetOutput());

	if (szEvalOutputString.IsEmpty() == TRUE) {
		ASSERT(0);
		return FALSE;
	}

	if (1)
	{
		CString szDebugInfo;
		szDebugInfo.Format("Parse Output String - Level 1: '%s'",szEvalOutputString);
		Convert.DebugInfo(szDebugInfo);
	}

	const int nSize = m_StringArray.GetSize();
	if (nSize == 0) 
	{
		return FALSE;
	}

	CString szTemp;
	szTemp.Format("%s%s",szEvalOutputString,
						 m_StringArray.GetAt(nSize-1));

	m_StringArray.SetAt(nSize-1,szTemp);

	return TRUE;
}

BOOL CBindingEvaluationStringSeperator::GenerateDebugInfo() 
{
	for (int i=0;i<m_StringArray.GetSize();i++)
	{
		CString s;
		s.Format("%2d: %s",i,m_StringArray.GetAt(i));
		Convert.DebugInfo(s);
	}
	return FALSE;
}



//////////////////////////////////////////////////////////////////////

BOOL CBindingEvaluationStringSeperator::AddOutputToString(
	CString szOutput,
	int nIndex)
{
	CString szTemp = m_StringArray.GetAt(nIndex);
	szTemp = szOutput + '=' + szTemp;
	m_StringArray.SetAt(nIndex,szTemp);
	return TRUE;
}

CBindingOperator* CBindingEvaluationStringSeperator::GetOperator(
	const CBindingOperators* const pBindingOperators,
	int index,
	CString s)
{
	ASSERT( pBindingOperators );
	if (pBindingOperators != 0)
	{
		char c = '\0';	
		int id = -1;
		const char* const szDataToParse = (LPCTSTR) s;				
		if (sscanf(&szDataToParse[index],"%c%i",&c,&id) == 2) {
			if ('X' == c || 'x' == c || 'Y' == c || 'y' == c) {		
				return (pBindingOperators->Find(id));
			}
		}
	}

	return 0;
}

CBindingUnit* CBindingEvaluationStringSeperator::GetBindingUnit(
	const CBindingUnits* const pBindingUnits,
	int index,
	CString s)
{
	ASSERT( pBindingUnits );
	if (pBindingUnits != 0) {
		char ch = '\0';	
		int id = -1;
		const char* const szDataToParse = (LPCTSTR) s;			
		if (sscanf(&szDataToParse[index],"%c%i",&ch,&id) == 2) {
			if ('U' == ch || 'u' == ch) {		
				return (pBindingUnits->Find(id));
			}
		}
	}
	return 0;
}