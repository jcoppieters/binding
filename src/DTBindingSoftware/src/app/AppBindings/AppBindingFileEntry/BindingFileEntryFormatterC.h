// BindingFileEntryFormatterC.h: interface for the CBindingFileEntryFormatterC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERC_H__911A8A49_36AE_4DB4_892F_D773C3F89DBE__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERC_H__911A8A49_36AE_4DB4_892F_D773C3F89DBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatter.h"

class CBindingFileEntryFormatterC : public CBindingFileEntryFormatter  
{
protected:
	CBindingUnit* const m_pForwardUnit;

public:
	CBindingFileEntryFormatterC(CBindingEntryInfo* pBindingEntryInfo,
								CBindingUnit* pForwardUnit);
	virtual ~CBindingFileEntryFormatterC();

public:
	BOOL Format();
};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERC_H__911A8A49_36AE_4DB4_892F_D773C3F89DBE__INCLUDED_)
