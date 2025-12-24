/**********************************************************//**
 * @file	ConfigAppUiCtrlFile
 * @brief	Implementatie bestand voor de File klasse
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *************************************************************/

#include "stdafx.h"
#include "ConfigAppUiCtrlFile.h"
#include "ConfigAppUiCtrlClasses.h"
/*************************************************************/

/*************************************************************/
using DUOTECNO::MFC_HELPER::CArrayErrors;
/*************************************************************/


/*************************************************************/
/***************** CConfigAppUiCtrlFile **********************/
/*************************************************************/

CConfigAppUiCtrlFile::CConfigAppUiCtrlFile() :
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[ConfigAppUiCtrlClasses::ENUM_CONFIGCLASS_LEDCFG_GENERAL] = 
		new CConfigLedCfgGeneralClass(&m_configErrors);	
	m_ppConfigBaseClass[ConfigAppUiCtrlClasses::ENUM_CONFIGCLASS_LEDCFG_LEDDATA] = 
		new CConfigLedCfgLedClass(&m_configErrors);	

	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}

CConfigAppUiCtrlFile::~CConfigAppUiCtrlFile()
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
	{
		delete m_pConfigFile;
	}

	m_configErrors.removeAll( );

	delete [] m_ppConfigBaseClass;
}

BOOL CConfigAppUiCtrlFile::Load(CString szFilename)
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

BOOL CConfigAppUiCtrlFile::Initialise(void)
{
	if (m_pConfigFile != 0)
	{
		m_pConfigFile->initialise( );
	}
	m_configErrors.removeAll( );

	return TRUE;
}

CString CConfigAppUiCtrlFile::GetErrors(int i)
{
	const char* szString = 0;

	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}

CConfigExternalClass* CConfigAppUiCtrlFile::GetClass(int type)
{
	if (type<MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

/*************************************************************/

