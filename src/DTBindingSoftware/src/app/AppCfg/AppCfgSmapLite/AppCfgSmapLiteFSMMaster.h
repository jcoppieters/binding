/**********************************************************//**
 * @file	AppCfgSmapLiteFSMMaster
 * @brief	header bestand voor de CAppCfgSmapLiteFSMMaster klasse.
 *
 * @author	Mitchell Tom
 * @date	20161128
 *
 * @since	0x0C05
 *************************************************************/
#ifndef DUOTECNO_CONFIG_APPCFGSMAPLITEFSMMASTER_H
#define DUOTECNO_CONFIG_APPCFGSMAPLITEFSMMASTER_H
/*************************************************************/

/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
/*************************************************************/
class CAppCfgSmapLiteInfo;
class IComApplication;

/*************************************************************/
/*************** CConfigRoomCtrlAVFSMMaster ******************/
/*************************************************************/
class CAppCfgSmapLiteFSMMaster : public CConfigExternalFSMMaster
{
	typedef CConfigExternalFSMMaster Base;
	
	enum 
	{ 
		CONFIG_CLASS_INDEX_SMAPLITE		= 0,
		CONFIG_CLASS_INDEX_MAX			= 1 
	};

	enum States
	{
		/**
		 * @def		STATE_INIT
		 * @brief	Initialiseren van de node
		 */
		STATE_INIT = 0,


		/**
		 * @def		STATE_IDLE
		 * @brief	Start doorsturen van de configuratie.
		 */
		STATE_IDLE,


		/**
		 * @def		STATE_BUSY
		 * @brief	De items werden net doorgestuurd
		 */
		STATE_BUSY,

	
		/**
		 * @def		STATE_DONE
		 * @brief	Gedaan, wachten op reply
		 */
		STATE_DONE 

	} m_currentState;

	enum Event
	{
		EVENT_START = 0,
		EVENT_CANCEL,
		EVENT_DONE,
		EVENT_ERROR
	};

	int m_currentIndex;		/* Huidige index in de config klasses */
	int m_maxIndex;			/* Maximum aantal items die moeten doorgestuurd worden */
	int m_currentClass;		/* huidige klasse - zie MAX_CONFIG_CLASSES */
	CConfigExternalClass* m_pCurrentItems[CONFIG_CLASS_INDEX_MAX];

public:
	CAppCfgSmapLiteFSMMaster(IComApplication& parent);
	~CAppCfgSmapLiteFSMMaster( );

	BOOL Start(CAppCfgSmapLiteInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );

	BOOL FSM(BYTE event);
	BOOL startFSMInit( );
	BOOL startFSMDone( );
	void notifyFSMDone( );
	void notifyFSMError( );

	// MAX_CONFIG_CLASSES functies
	BOOL startChildFSMCfgSmapLite( );

	BOOL (CAppCfgSmapLiteFSMMaster::*m_pfStartChildFsm[CONFIG_CLASS_INDEX_MAX])( );

	// Helper klasse.
	BOOL startChildFSM(
		CConfigExternalClass* pClass,
		const CConfigStateInfo* pStateInfo);
};
/*************************************************************/

/*************************************************************/
#endif /* DUOTECNO_CONFIG_APPCFGSMAPLITEFSMMASTER_H */
/*************************************************************/