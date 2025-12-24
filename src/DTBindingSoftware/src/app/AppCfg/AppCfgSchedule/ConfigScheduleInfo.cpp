#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigScheduleInfo.h"
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
CConfigScheduleInfo& CConfigScheduleInfo::getInstance( )
{
	static CConfigScheduleInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigScheduleInfo::CConfigScheduleInfo() :
m_bNodeAddress( 0xFF ),
m_fileLoaded( FALSE ),
m_configFile( ),
m_fReady(TRUE) { }
CConfigScheduleInfo::~CConfigScheduleInfo() { }
///////////////////////////////////////////////////////////////////////
BOOL CConfigScheduleInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;
	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded) {
		/* ?? */
	}
	return m_fileLoaded;
}
BOOL CConfigScheduleInfo::isLoaded( ) const
{
	return m_fileLoaded;
}
BYTE CConfigScheduleInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

CString CConfigScheduleInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir; 
	CString szName;
	szName.Format("scheduleConfig_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}

void CConfigScheduleInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];
	pClass->dump( );
}

BOOL CConfigScheduleInfo::isReady() const
{
	return m_fReady;
}

void CConfigScheduleInfo::setReady(BOOL flag)
{
	m_fReady = flag;
}