// NodeRuntimeStatus.h: interface for the CNodeRuntimeStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODERUNTIMESTATUS_H__45043506_2840_4058_97C5_A8BDFB9C7188__INCLUDED_)
#define AFX_NODERUNTIMESTATUS_H__45043506_2840_4058_97C5_A8BDFB9C7188__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------
#define DLL_STATUS_BLOCK0_LENGTH		5
#define DLL_STATUS_BLOCK1_LENGTH		5
// ----------------------------------------------

class CNodeRuntimeStatus  
{
private:
	CNodeRuntimeStatus(const CNodeRuntimeStatus& NodeRuntimeStatus);
	CNodeRuntimeStatus& operator=(const CNodeRuntimeStatus& NodeRuntimeStatus);

protected:
	BYTE pbDLLStatusBlock0[DLL_STATUS_BLOCK0_LENGTH]; 
	BYTE pbDLLStatusBlock1[DLL_STATUS_BLOCK1_LENGTH]; 

public:
	CNodeRuntimeStatus();
	virtual ~CNodeRuntimeStatus();

	void SetStatusBlock0(BYTE bIndex,BYTE bValue);
	void SetStatusBlock1(BYTE bIndex,BYTE bValue);

	virtual void Copy(const CNodeRuntimeStatus* const pSource);

protected:
	CString _GetNicErrorReset() const;
	CString _GetNicErrorFlowTimeout() const;
	CString _GetNicErrorTxWarning() const;
	CString _GetNicErrorRxPassive() const;
	CString _GetNicErrorTxPassive() const;
	CString _GetNicErrorTxBusOff() const;
	CString _GetNicErrorRX0Overrun() const;
	CString _GetNicErrorRX1Overrun() const;
	CString _GetNicErrorExplicitTimeout() const;
	CString _GetNicErrorDllTimeout() const;
	CString _GetNicErrorHostBusy() const;
	CString _GetNicErrorNoFreeMTC() const;
	CString _GetNicErrorFrameError() const;
	CString _GetNicErrorLostMessage() const;
	CString _GetNicErrorMessageTimeout() const;
	CString _GetNicTEC() const;
	CString _GetNicREC() const;

public:
	BYTE GetNicErrorReset() const;
	BYTE GetNicErrorFlowTimeout() const;
	BYTE GetNicErrorTxWarning() const;
	BYTE GetNicErrorRxPassive() const;
	BYTE GetNicErrorTxPassive() const;
	BYTE GetNicErrorTxBusOff() const;
	BYTE GetNicErrorRX0Overrun() const;
	BYTE GetNicErrorRX1Overrun() const;
	BYTE GetNicErrorExplicitTimeout() const;
	BYTE GetNicErrorDllTimeout() const;
	BYTE GetNicErrorHostBusy() const;
	BYTE GetNicErrorNoFreeMTC() const;
	BYTE GetNicErrorFrameError() const;
	BYTE GetNicErrorLostMessage() const;
	BYTE GetNicErrorMessageTimeout() const;
	BYTE GetNicTEC() const;
	BYTE GetNicREC() const;

	void DumpErrors(CStringArray& stringArray);
	void DumpCanControllerErrors(CStringArray& stringArray);
	void DumpNetworkErrors(CStringArray& stringArray);
};


/**
 *
 */

class CNodeGatewayRuntimeStatus : public CNodeRuntimeStatus
{
private:
	CNodeGatewayRuntimeStatus(const CNodeGatewayRuntimeStatus& NodeRuntimeStatus);
	CNodeGatewayRuntimeStatus& operator=(const CNodeGatewayRuntimeStatus& NodeRuntimeStatus);

public:
	
	typedef enum
	{	
		RT_WORKING_NORMAL = 0,
		RT_UPDATING_BUSY = 1,
		RT_DATABASE_EMPTY= 2,
		RT_ERROR_STATE = 3,
		RT_UNDEFINED = 0xFF

	} ENUM_STATES;

protected:

	ENUM_STATES CurrentState;

public:

	CNodeGatewayRuntimeStatus() {
		CurrentState = RT_UNDEFINED;
	}
	virtual ~CNodeGatewayRuntimeStatus() {
	
	}

	virtual void Copy(const CNodeRuntimeStatus* const pSource);

	void SetState(ENUM_STATES state);
	void ToString(CString &string) const;
	
	static ENUM_STATES Convert(BYTE value);
};

#endif // !defined(AFX_NODERUNTIMESTATUS_H__45043506_2840_4058_97C5_A8BDFB9C7188__INCLUDED_)
