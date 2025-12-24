#include "stdafx.h"
#include "tcpsocketwin.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
BEGIN_MESSAGE_MAP(CTCPSocketWin, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_MSG_SOCKETEVENTS, onMsgSocketEvents)
	ON_MESSAGE(WM_MSG_HOSTTOIPADDRESS, onMsgHostNameToIPAddress)
END_MESSAGE_MAP()
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static CString getWindowsErrorMsg(DWORD dwError) {
	
	LPSTR messageBuffer = NULL;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    const size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, dwError, 
								 //MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
								 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
								 (LPSTR)&messageBuffer, 0, NULL);
    
	const CString retValue(messageBuffer);

	//Free the Win32's string's buffer.
    LocalFree(messageBuffer);
	return retValue;
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
ITCPSocketWinHost::~ITCPSocketWinHost() { }

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketWin::CTCPSocketWin(ITCPSocketWinHost& refHost) : m_refHost(refHost), 
	m_state(STATE_DISCONNECTED), m_mysocket(NULL), m_hCurrentTask(NULL), m_pBufferHostName(NULL), m_pRXDataBuffer(NULL),
	m_ErrorMsg(), m_ErrorMsgDetail(), m_stateTracerTX(FALSE), m_stateTracerRX(FALSE) { 
	memset(&m_destSockAddr, 0, sizeof(m_destSockAddr));		
}

CTCPSocketWin::~CTCPSocketWin() 
{ 
	// Todo: Close handles: currentTask ??
	if (NULL != m_mysocket) {
		::closesocket(m_mysocket);
		m_mysocket = NULL;
	}	
	if (NULL != m_pBufferHostName) {
		delete [] m_pBufferHostName;
		m_pBufferHostName = NULL;
	}
	if (NULL != m_pRXDataBuffer) {
		delete [] m_pRXDataBuffer;
		m_pRXDataBuffer = NULL;
	}
}


HWND CTCPSocketWin::Create()
{		
	LPCTSTR classname = AfxRegisterWndClass(0);
	CWnd::CreateEx(NULL,classname,NULL,NULL,1,1,1,1,NULL,NULL);
	ASSERT(m_hWnd!=NULL);		
	return m_hWnd;
}


void CTCPSocketWin::PostNcDestroy() {	
	delete(this);
}

static BOOL isIpV4Address(const char* strHostName) {
	if (NULL == strHostName) {
		ASSERT(NULL != strHostName);
		return FALSE;
	}
	if (0 != isalpha(strHostName[0])) {
		return FALSE;
	}
	int t[4] = {0,0,0,0};
	if (4 != sscanf(strHostName,"%d.%d.%d.%d",&t[0],&t[1],&t[2],&t[3])) {
		return FALSE;
	}
	for (int i=0;i<4;++i) {
		if (t[i]<0 || t[i]>255) {
			return FALSE;
		}
	}
	return TRUE;
}


CTCPSocketWin::Result_t CTCPSocketWin::connect(const char* strHostName,int socketport) {

	m_ErrorMsg.Empty();
	m_ErrorMsgDetail.Empty();

	if (NULL == strHostName || socketport<0) {
		ASSERT(NULL != strHostName);
		ASSERT(socketport>=0);
		m_ErrorMsg = CString("Connect failure - Wrong parameters for hostname or port");
		return RESULT_EC_PARAMETERS;
	}

	if (STATE_DISCONNECTED != m_state) {
		m_ErrorMsg = CString("Connect failure - TCP/IP socket in wrong state - already connected ?");
		return RESULT_EC_CONNECT;
	}

	Result_t result = RESULT_EC_GENERAL;
	if (FALSE == isIpV4Address(strHostName)) {
		// Opvragen het IP-Address via DNS look-up.
		// Aanmaken van een buffer om het ip-address in op te slaan...
		// In dit buffer zit de hostInfo - Buffer wordt gewist na het ontvangen van de info.
		static const int bufferLength = MAXGETHOSTSTRUCT * sizeof(BYTE);
		if (NULL == m_pBufferHostName) {
			m_pBufferHostName = new BYTE[bufferLength];
		}
		m_hCurrentTask = WSAAsyncGetHostByName(this->GetSafeHwnd(), WM_MSG_HOSTTOIPADDRESS, strHostName, (char*) m_pBufferHostName, bufferLength);
		if (NULL != m_hCurrentTask) {
			memset(&m_destSockAddr,0,sizeof(m_destSockAddr));
			m_destSockAddr.sin_family = AF_INET;
			m_destSockAddr.sin_port = htons(socketport);
			// m_destSockAddr.sin_addr = addr;			<-- Deze wordt ingevuld wanneer we de info hebben...			
			{
				CString msgInfo;
				msgInfo.Format("Connect - look-up dns for hostname '%s'",strHostName);
				m_refHost.onMsgInfo(msgInfo);
			}
			result = RESULT_SUCCESS;		
		}
		else {
			m_ErrorMsg = CString("Connect failure - TCP/IP socket failed to lookup dns info.");
			return RESULT_EC_SOCKET;
		}		
	}
	else {				
		struct in_addr addr;
		memset(&addr,0,sizeof(addr));
		const unsigned long ulAddr = inet_addr(strHostName);
		memcpy(&addr,&ulAddr,sizeof(addr));

		memset(&m_destSockAddr,0,sizeof(m_destSockAddr));
		m_destSockAddr.sin_family = AF_INET;
		m_destSockAddr.sin_port = htons(socketport);
		m_destSockAddr.sin_addr = addr;	
		
		result = connect(m_destSockAddr);	
		if (RESULT_SUCCESS == result) {
			CString msgInfo;
			msgInfo.Format("Connect - trying to connect to %s:%d",strHostName,socketport);
			m_refHost.onMsgInfo(msgInfo);	
		}
	}
	return result;
}

CTCPSocketWin::Result_t CTCPSocketWin::send(const BYTE* pbData,int length) {

	m_ErrorMsg.Empty();
	m_ErrorMsgDetail.Empty();

	if (NULL == pbData || length<=0) {
		m_ErrorMsg = CString("send - Cannot send the data due to wrong parameters");
		return RESULT_EC_PARAMETERS;
	}
	if (m_state != STATE_CONNECTED) {
		m_ErrorMsg = CString("send - Cannot send the data because the socket is not connected.");
		return RESULT_EC_STATE;
	} 
	if (NULL == m_mysocket) {
		m_ErrorMsg = CString("send - Cannot send the data because the handle to the socket is invalid.");
		ASSERT(NULL != m_mysocket);
		return RESULT_EC_SOCKET;
	}
	
	CTCPSocketWin::Result_t res = RESULT_EC_GENERAL;
	const int error = ::send(m_mysocket,(const char*) pbData,length,0);
	if (SOCKET_ERROR != error) {
		if (m_stateTracerTX) {
			const CString msg(pbData);
			m_refHost.onTracerDataSend(msg);
		}
		res = RESULT_SUCCESS;
	}
	else {	
		const DWORD dwError = GetLastError();				
		m_ErrorMsg = CString("send - Cannot send the data because the send() failed.");
		m_ErrorMsgDetail = getWindowsErrorMsg(dwError);

		if (WSAECONNRESET == dwError) {	/* WSAECONNRESET = 10054 */
			// zie NOTE_DISCONNECT_WINSOCK
			// Sedert V16.07: "De externe host heeft een verbinding verbroken."
			res = RESULT_EC_CONNECT;
		}
		else {
			res = RESULT_EC_GENERAL;
		}
	}
	return res;
}

CTCPSocketWin::Result_t CTCPSocketWin::close() {

	m_ErrorMsg.Empty();
	m_ErrorMsgDetail.Empty();

	if (m_state == STATE_DISCONNECTED) {
		m_ErrorMsg = CString("close - Already in state disconnected.");
		return RESULT_EC_STATE;
	}	
	if (NULL == m_mysocket) {
		m_ErrorMsg = CString("close - no handle to the socket.");
		return RESULT_EC_SOCKET;
	}
	
	const int error = ::closesocket(m_mysocket);
	if (SOCKET_ERROR == error) {
		const DWORD dwError = GetLastError();
		m_ErrorMsg = CString("close - closesocket failed.");
		m_ErrorMsgDetail = getWindowsErrorMsg(dwError);
		return RESULT_EC_SOCKET;
	}
	m_mysocket = NULL;	
	m_refHost.onSocketConnected(FALSE);
	m_state = STATE_DISCONNECTED;	
	stopTimerTxKeepAlive();
	stopTimerRxKeepAlive();
	return RESULT_SUCCESS;
}

CTCPSocketWin::Result_t CTCPSocketWin::connect(const struct sockaddr_in& addr) {
	// Moeten een geldige socket address & poort hebben.
	ASSERT(addr.sin_family == AF_INET);
	ASSERT(addr.sin_port != 0);
	
	m_mysocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == m_mysocket) {		
		const DWORD dwError = GetLastError();		
		m_ErrorMsg = CString("Connect failure - TCP/IP socket failed to connect - could not create the socket.");	
		m_ErrorMsgDetail = getWindowsErrorMsg(dwError);	
		return RESULT_EC_SOCKET;	
	}

	// Bij de connect gaan we eerst enkel de events FD_CONNECT & FD_WRITE toelaten.
	// Bij het ontvangen van de FD_WRITE gaan we de FD_READ en FD_CLOSE toelaten...
	if (WSAAsyncSelect(m_mysocket,this->GetSafeHwnd(),WM_MSG_SOCKETEVENTS,FD_CONNECT | FD_WRITE) != 0) {	
		const DWORD dwError = GetLastError();	
		m_ErrorMsg = CString("Connect failure - TCP/IP socket failed to connect - could not set select events.");		
		m_ErrorMsgDetail = getWindowsErrorMsg(dwError);
		return RESULT_EC_GENERAL;
	}

	// win 32 socket connect message...
	Result_t result = RESULT_EC_GENERAL;
	if (::connect(m_mysocket,(sockaddr*) &addr,sizeof(SOCKADDR_IN)) != 0) {	
		const DWORD dwError = GetLastError();	
		if (WSAEWOULDBLOCK != dwError) {			
			m_ErrorMsg = CString("Connect failure - TCP/IP socket failed to connect.");		
			m_ErrorMsgDetail = getWindowsErrorMsg(dwError);		
			result = RESULT_EC_SOCKET;
		}
		else {		
			result = RESULT_SUCCESS;
		}
	}
	return result;
}

// NOTE_WINSOCK_KEEPALIVE:
// Principe:
//	-TIMER_ID_TX_KEEP_ALIVE:
//		Wanneer er 10 seconden geen data ontvangen wordt via de socket dan wordt er een hearbeat request bericht
//		verstuurd naar de homeserver.Op dit bericht moet de homeserver antwoorden met een heartbeat response.
//	- TIMER_ID_RX_KEEP_ALIVE
//		Wanneer er 30 seconden geen data ontvangen wordt dan wordt de socket automatisch gesloten.
//
void CTCPSocketWin::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_TX_KEEP_ALIVE == nIDEvent) {
		m_refHost.onRequestSendKeepAlive();		
		KillTimer(TIMER_ID_TX_KEEP_ALIVE);
	}
	else if (TIMER_ID_RX_KEEP_ALIVE == nIDEvent) {
		if (RESULT_SUCCESS == this->close()) {
			m_ErrorMsg = "Socket closed - heartbeat timeout.";
			m_refHost.onMsgError(m_ErrorMsg);
		}
		KillTimer(TIMER_ID_RX_KEEP_ALIVE);
	}
}

// TIMER_ID_TX_KEEP_ALIVE
// Timer die ervoor zorgt dat er een heartbeat request bericht verstuurd wordt wanneer er geen andere data over de socket ontvangen wordt.
void CTCPSocketWin::startTimerTxKeepAlive( ) {
	SetTimer(TIMER_ID_TX_KEEP_ALIVE, TIMER_VALUE_TX_KEEP_ALIVE_MSEC,NULL);
}
void CTCPSocketWin::stopTimerTxKeepAlive( ) {
	KillTimer(TIMER_ID_TX_KEEP_ALIVE);
}

// TIMER_ID_RX_KEEP_ALIVE
// Timer die controleert of er data over de socket ontvangen wordt.
// De 'TIMER_ID_TX_KEEP_ALIVE' zorgt ervoor dat er periodiek data moet ontvangen worden. 
void CTCPSocketWin::startTimerRxKeepAlive() {
	SetTimer(TIMER_ID_RX_KEEP_ALIVE, TIMER_VALUE_RX_KEEP_ALIVE_MSEC, NULL);
}
void CTCPSocketWin::stopTimerRxKeepAlive() {
	KillTimer(TIMER_ID_RX_KEEP_ALIVE);
}

/*********************** protected window messages ***************/
LONG CTCPSocketWin::onMsgSocketEvents(WPARAM wParam,LPARAM lParam)
{
	if (0 != WSAGETASYNCERROR(lParam)) {		
		const DWORD dwError = WSAGETASYNCERROR(lParam);				
		if (STATE_DISCONNECTED == m_state || STATE_CONNECT_WAIT_WRITE == m_state) {
			m_ErrorMsg = CString("Failed to connect - Server not online ?");	
			m_refHost.onSocketConnected(FALSE);		// Zo weet de parent dat de connect mislukt is...
		}
		else {
			if (WSAECONNABORTED == dwError) {
				// NOTE_DISCONNECT_WINSOCK
				// De windows host heeft de connectie met de server gesloten omdat er wellicht 
				// geen verbinding meer is. 
				// Bij de volgende send() via de socket zal er een socket fout 10054 optreden.
				// Deze fout kan gebruikt worden voor de client om een melding te geven naar de 
				// applicatie.
				m_ErrorMsg = CString("Error - Socket events - socket connected aborted (Server no longer online?)");

				// NOTE_DISCONNECT_WINSOCK
				// Vragen aan de host om een keep alive bericht te versturen.
				// Wanneer de socket verbroken werd gaan we hier een fout op krijgen.
				m_refHost.onRequestSendKeepAlive();
			}
			else {
				m_ErrorMsg = CString("Error - Socket events");				
			}
		}
		m_ErrorMsgDetail = getWindowsErrorMsg(dwError);		
		m_refHost.onMsgError(m_ErrorMsg);
		return TRUE;
	}

	const WORD wEvent = WSAGETSELECTEVENT(lParam);
	switch(wEvent) 
	{
		case FD_CONNECT:
		{			
			if (STATE_DISCONNECTED == m_state) {			
				// Mogen dit buffer opnieuw wissen: 
				// Was enkel nodig voor de conversie van DNS naar Ip-Address - Momenteel niet meer nodig...
				if (NULL != m_pBufferHostName) {
					delete [] m_pBufferHostName;
					m_pBufferHostName = NULL;
				}				
				m_state = STATE_CONNECT_WAIT_WRITE;
			}
		} break;

		case FD_WRITE:
		{		
			if (STATE_CONNECT_WAIT_WRITE == m_state) {						
				// Rx Data buffer alloceren: Deze wordt in de FD_READ event gebruikt...
				if (NULL == m_pRXDataBuffer) {
					m_pRXDataBuffer = new BYTE[MAX_RX_BUFFER];
				}

				// Nieuwe events voor lezen van data...
				int res = WSAAsyncSelect(m_mysocket,this->GetSafeHwnd(), WM_MSG_SOCKETEVENTS, FD_WRITE| FD_READ | FD_CLOSE);
				if (0 != res) {								
					const DWORD dwError = GetLastError();		
					m_ErrorMsg = CString("Error - Socket events - Could not select new socket events.");	
					m_ErrorMsgDetail = getWindowsErrorMsg(dwError);
					m_refHost.onMsgError(m_ErrorMsg);
					return TRUE;
				} 

				// Starten van de TX Keep alive timer.				
				startTimerTxKeepAlive();		

				// deze als eerste voor we de client op de hoogte brengen.
				m_state = STATE_CONNECTED;		
				
				// Notify de parent: Oppassen: Moeten eerst de socket in de toestand Connected zetten.
				m_refHost.onSocketConnected(TRUE);

				{
					const CString msg = CString("Connect - connected to the server socket.");			
					m_refHost.onMsgInfo(msg);
				}
			}
			else if (STATE_CONNECTED == m_state) { /* absorbe */ }
		} break;

		case FD_READ: 
		{
			if (STATE_CONNECTED == m_state) {

				if (NULL == m_pRXDataBuffer) {
					// Data buffer wordt gealloceerd in state STATE_CONNECT_WAIT_WRITE
					m_ErrorMsg = CString("Error - Socket events - NO buffer allocated to recieve data.");
					m_refHost.onMsgError(m_ErrorMsg);
					ASSERT(NULL != m_pRXDataBuffer);	
					return TRUE;
				}
							
				memset(m_pRXDataBuffer,'\0',MAX_RX_BUFFER * sizeof(BYTE));										
				const DWORD dwResult = recv(m_mysocket,(char*) m_pRXDataBuffer,MAX_RX_BUFFER-1,0);
				if ((SOCKET_ERROR != dwResult) && (dwResult<MAX_RX_BUFFER) && (dwResult>0)) {

					// Keep alive timers stoppen.
					stopTimerRxKeepAlive();
					stopTimerTxKeepAlive();

					m_pRXDataBuffer[dwResult] = '\0';

					// Oppassen: Wanneer de tracer aanligt moeten we de 0 char uit de data halen en 
					// vervangen door spaties. Enkel de het NULL char op het einde van alle ontvangen data
					// mag blijven.
					if (m_stateTracerRX) {
						BYTE* pbTempBuffer = new BYTE[dwResult+1];
						if (NULL != pbTempBuffer) {
							memset(pbTempBuffer,'\0',dwResult+1);
							for (int i=0;i<(int)dwResult;++i) {
								if (m_pRXDataBuffer[i] == '\0') {
									pbTempBuffer[i] = ' ';	// vervangen door een spatie...
								}
								else {
									pbTempBuffer[i] = m_pRXDataBuffer[i];
								}
							}					
							const CString msg(pbTempBuffer);
							m_refHost.onTracerDataRecieved(msg);
							delete [] pbTempBuffer;
						}
					}					
					m_refHost.onDataRecieved(m_pRXDataBuffer,dwResult);					

					// Keep alive timers opnieuw starten.
					startTimerTxKeepAlive();
					startTimerRxKeepAlive();
				}
				else {
					const DWORD dwError = GetLastError();	
					m_ErrorMsgDetail = getWindowsErrorMsg(dwError);
					if (SOCKET_ERROR == dwResult) {						
						m_ErrorMsg = CString("Failed to read data because of a socket error");						
					}
					else if (dwResult>=MAX_RX_BUFFER) {					
						m_ErrorMsg = CString("Failed to read data because the buffer is to small");					
					}
					else if (dwResult<=0) {					
						m_ErrorMsg = CString("Failed to read data");						
					}
					m_refHost.onMsgError(m_ErrorMsg);
				}								
			} 					  
		} break;

		case FD_CLOSE:
		{												
			if (NULL != m_mysocket) {					
				::closesocket(m_mysocket);
				m_mysocket = NULL;		
			}
			if (NULL != m_pRXDataBuffer) {
				delete [] m_pRXDataBuffer;
				m_pRXDataBuffer = NULL;
			}
			if (STATE_DISCONNECTED != m_state) {
				m_refHost.onSocketConnected(FALSE);
				m_state = STATE_DISCONNECTED;		
			}
			stopTimerTxKeepAlive( );
			stopTimerRxKeepAlive( );
		} break;
	}	
	return TRUE;
}

// Antwoord opvragen van de dns
LONG CTCPSocketWin::onMsgHostNameToIPAddress(WPARAM wParam,LPARAM lParam) 
{
	if (m_hCurrentTask == (HANDLE) wParam) {	
		if (NULL == m_pBufferHostName) {
			m_ErrorMsgDetail.Empty();
			m_ErrorMsg = CString("Connect failure - Dns lookup failed - No buffer allocated");				
			m_refHost.onMsgError(m_ErrorMsg);			
			ASSERT(0 != m_pBufferHostName);				
			return TRUE;
		}

		const WORD error = WSAGETASYNCERROR(lParam);
		if (0 == error) {			
			struct in_addr addr;			
			LPHOSTENT hpHost;
			hpHost = (LPHOSTENT) m_pBufferHostName;			
			memset(&addr,0,sizeof(addr));
			addr = (*(struct in_addr *)hpHost->h_addr);			
			m_destSockAddr.sin_addr = addr;			// De andere velden werden reeds ingevuld...			
			const CTCPSocketWin::Result_t res = connect(m_destSockAddr);
			if (RESULT_SUCCESS == res) {
				CString msg;
				msg.Format("Connect - Dns lookup ok - trying to connnect to the server");
				m_refHost.onMsgInfo(msg);
			}
			else {
				m_ErrorMsgDetail.Empty();
				m_ErrorMsg = CString("Connect - Dns lookup ok - connect failed");
				m_refHost.onMsgError(m_ErrorMsg);	
				m_refHost.onSocketConnected(FALSE);
			}
		}
		else {
			const DWORD dwError = GetLastError();		
			m_ErrorMsg = CString("Connect failure - Dns lookup failed");	
			m_refHost.onMsgError(m_ErrorMsg);						
			m_ErrorMsgDetail = getWindowsErrorMsg(dwError);	
			m_refHost.onSocketConnected(FALSE);
		}	
	}
	return TRUE;
}
/*****************************************************************/