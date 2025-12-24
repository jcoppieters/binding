#include "stdafx.h"
#include "ConfigScheduleFSMMaster.h"
///////////////////////////////////////////////////////////////////////
#include "ConfigScheduleInfo.h"
#include "ConfigScheduleClass.h"
#include "ConfigScheduleFile.h"
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigStateInfo.h"
#include "Nodemess.h"
///////////////////////////////////////////////////////////////////////
#include "IComApplication.h"			// temp
///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////
namespace
{
	enum 
	{	
		STATE_CONFIG_SCHEDULE_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_SCHEDULE_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_SCHEDULE_ITEMS = CConfigStateInfo_States::STATE_CONFIG_USER,	
		STATE_CONFIG_SCHEDULE_EVENTS,
		STATE_CONFIG_SCHEDULE_CONFIGDONE
	};

	const CConfigStateInfo configStateInfo[ ] =
	{
		{
			STATE_CONFIG_SCHEDULE_INITIALISE,					// m_currentState
			STATE_CONFIG_SCHEDULE_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			SCHEDULE_CONFIG_INITIALIZE,							// m_bMethodData
			"Initialise schedule config."						// m_szInfo
		},
		{
			STATE_CONFIG_SCHEDULE_ITEMS,						// m_currentState
			STATE_CONFIG_SCHEDULE_DONE,							// m_nextState
			0,													// m_configClassIndex
			SCHEDULEITEM_CONFIG_SET,							// m_bMethodData
			"Uploading schedule item:02X"						// m_szInfo
		},
		{
			STATE_CONFIG_SCHEDULE_EVENTS,						// m_currentState
			STATE_CONFIG_SCHEDULE_DONE,							// m_nextState
			0,													// m_configClassIndex
			SCHEDULEEVENT_CONFIG_SET,							// m_bMethodData
			"Uploading schedule event:02X"						// m_szInfo
		},
		{
			STATE_CONFIG_SCHEDULE_CONFIGDONE,					// m_currentState
			STATE_CONFIG_SCHEDULE_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			SCHEDULE_CONFIG_DONE,								// m_bMethodData
			"Uploading schedule config."						// m_szInfo
		}
	};
};
///////////////////////////////////////////////////////////////////////
CConfigScheduleFSMMaster::CConfigScheduleFSMMaster(IComApplication& parent) :
Base( parent ), 
m_currentState( STATE_INIT ) 
{ 
	CConfigScheduleInfo::getInstance().setReady(TRUE);
}
CConfigScheduleFSMMaster::~CConfigScheduleFSMMaster() 
{ 
	CConfigScheduleInfo::getInstance().setReady(TRUE);
}
/*************************************************************/
BOOL CConfigScheduleFSMMaster::Start(CConfigScheduleInfo& info)
{	
	m_pCurrentItems =
		reinterpret_cast<CConfigScheduleItemsClass*>
			(info.getConfigFile().GetClass(ConfigScheduleClasses::ENUM_CONFIGCLASS_SCHEDULE_ITEMS));

	m_pCurrentEvents = 
		reinterpret_cast<CConfigScheduleEventsClass*>
			(info.getConfigFile().GetClass(ConfigScheduleClasses::ENUM_CONFIGCLASS_SCHEDULE_EVENTS));
	
	
	if (m_pCurrentItems && m_pCurrentEvents)
	{
		setCurrentNode(info.getNodeAddress( ));
		return FSM( EVENT_START );	
	}
	return FALSE;
} 
BOOL CConfigScheduleFSMMaster::Cancel( )
{
	return (FSM( EVENT_CANCEL ));
}
BOOL CConfigScheduleFSMMaster::startFSMInit( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[0]));
	return fResult;
}
BOOL CConfigScheduleFSMMaster::startFSMDone( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[3]));
	return fResult;
}

BOOL CConfigScheduleFSMMaster::startChildFSMItems( )
{
	if (m_pCurrentItems)
	{
		const int temp = m_pCurrentItems->GetMax();		// CHANGES_MVS_2008_WARNINGS
		if (temp > 0)
		{				
			CConfigExternalClass* ptempClass = m_pCurrentItems;

			const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   &ptempClass,
							   const_cast<CConfigStateInfo*>(&configStateInfo[1]),
							   0);
			return fResult;
		}
	}
	return FALSE;
}
BOOL CConfigScheduleFSMMaster::startChildFSMEvents( )
{
	if (m_pCurrentEvents)
	{
		const int temp = m_pCurrentEvents->GetMax();	// CHANGES_MVS_2008_WARNINGS		
		if (temp > 0)
		{
			CConfigExternalClass* ptempClass = m_pCurrentEvents;

			const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   &ptempClass,
							   const_cast<CConfigStateInfo*>(&configStateInfo[2]),
							   0);
			return fResult;
		}
	}
	return FALSE;
}
/*************************************************************/
BOOL CConfigScheduleFSMMaster::FSM(BYTE event)
{
	switch (m_currentState)
	{
		case STATE_INIT:
		{
			switch (event)
			{
				case EVENT_START:
				{					
					if (startFSMInit())
					{
						// Versturen van het config init commando.
						m_currentState = STATE_IDLE;						
						CConfigScheduleInfo::getInstance().setReady(FALSE);
						return TRUE;
					}
					ASSERT(0);// konden geen init versturen ?
				} break;
				default:
				{
					ASSERT( 0 );
				} break;
			}
		} break;

		case STATE_IDLE:
		{
			switch(event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());					
				} break;

				case EVENT_DONE:
				{				
					// config initialise is verstuurd. 
					if (startChildFSMItems( ))
					{
						// Schedule items versturen.						
						m_currentState = STATE_ITEMS_DONE;
						return TRUE;
					}
					else
					{							
						if (startChildFSMEvents( ))
						{
							// indien er geen items te versturen zijn,
							// dan events versturen.							
							m_currentState = STATE_ENTRIES_DONE;
							return TRUE;
						}			
						else
						{
							// indien er geen items of events te versturen zijn,
							// dan config done commando versturen.
							startFSMDone( );
							m_currentState = STATE_DONE;
							return TRUE;						
						}
					}
				} break;

				case EVENT_ERROR:
				{				
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE );										
				} return TRUE;
				case EVENT_START:
				{
					ASSERT( 0 );					
				} return FALSE;
			}					
		} break;

		case STATE_ITEMS_DONE:
		{
			switch(event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());					
				} break;

				case EVENT_DONE:
				{	
					// Alle items zijn verstuurd. 
					if (startChildFSMEvents( ))
					{
						// Schedule events versturen.						
						m_currentState = STATE_ENTRIES_DONE;
						return TRUE;
					}			
					else
					{
						// indien er geen events te versturen zijn,
						// dan config done commando versturen.
						startFSMDone( );
						m_currentState = STATE_DONE;
						return TRUE;					
					}				
				} break;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE );										
				} return TRUE;

				case EVENT_START:
				{
					ASSERT( 0 );					
				} return FALSE;
			}
		} break;

		case STATE_ENTRIES_DONE:
		{
			switch (event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());					
				} break;

				case EVENT_DONE:
				{
					// Alle entries zijn verstuurd, 
					// het config done commando versturen.
					startFSMDone( );
					m_currentState = STATE_DONE;					

				} return TRUE;		// CHANGES_MVS_2008_WARNINGS

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE );														
				} return TRUE;		// CHANGES_MVS_2008_WARNINGS

				case EVENT_START:
				{
					ASSERT( 0 );					
				} return FALSE;
			}
		} break;

		case STATE_DONE:
		{
			switch (event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());					
				} break;

				case EVENT_DONE:
				{				
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE );
					return TRUE;
				} break;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE );
					return TRUE;										
				} return TRUE;

				case EVENT_START:
				{
					ASSERT( 0 );					
				} return FALSE;
			}					
		} break;
	}
	return FALSE;
}
/*************************************************************/
void CConfigScheduleFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}
void CConfigScheduleFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}
