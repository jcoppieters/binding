// ApplicationGlobalSettings.h: interface for the CApplicationGlobalSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONGLOBALSETTINGS_H__48D7E646_5CBA_4CD1_B91D_0127C05E73FD__INCLUDED_)
#define AFX_APPLICATIONGLOBALSETTINGS_H__48D7E646_5CBA_4CD1_B91D_0127C05E73FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ApplicationCommunicationSettings.h"
#include "ApplicationFileSettings.h"
#include "ApplicationDisplaySettings.h"

/**
 *
 */
class CApplicationGlobalSettings  
{
public:

	static CApplicationGlobalSettings* GetInstance();

public:

	CApplicationCommunicationSettings* GetCommunicationSettings() {
		return m_pCommunicationSettings;
	}

	CApplicationFileSettings* GetFileSettings() {
		return m_pFileSettings;
	}

	CApplicationDisplaySettings* GetDisplaySettings() {
		return m_pDisplaySettings;
	}

	CApplicationFileAccess* GetFileAccess() {
		return m_pFileAccess;
	}

	BOOL Load();
	BOOL Save();


protected:
	CApplicationGlobalSettings();

public:
	virtual ~CApplicationGlobalSettings();

	CApplicationCommunicationSettings* const m_pCommunicationSettings;
	CApplicationFileSettings* const m_pFileSettings;
	CApplicationDisplaySettings* const m_pDisplaySettings;
	CApplicationFileAccess* const m_pFileAccess;
};

#endif // !defined(AFX_APPLICATIONGLOBALSETTINGS_H__48D7E646_5CBA_4CD1_B91D_0127C05E73FD__INCLUDED_)
