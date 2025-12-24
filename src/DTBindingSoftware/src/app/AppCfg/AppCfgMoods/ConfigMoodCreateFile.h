#ifndef DUOTECNO_CONFIG_MOODCREATEFILE_H
#define DUOTECNO_CONFIG_MOODCREATEFILE_H
///////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////
struct ConfigMoodClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_MOOD = 0,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
///////////////////////////////////////////////////////////////////////
class CConfigMoodCreateFile 
{
private:
	enum 
	{ 
		MAX_CLASSES = ConfigMoodClasses::MAX_CONFIG_CLASSES 
	};

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigMoodCreateFile();
	virtual ~CConfigMoodCreateFile();

public:
	BOOL Load(CString szFilename);

	BOOL Initialise(void);

	int GetNrErrors(void) 
	{
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);

	CConfigExternalClass* GetClass(int type); 	

	CConfigExternalClass** GetClasses( ) {
		return m_ppConfigBaseClass;
	}
};
////////////////////////////////////////////////////////////////////////
#endif 
////////////////////////////////////////////////////////////////////////
