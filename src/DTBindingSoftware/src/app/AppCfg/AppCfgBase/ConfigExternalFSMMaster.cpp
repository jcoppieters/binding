#include "stdafx.h"
#include "ConfigExternalFSMMaster.h"
/***********************************************************/
#include "ConfigExternalFSM.h"
#include "IComApplication.h"
#include "ComApplication.h"			// temp ...

#include "utils/app/DisplayDevice.h"
/***********************************************************/
#pragma warning( disable:4355 )
/***********************************************************/

/***********************************************************/
/*************** CConfigExternalFSM_Events *****************/
/***********************************************************/
CConfigExternalFSM_Events::CConfigExternalFSM_Events(CConfigExternalFSMMaster& parent) :
m_parent( parent ) {  };
CConfigExternalFSM_Events::~CConfigExternalFSM_Events( )  {  };
/************************************************************/
void CConfigExternalFSM_Events::NotifyOnEventDone( )
{
	m_parent.onChildFSMDone( );
}
void CConfigExternalFSM_Events::NotifyOnEventError( )
{
	m_parent.onChildFSMError( );
};


/***********************************************************/
/*************** CConfigExternalFSM_Timers *****************/
/***********************************************************/
CConfigExternalFSM_Timers::CConfigExternalFSM_Timers(IComApplication& parent) :
m_parent( parent )  { };
CConfigExternalFSM_Timers::~CConfigExternalFSM_Timers( ) {  };
/***********************************************************/
void CConfigExternalFSM_Timers::startTimer(int ID)
{
	ASSERT(ID == CConfigExternalFSM::TIMER_ID);
	m_parent.StartTimer(TIMERID_EXTERNALCONFIG_ACKTIMEOUT,TIMER_EXTERNALCONFIG_ACKTIMEOUT_VALUE);	
}

void CConfigExternalFSM_Timers::startTimer(int ID, int /* reloadValue*/) {
	ASSERT(ID == CConfigExternalFSM::TIMER_ID);
	m_parent.StartTimer(TIMERID_EXTERNALCONFIG_ACKTIMEOUT,TIMER_EXTERNALCONFIG_ACKTIMEOUT_VALUE);	
}

void CConfigExternalFSM_Timers::stopTimer(int ID) {
	ASSERT(ID == CConfigExternalFSM::TIMER_ID);
	m_parent.StopTimer(TIMERID_EXTERNALCONFIG_ACKTIMEOUT);
}
/************************************************************/


/***********************************************************/
/*************** CConfigExternalFSM_Timers *****************/
/***********************************************************/
CConfigExternalFSM_Display::CConfigExternalFSM_Display(IComApplication& parent) :
m_parent( parent )  {  };
CConfigExternalFSM_Display::~CConfigExternalFSM_Display( )  { };
/***********************************************************/
void CConfigExternalFSM_Display::displayInfoMsg(const char* szMessage)
{
	CDisplayDevice::DisplayInfoMsg(szMessage);
}
void CConfigExternalFSM_Display::displayErrorMsg(const char* szMessage)
{
	CDisplayDevice::DisplayErrorMsg(szMessage);
}



/***********************************************************/
/*************** CConfigExternalFSMMaster ******************/
/***********************************************************/
CConfigExternalFSMMaster::CConfigExternalFSMMaster(IComApplication& parent) :
m_parent( parent ),
m_eventSink( *this ),
m_timers( parent ),
m_display( parent ),
m_fsmWorker( *this ),
m_bCurrentNodeAddress( 0xFF ) 
{ 
};
CConfigExternalFSMMaster::~CConfigExternalFSMMaster( ) 
{ 
};
/***********************************************************/
BOOL CConfigExternalFSMMaster::EvSignalStatusConfigOk(BYTE bNodeAddress)
{
	if (bNodeAddress != m_bCurrentNodeAddress)
		return FALSE;

	return (m_fsmWorker.EvSignalStatusConfigOk());
}
BOOL CConfigExternalFSMMaster::EvSignalStatusConfigError(BYTE bNodeAddress)
{
	if (bNodeAddress != m_bCurrentNodeAddress)
		return FALSE;

	return (m_fsmWorker.EvSignalStatusConfigError());
}
BOOL CConfigExternalFSMMaster::EvAfterConfigTimeout()
{
	return (m_fsmWorker.EvAfterConfigTimeout());
}