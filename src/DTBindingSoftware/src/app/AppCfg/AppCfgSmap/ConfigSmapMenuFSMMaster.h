// ConfigSmapMenuFSMMaster.h: interface for the CConfigSmapMenuFSMMaster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPMENUFSMMASTER_H__A55D4AFD_80BC_49E1_91D9_72A6D606C2B1__INCLUDED_)
#define AFX_CONFIGSMAPMENUFSMMASTER_H__A55D4AFD_80BC_49E1_91D9_72A6D606C2B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
///////////////////////////////////////////////////////////////////
class CConfigSmapMenuInfo;
class CConfigSmapMenuClass;
///////////////////////////////////////////////////////////////////
class IComApplication;
///////////////////////////////////////////////////////////////////
class CConfigSmapMenuFSMMaster : public CConfigExternalFSMMaster
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

	CConfigSmapMenuClass* m_pConfigClasses;

public:
	CConfigSmapMenuFSMMaster(IComApplication& parent);
	~CConfigSmapMenuFSMMaster( );

public:
	BOOL Start(CConfigSmapMenuInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );

	BOOL FSM(int event);
	BOOL StartFSM( );
	BOOL StartFSMDone( );
};
///////////////////////////////////////////////////////////////////


#endif // !defined(AFX_CONFIGSMAPMENUFSMMASTER_H__A55D4AFD_80BC_49E1_91D9_72A6D606C2B1__INCLUDED_)
