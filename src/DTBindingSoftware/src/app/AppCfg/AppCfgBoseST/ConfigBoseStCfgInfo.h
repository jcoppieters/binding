/**********************************************************//**
 * @file	configbosestcfginfo.h
 * @brief	header bestand voor de Bose st Info klasse
 *
 * @author	Mitchell Tom
 * @date	20150324
 *
 * @since	0x0802
 *************************************************************/

#pragma once

/*************************************************************/
#include "ConfigBoseStCfgFile.h"	
/*************************************************************/

/*************************************************************/
/*******************CConfigBoseSTCfgInfo**********************/
/*************************************************************/
class CConfigBoseSTCfgInfo
{
public:
	static CConfigBoseSTCfgInfo& getInstance( );
	
	~CConfigBoseSTCfgInfo(void);
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigBoseSTCfgFile& getConfigFile( ) { return m_configFile; }
	void dump( );

protected:
	CConfigBoseSTCfgInfo(void);

private:
	CConfigBoseSTCfgFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
};
/*************************************************************/

/*********************** END OF FILE *************************/