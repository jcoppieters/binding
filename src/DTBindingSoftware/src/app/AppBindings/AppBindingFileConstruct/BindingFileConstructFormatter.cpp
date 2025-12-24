// BindingFileConstructFormatter.cpp: implementation of the CBindingFileConstructFormatter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatter::CBindingFileConstructFormatter(CBindingEntryInfo* pBindingEntryInfo,
															   CStringArray* pArrayBindingStrings) :
CBindingFileConstruct(pArrayBindingStrings)
{
	m_pBindingEntryInfo = pBindingEntryInfo;

	if (m_pBindingEntryInfo != NULL)
	{
		m_pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
		ASSERT(m_pBindingEntry);

		m_pBindingEvaluationString = m_pBindingEntry->GetEvaluationString();
		ASSERT(m_pBindingEvaluationString);

		m_szEvaluationString = m_pBindingEvaluationString->Get();

		m_pBindingUnits = m_pBindingEntry->GetColUnit();
		ASSERT(m_pBindingUnits);
	}
}

CBindingFileConstructFormatter::~CBindingFileConstructFormatter()
{

}

BOOL CBindingFileConstructFormatter::Format()
{
	ASSERT(0);
	return FALSE;
}

void CBindingFileConstructFormatter::AddToStringArray(CString s)
{
	ASSERT(m_pArrayBindingStrings);

	if (m_pArrayBindingStrings != NULL)
	{
		m_pArrayBindingStrings->Add(s);
	}
}




