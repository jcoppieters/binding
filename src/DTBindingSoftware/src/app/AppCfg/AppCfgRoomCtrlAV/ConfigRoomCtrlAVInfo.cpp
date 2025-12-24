#include "stdafx.h"
#include "BindingSoftware.h"
#include "configRoomCtrlAVInfo.h"

#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
/*************************************************************/
#include "TMGlobals.h"
using namespace TMGlobalSettings;
/*************************************************************/

/*************************************************************/
/***************** CConfigRoomCtrlAVInfo *********************/
/*************************************************************/
CConfigRoomCtrlAVInfo& CConfigRoomCtrlAVInfo::getInstance( )
{
	static CConfigRoomCtrlAVInfo _instance;
	return _instance;
}
/*************************************************************/

CConfigRoomCtrlAVInfo::CConfigRoomCtrlAVInfo(void) :
m_bNodeAddress( 0xFF ), 
m_fileLoaded( FALSE ), 
m_configFile( ) 
{
}

CConfigRoomCtrlAVInfo::~CConfigRoomCtrlAVInfo(void)
{
}
/*************************************************************/

BOOL CConfigRoomCtrlAVInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;

	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));

	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CConfigRoomCtrlAVInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BYTE CConfigRoomCtrlAVInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

CString CConfigRoomCtrlAVInfo::GetFullPathName(BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;

	szName.Format("roomctrlaudio_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}

void CConfigRoomCtrlAVInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_configFile.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];
	pClass->dump( );
}
/*************************************************************/

/*********************** END OF FILE *************************/