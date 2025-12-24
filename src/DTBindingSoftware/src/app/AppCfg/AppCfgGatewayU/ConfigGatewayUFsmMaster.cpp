#include "stdafx.h"
#include "ConfigGatewayUFsmMaster.h"
///////////////////////////////////////////////////////////////////////
#include "ConfigGatewayUInfo.h"
#include "ConfigGatewayUClass.h"
#include "ConfigGatewayUFile.h"
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigStateInfo.h"
#include "Nodemess.h"
///////////////////////////////////////////////////////////////////////
#include "IComApplication.h"			// temp
///////////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON	0
///////////////////////////////////////////////////////////////////////
#if (DEBUG_LOCAL_ON == 1)
	#define TRACER(x)	TRACE(x)
#else
	#define TRACER(x)
#endif
///////////////////////////////////////////////////////////////////////
namespace
{
	enum 
	{	
		STATE_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_START = CConfigStateInfo_States::STATE_CONFIG_USER,	
		STATE_CONFIGDONE
	};

	const CConfigStateInfo _StateInfo[ ] =
	{
		{
			STATE_INITIALISE,					// m_currentState
			STATE_START,						// m_nextState
			0xFF,								// m_configClassIndex
			GATEWAY_CONFIG_UNIV_INITIALIZE,		// m_bMethodData
			"Initialise gateway config."		// m_szInfo
		},
		{
			STATE_START,						// m_currentState
			STATE_DONE,							// m_nextState
			0,									// m_configClassIndex
			GATEWAY_CONFIG_UNIV_SET,			// m_bMethodData
			"Uploading index:02X"				// m_szInfo
		},
		{
			STATE_CONFIGDONE,					// m_currentState
			STATE_DONE,							// m_nextState
			0xFF,								// m_configClassIndex
			GATEWAY_CONFIG_UNIV_DONE,			// m_bMethodData
			"Uploading gateway config done"		// m_szInfo
		}
	};
};
////////////////////////////////////////////////////////////////////////
CConfigGatewayUFsmMaster::CConfigGatewayUFsmMaster(IComApplication& parent) :
Base( parent ), 
m_currentIndex( 0 ), 
m_maxIndex( 0 ),
m_currentState( STATE_IDLE ),
m_pConfigClasses( 0 ) 
{ 
}

CConfigGatewayUFsmMaster::~CConfigGatewayUFsmMaster(void)
{
}

BOOL CConfigGatewayUFsmMaster::Start(CConfigGatewayUInfo& info)
{	
#if (DEBUG_LOCAL_ON == 1)

	if (1)
	{
		char szBuffer[255];
		for (int i=0;i<3;i++)
		{
			sprintf(szBuffer,
					"Index=%i\n"
					"m_CurrentState=%i\n"
					"m_NextState=%i\n"
					"m_configClassIndex=%i\n"
					"m_bMethodData=%i\n"
					"m_szInfo=%s\n",
					i,
					_StateInfo[i].m_currentState,
					_StateInfo[i].m_nextState,
					_StateInfo[i].m_configClassIndex,
					_StateInfo[i].m_bMethodData,
					_StateInfo[i].m_szInfo);
					
			TRACER(szBuffer);
		}
	}
#endif

	m_pConfigClasses =
		reinterpret_cast<CConfigGatewayUClass*>
			(info.getConfigFile().GetClass(ConfigGatewayClasses::ENUM_CONFIGCLASS_GATEWAY));
	
	if (m_pConfigClasses)
	{
		setCurrentNode(info.getNodeAddress( ));
		FSM( EVENT_START );
		return TRUE;
	}
	return FALSE;
} 
BOOL CConfigGatewayUFsmMaster::Cancel( )
{
	return (getFSM().Cancel());
}

BOOL CConfigGatewayUFsmMaster::StartFSM( )
{
	if (m_pConfigClasses)
	{		
		const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   (CConfigExternalClass**) &m_pConfigClasses,
							   const_cast<CConfigStateInfo*>(&_StateInfo[0]),
							   m_pConfigClasses->getStartIndex());
		return fResult;
	}
	return FALSE;
}

BOOL CConfigGatewayUFsmMaster::StartFSMDone( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						   
						   const_cast<CConfigStateInfo*>(&_StateInfo[2]));
	return fResult;
}

BOOL CConfigGatewayUFsmMaster::FSM(int event)
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
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG );
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
						getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG );
					}					
				} return TRUE;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG );
					
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
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG );

				} return TRUE;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG );

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

void CConfigGatewayUFsmMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}
void CConfigGatewayUFsmMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}
