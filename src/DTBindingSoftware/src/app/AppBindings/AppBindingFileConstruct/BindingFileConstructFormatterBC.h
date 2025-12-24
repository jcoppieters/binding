// BindingFileConstructFormatterBC.h: interface for the CBindingFileConstructFormatterBC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBC_H__8C4ED987_F752_4DA3_8ACE_1B880F897EF3__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTERBC_H__8C4ED987_F752_4DA3_8ACE_1B880F897EF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatter.h"

class CBindingFileConstructFormatterBC : public CBindingFileConstructFormatter  
{
public:
	CBindingFileConstructFormatterBC(CBindingEntryInfo* pBindingEntryInfo = NULL,
									 CStringArray* pArrayBindingStrings = NULL);

	virtual ~CBindingFileConstructFormatterBC();
	virtual BOOL Format();

protected:
	BOOL FormatCBindings();

protected:
	CBindingUnit* m_pForwardUnit;
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTERBC_H__8C4ED987_F752_4DA3_8ACE_1B880F897EF3__INCLUDED_)
