#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigIRRXInfo.h"
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
CConfigIRRXInfo& CConfigIRRXInfo::getInstance( )
{
	static CConfigIRRXInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigIRRXInfo::CConfigIRRXInfo() :
m_bNodeAddress( 0xFF ), m_fileLoaded( FALSE ), m_configFile( ) { }

CConfigIRRXInfo::~CConfigIRRXInfo() { }
///////////////////////////////////////////////////////////////////////
BOOL CConfigIRRXInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}
BOOL CConfigIRRXInfo::isLoaded( ) const
{
	return m_fileLoaded;
}
BYTE CConfigIRRXInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}
////////////////////////////////////////////////////////////////////////
CString CConfigIRRXInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;

	szName.Format("irrxconfig_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}
////////////////////////////////////////////////////////////////////////
void CConfigIRRXInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];
	pClass->dump( );
}