// BindingDatabaseDirector.h: interface for the CBindingDatabaseDirector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGDATABASEDIRECTOR_H__214424C8_67CC_4436_98A3_2429748699A2__INCLUDED_)
#define AFX_BINDINGDATABASEDIRECTOR_H__214424C8_67CC_4436_98A3_2429748699A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingAppCommon.h"

/********************************************************************/
class CBindingEntry;
class CNodeDatabase;
class CBindingConversionAuto;
class CBindingEntries;
class CBindingFiles;
class CBindingUnit;
/********************************************************************/



/*****************************************************************//**
 * @class	CBindingDatabaseDirector
 * @brief	Wrapper voor de binding database functies.
 * @author	Mitchell Tom
 * @todo	Weing verschil met CbindingDatabaseApplication ?
 *
 * @changes	TM,v0562: unsigned short Delete(BYTE bNodeAddress, BYTE bUnitAddress).
 * @changes	TM,v0576: void UpgradeBindingUnitTypes( )
 ********************************************************************/
class CBindingDatabaseDirector  
{
public:
	CBindingDatabaseDirector(CBindingFiles* pBindingFiles,
							 CBindingEntries* pBindingEntries,						
							 CNodeDatabase* pNodeDatabase);
	virtual ~CBindingDatabaseDirector();

	CBindingEntry* Update(unsigned short nID);
	unsigned short Delete(BYTE bNodeAddress);
	BOOL Delete(CBindingEntry* const pBindingEntry);
	unsigned short Add(CBindingEntry* const pBindingEntry,BindingStatusError::BindingError_t* errorStatus);
	BOOL Add(CBindingEntry* const pBindingEntry,CBindingConversionAuto* const pBindingConversionAuto,BindingStatusError::BindingError_t* errorStatus);
	void Clear();
	BOOL CreateNewFiles();
	BOOL AssignBindingFiles(CNodeDatabase* const pNodeDatabase);
/*
	unsigned short CompareDatabases(CStringArray* const pStringArray);
*/
	BOOL UpdateModifiedFlags(BOOL fSaveToBindingClass);
	void Load(CStringArray* pStrArrayError=0);
	void Save();
	unsigned short Delete(BYTE bNodeAddress, BYTE bUnitAddress);

	// Uppgraden van de binding unit types.
	// Momenteel:
	//	- Audio HC4 (Bno)
	//  - DALI units.
	void UpgradeBindingUnitTypes(int nodeAddress);


public:		
	// Sedert V16.17
	// Oppassen: Er wordt een copy aangemaakt van de bindingEntry.
	static unsigned short addBindingEntry(CBindingEntries* pBindingEntries,CBindingEntry* const pBindingEntry,
										  CBindingEntry** ppNewBindingEntry,BOOL modifyNames,BOOL nortifyGUI,
										  BindingStatusError::BindingError_t* errorStatus);

protected:
	BOOL _Delete(BYTE bNodeAddress, int unitAddress = -1);	/** since 0x0562 */

	void UpgradeBindingUnitTypes_Audio(CBindingUnit* const pBindingUnit);
	void UpgradeBindingUnitTypes_Dali(CBindingUnit* const pBindingUnit);

private:
	CBindingEntries* const m_pBindingEntries;
	CNodeDatabase* const m_pNodeDatabase;
	CBindingFiles* const m_pBindingFiles;
};
/*****************************************************************************/
#endif // !defined(AFX_BINDINGDATABASEDIRECTOR_H__214424C8_67CC_4436_98A3_2429748699A2__INCLUDED_)
