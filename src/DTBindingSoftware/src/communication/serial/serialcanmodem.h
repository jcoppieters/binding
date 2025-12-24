#ifndef INCLUDED_SERIALCANMODEM_H
#define INCLUDED_SERIALCANMODEM_H
/*****************************************************************/

/*
class CSerialCanModemConnectStatus  
{
public:
	CSerialCanModemConnectStatus();
	virtual ~CSerialCanModemConnectStatus();

	BOOL GetOnline() const {
		return m_fOnline;
	}
	void SetOnline(BOOL fOnline){
		m_fOnline = fOnline;
	}

	int GetComport() const {
		return m_nComport;
	}
	void SetComport(int comport) {	
		m_nComport = comport;
	}

private:
	BOOL m_fOnline;
	int m_nComport;
};
*/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSerialCanModemErrorStatus  
{
public:
	typedef enum
	{
		MODEM_ERROR_MAX_ACK_RETRIES			= 0,
		MODEM_ERROR_BUFFERS_FULL			= 1,
		MODEM_ERROR_NO_RESPONSE				= 2,
		MODEM_ERROR_RECEIVED_BAD_CHECKSUM	= 3

	} ENUM_MODEM_ERROR_TYPE;

	CSerialCanModemErrorStatus();
	virtual ~CSerialCanModemErrorStatus();

public:
	void SetFatalErrorFlag(BOOL flag) {
		m_FatalError = flag;
	}
	BOOL GetFatalErrorFlag() const {
		return m_FatalError;
	}

	void SetType(ENUM_MODEM_ERROR_TYPE type){
		m_Type = type;
	}
	ENUM_MODEM_ERROR_TYPE GetType() const {
		return m_Type;
	}
private:
	BOOL m_FatalError;
	ENUM_MODEM_ERROR_TYPE m_Type;
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Info ivm de serial Can Modem.
class CSerialCanModem  
{
public:

	typedef enum {	
		MODEM_STATUS_IDLE					= 0x00,
		MODEM_STATUS_BUFFER_OVERRUN			= 0x01,
		MODEM_STATUS_PENDING_RESET			= 0x02,
		MODEM_STATUS_BUFFER_OVERRUN_RESET	= 0x03,
		MODEM_STATUS_UNDEFINED				= 0x04
	} ENUM_MODEM_STATUS;

	typedef enum
	{
		MODEM_STATE_V24IDLE					= 0x00,
		MODEM_STATE_V24SOH					= 0x01,
		MODEM_STATE_V24LENGTH				= 0x02,
		MODEM_STATE_V24DATA1				= 0x03,
		MODEM_STATE_V24DATA2				= 0x04,
		MODEM_STATE_V24CHECKSUM				= 0x05,
		MODEM_STATE_V24EOT					= 0x06,
		MODEM_STATE_V24SEGMENTEDDATA1		= 0x07,
		MODEM_STATE_V24SEGMENTEDDATA2		= 0x08,
		MODEM_STATE_V24SEGMENTEDCHECKSUM	= 0x09,
		MODEM_STATE_V24SEGMENTEDEOT			= 0x0a,
		MODEM_STATE_V24UNDEFINED			= 0x0b
	} ENUM_MODEM_V24STATE;

	CSerialCanModem();
	virtual ~CSerialCanModem();
	
	void SetStatus(ENUM_MODEM_STATUS status) {
		m_ModemStatus = status;
	}
	ENUM_MODEM_STATUS GetStatus() const {
		return m_ModemStatus;
	}

	void SetStatus(BYTE bStatus)
	{
		if (bStatus <= (BYTE) MODEM_STATUS_UNDEFINED){
			m_ModemStatus = (ENUM_MODEM_STATUS) bStatus;
		}
		else{
			m_ModemStatus = MODEM_STATUS_UNDEFINED;
		}
	}

	void SetV24Status(ENUM_MODEM_V24STATE status) {	
		m_bV24CurrentStatus = status;
	}

	ENUM_MODEM_V24STATE GetV24Status() const {	
		return m_bV24CurrentStatus;
	}

	void SetV24Status(BYTE bStatus) {	
		if (bStatus <= (BYTE) MODEM_STATE_V24UNDEFINED) {		
			m_bV24CurrentStatus = (ENUM_MODEM_V24STATE) bStatus;
		}
		else {
			m_bV24CurrentStatus = MODEM_STATE_V24UNDEFINED;
		}
	}

	void GetStatus(CString &s) const;
	void GetV24Status(CString &s)const;


	// De error toestand.
	CSerialCanModemErrorStatus& getErrorStatus() {
		return m_errorStatus;
	}
	const CSerialCanModemErrorStatus& getErrorStatus() const {
		return m_errorStatus;
	}

protected:
	ENUM_MODEM_STATUS m_ModemStatus;	
	ENUM_MODEM_V24STATE m_bV24CurrentStatus;
	CSerialCanModemErrorStatus m_errorStatus;
};
/*****************************************************************/

/*****************************************************************/
#endif
/*****************************************************************/