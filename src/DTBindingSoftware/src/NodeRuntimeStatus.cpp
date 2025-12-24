// NodeRuntimeStatus.cpp: implementation of the CNodeRuntimeStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeRuntimeStatus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNodeRuntimeStatus::CNodeRuntimeStatus()
{
	int i;

	for (i=0;i<DLL_STATUS_BLOCK0_LENGTH;i++)
		pbDLLStatusBlock0[i] = 0;

	for (i=0;i<DLL_STATUS_BLOCK1_LENGTH;i++)
		pbDLLStatusBlock1[i] = 0;
}

CNodeRuntimeStatus::~CNodeRuntimeStatus()
{

}

void CNodeRuntimeStatus::Copy(const CNodeRuntimeStatus* const pSource)
{
	int i;
	ASSERT(pSource);

	for (i=0;i<DLL_STATUS_BLOCK0_LENGTH;i++)
		pbDLLStatusBlock0[i] = pSource->pbDLLStatusBlock0[i];

	for (i=0;i<DLL_STATUS_BLOCK1_LENGTH;i++)
		pbDLLStatusBlock1[i] = pSource->pbDLLStatusBlock1[i];
}

 
void CNodeRuntimeStatus::DumpErrors(CStringArray& stringArray)
{
	stringArray.RemoveAll();

	stringArray.Add(_GetNicErrorReset());	
	stringArray.Add(_GetNicErrorFlowTimeout());
	stringArray.Add(_GetNicErrorTxWarning());
	stringArray.Add(_GetNicErrorRxPassive());
	stringArray.Add(_GetNicErrorTxPassive());
	stringArray.Add(_GetNicErrorTxBusOff());
	stringArray.Add(_GetNicErrorRX0Overrun());
	stringArray.Add(_GetNicErrorRX1Overrun());
	stringArray.Add(_GetNicErrorExplicitTimeout());
	stringArray.Add(_GetNicErrorDllTimeout());
	stringArray.Add(_GetNicErrorHostBusy());
	stringArray.Add(_GetNicErrorNoFreeMTC());
	stringArray.Add(_GetNicErrorFrameError());
	stringArray.Add(_GetNicErrorLostMessage());
	stringArray.Add(_GetNicErrorMessageTimeout());
	stringArray.Add(_GetNicTEC());
	stringArray.Add(_GetNicREC());
}

void CNodeRuntimeStatus::DumpCanControllerErrors(CStringArray& stringArray)
{
	stringArray.RemoveAll();

	stringArray.Add(_GetNicErrorReset());	
	stringArray.Add(_GetNicErrorFlowTimeout());
	stringArray.Add(_GetNicErrorTxWarning());
	stringArray.Add(_GetNicErrorRxPassive());
	stringArray.Add(_GetNicErrorTxPassive());
	stringArray.Add(_GetNicErrorTxBusOff());
	stringArray.Add(_GetNicErrorRX0Overrun());
	stringArray.Add(_GetNicErrorRX1Overrun());	
	stringArray.Add(_GetNicTEC());
	stringArray.Add(_GetNicREC());
}

void CNodeRuntimeStatus::DumpNetworkErrors(CStringArray& stringArray)
{
	stringArray.RemoveAll();

	stringArray.Add(_GetNicErrorExplicitTimeout());
	stringArray.Add(_GetNicErrorDllTimeout());
	stringArray.Add(_GetNicErrorHostBusy());
	stringArray.Add(_GetNicErrorNoFreeMTC());
	stringArray.Add(_GetNicErrorFrameError());
	stringArray.Add(_GetNicErrorLostMessage());
	stringArray.Add(_GetNicErrorMessageTimeout());
}


///////////////////////////////////////////////////////////
CString CNodeRuntimeStatus::_GetNicErrorReset() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_RESET: %d",GetNicErrorReset());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorFlowTimeout() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_FLOW_TIMEOUT: %d",GetNicErrorFlowTimeout());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorTxWarning() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_TX_WARNING: %d",GetNicErrorTxWarning());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorRxPassive() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_RX_PASSIVE: %d",GetNicErrorRxPassive());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorTxPassive() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_TX_PASSIVE: %d",GetNicErrorTxPassive());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorTxBusOff() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_TX_BUSOFF: %d",GetNicErrorTxBusOff());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorRX0Overrun() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_RX0_OVERRUN: %d",GetNicErrorRX0Overrun());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorRX1Overrun() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_RX1_OVERRUN: %d",GetNicErrorRX1Overrun());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorExplicitTimeout() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_EXPLICIT_TIMEOUT: %d",GetNicErrorExplicitTimeout());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorDllTimeout() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_DLL_TIMEOUT: %d",GetNicErrorDllTimeout());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorHostBusy() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_HOST_BUSY: %d",GetNicErrorHostBusy());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorNoFreeMTC() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_NO_FREE_MTC: %d",GetNicErrorNoFreeMTC());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorFrameError() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_FRAME_ERROR: %d",GetNicErrorFrameError());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorLostMessage() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_LOST_MESSAGE: %d",GetNicErrorLostMessage());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicErrorMessageTimeout() const
{
	CString szTemp;
	szTemp.Format("NIC_ERROR_MESSAGE_TIMEOUT: %d",GetNicErrorMessageTimeout());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicTEC() const
{
	CString szTemp;
	szTemp.Format("TEC counter: %d",GetNicTEC());
	return szTemp;
}

CString CNodeRuntimeStatus::_GetNicREC() const
{
	CString szTemp;
	szTemp.Format("REC counter: %d",GetNicREC());
	return szTemp;
}

///////////////////////////////////////////////////////////

BYTE CNodeRuntimeStatus::GetNicErrorReset() const 
{
	return (pbDLLStatusBlock0[0]%16);
}

BYTE CNodeRuntimeStatus::GetNicErrorFlowTimeout() const
{
	return (pbDLLStatusBlock0[0]/16);
}

BYTE CNodeRuntimeStatus::GetNicErrorTxWarning() const
{
	return (pbDLLStatusBlock0[1]%16);
}

BYTE CNodeRuntimeStatus::GetNicErrorRxPassive() const
{
	return (pbDLLStatusBlock0[1]/16);
}

BYTE CNodeRuntimeStatus::GetNicErrorTxPassive() const
{
	return (pbDLLStatusBlock0[2]%16);
}	

BYTE CNodeRuntimeStatus::GetNicErrorTxBusOff() const
{
	return (pbDLLStatusBlock0[2]/16);
}

BYTE CNodeRuntimeStatus::GetNicErrorRX0Overrun() const
{
	return (pbDLLStatusBlock0[3]%16);
}

BYTE CNodeRuntimeStatus::GetNicErrorRX1Overrun() const
{
	return (pbDLLStatusBlock0[3]/16);
}

BYTE CNodeRuntimeStatus::GetNicErrorExplicitTimeout() const
{
	return (pbDLLStatusBlock0[4]%16);
}

BYTE CNodeRuntimeStatus::GetNicErrorDllTimeout() const
{
	return (pbDLLStatusBlock0[4]/16);
}	

BYTE CNodeRuntimeStatus::GetNicErrorHostBusy() const
{
	return (pbDLLStatusBlock1[0]%16);
}	

BYTE CNodeRuntimeStatus::GetNicErrorNoFreeMTC() const
{
	return (pbDLLStatusBlock1[0]/16);
}	

BYTE CNodeRuntimeStatus::GetNicErrorFrameError() const
{
	return (pbDLLStatusBlock1[1]%16);
}

BYTE CNodeRuntimeStatus::GetNicErrorLostMessage() const
{
	return (pbDLLStatusBlock1[1]/16);
}

BYTE CNodeRuntimeStatus::GetNicErrorMessageTimeout() const
{
	return (pbDLLStatusBlock1[2]%16);
}

BYTE CNodeRuntimeStatus::GetNicTEC() const
{
	return (pbDLLStatusBlock1[3]);
}

BYTE CNodeRuntimeStatus::GetNicREC() const
{
	return (pbDLLStatusBlock1[4]);
}

void CNodeRuntimeStatus::SetStatusBlock0(BYTE bIndex,BYTE bValue) 
{
	if (bIndex < DLL_STATUS_BLOCK0_LENGTH) 
		pbDLLStatusBlock0[bIndex] = bValue;
}

void CNodeRuntimeStatus::SetStatusBlock1(BYTE bIndex,BYTE bValue) 
{
	if (bIndex < DLL_STATUS_BLOCK1_LENGTH) 
		pbDLLStatusBlock1[bIndex] = bValue;
}


/************************************************************************
 *
 ************************************************************************/
void CNodeGatewayRuntimeStatus::Copy(const CNodeRuntimeStatus* const pSource)
{
	this->CNodeRuntimeStatus::Copy(pSource);

	CurrentState = 
		static_cast<const CNodeGatewayRuntimeStatus* const>(pSource)->CurrentState;
}

void CNodeGatewayRuntimeStatus::SetState(ENUM_STATES state)
{
	CurrentState = state;
}

void CNodeGatewayRuntimeStatus::ToString(CString &string) const 
{
	switch (CurrentState)
	{
		case RT_WORKING_NORMAL:
		{
			string.Format("Working normal");
		} break;

		case RT_UPDATING_BUSY:
		{
			string.Format("Updating busy");
		} break;

		case RT_DATABASE_EMPTY:
		{
			string.Format("Database empty");
		} break;

		case RT_ERROR_STATE:
		{
			string.Format("Error");
		} break;

		default:
		{
			string.Format("Undefined");
		} break;
	}
}

CNodeGatewayRuntimeStatus::ENUM_STATES CNodeGatewayRuntimeStatus::Convert(BYTE value) 
{
	if (value == RT_WORKING_NORMAL) return RT_WORKING_NORMAL;
	else if (value == RT_UPDATING_BUSY) return RT_UPDATING_BUSY;
	else if (value == RT_DATABASE_EMPTY) return RT_DATABASE_EMPTY;
	else if (value == RT_ERROR_STATE) return RT_ERROR_STATE;
	else return RT_UNDEFINED;
}

