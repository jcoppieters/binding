// BindingUnits.h: interface for the CBindingUnits class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGUNITS_H__C9953FA2_765C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGUNITS_H__C9953FA2_765C_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include "BindingUnit.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"

#include "TPtrListIterator.h"

////////////////////////////////////////////////////////////////
class CBindingUnits  
{
private:
	typedef TPtrListIterator<CBindingUnit*> CBindingUnits_it;

	CPtrList List;

protected:
	unsigned short GetNextAvailableID(void);
	void Copy(const CBindingUnits* const pSourceBindingUnits);

public:
	typedef CBindingUnits_it Iterator;

	CBindingUnits();
	virtual ~CBindingUnits();
	CBindingUnits(const CBindingUnits& BindingUnits);
	CBindingUnits& operator=(const CBindingUnits& BindingUnits);

	unsigned short Count(void) const;
	unsigned short Add(CBindingUnit* pBindingUnit);
	CBindingUnit* Find(unsigned short nID) const;
	CBindingUnit* Get(unsigned short nEntry) const;
	BOOL Remove(unsigned short nID);
	void RemoveAll(void);
	BOOL Replace(CBindingUnit* const pOld,CBindingUnit* const pNew);

	Iterator CreateIterator( )
	{
		return Iterator( List );
	}
};


#endif // !defined(AFX_BINDINGUNITS_H__C9953FA2_765C_11D8_B865_0050BAC412B1__INCLUDED_)
