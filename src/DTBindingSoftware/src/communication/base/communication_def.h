#ifndef INCLUDED_COMMUNICATION_DEF_H
#define INCLUDED_COMMUNICATION_DEF_H
/*****************************************************************/
typedef enum {
	CONNECTION_METHOD_LOCAL = 0,				// Lokale tcp/ip verbinding
	CONNECTION_METHOD_REMOTE_DIRECT,			// remote tcp/ip verbinding via port forwarding
	CONNECTION_METHOD_REMOTE_CLOUD,				// remote tcp/ip verbinding via de cloud server
	CONNECTION_METHOD_UNKNOWN = -1
} ConnectionMethod_t;
/*****************************************************************/

/*****************************************************************/
#define CLOUD_SERVER_ADDRESS	("connect.duotecno.eu")		
#define CLOUD_SERVER_TCPPORT	(5099)
/*****************************************************************/
#endif 
/*****************************************************************/
