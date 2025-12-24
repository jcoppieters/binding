// BindingFileConstruct.h: interface for the CBindingFileConstruct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCT_H__7F0C449D_36B7_45C8_BCE7_3F054B20A428__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCT_H__7F0C449D_36B7_45C8_BCE7_3F054B20A428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntry.h"

#include "BindingEntryInfo.h"
#include "BindingEntry.h"

class CBindingFileConstruct  
{
protected:
	CStringArray* const m_pArrayBindingStrings;
	CBindingEntryInfo* m_pBindingEntryInfo;

public:
	CBindingFileConstruct(CStringArray* pArrayBindingStrings=NULL);	
	virtual ~CBindingFileConstruct();
	static int GetNrOfEntries(
		CStringArray* pStringArray,
		BYTE bNodeAddress);
};

#endif // !defined(AFX_BINDINGFILECONSTRUCT_H__7F0C449D_36B7_45C8_BCE7_3F054B20A428__INCLUDED_)
