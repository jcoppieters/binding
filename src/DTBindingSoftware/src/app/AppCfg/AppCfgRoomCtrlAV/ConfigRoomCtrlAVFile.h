/**********************************************************//**
 * @file	configRoomCtrlAVClass.h
 * @brief	header bestand voor de RoomCtrl File klasse
 *
 * @author	Mitchell Tom
 * @date	20130116
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
/*************** ConfigRoomCtrlAVClasses *********************/
/*************************************************************/
struct ConfigRoomCtrlAVClasses
{
	enum 
	{	
		ENUM_CONFIGCLASS_ROOMCTRLAV_DEST = 0,	
		ENUM_CONFIGCLASS_ROOMCTRLAV_SOURCE,
		ENUM_CONFIGCLASS_ROOMCTRLAV_FUNC,
		ENUM_CONFIGCLASS_ROOMCTRLAV_TNRPRESET,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
/*************************************************************/

/*************************************************************/
/*************** CConfigRoomCtrlAVFile **********************/
/*************************************************************/
class CConfigRoomCtrlAVFile 
{
private:
	enum {  MAX_CLASSES = ConfigRoomCtrlAVClasses::MAX_CONFIG_CLASSES };

protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

public:
	CConfigRoomCtrlAVFile();
	virtual ~CConfigRoomCtrlAVFile();
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

