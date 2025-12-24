#ifndef DUOTECNO_CONFIG_SCHEDULEINFO_H
#define DUOTECNO_CONFIG_SCHEDULEINFO_H
///////////////////////////////////////////////////////////////////////
#ifndef DUOTECNO_CONFIG_SCHEDULEFILE_H
	#include "ConfigScheduleFile.h"
#endif
///////////////////////////////////////////////////////////////////////
class CConfigScheduleInfo
{
public:
	static CConfigScheduleInfo& getInstance( );

protected:
	CConfigScheduleInfo();

public:
	virtual ~CConfigScheduleInfo();
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigScheduleFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
	// Zie BUG_SOLVE_0158
	BOOL isReady() const;
	void setReady(BOOL flag);

private:
	CConfigScheduleFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
	BOOL m_fReady;	// Zie BUG_SOLVE_0158
};
////////////////////////////////////////////////////////////////////////
#endif 
////////////////////////////////////////////////////////////////////////
