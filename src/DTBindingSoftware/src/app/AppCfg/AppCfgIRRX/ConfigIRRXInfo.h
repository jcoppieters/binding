#ifndef DUOTECNO_CONFIG_IRRXINFO_H
#define DUOTECNO_CONFIG_IRRXINFO_H
///////////////////////////////////////////////////////////////////////
#include "ConfigirrxFile.h"
///////////////////////////////////////////////////////////////////////
class CConfigIRRXInfo
{
public:
	static CConfigIRRXInfo& getInstance( );

protected:
	CConfigIRRXFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;

protected:
	CConfigIRRXInfo();

public:
	virtual ~CConfigIRRXInfo();
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigIRRXFile& getConfigFile( ) { return m_configFile; }
	void dump( );
};
////////////////////////////////////////////////////////////////////////
#endif 
////////////////////////////////////////////////////////////////////////
