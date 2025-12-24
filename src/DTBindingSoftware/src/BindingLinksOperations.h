// BindingLinksOperations.h: interface for the CBindingLinksOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGLINKSOPERATIONS_H__B388D915_274F_4C95_A3FE_C0863F3167BB__INCLUDED_)
#define AFX_BINDINGLINKSOPERATIONS_H__B388D915_274F_4C95_A3FE_C0863F3167BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingLinks.h"
#include "BindingUnits.h"
#include "BindingOperators.h"

class CBindingLinksOperations  
{
protected:
	CBindingLinks* const m_pBindingLinks;

public:
	CBindingLinksOperations(CBindingLinks* pBindingLinks);
	virtual ~CBindingLinksOperations();

	BOOL Add(CBindingUnit* pIn,
			 CBindingUnit* pOut,
			 BOOL fInverse = FALSE);

	BOOL Add(CBindingUnit* pIn,
			 CBindingOperator *pOut,
			 BOOL fInverse = FALSE);

	BOOL Add(CBindingOperator* pIn,
			 CBindingOperator *pOut,
			 BOOL fInverse = FALSE);

	BOOL Add(CBindingOperator* pIn,
			 CBindingUnit *pOut,
			 BOOL fInverse = FALSE);
	
	BOOL UpdateData(const CBindingUnits* const pOldBindingUnits,
					CBindingUnits* const pNewBindingUnits);

	BOOL UpdateData(const CBindingOperators* const pOldBindingOperators,
				    CBindingOperators* const pNewBindingOperators);

	BOOL Import(const CBindingLinks* const pLinksNew);

protected:
	BOOL Add(CBindingLink *pNewLink);

};

#endif // !defined(AFX_BINDINGLINKSOPERATIONS_H__B388D915_274F_4C95_A3FE_C0863F3167BB__INCLUDED_)
