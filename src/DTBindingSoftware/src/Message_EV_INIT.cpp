// Message_EV_INIT.cpp: implementation of the CMessage_EV_INIT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "Message_EV_INIT.h"

#include "communication/communication.h"
#include "NodeMess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

const char* CMessage_EV_INIT::m_szResetMode[MAX_RESET_MODES] =
{
	"WATCHDOG RESET",			//WATCHDOG_RESET
	"Node Reset",					// EXTERNAL_RESET
	"Power On Reset",				// P0WER_ON_RESET1
	"Power On Reset",				// P0WER_ON_RESET2
	"Node Reset",					// NODERESET
	"Node Reset"					// UNIT_SOFTWARE_RESET
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMessage_EV_INIT::CMessage_EV_INIT(CComApplication* const parent,const ICommunicationFrame* const pModemFrame) :
m_bResetCode(pModemFrame->GetDataBYTE(0)),
m_bNodeAddress(pModemFrame->GetNodeAddress()),
m_bUnitAddress(pModemFrame->GetUnitAddress())
{
	ASSERT(pModemFrame->GetMsgCode() == EV_INIT);
}

CMessage_EV_INIT::~CMessage_EV_INIT()
{

}

CString CMessage_EV_INIT::ToString() const
{
	CString szReturn;

	if (m_bResetCode < MAX_RESET_MODES) 
	{
		if (m_bUnitAddress == BROADCAST_UNIT)
		{
			szReturn.Format("EV_INIT : %s : received from node 0x%02x",
								 m_szResetMode[m_bResetCode],
							     m_bNodeAddress);							    
		}
		else
		{
			szReturn.Format("EV_INIT : %s : received from node 0x%02x,Unit 0x%02x",
								 m_szResetMode[m_bResetCode],
							     m_bNodeAddress,
							     m_bUnitAddress);								 
		}
	}
	else
	{
		if (m_bUnitAddress == BROADCAST_UNIT)
		{
			szReturn.Format("EV_INIT : received from node 0x%02x",					
							     m_bNodeAddress);							    
		}
		else
		{
			szReturn.Format("EV_INIT : received from node 0x%02x,Unit 0x%02x",					
							     m_bNodeAddress,
							     m_bUnitAddress);			
		}
	}
	return szReturn;
}

CString CMessage_EV_INIT::ToSmallString() const
{
	CString szReturn;

	if (m_bResetCode < MAX_RESET_MODES) 
	{
		if (m_bUnitAddress == BROADCAST_UNIT)
		{
			szReturn.Format("RX: 0x%02x: %s",								 
							     m_bNodeAddress,
								 m_szResetMode[m_bResetCode]);							    
		}
		else
		{
			szReturn.Format("RX: 0x%02x,0x%02x: %s",								 
							     m_bNodeAddress,
								 m_bUnitAddress,
								 m_szResetMode[m_bResetCode]);								 
		}
	}
	else
	{
		if (m_bUnitAddress == BROADCAST_UNIT)
		{
			szReturn.Format("RX: 0x%02x: EV_INIT", m_bNodeAddress);													    
		}
		else
		{
			szReturn.Format("RX: 0x%02x,0x%02x: EV_INIT", 
							m_bNodeAddress, 
							m_bUnitAddress);			
		}
	}
	return szReturn;
}