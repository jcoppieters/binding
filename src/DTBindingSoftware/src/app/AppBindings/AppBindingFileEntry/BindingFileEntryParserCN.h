// BindingFileEntryParserCN.h: interface for the CBindingFileEntryParserCN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERCN_H__26F8F6BD_BCA2_4798_82AF_B6CAC2D9DF32__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERCN_H__26F8F6BD_BCA2_4798_82AF_B6CAC2D9DF32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParserC.h"

class CBindingFileEntryParserCN : public CBindingFileEntryParserC  
{
	friend class CBindingFileEntry;

protected:
	CBindingFileEntryParserCN();

public:
	virtual ~CBindingFileEntryParserCN();

protected:
	virtual BOOL IsTypeOf();

public:
	virtual BOOL IsTypeC() const {
		return FALSE;
	}
	virtual BOOL IsTypeCN() const {
		return TRUE;
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERCN_H__26F8F6BD_BCA2_4798_82AF_B6CAC2D9DF32__INCLUDED_)
