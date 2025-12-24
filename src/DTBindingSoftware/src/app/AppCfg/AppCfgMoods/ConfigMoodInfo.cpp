#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigMoodInfo.h"
///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
CConfigMoodInfo& CConfigMoodInfo::getInstance( )
{
	static CConfigMoodInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigMoodInfo::CConfigMoodInfo() :
m_bNodeAddress( 0xFF ),
m_fileLoaded( FALSE ),
m_configFile( ),
m_fReady(TRUE) { }
CConfigMoodInfo::~CConfigMoodInfo() { }
///////////////////////////////////////////////////////////////////////
BOOL CConfigMoodInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;
	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded) {
		/* ?? */	
	}
	return m_fileLoaded;
}
BOOL CConfigMoodInfo::isLoaded( ) const
{
	return m_fileLoaded;
}
BYTE CConfigMoodInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

CString CConfigMoodInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir; 
	CString szName;
	szName.Format("MoodConfig_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}

void CConfigMoodInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];

	pClass->dump( );
}

BOOL CConfigMoodInfo::isReady() const
{
	return m_fReady;
}

void CConfigMoodInfo::setReady(BOOL flag)
{
	m_fReady = flag;
}