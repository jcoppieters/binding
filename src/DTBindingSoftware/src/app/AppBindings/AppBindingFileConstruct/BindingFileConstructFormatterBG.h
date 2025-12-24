// BindingFileConstructFormatterBG.h: interface for the CBindingFileConstructFormatterBG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBG_H__A1C89787_844A_44E3_8307_25500C6D727E__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTERBG_H__A1C89787_844A_44E3_8307_25500C6D727E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatter.h"

class CBindingFileConstructFormatterBG : public CBindingFileConstructFormatter  
{
protected:
	CBindingUnit* m_pForwardUnit;

protected:
	BOOL FormatGBindings();

public:
	CBindingFileConstructFormatterBG(CBindingEntryInfo* pBindingEntryInfo = NULL,
									 CStringArray* pArrayBindingStrings = NULL);

	virtual ~CBindingFileConstructFormatterBG();

public:
	virtual BOOL Format();
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBG_H__A1C89787_844A_44E3_8307_25500C6D727E__INCLUDED_)
