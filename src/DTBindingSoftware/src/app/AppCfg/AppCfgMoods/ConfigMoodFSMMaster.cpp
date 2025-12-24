#include "stdafx.h"
#include "ConfigMoodFSMMaster.h"
///////////////////////////////////////////////////////////////////////
#include "ConfigMoodInfo.h"
///////////////////////////////////////////////////////////////////////
#include "ComApplication.h"			// temp
#include "NodeMess.h"
#include "app\appcfg\appcfgbase\ConfigStateInfo.h"
///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON		0
///////////////////////////////////////////////////////////////////////
#if (DEBUG_LOCAL_ON == 1)
	#define DEBUG_TRACE(x)		TRACE(x)
#else
	#define	DEBUG_TRACE(x)
#endif
///////////////////////////////////////////////////////////////////////
namespace
{
	enum 
	{	
		STATE_CONFIG_MOOD_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_MOODS_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_MOODS_START = CConfigStateInfo_States::STATE_CONFIG_USER,	
	};

	const CConfigStateInfo InitStateInfo[ ] =
	{
		{
			STATE_CONFIG_MOOD_INITIALISE,	// !!!!!			// m_currentState
			STATE_CONFIG_MOODS_DONE,							// m_nextState
			0xFF,							// !!!!!			// m_configClassIndex
			MOOD_CONFIG_INITIALIZE,								// m_bMethodData
			"Initialising Node"									// m_szInfo
		}
	};

	const CConfigStateInfo MoodHeaderStateInfo[ ] =
	{
		{
			STATE_CONFIG_MOODS_START,		// !!!!!			// m_currentState
			STATE_CONFIG_MOODS_DONE,							// m_nextState
			0x00,							// !!!!!			// m_configClassIndex
			MOODHEADER_CONFIG_SET,								// m_bMethodData
			"Uploading mood:02X"								// m_szInfo
		}
	};
	const CConfigStateInfo MoodEntryStateInfo[ ] =
	{
		{
			STATE_CONFIG_MOODS_START,		// !!!!!			// m_currentState
			STATE_CONFIG_MOODS_DONE,							// m_nextState
			0x00,							// !!!!!			// m_configClassIndex
			MOODENTRY_CONFIG_SET,								// m_bMethodData
			0	//"Uploading moodEntry:02X"							// m_szInfo
		}
	};

	const CConfigStateInfo MoodStateDoneInfo[ ] =
	{
		{
			STATE_CONFIG_MOODS_START,		// !!!!!			// m_currentState
			STATE_CONFIG_MOODS_DONE,							// m_nextState
			0xFF,							// !!!!!			// m_configClassIndex
			MOOD_CONFIG_DONE,								// m_bMethodData
			"uploading moods done"								// m_szInfo
		}
	};
};
///////////////////////////////////////////////////////////////////////
CConfigMoodFSMMaster::CConfigMoodFSMMaster(IComApplication& parent) :
Base( parent ),
m_currHeader( 0 ),
m_maxEntries( 0 ),
m_maxHeaders( 0 ),
m_pMoodClasses( 0 ),
m_currentState(STATE_INIT) 
{
	CConfigMoodInfo::getInstance().setReady(TRUE);		// BUG_SOLVE_0158
}
CConfigMoodFSMMaster::~CConfigMoodFSMMaster()
{
	CConfigMoodInfo::getInstance().setReady(TRUE);		// BUG_SOLVE_0158
}
/*************************************************************/
BOOL CConfigMoodFSMMaster::Start(CConfigMoodInfo& info)
{
	setCurrentNode(info.getNodeAddress( ));

	m_pMoodClasses = static_cast<CConfigMoodClass*>
		(info.getConfigFile().GetClass(ConfigMoodClasses::ENUM_CONFIGCLASS_MOOD));

	if (m_pMoodClasses)
	{		
		setCurrentNode(info.getNodeAddress( ));
		return FSM( EVENT_START );	
	}
	return FALSE;
} 
BOOL CConfigMoodFSMMaster::Cancel( )
{
	return (FSM( EVENT_CANCEL) );
}
/**************************************************************/
BOOL CConfigMoodFSMMaster::startFSMInit( )
{
	const BOOL fResult =
		getFSM().Start(getCurrentNode(),					  
					   const_cast<CConfigStateInfo*>(&InitStateInfo[0]));

	return fResult;		// CHANGES_MVS_2008_WARNINGS : was TRUE
}
BOOL CConfigMoodFSMMaster::startFSMDone( )
{
	const BOOL fResult =
		getFSM().Start(getCurrentNode(),					  
					   const_cast<CConfigStateInfo*>(&MoodStateDoneInfo[0]));

	return fResult;		// CHANGES_MVS_2008_WARNINGS : was true
}
BOOL CConfigMoodFSMMaster::startChildFSMHeader(int headerIndex)
{
	CMoodHeader* const ptemp =
		m_pMoodClasses->getMoodHeader( headerIndex );

	m_moodHeader.set( ptemp );
	m_moodEntry.set( ptemp );
					
	m_maxEntries = m_moodEntry.GetMax( );

	CConfigExternalClass* ptempClass = &m_moodHeader;
					
	const BOOL fResult =
		getFSM().Start(getCurrentNode(),
					   &ptempClass,
					   const_cast<CConfigStateInfo*>(&MoodHeaderStateInfo[0]),
					   m_moodHeader.getStartIndex());
	return fResult;
}
BOOL CConfigMoodFSMMaster::startChildFSMEntry( )
{
	CConfigExternalClass* ptempClass = &m_moodEntry;

	const BOOL fResult = 
		getFSM().Start(getCurrentNode(),
				       &ptempClass,
					   const_cast<CConfigStateInfo*>(&MoodEntryStateInfo[0]));
	return fResult;
}

BOOL CConfigMoodFSMMaster::FSM(Event ev)
{
	BOOL fResult = FALSE;

	switch( m_currentState )
	{
		case STATE_INIT:
		{
			switch (ev)
			{
				case EVENT_START:
				{	
					DEBUG_TRACE("STATE_INIT: EVENT_START\n");
					fResult = startFSMInit( );
					CConfigMoodInfo::getInstance().setReady(FALSE);		// BUG_SOLVE_0158
					m_currentState = STATE_IDLE;
				} break;
				default:
				{
					ASSERT( 0 );
					fResult = FALSE;					
				} break;
			}
		} break;

		case STATE_IDLE:
		{
			switch (ev)
			{				
				case EVENT_DONE:
				{
					DEBUG_TRACE("STATE_IDLE: EVENT_DONE\n");				
					m_currHeader = 0;
					m_maxHeaders = m_pMoodClasses->GetMax( );
					fResult = startChildFSMHeader(m_currHeader);
					m_currentState = (fResult==TRUE ? STATE_HEADER_DONE : STATE_ERROR);				
				} break;
				case EVENT_CANCEL:
				{
					DEBUG_TRACE("STATE_IDLE: EVENT_CANCEL\n");
					fResult = getFSM().Cancel();
				} break;
				case EVENT_ERROR:
				{
					DEBUG_TRACE("STATE_IDLE: EVENT_ERROR\n");
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_MOODS );					
					fResult = TRUE;
				} break;					
			}
		} break;

		case STATE_HEADER_DONE:
		{
			switch (ev)
			{
				case EVENT_DONE:
				{				
					DEBUG_TRACE("STATE_HEADER: EVENT_DONE\n");
					if (m_maxEntries > 0)
					{						
						fResult = startChildFSMEntry( );						
						m_currentState = (fResult==TRUE ? STATE_ENTRY_DONE : STATE_ERROR);				
					}
					else
					{
						if (++m_currHeader < m_maxHeaders)
						{
							fResult = startChildFSMHeader(m_currHeader);
							m_currentState = (fResult==TRUE ? STATE_HEADER_DONE : STATE_ERROR);						
						}
						else
						{													
							startFSMDone( );
							m_currentState = STATE_DONE;
							fResult = TRUE;
						}
					}
				} break;

				case EVENT_CANCEL:
				{
					DEBUG_TRACE("STATE_HEADER: EVENT_CANCEL\n");
					fResult = getFSM().Cancel();
				} break;

				case EVENT_ERROR:
				{
					DEBUG_TRACE("STATE_HEADER: EVENT_ERROR\n");
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_MOODS );
					fResult = TRUE;
				} break;
			}
		} break;

		case STATE_ENTRY_DONE:
		{
			switch (ev)
			{
				case EVENT_CANCEL:
				{
					DEBUG_TRACE("STATE_ENTRY: EVENT_CANCEL\n");
					fResult = getFSM().Cancel();
				} break;

				case EVENT_ERROR:
				{
					DEBUG_TRACE("STATE_ENTRY: EVENT_ERROR\n");
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_MOODS );
					fResult = TRUE;
				} break;

				case EVENT_DONE:
				{							
					DEBUG_TRACE("STATE_ENTRY: EVENT_DONE\n");
					if (++m_currHeader < m_maxHeaders)
					{
						fResult = startChildFSMHeader(m_currHeader);
						m_currentState = (fResult==TRUE ? STATE_HEADER_DONE : STATE_ERROR);					
					}
					else
					{
						// FSM Done !!!								
						startFSMDone( );
						m_currentState = STATE_DONE;
						fResult = TRUE;
					}					
				} break;
			}			
		} break;

		case STATE_DONE:
		{
			switch (ev)
			{
				case EVENT_CANCEL:
				{
					DEBUG_TRACE("STATE_DONE: EVENT_CANCEL\n");
					fResult = getFSM().Cancel();
				} break;
				case EVENT_DONE:
				{
					DEBUG_TRACE("STATE_DONE: EVENT_DONE\n");
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_MOODS );		
					fResult = TRUE;
				} break;
			}			
		} break;

		case STATE_ERROR:
		{
			getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_MOODS );		
			fResult = TRUE;	
		} break;
	}
	return fResult;
}
/*************************************************************/
void CConfigMoodFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}
void CConfigMoodFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}
/**************************************************************/
