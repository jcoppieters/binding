// BindingFileEntryFormatterNN.h: interface for the CBindingFileEntryFormatterNN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYFORMATTERNN_H__CEDFDD0E_26AB_4914_A994_D0F9AB700A2A__INCLUDED_)
#define AFX_BINDINGFILEENTRYFORMATTERNN_H__CEDFDD0E_26AB_4914_A994_D0F9AB700A2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryFormatterN.h"

class CBindingFileEntryFormatterNN : public CBindingFileEntryFormatterN
{
protected:
	CBindingUnit* const m_pNextUnit;

public:
	CBindingFileEntryFormatterNN(CBindingEntryInfo* pBindingEntryInfo,
								 CBindingUnit* pForwardUnit,
								 CBindingUnit* pReturnUnit,
								 CBindingUnit* pNextUnit);
	virtual ~CBindingFileEntryFormatterNN();

public:
	BOOL Format();

};

#endif // !defined(AFX_BINDINGFILEENTRYFORMATTERNN_H__CEDFDD0E_26AB_4914_A994_D0F9AB700A2A__INCLUDED_)
