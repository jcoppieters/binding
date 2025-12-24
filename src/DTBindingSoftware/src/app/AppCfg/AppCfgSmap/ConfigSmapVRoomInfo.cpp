#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigSmapVRoomInfo.h"

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
CConfigSmapAVRoomInfo& CConfigSmapAVRoomInfo::getInstance( )
{
	static CConfigSmapAVRoomInfo _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////////
CConfigSmapAVRoomInfo::CConfigSmapAVRoomInfo() :
m_bNodeAddress( 0xFF ),
m_fileLoaded( FALSE ),
m_configFile( ) 
{ 
}
CConfigSmapAVRoomInfo::~CConfigSmapAVRoomInfo() 
{ 
}
///////////////////////////////////////////////////////////////////////
BOOL CConfigSmapAVRoomInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CConfigSmapAVRoomInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BYTE CConfigSmapAVRoomInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

////////////////////////////////////////////////////////////////////////
CString CConfigSmapAVRoomInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;

	szName.Format("scrmap_%02x_avroom.txt",bNodeAddress);

	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	

	szSubDir.LoadString(IDS_FILENAME_SUBDIRSCREENMAP);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}
////////////////////////////////////////////////////////////////////////
void CConfigSmapAVRoomInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];

	pClass->dump( );
}
/////////////////////////////////////////////