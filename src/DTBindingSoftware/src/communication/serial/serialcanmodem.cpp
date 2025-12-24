#include "stdafx.h"
#include "serialcanmodem.h"
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSerialCanModemErrorStatus::CSerialCanModemErrorStatus() { }
CSerialCanModemErrorStatus::~CSerialCanModemErrorStatus() { }

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CSerialCanModem::CSerialCanModem() :
// m_SWRevision( 0xFF ), m_SWVersion( 0xFF ), m_bAddress( 0xFF ),
m_ModemStatus( MODEM_STATUS_UNDEFINED ), m_bV24CurrentStatus( MODEM_STATE_V24UNDEFINED ),
m_errorStatus( ) /* m_connectStatus() */ { }

CSerialCanModem::~CSerialCanModem() { }

void CSerialCanModem::GetStatus(CString &s) const
{
	switch (m_ModemStatus)
	{
		case MODEM_STATUS_IDLE:
		{
			s = CString("Modem status OK!");
		} break;

		case MODEM_STATUS_BUFFER_OVERRUN :
		{
			s = CString("Buffer overrun");
		} break;

		case MODEM_STATUS_PENDING_RESET :
		{
			s = CString("Reset pending");
		} break;

		case MODEM_STATUS_BUFFER_OVERRUN_RESET :
		{
			s = CString("Buffer overrun + Reset pending");
		} break;

		default:
		case MODEM_STATUS_UNDEFINED :
		{
			s = CString("Undefined status");

		} break;	
	}
}

void CSerialCanModem::GetV24Status(CString &s) const
{
	switch (m_bV24CurrentStatus)
	{
		case MODEM_STATE_V24IDLE:
		{			
			s = CString("Modem is idle");

		} break;

		case MODEM_STATE_V24SOH:
		{
			s = CString("RS232 RX Channels OK");
		} break;

		case MODEM_STATE_V24EOT:
		case MODEM_STATE_V24CHECKSUM:
		case MODEM_STATE_V24DATA2:
		case MODEM_STATE_V24DATA1:
		case MODEM_STATE_V24LENGTH:
		{
			s = CString("Busy parsing frame");

		} break;
			
		case MODEM_STATE_V24SEGMENTEDDATA1:
		{
			s = CString("Parsing Segmented data1");
			
		} break;

		case MODEM_STATE_V24SEGMENTEDDATA2:
		{
			s = CString("Parsing Segmented data2");	
		} break;

		case MODEM_STATE_V24SEGMENTEDCHECKSUM:
		{
			s = CString("Parsing Segmented CRC");	
		} break;

		case MODEM_STATE_V24SEGMENTEDEOT:
		{
			s = CString("Parsing Segmented EOT");	
		} break;

		default:
		case MODEM_STATE_V24UNDEFINED:
		{
			s = CString("Undefined state");		
		} break;
	}
}
