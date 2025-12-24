#ifndef INCLUDED_DUOTECNO_CONFIGEXTERNALMASTER_FSM_H
#define INCLUDED_DUOTECNO_CONFIGEXTERNALMASTER_FSM_H
/***********************************************************/
#include "ConfigExternalFSM.h"
/***********************************************************/
class IComApplication;
class CConfigExternalFSMMaster;
/***********************************************************/
class CConfigExternalFSM_Events : public IConfigExternalFSM_Events
{
protected:	
	CConfigExternalFSMMaster& m_parent;

public:
	CConfigExternalFSM_Events(CConfigExternalFSMMaster& parent);
	~CConfigExternalFSM_Events( );

public:
	virtual void NotifyOnEventDone( );
	virtual void NotifyOnEventError( );	
};
/***********************************************************/
class CConfigExternalFSM_Timers : public DUOTECNO::MFC_HELPER::ITimerControl
{
protected:	
	IComApplication& m_parent;

public:	
	CConfigExternalFSM_Timers(IComApplication& parent);
	~CConfigExternalFSM_Timers( );

public:
	virtual void startTimer(int ID );
	virtual void startTimer(int ID, int reloadValue);
	virtual void stopTimer(int ID);
};
/***********************************************************/
class CConfigExternalFSM_Display : 
	public DUOTECNO::MFC_HELPER::IDisplayDevice
{
protected:	
	IComApplication& m_parent;

public:	
	CConfigExternalFSM_Display(IComApplication& parent);
	~CConfigExternalFSM_Display( );

public:
	virtual void displayInfoMsg(const char* szMessage);
	virtual void displayErrorMsg(const char* szMessage);
};
/***********************************************************/
class CConfigExternalFSMMaster : public IConfigExternalFSM_Master
{
	friend class CConfigExternalFSM_Events;

private:
	IComApplication& m_parent;
	CConfigExternalFSM_Events	m_eventSink;
	CConfigExternalFSM_Timers	m_timers;
	CConfigExternalFSM_Display	m_display;
	CConfigExternalFSM			m_fsmWorker;
	BYTE m_bCurrentNodeAddress;

protected:
	CConfigExternalFSM& getFSM( ) {
		return m_fsmWorker;
	}
	IComApplication& getParent( ) {
		return m_parent;
	}

public:
	CConfigExternalFSMMaster(IComApplication& app);
	virtual ~CConfigExternalFSMMaster( );

	void setCurrentNode(BYTE bNodeAddress) {
		m_bCurrentNodeAddress = bNodeAddress;
	}
	BYTE getCurrentNode( ) const {
		return m_bCurrentNodeAddress;
	}


public:		// IConfigExternalFSM_Master
	virtual IConfigExternalFSM_Events& getEventSink( ) {
		return m_eventSink; 
	};
	virtual DUOTECNO::MFC_HELPER::ITimerControl& getTimerControl( ) {
		return m_timers;
	}
	virtual DUOTECNO::MFC_HELPER::IDisplayDevice& getDisplayDevice( ) {
		return m_display; 
	}

public:
	// from parent
	BOOL EvSignalStatusConfigOk(BYTE bNodeAddress);
	BOOL EvSignalStatusConfigError(BYTE bNodeAddress);
	BOOL EvAfterConfigTimeout();	

protected:	// wordt aangeroepen door CConfigExternalFSM_Events
	virtual void onChildFSMDone( )	= 0;
	virtual void onChildFSMError( ) = 0;
};

/***********************************************************/
#endif 
/***********************************************************/
