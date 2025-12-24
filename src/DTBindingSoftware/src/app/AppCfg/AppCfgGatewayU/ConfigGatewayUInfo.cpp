#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigGatewayUInfo.h"
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
///////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalSettings;
///////////////////////////////////////////////////////////////////////
CConfigGatewayUInfo& CConfigGatewayUInfo::getInstance( )
{
	static CConfigGatewayUInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigGatewayUInfo::CConfigGatewayUInfo(void) :
m_bNodeAddress( 0xFF ), 
m_fileLoaded( FALSE ), 
m_configFile( ) 
{
}

CConfigGatewayUInfo::~CConfigGatewayUInfo(void)
{
}
////////////////////////////////////////////////////////////////////////
BOOL CConfigGatewayUInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));

	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CConfigGatewayUInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BYTE CConfigGatewayUInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}


CString CConfigGatewayUInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;

	szName.Format("gatewaycfg_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}

void CConfigGatewayUInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];
	pClass->dump( );
}