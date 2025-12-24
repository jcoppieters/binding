#pragma once

#ifdef __cplusplus
#   define EXTERN_C     extern "C"
#else
#   define EXTERN_C
#endif // __cplusplus


#ifdef __cplusplus
namespace DUOTECNO {
#endif

/**
 * DDNS Status
 */
typedef enum
{
	DDNS_STATUS_DISABLED 			= 0,   		// Niet veranderen
	DDNS_STATUS_GOOD				= 1,       	// Niet veranderen
	DDNS_STATUS_ERROR		 		= 2,		// Niet veranderen
	DDNS_STATUS_WAIT 				= 3			// Niet veranderen

} DDNSStatus;

/**
 * DDNS Service provider
 */
typedef enum
{
   DDNS_SERVICE_PROVIDER_NONE 		= 0,		// Niet veranderen
   DDNS_SERVICE_PROVIDER_NOIP 	 	= 1			// Niet veranderen
   // Nieuwe providers komen hier...

} DDNSProvider;

/**
 * SystemMode
 */
typedef enum
{
	SYSTEM_MODE_BASIC			= 0,
	SYSTEM_MODE_EXTENDED		= 1

} SystemMode;

/**
 * Application supported configuration flags.
 */
typedef enum
{
	SUPPORTED_FLAG_SERVER		= (0x01<<0),		// bit 0 - Niet veranderen
	SUPPORTED_FLAG_DDNSCLIENT	= (0x01<<1),		// bit 1 - Niet veranderen
	SUPPORTED_FLAG_SYSMODE		= (0x01<<2),		// bit 2 - Niet veranderen
	SUPPORTED_FLAG_TCPIPCLIENT	= (0x01<<3),		// bit 3 - Niet veranderen - sedert v2.0
	SUPPORTED_FLAG_DNSPROVIDER	= (0x01<<4),		// bit 4 - Niet veranderen - sedert v2.1
	SUPPORTED_FLAG_SNTP			= (0x01<<5),		// bit 5 - Niet veranderen
	SUPPORTED_FLAG_TIME			= (0x01<<6),		// bit 6 - Niet veranderen
	SUPPORTED_FLAG_API			= (0x01<<7)			// bit 7 - Nieuwe

	// Max bit 31...

} AppCfgSupportedFlags;


#ifdef __cplusplus
};	// end namespace DUOTECNO
#endif