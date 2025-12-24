// BindingDatabaseApplication.h: interface for the CBindingDatabaseApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGDATABASEAPPLICATION_H__A1F90A6A_B48B_4B4C_9E67_627634814EA0__INCLUDED_)
#define AFX_BINDINGDATABASEAPPLICATION_H__A1F90A6A_B48B_4B4C_9E67_627634814EA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/***********************************************************************/
#include "BindingConversionAuto.h"
#include "BindingAppCommon.h"
/***********************************************************************/
class CBindingDatabaseDirector;
class CNodeDatabase;
class CBindingFiles;
class CBindingEntries;
/***********************************************************************/

/*******************************************************************//**
 * @class	CBindingDatabaseApplication
 * @brief	Wrapper voor de binding databases
 * @author	Mitchell Tom
 * 
 * @changes	TM,v0562: Delete(BYTE bNodeAddress,BYTE bUnitAddress);
 * @changes	TM,v0576: UpgradeBindingUnitTypes( ).
 **********************************************************************/
class CBindingDatabaseApplication  
{
public:
	static CBindingDatabaseApplication* GetInstance();
	CBindingDatabaseApplication();
	virtual ~CBindingDatabaseApplication();
	CBindingEntries* GetBindingEntries();
	CBindingFiles* GetBindingFiles();
	CBindingConversionAutoOptions* GetAutoBindingOptions();	
	void Load(CStringArray* pStrArrayError=0);
	void Save();
	void Clear();
	unsigned short Add(CBindingEntry *pEntry,BindingStatusError::BindingError_t* errorStatus);
	BOOL AddAuto(CBindingEntry *pEntry,BindingStatusError::BindingError_t* errorStatus);
	void AssignBindingFilesToNodes(CNodeDatabase *pDatabase);
	void UpdateData(BOOL fSaveToBindingClass);
	void SetAllBindingsSendFailed(BYTE bNodeAddress);	
	void SetAllSend();
	void SetAllErased();
	BOOL ModifiedBindingFiles();	
	void CreateBindingFiles();
	BOOL Delete(CBindingEntry *pEntry);
	unsigned short Delete(BYTE bNodeAddress);
	unsigned short Delete(BYTE bNodeAddress,BYTE bUnitAddress);	// since v0562
	CBindingEntry* Update(unsigned short nID);
	void UpgradeBindingUnitTypes(int nodeAddress);

private:
	CBindingEntries* const m_pBindingEntries;
	CBindingFiles* const m_pBindingFiles;
	CBindingConversionAuto BindingConverterAuto;
	CBindingDatabaseDirector* const m_pBindingDatabaseDirector;
};
/************************************************************************/


#endif // !defined(AFX_BINDINGDATABASEAPPLICATION_H__A1F90A6A_B48B_4B4C_9E67_627634814EA0__INCLUDED_)
