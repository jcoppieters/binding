/**********************************************************//**
 * @file	ConfigAppUiCtrlFile
 * @brief	header bestand voor de File klasse
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *************************************************************/
#ifndef DUOTECNO_CONFIG_UICTRLFILE
#define DUOTECNO_CONFIG_UICTRLFILE
/*************************************************************/

/*************************************************************/
struct CConfigExternalClass;
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
/*************************************************************/

/*************************************************************/
/**************** ConfigAppUiCtrlClasses *********************/
/*************************************************************/
struct ConfigAppUiCtrlClasses
{
	enum 
	{	
		ENUM_CONFIGCLASS_LEDCFG_GENERAL = 0,	
		ENUM_CONFIGCLASS_LEDCFG_LEDDATA = 1,
	
		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
/*************************************************************/


/*************************************************************/
/***************** CConfigAppUiCtrlFile **********************/
/*************************************************************/
class CConfigAppUiCtrlFile 
{
private:
	enum {  MAX_CLASSES = ConfigAppUiCtrlClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigAppUiCtrlFile();
	virtual ~CConfigAppUiCtrlFile();
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

/*************************************************************/
#endif /* DUOTECNO_CONFIG_UICTRLFILE */
/*************************************************************/