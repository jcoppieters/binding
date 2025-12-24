/**********************************************************//**
 * @file	AppCfgSmapLiteFile
 * @brief	header bestand voor de CAppCfgSmapLite klasse.
 *
 * @author	Mitchell Tom
 * @date	20161123
 *
 * @since	0x0C05
 *************************************************************/
#ifndef DUOTECNO_CONFIG_APPCFGSMAPLITEFILE_H
#define DUOTECNO_CONFIG_APPCFGSMAPLITEFILE_H
/*************************************************************/

/*************************************************************/
struct CConfigExternalClass;
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"

/*************************************************************/
/*************** ConfigRoomCtrlAVClasses *********************/
/*************************************************************/
struct AppCfgSmapLiteClasses
{
	enum 
	{	
		ENUM_CONFIGCLASS_SMAPLITE = 0,	

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
/*************************************************************/


class CAppCfgSmapLiteFile 
{
private:
	enum {  MAX_CLASSES = AppCfgSmapLiteClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CAppCfgSmapLiteFile();
	virtual ~CAppCfgSmapLiteFile();
public:
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

/*************************************************************/
#endif /* DUOTECNO_CONFIG_APPCFGSMAPLITEFILE_H */
/*************************************************************/