// BindingEntries.h: interface for the CBindingEntries class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGENTRIES_H__03B780A1_8164_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGENTRIES_H__03B780A1_8164_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingEntry.h"
#include "TPtrListIterator.h"

class CBindingEntries  
{
	friend class CBindingEntriesOperations;
	typedef TPtrListIterator<CBindingEntry*> CBindingEntries_it;

private:
	CPtrList List;

protected:
	unsigned short Add(CBindingEntry *pEntry);	
	CBindingEntry* _Get(unsigned short nID);
	unsigned short GetNextAvailableID(void);
	unsigned short GetNextAvailableBindingNr(void);

public:
	typedef CBindingEntries_it Iterator;

public:
	CBindingEntries();
	virtual ~CBindingEntries();

	unsigned short Count(void);
	CBindingEntry* Get(unsigned short nID,BOOL fShowError = TRUE);
	CBindingEntry* GetEntry(unsigned short nEntry,BOOL fShowError = TRUE);
	BOOL Remove(unsigned short nID,BOOL fShowError = TRUE);
	void RemoveAll(void);

	CBindingEntries_it CreateIterator( )
	{
		return CBindingEntries_it( List );
	}
};

#endif // !defined(AFX_BINDINGENTRIES_H__03B780A1_8164_11D8_B865_0050BAC412B1__INCLUDED_)
