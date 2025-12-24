// BindingLinks.h: interface for the CBindingLinks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGLINKS_H__015859A4_2F77_11D7_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGLINKS_H__015859A4_2F77_11D7_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////
#include "BindingLink.h"
/////////////////////////////////////////////////////////////

class CBindingLinks  
{
public:
	CBindingLinks();
	CBindingLinks(const CBindingLinks& pBindingLinks);
	virtual ~CBindingLinks();

	CBindingLinks& operator=(const CBindingLinks& pBindingLinks);
	unsigned short Count(void) const;
	unsigned short Add(CBindingLink* pLink);
	CBindingLink* Find(unsigned short nID) const;
	CBindingLink* Get(unsigned short nEntry) const;
	BOOL Remove(unsigned short nID);
	void RemoveAll(void);

private:
	CPtrList List;
	unsigned short GetNextAvailableID(void);

protected:
	void Copy(const CBindingLinks* const pBindingLinks);
};


#endif // !defined(AFX_BINDINGLINKS_H__015859A4_2F77_11D7_B865_0050BAC412B1__INCLUDED_)
