// BindingFileConstructParserN_H.h: interface for the CBindingFileConstructParserN_H class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERN_H_H__E8A28B9D_1389_42EE_AD8A_13FF212FA266__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERN_H_H__E8A28B9D_1389_42EE_AD8A_13FF212FA266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructParser.h"
class CBindingEntry;

class CBindingFileConstructParserN_H : public CBindingFileConstructParser  
{
protected:
	CBindingEntry m_BindingEntry;
	CBindingFileEntryParserN* m_pBindingFileEntryParserN;

public:
	CBindingFileConstructParserN_H(CStringArray* pStringArray);
	virtual ~CBindingFileConstructParserN_H();

public:
	BOOL Construct();

	const CBindingEntry* GetBindingEntry() const {
		return (&m_BindingEntry);
	}

	CBindingEntry* GetBindingEntry() {
		return (&m_BindingEntry);
	}
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERN_H_H__E8A28B9D_1389_42EE_AD8A_13FF212FA266__INCLUDED_)
