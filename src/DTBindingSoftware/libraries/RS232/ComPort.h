// ComPort.h: interface for the CComPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPORT_H__66C79C73_1BBA_4668_AE21_A3E05F8FDB52__INCLUDED_)
#define AFX_COMPORT_H__66C79C73_1BBA_4668_AE21_A3E05F8FDB52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)	
//////////////////////////////////////////////////////////////////////


#include "ReaderThread.h"
#include "WriterThread.h"

#include "BuildingBlocks_Include.h"


///////////////////////////////////////////////////////////////////////////////
namespace DUOTECNO {
namespace RS232 {
///////////////////////////////////////////////////////////////////////////////

typedef TQueueSimpleCirc<1024> CRXMessageQueue;
typedef TQueueSimpleCirc<1024> CTXMessageQueue;

///////////////////////////////////////////////////////////////////////////////

class CComportSettings
{
public:
	//enum { MAX_PORTS = 10, };
	enum { MAX_PORTS = 255, };

protected:
	int m_Port;
	unsigned long m_ulBaudrate;

public:
	CComportSettings() : m_Port(-1), m_ulBaudrate(9600) {};
	virtual ~CComportSettings() {};

	void set_Comport(int port);

	void set_Baudrate(unsigned long ulBaudrate) {
		m_ulBaudrate = ulBaudrate;
	}
	int get_Comport( ) const {
		return m_Port;
	}
	unsigned long get_Baudrate() const {
		return m_ulBaudrate;
	}
};

///////////////////////////////////////////////////////////////////////////////

class CComportStatus
{
protected:
	HANDLE m_hfile;
	BOOL m_fConnected;	

public:
	CComportStatus() : m_hfile((HANDLE) -1) , m_fConnected(FALSE) { };
	virtual ~CComportStatus() { };

	void set_Handle(HANDLE file) {
		m_hfile = file;
	}
	HANDLE get_Handle() const {
		return m_hfile;
	}
	BOOL valid_Handle() const {
		return (m_hfile != (HANDLE)-1);
	}

	BOOL get_Connected() const {
		return (m_fConnected);
	}
	void set_Connected(BOOL flag) {
		m_fConnected = flag;
	}
};


///////////////////////////////////////////////////////////////////////////////

class CComPort;


class CComPortThreads
{
protected:
	CComPort&		m_Parent;
	CReaderThread	m_Reader;
	CWriterThread	m_Writer;

public:
	CComPortThreads(CComPort& parent);
	virtual ~CComPortThreads( );

	void Start( );
	void Stop( );	
	void Sleep( );

	CWriterThread& get_Writer() {
		return m_Writer;
	}
};

///////////////////////////////////////////////////////////////////////////////

class CComportResources
{
protected:
	BYTE bTransmitBuffer[512];
	BYTE bReceiveBuffer[512];

	CTXMessageQueue m_txQueue;
	CRXMessageQueue m_rxQueue;

public:
	CComportResources() { };
	virtual ~CComportResources() { };

	void Reset( ) 
	{
		//m_TxQueue.Flush();
		//m_RxQueue.Flush();
	}

	DUOTECNO::RS232::CTXMessageQueue& get_TxQueue() {
		return m_txQueue;
	}
	const DUOTECNO::RS232::CTXMessageQueue& get_TxQueue() const {
		return m_txQueue;
	}
	DUOTECNO::RS232::CRXMessageQueue& get_RxQueue() {
		return m_rxQueue;
	}
	const DUOTECNO::RS232::CRXMessageQueue& get_RxQueue() const {
		return m_rxQueue;
	}

	BYTE* get_TxBuffer() {
		return &bTransmitBuffer[0];
	}
	const BYTE* get_TxBuffer() const {
		return &bTransmitBuffer[0];
	}
	void clear_TxBuffer() {
		memset(bTransmitBuffer,0,512);
	}
	
	BYTE* get_RxBuffer() {
		return &bReceiveBuffer[0];
	}
	const BYTE* get_RxBuffer() const {
		return &bReceiveBuffer[0];
	}
};



///////////////////////////////////////////////////////////////////////////////

class CComportEvents
{
protected:
	CComPort&	m_Parent;
	HWND		m_hWndEventSink;
	UINT		m_MessageID;	

public:
	CComportEvents(CComPort& parent);
	virtual ~CComportEvents();

	void OnMessageReceived(const BYTE* pbData, int length);
	void OnError(DWORD dwError);

	void subscribe(HWND hwnd, UINT Id);

};

//////////////////////////////////////////////////////////////////////////////

class CThreadSync
{
public:
	DUOTECNO::KERNEL::WIN_32::CEvent m_DataReady;
	DUOTECNO::KERNEL::WIN_32::CEvent m_ProccesingDone;
	DUOTECNO::KERNEL::WIN_32::CEvent m_DispatchDone;


public:
	CThreadSync( );
	virtual ~CThreadSync( );
};

//////////////////////////////////////////////////////////////////////////////

class CComPortActions
{
public: 
	enum PURGE_ACTIONS			// TM 20081222
	{
		PURGE_RX,
		PURGE_TX,
		PURGE_ALL
	};

protected:
	CComPort& m_Parent;

public:
	CComPortActions(CComPort& parent);
	virtual ~CComPortActions( );

	void ClearErrors( );
	void PurgeComm(PURGE_ACTIONS flags = PURGE_ALL);	// TM 20081222
	void HandleErrors( );
};

//////////////////////////////////////////////////////////////////////////////



class CComPort  
{
protected:
	CComPortThreads		m_Threads;
	CComportSettings	m_Settings;
	CComportStatus		m_Status;
	CComportResources	m_Resources;
	CComportEvents		m_Events;
	CThreadSync			m_ThreadSync;

public:
	CComPort();
	virtual ~CComPort();

	BOOL Connected( ) const;
	BOOL Open( );
	BOOL Close( );
	BOOL Send(BYTE* pbBuffer,int length);
	
	int Get(BYTE** pbBuffer);

public:
	CComportSettings& get_Settings() {
		return m_Settings;
	}
	const CComportSettings& get_Settings() const {
		return m_Settings;
	}
	CComportStatus& get_Status() {
		return m_Status;
	}
	const CComportStatus& get_Status() const {
		return m_Status;
	}
	CComportResources& get_Resources() {
		return m_Resources;
	}
	const CComportResources& get_Resources() const {
		return m_Resources;
	}
	CComportEvents& get_Events() {
		return m_Events;
	}
	CThreadSync& get_ThreadSync() {
		return m_ThreadSync;
	}

protected:
	BOOL SetupConnection( );
	BOOL CloseConnection( );

};


/////////////////////////////////////////////////////////////////////////////////
};
};
/////////////////////////////////////////////////////////////////////////////////
#endif




#endif // !defined(AFX_COMPORT_H__66C79C73_1BBA_4668_AE21_A3E05F8FDB52__INCLUDED_)
