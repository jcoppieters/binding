#ifndef DUOTECNO_CONFIG_MOODINFO_H
#define DUOTECNO_CONFIG_MOODINFO_H
///////////////////////////////////////////////////////////////////////
#include "ConfigMoodCreateFile.h"
///////////////////////////////////////////////////////////////////////
class CConfigMoodInfo
{
public:
	static CConfigMoodInfo& getInstance( );

protected:
	CConfigMoodInfo();

public:
	virtual ~CConfigMoodInfo();
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigMoodCreateFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
	// Zie BUG_SOLVE_0158
	BOOL isReady() const;
	void setReady(BOOL flag);

private:
	CConfigMoodCreateFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
	BOOL m_fReady;	// Zie BUG_SOLVE_0158
};
////////////////////////////////////////////////////////////////////////
#endif 
////////////////////////////////////////////////////////////////////////
