#ifndef SHELLCOMMANDS_20080121_H
#define SHELLCOMMANDS_20080121_H

#include "NotifyError.h"
/////////////////////////////////////////////////////////////////////////////

template <class E_NOTIFY_POLICY = CNotifyError_Simple>
	class TCommandShell
{
protected:
	CString m_szAction;
	CString m_szFileName;

protected:
	
	void OnError(int ec)
	{
		E_NOTIFY_POLICY	notify;
		notify(ec);
	};

public:
	
	TCommandShell(CString szFileName, CString szAction = "OPEN") :
	  m_szFileName(szFileName),
	  m_szAction(szAction)
	  {
	  
	  };
	
	virtual void Execute()
	{
		HINSTANCE hError = 
			ShellExecute(0,	//GetSafeHwnd(),
						 m_szAction,
						 m_szFileName,
						 NULL,
						 NULL,
						 SW_SHOWNORMAL);
	
		if ((DWORD) hError <= 32) 
		{		
			if ((DWORD) hError == SE_ERR_ACCESSDENIED)
			{
				hError = ShellExecute(0,	//GetSafeHwnd(),
						 "runas",
						 m_szFileName,
						 NULL,
						 NULL,
						 SW_SHOWNORMAL);	
			}				

			if ((DWORD) hError <= 32) 			
				OnError((int) hError);			
		}		
	}
};
/////////////////////////////////////////////////////////////////////////////
#endif