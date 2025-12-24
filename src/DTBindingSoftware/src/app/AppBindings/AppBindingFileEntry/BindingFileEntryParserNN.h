// BindingFileEntryParserNN.h: interface for the CBindingFileEntryParserNN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERNN_H__FA9B8ACF_C458_4AB1_AB29_722FE513F782__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERNN_H__FA9B8ACF_C458_4AB1_AB29_722FE513F782__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParserN.h"

class CBindingFileEntryParserNN : public CBindingFileEntryParserN  
{
	friend class CBindingFileEntry;

protected:
	CBindingFileEntryParserNN();

public:
	virtual ~CBindingFileEntryParserNN();

protected:
	virtual BOOL IsTypeOf();

public:
	virtual BOOL IsTypeN() const {
		return FALSE;
	}
	virtual BOOL IsTypeNN() const {
		return TRUE;
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERNN_H__FA9B8ACF_C458_4AB1_AB29_722FE513F782__INCLUDED_)
