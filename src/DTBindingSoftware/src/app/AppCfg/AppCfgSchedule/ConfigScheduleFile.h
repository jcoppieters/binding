#ifndef DUOTECNO_CONFIG_SCHEDULEFILE_H
#define DUOTECNO_CONFIG_SCHEDULEFILE_H
///////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////
struct ConfigScheduleClasses
{
	enum 
	{	
		ENUM_CONFIGCLASS_SCHEDULE_ITEMS = 0,	
		ENUM_CONFIGCLASS_SCHEDULE_EVENTS,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
///////////////////////////////////////////////////////////////////////
class CConfigScheduleFile 
{
private:
	enum {  MAX_CLASSES = ConfigScheduleClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigScheduleFile();
	virtual ~CConfigScheduleFile();
	BOOL Load(CString szFilename);
	BOOL Initialise(void);
	int GetNrErrors(void) {
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);
	CConfigExternalClass* GetClass(int type); 	
	CConfigExternalClass** GetClasses( ) { return m_ppConfigBaseClass;}
};
////////////////////////////////////////////////////////////////////////
#endif 
////////////////////////////////////////////////////////////////////////
