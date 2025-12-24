// SerialCommObject.h: interface for the CSerialCommObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALCOMMOBJECT_H__31536D58_354B_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_SERIALCOMMOBJECT_H__31536D58_354B_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*****************************************************************/
//#include "base/icommunicationobject.h"		// tijdelijk - interface...
#include "serialcommobject_def.h"
#include "iserialcommobject.h"
/*****************************************************************/
#include "TXMessageQueue.h"
#include "RXMessageQueue.h"
#include "ModemFrame.h"
/*****************************************************************/
#if defined(USE_MSCOMMCONTROL)				// TM 20080128
	#include "MsComm.h"
#endif
#if !defined(USE_MSCOMMCONTROL)				// TM 20080128
	#include "RS232\ComPort.h"	
#endif
/*****************************************************************/
// V24 frame receive
#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15

// ActiveX communication ID for data exchange...
#define ID_COMMCTRL 2001  // Ctrl ID for comm control

#define MAX_RETRY	15
#define TRAN(x) bTxCurrentState = (x)

#define RX_BUFFER_LENGTH	512
#define TX_BUFFER_LENGTH	255	
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSerialCommObject : public ICommunicationObject_Serial
{
private:

	enum TxState 
	{
		Idle=0,
		WaitForAck,
		WaitForRetransmit,
		WaitForNextSend,
		MaxState

	};

	enum TxEvents 
	{
		EV_ACK=0,
		EV_NAK,
		EV_ACK_TIMEOUT,
		EV_REQUEST_FOR_SEND,
		EV_RETRANSMIT,
		EV_DISCONNECT,
		EV_RESET_MODEM					/* 20060328 */
	//	EV_COMMAND_REQUEST_FOR_SEND		/* 20060328 */
	};

private:
	ISerialCommObject_Host& m_parent;
	TxState bTxCurrentState;
	BYTE bCommACKRetries;
	BOOL fConnected;	 

#ifdef _DEBUG	
public:
#endif
	void SendAck(void);
	void SendNack(void);

public:
	#if defined(USE_MSCOMMCONTROL)	
		CMSComm* GetCommCtrl()	{	return &m_commctrl; }
	#endif

	#if !defined(USE_MSCOMMCONTROL)
		DUOTECNO::RS232::CComPort& get_Comport() { return m_comport; };
	#endif

public:
	CSerialCommObject(ISerialCommObject_Host& parent); // ,IComApplication* pCom);
	virtual ~CSerialCommObject();

public:
	virtual void Init(HWND hParent = 0);
	virtual BOOL IsConnectionOpen(void);	
	// virtual BOOL SetConnected(BOOL Connected);

	virtual BOOL OpenConnection();
	virtual BOOL CloseConnection();
	virtual BYTE MessageTransfer(BYTE cmd);
	virtual BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
								 BYTE bLength=0,const CString& szData ='\0');

	virtual const ICommunicationFrame* GetRxModemFrame() const {
		return (&pRXMsgQueueFrame);
	}
	virtual BOOL GetRXMsgFromQueue(void);
	virtual void ParseRXMessage(void);

	virtual void EvAfterAckTimeout(void) { 
		TxStateMachine(EV_ACK_TIMEOUT); 
	}
	virtual void EvAfterRetryTimeout(void) { 
		TxStateMachine(EV_RETRANSMIT); 
	}

	virtual void SetComPort(short Port);
	virtual short GetComPort(void);
	virtual void ResetModem();

	virtual BOOL Send_PininstallConnect();
	virtual BOOL Send_PininstallDisconnect();	
	virtual BOOL Send_CanLoggerEnable(BOOL flag);

public:		
	BYTE DispatchMsg(const CString& CStringMsg);

protected:
	#if defined(USE_MSCOMMCONTROL)
		CMSComm m_commctrl;	
	#endif
	#if !defined(USE_MSCOMMCONTROL)
		DUOTECNO::RS232::CComPort m_comport;
	#endif

	CModemFrame* const pRXModemFrame;
	BYTE cReceiveBuffer[RX_BUFFER_LENGTH];				// Receive buffer
    unsigned char bReceiveBufferIndex;	
	CRXMessageQueue* const pRxQueue;
	CModemFrame pRXMsgQueueFrame;	
	CTXMessageQueue* const pTxQueue;
	CModemFrame* const pTXModemFrame;

protected:
	void SetConnect();
	void SetDisconnect();
	void Parse(CString &str);
	inline void Fire_OnMessageSend(CModemFrame* pTxModemFrame);
	inline void Fire_OnReceivedAck(void);
	inline void Fire_OnReceivedNack(void);
	inline void Fire_OnReceivedMessage(void);
	inline void Fire_OnReceivedModemCommand(void);
	inline void Fire_OnReceivedBadChecksum(void);
	inline void Fire_OnCommBuffersFull(void);
	inline void Fire_OnNoResponse(void);
	inline void Fire_OnMaxRetries(void);
	void StartAckTimer(void);
	void StopAckTimer(void);
	void StartRetryTimer(void);
	void StopRetryTimer(void);
	void TxStateMachine(TxEvents Event);
	void SendMessage(const CString& CStringMsg);
};

#endif // !defined(AFX_SERIALCOMMOBJECT_H__31536D58_354B_11D8_B865_0050BAC412B1__INCLUDED_)
