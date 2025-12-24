// ConfigSmapPageInfoInfo.cpp: implementation of the CConfigSmapPageInfoInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigSmapPageInfoInfo.h"

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
CConfigSmapPageInfoInfo& CConfigSmapPageInfoInfo::getInstance( )
{
	static CConfigSmapPageInfoInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigSmapPageInfoInfo::CConfigSmapPageInfoInfo() :
m_bNodeAddress( 0xFF ),
m_fileLoaded( FALSE ),
m_configFile( ),
m_fReady(TRUE)
{ 
}
CConfigSmapPageInfoInfo::~CConfigSmapPageInfoInfo() 
{ 
}
///////////////////////////////////////////////////////////////////////
BOOL CConfigSmapPageInfoInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}
BOOL CConfigSmapPageInfoInfo::isLoaded( ) const
{
	return m_fileLoaded;
}
BYTE CConfigSmapPageInfoInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

CString CConfigSmapPageInfoInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;
	szName.Format("scrmap_%02x_names.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRSCREENMAP);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}

void CConfigSmapPageInfoInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];
	pClass->dump( );
}

BOOL CConfigSmapPageInfoInfo::isReady() const
{
	return m_fReady;
}

void CConfigSmapPageInfoInfo::setReady(BOOL flag)
{
	m_fReady = flag;
}

