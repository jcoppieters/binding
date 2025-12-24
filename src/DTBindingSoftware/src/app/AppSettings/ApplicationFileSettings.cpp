// ApplicationFileSettings.cpp: implementation of the CApplicationFileSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationFileSettings.h"
#include "BindingSoftwareDoc.h"

#include "utils/win32/win32filefind.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplicationFileSettings::CApplicationFileSettings() :
dtConfigurator( ),
m_bindingExamples(CBindingManuals::ID_MANUAL_BINDING_EXAMPLES), 
m_SensModbusFunctions(CBindingManuals::ID_MANUAL_MODBUS_OVERVIEW),
m_manualHardware(CBindingManuals::ID_MANUAL_HARDWARE),
m_manualSoftwarePart1(CBindingManuals::ID_MANUAL_SOFTWARE_PART1),
m_manualSoftwarePart2(CBindingManuals::ID_MANUAL_SOFTWARE_PART2),
m_manualSoftwareExpert(CBindingManuals::ID_MANUAL_SOFTWARE_EXPERT)
{
	// TM_CHANGES_BUG_0144: lengte is MAX_PATH-1
	char str[MAX_PATH];
	const DWORD dwResult = GetCurrentDirectory(MAX_PATH-1,&str[0]);

	TRACE("\nCurrent Directory =");
	TRACE(str);
	TRACE("\n");

	if (dwResult!=0)
	{		
		m_strDirExamples.Format("%s",&str[0]);

		if (m_strDirExamples.Find("Duotecno Installation Software") == -1)
		{
			// TM 20140423: versie 0x0642
			// Opzoeken program files path via SHGetFolderPath( ) functie.
			// Nodig voor ondersteuning van Program files(x86)

			TCHAR szPath[MAX_PATH];

			if(SUCCEEDED(SHGetFolderPath(NULL, 
                         CSIDL_PROGRAM_FILES, 
                         NULL, 
                         0, 
                         szPath))) 
			{
				m_strDirExamples.Format("%s\\Duotecno\\Duotecno Installation Software",szPath);
			}
			else
			{
				m_strDirExamples.Format("c:\\Program files\\Duotecno\\Duotecno Installation Software");
			}
		}
		
		m_strDirHelp = m_strDirExamples + "\\HELP\\";
		m_strDirConfig = m_strDirExamples + "\\CONFIG\\";
		m_strDirExamples += "\\EXAMPLES\\";	
				
		// V16.29 herwerkt.
		m_bindingExamples.init(m_strDirHelp, m_strDirExamples );
		m_SensModbusFunctions.init( m_strDirHelp, m_strDirExamples );

		// Sedert V16.29
		m_manualHardware.init(m_strDirHelp, m_strDirExamples);
		m_manualSoftwarePart1.init(m_strDirHelp, m_strDirExamples);
		m_manualSoftwarePart2.init(m_strDirHelp, m_strDirExamples);
		m_manualSoftwareExpert.init(m_strDirHelp, m_strDirExamples);
	}
}

CApplicationFileSettings::~CApplicationFileSettings()
{

}

BOOL CApplicationFileSettings::LoadDefaultActiveProjectDir()
{
	BOOL fResult = FALSE;

	try
	{
		CString szRegKeySection((LPCSTR) IDS_REGKEY_FILE_SETTINGS);
		CString szRegKeySubSection((LPCSTR) IDS_REGKEY_PROJECT_DIRECTORY);

		m_strDefaultProjectDir = AfxGetApp()->GetProfileString(szRegKeySection,
															   szRegKeySubSection);

		TRACE("\nm_strDefaultProjectDir =");
		TRACE(m_strDefaultProjectDir);
		TRACE("\n");

		fResult = TRUE;
	
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		fResult = FALSE;
	}

	return fResult;
}

BOOL CApplicationFileSettings::SaveDefaultActiveProjectDir()
{
	BOOL fResult = TRUE;

	if (m_strDefaultProjectDir.IsEmpty() == FALSE)
	{
		fResult = FALSE;

		try
		{
			CString szRegKeySection((LPCSTR) IDS_REGKEY_FILE_SETTINGS);
			CString szRegKeySubSection((LPCSTR) IDS_REGKEY_PROJECT_DIRECTORY);

			fResult = AfxGetApp()->WriteProfileString(szRegKeySection,
													  szRegKeySubSection,
													  m_strDefaultProjectDir);

			if (fResult == FALSE)
			{
				CString s("Error: Couldn't save the active project directory to the registry");
				AfxMessageBox(s);
			}

		}
		catch (CException *e)
		{
			e->ReportError();
			e->Delete();
			fResult = FALSE;
		}
	}

	return fResult;
}

BOOL CApplicationFileSettings::Load()
{
	const BOOL fResult = LoadDefaultActiveProjectDir();
	ASSERT(fResult);

	return fResult;
}
BOOL CApplicationFileSettings::Save()
{
	const BOOL fResult = SaveDefaultActiveProjectDir();
	ASSERT(fResult);

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


/* 
    Zie BUG-0168

	m_strPreviousPath=D:\developer\test installations\binding software installations\0x1010\20220207 test\
	m_strDefaultProjectDir=d:\developer\test installations\binding software installations\0x1010\
	m_strTitle=20220214 test.duo
	m_strFullyQualifiedFileName=d:\developer\test installations\binding software installations\0x1010\20220214 test\20220214 test.duo
	m_strFullyQualifiedPathName=d:\developer\test installations\binding software installations\0x1010\20220214 test\
*/

BOOL CApplicationFileSettings::UpdateData(CDocument* pDoc,const CString& strFullyQualifiedFileName, 
										  const CString& pathName,const CString& projectDir,const CString& filetitle) {

	ASSERT(!strFullyQualifiedFileName.IsEmpty());
	ASSERT(!pathName.IsEmpty());
	ASSERT(!projectDir.IsEmpty());
	ASSERT(!filetitle.IsEmpty());


	m_strPreviousPath = this->GetFullyQualifiedPathName();

	if (!projectDir.IsEmpty()) {
		m_strDefaultProjectDir = projectDir;			
	}
	m_strTitle = filetitle;
	this->SetFullyQualifiedFileName(strFullyQualifiedFileName);


	CString szTemp(pathName + "\\");
	setFullyQualifiedPathName(szTemp);					// Oppassen moeten een extra '\\' toevoegen.			


	DBG_DUMP_SEPERATOR("UpdateData()-enter");
	DBG_DUMP("m_strPreviousPath",m_strPreviousPath);
	DBG_DUMP("m_strDefaultProjectDir",m_strDefaultProjectDir);
	DBG_DUMP("m_strTitle",m_strTitle);
	DBG_DUMP("m_strFullyQualifiedFileName",this->GetFullyQualifiedFileName());
	DBG_DUMP("m_strFullyQualifiedPathName",this->GetFullyQualifiedPathName());
	DBG_DUMP_SEPERATOR("UpdateData()-exit");

	return TRUE;
}


/****************************************************************************************************************************************
 * Zie BUG-0168
 *
 * CString m_strFullyQualifiedPathName;		--> d:\developer\test installations\binding software installations\0x1010\20220214 test\	
 *
 * Oppassen: Moet altijd afgesloten zijn met een '\'
 ***************************************************************************************************************************************/

void CApplicationFileSettings::setFullyQualifiedPathName(const CString& s) {
	ASSERT((!s.IsEmpty()) && (s.GetAt(s.GetLength()-1) == '\\'));
	m_strFullyQualifiedPathName = s;
}

CString CApplicationFileSettings::GetFullyQualifiedPathName() const {
	if (!m_strFullyQualifiedPathName.IsEmpty()) {
		ASSERT(m_strFullyQualifiedPathName.GetAt(m_strFullyQualifiedPathName.GetLength()-1) == '\\');
	}	
	return m_strFullyQualifiedPathName;
}


/****************************************************************************************************************************************
 * Zie BUG-0168
 *
 * CString m_strFullyQualifiedFileName;		--> // d:\developer\test installations\binding software installations\0x1010\20220214 test\20220214 test.duo	
 *
 * Oppassen: Moet altijd eindigen met "*.duo"
 ***************************************************************************************************************************************/
CString CApplicationFileSettings::GetFullyQualifiedFileName() const {
	return m_strFullyQualifiedFileName;
}
void CApplicationFileSettings::SetFullyQualifiedFileName(const CString& s) {
	
#if (_DEBUG==1)
	CString szTemp(s);
	if (!szTemp.IsEmpty()) {
		szTemp.MakeLower( );
		if (szTemp.Find(".duo",szTemp.GetLength()-4) == -1) {
			ASSERT(0);		
		}			
	}
#endif

	m_strFullyQualifiedFileName = s;
}



// szActiveProjectDir = "d:\developer\test installations\binding software installations\0x1010\"
BOOL CApplicationFileSettings::UpdateData(CDocument* pDoc,CString szActiveProjectDir)
{
	if (pDoc != 0)
	{
		m_strPreviousPath = this->GetFullyQualifiedPathName();

		TRACE("\nCApplicationFileSettings::UpdateData( ) - m_strPreviousPath =");
		TRACE(m_strPreviousPath);
		TRACE("\n");

		if (szActiveProjectDir.IsEmpty() == FALSE) 
		{
			m_strDefaultProjectDir = szActiveProjectDir;			// szActiveProjectDir => "d:\developer\test installations\binding software installations\0x1010\"
		}

		m_strTitle = pDoc->GetTitle();								// m_strTitle = "20220207 test.duo"

// since v0x0565 : TM20100623
// als bij de map opties "Extensies voor bekende bestandstypes verbergen" aangevinkt staat
// is het mogelijk dat de title zonder "*.duo" weergegeven wordt, daarom wordt deze er
// altijd achter gezet.

		CString szTemp = pDoc->GetTitle();

		if (!szTemp.IsEmpty())
		{
			szTemp.MakeLower( );

			if (szTemp.Find(".duo",0) == -1)
			{
				szTemp += ".duo";
			}			
		}


		// m_strTitle = "20220207 test.duo"
		// m_strFullyQualifiedFileName = "D:\developer\test installations\binding software installations\0x1010\20220207 test\20220207 test.duo"

		m_strTitle = szTemp;										
		this->SetFullyQualifiedFileName(pDoc->GetPathName());			
	
		int nStart = 0;

		while(1) 
		{
			const int nResult = this->GetFullyQualifiedFileName().Find('\\',nStart+1);
			if (nResult == -1) 
			{
				break;		
			}
			nStart = nResult;		
		}

		// m_strFullyQualifiedPathName = "D:\developer\test installations\binding software installations\0x1010\20220207 test\"
		this->setFullyQualifiedPathName(this->GetFullyQualifiedFileName().Left(nStart+1));	

		TRACE("\nCApplicationFileSettings::UpdateData( ) - m_strFullyQualifiedPathName =");
		TRACE(this->GetFullyQualifiedPathName());
		TRACE("\n");

		return TRUE;
	}
	return FALSE;
}

void CApplicationFileSettings::Clear()
{
	m_strPreviousPath.Empty();
	m_strTitle.Empty();
	m_strFullyQualifiedFileName.Empty();
	m_strFullyQualifiedPathName.Empty();
}

///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
CString CApplicationFileSettings::CBindingManuals::m_dirManualsHelp = CString();
CString CApplicationFileSettings::CBindingManuals::m_dirExamples = CString();
///////////////////////////////////////////////////////////////////////
CApplicationFileSettings::CBindingManuals::CBindingManuals(Identifier_t id) : m_id(id), m_found(FALSE) { }
	
void CApplicationFileSettings::CBindingManuals::init(const CString& dirManualsHelp,const CString& dirExamples) {

	m_dirManualsHelp = dirManualsHelp;		
	m_dirExamples = dirExamples;

	m_found = CFileFindFunctions::find(this->getFullPathName());
}

const CString CApplicationFileSettings::CBindingManuals::getFullPathName( ) const {
	switch(m_id) {
		case ID_MANUAL_BINDING_EXAMPLES:
			return m_dirExamples + "BINDING MANUAL EXAMPLES.pdf";		// Oppassen: examples dir!! andere uit de help dir...
		case ID_MANUAL_MODBUS_OVERVIEW:
			return m_dirManualsHelp + "modbus overview sensor functions.pdf";
		case ID_MANUAL_HARDWARE:
			return m_dirManualsHelp + "manual hardware.pdf";
		case ID_MANUAL_SOFTWARE_PART1:
			return m_dirManualsHelp + "manual software part1.pdf";			
		case ID_MANUAL_SOFTWARE_PART2:
			return m_dirManualsHelp + "manual software part2.pdf";	
		case ID_MANUAL_SOFTWARE_EXPERT:
			return m_dirManualsHelp + "manual software expert.pdf";	
	}
	return "unknown";
}


//////////////////////////////////////////////////////////////////////
// Nested class CApplicationFileSettings::CDTConfigurator.
//////////////////////////////////////////////////////////////////////

CApplicationFileSettings::CDTConfigurator::
CDTConfigurator( ) :
m_szWindowName("Duotecno Configurator - "),
m_szFullPathName( )
{
	// %ProgramFiles(x86)% : enkel in XP,2003 en vista
	// %ProgramW6432% : enkel in XP,2003 en vista
	// %ProgramFiles% : indien opgevraagd door een 32 bit,
	// zal altijd naar de 32 bit directory verwijzen.
			
	CString szTemp("%ProgramFiles%\\Duotecno\\"
				   "Duotecno Configurator\\Duotecno Configurator.exe");

	// TM_CHANGES_BUG_0144: lengte van de string is _MAX_PATH-1
	char szProgramFiles[_MAX_PATH];

	if (0 < ExpandEnvironmentStrings(szTemp,szProgramFiles,_MAX_PATH-1))
	{
		m_szFullPathName = szProgramFiles;
	}
	else
	{
		m_szFullPathName = "c:\\ProgramFiles\\Duotecno\\"
			   "Duotecno Configurator\\Duotecno Configurator.exe";
	}		
}

CApplicationFileSettings::CDTConfigurator::
~CDTConfigurator( )
{

}
///////////////////////////////////////////////////////////////////////



