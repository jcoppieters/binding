// ConfigMultiMediaFSMMaster.cpp: implementation of the CConfigMultiMediaFSMMaster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigMultiMediaFSMMaster.h"


// ------------------------------------------------
#include "ConfigMultiMediaInfo.h"
#include "ComApplication.h"

#include "utils/app/DisplayDevice.h"
// ------------------------------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigMultiMediaFSMMaster::CConfigMultiMediaFSMMaster(IComApplication *pCom) :
m_pParent(pCom),
m_CurrentState( STATE_TASK_IDLE ),
m_pMultiMediaFSM( 0 ),
m_pMultiMediaInfo( 0 ),
m_bCurrentIndex( 0 )
{
	m_pMultiMediaFSM = new CConfigMultiMediaFSM(this);
	ASSERT(m_pMultiMediaFSM);
	
	/** @todo	Hier de FSM Toestanden opbouwen !!! */
}

CConfigMultiMediaFSMMaster::~CConfigMultiMediaFSMMaster()
{
	if (m_pMultiMediaFSM != NULL)
	{
		delete m_pMultiMediaFSM;
		m_pMultiMediaFSM = NULL;
	}
}

////////////////////////////////////////////////////////////////////////
BOOL CConfigMultiMediaFSMMaster::Start(CConfigMultiMediaInfo *pInfo)
{	
	ASSERT( pInfo );
	ASSERT( m_pMultiMediaFSM );

	if (m_pMultiMediaFSM == 0)
	{
		return FALSE;
	}
	if (pInfo == 0) 
	{
		return FALSE;
	}
	if (m_CurrentState != STATE_TASK_IDLE) 
	{
		return FALSE;
	}
	
	m_pMultiMediaInfo = pInfo;
	m_bCurrentIndex = 0;
	
	const BYTE bMaxIndex = m_pMultiMediaInfo->GetNrNodes( );
	if (bMaxIndex > 0)
	{
		FSM(FSM_EV_OPERATION_START);
		return TRUE;
	}
	else
	{
		CString s("No nodes found to send multimedia configuration");
		CDisplayDevice::DisplayInfoMsg(s);

		ASSERT( bMaxIndex > 0 );
		return FALSE;	
	}
}

BOOL CConfigMultiMediaFSMMaster::Cancel(void)
{
	if (m_pMultiMediaFSM != NULL)
	{
		m_pMultiMediaFSM->Cancel();
	}
	return TRUE;
}

// ------------------- EVENT RECEIVED FROM UPPER LAYER ------------------------------

BOOL CConfigMultiMediaFSMMaster::EvSignalStatusConfigOk(BYTE bNode) 
{
	ASSERT(m_pMultiMediaFSM && m_pMultiMediaInfo );

	if (m_pMultiMediaFSM && m_pMultiMediaInfo)
	{
		const BYTE bCurrentNodeAddress =
			m_pMultiMediaInfo->GetNodeAddress( m_bCurrentIndex );
		
		if (bCurrentNodeAddress == bNode) 
		{		
			return (m_pMultiMediaFSM->EvSignalStatusConfigOk(bCurrentNodeAddress, m_pMultiMediaInfo));		
		}
	}
	return FALSE;
}
BOOL CConfigMultiMediaFSMMaster::EvSignalStatusConfigError(BYTE bNode) 
{	
	ASSERT(m_pMultiMediaFSM && m_pMultiMediaInfo );

	if (m_pMultiMediaFSM && m_pMultiMediaInfo)
	{	
		const BYTE bCurrentNodeAddress =
			m_pMultiMediaInfo->GetNodeAddress( m_bCurrentIndex );

		if (bCurrentNodeAddress == bNode) 
		{
			return (m_pMultiMediaFSM->EvSignalStatusConfigError(bCurrentNodeAddress, m_pMultiMediaInfo));		
		}
	}
	return FALSE;
}
BOOL CConfigMultiMediaFSMMaster::EvAfterConfigTimeout(void) 
{
	ASSERT(m_pMultiMediaFSM);
	ASSERT(m_pMultiMediaInfo);

	if (m_pMultiMediaFSM != NULL)
	{
		BYTE bCurrentNodeAddress = 0xFF;

		if (m_pMultiMediaInfo)
		{
			bCurrentNodeAddress = m_pMultiMediaInfo->GetNodeAddress( m_bCurrentIndex );
		}

		return (m_pMultiMediaFSM->EvAfterConfigTimeout(bCurrentNodeAddress, m_pMultiMediaInfo));
	}
	return FALSE;
}


// ---------TRANSPARENT EVENTS RECEIVED FROM LOWER LAYER TO UPPER LAYER --------------

void CConfigMultiMediaFSMMaster::StartTimer(void) 
{
	if (m_pParent != NULL) {
		m_pParent->StartTimer(TIMERID_MMCONFIG_ACKTIMEOUT,TIMER_MMCONFIG_ACKTIMEOUT_VALUE);
	}
}

void CConfigMultiMediaFSMMaster::StopTimer(void) 
{
	if (m_pParent != NULL) {
		m_pParent->StopTimer(TIMERID_MMCONFIG_ACKTIMEOUT);
	}
}



//-----------------------EVENTS FROM LOWER LAYER ---------------------------------------
void CConfigMultiMediaFSMMaster::NotifyOnEventInfo(CString s)
{
	ASSERT( m_pMultiMediaInfo );

	CString szMessage;

	if ( m_pMultiMediaInfo )
	{	
		const BYTE bMaxIndex = m_pMultiMediaInfo->GetNrNodes( );
		const BYTE bCurrrentNodeAddress = 
			m_pMultiMediaInfo->GetNodeAddress( m_bCurrentIndex );

		szMessage.Format(_T("Node %d/%d : Adres: 0x%02x : %s"),
						 m_bCurrentIndex+1,
						 bMaxIndex,
						 bCurrrentNodeAddress,
						 s);

		CDisplayDevice::DisplayInfoMsg(szMessage);
	}
}

void CConfigMultiMediaFSMMaster::NotifyOnEventDone(void) 
{
	FSM(FSM_EV_SIGNAL_DONE);
}
void CConfigMultiMediaFSMMaster::NotifyOnEventError(void) 
{
	FSM(FSM_EV_SIGNAL_ERROR);
}
//-----------------------EVENTS FROM LOCAL FSM -------------------------------------
void CConfigMultiMediaFSMMaster::Fire_OnEventDone(void)
{
	if (m_pParent != NULL)
	{
		m_pParent->NotifyOnTaskDone(COMAPPLICATION_TASK_ID_MULTIMEDIA_CONFIG);
	}
	CConfigMultiMediaInfo::GetInstance()->setReady(TRUE);	// BUG_SOLVE_0158
}

void CConfigMultiMediaFSMMaster::Fire_OnEventStart(void) 
{
	CConfigMultiMediaInfo::GetInstance()->setReady(FALSE);	// BUG_SOLVE_0158
}

void CConfigMultiMediaFSMMaster::Fire_OnEventError(void) 
{
	if (m_pParent != NULL)
	{	
		m_pParent->NotifyOnTaskError(COMAPPLICATION_TASK_ID_MULTIMEDIA_CONFIG);
	}
	CConfigMultiMediaInfo::GetInstance()->setReady(TRUE);	// BUG_SOLVE_0158
}	

void CConfigMultiMediaFSMMaster::Fire_OnEventCancel(void) 
{	
	if (m_pParent != NULL)
	{		
		m_pParent->NotifyOnTaskCancelled(COMAPPLICATION_TASK_ID_MULTIMEDIA_CONFIG);
	}
	CConfigMultiMediaInfo::GetInstance()->setReady(TRUE);	// BUG_SOLVE_0158
}	

//////////////////////////////////////////////////////////////
void CConfigMultiMediaFSMMaster::FSM(ENUM_FSM_EVENTS e)
{
	switch (m_CurrentState)
	{
		case STATE_TASK_IDLE:
		{
			switch(e)
			{
				case FSM_EV_OPERATION_START:
				{
					const BYTE bCurrentNodeAddress = 
						m_pMultiMediaInfo->GetNodeAddress(m_bCurrentIndex);

					const int mode =
						m_pMultiMediaInfo->GetNodeInfoType(m_bCurrentIndex);

					m_pMultiMediaFSM->Start(m_pMultiMediaInfo,
											bCurrentNodeAddress,
											mode);

					this->Fire_OnEventStart( );
					m_CurrentState = STATE_TASK_BUSY;
				
				} break;
		
				case FSM_EV_SIGNAL_DONE :
				case FSM_EV_SIGNAL_ERROR :

				break;
			}
		} break;

		case STATE_TASK_BUSY:
		{
			switch(e)
			{
				case FSM_EV_OPERATION_START:
				break;
		
				case FSM_EV_SIGNAL_DONE :
				{
					const BYTE bMaxIndex = m_pMultiMediaInfo->GetNrNodes( );	

					if (++m_bCurrentIndex < bMaxIndex) 
					{
						const BYTE bCurrentNodeAddress = 
							m_pMultiMediaInfo->GetNodeAddress(m_bCurrentIndex);

						const int mode =
							m_pMultiMediaInfo->GetNodeInfoType(m_bCurrentIndex);

						m_pMultiMediaFSM->Start(m_pMultiMediaInfo,
											    bCurrentNodeAddress,
												mode);
					}
					else 
					{						
						Fire_OnEventDone();
						m_CurrentState = STATE_TASK_IDLE;						
					}
				} break;

				case FSM_EV_SIGNAL_ERROR :
				{
					Fire_OnEventError();
					m_CurrentState = STATE_TASK_IDLE;			
				} break;
			}
		} break;
	}
}