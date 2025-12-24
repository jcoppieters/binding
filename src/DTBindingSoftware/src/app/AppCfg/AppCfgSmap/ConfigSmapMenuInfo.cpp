// ConfigSmapMenuInfo.cpp: implementation of the CConfigSmapMenuInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigSmapMenuInfo.h"

#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
///////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalSettings;
///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigSmapMenuInfo& CConfigSmapMenuInfo::getInstance( )
{
	static CConfigSmapMenuInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigSmapMenuInfo::CConfigSmapMenuInfo() :
m_bNodeAddress( 0xFF ),
m_fileLoaded( FALSE ),
m_configFile( ) 
{ 
}
CConfigSmapMenuInfo::~CConfigSmapMenuInfo() 
{ 
}
///////////////////////////////////////////////////////////////////////
BOOL CConfigSmapMenuInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CConfigSmapMenuInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BYTE CConfigSmapMenuInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

////////////////////////////////////////////////////////////////////////
CString CConfigSmapMenuInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;

	szName.Format("scrmap_%02x_menu.txt",bNodeAddress);

	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	

	szSubDir.LoadString(IDS_FILENAME_SUBDIRSCREENMAP);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}
////////////////////////////////////////////////////////////////////////
void CConfigSmapMenuInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];

	pClass->dump( );
}
/////////////////////////////////////////////

