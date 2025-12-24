// BindingFileEntryFormatterG.h: interface for the CBindingFileEntryFormatterG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERG_H__20F25882_899E_4849_8D49_8A84512E128D__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERG_H__20F25882_899E_4849_8D49_8A84512E128D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatter.h"

class CBindingFileEntryFormatterG : public CBindingFileEntryFormatter
{
protected:
	CBindingUnit* const m_pForwardUnit;
	CBindingUnit* const m_pReturnUnit;
public:
	CBindingFileEntryFormatterG(CBindingEntryInfo* pBindingEntryInfo,
								CBindingUnit* pForwardUnit,
								CBindingUnit* pReturnUnit);

	virtual ~CBindingFileEntryFormatterG();

public:
	BOOL Format();

};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERG_H__20F25882_899E_4849_8D49_8A84512E128D__INCLUDED_)
