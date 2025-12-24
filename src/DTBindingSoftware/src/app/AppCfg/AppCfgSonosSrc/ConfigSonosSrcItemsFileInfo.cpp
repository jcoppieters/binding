#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigSonosSrcItemsFileInfo.h"
#include <stdio.h>

#include "utils/win32/win32filefind.h"

#include "TMGlobals.h"
using namespace TMGlobalSettings;

////////////////////////////////////////////////
CConfigSonosSrcItemsFileInfo& CConfigSonosSrcItemsFileInfo::getInstance( )
{
	static CConfigSonosSrcItemsFileInfo _instance;
	return _instance;
}
///////////////////////////////////////////////


CConfigSonosSrcItemsFileInfo::CConfigSonosSrcItemsFileInfo( )
{
}

CConfigSonosSrcItemsFileInfo::~CConfigSonosSrcItemsFileInfo( )
{

}
////////////////////////////////////////////////

CString CConfigSonosSrcItemsFileInfo::
GetFullPathName(BYTE bNodeAddress) const
{
	CString sztemp;
	CString szSubDir;
	CString szName;
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	

	szName.Format("srccfg_%02x.dat",bNodeAddress);
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}

BOOL CConfigSonosSrcItemsFileInfo::load( )
{	
	return FALSE;
}

BOOL CConfigSonosSrcItemsFileInfo::isLoaded( ) const
{
	return FALSE;
}

BOOL CConfigSonosSrcItemsFileInfo::Initialise(void)
{
	return TRUE;
}

