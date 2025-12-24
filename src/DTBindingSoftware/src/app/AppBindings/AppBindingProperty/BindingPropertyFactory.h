// BindingPropertyFactory.h: interface for the CBindingPropertyFactory class.
//
// Changes: TM20160317: Referentie naar CCanNode meegeven in de create functie.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGPROPERTYFACTORY_H__C73DB61F_8976_4D23_9F99_C35927FD4284__INCLUDED_)
#define AFX_BINDINGPROPERTYFACTORY_H__C73DB61F_8976_4D23_9F99_C35927FD4284__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*******************************************************************************/
class CBindingPropertyUnit;
class CCanNode;
struct STRUCT_UNIT;
/*******************************************************************************/

/*******************************************************************************/
struct CBindingPropertyFactory  
{
	static CBindingPropertyUnit* Create(const CCanNode* const pNode,const STRUCT_UNIT* const pUnit);
	static void destroy(STRUCT_UNIT* pUnit);	/** @since v0562 */
};
/*******************************************************************************/

#endif // !defined(AFX_BINDINGPROPERTYFACTORY_H__C73DB61F_8976_4D23_9F99_C35927FD4284__INCLUDED_)
