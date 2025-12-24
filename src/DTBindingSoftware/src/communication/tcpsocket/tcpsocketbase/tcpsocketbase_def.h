#ifndef INCLUDED_TCPSOCKET_BASE_DEF_H
#define INCLUDED_TCPSOCKET_BASE_DEF_H
/*****************************************************************/

typedef enum {
	SOCKET_STATE_DISCONNECTED = 0,
	SOCKET_STATE_WAIT_SOCKET_CONNECTED,
	SOCKET_STATE_CONNECT_WAIT_LOGIN,
	SOCKET_STATE_CONNECT_LOGIN_SUCCESS			// Paswoord + Subscribe CAN-MSG is ok.
} SocketConnectState_t;

/*****************************************************************/
#endif
/*****************************************************************/
