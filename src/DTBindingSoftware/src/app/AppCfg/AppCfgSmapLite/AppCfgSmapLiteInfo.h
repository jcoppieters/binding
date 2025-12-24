/**********************************************************//**
 * @file	AppCfgSmapLiteInfo
 * @brief	header bestand voor de CAppCfgSmapLiteInfo klasse.
 *
 * @author	Mitchell Tom
 * @date	20161123
 *
 * @since	0x0C05
 *************************************************************/
#ifndef DUOTECNO_CONFIG_APPCFGSMAPLITEINFO_H
#define DUOTECNO_CONFIG_APPCFGSMAPLITEINFO_H
/*************************************************************/
#include "AppCfgSmapLiteFile.h"

class CAppCfgSmapLiteInfo
{
public:
	static CAppCfgSmapLiteInfo& getInstance( );

protected:
	CAppCfgSmapLiteFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
	BOOL m_updateDatabaseNeeded;	// Bij een smarttouch oled is er een update dbase van de master nodig er wordt een melding getoond.

protected:
	CAppCfgSmapLiteInfo();

public:
	virtual ~CAppCfgSmapLiteInfo();
	CString GetFullPathName(BYTE bNodeAddress);

public:
	BOOL load(BYTE bNodeAddress);
	BOOL save(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	void setUpdateDatabaseNeeded(BOOL flag) { m_updateDatabaseNeeded = flag; } 
	BOOL getUpdateDatabaseNeeded( ) const { return m_updateDatabaseNeeded; }

public:
	CAppCfgSmapLiteFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
};

/*************************************************************/
#endif /* DUOTECNO_CONFIG_APPCFGSMAPLITEINFO_H */
/*************************************************************/