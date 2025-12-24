#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigUnitCreateInfo.h"
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
CConfigUnitCreateInfo& CConfigUnitCreateInfo::getInstance( )
{
	static CConfigUnitCreateInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigUnitCreateInfo::CConfigUnitCreateInfo() :
m_bNodeAddress( 0xFF ), m_fileLoaded( FALSE ), m_configFile( ) { }
CConfigUnitCreateInfo::~CConfigUnitCreateInfo() { }
///////////////////////////////////////////////////////////////////////
BOOL CConfigUnitCreateInfo::load(BYTE bNodeAddress,ICConfigUnitCreateDataValidator* pValidator) {
	this->m_bNodeAddress = bNodeAddress;
	this->m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress), bNodeAddress, pValidator);
	if (!m_fileLoaded) {
	
	}
	return m_fileLoaded;
}
BOOL CConfigUnitCreateInfo::isLoaded( ) const
{
	return this->m_fileLoaded;
}
BYTE CConfigUnitCreateInfo::getNodeAddress( ) const
{
	return this->m_bNodeAddress;
}
////////////////////////////////////////////////////////////////////////
CString CConfigUnitCreateInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;	// ("ExtConfig");

	CString szName;
	szName.Format("UnitConfig_%02x.txt",bNodeAddress);

	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}
////////////////////////////////////////////////////////////////////////
void CConfigUnitCreateInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];
	pClass->dump( );
}