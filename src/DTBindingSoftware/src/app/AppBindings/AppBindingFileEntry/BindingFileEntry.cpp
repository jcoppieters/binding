// BindingFileEntry.cpp: implementation of the CBindingFileEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileEntry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileEntry::CBindingFileEntry(CString s) :
m_szBindingString(s)
{
	m_pBindingFileEntryTypes[0] = &m_BindingFileEntryTypeB;
	m_pBindingFileEntryTypes[1] = &m_BindingFileEntryTypeC;
	m_pBindingFileEntryTypes[2] = &m_BindingFileEntryTypeG;
	m_pBindingFileEntryTypes[3] = &m_BindingFileEntryTypeI;
	m_pBindingFileEntryTypes[4] = &m_BindingFileEntryTypeCN;
	m_pBindingFileEntryTypes[5] = &m_BindingFileEntryTypeGN;
	m_pBindingFileEntryTypes[6] = &m_BindingFileEntryTypeN;
	m_pBindingFileEntryTypes[7] = &m_BindingFileEntryTypeNN;
	m_pBindingFileEntryTypes[8] = &m_BindingFileEntryTypeP;
}

CBindingFileEntry::~CBindingFileEntry()
{

}

/**
 * use of Pattern Chain of Responsibilities ...
 */
CBindingFileEntryParser* CBindingFileEntry::Parse()
{
	CBindingFileEntryParser* pBindingFileEntryType = NULL;

	for (int i=0;i<MAX_BINDINGENTRY_TYPES;i++)
	{
		if (m_pBindingFileEntryTypes[i]->Parse(m_szBindingString) == TRUE) 
		{
			pBindingFileEntryType = m_pBindingFileEntryTypes[i];
			break;
		}
	}
	return pBindingFileEntryType;
}



