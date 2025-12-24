// ConfigSmapMenuFile.h: interface for the CConfigSmapMenuFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPMENUFILE_H__271F79C7_9931_4188_8BB3_3B3B22AB8DB1__INCLUDED_)
#define AFX_CONFIGSMAPMENUFILE_H__271F79C7_9931_4188_8BB3_3B3B22AB8DB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////

struct ConfigSmapMenuClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_SMAP_MENU = 0,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};

///////////////////////////////////////////////////////////////////////

class CConfigSmapMenuFile  
{
private:
	enum {  MAX_CLASSES = ConfigSmapMenuClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigSmapMenuFile();
	virtual ~CConfigSmapMenuFile();

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


#endif // !defined(AFX_CONFIGSMAPMENUFILE_H__271F79C7_9931_4188_8BB3_3B3B22AB8DB1__INCLUDED_)
