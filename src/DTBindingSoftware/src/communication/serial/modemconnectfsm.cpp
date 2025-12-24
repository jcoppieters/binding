// ModemConnectFSM.cpp: implementation of the CModemConnectFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ModemConnectFSM.h"
#include "modemframe_def.h"
#include "mywindowtimers.h"
#include "iserialcommobject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModemConnectFSM::CModemConnectFSM(ICCommunicationConnectFsm_Host& refHost,ICommunicationObject* pCommObject) :
	m_refHost(refHost), m_pSerialComObject(NULL), m_CurrentState(FSM_STATE_CLOSED),
	m_nRetryCnt(0) 
{ 	
	ASSERT(NULL != pCommObject);

	m_pSerialComObject = dynamic_cast<ICommunicationObject_Serial*>(pCommObject);
	ASSERT(NULL != m_pSerialComObject);
}

CModemConnectFSM::~CModemConnectFSM() { }


void CModemConnectFSM::FSM(ENUM_FSM_EVENTS e)
{
	switch (m_CurrentState)
	{		
		case FSM_STATE_CLOSED:
		{
			switch(e)
			{	
				case FSM_EV_OPERATION_OPEN:
				{
					// CHANGES_COMPORT_CONNECT
					// Mogen hier de StartTimer() niet meer aanroepen, wachten tot SetConnect()
					// aangeroepen werd.

					// StartTimer();
					try
					{
						if (m_pSerialComObject != NULL)
						{
							m_pSerialComObject->OpenConnection();

							// CHANGES_COMPORT_CONNECT:
							// Sedert versie 0x0800: ASSERT Failure !!
							// Moeten hier controleren dat de Com port kon geopend worden.
							// Anders moeten niet verder gaan naar een andere toestand.
							// ASSERT(TRUE==m_pSerialComObject->IsConnectionOpen());

							m_CurrentState = FSM_STATE_WAIT_FOR_ACK;

							// CHANGES_COMPORT_CONNECT:
							// Moeten hier de StartTimer aanroepen omdat in sommige gevallen de 
							// timer expired wanneer nog niet van toestand veranderd zijn er dan
							// geen connect retries zijn.
							StartTimer();
						}
					}

					catch(CException *e)
					{
						const CString s("Connect to modem Failed, Communication port is possible in use");
						m_refHost.displayMsgError(s);					

						e->Delete();
						Fire_DoneError();
					}
				} break;

				case FSM_EV_AFTER_TIMEOUT:
				{
					ASSERT(0);
				} break;

				default:
				{					
				} break;
			}
		} break;

		case FSM_STATE_WAIT_FOR_ACK:
		{
			switch(e)
			{					
				case FSM_EV_SIGNAL_ACK :
				{
					TRACE("\nFSM_STATE_WAIT_FOR_ACK - FSM_EV_SIGNAL_ACK");

					const CString s("Acknowledge received from Modem");
					m_refHost.displayMsgInfo(s);				

					m_nRetryCnt = 0;
					StopTimer();					
					m_CurrentState = FSM_STATE_RX_MESSAGES;
					StartTimer();
						
				} break;					
				
				case FSM_EV_AFTER_TIMEOUT:
				{
					// No Acknowledge received !!!				
					TRACE("\nFSM_STATE_WAIT_FOR_ACK - FSM_EV_AFTER_TIMEOUT");

					/** TM 20061004 : Retransmit Pininstall Connect */
 					if (m_nRetryCnt < MODEM_MAX_CONNECT_RETRY_COUNT)
					{					
						if (m_pSerialComObject != NULL) {
							if (m_pSerialComObject->IsConnectionOpen() == FALSE) {
								m_pSerialComObject->OpenConnection();
							}						
						}

						StartTimer();								
					}
					else if (m_nRetryCnt == MODEM_MAX_CONNECT_RETRY_COUNT)
					{
						const CString s("Reset Communication ...");
						m_refHost.displayMsgInfo(s);
			
						if (m_pSerialComObject != NULL)		
							m_pSerialComObject->ResetModem();

						StartTimer();					
						m_CurrentState = FSM_STATE_WAIT_FOR_RESET;
					}
					else
					{
						const CString s("Failed to Connect to modem ...");
						m_refHost.displayMsgInfo(s);
					
						try
						{
							if (m_pSerialComObject != NULL)							
								m_pSerialComObject->CloseConnection();							
						}
						catch(CException *e)
						{
							e->Delete();
						}

						Fire_DoneError();
					}

					m_nRetryCnt++;

				} break;

				case FSM_EV_SIGNAL_MODEMSTATUS:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_ACK - FSM_EV_SIGNAL_MODEMSTATUS");

				} break;
				
				case FSM_EV_SIGNAL_RXMESSAGE:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_ACK - FSM_EV_SIGNAL_RXMESSAGE");

				} break;

				case FSM_EV_SIGNAL_MODEMADDRESS:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_ACK - FSM_EV_SIGNAL_MODEMADDRESS");

				}break;
			}
		} break;
		
		case FSM_STATE_RX_MESSAGES :
		{
			switch(e)
			{						
				case FSM_EV_SIGNAL_ACK:
				case FSM_EV_SIGNAL_RXMESSAGE:
				{
					TRACE("\nFSM_STATE_RX_MESSAGES - FSM_EV_SIGNAL_RXMESSAGE + ACK");
									
					StopTimer();			
					StartAckAbsorbeTimer();

				} break;

				case FSM_EV_SIGNAL_MODEMADDRESS:
				{
					TRACE("\nFSM_STATE_RX_MESSAGES - FSM_EV_SIGNAL_MODEMADDRESS");

				}break;

				case FSM_EV_AFTER_TIMEOUT:
				{
					TRACE("\nFSM_STATE_RX_MESSAGES - FSM_EV_AFTER_TIMEOUT");

					StopTimer();							
					m_nRetryCnt = 0;				
					//m_pComApplication->RequestModemStatus();

					if (m_pSerialComObject != NULL)
						m_pSerialComObject->MessageTransfer(PININSTALL_MODEMSTATUS);
					m_CurrentState = FSM_STATE_WAIT_FOR_STATUS;		
					StartTimer();
					
				} break;
			}
		} break;

		case FSM_STATE_WAIT_FOR_STATUS:
		{
			switch(e)
			{						
				case FSM_EV_SIGNAL_MODEMSTATUS :
				{										
					TRACE("\nFSM_STATE_WAIT_FOR_STATUS - FSM_EV_SIGNAL_MODEMSTATUS");

					StopTimer();
					//m_pComApplication->RequestModemAddress();
					if (m_pSerialComObject != NULL)
						m_pSerialComObject->MessageTransfer(PININSTALL_ADDRESS);
					m_CurrentState = FSM_STATE_WAIT_FOR_ADDRESS;							
					StartTimer();					

				} break;

				case FSM_EV_SIGNAL_MODEMADDRESS:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_STATUS - FSM_EV_SIGNAL_MODEMADDRESS");

				}break;

				case FSM_EV_AFTER_TIMEOUT:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_STATUS - FSM_EV_AFTER_TIMEOUT");

					StopTimer();

					if (m_nRetryCnt++ < MODEM_MAX_CONNECT_RETRY_COUNT)
					{
						if (m_pSerialComObject != NULL)
							m_pSerialComObject->MessageTransfer(PININSTALL_MODEMSTATUS);
						StartTimer();
					}
					else
					{	
						const CString s("No Modem status relpy received");
						m_refHost.displayMsgError(s);				

						try
						{
							if (m_pSerialComObject != NULL)							
								m_pSerialComObject->CloseConnection();							
						}
						catch(CException *e)
						{
							e->Delete();
						}

						Fire_DoneError();
					}

				} break;

				case FSM_EV_SIGNAL_ACK :
				case FSM_EV_SIGNAL_RXMESSAGE:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_STATUS - FSM_EV_AFTER_RXMESSAGE + ACK");

					StopTimer();					
					m_nRetryCnt = 0;
					StartAckAbsorbeTimer();
				} break;
			}
		} break;

		case FSM_STATE_WAIT_FOR_ADDRESS:
		{
			switch(e)
			{						
				case FSM_EV_SIGNAL_MODEMADDRESS :
				{
					StopTimer();
					m_nRetryCnt = 0; 
					if (m_pSerialComObject != NULL)
						m_pSerialComObject->MessageTransfer(PININSTALL_SOFTWARE_VERSION);
					m_CurrentState = FSM_STATE_WAIT_FOR_SOFTWARE_VERSION;							
					StartTimer();	

				} break;

				case FSM_EV_AFTER_TIMEOUT:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_STATUS - FSM_EV_AFTER_TIMEOUT");

					StopTimer();

					if (m_nRetryCnt++ < MODEM_MAX_CONNECT_RETRY_COUNT)
					{
						//m_pComApplication->RequestModemAddress();

					if (m_pSerialComObject != NULL)
						m_pSerialComObject->MessageTransfer(PININSTALL_ADDRESS);
						StartTimer();
					}
					else
					{	
						const CString s("No Modem status relpy received");
						m_refHost.displayMsgError(s);
					
						try
						{
							if (m_pSerialComObject != NULL)							
								m_pSerialComObject->CloseConnection();
							
						}
						catch(CException *e)
						{
							e->Delete();
						}

						Fire_DoneError();
					}

				} break;

				case FSM_EV_SIGNAL_ACK :
				case FSM_EV_SIGNAL_RXMESSAGE:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_STATUS - FSM_EV_AFTER_RXMESSAGE + ACK");

					StopTimer();					
					m_nRetryCnt = 0;
					StartAckAbsorbeTimer();
				} break;
			}
		} break;

		case FSM_STATE_WAIT_FOR_SOFTWARE_VERSION:			/** @since v0562 */
		{
			switch (e)
			{
				case FSM_EV_SIGNAL_MODEMVERSION:
				{
					m_nRetryCnt = 0; 
					StopTimer();
					Fire_DoneOk();

				} break;

				case FSM_EV_AFTER_TIMEOUT:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_STATUS - FSM_EV_AFTER_TIMEOUT");

					StopTimer();

					if (++m_nRetryCnt < MODEM_MAX_CONNECT_RETRY_COUNT)
					{
						//m_pComApplication->RequestModemVersion();

						if (m_pSerialComObject != NULL)
							m_pSerialComObject->MessageTransfer(PININSTALL_SOFTWARE_VERSION);
						StartTimer();
					}
					else
					{	
						m_nRetryCnt = 0; 
						StopTimer();
						Fire_DoneOk();
					}
				} break;

				case FSM_EV_SIGNAL_ACK :
				case FSM_EV_SIGNAL_RXMESSAGE:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_SOFTWARE_VERSION - FSM_EV_AFTER_RXMESSAGE + ACK");

					StopTimer();					
					m_nRetryCnt = 0;
					StartAckAbsorbeTimer();

				} break;
			}
		} break;

		case FSM_STATE_WAIT_FOR_RESET:
		{
			switch(e)
			{						
				case FSM_EV_AFTER_TIMEOUT:
				{
					TRACE("\nFSM_STATE_WAIT_FOR_RESET - FSM_EV_AFTER_TIMEOUT");

					StopTimer();
			
					if (m_pSerialComObject != NULL)						
						m_pSerialComObject->Send_PininstallConnect();

					m_CurrentState = FSM_STATE_WAIT_FOR_ACK;

					StartTimer();

				} break;

				case FSM_EV_SIGNAL_ACK :
				case FSM_EV_SIGNAL_RXMESSAGE:
				case FSM_EV_SIGNAL_MODEMSTATUS :
				{
					TRACE("\nFSM_STATE_WAIT_FOR_RESET - DEFAULT");

				} break;
			}
		} break;
	
		default:
		{			
		} break;
	}
}


void CModemConnectFSM::EvOperationConnect() {
	FSM(FSM_EV_OPERATION_OPEN);
}
inline void CModemConnectFSM::StartTimer() {
	m_refHost.startTimer( TIMERID_MODEMCONNECT_TIMEOUT, TIMER_MODEMCONNECT_TIMEOUT_VALUE);	
}
inline void CModemConnectFSM::StartAckAbsorbeTimer() {
	m_refHost.startTimer( TIMERID_MODEMCONNECT_TIMEOUT,TIMER_MODEMCONNECT_ACKABSORBE_VALUE);
}
inline void CModemConnectFSM::StopTimer() {	
	m_refHost.stopTimer( TIMERID_MODEMCONNECT_TIMEOUT );	
}
inline void CModemConnectFSM::Fire_DoneOk() {	
	m_refHost.notifyFsmDone();	
}
inline void CModemConnectFSM::Fire_DoneError() {
	m_refHost.notifyFsmError();	
}