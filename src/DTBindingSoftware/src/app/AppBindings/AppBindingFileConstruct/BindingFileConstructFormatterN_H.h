// BindingFileConstructFormatterN_H.h: interface for the CBindingFileConstructFormatterN_H class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTERN_H_H__8432E482_3417_4639_9A55_2D3C5DEA1EA5__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTERN_H_H__8432E482_3417_4639_9A55_2D3C5DEA1EA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatter.h"

class CBindingFileConstructFormatterN_H : public CBindingFileConstructFormatter  
{
public:
	CBindingFileConstructFormatterN_H(CBindingEntryInfo* pBindingEntryInfo,
									  CStringArray* pArrayBindingStrings);									 

	virtual ~CBindingFileConstructFormatterN_H();
public:
	BOOL Format();
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTERN_H_H__8432E482_3417_4639_9A55_2D3C5DEA1EA5__INCLUDED_)
