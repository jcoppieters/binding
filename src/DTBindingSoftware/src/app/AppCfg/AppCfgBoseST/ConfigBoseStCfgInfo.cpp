#include "stdafx.h"
#include "BindingSoftware.h"
#include "configbosestcfginfo.h"

#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
/*************************************************************/
#include "TMGlobals.h"
using namespace TMGlobalSettings;
/*************************************************************/

/*************************************************************/
/***************** CConfigBoseSTCfgInfo *********************/
/*************************************************************/
CConfigBoseSTCfgInfo& CConfigBoseSTCfgInfo::getInstance( )
{
	static CConfigBoseSTCfgInfo _instance;
	return _instance;
}
/*************************************************************/

CConfigBoseSTCfgInfo::CConfigBoseSTCfgInfo(void) :
m_bNodeAddress( 0xFF ), 
m_fileLoaded( FALSE ), 
m_configFile( ) 
{
}

CConfigBoseSTCfgInfo::~CConfigBoseSTCfgInfo(void)
{
}
/*************************************************************/

BOOL CConfigBoseSTCfgInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));

	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CConfigBoseSTCfgInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BYTE CConfigBoseSTCfgInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

CString CConfigBoseSTCfgInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;

	szName.Format("bosestcfg_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}

void CConfigBoseSTCfgInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];
	pClass->dump( );
}
/*************************************************************/

/*********************** END OF FILE *************************/