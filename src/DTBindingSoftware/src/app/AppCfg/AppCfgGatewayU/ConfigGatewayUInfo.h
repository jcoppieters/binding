#pragma once

#include "configgatewayufile.h"

class CConfigGatewayUInfo
{
public:
	static CConfigGatewayUInfo& getInstance( );
	
	~CConfigGatewayUInfo(void);
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigGatewayUFile& getConfigFile( ) { return m_configFile; }
	void dump( );

protected:
	CConfigGatewayUInfo(void);

private:
	CConfigGatewayUFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
};
