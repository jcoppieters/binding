// BindingFileConstructFactory.h: interface for the CBindingFileConstructFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFACTORY_H__4ABD5F5F_1862_424B_9310_1A5934E9DF6D__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFACTORY_H__4ABD5F5F_1862_424B_9310_1A5934E9DF6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatter.h"
#include "BindingFileConstructFormatterBC.h"
#include "BindingFileConstructFormatterBG.h"
#include "BindingFileConstructFormatterBCN.h"
#include "BindingFileConstructFormatterBGN.h"
#include "BindingFileConstructFormatterI.h"

class CBindingEntryInfo;

class CBindingFileConstructFactory  
{
protected:
	CBindingEntryInfo* const m_pBindingEntryInfo;
	CStringArray* const m_pArrayBindingStrings;
	CBindingFileConstructFormatter* m_pBindingFileConstruct;

public:
	CBindingFileConstructFactory(
		CBindingEntryInfo* pBindingEntryInfo,
		CStringArray* pArrayBindingStrings);

	virtual ~CBindingFileConstructFactory();
	CBindingFileConstructFormatter* Create();
};


#endif // !defined(AFX_BINDINGFILECONSTRUCTFACTORY_H__4ABD5F5F_1862_424B_9310_1A5934E9DF6D__INCLUDED_)
