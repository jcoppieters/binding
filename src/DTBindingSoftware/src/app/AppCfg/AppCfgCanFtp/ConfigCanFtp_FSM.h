#ifndef DUOTECNO_CONFIG_CANFTP_FSM_H
#define	DUOTECNO_CONFIG_CANFTP_FSM_H

#ifndef DUOTECNO_CONFIG_CANFTP_NOTIFIER_INTERFACE_H
	#include "IConfigCanFTP_Notifier.h"
#endif
#ifndef DUOTECNO_CONFIG_CANFTP_DEF_H
	#include "ConfigCanFTP_def.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
class CConfigCanFTP_FSM;			// Voorwaartse declaratie.
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/////////////////// CConfigCanFTP_State ///////////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_State
{
public:

	CConfigCanFTP_State( );
	virtual ~CConfigCanFTP_State( ) = 0;		// Pure virtual class

	virtual void enter(CConfigCanFTP_FSM* parent);
	virtual void leave(CConfigCanFTP_FSM* parent);
	virtual BOOL openFileWrite(CConfigCanFTP_FSM* parent, BYTE bNodeAddress,  const char* szFileName, const char* szInfo);
	virtual BOOL openFileRead(CConfigCanFTP_FSM* parent, BYTE bNodeAddress, const char* szFileName, const char* szInfo, BOOL flagFileNotFoundSuppressError);
	virtual BOOL abortFile(CConfigCanFTP_FSM* parent,BYTE bNodeAddress);
	virtual BOOL appendData(CConfigCanFTP_FSM* parent,BYTE bNodeAddress,BYTE* pbData, int length);
	virtual BOOL closeFile(CConfigCanFTP_FSM* parent, BYTE bNodeAddress);		
	virtual void signalFileWriteOpened(CConfigCanFTP_FSM* parent, BYTE bNodeAddress, unsigned long ulFileSize);
	virtual void signalFileReadOpened(CConfigCanFTP_FSM* parent,BYTE bNodeAddress, unsigned long ulFileSize);
	virtual void signalDataAppended(CConfigCanFTP_FSM* parent,BYTE bNodeAddress);
	virtual void signalFileClosed(CConfigCanFTP_FSM* parent,BYTE bNodeAddress,unsigned long ulFileSize);
	virtual void signalFileAborted(CConfigCanFTP_FSM* parent,BYTE bNodeAddress, unsigned long ulFileSize);
	virtual void signalError(CConfigCanFTP_FSM* parent,BYTE bNodeAddress,FtpErrorCode errorCode);
	virtual void signalDataReceived(CConfigCanFTP_FSM* parent,BYTE bNodeAddress,const BYTE* pbData,int numBytes);
	virtual void signalInfoDialogACK(CConfigCanFTP_FSM* parent,BYTE bNodeAddress);

protected:	
	enum WaitFlag
	{
		WAIT_FLAG_IDLE       = 0x00,
		WAIT_FLAG_OPEN_WRITE = 0x01,
		WAIT_FLAG_OPEN_READ  = 0x02,
		WAIT_FLAG_ACK        = 0x04
	};

	enum ActionFlag
	{
		ACTION_FLAG_IDLE	= 0x00,
		ACTION_FLAG_READ	= 0x01,
		ACTION_FLAG_WRITE	= 0x02
	};

	enum CtxFlag
	{
		// Als een bestand niet kon geopend worden omdat het niet aanwezig is
		// een foutmelding geven.
		CTXFLAG_READ_FILENOTFOUND_SUPPRESS_ERROR	= (1<<0)		// bit 1

	};

	void setFileSize(unsigned long ulSize);
	unsigned long getFileSize( ) const;

	int getWaitFlags( ) const;
	void setWaitFlag(WaitFlag flag);
	void clearWaitFlag(WaitFlag flag);
	void clearWaitFlags( );

	int getActionFlags( ) const;
	void setActionFlag(ActionFlag flag);
	void clearActionFlag(ActionFlag flag);
	void clearActionFlags( );

	int getCtxFlags( ) const;
	void setCtxFlag(CtxFlag flag);
	void clearCtxFlag(CtxFlag flag);
	void clearCtxFlags(void);


private:

	enum
	{
		WAIT_FLAGS_MASK		  = 0x07,
		ACTION_FLAGS_MASK	  = 0x03
	};

	static unsigned long m_fileSize;
	static BYTE m_waitFlags;
	static BYTE m_actionFlags;
	static BYTE m_ctxFlags;

	friend class CConfigCanFTP_FSM;
};
//////////////////// inline functions /////////////////////////////
inline void CConfigCanFTP_State::setFileSize(unsigned long ulSize)
{
	m_fileSize = ulSize;
}
inline unsigned long CConfigCanFTP_State::getFileSize( ) const
{
	return m_fileSize;
}
inline int CConfigCanFTP_State::getWaitFlags( ) const
{
	return (m_waitFlags & WAIT_FLAGS_MASK);
}
inline void CConfigCanFTP_State::setWaitFlag(WaitFlag flag)
{
	m_waitFlags |= flag;	
}
inline void CConfigCanFTP_State::clearWaitFlag(WaitFlag flag)
{
	m_waitFlags &= (flag^WAIT_FLAGS_MASK);
}
inline void CConfigCanFTP_State::clearWaitFlags( )
{
	m_waitFlags = WAIT_FLAG_IDLE;
}
inline int CConfigCanFTP_State::getActionFlags( ) const
{
	return (m_actionFlags & ACTION_FLAGS_MASK);
}
inline void CConfigCanFTP_State::setActionFlag(ActionFlag flag)
{
	m_actionFlags |= flag;	
}
inline void CConfigCanFTP_State::clearActionFlag(ActionFlag flag)
{
	m_actionFlags &= (flag^ACTION_FLAGS_MASK);
}
inline void CConfigCanFTP_State::clearActionFlags( )
{
	m_actionFlags = ACTION_FLAG_IDLE;
}

inline int CConfigCanFTP_State::getCtxFlags( ) const
{
	return m_ctxFlags;
}
inline void CConfigCanFTP_State::setCtxFlag(CtxFlag flag)
{	
	m_ctxFlags|=flag;
}
inline void CConfigCanFTP_State::clearCtxFlag(CtxFlag flag)
{
	m_ctxFlags&=~flag;
}
inline void CConfigCanFTP_State::clearCtxFlags(void)
{
	m_ctxFlags=0;
}
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
////////////////// CConfigCanFTP_StateIDLE ////////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_StateIDLE : public CConfigCanFTP_State
{
public:
	CConfigCanFTP_StateIDLE( );
	virtual ~CConfigCanFTP_StateIDLE( );

	virtual void enter(CConfigCanFTP_FSM* parent);

	virtual BOOL openFileWrite(CConfigCanFTP_FSM* parent,BYTE bNodeAddress, const char* szFileName, const char* szInfo);
	virtual BOOL openFileRead(CConfigCanFTP_FSM* parent,BYTE bNodeAddress, const char* szFileName, const char* szInfo, BOOL flagFileNotFoundSuppressError);
	virtual void signalError(CConfigCanFTP_FSM* parent,BYTE bNodeAddress, FtpErrorCode errorCode);

private:
	typedef CConfigCanFTP_State Base;
};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
///////////////// CConfigCanFTP_StateWAITOPEN /////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_StateWAITOPEN : public CConfigCanFTP_State
{
public:
	CConfigCanFTP_StateWAITOPEN( );
	virtual ~CConfigCanFTP_StateWAITOPEN( );

	virtual void enter(
		CConfigCanFTP_FSM* parent);

	virtual void signalFileWriteOpened(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress,
		unsigned long ulFileSize);

	virtual void signalFileReadOpened(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress, 
		unsigned long ulFileSize);

	virtual BOOL abortFile(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress);

	virtual void signalError(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress,
		FtpErrorCode errorCode);

	virtual void signalInfoDialogACK(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress);


private:
	typedef CConfigCanFTP_State Base;
};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
///////////////// CConfigCanFTP_StateOPEN /////////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_StateOPEN : public CConfigCanFTP_State
{
public:
	CConfigCanFTP_StateOPEN( );
	virtual ~CConfigCanFTP_StateOPEN( ) = 0;	// pure virtual

	virtual void enter(
		CConfigCanFTP_FSM* parent);

	virtual BOOL abortFile(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress);

	virtual BOOL closeFile(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress);

	virtual void signalError(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress,
		FtpErrorCode errorCode);

private:
	typedef CConfigCanFTP_State Base;
};
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
///////////////// CConfigCanFTP_StateOPENWRITE ////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_StateOPENWRITE : public CConfigCanFTP_StateOPEN
{
public:
	CConfigCanFTP_StateOPENWRITE( );
	virtual ~CConfigCanFTP_StateOPENWRITE( );

	virtual void enter(
		CConfigCanFTP_FSM* parent);

	virtual BOOL appendData(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress,
		BYTE* pbData, 
		int length);

	virtual void signalDataAppended(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress);

private:
	unsigned long m_ulPosition;

	typedef CConfigCanFTP_StateOPEN Base;
};
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
///////////////// CConfigCanFTP_StateOPENREAD /////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_StateOPENREAD : public CConfigCanFTP_StateOPEN
{
public:
	CConfigCanFTP_StateOPENREAD( );
	virtual ~CConfigCanFTP_StateOPENREAD( );

	virtual void enter(CConfigCanFTP_FSM* parent);
	virtual void signalDataReceived(CConfigCanFTP_FSM* parent,BYTE bNodeAddress,const BYTE* pbData, int numBytes);
private:
	unsigned long m_ulBytesReceived;
	typedef CConfigCanFTP_StateOPEN Base;
};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/////////////////// CConfigCanFTP_StateCLOSE //////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_StateCLOSE : public CConfigCanFTP_State
{
public:
	CConfigCanFTP_StateCLOSE( );
	virtual ~CConfigCanFTP_StateCLOSE( );

	virtual void enter(
		CConfigCanFTP_FSM* parent);

	virtual void signalFileClosed(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress,
		unsigned long ulFileSize);

	virtual void signalError(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress,
		FtpErrorCode errorCode);

	virtual void signalFileAborted(
		CConfigCanFTP_FSM* parent,
		BYTE bNodeAddress, 
		unsigned long ulFileSize);

private:
	typedef CConfigCanFTP_State Base;
};
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//////////////////// CConfigCanFTP_FSM ////////////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_FSM : public CConfigCanFTP_State
{
public:
	CConfigCanFTP_FSM(IConfigCanFTP_Notifier& notifier);
	~CConfigCanFTP_FSM( );

	void reset( );
	unsigned long getFileSize( ) const;

	BOOL openFileWrite(BYTE bNodeAddress, const char* szFileName, const char* szInfo);
	BOOL openFileRead(BYTE bNodeAddress, const char* szFileName, const char* szInfo, BOOL flagFileNotFoundSuppressError = FALSE);
	BOOL appendData(BYTE bNodeAddress,BYTE* pbData, int length);
	BOOL closeFile(BYTE bNodeAddress);
	BOOL abortFile(BYTE bNodeAddress);
	void signalFileWriteOpened(BYTE bNodeAddress, unsigned long ulFileSize);
	void signalFileReadOpened(BYTE bNodeAddress, unsigned long ulFileSize);
	void signalFileAborted(BYTE bNodeAddress, unsigned long ulFileSize);	
	void signalDataAppended(BYTE bNodeAddress);	
	void signalFileClosed(BYTE bNodeAddress,unsigned long ulFileSize);
	void signalError(BYTE bNodeAddress,FtpErrorCode errorCode);
	void signalDataReceived(BYTE bNodeAddress,const BYTE* pbData,int numBytes);
	void signalInfoDialogACK(BYTE bNodeAddress);

	void setState_IDLE( );
	void setState_WAITOPEN( );
	void setState_OPENWRITE( );
	void setState_CLOSE( );
	void setState_OPENREAD( );

	void notifyTaskDone( );
	void notifyTaskError( );
	void notifyTaskCancelled( );
	void notifyTaskStarted( );

	void tran(CConfigCanFTP_State& newState);

private:
	IConfigCanFTP_Notifier& m_notifier;
	CConfigCanFTP_State* m_pCurrentState;
	CConfigCanFTP_StateIDLE m_stateIDLE;
	CConfigCanFTP_StateWAITOPEN m_stateWAITOPEN;
	CConfigCanFTP_StateCLOSE m_stateCLOSE;
	CConfigCanFTP_StateOPENWRITE m_stateOPENWRITE;
	CConfigCanFTP_StateOPENREAD m_stateOPENREAD;
};
///////////////////////////////////////////////////////////////////
inline void CConfigCanFTP_FSM::setState_IDLE( )
{
	tran( m_stateIDLE );
}
inline void CConfigCanFTP_FSM::setState_WAITOPEN( )
{
	tran( m_stateWAITOPEN );
}
inline void CConfigCanFTP_FSM::setState_OPENWRITE( )
{
	tran( m_stateOPENWRITE );
}
inline void CConfigCanFTP_FSM::setState_CLOSE( )
{
	tran( m_stateCLOSE );
}
inline void CConfigCanFTP_FSM::setState_OPENREAD( )
{
	tran( m_stateOPENREAD );
}
inline void CConfigCanFTP_FSM::notifyTaskDone( )
{
	m_notifier.signalTaskDone();
}
inline void CConfigCanFTP_FSM::notifyTaskError( )
{
	m_notifier.signalTaskError();
}
inline void CConfigCanFTP_FSM::notifyTaskCancelled( )
{
	m_notifier.signalTaskCancelled();
}
inline void CConfigCanFTP_FSM::notifyTaskStarted( )
{
	m_notifier.signalTaskStarted( );
}
inline unsigned long CConfigCanFTP_FSM::getFileSize( ) const
{
	return m_pCurrentState->getFileSize( );
}
///////////////////////////////////////////////////////////////////
#endif