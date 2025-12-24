// QHSMBindingConversionFromNodes.h: interface for the CQHSMBindingConversionFromNodes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QHSMBINDINGCONVERSIONFROMNODES_H__BFB1661B_D55C_4DC8_9C09_04082F5A164B__INCLUDED_)
#define AFX_QHSMBINDINGCONVERSIONFROMNODES_H__BFB1661B_D55C_4DC8_9C09_04082F5A164B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "qf\include\qf_win32.h"
#include "MyWindowTimers.h"



#define WM_QFSM_CONVERTFROMNODES_EVENT			(WM_USER + 2005)
#define WM_QFSM_CONVERTFROMNODES_INITIALISE		(WM_USER + 2006)



class CBindingConversionFromNodesOperations;
class CLogFileConversionFromNodes;



/**
 *
 */
class CQHSMBindingConversionFromNodes : public QHsm  
{
	typedef enum 
	{
		EV_OPERATION_TASK_START = Q_USER_SIG,
		EV_OPERATION_TASK_CANCEL,	
		EV_SIGNAL_TASK_DONE,
		EV_SIGNAL_TASK_CANCELLED,
		EV_SIGNAL_TASK_PROCESS,
		EV_AFTER_TIMER_NEXT,
		EV_OPERATION_TASK_CLEAR

	} EV_FSM_CONVERSION_FSM_NODES;

public:
	static CQHSMBindingConversionFromNodes* GetInstance();

protected:
	CBindingConversionFromNodesOperations* m_pConversionFromNodes;
	CLogFileConversionFromNodes* m_pLogFile;

	unsigned short m_usMaxBindingNr;
	unsigned short m_usCurrentBindingNr;
	unsigned short m_usCurrentBindingFile;

public:
	CQHSMBindingConversionFromNodes(): QHsm((QPseudoState)&CQHSMBindingConversionFromNodes::initial)		// CHANGES_MVS_2008
	{
		m_pConversionFromNodes = 0;
	}

	virtual ~CQHSMBindingConversionFromNodes();

public:
	void initial(QEvent const *e);
	QSTATE Main(QEvent const *e);
	QSTATE Idle(QEvent const *e);
	QSTATE Busy(QEvent const *e);
		QSTATE Busy_Verify(QEvent const *e);
			QSTATE Busy_Verify_Syntaxsis(QEvent const *e);
			QSTATE Busy_Verify_Addresses(QEvent const *e);			
		QSTATE Busy_Adjust(QEvent const *e);
			QSTATE Busy_Adjust_PropertyStrings(QEvent const *e);
			QSTATE Busy_Adjust_InstructionStrings(QEvent const *e);
		QSTATE Busy_Normal(QEvent const *e);
			QSTATE Busy_Normal_Process(QEvent const *e);
		QSTATE Busy_Property(QEvent const *e);
			QSTATE Busy_Property_Upgrade(QEvent const *e);
			QSTATE Busy_Property_Process(QEvent const *e);
		QSTATE Busy_Clear(QEvent const *e);
	QSTATE Done(QEvent const *e);

public:
	
	void EvOperationStart() 
	{
		Trigger(EV_OPERATION_TASK_START);
	}
	void EvOperationCancel() 
	{
		Trigger(EV_OPERATION_TASK_CANCEL);
	}
	void EvAfterTimerNext()
	{
		Trigger(EV_AFTER_TIMER_NEXT);
	}

	void EvOperationClear()
	{
		Trigger(EV_OPERATION_TASK_CLEAR);
	}

protected:

	inline void Trigger(EV_FSM_CONVERSION_FSM_NODES event);
	inline void StartTimer();
	inline void StopTimer();
	inline void NotifyUser(CString s);

};

#endif // !defined(AFX_QHSMBINDINGCONVERSIONFROMNODES_H__BFB1661B_D55C_4DC8_9C09_04082F5A164B__INCLUDED_)
