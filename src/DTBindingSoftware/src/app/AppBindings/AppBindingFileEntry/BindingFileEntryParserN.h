// BindingFileEntryParserN.h: interface for the CBindingFileEntryParserN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERN_H__FE72603E_3399_46F9_8956_A7A390D03BA1__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERN_H__FE72603E_3399_46F9_8956_A7A390D03BA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParser.h"

class CBindingFileEntryParserN : public CBindingFileEntryParser  
{
	friend class CBindingFileEntry;

protected:
	CBindingUnit m_ReturnUnit;

protected:
	CBindingFileEntryParserN();

public:
	virtual ~CBindingFileEntryParserN();

protected:
	virtual BOOL IsTypeOf();
	virtual BOOL ParseReturnUnit(const char c);

public:
	virtual BOOL IsTypeN() const {
		return TRUE;
	}

public:
	CBindingUnit* GetReturnUnit();
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERN_H__FE72603E_3399_46F9_8956_A7A390D03BA1__INCLUDED_)
