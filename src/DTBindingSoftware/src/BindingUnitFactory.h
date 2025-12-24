// BindingUnitFactory.h: interface for the CBindingUnitFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGUNITFACTORY_H__4E7BC34D_D683_4FA8_8B91_563379C8FAB4__INCLUDED_)
#define AFX_BINDINGUNITFACTORY_H__4E7BC34D_D683_4FA8_8B91_563379C8FAB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingUnit;
class CBindingDimmerUnit;


struct STRUCT_UNIT;

class CBindingUnitFactory  
{
public:

	/**
	 * Aanmaken van een simpele binding unit.
	 */
	static CBindingUnit* Create();

	/**
	 * Aanmaken van een binding unit.
	 * Oppassen enkel de data van de basis CBindingUnit wordt gekopieerd.
	 * o.a. NodeAddress + UnitAddress
	 */
	static CBindingUnit* Clone(const CBindingUnit *const pBindingUnit);


	/**
	 *	Aanmaken en copieren van een binding unit.
	 *  Roept de copy constructor aan.
	 */
	static CBindingUnit* _Clone(const CBindingUnit *const pBindingUnit);

#ifdef SKIP
	/**
	 * Aanmaken van een binding unit.
	 */
	static CBindingUnit* Create(BYTE bBindingType);

#endif

	/**
	 * Aanmaken van een binding unit ifv een unit uit de node database.
	 */
	static CBindingUnit* Create(STRUCT_UNIT* pUnit);

};

#endif // !defined(AFX_BINDINGUNITFACTORY_H__4E7BC34D_D683_4FA8_8B91_563379C8FAB4__INCLUDED_)
