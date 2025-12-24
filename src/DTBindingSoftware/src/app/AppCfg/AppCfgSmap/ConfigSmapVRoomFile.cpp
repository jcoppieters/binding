#include "stdafx.h"
#include "ConfigSmapVRoomFile.h"
#include "configsmapvroomclass.h"

CConfigSmapAVRoomFile::CConfigSmapAVRoomFile(void):
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = 0;
	}

	m_ppConfigBaseClass[ConfigSmapAVRoomClasses::ENUM_CONFIGCLASS_SMAP_AVROOM] = 
		new CConfigSmapVRoomClass(&m_configErrors);	
	
	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}

CConfigSmapAVRoomFile::~CConfigSmapAVRoomFile(void)
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

CConfigExternalClass* 
CConfigSmapAVRoomFile::GetClass(int type)	
{
	if (type < MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

CString CConfigSmapAVRoomFile::GetErrors(int i)
{
	const char* szString = 0;
	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}

BOOL CConfigSmapAVRoomFile::Initialise(void)
{
	if (m_pConfigFile != 0)
		m_pConfigFile->initialise( );

	m_configErrors.removeAll( );
	return TRUE;
}

BOOL CConfigSmapAVRoomFile::Load(CString szFilename)
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
