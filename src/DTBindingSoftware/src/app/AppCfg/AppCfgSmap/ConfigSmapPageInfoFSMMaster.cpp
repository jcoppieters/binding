// ConfigSmapPageInfoFSMMaster.cpp: implementation of the CConfigSmapPageInfoFSMMaster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigSmapPageInfoFSMMaster.h"

#include "configsmappageinfoinfo.h"
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
		STATE_CONFIG_SMAP_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_SMAP_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_SMAP_START = CConfigStateInfo_States::STATE_CONFIG_USER,	
	};

	const CConfigStateInfo InitStateInfo[ ] =
	{
		{
			STATE_CONFIG_SMAP_INITIALISE,	// !!!!!			// m_currentState
			STATE_CONFIG_SMAP_DONE,								// m_nextState
			0xFF,							// !!!!!			// m_configClassIndex
			SMAP_CONFIG_INITIALIZE,								// m_bMethodData
			"Initialising screenmaps"							// m_szInfo
		}
	};

	const CConfigStateInfo headerStateInfo[ ] =
	{
		{
			STATE_CONFIG_SMAP_START,		// !!!!!			// m_currentState
			STATE_CONFIG_SMAP_DONE,								// m_nextState
			0x00,							// !!!!!			// m_configClassIndex
			SMAP_CONFIG_PAGEINFO_HEADER,						// m_bMethodData
			"Uploading screenmap name info:02X"						// m_szInfo
		}
	};

	const CConfigStateInfo entryStateInfo[ ] =
	{
		{
			STATE_CONFIG_SMAP_START,		// !!!!!			// m_currentState
			STATE_CONFIG_SMAP_DONE,								// m_nextState
			0x00,							// !!!!!			// m_configClassIndex
			SMAP_CONFIG_PAGEINFO_DATA,							// m_bMethodData
			0	//"Uploading moodEntry:02X"						// m_szInfo
		}
	};

	const CConfigStateInfo stateDoneInfo[ ] =
	{
		{
			STATE_CONFIG_SMAP_START,		// !!!!!			// m_currentState
			STATE_CONFIG_SMAP_DONE,								// m_nextState
			0xFF,							// !!!!!			// m_configClassIndex
			SMAP_CONFIG_DONE,									// m_bMethodData
			"uploading screenmap info done"						// m_szInfo
		}
	};
};
///////////////////////////////////////////////////////////////////////
CConfigSmapPageInfoFSMMaster::CConfigSmapPageInfoFSMMaster(IComApplication& parent) :
Base( parent ),
m_currHeader( 0 ),
m_maxEntries( 0 ),
m_maxHeaders( 0 ),
m_pClasses( 0 ),
m_currentState(STATE_INIT) 
{
	CConfigSmapPageInfoInfo::getInstance().setReady(TRUE);	// Zie BUG_SOLVE_0158
}
CConfigSmapPageInfoFSMMaster::~CConfigSmapPageInfoFSMMaster()
{
	CConfigSmapPageInfoInfo::getInstance().setReady(TRUE);	// Zie BUG_SOLVE_0158
}

/*************************************************************/

BOOL CConfigSmapPageInfoFSMMaster::Start(CConfigSmapPageInfoInfo& info)
{
	setCurrentNode(info.getNodeAddress( ));

	m_pClasses = static_cast<CConfigSmapPageInfoClass*>
		(info.getConfigFile().GetClass(ConfigSmapPageInfoClasses::ENUM_CONFIGCLASS_SMAP_PAGEINFO));

	if (m_pClasses)
	{		
		setCurrentNode(info.getNodeAddress( ));
		return FSM( EVENT_START );	
	}
	return FALSE;
} 

BOOL CConfigSmapPageInfoFSMMaster::Cancel( )
{
	return (FSM( EVENT_CANCEL) );
}

/**************************************************************/

BOOL CConfigSmapPageInfoFSMMaster::startFSMInit( )
{
	const BOOL fResult =
		getFSM().Start(getCurrentNode(),					  
					   const_cast<CConfigStateInfo*>(&InitStateInfo[0]));
	return fResult;		
}

BOOL CConfigSmapPageInfoFSMMaster::startFSMDone( )
{
	const BOOL fResult =
		getFSM().Start(getCurrentNode(),					  
					   const_cast<CConfigStateInfo*>(&stateDoneInfo[0]));
	return fResult;	
}

BOOL CConfigSmapPageInfoFSMMaster::startChildFSMHeader(int headerIndex)
{
	DEBUG_TRACE("startChildFSMHeader( )\n");

	/* BUG-0126: Controleren of header aanwezig is*/

	BOOL fResult = FALSE;
	CSmapPageInfoHeader* const ptemp = m_pClasses->getHeader( headerIndex );
	if (ptemp)
	{
		m_Header.set( ptemp );
		m_Entry.set( ptemp );					
		m_maxEntries = m_Entry.GetMax( );
		CConfigExternalClass* ptempClass = &m_Header;					
		fResult = getFSM().Start(getCurrentNode(),
						   &ptempClass,
						   const_cast<CConfigStateInfo*>(&headerStateInfo[0]),
						   m_Header.getStartIndex());
	}
	return fResult;
}

BOOL CConfigSmapPageInfoFSMMaster::startChildFSMEntry( )
{
	DEBUG_TRACE("startChildFSMEntry( )\n");

	CConfigExternalClass* ptempClass = &m_Entry;

	const BOOL fResult = 
		getFSM().Start(getCurrentNode(),
				       &ptempClass,
					   const_cast<CConfigStateInfo*>(&entryStateInfo[0]));
	return fResult;
}

BOOL CConfigSmapPageInfoFSMMaster::FSM(Event ev)
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
					CConfigSmapPageInfoInfo::getInstance().setReady(FALSE);	// Zie BUG_SOLVE_0158
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
					m_maxHeaders = m_pClasses->GetMax( );
					fResult = startChildFSMHeader(m_currHeader);
					m_currentState = (fResult==TRUE ? STATE_HEADER_DONE : STATE_ERROR);
					if (fResult == FALSE)
					{
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO );
					}				
				} break;

				case EVENT_CANCEL:
				{
					DEBUG_TRACE("STATE_IDLE: EVENT_CANCEL\n");
					fResult = getFSM().Cancel();
				} break;

				case EVENT_ERROR:
				{
					DEBUG_TRACE("STATE_IDLE: EVENT_ERROR\n");
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO );
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
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO );
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
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO );
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
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO );		
					fResult = TRUE;
				} break;
			}			
		} break;

		case STATE_ERROR:
		{
			getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO );		
			fResult = TRUE;	
		} break;
	}
	return fResult;
}

/*************************************************************/

void CConfigSmapPageInfoFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}

void CConfigSmapPageInfoFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}
/**************************************************************/
