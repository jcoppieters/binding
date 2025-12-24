// BindingSoftwareDoc.cpp : implementation of the CTimerDoc class
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingSoftwareDoc.h"
// ---------------------------------
#include "MainFrm.h"
#include "MyWindowMessages.h"
#include "ExportHtmlFilesApp.h"
#include "ExportTextFilesApp.h"

#include "app\appfileio\AppFileIoAPI.h"
#include "app\applicactcode\applicactcodeAPI2.h"
#include "app\applicactcode\applicactcode.h"

#include "utils/app/DisplayDevice.h"
#include "utils/win32/ShellFunctions.h"	
#include "utils/win32/NotifyError.h"

// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
using namespace TMBindingDatabase;
using namespace TMGlobalSettings;

#include "TMConvenience.h"
using namespace TMConvenience;
// ------------------------------------



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/*////////////////////////////////////////////////////////////////

  Remarks :

----------

  * SetPathName(CString szPathName,TRUE);
	szPathName holds full filename :
		c:\Program files\Duotecno\Installations\20040630\20040630.duo

		--> Mainframe converts this pathname to 
				"c:\Program files\Duotecno\Installations\20040630\"
			other file I/O Operatons user this pathName ...

----------

  * BOOL SetActiveProjectDirectory(void)
  * CString GetDefaultActiveProjectDirectory(void);
	
	  Functions used to access Registry : Retrieve + Store active project
	  directory:
		
		  c:\Program files\Duotecno\Installations\


-----------

  Remarks 20040630 (BUG-0121: Veranderd - GetCurrentDirectory( ) mag niet meer gebruikt worden.)
		
	  To be compatible WIN98 / ME / XP we must set flag	
		" CFileOpen.m_ofn.lpstrInitialDir = szWorkingDir; "
	  specifying current directory ...
	  (Cannot use SetCurrentDirectory anymore!)

//////////////////////////////////////////////////////////////////*/

/////////////////////////////////////////////////////////////////////////////
// CTimerDoc

IMPLEMENT_DYNCREATE(CTimerDoc, CDocument)

BEGIN_MESSAGE_MAP(CTimerDoc, CDocument)
	//{{AFX_MSG_MAP(CTimerDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerDoc construction/destruction

CTimerDoc::CTimerDoc() :
m_fDataLoaded(FALSE), m_fDataSaved(FALSE)
{
	// TODO: add one-time construction code here
}

CTimerDoc::~CTimerDoc()
{
}

BOOL CTimerDoc::OnNewDocument()
{
	static BOOL fInit = FALSE;
	LONG result = FALSE;

	if (fInit == TRUE)
	{
		if (AfxGetMainWnd() != NULL) 
		{		
			// Vragen aan de gebruiker of er een nieuw prj mag geopend worden.
			// TRUE = Ja, FALSE = nee
			result = AfxGetMainWnd()->SendMessage(WM_APPLICATION_FILENEW_PRE);
			
			if (result == FALSE)
			{			
				// Herstellen van de naam van het document. 
				CString szTitle(TMGetFileSettings()->GetTitle());
				AfxGetMainWnd()->SetWindowText(szTitle);

				return FALSE;		// Hier stoppen !!!
			}
		}
	}

	if (!CDocument::OnNewDocument())
		return FALSE;
		
	if (AfxGetMainWnd() != NULL) 
	{		
		AfxGetMainWnd()->PostMessage(WM_APPLICATION_FILENEW);
	}

	TMGetFileSettings()->Clear();
	fInit = TRUE;
	return TRUE;
}

BOOL CTimerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	TMInitialiseDatabases();

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return TRUE;
}

void CTimerDoc::OnCloseDocument() 
{
	TMInitialiseDatabases();
	CDocument::OnCloseDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CTimerDoc serialization

void CTimerDoc::Serialize(CArchive& ar)
{
	CFile* fp = ar.GetFile();
	
	TRACE1("\nCTimerDoc::Serialize - fp-Name:'%s'",fp->GetFileName());
	TRACE1("\nCTimerDoc::Serialize - fp-path:'%s'",fp->GetFilePath());

	if (ar.IsStoring()) {	
		// Opslaan.

		// CHANGES_ACTIVATION_LICENSE_CODE
		// De applicatie 'CAppLicActCode' instantie meegeven in de ctor.
		const CAppLicActCode& refApplicActCode = CAppLicActCodeAPI2::getLicActCode( );
		CAppFileIoAPI fileIO(TMGetDocumentNodes(),
							 TMGetBindingEntries(),
							 const_cast<CAppLicActCode*>(&refApplicActCode));

		TMGetBindingApp()->UpdateData(FALSE);													
																	   
		if (fileIO.Save(fp)) {			
			TMSaveDatabases();			
		}	


		//
		// Oppassen !!!!
		// Mogen hier geen bestanden opslaan onder de 'CONFIG' en 'DATA' subdirectories.
		// zie BUG-0172
		// 
		
		CExportTextFilesApp txtExport;		// CHANGES_MVS_2008
		txtExport.StartExport( );			// CHANGES_MVS_2008

		// Sedert V16.11: Oplossing BUG-0166
		m_fDataSaved = TRUE;
	}
	else 
	{
		// Laden.

		// CHANGES_ACTIVATION_LICENSE_CODE
		// Een tijdelijke 'CAppLicActCode' instantie aanmaken om in te laden.
		// Indien correct ingeladen is, instantie doorgeven aan de Applicatie.

		CAppLicActCode appLicActCode;
		CAppFileIoAPI fileIO(TMGetDocumentNodes(),TMGetBindingEntries(),&appLicActCode);
		if (fileIO.Load(fp)) 
		{	
			CAppLicActCodeAPI2::setLicActCode(appLicActCode);

			SetPathName(fp->GetFilePath(),TRUE);
			TMGetFileSettings()->UpdateData(this);		

			// Sedert V16.09: inladen van de inlog gegevens start dialoog
			// Methode connect: offline / usb / tcpip
			// tcp settings local & remote.
			// Oppassen: MOET AANGEROEPEN WORDEN NA TMGetFileSettings()->UpdateData()
			//
			// Sedert V16.10: Inladen van de node services.
			// Bevat een lijst van de nodes + services/protocols die ze ondersteunen.
			CMainFrame* const pMainWnd = (CMainFrame*)(AfxGetMainWnd());
			if (0 != pMainWnd) {
				pMainWnd->loadCfgLoginInfo();
				pMainWnd->loadCfgNodeServices();
			}

			// CHANGES_INCOMPATIBLE_VERSION
			m_fDataLoaded=TRUE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTimerDoc diagnostics

#ifdef _DEBUG
void CTimerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTimerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



/**
 * Ref: BUG-0167
 * Lange namen van de title en bestansnaam worden door de CFileDialog niet juist teruggegeven.
 * Zie voorbeelden hieronder.
 * 
 * Oplossing: Door zelf namen aan te maken adhv strfilePathName
 *
 * fileTitle='202210202 testopstelling - problem solve wizard - test'
 * fileName='202210202 testopstelling - problem solve wizard - test.duo'
 * filePathName='D:\developer\test installations\binding software installations\0x1000\202210202 testopstelling - problem solve wizard - test.duo'
 * 
 * fileTitle=20230203 - abcdefghijklmnopqrstuvwqyz123456789abcdefghijklmnopq
 * fileName=20230203 - abcdefghijklmnopqrstuvwqyz123456789abcdefghijklmnopq 
 * filePathName=D:\developer\test installations\binding software installations\0x1000\20230203 - abcdefghijklmnopqrstuvwqyz123456789abcdefghijklmnopqrstuvwxyz0123456789.duo
 */
static void NormalizeFileNames(const CString& strfileTitle, const CString& strfileName,const CString& strfilePathName,
	 						  CString* pStringTitle, CString* pStringFileName, CString* pStringPathName) 
{

	CString fileTitle = strfileTitle;
	CString fileName = strfileName;
	CString filePathName = strfilePathName;

	fileTitle.MakeLower();
	fileName.MakeLower();
	filePathName.MakeLower();

	TRACE("fileTitle='%s'\n",fileTitle);
	TRACE("fileName='%s'\n",fileName);
	TRACE("filePathName='%s'\n",filePathName);

	BOOL validNames = TRUE;

	/////////////////////////////////////////////////////////////////////////////////
	// Controles uitvoeren...	
	/////////////////////////////////////////////////////////////////////////////////
	if (validNames) {
		// De laatste 4 char van de bestandsnaam moeten ".duo" zijn.
		CString strTemp = fileName.Right(4);
		if (0 != strTemp.Compare(".duo")) {
			validNames = FALSE;
		}	
	}

	if (validNames) {
		// Wanneer we de laatste 4 chars verwijderen dan moet de bestandsnaam gelijk zijn aan de title...
		CString strTemp = fileName;
		if (strTemp.GetLength()>4) {
			strTemp = strTemp.Left(strTemp.GetLength()-4);	// ".duo" verwijderen.
			if (0 != strTemp.Compare(fileTitle)) {
				validNames = FALSE;
			}
		}
	}

	// ??
	// Todo: Kunnen we nog andere controls uitvoeren ?
	// ??

	///////////////////////////////////////////////////////////////////////////////////////////
	// Indien de namen correct zijn dan deze doorgeven anders zelf de bestandsnamen aanmaken...
	///////////////////////////////////////////////////////////////////////////////////////////
	if (validNames) {
		*pStringTitle = fileTitle;
		*pStringFileName = fileName;
		*pStringPathName = filePathName;
	}
	else {

		BOOL formatOk = FALSE;
		CString szTemp = filePathName.Right(4);
		if (0 == szTemp.Compare(".duo")) {
			int fileSeperator = filePathName.ReverseFind('\\');
			if (fileSeperator > 0) {
				const int length = (filePathName.GetLength() - (fileSeperator+1));
				if (length > 4) {	
					CString s = filePathName.Right(length);
					*pStringFileName = s;										
					*pStringTitle = s.Left(s.GetLength()-4);	// ".duo" verwijderen...
					*pStringPathName = filePathName;
					formatOk = TRUE;
				}
			}
		}
		
		// Indien we de namen niet kunnen bepalen dan standaard namen geven.
		// "C:\\Users\\tom\\AppData\\Roaming\\202302031133.duo"
		if (!formatOk) {

			TCHAR szPath[MAX_PATH];
			memset(szPath, 0, sizeof(szPath));
			if (FAILED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
				sprintf(szPath,"c:\\temp");
			}

			const CTime t1 = CTime::GetCurrentTime();
			const CString strTemp = t1.Format("%Y%m%d%H%M");

			pStringFileName->Format("%s.duo",strTemp);
			*pStringTitle = strTemp;
			pStringPathName->Format("%s\\%s",szPath,*pStringFileName);
		}
	}
}

/*****************************************************************************************
 * strPathName		"d:\developer\test installations\binding software installations\0x1010\20220207 test"
 *						of
 * strPathName		"d:\developer\test installations\binding software installations\0x1010\20220207 test.duo"
 *
 * strFileTitle		"20220207 test"
 *
 *****************************************************************************************/
static CString getProjectDir(const CString& strFileTitle, const CString& strPathName)
{
	int lengthPathName = strPathName.GetLength();
	const int lengthFileTitle = strFileTitle.GetLength();

	if (lengthPathName < lengthFileTitle) {
		ASSERT(0);
		return CString("");
	}

	int lengthExtension = 0;
	if (strPathName.Find(".duo",strPathName.GetLength()-4)>0) {
		lengthExtension = 4;
	}

	CString projectDir = strPathName.Left(lengthPathName - lengthFileTitle - lengthExtension);
	ASSERT(projectDir.GetAt(projectDir.GetLength()-1) == '\\');
	return projectDir;
}

/////////////////////////////////////////////////////////////////////////////
// CTimerDoc commands
//
// NOTE_SETPATHNAME( )
//
// Probleem: Vroeger werd in de CreateFolder( ) de SetPathName(xx,TRUE) aangeroepen.
// TRUE --> bAddToMRU (Add to recent file list.)
// In VS2015 krijgen we een fout _AfxSHCreateItemFromParsingName( ) antwoord met ERROR_FILE_NOT_FOUND.
// Functie mislukt omdat hij eerst controleert of het bestand aanwezig is op deze plaats.
//
// Oplossing: CreateFolder( ) herwerkt. Laatste param = filePath.
// Roepen voor het opslaan eens SetPathName( xxx, FALSE) aan: Zorgt ervoor dat de app weet welke
// directories er gebruikt worden.
// Na het opslaan roepen eens SetPathName( xxx, TRUE) aan zodat het path bestaat en deze toegevoegd wordt
// aan de laatst opgeslagen locaties.
//
//
// Zie BUG-0168:
// Gebruik van strFullQualifiedFileName en strFullQualifiedPathName
// 
// 
BOOL CTimerDoc::SaveApplicationDirectory(void)
{
	BOOL fResult = FALSE;
	const CString szAppDocName(GetTitle());

	CFileDialog CFileSave(FALSE,_T("duo"), szAppDocName, OFN_HIDEREADONLY,
						  _T("Duotecno Files (*.duo)|*.duo||"), NULL); 
	
	if (TMGetFileSettings() != NULL)
	{
		CString szWorkingDir(TMGetFileSettings()->GetDefaultActiveProjectDirectory());

		TRACE("Working DIR = ");
		TRACE(szWorkingDir);
		TRACE("\n");

		if (szWorkingDir.IsEmpty() == FALSE) {
			CFileSave.m_ofn.lpstrInitialDir = szWorkingDir;	
		}
	}
	
	if (CFileSave.DoModal() == IDOK) 
	{		
		CString strTmpFileTitle(CFileSave.GetFileTitle());
		CString strTmpFileName(CFileSave.GetFileName());		
		CString szTmpPathName(CFileSave.GetPathName( ));

		CString strFileTitle;
		CString strFileName;
		CString strFilePathName;
		CString strFullQualifiedPathName;				// NOTE_SETPATHNAME
		CString strProjectDir;							// De directory waar alle projecten opgeslagen worden.
		CString strFullQualifiedFileName;


		// strTmpFileTitle = "20230206 nieuw project"
		// strTmpFileName = "20230206 nieuw project.duo"
		// szTmpPathName = "D:\\developer\\test installations\\20230206 nieuw project\\20230206 nieuw project.duo"
		NormalizeFileNames(strTmpFileTitle, strTmpFileName, szTmpPathName, &strFileTitle, &strFileName, &strFilePathName);


		// strFileTitle = "20230206 nieuw project"
		// strFileName = "20230206 nieuw project.duo"
		// strFilePathName = "d:\\developer\\test installations\\20230206 nieuw project\\20230206 nieuw project.duo"


		///////////////////////////////////////////////////////////////////////////////////////////////////
		// strPathName is soms 
		//	"d:\developer\test installations\binding software installations\0x1010\20220207 test"		(1 = OK)
		//		of
		//	"d:\developer\test installations\binding software installations\0x1010\20220207 test.duo"	(2 = NIET OK)
		//
		CreateFolder(strFilePathName, strFileTitle, strFileName, &strFullQualifiedFileName,&strFullQualifiedPathName);		// BUG-0121
		
		// strPathName		"d:\developer\test installations\binding software installations\0x1010\20220207 test"
		// strFileTitle		"20220207 test"
		// strFileName		"20220207 test.duo"

		strProjectDir = getProjectDir(strFileTitle, strFullQualifiedPathName);


		// strProjectDir					"d:\developer\test installations\binding software installations\0x1010\
		// strFullQualifiedPathName			"d:\developer\test installations\binding software installations\0x1010\20220207 test"

		// Sedert V16.16: NOTE_SETPATHNAME
		SetPathName(strFullQualifiedPathName, FALSE);				// Zie BUG-0168 - Oppassen: 'Wordt hier toegekend zonder '\\'


		// Sedert V16.17 (Zie BUG-0168)
		TMGetFileSettings()->UpdateData(this, strFullQualifiedFileName,strFullQualifiedPathName,strProjectDir,strFileName);

		if (1)
		{
			CString s;
			s.LoadString(IDS_FILENAME_SUBDIR_DATA);
			CreateAppSubDirectory(s);

			s.LoadString(IDS_FILENAME_SUBDIRCONFIG);
			CreateAppSubDirectory(s);			
		}

	
		// Oppassen: Zie BUG-0168
		// Volledige bestandsnaam met *.duo extensie !
		OnSaveDocument(strFullQualifiedFileName);				

		// Sedert V16.16: NOTE_SETPATHNAME
		SetPathName(strFullQualifiedPathName, TRUE);				// Zie BUG-0168 - Oppassen: 'Wordt hier toegekend zonder '\\'

		// Sedert V16.46: Expliciet de naam van het bestand nemen als title van het venster.
		// Wanneer we dit niet doen is er soms een probleem dat de "*.duo" niet in de naam van het venster zit.
		// Deze "*.duo" extensie wordt wel doorgegeven aan de configurator.
		// Beide moeten identiek zijn voor de IPC communicatie tss de binding software en de configurator.				
		CDocument::SetTitle(strFileName);
	
		fResult = TRUE;
	}	
	return fResult;
}



#define DBG_DUMP_ENABLE		1

#if(DBG_DUMP_ENABLE == 1)
	#define DBG_DUMP(x,y)				dump(x,y)
	#define DBG_DUMP_SEPERATOR(x)		dumpSeperator(x)
#else
	#define DBG_DUMP(x,y)	
	#define DBG_DUMP_SEPERATOR(x)
#endif 

#if(DBG_DUMP_ENABLE == 1)
static void dump(const CString& key, const CString& value) {

	CString s;
	s.Format("%s=%s\n",key,value);
	TRACE(s);
}
static void dumpSeperator(CString s) {
	TRACE("--------------- %s ---------------------\n",s);
}
#endif




// When we save the project under a new project directory, certain folders
// have to be copied under this new project-dir...
// After the prj. is saved we compare previous and current directory name 
// if they are different the subfolders are copied to the new prj. dir !
void CTimerDoc::OnFileSaveAs() 
{
	//Store previous directory so we can copy folders ...
	const HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));

	if (AfxGetMainWnd() != NULL)		// TM 20100511
	{		
		AfxGetMainWnd()->SendMessage( WM_APPLICATION_FILESAVE_PRE );
	}

	SaveApplicationDirectory();

	CString szPreviousPath(TMGetFileSettings()->GetPreviousPathName());
	CString szCurrentActivePath(TMGetFileSettings()->GetFullyQualifiedPathName());

	if (!szPreviousPath.IsEmpty())				/* TM 20110614 : BUG-0123 */
	{
		szPreviousPath.MakeLower( );
	}

	if (!szCurrentActivePath.IsEmpty())			/* TM 20110614 : BUG-0123 */
	{
		szCurrentActivePath.MakeLower( );
	}
	
	if ((szPreviousPath.IsEmpty() == FALSE) && (szCurrentActivePath != szPreviousPath)) {			
		CopyDirectories(szCurrentActivePath,szPreviousPath);
	}

	// Sedert V16.28: zie BUG-0172
	// Extra data onder de 'CONFIG' subdirectory opslaan.
	this->saveAdditionalConfigFiles( );

	if (AfxGetMainWnd() != NULL) {			
		AfxGetMainWnd()->SendMessage( WM_APPLICATION_FILESAVE_POST );
	}

	DBG_DUMP_SEPERATOR("OnFileSaveAs - Enter");
	DBG_DUMP("szPreviousPath",szPreviousPath);
	DBG_DUMP("szCurrentActivePath", szCurrentActivePath);
	DBG_DUMP_SEPERATOR("OnFileSaveAs - Exit");

	SetCursor(oldCursur);
}

void CTimerDoc::OnFileSave() 
{
	HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));

	if (AfxGetMainWnd() != NULL)	// TM 20100511
	{	
		AfxGetMainWnd()->SendMessage( WM_APPLICATION_FILESAVE_PRE );
	}

	CString FilePath(GetPathName());
	if (FilePath.IsEmpty())
	{			
		SaveApplicationDirectory();			

		DBG_DUMP_SEPERATOR("OnFileSave - path empty - Enter");
		DBG_DUMP("FilePath",FilePath);		
		DBG_DUMP_SEPERATOR("OnFileSave - Exit");
	}
	else 
	{						
#if(0)
		SetActiveProjectDirectory(FilePath);
		OnSaveDocument(FilePath);	
#endif 

		const CString szFullyQualifiedFileName(TMGetFileSettings()->GetFullyQualifiedFileName());
		OnSaveDocument(szFullyQualifiedFileName);

		DBG_DUMP_SEPERATOR("OnFileSave - Enter");
		DBG_DUMP("FilePath",FilePath);
		DBG_DUMP("szFullyQualifiedFileName",szFullyQualifiedFileName);
		DBG_DUMP_SEPERATOR("OnFileSave - Exit");
	}

	// Sedert V16.28: zie BUG-0172
	// Extra data onder de 'CONFIG' subdirectory opslaan.
	this->saveAdditionalConfigFiles( );

	if (AfxGetMainWnd() != NULL) {			
		AfxGetMainWnd()->SendMessage( WM_APPLICATION_FILESAVE_POST );
	}

	SetCursor(oldCursur);
}

void CTimerDoc::saveAdditionalConfigFiles( ) {
	// Sedert V16.09: Oplsaan van de inlog gegevens start dialoog
	// Methode connect: offline / usb / tcpip
	// tcp settings local & remote.
	//
	// Sedert V16.10: Opslaan van de node services.
	// Bevat een lijst van de nodes + services/protocols die ze ondersteunen.
	//
	// Sedert V16.19: saveCfgLoginInfoExport( )
	// Info voor de cfg'er ivm de login (usb / tcp local of remote / offline).
	//
	// Sedert V16.28: saveCfgNodeServicesExport( )
	// Info voor de cfg'er ivm de nodes (en hun tcp/ip instellingen.)
	CMainFrame* const pMainWnd = (CMainFrame*)(AfxGetMainWnd());
	if (0 != pMainWnd) {
		pMainWnd->saveCfgLoginInfo();
		pMainWnd->saveCfgNodeServices();
		pMainWnd->saveCfgLoginInfoExport();
		pMainWnd->saveCfgNodeServicesExport();
	}
}

BOOL CTimerDoc::CopyDirectory(CString szDestination,CString szSource,CString szSubFolder,CString szExtension)
{
	CString szNewSourceFolder,szNewDestFolder;
	CString szSourceFileName;
	CString szDestFileName;

	BOOL fFinished = FALSE;

	WIN32_FIND_DATA FileData; 
	
	szNewSourceFolder.Format(_T("%s%s"),szSource,szSubFolder);
	szNewDestFolder.Format(_T("%s%s"),szDestination,szSubFolder);	
	szSourceFileName.Format(_T("%s%s"),szNewSourceFolder,szExtension);

	HANDLE hSearch = FindFirstFile(szSourceFileName,&FileData);
	if (hSearch == INVALID_HANDLE_VALUE) 
	{
		FindClose(hSearch);
		return FALSE;		// Can't find 1st file -> Break ...
	}			

	if (CreateSubDirectory(szNewDestFolder) == 0)	// Create subdirectory in destination folder ...
	{	
		// TM: 20080417: Copy all files from a source to a destination directory.
		// if FindFirstFile() is called with the "*.*" wildcards then FileData.cFileName == ".".
		// This indicates a directory has to be created ...
		if (strcmp(FileData.cFileName,".") == 0)
		{		
			FindClose(hSearch);

			szNewSourceFolder.TrimRight("\\");
			szNewDestFolder = szDestination;
			szNewDestFolder.TrimRight("\\");

			SHFILEOPSTRUCT sfo;
			sfo.hwnd = 0;
			sfo.wFunc = FO_COPY;
			sfo.pFrom = (LPCSTR) szNewSourceFolder;
			sfo.pTo = (LPCSTR) szNewDestFolder;
			sfo.fFlags = FOF_SIMPLEPROGRESS | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR; //  | FOF_RENAMEONCOLLISION;
			sfo.lpszProgressTitle  = "Copying files";

			const int result = SHFileOperation(&sfo);
			if (result != 0)
			{
				CNotifyError_Simple notify;
				notify(GetLastError());

				return FALSE;
			}				
			return TRUE;
		}
		else
		{
			while (!fFinished) 
			{
				szSourceFileName.Format("%s%s",szNewSourceFolder,FileData.cFileName);
				szDestFileName.Format("%s%s",szNewDestFolder,FileData.cFileName);


#if(0)
				// Aanpassing: Sedert V16.28.
				// Moeten de 'bFailIfExists' param op TRUE zetten.
				// Controle op dwLastError indien deze een 'ERROR_FILE_EXISTS' is dan de fout negeren.
				static const BOOL bFailIfExists = TRUE;
				if (!CopyFile(szSourceFileName,szDestFileName,bFailIfExists)) {			
					const DWORD dwLastError = GetLastError();
					if (ERROR_FILE_EXISTS != dwLastError) {				
						CNotifyError_Simple notify;
						notify(GetLastError());
						FindClose(hSearch);
						return FALSE;
					}
				}
#endif 

				static const BOOL bFailIfExists = FALSE;
				if (!CopyFile(szSourceFileName,szDestFileName,bFailIfExists)) {											
					CNotifyError_Simple notify;
					notify(GetLastError());
					FindClose(hSearch);
					return FALSE;					
				}			
				if (!FindNextFile(hSearch, &FileData)) fFinished = TRUE;			
			}
		}
	}
	else 
	{
		CNotifyError_Simple notify;
		notify(GetLastError());

		FindClose(hSearch);
		return FALSE;		
	}

	FindClose(hSearch);
	return TRUE;
}

// Changes V15.15: Bij een "save-as" de Json App config subdirectory copieren.
BOOL CTimerDoc::CopyDirectories(CString szDestination,CString szSource)
{
	CString szSubDir;
	CString szExtension;
	CString szTemp;

	if ((szSource.IsEmpty() == FALSE) && (szDestination.IsEmpty() == FALSE)) 
	{
		if (szTemp.LoadString(IDS_FILENAME_SUBDIRBACKUP)) 
		{
			// BUG-0109 : Solved
			// szExtension.Format(_T("*.bin"));		

			szExtension.Format(_T("*.*"));		
			szSubDir.Format(_T("%s\\"),szTemp);
		
			if (CopyDirectory(szDestination,szSource,szSubDir,szExtension)) 
			{					
				szTemp.Format(_T("Copying %s-files to subdirectory : %s%s"),szExtension,szDestination,szSubDir);
				CDisplayDevice::DisplayInfoMsg(szTemp);
			}
		}
		else 
		{
			szTemp.Format(_T("Error loading string : IDS_FILENAME_SUBDIRBACKUP"));
			AfxMessageBox(szTemp);
		}
	
		if (szTemp.LoadString(IDS_FILENAME_SUBDIRSCREENMAP)) 
		{		
			// BUG-0109 : Solved
			//szExtension.Format(_T("*.bin"));		

			szExtension.Format(_T("*.*"));		
			szSubDir.Format(_T("%s\\"),szTemp);

			if (CopyDirectory(szDestination,szSource,szSubDir,szExtension)) 
			{		
				szTemp.Format(_T("Copying %s-files to subdirectory : %s%s"),szExtension,szDestination,szSubDir);
				CDisplayDevice::DisplayInfoMsg(szTemp);
			}
		}
		else
		{
			szTemp.Format(_T("Error loading string : IDS_FILENAME_SUBDIRSCREENMAP"));
			AfxMessageBox(szTemp);
		}

		if (szTemp.LoadString(IDS_FILENAME_SUBDIR_AUDIOCONFIG)) 
		{
			// BUG-0109 : Solved
			//szExtension.Format(_T("*.txt"));

			szExtension.Format(_T("*.*"));		
			szSubDir.Format(_T("%s\\"),szTemp);
		
			if (CopyDirectory(szDestination,szSource,szSubDir,szExtension)) 
			{		
				szTemp.Format(_T("Copying %s-files to subdirectory : %s%s"),szExtension,szDestination,szSubDir);
				CDisplayDevice::DisplayInfoMsg(szTemp);
			}
		}
		else
		{
			szTemp.Format(_T("Error loading string : IDS_FILENAME_SUBDIR_AUDIOCONFIG"));
			AfxMessageBox(szTemp);
		}
	
		if (szTemp.LoadString(IDS_FILENAME_SUBDIR_DATA)) 
		{
			szExtension.Format(_T("*.*"));
			szSubDir.Format(_T("%s\\"),szTemp);
		
			if (CopyDirectory(szDestination,szSource,szSubDir,szExtension)) 
			{		
				szTemp.Format(_T("Copying %s-files to subdirectory : %s%s"),szExtension,szDestination,szSubDir);
				CDisplayDevice::DisplayInfoMsg(szTemp);
			}
		}
		else
		{
			szTemp.Format(_T("Error loading string : IDS_FILENAME_SUBDIR_DATA"));
			AfxMessageBox(szTemp);
		}

		if (szTemp.LoadString(IDS_FILENAME_SUBDIRCONFIG)) 
		{
			szExtension.Format(_T("*.*"));
			szSubDir.Format(_T("%s\\"),szTemp);
		
			if (CopyDirectory(szDestination,szSource,szSubDir,szExtension)) 
			{		
				szTemp.Format(_T("Copying %s-files to subdirectory : %s%s"),szExtension,szDestination,szSubDir);
				CDisplayDevice::DisplayInfoMsg(szTemp);
			}
		}
		else
		{
			szTemp.Format(_T("Error loading string : IDS_FILENAME_SUBDIRCONFIG"));
			AfxMessageBox(szTemp);
		}

		if (szTemp.LoadString(IDS_FILENAME_SUBDIRHTMLFILES)) 
		{
			szExtension.Format(_T("*.*"));
			szSubDir.Format(_T("%s\\"),szTemp);
		
			if (CopyDirectory(szDestination,szSource,szSubDir,szExtension)) 
			{		
				szTemp.Format(_T("Copying %s-files to subdirectory : %s%s"),szExtension,szDestination,szSubDir);
				CDisplayDevice::DisplayInfoMsg(szTemp);
			}
		}
		else
		{
			szTemp.Format(_T("Error loading string : IDS_FILENAME_SUBDIRHTMLFILES"));
			AfxMessageBox(szTemp);
		}


		if (szTemp.LoadString(IDS_FILENAME_APPJSON)){
			szExtension.Format(_T("*.*"));
			szSubDir.Format(_T("%s\\"),szTemp);		
			if (CopyDirectory(szDestination,szSource,szSubDir,szExtension)) {		
				szTemp.Format(_T("Copying %s-files to subdirectory : %s%s"),szExtension,szDestination,szSubDir);
				CDisplayDevice::DisplayInfoMsg(szTemp);
			}
		}
		else {
			szTemp.Format(_T("Error loading string : IDS_FILENAME_APPJSON"));
			AfxMessageBox(szTemp);
		}

		return TRUE;
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////
// Public Method : Invoked by CMainFrame::LoadDocumentFile()
////////////////////////////////////////////////////////////////////////
BOOL CTimerDoc::OpenApplicationFile(CStringArray* pStrArrayError)
{		
	CFileDialog CFileOpen(TRUE,
						  _T("duo"),
						  _T("*.duo"),
						  OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
						  _T("Duotecno Files (*.duo)|*.duo||"),
						  NULL); 
	

	if (TMGetFileSettings() != NULL)
	{
		CString szWorkingDir(TMGetFileSettings()->GetDefaultActiveProjectDirectory());
		if (szWorkingDir.IsEmpty() == FALSE) {
			CFileOpen.m_ofn.lpstrInitialDir = szWorkingDir;
		}
	}	

	if (CFileOpen.DoModal() == IDOK) 
	{		
		const CString PathName(CFileOpen.GetPathName());			
	
		/**
		 * TM 20060509: Cannot use CWinApp::OpenDocumentFile(CString)
		 * Function returns the same document if filename is the same !!
		 * Therefore doesn't invoke Serialize(..)
		 */
				
		if ((this->GetDocTemplate()->OpenDocumentFile((LPCTSTR) PathName)) != NULL) 		
		{	
			TRACE1("\nCTimerDoc::OpenApplicationFile: SetPathName %s",PathName);

			if(TRUE==this->m_fDataLoaded)
			{			
				TMLoadDatabases(pStrArrayError);
				TMCreateUnitListDelegation( );
				TMCreateUnitListRequestMeasuredValues();	// sedert versie 0x0C03


				// Sedert V16.00: USE_SENSOR_PIDCONTROL	
				// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
				// Controleren of alle units die gekoppeld werden als PID control unit nog aanwezig zijn.
				// Indien er units gekoppeld werden die niet meer beschikbaar zijn de P-Binding verwijderen.
				TMCreateUnitDimListPIDControl();

				// Sedert V16.49: Inladen van de DALI bridge configuratie.
				TMLoadDaliBridgeConfig( );

				// Sedert V16.49: Hier ook de binding units upgraden. Nodig voor de DALI bindings.
				TMUpgradeBindingUnitTypes();
			}

			SetPathName((LPCTSTR) PathName,TRUE);		
			SetActiveProjectDirectory(PathName);

			// CHANGES_INCOMPATIBLE_VERSION
			return this->m_fDataLoaded;
		}
	}	
	return FALSE;
}

BOOL CTimerDoc::SaveModified() {
	if (IsModified() == TRUE) {
		CString szMessage("Save changes to document ?");
		if (AfxMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO) == IDYES) {		
			OnFileSave();
		}	
	}	
	return TRUE;
}

/*////////////////////////////////////////////////////////////////////////

  BOOL CreateFolder(CString szDestDir,CString szFileTitle)

  Arguments : szDestDir : Name that is retrieved from CFileOpenDialog without
						  extension "*.duo" : 
						  "20040630 Test Directory"
			  szFileTitle : Name that is insert in dialog Box CFileOpenDialog
							+ extension "*.duo" : 
							"20040630 Test Directory.duo"

  Tasks:
	1) Checks if dest directory has to be made ...
	   If so, CreateDirectory ...
	2) SetPathName() --> This is used for Application !


  Notes:
	1) szCurrentDirecytory 
	   if RootDirectory : 
			Retrieves : "c:\"  --> this becomes "c:"
	
	2) Must absorbe creation of Directory in Destination Directory ...
		c:\Program files\...\Installations\20040630\20040630\20040630.duo
		becomes :
			c:\Program files\...\Installations\20040630\20040630.duo
	   --> This happens when you use Save as and select the same file name.

	3) Searchpath checks if destination folder already exists ...
		"c:\Program files\...\Installations\20040630"

    3.1.) If not exists there is a new folder created + fullpathname 
		  is set via function "SetPathName"

	3.2.) Path exists : SetPathName

/////////////////////////////////////////////////////////////////////////*/

BOOL CTimerDoc::CreateFolder(			// BUG-0121
	CString& szCurrentDirecytory,
	CString& szDestDir,
	CString& szFileTitle,
	CString* pszFullName,
	CString* pszPathName)
{
	CString szError;
	CString szTemp,szTempPath;
	char path_buffer[_MAX_PATH];	
	CString szPathName;
	LPTSTR pszPath;
	int nLenPath,nLenDir,k;
	DWORD dwResult = 0;

	TRACE("\nCTimerDoc::CreateFolder( )\n");
	TRACE("szDestDir=");
	TRACE(szDestDir);
	TRACE("\n");
	TRACE("szFileTitle=");
	TRACE(szDestDir);
	TRACE("\n");	
	TRACE("szPathName=");
	TRACE(szPathName);
	TRACE("\n");

	CString path_Current(szCurrentDirecytory);
	path_Current.MakeLower( );
	path_Current.TrimRight(".duo");
	
	CString szTempString(szDestDir);
	szTempString.MakeLower( );				/* TM 20110614 : BUG-0122*/
	szTempString.TrimRight(".duo");
	path_Current.TrimRight(szTempString);
	path_Current.TrimRight('\\');

	// see note (1)	
	if (path_Current.GetLength( ) == 2)
	{
		szPathName.Format("%s%s",path_Current,szDestDir);
	}
	else
	{
		szPathName.Format("%s\\%s",path_Current,szDestDir);	
	}

	szPathName.MakeLower( );				/* TM 20110614 : BUG-0122*/

	// See note (2)
    szTempPath.Format("%s",path_Current);
	szTempPath.MakeLower( );				/* TM 20110614 : BUG-0122*/

	nLenPath = szTempPath.GetLength();
	nLenDir = szDestDir.GetLength();	
	if (nLenPath > nLenDir) 
	{
		k = szTempPath.ReverseFind('\\');	
		szTemp = szTempPath.Right(nLenPath-k-1);

		if (szTemp == szDestDir) 
		{
			szPathName.Format("%s",path_Current);		
		}
	}

	szPathName.MakeLower( );			/* TM 20110614 : BUG-0122*/

	TRACE("szPathName=");
	TRACE(szPathName);
	TRACE("\n");

	*pszPathName = szPathName;

	// TM_CHANGES_BUG_0144: lengte is MAX_PATH-1

	// dwResult returns nr of characters found as PathName ...
	dwResult = SearchPath(NULL,
						  szPathName,
						  NULL,
						  _MAX_PATH-1,
						  &path_buffer[0],
						  &pszPath);	
	if (dwResult == 0) 
	{
		if (CreateDirectory(szPathName,NULL) == FALSE) 
		{	 
			DWORD dwError = GetLastError();
			szError.Format("CreateDirectory returned error code : %u",dwError);
			AfxMessageBox(szError);
		}

		szTempPath.Format("\\%s",szFileTitle);
		szPathName+=szTempPath;		
		szPathName.MakeLower( );				/* TM 20110614 : BUG-0122*/
		*pszFullName = szPathName;				// sedert V16.16
	}
	else 
	{		
		szTempPath.Format("%s\\%s",szPathName,szFileTitle);
		szTempPath.MakeLower( );				/* TM 20110614 : BUG-0122*/		
		*pszFullName = szTempPath;				// sedert V16.16
	}
	return TRUE;
}


BOOL CTimerDoc::SetActiveProjectDirectory(const CString& strPathName)
{
	int nExtensionLen;

	CString szProjectDir(strPathName);		
	CString szTitle(GetTitle());

	szTitle.MakeLower( );			/* TM 20110614 */
	szProjectDir.MakeLower( );		/* TM 20110614 */

	const int nPathLen = szProjectDir.GetLength();
	const int nTitleLen = szTitle.GetLength();

	// 20050222 !
	// Afhankelijk van mapopties --> als extensies getoond worden is title bar:
	// *.duo anders wordt enkel de naam getoond en niet de extensie !!!!
	// Oplossen door --> Controleren of *.duo in de title aanwezig is ...
	if (szTitle.Find(".duo",0) == -1)
	{
		nExtensionLen = -5;	
	}
	else 
	{
		nExtensionLen = 3;
	}

	if (nPathLen >= (nTitleLen + nTitleLen-4)) 
	{	
		CString szTemp = szProjectDir.Left(nPathLen - nTitleLen - (nTitleLen-nExtensionLen));
		szProjectDir = szTemp;
	}
	else 
	{
		return FALSE;
	}

	if (szProjectDir.IsEmpty()) 
	{
		return FALSE;
	}


	// szProjectDir: "d:\developer\test installations\binding software installations\0x1010\"
	if (TMGetFileSettings() != NULL)
	{		
		TMGetFileSettings()->UpdateData(this,szProjectDir);
	}

	return TRUE;
}


BOOL CTimerDoc::CreateAppSubDirectory(CString szSubDirectory)	// TM 20080421
{
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());

	if (szPathName.IsEmpty()) 
	{
		CString szInfo(_T("First Save installation, so a current folder name can be retrieved ..."));
		AfxMessageBox(szInfo);
		return FALSE;
	}

	CString szNewPathName;
	szNewPathName.Format(_T("%s%s"),szPathName,szSubDirectory);

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

BOOL CTimerDoc::ExportHtmlFiles(void)
{
	CExportHtmlFilesApp ExportFiles;
	if (CreateAppSubDirectory(ExportFiles.GetDestDirectory()))
	{
		ExportFiles.StartExport();
		return TRUE;	
	}
	return FALSE;
}

void CTimerDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetFileAccess()->Is_SaveAs_Enabled());	
}

void CTimerDoc::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);	
}

void CTimerDoc::OnUpdateFilePrintSetup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}

// CHANGES_INCOMPATIBLE_VERSION
// Implementatie virtuele functie: var m_fDataLoaded 
void CTimerDoc::DeleteContents()
{
	m_fDataLoaded = FALSE;				
	m_fDataSaved = FALSE;
	CDocument::DeleteContents();
}


