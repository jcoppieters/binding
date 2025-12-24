#include "stdafx.h"
#include "configirrxfile.h"
//////////////////////////////////////////////////////////////////////
#include "ConfigirrxClass.h"
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
using DUOTECNO::MFC_HELPER::CArrayErrors;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigIRRXFile::CConfigIRRXFile() :
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[ConfigIRRXClasses::ENUM_CONFIGCLASS_IRRX] = 
		new CConfigIRRXClass(&m_configErrors);		
	
	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}

CConfigIRRXFile::~CConfigIRRXFile()
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
CConfigIRRXFile::GetClass(int type)	
{
	if (type < MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

CString CConfigIRRXFile::GetErrors(int i)
{
	const char* szString = 0;
	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}
///////////////////////////////////////////////////////////////////////
BOOL CConfigIRRXFile::Initialise(void)
{
	if (m_pConfigFile != 0)
		m_pConfigFile->initialise( );

	m_configErrors.removeAll( );
	return TRUE;
}

BOOL CConfigIRRXFile::Load(CString szFilename)
{
	if (m_pConfigFile != 0)
	{
		Initialise( );

		if (m_pConfigFile->load((LPCTSTR) szFilename))		// CHANGES_MVS_2008_CSTRING
		{
			return (m_configErrors.Empty());			
		}	
	}
	return FALSE;
}