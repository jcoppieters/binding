// BindingFileOperations.h: interface for the CBindingFileOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEOPERATIONS_H__3D2CBE73_5F32_417A_9BAE_EEDB9995BE21__INCLUDED_)
#define AFX_BINDINGFILEOPERATIONS_H__3D2CBE73_5F32_417A_9BAE_EEDB9995BE21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*********************************************************************/
class CBindingFile;
class CBindingFileEntryParser;
/*********************************************************************/

/******************************************************************//**
 * @class	CBindingFileOperations
 * @brief	Operaties die uitgevoerd worden op een binding file.
 * @author	Mitchell Tom
 * 
 * @changes	TM,v0562: find( )
 * @changes	TM,v0562: remove( )
 *********************************************************************/
class CBindingFileOperations  
{
public:
	CBindingFileOperations(CBindingFile* pBindingFile);
	virtual ~CBindingFileOperations();

	BOOL HasStrings(unsigned short usBindingNr);
	BOOL FindStrings(unsigned short usBindingNr,CStringArray& strArray);
	unsigned short GetMax(unsigned short usMin=1,unsigned short usMax=0x8FFF);
	BOOL FindPropertyStrings(CStringArray& strArray);
	BOOL FindInstructionStrings(CStringArray& strArray);
	BOOL Replace(const CStringArray &m_StringArray,const CStringArray& StringsToReplace);
	unsigned short CheckSyntaxsis(CStringArray& strArray);
	unsigned short CheckAddresses(CStringArray& strArray);
	BOOL AdjustPBindings(const CStringArray &m_StringArray);
	unsigned short AdjustIBindings(
		const CStringArray &m_StringArray,
		unsigned short UsNextBindingNr);

	/****************************************************************//**
     * @brief	Alle binding strings zoeken voor een bepaalde unit.
	 * @param	bNodeAddress: [IN]
	 * @param	bUnitAddress: [IN]
	 * @param	stringArray: [OUT] array met binding strings.
	 * @return	Aantal gevonden strings.
	 *			
	 * @since	v0562 
	 * @note	Geeft een array met alle type bindingstrings terug.\n
	 *			OOK P-Bindings.
	 *******************************************************************/
	int find(BYTE bNodeAddress,				
			 BYTE bUnitAddress,
			 CStringArray& stringArray);

	/****************************************************************//**
     * @brief	Alle binding strings uit de bindingfile verwijderen.	
	 * @param	stringArray: [IN] array met binding strings.
	 * @return	TRUE/FALSE
	 *			
	 * @since	v0562 	
	 *******************************************************************/
	BOOL remove(CStringArray& stringArray);		


	// Sedert 0x0902
	BOOL findString(const CString& refString);

protected:
	/****************************************************************//**
     * @brief	Geeft een BindingFile entry terug 	
	 * @param	EntryNr: [IN] index in de binding file.
	 * @param	pBindingEntryType: [OUT] pointer naar Nieuwe parser.
	 * @return	TRUE/FALSE
	 *			
	 * @since	v0562 	
	 * @attention	De parser is dynamisch gealloceerd en moet door
	 *				de client uit het geheugen verwijderd worden.
	 *******************************************************************/
	BOOL GetEntry(
		int EntryNr,
		CBindingFileEntryParser** pBindingEntryType);

private:
	CBindingFile* const m_pBindingFile;
};
/*********************************************************************/

#endif // !defined(AFX_BINDINGFILEOPERATIONS_H__3D2CBE73_5F32_417A_9BAE_EEDB9995BE21__INCLUDED_)
