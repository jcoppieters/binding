// BindingFileConstructFormatterI.h: interface for the CBindingFileConstructFormatterI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTERI_H__6E45749D_5B3D_46B0_8CBD_178FE7371C67__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTERI_H__6E45749D_5B3D_46B0_8CBD_178FE7371C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatter.h"

class CBindingFileConstructFormatterI : public CBindingFileConstructFormatter  
{
public:
	CBindingFileConstructFormatterI(CBindingEntryInfo* pBindingEntryInfo = NULL,
									CStringArray* pArrayBindingStrings = NULL);
	virtual ~CBindingFileConstructFormatterI();

public:
	virtual BOOL Format();
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTERI_H__6E45749D_5B3D_46B0_8CBD_178FE7371C67__INCLUDED_)
