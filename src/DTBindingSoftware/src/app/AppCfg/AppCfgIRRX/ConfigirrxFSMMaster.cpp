#include "stdafx.h"
#include "ConfigIRRXFSMMaster.h"
///////////////////////////////////////////////////////////////////////
#include "ConfigIRRXInfo.h"
#include "ConfigIRRXClass.h"
#include "ConfigIRRXFile.h"
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
		STATE_CONFIG_IRRX_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_IRRX_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_IRRX_START = CConfigStateInfo_States::STATE_CONFIG_USER,	
		STATE_CONFIG_IRRX_CONFIGDONE
	};

	const CConfigStateInfo UnitCreateStateInfo[ ] =
	{
		{
			STATE_CONFIG_IRRX_INITIALISE,	// !!!!!			// m_currentState
			STATE_CONFIG_IRRX_START,							// m_nextState
			0xFF,							// !!!!!			// m_configClassIndex
			IRRX_CONFIG_INITIALIZE,								// m_bMethodData
			"Initialise irrx config."							// m_szInfo
		},
		{
			STATE_CONFIG_IRRX_START,		// !!!!!			// m_currentState
			STATE_CONFIG_IRRX_DONE,								// m_nextState
			0,								// !!!!!			// m_configClassIndex
			IRRX_CONFIG_SET,									// m_bMethodData
			"Uploading ircode info:02X"							// m_szInfo
		},
		{
			STATE_CONFIG_IRRX_CONFIGDONE,						// m_currentState
			STATE_CONFIG_IRRX_DONE,								// m_nextState
			0xFF,												// m_configClassIndex
			IRRX_CONFIG_DONE,									// m_bMethodData
			"Uploading ircode done"								// m_szInfo
		}
	};
};
///////////////////////////////////////////////////////////////////////
CConfigIRRXFSMMaster::CConfigIRRXFSMMaster(IComApplication& parent) :
Base( parent ), 
m_currentIndex( 0 ), 
m_maxIndex( 0 ),
m_currentState( STATE_IDLE ),
m_pConfigClasses( 0 ) 
{ 
}
CConfigIRRXFSMMaster::~CConfigIRRXFSMMaster() 
{ 
}
/*************************************************************/
BOOL CConfigIRRXFSMMaster::Start(CConfigIRRXInfo& info)
{	
	m_pConfigClasses =
		reinterpret_cast<CConfigIRRXClass*>
			(info.getConfigFile().GetClass(ConfigIRRXClasses::ENUM_CONFIGCLASS_IRRX));
	
	if (m_pConfigClasses)
	{
		setCurrentNode(info.getNodeAddress( ));
		FSM( EVENT_START );
		return TRUE;
	}
	return FALSE;
} 
BOOL CConfigIRRXFSMMaster::Cancel( )
{
	return (getFSM().Cancel());
}
/*************************************************************/
BOOL CConfigIRRXFSMMaster::StartFSM( )
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
BOOL CConfigIRRXFSMMaster::StartFSMDone( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						   
						   const_cast<CConfigStateInfo*>(&UnitCreateStateInfo[2]));
	return fResult;
}
/*************************************************************/
BOOL CConfigIRRXFSMMaster::FSM(int event)
{
	switch (m_currentState)
	{
		case STATE_IDLE:
		{
			switch (event)
			{
				case EVENT_START:
				{
					if (StartFSM())
					{
						m_currentState = STATE_IRCODES_DONE;
					}
					else
					{
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_IRRX );
					}
				} return TRUE;

				default:
				{
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
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_IRRX );
					}					
				} return TRUE;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_IRRX );
					
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
				case EVENT_DONE:
				{	
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_IRRX );

				} return TRUE;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_IRRX );

				} return TRUE;

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
void CConfigIRRXFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}
void CConfigIRRXFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}
