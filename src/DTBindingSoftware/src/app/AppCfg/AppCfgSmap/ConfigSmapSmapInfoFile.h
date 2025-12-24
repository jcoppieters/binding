// ConfigSmapSmapInfoFile.h: interface for the CConfigSmapSmapInfoFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPSMAPINFOFILE_H__2CC87512_8368_432B_921E_E57ACF1AB98D__INCLUDED_)
#define AFX_CONFIGSMAPSMAPINFOFILE_H__2CC87512_8368_432B_921E_E57ACF1AB98D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////

struct ConfigSmapSmapInfoClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_SMAP_SMAPINFO = 0,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
///////////////////////////////////////////////////////////////////////

class CConfigSmapSmapInfoFile  
{
private:
	enum {  MAX_CLASSES = ConfigSmapSmapInfoClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigSmapSmapInfoFile();
	virtual ~CConfigSmapSmapInfoFile();

	BOOL Load(CString szFilename);
	BOOL Save(CString szFilename);
	BOOL Initialise(void);
	int GetNrErrors(void) {
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);
	CConfigExternalClass* GetClass(int type); 	
	CConfigExternalClass** GetClasses( ) { return m_ppConfigBaseClass;}

};
////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CONFIGSMAPSMAPINFOFILE_H__2CC87512_8368_432B_921E_E57ACF1AB98D__INCLUDED_)
