#ifndef DUOTECNO_CONFIG_IRRXFILE_H
#define DUOTECNO_CONFIG_IRRXFILE_H
///////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////
struct ConfigIRRXClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_IRRX = 0,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
///////////////////////////////////////////////////////////////////////
class CConfigIRRXFile 
{
private:
	enum {  MAX_CLASSES = ConfigIRRXClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigIRRXFile();
	virtual ~CConfigIRRXFile();
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
