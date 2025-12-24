// BindingEntryFactory.h: interface for the CBindingEntryFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGENTRYFACTORY_H__02CB8F3F_C09A_4F7D_AB8F_DB9D0784F95E__INCLUDED_)
#define AFX_BINDINGENTRYFACTORY_H__02CB8F3F_C09A_4F7D_AB8F_DB9D0784F95E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingEntry;

class CBindingEntryFactory  
{
public:
	CBindingEntryFactory();
	virtual ~CBindingEntryFactory();
	static CBindingEntry* Create();
	static CBindingEntry* Clone(CBindingEntry* pBindingEntry);

};

#endif // !defined(AFX_BINDINGENTRYFACTORY_H__02CB8F3F_C09A_4F7D_AB8F_DB9D0784F95E__INCLUDED_)
