#ifndef INCLUDED_TCPSOCKETBASE_TCPSOCKETWIN_H
#define INCLUDED_TCPSOCKETBASE_TCPSOCKETWIN_H
/*****************************************************************/

struct ITCPSocketWinHost
{
	virtual ~ITCPSocketWinHost() = 0;

	virtual void onSocketConnected(BOOL status) = 0;
	virtual void onDataRecieved(const BYTE* pbData,int length) = 0;
	virtual void onMsgInfo(const CString& str) = 0;
	virtual void onMsgError(const CString& str) = 0;
	virtual void onTracerDataSend(const CString& msg) = 0;
	virtual void onTracerDataRecieved(const CString& msg) = 0;	
	virtual void onRequestSendKeepAlive( ) = 0;		// zie NOTE_DISCONNECT_WINSOCK en NOTE_WINSOCK_KEEPALIVE
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
enum {
	WM_MSG_HOSTTOIPADDRESS = WM_USER + 5000,
	WM_MSG_SOCKETEVENTS,
	WM_MSG_TEST
};

// De TCP/IP Socket...
// Afgeleiden van CWND zodat we een messgaeQueue hebben.
// Ook nodig voor de Timer.
class CTCPSocketWin  : public CWnd
{
public:
	CTCPSocketWin(ITCPSocketWinHost& refHost);
	virtual ~CTCPSocketWin();

	typedef enum {
		RESULT_SUCCESS = 0,
		RESULT_EC_GENERAL = -1,
		RESULT_EC_PARAMETERS = -2,
		RESULT_EC_SOCKET  = -3,
		RESULT_EC_CONNECT = -4,
		RESULT_EC_STATE = -5
	} Result_t;

	
	HWND Create();	// Window create functie.
	Result_t connect(const char* hostName,int socketport);	
	Result_t send(const BYTE* pbData,int length);
	Result_t close();

	BOOL connected() const { return (STATE_CONNECTED == m_state); }
	const CString& getErrorMsg() const {
		return m_ErrorMsg;
	}
	const CString& getErrorMsgDetail() const {
		return m_ErrorMsgDetail;
	}

	void setTracerRxOnOff(BOOL state) { m_stateTracerRX = state; }
	void setTracerTxOnOff(BOOL state) { m_stateTracerTX = state; }

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	LONG onMsgHostNameToIPAddress(WPARAM wParam,LPARAM lParam);
	LONG onMsgSocketEvents(WPARAM wParam,LPARAM lParam);	

	virtual void PostNcDestroy();
	Result_t connect(const struct sockaddr_in& addr);

	enum { 
		// Timer die ervoor zorgt dat er een heartbeat request bericht verstuurd wordt wanneer er 
		// geen andere data over de socket ontvangen wordt.
		TIMER_ID_TX_KEEP_ALIVE = 1,
		TIMER_VALUE_TX_KEEP_ALIVE_MSEC = (10 * 1000),									

		// Timer die controleert of er data ontvangen wordt.
		TIMER_ID_RX_KEEP_ALIVE = 2,
		TIMER_VALUE_RX_KEEP_ALIVE_MSEC = (3 * TIMER_VALUE_TX_KEEP_ALIVE_MSEC)
	};

	void startTimerTxKeepAlive( );
	void stopTimerTxKeepAlive( );

	void startTimerRxKeepAlive();
	void stopTimerRxKeepAlive();


private:
	enum { MAX_RX_BUFFER = 1024 };

	typedef enum {
		STATE_DISCONNECTED = 0,
		STATE_CONNECT_WAIT_WRITE,
		STATE_CONNECTED,
	} State_t;

	ITCPSocketWinHost& m_refHost;
	State_t m_state;
	SOCKET m_mysocket;	
	HANDLE m_hCurrentTask;
	BYTE* m_pBufferHostName;
	BYTE* m_pRXDataBuffer;
	struct sockaddr_in m_destSockAddr;
	CString m_ErrorMsg;
	CString m_ErrorMsgDetail;
	BOOL m_stateTracerTX;
	BOOL m_stateTracerRX;
};
/*****************************************************************/
#endif 
/*****************************************************************/