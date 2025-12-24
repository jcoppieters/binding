/**********************************************************//**
 * @file	ConfigRoomCtrlAVFSMMaster.h
 * @brief	header bestand voor de RoomCtrl audiocfg fsm klasse
 *
 * @author	Mitchell Tom
 * @date	20130117
 *
 * @since	0x0625
 *************************************************************/
#pragma once


/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
/*************************************************************/
class CConfigRoomCtrlAVInfo;


class IComApplication;

/*************************************************************/
/*************** CConfigRoomCtrlAVFSMMaster ******************/
/*************************************************************/
class CConfigRoomCtrlAVFSMMaster : public CConfigExternalFSMMaster
{
	typedef CConfigExternalFSMMaster Base;
	
	enum 
	{ 
		CONFIG_CLASS_INDEX_DEST			= 0,
		CONFIG_CLASS_INDEX_SOURCE		= 1,
		CONFIG_CLASS_INDEX_FUNCTION		= 2,
		CONFIG_CLASS_INDEX_TNRPRESET	= 3,

		CONFIG_CLASS_INDEX_MAX			= 4 
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
	CConfigRoomCtrlAVFSMMaster(IComApplication& parent);
	~CConfigRoomCtrlAVFSMMaster( );
	BOOL Start(CConfigRoomCtrlAVInfo& info);
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
	BOOL startChildFSMCfgDest( );
	BOOL startChildFSMCfgSrc( );
	BOOL startChildFSMCfgFunc( );
	BOOL startChildFSMCfgTnrPreset( );

	BOOL (CConfigRoomCtrlAVFSMMaster::*m_pfStartChildFsm[CONFIG_CLASS_INDEX_MAX])( );

	// Helper klasse.
	BOOL startChildFSM(
		CConfigExternalClass* pClass,
		const CConfigStateInfo* pStateInfo);
};
/*************************************************************/

/*********************** END OF FILE *************************/