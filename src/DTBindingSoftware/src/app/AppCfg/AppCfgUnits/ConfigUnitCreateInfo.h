#ifndef DUOTECNO_CONFIG_UNITCREATEINFO_H
#define DUOTECNO_CONFIG_UNITCREATEINFO_H
///////////////////////////////////////////////////////////////////////
#include "ConfigUnitCreateFile.h"
///////////////////////////////////////////////////////////////////////

// Sedert V16.40:
// ICConfigUnitCreateDataValidator meegeven in de load functie.
class CConfigUnitCreateInfo
{
public:
	static CConfigUnitCreateInfo& getInstance( );

protected:
	CConfigUnitCreateFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;

protected:
	CConfigUnitCreateInfo();

public:
	virtual ~CConfigUnitCreateInfo();
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress,ICConfigUnitCreateDataValidator* pValidator);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigUnitCreateFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
};
////////////////////////////////////////////////////////////////////////
#endif 
////////////////////////////////////////////////////////////////////////
