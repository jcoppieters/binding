#pragma once

struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////

struct ConfigSmapAVRoomClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_SMAP_AVROOM = 0,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};

////////////////////////////////////////////////////////////////////////
class CConfigSmapAVRoomFile
{
private:
	enum {  MAX_CLASSES = ConfigSmapAVRoomClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigSmapAVRoomFile();
	virtual ~CConfigSmapAVRoomFile();

	BOOL Load(CString szFilename);
	BOOL Initialise(void);
	int GetNrErrors(void) {
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);
	CConfigExternalClass* GetClass(int type); 	
	CConfigExternalClass** GetClasses( ) { return m_ppConfigBaseClass;}
};
