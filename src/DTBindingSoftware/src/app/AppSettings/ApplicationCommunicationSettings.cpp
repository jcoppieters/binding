// ApplicationCommunicationSettings.cpp: implementation of the CApplicationCommunicationSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationCommunicationSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplicationCommunicationSettings::CApplicationCommunicationSettings() :
m_bDefaultComPort(0)
{
	FillSerialPortsInfo();
}

CApplicationCommunicationSettings::~CApplicationCommunicationSettings()
{

}


LONG CApplicationCommunicationSettings::EnumerateSerialPorts(char* deviceName,
															 DWORD maxLen,
															 DWORD dwIndex)
{
	const char RegPath[MAX_PATH] = "HARDWARE\\DEVICEMAP\\SERIALCOMM";				
	const HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
	HKEY hKey;
	DWORD retCode;	
	
	if ((retCode = RegOpenKeyEx(
						hKeyRoot,
						RegPath,
						0,
						KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE,
						&hKey)) != ERROR_SUCCESS)
	{
		return FALSE;
	}


	// TM_CHANGES_BUG_0144: Is een string, dus lengte is MAX_PATH-1
	char ClassName[MAX_PATH] = "";
	DWORD dwcClassLen = MAX_PATH-1;
	FILETIME ftLastWriteTime;
	DWORD dwcSubKeys,dwcMaxSubKeys,dwcMaxClass,dwcValues;
	DWORD dwcMaxValueName,dwcMaxValueData,dwcSecDesc;
	DWORD dwType;

	// Sedert 0x0642: Controle op de error code.
	if (ERROR_SUCCESS == RegQueryInfoKey(
							hKey,
							ClassName,
							&dwcClassLen,
							NULL,
							&dwcSubKeys,
							&dwcMaxSubKeys,
							&dwcMaxClass,
							&dwcValues,
							&dwcMaxValueName,
							&dwcMaxValueData,
							&dwcSecDesc,
							&ftLastWriteTime))
	{	

		DWORD dwcValueName = MAX_VALUE_NAME;
		char ValueName[MAX_VALUE_NAME];
		DWORD cbData = maxLen;

		// TM_CHANGES_BUG_0144: Is een string, dus -1 voor het 0 karakter
		dwcValueName = MAX_VALUE_NAME-1;
		memset(ValueName,0,sizeof(ValueName));

		retCode = RegEnumValue(hKey,
							   dwIndex,
							   ValueName,
							   &dwcValueName,
							   NULL,
							   &dwType,
							   (BYTE*) &deviceName[0],
							    &cbData);

		RegCloseKey(hKey);

		if (dwType == REG_SZ && retCode == (DWORD) ERROR_SUCCESS)
		{	
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		RegCloseKey(hKey);
	}
	return FALSE;
}

void CApplicationCommunicationSettings::FillSerialPortsInfo()
{
	DWORD count = 0;
	LONG retCode = FALSE;

	for (int i=0;i<MAX_PORTS;i++)
	{
		m_bAvailablePorts[i] = 0xFF;
	}

	while(TRUE)
	{
		char portname[MAX_PORT_NAME];

		// TM_CHANGES_BUG_0144
		// Moeten de lengte van het buffer -1 doen, is een string dus heeft ook een
		// 0 karakter...

		if ((retCode = EnumerateSerialPorts(&portname[0],
											sizeof(portname)-1,		// TM_CHANGES_BUG_0144
											count)) 
											== TRUE)
		{	
			// TM_CHANGES_BUG_0144
			// Controle op geldige string + upper string...

			if (portname[0]!='\0')
			{
				char* const upper = _strupr(&portname[0]);
				if (upper)
				{
					int port=0;

					if (sscanf(upper,"COM%d",&port)==1)			// CHECKED_SSCANF_OK
					{
						port--;

						if ((port>=0) && (port<MAX_PORTS))
						{
							ASSERT(port>=0 && port<=0xFF);		// CHANGES_MVS_2008_WARNINGS

							if (port>=0 && port<=0xFF)
							{
								m_bAvailablePorts[port] = static_cast<BYTE>(port);	// CHANGES_MVS_2008_WARNINGS	
							}
						}
					}
					count++;
				}
				else
				{
					// break while loop, fout gebeurd...
					count=0;
					break;	
				}
			}
			else
			{
				// break while loop, fout gebeurd...
				count=0;
				break;	
			}
		}
		else
		{
			break;
		}	
	}

	if (count==0)
	{
		for (int i=0;i<MAX_PORTS;i++)
		{
			ASSERT(i>=0 && i<=0xFF);						// CHANGES_MVS_2008_WARNINGS
			m_bAvailablePorts[i] = static_cast<BYTE>(i);	// CHANGES_MVS_2008_WARNINGS
		}
	}
}

BOOL CApplicationCommunicationSettings::GetAvailableComPorts(
	BYTE* pbData,
	int* Count,
	const BYTE bMaxCount)
{
	if (pbData != NULL)
	{		
		for (int i=0,cnt=0;i<MAX_PORTS;i++)
		{			
			if (m_bAvailablePorts[i] != 0xFF)
			{
				if (cnt<bMaxCount)
				{
					pbData[cnt++] = m_bAvailablePorts[i];
					*Count = cnt;
				}
			}
		}	
		
		return TRUE;
	}
	return FALSE;
}
/**
 *
 */
BOOL CApplicationCommunicationSettings::GetRegistryDefaultComPort()
{
	try
	{
		CString szRegKeySection((LPCSTR) IDS_REGKEY_COMMUNICATION_SETTINGS);
		CString szRegKeySubSection((LPCSTR) IDS_REGKEY_COMPORT);

		const UINT tempValue = AfxGetApp()->GetProfileInt(szRegKeySection,
				  										  szRegKeySubSection,
														  1);	// CHANGES_MVS_2008_WARNINGS
		ASSERT(tempValue >= 0 && tempValue <= 0xFF);			// CHANGES_MVS_2008_WARNINGS

		m_bDefaultComPort = static_cast<BYTE>(tempValue);		// CHANGES_MVS_2008_WARNINGS

		return TRUE;
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();

		return FALSE;
	}
}

BOOL CApplicationCommunicationSettings::SetRegistryDefaultComPort()
{
	try
	{
		CString szRegKeySection((LPCSTR) IDS_REGKEY_COMMUNICATION_SETTINGS);
		CString szRegKeySubSection((LPCSTR) IDS_REGKEY_COMPORT);

		return (AfxGetApp()->WriteProfileInt(szRegKeySection,
			  						           szRegKeySubSection,
											   m_bDefaultComPort));
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();

		return FALSE;
	}
}

/**
 *
 */
BOOL CApplicationCommunicationSettings::Load()
{
	FillSerialPortsInfo();
	GetRegistryDefaultComPort();

	return TRUE;
}

BOOL CApplicationCommunicationSettings::Save()
{
	SetRegistryDefaultComPort();
	return TRUE;
}