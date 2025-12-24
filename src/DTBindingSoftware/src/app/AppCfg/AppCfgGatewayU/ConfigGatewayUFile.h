#pragma once

///////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////

struct ConfigGatewayClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_GATEWAY = 0,
		ENUM_CONFIGCLASS_SYSTEM,			/* Reserve klasse, wordt niet doorgestuurd */

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
///////////////////////////////////////////////////////////////////////

class CConfigGatewayUFile
{
private:
	enum {  MAX_CLASSES = ConfigGatewayClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigGatewayUFile(void);
	~CConfigGatewayUFile(void);

	BOOL Load(CString szFilename);
	BOOL Initialise(void);
	int GetNrErrors(void) {
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);
	CConfigExternalClass* GetClass(int type); 	
	CConfigExternalClass** GetClasses( ) { return m_ppConfigBaseClass;}
};

///////////////////////////////////////////////////////////////////////