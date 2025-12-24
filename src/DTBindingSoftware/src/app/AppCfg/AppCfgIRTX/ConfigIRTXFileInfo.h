#pragma once

#include "ConfigIRTXFile.h"

////////////////////////////////////////////////

class CConfigIRTXFileInfo
{
public:
	static CConfigIRTXFileInfo& getInstance( );

protected:
	CConfigIRTXFile m_configFile;	
	BOOL m_fileLoaded;

protected:
	CConfigIRTXFileInfo();

public:
	virtual ~CConfigIRTXFileInfo();
	CString GetFullPathName( ) const;
	BOOL Initialise(void);
	BOOL load( );
	BOOL isLoaded( ) const;
	BOOL isAvailable( ) const;
	CConfigIRTXFile& getConfigFile( ) { return m_configFile; }
	// void dump( );
};

////////////////////////////////////////////////