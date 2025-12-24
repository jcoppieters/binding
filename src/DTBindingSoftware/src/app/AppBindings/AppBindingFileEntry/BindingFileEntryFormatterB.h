// BindingFileEntryFormatterB.h: interface for the CBindingFileEntryFormatterB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERB_H__B0365D97_E0EC_43A6_9927_9092D31AB8D6__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERB_H__B0365D97_E0EC_43A6_9927_9092D31AB8D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatter.h"

class CBindingFileEntryFormatterB : public CBindingFileEntryFormatter  
{
protected:
	CBindingUnit* const m_pReturnUnit;
	CBindingUnit* const m_pForwardUnit;
public:
	CBindingFileEntryFormatterB(CBindingEntryInfo* pBindingEntryInfo,
								CBindingUnit* pForwardUnit,
								CBindingUnit* pReturnUnit);
	virtual ~CBindingFileEntryFormatterB();

public:
	BOOL Format();
};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERB_H__B0365D97_E0EC_43A6_9927_9092D31AB8D6__INCLUDED_)
