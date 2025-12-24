#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigIRTXFileInfo.h"
#include <stdio.h>

#include "utils/win32/win32filefind.h"

#include "TMGlobals.h"
using namespace TMGlobalSettings;

////////////////////////////////////////////////
CConfigIRTXFileInfo& CConfigIRTXFileInfo::getInstance( )
{
	static CConfigIRTXFileInfo _instance;
	return _instance;
}
///////////////////////////////////////////////
CConfigIRTXFileInfo::CConfigIRTXFileInfo( ) :
m_fileLoaded( FALSE ),
m_configFile( )
{
}

CConfigIRTXFileInfo::~CConfigIRTXFileInfo( )
{

}
////////////////////////////////////////////////

CString CConfigIRTXFileInfo::GetFullPathName( ) const
{
	CString sztemp;
	CString szSubDir;
	CString szName("IRTX.dat");

	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}

BOOL CConfigIRTXFileInfo::load( )
{	
	Initialise( );

	if (m_configFile.Load(GetFullPathName( )))		// CHANGES_MVS_2008_CSTRING
	{
		m_fileLoaded = TRUE;
		return TRUE;			
	}	
	return FALSE;
}

BOOL CConfigIRTXFileInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BOOL CConfigIRTXFileInfo::Initialise(void)
{
	m_configFile.removeAll( );
	
	return TRUE;
}

BOOL CConfigIRTXFileInfo::isAvailable( ) const
{
	return CFileFindFunctions::find(GetFullPathName());
}

