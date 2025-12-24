/**********************************************************//**
 * @file	ConfigAppUiCtrlInfo
 * @brief	Implementatie bestand voor de CConfigAppUiCtrlInfo klasse.
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *************************************************************/

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigAppUiCtrlInfo.h"

#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
/*************************************************************/
#include "TMGlobals.h"
using namespace TMGlobalSettings;
/*************************************************************/

CConfigAppUiCtrlInfo&
CConfigAppUiCtrlInfo::getInstance( )
{
	static CConfigAppUiCtrlInfo _instance;
	return _instance;
}

/************************************************************/

CConfigAppUiCtrlInfo::CConfigAppUiCtrlInfo(void) :
m_bNodeAddress( 0xFF ), 
m_fileLoaded( FALSE ), 
m_file( ) 
{
}

CConfigAppUiCtrlInfo::~CConfigAppUiCtrlInfo(void)
{
}

/************************************************************/

CString CConfigAppUiCtrlInfo::GetFullPathName(
	BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir;
	CString szName;

	szName.Format("ledscfg_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}

BOOL CConfigAppUiCtrlInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;
	m_fileLoaded = m_file.Load(GetFullPathName(bNodeAddress));

	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CConfigAppUiCtrlInfo::isLoaded( ) const
{
	return m_fileLoaded;
}

BYTE CConfigAppUiCtrlInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

void CConfigAppUiCtrlInfo::dump( )
{
	CConfigExternalClass** ppClasses = m_file.GetClasses( );
	CConfigExternalClass* pClass = ppClasses[0];

	pClass->dump( );
}

/*************************************************************/