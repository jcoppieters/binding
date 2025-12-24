#ifndef INCLUDED_COMMUNICATION_STATUS_H
#define INCLUDED_COMMUNICATION_STATUS_H
/*****************************************************************/
#include "serial/serialcanmodem.h"

#ifndef INCLUDED_COMMUNICATION_SERVERINFO_H
	#include "base/communicationserverinfo.h"
#endif
/*****************************************************************/
struct ICommunicationFrame;
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CCommunicationAppFrame
{
public:
	enum Types
	{
		_UNKNOWN = -1,
		_COMMAND_RECEIVED_ACK		= 0,
		_COMMAND_RECEIVED_NAK		= 1,
		_MESSAGE_SEND				= 2,
		_MESSAGE_RECEIVED			= 3,
		_COMMAND_SEND				= 4,
		_COMMAND_RECEIVED			= 5,
		_MESSAGE_LOG				= 6			// Sedert V20.16			
	};

	CCommunicationAppFrame();
	CCommunicationAppFrame(Types type,const ICommunicationFrame* const ptr = NULL);

	Types GetType() const;
	const ICommunicationFrame* GetModemFrame() const;

	const CString toString() const;

protected:
	const CString msg2String() const;
	const CString logmsg2String() const;		// sedert V16.20
	const CString cmd2String() const;	

	// Logger - Sedert V16.20
	enum {
		CAN_FRAME_FREE                = 0x00,
		CAN_FRAME_HEADER              = 0x08,
		CAN_FRAME_MESSAGE             = 0x28,
		CAN_FRAME_BROADCAST_ALL       = 0x48,
		CAN_FRAME_BROADCAST_LIM       = 0x68,
		CAN_FRAME_BROADCAST_PNP       = 0x88,
		CAN_FRAME_NO_FREE_MTC         = 0x2A,
		CAN_FRAME_MESSAGE_ACK         = 0xC8,
		CAN_FRAME_STREAM              = 0xCB,
		CAN_FRAME_SRR_BIT             = 0xEF
	};

	enum {
		FRAME_IDX_LENGTH = 0,
		FRAME_IDX_MODEMCOMMAND = 1,
		FRAME_IDX_SIDH = 2,
		FRAME_IDX_SIDL = 3,
		FRAME_IDX_EIDH = 4,
		FRAME_IDX_EIDL = 5,
		FRAME_IDX_DATA = 6,
	};

	const CString formatMsgLog( ) const;
	void formatMsgLogAddressOriented(int t) const;	// helper
	void formatMsgLogBroadcast(int t) const;			// helper
	void formatMsgLogHeader( ) const;
	void formatMsgLogMessage( ) const;
	void formatMsgLogMACK( ) const;
	void formatMsgLogBroadcastAll( ) const;
	void formatMsgLogBroadcastLimit( ) const;
	void formatMsgLogBroadcastPNP( ) const;
	void formatMsgLogNoFreeMTC( ) const;
	void formatMsgLogUnknown( ) const;

private:	
	Types m_type;
	const ICommunicationFrame* m_ptrToModemFrame;
	mutable char m_szBufferText[512];			// sedert V16.20
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CCommunicationStatus  
{
public:
	CCommunicationStatus();
	virtual ~CCommunicationStatus();

	BOOL connected() const;
	void setConnected(BOOL flag);

	BOOL canLoggerEnabled() const;
	void setCanLoggerEnabled(BOOL stateOnOff);

	CSerialCanModem* GetModemInfo() {
		return (&m_CanModem);
	}	
	const CSerialCanModem* GetModemInfo() const {
		return (&m_CanModem);
	}
	CCommunicationServerInfo& getServerInfo() {
		return m_serverInfo;
	}
	const CCommunicationServerInfo& getServerInfo() const {
		return m_serverInfo;
	}

	const CCommunicationAppFrame& getLastTxFrame() const;
	void setLastTxFrame(const CCommunicationAppFrame& frame);

	const CCommunicationAppFrame& getLastRxFrame() const;
	void setLastRxFrame(const CCommunicationAppFrame& frame);

private:
	BOOL m_fConnected;
	BOOL m_fCanLoggerEnabled;
	CSerialCanModem	m_CanModem;
	CCommunicationServerInfo m_serverInfo;
	CCommunicationAppFrame m_lastTxFrame;
	CCommunicationAppFrame m_lastRxFrame;
};
/*****************************************************************/
#endif
/*****************************************************************/