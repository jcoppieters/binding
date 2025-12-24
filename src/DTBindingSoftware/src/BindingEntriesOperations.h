// BindingEntriesOperations.h: interface for the CBindingEntriesOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGENTRIESOPERATIONS_H__D883E76D_98D6_4397_AD5D_C9EFBB42D607__INCLUDED_)
#define AFX_BINDINGENTRIESOPERATIONS_H__D883E76D_98D6_4397_AD5D_C9EFBB42D607__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingEntries;
class CBindingEntry;

class CBindingEntriesOperations  
{
protected:
	CBindingEntries* const m_pBindingEntries;

public:
	CBindingEntriesOperations(CBindingEntries* pBindingEntries);
	virtual ~CBindingEntriesOperations();

public:
	unsigned short Add(CBindingEntry *pSource,unsigned short usBindingNr = 0);
	void SetAllSend();
	void SetAllErased();
	void SetDefaultNames();
	void SetSendFailed(BYTE bNodeAddress);
};

#endif // !defined(AFX_BINDINGENTRIESOPERATIONS_H__D883E76D_98D6_4397_AD5D_C9EFBB42D607__INCLUDED_)
