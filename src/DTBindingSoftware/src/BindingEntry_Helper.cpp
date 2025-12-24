#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEntry_Helper.h"

#include "BindingEntry.h"
#include "BindingLink.h"
#include "BindingLinks.h"
#include "BindingUnits.h"
#include "BindingOperatorFactory.h"
#include "BindingUnitFactory.h"
#include "BindingLinkFactory.h"
#include "BindingOperators.h"
#include "BindingOperator.h"

/************************************************************************************/
/********************************* CBindingEntry_Helper *****************************/
/************************************************************************************/
int CBindingEntry_Helper::AddBindingOperator(CBindingEntry* pDest,OperatorType type)
{	
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return -1;
	}	
	CBindingOperators* const pOperators = pDest->GetColOperator();
	if (NULL == pOperators) {
		ASSERT(NULL != pOperators);
		return -1;
	}
	CBindingOperator* const pNewOperator = CBindingOperatorFactory::Create(type);
	if (NULL == pNewOperator) {
		ASSERT(NULL != pNewOperator);
		return -1;
	}

	pNewOperator->SetID(0);	// !!

	const int retValue = pOperators->Add(pNewOperator);
	ASSERT(retValue > 0);
	ASSERT(retValue == pNewOperator->GetID());
	return retValue;
}

int CBindingEntry_Helper::AddBindingUnit(CBindingEntry* pDest,const CBindingUnit& refUnit) {

	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return -1;
	}										
	CBindingUnits* const pBindingUnits = pDest->GetColUnit();
	if (pBindingUnits == NULL) {
		ASSERT(NULL != pBindingUnits);
		return -1;
	}
	
	/** Clone Derived Classes of CBindingUnit */
	ASSERT(refUnit.GetBindingUnitType() != 0);
	
	CBindingUnit* const pNew = CBindingUnitFactory::_Clone(&refUnit);
	if (pNew == NULL) {
		ASSERT(NULL != pNew);
		return -1;
	}
					
	ASSERT(pNew->GetBindingUnitType() != 0);
	// ASSERT(pNew->GetID() != NULL);
	ASSERT(pNew->GetID() == refUnit.GetID());

	const int retValue = pBindingUnits->Add(pNew);					
	ASSERT(retValue > 0);
	ASSERT(retValue == pNew->GetID());
	return retValue;
}


int CBindingEntry_Helper::AddBindingLink(CBindingEntry *pDest,const CBindingLink& refLink) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return -1;
	}	

	CBindingLinks* const pBindingLinks = pDest->GetColLink();	
	if (NULL == pBindingLinks) {		
		ASSERT(NULL != pBindingLinks);
		return -1;
	}
	
	CBindingLink* const pNewLink = CBindingLinkFactory::Clone(&refLink);
	if (NULL == pNewLink) {
		ASSERT(NULL != pNewLink);
		return -1;
	}
							
	pNewLink->SetID(0);	// !!!!

	const int retValue = pBindingLinks->Add(pNewLink);
	ASSERT(retValue > 0);
	ASSERT(retValue == pNewLink->GetID());
	return retValue;			
}
/************************************************************************************/