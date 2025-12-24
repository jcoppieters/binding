#include "stdafx.h"
#include "ConfigCanFtp_notifier.h"
///////////////////////////////////////////////////////////////////////
#include "IComApplication.h"		
///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


///////////////////////////////////////////////////////////////////////
////////////////////// CConfigCanFTP_Notifier /////////////////////////
///////////////////////////////////////////////////////////////////////
CConfigCanFTP_Notifier::CConfigCanFTP_Notifier(IComApplication* p) :
m_pCommApplication( p )
{
}
CConfigCanFTP_Notifier::~CConfigCanFTP_Notifier( )
{
}
///////////////////////////////////////////////////////////////////////
void CConfigCanFTP_Notifier::signalTaskDone( )
{
	ASSERT( m_pCommApplication );
	m_pCommApplication->NotifyOnTaskDone( COMAPPLICATION_TASK_ID_FTP );
}
void CConfigCanFTP_Notifier::signalTaskStarted( )
{
	ASSERT( m_pCommApplication );
}
void CConfigCanFTP_Notifier::signalTaskCancelled( )
{
	ASSERT( m_pCommApplication );
	m_pCommApplication->NotifyOnTaskCancelled( COMAPPLICATION_TASK_ID_FTP );
}
void CConfigCanFTP_Notifier::signalTaskError( )
{
	ASSERT( m_pCommApplication );
	m_pCommApplication->NotifyOnTaskError( COMAPPLICATION_TASK_ID_FTP );
}
/////////////////////////////////////////////////////////////////////////




