// ApplicationGlobalSettings.cpp: implementation of the CApplicationGlobalSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationGlobalSettings.h"

#include "ApplicationCommunicationSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CApplicationGlobalSettings* CApplicationGlobalSettings::GetInstance()
{
	static CApplicationGlobalSettings instance;
	return (&instance);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CApplicationGlobalSettings::CApplicationGlobalSettings() :
m_pCommunicationSettings(new CApplicationCommunicationSettings()),
m_pFileSettings(new CApplicationFileSettings()),
m_pDisplaySettings(new CApplicationDisplaySettings()),
m_pFileAccess(new CApplicationFileAccess())
{
	ASSERT(m_pCommunicationSettings);
	ASSERT(m_pFileSettings);
	ASSERT(m_pDisplaySettings);
	ASSERT(m_pFileAccess);
}

CApplicationGlobalSettings::~CApplicationGlobalSettings()
{
	delete m_pCommunicationSettings;
	delete m_pFileSettings;
	delete m_pDisplaySettings;
	delete m_pFileAccess;
}

//////////////////////////////////////////////////////////////////////

BOOL CApplicationGlobalSettings::Load()
{
	BOOL fResult = FALSE;

	if (m_pCommunicationSettings != NULL)
	{
		fResult = m_pCommunicationSettings->Load();
		ASSERT(fResult);
	}

	if (m_pFileSettings != NULL)
	{
		fResult = m_pFileSettings->Load();
		ASSERT(fResult);
	}
	
	return fResult;
}

BOOL CApplicationGlobalSettings::Save()
{
	BOOL fResult = FALSE;

	if (m_pCommunicationSettings != NULL)
	{
		fResult = m_pCommunicationSettings->Save();
		ASSERT(fResult);
	}

	if (m_pFileSettings != NULL)
	{
		fResult = m_pFileSettings->Save();
		ASSERT(fResult);
	}
	
	return fResult;
}