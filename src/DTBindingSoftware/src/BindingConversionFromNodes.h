// BindingConversionFromNodes.h: interface for the CBindingConversionFromNodes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONFROMNODES_H__0DD6915D_C735_4B1D_9D99_B52BAC24051E__INCLUDED_)
#define AFX_BINDINGCONVERSIONFROMNODES_H__0DD6915D_C735_4B1D_9D99_B52BAC24051E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qf_win32.h"
#include "MyWindowTimers.h"



#define WM_QFSM_CONVERTFROMNODES_EVENT			(WM_USER + 2005)
#define WM_QFSM_CONVERTFROMNODES_INITIALISE		(WM_USER + 2006)


class CBindingFilesOperations;
class CBindingEntry;



class CBindingConversionFromNodes : public QFsm
{
public:
	static CBindingConversionFromNodes* GetInstance();

protected:

	typedef enum 
	{
		EV_OPERATION_TASK_START = Q_USER_SIG,
		EV_OPERATION_TASK_CANCEL,	
		EV_SIGNAL_TASK_DONE,
		EV_SIGNAL_TASK_CANCELLED,
		EV_SIGNAL_TASK_PROCESS,
		EV_AFTER_TIMER_NEXT

	} EV_FSM_CONVERSION_FSM_NODES;

protected:
	CBindingConversionFromNodes();

	BOOL Update(CBindingEntry* pBindingEntry);
	BOOL Add(CBindingEntry* pBindingEntry);

protected:
	CBindingFilesOperations *m_pBindingFilesOperations;
	unsigned short m_usMaxBindingNr;
	unsigned short m_usCurrentBindingNr;
	CStringArray m_StringArray;

public:
	virtual ~CBindingConversionFromNodes();

public:
	BOOL Start_Debug();


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
	
protected:
	void initial(QEvent const*  );
	void State_Idle(QEvent const* e);
	void State_Busy(QEvent const* e);
	void State_Done(QEvent const* e);

	inline void Trigger(EV_FSM_CONVERSION_FSM_NODES event);
	inline void StartTimer();
	inline void StopTimer();
	inline void NotifyUser(CString s);
};

#endif // !defined(AFX_BINDINGCONVERSIONFROMNODES_H__0DD6915D_C735_4B1D_9D99_B52BAC24051E__INCLUDED_)
