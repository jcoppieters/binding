// BindingFileConstructFormatter.h: interface for the CBindingFileConstructFormatter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTER_H__637640D7_524F_48A9_B2D9_2BB482759C0F__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTER_H__637640D7_524F_48A9_B2D9_2BB482759C0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstruct.h"

class CBindingFileConstructFormatter : public CBindingFileConstruct  
{
public:
	CBindingFileConstructFormatter(CBindingEntryInfo* pBindingEntryInfo = NULL,
								   CStringArray* pArrayBindingStrings = NULL);
	virtual ~CBindingFileConstructFormatter();
	virtual BOOL Format();

protected:
	void AddToStringArray(CString s);
	
protected:
	CString m_szEvaluationString;
	CBindingUnits* m_pBindingUnits;
	CBindingEntry* m_pBindingEntry;
	CBindingEvaluationString* m_pBindingEvaluationString;
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTER_H__637640D7_524F_48A9_B2D9_2BB482759C0F__INCLUDED_)
