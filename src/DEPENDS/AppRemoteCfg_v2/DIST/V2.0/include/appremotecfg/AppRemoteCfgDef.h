#ifndef INCLUDED_APPREMOTECFG_DEF_H
#define INCLUDED_APPREMOTECFG_DEF_H
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
typedef enum {
	SERVICE_UNKNOWN = -1,
	SERVICE_TCPIP = 0,					// Homeclient.
	SERVICE_HTTP,						// API poort 8080
	SERVICE_HTTP_S,						// API Poort 8081	
} AppRemoteCfgService_t;

typedef enum {
	// HOSTADDRESS_TYPE_UNKNOWN = -1,
	HOSTADDRESS_TYPE_LOCAL = 0,			// Local ip-address & poort
	HOSTADDRESS_TYPE_REMOTE = 1			// Remote dns & poort.

} AppRemoteCfgHostAddress_t;
/**************************************************************/
}; /* end namespace */
/**************************************************************/
#endif 
/**************************************************************/