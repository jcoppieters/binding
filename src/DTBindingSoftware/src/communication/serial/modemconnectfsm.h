// ModemConnectFSM.h: interface for the CModemConnectFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODEMCONNECTFSM_H__E6918690_9231_4FD7_B1E2_2E398B4609BD__INCLUDED_)
#define AFX_MODEMCONNECTFSM_H__E6918690_9231_4FD7_B1E2_2E398B4609BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/********************************************************************/
#ifndef INCLUDED_ISERIALMODEMCONNECTFSM_HOST_H
	#include "iserialmodemconnectfsm.h"
#endif 
/********************************************************************/
struct ICommunicationObject_Serial;
struct ICommunicationObject;
/********************************************************************/

/****************************************************************//**
 * @class	CModemConnectFSM
 * @brief	FSM die bij het connecteren handshaking van commandos
 *			doet met de modem.
 *
 * @author	Mitchell Tom
 * @changes	TM,v0562: opvragen van de softwareversie van de modem.
 *
 * @attention
 * sedert versie v0562 wordt de software versie van de modem 
 * opgevraagd. (Major + Minor versie).
 * Dit is noodzakelijk omdat bepaalde fucties enkel mogelijk zijn
 * vanaf een bepaalde software versie.
 *******************************************************************/
class CModemConnectFSM : public ISerialModemConnectFsm
{
protected:

	#define MODEM_MAX_CONNECT_RETRY_COUNT		3
	#define MODEM_ABSORBE_COUNT					2

	typedef enum
	{	
		FSM_EV_SIGNAL_ACK = 0,
		FSM_EV_SIGNAL_RXMESSAGE,
		FSM_EV_SIGNAL_MODEMSTATUS,
		FSM_EV_SIGNAL_MODEMADDRESS,
		FSM_EV_SIGNAL_MODEMVERSION,				/** @since v0562 */
		FSM_EV_AFTER_TIMEOUT,
		FSM_EV_OPERATION_OPEN
	
	}ENUM_FSM_EVENTS;

	typedef enum
	{	
		FSM_STATE_CLOSED			= 0,
		FSM_STATE_WAIT_FOR_ACK,
		FSM_STATE_RX_MESSAGES,
		FSM_STATE_WAIT_FOR_STATUS,
		FSM_STATE_WAIT_FOR_ADDRESS,
		FSM_STATE_WAIT_FOR_SOFTWARE_VERSION,	/** @since v0562 */
		FSM_STATE_WAIT_FOR_RESET

	} ENUM_FSM_STATES;

	void FSM(ENUM_FSM_EVENTS e);	
	void StartTimer();
	void StartAckAbsorbeTimer();
	void StopTimer();
	void Fire_DoneOk();
	void Fire_DoneError();

public:
	CModemConnectFSM(ICCommunicationConnectFsm_Host& refHost,ICommunicationObject* pCommObject);
	virtual ~CModemConnectFSM();	
	void EvOperationConnect(); 
	void EvSignalAckReceived();
	void EvAfterTimeout();
	void EvSignalMessageReceived();
	void EvSignalModemStatus();
	void EvSignalModemAddress();
	void EvSignalSoftwareVersion( );

private:	
	ICCommunicationConnectFsm_Host& m_refHost;
	ICommunicationObject_Serial* m_pSerialComObject;		
	ENUM_FSM_STATES m_CurrentState;				
	int m_nRetryCnt;
};
/******************************* inline functies ***************************************/

inline void CModemConnectFSM::EvSignalAckReceived()
{
	FSM(FSM_EV_SIGNAL_ACK);
}

inline void CModemConnectFSM::EvAfterTimeout()
{
	FSM(FSM_EV_AFTER_TIMEOUT);
}

inline void CModemConnectFSM::EvSignalMessageReceived()
{
	FSM(FSM_EV_SIGNAL_RXMESSAGE);
}

inline void CModemConnectFSM::EvSignalModemStatus() 
{
	FSM(FSM_EV_SIGNAL_MODEMSTATUS);
}

inline void CModemConnectFSM::EvSignalModemAddress() 
{
	FSM(FSM_EV_SIGNAL_MODEMADDRESS);
}

inline void CModemConnectFSM::EvSignalSoftwareVersion( )
{
	FSM(FSM_EV_SIGNAL_MODEMVERSION);
}
/*****************************************************************************************/

#endif // !defined(AFX_MODEMCONNECTFSM_H__E6918690_9231_4FD7_B1E2_2E398B4609BD__INCLUDED_)
