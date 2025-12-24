// BindingFileEntryFormatterCN.h: interface for the CBindingFileEntryFormatterCN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERCN_H__CC1D8EC3_78C0_4C40_AE72_2766CC1B56AF__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERCN_H__CC1D8EC3_78C0_4C40_AE72_2766CC1B56AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatterC.h"

class CBindingFileEntryFormatterCN : public CBindingFileEntryFormatterC
{
public:
	CBindingFileEntryFormatterCN(CBindingEntryInfo* pBindingEntryInfo,
								 CBindingUnit* pForwardUnit);
	virtual ~CBindingFileEntryFormatterCN();

public:
	BOOL Format();

};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERCN_H__CC1D8EC3_78C0_4C40_AE72_2766CC1B56AF__INCLUDED_)
