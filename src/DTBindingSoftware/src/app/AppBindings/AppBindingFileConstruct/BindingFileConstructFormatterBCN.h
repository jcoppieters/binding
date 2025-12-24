// BindingFileConstructFormatterBCN.h: interface for the CBindingFileConstructFormatterBCN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBCN_H__99F2560D_4E5B_4511_A255_7C45C9721BA7__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTERBCN_H__99F2560D_4E5B_4511_A255_7C45C9721BA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatterBC.h"

class CBindingFileConstructFormatterBCN : public CBindingFileConstructFormatterBC  
{
protected:
	BOOL FormatCNBindings();

public:
	CBindingFileConstructFormatterBCN(CBindingEntryInfo* pBindingEntryInfo = NULL,
									  CStringArray* pArrayBindingStrings = NULL);

	virtual ~CBindingFileConstructFormatterBCN();

public:
	virtual BOOL Format();
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBCN_H__99F2560D_4E5B_4511_A255_7C45C9721BA7__INCLUDED_)
