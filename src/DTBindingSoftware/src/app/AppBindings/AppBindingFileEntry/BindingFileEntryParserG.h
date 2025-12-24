// BindingFileEntryParserG.h: interface for the CBindingFileEntryParserG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERG_H__AAB2666E_DE34_4E98_9992_E674020AA6B2__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERG_H__AAB2666E_DE34_4E98_9992_E674020AA6B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParser.h"

class CBindingFileEntryParserG : public CBindingFileEntryParser  
{
	friend class CBindingFileEntry;

protected:
	CBindingUnit m_ReturnUnit;

protected:
	CBindingFileEntryParserG();

public:
	virtual ~CBindingFileEntryParserG();

protected:
	virtual BOOL IsTypeOf();
	virtual BOOL ParseReturnUnit(const char c);

public:
	virtual BOOL IsTypeG() const {
		return TRUE;
	}
	CBindingUnit* GetReturnUnit() {
		return &m_ReturnUnit;
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERG_H__AAB2666E_DE34_4E98_9992_E674020AA6B2__INCLUDED_)
