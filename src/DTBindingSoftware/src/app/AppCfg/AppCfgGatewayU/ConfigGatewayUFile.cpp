#include "stdafx.h"
#include "ConfigGatewayUFile.h"
//////////////////////////////////////////////////////////////////////
#include "ConfigGatewayUclass.h"
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
using DUOTECNO::MFC_HELPER::CArrayErrors;
//////////////////////////////////////////////////////////////////////

CConfigGatewayUFile::CConfigGatewayUFile(void) :
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[ConfigGatewayClasses::ENUM_CONFIGCLASS_GATEWAY] = 
		new CConfigGatewayUClass(&m_configErrors);	

	// Speciale klasse die ervoor zorgt dat bij het parsen van het configuratie bestand
	// "GATEWAYSYSTEM" header mag ingevoegd worden.
	// Deze kan nodig zijn voor de configurator tool.
	m_ppConfigBaseClass[ConfigGatewayClasses::ENUM_CONFIGCLASS_SYSTEM] = 
		new CConfigGatewayUSystemClass(&m_configErrors);
	
	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}

CConfigGatewayUFile::~CConfigGatewayUFile(void)
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
CConfigGatewayUFile::GetClass(int type)	
{
	if (type < MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

CString CConfigGatewayUFile::GetErrors(int i)
{
	const char* szString = 0;
	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}

///////////////////////////////////////////////////////////////////////

BOOL CConfigGatewayUFile::Initialise(void)
{
	if (m_pConfigFile != 0)
		m_pConfigFile->initialise( );

	m_configErrors.removeAll( );
	return TRUE;
}

BOOL CConfigGatewayUFile::Load(CString szFilename)
{
	if (m_pConfigFile != 0)
	{
		Initialise( );

		if (m_pConfigFile->load((LPCTSTR) szFilename))		
		{
			return (m_configErrors.Empty());			
		}	
	}
	return FALSE;
}