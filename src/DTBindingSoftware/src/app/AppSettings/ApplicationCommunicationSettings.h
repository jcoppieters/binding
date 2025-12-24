// ApplicationCommunicationSettings.h: interface for the CApplicationCommunicationSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONCOMMUNICATIONSETTINGS_H__D6029690_19CC_4FA2_A1B6_47244C81D8C8__INCLUDED_)
#define AFX_APPLICATIONCOMMUNICATIONSETTINGS_H__D6029690_19CC_4FA2_A1B6_47244C81D8C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CApplicationCommunicationSettings  
{
	#define MAX_VALUE_NAME	255
	#define MAX_PORT_NAME	255

	enum { MAX_PORTS = 255 };

protected:
	BYTE m_bAvailablePorts[MAX_PORTS];
	BYTE m_bDefaultComPort;

protected:
	LONG EnumerateSerialPorts(char* deviceName,DWORD maxLen,DWORD dwIndex);
	void FillSerialPortsInfo();
	BOOL GetRegistryDefaultComPort();
	BOOL SetRegistryDefaultComPort();

public:
	CApplicationCommunicationSettings();
	virtual ~CApplicationCommunicationSettings();

	BOOL GetAvailableComPorts(BYTE* pbData,
							  int* Count,
							  const BYTE bMaxCount = MAX_PORTS);
	int GetDefaultComPort() {
		return m_bDefaultComPort;
	}

	void SetDefaultComPort(int port) {
		if (port>=0 && port<=255) {
			if (port != m_bDefaultComPort) {
				m_bDefaultComPort = static_cast<BYTE>(port);		
				this->Save();
			}
		}
		else { 
			ASSERT(port >= 0 && port <= 255); 
		}
	}	

	BOOL Load();	
	BOOL Save();
};

#endif // !defined(AFX_APPLICATIONCOMMUNICATIONSETTINGS_H__D6029690_19CC_4FA2_A1B6_47244C81D8C8__INCLUDED_)
