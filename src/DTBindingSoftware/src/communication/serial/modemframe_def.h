#ifndef INCLUDED_SERIAL_MODEMFRAME_DEF_H
#define INCLUDED_SERIAL_MODEMFRAME_DEF_H
/*****************************************************************/
// Commands & Events       
#define PININSTALL_WRITE				0x00
#define PININSTALL_READ					0x01
#define PININSTALL_MODEM_RESET			0x02
#define PININSTALL_SETNICMODE			0x03
#define MODEM_RXMESSAGE					0x04
#define MODEM_RX_BUFFEREMPTY			0x05
#define PININSTALL_READY				0x06
#define PININSTALL_SHUTDOWN				0x07
#define PININSTALL_READTXBUFFERITEMS	0x08
#define PININSTALL_READRXBUFFERITEMS	0x09
#define MODEM_RXBUFFERITEMS				0x0A
#define MODEM_TXBUFFERITEMS				0x0B
#define PININSTALL_SETMODEMTXMODE		0x0C
#define MODEM_TX_PL_DLL_ERROR			0x0D
#define PININSTALL_MODEMSTATUS			0x0E
#define MODEM_STATUS					0x0F
#define PININSTALL_DLLSTATUS0			0x10
#define MODEM_DLL_STATUS0				0x11
#define PININSTALL_DLLSTATUS1			0x12
#define MODEM_DLL_STATUS1				0x13
#define PININSTALL_ADDRESS				0x14
#define MODEM_ADDRESS					0x15
#define PININSTALL_RESETCAN				0x16
#define MODEM_SOFTWARE_VERSION			0x17
#define PININSTALL_SOFTWARE_VERSION	    0x18		// since 0x0562
#define PININSTALL_CANLOG_ENABLE	    0x19		// Sedert V16.20
#define PININSTALL_CANLOG_READSTATUS    0x1A		// Sedert V16.20
#define MODEM_CANLOG_STATUS			    0x1B		// Sedert V16.20
#define MODEM_RXMESSAGE_CANLOG		    0x1C		// Sedert V16.20
/*****************************************************************/
#endif 
/*****************************************************************/