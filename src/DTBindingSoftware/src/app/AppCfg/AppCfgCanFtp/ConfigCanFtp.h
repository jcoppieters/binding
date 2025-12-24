#ifndef DUOTECNO_CONFIG_CANFTP_H
#define	DUOTECNO_CONFIG_CANFTP_H

#ifndef DUOTECNO_CONFIG_CANFTP_FSM_H
	#include"ConfigCanFTP_FSM.h"
#endif
#ifndef DUOTECNO_CONFIG_CANFTP_NOTIFIER_H
	#include"ConfigCanFTP_Notifier.h"
#endif
#ifndef DUOTECNO_CONFIG_CANFTP_FILE_H
	#include"ConfigCanFTP_File.h"
#endif
#ifndef DUOTECNO_CONFIG_CANFTP_DEF_H
	#include "ConfigCanFTP_def.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
class IComApplication;					// voorwaartse declaratie.
class CConfigCanFTP_Info;				// voorwaartse declaratie.
class CConfigCanFTP_FileEntryIterator;	// voorwaarste declaratie.
class CConfigCanFTP_FileEntry;			// voorwaartse declaratie.
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
///////////////////////////// CConfigCanFTP //////////////////////
//////////////////////////////////////////////////////////////////
class CConfigCanFTP
{	
public:
	CConfigCanFTP(IComApplication& parent);
	~CConfigCanFTP( );

	BOOL Start(CConfigCanFTP_Info& info);
	BOOL Cancel( );
	
	// Events 
	void signalWriteOpen(BYTE bNodeAddress,unsigned long fileSize);
	void signalReadOpen(BYTE bNodeAddress,unsigned long fileSize);
	void signalFileClosed(BYTE bNodeAddress,unsigned long fileSize);
	void signalFileAborted(BYTE bNodeAddress,unsigned long fileSize);
	void signalDataAppended(BYTE bNodeAddress);
	void signalError(BYTE bNodeAddress,FtpErrorCode ec);
	void signalDataReceived(BYTE bNodeAddress,const BYTE* pbData,int numBytes);
	void signalInfoDialogACK(BYTE bNodeAddress);

protected:
	void destroyIterator( );
	bool createIterator(CConfigCanFTP_Info& info);
	bool startWriteOpen(CConfigCanFTP_FileEntry& entry);
	bool startReadOpen(CConfigCanFTP_FileEntry& entry);
	void displayFileInfo( );
	bool startOperation(CConfigCanFTP_FileEntry& entry);
	BYTE getCurrentNodeAddress( );

private:	
	////////////////// Nested class ///////////////////////
	class CFsmEventListener : public IConfigCanFTP_Notifier
	{
	public:
		CFsmEventListener(CConfigCanFTP& parent);
		~CFsmEventListener( );

		virtual void signalTaskDone( );
		virtual void signalTaskStarted( );
		virtual void signalTaskCancelled( );
		virtual void signalTaskError( );

	private:
		CConfigCanFTP& m_parent;
	};
	/////////////////////////////////////////////////////////

	CConfigCanFTP_FileEntryIterator* m_pIterator;
	CConfigCanFTP_FSM m_fsm;
	CConfigCanFTP_Notifier m_eventNotifier;
	CFsmEventListener m_fsmEventListener;
	CConfigCanFTP_File m_fileOpener;
	IComApplication* m_pCommApplication;

	friend class CFsmEventListener;
};
///////////////////////////////////////////////////////////////////
#endif