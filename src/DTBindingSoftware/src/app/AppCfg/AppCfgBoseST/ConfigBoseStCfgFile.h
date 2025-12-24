/**********************************************************//**
 * @file	ConfigBoseStCfgFile.h
 * @brief	header bestand voor de Bose ST Cfg File klasse
 *
 * @author	Mitchell Tom
 * @date	20150324
 *
 * @since	0x0625
 *************************************************************/

#pragma once

/*************************************************************/
struct CConfigExternalClass;
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
/*************************************************************/

/*************************************************************/
/**************** ConfigBoseSTCfgClasses *********************/
/*************************************************************/
struct ConfigBoseSTCfgClasses
{
	enum 
	{	
		ENUM_CONFIGCLASS_BOSESTCFG_RENDER = 0,	

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
/*************************************************************/



/*************************************************************/
/***************** CConfigBoseSTCfgFile **********************/
/*************************************************************/
class CConfigBoseSTCfgFile 
{
private:
	enum {  MAX_CLASSES = ConfigBoseSTCfgClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigBoseSTCfgFile();
	virtual ~CConfigBoseSTCfgFile();
	BOOL Load(CString szFilename);
	BOOL Initialise(void);
	int GetNrErrors(void) {
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);
	CConfigExternalClass* GetClass(int type); 	
	CConfigExternalClass** GetClasses( ) { return m_ppConfigBaseClass;}
};
/*************************************************************/

/*********************** END OF FILE *************************/

