#ifndef DUOTECNO_CONFIG_CANFTP_NOTIFIER_INTERFACE_H
#define	DUOTECNO_CONFIG_CANFTP_NOTIFIER_INTERFACE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
class IComApplication;					// voorwaartse declaratie.
///////////////////////////////////////////////////////////////////

struct IConfigCanFTP_Notifier
{
	virtual void signalTaskDone( )			= 0;
	virtual void signalTaskStarted( )		= 0;
	virtual void signalTaskCancelled( )		= 0;
	virtual void signalTaskError( )			= 0;
};
///////////////////////////////////////////////////////////////////
#endif