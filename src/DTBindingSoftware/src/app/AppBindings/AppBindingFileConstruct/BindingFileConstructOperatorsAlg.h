// BindingFileConstructOperatorsAlg.h: interface for the CBindingFileConstructOperatorsAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTOPERATORSALG_H__B9F3BEBC_E92D_42B8_ABE8_90F3DD21B354__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTOPERATORSALG_H__B9F3BEBC_E92D_42B8_ABE8_90F3DD21B354__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingEntry;

#include "BindingOperators.h"

class CBindingFileConstructOperatorsAlg  
{

protected:
	CBindingEntry* const m_pBindingEntry;
	CBindingOperators* const m_pBindingOperators;

	CBindingOperators m_BindingOperators;

	CStringArray m_StringArray;

public:
	CBindingFileConstructOperatorsAlg(CBindingEntry* pBindingEntry);
	virtual ~CBindingFileConstructOperatorsAlg();

protected:
	BOOL Verify();
	BOOL Add(CString s);

public:
	BOOL Build();
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTOPERATORSALG_H__B9F3BEBC_E92D_42B8_ABE8_90F3DD21B354__INCLUDED_)
