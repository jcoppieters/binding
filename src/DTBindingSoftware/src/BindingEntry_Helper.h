#ifndef INCLUDED_BINDINGENTRY_HELPER_H
#define INCLUDED_BINDINGENTRY_HELPER_H
/************************************************************************************/

#include "BindingConstants.h"

class CBindingEntry;
class CBindingUnit;
class CBindingLink;

/************************************************************************************/
/********************************* CBindingEntry_Helper *****************************/
/************************************************************************************/

// Nieuw sedert V16.16.
struct CBindingEntry_Helper
{
	// Return value: <= 0 wanneer mislukt, anders het id.
	static int AddBindingOperator(CBindingEntry *pDest,OperatorType type);
	static int AddBindingUnit(CBindingEntry* pDest,const CBindingUnit& refUnit);
	static int AddBindingLink(CBindingEntry *pDest,const CBindingLink& refLink);
};
/************************************************************************************/
#endif /* INCLUDED_BINDINGENTRY_HELPER_H */
/************************************************************************************/