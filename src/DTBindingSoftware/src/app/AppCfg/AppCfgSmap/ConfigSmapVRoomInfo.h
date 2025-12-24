#pragma once

#include "ConfigSmapVRoomFile.h"

class CConfigSmapAVRoomInfo
{
public:
	static CConfigSmapAVRoomInfo& getInstance( );

protected:
	CConfigSmapAVRoomFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;

protected:
	CConfigSmapAVRoomInfo();

public:
	~CConfigSmapAVRoomInfo();
	CString GetFullPathName(BYTE bNodeAddress);

	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigSmapAVRoomFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
};
