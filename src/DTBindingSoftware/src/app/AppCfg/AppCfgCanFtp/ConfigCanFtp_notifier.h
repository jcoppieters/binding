#ifndef DUOTECNO_CONFIG_CANFTP_NOTIFIER_H
#define	DUOTECNO_CONFIG_CANFTP_NOTIFIER_H

#ifndef DUOTECNO_CONFIG_CANFTP_NOTIFIER_INTERFACE_H
	#include "IConfigCanFtp_Notifier.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
class IComApplication;					// voorwaartse declaratie.
///////////////////////////////////////////////////////////////////

class CConfigCanFTP_Notifier : public IConfigCanFTP_Notifier
{
public:
	CConfigCanFTP_Notifier(IComApplication* p);
	~CConfigCanFTP_Notifier( );

	void signalTaskDone( );
	void signalTaskStarted( );
	void signalTaskCancelled( );
	void signalTaskError( );
private:
	IComApplication* m_pCommApplication;
};
///////////////////////////////////////////////////////////////////
#endif