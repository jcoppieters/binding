#include "stdafx.h"
#include "ConfigExternalFSM.h"
/***********************************************************/
#include "ConfigStateInfo.h"
#include "ConfigExternalClass.h"	
/***********************************************************/
#include "Nodemess.h"
#include "TxNodeMessage.h"
/***********************************************************/
using DUOTECNO::MFC_HELPER::ITimerControl;
using DUOTECNO::MFC_HELPER::IDisplayDevice;
/***********************************************************/
#define DEBUG_LOCAL_ON		0
/***********************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define DEBUG_TRACE(x)		TRACE(x)
#else
	#define	DEBUG_TRACE(x)
#endif
/***********************************************************/
CConfigExternalFSM::CConfigExternalFSM(IConfigExternalFSM_Master& parent) :
m_parent( parent ),
m_pStateInfo( 0 ),
m_pCurrentState( 0 ),
m_pCurrentClass( 0 ),
m_pConfigClasses( 0 ),
m_currentState( CConfigStateInfo_States::STATE_CONFIG_IDLE ),
m_currentIndex( 0 ),
m_visualisationIndex( 0 ),
m_bNodeAddress( 0xFF )
{
}
CConfigExternalFSM::~CConfigExternalFSM( )
{
}
/***********************************************************/
void CConfigExternalFSM::TxMessage(BYTE bMethod) 
{ 
	CTXExternalConfigMessages::getInstance()->SendControl(m_bNodeAddress,bMethod);
}
void CConfigExternalFSM::TxData( )
{ 
	enum { MAX_SEGMENTED_LENGTH	= 128 };
	BYTE pbData[MAX_SEGMENTED_LENGTH];

#if (DEBUG_LOCAL_ON == 1)

	if (1)
	{
		char szBuffer[255];
		sprintf(szBuffer,
				"TxData: CurrentIndex = %d\n", 
				m_currentIndex);
		DEBUG_TRACE( szBuffer );
	}

	m_pCurrentClass->dump( );

#endif
				
	memset(pbData,'\0',MAX_SEGMENTED_LENGTH);								// TM20090716

	m_pCurrentClass->GetData(m_currentIndex,&pbData[0]);

	ASSERT(m_currentIndex >= 0 && m_currentIndex <= 0xFF);					// CHANGES_MVS_2008_WARNINGS

	CTXExternalConfigMessages::getInstance()
				->SendConfig(m_bNodeAddress, 
							 m_pCurrentState->m_bMethodData, 
							 static_cast<BYTE>(m_currentIndex),				// CHANGES_MVS_2008_WARNINGS
							 &pbData[0]);
}
void CConfigExternalFSM::DisplayCurrent(const char* s)
{
	CString szInfo;
	CString szTemp(m_pCurrentState->m_szInfo);

	if (!szTemp.IsEmpty())
	{
		int index = szTemp.Find("02X");
		if (index != -1)
		{
			szTemp.TrimRight("02X");
			szInfo.Format("%s%02x - %s",					 
						 szTemp,
						 m_visualisationIndex+m_currentIndex,
						 s);
		}
		else
		{	
			szInfo.Format("%s - %s", 					  
						  szTemp,
						  s);
		}			
		DisplayInfoMsg(szInfo);			
	}
}

void CConfigExternalFSM::Fire_EvDone()
{
	m_currentState = CConfigStateInfo_States::STATE_CONFIG_IDLE;
	m_parent.getEventSink().NotifyOnEventDone( );
}
void CConfigExternalFSM::Fire_EvError()
{
	m_currentState = CConfigStateInfo_States::STATE_CONFIG_IDLE;
	m_parent.getEventSink().NotifyOnEventError( );
}
inline void CConfigExternalFSM::DisplayInfoMsg(const char* s)
{
	m_parent.getDisplayDevice().displayInfoMsg( s );
}
inline void CConfigExternalFSM::DisplayErrorMsg(const char* s)
{
	m_parent.getDisplayDevice().displayErrorMsg( s );
}
inline void CConfigExternalFSM::StartTimer(void)
{
	m_parent.getTimerControl().startTimer( TIMER_ID );
}
inline void CConfigExternalFSM::StopTimer(void)
{
	m_parent.getTimerControl().stopTimer( TIMER_ID );
}
/***********************************************************/
BOOL CConfigExternalFSM::
Start(BYTE bNodeAddress,CConfigStateInfo* pStateInfo)
{
	m_bNodeAddress = bNodeAddress;
	m_pStateInfo = pStateInfo;
	m_pConfigClasses = 0;
	m_visualisationIndex = 0;
	m_currentState = 0;
	m_currentIndex = 0;

	m_pCurrentState = &m_pStateInfo[m_currentState];
	if (m_pCurrentState)
	{
		m_currentState = m_pCurrentState->m_currentState;
	
		m_pCurrentClass = 0;
		TxMessage(m_pCurrentState->m_bMethodData);
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigExternalFSM::Start(BYTE bNodeAddress, 
								CConfigExternalClass** pConfigClasses,
								CConfigStateInfo* pStateInfo,
								int visualisationIndex)
{
	m_bNodeAddress = bNodeAddress;
	m_pStateInfo = pStateInfo;
	m_pConfigClasses = pConfigClasses;
	m_visualisationIndex = visualisationIndex;

	CString s;

	if (m_currentState != CConfigStateInfo_States::STATE_CONFIG_IDLE) 
	{
		return FALSE;
	}

	if (m_pStateInfo == 0 || m_pConfigClasses == 0)
	{
		/**
		 * @todo
		 */
	}

	m_currentState = 0;
	m_pCurrentState = &m_pStateInfo[m_currentState];

	if (m_pCurrentState == 0)
	{		
		return FALSE;
	}

	m_currentState = m_pCurrentState->m_currentState;
	m_currentIndex = 0;
	
	if (m_currentState == CConfigStateInfo_States::STATE_CONFIG_INITIALISE)
	{
		m_pCurrentClass = 0;
		TxMessage(m_pCurrentState->m_bMethodData);	
		return TRUE;
	}
	else
	{		
		m_pCurrentClass = m_pConfigClasses[m_pCurrentState->m_configClassIndex];
		ASSERT( m_pCurrentClass );

		TxData( );						

		return TRUE;
	}
	return FALSE;
}

BOOL CConfigExternalFSM::Cancel( )
{
	if (m_currentState == CConfigStateInfo_States::STATE_CONFIG_IDLE) 
	{
		return TRUE;
	}

	m_currentState = CConfigStateInfo_States::STATE_CONFIG_IDLE;

	CString s("User cancel - FSM is stopped");
	DisplayInfoMsg(s);

	m_parent.getEventSink().NotifyOnEventError( );	
	return TRUE;
}

BOOL CConfigExternalFSM::EvSignalStatusConfigError( )
{
	DisplayCurrent("Failed");

	if (m_currentState == CConfigStateInfo_States::STATE_CONFIG_IDLE)
	{
		return TRUE;
	}

	m_currentState = CConfigStateInfo_States::STATE_CONFIG_IDLE;

	CString s("Error Received - FSM is stopped");
	DisplayInfoMsg(s);
	
	m_parent.getEventSink().NotifyOnEventError( );
	return TRUE;
}

BOOL CConfigExternalFSM::EvAfterConfigTimeout( )
{
	DisplayCurrent("Failed, timedout");
	if (m_currentState == CConfigStateInfo_States::STATE_CONFIG_IDLE) {
		return TRUE;
	}

	m_currentState = CConfigStateInfo_States::STATE_CONFIG_IDLE;

	CString s("Timed out - FSM is stopped");
	DisplayInfoMsg(s);
	
	m_parent.getEventSink().NotifyOnEventError( );
	return TRUE;
}

BOOL CConfigExternalFSM::EvSignalStatusConfigOk( )
{	
	DisplayCurrent("Done");

	if (m_currentState != CConfigStateInfo_States::STATE_CONFIG_IDLE) 
	{
		StopTimer();

		m_currentIndex++;		// NEW !!!!!!!!!!!!!!!!!!!!!!!!!	

#if (DEBUG_LOCAL_ON == 1)
		if (1)
		{
			char szBuffer[255];
			sprintf(szBuffer,
					"EvSignalStatusOk: CurrentIndex = %d\n", 
					m_currentIndex);
			DEBUG_TRACE( szBuffer );
		}
#endif

		if (m_pCurrentClass == 0 || m_currentIndex >= m_pCurrentClass->GetMax())
		{
			if (m_pCurrentState != 0)
			{
				if (m_pCurrentState->m_nextState == 
					CConfigStateInfo_States::STATE_CONFIG_DONE)
				{
					Fire_EvDone();
					return TRUE;
				}
				else
				{
					m_currentState = m_pCurrentState->m_nextState;

					// TM20091001: Solved bug
					// De volgende toestand is niet de index van de stateInfo klassen.

					m_pCurrentState = 0;
					for (int i=0; ;i++)
					{
#if (DEBUG_LOCAL_ON == 1)
						if (1)
						{
							char szBuffer[255];
							sprintf(szBuffer,
									"i=%i,"
									"m_pStateInfo[i].m_CurrentState=%i,"
									"m_currentState=%i\n",
									i,
									m_pStateInfo[i].m_currentState,
									m_currentState);

							DEBUG_TRACE( szBuffer );									
						}
#endif
						if (m_pStateInfo[i].m_currentState == m_currentState) 
						{
							m_pCurrentState = &m_pStateInfo[i];

#if (DEBUG_LOCAL_ON == 1)
							if(1)
							{
								char szBuffer[255];
								sprintf(szBuffer,"CurrentState=%i",m_currentState);
								DEBUG_TRACE( szBuffer );
							}
#endif
							break;
						}
					}				
					ASSERT( m_pCurrentState );
									
					if ( m_pCurrentState )
					{
						m_pCurrentClass = m_pConfigClasses[m_pCurrentState->m_configClassIndex];
					}
					ASSERT( m_pCurrentClass );

					m_currentIndex = 0;
				}
			}
		}
			
		while(1) 
		{				
			if (m_pCurrentClass &&
				m_pCurrentClass->IsUsed(m_currentIndex) && 
				m_currentIndex < m_pCurrentClass->GetMax())	
			{				
				StartTimer();				
				TxData( );																			
				return TRUE;			
			}	
			else
			{
				if ((m_pCurrentClass == 0) || 
					(++m_currentIndex >= m_pCurrentClass->GetMax()))
				{
					// We leave the while loop and no messages has been send ...
					break;
				}
			}

		} // end while ...
	
		StartTimer();		
	
		EvSignalStatusConfigOk();

		return TRUE;
	}
	return FALSE;
}

