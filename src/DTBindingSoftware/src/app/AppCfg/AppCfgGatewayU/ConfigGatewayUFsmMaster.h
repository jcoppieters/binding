#pragma once
///////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
///////////////////////////////////////////////////////////////////
class CConfigGatewayUInfo;
class CConfigGatewayUClass;
///////////////////////////////////////////////////////////////////
class IComApplication;
///////////////////////////////////////////////////////////////////

class CConfigGatewayUFsmMaster :
	public CConfigExternalFSMMaster
{
private:
	typedef CConfigExternalFSMMaster Base;

	enum 
	{
		EVENT_START,
		EVENT_DONE,
		EVENT_ERROR
	};

	enum State
	{
		STATE_IDLE = 0,
		STATE_IRCODES_DONE,
		STATE_DONE
	
	} m_currentState;

	BYTE m_currentIndex;
	BYTE m_maxIndex;
	CConfigGatewayUClass* m_pConfigClasses;

public:
	CConfigGatewayUFsmMaster(IComApplication& parent);
	~CConfigGatewayUFsmMaster(void);
	BOOL Start(CConfigGatewayUInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );
	BOOL FSM(int ev);
	BOOL StartFSM( );
	BOOL StartFSMDone( );
};

///////////////////////////////////////////////////////////////////