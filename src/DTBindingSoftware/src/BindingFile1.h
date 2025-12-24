// BindingFile1.h: interface for the CBindingFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILE1_H__233366A1_9D08_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGFILE1_H__233366A1_9D08_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IBindingFileInfo;
class CBindingFileInfo;
class CBindingFileFSM;
class IBindingFileFSM;


/*************************************************************//**
 * @class	CBindingFile
 * @brief	Binding file klasse.
 *
 * @author	Mitchell Tom
 * @todo	Functies naar CBindingFileOperations verhuizen.
 *
 * @changes	TM,v0562 Remove(const char* s);
 ****************************************************************/
class CBindingFile /*: public IBindingFile */
{
public:
	CBindingFile(int nodeAddress);
	virtual ~CBindingFile();
	IBindingFileFSM *GetFSM( );	
	CBindingFileInfo *GetBindingFileInfo( );
	BOOL Save(void);
	BOOL Load(CStringArray* pStrArrayError=0);
	// void SetNodeAddress(BYTE bAddress);	
	BYTE GetNodeAddress(void) const;
	unsigned short Count(void);	
	BOOL AddString(unsigned short usBindingNr, CString szNewString);
	BOOL RemoveStrings(unsigned short usBindingNr);
	BOOL Remove(const char* s);	/** @since v0562 */
	BOOL RemovePropertyStrings(BYTE bUnitAddress);
	int RemovePropertyStrings(BYTE bUnitAddress,BYTE bFunction,BYTE bMethodData);	/** @since 0x0900 */
	int RemovePropertyStrings(BYTE bUnitAddress,BYTE bFunction,BYTE bMethodData,BYTE bData0);
	CString GetString(int iEntryNr);
	unsigned short GetBindingNr(int iEntryNr);
	void Clear(void);	
	BOOL FindStrings(unsigned short usBindingNr);
	BOOL Replace(int index,CString s);	
	BOOL IsModified( ) const;
	void SetModified( );
	void ClearModified( );

protected:	
	typedef enum
	{
		RESULT_EC_SUCCESS		= 0,
		RESULT_EC_NON_ASCII		= -1,
		RESULT_EC_NO_END		= -2,
		RESULT_EC_NO_DATA		= -3,
		RESULT_EC_NULL			= -4

	} Result_t;

	Result_t ParseStringFromFile(char *szBuffer,int maxLength);

#ifdef SKIP
	void HandleFileIoError(CString szFileName);
#endif

	unsigned short GetBindingNr(CString s);

	// zoeken nr een P-Binding ifv het unitAddress
	int FindPropertyString(BYTE bUnitAddress,int startIdx=0);
	// zoeken nr een P-Binding ifv unitAddress, functionData & methoddata.
	int FindPropertyString(BYTE bUnitAddress,BYTE bFunctionData,BYTE bMethodData,int startIdx=0);	
	// zoeken nr een P-Binding ifv unitAddress, functionData & methoddata & de eerste data byte.
	int FindPropertyString(BYTE bUnitAddress,BYTE bFunctionData,BYTE bMethodData,BYTE data0,int startIdx=0);

	// Sedert versie 0x0902:
	// Deze functie vervangt de HandleFileIoError( ) functie.
	BOOL FormatFileIOError(const CString& szFileName,CString* pStrDest,DWORD dwError);

private:
	CStringArray *pBindingFile;
	CBindingFileFSM *pBindingFileFSM;	
	CBindingFileInfo *pBindingFileInfo;	
	const BYTE bNodeAddress;
	BOOL fModified;
};
/********************************** inline functions ********************************/
inline BOOL CBindingFile::IsModified(void) const
{
	return fModified;	
}
inline void CBindingFile::SetModified( )		
{
	fModified = TRUE;  
}
inline void CBindingFile::ClearModified( )	
{
	fModified = FALSE; 
}

inline BYTE CBindingFile::GetNodeAddress( ) const
{ 
	return bNodeAddress;
}
inline unsigned short CBindingFile::Count( ) 
{ 
	const int size = pBindingFile->GetSize();			// CHANGES_MVS_2008_WARNINGS
	ASSERT( size >= 0 && size <= 0xFFFF);				// CHANGES_MVS_2008_WARNINGS
	return static_cast<unsigned short>(size);			// CHANGES_MVS_2008_WARNINGS
};
inline IBindingFileFSM* CBindingFile::GetFSM( )
{
	return ((IBindingFileFSM*) pBindingFileFSM); 
};
inline CBindingFileInfo* CBindingFile::GetBindingFileInfo( )	
{
	return (/*(IBindingFileInfo*)*/ pBindingFileInfo); 
};
/************************************************************************************/
#endif // !defined(AFX_BINDINGFILE1_H__233366A1_9D08_11D8_B865_0050BAC412B1__INCLUDED_)
