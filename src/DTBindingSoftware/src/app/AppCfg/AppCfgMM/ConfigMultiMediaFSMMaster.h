// ConfigMultiMediaFSMMaster.h: interface for the CConfigMultiMediaFSMMaster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGMULTIMEDIAFSMMASTER_H__96896D06_F38A_4368_B46E_9E494483F222__INCLUDED_)
#define AFX_CONFIGMULTIMEDIAFSMMASTER_H__96896D06_F38A_4368_B46E_9E494483F222__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
#include "ConfigMultiMediaFSM.h"
class CConfigMultiMediaInfo;
class IComApplication;
///////////////////////////////////////////////////////////////////

/***************************************************************//**
 *
 ******************************************************************/
class IConfigMultiMediaFSMMaster
{
public:
	virtual void NotifyOnEventInfo(CString s)		= 0;
	virtual void NotifyOnEventDone(void)			= 0;
	virtual void NotifyOnEventError(void)			= 0;

	virtual void StartTimer(void)					= 0;
	virtual void StopTimer(void)					= 0;

};

/***************************************************************//**
 * @class	CConfigMultiMediaFSMMaster
 *
 * @changes	TM20091218 Het nodetype (versie nr) van de nodes wordt
 *			gebruikt om te bepalen welke FSM er uitgevoerd wordt.
 ******************************************************************/
class CConfigMultiMediaFSMMaster : public IConfigMultiMediaFSMMaster
{
private:
	typedef enum 
	{
		FSM_EV_OPERATION_START = 0,
		FSM_EV_SIGNAL_DONE,
		FSM_EV_SIGNAL_ERROR

	}ENUM_FSM_EVENTS;

	typedef enum 
	{
		STATE_TASK_IDLE = 0,
		STATE_TASK_BUSY

	}ENUM_FSM_STATES;

	BYTE						m_bCurrentIndex;
	CConfigMultiMediaFSM*		m_pMultiMediaFSM;
	CConfigMultiMediaInfo*		m_pMultiMediaInfo;	
	IComApplication *const		m_pParent;
	ENUM_FSM_STATES				m_CurrentState;

	void FSM(ENUM_FSM_EVENTS e);
	void Fire_OnEventDone(void);
	void Fire_OnEventStart(void);
	void Fire_OnEventError(void);
	void Fire_OnEventCancel(void);

public:
	CConfigMultiMediaFSMMaster(IComApplication *pCom);
	virtual ~CConfigMultiMediaFSMMaster();

	// from child FSM
	void NotifyOnEventInfo(CString s);
	void NotifyOnEventDone(void);
	void NotifyOnEventError(void);
	void StartTimer(void);
	void StopTimer(void);

	// from parent
	BOOL EvSignalStatusConfigOk(BYTE bNodeAddress);
	BOOL EvSignalStatusConfigError(BYTE bNodeAddress);
	BOOL EvAfterConfigTimeout();

	BOOL Start(CConfigMultiMediaInfo *pInfo);
	BOOL Cancel(void);
};

#endif // !defined(AFX_CONFIGMULTIMEDIAFSMMASTER_H__96896D06_F38A_4368_B46E_9E494483F222__INCLUDED_)
