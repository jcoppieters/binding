// BindingFileEntryParserI.h: interface for the CBindingFileEntryParserI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERI_H__295FAC97_8ABB_4DB6_A0BF_C0EE5D950A98__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERI_H__295FAC97_8ABB_4DB6_A0BF_C0EE5D950A98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParser.h"

class CBindingFileEntryParserI : public CBindingFileEntryParser  
{
	friend class CBindingFileEntry;

protected:
	CBindingUnit m_ReturnUnit;

protected:
	CBindingFileEntryParserI();

public:
	virtual ~CBindingFileEntryParserI();

protected:
	virtual BOOL IsTypeOf();

	/* FSM */
	virtual BOOL ParseReturnUnit(const char c);
	virtual BOOL ParseForwardUnit(const char c);

public:
	virtual BOOL IsTypeI() const{
		return TRUE;
	}

	CBindingUnit* GetReturnUnit() {
		return &m_ReturnUnit;
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERI_H__295FAC97_8ABB_4DB6_A0BF_C0EE5D950A98__INCLUDED_)
