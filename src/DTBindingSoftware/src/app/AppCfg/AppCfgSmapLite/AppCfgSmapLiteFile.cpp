/**********************************************************//**
 * @file	AppCfgSmapLiteFile.cpp
 * @brief	Immplementatie bestand voor de CAppCfgSmapLite klasse.
 *
 * @author	Mitchell Tom
 * @date	20161123
 *
 * @since	0x0C05
 *************************************************************/

#include "stdafx.h"
#include "AppCfgSmapLiteFile.h"
#include "AppCfgSmapLiteClass.h"

/*************************************************************/
using DUOTECNO::MFC_HELPER::CArrayErrors;
/*************************************************************/

CAppCfgSmapLiteFile::CAppCfgSmapLiteFile() :
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[AppCfgSmapLiteClasses::ENUM_CONFIGCLASS_SMAPLITE] = 
		new CAppCfgSmapLiteClass(&m_configErrors);		
		
	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}

CAppCfgSmapLiteFile::~CAppCfgSmapLiteFile()
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

/*************************************************************/
CConfigExternalClass* CAppCfgSmapLiteFile::GetClass(int type)	
{
	if (type<MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

CString CAppCfgSmapLiteFile::GetErrors(int i)
{
	const char* szString = 0;

	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}

/*************************************************************/

BOOL CAppCfgSmapLiteFile::Initialise(void)
{
	if (m_pConfigFile != 0)
		m_pConfigFile->initialise( );

	m_configErrors.removeAll( );
	return TRUE;
}

BOOL CAppCfgSmapLiteFile::Load(CString szFilename)
{
	if (m_pConfigFile != 0)
	{
		Initialise( );

		if (m_pConfigFile->load((LPCTSTR)szFilename))	// CHANGES_MVS_2008_CSTRING
		{
			return (m_configErrors.Empty());			
		}	
	}
	return FALSE;
}

BOOL CAppCfgSmapLiteFile::Save(CString szFilename)
{
	if (m_pConfigFile != 0)
	{		
		return m_pConfigFile->save((LPCTSTR)szFilename);
	}
	return FALSE;
}
/*************************************************************/

/*********************** END OF FILE *************************/


