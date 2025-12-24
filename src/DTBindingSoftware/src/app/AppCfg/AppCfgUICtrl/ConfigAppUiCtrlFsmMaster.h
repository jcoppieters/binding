/**********************************************************//**
 * @file	ConfigAppUiCtrlFsmMaster
 * @brief	header bestand voor de File klasse
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *************************************************************/
#ifndef DUOTECNO_CONFIG_UICTRLFSMMASTER
#define DUOTECNO_CONFIG_UICTRLFSMMASTER
/*************************************************************/

#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"

class CConfigAppUiCtrlInfo;
class CConfigLedCfgGeneralClass;
class CConfigLedCfgLedClass;
class IComApplication;

class CConfigAppUiCtrlFSMMaster : public CConfigExternalFSMMaster
{
	typedef CConfigExternalFSMMaster Base;
	
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
		 * @def		STATE_LEDCFG_GENERAL_DONE
		 * @brief	De algemene ledscfg werd doorgestuurd.
		 */
		STATE_LEDCFG_GENERAL_DONE,

		/**
		 * @def		STATE_ENTRIES_DONE
		 * @brief	De individuele ledcfg werd doorgestuurd.
		 */
		STATE_LEDCFG_LED_DONE,

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

	int m_currentIndex;
	int m_maxIndex;

	CConfigLedCfgGeneralClass* m_pLedCfgGeneral;
	CConfigLedCfgLedClass* m_pLedCfgLeds;

public:
	CConfigAppUiCtrlFSMMaster(IComApplication& parent);
	~CConfigAppUiCtrlFSMMaster( );

	BOOL Start(CConfigAppUiCtrlInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );

	BOOL FSM(BYTE event);
	BOOL startFSMInit( );
	BOOL startFSMDone( );
	BOOL startChildFSMGeneral( );
	BOOL startChildFSMLeds( );
};

/*************************************************************/
#endif /* DUOTECNO_CONFIG_UICTRLFSMMASTER */
/*************************************************************/