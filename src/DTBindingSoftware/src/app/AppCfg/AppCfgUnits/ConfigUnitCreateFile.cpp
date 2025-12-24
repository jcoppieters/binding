#include "stdafx.h"
#include "ConfigUnitCreateFile.h"
#include "ConfigUnitCreateClass.h"
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
using DUOTECNO::MFC_HELPER::CArrayErrors;
//////////////////////////////////////////////////////////////////////

ICConfigUnitCreateDataValidator::~ICConfigUnitCreateDataValidator( ) { }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigUnitCreateFile::CConfigUnitCreateFile() :
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[ConfigUnitCreateClasses::ENUM_CONFIGCLASS_UNIT] = 
		new CConfigUnitCreateClass(&m_configErrors);		
	
	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}

CConfigUnitCreateFile::~CConfigUnitCreateFile()
{
	for (int i=0;i<MAX_CLASSES;i++) 
	{
		if (m_ppConfigBaseClass[i] != 0)
		{
			delete m_ppConfigBaseClass[i];
			m_ppConfigBaseClass[i] = 0;
		}
	}

	if (m_pConfigFile != 0)
		delete m_pConfigFile;

	m_configErrors.removeAll( );

	delete [] m_ppConfigBaseClass;
}

///////////////////////////////////////////////////////////////////////
CConfigExternalClass* 
CConfigUnitCreateFile::GetClass(int type)	
{
	if (type < MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

CString CConfigUnitCreateFile::GetErrors(int i)
{
	const char* szString = 0;
	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}
///////////////////////////////////////////////////////////////////////
BOOL CConfigUnitCreateFile::Initialise(void)
{
	if (m_pConfigFile != 0)
		m_pConfigFile->initialise( );

	m_configErrors.removeAll( );
	return TRUE;
}

// Sedert V16.40:
BOOL CConfigUnitCreateFile::Load(CString szFilename,int nodeAddress, ICConfigUnitCreateDataValidator* pValidator) {
	if (NULL != this->m_pConfigFile) {
		this->Initialise( );
		if (this->m_pConfigFile->load((LPCTSTR) szFilename)) {
			this->NormaliseNames(nodeAddress,pValidator);		// V16.40
			return (m_configErrors.Empty());			
		}
		else {
			m_configErrors.reportError("Could not open file");
		}
	}
	return FALSE;
}

// Sedert V16.40:
// Alle namen van de configfuratie overlopen.
// Indien de naam ook aanwezig is in de node datbase dan deze naam wijzigen.
BOOL CConfigUnitCreateFile::NormaliseNames(int nodeAddress, ICConfigUnitCreateDataValidator* pValidator) {
	if (NULL == pValidator) {
		REQUIRE(NULL != pValidator);
		return FALSE;
	}
	if (nodeAddress<0 || nodeAddress>0xff) {
		REQUIRE(0);
		return FALSE;
	}
	CConfigUnitCreateClass* const pConfigClasses = reinterpret_cast<CConfigUnitCreateClass*>
			(this->GetClass(ConfigUnitCreateClasses::ENUM_CONFIGCLASS_UNIT));
	if (NULL == pConfigClasses) {
		ASSERT(NULL != pConfigClasses);
		return FALSE;
	}

	BOOL result = FALSE;
	const int maxItems = pConfigClasses->GetMax();
	for (int i=0;i<maxItems;++i) {		
		DUOTECNO::DATA::CSerUnit* pSerData = pConfigClasses->GetEntry(i);
		if (NULL != pSerData) {			
			if (pSerData->m_bNodeLogicalAddress == nodeAddress) {
				CString strName;
				if (pValidator->getName(pSerData->m_bNodeLogicalAddress,pSerData->m_bUnitAddress,&strName)) {			
					CString strSerName(pSerData->m_szName);
					if (0 != strName.Compare(strSerName)) {											
						strncpy(pSerData->m_szName,(LPCSTR)strName,sizeof(pSerData->m_szName));	
						pSerData->m_szName[sizeof(pSerData->m_szName)-1] = '\0';
						result = TRUE;
					}
				}				
			}
			else { ASSERT(0); };
		}	
	}
	return result;
}