#ifndef DUOTECNO_CONFIGSCHEDULE_FSMMASTER_H
#define	DUOTECNO_CONFIGSCHEDULE_FSMMASTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
///////////////////////////////////////////////////////////////////
class CConfigScheduleInfo;
class CConfigScheduleItemsClass;
class CConfigScheduleEventsClass;
///////////////////////////////////////////////////////////////////
class IComApplication;
///////////////////////////////////////////////////////////////////
class CConfigScheduleFSMMaster : public CConfigExternalFSMMaster
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
		 * @def		STATE_ITEMS_DONE
		 * @brief	De scheduling items werden net doorgestuurd
		 */
		STATE_ITEMS_DONE,

		/**
		 * @def		STATE_ENTRIES_DONE
		 * @brief	De scheduling entries werden net doorgestuurd.
		 */
		STATE_ENTRIES_DONE,

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

	CConfigScheduleItemsClass* m_pCurrentItems;
	CConfigScheduleEventsClass* m_pCurrentEvents;

public:
	CConfigScheduleFSMMaster(IComApplication& parent);
	~CConfigScheduleFSMMaster( );

public:
	BOOL Start(CConfigScheduleInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );

	BOOL FSM(BYTE event);
	BOOL startFSMInit( );
	BOOL startFSMDone( );
	BOOL startChildFSMItems( );
	BOOL startChildFSMEvents( );
};
///////////////////////////////////////////////////////////////////
#endif