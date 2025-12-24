// BindingFileEntryFormatterGN.h: interface for the CBindingFileEntryFormatterGN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERGN_H__3CBAE1C4_177D_40C3_9614_4FC5E03B85BA__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERGN_H__3CBAE1C4_177D_40C3_9614_4FC5E03B85BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatterG.h"

class CBindingFileEntryFormatterGN : public CBindingFileEntryFormatterG
{
protected:
	CBindingUnit* const m_pNextUnit;

public:
	CBindingFileEntryFormatterGN(CBindingEntryInfo* pBindingEntryInfo,
								 CBindingUnit* pForwardUnit,
								 CBindingUnit* pReturnUnit,
								 CBindingUnit* pNextUnit);
	virtual ~CBindingFileEntryFormatterGN();

public:
	BOOL Format();
	
};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERGN_H__3CBAE1C4_177D_40C3_9614_4FC5E03B85BA__INCLUDED_)
