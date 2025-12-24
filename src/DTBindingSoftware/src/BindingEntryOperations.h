// BindingEntryOperations.h: interface for the CBindingEntryOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGENTRYOPERATIONS_H__6D041FDB_1FB9_423A_8931_94615C3C032F__INCLUDED_)
#define AFX_BINDINGENTRYOPERATIONS_H__6D041FDB_1FB9_423A_8931_94615C3C032F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingEntry.h"
class CNodeDatabase;

class CBindingEntryOperations  
{
protected:
	CBindingEntry* const m_pBindingEntry;

public:
	CBindingEntryOperations(CBindingEntry* pBindingEntry);
	virtual ~CBindingEntryOperations();

	BOOL Import(CBindingEntry* const pImport);
	BOOL SetDefaultName(CBindingUnit* const pFirst,CBindingUnit* const pLast);

	BOOL Export(FILE *fp);
	BOOL SetDefaultName(BOOL fUpdateUnits = FALSE);

};

#endif // !defined(AFX_BINDINGENTRYOPERATIONS_H__6D041FDB_1FB9_423A_8931_94615C3C032F__INCLUDED_)
