/**********************************************************//**
 * @file	ConfigAppUiCtrlInfo
 * @brief	header bestand voor de CConfigAppUiCtrlInfo klasse.
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *************************************************************/
#ifndef DUOTECNO_CONFIG_UICTRLINFO
#define DUOTECNO_CONFIG_UICTRLINFO
/*************************************************************/

/*************************************************************/
#include "ConfigAppUiCtrlFile.h"
/*************************************************************/

/*************************************************************/
/*******************CConfigAppUiCtrlInfo**********************/
/*************************************************************/
class CConfigAppUiCtrlInfo
{
public:
	static CConfigAppUiCtrlInfo& getInstance( );
	
	~CConfigAppUiCtrlInfo(void);
	CString GetFullPathName(BYTE bNodeAddress);
	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigAppUiCtrlFile& getConfigFile( ) 
	{ 
		return m_file; 
	}
	void dump( );

protected:
	CConfigAppUiCtrlInfo(void);

private:
	CConfigAppUiCtrlFile m_file;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
};
/*************************************************************/


/*************************************************************/
#endif /* DUOTECNO_CONFIG_UICTRLINFO */
/*************************************************************/