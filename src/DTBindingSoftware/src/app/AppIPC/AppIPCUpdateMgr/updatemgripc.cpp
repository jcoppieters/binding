#include "stdafx.h"

#include "updatemgripc.h"

// Nodig voor de winsock library...
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") 

/**************************************************************/
#define DEBUG_LOCAL_ON		0	// zie BUG-0140: stond nog op 1 ???
/**************************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define TRACER(x)		TRACE x
#else
	#define TRACER(x)
#endif
/**************************************************************/


// private
static int winsockInit(void)
{
	WSADATA wsa;
	int retVal = -1;

	// zie BUG-0140:
	// Extra controle of de winsock library de correcte versie is...

	if (WSAStartup(MAKEWORD(2,2),&wsa) == 0)
	{
		if (LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wVersion) != 2) 
		{
			TRACER(("Wrong version\r\n"));
			WSACleanup( );
		}
		else
		{
			retVal = 0;
		}
	}
	else
    {
        TRACER(("Failed. Error Code : %d",WSAGetLastError()));        
    }
	return retVal;
}

static int createUDPSocket(void)
{
	const int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sd)
	{
		TRACER(("socket() failed with error code : %d" , WSAGetLastError()));		
	}
	return sd;
}

static int sendUpdateString(
	int sd,
	const char* szIPAddress,
	int port,
	const char* szMessage)	
{

	struct sockaddr_in si_other;
	int result = -1;
	const int slen = sizeof(si_other);

	memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    si_other.sin_addr.S_un.S_addr = inet_addr(szIPAddress);	 

	result = sendto(sd, 
					szMessage, 
					strlen(szMessage), 
					0, 
					(struct sockaddr *) &si_other, 
					slen);

	if (SOCKET_ERROR == result)
	{
		TRACER(("sendto() failed with error code : %d" , WSAGetLastError()));		
	}

	return (result == SOCKET_ERROR ? -1 : 0);
}

static void destroyUDPSocket(int sd)
{
	if (-1 != sd)
	{
		closesocket(sd);
	}
}


// public
int updatemgripc_SendUpdateString(void)
{
	int result = -1;

	if (0 == winsockInit())
	{
		const int sd = createUDPSocket( );
		if (SOCKET_ERROR != sd)
		{
			const char* const szMessage = "Update";
			const char* const szIPAddress = "127.0.0.1";
			const int port = 45301;

			result = sendUpdateString(sd,szIPAddress,port,szMessage);

			destroyUDPSocket(sd);
		}
	}

	return result;
}