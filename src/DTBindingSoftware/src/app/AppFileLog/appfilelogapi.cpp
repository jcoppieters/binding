#include "stdafx.h"
#include "BindingSoftware.h"
#include "appfilelogapi.h"

#include "utils/win32/ShellCommands.h"
#include "utils/win32/ShellFunctions.h"

// -----------------------------------
#include "TMGlobals.h"
using namespace TMGlobalSettings;

#include "TMConvenience.h"
using namespace TMConvenience;
// ------------------------------------


/****************************************************************************************/

const char* const APPFILELOG_BINDINGFILES_LOAD_EC = "bindfilesload ec.txt";
const char* const APPFILELOG_BINDINGFILES_COMP_EC = "bindfilescomp ec.txt";
const char* const APPFILELOG_BINDINGFILES_ADD_EC = "bindfilesadd ec.txt";
const char* const APPFILELOG_BINDINGFILES_PROPERTY_ADD_EC = "bindfilesaddpbind ec.txt";
const char* const APPFILELOG_BINDINGFILES_MANUALBINDINGS = "manual bindings.txt";

/****************************************************************************************/


namespace
{
	// Momenteel is dit dezelfde functie als in timerdoc...
	// TODO !!!
	BOOL CreateAppSubDirectory(CString szSubDirectory)
	{
		CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());

		if (szPathName.IsEmpty()) 
		{
			CString szInfo(_T("First Save installation, so a current folder name can be retrieved ..."));
			AfxMessageBox(szInfo);
			return FALSE;
		}

		CString szNewPathName;
		szNewPathName.Format(_T("%s%s"),
							 szPathName,
							 szSubDirectory);

		DWORD dwResult = CreateSubDirectory(szNewPathName); 
		if (dwResult != 0) 
		{
			CString szInfo;
			szInfo.Format("CreateDirectory returned error code : %u",dwResult);
			AfxMessageBox(szInfo);
			return FALSE;
		}
		return TRUE;
	}
};




// public:

void CAppFileLogAPI::formatFullName(
	CString* pStrFullName,
	const char* szFileName)
{
	CString SubDir((LPCSTR) IDS_FILENAME_SUBDIRLOGFILE );
	CString szPathName = TMGetFileSettings()->GetFullyQualifiedPathName();

	if(0!=pStrFullName)
	{
		pStrFullName->Format("%s%s\\%s",
			  szPathName,
			  SubDir,
			  szFileName);
	}
}

void CAppFileLogAPI::showLogFileErrors(
	const char* s,
	const char* szFileName,
	CStringArray& refStringsError)
{
	if(!refStringsError.IsEmpty())
	{
		CString szMessage;
		
		szMessage.Format(_T("%s\r\nShow logfile ?"),s);

		if (IDYES==AfxMessageBox(szMessage,MB_YESNO))
		{
			showLogFileErrors(szFileName);
		}
	}
}


void CAppFileLogAPI::showLogFileErrors(
	const char* szFileName)
{
	CString szFilenameFull;
	formatFullName(&szFilenameFull,szFileName);

	TCommandShell<> shellCommand(szFilenameFull);
	shellCommand.Execute( );
}

void CAppFileLogAPI::saveLogFile(
	const char* szFileNameFull,
	CStringArray& refStringsError)
{
	FILE* f=0;

	f=fopen((LPCTSTR)szFileNameFull,"w");

	if (NULL!=f)
	{
		int i;
		const int count=refStringsError.GetCount();				

		for(i=0;i<count;++i)
		{
			fputs(refStringsError.GetAt(i),f);		// CHANGES_0X0A01_USE_FPUTS
			fputs("\r\n",f);						// CHANGES_0X0A01_USE_FPUTS
		}										
		fclose(f);
	}
}


void CAppFileLogAPI::createLogFile(
	const char* szFileName,
	CStringArray& refStringsError)
{
	if (0!=szFileName)
	{
		CString szFilenameFull;
		formatFullName(&szFilenameFull,szFileName);

		if(refStringsError.GetCount()>0)
		{		
			CString SubDir((LPCSTR) IDS_FILENAME_SUBDIRLOGFILE );

			if (TRUE==CreateAppSubDirectory(SubDir))
			{
			
			}
							
			saveLogFile(szFilenameFull,refStringsError);
		}
		else
		{
			DeleteFile(szFilenameFull);
		}
	}
}