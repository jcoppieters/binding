// BindingFileEntryParserB.h: interface for the CBindingFileEntryParserB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERB_H__8D73BDCD_1395_4831_AE55_7B2BA6065C6A__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERB_H__8D73BDCD_1395_4831_AE55_7B2BA6065C6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParser.h"

class CBindingFileEntryParserB : public CBindingFileEntryParser  
{
	friend class CBindingFileEntry;

protected:
	CBindingUnit m_ReturnUnit;

protected:
	CBindingFileEntryParserB();

public:
	virtual ~CBindingFileEntryParserB();

protected:

	virtual BOOL IsTypeOf();

	/* FSM */
	virtual BOOL ParseReturnUnit(const char c);
	virtual BOOL ParseForwardUnit(const char c);

public:

	//BOOL IsTypeOf(CString s);

public:
	CBindingUnit* GetForwardUnit();
	CBindingUnit* GetReturnUnit();

	virtual BOOL IsTypeB() const{
		return TRUE;
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERB_H__8D73BDCD_1395_4831_AE55_7B2BA6065C6A__INCLUDED_)
