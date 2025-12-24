#include "stdafx.h"
#include "ConfigUnitFSMMaster.h"
///////////////////////////////////////////////////////////////////////
#include "ConfigUnitCreateInfo.h"
#include "ConfigUnitCreateClass.h"
#include "ConfigUnitCreateFile.h"
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
		STATE_CONFIG_UNITS_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_UNITS_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,
		STATE_CONFIG_UNITS_START = CConfigStateInfo_States::STATE_CONFIG_USER,
		STATE_CONFIG_UNITS_CONFIGDONE
	};

	const CConfigStateInfo UnitCreateStateInfo[ ] =
	{
		{
			STATE_CONFIG_UNITS_INITIALISE,
			STATE_CONFIG_UNITS_START,
			0xFF,
			UNIT_CONFIG_INITIALIZE,
			"Start unit config"
		},
		{
			STATE_CONFIG_UNITS_START,		// !!!!!			// m_currentState
			STATE_CONFIG_UNITS_DONE,							// m_nextState
			0x00,							// !!!!!			// m_configClassIndex
			UNIT_CONFIG_SET,									// m_bMethodData
			"Uploading unit info:02X"							// m_szInfo
		},
		{
			STATE_CONFIG_UNITS_CONFIGDONE,						// m_currentState
			STATE_CONFIG_UNITS_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			UNIT_CONFIG_DONE,									// m_bMethodData
			"Unit config done"									// m_szInfo
		}
	};
};
///////////////////////////////////////////////////////////////////////
CConfigUnitFSMMaster::CConfigUnitFSMMaster(IComApplication& parent) :
Base( parent ), 
m_currentIndex( 0 ), 
m_maxIndex( 0 ),
m_currentState( STATE_IDLE ),
m_pConfigClasses( 0 )  
{ 
}
CConfigUnitFSMMaster::~CConfigUnitFSMMaster() 
{ 
}
/*************************************************************/
BOOL CConfigUnitFSMMaster::Start(CConfigUnitCreateInfo& info)
{
	m_pConfigClasses =
		reinterpret_cast<CConfigUnitCreateClass*>
			(info.getConfigFile().GetClass(ConfigUnitCreateClasses::ENUM_CONFIGCLASS_UNIT));
	
	if (m_pConfigClasses)
	{
		setCurrentNode(info.getNodeAddress( ));
		FSM( EVENT_START );
		return TRUE;
	}
	return FALSE;
} 
BOOL CConfigUnitFSMMaster::Cancel( )
{
	return (getFSM().Cancel());
}
/*************************************************************/
BOOL CConfigUnitFSMMaster::StartFSM( )
{
	if (m_pConfigClasses)
	{		
		const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   (CConfigExternalClass**) &m_pConfigClasses,
							   const_cast<CConfigStateInfo*>(&UnitCreateStateInfo[0]),
							   m_pConfigClasses->getStartIndex());
		return fResult;
	}
	return FALSE;
}
BOOL CConfigUnitFSMMaster::StartFSMDone( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						   
						   const_cast<CConfigStateInfo*>(&UnitCreateStateInfo[2]));
	return fResult;
}
/*************************************************************/
BOOL CConfigUnitFSMMaster::FSM(int event)
{
	switch (m_currentState)
	{
		case STATE_IDLE:
		{
			switch (event)
			{
				case EVENT_START: {
					if (StartFSM()) {
						m_currentState = STATE_IRCODES_DONE;
					}
					else {
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG );
					}
				} return TRUE;

				default: {
					ASSERT( 0 );					
				} break;
			}
		} break;

		case STATE_IRCODES_DONE:
		{
			switch (event)
			{
				case EVENT_DONE:
				{
					// De IRCodes werden goed verstuurd.
					if (StartFSMDone( ))
					{
						// Versturen van het CONFIG_DONE commando.
						m_currentState = STATE_DONE;
					}
					else
					{
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG );
					}					
				} return TRUE;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG );
					
				} break;

				default:
				{
					ASSERT( 0 );
				} break;
			}
		} break;

		case STATE_DONE:
		{
			switch (event)
			{
				case EVENT_DONE: {	
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG );
				} return TRUE;

				case EVENT_ERROR: {
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG );
				} return TRUE;

#if(0)
				case EVENT_START: {
					if (StartFSM()) {
						m_currentState = STATE_IRCODES_DONE;
					}
					else {
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG );
					}
				} return TRUE;
#endif 

				default:
				{
					ASSERT( 0 );
				} break;
			}
		} break;
	}
	return FALSE;
}
/*************************************************************/
void CConfigUnitFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}
void CConfigUnitFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}
