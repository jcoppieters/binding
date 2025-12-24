// ConfigSmapSmapInfoFile.cpp: implementation of the CConfigSmapSmapInfoFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigSmapSmapInfoFile.h"
#include "ConfigSmapSmapInfoClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigSmapSmapInfoFile::CConfigSmapSmapInfoFile() :
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[ConfigSmapSmapInfoClasses::ENUM_CONFIGCLASS_SMAP_SMAPINFO] = 
		new CConfigSmapSmapInfoClass(&m_configErrors);		
	
	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}


CConfigSmapSmapInfoFile::~CConfigSmapSmapInfoFile()
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

////////////////////////////////////////////////////////////////////////

CConfigExternalClass* 
CConfigSmapSmapInfoFile::GetClass(int type)	
{
	if (type < MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

CString CConfigSmapSmapInfoFile::GetErrors(int i)
{
	const char* szString = 0;
	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}

///////////////////////////////////////////////////////////////////////

BOOL CConfigSmapSmapInfoFile::Initialise(void)
{
	if (m_pConfigFile != 0)
		m_pConfigFile->initialise( );

	m_configErrors.removeAll( );
	return TRUE;
}

BOOL CConfigSmapSmapInfoFile::Load(CString szFilename)
{
	if (m_pConfigFile != 0)
	{
		Initialise( );

		if (m_pConfigFile->load((LPCTSTR) szFilename))	// CHANGES_MVS_2008_CSTRING
		{
			return (m_configErrors.Empty());			
		}	
	}
	return FALSE;
}

BOOL CConfigSmapSmapInfoFile::Save(CString szFilename)
{	
	BOOL result = FALSE;
	FILE* const fs = fopen(szFilename,"w");

	if (fs != NULL)
	{   
		CConfigSmapSmapInfoClass* pClass = static_cast<CConfigSmapSmapInfoClass*>
			(GetClass(ConfigSmapSmapInfoClasses::ENUM_CONFIGCLASS_SMAP_SMAPINFO));

		if (pClass)
		{
			result = pClass->save(fs);		
		}

		fclose(fs);	
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
