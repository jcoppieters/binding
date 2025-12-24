// BindingFileEntryFormatter.cpp: implementation of the CBindingFileEntryFormatter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryFormatter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryFormatter::CBindingFileEntryFormatter(CBindingEntryInfo* pBindingEntryInfo) :
m_pBindingEntryInfo(pBindingEntryInfo)
{
	ASSERT(m_pBindingEntryInfo);

	if (m_pBindingEntryInfo != 0)
	{
		m_pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
		ASSERT(m_pBindingEntry);	
	}

	m_szBindingFileEntry.Empty();
}

CBindingFileEntryFormatter::~CBindingFileEntryFormatter()
{
}

const char* CBindingFileEntryFormatter::ToString() const
{
	return ((LPCTSTR) m_szBindingFileEntry);
}
