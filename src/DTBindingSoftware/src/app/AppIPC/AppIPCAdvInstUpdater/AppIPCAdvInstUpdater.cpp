#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppIPCAdvInstUpdater.h"
#include "utils/win32/win32filefind.h"
/*******************************************************************/
#define USE_PATH_PROGRAMFILES				1

// Wanneer we de applicatie in DEBUG mode uitvoeren dan worden de txt update bestanden gebruikt.
// Deze bevinden zich in een extra 'test/' subdirectory.
#define USE_CLOUDVERSIONS_TEST				0
/*******************************************************************/
#if (_DEBUG) 
	#undef USE_CLOUDVERSIONS_TEST
	#undef USE_PATH_PROGRAMFILES

	#define USE_CLOUDVERSIONS_TEST			0
	#define USE_PATH_PROGRAMFILES			1
#endif 
/*******************************************************************/
#if (USE_CLOUDVERSIONS_TEST == 1)
#pragma message("WARNING: The software updater is using the 'test' URL location")
#endif 
/*******************************************************************/
#if (USE_CLOUDVERSIONS_TEST == 0)
	#define BASE_URL_CLOUD_DATA_VARIA	"http://www.duotecno-cloud.be/data/varia/"
#else
	#define BASE_URL_CLOUD_DATA_VARIA	"http://www.duotecno-cloud.be/data/varia/test/"
#endif 

// Naam van de 'exe' updater
static const char* const UPDATER_NAME = "updaterBindingSoftware.exe";

// Naam van het ini/txt bestand die info bevat ivm de updates.
// Dit bestand wodrt aangemaakt in een advanced installer updater project.
static const char* const UPDATER_FILENAME_RELEASE = BASE_URL_CLOUD_DATA_VARIA "updaterBindingsoftware.txt";

// Naam van het ini/txt bestand die info bevat ivm de BETA updates.
// Dit bestand wodrt aangemaakt in een advanced installer updater project.
static const char* const UPDATER_FILENAME_BETA = BASE_URL_CLOUD_DATA_VARIA "updaterBindingsoftware_beta.txt";
 
// Naam van het ini/txt bestand die info bevat ivm de extra software packages.
// Dit bestand wodrt aangemaakt in een advanced installer updater project.
static const char* const UPDATER_FILENAME_EXTRA = BASE_URL_CLOUD_DATA_VARIA "updaterBindingsoftware_extra.txt";

// Naam van een bestandje die aangemaakt wordt door de advanced installer.
// Wordt aangemaakt wanneer de updater aangeroepen wordt via de /justcheck met extra params -dumpdetected <path>
// Bevat info ivm de gevonden updates.
static const char* const UPDATER_FILENAME_DETECTEDINFO = "duotecno\\detectedUpdates.txt";
/****************************** private ****************************/


#if (USE_PATH_PROGRAMFILES == 0)

static BOOL getFQPNUpdater(CString* pStrPathName) {

	if (NULL == pStrPathName) {
		ASSERT(0 != pStrPathName);
		return FALSE;
	}

	TCHAR path[MAX_PATH];
	if (0 == ::GetModuleFileName(NULL, path, MAX_PATH)) {
		ASSERT(0);
		return FALSE;
	}

    CString updaterPath(path);
    int ind = updaterPath.ReverseFind(_T('\\'));
    if (ind > 0)
      updaterPath = updaterPath.Left(ind);
    
	if (updaterPath[updaterPath.GetLength() - 1] != _T('\\'))
         updaterPath += _T('\\');

    updaterPath += UPDATER_NAME;
	
	*pStrPathName = updaterPath;
	return TRUE;
}

#endif 


#if (USE_PATH_PROGRAMFILES == 1)

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

static BOOL getPathNameUpdate(CString* pStrPathName)
{
	if (NULL == pStrPathName) {
		ASSERT(0 != pStrPathName);
		return FALSE;
	}

	char szPathName[MAX_PATH];
	memset(szPathName,0,sizeof(szPathName));

	// 0x0026 => PROGRAMFILES
	if (GetDirLocation(&szPathName[0],CSIDL_PROGRAM_FILES) == FALSE) {
		return FALSE;	
	}

	pStrPathName->Format("%s/Duotecno/Duotecno Installation Software",szPathName);
	return TRUE;
}

static BOOL getPathNameUpdateInfo(CString* pStrPathName)
{
	if (NULL == pStrPathName) {
		ASSERT(0 != pStrPathName);
		return FALSE;
	}

	char szPathName[MAX_PATH];
	memset(szPathName,0,sizeof(szPathName));

	// All Users\Application Data
	if (GetDirLocation(&szPathName[0],CSIDL_COMMON_APPDATA) == FALSE) {
		return FALSE;	
	}

	pStrPathName->Format("%s\\%s",szPathName,UPDATER_FILENAME_DETECTEDINFO);
	return TRUE;
}


static BOOL getFQPNUpdater(CString* pStrPathName) {

	CString s;
	if (!getPathNameUpdate(&s)) {
		return FALSE;
	}
	pStrPathName->Format("%s/%s",s,UPDATER_NAME);
	return TRUE;
}

#endif


typedef enum {
	// Controle of er nieuwe updates aanwezig zijn: Toont geen user interface.
	// Deze wordt gebruikt om bij het opstarten te controleren of er updates aanwezig zijn.
	COMMANDLINE_JUSTCHECK_RELEASE = 0,
	COMMANDLINE_JUSTCHECK_BETA,
	COMMANDLINE_JUSTCHECK_EXTRA,

	// Deze toont een user interface.
	// Zoekt naar updates en toont de mogelijkheid om de verschillende updates te installeren.
	COMMANDLINE_UPDATE_RELEASE,
	COMMANDLINE_UPDATE_BETA,
	COMMANDLINE_UPDATE_EXTRA

} CommandLineID_t;

static BOOL formatCommandLine(CommandLineID_t id,CString* pString) 
{
	BOOL result = FALSE;
	switch(id) {
		case COMMANDLINE_JUSTCHECK_RELEASE: {						
			// "updaterBindingSoftware.exe /justcheck"			
			CString strTemp(UPDATER_NAME);
			strTemp += CString(" /justcheck");			
			strTemp += CString(" -url ");	
			strTemp += CString(UPDATER_FILENAME_RELEASE);

			CString strFileNameInfo;
			if (getPathNameUpdateInfo(&strFileNameInfo)) {
				strTemp += " -dumpdetected ";
				strTemp += strFileNameInfo;
			}

			*pString = strTemp;
			return TRUE;							
		}
		case COMMANDLINE_JUSTCHECK_BETA: {			
			// "updaterBindingSoftware.exe /justcheck -url http://www.duotecno-cloud.be/data/varia/updaterBindingsoftware_beta.txt"						
			CString strTemp(UPDATER_NAME);
			strTemp += CString(" /justcheck");
			strTemp += CString(" -url ");	
			strTemp += CString(UPDATER_FILENAME_BETA);
			
			CString strFileNameInfo;
			if (getPathNameUpdateInfo(&strFileNameInfo)) {
				strTemp += " -dumpdetected ";
			strTemp += strFileNameInfo;
			}

			*pString = strTemp;
			return TRUE;							
		}

		case COMMANDLINE_JUSTCHECK_EXTRA: {
				
			// "updaterBindingSoftware.exe /justcheck -url http://www.duotecno-cloud.be/data/varia/updaterBindingsoftware_extra.txt"						
			CString strTemp(UPDATER_NAME);
			strTemp += CString(" /justcheck");
			strTemp += CString(" -url ");	
			strTemp += CString(UPDATER_FILENAME_EXTRA);
			
			CString strFileNameInfo;
			if (getPathNameUpdateInfo(&strFileNameInfo)) {
				strTemp += " -dumpdetected ";
			strTemp += strFileNameInfo;
			}

			*pString = strTemp;
			return TRUE;												  
		}

		case COMMANDLINE_UPDATE_RELEASE: {
			// "updaterBindingSoftware.exe /checknow"
			CString strTemp(UPDATER_NAME);
			strTemp += CString(" /checknow");
			strTemp += CString(" -url ");	
			strTemp += CString(UPDATER_FILENAME_RELEASE);
			*pString = strTemp;
			return TRUE;							
		}
		case COMMANDLINE_UPDATE_BETA: {
			// "updaterBindingSoftware.exe /checknow -url http://www.duotecno-cloud.be/data/varia/updaterBindingsoftware_beta.txt"
			CString strTemp(UPDATER_NAME);
			strTemp += CString(" /checknow");
			strTemp += CString(" -url ");	
			strTemp += CString(UPDATER_FILENAME_BETA);
			*pString = strTemp;		
			return TRUE;							
		}

		  case COMMANDLINE_UPDATE_EXTRA: {
			// "updaterBindingSoftware.exe /checknow -url http://www.duotecno-cloud.be/data/varia/updaterBindingsoftware_extra.txt"
			CString strTemp(UPDATER_NAME);
			strTemp += CString(" /checknow");
			strTemp += CString(" -url ");	
			strTemp += CString(UPDATER_FILENAME_EXTRA);
			*pString = strTemp;		
			return TRUE;							
		}
	}
	return FALSE;
}

/****************************** public ****************************/
AppIPCAdvInstallerUpdater::AppIPCAdvInstallerUpdater() : 
	m_flags(0)
{ 
	loadRegistryKeys();	
}

BOOL AppIPCAdvInstallerUpdater::checkForUpdates(SearchVersionType_t type,ResultInfo_t* info,CStringList* pListAvailableUpdates) 
{
	BOOL result = FALSE;
	DWORD dwResultCode = 0xffffffff;
	*info = RESULT_INFO_NONE;
		
	CommandLineID_t id = COMMANDLINE_JUSTCHECK_RELEASE;
	switch(type) {
		case SEARCHVERSIONS_RELEASE_ONLY:
			id = COMMANDLINE_JUSTCHECK_RELEASE;
		break;
		case SEARCHVERSIONS_RELEASE_AND_BETA:
			id = COMMANDLINE_JUSTCHECK_BETA;
		break;
		case SEARCHVERSIONS_EXTRA_SOFTWARE:
			id = COMMANDLINE_JUSTCHECK_EXTRA;
		break;
	}

	CString strCommandLine;
	if(!formatCommandLine(id,&strCommandLine)) {
		ASSERT(0);
		return RESULT_INFO_NONE;
	}

	// Bestand wissen die de info ivm de gevonden update bijhoudt...
	// Er wordt een nieuwe aangemaakt.
	CString strFileNameInfo;
	if (getPathNameUpdateInfo(&strFileNameInfo)) {
		DeleteFile(strFileNameInfo);
	}

	// Uitvoeren van de advanced installer via commandline params.
	if (TRUE == doRunUpdater(strCommandLine,&dwResultCode)) {
		if (0 == dwResultCode) {
			*info = RESULT_INFO_UPDATES_AVAILABLE;

			// Namen van de updates...
			readFileInfoUpdates(pListAvailableUpdates);

		}
		else {
			*info = RESULT_INFO_UPDATES_NONE;
		}
		result = TRUE;
	}
	return result;
}
	

BOOL AppIPCAdvInstallerUpdater::startUpdate(SearchVersionType_t type) 
{
	BOOL result = FALSE;
	DWORD dwResultCode = 0xffffffff;
	CommandLineID_t id = COMMANDLINE_UPDATE_RELEASE;
	switch(type) {
		case SEARCHVERSIONS_RELEASE_ONLY:
			id = COMMANDLINE_UPDATE_RELEASE;
		break;
		case SEARCHVERSIONS_RELEASE_AND_BETA:
			id = COMMANDLINE_UPDATE_BETA;
		break;
		case SEARCHVERSIONS_EXTRA_SOFTWARE:
			id = COMMANDLINE_UPDATE_EXTRA;
		break;
	}

	CString strCommandLine;
	if(!formatCommandLine(id,&strCommandLine)) {
		ASSERT(0);
		return FALSE;
	}

	if (doRunUpdater(strCommandLine,&dwResultCode)) {
		result = TRUE;
	}
	return result;
}

// protected...
BOOL AppIPCAdvInstallerUpdater::doRunUpdater(const CString& commandLine, DWORD* dwResultCode) {

	enum { TIMEOUT_WAIT_PROCESS_MSEC = (10 * 1000) };	// Maximum 10 seconden wachten.

	CString strUpdaterPath;
	if (FALSE == getFQPNUpdater(&strUpdaterPath)) {
		return FALSE;
	}

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	
    if (FALSE == ::CreateProcess(strUpdaterPath, const_cast<LPSTR>((LPCTSTR)commandLine),
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) 
	{
		return FALSE;
	}

	*dwResultCode = 0xffffffff;

	 // Wait until child process exits.
	if (WAIT_OBJECT_0 != WaitForSingleObject( pi.hProcess, TIMEOUT_WAIT_PROCESS_MSEC)) {
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return FALSE;
	}

	// Process has exited - check its exit code	
    DWORD exitCode = 0;
	if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
		*dwResultCode = exitCode;		
	}

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	return TRUE;
}


// Vlaggen.
void AppIPCAdvInstallerUpdater::setOptFlagCheckForBeta(BOOL flag,BOOL save) {
	if (flag) {
		m_flags |= FLAG_OPTION_BETA;
	}
	else {
		m_flags &= ~FLAG_OPTION_BETA;
	}

	if (save) {
		const CString szRegKeySection((LPCSTR) IDS_REGKEY_OPTIONSUPDATER);
		const CString szRegKeySubSection((LPCSTR) IDS_REGKEY_OPTIONSUPDATER_BETA);
		AfxGetApp()->WriteProfileInt(szRegKeySection, szRegKeySubSection, (flag == TRUE ? 1 : 0));
	}
}
BOOL AppIPCAdvInstallerUpdater::getOptFlagCheckForBeta() const {
	return (FLAG_OPTION_BETA == (m_flags & FLAG_OPTION_BETA));
}

void AppIPCAdvInstallerUpdater::setOptFlagCheckAtStartup(BOOL flag, BOOL save) {
	if (flag) {
		m_flags |= FLAG_OPTION_STARTUP;
	}
	else {
		m_flags &= ~FLAG_OPTION_STARTUP;
	}

	if (save) {		
		const CString szRegKeySection((LPCSTR) IDS_REGKEY_OPTIONSUPDATER);
		const CString szRegKeySubSection((LPCSTR) IDS_REGKEY_OPTIONSUPDATER_CHECK);
		AfxGetApp()->WriteProfileInt(szRegKeySection, szRegKeySubSection, (flag == TRUE ? 1 : 0));
	}
}
BOOL AppIPCAdvInstallerUpdater::getOptFlagCheckAtStartup() const {
	return (FLAG_OPTION_STARTUP == (m_flags & FLAG_OPTION_STARTUP));
}

void AppIPCAdvInstallerUpdater::loadRegistryKeys() 
{		
	const CString szRegKeySection((LPCSTR) IDS_REGKEY_OPTIONSUPDATER);		
	{
		const CString szRegKeySubSection((LPCSTR) IDS_REGKEY_OPTIONSUPDATER_BETA);
		BOOL checkForBeta = FALSE;
		const UINT tempValue = AfxGetApp()->GetProfileInt(szRegKeySection, szRegKeySubSection , 0);
		checkForBeta = (tempValue == 1 ? TRUE : FALSE);
		setOptFlagCheckForBeta(checkForBeta, FALSE);
	}

	{
		const CString szRegKeySubSection((LPCSTR) IDS_REGKEY_OPTIONSUPDATER_CHECK);
		BOOL ckeckAtStartup = TRUE;
		const UINT tempValue = AfxGetApp()->GetProfileInt(szRegKeySection, szRegKeySubSection, 1);
		ckeckAtStartup = (tempValue == 1 ? TRUE : FALSE);
		setOptFlagCheckAtStartup(ckeckAtStartup, FALSE);
	}
}


BOOL AppIPCAdvInstallerUpdater::readFileInfoUpdates(CStringList* pStrListAvailableUpdates) {
	if (NULL == pStrListAvailableUpdates) {
		return FALSE;
	}

	CString strFileName;
	if (!getPathNameUpdateInfo(&strFileName)) {
		return FALSE;
	}

	FILE* const fs = fopen(strFileName.GetBuffer(0),"r");
	if (fs == NULL) {    
		return FALSE;
	}
	
	enum { MAX_BUFFER_LENGTH = 255 };
	char buffer[MAX_BUFFER_LENGTH];
	char szAttribute[MAX_BUFFER_LENGTH];
	char szData[MAX_BUFFER_LENGTH];

	memset(buffer,'\0',sizeof(buffer));
	while(fgets(&buffer[0],(MAX_BUFFER_LENGTH-1),fs) != NULL) {
		memset(szAttribute,'\0',sizeof(szAttribute));	
		memset(szData,'\0',sizeof(szData));					
		sscanf(buffer,"%[^'=']=%[^\n]",szAttribute,szData);  // CHECKED_SSCANF_OK		
		if ((szAttribute[0]!=0x0a)&&
			(szAttribute[0]!=0x0d)&&		/* since 0x0C05 */
			(szAttribute[0]!=0x20)&& 
			(szAttribute[0]!=';')) 
		{
			if ((0 != szAttribute) && (0 == strcmp(szAttribute,"Name"))) {
				if (0 != szData) {
					pStrListAvailableUpdates->AddTail(szData);
				}
			}
		}
	}
	fclose(fs);	
	return TRUE;
}



// AppIPCAdvInstallerUpdater_Util
//
BOOL AppIPCAdvInstallerUpdater_Util::getNamesAvailable(CStringList& list,CString* pStr) {
	if (list.IsEmpty()) {
		return FALSE;
	}
	CString temp;
	POSITION pos = list.GetHeadPosition();
	while(pos!=NULL) {
		temp += "- ";
		temp += list.GetNext(pos);
		temp += "\n";
	}
	*pStr = temp;
	return TRUE;
}

BOOL AppIPCAdvInstallerUpdater_Util::checkUpdatesAtStartup(CWnd* pParent,AppIPCAdvInstallerUpdater& ref)
{
	BOOL runUpdater = FALSE;
	AppIPCAdvInstallerUpdater::SearchVersionType_t type = AppIPCAdvInstallerUpdater::SEARCHVERSIONS_RELEASE_ONLY;
	if (ref.getOptFlagCheckAtStartup()) {						
		if (ref.getOptFlagCheckForBeta()) {
			type = AppIPCAdvInstallerUpdater::SEARCHVERSIONS_RELEASE_AND_BETA;
		}						
		CStringList listNameAvailableUpdates;
		AppIPCAdvInstallerUpdater::ResultInfo_t res = AppIPCAdvInstallerUpdater::RESULT_INFO_NONE;
		if (ref.checkForUpdates(type,&res,&listNameAvailableUpdates)) {
			if (AppIPCAdvInstallerUpdater::RESULT_INFO_UPDATES_AVAILABLE == res) {
				CString msg;
				if (listNameAvailableUpdates.IsEmpty()) {
					msg = CString("New software updates available.\nClose the software and update now?");
				}
				else {
					CString temp;
					getNamesAvailable(listNameAvailableUpdates,&temp);
					ASSERT(!temp.IsEmpty());
					msg.Format("Software updates available:\n%s\nClose the software and update now?",temp);
				}
				if (IDYES == pParent->MessageBox(msg,"Software updates available",MB_YESNO | MB_ICONQUESTION)) {
					runUpdater = TRUE;
				}
			}
		}
	}
	if ((runUpdater == TRUE) && (ref.startUpdate(type))) {
		return TRUE;		
	}
	return FALSE;
}

BOOL AppIPCAdvInstallerUpdater_Util::checkSoftwareUpdates(CWnd* pParent,AppIPCAdvInstallerUpdater& ref,AppIPCAdvInstallerUpdater::SearchVersionType_t type) {

	BOOL runUpdater = FALSE;	
	if (ref.getOptFlagCheckAtStartup()) {														
		CStringList listNameAvailableUpdates;
		AppIPCAdvInstallerUpdater::ResultInfo_t res = AppIPCAdvInstallerUpdater::RESULT_INFO_NONE;
		if (ref.checkForUpdates(type,&res,&listNameAvailableUpdates)) {
			if (AppIPCAdvInstallerUpdater::RESULT_INFO_UPDATES_AVAILABLE == res) {
				CString msg;
				if (listNameAvailableUpdates.IsEmpty()) {
					msg = CString("New software updates available.\nClose the software and update now?");
				}
				else {
					CString temp;
					getNamesAvailable(listNameAvailableUpdates,&temp);
					ASSERT(!temp.IsEmpty());
					msg.Format("Software updates available:\n%s\nClose the software and update now?",temp);
				}
				if (IDYES == pParent->MessageBox(msg,"Software updates available",MB_YESNO | MB_ICONQUESTION)) {
					runUpdater = TRUE;
				}
			}
			else {
				CString msg("No software updates found");
				pParent->MessageBox(msg,"Search for software updates",MB_OK | MB_ICONASTERISK);
			}
		}
	}
	if ((runUpdater == TRUE) && (ref.startUpdate(type))) {
		return TRUE;		
	}
	return FALSE;
}
