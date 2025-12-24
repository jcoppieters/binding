/**********************************************************//**
 * @file	AppCfgSmapLiteInfo.cpp
 * @brief	Immplementatie bestand voor de CAppCfgSmapLiteInfo klasse.
 *
 * @author	Mitchell Tom
 * @date	20161123
 *
 * @since	0x0C05
 *************************************************************/

#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppCfgSmapLiteInfo.h"

#include "screenmaptypes.h"

///////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalSettings;
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////

CAppCfgSmapLiteInfo& CAppCfgSmapLiteInfo::getInstance( )
{
	static CAppCfgSmapLiteInfo instance;
	return instance;
}
///////////////////////////////////////////////////////////////////////

CAppCfgSmapLiteInfo::CAppCfgSmapLiteInfo() :
m_configFile( ),
m_bNodeAddress(0xFF),
m_fileLoaded(FALSE),
m_updateDatabaseNeeded(FALSE)
{
}

CAppCfgSmapLiteInfo::~CAppCfgSmapLiteInfo()
{
}

///////////////////////////////////////////////////////////////////////

CString CAppCfgSmapLiteInfo::GetFullPathName(
	BYTE bNodeAddress)
{
	CString sztemp;
	CString szSubDir; 
	CString szName;

	szName.Format("smaplite_%02x.txt",bNodeAddress);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	

	return sztemp;
}

BOOL CAppCfgSmapLiteInfo::load(BYTE bNodeAddress)
{
	m_bNodeAddress = bNodeAddress;
	m_fileLoaded = m_configFile.Load(GetFullPathName(bNodeAddress));
	if (!m_fileLoaded)
	{
	
	}
	return m_fileLoaded;
}

BOOL CAppCfgSmapLiteInfo::save(BYTE bNodeAddress)
{
	BOOL fResult=FALSE;	

	fResult=m_configFile.Save(GetFullPathName(bNodeAddress));
	//ASSERT(TRUE==fResult);
	return fResult;
}

BOOL CAppCfgSmapLiteInfo::isLoaded( ) const
{
	return m_fileLoaded;
}


BYTE CAppCfgSmapLiteInfo::getNodeAddress( ) const
{
	return m_bNodeAddress;
}

void CAppCfgSmapLiteInfo::dump( )
{
	// TODO - DUMP

/*
	char szBuffer[255];

	sprintf(szBuffer, 
			"CSerIRRXCode:(0x%02x,0x%04x)\n",
			 pSerEntry->m_usIndex,
		 	 pSerEntry->m_usIrCode);

	TRACE( szBuffer );
*/
}

///////////////////////////////////////////////////////////////////////


