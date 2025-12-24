// BindingUnitsOperations.h: interface for the CBindingUnitsOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGUNITSOPERATIONS_H__E8811C76_D7DB_4931_9100_A6B83D74325F__INCLUDED_)
#define AFX_BINDINGUNITSOPERATIONS_H__E8811C76_D7DB_4931_9100_A6B83D74325F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingUnits.h"

class CBindingUnitsOperations  
{
protected:
	CBindingUnits* const m_pBindingUnits;

public:
	CBindingUnitsOperations(CBindingUnits* pBindingUnits);
	virtual ~CBindingUnitsOperations();

public:
	BOOL Import(const CBindingUnits* const pImport);

	BYTE GetUnitType(unsigned short nID);	
	BOOL GetUnitType(BYTE bUnitType,int iIndex,CBindingUnit **pReturnUnit);

	BOOL GetInputs(int iIndex,CBindingUnit **pReturnUnit);
	BOOL GetOutputs(int iIndex,CBindingUnit **pReturnUnit);

	BOOL FindUnit(const CBindingUnit *const pIn,CBindingUnit** pOut);
	BOOL RemoveUnit(CBindingUnit* pBindingUnit);
	BOOL UpgradeBindingUnitTypes();

	int GetEquals(const CBindingUnit* const pIn,CBindingUnits* const pBindingUnits = 0);

	BOOL SetDefaultNames(CBindingUnit* const pLast);

	// Bepalen van het aantal unieke node/unit addressen in de collectie.
	int getCountUniqueUnitAddresses();

	// Aantal units in deze collectie met dit node en unit adres.
	int getCountUnitAddress(int nodeAddress, int unitAddress);
};

#endif // !defined(AFX_BINDINGUNITSOPERATIONS_H__E8811C76_D7DB_4931_9100_A6B83D74325F__INCLUDED_)
