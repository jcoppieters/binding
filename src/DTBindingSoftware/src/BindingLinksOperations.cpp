// BindingLinksOperations.cpp: implementation of the CBindingLinksOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingLinksOperations.h"

#include "BindingUnitsOperations.h"
#include "BindingLinkFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingLinksOperations::CBindingLinksOperations(CBindingLinks* pBindingLinks) :
m_pBindingLinks(pBindingLinks)
{
	ASSERT(m_pBindingLinks);
}

CBindingLinksOperations::~CBindingLinksOperations()
{

}


BOOL CBindingLinksOperations::Add(CBindingLink *pNewLink)
{
	if (pNewLink == 0 || m_pBindingLinks == 0) 
	{
		ASSERT(pNewLink);
		ASSERT(m_pBindingLinks);

		return FALSE;
	}

	pNewLink->SetID(0);

	const unsigned short nID = m_pBindingLinks->Add(pNewLink);
	
	ASSERT(nID);
	ASSERT(nID == pNewLink->GetID());

	if (nID != 0)
	{
		return TRUE;
	}

	return FALSE;
}


/**
 * BOOL Add(CBindingUnit* pIn,CBindingUnit* pOut,BOOL fInverse)
 */
BOOL CBindingLinksOperations::Add(CBindingUnit* pIn,CBindingUnit* pOut,BOOL fInverse)
{
	ASSERT(m_pBindingLinks);
	ASSERT(pIn);
	ASSERT(pOut);

	if (pIn != NULL && pOut != NULL)
	{			
		CBindingLink* const pNew = CBindingLinkFactory::Create();
		ASSERT(pNew);
		if (pNew != 0)
		{
			pNew->SetInverse(fInverse);
			pNew->SetOutLink(pOut->GetID(),PM_UNIT);
			pNew->SetInLink(pIn->GetID(),PM_UNIT);	
				
			BOOL fResult = Add(pNew);
			if (fResult == FALSE) 
			{
				delete pNew;				
			}
			return fResult;
		}
	}	
	return FALSE;
}

/**
 * BOOL Add(CBindingUnit* pIn,CBindingOperator *pOut,BOOL fInverse)
 */
BOOL CBindingLinksOperations::Add(CBindingUnit* pIn,CBindingOperator *pOut,BOOL fInverse)
{
	ASSERT(m_pBindingLinks);
	ASSERT(pIn);
	ASSERT(pOut);

	if (pIn != NULL && pOut != NULL)
	{			
		CBindingLink* const pNew = CBindingLinkFactory::Create();
		ASSERT(pNew);
		if (pNew != 0)
		{
			pNew->SetInverse(fInverse);
			pNew->SetOutLink(pOut->GetID(),PM_OPERATOR);
			pNew->SetInLink(pIn->GetID(),PM_UNIT);	
				
			BOOL fResult = Add(pNew);
			if (fResult == FALSE) 
			{
				delete pNew;				
			}
			return fResult;
		}
	}	
	return FALSE;
}

/**
 * BOOL Add(CBindingOperator* pIn,CBindingOperator *pOut,BOOL fInverse)
 */
BOOL CBindingLinksOperations::Add(CBindingOperator* pIn,CBindingOperator *pOut,BOOL fInverse)
{
	ASSERT(m_pBindingLinks);
	ASSERT(pIn);
	ASSERT(pOut);

	if (pIn != NULL && pOut != NULL)
	{			
		CBindingLink* const pNew = CBindingLinkFactory::Create();
		ASSERT(pNew);
		if (pNew != 0)
		{
			pNew->SetInverse(fInverse);
			pNew->SetOutLink(pOut->GetID(),PM_OPERATOR);
			pNew->SetInLink(pIn->GetID(),PM_OPERATOR);	
				
			BOOL fResult = Add(pNew);
			if (fResult == FALSE) 
			{
				delete pNew;				
			}
			return fResult;
		}
	}	
	return FALSE;
}

/**
 * BOOL Add(CBindingOperator* pIn,CBindingUnit *pOut,BOOL fInverse = FALSE)
 */
BOOL CBindingLinksOperations::Add(CBindingOperator* pIn,CBindingUnit *pOut,BOOL fInverse)
{
	ASSERT(m_pBindingLinks);
	ASSERT(pIn);
	ASSERT(pOut);

	if (pIn != NULL && pOut != NULL)
	{			
		CBindingLink* const pNew = CBindingLinkFactory::Create();
		ASSERT(pNew);
		if (pNew != 0)
		{
			pNew->SetInverse(fInverse);
			pNew->SetOutLink(pOut->GetID(),PM_UNIT);
			pNew->SetInLink(pIn->GetID(),PM_OPERATOR);	
				
			BOOL fResult = Add(pNew);
			if (fResult == FALSE) 
			{
				delete pNew;				
			}
			return fResult;
		}
	}	
	return FALSE;
}


/**
 * BOOL CBindingLinksOperations::UpdateData(CBindingUnits* pOldBindingUnits,
 *	   								        CBindingUnits* pNewBindingUnits)
 * 
 *
 * Replace ID's of units in attached bindinglink collection...
 *
 */
BOOL CBindingLinksOperations::UpdateData(const CBindingUnits* const pOldBindingUnits,
										 CBindingUnits* const pNewBindingUnits)
{
	BOOL fResult = FALSE;

	ASSERT(pOldBindingUnits);
	ASSERT(pNewBindingUnits);

	const int nMaxEntries = m_pBindingLinks->Count();
	CBindingUnitsOperations BindingUnitOperations(pNewBindingUnits);

	for (int i=0;i<nMaxEntries;i++)
	{
		CBindingLink *const pLink = m_pBindingLinks->Get(i);

		if (pLink != 0)
		{
			if (pLink->GetInLinkType() == PM_UNIT)
			{
				const unsigned short nID = pLink->GetInLinkID();
				ASSERT(nID);

				const CBindingUnit* const pBindingUnit = pOldBindingUnits->Find(nID);
				if (pBindingUnit == 0)
				{	
					ASSERT(pBindingUnit);
					return FALSE;
				}

				CBindingUnit* pNewBindingUnit = NULL;
				
				if (BindingUnitOperations.FindUnit(pBindingUnit,&pNewBindingUnit) == FALSE)
				{
					ASSERT(pNewBindingUnit);
					return FALSE;
				}

				ASSERT(pNewBindingUnit->GetID() != 0);

				pLink->SetInLink(pNewBindingUnit->GetID(),PM_UNIT);
				fResult = TRUE;
			}

			if (pLink->GetOutLinkType() == PM_UNIT)
			{
				const unsigned short nID = pLink->GetOutLinkID();
				ASSERT(nID);

				const CBindingUnit* const pBindingUnit = pOldBindingUnits->Find(nID);
				if (pBindingUnit == 0)
				{	
					ASSERT(pBindingUnit);
					return FALSE;
				}

				CBindingUnit* pNewBindingUnit = NULL;				
				if (BindingUnitOperations.FindUnit(pBindingUnit,&pNewBindingUnit) == FALSE)
				{
					ASSERT(pNewBindingUnit);
					return FALSE;
				}

				ASSERT(pNewBindingUnit->GetID() != 0);

				pLink->SetOutLink(pNewBindingUnit->GetID(),PM_UNIT);
				fResult = TRUE;
			}
		}
	}
	
	return fResult;

}

BOOL CBindingLinksOperations::UpdateData(const CBindingOperators* const pOldBindingOperators,
										 CBindingOperators* const pNewBindingOperators)
{
	ASSERT(0);	// TODO !!!!
	return FALSE;
}

BOOL CBindingLinksOperations::Import(const CBindingLinks* const pLinksNew)
{
	BOOL fResult = FALSE;

	ASSERT(m_pBindingLinks);
	ASSERT(pLinksNew);

	const int nMaxBindingLinks = pLinksNew->Count();

	for (int i=0;i<nMaxBindingLinks;i++)
	{
		const CBindingLink* const pBindingLink = pLinksNew->Get(i);
		ASSERT(pBindingLink);
		
		CBindingLink* const pNew = 
			CBindingLinkFactory::Clone(const_cast<CBindingLink*>(pBindingLink));		

		ASSERT(pNew);

		if (pNew != NULL)
		{			
			pNew->SetID(0);	// !!!!!
			const unsigned short nID = m_pBindingLinks->Add(pNew);
			
			ASSERT(nID);
			if (nID != 0)
			{
				fResult = TRUE;
			}
			else
			{
				fResult = FALSE;
				break;
			}
		}
	}
	
	ASSERT(fResult);
	return fResult;
}

