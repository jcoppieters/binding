// RequestForNodeStatus.h: interface for the CRequestForNodeStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTFORNODESTATUS_H__780963E2_DEB0_489C_A173_CD15E7E92DF6__INCLUDED_)
#define AFX_REQUESTFORNODESTATUS_H__780963E2_DEB0_489C_A173_CD15E7E92DF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef BINDING_SOFTWARE	
	class IComApplication;
#else
	class CCommunicationClass;
#endif

class CNodeDatabase;
class CCanNode;
struct STRUCT_UNIT;
#include "FSMUnitStatus.h"
////////////////////////////////////////////////////////////////
class IRequestForNodeStatus
{
public:
	virtual void StartTimer()					= 0;
	virtual void StopTimer()					= 0;
	virtual void NotifyEvUnitDone()				= 0;
	virtual void NotifyEvUnitError()			= 0;
	virtual void NotifyEvCancelled()			= 0;
};
///////////////////////////////////////////////////////////////


// TODO: Cancel is nog niet uitgewerkt.
// Kunnen het algoritme momenteel nog niet stoppen.
// Wanneer er een unit niet antwoord gaat hij naar de volgende unit in het systeem.
//
// TODO: Uitwerken wanneer er nodes/units niet antwoorden.
// Het algoritme wordt gezien als voltooid (ook als niet alle nodes/units) hun requests beantwoord hebben.
// Bij sommige units kan dit een probleem zijn (bvb bij de alarm unit). In dit geval kan het type van de 
// alarm centrale verkeerd weergegeven worden (en ook de mogelijkheid van bindings.)
class CRequestForNodeStatus : public IRequestForNodeStatus
{
public:
#ifdef BINDING_SOFTWARE
	CRequestForNodeStatus(IComApplication *ptr,CNodeDatabase *ptr2);
#else
	CRequestForNodeStatus(CCommunicationClass *ptr,CNodeDatabase *ptr2);
#endif

	virtual ~CRequestForNodeStatus();	

	void Start(void);
	void Start(int nodeAddress);
	void Cancel(void);							// TODO: Cancel is nog niet uitgewerkt.

	void EvSignalStatusStatusReceived(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMethodData);
	void EvAfterTimerExpiry(void);		
	void StartTimer();
	void StopTimer();
	void NotifyEvUnitDone();
	void NotifyEvUnitError();
	void NotifyEvCancelled();

protected:
	enum enum_Events {
		FSM_EV_OPERATION_START = 0,	
		FSM_EV_OPERATION_CANCEL,				// TODO: Cancel is nog niet uitgewerkt.
		FSM_EV_SIGNAL_UNITDONE,	
		FSM_EV_SIGNAL_UNITERROR		
	};
	BOOL FSM(enum_Events e);
	BOOL CreateUnitStatusFsm(void);
	BOOL CreateUnitStatusFsm(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bUnitType);
	void Fire_OnFsmDone(void);
	void Fire_OnFsmCancelled(void);
	void Fire_OnFsmStart(void);

private:
	void destroyChildFsm();

	typedef enum {		
		FSM_STATE_IDLE = 0,			
		FSM_STATE_WAIT_FOR_ACK,		
		FSM_STATE_DONE			
	} States_t; 

	typedef enum {
		MODE_REQUEST_NODES_ALL = 0,
		MODE_REQUEST_NODES_SINGLE
	} Mode_t;

#ifdef BINDING_SOFTWARE
	IComApplication *const m_pComApplication;
#else
	CCommunicationClass *pComApplication;
#endif
	CNodeDatabase* const m_pNodeDatabase;
	CFSMUnitStatus* m_pFsmUnit;	
	CCanNode* m_pCurrentNode;
	STRUCT_UNIT* m_pCurrentUnit;
	States_t m_currentState;
	int m_iNodeNr;
	int m_iUnitNr;
	int m_iMaxNodes;
	int m_currentNodeAddress;		// Enkel gebruiken in MODE_REQUEST_NODES_SINGLE
	Mode_t m_mode;
};

#endif // !defined(AFX_REQUESTFORNODESTATUS_H__780963E2_DEB0_489C_A173_CD15E7E92DF6__INCLUDED_)
