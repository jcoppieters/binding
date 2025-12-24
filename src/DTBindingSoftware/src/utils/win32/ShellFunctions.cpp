#include "stdAfx.h"
#include "ShellFunctions.h"
#include "NotifyError.h"


EXTERN BOOL CreateSubDirectory(CString szPathName,CString szNewSubDir)
{
	if (szPathName.IsEmpty() == TRUE)
	{
		CString szInfo("First save the project");
		AfxMessageBox(szInfo);
		return FALSE;
	}
	
	CString szNewDirectory;
	DWORD dwResult;

	szNewDirectory.Format(_T("%s%s\\"),szPathName,szNewSubDir);

	if ((dwResult=CreateSubDirectory(szNewDirectory)) != 0) 
	{
		// TM 21/01/2008
		CNotifyError_Simple notify;
		notify(GetLastError());

		return FALSE;
	}

	return TRUE;
}

DWORD CreateSubDirectory(CString szNewPathName)
{

	// 1st parameter specifies the search path ...
	CString szSearchPath;

	const int iPos = szNewPathName.ReverseFind('\\');
	
	if (iPos != -1) 
	{
		szSearchPath = szNewPathName.Left(iPos);
	}
	else 
	{
		szSearchPath.Empty();
	}

	// TM_CHANGES_BUG_0144: 
	// buffer array size is MAX_PATH, dus lengte is MAX_PATH-1
	char path_buffer[_MAX_PATH];
	LPTSTR pszPath;
	
	DWORD dwResult = SearchPath(szSearchPath,
								szNewPathName,
								NULL,
								_MAX_PATH-1,
								&path_buffer[0],
								&pszPath);
	
	if (dwResult == 0) 
	{	
		if (CreateDirectory(szNewPathName,NULL) == FALSE) 
		{	 
			dwResult = GetLastError();
			if (dwResult == ERROR_ALREADY_EXISTS)
			{
				// Win98 function : SearchPath fails ...			
				return 0;
			}

			return dwResult;
		}
	}

	return 0;
}

BOOL FindFile(CString szPathName,CString szFileName)
{
	// TM_CHANGES_BUG_0144: 
	// buffer array size is MAX_PATH, dus lengte is MAX_PATH-1
	char path_buffer[_MAX_PATH];
	LPTSTR pszPath;

	DWORD dwResult = SearchPath(szFileName,
							  szPathName,
							  NULL,
							  _MAX_PATH-1,
							  &path_buffer[0],
							  &pszPath);

	if (dwResult== 0) 
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL FindFile(CString szFulPathName,WIN32_FIND_DATA *FileInfo)
{
	const HANDLE hFind = FindFirstFile(szFulPathName,FileInfo);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return FALSE;
	}
	else 
	{
		return TRUE;
	}
}

EXTERN BOOL FindFile(CString szFulPathName)
{
	WIN32_FIND_DATA FileInfo;	
	return (FindFile(szFulPathName,&FileInfo));
}

EXTERN BOOL FindFileDate(CString szFulPathName,CString *szFileDate)
{
	WIN32_FIND_DATA FileInfo;
	FILETIME	FileTime;
    SYSTEMTIME st;
	
	if (FindFile(szFulPathName,&FileInfo) == FALSE) 
	{
		szFileDate->Empty();
		return FALSE;
	}
	else 
	{	
		if (FileTimeToLocalFileTime(&(FileInfo.ftLastWriteTime),&FileTime)) 
		{						
			if (FileTimeToSystemTime(&FileTime,&st)) 
			{
				char szLocalDate[255], szLocalTime[255];

				GetDateFormat(LOCALE_USER_DEFAULT, 
							  DATE_SHORTDATE, &st, NULL,
							  szLocalDate, 
							  255 );
			
				GetTimeFormat(LOCALE_USER_DEFAULT, 
							  0, 
							  &st, 
							  NULL, 
							  szLocalTime, 
							  255 );
				
				szFileDate->Format("%s %s",szLocalDate,szLocalTime);
	
				return TRUE;	
			}
		}
	}

	szFileDate->Format("Error -Data+Time");

	return TRUE;
}