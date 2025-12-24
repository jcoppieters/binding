// BindingFilesOperations.h: interface for the CBindingFilesOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILESOPERATIONS_H__38F7BBA4_14C8_4E75_A7B9_E2FADC052A6B__INCLUDED_)
#define AFX_BINDINGFILESOPERATIONS_H__38F7BBA4_14C8_4E75_A7B9_E2FADC052A6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/************************************************************************/
#include "BindingFiles.h"
/************************************************************************/

/********************************************************************//**
 * @class	CBindingFilesOperations
 * @brief	Operaties op de binding files.
 * @author	Mitchell Tom
 * @changes	TM,v0562: RemoveStrings( ) 
 ***********************************************************************/
class CBindingFilesOperations  
{
public:
	CBindingFilesOperations(CBindingFiles* pBindingFiles);
	virtual ~CBindingFilesOperations();

	BOOL FindStrings(
		unsigned short usBindingNr,
		CStringArray &StrArray);
	unsigned short GetMax(
		unsigned short usMin=1,
		unsigned short usMax=0x8FFF);
	BOOL CheckModified();	
	BOOL Replace(
		const CStringArray &m_StringArray,
		const CStringArray& StringsToReplace);
	BOOL CreateNewFiles(CNodeDatabase* const pNodeDatabase);
	BOOL Assign(CNodeDatabase* const pNodeDatabase);
	int RemoveStrings(
		BYTE bNodeAddress,
		BYTE bUnitAddress);

	// Sedert versie 0x0902: Zoeken of de strings aanwezig zijn.
	// Return value = Node Address 
	// Niet gevonden = -1
	int FindString(const CString& refString);

private:
	CBindingFiles* const m_pBindingFiles;
};
/************************************************************************/

#endif // !defined(AFX_BINDINGFILESOPERATIONS_H__38F7BBA4_14C8_4E75_A7B9_E2FADC052A6B__INCLUDED_)
