// BindingFileEntryFormatterI.h: interface for the CBindingFileEntryFormatterI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERI_H__2CC6F6E5_A8CE_46D0_BEB5_42EB0A4A807E__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERI_H__2CC6F6E5_A8CE_46D0_BEB5_42EB0A4A807E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatter.h"

class CBindingFileEntryFormatterI : public CBindingFileEntryFormatter
{
protected:
	CBindingUnit* const m_pForwardUnit;
	CBindingUnit* const m_pReturnUnit;

public:
	CBindingFileEntryFormatterI(CBindingEntryInfo* pBindingEntryInfo,
								CBindingUnit* pForwardUnit,
								CBindingUnit* pReturnUnit);

	virtual ~CBindingFileEntryFormatterI();

public:
	BOOL Format();

};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERI_H__2CC6F6E5_A8CE_46D0_BEB5_42EB0A4A807E__INCLUDED_)
