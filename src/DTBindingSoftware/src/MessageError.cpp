// MessageError.cpp: implementation of the CMessageError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MessageError.h"

#include "communication/communication.h"
#include "NodeMess.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMessageError::CMessageError(CComApplication* const parent,const ICommunicationFrame* const pModemFrame) :
m_bErrorCode(pModemFrame->GetDataBYTE(0)),
m_bNodeAddress(pModemFrame->GetNodeAddress()),
m_bUnitAddress(pModemFrame->GetUnitAddress())
{
}

CMessageError::~CMessageError()
{

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
const char* CMessageError_MessageError::m_szErrorCodes[MAX_MESSAGEERROR_ERROR_CODES] =
{
	"REFUSED RUNNING STATE",			//MESSAGE_ERROR_REFUSED_RUNNING_STATE
	"INVALID STATE",					//MESSAGE_ERROR_INVALID
	"BUFFER FULL",						//MESSAGE_ERROR_BUFFER_FULL
	"BINDING NOT FOUND",				//MESSAGE_ERROR_BINDING_NOT_FOUND
	"REFERENCE_NOT_FOUND",				//MESSAGE_ERROR_REFERENCE_NOT_FOUND
	"BINDING TYPE INVALID",				//MESSAGE_ERROR_BINDING_TYPE_INVALID
	"MESSAGE TRANSFER TIMEOUT",			//MESSAGE_ERROR_MESSAGE_TRANSFER_TIMEOUT
	"DLL ACK TIMEOUT",					//MESSAGE_ERROR_DLL_ACK_TIMEOUT
	"TRANCEIVER CHIP",					//MESSAGE_ERROR_TRANCEIVER_CHIP
	"UNIT NR INVALID",					//MESSAGE_ERROR_UNIT_NR_INVALID
	"BINDING CONFIG REFUSED",			//MESSAGE_ERROR_BINDING_CONFIG_REFUSED
	"WRONG OBJECT MEMBER",				//MESSAGE_ERROR_WRONG_OBJECT_MEMBER
	"INVALID MESSAGE CODE",				//MESSAGE_ERROR_INVALID_MESSAGE_CODE
	"WRONG DIRECT CALL",				//MESSAGE_ERROR_WRONG_DIRECT_CALL
	"REFUSED WRONG DATA",				//MESSAGE_ERROR_REFUSED_WRONG_DATA
	"REFUSED BINDING STATE",			//MESSAGE_ERROR_REFUSED_BINDING_STATE
	"CAN RX PROTOCOL TIMEOUT",			//MESSAGE_ERROR_CAN_RX_PROTOCOL_TIMEOUT
	"CAN TX PROTOCOL TIMEOUT",			//MESSAGE_ERROR_CAN_TX_PROTOCOL_TIMEOUT
	"ONLY FOR MASTER LCD",				//MESSAGE_ERROR_ONLY_FOR_MASTER_LCD
	"NO FREE MTC MAX RETRIES",			//MESSAGE_ERROR_NO_FREE_MTC_MAX_RETRIES
	"BINDING SYNTAX ERROR",				//MESSAGE_ERROR_BINDING_SYNTAX_ERROR
	"BINDING MAX CONDITIONS",			//MESSAGE_ERROR_BINDING_MAX_CONDITIONS
	"BINDING MAX TIMEROPERATORS",		//MESSAGE_ERROR_BINDING_MAX_TIMEROPERATORS
	"BINDING MAX ENTRIES",				//MESSAGE_ERROR_BINDING_MAX_ENTRIES
	"BINDING CONDITION LENGTH",			//MESSAGE_ERROR_BINDING_CONDITION_LENGTH
	"BINDING MAX REFERENCES",			//MESSAGE_ERROR_BINDING_MAX_REFERENCES
	"BINDING BRACES SYNTAX"				//MESSAGE_ERROR_BINDING_BRACES_SYNTAX 0x1A
};

///////////////////////////////////////////////////////////////////////////////////////////
CMessageError_MessageError::CMessageError_MessageError(CComApplication* const pParent,const ICommunicationFrame* const pModemFrame) :
CMessageError(pParent,pModemFrame)
{
	ASSERT(pModemFrame->GetMsgCode() == EV_MESSAGEERROR);
}

CMessageError_MessageError::~CMessageError_MessageError() { }

CString CMessageError_MessageError::ToString() const
{
	CString szReturn;	
	if (m_bErrorCode < MAX_MESSAGEERROR_ERROR_CODES) {
		szReturn.Format("Recieved EV_MessageError '%s' from 0x%02x,0x%02x",m_szErrorCodes[m_bErrorCode], m_bNodeAddress, m_bUnitAddress);								 
	}
	else {
		szReturn.Format("Recieved EV_MessageError from 0x%02x,0x%02x", m_bNodeAddress, m_bUnitAddress);			
	}
	return szReturn;
}

CString CMessageError_MessageError::ToSmallString() const
{
	CString szReturn;
	if (m_bErrorCode < MAX_MESSAGEERROR_ERROR_CODES) {
		szReturn.Format("RX: %02X,%02X: ERROR: %s", m_bNodeAddress, m_bUnitAddress, m_szErrorCodes[m_bErrorCode]);						     								 
	}
	else {
		szReturn.Format("RX: %02X,%02X: EV_MESSAGEERROR", m_bNodeAddress, m_bUnitAddress);					   
	}
	return szReturn;
}

// Sedert V16.30
CMessageError_MessageError::LogLevel_t CMessageError_MessageError::getLogLevel( ) const {
	LogLevel_t logLevel = LOGLEVEL_UNDEFINED;	
	switch(m_bErrorCode) {	
		// Binding configuratie fout - Altijd weergeven.
		case MESSAGE_ERROR_REFUSED_RUNNING_STATE: 
		case MESSAGE_ERROR_BINDING_SYNTAX_ERROR:
		case MESSAGE_ERROR_BINDING_MAX_CONDITIONS:
		case MESSAGE_ERROR_BINDING_MAX_TIMEROPERATORS:
		case MESSAGE_ERROR_BINDING_MAX_ENTRIES:                              
		case MESSAGE_ERROR_BINDING_CONDITION_LENGTH:
		case MESSAGE_ERROR_BINDING_MAX_REFERENCES:
		case MESSAGE_ERROR_BINDING_BRACES_SYNTAX:
		{			
			logLevel = LOGLEVEL_CONFIG;
		} break;
		
		// Normale werking.
		case MESSAGE_ERROR_INVALID:
		case MESSAGE_ERROR_INVALID_MESSAGE_CODE:
		case MESSAGE_ERROR_BUFFER_FULL: 
		case MESSAGE_ERROR_TRANCEIVER_CHIP:
		case MESSAGE_ERROR_UNIT_NR_INVALID:
		case MESSAGE_ERROR_ONLY_FOR_MASTER_LCD:
		case MESSAGE_ERROR_NO_FREE_MTC_MAX_RETRIES:
		{
			logLevel = LOGLEVEL_NORMAL;
		} break;
				
		// Bericht wordt verstuurd wanneer er geen result ontvangen wordt van de remote node.
		// Probleem in de firmware van de CAN-NIC < V90.29.01		
		// Wanneer het unitAddress != 0xFF dan werd het bericht verzonden door een unit van de DT18-PRO.
		// Deze meldingen niet meer weergeven in de GUI (enkel in de advanced tracer).
		case MESSAGE_ERROR_MESSAGE_TRANSFER_TIMEOUT: {
			if (m_bUnitAddress < 0xff) {				
				logLevel = LOGLEVEL_DEBUG;
			}					
		} break;
		
		// Wanneer het unitAddress != 0xFF dan werd het bericht verzonden door een unit van de DT18-PRO.
		// Deze meldingen niet meer weergeven in de GUI (enkel in de advanced tracer).
		case MESSAGE_ERROR_CAN_TX_PROTOCOL_TIMEOUT: {		
			// if (m_bUnitAddress < 0xff) {				
			//	logLevel = LOGLEVEL_DEBUG;
			// }	
			logLevel = LOGLEVEL_NORMAL;;
		} break;
								
		// Veroorzaakt door probleem in de CAN-NIC V90.21.01.
		// Voorlopig altijd tonen.
		case MESSAGE_ERROR_CAN_RX_PROTOCOL_TIMEOUT: {		
			// logLevel = LOGLEVEL_DEBUG;
			logLevel = LOGLEVEL_NORMAL;;
		} break;
								
		// Onderstaande bindings zijn van de runtime binding werking.
		// Wellicht veroorzaakt door de runtime werking van de units van de DT18-PRP.
		// Deze meldingen niet meer weergeven in de GUI (enkel in de advanced tracer).
		case MESSAGE_ERROR_BINDING_NOT_FOUND:
		case MESSAGE_ERROR_REFERENCE_NOT_FOUND:
		case MESSAGE_ERROR_BINDING_TYPE_INVALID: {
			logLevel = LOGLEVEL_RUNTIME_BINDINGS;
		} break;
					
		case MESSAGE_ERROR_DLL_ACK_TIMEOUT: {
			logLevel = LOGLEVEL_NORMAL;
		} break;		
	}
	return logLevel;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
const char* CMessageError_NodeBindingEntry::m_szErrorCodes[MAX_BINDINGENTRY_ERROR_CODES] =
{
	"Wrong binding entry number",		//WRONG_BINDING_ENTRY_NR		0x80
	"Write error wrong state",			//WRITE_ERROR_WRONG_STATE		0x81
	"Read error wrong state",			//READ_ERROR_WRONG_STATE		0x82 
	"Binding file full",				//BINDING_FILE_FULL				0x83
	"Binding file entry error",			//BINDING_ENTRY_ERROR			0x84
	"Binding file closed",				//BINDING_FILE_CLOSED			0x85
	"Binding file empty",				//BINDING_FILE_EMPTY			0x86
	"Binding file corrupt",				//BINDING_FILE_CORRUPT			0x87
	"Binding config wrong state",		//BINDING_CONFIG_WRONG_STATE	0x88
	"Binding temp buffer full"			//BINDING_TEMP_BUFFER_FULL		0x89		
};

///////////////////////////////////////////////////////////////////////////////////////////
CMessageError_NodeBindingEntry::CMessageError_NodeBindingEntry(CComApplication* const pParent,const ICommunicationFrame* const pModemFrame) :
CMessageError(pParent,pModemFrame)
{
	ASSERT(pModemFrame->GetMsgCode() == EV_NODEBINDINGENTRY);
}

CMessageError_NodeBindingEntry::~CMessageError_NodeBindingEntry()
{
}

CString CMessageError_NodeBindingEntry::ToString() const
{
	CString szReturn;
	const BYTE bErrorCode = m_bErrorCode - WRONG_BINDING_ENTRY_NR;

	if (bErrorCode < MAX_BINDINGENTRY_ERROR_CODES) 
	{
		szReturn.Format("EV_NodeBindingEntry: %s: received from node 0x%02x,Unit 0x%02x",
							 m_szErrorCodes[bErrorCode],
						     m_bNodeAddress,
						     m_bUnitAddress);								 
	}
	else
	{
		szReturn.Format("EV_NodeBindingEntry error received from node 0x%02x,Unit 0x%02x",					
						     m_bNodeAddress,
						     m_bUnitAddress);			
	}
	return szReturn;
}

CString CMessageError_NodeBindingEntry::ToSmallString() const
{
	CString szReturn;

	if (m_bErrorCode < MAX_BINDINGENTRY_ERROR_CODES) 
	{
		szReturn.Format("RX: 0x%02x,0x%02x: %s",
						m_bNodeAddress,
					    m_bUnitAddress,
					    m_szErrorCodes[m_bErrorCode]);
						     								 
	}
	else
	{
		szReturn.Format("RX: 0x%02x,0x%02x: BindingEntry error",
						m_bNodeAddress,
					    m_bUnitAddress);
					   
	}
	return szReturn;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
const char* CMessageError_NodeBindingFileControl::m_szErrorCodes[MAX_BINDINGFILE_ERROR_CODES] =
{
	"Binding file close error",		//CLOSE_ERROR					0x80 
	"Binding file open error",		//OPEN_ERROR					0x81
	"Binding file share violation",	//SHARE_VIOLATION				0x82
	"Temporary buffer in use"		//BINDING_TEMP_BUFFER_IN_USE	0x83
};

///////////////////////////////////////////////////////////////////////////////////////////
CMessageError_NodeBindingFileControl::CMessageError_NodeBindingFileControl(CComApplication* const pParent,
									  const ICommunicationFrame* const pModemFrame) :
CMessageError(pParent,pModemFrame)
{
	ASSERT(pModemFrame->GetMsgCode() == EV_NODEBINDINGENTRY);
}

CMessageError_NodeBindingFileControl::~CMessageError_NodeBindingFileControl()
{
}

CString CMessageError_NodeBindingFileControl::ToString() const
{
	CString szReturn;
	const BYTE bErrorCode = m_bErrorCode - CLOSE_ERROR;

	if (bErrorCode < MAX_BINDINGFILE_ERROR_CODES) 
	{
		szReturn.Format("EV_NodeBindingFileControl: %s: received from node 0x%02x,Unit 0x%02x",
							 m_szErrorCodes[bErrorCode],
						     m_bNodeAddress,
						     m_bUnitAddress);								 
	}
	else
	{
		szReturn.Format("EV_NodeBindingFileControl error received from node 0x%02x,Unit 0x%02x",					
						     m_bNodeAddress,
						     m_bUnitAddress);			
	}
	return szReturn;
}


CString CMessageError_NodeBindingFileControl::ToSmallString() const
{
	CString szReturn;

	if (m_bErrorCode < MAX_BINDINGFILE_ERROR_CODES) 
	{
		szReturn.Format("RX: 0x%02x,0x%02x: %s",
						m_bNodeAddress,
					    m_bUnitAddress,
					    m_szErrorCodes[m_bErrorCode]);
						     								 
	}
	else
	{
		szReturn.Format("RX: 0x%02x,0x%02x: BindingFile error",
						m_bNodeAddress,
					    m_bUnitAddress);
					   
	}
	return szReturn;
}