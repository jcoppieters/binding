// BindingFileConstructFormatterBGN.h: interface for the CBindingFileConstructFormatterBGN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBGN_H__D556EC38_5BBD_418C_BEEF_37FB8A995F2E__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTERBGN_H__D556EC38_5BBD_418C_BEEF_37FB8A995F2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatterBG.h"

class CBindingFileConstructFormatterBGN : public CBindingFileConstructFormatterBG  
{
protected:
	BOOL FormatGNBindings();

public:
	CBindingFileConstructFormatterBGN(CBindingEntryInfo* pBindingEntryInfo = NULL,
									  CStringArray* pArrayBindingStrings = NULL);
	virtual ~CBindingFileConstructFormatterBGN();

public:
	virtual BOOL Format();	

};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBGN_H__D556EC38_5BBD_418C_BEEF_37FB8A995F2E__INCLUDED_)
