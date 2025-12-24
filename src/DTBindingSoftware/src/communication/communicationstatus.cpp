#include "stdafx.h"
#include "communicationstatus.h"
#include "base/icommunicationframe.h"
#include "serial/modemframe_def.h"
/*****************************************************************/

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

static const char* const m_szProtocolMessages[256] =
{
	"",									// 0x00
	"EV_UNITCONTROLTOGGLE",				// 0x01
	"EV_UNITCONTROLPULSTOGGLE",			// 0x02
	"EV_UNITCONTROLPULS",				// 0x03
	"EV_UNITCONTROLSTATUS",				// 0x04
	"EV_UNITDIMSTATUS",					// 0x05
	"EV_UNITSWITCHSTATUS",				// 0x06
	"EV_UNITSENSSTATUS",				// 0x07
	"EV_UNITCONFIG",					// 0x08
	"EV_UNITNAME",						// 0x09
	"EV_NODENAME",						// 0x0a
	"EV_BINDINGRESPONSE",				// 0x0b
	"EV_NODECONFIG",					// 0x0c
	"EV_NODERESOURCES",					// 0x0d
	"EV_NODESTATUS",					// 0x0e
	"EV_NODEPNPRESPONSE",				// 0x0f
	"EV_NODEBINDINGENTRY",				// 0x10
	"EV_MESSAGEERROR",					// 0x11
	"EV_INIT",							// 0x12
	"EV_NODEBINDINGSFILECONTROL",		// 0x13
	"EV_UNITSELECTOR",					// 0x14
	"EV_NODEEEPROMERASED",				// 0x15
	"EV_NODETEXTSTRING",				// 0x16
	"EV_UNITAUDIOSTATUS",				// 0x17
	"EV_NODEDLLSTATUS",					// 0x18
	"EV_NODELCDSTATUS",					// 0x19
	"EV_UNITDIRECTSET",					// 0x1a
	"EV_NODEDIRECTSET",					// 0x1b
	"EV_SLAVELCDBROWSINGDONE",			// 0x1c
	"EV_NODELCDMASTERSTATUS",			// 0x1d
	"EV_DATABASEMANAGMENT",				// 0x1e
	"EV_NODEIRCODEDATABASECHANGED",		// 0x1f
	"EV_NODEAVIRDEVICESDATABASECHANGED",// 0x20
	"EV_NODEAVIRDEVICESENDIRCODE",		// 0x21
	"EV_UNITRC5IRRECEIVERSTATUS",		// 0x22
	"EV_NODEAVIRMATRIXSTATUS",			// 0x23
	"EV_FTPFILECONTROLDATA",			// 0x24
	"EV_FTPFILECONTROLSTATUS",			// 0x25
	"EV_UNITDUOSWITCHSTATUS",			// 0x26
	"EV_NODEPHYSICALADDRESS",			// 0x27 
	"EV_PNPLOGICALADDRESSASSIGNED",		// 0x28
	"EV_LCDPMDBASEADDNODEENTRY",		// 0x29
	"EV_LCDPMDBASEADDUNITENTRY",		// 0x2A 
	"EV_LCDINFODIALOG",					// 0x2B 
	"EV_IRCODEDATA",					// 0x2C 
	"EV_UNITIRTXSTATUS",				// 0x2D 
	"EV_RC5CODEMATCH",					// 0x2E 
	"EV_IRCODEDATABASEERROR",			// 0x2F 
	"EV_UNITSELECTORSTATUS",			// 0x30
	"EV_UNITVIDEOMUXSTATUS",			// 0x31
	"EV_UNITPS2INTELLIMOUSESTATUS",		// 0x32  
	"EV_UNITSPEECHCOMMSTATUS",			// 0x33
	"EV_LCDBUILDSCREENMAP",				// 0x34
	"EV_EXTERNALCONFIG",				// 0x35
	"EV_UNITAVMATRIXSTATUS",			// 0x36
	"EV_UNITALARMSTATUS",				// 0x37
	"",									// 0x38
	"",									// 0x39
	"EV_UNITENERGYMEASUREMENTSTATUS",	// 0x3A - IMPLEMENT_UNIT_ENERGY
	"",									// 0x3B
	"",									// 0x3C
	"",									// 0x3D
	"",									// 0x3E
	"",									// 0x3F
	"",									// 0x40
	"",									// 0x41
	"",									// 0x42
	"",									// 0x43
	"",									// 0x44
	"",									// 0x45
	"",									// 0x46
	"",									// 0x47
	"",									// 0x48
	"",									// 0x49
	"",									// 0x4A
	"",									// 0x4B
	"",									// 0x4C
	"",									// 0x4D
	"",									// 0x4E
	"",									// 0x4F
	"",									// 0x50
	"",									// 0x51
	"",									// 0x52
	"",									// 0x53
	"",									// 0x54
	"",									// 0x55
	"",									// 0x56
	"",									// 0x57
	"",									// 0x58
	"",									// 0x59
	"",									// 0x5A
	"",									// 0x5B
	"",									// 0x5C
	"",									// 0x5D
	"",									// 0x5E
	"",									// 0x5F
	"",									// 0x60
	"",									// 0x61
	"",									// 0x62
	"",									// 0x63
	"",									// 0x64
	"",									// 0x65
	"",									// 0x66
	"",									// 0x67
	"",									// 0x68
	"",									// 0x69
	"",									// 0x6A
	"",									// 0x6B
	"",									// 0x6C
	"",									// 0x6D
	"",									// 0x6E
	"",									// 0x6F
	"",									// 0x70
	"",									// 0x71
	"",									// 0x72
	"",									// 0x73
	"",									// 0x74
	"",									// 0x75
	"",									// 0x76
	"",									// 0x77
	"",									// 0x78
	"",									// 0x79
	"",									// 0x7A
	"EV_NODEGATEWAYSTATUS",				// 0x7B
	"EV_MEMORYDATADUMP",				// 0x7C 
	"EV_WRITEMEMORYDATADONE",			// 0x7D
	"EV_NODEEEPROMREAD",				// 0x7E
	"EV_NODEEEPROMWRITTEN",				// 0x7F
	"FC_NODEMANAGEAVIRDEVICES",			// 0x80
	"FC_NODETEXTSTRING",				// 0x81
	"FC_NODEGETTEXTSTRING",				// 0x82
	"FC_UNITREQUESTDIMSTATUS",			// 0x83
	"FC_UNITREQUESTCONFIG",				// 0x84
	"",									// 0x85
	"FC_UNITREQUESTSWITCHSTATUS",		// 0x86
	"FC_UNITREQUESTCONTROLSTATUS",		// 0x87
	"FC_UNITSENSSET",					// 0x88
	"FC_UNITREQUESTSENSSTATUS",			// 0x89
	"FC_NODEREQUESTCONFIG",				// 0x8a
	"FC_UNITSETNAME",					// 0x8b
	"FC_NODESETNAME",					// 0x8c
	"FC_UNITREQUESTNAME",				// 0x8d
	"FC_NODEREQUESTNAME",				// 0x8e
	"FC_NODEREQUESTSTATUS",				// 0x8f
	"FC_BROWSEFORNODES",				// 0x90
	"FC_BROWSEFORNEWNODES",				// 0x91
	"",									// 0x92
	"FC_UNITCONFIG",					// 0x93
	"FC_NODECONFIG",					// 0x94
	"FC_NODEREQUESTDLLSTATUS",			// 0x95
	"FC_NODESETMASTERLCDNODELA",		// 0x96
	"FC_NODEMASTERLCDSHUTDOWN",			// 0x97
	"FC_NODEBINDINGSFILECONTROL",		// 0x98
	"FC_NODEBINDINGENTRY",				// 0x99
	"FC_UNITRESET",						// 0x9a
	"FC_NODERESET",						// 0x9b
	"FC_NODELCDSHUTDOWN",				// 0x9c
	"FC_NODEGETRESOURCES",				// 0x9d
	"FC_SLAVELCDINITIATEBROWSING",		// 0x9e
	"FC_UNITAUDIOSET",					// 0x9f
	"FC_NODEERASEEEPROM",				// 0xa0
	"FC_NODELCDSET",					// 0xa1 
	"FC_UNITDIMSET",					// 0xa2
	"FC_UNITSWITCHSET",					// 0xa3
	"FC_UNITREQUESTLCDSTATUS",			// 0xa4
	"FC_UNITREQUESTAUDIOSTATUS",		// 0xa5
	"FC_UNITLCDVIRTUALSET",				// 0xa6
	"FC_UNITREQUESTLCDVIRTUALSTATUS",   // 0xa7
	"FC_UNITCONTROLSET",				// 0xa8
	"FC_NODELCDMASTERSET",				// 0xa9
	"FC_NODELCDMASTERTIMEDATE",			// 0xaa
	"FC_NODELCDMASTERREQUESTSTATUS",	// 0xab
	"FC_UNITFEEDBACKCONFIG",			// 0xac
	"FC_UNITIRTXSET",					// 0xad
	"FC_FTPFILECONTROL",				// 0xae
	"FC_REQUESTFTPFILECONTROL",			// 0xaf
	"FC_NODEAVIRCONNECTIONMATRIXSET",	// 0xb0
	"FC_NODEREQUESTAVIRMATRIXSTATUS",	// 0xb1
	"FC_NODEAVIRDEVICESENDIRCODE",		// 0xb2
	"FC_UNITRC5IRRECEIVERSET",			// 0xb3
	"FC_UNITREQUESTRC5IRRECEIVERSTATUS",// 0xb4
	"FC_DATABASEMANAGEMENT",			// 0xb5
	"FC_UNITDUOSWITCHSET",				// 0xb6
	"FC_UNITREQUESTDUOSWITCHSTATUS",	// 0xb7
	"FC_BROWSEFORPHYSICALNODES",		// 0xb8   
	"FC_PNPASSIGNLOGICALADDRESS",       // 0xb9  
	"FC_LCDPMDBASEADDNODEENTRY",		// 0xBA 
	"FC_LCDPMDBASEADDUNITENTRY",		// 0xBB 
	"FC_LCDINFODIALOG",					// 0xBC 
	"FC_PNPPHYSADRACK",					// 0xBD 
	"FC_GETIRCODE",						// 0xBE
	"FC_UNITREQUESTIRTXSTATUS",			// 0xBF 
	"FC_CHECKRC5CODE",					// 0xC0 
	"FC_UNITVIDEOMUXSET",				// 0xC1   
	"FC_UNITREQUESTVIDEOMUXSTATUS",		// 0xC2 
	"FC_UNITPS2INTELLIMOUSESET",		// 0xC3
	"FC_UNITREQUESTPS2INTELLIMOUSESTATUS", // 0xC4
	"FC_UNITSPEECHCOMMSET",				// 0xC5
	"FC_UNITREQUESTSPEECHCOMMSTATUS",	// 0xC6
	"FC_NODEMASTERLCDTAPISET",			// 0xC7
	"FC_LCDBUILDSCREENMAP",				// 0xC8 
	"FC_EXTERNALCONFIG",				// 0xC9 
	"FC_UNITAVMATRIXSET",				// 0xCA 
	"FC_UNITREQUESTAVMATRIXSTATUS",		// 0xCB 
	"FC_UNITALARMSET",					// 0xCC
	"FC_UNITREQUESTALARMSTATUS",		// 0xCD
	"",									// 0xCE
	"",									// 0xCF
	"",									// 0xD0
	"",									// 0xD1
	"",									// 0xD2
	"",									// 0xD3
	"",									// 0xD4
	"",									// 0xD5
	"",									// 0xD6
	"",									// 0xD7
	"",									// 0xD8
	"",									// 0xD9
	"",									// 0xDA
	"",									// 0xDB
	"",									// 0xDC
	"",									// 0xDD
	"",									// 0xDE
	"",									// 0xDF
	"",									// 0xE0
	"",									// 0xE1
	"",									// 0xE2
	"",									// 0xE3
	"",									// 0xE4
	"",									// 0xE5
	"",									// 0xE6
	"",									// 0xE7
	"",									// 0xE8
	"",									// 0xE9
	"",									// 0xEA
	"",									// 0xEB
	"",									// 0xEC
	"",									// 0xED
	"",									// 0xEE
	"",									// 0xEF
	"",									// 0xF0
	"",									// 0xF1
	"",									// 0xF2
	"",									// 0xF3
	"",									// 0xF4
	"",									// 0xF5
	"",									// 0xF6
	"",									// 0xF7		
	"FC_NODEGATEWAYSET",				// 0xF8		
	"FC_NODEGATEWAYREQUESTSTATUS",		// 0xF9	
	"FC_UNLOCKLCD",						// 0xFA 
	"FC_LOCKLCD",						// 0xFB 
	"FC_GETMEMORYDATA",					// 0xFC
	"FC_WRITEMEMORYDATA",				// 0xFD 
	"FC_NODEEEPROMREAD",				// 0xfe
	"FC_NODEEEPROMWRITE"				// 0xff
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Free functie...
static const char* getMsgCode2Str(BYTE index) {
	return (m_szProtocolMessages[index]);
}

/******************************* private **************************************/
static void H2A(BYTE bVal,BYTE *pbAMSN,BYTE *pbALSN)
{
   BYTE bTemp;

   bTemp = bVal/16;
   if ((bTemp == 0)||((bTemp >= 1)&&(bTemp <= 9))) *pbAMSN = bTemp + 48;
   else if ((bTemp >= 10) && (bTemp <= 15)) *pbAMSN = bTemp + 55;

   bTemp = bVal%16;
   if ((bTemp == 0) || ((bTemp >= 1) && (bTemp <= 9))) *pbALSN = bTemp + 48;
   else if ((bTemp >= 10) && (bTemp <= 15)) *pbALSN = bTemp + 55;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationAppFrame::CCommunicationAppFrame() : m_type(_UNKNOWN), m_ptrToModemFrame(NULL) { }

CCommunicationAppFrame::CCommunicationAppFrame(Types type,const ICommunicationFrame* const ptr) : 
	m_type(type), m_ptrToModemFrame(ptr) 
{
	memset(m_szBufferText,0,sizeof(m_szBufferText));

	if (_MESSAGE_SEND  == type || _MESSAGE_RECEIVED	== type ||  _COMMAND_SEND == type || _COMMAND_RECEIVED == type || _MESSAGE_LOG == type) {
		ASSERT(NULL != ptr);
	}
}

CCommunicationAppFrame::Types CCommunicationAppFrame::GetType() const {
		return m_type;
}
const ICommunicationFrame* CCommunicationAppFrame::GetModemFrame() const {
	return m_ptrToModemFrame;
}

const CString CCommunicationAppFrame::toString() const {
	if (m_type == _COMMAND_RECEIVED_ACK) {
		return CString("modem ACK");
	}
	else if (m_type == _COMMAND_RECEIVED_NAK) {
		return CString("modem NAK");
	}
	else if (m_type == _COMMAND_RECEIVED || m_type == _COMMAND_SEND) {
		return cmd2String();
	}
	else if (m_type == _MESSAGE_LOG) {
		return logmsg2String();
	}
	return msg2String();
}

// Sedert V16.30:
// Enkel de eerste 6 databytes dumpen...
const CString CCommunicationAppFrame::msg2String() const
{
	enum { MAX_TRACER_DATALENGTH = 6 };

	if (NULL == m_ptrToModemFrame) {
		ASSERT(NULL != m_ptrToModemFrame);
		return CString();
	}

	CString	szStringMessage;		
	szStringMessage.Format("%02X,%02X -%s (%02X):",
							m_ptrToModemFrame->GetNodeAddress(),
							m_ptrToModemFrame->GetUnitAddress(),
							getMsgCode2Str(m_ptrToModemFrame->GetMsgCode()),
							m_ptrToModemFrame->GetMsgCode());
			
	CString szData;
	const int length = m_ptrToModemFrame->GetMessageLength();
	for (int i=0; i<length; i++) 
	{
		szData.Format("%02X ",m_ptrToModemFrame->GetDataBYTE(i));
		szStringMessage += szData;					

		if (((MAX_TRACER_DATALENGTH-1) == i) && (length>MAX_TRACER_DATALENGTH)) {
			szStringMessage += "...";
			break;
		} 
	}	
	return szStringMessage;
}

const CString CCommunicationAppFrame::cmd2String() const {
	if (NULL == m_ptrToModemFrame) {
		ASSERT(NULL != m_ptrToModemFrame);
		return CString();
	}
	CString	szStringMessage;			
	int length = 0;
	const BYTE bCommand = m_ptrToModemFrame->GetModemCmd();
	if (bCommand == MODEM_STATUS) {
		szStringMessage = CString("MODEM STATUS: ");
		length = 6;
	}
	else if (bCommand == MODEM_ADDRESS) {
		szStringMessage = CString("MODEM ADDRESS: ");
		length = 3;
	}
	else if (bCommand == MODEM_SOFTWARE_VERSION) {
		szStringMessage = CString("MODEM SOFTWARE: ");
		length = 2;
	}
	else if (bCommand == MODEM_CANLOG_STATUS) {
		szStringMessage = CString("MODEM CAN-LOGGER: ");
		length = 1;
	}	
	else {
		szStringMessage.Format("MODEM COMMAND: %02xh",bCommand);
	}

	if (length > 0) {
		CString szData;
		for (BYTE i=0;i<length;i++) {
			szData.Format("%02xh,",m_ptrToModemFrame->GetCmdDataBYTE(i));
			szStringMessage += szData;					
		}
	}
	return szStringMessage;
}

const CString CCommunicationAppFrame::logmsg2String() const {
	if (NULL == m_ptrToModemFrame) {
		ASSERT(NULL != m_ptrToModemFrame);
		return CString();
	}
	const BYTE bCommand = m_ptrToModemFrame->GetModemCmd();
	if (MODEM_RXMESSAGE_CANLOG == bCommand) {
		return formatMsgLog( );
	}	
	return CString( );	
}

const CString CCommunicationAppFrame::formatMsgLog() const
{
	memset(m_szBufferText,0,sizeof(m_szBufferText));
	const int SIDL = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_SIDL);
	const int frameControl = SIDL & CAN_FRAME_SRR_BIT;
	switch(frameControl)
	{
		case CAN_FRAME_HEADER:
			formatMsgLogHeader();
		break;
		case CAN_FRAME_MESSAGE:
			formatMsgLogMessage( );
		break;
		case CAN_FRAME_MESSAGE_ACK:
			formatMsgLogMACK( );
		break;
		case CAN_FRAME_BROADCAST_ALL:
			formatMsgLogBroadcastAll( );
		break;
		case CAN_FRAME_BROADCAST_LIM:
			formatMsgLogBroadcastLimit( );
		break;
		case CAN_FRAME_BROADCAST_PNP:
			formatMsgLogBroadcastPNP( );
		break;
		case CAN_FRAME_NO_FREE_MTC:
			formatMsgLogNoFreeMTC( );
		break;
		default:
			formatMsgLogUnknown( );
		break;
	}
	return CString(m_szBufferText);
}

void CCommunicationAppFrame::formatMsgLogAddressOriented(int t) const
{
	BYTE bMSB,bLSB;
	int i;

	// Source & Destination address
	const int SIDH = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_SIDH);
	H2A(SIDH,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;

	const int EIDH = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_EIDH);
	H2A(EIDH,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;
	m_szBufferText[++t] = 0x20;


	//   -     0      1    2     3      4    5      6
	// [SOH][LENGTH][CMD][SIDH][SIDL][EIDH][EIDL][DATA0]...[DATA7][EOT]
	// LENGTH => CMD + DATA...
	//
	// FrameLength = ASCII Length	
	const int length = ((m_ptrToModemFrame->getFrameIndex(FRAME_IDX_LENGTH)-10)/2);
	for (i=0;i<length;i++)
	{
		const int data = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_DATA + i);
		H2A(data,&bMSB,&bLSB);
		m_szBufferText[++t] = bMSB;
		m_szBufferText[++t] = bLSB;
	}
	m_szBufferText[++t] = '\0';	
}

void CCommunicationAppFrame::formatMsgLogBroadcast(int t) const
{
	BYTE bMSB,bLSB;
	int i;

	// Source address
	const int SIDH = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_SIDH);
	H2A(SIDH,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;
	m_szBufferText[++t] = 0x20;

	const int EIDH = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_EIDH);
	H2A(EIDH,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;


	const int EIDL = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_EIDL);
	H2A(EIDL,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;
	m_szBufferText[++t] = 0x20;

	// data
	//   -     0      1    2     3      4    5      6
	// [SOH][LENGTH][CMD][SIDH][SIDL][EIDH][EIDL][DATA0]...[DATA7][EOT]
	// LENGTH => CMD + DATA...
	//
	// FrameLength = ASCII Length	
	const int length = ((m_ptrToModemFrame->getFrameIndex(FRAME_IDX_LENGTH)-10)/2);
	for (i=0;i<length;i++)
	{
		const int data = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_DATA + i);
		H2A(data,&bMSB,&bLSB);
		m_szBufferText[++t] = bMSB;
		m_szBufferText[++t] = bLSB;
	}
	m_szBufferText[++t] = '\0';
}

void CCommunicationAppFrame::formatMsgLogHeader( ) const
{
	int t=0;

	// info
	m_szBufferText[t] = 'H';
	m_szBufferText[++t] = 'E';
	m_szBufferText[++t] = 0x20;	
	formatMsgLogAddressOriented(t);
}

void CCommunicationAppFrame::formatMsgLogMessage( ) const
{
	int t=0;

	// info
	m_szBufferText[t] = 'M';
	m_szBufferText[++t] = 'E';
	m_szBufferText[++t] = 0x20;
	formatMsgLogAddressOriented(t);
}



void CCommunicationAppFrame::formatMsgLogMACK( ) const
{ 
	int t=0;

	// info
	m_szBufferText[t] = 'M';
	m_szBufferText[++t] = 'A';
	m_szBufferText[++t] = 'C';
	m_szBufferText[++t] = 'K';
	m_szBufferText[++t] = 0x20;
	formatMsgLogAddressOriented(t);

	ASSERT(((m_ptrToModemFrame->getFrameIndex(FRAME_IDX_LENGTH)-10)/2) == 0);
}

void CCommunicationAppFrame::formatMsgLogBroadcastAll( ) const
{
	int t=0;

	m_szBufferText[t] = 'B';
	m_szBufferText[++t] = 'A';
	m_szBufferText[++t] = 0x20;
	formatMsgLogBroadcast(t);
}


void CCommunicationAppFrame::formatMsgLogBroadcastLimit( ) const
{
	int t=0;

	m_szBufferText[t] = 'B';
	m_szBufferText[++t] = 'L';
	m_szBufferText[++t] = 0x20;
	formatMsgLogBroadcast(t);
}

void CCommunicationAppFrame::formatMsgLogBroadcastPNP( ) const
{
	int t=0;

	m_szBufferText[t] = 'B';
	m_szBufferText[++t] = 'P';
	m_szBufferText[++t] = 0x20;
	formatMsgLogBroadcast(t);
}

void CCommunicationAppFrame::formatMsgLogNoFreeMTC() const
{
	int t=0;

	// info
	m_szBufferText[t] = 'N';
	m_szBufferText[++t] = 'O';
	m_szBufferText[++t] = 0x20;
	m_szBufferText[++t] = 'F';
	m_szBufferText[++t] = 'R';
	m_szBufferText[++t] = 'E';
	m_szBufferText[++t] = 'E';
	m_szBufferText[++t] = 0x20;
	m_szBufferText[++t] = 'M';
	m_szBufferText[++t] = 'T';
	m_szBufferText[++t] = 'C';
	m_szBufferText[++t] = 0x20;
	formatMsgLogAddressOriented(t);
}

void CCommunicationAppFrame::formatMsgLogUnknown() const
{
	int t=0;
	BYTE bMSB,bLSB;
	int i;

	m_szBufferText[t] = '?';
	m_szBufferText[++t] = '?';
	m_szBufferText[++t] = 0x20;

	const int SIDH = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_SIDH);
	H2A(SIDH,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;

	const int SIDL = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_SIDL);
	H2A(SIDL,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;

	const int EIDH = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_EIDH);
	H2A(EIDH,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;

	const int EIDL = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_EIDL);
	H2A(EIDL,&bMSB,&bLSB);
	m_szBufferText[++t] = bMSB;
	m_szBufferText[++t] = bLSB;
	m_szBufferText[++t] = 0x20;

	// data
	//   -     0      1    2     3      4    5      6
	// [SOH][LENGTH][CMD][SIDH][SIDL][EIDH][EIDL][DATA0]...[DATA7][EOT]
	// LENGTH => CMD + DATA...
	//
	// FrameLength = ASCII Length	
	const int length = ((m_ptrToModemFrame->getFrameIndex(FRAME_IDX_LENGTH)-10)/2);
	for (i=0;i<length;i++)
	{
		const int data = m_ptrToModemFrame->getFrameIndex(FRAME_IDX_DATA + i);
		H2A(data,&bMSB,&bLSB);
		m_szBufferText[++t] = bMSB;
		m_szBufferText[++t] = bLSB;
	}
	m_szBufferText[++t] = '\0';
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationStatus::CCommunicationStatus() : 
	m_fConnected(FALSE), m_serverInfo(), m_lastTxFrame(), m_lastRxFrame(), m_fCanLoggerEnabled(FALSE) { 
	}
CCommunicationStatus::~CCommunicationStatus() { }

BOOL CCommunicationStatus::connected() const {
	return m_fConnected;
}
void CCommunicationStatus::setConnected(BOOL flag) {
	m_fConnected = flag;
}

BOOL CCommunicationStatus::canLoggerEnabled() const {
	return m_fCanLoggerEnabled;
}
void CCommunicationStatus::setCanLoggerEnabled(BOOL stateOnOff) {
	m_fCanLoggerEnabled = stateOnOff;
}

void CCommunicationStatus::setLastTxFrame(const CCommunicationAppFrame& frame) {
	ASSERT(frame.GetType() == CCommunicationAppFrame::_MESSAGE_SEND || 
		   frame.GetType() == CCommunicationAppFrame::_COMMAND_SEND);
	m_lastTxFrame = frame;
}
const CCommunicationAppFrame& CCommunicationStatus::getLastTxFrame() const {
	ASSERT(m_lastTxFrame.GetType() == CCommunicationAppFrame::_MESSAGE_SEND || m_lastTxFrame.GetType() == CCommunicationAppFrame::_COMMAND_SEND);
	return m_lastTxFrame;
}

const CCommunicationAppFrame& CCommunicationStatus::getLastRxFrame()  const {
	ASSERT(m_lastRxFrame.GetType() == CCommunicationAppFrame::_UNKNOWN ||				// Nodig voor updaten van de GUI
		   m_lastRxFrame.GetType() == CCommunicationAppFrame::_MESSAGE_RECEIVED || 
		   m_lastRxFrame.GetType() == CCommunicationAppFrame::_COMMAND_RECEIVED ||
		   m_lastRxFrame.GetType() == CCommunicationAppFrame::_COMMAND_RECEIVED_ACK ||
		   m_lastRxFrame.GetType() == CCommunicationAppFrame::_COMMAND_RECEIVED_NAK ||
		   m_lastRxFrame.GetType() == CCommunicationAppFrame::_MESSAGE_LOG);
	return m_lastRxFrame;
}
void CCommunicationStatus::setLastRxFrame(const CCommunicationAppFrame& frame) {
	ASSERT(frame.GetType() == CCommunicationAppFrame::_MESSAGE_RECEIVED || 
		   frame.GetType() == CCommunicationAppFrame::_COMMAND_RECEIVED ||
		   frame.GetType() == CCommunicationAppFrame::_COMMAND_RECEIVED_ACK ||
		   frame.GetType() == CCommunicationAppFrame::_COMMAND_RECEIVED_NAK ||
		   frame.GetType() == CCommunicationAppFrame::_MESSAGE_LOG);
	m_lastRxFrame = frame;
}
/*****************************************************************/