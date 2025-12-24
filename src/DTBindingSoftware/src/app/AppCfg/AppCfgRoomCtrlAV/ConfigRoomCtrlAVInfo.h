/**********************************************************//**
 * @file	configRoomCtrlAVInfo.h
 * @brief	header bestand voor de RoomCtrl Info klasse
 *
 * @author	Mitchell Tom
 * @date	20130116
 *
 * @since	0x0625
 *************************************************************/

#pragma once

/*************************************************************/
#include "configRoomCtrlAVFile.h"
/*************************************************************/

/*************************************************************/
/*******************CConfigRoomCtrlAVInfo*********************/
/*************************************************************/
class CConfigRoomCtrlAVInfo
{
public:
	static CConfigRoomCtrlAVInfo& getInstance( );
	
	~CConfigRoomCtrlAVInfo(void);
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigRoomCtrlAVFile& getConfigFile( ) { return m_configFile; }
	void dump( );

protected:
	CConfigRoomCtrlAVInfo(void);

private:
	CConfigRoomCtrlAVFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
};
/*************************************************************/

/*********************** END OF FILE *************************/