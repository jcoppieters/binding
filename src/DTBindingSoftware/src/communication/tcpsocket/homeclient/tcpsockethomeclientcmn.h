#ifndef INCLUDED_TCPSOCKET_TCPSOCKETHOMECLIENTCMN_H
#define INCLUDED_TCPSOCKET_TCPSOCKETHOMECLIENTCMN_H
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
struct TcpSocketHomeClient_Tasks
{
	typedef enum { 	/* Oppassen: Waarden niet aanpassen - dezelfde als in protocol.*/
		TASK_BROWSE = 0, TASK_BROWSE_CLEAR = 1, TASK_PNP = 2, TASK_UPDATE_DBASE = 3,
		TASK_RESETALL = 4, TASK_FACTORY_RESETALL = 5,
	} Task_t;

	typedef enum {	/* Oppassen: Waarden niet aanpassen - dezelfde als in protocol.*/
		STATE_IDLE = 0, STATE_BUSY_PNP = 1, STATE_BUSY_BROWSE = 2,
		STATE_BUSY_UPDATEDBASE = 3, STATE_BUSY_GENERAL = 4
	} State_t;

	typedef enum {	/* Oppassen: Waarden niet aanpassen - dezelfde als in protocol.*/
		STATUS_FAILED = 0, STATUS_OK = 1
	} Status_t;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
struct TcpSocketHomeClient_NodeDbase {
	typedef enum {
		STATUS_EMPTY = 0, STATUS_BUSY = 1, STATUS_READY = 2
	} Status_t;
};
/**************************************************************/


#if(USE_HOMECLIENT_STANDALONE == 0)

	typedef enum {

		SOCKET_APPTYPE_UNKNOWN = -1,
		SOCKET_APPTYPE_COMMOBJECT = 0,		// Modem (& homeclient)
		SOCKET_APPTYPE_HOMECLIENT = 1,		// Afzonderlijke socket voor de homelient.

	} SocketAppType_t;

#endif

/**************************************************************/
#endif
/**************************************************************/