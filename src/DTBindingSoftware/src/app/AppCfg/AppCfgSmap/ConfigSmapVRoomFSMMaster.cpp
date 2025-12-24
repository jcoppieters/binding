#include "stdafx.h"
#include "ConfigSmapVRoomFSMMaster.h"
#include "configsmapvroomclass.h"
#include "configsmapvroominfo.h"
#include "configsmapvroomfile.h"

#include "app\appcfg\appcfgbase\ConfigStateInfo.h"
#include "Nodemess.h"

#include "IComApplication.h"			// temp


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
		STATE_CONFIG_SMAP_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_SMAP_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_SMAP_START = CConfigStateInfo_States::STATE_CONFIG_USER,	
		STATE_CONFIG_SMAP_CONFIGDONE
	};

	const CConfigStateInfo SmapAVRoomStateInfo[ ] =
	{
		{
			STATE_CONFIG_SMAP_INITIALISE,	// !!!!!			// m_currentState
			STATE_CONFIG_SMAP_START,							// m_nextState
			0xFF,							// !!!!!			// m_configClassIndex
			SMAP_CONFIG_AVROOM_INITIALIZE,						// m_bMethodData
			"Initialise smap AV-Room config."					// m_szInfo
		},
		{
			STATE_CONFIG_SMAP_START,		// !!!!!			// m_currentState
			STATE_CONFIG_SMAP_DONE,								// m_nextState
			0,								// !!!!!			// m_configClassIndex
			SMAP_CONFIG_AVROOM_SET,								// m_bMethodData
			"Uploading smap AV-Room info:02X"					// m_szInfo
		},
		{
			STATE_CONFIG_SMAP_CONFIGDONE,						// m_currentState
			STATE_CONFIG_SMAP_DONE,								// m_nextState
			0xFF,												// m_configClassIndex
			SMAP_CONFIG_AVROOM_DONE,							// m_bMethodData
			"Uploading smap AV-Room done"						// m_szInfo
		}
	};
};
///////////////////////////////////////////////////////////////////////
CConfigSmapAVRoomFSMMaster::CConfigSmapAVRoomFSMMaster(IComApplication& parent) :
Base( parent ), 
m_currentIndex( 0 ), 
m_maxIndex( 0 ),
m_currentState( STATE_IDLE ),
m_pConfigClasses( 0 ) 
{ 
}
CConfigSmapAVRoomFSMMaster::~CConfigSmapAVRoomFSMMaster() 
{ 
}
/*************************************************************/
BOOL CConfigSmapAVRoomFSMMaster::Start(CConfigSmapAVRoomInfo& info)
{	
	m_pConfigClasses =
		reinterpret_cast<CConfigSmapVRoomClass*>
			(info.getConfigFile().GetClass(ConfigSmapAVRoomClasses::ENUM_CONFIGCLASS_SMAP_AVROOM));
	
	if (m_pConfigClasses)
	{
		setCurrentNode(info.getNodeAddress( ));
		FSM( EVENT_START );
		return TRUE;
	}
	return FALSE;
} 
BOOL CConfigSmapAVRoomFSMMaster::Cancel( )
{
	return (getFSM().Cancel());
}
/*************************************************************/
BOOL CConfigSmapAVRoomFSMMaster::StartFSM( )
{
	if (m_pConfigClasses)
	{		
		const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   (CConfigExternalClass**) &m_pConfigClasses,
							   const_cast<CConfigStateInfo*>(&SmapAVRoomStateInfo[0]),
							   m_pConfigClasses->getStartIndex());
		return fResult;
	}
	return FALSE;
}
BOOL CConfigSmapAVRoomFSMMaster::StartFSMDone( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						   
						   const_cast<CConfigStateInfo*>(&SmapAVRoomStateInfo[2]));
	return fResult;
}
/*************************************************************/
BOOL CConfigSmapAVRoomFSMMaster::FSM(int event)
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
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM );
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
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM );
					}					
				} return TRUE;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM );
					
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
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM );

				} return TRUE;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM );

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
void CConfigSmapAVRoomFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}
void CConfigSmapAVRoomFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}

