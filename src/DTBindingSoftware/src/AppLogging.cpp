#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppLogging.h"
#include "utils/win32/win32filefind.h"
/***********************************************************************************************/

// C:\ProgramData\Duotecno Binding Software

/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
static BOOL GetDirLocation(char* szPathname,unsigned long ulCSIDL)
{
	ASSERT(NULL != AfxGetMainWnd());
	ASSERT(NULL != AfxGetMainWnd()->m_hWnd);

	if (SHGetSpecialFolderPath(AfxGetMainWnd()->m_hWnd,
							   szPathname,
							   ulCSIDL,
							   FALSE)==FALSE) 
	{
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
};

/***********************************************************************************************/
AppLogging& AppLogging::getInstance() {
	static AppLogging instance;
	return instance;
}

/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
AppLogging::AppLogging() : m_strFileName( ) { 
	init( );
}

void AppLogging::init() {
	char szPathName[MAX_PATH];
	memset(szPathName,0,sizeof(szPathName));

	// All Users\Application Data
	if (GetDirLocation(&szPathName[0],CSIDL_COMMON_APPDATA) == FALSE) {
		return;	
	}
	
	// Directory aanmaken.
	CString destDirectory;
	destDirectory.Format("%s/Duotecno Binding Software",szPathName);	
	if (CreateDirectory(destDirectory,NULL) == FALSE) {
		const DWORD dwResult = GetLastError();
		if (dwResult != ERROR_ALREADY_EXISTS) {
			ASSERT(0);
			return;
		}
	}

	// Oude logbestanden verwijderen.
	cleanUpDirectory(destDirectory);

	// bestandsnaam aanmaken.
	const CTime t = CTime::GetCurrentTime();
	m_strFileName.Format("%s/log_%s.log",destDirectory,t.Format(_T("%Y%m%d%H%M%S")));
}

void AppLogging::addInfo(const CString& msg) {
	CString tmpMsg;
	tmpMsg.Format("%s:INF:%s\r\n",getTimeDate(),msg);
	add(tmpMsg);
}
void AppLogging::addError(const CString& msg) {
	CString tmpMsg;
	tmpMsg.Format("%s:ERR:%s\r\n",getTimeDate(),msg);
	add(tmpMsg);
}
void AppLogging::addWarning(const CString& msg) {
	CString tmpMsg;
	tmpMsg.Format("%s:WRN:%s\r\n",getTimeDate(),msg);
	add(tmpMsg);
}

void AppLogging::add(const CString& msg) {	
	ASSERT(!msg.IsEmpty());	
	if (m_strFileName.IsEmpty()) {
		ASSERT(!m_strFileName.IsEmpty());
		return;
	}
	FILE* fp = fopen ((LPCSTR)m_strFileName, "a");
	if (NULL == fp) {
		return;
	}
	fputs((LPCSTR)msg,fp);
	fclose(fp);
}

CString AppLogging::getTimeDate( ) {	
	const CTime t = CTime::GetCurrentTime();  
	return t.Format(_T("%Y/%m/%d %H:%M:%S"));
}

	
void AppLogging::cleanUpDirectory(const CString& strDir) {
	CString szFindFileName;
	CString szRemoveFileName;
	szFindFileName.Format("%s\\*.log",strDir);
	int count = 0;
	FILETIME previousTime = { 0xffffffff, 0xffffffff };
	IFileFindIterator* it = 0;
	for (it = CFileFindFactory::createIterator(szFindFileName);!it->isDone();it->next()) {
		CFileFindData* const pFileFindData = it->currentItem( );
		if ((NULL != pFileFindData) && (NULL != pFileFindData->getFileName())) {						
			const FILETIME fileTime = ((LPWIN32_FIND_DATA)pFileFindData)->ftCreationTime;
			if (-1 == CompareFileTime(&fileTime,&previousTime)) {
				szRemoveFileName = pFileFindData->getFileName();
				previousTime = fileTime;
			}
			count++;
		}
	}

	if ((count >= MAX_LOGFILES) && (!szRemoveFileName.IsEmpty())) {
		CString strFullName;
		strFullName.Format("%s//%s",strDir,szRemoveFileName);
		DeleteFile(strFullName);		
	}
}
/***********************************************************************************************/