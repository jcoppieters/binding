// ConfigSmapPageInfoFile.h: interface for the CConfigSmapPageInfoFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPPAGEINFOFILE_H__FB894766_C8CC_4B61_958C_048AED1F07E2__INCLUDED_)
#define AFX_CONFIGSMAPPAGEINFOFILE_H__FB894766_C8CC_4B61_958C_048AED1F07E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////

struct ConfigSmapPageInfoClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_SMAP_PAGEINFO = 0,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};

///////////////////////////////////////////////////////////////////////

class CConfigSmapPageInfoFile  
{
private:
	enum {  MAX_CLASSES = ConfigSmapPageInfoClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigSmapPageInfoFile();
	virtual ~CConfigSmapPageInfoFile();

	BOOL Load(CString szFilename);
	BOOL Initialise(void);
	int GetNrErrors(void) {
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);
	CConfigExternalClass* GetClass(int type); 	
	CConfigExternalClass** GetClasses( ) { return m_ppConfigBaseClass;}

};

#endif // !defined(AFX_CONFIGSMAPPAGEINFOFILE_H__FB894766_C8CC_4B61_958C_048AED1F07E2__INCLUDED_)
