// BindingFileEntryParserP.h: interface for the CBindingFileEntryParserP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERP_H__A9ECCB4C_C0EA_41A1_AA8D_E571F6CEFAF5__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERP_H__A9ECCB4C_C0EA_41A1_AA8D_E571F6CEFAF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParser.h"

class CBindingFileEntryParserP : public CBindingFileEntryParser  
{
	friend class CBindingFileEntry;

protected:
	CBindingFileEntryParserP();

public:
	virtual ~CBindingFileEntryParserP();

protected:
	virtual BOOL IsTypeOf();

public:
	virtual BOOL IsTypeP() const {
		return TRUE;
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERP_H__A9ECCB4C_C0EA_41A1_AA8D_E571F6CEFAF5__INCLUDED_)
