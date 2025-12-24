// BindingFile1.cpp: implementation of the CBindingFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFile1.h"
#include "MainFrm.h"
#include "BindingFileFSM.h"
#include "BindingFileInfo.h"

#include "App\Appbindings\AppbindingFileEntry\bindingfileentry.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"		// BUG-0100

#include "utils/app/DisplayDevice.h"

// -------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalSettings;
// -------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define MAX_BUFFER_LENGTH	300
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFile::CBindingFile(int nodeAddress) : 
	bNodeAddress(nodeAddress), pBindingFile(NULL), pBindingFileFSM(NULL), pBindingFileInfo(NULL), fModified(TRUE) {
	ASSERT(nodeAddress>=0 && nodeAddress<=0xFF);
	pBindingFile = new CStringArray;		
	pBindingFileFSM = new CBindingFileFSM(this);
	pBindingFileInfo = new CBindingFileInfo(/*(IBindingFile*)*/ this);		
}

CBindingFile::~CBindingFile() {
	if (pBindingFile != NULL) {
		delete pBindingFile;
		pBindingFile = NULL;
	}
	if (pBindingFileFSM != NULL) {
		delete pBindingFileFSM;
		pBindingFileFSM = NULL;
	}
	if (pBindingFileInfo != NULL) {
		delete pBindingFileInfo;
		pBindingFileInfo = NULL;
	}
}

void CBindingFile::Clear(void) {
	pBindingFile->RemoveAll();
	fModified = TRUE;
}


BOOL CBindingFile::AddString(unsigned short usBindingNr,CString szNewString)
{
	ASSERT(szNewString.IsEmpty() == FALSE);

	pBindingFile->Add(szNewString);
	fModified = TRUE;
	return TRUE;
}

BOOL CBindingFile::Replace(int index,CString s)
{
	if (s.IsEmpty() == FALSE)
	{
		if (index < Count())
		{
			pBindingFile->SetAt(index, s );
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBindingFile::FindStrings(unsigned short usBindingNr)
{
	if (pBindingFile != NULL)
	{
		for (int i=0;
			 i<pBindingFile->GetSize();
			 i++) 
		{
			CString s(pBindingFile->GetAt(i));	

			if (GetBindingNr(s) == usBindingNr)
			{
				return TRUE;
			}		
		}
	}
	return FALSE;
}


// Remove all string that have bindingnr == usbindingnr
BOOL CBindingFile::RemoveStrings(unsigned short usBindingNr)
{
	for (int i=0;i<pBindingFile->GetSize();i++) 
	{
		CString s(pBindingFile->GetAt(i));
		ASSERT(s.IsEmpty() == FALSE);

		const unsigned short bindingnr = GetBindingNr(s);		
		if (bindingnr == usBindingNr) 
		{
			pBindingFile->RemoveAt(i);			
			RemoveStrings(usBindingNr);	//RECURSIEF -Tricky !!!!!		
			fModified = TRUE;
		}
	}
	return TRUE;
}

BOOL CBindingFile::Remove(const char* s)
{
	BOOL result = FALSE;
	const int Size = pBindingFile->GetSize();
	for (int i=0;i<Size;i++) 
	{
		CString szString(pBindingFile->GetAt(i));
		if (szString.CompareNoCase(s) == 0)
		{
			pBindingFile->RemoveAt(i);	
			fModified = TRUE;
			result = TRUE;
			break;	// break for loop...
		}
	}
	return result;
}

BOOL CBindingFile::RemovePropertyStrings(BYTE bUnitAddress)
{
	CString szRemove;
	szRemove.Format(_T("_PU%02XF"),bUnitAddress);
	for (int i=0;i<pBindingFile->GetSize();i++) {
		CString s(pBindingFile->GetAt(i));
		CString szCompare(s.Mid(14,6));
		ASSERT(s.IsEmpty() == FALSE);
		ASSERT(szCompare.IsEmpty() == FALSE);		
		if (szCompare == szRemove)  {
			pBindingFile->RemoveAt(i);
			RemovePropertyStrings(bUnitAddress);
			fModified = TRUE;
		}
	}
	return TRUE;
}

// Zoeken naar P-Bindings van een bepaald unit adres.
int CBindingFile::FindPropertyString(BYTE bUnitAddress, int idx)
{
	int retVal=-1;
	CString szFind;
	szFind.Format(_T("_PU%02XF"),bUnitAddress);
	for(/*idx*/;idx<pBindingFile->GetSize();++idx) {
		const CString s(pBindingFile->GetAt(idx));
		if (!s.IsEmpty()) {
			const CString szCompare(s.Mid(14,6));
			if (!szCompare.IsEmpty()) {
				if (szCompare==szFind) {
					TRACE("Found:'%s'\r\n",s);
					retVal=idx;
					break;
				}
			}
		}
	}
	return retVal;
}

int CBindingFile::FindPropertyString(BYTE bUnitAddress,BYTE bFunctionData,BYTE bMethodData,int idx)
{
	BOOL fResult=FALSE;
	int retVal=-1;
	CString szFunction;

	szFunction.Format("F%02XD",bFunctionData);

	// 0123456789ABCDEF0123456789
	// 000004_00_0000_PU00F88D03150300S

	for (/* */ ;pBindingFile->GetSize();++idx) {
		idx=FindPropertyString(bUnitAddress,idx);
		if (idx!=-1) {
			CString s;			
			ASSERT(idx<pBindingFile->GetSize());
			s=this->GetString(idx);
			if (!s.IsEmpty()) {
				const int startIndex = 19;
				int chIdx = s.Find(szFunction,startIndex);
				if (chIdx>=0) {
					if (s.GetLength()>=(chIdx+6+2)) {
						CString szMethod(s.Mid(chIdx+6,2));
						if (!szMethod.IsEmpty()) {
							int temp;
							if (sscanf((const char*) szMethod,"%02X",&temp)==1) {
								if (temp==bMethodData) {
									TRACE("Found:'%s'\r\n",s);
									retVal=idx;
									break;
								}
							}
						}
					}
				}
			}
		}	
		else {
			retVal=-1;
			break;
		}
	} 
	ASSERT((retVal==-1)|| (retVal<pBindingFile->GetSize()));
	return retVal;
}

int CBindingFile::FindPropertyString(BYTE bUnitAddress,BYTE bFunctionData,BYTE bMethodData,BYTE data0,int idx) {
	BOOL fResult=FALSE;
	int retVal=-1;
	CString szFunction;

	szFunction.Format("F%02XD",bFunctionData);

	// 0123456789ABCDEF0123456789012
	// 000004_00_0000_PU00F88D03150300S

	for (/* */ ;pBindingFile->GetSize();++idx) {
		idx=FindPropertyString(bUnitAddress,idx);
		if (idx!=-1) {
			CString s;			
			ASSERT(idx<pBindingFile->GetSize());
			s=this->GetString(idx);
			if (!s.IsEmpty()) {
				const int startIndex = 19;
				int chIdx = s.Find(szFunction,startIndex);
				if (chIdx>=0) {
					if (s.GetLength()>=(chIdx+6+4)) {
						CString szMethod(s.Mid(chIdx+6,4));
						if (!szMethod.IsEmpty()) {
							int temp[2] = { 0, 0 };
							if (sscanf((const char*) szMethod,"%02X%02X",&temp[0],&temp[1])==2) {
								if ((temp[0]==bMethodData) && (temp[1]==data0)) {
									TRACE("Found:'%s'\r\n",s);
									retVal=idx;
									break;
								}
							}
						}
					}
				}
			}
		}	
		else {
			retVal=-1;
			break;
		}
	} 
	ASSERT((retVal==-1)|| (retVal<pBindingFile->GetSize()));
	return retVal;
}

// Sedert versie 0x0900:
BOOL CBindingFile::RemovePropertyStrings(BYTE bUnitAddress,BYTE bFunction,BYTE bMethodData)
{
	int idx=0;
	BOOL fResult=FALSE;	
	do {
		idx=FindPropertyString(bUnitAddress,bFunction,bMethodData,idx);		
		if (idx>=0) {
			ASSERT(idx<pBindingFile->GetSize());
			pBindingFile->RemoveAt(idx);
			fResult=TRUE;
		}
	} while(idx>=0);
	// TODO: MOETEN WE SetModified() aanroepen ?
	return fResult;
}

BOOL CBindingFile::RemovePropertyStrings(BYTE bUnitAddress,BYTE bFunction,BYTE bMethodData,BYTE bData0) {
	int idx=0;
	BOOL fResult=FALSE;	
	do {
		idx=FindPropertyString(bUnitAddress,bFunction,bMethodData,bData0,idx);		
		if (idx>=0) {
			ASSERT(idx<pBindingFile->GetSize());
			pBindingFile->RemoveAt(idx);
			fResult=TRUE;
		}
	} while(idx>=0);
	// TODO: MOETEN WE SetModified() aanroepen ?
	return fResult;
}

CString CBindingFile::GetString(int iEntryNr) {	
	if (iEntryNr < pBindingFile->GetSize()) {
		return (pBindingFile->GetAt(iEntryNr));	
	}	
	return CString("");
}

unsigned short CBindingFile::GetBindingNr(int iEntryNr)
{
	CString szBindingString(GetString(iEntryNr));	
	if (szBindingString.IsEmpty()) 
		return 0;	
	return(GetBindingNr(szBindingString));
}

unsigned short CBindingFile::GetBindingNr(CString s)
{
	ASSERT(s.IsEmpty() == FALSE);	
	int nCount = 0;
	const char* const szDataToParse = (LPCTSTR) s;	// CHANGES_MVS_2008_CSTRING
	const int nLen = s.GetLength();
	for (int i=0;i<nLen;i++)  { 
		const char ch = s.GetAt(i);	
		switch(ch) {
			case '_': {
				if (++nCount == 2) {					
					unsigned int bindingNr=0;	
					const int n = sscanf(&szDataToParse[i],"_%04X",&bindingNr);									
					ASSERT(n);
					return (unsigned short) bindingNr;
				}
			} break;
		}	
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// LOAD + SAVE
////////////////////////////////////////////////////////////////////////

// Sedert versie 0x0902:
// Bij mislukken van fprintf() een foutmelding geven.
BOOL CBindingFile::Save(void)
{	
	if (TMGetFileSettings() == 0)
	{
		CString szMessage("Unable to get file settings, files not saved!");
		AfxMessageBox(szMessage);
		return FALSE;
	}

	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());
	CString szFilename;

	szFilename.Format("%sbind%02x.txt",szPathName,bNodeAddress);

	FILE* const fBindingFile = fopen((LPCTSTR) szFilename,"w");		// CHANGES_MVS_2008_CSTRING

	if (fBindingFile != NULL)
	{
		const int NrOfBindings = pBindingFile->GetSize(); 
		for (int i=0;i<NrOfBindings;i++) 
		{		
			// BUG-0100

			CString s;		

			if (CBindingFileEntryOperations::normalize(pBindingFile->GetAt(i),&s)) {			
				const int res = fprintf(fBindingFile,"%s\n", (LPCSTR)s);
				if (res<0)
				{
					// Foutmelding geven...
					CString szErrorMessage;

					if(TRUE==FormatFileIOError(
								szFilename,
								&szErrorMessage,
								GetLastError()))
					{
						AfxMessageBox(szErrorMessage);
					}
				
					break;
				}
			}
#if (0)	// Onderstaande code is niet nodig...
			else
			{
				// Sedert versie 0x0901: Foutmelding.
				CString szMessage;

				szMessage.Format(_T("Error: Failed to normalize and save binding.\r\n"
									"file: bind%02x.txt"),bNodeAddress);
				AfxMessageBox(szMessage,MB_OK);	
				break;
			}
#endif
		}

		fclose(fBindingFile);

		CString szMessage;
		szMessage.Format(_T("Save file : '%s'"),szFilename);
		CDisplayDevice::DisplayInfoMsg(szMessage);

		return TRUE;
	}
	

	{
		CString szErrorMessage;

		if(TRUE==FormatFileIOError(
					szFilename,
					&szErrorMessage,
					GetLastError()))
		{
			AfxMessageBox(szErrorMessage);
		}
	}

	return TRUE;
}


// Changes TM, versie 0x0902 										
BOOL CBindingFile::Load(CStringArray* pStrArrayError)
{	
	if (TMGetFileSettings() == 0)
	{
		CString szMessage("Unable to get file settings, files not loaded!");
		AfxMessageBox(szMessage);
		return FALSE;
	}

	CString szErrorHeader;
	CString szError;
	CString szFilename;
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	

	Clear();	//Clear BindingFile ...
	
	FILE *fBindingFile;
	szFilename.Format("%sbind%02x.txt",szPathName,bNodeAddress);
	szErrorHeader.Format("bind%02x.txt",bNodeAddress);	

	if ((fBindingFile = fopen((LPCTSTR) szFilename,"r")) != NULL)	
	{		
		enum { MAX_BUFFER_LENGTH=300 };

		char szBuffer[MAX_BUFFER_LENGTH];
		int nEntries=0;
		int lineNumber=0;
		int countChNull=0;

		memset(szBuffer,0,sizeof(szBuffer));

		while (fgets(szBuffer, 
					 MAX_BUFFER_LENGTH, 
					 fBindingFile ) 
					 != NULL)
		{
			BOOL fAddBinding=FALSE;	
			BOOL fAddMessageError=FALSE;
			const Result_t res = ParseStringFromFile(
					szBuffer,
					MAX_BUFFER_LENGTH);

			switch(res)
			{
				case RESULT_EC_SUCCESS:
				{				
					fAddBinding = TRUE;
					szError.Empty();
				} break;
				case RESULT_EC_NON_ASCII:
				{
					fAddBinding = FALSE;
					szError.Format(_T("#%d: Non ASCII char"),lineNumber+1);
					fAddMessageError=TRUE;
				} break;
				case RESULT_EC_NO_END:
				{
					fAddBinding = FALSE;
					szError.Format(_T("#%d: Erroneous end"),lineNumber+1);
					fAddMessageError=TRUE;
				} break;
				case RESULT_EC_NO_DATA:
				{
					fAddBinding = FALSE;
					szError.Format(_T("#%d: No data"),lineNumber+1);
					fAddMessageError=TRUE;
				} break;				
				case RESULT_EC_NULL:
				{
					szError.Empty();
					fAddBinding = FALSE;
					countChNull++;
				} break;			
			}

			if (TRUE==fAddBinding) 
			{	
				CString szBinding;
				szBinding.Format(_T("%s"),szBuffer);
				
				// TM 20060403 : Convert To Upper !!!
				szBinding.MakeUpper();

				szBinding.Remove(0x0A);				
				szBinding.Remove(0x0D);

				// Sedert versie 0x0902:
				// Controleren of het een geldige binding string is.

				CBindingFileEntry BindingFileEntry(szBinding);
				CBindingFileEntryParser* pBindingFileEntryType = BindingFileEntry.Parse();

				if (pBindingFileEntryType != 0)
				{				
					pBindingFile->InsertAt(nEntries,szBinding);
					nEntries++;
				}
				else
				{
					szError.Format(_T("#%d: Wrong binding syntax"),lineNumber+1);
					fAddMessageError=TRUE;				
				}
			}

			if(TRUE==fAddMessageError)
			{
				if (!szError.IsEmpty())
				{					
					CString s;
					s.Format("Error in '%s' %s",szErrorHeader,szError);

					if (0!=pStrArrayError)
					{
						pStrArrayError->Add(s);
					}					
				}
			}

			lineNumber++;
		}
		fclose(fBindingFile);

		if (0!=countChNull)
		{
			CString s;
			s.Format("Error in '%s': holds null char",szErrorHeader);

			if (0!=pStrArrayError)
			{
				pStrArrayError->Add(s);
			}	
		}

		fModified = TRUE;

		CString szMessage;
		szMessage.Format(_T("Load file : '%s'"),szFilename);
		CDisplayDevice::DisplayInfoMsg(szMessage);

		return TRUE;
	}

	// HandleFileIoError(szFilename);

	{
		CString szErrorMessage;

		if(TRUE==FormatFileIOError(
					szFilename,
					&szErrorMessage,
					GetLastError()))
		{
			if (0!=pStrArrayError)
			{
				pStrArrayError->Add(szErrorMessage);
			}
			else
			{
				AfxMessageBox(szErrorMessage);
			}
		}
	}

	return TRUE;
}

// Sedert versie 0x0902:
// Deze functie vervangt de HandleFileIoError( ) functie.
BOOL CBindingFile::FormatFileIOError(
	const CString& szFileName,
	CString* pStrDest,
	DWORD dwError)
{
	BOOL fResult=FALSE;

	if(pStrDest)
	{
		CString szError;

		switch (dwError) 
		{
			case ERROR_FILE_NOT_FOUND:
				szError.Format(_T("File Io Error: File not Found"));
			break;
			case ERROR_PATH_NOT_FOUND:
				szError.Format(_T("File Io Error: Path not Found"));
			break;
			case ERROR_TOO_MANY_OPEN_FILES:
				szError.Format(_T("File Io Error: Too many open Files"));
			break;
			case ERROR_ACCESS_DENIED:
				szError.Format(_T("File Io Error: Access denied: File is probably set as read only"));
			break;
			case ERROR_SHARING_VIOLATION:
				szError.Format(_T("File Io Error: Access denied: File is being used by another process"));
			break;			
			default:
				szError.Format(_T("File Io Error: Error code=%02x"),dwError);
			break;	
		}

		pStrDest->FormatMessage(_T("%1 :\n%2"),szFileName,szError);
		fResult=TRUE;
	}
	return fResult;
}


#ifdef SKIP
	void CBindingFile::HandleFileIoError(CString szFileName)
	{
		DWORD error = GetLastError();
		CString szError,szErrorMessage;

		switch (error) 
		{
			case ERROR_FILE_NOT_FOUND:
				szError.Format(_T("File Io Error: File not Found"));
			break;
			case ERROR_PATH_NOT_FOUND:
				szError.Format(_T("File Io Error: Path not Found"));
			break;
			case ERROR_TOO_MANY_OPEN_FILES:
				szError.Format(_T("File Io Error: Too many open Files"));
			break;
			case ERROR_ACCESS_DENIED:
				szError.Format(_T("File Io Error: Access denied: File is probably set as read only"));
			break;
			case ERROR_SHARING_VIOLATION:
				szError.Format(_T("File Io Error: Access denied: File is being used by another process"));
			break;			
			default:
				szError.Format(_T("File Io Error - Error code %02x"),error);
			break;	
		}
		szErrorMessage.FormatMessage(_T("%1 :\n%2"),szFileName,szError);
		AfxMessageBox(szErrorMessage);
	}
#endif



// sedert 0x0902: Optimalisatie.
CBindingFile::Result_t 
CBindingFile::ParseStringFromFile(
	char *szBuffer,
	int maxLength)
{	
	Result_t res = RESULT_EC_NO_END;
	BOOL fContinue = TRUE;

	for (int i=0;(i<maxLength)&&(TRUE==fContinue);i++) 
	{
		const char ch = szBuffer[i];

		switch(ch) 
		{
			case '\0':
			{
				res=RESULT_EC_NULL;
				fContinue=FALSE;
			} break;

			case '>':
			case '=':
			case '_':
			case '(':
			case ')':
			case '+':
			case '^':
			case '*':
			case '!':
			{
				fContinue=TRUE;
			} break;

			case 0x0A:
			case 0x0D:
			case ';':
			{
				if (i>0) 
				{
					res=RESULT_EC_SUCCESS;
				}
				else 
				{
					res=RESULT_EC_NO_DATA;
				}
				fContinue=FALSE;
			} break;		

			default:
			{
				if (((ch>=0x30)&&(ch<=0x39))||  //0...9
					((ch>=0x41)&&(ch<=0x5A))||  //A...Z	
					((ch>=0x61)&&(ch<=0x7A)))	//a...z
				{
					fContinue=TRUE;
				}
				else 
				{
					res=RESULT_EC_NON_ASCII;
					fContinue=FALSE;
				}
			}break;
		} /* switch(ch) */
	} /* for (int i=0;(i<maxLength)&&(TRUE==fContinue);i++)  */
	return res;
}
