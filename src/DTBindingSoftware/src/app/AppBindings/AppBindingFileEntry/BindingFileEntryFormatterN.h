// BindingFileEntryFormatterN.h: interface for the CBindingFileEntryFormatterN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERN_H__03D085C2_F607_4F89_BD9E_30576733CF29__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERN_H__03D085C2_F607_4F89_BD9E_30576733CF29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatter.h"

class CBindingFileEntryFormatterN : public CBindingFileEntryFormatter
{
protected:
	CBindingUnit* const m_pForwardUnit;
	CBindingUnit* const m_pReturnUnit;

public:
	CBindingFileEntryFormatterN(CBindingEntryInfo* pBindingEntryInfo,
								CBindingUnit* pForwardUnit,
								CBindingUnit* pReturnUnit);
	virtual ~CBindingFileEntryFormatterN();

public:
	BOOL Format();
};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERN_H__03D085C2_F607_4F89_BD9E_30576733CF29__INCLUDED_)
