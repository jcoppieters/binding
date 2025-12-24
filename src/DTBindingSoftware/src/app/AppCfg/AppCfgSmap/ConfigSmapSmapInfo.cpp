// ConfigSmapSmapInfo.cpp: implementation of the CConfigSmapSmapInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigSmapSmapInfo.h"

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
///////////////////////////////////////////////////////////////////////
CConfigSmapSmapInfo& CConfigSmapSmapInfo::getInstance( )
{
	static CConfigSmapSmapInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigSmapSmapInfo::CConfigSmapSmapInfo() :
m_bNodeAddress( 0xFF ),
m_fileLoaded( FALSE ),
m_configFile( ) 
{ 
}
CConfigSmapSmapInfo::~CConfigSmapSmapInfo() 
{ 
}
///////////////////////////////////////////////////////////////////////
BOOL CConfigSmapSmapInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CConfigSmapSmapInfo::save(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;
	return m_configFile.Save(GetFullPathName(bNodeAddress));
}

BOOL CConfigSmapSmapInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BYTE CConfigSmapSmapInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

////////////////////////////////////////////////////////////////////////
CString CConfigSmapSmapInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir; 
	CString szName;

	szName.Format("scrmap_%02x_info.txt",bNodeAddress);

	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	

	szSubDir.LoadString(IDS_FILENAME_SUBDIRSCREENMAP);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}
////////////////////////////////////////////////////////////////////////
void CConfigSmapSmapInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];

	pClass->dump( );
}
/////////////////////////////////////////////


