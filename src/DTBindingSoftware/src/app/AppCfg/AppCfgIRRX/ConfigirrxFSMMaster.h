#ifndef DUOTECNO_CONFIGIRRX_FSMMASTER_H
#define	DUOTECNO_CONFIGIRRX_FSMMASTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
///////////////////////////////////////////////////////////////////
class CConfigIRRXInfo;
class CConfigIRRXClass;
///////////////////////////////////////////////////////////////////
class IComApplication;
///////////////////////////////////////////////////////////////////
class CConfigIRRXFSMMaster : public CConfigExternalFSMMaster
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

	CConfigIRRXClass* m_pConfigClasses;

public:
	CConfigIRRXFSMMaster(IComApplication& parent);
	~CConfigIRRXFSMMaster( );

public:
	BOOL Start(CConfigIRRXInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );

	BOOL FSM(int event);
	BOOL StartFSM( );
	BOOL StartFSMDone( );
};
///////////////////////////////////////////////////////////////////
#endif