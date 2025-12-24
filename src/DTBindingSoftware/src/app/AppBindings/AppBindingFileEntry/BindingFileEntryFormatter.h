// BindingFileEntryFormatter.h: interface for the CBindingFileEntryFormatter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTER_H__481F9C7E_252D_49BC_A905_DB42A8C7F5D5__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTER_H__481F9C7E_252D_49BC_A905_DB42A8C7F5D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntry.h"
#include "BindingEntryInfo.h"

class CBindingFileEntryFormatter 
{
protected:
	CBindingEntryInfo* const m_pBindingEntryInfo;
	CBindingEntry* m_pBindingEntry;
	CString m_szBindingFileEntry;

public:
	CBindingFileEntryFormatter(CBindingEntryInfo* pBindingEntryInfo = NULL);
	virtual ~CBindingFileEntryFormatter();
	
	CString GetBindingFileEntry() const
	{
		return m_szBindingFileEntry;
	}
	const char* ToString() const;
};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTER_H__481F9C7E_252D_49BC_A905_DB42A8C7F5D5__INCLUDED_)
