#pragma once

///////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
///////////////////////////////////////////////////////////////////
class CConfigSmapAVRoomInfo;
class CConfigSmapVRoomClass;
class IComApplication;
///////////////////////////////////////////////////////////////////

class CConfigSmapAVRoomFSMMaster : public CConfigExternalFSMMaster
{
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
	CConfigSmapVRoomClass* m_pConfigClasses;

public:
	CConfigSmapAVRoomFSMMaster(IComApplication& parent);
	~CConfigSmapAVRoomFSMMaster( );

	BOOL Start(CConfigSmapAVRoomInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );

	BOOL FSM(int event);
	BOOL StartFSM( );
	BOOL StartFSMDone( );
};
///////////////////////////////////////////////////////////////////